# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

HamClock-Next is a modern SDL2-based C++20 reconstruction of the classic HamClock for amateur radio operators. It renders at a logical 800×480 resolution with hardware-accelerated super-sampling for high-DPI displays. Currently at v0.5B (public beta). No automated tests exist yet.

## Build Commands

### Quick local build (macOS or Linux with deps installed)
```bash
./scripts/build.sh          # cmake -B build && cmake --build build -j10 --clean-first
```

### macOS release build (arm64, static SDL2)
```bash
./scripts/build-macos.sh    # Output: build-macos/hamclock-next
```

### Manual build
```bash
cmake -B build -DENABLE_DEBUG_API=ON    # Enable web server debug endpoints
cmake --build build -j$(nproc)
./build/hamclock-next                    # -f fullscreen, -s software renderer
```

### Linux/RPi dependencies (apt)
```bash
sudo apt-get install cmake build-essential pkg-config \
  libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev \
  libcurl4-openssl-dev libsqlite3-dev
```

### Update embedded data
```bash
cmake --build build --target update-prefixes   # Country prefix data from country-files.com
cmake --build build --target update-cities     # City data from geonames.org
cmake --build build --target update-data       # Both
```

### Platform-specific release builds
Scripts in `scripts/`: `build-rpi-bullseye-fb0.sh`, `build-rpi-bullseye-x11.sh`, `build-rpi-bookworm-x11.sh`, `build-rpi-trixie-x11.sh`, `build-win64.sh` (cross-compiled from Linux), `build-opi-*.sh`.

## Architecture

### Layer Structure
```
src/main.cpp          — SDL2 init, event loop, provider/widget orchestration
src/core/             — State, config, data managers (ConfigManager, DatabaseManager, HamClockState)
src/services/         — Data providers (each fetches from one external API independently)
src/network/          — NetworkManager (curl wrapper), WebServer (REST API + live JPEG at :8080)
src/ui/               — Widget-based UI (all inherit from Widget base class)
include/              — Shared headers
```

### Data Flow
1. **Providers** (in `src/services/`) fetch data from external APIs on background threads
2. **Data structs** (in `src/core/` — `SolarData.h`, `LiveSpotData.h`, etc.) hold current state
3. **Panels** (in `src/ui/`) read state and render via SDL2; each panel is a `Widget` subclass
4. **PaneContainer** manages the grid layout and widget rotation
5. **ConfigManager** persists settings to `config.json`; **DatabaseManager** uses SQLite for persistent data
6. **WebServer** exposes REST API for remote control and a live JPEG mirror (see `API.md`)

### Key Patterns
- **All dependencies auto-fetched via CMake FetchContent** — no manual installs needed for SDL2, curl, sqlite, nlohmann_json, libpredict, httplib, spdlog
- **Zero-asset binary** — fonts (`EmbeddedFont.h`, 63k lines), icons, city data (`CitiesData.h`, 33k lines), prefix data (`PrefixData.h`, 28k lines) are compiled in
- **Single-threaded rendering** — SDL2 updates on main thread; network ops run on background threads
- **`ENABLE_DEBUG_API` compile flag** — gates debug endpoints and live view (off for release builds)
- **libpredict forced static on macOS** — `ld` doesn't support `--version-script`

### Adding New Features
- **New data source**: Create a `FooProvider` in `src/services/`, a data struct in `src/core/`, and a `FooPanel` in `src/ui/`
- **New panel**: Subclass `Widget`, implement `update()`, `render()`, and input handlers; register in `main.cpp` and `PaneContainer`
- **New source files must be added to `CMakeLists.txt`** manually (no glob)
- MCP scaffolding tools available — see `MCP_GUIDE.md` for AI-assisted development workflow

### Build Configuration
- `ENABLE_DEBUG_API=ON/OFF` — controls debug REST endpoints and live view
- SSL backends: mbedtls (Linux/RPi), Schannel (Windows), system OpenSSL (macOS)
- Static linking on Linux for portability (`-static-libstdc++ -static-libgcc`)

## Key Files

| File | Purpose |
|------|---------|
| `src/core/HamClockState.h` | Central application state struct |
| `src/core/ConfigManager.h/cpp` | Persistent settings (XDG paths) |
| `src/ui/Widget.h` | Base class for all UI panels |
| `src/ui/MapWidget.cpp` | Interactive world map (terminator, overlays, great circles) |
| `src/ui/PaneContainer.cpp` | Grid layout and widget rotation |
| `src/ui/SetupScreen.cpp` | Multi-tab configuration UI |
| `src/network/WebServer.cpp` | REST API + live JPEG |
| `API.md` | WebServer endpoint documentation |
| `TICKETS.md` | Partial features ranked by implementation effort |
| `.mcp/` | MCP server for AI-assisted development |

## CI/CD

GitHub Actions workflows build for all platforms on tagged releases (`refs/tags/v*`):
- `.github/workflows/build-macos.yml` — macOS arm64
- `.github/workflows/build-rpi.yml` — 4 RPi variants (.deb packages)
- `.github/workflows/build-windows.yml` — cross-compiled from Linux
- `.github/workflows/build-opi.yml` — OrangePi

## Gotchas

- Embedded data headers (`CitiesData.h`, `PrefixData.h`, `EmbeddedFont.h`) inflate compile times significantly — avoid touching them unless updating data
- First build is slow (FetchContent downloads all deps); incremental builds are fast
- RPi builds: use `-j1` on 1GB RAM devices to avoid OOM
- `main.cpp` (1,343 lines) orchestrates everything — candidate for extraction if it grows further
