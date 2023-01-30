<img src='https://raw.githubusercontent.com/Scarbyte/gupta/main/screenshot.png' width=600>

# gupta
Roguelike game with chess-based movement. Programmed in C using Raylib.
This game was created in 2 weeks for <a href='https://experimentaljams.com'>Experimental Game Jams</a>. The theme was grid.

View the <a href='https://experimentaljams.com/game/?id=44'>game page</a> to view more info on the game itself, or to download it.

## Compilation instructions
- First regardless of OS, clone the <a href='https://github.com/raysan5/raylib'>raylib repository</a>. Then copy the files into a new folder in the project directory named `dependencies`. NOTE: You may have to edit `CMakelists.txt` and change the directory name of raylib depending on what version of raylib you have.

### Windows (Cmake and MinGW)
- Please make sure you're using an up to date version of MinGW (you can use the raylib installer if needed).
- After cloning raylib into the project directory:
- Create a build directory `mkdir build`
- `cd build`
- `cmake .. -G "MinGW Makefiles"`
- `cmake --build .`

### Linux (Cmake and GCC)
- After cloning raylib into the project directory:
- Create a build directory `mkdir build`
- `cd build`
- `cmake ..`
- `cmake --build .`

### After compiling
Then make sure your compiled executable is in a new folder with the `res` folder.
