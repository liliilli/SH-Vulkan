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

#include "Temp/DDefaultVertex.h"

namespace sh
{

VkVertexInputBindingDescription& DDefaultVertex::GetBindingDescription()
{
  // Describes to load data from memory through the vertices. 
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkVertexInputBindingDescription.html
  static VkVertexInputBindingDescription bindingDescription = {};
  static bool isFilled = false;
  if (isFilled == false)
  {
    bindingDescription.binding    = 0;
    bindingDescription.stride     = sizeof(DDefaultVertex);
    // VERTEX : Move to the next data entry after each vertex.
    // INSTANCE : Move to the next data entry after each instance.
    bindingDescription.inputRate  = VK_VERTEX_INPUT_RATE_VERTEX;
    isFilled = true;
  }
  return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription>& DDefaultVertex::GetAttributeDescriptons()
{
  // Describes per attribute information.
  // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkVertexInputAttributeDescription.html
  static std::vector<VkVertexInputAttributeDescription> attributeDescription = {};
  static bool isFilled = false;
  if (isFilled == false)
  {
    attributeDescription.resize(2);
    // the binding number which this attribute takes its data from.
    attributeDescription[0].binding   = 0;
    attributeDescription[0].location  = 0;
    attributeDescription[0].format    = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[0].offset    = offsetof(DDefaultVertex, mPosition);

    attributeDescription[1].binding   = 0;
    attributeDescription[1].location  = 1;
    attributeDescription[1].format    = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescription[1].offset    = offsetof(DDefaultVertex, mBaseColor);
    isFilled = true;
  }

  return attributeDescription;
}

} /// ::sh namespace