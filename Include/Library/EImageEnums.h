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

namespace dy
{

/// @enum EImageColorFormatStyle
/// @brief Image color format which is a type of each pixel data.
enum class EImageColorFormatStyle
{
  NoneError,  // Do not use this.
  R,          // Red-Green 8bit unsigned.
  RG,         // Red-Green 8bit unsigned.
  RGB,        // Red-Green-Blue 8bit unsigned.
  RGBA        // Red-Green-Blue and alpha 8bit unsigned.
};

} /// ::dy namespace
