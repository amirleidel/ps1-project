# Quick Setup
## SDL2 + OpenGL 3.3 Project Setup (Windows, Standalone MinGW-w64)

Minimal C++ environment for building **SDL2 + OpenGL 3.3** projects using **standalone MinGW-w64**

---

### Prerequisites

1. **Install MinGW-w64**  
   - Use the official [MinGW-w64 builds from winlibs.com](https://winlibs.com/).  
   - Choose **UCRT64** architecture, e.g. `x86_64-posix-seh`.  
   - Add its `/bin` folder to your **PATH** environment variable.  
     Example:
     ```
     C:\mingw64\bin
     ```

2. **Install SDL2 Development Libraries**
   - Download **SDL2-devel-2.x.x-mingw.zip** from  
     [https://github.com/libsdl-org/SDL/releases](https://github.com/libsdl-org/SDL/releases)
   - Extract it to:
     ```
     C:\SDL2\
     ```
   - After extraction, you should have:
     ```
     C:\SDL2\x86_64-w64-mingw32\include\SDL2\
     C:\SDL2\x86_64-w64-mingw32\lib\
     ```

3. **Verify the Compiler**
   ```bash
   g++ --version
   ```

**Example Project Structure**

```
project/
│
├─ src/
│   └─ main.cpp
│
├─ include/
│
├─ build/
│
├─ readme.md
│
└─ CMakeLists.txt

```

4. **Download GLEW (or use GLAD if you prefer single-file loader)**
    - Download the GLEW binaries (e.g. glew-2.1.0 or similar) and extract to:
    ```
    C:\GLEW\glew-2.1.0
    ```
    - Confirm `include\GL\glew.h`, `lib\Release\x64\glew32.lib`, `bin\Release\x64\glew32.dll` exist. 
     GLEW homepage / downloads: 
     [https://glew.sourceforge.net/](https://glew.sourceforge.net/)
     
5. **If you want to use CMD/PowerShell outside MSYS2 add MinGW bin to PATH**
    - If you want `gcc` / `mingw32-make` from regular CMD, add the MinGW64 bin to PATH (e.g. `C:\msys64\mingw64\bin`) via Edit environment variables. After adding, close/reopen terminals.

6. **Build (from MINGW64 shell — recommended)**
    - From your project root:
    ```
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    mingw32-make
    ```
7. **Copy the runtime DLLs next to your `.exe`**
    - After build you must have `SDL2.dll` and `glew32.dll` next to the executable. You can copy manually:
    ```
    copy "C:\SDL2\x86_64-w64-mingw32\bin\SDL2.dll" build\
    copy "C:\GLEW\glew-2.1.0\bin\Release\x64\glew32.dll" build\
    ```

8. **Run & verify OpenGL version**
    - If your program prints `glGetString(GL_VERSION)` after `glewInit()`, you’ll see your driver’s OpenGL version (e.g. `4.6.0 NVIDIA` ...). If it’s `>= 3.3` you’re set.

    
<a href="https://creativecommons.org">ps1-project</a> © 2025 by <a href="https://creativecommons.org">Amir J. G. Leidel</a> is licensed under <a href="https://creativecommons.org/licenses/by-nc-sa/4.0/">CC BY-NC-SA 4.0</a><img src="https://mirrors.creativecommons.org/presskit/icons/cc.svg" alt="" style="max-width: 1em;max-height:1em;margin-left: .2em;"><img src="https://mirrors.creativecommons.org/presskit/icons/by.svg" alt="" style="max-width: 1em;max-height:1em;margin-left: .2em;"><img src="https://mirrors.creativecommons.org/presskit/icons/nc.svg" alt="" style="max-width: 1em;max-height:1em;margin-left: .2em;"><img src="https://mirrors.creativecommons.org/presskit/icons/sa.svg" alt="" style="max-width: 1em;max-height:1em;margin-left: .2em;">