#pragma once

#include "../core/DXClusterData.h"
#include "ListPanel.h"
#include <chrono>
#include <memory>

class DXClusterPanel : public ListPanel {
public:
  DXClusterPanel(int x, int y, int w, int h, FontManager &fontMgr,
                 std::shared_ptr<DXClusterDataStore> store);

  void update() override;
  bool onMouseUp(int mx, int my, Uint16 mod) override;
  bool onMouseWheel(int scrollY) override;

  bool isSetupRequested() const { return setupRequested_; }
  void clearSetupRequest() { setupRequested_ = false; }

  std::string getName() const override { return "DXCluster"; }
  std::vector<std::string> getActions() const override;
  SDL_Rect getActionRect(const std::string &action) const override;
  nlohmann::json getDebugData() const override;

private:
  void rebuildRows(const DXClusterData &data);
  std::string
  formatAge(const std::chrono::system_clock::time_point &spottedAt) const;

  std::shared_ptr<DXClusterDataStore> store_;
  std::chrono::system_clock::time_point lastUpdate_{};
  bool setupRequested_ = false;

  std::vector<std::string> allRows_;
  int scrollOffset_ = 0;
  static constexpr int MAX_VISIBLE_ROWS = 15;
};
