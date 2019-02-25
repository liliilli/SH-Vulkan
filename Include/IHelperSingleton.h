#ifndef GUARD_DY_MANAGEMENT_INTERFACE_ISINGLETONCRTP_H
#define GUARD_DY_MANAGEMENT_INTERFACE_ISINGLETONCRTP_H
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

#include "FMacro.h"
#include "ESuccess.h"

template <typename TType>
class IHelperSingleton
{
public:
  /// @brief Return reference of instance of MDyTime manager.
  /// This function is thread safe.
  static TType& GetInstance() noexcept
  {
    static TType instance;
    return instance;
  }

  /// @brief Initialize singleton.
  EDySuccess static Initialize() noexcept
  {
    const auto flag = GetInstance().pfInitialize();
    return flag;
  }

  /// @brief Shutdown singleton.
  EDySuccess static Release() noexcept
  {
    const auto flag = GetInstance().pfRelease();
    return flag;
  }

protected:
  IHelperSingleton()          = default;
  virtual ~IHelperSingleton() = 0;

  MCY_SINGLETON_PROPERTIES(IHelperSingleton);
};

template <typename TType>
IHelperSingleton<TType>::~IHelperSingleton() = default;

#endif /// GUARD_DY_MANAGEMENT_INTERFACE_ISINGLETONCRTP_H