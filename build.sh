#!/bin/bash
# Bombic Native - Build script pro Manjaro/Arch Linux

set -e

# Kontrola závislostí
if ! pacman -Qi sdl2 sdl2_image sdl2_ttf sdl2_mixer sdl2_net cmake gcc &>/dev/null; then
    echo "Instaluji potřebné balíčky..."
    sudo pacman -S --needed git cmake gcc sdl2 sdl2_image sdl2_ttf sdl2_mixer sdl2_net
fi

# Build
cd "$(dirname "$0")"
mkdir -p build
cd build
cmake ..
make -j$(nproc)

echo ""
echo "════════════════════════════════════════════════════════"
echo "  Build dokončen!"
echo "════════════════════════════════════════════════════════"
echo ""
echo "  Pro instalaci do systému (doporučeno):"
echo "    ./install.sh"
echo ""
echo "  Pak spustíš hru odkudkoliv příkazem:"
echo "    bombic"
echo ""
