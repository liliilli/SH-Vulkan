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

#include <vector>
#include "Type/DVector3.h"
#include "ASystemInclude.h"

namespace sh
{

struct DDefaultVertex final
{
  DVector3 mPosition;
  DVector3 mBaseColor;
  
  /// @brief Get overall vertex structure binding descriptor for Vulkan.
  [[nodiscard]] static VkVertexInputBindingDescription& GetBindingDescription();

  /// @brief Get per attribute structure binding descriptor for Vulkan.
  [[nodiscard]] static std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptons();;
};

} /// ::sh namespace
