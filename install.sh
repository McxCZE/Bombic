#!/bin/bash
# Bombic Native - Install script

set -e
cd "$(dirname "$0")"

if [ ! -f build/src/bombic ]; then
    echo "Chyba: Nejdřív musíš hru zkompilovat."
    echo ""
    echo "Spusť: ./build.sh"
    exit 1
fi

echo "Instaluji Bombic do systému..."
cd build
sudo make install

echo ""
echo "════════════════════════════════════════════════════════"
echo "  Instalace dokončena!"
echo "════════════════════════════════════════════════════════"
echo ""
echo "  Spusť hru příkazem:"
echo "    bombic"
echo ""
