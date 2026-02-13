#pragma once

#include "../core/MoonData.h"
#include "../network/NetworkManager.h"
#include <memory>

class MoonProvider {
public:
  MoonProvider(NetworkManager &net, std::shared_ptr<MoonStore> store);

  void update(double lat, double lon);

private:
  NetworkManager &net_;
  std::shared_ptr<MoonStore> store_;
};
