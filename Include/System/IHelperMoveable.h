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

#include <memory>

enum class ERaiiStyle
{
  Raw,
  UniquePtr,
};

template <ERaiiStyle ERaii, typename TType>
class IHelperMoveable;

template <typename TType>
class IHelperMoveable<ERaiiStyle::Raw, TType> 
{
public:
  virtual ~IHelperMoveable() = 0;
  IHelperMoveable(IHelperMoveable&) = delete;
  IHelperMoveable& operator=(IHelperMoveable&) = delete;

  IHelperMoveable(IHelperMoveable&& iFrom) noexcept : 
      mOwnerResource{iFrom.mOwnerResource}
  {
    iFrom.mOwnerResource = nullptr;
    iFrom.mIsResourceMoved = true;
  }

  IHelperMoveable& operator=(IHelperMoveable&& iFrom) noexcept 
  {
    if (this == &iFrom) { return *this; }

    this->mOwnerResource = std::move(iFrom.mOwnerResource);
    iFrom.mOwnerResource = nullptr;
    iFrom.mIsResourceMoved = true;
    return *this;
  }

protected:
  TType* mOwnerResource = nullptr;

private:
  bool mIsResourceMoved = false;
};

template <typename TType>
IHelperMoveable<ERaiiStyle::Raw, TType>::~IHelperMoveable()
{
  if (this->mIsResourceMoved == false && this->mOwnerResource != nullptr) 
  { 
    delete this->mOwnerResource; 
  }
}

template <typename TType>
class IHelperMoveable<ERaiiStyle::UniquePtr, TType> 
{
public:
  virtual ~IHelperMoveable() = 0;
  IHelperMoveable(IHelperMoveable&) = delete;
  IHelperMoveable& operator=(IHelperMoveable&) = delete;

  IHelperMoveable(IHelperMoveable&& iFrom) noexcept : 
      mOwnerResource{std::move(iFrom.mOwnerResource)}
  {
    iFrom.mOwnerResource = nullptr;
    iFrom.mIsResourceMoved = true;
  }

  IHelperMoveable& operator=(IHelperMoveable&& iFrom) noexcept
  {
    if (this == &iFrom) { return *this; }

    this->mOwnerResource = std::move(iFrom.mOwnerResource);
    iFrom.mOwnerResource = nullptr;
    iFrom.mIsResourceMoved = true;
    return *this;
  }

protected:
  std::unique_ptr<TType> mOwnerResource = nullptr;

private:
  bool mIsResourceMoved = false;
};

template <typename TType>
IHelperMoveable<ERaiiStyle::UniquePtr, TType>::~IHelperMoveable()
{
  if (this->mIsResourceMoved == false) { this->mOwnerResource = nullptr; }
}
