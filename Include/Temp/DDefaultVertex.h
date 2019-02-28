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
#include "Type/DVector2.h"
#include "ASystemInclude.h"

namespace dy
{

struct DDefaultVertex final
{
  DVector3 mPosition;
  DVector3 mBaseColor;
  DVector2 mTextureUv0;
  
  /// @brief Get overall vertex structure binding descriptor for Vulkan.
  [[nodiscard]] static VkVertexInputBindingDescription& GetBindingDescription();

  /// @brief Get per attribute structure binding descriptor for Vulkan.
  [[nodiscard]] static std::vector<VkVertexInputAttributeDescription>& GetAttributeDescriptons();;

  bool operator==(const DDefaultVertex& other) const noexcept
  {
    return this->mPosition == other.mPosition
        && this->mBaseColor == other.mBaseColor
        && this->mTextureUv0 == other.mTextureUv0;
  }
};

} /// ::dy namespace

namespace std
{
  template <> struct hash<dy::DDefaultVertex>
  {
    size_t operator()(const dy::DDefaultVertex& vertex) const 
    {
      return ((hash<dy::DVector3>()(vertex.mPosition) 
            ^ (hash<dy::DVector3>()(vertex.mBaseColor) << 1)) >> 1)
        ^ (hash<dy::DVector2>()(vertex.mTextureUv0) << 1);
    }
  };
}
