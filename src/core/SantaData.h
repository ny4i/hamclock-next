#pragma once

#include <chrono>

struct SantaData {
  double lat;
  double lon;
  bool active = false;
  std::chrono::system_clock::time_point lastUpdate{};
};

class SantaStore {
public:
  SantaData get() const { return data_; }
  void set(const SantaData &d) { data_ = d; }

private:
  SantaData data_;
};
