#pragma once
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

#include "IHelperSingleton.h"
#include "ASystemInclude.h"
#include "DQueueFamilyIndices.h"
#include "DVkSwapChainSupportDetails.h"

class MVulkanRenderer final : public IHelperSingleton<MVulkanRenderer>
{
public:
  MCY_SINGLETON_PROPERTIES(MVulkanRenderer);
  MDY_SINGLETON_DERIVED(MVulkanRenderer);

  /// @brief Draw frame using command queue + render pass into given framebuffer using ImageView
  /// that is wrapper of VkImage from swap chain extension for rendering & presenting to screen.
  ///
  /// This function will perform the following operations.
  /// 1. Acquire an image from the swap chain.
  /// 2. Execute the command buffer with that image as attachment in the framebuffer.
  /// 3. Return the image to the swap chain for presentation.
  ///
  /// But they are executed asynchronously. So, Function calls will return before the operation are
  /// actually finished, and afterward if next function is called, result will be undefined.
  /// So we have to use fence, or semaphore to synchronize exeuction.
  /// Vulkan provides `VkSemaphore` or `VkWaitForFences` to blocking sequences.
  /// 
  /// `Fence` are manily designed to synchornize application itself with rendering operation.
  /// `Semaphore` are used to synchornize operations within or across command queues.
  ///
  /// In this case, rendering queue and presenting queue should be synchornized so 
  /// we have to semaphores `VkSemaphore`.
  void DrawFrame();
  /// 
  void UpdateUniformBuffer(TU32 iCurrentImageIndex);

private:
  /// @brief Framebuffer resization callback function.
  static void CbGLFWFrameBufferResize(GLFWwindow* iPtrWindow, int width, int height);

  /// @brief Create glfw window instance.
  void InitGlfw();

  /// @brief 
  ///
  /// @memo VULKAN GENERAL PATTERN FOR CREATION OF OBJECT...
  /// * Pointer to struct with creation info as VkInstanceCreationInfo for creating vk instance.
  /// * Pointer to custom allocator callbacks.
  /// * Pointer to the variable that stores the `handle` to the new object.
  MCR_NODISCARD VkInstance pCreateVulkanInstance();
  /// @brief
  MCR_NODISCARD bool ppVkCheckValidationLayerSupport();
  /// @brief
  MCR_NODISCARD std::vector<const char*> ppVkGetRequiredExtensions();

  /// @brief
  /// @memo VULKAN validation layer system.
  /// https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers 
  /// Handling error and events checking in the Vulkan API.
  /// Hook into vulkan function calls to apply addtional operations.
  /// Common operations in validation layers are...
  ///
  /// 1. Checking the values of parameters against the specification to detect misuse.
  /// 2. Tracking creation and destruction of vulkan object. & find mem leak.
  /// 3. Checking thread safety by tracking the threads that calls originate from...
  /// 4. Logging every vulkan call and its parateters.
  /// 5. Tracing Vulkan calls for profiling and replaying.
  ///
  /// Vanilla Vulkan does not come with any validation layers built-in, but the LunarG Vulkan SDK provides
  /// a nice set of layers that check for common errors.
  /// https://github.com/KhronosGroup/Vulkan-ValidationLayera
  ///
  /// Validation layers can only be used if they have been installed onto the system.
  /// Vulkan's validation layers were separated by two layers (instance & device)
  /// But device are now deprecated and only instance layer are called and applied to all Vulkan calls.
  ///
  /// Instance layers would only check calls related to global Vulkan object and GPU call.
  /// The specification document still recommends that you enable validation layers at device level 
  /// sas well for compatibility, which is required by some implementations. 
  MCR_NODISCARD VkDebugUtilsMessengerEXT pSetupDebugManager(VkInstance iInstance);

  /// @brief Get valid physical device that supports Vulkan API given version.
  MCR_NODISCARD VkPhysicalDevice pPickPhysicalDevice(VkInstance iInstance);
  /// @brief Check physical device is suitable.
  MCR_NODISCARD bool ppIsDeviceSuitable(VkPhysicalDevice iPhysicalDevice);
  /// @brief Check physical device extensions with `iExtensionRequisition`.
  MCR_NODISCARD bool CheckDeviceExtensionSupport(
      VkPhysicalDevice iPhysicalDevice, 
      const std::vector<const char*>& iExtensionRequsition);
  /// @brief Get queue families that supports graphics queue.
  MCR_NODISCARD DVkQueueFamilyIndices GetFindQueueFamilies(
      VkPhysicalDevice iPhysicalDevice,
      VkQueueFlagBits iQueueFlagBits);
  /// @brief Check swapchain support query to valid physical device, and get details instance.
  MCR_NODISCARD DVkSwapChainSupportDetails QuerySwapChainSupport(
      VkPhysicalDevice iPhysicalDevice);

  /// @brief
  MCR_NODISCARD std::tuple<VkDevice, VkQueue, VkQueue> pCreateVkLogicalDevice(VkPhysicalDevice iPhysicalDevice);

  /// @brief Create swap chain.
  /// Before call this function, VkPhysicalDevice and VkSurfaceKHR are valid,
  /// and make sure VkSurfaceKHR must be created from VkPhysicalDevice. \n
  ///
  /// To create SwapChain instances but more optimally, have to check right settings to create
  /// the best possible swap chain because there may still be many different modes. \n
  /// 
  /// We need to set three types in this function.
  /// 1. Surface format (color & depth)
  /// 2. Presentation mode (conditions for "swapping" images to screen)
  /// 3. Swap extent (resolution of images)
  void CreateSwapChain();
  /// @brief Create preferred color format and colorspace format of SwapChain.
  /// To see information of `VkSurfaceFormatKHR`, 
  /// access to https://vulkan.lunarg.com/doc/view/1.0.26.0/linux/vkspec.chunked/ch29s05.html
  MCR_NODISCARD VkSurfaceFormatKHR ChoooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& iAvailableFormatList);
  /// @brief Create present mode that represents the actual conditions for showing images to the screen. 
  /// 
  /// There are four possible mode available in Vulkan. 
  /// 1. VK_PRESENT_MODE_IMMEDIATE_KHR : Image submitted are transferred to the screen right away.
  /// But may cause tearing.
  /// 2. VK_PRESENT_MODE_FIFO_KHR : The swap chain will be queue, If the queue is full, then program has to wait.
  /// 3. VK_PRESENT_MODE_FIFO_RELAXED_KHR : If the queue is empty, the image that arrived to the swap chain
  /// is transferred right away before next refresh of screen. This may cause tearing.
  /// 4. VK_PRESENT_MODE_MAILBOX_KHR : FIFO's variation. Instead of blocking when full, the images that are
  /// queued are simpled replaced with the newer image.
  /// This mode can be used to implement triple buffering.
  MCR_NODISCARD VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& iAvailablePresentModeList);
  /// @brief Get the swap extent that is the resolution of the swap chain images.
  MCR_NODISCARD VkExtent2D ChooseSwapResolution(const VkSurfaceCapabilitiesKHR& iCapabiliteis);

  /// @brief Get handles of VkImage in given valid iSwapChain instance.
  MCR_NODISCARD std::vector<VkImage> GetSwapChainImageHandles(VkSwapchainKHR& iSwapChain);

  /// @brief Create member of `VkImageView` list that matches to `VkImage` handle of valid SwapChain.
  ///
  /// To use any `VkImage`, including thoes in the create swap chain, use must create `VkImageView` object. 
  /// `VkImageView` describes how to access the image and which part of the image to access,
  /// such as 2D texure depth texture, color texture, or etc.
  void CreateSwapChainImageViews();

  /// @brief We need to also specify and create VkImage, VkMem, VkImageView for depth attachment.
  /// and created resource will be attached when creating render pass of graphics rendering.
  ///
  /// Created depth image should have the same resolution as the color attachment,
  /// defined by the swap chain extent, and more options.
  void CreateDefaultDepthResource();
  MCR_NODISCARD VkFormat FindDepthFormat();
  /// @brief Find most optimal supported format.
  MCR_NODISCARD VkFormat FindSuppotedFormat(
      const std::vector<VkFormat>& iCandidates, 
      VkImageTiling iTiling,
      VkFormatFeatureFlags iFeatures);
  MCR_NODISCARD bool HasStencilCompnent(VkFormat iFormat);
   
  /// @brief Before creating the pipeline, need to tell Vulkan about framebuffer attachments that
  /// will be used while rendering like a how many color and depth buffer there will be,
  /// and how many samples to use for each of them, how their contents should be handled.
  ///
  /// This function must be called before CreateGraphicsPipeline because
  /// Vulkan must have framebuffer attachment information in advance.
  void CreateRenderPass();

  /// @brief Create `Resource Descriptor` that lets shaders freely access resources like buffers
  /// and images. (as textures)
  /// There are many types of descriptors.
  /// Descriptor layout describese the type of descriptors that can be bound.
  ///
  /// To handle uniform buffer & uniform varaibles of specified pipeline stage, we should create
  /// Resource descriptor to be accessed from shader of pipeline.
  /// 
  /// Usage of descriptors consists of three parts...
  /// 1. Specify a descriptor layout during pipeline creation.
  /// 2. Allocate a descriptor set from a descriptor pool.
  /// 3. Bind the descripor set during rendering.
  /// 
  /// And `Descriptor layout` specifies the types of resources that are going to be accessed by the pipeline,
  /// just like a render pass specifies the types of attachments that will be accessed.
  ///
  /// `Descriptor set` specifies the actual bufferor image resources that will be bound to the descriptors,
  /// just like a framebuffer specifies the actual image views to bind to render pass attachments.
  ///
  /// In this function, we just create descriptor layout & set as UBO.
  void CreateDescriptorSetLayout();
  /// @brief Create graphics pipeline for actual rendering.
  void CreateGraphicsPipeline();
  /// @brief Create shader module with spir-v code buffer.
  /// spir-v code buffer size must be 4 times integer.
  /// VkShaderModule : 
  /// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkShaderModule.html
  VkShaderModule CreateShaderModule(const std::vector<char>& iCodeBuffer);
  /// @brief In Vulkan, you have to be explicit about everything, 
  /// including setting up the stages of graphics pipeline.
  void CreateFixedRenderPipeline(const std::vector<VkPipelineShaderStageCreateInfo>& iShaderStages);

  /// @brief After creating pipeline, but need to create framebuffer instance explicitly.
  /// Swap-chain and subpass are not same to framebuffer but just describes how to render using
  /// framebuffer to be created in this function.
  void CreateFrameBuffer();

  /// @brief Create command pool for storing & operating commands. 
  /// Command pool must be explicitly created and destroyed, creating swap chain does not create
  /// command pool implicitly, just know how to control these.
  /// 
  /// Commands like drawing operations and memory transfers are not exected directly using
  /// just function call. You have to record all of the operations in command buffer object.
  ///
  /// The advantage of this is all of setting up the drawing commands can be done in adanvce,
  /// and in multiple threads.
  /// After that, user can tell Vulkan to execute the commands in the main loop.
  void CreateCommandPool();
  /// @brief Create command buffers for each image of specified swap chain.
  ///
  /// One of the drawing commands involveds binding the right `VkFramebuffer`, so we actually
  /// have to record a command buffer for every image in the swap chain once again.
  void CreateCommandBuffers();

  /// @brief Create default semaphores to be used when rendering and synchornize between
  /// rendering queue and present queue of default (first) framebuffer & swap chain.
  /// Created semaphores must be destroyed explicitly.
  void CreateDefaultSemaphores();

  /// @brief Create texture image.
  ///
  /// We've seen before, with the swap chain images and the framebuffer, that images are accessed
  /// through image views rather than directly.
  /// We will also need to create such an image view for the texture image.
  void CreateTextureImage();
  /// @brief
  void CreateImage(
      TU32 iWidth, TU32 iHeight, VkFormat iFormat, VkImageTiling iTiling,
      VkImageUsageFlags iUsage, VkMemoryPropertyFlags iProperties,
      VkImage& outImage, VkDeviceMemory& outImageMemory);
  /// @brief Handle layout transition.
  /// When copy buffer to image, we must check the image (destination) to be in the
  /// right layout first.
  ///
  /// @param iOldLayout the old layout in an image layout transition.
  /// @param iNewLayout the new layout in an image layout tarnsition.
  /// @link https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#synchronization-image-layout-transitions
  void TransitImageLayout(
      VkImage iImage, VkFormat iFormat, 
      VkImageLayout iOldLayout, VkImageLayout iNewLayout);

  /// @brief Create texture image view for accessing texture image.
  void CreateTextureImageView();
  /// @brief Create image view with image and given format.
  MCR_NODISCARD VkImageView CreateImageView(    
      VkImage iImage, 
      VkFormat iFormat, 
      VkImageAspectFlagBits iAspectMaskFlag);

  /// @brief Create samplers for texture to access following special way,
  /// such as GL_REPEAT, Bilinear filtering, Anisotroic, etc...
  ///
  /// in Vulkan, textures are usually accessed through samplers, which will apply filtering
  /// said above and transformation to compute the fianl color that is retrieved.
  /// Oversampling is the program that happened when more geometries are assigned per one texel.
  /// Undersampling is vice versa.
  ///
  /// Using sampler with filtering, we can avoid oversampling & undersampling.
  /// And we can also determine how to read outside texel using `Addressing mode`.
  void CreateTextureSampler();

  /// @brief Create vertex buffers. 
  ///
  /// Buffers in Vulkan (not builtin) are storing arbitary (important!) data that can be
  /// interpreted by the graphics card and descriptor specification freedomly.
  /// They can be used store vertex data, but they can also be used for many other purpose.
  /// But, unlike builtin buffer, customized buffer do not automatically allocate memory themselves.
  void CreateVertexBuffer();
  /// @brief Create indice buffers.
  ///
  /// Actually, vertex and indices should be in one VkBuffer as written in Vulkan memory management link below.
  /// This is cache friendly, and use offsets in commands like `vkCmdBindVertexBuffers`.
  /// It is even possible to reuse the same chunk of memory for multiple resources if they are not
  /// used during the same render operations. (but be careful of operation will be held asynchronously.)
  /// 
  /// Replace other resource to VkBuffer that are used by another resource is Aliasing.
  /// And vulkan functions have explicit flags to specify that you want to do Aliasing.
  /// @link https://developer.nvidia.com/vulkan-memory-management
  void CreateIndiceBuffer();
  /// @brief Create proper uniform buffer object buffers.
  /// 
  /// We're going to copy new data to the uniform buffer EVERY FRAME, so it doesn't really make any
  /// sense to make a staging buffer. (It just add extra overhead instead of improving.)
  void CreateUniformBuffers();
  /// @brief Create Descriptor pool that can allocate descriptor sets.
  void CreateDescriptorPool();
  /// @brief Afterward creating descriptor pool, we can create actual descriptor sets.
  void CreateDescriptorSets();
  /// @brief Get preferred physical devices that on now valid memory type.
  /// iTypeFilter this will be used to specify the bit field of memory types that are suitable.
  ///
  /// But, not only preferred memory type and but also preferred memory type also supports
  /// mapping buffer from CPU to GPU and such as special features.
  /// For example, Mapping buffer data from CPU to GPU is specified as VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT.
  MCR_NODISCARD TU32 FindMemoryTypes(TU32 iTypeFilter, VkMemoryPropertyFlags iProperties);

  /// @brief Helper function just create plain buffer with size & usage.
  /// Created buffer has exclusive sharing mode. and preferred memory type.
  void CreateBuffer(
      VkDeviceSize iSize, VkBufferUsageFlags iUsage, 
      VkMemoryAllocateFlags iMemoryAllocationFlags,
      VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory);
  /// @brief Copy SRC_BIT source buffer to DST_BIT buffer with inSize from offset 0.
  /// Memory transfer operations are executed usig command buffers, like a drawing commands.
  /// So, we have to allocate command buffer first, 
  void CopyBuffer(VkBuffer inSourceBuffer, VkDeviceSize inSize, VkBuffer outDestBuffer);
  /// @brief Copy buffer to image. Before calling this function, 
  /// image must be transited to appropriate layout.
  void CopyBufferToImage(VkBuffer iBuffer, VkImage iImage, TU32 iWidth, TU32 iHeight);
  /// @brief
  MCR_NODISCARD VkCommandBuffer BeginSingleTimeCommands();
  /// @brief
  void EndSingleTimeCommands(VkCommandBuffer iValidCommandBuffer);

  /// @brief Recreate swap chain when window property was changed.
  /// Created swap chain is no longer compatible with it because 
  void RecreateSwapChain();
  /// @brief Clean up related swap chain instance.
  void CleanupSwapChain();

  /// @brief Clean up internal vulkan & glfw handles.
  void CleanUp();

public:
  GLFWwindow* mGlfwWindow = nullptr;

  VkInstance mInstance{};
  VkDebugUtilsMessengerEXT mMessengerExt{};
  VkSurfaceKHR mSurface{};
  VkPhysicalDevice mPhysicalDevice{};
  VkDevice mGraphicsDevice{};
  VkQueue mGraphicsQueue{};
  VkQueue mPresentQueue{};

  /// @brief Swap chain handle instance for rendering & presenting images.
  VkSwapchainKHR mSwapChain;
  /// @brief
  VkFormat       mSwapChainImageFormat;
  /// @brief
  VkExtent2D     mSwapChainExtent;
  /// @brief The handles of the VkImages of valid mSwapChain.
  std::vector<VkImage>     mSwapChainImages;
  /// @brief The view handle for viewing VkImage handle list of valid mSwapChain.
  std::vector<VkImageView> mSwapChainImageViews;
  /// @brief The attachment specified during render pass creatin are bound by wrapping them into
  /// a `VkFrameBuffer` object.
  /// VkFramebuffer references all of the `VkImageView` that represent attachments.
  /// Even though user use one attachment buffer, but should create same number of swap chain images.
  /// and reference them one-by-one.
  std::vector<VkFramebuffer> mSwapChainFrameBuffers;
  /// @brief Command pools manage the memory that is used to store the buffers and 
  /// command buffers are allocated from them.
  VkCommandPool     mCommandPool;
  /// @brief Command buffer list for each image in the swap chain.
  /// Command buffers will be automatically freed when their command pool is destroyed.
  std::vector<VkCommandBuffer> mCommandBuffers;

  /// @brief Render pass for describing rendering sequence of swap chain of logical device.
  VkRenderPass      mRenderPass;
  /// @brief opaque handle to a pipeline layout object.
  /// Access to descriptor sets from a pipeline is accomplished through a pipeline layout.
  /// ... Each pipeline is created using a pipeline layout.
  /// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkPipelineLayout.html
  VkPipelineLayout  mPipelineLayout;
  /// @brief Opaque handle to a descriptor set layout object.
  /// In this case, we set layout for UBO.
  /// Descriptor layout describese the type of descriptors that can be bound.
  /// https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkDescriptorSetLayout.html
  VkDescriptorSetLayout mDescriptorSetLayout;
  /// @brief
  VkDescriptorPool  mDescriptorPool;
  /// @brief
  std::vector<VkDescriptorSet> mDescriptorSets;
  /// @brief
  VkPipeline        mPipeline;
  
  /// @brief Each pair of semaphore of each index will be used one-and-one.
  /// CPU-GPU synchronization.
  std::vector<VkSemaphore>  mSemaphoreImageAvailable;
  /// @brief Used for switching presenting mode from rendering mode.
  /// CPU-GPU synchronization.
  std::vector<VkSemaphore>  mSemaphoreRenderFinished;
  /// @brief GPU-GPU synchronization.
  std::vector<VkFence>      mFencesInFlight;
  /// @brief Defines how many frames should be processed concurrently.
  static constexpr TI32     kMaxFramesInFlight = 2;
  /// @brief Defines frame index for managing vulkan semaphores.
  size_t                    mCurrentRenderFrame = 0;

  VkImage         mDepthImage;
  VkDeviceMemory  mDepthImageMemory;
  VkImageView     mDepthImageView;

  VkImage         mTextureImage;
  VkDeviceMemory  mTextureImageMemory;
  /// @brief
  VkImageView     mTextureImageView;
  /// @brief
  VkSampler       mTextureSampler;

  /// @brief
  bool mIsWindowResizeDirty = false;
};
