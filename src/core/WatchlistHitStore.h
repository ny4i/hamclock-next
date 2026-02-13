#pragma once

#include <chrono>
#include <mutex>
#include <string>
#include <vector>

struct WatchlistHit {
  std::string call;
  float freqKhz;
  std::string mode;
  std::string source; // "Cluster", "PSK", etc
  std::chrono::system_clock::time_point time;
};

class WatchlistHitStore {
public:
  void addHit(const WatchlistHit &hit) {
    std::lock_guard<std::mutex> lock(mutex_);
    hits_.insert(hits_.begin(), hit);
    if (hits_.size() > 50)
      hits_.pop_back();
    lastUpdate_ = std::chrono::system_clock::now();
  }

  std::vector<WatchlistHit> getHits() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return hits_;
  }

  std::chrono::system_clock::time_point lastUpdate() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lastUpdate_;
  }

private:
  mutable std::mutex mutex_;
  std::vector<WatchlistHit> hits_;
  std::chrono::system_clock::time_point lastUpdate_{};
};
