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

#include "Library/DImageBuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "System/AAssertion.h"

namespace
{

/// @brief Return color format
/// @param[in] channelsValue Color channels value for being used to get GL_COLOR channels.
dy::EImageColorFormatStyle GetColorFormat(const int32_t channelsValue) noexcept 
{
  switch (channelsValue) {
  /// Red (one channel)
  case 1:  return dy::EImageColorFormatStyle::R;
  /// Red and Green only.
  case 2:  return dy::EImageColorFormatStyle::RG;
  /// RGB without alpha
  case 3:  return dy::EImageColorFormatStyle::RGB;
  /// RGB with alpha
  case 4:  return dy::EImageColorFormatStyle::RGBA;
  /// else, return Error type
  default: return dy::EImageColorFormatStyle::NoneError;
  }
}

}

namespace dy
{

DDyImageBinaryDataBuffer::DDyImageBinaryDataBuffer(const std::string& imagePath)
{
  stbi_set_flip_vertically_on_load(true);
  this->mBufferStartPoint = stbi_load(
      imagePath.c_str(), 
      &this->mWidth, &this->mHeight, 
      &this->mImageChannel, STBI_rgb_alpha);
  this->mImageFormat      = GetColorFormat(this->mImageChannel);

  if (this->mImageFormat == EImageColorFormatStyle::NoneError)
  {
    stbi_image_free(this->mBufferStartPoint);
    MDY_ASSERT(false);
    this->mIsBufferCreatedProperly = false;
  }
  else if (this->mBufferStartPoint == nullptr)  { this->mIsBufferCreatedProperly = false; }
  else                                          { this->mIsBufferCreatedProperly = true; }
}

DDyImageBinaryDataBuffer::~DDyImageBinaryDataBuffer()
{
  if (this->mIsBufferCreatedProperly == true)
  {
    stbi_image_free(this->mBufferStartPoint);
  }
}

} /// ::dy namespace