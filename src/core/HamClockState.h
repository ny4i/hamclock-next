#pragma once

#include "Astronomy.h"

#include <chrono>
#include <map>
#include <string>

struct ServiceStatus {
  bool ok = false;
  std::string lastError;
  std::chrono::system_clock::time_point lastSuccess{};
};

struct HamClockState {
  // DE (home) station — set from config
  LatLon deLocation = {0, 0};
  std::string deCallsign;
  std::string deGrid;

  // DX (target) station — set by map clicks
  LatLon dxLocation = {0, 0};
  std::string dxGrid;
  bool dxActive = false;

  // Telemetry
  float fps = 0.0f;
  std::map<std::string, ServiceStatus> services;
};
