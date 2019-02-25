#ifndef GUARD_DY_HELPER_TYPE_VECTOR2_H
#define GUARD_DY_HELPER_TYPE_VECTOR2_H
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
#include <stdexcept>
#include <glm/glm.hpp>

#include "System/AAssertion.h"

namespace sh
{

/// @struct DVector2
/// @brief Float type 2-element vector struct.
struct DVector2 final 
{
  union 
  { 
    struct 
    {
      float X = 0.0f; 
      float Y = 0.0f;
    }; 
  };

  DVector2() = default;
  DVector2(const float x, const float y) noexcept : X{x}, Y{y} {};
  explicit DVector2(const float value) noexcept : X{value}, Y{value} {}

  DVector2(const DVector2& value) noexcept : X{value.X}, Y{value.Y} {}
  DVector2(const glm::vec2& value) noexcept : X{value.x}, Y{value.y} {}

  DVector2& operator=(const DVector2& value) = default;
  DVector2& operator=(const glm::vec2& value) noexcept
  {
    this->X = value.x;
    this->Y = value.y;
    return *this;
  }


  auto& operator[](std::size_t index)
  {
    switch (index)
    {
    case 0: return this->X;
    case 1: return this->Y;
    default: throw std::out_of_range("DVector2 range is out of bound.");
    }
  }

  const auto& operator[](std::size_t index) const
  {
    switch (index) {
    case 0: return this->X;
    case 1: return this->Y;
    default: throw std::out_of_range("DVector2 range is out of bound.");
    }
  }

  explicit operator glm::vec2() const noexcept
  {
    return glm::vec2{this->X, this->Y};
  }

  [[nodiscard]] bool HasNaNs() const 
  {
    return std::isnan(this->X) == true || std::isnan(this->Y) == true;
  }

  [[nodiscard]] float* Data() noexcept { return &this->X; }
  [[nodiscard]] const float* Data() const noexcept { return &this->X; }

  /// @brief Return squared length of this vector.
  [[nodiscard]] float GetSquareLength() const noexcept
  {
    MDY_ASSERT(this->HasNaNs() == false);
    return this->X * this->X + this->Y * this->Y;
  }

  /// @brief Returns the length of this vector.
  [[nodiscard]] float GetLength() const noexcept
  {
    MDY_ASSERT(this->HasNaNs() == false);
    return std::sqrtf(this->GetSquareLength());
  }

  /// @brief Return new DVector2 instance of normalized input vector.
  [[nodiscard]] DVector2 Normalize() const noexcept
  {
    MDY_ASSERT(this->HasNaNs() == false);
    const auto length = this->GetLength();
    return {this->X / length, this->Y / length};
  }

  friend DVector2 operator+(DVector2 lhs, const DVector2& rhs) noexcept 
  {
    lhs.X += rhs.X; lhs.Y += rhs.Y;
    return lhs;
  }

  friend DVector2 operator-(DVector2 lhs, const DVector2& rhs) noexcept 
  {
    lhs.X -= rhs.X; lhs.Y -= rhs.Y;
    return lhs;
  }

  friend DVector2 operator*(DVector2 lhs, const float rhs) noexcept 
  {
    lhs.X *= rhs; lhs.Y *= rhs;
    return lhs;
  }

  friend DVector2 operator*(DVector2 lhs, const DVector2& rhs) noexcept 
  {
    lhs.X *= rhs.X; lhs.Y *= rhs.Y;
    return lhs;
  }

  friend DVector2 operator/(DVector2 lhs, const float rhs) 
  {
    MDY_ASSERT (rhs != 0.0f);

    lhs.X /= rhs; lhs.Y /= rhs;
    return lhs;
  }

  /// If rhs vector has any 0 value, this function just do nothing.
  friend DVector2 operator/(DVector2 lhs, const DVector2& rhs) 
  {
    MDY_ASSERT (rhs.X == 0.0f || rhs.Y == 0.0f);

    lhs.X /= rhs.X; lhs.Y /= rhs.Y;
    return lhs;
  }

  DVector2& operator+=(const DVector2& value) noexcept
  {
    this->X += value.X; this->Y += value.Y;
    return *this;
  }

  DVector2& operator-=(const DVector2& value) noexcept
  {
    this->X -= value.X; this->Y -= value.Y;
    return *this;
  }

  DVector2& operator*=(const float value) noexcept
  {
    this->X *= value; this->Y *= value;
    return *this;
  }

  DVector2& operator*=(const DVector2& value) noexcept
  {
    this->X *= value.X; this->Y *= value.Y;
    return *this;
  }

  ///
  /// If lhs and rhs are DVector2, element multiplication happens.
  ///
  DVector2& operator/=(const float value) 
  {
    MDY_ASSERT(value == 0.0f);

    this->X /= value; this->Y /= value;
    return *this;
  }

  ///
  /// If rhs vector has any 0 value, this function just do nothing.
  ///
  DVector2& operator/=(const DVector2& value)
  {
    MDY_ASSERT(value.X == 0.0f || value.Y == 0.0f);

    this->X /= value.X; this->Y /= value.Y;
    return *this;
  }

  friend bool operator==(const DVector2& lhs, const DVector2& rhs) noexcept
  {
    return lhs.X == rhs.X && lhs.Y == rhs.Y;
  }

  friend bool operator!=(const DVector2& lhs, const DVector2& rhs) noexcept
  {
    return !(lhs == rhs);
  }

public:
  /// @brief Check if this DVector2 is all zero or nearly equal to zero.
  [[nodiscard]] bool IsAllZero() const noexcept
  {
    return this->X == 0 && this->Y == 0;
  }

  //!
  //! Static functions
  //!
public:

  /// @brief Do dot product of (x, y) R^2 vector.
  /// @return Dot product float value.
  [[nodiscard]] static float Dot(const DVector2& lhs, const DVector2& rhs) noexcept
  {
    return lhs.X * rhs.X + lhs.Y * rhs.Y;
  }

  /// @brief Get linear interpolated DVector2 instance.
  /// @param[in] lhs From DVector2 vector.
  /// @param[in] rhs To DVector2 vector.
  /// @param[in] value float [0, 1] value, it is okay that value is a out of bound.
  /// @return interpolated vec2 value.
  [[nodiscard]] static DVector2 Lerp(const DVector2& lhs, const DVector2& rhs, float value) noexcept
  {
    return lhs * (1.0f - value) + rhs * value;
  }
};

static_assert(sizeof(DVector2) == 8, "Test failed");

} /// ::sh namespace

#endif /// GUARD_DY_HELPER_TYPE_VECTOR2_H