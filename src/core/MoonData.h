#pragma once

#include <mutex>
#include <string>

struct MoonData {
  double phase;        // 0.0 to 1.0
  double illumination; // 0 to 100
  double azimuth;
  double elevation;
  std::string phaseName;
  std::string imageUrl;
  double posangle = 0.0;

  // EME Info
  double distance_km;
  double path_loss_db; // 144 MHz fallback
  double dx_elevation;
  double dx_azimuth;
  bool mutual_window;

  bool valid = false;
};

class MoonStore {
public:
  void update(const MoonData &data) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_ = data;
  }

  MoonData get() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_;
  }

private:
  mutable std::mutex mutex_;
  MoonData data_;
};
