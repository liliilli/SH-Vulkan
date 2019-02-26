#ifndef GUARD_DY_HELPER_TYPE_MATRIX4_H
#define GUARD_DY_HELPER_TYPE_MATRIX4_H
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
#include "DVector4.h"
#include "DVector3.h"

#include "System/AUndef.h"

namespace dy
{

/// @class DMatrix4
/// @brief
class DMatrix4 final
{
public:
  DMatrix4()  = default;
  ~DMatrix4() = default;

  DMatrix4(const DMatrix4& value) = default;
  DMatrix4(DMatrix4&& value) = default;
  DMatrix4& operator=(const DMatrix4& value) = default;
  DMatrix4& operator=(DMatrix4&& value) = default;
  DMatrix4(std::initializer_list<float>&) = delete;

  DMatrix4(const float _00, const float _01, const float _02, const float _03,
           const float _10, const float _11, const float _12, const float _13,
           const float _20, const float _21, const float _22, const float _23,
           const float _30, const float _31, const float _32, const float _33) :
      mMatrixValue{DVector4{_00, _10, _20, _30},
                   DVector4{_01, _11, _21, _31},
                   DVector4{_02, _12, _22, _32},
                   DVector4{_03, _13, _23, _33}} {}

  DMatrix4(
    const DVector4& column1, const DVector4& column2,
    const DVector4& column3, const DVector4& column4);

  DMatrix4(const glm::mat4& glmMatrix) noexcept;

  DMatrix4& operator=(const glm::mat4& value) noexcept;

  explicit operator glm::mat4() const noexcept;

  DVector4& operator[](std::size_t index) noexcept
  {
    MDY_ASSERT(index <= 3);
    return mMatrixValue[index];
  }

  const DVector4& operator[](std::size_t index) const noexcept
  {
    MDY_ASSERT(index <= 3);
    return mMatrixValue[index];
  }

  DMatrix4 operator*(const DMatrix4& rhs) const noexcept;

  DMatrix4& operator*=(const DMatrix4& rhs) noexcept;

  DMatrix4 operator/(const DMatrix4& rhs) const;

  DMatrix4& operator/=(const DMatrix4& rhs);

  DMatrix4 operator+(const DMatrix4& rhs) const noexcept;

  DMatrix4& operator+=(const DMatrix4& rhs) noexcept;

  DMatrix4 operator-(const DMatrix4& rhs) const noexcept;

  DMatrix4& operator-=(const DMatrix4& rhs) noexcept;

  friend bool operator==(const DMatrix4& lhs, const DMatrix4& rhs) noexcept;

  friend bool operator!=(const DMatrix4& lhs, const DMatrix4& rhs) noexcept;

  /// @brief
  DMatrix4 Transpose() const noexcept;

  /// @brief P = this * V = rhs as mathmethical PV matrix multiplication.
  DMatrix4 Multiply(const DMatrix4& rhs) const noexcept;

  /// @brief P = this, v = rhs, r = result so r = P(v^T)
  DVector4 MultiplyVector(const DVector4& rhs) const noexcept;

  /// @brief Inverse.
  DMatrix4 Inverse() const;

  /// @brief Get identity matrix. \n
  /// [ 1 0 0 0 ] \n
  /// [ 0 1 0 0 ] \n
  /// [ 0 0 1 0 ] \n
  /// [ 0 0 0 1 ] \n
  static DMatrix4 Identity() noexcept;

  /// @brief 
  static DMatrix4 CreateWithScale(const DVector3& scaleVector);

  /// @brief
  static DMatrix4 CreateWithTranslation(const DVector3& translationPoint);

  /// @brief Get orthographic projection of OpenGL. \n
  /// [ 2/(r-l)    0       0    -(r+l)/(r-l) ] \n
  /// [   0     2/(t-b)    0    -(t+b)/(t-b) ] \n
  /// [   0        0   -2/(f-n) -(f+n)/(f-n) ] \n
  /// [   0        0       0          1      ]
  static DMatrix4 OrthoProjection(float left, float right, float bottom, float top, float near, float far);

  /// @brief Scale matrix.
  DMatrix4& Scale(const DVector3& iScaleFactor);
  /// @brief Rotate matrix.
  DMatrix4& Rotate(const DVector3& iRotationDegreeAngle);
  /// @brief Translate matrix.
  DMatrix4& Translate(const DVector3& iPosition);

private:
  /// Identity matrix constructor
  explicit DMatrix4(bool);

  /// Column major
  std::array<DVector4, 4> mMatrixValue;
};

} /// ::dy namespace

#endif /// GUARD_DY_HELPER_TYPE_MATRIX4_H