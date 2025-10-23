# RunningTests

Ovaj projekat koristi **Catch2** za testove i **OpenCppCoverage** za pokrivenost koda.  
Originalni projekat se nalazi kao git submodul u `external/Minesweeper-Game`.

---

## 1) Build (Windows, Debug)

> *Ako `cmake` nije u PATH-u, koristimo punu putanju iz Visual Studio instalacije.*

```powershell
# iz root foldera repozitorijuma
Set-Location "C:\Users\Milan Manojlovic\Minesweeper-Game"

# Configure + Generate
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" `
  -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug

# Build
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" `
  --build build --config Debug

