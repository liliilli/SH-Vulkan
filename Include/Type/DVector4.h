#ifndef GUARD_DY_HELPER_TYPE_VECTOR4_H
#define GUARD_DY_HELPER_TYPE_VECTOR4_H
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

#include <array>
#include <glm/glm.hpp>
#include "System/AAssertion.h"

namespace dy
{

/// @struct DVector4
/// @brief Float type 4-element vector struct.
struct DVector4 final
{
  union 
  { 
    __m128 __Simd{}; 
    struct { float X; float Y; float Z; float W; };
  };

  DVector4() = default;
  DVector4(const DVector4&) = default;
  DVector4& operator=(const DVector4&) = default;

  explicit DVector4(__m128 __iSimd) : __Simd{__iSimd} {};
  explicit DVector4(const float value) noexcept : X{value}, Y{value}, Z{value}, W{value} {};
  DVector4(const float x, const float y, const float z, const float w) noexcept : X(x), Y(y), Z(z), W{w} {};
  DVector4(const glm::vec4& value) noexcept : X{value.x}, Y{value.y}, Z{value.z}, W{value.w} {}

  DVector4& operator=(const __m128& __iSimd) noexcept
  {
    this->__Simd = __iSimd;
    return *this;
  }

  DVector4& operator=(const glm::vec4& value) noexcept
  {
    this->X = value.x; this->Y = value.y; this->Z = value.z; this->W = value.w;
    return *this;
  }

  auto& operator[](std::size_t index)
  {
    switch (index)
    {
    case 0: return this->X;
    case 1: return this->Y;
    case 2: return this->Z;
    case 3: return this->W;
    default: throw std::out_of_range("DDyVector2 range is out of bound.");
    }
  }

  const auto& operator[](std::size_t index) const
  {
    switch (index) {
    case 0: return this->X;
    case 1: return this->Y;
    case 2: return this->Z;
    case 3: return this->W;
    default: throw std::out_of_range("DDyVector2 range is out of bound.");
    }
  }

  explicit operator glm::vec4() const noexcept
  {
    return glm::vec4{this->X, this->Y, this->Z, this->W};
  }

  [[nodiscard]] float* Data() noexcept { return &this->X; }
  [[nodiscard]] const float* Data() const noexcept { return &this->X; }

  friend DVector4 operator+(DVector4 lhs, const DVector4& rhs) noexcept
  {
    lhs.__Simd = _mm_add_ps(lhs.__Simd, rhs.__Simd);
    return lhs;
  }

  friend DVector4 operator-(DVector4 lhs, const DVector4& rhs) noexcept
  {
    lhs.__Simd = _mm_sub_ps(lhs.__Simd, rhs.__Simd);
    return lhs;
  }

  friend DVector4 operator*(DVector4 lhs, const float rhs) noexcept
  {
    lhs.__Simd = _mm_mul_ps(lhs.__Simd, _mm_set_ps(rhs, rhs, rhs, rhs));
    return lhs;
  }

  /// If lhs and rhs are DVector4, element multiplication happens.
  friend DVector4 operator*(DVector4 lhs, const DVector4& rhs) noexcept
  {
    lhs.__Simd = _mm_mul_ps(lhs.__Simd, rhs.__Simd);
    return lhs;
  }

  /// If rhs has 0 value, this function just do nothing.
  friend DVector4 operator/(DVector4 lhs, const float rhs) noexcept
  {
    MDY_ASSERT(rhs != 0.0f);

    lhs.__Simd = _mm_div_ps(lhs.__Simd, _mm_set_ps(rhs, rhs, rhs, rhs));
    return lhs;
  }

  /// If rhs vector has any 0 value, this function just do nothing.
  friend DVector4 operator/(DVector4 lhs, const DVector4& rhs) noexcept
  {
    MDY_ASSERT(rhs.X != 0.0f && rhs.Y != 0.0f && rhs.Z != 0.0f && rhs.W != 0.0f);

    lhs.__Simd = _mm_div_ps(lhs.__Simd, rhs.__Simd);
    return lhs;
  }

  DVector4& operator+=(const DVector4& value) noexcept
  {
    this->__Simd = _mm_add_ps(this->__Simd, value.__Simd);
    return *this;
  }

  DVector4& operator-=(const DVector4& value) noexcept
  {
    this->__Simd = _mm_sub_ps(this->__Simd, value.__Simd);
    return *this;
  }

  DVector4& operator*=(const float value) noexcept
  {
    this->__Simd = _mm_mul_ps(this->__Simd, _mm_set_ps(value, value, value, value));
    return *this;
  }

  DVector4& operator*=(const DVector4& value) noexcept
  {
    this->__Simd = _mm_sub_ps(this->__Simd, value.__Simd);
    return *this;
  }

  /// If lhs and rhs are DVector4, element multiplication happens.
  DVector4& operator/=(const float value) noexcept
  {
    MDY_ASSERT(value != 0.0f);
    this->__Simd = _mm_div_ps(this->__Simd, _mm_set_ps(value, value, value, value));
    return *this;
  }

  /// If rhs vector has any 0 value, this function just do nothing.
  DVector4& operator/=(const DVector4& value) noexcept
  {
    MDY_ASSERT(value.X != 0.0f && value.Y != 0.0f && value.Z != 0.0f && value.W != 0.0f);
    this->__Simd = _mm_div_ps(this->__Simd, value.__Simd);
    return *this;
  }

  // https://stackoverflow.com/questions/6042399/how-to-compare-m128-types
  friend bool operator==(const DVector4& lhs, const DVector4& rhs) noexcept
  {
    return _mm_movemask_ps(_mm_cmpeq_ps(lhs.__Simd, rhs.__Simd)) == 0xF;
  }
  
  friend bool operator!=(const DVector4& lhs, const DVector4& rhs) noexcept
  {
    return !(lhs == rhs);
  }
};

static_assert(sizeof(DVector4) == 16, "Test failed");

} /// ::dy namespace

#endif /// GUARD_DY_HELPER_TYPE_VECTOR4_H
