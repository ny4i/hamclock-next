#pragma once

#include <mutex>
#include <string>

class CitiesManager {
public:
  static CitiesManager &getInstance();

  void init();

  // Find name of nearest city within a reasonable distance (e.g. 50km)
  // Returns empty string if none found.
  std::string findNearest(float lat, float lon, float *dist_miles = nullptr);

private:
  CitiesManager() = default;
  std::mutex mutex_;
  bool initialized_ = false;
};
