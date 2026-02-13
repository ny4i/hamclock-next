#pragma once

#include "../core/SantaData.h"
#include <memory>

class SantaProvider {
public:
  SantaProvider(std::shared_ptr<SantaStore> store) : store_(store) {}
  void update();

private:
  std::shared_ptr<SantaStore> store_;
};
