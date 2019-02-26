#ifndef GUARD_DY_HELPER_SYSTEM_IDIOMS_H
#define GUARD_DY_HELPER_SYSTEM_IDIOMS_H
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
#include <vector>
#include <utility>
#include "FGlobalType.h"
#include "FMacro.h"

namespace dy
{

/// @brief Vector fast erase.
template <typename TType, typename TAllocator>
void DyFastErase(std::vector<TType, TAllocator>& iVector, TU32 iIndex)
{
  MDY_ASSERT(iIndex < iVector.size(), "Unexpected error occurred.");

  std::swap(iVector[iIndex], iVector.back());
  iVector.pop_back();
}

/// @brief Vector fast erase.
template <typename TType, typename TAllocator>
void DyFastErase(
    std::vector<TType, TAllocator>& iVector, 
    const typename std::vector<TType, TAllocator>::iterator& iIndex)
{
  if (iIndex == iVector.end()) { return; }
  DyFastErase(iVector, static_cast<TU32>(std::distance(iVector.begin(), iIndex)));
}

/// @brief Vector fast erase.
template <typename TType, typename TAllocator>
void DyEraseRemove(
    std::vector<TType, TAllocator>& p, 
    const typename std::vector<TType, TAllocator>::value_type& iValue)
{
  p.erase( std::remove( p.begin(), p.end(), iValue ), p.end() );
}

/// @brief Vector fast erase.
template <typename TType, typename TAllocator, typename TFunctor>
void DyEraseRemoveIf(std::vector<TType, TAllocator>& p, TFunctor&& iPredcate)
{
  p.erase( std::remove_if( p.begin(), p.end(), iPredcate ), p.end() );
}

/// @brief Vector contains.
template <typename TType, typename TAllocator, typename TFunctor>
MCR_NODISCARD bool ContainsIf(const std::vector<TType, TAllocator>& p, TFunctor&& iPredcate)
{
  return std::any_of(p.begin(), p.end(), iPredcate);
}

/// @brief Vector contains.
template <typename TType, typename TAllocator>
MCR_NODISCARD bool Contains(const std::vector<TType, TAllocator>& p, const TType& target)
{
  for (const auto& item : p) { if (item == target) { return true; } }
  return false;
}

/// @brief Array contains.
template <typename TType, std::size_t TAmount, typename TFunctor>
MCR_NODISCARD bool ContainsIf(const std::array<TType, TAmount>& p, TFunctor&& iPredicate)
{
  return std::any_of(MDY_BIND_CBEGIN_CEND(p), iPredicate);
}

/// @brief Array contains.
template <typename TType, std::size_t TAmount>
MCR_NODISCARD bool Contains(const std::array<TType, TAmount>& p, const TType& target)
{
  for (const auto& item : p) { if (item == target) { return true; } }
  return false;
}

/// @brief 
/// @param 
/// @tparam TCtorArgs
template <typename TType, typename TAllocator, typename... TCtorArgs>
void DySafeUniquePtrEmplaceBack(
    std::vector<std::unique_ptr<TType>, TAllocator>& list,
    TCtorArgs&&... args)
{
  list.emplace_back(nullptr);
  MDY_ASSERT(list.back() == nullptr, "Unexpected error occurred.");

  auto ptrsmtInstance = std::make_unique<TType>(std::forward<TCtorArgs>(args)...);
  list.back() = std::move(ptrsmtInstance);
  MDY_ASSERT(list.back() != nullptr, "Unexpected error occurred.");
}

}

#endif /// GUARD_DY_HELPER_SYSTEM_IDIOMS_H