# Bombic

> **FORK** - Toto je fork původní hry Bombic od Bernarda Lidického (Hippo Games, 2001).
> Původní hra byla vytvořena pro Windows a je licencována pod **GNU General Public License v2**.

Bombic je klasická arkádová hra ve stylu Bombermana. Původně vytvořena pro Windows, nyní portována na Linux s použitím SDL2.

## Novinky oproti originálu

- **LAN multiplayer** - Možnost hrát po síti s více hráči (host/client režim)

## Rychlá instalace (Arch Linux / Manjaro)

```bash
cd ~/Games
git clone https://github.com/user/Bombic-Native.git
cd Bombic-Native
./build.sh
./install.sh
bombic
```

## Rychlá instalace (Ubuntu / Debian)

```bash
sudo apt update && sudo apt install -y git cmake g++ libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
cd ~/Games
git clone https://github.com/user/Bombic-Native.git
cd Bombic-Native
./build.sh
./install.sh
bombic
```

## Rychlá instalace (Fedora)

```bash
sudo dnf install git cmake gcc-c++ SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel
cd ~/Games
git clone https://github.com/user/Bombic-Native.git
cd Bombic-Native
./build.sh
./install.sh
bombic
```

## Ovládání

### Hráč 1 (šipky)
- **Pohyb**: Šipky (←↑↓→)
- **Položit bombu**: Mezerník

### Hráč 2 (WASD)
- **Pohyb**: W, A, S, D
- **Položit bombu**: Levý Ctrl

### Hráč 3
- **Pohyb**: I, J, K, L
- **Položit bombu**: Pravý Ctrl

### Hráč 4 (numerická klávesnice)
- **Pohyb**: 8, 4, 5, 6
- **Položit bombu**: 0

### Menu
- **Navigace**: Šipky nahoru/dolů
- **Výběr**: Enter nebo Mezerník
- **Zpět**: Escape

## Nastavení

Nastavení se automaticky ukládá do:
```
~/.config/bombic/bombic.conf
```

## Odinstalace

```bash
sudo rm /usr/local/bin/bombic
sudo rm -rf /usr/local/share/bombic
```

## Řešení problémů

### Hra se nespustí

1. Ujistěte se, že máte nainstalované všechny knihovny
2. Spusťte `./install.sh` pro instalaci do systému
3. Spusťte hru z terminálu a podívejte se na chybové hlášky

### Není slyšet zvuk

1. Zkontrolujte, zda není ztlumený systémový zvuk
2. V nastavení hry zkontrolujte, zda je zvuk zapnutý

## Příkazový řádek

```
bombic [možnosti]

Možnosti:
  -h, --help         Zobrazí nápovědu
  -f, --fullscreen   Spustí v režimu celé obrazovky
  -p X, --players X  Počet hráčů (1-4)
  -d, --deathmatch   Režim deathmatch
  -m SOUBOR          Načte konkrétní mapu
```

## Autoři

### Původní tým (Hippo Games, 2001)
- **Programování**: Bernard Lidický <bernard@matfyz.cz>
- **Grafika**: Zdenek Boswart <2zdeny@seznam.cz>
- **Překlad DE**: Ronny <ronny@gamezworld.de>
- **Překlad PL**: Simperium <simperium@tlen.pl>
- **Přispěvatel**: Michal Marek <michal.marek@matfyz.cz>

### Linux port
- SDL2 migrace a modernizace

## Licence

Tato hra je open source pod licencí **GNU General Public License v2 (GPLv2)**.
Viz soubor [COPYING](COPYING) pro úplné znění licence.
