#pragma once

#include "../core/DstData.h"
#include "../network/NetworkManager.h"
#include <memory>

class DstProvider {
public:
  DstProvider(NetworkManager &net, std::shared_ptr<DstStore> store);
  void fetch();

private:
  NetworkManager &net_;
  std::shared_ptr<DstStore> store_;
};
