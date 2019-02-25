///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "ESuccess.h"
#include "MVulkanRenderer.h"
#include "FHelperVulkan.h"
#include "FHelperFileIO.h"
#include <set>
#include "Temp/DDefaultVertex.h"

namespace
{

const std::vector<const char*> validationLayers =
{
  "VK_LAYER_LUNARG_standard_validation"
};

/// ~Swap chain~
/// Vulkan does not have the concept of a `Default Framebuffer` like OpenGL,
/// It requires an infrastructure that will onw the buffers we will render to.
/// This infrastructure is known as swap chain and must be created when rendering.
///
/// The swap chain is essentially a queue of images that are waiting to be presented.
/// The general purpose of the swap chain is to synchronize the presentation of images
/// WIth the refresh rate of the screen.
///
/// * Not all graphics cards are capable of presenting images directly to screen (such as server)
/// * Image presentation is very tied to the window system and surface (VkSurfaceKHR)
/// * It's not part of vulkan core. so have to enabled the `VK_KHR_swapchain` extension.
const std::vector<const char*> sDeviceExtensions = 
{
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/// @brief Vulkan validation (LUNARG SDK) layer debug callback function. \n
/// This function follows `PFN_vkDebugUtilsMessengerCallbackEXT` signature, \n
/// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/PFN_vkDebugUtilsMessengerCallbackEXT.html \n
///
/// @param iMessageSeverity Specifies the severity of the message, which is one of the following flags. \n
/// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkDebugUtilsMessageSeverityFlagBitsEXT.html \n
/// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT : Diagnostic message \n
/// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : Information message such as creation of resource. \n
/// VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : Message about behaviour that might be a b-ug. \n
/// VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : Message about behaviour that is invalid & error. \n
///
/// @param iMessageType More detailed message type from Vulkan. \n
/// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkDebugUtilsMessageTypeFlagBitsEXT.html \n
/// VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT : Some event that is unrelated to the specification. \n
/// VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT : Violates the specification or possible mistake. \n
/// VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT : Specifies a potentially non-optimal use of Vulkan. \n
///
/// @param ipCallbackData Structure specifying parameters returned to the callback.
/// Contains the low-level details of the message itself, with the most important members being. \n
/// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkDebugUtilsMessengerCallbackDataEXT.html \n
/// 
/// @param ipUserData pointer of user data, so User can pass user's own data to callback. \n
///
/// The callback returns a VkBool32, which is interpreted in a layer-specified manner. \n
/// The application should always return VK_FALSE. \n
/// The VK_TRUE value is reserved for use in layer development. \n
/// If the callback returns true, then the call is aborted with the `VK_ERROR_VALIDATION_FAILED_EXT` error. 
VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
    [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT iMessageSeverity,
    [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT iMessageType,
    const VkDebugUtilsMessengerCallbackDataEXT* ipCallbackData,
    [[maybe_unused]] void* ipUserData)
{
  std::fprintf(stderr, "Validation Layer : %s\n", ipCallbackData->pMessage);
  return VK_FALSE;
}

const std::vector<sh::DDefaultVertex> sTempVertices = {
    // Position,          // Color,
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
};

VkBuffer sVertexBufferObject;
VkDeviceMemory sVertexBufferMemory;

} /// anonymouse namespace

//!
//!
//!

EDySuccess MVulkanRenderer::pfInitialize()
{
  this->InitGlfw();

  //
  this->mInstance = this->pCreateVulkanInstance();

  // Unfortunately, just enabling the layers doesn't help much, because they currently have no way to relay
  // the b-ug message to our program.
  // To receive those message we have to set up a debug messenger with a callback,
  // which requires the `VK_EXT_DEBUG_UTILS_` extension.
  if constexpr (kEnabledValidationLayers == true)
  {
    this->mMessengerExt = this->pSetupDebugManager(this->mInstance);
  }

  /* https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface
   * Since Vulkan is a platform independent API, it can not interface directly with the window system.
   * To establish the connection between Vulkan and the window system to present results to the screen,
   * we need to use the WSI (Window system integration) extensions that starts with `VK_KHR_`.
   * 
   * VK_KHR_surface
   * http://vulkan-spec-chunked.ahcox.com/apes02.html
   * exposes VkSurfaceKHR object that represents an abstract type of surface to present rendered imaged to.
   * The VK_KHR_surface extension is an instance level extension, and it is retruned when calling
   * `glfwGetRequiredInstanceExtensions` function.
   *
   * Window suface need to be created afterward the instance creation,
   * because it can actually influence the physical device selection.
   * And, window surface is just optional component in Vulkan (so default framebuffer is not exist)
   */
  // glfwCreateWindowSurface function performs detailed VkSurfaceKHR creation platform indenpendently.
  // Creation of surface must be held before creation of physical device.
  if (glfwCreateWindowSurface(this->mInstance, this->mGlfwWindow, nullptr, &this->mSurface) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create window surface.");
  }

  // After initilaizing the vulkan library through a `VkInstance`,
  // we need to look for and select a graphics card in the system that supports the features we need.
  // In fact we can select any number of graphics card and use them stimutaneously.
  this->mPhysicalDevice = this->pPickPhysicalDevice(this->mInstance);

  // After selecting a physical device to use,
  // We need to set up a `Logical device` to interface with physical device from application layer.
  // The logcial device creation process is similiar to the instance creation process.
  // Also, user can create multiple logical devices from the same phyiscal device.
  std::tie(this->mGraphicsDevice, this->mGraphicsQueue, this->mPresentQueue) 
      = this->pCreateVkLogicalDevice(this->mPhysicalDevice);

  // Create swap chain. This function must be succeeded.
  this->CreateSwapChain();
  this->mSwapChainImages = this->GetSwapChainImageHandles(this->mSwapChain);
  // Create swap chain image views to view image handle list of swap chain.
  this->CreateSwapChainImageViews();
  // 
  this->CreateRenderPass();
  this->CreateGraphicsPipeline();
  this->CreateFrameBuffer();
  //
  this->CreateCommandPool();
  //
  this->CreateVertexBuffer();
  this->CreateCommandBuffers();
  //
  this->CreateDefaultSemaphores();

  return DY_SUCCESS;
}

void MVulkanRenderer::InitGlfw()
{
  glfwInit();
  // GFLW was originally designed to create an OpenGL Client context,
  // but we are going to use Vulkan, so does not have to create OpneGL Context instance
  // in internal storage.
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // Handling windows also take care special routine when using Vulkan.
  // so we have to disable anyway.
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  
  // Create window.
  this->mGlfwWindow = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
  if (this->mGlfwWindow == nullptr)
  {
    throw std::runtime_error("Failed to create glfw window.");
  }
  
  // Set user data.
  glfwSetWindowUserPointer(this->mGlfwWindow, this);
  glfwSetFramebufferSizeCallback(this->mGlfwWindow, CbGLFWFrameBufferResize);
}

VkInstance MVulkanRenderer::pCreateVulkanInstance()
{
   // Check validation layer.
  if constexpr (kEnabledValidationLayers == true)
  {
    if (this->ppVkCheckValidationLayerSupport() == false)
    {
      throw std::runtime_error("Validation layers requested, but not available.");
    }
  }
  
  VkApplicationInfo appInfo = {};
  // Many struct in Vulkan requires you to explicitly specify the type in the `sType` member. (OPTIONAL)
  appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName    = "Hello Triangle";
  appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName         = "No Engine";
  appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion          = VK_API_VERSION_1_1;

  // A lot of information in Vulkan is passed through structs instead of function parameters
  // such as Descriptor. we'll have to fill in one more struct to provide sufficient
  // informaiton for creating an instance.
  // (REQUIRED)
  VkInstanceCreateInfo createInfo = {};
  createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  // If the check was successful then vkCreateInstance should not ever return a VK_ERROR_LAYER_NOT_PRESENT error, 
  // but you should run the program to make sure.
  if constexpr (kEnabledValidationLayers == true)
  {
    createInfo.enabledLayerCount    = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames  = validationLayers.data();
  }

  // Check extension count.
  // And need an extension to interface with the window system.
  // so, insert VK extension in VkInstanceCreateInfo.
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  // To allocate an array to hold the extension details we first need to know
  // how many there are. and allocate an array to hold the extension details.
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  // Print given extensions information as name.
  std::printf("Available vulkan extensions :\n");
  for (const auto& extension : extensions) { std::printf("\t%s\n", extension.extensionName); }

  // Check and get
  auto enabledExtensionNames = ppVkGetRequiredExtensions();
  createInfo.enabledExtensionCount   = static_cast<TU32>(enabledExtensionNames.size());
  createInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
  
  VkInstance instance;
  // Create & hold the handle to the instance.
  const auto result = vkCreateInstance(&createInfo, nullptr, &instance); 
  if (result != VK_SUCCESS) { throw std::runtime_error("Failed to create instance."); }

  return instance;

#ifdef false
  uint32_t      glfwExtensionCount  = 0;
  const auto**  glfwExtensionList   = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  for (uint32_t i = 0; i < glfwExtensionCount; ++i)
  {
    // Check extension.
    auto it = std::find_if(
        extensions.begin(), extensions.end(),
        [ptrString = glfwExtensionList[i]](const VkExtensionProperties& iProperty)
        {
          return std::strcmp(iProperty.extensionName, ptrString) == 0;
        }
    );
    // Check validation.
    // All glfw vulkan extension must be included in given Vulkan extension enumeration
    // property list. Otherwise, GLFW vulkan might not be able to create appropriate
    // instance.
    if (it == extensions.end())
    {
      std::printf("GLFW extension %s not found.\n", glfwExtensionList[i]);
      std::printf("Failed to create Vulkan instance. Abort...\n");
      exit(1);
    }
  }
  std::transform(
      extensions.begin(), extensions.end(), 
      std::back_inserter(enabledExtensionNames),
      [](const VkExtensionProperties& c) -> const char* { return c.extensionName; }
  );
#endif
}

bool MVulkanRenderer::ppVkCheckValidationLayerSupport()
{
  // Get layer count 
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  // Get layer properties as list.
  std::vector<VkLayerProperties> layerPropertiesList(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layerPropertiesList.data());

  // Check given validationLayer list is exist in given Vulkan layerProperties.
  for (const auto& layerName : validationLayers)
  {
    bool layerFound = false;
    // Check given layerName is in layerPropertiesList by iterating list.
    for (const auto& layerProperty : layerPropertiesList)
    {
      if (std::strcmp(layerProperty.layerName, layerName) == 0)
      {
        layerFound = true; break;
      }
    }
    // If not found, return false.
    if (layerFound == false) { return false; }
  }

  return true;
}

std::vector<const char*> MVulkanRenderer::ppVkGetRequiredExtensions()
{
  // Get required extension names by glfw.
  TU32 glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  std::vector<const char*> returnExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  
  // If validation layer feature is enabled, push debug message callback extension also.
  if constexpr (kEnabledValidationLayers == true)
  {
    returnExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return returnExtensions;
}

VkDebugUtilsMessengerEXT MVulkanRenderer::pSetupDebugManager(VkInstance iInstance)
{
  // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = 
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = 
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
    | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = VkDebugCallback;
  createInfo.pUserData = nullptr;

  // and `VkDebugUtilsMessengerCreateInfoEXT` structure should be passed to the 
  // `vkCreateDebugUtilsMessengerEXT` function to create the `VkDebugUtilsMessengerEXT` object.
  // BUT, it is not automatically loaded because `VkDebugUtilsMessengerEXT` is extension function.
  //
  // So we have to look up its address ourselves using `vkGetInstanceProcAddr` function.
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(iInstance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr)
  {
    VkDebugUtilsMessengerEXT debugMessenger;
    const auto flag = func(iInstance, &createInfo, nullptr, &debugMessenger);
    if (flag != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to set up debug messenger.");
    }
    return debugMessenger;
  }

  throw std::runtime_error("Failed to set up debug messenger. Extension not supported.");
}

VkPhysicalDevice MVulkanRenderer::pPickPhysicalDevice(VkInstance iInstance)
{
  // Get supported physical device count.
  TU32 deviceCount = 0;
  vkEnumeratePhysicalDevices(iInstance, &deviceCount, nullptr);
  // Check unrecoverable error.
  if (deviceCount == 0)
  {
    throw std::runtime_error("Failed to find GPUs that supports Vulkan.");
  }

  // Get supported physical device list.
  std::vector<VkPhysicalDevice> physicalDeviceList(deviceCount);
  vkEnumeratePhysicalDevices(iInstance, &deviceCount, physicalDeviceList.data());

  // Check condition satisfied physical device and return one.
  for (const auto& device : physicalDeviceList)
  {
    if (this->ppIsDeviceSuitable(device) == true)
    {
      return device;
    }
  }

  // If not found, just abort.
  throw std::runtime_error("Failed to find a suitable GPU.");
}

bool MVulkanRenderer::ppIsDeviceSuitable(VkPhysicalDevice iPhysicalDevice)
{
  // To evaluate the suitability of a device, we cahn start by querying for some details.
  // Basic device properties like the name, type and supported Vulkan version can be queried using
  // `vkGetPhysicalDeviceProperties`.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPhysicalDeviceProperties.html
  VkPhysicalDeviceProperties givenPhysicalDeviceProperties;
  vkGetPhysicalDeviceProperties(iPhysicalDevice, &givenPhysicalDeviceProperties);

  // The support for `OPTIONAL FEATURES` like texture compressio, 64bit float, multiple viewport rendering like VR
  // can be queried using `vkGetPhysicalDeviceFeatures`.
  //https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPhysicalDeviceFeatures.html
  VkPhysicalDeviceFeatures givenPhysicalDeviceFeatures;
  vkGetPhysicalDeviceFeatures(iPhysicalDevice, &givenPhysicalDeviceFeatures);

  // Check device's graphics queue family that satisfies graphics command queue condition.
  // in Vulkan, anything from drawing to uploading textures requires commands to be summitted to a queue.
  // There are different types of queues that originate from `Different queue families`.
  //
  // Each family of queue allows only a subset of commands.
  // For example, there could be a queue family that only allows processing of compute commands.
  // or memory transfer, and graphics queue also.
  auto indices = GetFindQueueFamilies(iPhysicalDevice, VK_QUEUE_GRAPHICS_BIT);

  // Check this physical device supports swap-chain extension.
  const bool isSwapChainExtSupported = this->CheckDeviceExtensionSupport(iPhysicalDevice, sDeviceExtensions);

  // ...Just checking if a swap chain is available is not sufficient, 
  // because it may not actually be compatible with our window surface. 
  // Creating a swap chain also involves a lot more settings than instance and device creation, 
  // so we need to query for some more details before we're able to proceed...
  //
  // Get swap-chain details if swap-chain extension is supported,
  // and check detailed information is adequate for using.
  bool isSwapChainConditionAdequate = false;
  if (isSwapChainExtSupported == true)
  {
    const auto swapChainDetails = this->QuerySwapChainSupport(iPhysicalDevice);
    isSwapChainConditionAdequate = 
        swapChainDetails.mFormats.empty() == false
    &&  swapChainDetails.mPresentModes.empty() == false;  
  }

  return givenPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
      && VkIsTrue(givenPhysicalDeviceFeatures.geometryShader)
      && isSwapChainExtSupported == true
      && isSwapChainConditionAdequate == true
      && indices.IsComplete() == true;
}

bool MVulkanRenderer::CheckDeviceExtensionSupport(
    VkPhysicalDevice iPhysicalDevice,
    const std::vector<const char*>& iExtensionRequsition)
{
  // `vkEnumerateDeviceExtensionProperties`
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkEnumerateDeviceExtensionProperties.html
  TU32 extensionCount;
  vkEnumerateDeviceExtensionProperties(iPhysicalDevice, nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(iPhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

  // Set set of required extension name list, 
  // and erase if found by iterating device extension list.
  std::unordered_set<std::string> requiredExtensionList(MCR_BIND_BEGIN_END(iExtensionRequsition));
  for (const auto& extension : availableExtensions)
  {
    requiredExtensionList.erase(extension.extensionName);
  }

  // If all item are removed (satisfies requisition), return true or false.
  return requiredExtensionList.empty() == true;
}

DVkQueueFamilyIndices MVulkanRenderer::GetFindQueueFamilies(
    VkPhysicalDevice iPhysicalDevice,
    VkQueueFlagBits iQueueFlagBits)
{
  // Get queue family count & property list from iPhysicalDevice.
  TU32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(iPhysicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(iPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

  // Check.
  DVkQueueFamilyIndices indices;
  for (TU32 i = 0; i < queueFamilyCount; ++i)
  {
    // Check there is graphic queue in index [i] of given graphics queue of physical device..
    const auto& queueFamilyProperty = queueFamilyProperties[i];
    if (queueFamilyProperty.queueCount > 0
    &&  queueFamilyProperty.queueFlags & iQueueFlagBits)
    {
      indices.moptGraphicsQueueFamiliy = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(iPhysicalDevice, i, this->mSurface, &presentSupport);

    // Check there is present queue in index[i] of given graphics queue of physical device..
    if (queueFamilyProperty.queueCount > 0 && VkIsTrue(presentSupport))
    {
      indices.moptPresentQueueFamily = i;
    }

    if (indices.IsComplete() == true) { break; }
  }

  // Note that it's very likely that these end up being the same queue family after all, 
  // but throughout the program we will treat them as if they were separate queues for a uniform approach. 
  // Nevertheless, you could add logic to explicitly prefer a physical device that 
  // supports drawing and presentation in the same queue for improved performance.
  return indices;
}

DVkSwapChainSupportDetails MVulkanRenderer::QuerySwapChainSupport(VkPhysicalDevice iPhysicalDevice)
{
  // Get basic surface capabilities.
  DVkSwapChainSupportDetails details;
  // All of the support querying functions have `VkPhysicalDevice` and `VkSurfaceKHR` as parameters
  // because they are the core components of the swap chain.
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(iPhysicalDevice, this->mSurface, &details.mCapabilities);

  TU32 formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(
      iPhysicalDevice, this->mSurface, 
      &formatCount, nullptr);
  if (formatCount != 0)
  {
    details.mFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        iPhysicalDevice, this->mSurface, 
        &formatCount, details.mFormats.data());
  }

  TU32 presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      iPhysicalDevice, this->mSurface, 
      &presentModeCount, nullptr);
  if (presentModeCount != 0)
  {
    details.mPresentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        iPhysicalDevice, this->mSurface, 
        &presentModeCount, details.mPresentModes.data());
  }

  return details;
}

std::tuple<VkDevice, VkQueue, VkQueue> MVulkanRenderer::pCreateVkLogicalDevice(VkPhysicalDevice iPhysicalDevice)
{
  auto indices = GetFindQueueFamilies(iPhysicalDevice, VK_QUEUE_GRAPHICS_BIT);
  // Set unique queue families index to render & present.
  std::set<TU32> uniqueQueueFamilies = {
    *indices.moptGraphicsQueueFamiliy,
    *indices.moptPresentQueueFamily
  };
  
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList;
  const TF32 queuePriority = 1.0f;
  for (const auto& queueFamilyIndex : uniqueQueueFamilies)
  {
    // Structure specifying parameters of a newly create device queue.
    // ::queueFamily has a multiple queue, so ::queueCount can be lager than 1.
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkDeviceQueueCreateInfo.html
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    // Vulkan lets youassign priorities to queues to influence the scheduling of commnad buffer
    // execution, using floating point numbers between [0.0f, 1.0f].
    queueCreateInfo.pQueuePriorities = &queuePriority;
    // Insertion
    queueCreateInfoList.emplace_back(queueCreateInfo);
  }

  // Set up devices features that we'll using.
  VkPhysicalDeviceFeatures logicalDeviceFeatures = {};
  
  // Using `VkPhysicalDeviceFeatures` and `VkDeviceQueueCreateInfo`,
  // we can start filling `VkDeviceCreateInfo` structure.
  // This filling process is resemble to `VkInstanceCreateInfo`, but this is device specific.
  //
  // For example, `VK_KHR_swapchain` is device specified extension, that present rendered images
  // from that device to screen. but it's possible that there are devices that lack this feature.
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pEnabledFeatures         = &logicalDeviceFeatures;
  createInfo.queueCreateInfoCount     = static_cast<TU32>(queueCreateInfoList.size());
  createInfo.pQueueCreateInfos        = queueCreateInfoList.data();
  createInfo.enabledExtensionCount    = static_cast<TU32>(sDeviceExtensions.size());
  createInfo.ppEnabledExtensionNames  = sDeviceExtensions.data();
  if constexpr (kEnabledValidationLayers == true)
  {
    createInfo.enabledLayerCount   = static_cast<TU32>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }

  // Create logical device `VkDevice` using `vkCreateDevice`.
  VkDevice logicalDevice;
  if (vkCreateDevice(iPhysicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create logcial vulkan device.");
  }

  // Device queues are automatically created along with the logical device.
  // But have to have a handle to interface with internal system.
  // VkQueue are implicitly cleaned up when the logical device is destroyed.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkQueue.html
  VkQueue graphicsQueue;
  vkGetDeviceQueue(logicalDevice, *indices.moptGraphicsQueueFamiliy, 0, &graphicsQueue);

  // The queueFamilyIndex member of each element of pQueueCreateInfos must be unique 
  // within pQueueCreateInfos, except that two members can share the same queueFamilyIndex 
  // if one is a protected-capable queue and one is not a protected-capable queue.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VUID-VkDeviceCreateInfo-queueFamilyIndex-00372
  VkQueue presentQueue;
  vkGetDeviceQueue(logicalDevice, *indices.moptPresentQueueFamily, 0, &presentQueue);

  return {logicalDevice, graphicsQueue, presentQueue};
}

void MVulkanRenderer::CreateSwapChain()
{
  // Get Swapchain detail instance.
  const DVkSwapChainSupportDetails swapChainDetails = this->QuerySwapChainSupport(this->mPhysicalDevice);

  // Get preferred swapchain setting. 
  auto preferredSwapChainSurfaceFormat = this->ChoooseSwapSurfaceFormat(swapChainDetails.mFormats);
  auto preferredSwapChainPresentMode = this->ChooseSwapPresentMode(swapChainDetails.mPresentModes);
  auto preferredSwapChainResolution = this->ChooseSwapResolution(swapChainDetails.mCapabilities);

  // Also, we need to set how many images we use in the swap chain.
  // imageCount must not be exceed given minImageCount and maxImageCount.
  TU32 imageCount = swapChainDetails.mCapabilities.minImageCount + 1;
  // But if mCapabilities.maxImageCount is 0, there is no maximum on this physics device swapchain...
  // So do clamp if only maxImageCount is not 0 (set by some value)
  if (swapChainDetails.mCapabilities.maxImageCount > 0)
  {
    imageCount = std::clamp(
        imageCount, 
        swapChainDetails.mCapabilities.minImageCount,
        swapChainDetails.mCapabilities.maxImageCount);
  }

  // Also, requires structure to create swap chain instance.
  // See http://vulkan-spec-chunked.ahcox.com/ch29s06.html
  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface  = this->mSurface; // This surface must be matched to physical device of input arg.
  createInfo.minImageCount    = imageCount;
  createInfo.imageFormat      = preferredSwapChainSurfaceFormat.format;
  createInfo.imageColorSpace  = preferredSwapChainSurfaceFormat.colorSpace;
  createInfo.imageExtent      = preferredSwapChainResolution;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // We should insert information of how to handle swap chain images that will be used
  // across multiple queue families. 
  auto indices = this->GetFindQueueFamilies(this->mPhysicalDevice, VK_QUEUE_GRAPHICS_BIT);
  std::vector<TU32> queueFamilyIndices = {*indices.moptGraphicsQueueFamiliy, *indices.moptPresentQueueFamily};

  // If queue familiy index is different each other (graphics & present)
  // We must be drawing on the images in the swap chain from the graphics queue and submitting them
  // on presentation queue.
  // 
  // VkSharingMode : https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkSharingMode.html
  // ...VK_SHARING_MODE_CONCURRENT may result in lower performance 
  // access to the buffer or image than VK_SHARING_MODE_EXCLUSIVE...
  if (queueFamilyIndices[0] != queueFamilyIndices[1])
  {
    // If the queue families differ, then we'l be using the concurrent mode.
    // Concurrent mode requires to specify in advance between which queue families will be sharing
    // using `queueFamilyIndexCount` and `pQueueFamilyIndices`.
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; 
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
  }
  else
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  //
  createInfo.preTransform   = swapChainDetails.mCapabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode    = preferredSwapChainPresentMode;
  createInfo.clipped        = VK_TRUE;
  // With Vulkan, it's possible that your swap chain becomes invalid or unoptimized while aplication
  // is running such as resizing of the window.
  // In that case the swap chain actually need to be recreated from scratch, and a reference to the old one
  // must be specified as `oldSwapChain`.
  createInfo.oldSwapchain   = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(this->mGraphicsDevice, &createInfo, nullptr, &this->mSwapChain) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create swap chain.");
  }

  this->mSwapChainExtent      = preferredSwapChainResolution;
  this->mSwapChainImageFormat = preferredSwapChainSurfaceFormat.format;
}

VkSurfaceFormatKHR MVulkanRenderer::ChoooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& iAvailableFormatList)
{
  // If available format items is only one and undefined yet,
  // just return default format & colorSpace.
  if (iAvailableFormatList.size() == 1
  &&  iAvailableFormatList[0].format == VK_FORMAT_UNDEFINED)
  {
    // VkFormat : https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkFormat.html
    // VkColorSpaceKHR : https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkColorSpaceKHR.html
    return VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }

  // If there are more format on surface of physical device, See if the preferred combination is available.
  for (const auto& [format, colorSpace] : iAvailableFormatList)
  {
    if (format == VK_FORMAT_B8G8R8A8_UNORM 
    &&  colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      return VkSurfaceFormatKHR{format, colorSpace};
    }
  }

  // If not find any preferred VkSurfaceFormat, just return first one.
  return iAvailableFormatList.front();
}

VkPresentModeKHR MVulkanRenderer::ChooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& iAvailablePresentModeList)
{
  // VkPresentModeKHR :
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPresentModeKHR.html
  const VkPresentModeKHR preferredMode = VK_PRESENT_MODE_FIFO_KHR;
  
  // Some drivers don't properly support `VK_PRESENT_MODE_FIFO_KHR`,
  // if `VK_PRESENT_MODE_MAILBOX_KHR` is not available, we should return first mode, `IMMEDIATELY`.
  for (const auto& availablePresentMode : iAvailablePresentModeList)
  {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      return availablePresentMode;
    }
    else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
    {
      return availablePresentMode;
    }
  }

  return preferredMode;
}

VkExtent2D MVulkanRenderer::ChooseSwapResolution(const VkSurfaceCapabilitiesKHR& iCapabiliteis)
{
  if (iCapabiliteis.currentExtent.width != NumericalMax<TU32>)
  {
    return iCapabiliteis.currentExtent;
  }
  else
  {
    // VkExtent2D :
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkExtent2D.html
    TI32 width, height;
    glfwGetFramebufferSize(this->mGlfwWindow, &width, &height);
    VkExtent2D actualExtent = {TU32(width), TU32(height)};

    actualExtent.width = std::clamp(
        actualExtent.width, 
        iCapabiliteis.minImageExtent.width, 
        iCapabiliteis.maxImageExtent.width);
    actualExtent.height = std::clamp(
        actualExtent.height, 
        iCapabiliteis.minImageExtent.height, 
        iCapabiliteis.maxImageExtent.height);

    return actualExtent;
  }
}

std::vector<VkImage> MVulkanRenderer::GetSwapChainImageHandles(VkSwapchainKHR& iSwapChain)
{
  TU32 imageCount;
  vkGetSwapchainImagesKHR(this->mGraphicsDevice, iSwapChain, &imageCount, nullptr);
  std::vector<VkImage> imageHandleList(imageCount);
  vkGetSwapchainImagesKHR(this->mGraphicsDevice, iSwapChain, &imageCount, imageHandleList.data());

  return imageHandleList;
}

void MVulkanRenderer::CreateSwapChainImageViews()
{
  // We need to resize to match all swapchain image handle one-by-one.
  this->mSwapChainImageViews.resize(this->mSwapChainImages.size());

  for (TU32 imageId = 0, size = static_cast<TU32>(this->mSwapChainImageViews.size());
       imageId < size; ++imageId)
  {
    // We also create VkImageView using VkImageViewCreateInfo and vkCreateImageView function.
    // VkImageViewCreateInfo : 
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkImageViewCreateInfo.html
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image    = this->mSwapChainImages[imageId];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // We use 2D texture view.
    createInfo.format   = this->mSwapChainImageFormat; 
    // `components` field allows to swizzle the color channels around. (like variable.xxyw)
    // VkComponentSwizzle : 
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkComponentSwizzle.html
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; // .r
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY; // .g
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY; // .b
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY; // .a default.
    // `subresourceRange` describes what the image's purpose is
    // and which part of the image should be accessed...
    createInfo.subresourceRange.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel    = 0;
    createInfo.subresourceRange.levelCount      = 1;
    createInfo.subresourceRange.baseArrayLayer  = 0;
    createInfo.subresourceRange.layerCount      = 1;

    // Created `VkImageView` is not bound to `VkSwapChainKHR` so need to be destroyed explicitly.
    if (vkCreateImageView(this->mGraphicsDevice, &createInfo, nullptr, &this->mSwapChainImageViews[imageId])
        != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create image view.");
    }
  }
}

void MVulkanRenderer::CreateRenderPass()
{
  // (1) In our case, we have just a single color buffer attachment. (CreateSwapChain)
  // Textures and framebuffers in Vulkan are represented by `VkImage` object with a certain pixel format.
  // Layout != Format... and layout can be changed based on what to do with an `VkImage`.
  //
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkAttachmentDescription.html
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format  = this->mSwapChainImageFormat; // Format must be matched to swapchain image.
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  // Two option determine what to do with the data in the attachment before rendering and after rendering.
  // these options are applied to color and depth data.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkAttachmentLoadOp.html
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkAttachmentStoreOp.htmlH
  colorAttachment.loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear operation when load framebuffer data.
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;// Just store!
  // and another two options below are applied to stencil data.
  colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  // So, `VkImage` need to be transitted to specified layouts that are suitable for the operation.
  // `initialLayout` specifies which layout the image will have before the render pass begins.
  // `finalLayout` specifies the layout to automatically transition to when the render pass finishes.
  // 
  // VkImageLayout : https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkImageLayout.html
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout   = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // (2) Setup subpasses and attachment references
  // A single render pass can consist of `multiple subpass`, like a post-processing.
  // If you group these rendering subpass operations into one render pass,
  // then Vulkan is able to reorder the operations and conserve memory bandwidth for better performance.
  //
  // In this case, we only use one subpass for just rendering triangle.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkAttachmentReference.html
  // 
  // `layout` specifies which layout we would like the attachment to have, during a subpass uses reference.
  // Vulkan will automatically transition the attachment to this layout when the subpass is started.
  VkAttachmentReference colorAttachmentRef;
  colorAttachmentRef.attachment = 0; // Index of attachment description.
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // We use attachment as color buffer.

  // (3) The subpass also should be described.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkSubpassDescription.html
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkCmdBindPipeline.html
  VkSubpassDescription subpassDesc  = {};
  subpassDesc.pipelineBindPoint     = VK_PIPELINE_BIND_POINT_GRAPHICS; // We use this subpass as graphics.
  // layout(location = 0) out vec4 outColor
  subpassDesc.colorAttachmentCount  = 1;
  subpassDesc.pColorAttachments     = &colorAttachmentRef;
  
  // (4) Although subpass list are consists of one, there are implicit right before and right after
  // subpass.
  //
  // The subpasses in a render pass automatically take care of image layout transitions. But these
  // transitions are controlled by `Subpass dependencies` that specify memory and execution dependencies
  // between subpasses.
  //
  // There are two built-in depdendencies that take care of the transition at the start of the
  // render pass and at the end of the render pass.. but the former does not occur at the right time.
  // Because, it also relevant to asynchonous execution of Vulkan rendere command queue and cpu function call.
  // So, we have also two selection to avoid subpass transition before acquiring image index.
  // 
  // 1. Change waitStages (mSemaphoreImageAvailable) to wait before PIPELINE_STAGE_TOP_OF_PIPE_BIT.
  // Render pass don't begin until imageIndex is available.
  // 2. Make the render pass wait for VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT stage because
  // writing to buffer in fragment need to imageIndex of VkImage of VkFramebuffer of swapchain.
  //
  // In this case, we use selection 2.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkSubpassDependency.html
  VkSubpassDependency dependency = {};
  // The special value VK_SUBPASS_EXTERNAL refers to the implicit subpass before 
  // or after the render pass depending on whether it is specified in srcSubpass or dstSubpass
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0; // Our subpass index.
  // 
  dependency.srcStageMask   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask  = 0;
  dependency.dstStageMask   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask  = 
      VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
    | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  // (5) Make Render pass handle instance.
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType            = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount  = 1;
  renderPassInfo.pAttachments     = &colorAttachment;
  renderPassInfo.subpassCount     = 1;
  renderPassInfo.pSubpasses       = &subpassDesc;
  renderPassInfo.dependencyCount  = 1;
  renderPassInfo.pDependencies    = &dependency;

  // Render pass handle must be destroyed explicitly.
  if (vkCreateRenderPass(this->mGraphicsDevice, &renderPassInfo, nullptr, &this->mRenderPass)
      != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create render pass.");
  }
}

void MVulkanRenderer::CreateGraphicsPipeline()
{
  // Temporary code. Read spir-v shader file.
  // To create `VkShaderModule`, shader code must be in the SPIR-V format.
  auto vertShaderCode = ReadBinaryFile("../../Resource/vert.spv");
  auto fragShaderCode = ReadBinaryFile("../../Resource/frag.spv");

  // Create `VkShaderModule`. 
  // VkShaderModule : https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkShaderModule.html
  auto vertShaderModule = this->CreateShaderModule(*vertShaderCode);
  auto fragShaderModule = this->CreateShaderModule(*fragShaderCode);

  // To actually use the shaders, must assign populated VkShaderModule 
  // to a specified pipeline stage through `VkPipelineShaderStageCreateInfo`.
  //
  // VkPipelineShaderStageCreateInfo :
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineShaderStageCreateInfo.html
  VkPipelineShaderStageCreateInfo vertCreateInfo = {};
  vertCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertCreateInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
  vertCreateInfo.module = vertShaderModule;
  vertCreateInfo.pName  = "main"; // entry point name of the shader for this stage.

  VkPipelineShaderStageCreateInfo fragCreateInfo = {};
  fragCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragCreateInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragCreateInfo.module = fragShaderModule;
  fragCreateInfo.pName  = "main"; // entry point name of the shader for this stage.

  // Define an array of `VkPipelineShaderStageCreateInfo` structure.
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages{};
  shaderStages.emplace_back(vertCreateInfo);
  shaderStages.emplace_back(fragCreateInfo);

  // In Vulkan, you have to be explicit about everything, including setting up the stages of graphics
  // pipeline.
  this->CreateFixedRenderPipeline(shaderStages);

  // The cleanup should happen at the end of the function by adding two calls.
  vkDestroyShaderModule(this->mGraphicsDevice, vertShaderModule, nullptr);
  vkDestroyShaderModule(this->mGraphicsDevice, fragShaderModule, nullptr);
}

VkShaderModule MVulkanRenderer::CreateShaderModule(const std::vector<char>& iCodeBuffer)
{
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = iCodeBuffer.size();
  createInfo.pCode    = reinterpret_cast<const TU32*>(iCodeBuffer.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(this->mGraphicsDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create shader module.");
  }

  return shaderModule;
}

void MVulkanRenderer::CreateFixedRenderPipeline(
    const std::vector<VkPipelineShaderStageCreateInfo>& iShaderStages)
{
  // (1) Vertex input (not assembly yet!)
  // * Bindings : spacing between data and whether the data is per-vertex or per-instance
  // This is like `stride` and `instancing`.
  // * Attribute : type of the attributes passed to the vertex shader.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineVertexInputStateCreateInfo.html
  //
  // We now need to set up the graphics pipeline to accept vertex data in this format 
  // by referencing the structures in createGraphicsPipeline. 
  // Find the vertexInputInfo struct and modify it to reference the two descriptions:
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  // Like this.
  vertexInputInfo.vertexBindingDescriptionCount   = 1;
  vertexInputInfo.pVertexBindingDescriptions      = &sh::DDefaultVertex::GetBindingDescription();
  vertexInputInfo.vertexAttributeDescriptionCount = TU32(sh::DDefaultVertex::GetAttributeDescriptons().size());
  vertexInputInfo.pVertexAttributeDescriptions    = sh::DDefaultVertex::GetAttributeDescriptons().data();

  // (2) Input assembly
  // creatInfo structure describes two things.
  // 1. What kind of geometry ((primitive)) will be drawn.
  // 2. if primitive restart should be enabled. (like a OpenGL's PrimitiveRestart)
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineInputAssemblyStateCreateInfo.html
  //
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
  inputAssemblyInfo.sType     = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyInfo.topology  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  // (3) Setting up Viewport & Scissor testing.
  // Viewport basically describes the region of the framebuffer that the output will be rendered to.
  // but scissor testing also relevent to rendering of something into screen.
  // Difference is viewport is resizing of rendered image totally so image can be shrinked or magnified.
  // but Scissor test is just crop total rendered image with blank space of overall window,
  // so does not shrink or magnify rendered image.
  // VkViewport : https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkViewport.html
  // 
  VkViewport viewport;
  viewport.x      = 0.0f; 
  viewport.y      = 0.0f;
  viewport.width  = static_cast<TF32>(this->mSwapChainExtent.width); 
  viewport.height = static_cast<TF32>(this->mSwapChainExtent.height);
  viewport.minDepth = 0.0f; viewport.maxDepth = 1.0f;

  VkRect2D scissor;
  scissor.offset = {0, 0};
  scissor.extent = this->mSwapChainExtent;

  // And construct sturcture for viewport & scissor testing.
  // Structure specifying parameters of a newly created pipeline viewport state
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineViewportStateCreateInfo.html
  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount  = 1;
  viewportState.pViewports    = &viewport;
  viewportState.pScissors     = &scissor;

  // (4) Rasterizer
  // Geometry shader will be omitted because we just use vertex & fragment shader.
  // In rasterizer stage, performs depth testing & face culling & the scissor test.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineRasterizationStateCreateInfo.html
  VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
  rasterizerInfo.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  // If z/w value is out of bound [0, 1], just discard them.
  rasterizerInfo.depthClampEnable = VK_FALSE; 
  // VK_TRUE might be used when transform feedback (using only pre-rasterization phase)
  rasterizerInfo.rasterizerDiscardEnable = VK_FALSE; 
  rasterizerInfo.polygonMode  = VK_POLYGON_MODE_FILL;
  rasterizerInfo.lineWidth    = 1.0f;
  rasterizerInfo.cullMode     = VK_CULL_MODE_BACK_BIT;
  rasterizerInfo.frontFace    = VK_FRONT_FACE_CLOCKWISE;
  // We don't use depth bias now. (This used when implementing shadow mapping)
  rasterizerInfo.depthBiasEnable = VK_FALSE;

  // (5) Setting multisampling descriptor.
  // Setting to perform anti-aliasing.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineMultisampleStateCreateInfo.html
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable   = VK_FALSE;
  multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading      = 1.0f;     // Optional
  multisampling.pSampleMask           = nullptr;  // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable      = VK_FALSE; // Optional

  // (6) Depth and stencil testing descriptor.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineDepthStencilStateCreateInfo.html
  

  // (7) Setting color blending descriptor.
  // After a fragment shader has returned a color (source),
  // it need to be combined with the color that is already in the framebuffer (dest).
  //
  // First, we need to setup first struct, that contains the configuration per attached framebuffer.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineColorBlendAttachmentState.html
  VkPipelineColorBlendAttachmentState colorBlendingAttachment;
  // VkColorComponentFlagBits : How to write into given each channel.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkColorComponentFlagBits.html
  colorBlendingAttachment.colorWriteMask = 
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_B_BIT 
    | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendingAttachment.blendEnable = VK_FALSE;
  // Default value is not specified, so we should have specify values.
  colorBlendingAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendingAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendingAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
  colorBlendingAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  
  colorBlendingAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
  colorBlendingAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

  // Second structure references the array of structures for all of the framebuffers.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineColorBlendStateCreateInfo.html
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable     = VK_FALSE; // If enabled, this will disable first function blending.
  colorBlending.logicOp           = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount   = 1;
  colorBlending.pAttachments      = &colorBlendingAttachment;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional

  // (8) Setting dynamic state.
  // A limited amount of the state in the previous structs can actually be chagned without
  // reconstructing all of pipeline.
  // Viewport, line width, blend constant can be changed in real-time.
  //
  // Do these, have to fill `VkPipelineDynamicStateCreateInfo` structure.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineDynamicStateCreateInfo.html

  // (9) Pipeline layout 
  // To create pipeline, and to access uniform variable and unifrom buffer object, etc,
  // need to be specified during pipeline creation by creating `VkPipelineLayout` object.
  // Even though unifrom varaibles are not used yet, must be required one pipeline layout.
  //
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkCreatePipelineLayout.html
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0; // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  // Created pipeline layout must be destroyed explicitly.
  if (vkCreatePipelineLayout(this->mGraphicsDevice, &pipelineLayoutInfo, nullptr, &this->mPipelineLayout) 
      != VK_SUCCESS) 
  {
      throw std::runtime_error("failed to create pipeline layout!");
  }

  // (10) All of these combined and create VkGraphicsPipeline.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineShaderStageCreateInfo.html
  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages    = iShaderStages.data();
  pipelineInfo.pVertexInputState    = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState  = &inputAssemblyInfo;
  pipelineInfo.pViewportState       = &viewportState;
  pipelineInfo.pRasterizationState  = &rasterizerInfo;
  pipelineInfo.pMultisampleState    = &multisampling;
  pipelineInfo.pColorBlendState     = &colorBlending;
  pipelineInfo.pDepthStencilState   = nullptr;
  pipelineInfo.pDynamicState        = nullptr;

  // It is also possible to use other render passes with this pipeline instead of this specific instance, 
  // but they have to be compatible with renderPass
  // https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#renderpass-compatibility
  pipelineInfo.layout     = this->mPipelineLayout;
  pipelineInfo.renderPass = this->mRenderPass;
  pipelineInfo.subpass    = 0; // Index.

  // Vulkan allows you to create a new graphics pipeline by deriving old exist one.
  // It's less expensive than creating all settings to create new one.
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1; // Optional

  if (vkCreateGraphicsPipelines(
      this->mGraphicsDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->mPipeline)
      != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create graphics pipeline.");
  }
}

void MVulkanRenderer::CreateFrameBuffer()
{
  // Resize...
  this->mSwapChainFrameBuffers.resize(this->mSwapChainImageViews.size());

  // 
  for (size_t i = 0; i < this->mSwapChainImageViews.size(); ++i)
  {
    VkImageView attachment = this->mSwapChainImageViews[i];

    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkFramebufferCreateInfo.html
    VkFramebufferCreateInfo frameBufferInfo = {};
    frameBufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.renderPass      = this->mRenderPass;
    frameBufferInfo.attachmentCount = 1;
    frameBufferInfo.pAttachments    = &attachment;
    frameBufferInfo.width           = this->mSwapChainExtent.width;
    frameBufferInfo.height          = this->mSwapChainExtent.height;
    frameBufferInfo.layers          = 1;

    if (vkCreateFramebuffer(this->mGraphicsDevice, &frameBufferInfo, 
        nullptr, &this->mSwapChainFrameBuffers[i])
        != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create framebuffer.");
    }
  }
}

void MVulkanRenderer::CreateCommandPool()
{
  auto queueFamilyIndices = this->GetFindQueueFamilies(this->mPhysicalDevice, VK_QUEUE_GRAPHICS_BIT);

  // Fill information to command pool create info instance.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkCommandPoolCreateInfo.html
  VkCommandPoolCreateInfo createInfo = {};
  createInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  createInfo.queueFamilyIndex = *queueFamilyIndices.moptGraphicsQueueFamiliy;
  // Command bffers are executed by submitting them on one of the device queues,
  // Each command pool can only allocate command bufrs that are submited on single type of queue.
  // in flags, there are possible flags that change allocation behaviour of command queue.
  createInfo.flags            = 0;

  // Created command pool handle instance must be destroyed explicitly.
  if (vkCreateCommandPool(this->mGraphicsDevice, &createInfo, nullptr, &this->mCommandPool) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create command pool.");
  }
}

void MVulkanRenderer::CreateCommandBuffers()
{
  // (1) Command buffers resizing following framebuffer list size.
  this->mCommandBuffers.resize(this->mSwapChainFrameBuffers.size());

  // (2) Command buffers are allocated with the `VkCommandBufferAllocateInfo`. 
  // This speicifies the command pool and number of buffer to allocate.
  //
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkCommandBufferAllocateInfo.html
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType               = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool         = this->mCommandPool;
  // PRIMARY : Can be submitted to queue for execution diretly, but cannot be called from other buffer.
  // SECONDARY : Cannot be submitted to queue, but can be called from other buffer.
  // SECONDARY level is helpful to reuse common operations.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VkCommandBufferLevel
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkCommandBufferLevel.html
  allocInfo.level               = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount  = static_cast<TU32>(this->mCommandBuffers.size());

  if (vkAllocateCommandBuffers(this->mGraphicsDevice, &allocInfo, this->mCommandBuffers.data())
      != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to allocated command buffers.");
  }

  // (3) Begin recording a command buffer with `VkCommandBufferBeginInfo`.
  // This structure specifies some details about the usage of this specific command buffer.
  for (size_t i = 0; i < this->mCommandBuffers.size(); ++i)
  {
    // Structure specifying a command buffer begin operation
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkCommandBufferBeginInfo.html
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // `flags` parameter specifies how we're going to use the command buffer.
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkCommandBufferUsageFlagBits.html
    // In this case, we are going to use STIMUTANEOUS because we may already be scheduling the drawing
    // commands for the next frame while the last frmae is not finished yet.

    // ...specifies that a command buffer can be resubmitted to a queue while it is in the pending state, 
    // and recorded into multiple primary command buffers...
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr; // Is only relevant for secondary command buffer.

    // If the command buffer was already recorded once, then a call to `vkBeginCommandBuffer`
    // will implicitly reset it.
    // Start recording a command buffer.
    if (vkBeginCommandBuffer(this->mCommandBuffers[i], &beginInfo) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to begin recording command buffer.");
    }

    // Drawing starts by beginning the render pass with `vkCmdBeginRenderPass`.
    // using `VkRenderPassBeginInfo`...
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkRenderPassBeginInfo.html
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType      = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->mRenderPass;
    renderPassInfo.framebuffer= this->mSwapChainFrameBuffers[i];
    // Render area defines where shader loades and stores will take place.
    // It should match the size of the attachments for performance.
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = this->mSwapChainExtent;
    // Clear color to attachment 1 of framebuffer.
    // This parameters are for VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkClearValue clearColor         = {0, 0, 0, 1};
    renderPassInfo.clearValueCount  = 1; 
    renderPassInfo.pClearValues     = &clearColor;

    // Begin a new render pass + Push commands. 
    // Upper `vkBeginCommandBuffer` is just reset command buffer and start to recording commands.
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkCmdBeginRenderPass.html
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkSubpassContents.html
    // INLINE must be executed on primary buffer.
    vkCmdBeginRenderPass(this->mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // We can now bind the graphics pipeline.
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkCmdBindPipeline.html
    // specifies binding as a graphic pipeline.
    vkCmdBindPipeline(this->mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->mPipeline);

    std::vector<VkBuffer> vertexBuffers = {sVertexBufferObject};
    std::vector<VkDeviceSize> offset = {0};
    vkCmdBindVertexBuffers(this->mCommandBuffers[i], 0, 1, vertexBuffers.data(), offset.data());

    // Draw!! (glDrawArrays)
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkCmdDraw.html
    vkCmdDraw(this->mCommandBuffers[i], static_cast<TU32>(sTempVertices.size()), 1, 0, 0);

    // Finish render pass. 
    vkCmdEndRenderPass(this->mCommandBuffers[i]);
    if (vkEndCommandBuffer(this->mCommandBuffers[i]) != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to record command buffer.");
    }
  }
}

void MVulkanRenderer::CreateDefaultSemaphores()
{
  this->mSemaphoreRenderFinished.resize(kMaxFramesInFlight);
  this->mSemaphoreImageAvailable.resize(kMaxFramesInFlight);
  this->mFencesInFlight.resize(kMaxFramesInFlight);

  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkSemaphoreCreateInfo.html
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  // We must check this flag for enter initial render phase,
  // default vulkan fence must be signalized before using but as setting SINGALED_BIT flag
  // we can create VkFence as signalized in advance.
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < kMaxFramesInFlight; ++i)
  {
    if (vkCreateSemaphore(this->mGraphicsDevice, &semaphoreInfo, nullptr, &this->mSemaphoreImageAvailable[i])
        != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create semaphore.");
    }

    if (vkCreateSemaphore(this->mGraphicsDevice, &semaphoreInfo, nullptr, &this->mSemaphoreRenderFinished[i])
        != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create semaphore.");
    }

    if (vkCreateFence(this->mGraphicsDevice, &fenceInfo, nullptr, &this->mFencesInFlight[i])
        != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create synchronization objects for a frame.");
    }
  }
}

void MVulkanRenderer::CreateVertexBuffer()
{
  // (1) Create buffer creation information.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkBufferCreateInfo.html
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size   = sizeof(sTempVertices[0]) * sTempVertices.size();
  // How to use this buffer.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkBufferUsageFlagBits.html
  bufferInfo.usage  = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; // VBO in OpenGL?
  // Access to any range or image subresouce of the object will be exclusive and not shared.
  // Just like the images in the swap chain, buffers can also be owned by a specified queue (EXCLUSIVE)
  // family or be shared between multiple at same time (CONCURRENT)
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkSharingMode.html
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  
  // Created VtBuffer must be destroyed explicitly.
  // However, vkCreateBuffer does not allocate any memory space for it.
  // So, after this user should fill `VkMemoryRequirements`.
  // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCreateBuffer.html
  if (vkCreateBuffer(this->mGraphicsDevice, &bufferInfo, nullptr, &sVertexBufferObject)
      != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to create vertex buffer.");
  }

  // (2) Allocating memory for the buffer above is to query its memory requirements
  // using name `vkGetBufferMemoryRequirements`...
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkGetBufferMemoryRequirements.html
  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(this->mGraphicsDevice, sVertexBufferObject, &memoryRequirements);

  // Graphics cards can offer different types of memory to allocate from. (IMPORTANT)
  // Independent to bufferInfo yet.
  VkMemoryAllocateInfo allocateInfo = {};
  allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocateInfo.allocationSize = memoryRequirements.size;
  // To allocate buffer memory to GPU VRAM, we need VISIBLE_BIT and also COHERENT_BIT.
  // COHERENT_BIT flag would be needed to synchronize and avoid indirect bidning to cache instead of
  // direct VRAM memory space.
  allocateInfo.memoryTypeIndex = FindMemoryTypes(
      memoryRequirements.memoryTypeBits, 
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  // Create a class member to store the handle to the VRAM memory and allocate it.
  // https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkAllocateMemory.html
  if (vkAllocateMemory(this->mGraphicsDevice, &allocateInfo, nullptr, &sVertexBufferMemory)
      != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to allocate vertex buffer memory.");
  }

  // (3) If memory allocation is successful, finally associate this memory buffer with
  // the buffer using `vkBindBufferMemory`.
  // the fourth parameter is the offset within the region of memory. 
  // If the offset is non-zero, then it is required to be divisible by `memRequirements.alignment`.
  if (vkBindBufferMemory(this->mGraphicsDevice, sVertexBufferObject, sVertexBufferMemory, 0)
      != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to bind buffer memory.");
  }

  // (4) Copy the vertex data to the VRAM buffer.
  // This is done by mapping the buffer memory into CPU accessible memory `vkMapMemory`.
  // So,... using `vkMapMemory`, VRAM buffer memory will be mapped into CPU memory to be accessible
  // from CPU code.
  void* data;
  // `vkMapMemory` function allows us to access a region of the specified memory resource
  // defined by an [s := sVertexBufferMemory + offset, s + bufferInfo.size). 
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkMapMemory.html
  vkMapMemory(this->mGraphicsDevice, sVertexBufferMemory, 0, bufferInfo.size, 0, &data);

  // Unfortunately the driver may not immediately copy the data into the buffer memory, 
  // for example because of caching. 
  // It is also possible that writes to the buffer are not visible in the mapped memory yet. 
  // There are two ways to deal with that problem:
  //
  // 1. Use a memory heap that is host coherent, indicated with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  // 2. Call vkFlushMappedMemoryRanges to after writing to the mapped memory, 
  // and call vkInvalidateMappedMemoryRanges before reading from the mapped memory
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkFlushMappedMemoryRanges.html
  memcpy(data, sTempVertices.data(), static_cast<size_t>(bufferInfo.size));
  vkUnmapMemory(this->mGraphicsDevice, sVertexBufferMemory);

  // Flushing memory ranges or using a coherent memory heap means that 
  // the driver will be aware of our writes to the buffer, 
  // BUT IT DOESN'T MEAN THAT THEY ARE ACTUALLY VISIBLE ON THE GPU YET. 
  // The transfer of data to the GPU is an operation that happens in the background 
  // and the specification simply tells us that it is guaranteed to be complete 
  // as of the next call to `vkQueueSubmit`.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkQueueSubmit.html
}

TU32 MVulkanRenderer::FindMemoryTypes(TU32 iTypeFilter, VkMemoryPropertyFlags iProperties)
{
  // First we need to query information about the available types of memory.
  // We should note `memoryTypes` and `memoryHeaps`.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPhysicalDeviceMemoryProperties.html
  VkPhysicalDeviceMemoryProperties memoryProperties;
  vkGetPhysicalDeviceMemoryProperties(this->mPhysicalDevice, &memoryProperties);

  // `memoryHeaps` are VRAM or swap space of RAM when VRAM runs out.
  // We should concern with the type of memory, not the heap itself.
  for (TU32 i = 0; i < memoryProperties.memoryTypeCount; ++i)
  {
    // Equal to the desired proeprties bit field.
    // If there is a memory type suitable for the buffer that also has all of their properties,
    // return index i.
    if (iTypeFilter & (1 << i)
    &&  (memoryProperties.memoryTypes[i].propertyFlags & iProperties) == iProperties) 
    {
      return i;
    }
  }

  throw std::runtime_error("Failed to find suitable memory type.");
}

void MVulkanRenderer::RecreateSwapChain()
{
  // When minimized, width and height will be 0.
  // But when size is 0, recreation of swapchain might be failed, so loop until unminimized.
  int width = 0, height = 0;
  while (width == 0 || height == 0)
  {
    glfwGetFramebufferSize(this->mGlfwWindow, &width, &height);
    glfwWaitEvents();
  }

  // Wait until previous and already required commands are executed.
  vkDeviceWaitIdle(this->mGraphicsDevice);
  this->CleanupSwapChain();

  // Create swap chain. This function must be succeeded.
  this->CreateSwapChain();
  this->mSwapChainImages = this->GetSwapChainImageHandles(this->mSwapChain);
  this->CreateSwapChainImageViews();
  this->CreateRenderPass();
  this->CreateGraphicsPipeline();
  this->CreateFrameBuffer();
  this->CreateCommandBuffers();
}

void MVulkanRenderer::CleanupSwapChain()
{
  for (auto& swapChainFrameBuffer : this->mSwapChainFrameBuffers)
  {
    vkDestroyFramebuffer(this->mGraphicsDevice, swapChainFrameBuffer, nullptr);
  }

  // Clean up the existing command buffers with the `vkFreeCommandBuffers`.
  // By calling this function, we can reuse the existing pool. (not buffer)
  vkFreeCommandBuffers(
      this->mGraphicsDevice, this->mCommandPool, 
      static_cast<TU32>(this->mCommandBuffers.size()), this->mCommandBuffers.data());

  vkDestroyPipeline(this->mGraphicsDevice, this->mPipeline, nullptr);
  vkDestroyPipelineLayout(this->mGraphicsDevice, this->mPipelineLayout, nullptr);
  vkDestroyRenderPass(this->mGraphicsDevice, this->mRenderPass, nullptr);

  for (auto& imageView : this->mSwapChainImageViews)
  {
    vkDestroyImageView(this->mGraphicsDevice, imageView, nullptr);
  }

  // Clean up code.
  vkDestroySwapchainKHR(this->mGraphicsDevice, this->mSwapChain, nullptr);
}

EDySuccess MVulkanRenderer::pfRelease()
{
  this->CleanUp();
  return DY_SUCCESS;
}

void MVulkanRenderer::CleanUp()
{
  // To synchronize drawFrame functions, this function must be called.
  vkDeviceWaitIdle(this->mGraphicsDevice);
  this->CleanupSwapChain();

  vkFreeMemory(this->mGraphicsDevice, sVertexBufferMemory, nullptr);
  vkDestroyBuffer(this->mGraphicsDevice, sVertexBufferObject, nullptr);

  for (auto& fence : this->mFencesInFlight)
  {
    vkDestroyFence(this->mGraphicsDevice, fence, nullptr);
  }
  for (auto& semaphore : this->mSemaphoreImageAvailable)
  {
    vkDestroySemaphore(this->mGraphicsDevice, semaphore, nullptr);
  }
  for (auto& semaphore : this->mSemaphoreRenderFinished)
  {
    vkDestroySemaphore(this->mGraphicsDevice, semaphore, nullptr);
  }

  vkDestroyCommandPool(this->mGraphicsDevice, this->mCommandPool, nullptr);
  vkDestroyDevice(this->mGraphicsDevice, nullptr);
  vkDestroySurfaceKHR(this->mInstance, mSurface, nullptr);

  if constexpr (kEnabledValidationLayers == true)
  {
    // validation messenger ext instance must be removed before detroying vkInstance.
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(this->mInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) 
    {
      func(this->mInstance, this->mMessengerExt, nullptr);
    }
  }

  // If want to destroy VkInstance, just call `vkDestroyInstance` function API.
  vkDestroyInstance(this->mInstance, nullptr);

  glfwDestroyWindow(this->mGlfwWindow);
  glfwTerminate();
}

void MVulkanRenderer::DrawFrame()
{
  // Wait fence. GPU-GPU.
  // Wait for one or more fences to become signaled
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkWaitForFences.html
  vkWaitForFences(
      this->mGraphicsDevice, 1, 
      &this->mFencesInFlight[this->mCurrentRenderFrame], 
      VK_TRUE, NumericalMax<TU64>); // Check already (Unsignal => Signaled)

  // (1) Acquire an image from the swap chain.
  // https://vulkan.lunarg.com/doc/view/1.0.33.0/linux/vkspec.chunked/ch29s06.html
  //
  // Third parameter is timeout of acquiring next image, with nanoseconds.
  // But using TU64::max disables timeout feature.
  //
  // semaphore and fence (Forth, Fifth) parameter specify synchronization objects that
  // are to be signaled when the presentation engine is finished using the image.
  // When signaled, we can start drawing to given image.
  TU32 imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      this->mGraphicsDevice, this->mSwapChain, NumericalMax<TU64>, 
      this->mSemaphoreImageAvailable[this->mCurrentRenderFrame], VK_NULL_HANDLE, &imageIndex);

  // (1+) Luckily, 
  // Vulkan will usually just tell us that the swap chain is no longer adequate during presentation. 
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
  ||  this->mIsWindowResizeDirty == true)
  { // OUT_OF_DATE_KHR : The swap chain is not able to present image to screen.
    // VK_SUBOPTIMAL_KHR : The swap chain can still be presentable but properties not matched exactly.
    this->mIsWindowResizeDirty = false;
    this->RecreateSwapChain();
    return;
  }
  else if (result != VK_SUCCESS)
  { 
    throw std::runtime_error("Failed to acquire swap chain image.");
  }
  
  // If we get imageIndex, imageIndex refers to the `VkImage` in member variable.
  // (If we align list of VkImage, RIP)

  // (2) Queue submission and synchronization is configured using `VkSubmitIfo` structure.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkSubmitInfo.html
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  // We want to wait with writing colors to the image until it's available,
  // VPSCAOB is the stage that writes to the color attachment.
  std::vector<VkSemaphore> waitSemaphores       = {this->mSemaphoreImageAvailable[this->mCurrentRenderFrame]};
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineStageFlagBits.html
  std::vector<VkPipelineStageFlags> waitStages  = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  // Three parameters specify which semaphores to wait on before execution begins,
  // and in which stages of the pipeline to wait.
  // So, `mSemaphoreImageAvailable` will be await before the stage of fragment shader,
  // but vertex shader will be proceeded even though `vkAcquireNextImageKHR` is not finished yet.
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores    = waitSemaphores.data();
  submitInfo.pWaitDstStageMask  = waitStages.data();
  // Get command buffer that corresponds to acquired VkImage of swap chain.
  // Acquired command buffers are primary buffer and be executed and store swapchain framebuffer.
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &this->mCommandBuffers[imageIndex];
  // The signalSemaphoreCount and pSignalSemaphores parameters specify which semaphores 
  // to signal once the command buffer(s) have finished execution.
  std::vector<VkSemaphore> signalSemaphore = {this->mSemaphoreRenderFinished[this->mCurrentRenderFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = signalSemaphore.data();

  // Reset one or more fence object. (Signaled => Unsignaled)
  // it defines a fence unsignal operation for each fence, which resets the fence to the unsignaled state.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkResetFences.html
  vkResetFences(this->mGraphicsDevice, 1, &this->mFencesInFlight[this->mCurrentRenderFrame]);

  // Push submit information into graphics queue. (Wait) If end, Unsig => sig async.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkQueueSubmit.html
  if (vkQueueSubmit(this->mGraphicsQueue, 1, 
      &submitInfo, this->mFencesInFlight[this->mCurrentRenderFrame]) != VK_SUCCESS)
  {
    throw std::runtime_error("Failed to submit draw command buffer.");
  }

  // (3) Presentation
  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount  = 1;
  presentInfo.pWaitSemaphores     = signalSemaphore.data();

  std::vector<VkSwapchainKHR> swapChains = {this->mSwapChain};
  presentInfo.swapchainCount      = 1;
  presentInfo.pSwapchains         = swapChains.data();
  presentInfo.pImageIndices       = &imageIndex;

  // Present image to screen.
  vkQueuePresentKHR(this->mPresentQueue, &presentInfo);

  // By using the modulo (%) operator, 
  // we ensure that the frame index loops around after every MAX_FRAMES_IN_FLIGHT enqueued frames.
  this->mCurrentRenderFrame = (this->mCurrentRenderFrame + 1) % kMaxFramesInFlight;
}

void MVulkanRenderer::CbGLFWFrameBufferResize(
    GLFWwindow* iPtrWindow, 
    [[maybe_unused]] int width, 
    [[maybe_unused]] int height)
{
  auto app = static_cast<MVulkanRenderer*>(glfwGetWindowUserPointer(iPtrWindow));
  app->mIsWindowResizeDirty = true;
}
