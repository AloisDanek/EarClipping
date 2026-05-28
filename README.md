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

- KD-tree setup: O(n log n) expected time to build a balanced spatial index over the original polygon vertices, with O(n) additional space.
- Per ear candidate: the convexity check is O(1). 
    The "point inside candidate triangle" check first queries the KD-tree with the candidate triangle's bounding box, 
    then runs the exact point-in-triangle test only on vertices returned by that range query.
- Typical time: for spatially well-distributed polygons, the KD-tree reduces the number of vertices 
    tested per ear candidate, so candidate validation is closer to O(log n + k), 
    where k is the number of vertices inside the candidate triangle's bounding box.
- Worst-case time: still O(n^3). Ear clipping may examine O(n) candidate vertices for each of O(n) clipped ears,
    and a KD-tree range query can still return O(n) vertices when the bounding box covers most of the polygon 
    or the input is pathologically distributed.
- Space: O(n), excluding the output triangle list. This includes clipping state plus the KD-tree.

## SVG visualization

To generate one SVG image per ear-clipping step:

```bash
./ear_clip polygon.csv --svg svg_steps
```

Open `svg_steps/step_000.svg`, `step_001.svg`, etc. in a web browser.

## Source layout

- `Point.h/.cpp`: point data and point geometry helpers.
- `Triangle.h/.cpp`: triangle data and triangle area helpers.
- `Polygon.h/.cpp`: polygon data and polygon-specific ear helper methods.
- `KDTree.h/.cpp`: spatial index used to reduce candidate vertices during ear validation.
- `GeometryTypes.h/.cpp`: shared geometry constants and indexing helpers.
- `EarClipping.h/.cpp`: ear clipping triangulation.
- `CsvInput.h/.cpp`: polygon CSV reader.
- `CsvOutput.h/.cpp`: console output and triangle CSV writer.
- `SvgOutput.h/.cpp`: optional SVG step visualization writer.
- `main.cpp`: command-line parsing and basic tests.
