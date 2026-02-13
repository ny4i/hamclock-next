#include "AuroraProvider.h"

AuroraProvider::AuroraProvider(NetworkManager &net) : net_(net) {}

void AuroraProvider::fetch(bool north, DataCb cb) {
  const char *url = north ? "https://services.swpc.noaa.gov/images/"
                            "aurora-forecast-northern-hemisphere.jpg"
                          : "https://services.swpc.noaa.gov/images/"
                            "aurora-forecast-southern-hemisphere.jpg";

  net_.fetchAsync(url, [cb](std::string body) {
    if (!body.empty()) {
      cb(body);
    }
  });
}
