#pragma once

#include <chrono>
#include <mutex>
#include <vector>

struct AuroraDataPoint {
  float percent;
  std::chrono::system_clock::time_point timestamp;
};

class AuroraHistoryStore {
public:
  static constexpr int MAX_POINTS = 48; // 24 hours at 30-min intervals

  void addPoint(float percent) {
    std::lock_guard<std::mutex> lock(mutex_);

    AuroraDataPoint point;
    point.percent = percent;
    point.timestamp = std::chrono::system_clock::now();

    history_.push_back(point);

    // Keep only MAX_POINTS
    if (history_.size() > MAX_POINTS) {
      history_.erase(history_.begin());
    }
  }

  std::vector<AuroraDataPoint> getHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return history_;
  }

  float getCurrentPercent() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (history_.empty())
      return 0.0f;
    return history_.back().percent;
  }

  bool hasData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !history_.empty();
  }

private:
  mutable std::mutex mutex_;
  std::vector<AuroraDataPoint> history_;
};
