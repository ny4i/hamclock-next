#include "DRAPProvider.h"
#include "../core/Logger.h"
#include <sstream>

DRAPProvider::DRAPProvider(NetworkManager &net) : net_(net) {}

void DRAPProvider::fetch(DataCb cb) {
  const char *url =
      "https://services.swpc.noaa.gov/text/drap_global_frequencies.txt";

  net_.fetchAsync(url, [cb](std::string body) {
    if (body.empty()) {
      LOG_W("DRAPProvider", "Empty response from DRAP data source");
      return;
    }

    try {
      float max_freq = 0.0f;
      bool found_any = false;

      std::stringstream ss(body);
      std::string line;

      while (std::getline(ss, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == '\r' ||
            line[0] == '\n')
          continue;

        // Look for pipe delimiter
        size_t pipe_pos = line.find('|');
        if (pipe_pos != std::string::npos) {
          // All numbers after pipe are frequencies
          std::string freqs = line.substr(pipe_pos + 1);
          std::stringstream ss_vals(freqs);
          float val;

          while (ss_vals >> val) {
            if (val > max_freq)
              max_freq = val;
            found_any = true;
          }
        }
      }

      if (found_any) {
        // Format as a simple string with the max frequency
        char buf[64];
        std::snprintf(buf, sizeof(buf), "%.1f", max_freq);
        cb(std::string(buf));
        LOG_D("DRAPProvider", "DRAP max frequency: {:.1f} MHz", max_freq);
      } else {
        LOG_W("DRAPProvider", "No DRAP data found in response");
      }
    } catch (const std::exception &e) {
      LOG_E("DRAPProvider", "Error parsing DRAP data: {}", e.what());
    }
  });
}
