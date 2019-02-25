#ifndef GUARD_DY_HELPER_TYPE_VECTOR3_H
#define GUARD_DY_HELPER_TYPE_VECTOR3_H
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

namespace sh 
{

/// @struct DVector3
/// @brief Float type 3-element vector struct.
struct DVector3 final 
{
  union 
  { 
    struct 
    {
      float X; 
      float Y;
      float Z;
    }; 
  };

  DVector3() = default;
  DVector3(const float x, const float y, const float z) noexcept : X(x), Y(y), Z(z) {};
  explicit DVector3(const float value) noexcept : X{value}, Y{value}, Z{value} {};

  DVector3(const DVector3& value) noexcept = default;
  DVector3(const glm::vec3& value) noexcept : X{value.x}, Y{value.y}, Z{value.z} {};

  DVector3& operator=(const DVector3& value) noexcept = default;
  DVector3& operator=(const glm::vec3& value) noexcept
  {
    this->X = value.x;
    this->Y = value.y;
    this->Z = value.z;
    return *this;
  }

  auto& operator[](std::size_t index)
  {
    switch (index)
    {
    case 0: return this->X;
    case 1: return this->Y;
    case 2: return this->Z;
    default: throw std::out_of_range("DVector2 range is out of bound.");
    }
  }

  const auto& operator[](std::size_t index) const
  {
    switch (index)
    {
    case 0: return this->X;
    case 1: return this->Y;
    case 2: return this->Z;
    default: throw std::out_of_range("DVector2 range is out of bound.");
    }
  }

  explicit operator glm::vec3() const noexcept
  {
    return glm::vec3{this->X, this->Y, this->Z};
  }

  [[nodiscard]] bool HasNaNs() const 
  {
    return std::isnan(this->X) == true 
        || std::isnan(this->Y) == true
        || std::isnan(this->Z) == true;
  }

  [[nodiscard]] float* Data() noexcept { return &this->X; }
  [[nodiscard]] const float* Data() const noexcept { return &this->X; }

  /// @brief Return squared length of this vector.
  [[nodiscard]] float GetSquareLength() const noexcept
  {
    MDY_ASSERT(this->HasNaNs() == false);
    return this->X * this->X + this->Y * this->Y + this->Z * this->Z;
  }

  /// @brief Returns the length of this vector.
  [[nodiscard]] float GetLength() const noexcept
  {
    MDY_ASSERT(this->HasNaNs() == false);
    return std::sqrtf(GetSquareLength());
  }

  /// @brief Return new DVector3 instance of normalized input vector.
  [[nodiscard]] DVector3 Normalize() const noexcept
  {
    MDY_ASSERT(this->HasNaNs() == false);
    const auto length = this->GetLength();
    return {this->X / length, this->Y / length, this->Z / length};
  }

  friend DVector3 operator+(DVector3 lhs, const DVector3& rhs) noexcept
  {
    lhs.X += rhs.X;
    lhs.Y += rhs.Y;
    lhs.Z += rhs.Z;
    return lhs;
  }

  friend DVector3 operator-(DVector3 lhs, const DVector3& rhs) noexcept
  {
    lhs.X -= rhs.X;
    lhs.Y -= rhs.Y;
    lhs.Z -= rhs.Z;
    return lhs;
  }

  friend DVector3 operator*(DVector3 lhs, const float rhs) noexcept
  {
    lhs.X *= rhs;
    lhs.Y *= rhs;
    lhs.Z *= rhs;
    return lhs;
  }

  friend DVector3 operator*(DVector3 lhs, const DVector3& rhs) noexcept
  {
    lhs.X *= rhs.X;
    lhs.Y *= rhs.Y;
    lhs.Z *= rhs.Z;
    return lhs;
  }

  friend DVector3 operator/(DVector3 lhs, const float rhs) noexcept
  {
    MDY_ASSERT(rhs == 0.0f);
    lhs.X /= rhs; lhs.Y /= rhs; lhs.Z /= rhs;
    return lhs;
  }

  friend DVector3 operator/(DVector3 lhs, const DVector3& rhs) noexcept
  {
    MDY_ASSERT(rhs.X == 0.0f || rhs.Y == 0.0f || rhs.Z == 0.0f);
    lhs.X /= rhs.X; lhs.Y /= rhs.Y; lhs.Z /= rhs.Z;
    return lhs;
  }

  DVector3& operator+=(const DVector3& value) noexcept
  {
    this->X += value.X; this->Y += value.Y; this->Z += value.Z;
    return *this;
  }

  DVector3& operator-=(const DVector3& value) noexcept
  {
    this->X -= value.X; this->Y -= value.Y; this->Z -= value.Z;
    return *this;
  }

  DVector3& operator*=(const float value) noexcept
  {
    this->X *= value; this->Y *= value; this->Z *= value;
    return *this;
  }

  DVector3& operator*=(const DVector3& value) noexcept
  {
    this->X *= value.X; this->Y *= value.Y; this->Z *= value.Z;
    return *this;
  }

  DVector3& operator/=(const float value) noexcept
  {
    MDY_ASSERT(value == 0.0f);
    this->X /= value; this->Y /= value; this->Z /= value;
    return *this;
  }

  DVector3& operator/=(const DVector3& value) noexcept
  {
    MDY_ASSERT(value.X == 0.0f || value.Y == 0.0f || value.Z == 0.0f);
    this->X /= value.X; this->Y /= value.Y; this->Z /= value.Z;
    return *this;
  }

  friend bool operator==(const DVector3& lhs, const DVector3& rhs) noexcept
  {
    return lhs.X == rhs.X && lhs.Y == rhs.Y && lhs.Z == rhs.Z;
  }
  friend bool operator!=(const DVector3& lhs, const DVector3& rhs) noexcept
  {
    return !(lhs == rhs);
  }

  /// @brief Check if this DVector3 is all zero or nearly equal to zero.
  [[nodiscard]] bool IsAllZero() const noexcept
  {
    return this->X == 0 && this->Y == 0 && this->Z == 0;
  }

  /// @brief Do dot product of (x, y, z) R^3 vector.
  [[nodiscard]] static float Dot(const DVector3& lhs, const DVector3& rhs) noexcept
  {
    return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
  }

  /// @brief Cross product of (x, y, z) R^3 vector.
  [[nodiscard]] static DVector3 Cross(const DVector3& lhs, const DVector3& rhs) noexcept
  {
    return
    {
      lhs.Y * rhs.Z - rhs.Y * lhs.Z,
      lhs.Z * rhs.X - rhs.Z * lhs.X,
      lhs.X * rhs.Y - rhs.X * lhs.Y
    };
  }

  [[nodiscard]] static DVector3 Lerp(const DVector3& lhs, const DVector3& rhs, float value) noexcept
  {
    return lhs * (1.0f - value) + rhs * value;
  }

  /// @brief Return {0, 0, 1} front DVector3 vector.
  static DVector3 FrontZ() noexcept
  {
    static DVector3 ret{0, 0, 1};
    return ret;
  }

  /// @brief Return {1, 0, 0} right DVector3 vector.
  static DVector3 RightX() noexcept
  {
    static DVector3 ret{1, 0, 0};
    return ret;
  }

  /// @brief Return {0, 1, 0} up DVector3 vector.
  static DVector3 UpY() noexcept
  {
    static DVector3 ret{0, 1, 0};
    return ret;
  }
};

static_assert(sizeof(DVector3) == 12, "Test failed");

} /// ::sh namespace

#endif /// GUARD_DY_HELPER_TYPE_VECTOR3_H
