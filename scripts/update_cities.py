#!/usr/bin/env python3
import csv
import io
import os
import zipfile
import urllib.request

# URLs
CITIES_URL = "https://download.geonames.org/export/dump/cities15000.zip"
COUNTRY_URL = "https://download.geonames.org/export/dump/countryInfo.txt"
ADMIN1_URL = "https://download.geonames.org/export/dump/admin1CodesASCII.txt"

OUT_H = "src/core/CitiesData.h"


def download_text(url):
    print(f"Fetching {url}...")
    with urllib.request.urlopen(url) as response:
        return response.read().decode("utf-8")


def load_countries():
    data = download_text(COUNTRY_URL)
    countries = {}
    for line in data.splitlines():
        if line.startswith("#") or not line.strip():
            continue
        parts = line.split("\t")
        if len(parts) > 4:
            countries[parts[0]] = parts[4]  # ISO -> Name
    return countries


def load_admin1():
    data = download_text(ADMIN1_URL)
    admins = {}
    for line in data.splitlines():
        parts = line.split("\t")
        if len(parts) > 1:
            admins[parts[0]] = parts[1]
    return admins


def format_pop(n_str):
    try:
        n = int(n_str)
    except:
        return "0"
    if n >= 1000000:
        val = n / 1000000.0
        return f"{val:.1f}M".replace(".0M", "M")
    if n >= 1000:
        val = n / 1000.0
        return f"{val:.0f}K"
    return str(n)


def update():
    countries = load_countries()
    admins = load_admin1()

    if not os.path.exists("cities15000.txt"):
        print("Downloading cities15000.zip...")
        with urllib.request.urlopen(CITIES_URL) as r:
            z = zipfile.ZipFile(io.BytesIO(r.read()))
            z.extract("cities15000.txt")

    cities = []
    print(f"Processing cities15000.txt...")
    with open("cities15000.txt", "r", encoding="utf-8") as f:
        reader = csv.reader(f, delimiter="\t")
        for row in reader:
            if len(row) < 15:
                continue
            try:
                name = row[2] or row[1]
                lat = float(row[4])
                lon = float(row[5])
                cc = row[8]
                admin1 = row[10]
                pop = row[14]

                cname = countries.get(cc, cc)
                key = f"{cc}.{admin1}"
                aname = admins.get(key, admin1)

                parts = [name]
                if aname and aname != name and aname != cname:
                    parts.append(aname)
                parts.append(cname)

                desc = ", ".join(parts) + f". Pop {format_pop(pop)}"
                cities.append((lat, lon, desc))
            except:
                continue

    print(f"Writing {len(cities)} cities to {OUT_H}...")
    with open(OUT_H, "w", encoding="utf-8") as f:
        f.write("#pragma once\n\n")
        f.write("#include <cstddef>\n\n")
        f.write("struct StaticCityEntry {\n")
        f.write("    float lat;\n")
        f.write("    float lon;\n")
        f.write("    const char* name;\n")
        f.write("};\n\n")
        f.write("static const StaticCityEntry g_CityData[] = {\n")
        for lat, lon, name in cities:
            # Escape quotes in name
            safe_name = name.replace('"', '\\"')
            f.write(f'    {{{lat:.4f}f, {lon:.4f}f, "{safe_name}"}},\n')
        f.write("};\n\n")
        f.write(f"static const size_t g_CityDataSize = {len(cities)};\n")


if __name__ == "__main__":
    if not os.path.exists("src/core"):
        print("Run from project root!")
    else:
        update()
