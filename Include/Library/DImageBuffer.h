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

#include <string>
#include "FGlobalType.h"
#include "FMacro.h"
#include "Library/EImageEnums.h"

namespace dy
{

/// @class DDyImageBinaryDataBuffer
/// @brief Image binary buffer that manages binary buffer chunk, 
/// automatically released when it's be out of scope.
class DDyImageBinaryDataBuffer final
{
public:
  DDyImageBinaryDataBuffer(const std::string& imagePath);
  ~DDyImageBinaryDataBuffer();

  DDyImageBinaryDataBuffer(const DDyImageBinaryDataBuffer&)                 = delete;
  DDyImageBinaryDataBuffer& operator=(DDyImageBinaryDataBuffer&)            = delete;
  DDyImageBinaryDataBuffer(DDyImageBinaryDataBuffer&&) noexcept             = default;
  DDyImageBinaryDataBuffer& operator=(DDyImageBinaryDataBuffer&&) noexcept  = default;

  /// @brief Check if buffer chunk is created properly when construction time.
  MCR_NODISCARD bool IsBufferCreatedProperly() const noexcept
  {
    return this->mIsBufferCreatedProperly;
  }

  /// @brief Get image width.
  MCR_NODISCARD TI32 GetImageWidth() const noexcept
  {
    return this->mWidth;
  }

  /// @brief Get image height.
  MCR_NODISCARD TI32 GetImageHeight() const noexcept
  {
    return this->mHeight;
  }

  /// @brief Get image format value.
  MCR_NODISCARD EImageColorFormatStyle GetImageFormat() const noexcept
  {
    return this->mImageFormat;
  }

  /// @brief Get the start point of binary buffer chunk.
  MCR_NODISCARD const unsigned char* GetBufferStartPoint() const noexcept
  {
    return this->mBufferStartPoint;
  }

  /// @brief Get buffer size of image chunk.
  MCR_NODISCARD TU64 GetBufferSize() const noexcept
  {
    //return this->mWidth * this->mHeight * this->mImageChannel;
    return this->mWidth * this->mHeight * 4;
  }

private:
  TI32 mImageChannel   = 0;
  TI32 mWidth          = 0;
  TI32 mHeight         = 0;
  EImageColorFormatStyle mImageFormat = EImageColorFormatStyle::NoneError;
  unsigned char* mBufferStartPoint    = nullptr;
  bool mIsBufferCreatedProperly       = false;
};

} /// ::dy namespace