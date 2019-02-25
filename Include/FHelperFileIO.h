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
#include <optional>
#include <vector>
#include "FMacro.h"

/// @brief Check file path is valid and file is exist on present filesystem. \n
/// This function supports relative path.
MCR_NODISCARD bool IsFileExist(const std::string& iFilePath);

/// @brief Read file as binary mode and return chunk of file buffer.
MCR_NODISCARD std::optional<std::vector<char>> ReadBinaryFile(const std::string& filePath);

/// @brief Get file name from path without file speicification (.exe, like)
MCR_NODISCARD std::string GetFileNameFromPath(const std::string& path) noexcept;
