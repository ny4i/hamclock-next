#pragma once

#include "../core/AuroraHistoryStore.h"
#include "FontManager.h"
#include "Widget.h"

#include <memory>

class AuroraGraphPanel : public Widget {
public:
  AuroraGraphPanel(int x, int y, int w, int h, FontManager &fontMgr,
                   std::shared_ptr<AuroraHistoryStore> store);

  void update() override;
  void render(SDL_Renderer *renderer) override;

private:
  FontManager &fontMgr_;
  std::shared_ptr<AuroraHistoryStore> store_;
};
