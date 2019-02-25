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

//!
//! Global macroes
//!

/// DONT USE THIS DIRECTLY
#define MCR_TOKENPASTE(__MAX__, __MAY__) __MAX__ ## __MAY__
/// USE THIS DIRECTLY FOR CONCATNATION.
#define MCR_TOKENPASTE2(__MAX__, __MAY__) MDY_TOKENPASTE(__MAX__, __MAY__)

/// @macro MCR_TOSTRING
/// @macro Convert __MAString__ to const char* literal.
#define MCR_TOSTRING(__MAString__) #__MAString__

/// @macro MCR_NODISCARD
/// @brief Nodiscard specifier
#define MCR_NODISCARD [[nodiscard]]

/// @macro MCR_DEPRECATED
/// @brief Depereacted speicifer.
#define MCR_DEPRECATED(__MAVersion__, __MAFunction__, __MAAlternative__) \
  [[deprecated(MDY_TO_STRING(__MAFunction__) " is deprecated from " MDY_TO_STRING(__MAVersion__) ". Use " MDY_TO_STRING(__MAAlternative__) " instead.")]]

/// @macro MCR_FALLTHROUGH
/// @brief switch/case statement fallthrough next case.
#define MCR_FALLTHROUGH [[fallthrough]]

/// @brief Add not used variable as maybe_unused to avoid /W4 /Wall warning as error checking.
template <typename TType>
void __NotUsedButAccess([[maybe_unused]] const TType& type) {};

/// @define MCR_NOTUSED
/// @brief Wrapper macro of `__NotUsedButAccess` function.
#define MCR_NOTUSED(__Variable__) __NotUsedButAccess(__Variable__)

/// @macro MCR_BIND_BEGIN_END
/// @brief Help forward iteratable type to bind .begin() and .end() to function.
#define MCR_BIND_BEGIN_END(__MAIteratorableType__) \
    __MAIteratorableType__.begin(), __MAIteratorableType__.end()

/// @macro MCR_BIND_CBEGIN_CEND
/// @brief Help forward iteratable type to bind .begin() and .end() to function.
#define MCR_BIND_CBEGIN_CEND(__MAIteratorableType__) \
    __MAIteratorableType__.cbegin(), __MAIteratorableType__.cend()

/// @macro MCY_SINGLETON_PROPERTIES
/// This macro must not be attached to whichever class inherits IDySingleton<>.
#define MCY_SINGLETON_PROPERTIES(__MASingletonType__) \
public: \
    __MASingletonType__(const __MASingletonType__##&) = delete; \
    __MASingletonType__(__MASingletonType__##&&) = delete; \
    __MASingletonType__##& operator=(const __MASingletonType__##&) = delete; \
    __MASingletonType__##& operator=(__MASingletonType__##&&) = delete

/// @macro MDY_SINGLETON_DERIVED
/// This macro must not be attached to whichever class inherits IDySingleton<>.
#define MDY_SINGLETON_DERIVED(__MADerivedSingletonType__) \
private:                                                  \
    __MADerivedSingletonType__() = default;               \
    MCR_NODISCARD EDySuccess pfInitialize();              \
    MCR_NODISCARD EDySuccess pfRelease();                 \
    friend class IHelperSingleton<__MADerivedSingletonType__>;  \
public:                                                   \
    virtual ~__MADerivedSingletonType__() = default;
