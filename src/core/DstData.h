#pragma once

#include <string>
#include <vector>

struct DstPoint {
  float age_hrs; // negative, hours ago
  float value;   // nT
};

struct DstData {
  std::vector<DstPoint> points;
  float current_val = 0.0f;
  bool valid = false;
};

class DstStore {
public:
  const DstData &get() const { return data_; }
  void set(const DstData &d) { data_ = d; }

private:
  DstData data_;
};
