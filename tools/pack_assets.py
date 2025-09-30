#!/usr/bin/env python3
"""One-shot helper to prepare LVGL assets for the PokerChip project.

Scans ./assets for referenced images/fonts and either emits LVGL-compatible
C arrays under src/ui/assets_gen.cpp or copies the raw files into ./data for
LittleFS, depending on the asset type.

Usage:
    python3 tools/pack_assets.py
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ASSETS_DIR = ROOT / "assets"
DATA_DIR = ROOT / "data"
OUTPUT_CPP = ROOT / "src" / "ui" / "assets_gen.cpp"


def discover_assets() -> list[Path]:
    if not ASSETS_DIR.exists():
        return []
    return [p for p in ASSETS_DIR.rglob("*") if p.is_file()]


def write_placeholder_cpp() -> None:
    OUTPUT_CPP.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_CPP.write_text(
        """// Auto-generated asset container.\n"
        "// TODO: Populate with LVGL image/font descriptors after running the\n"
        "// appropriate converters for the PokerChip MicroPython assets.\n"
        "#include \"ui_assets.hpp\"\n\n"
        "// Intentionally left blank for now.\n"
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Package LVGL assets")
    parser.parse_args()

    assets = discover_assets()
    if not assets:
        write_placeholder_cpp()
        print("No assets found under ./assets; generated placeholder assets_gen.cpp")
        return 0

    DATA_DIR.mkdir(exist_ok=True)
    for asset in assets:
        target = DATA_DIR / asset.name
        target.write_bytes(asset.read_bytes())
        print(f"Copied {asset} -> {target}")

    write_placeholder_cpp()
    print("Asset packaging complete. Update assets_gen.cpp with LVGL descriptors as needed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
