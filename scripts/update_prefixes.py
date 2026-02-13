#!/usr/bin/env python3
import csv
import io
import re
import os
import subprocess

# Official Big CTY CSV from country-files.com
URL = "https://www.country-files.com/bigcty/cty.csv"
OUT_H = "src/core/PrefixData.h"


def update():
    print(f"Fetching {URL}...")
    try:
        # Using curl for robustness
        result = subprocess.run(
            ["curl", "-sSL", URL], capture_output=True, text=True, timeout=30
        )
        if result.returncode != 0:
            print(f"Curl failed with code {result.returncode}")
            return
        content = result.stdout
    except Exception as e:
        print(f"Error fetching: {e}")
        return

    # Store results: prefix -> {lat, lon, dxcc}
    db = {}

    # CSV Format (official):
    # 0: Primary Prefix
    # 1: Entity Name
    # 2: DXCC
    # 3: Continent
    # 4: CQ Zone
    # 5: ITU Zone
    # 6: Latitude
    # 7: Longitude (Positive West in this CSV!)
    # 8: Time Zone
    # 9: Prefixes (Space separated list)

    reader = csv.reader(io.StringIO(content))
    for row in reader:
        if not row or len(row) < 10:
            continue

        primary = row[0]
        dxcc = int(row[2])
        lat = float(row[6])
        # CSV uses positive West. Our application uses positive East.
        lon = -float(row[7])

        # Prefixes are in col 9, but col 0 is sometimes not in col 9
        prefix_list = row[9].strip(";").split()
        if primary not in prefix_list:
            prefix_list.append(primary)

        for p in prefix_list:
            # Remove overrides: (CQ), [ITU], <CONT>, {LAT/LON}
            p = re.sub(r"\(.*?\)", "", p)
            p = re.sub(r"\[.*?\]", "", p)
            p = re.sub(r"<.*?>", "", p)
            p = re.sub(r"\{.*?\}", "", p)

            p = p.strip()
            if not p:
                continue

            # Use specific lat/lon if they were in {} (some cty versions have them)
            # but standard cty.csv seems to avoid them or uses separate entries.
            # In bigcty, many entries are already separate rows.

            db[p] = (lat, lon, dxcc)

    # Flatten into sorted list
    entries = []
    for p, (lat, lon, dxcc) in db.items():
        entries.append({"prefix": p, "lat": lat, "lon": lon, "dxcc": dxcc})

    # Sort entries by prefix string for binary search compatibility
    entries.sort(key=lambda x: x["prefix"])

    print(f"Writing {len(entries)} entries to {OUT_H}...")

    with open(OUT_H, "w") as f:
        f.write("#pragma once\n\n")
        f.write("#include <cstdint>\n")
        f.write("#include <cstddef>\n\n")
        f.write("struct StaticPrefixEntry {\n")
        f.write("    const char* prefix;\n")
        f.write("    float lat;\n")
        f.write("    float lon;\n")
        f.write("    int16_t dxcc;\n")
        f.write("};\n\n")
        f.write(f"static const StaticPrefixEntry g_PrefixData[] = {{\n")
        for e in entries:
            safe_p = e["prefix"].replace('"', '\\"')
            f.write(
                f'    {{"{safe_p}", {e["lat"]:.2f}f, {e["lon"]:.2f}f, {e["dxcc"]}}},\n'
            )
        f.write("};\n\n")
        f.write(f"static const size_t g_PrefixDataSize = {len(entries)};\n")


if __name__ == "__main__":
    if not os.path.exists("src/core"):
        print("Run from project root!")
    else:
        update()
