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

#include <chrono>
#include <vector>
#include <unordered_map>
#include "FIdiom.h"

class TimerChecker final
{
public:
  using TDurationValue = std::chrono::duration<double>;
  class Timer final
  {
  public:
    Timer(TimerChecker& timer, const std::string& key) : 
        mTimerKey{key}, 
        mStartPoint{std::chrono::system_clock::now()}, 
        mContainer{timer} { };

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    ~Timer()
    {
      //
      TDurationValue duration = std::chrono::system_clock::now() - this->mStartPoint;
      //
      if (const auto it = this->mContainer.mTimerMap.find(this->mTimerKey); 
          it != this->mContainer.mTimerMap.end())
      {
        auto& [key, list] = *it;

        if (list.size() >= 60)
        {
          dy::DyFastErase(list, list.begin());
        }
        list.emplace_back(duration);
      }
      else 
      { 
        this->mContainer.mTimerMap[this->mTimerKey] = std::vector<TDurationValue>{duration}; 
      }
    }

  private:
    std::string mTimerKey = "";
    std::chrono::time_point<std::chrono::system_clock> mStartPoint;
    TimerChecker& mContainer;
  };

  [[nodiscard]] Timer CheckTime(const std::string& key)
  {
    if (this->mTimerMap.find(key) != this->mTimerMap.end()) { throw std::exception("Key is already bound."); }
    return Timer{*this, key};
  }

  [[nodiscard]] std::vector<TDurationValue> GetDuration(const std::string& key)
  {
    const auto it = this->mTimerMap.find(key); 
    if (it != this->mTimerMap.end()) { return it->second; }
    else { throw std::exception("Could not find duration."); }
  }

private:
  std::unordered_map<std::string, std::vector<TDurationValue>> mTimerMap;

  friend class Timer;
};
