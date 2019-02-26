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

/// Header file
#include "Type/DMatrix4.h"

#include <glm/gtc/matrix_transform.hpp>
#include "FMacro.h"

namespace dy
{

DMatrix4::DMatrix4(const DVector4& column1, const DVector4& column2,
                   const DVector4& column3, const DVector4& column4) :
    mMatrixValue{ column1, column2, column3, column4 } { }

DMatrix4::DMatrix4(const glm::mat4& glmMatrix) noexcept
{
  mMatrixValue[0] = glmMatrix[0];
  mMatrixValue[1] = glmMatrix[1];
  mMatrixValue[2] = glmMatrix[2];
  mMatrixValue[3] = glmMatrix[3];
}

DMatrix4& DMatrix4::operator=(const glm::mat4& value) noexcept
{
  this->mMatrixValue[0] = value[0];
  this->mMatrixValue[1] = value[1];
  this->mMatrixValue[2] = value[2];
  this->mMatrixValue[3] = value[3];
  return *this;
}

DMatrix4::operator glm::mat4() const noexcept
{
  return glm::mat4{
      mMatrixValue[0][0], mMatrixValue[0][1], mMatrixValue[0][2], mMatrixValue[0][3],
      mMatrixValue[1][0], mMatrixValue[1][1], mMatrixValue[1][2], mMatrixValue[1][3],
      mMatrixValue[2][0], mMatrixValue[2][1], mMatrixValue[2][2], mMatrixValue[2][3],
      mMatrixValue[3][0], mMatrixValue[3][1], mMatrixValue[3][2], mMatrixValue[3][3]
  };
}

DMatrix4 DMatrix4::operator*(const DMatrix4& rhs) const noexcept
{
  return DMatrix4{
      (*this)[0][0] * rhs[0][0], (*this)[1][0] * rhs[1][0], (*this)[2][0] * rhs[2][0], (*this)[3][0] * rhs[3][0],
      (*this)[0][1] * rhs[0][1], (*this)[1][1] * rhs[1][1], (*this)[2][1] * rhs[2][1], (*this)[3][1] * rhs[3][1],
      (*this)[0][2] * rhs[0][2], (*this)[1][2] * rhs[1][2], (*this)[2][2] * rhs[2][2], (*this)[3][2] * rhs[3][2],
      (*this)[0][3] * rhs[0][3], (*this)[1][3] * rhs[1][3], (*this)[2][3] * rhs[2][3], (*this)[3][3] * rhs[3][3],
  };
}

DMatrix4& DMatrix4::operator*=(const DMatrix4& rhs) noexcept
{
  this->mMatrixValue[0][0] = (*this)[0][0] * rhs[0][0];
  this->mMatrixValue[1][0] = (*this)[1][0] * rhs[1][0];
  this->mMatrixValue[2][0] = (*this)[2][0] * rhs[2][0];
  this->mMatrixValue[3][0] = (*this)[3][0] * rhs[3][0];

  this->mMatrixValue[0][1] = (*this)[0][1] * rhs[0][1];
  this->mMatrixValue[1][1] = (*this)[1][1] * rhs[1][1];
  this->mMatrixValue[2][1] = (*this)[2][1] * rhs[2][1];
  this->mMatrixValue[3][1] = (*this)[3][1] * rhs[3][1];

  this->mMatrixValue[0][2] = (*this)[0][2] * rhs[0][2];
  this->mMatrixValue[1][2] = (*this)[1][2] * rhs[1][2];
  this->mMatrixValue[2][2] = (*this)[2][2] * rhs[2][2];
  this->mMatrixValue[3][2] = (*this)[3][2] * rhs[3][2];

  this->mMatrixValue[0][3] = (*this)[0][3] * rhs[0][3];
  this->mMatrixValue[1][3] = (*this)[1][3] * rhs[1][3];
  this->mMatrixValue[2][3] = (*this)[2][3] * rhs[2][3];
  this->mMatrixValue[3][3] = (*this)[3][3] * rhs[3][3];
  return *this;
}

DMatrix4 DMatrix4::operator/(const DMatrix4& rhs) const
{
  for (int32_t i = 0; i < 4; ++i)
  {
    for (int32_t j = 0; j < 4; ++j)
    {
      if (rhs[i][j] == 0.f) throw std::runtime_error("Divide by zero.");
    }
  }

  return DMatrix4{
      (*this)[0][0] / rhs[0][0], (*this)[1][0] / rhs[1][0], (*this)[2][0] / rhs[2][0], (*this)[3][0] / rhs[3][0],
      (*this)[0][1] / rhs[0][1], (*this)[1][1] / rhs[1][1], (*this)[2][1] / rhs[2][1], (*this)[3][1] / rhs[3][1],
      (*this)[0][2] / rhs[0][2], (*this)[1][2] / rhs[1][2], (*this)[2][2] / rhs[2][2], (*this)[3][2] / rhs[3][2],
      (*this)[0][3] / rhs[0][3], (*this)[1][3] / rhs[1][3], (*this)[2][3] / rhs[2][3], (*this)[3][3] / rhs[3][3],
  };
}

DMatrix4& DMatrix4::operator/=(const DMatrix4& rhs)
{
  for (int32_t i = 0; i < 4; ++i)
  {
    for (int32_t j = 0; j < 4; ++j)
    {
      if (rhs[i][j] == 0.f) throw std::runtime_error("Divide by zero.");
    }
  }

  this->mMatrixValue[0][0] = (*this)[0][0] / rhs[0][0];
  this->mMatrixValue[1][0] = (*this)[1][0] / rhs[1][0];
  this->mMatrixValue[2][0] = (*this)[2][0] / rhs[2][0];
  this->mMatrixValue[3][0] = (*this)[3][0] / rhs[3][0];

  this->mMatrixValue[0][1] = (*this)[0][1] / rhs[0][1];
  this->mMatrixValue[1][1] = (*this)[1][1] / rhs[1][1];
  this->mMatrixValue[2][1] = (*this)[2][1] / rhs[2][1];
  this->mMatrixValue[3][1] = (*this)[3][1] / rhs[3][1];

  this->mMatrixValue[0][2] = (*this)[0][2] / rhs[0][2];
  this->mMatrixValue[1][2] = (*this)[1][2] / rhs[1][2];
  this->mMatrixValue[2][2] = (*this)[2][2] / rhs[2][2];
  this->mMatrixValue[3][2] = (*this)[3][2] / rhs[3][2];

  this->mMatrixValue[0][3] = (*this)[0][3] / rhs[0][3];
  this->mMatrixValue[1][3] = (*this)[1][3] / rhs[1][3];
  this->mMatrixValue[2][3] = (*this)[2][3] / rhs[2][3];
  this->mMatrixValue[3][3] = (*this)[3][3] / rhs[3][3];
  return *this;;
}

DMatrix4 DMatrix4::operator+(const DMatrix4& rhs) const noexcept
{
  return DMatrix4{
      (*this)[0][0] + rhs[0][0], (*this)[1][0] + rhs[1][0], (*this)[2][0] + rhs[2][0], (*this)[3][0] + rhs[3][0],
      (*this)[0][1] + rhs[0][1], (*this)[1][1] + rhs[1][1], (*this)[2][1] + rhs[2][1], (*this)[3][1] + rhs[3][1],
      (*this)[0][2] + rhs[0][2], (*this)[1][2] + rhs[1][2], (*this)[2][2] + rhs[2][2], (*this)[3][2] + rhs[3][2],
      (*this)[0][3] + rhs[0][3], (*this)[1][3] + rhs[1][3], (*this)[2][3] + rhs[2][3], (*this)[3][3] + rhs[3][3],
  };
}

DMatrix4& DMatrix4::operator+=(const DMatrix4& rhs) noexcept
{
  this->mMatrixValue[0][0] += rhs[0][0];
  this->mMatrixValue[1][0] += rhs[1][0];
  this->mMatrixValue[2][0] += rhs[2][0];
  this->mMatrixValue[3][0] += rhs[3][0];

  this->mMatrixValue[0][1] += rhs[0][1];
  this->mMatrixValue[1][1] += rhs[1][1];
  this->mMatrixValue[2][1] += rhs[2][1];
  this->mMatrixValue[3][1] += rhs[3][1];

  this->mMatrixValue[0][2] += rhs[0][2];
  this->mMatrixValue[1][2] += rhs[1][2];
  this->mMatrixValue[2][2] += rhs[2][2];
  this->mMatrixValue[3][2] += rhs[3][2];

  this->mMatrixValue[0][3] += rhs[0][3];
  this->mMatrixValue[1][3] += rhs[1][3];
  this->mMatrixValue[2][3] += rhs[2][3];
  this->mMatrixValue[3][3] += rhs[3][3];;

  return *this;
}

DMatrix4 DMatrix4::operator-(const DMatrix4& rhs) const noexcept
{
  return DMatrix4{
      (*this)[0][0] - rhs[0][0], (*this)[1][0] - rhs[1][0], (*this)[2][0] - rhs[2][0], (*this)[3][0] - rhs[3][0],
      (*this)[0][1] - rhs[0][1], (*this)[1][1] - rhs[1][1], (*this)[2][1] - rhs[2][1], (*this)[3][1] - rhs[3][1],
      (*this)[0][2] - rhs[0][2], (*this)[1][2] - rhs[1][2], (*this)[2][2] - rhs[2][2], (*this)[3][2] - rhs[3][2],
      (*this)[0][3] - rhs[0][3], (*this)[1][3] - rhs[1][3], (*this)[2][3] - rhs[2][3], (*this)[3][3] - rhs[3][3],
  };
}

DMatrix4& DMatrix4::operator-=(const DMatrix4& rhs) noexcept
{
  this->mMatrixValue[0][0] -= rhs[0][0];
  this->mMatrixValue[1][0] -= rhs[1][0];
  this->mMatrixValue[2][0] -= rhs[2][0];
  this->mMatrixValue[3][0] -= rhs[3][0];

  this->mMatrixValue[0][1] -= rhs[0][1];
  this->mMatrixValue[1][1] -= rhs[1][1];
  this->mMatrixValue[2][1] -= rhs[2][1];
  this->mMatrixValue[3][1] -= rhs[3][1];

  this->mMatrixValue[0][2] -= rhs[0][2];
  this->mMatrixValue[1][2] -= rhs[1][2];
  this->mMatrixValue[2][2] -= rhs[2][2];
  this->mMatrixValue[3][2] -= rhs[3][2];

  this->mMatrixValue[0][3] -= rhs[0][3];
  this->mMatrixValue[1][3] -= rhs[1][3];
  this->mMatrixValue[2][3] -= rhs[2][3];
  this->mMatrixValue[3][3] -= rhs[3][3];;

  return *this;
}

DMatrix4 DMatrix4::Transpose() const noexcept
{
  return DMatrix4{
      this->mMatrixValue[0][0], this->mMatrixValue[0][1], this->mMatrixValue[0][2], this->mMatrixValue[0][3],
      this->mMatrixValue[1][0], this->mMatrixValue[1][1], this->mMatrixValue[1][2], this->mMatrixValue[1][3],
      this->mMatrixValue[2][0], this->mMatrixValue[2][1], this->mMatrixValue[2][2], this->mMatrixValue[2][3],
      this->mMatrixValue[3][0], this->mMatrixValue[3][1], this->mMatrixValue[3][2], this->mMatrixValue[3][3]
  };
}

DMatrix4 DMatrix4::Multiply(const DMatrix4& rhs) const noexcept
{
  // Naive! O(N^3)
  return DMatrix4{
      (*this)[0][0] * rhs[0][0] + (*this)[1][0] * rhs[0][1] + (*this)[2][0] * rhs[0][2] + (*this)[3][0] * rhs[0][3],
      (*this)[0][0] * rhs[1][0] + (*this)[1][0] * rhs[1][1] + (*this)[2][0] * rhs[1][2] + (*this)[3][0] * rhs[1][3],
      (*this)[0][0] * rhs[2][0] + (*this)[1][0] * rhs[2][1] + (*this)[2][0] * rhs[2][2] + (*this)[3][0] * rhs[2][3],
      (*this)[0][0] * rhs[3][0] + (*this)[1][0] * rhs[3][1] + (*this)[2][0] * rhs[3][2] + (*this)[3][0] * rhs[3][3],

      (*this)[0][1] * rhs[0][0] + (*this)[1][1] * rhs[0][1] + (*this)[2][1] * rhs[0][2] + (*this)[3][1] * rhs[0][3],
      (*this)[0][1] * rhs[1][0] + (*this)[1][1] * rhs[1][1] + (*this)[2][1] * rhs[1][2] + (*this)[3][1] * rhs[1][3],
      (*this)[0][1] * rhs[2][0] + (*this)[1][1] * rhs[2][1] + (*this)[2][1] * rhs[2][2] + (*this)[3][1] * rhs[2][3],
      (*this)[0][1] * rhs[3][0] + (*this)[1][1] * rhs[3][1] + (*this)[2][1] * rhs[3][2] + (*this)[3][1] * rhs[3][3],

      (*this)[0][2] * rhs[0][0] + (*this)[1][2] * rhs[0][1] + (*this)[2][2] * rhs[0][2] + (*this)[3][2] * rhs[0][3],
      (*this)[0][2] * rhs[1][0] + (*this)[1][2] * rhs[1][1] + (*this)[2][2] * rhs[1][2] + (*this)[3][2] * rhs[1][3],
      (*this)[0][2] * rhs[2][0] + (*this)[1][2] * rhs[2][1] + (*this)[2][2] * rhs[2][2] + (*this)[3][2] * rhs[2][3],
      (*this)[0][2] * rhs[3][0] + (*this)[1][2] * rhs[3][1] + (*this)[2][2] * rhs[3][2] + (*this)[3][2] * rhs[3][3],

      (*this)[0][3] * rhs[0][0] + (*this)[1][3] * rhs[0][1] + (*this)[2][3] * rhs[0][2] + (*this)[3][3] * rhs[0][3],
      (*this)[0][3] * rhs[1][0] + (*this)[1][3] * rhs[1][1] + (*this)[2][3] * rhs[1][2] + (*this)[3][3] * rhs[1][3],
      (*this)[0][3] * rhs[2][0] + (*this)[1][3] * rhs[2][1] + (*this)[2][3] * rhs[2][2] + (*this)[3][3] * rhs[2][3],
      (*this)[0][3] * rhs[3][0] + (*this)[1][3] * rhs[3][1] + (*this)[2][3] * rhs[3][2] + (*this)[3][3] * rhs[3][3]
  };
}

DVector4 DMatrix4::MultiplyVector(const DVector4& rhs) const noexcept
{
  return DVector4{
      (*this)[0][0] * rhs.X + (*this)[1][0] * rhs.Y + (*this)[2][0] * rhs.Z + (*this)[3][0] * rhs.W,
      (*this)[0][1] * rhs.X + (*this)[1][1] * rhs.Y + (*this)[2][1] * rhs.Z + (*this)[3][1] * rhs.W,
      (*this)[0][2] * rhs.X + (*this)[1][2] * rhs.Y + (*this)[2][2] * rhs.Z + (*this)[3][2] * rhs.W,
      (*this)[0][3] * rhs.X + (*this)[1][3] * rhs.Y + (*this)[2][3] * rhs.Z + (*this)[3][3] * rhs.W
  };
}

DMatrix4 DMatrix4::Inverse() const
{
  // temporary
  const glm::mat4 mat = static_cast<glm::mat4>(*this);
  return glm::inverse(mat);
}

DMatrix4 DMatrix4::Identity() noexcept
{
  static DMatrix4 staticInstance { true };
  return staticInstance;
}

DMatrix4 DMatrix4::CreateWithScale(const DVector3& scaleVector)
{
  return DMatrix4{
    scaleVector.X, 0, 0, 0,
    0, scaleVector.Y, 0, 0,
    0, 0, scaleVector.Z, 0,
    0, 0, 0,             1};
}

DMatrix4 DMatrix4::CreateWithTranslation(const DVector3& translationPoint)
{
  return DMatrix4{
    1, 0, 0, translationPoint.X,
    0, 1, 0, translationPoint.Y,
    0, 0, 1, translationPoint.Z,
    0, 0, 0, 1};
}

DMatrix4 DMatrix4::OrthoProjection(float left, float right, float bottom, float top, float near, float far)
{
  return glm::ortho(left, right, bottom, top, near, far);
}

DMatrix4& DMatrix4::Scale( const DVector3& iScaleFactor)
{
  const auto mat = glm::scale(static_cast<glm::mat4>(*this), static_cast<glm::vec3>(iScaleFactor));
  (*this)[0] = mat[0]; (*this)[1] = mat[1]; (*this)[2] = mat[2]; (*this)[3] = mat[3];
  return *this;
}

DMatrix4& DMatrix4::Rotate(const DVector3& iRotationDegreeAngle)
{
  MCR_NOTUSED(iRotationDegreeAngle);
  MDY_NOT_IMPLEMENTED_ASSERT();
  //const auto mat = this->Multiply(DDyQuaternion(iRotationDegreeAngle).GetRotationMatrix4x4());
  //(*this)[0] = mat[0]; (*this)[1] = mat[1]; (*this)[2] = mat[2]; (*this)[3] = mat[3];
  return *this;
}

DMatrix4& DMatrix4::Translate( const DVector3& iPosition)
{
  (*this)[3][0] += iPosition.X;
  (*this)[3][1] += iPosition.Y;
  (*this)[3][2] += iPosition.Z;
  return *this;
}

DMatrix4::DMatrix4(bool)
{
  this->mMatrixValue[0][0] = 1;
  this->mMatrixValue[0][3] = 0;
  this->mMatrixValue[1][1] = 1;
  this->mMatrixValue[1][3] = 0;
  this->mMatrixValue[2][2] = 1;
  this->mMatrixValue[2][3] = 0;
  this->mMatrixValue[3][3] = 1;
}

bool operator==( const DMatrix4& lhs,  const DMatrix4& rhs) noexcept
{
  for (size_t i = 0; i < 4; ++i)
  { 
    if (lhs.mMatrixValue[i] != rhs.mMatrixValue[i]) { return false; }
  }
  return true;
}

bool operator!=( const DMatrix4& lhs,  const DMatrix4& rhs) noexcept
{
  return !(lhs == rhs);
}

} /// ::dy namespace