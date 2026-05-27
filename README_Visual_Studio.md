# Building in Visual Studio

This project can be built in Visual Studio in either of two ways.

## Option 1: Recommended - Open the CMake project

Visual Studio 2019/2022 supports CMake projects directly.

1. Start Visual Studio.
2. Choose **File > Open > Folder...**.
3. Select the `geometry_ear_clipping` folder.
4. Visual Studio should detect `CMakeLists.txt` automatically.
5. In the top toolbar, select a configuration such as:
   - `x64-Debug`
   - `x64-Release`
6. Choose **Build > Build All**.
7. Run tests by opening **View > Terminal** inside Visual Studio and running:

```bat
out\build\x64-Debug\ear_clip.exe --test
```

Depending on your Visual Studio/CMake configuration, the executable may also be under a path similar to:

```bat
out\build\x64-Debug\Debug\ear_clip.exe
```

Run with a CSV file:

```bat
out\build\x64-Debug\ear_clip.exe simple_concave_poly.csv
```

## Option 2: Use the included Visual Studio solution

The ZIP also includes:

- `GeometryEarClipping.sln`
- `GeometryEarClipping.vcxproj`

Steps:

1. Open `GeometryEarClipping.sln` in Visual Studio 2022.
2. Select `x64` and `Debug` or `Release`.
3. Choose **Build > Build Solution**.
4. The executable will be generated under a folder like:

```bat
x64\Debug\ear_clip.exe
```

Run tests from a Developer Command Prompt or Visual Studio Terminal:

```bat
x64\Debug\ear_clip.exe --test
```

Run on CSV input:

```bat
x64\Debug\ear_clip.exe simple_concave_poly.csv
```

## Notes

- The project uses standard C++17 only.
- No external libraries are required.
- The Visual Studio project sets `/std:c++17`.
- The CMake build is usually cleaner for code challenge submissions because it also builds easily on Linux/macOS.

## Generate SVG graphics showing ear-clipping progress

The program can also write one SVG file per clipping step. SVG files can be opened directly in Chrome, Edge, Firefox, or Visual Studio Code.

Example using the Visual Studio solution build:

```bat
x64\Debug\ear_clip.exe simple_concave_poly.csv --svg svg_steps
```

Example using a full CSV path:

```bat
x64\Debug\ear_clip.exe "C:\Users\Alois\Downloads\simple_concave_poly.csv" --svg "C:\Users\Alois\Downloads\ear_steps"
```

The output folder will contain files like:

```text
step_000.svg
step_001.svg
step_002.svg
step_003.svg
```

Open them in order to see the polygon before clipping, each highlighted ear, and the final triangle.
