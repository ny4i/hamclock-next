#include "CitiesManager.h"
#include "CitiesData.h"
#include "Logger.h"
#include <cmath>

CitiesManager &CitiesManager::getInstance() {
  static CitiesManager instance;
  return instance;
}

void CitiesManager::init() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (initialized_)
    return;
  initialized_ = true;
  LOG_I("CitiesManager", "Initialized with {} static cities.", g_CityDataSize);
}

// Simple squared Euclidean distance in degrees (corrected for longitude)
// Sufficient for finding the nearest neighbor.
static float get_dist_sq(float lat1, float lon1, float lat2, float lon2) {
  float dy = lat1 - lat2;
  float dx = (lon1 - lon2) * std::cos(lat1 * 0.0174532925f);
  return dy * dy + dx * dx;
}

std::string CitiesManager::findNearest(float lat, float lon,
                                       float *dist_miles) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!initialized_)
    return "";

  float best_d2 = 1e10f;
  const char *best_name = nullptr;

  for (size_t i = 0; i < g_CityDataSize; ++i) {
    float d2 = get_dist_sq(lat, lon, g_CityData[i].lat, g_CityData[i].lon);
    if (d2 < best_d2) {
      best_d2 = d2;
      best_name = g_CityData[i].name;
    }
  }

  if (best_name) {
    if (dist_miles) {
      // Very rough conversion: 1 degree is ~69.1 miles
      *dist_miles = std::sqrt(best_d2) * 69.1f;
    }
    return best_name;
  }
  return "";
}
