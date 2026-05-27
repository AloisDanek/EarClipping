# C++ Geometry Code Challenge: Ear Clipping Triangulation

Build:

```bash
cmake -S . -B build
cmake --build build
```

Run tests:

```bash
./build/ear_clip --test
```

Run on a polygon CSV:

```bash
./build/ear_clip simple_concave_demo.csv
./build/ear_clip simple_concave_demo.csv --csv triangles.csv
```

CSV format:

```csv
x,y
0,0
1,0
1,1
0,1
```

The polygon may be clockwise or counterclockwise. If the last point repeats the first point, it is accepted and removed internally.

Algorithm notes:

- The polygon is normalized to counterclockwise orientation.
- For each candidate vertex, the code checks whether the vertex is convex and whether any other polygon vertex lies inside the candidate triangle.
- If both tests pass, the candidate triangle is an ear; it is saved and the ear tip is removed.
- The loop continues until one final triangle remains.

Complexity:

- Time: O(n^3) in this simple implementation, because up to O(n) scans may be needed per clipped ear and each candidate test can scan O(n) vertices.
- Space: O(n), excluding the output triangle list.

## SVG visualization

To generate one SVG image per ear-clipping step:

```bash
./ear_clip polygon.csv --svg svg_steps
```

Open `svg_steps/step_000.svg`, `step_001.svg`, etc. in a web browser.

## Source layout

- `GeometryTypes.h/.cpp`: shared point, polygon, triangle, and geometry helper functions.
- `EarClipping.h/.cpp`: ear clipping triangulation.
- `CsvInput.h/.cpp`: polygon CSV reader.
- `CsvOutput.h/.cpp`: console output and triangle CSV writer.
- `SvgOutput.h/.cpp`: optional SVG step visualization writer.
- `main.cpp`: command-line parsing and basic tests.
