# SDL3 Survival Game 🎮

**A simple survival game made in C++ with the SDL3 library. You control a red square and must avoid a blue enemy square that chases you for as long as possible. Survival time is displayed in real time and the high score is saved.**
- **Controls: arrow keys or WASD to move the player.**
- **Features: basic chasing AI, survival timer with high score, real-time debug text rendered on screen via SDL3 debug tools, minimal dependencies — only SDL3 is required.**
  
**How to install and run:**

Requirements: C++17 compiler (GCC, Clang, or MSVC), CMake 3.16 or higher, SDL3 (not SDL2!) — download and build from source: https://github.com/libsdl-org/SDL

1. Clone the repository
   git clone https://github.com/MZ-DevCode/SDL3_Projects
   cd SDL3_Projects

2. Create a build folder
   mkdir build && cd build

3. Configure CMake
   cmake .. -DCMAKE_BUILD_TYPE=Release
   If SDL3 is in a custom location:
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/SDL3

4. Build the project
   cmake --build . --config Release

5. Run the game
   Windows: .\sdl3-survival-game.exe
   Linux/macOS: ./sdl3-survival-game

Project files: CMakeLists.txt + src/ (main.cpp, game.cpp/h, player.cpp/h, enemy.cpp/h)

Dependencies: only SDL3
