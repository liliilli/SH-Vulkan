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

#include <optional>
#include "FGlobalType.h"

/// @stuct DQueueFamilyIndices
/// @brief Stores vulkan physical device's valid graphics queue family index.
///
/// @warning Graphics rendering queue and Presentation queue are different each other.
/// So, PresentQueueFamily also need to be specified to output surfaceKHR to screen.
struct DQueueFamilyIndices final
{
  std::optional<TU32> moptGraphicsQueueFamiliy;
  std::optional<TU32> moptPresentQueueFamily;

  /// @brief Check graphics queue family variable is exist.
  [[nodiscard]] bool IsComplete() const noexcept
  {
    return this->moptGraphicsQueueFamiliy.has_value()
        && this->moptPresentQueueFamily.has_value();
  }
};

