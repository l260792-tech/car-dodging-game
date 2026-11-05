# Group 5 — Car Dodging Game

A 2D C++ game made with SFML 2.5. The game uses drawn shapes, so no downloaded car sprites are required.

## Team
- Muhammad Tayyab — 26L-0765
- Muhammad Asim — 26L-0792
- Maryam Sajjad — 26L-0851

## Features
- Easy, Medium and Hard difficulty selection
- Three-lane road and player movement
- Random incoming traffic
- Collision detection and Game Over
- Score and persistent high score (`highscore.txt`)
- Main menu, instructions, pause, restart and return-to-menu states

## Requirements
- C++17 compiler
- CMake 3.16+
- SFML 2.5 (the source uses the SFML 2.x API)
- A TrueType font at `assets/arial.ttf`, or Windows Arial at `C:/Windows/Fonts/arial.ttf`

If the font cannot be found, the game exits with code 1. Copy a legally available Arial-compatible `.ttf` font into `assets/` and name it `arial.ttf`.

## Build (Windows)
Install a C++ compiler, CMake, and an SFML 2.5 development package. Then open a terminal in this repository:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Run the executable from the repository root so it can find `assets/arial.ttf` and save `highscore.txt` in the root. Depending on the SFML package, its runtime DLLs may need to be beside the executable or on PATH.

## Build (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake libsfml-dev
mkdir -p assets
# Copy an Arial-compatible font to assets/arial.ttf
cmake -S . -B build
cmake --build build -j
./build/CarDodgingGame
```

## Controls
- Menu: `1`, `2`, `3` select difficulty; `Enter` starts; `I` opens instructions.
- Playing: `Left` and `Right` change lanes; `P` pauses.
- Instructions: `Esc` returns to menu.
- Paused: `P` resumes; `M` returns to menu.
- Game Over: `R` restarts; `M` returns to menu.

## Project structure
```text
Car_Dodging_Game_Group5/
├── src/main.cpp
├── assets/                 # Put arial.ttf here
├── CMakeLists.txt
├── .gitignore
└── README.md
```

## Important implementation note
This is a complete, playable first version based on the submitted proposal. It draws cars and the road with SFML shapes rather than image assets. The proposal says “other obstacles” as well as traffic; this version models incoming cars as the obstacles. Add extra obstacle shapes only if your instructor expects non-vehicle hazards.

## Academic integrity / learning
All three members should compile, run, test, and understand the source. Be ready to explain the game states, structure, vector, random lane selection, frame-time movement, collision check, and file handling. Do not claim features you have not personally tested.
