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

#include "ASystemInclude.h"

#if defined(NDEBUG)
constexpr bool kEnabledValidationLayers = false;
#else
constexpr bool kEnabledValidationLayers = true;
#endif

/// @brief Helper function of checking VkBool32 is true.
/// VkBool32 native type is uint32_t, so could not check as comparing with boolean value, true or false.
/// so we have to use this function to check true or false.
[[nodiscard]] constexpr bool VkIsTrue(VkBool32 iExpressionResult) noexcept
{
  return iExpressionResult > 0;
}

/// @brief Helper function of checking VkBool32 is false.
/// VkBool32 native type is uint32_t, so could not check as comparing with boolean value, true or false.
/// so we have to use this function to check true or false.
[[nodiscard]] constexpr bool VkIsFalse(VkBool32 iExpressionResult) noexcept
{
  return VkIsTrue(iExpressionResult) != true;
}

