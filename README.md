# canvas-drawer

Implements a simple drawing api


## How to build

*Windows*

Open git bash to the directory containing this repository.

```
canvas-drawer $ mkdir build
canvas-drawer $ cd build
canvas-drawer/build $ cmake -G "Visual Studio 16 2019" ..
canvas-drawer/build $ start Draw-2D.sln
```

Your solution file should contain two projects: `pixmap_art` and `pixmap_test`.
To run from the git bash command shell, 

```
canvas-drawer/build $ ../bin/Debug/draw_test
canvas-drawer/build $ ../bin/Debug/draw_art
```

*macOS*

Open terminal to the directory containing this repository.

```
canvas-drawer $ mkdir build
canvas-drawer $ cd build
canvas-drawer/build $ cmake ..
canvas-drawer/build $ make
```

To run each program from build, you would type

```
canvas-drawer/build $ ../bin/draw_test
canvas-drawer/build $ ../bin/draw_art
```

## Supported primitives

This program supports drawing lines, triangles, circles, roses with n (if odd) or 2n (if even) number of petals, flow field curves using perlin noise, additive blending, and alpha blending.

- Draw lines by specifying LINES type and specifying two points
- Draw triangles by specifying TRIANGLES type and specifying three points
- Draw circles by specifiying CIRCLES type and specifying a point and radius
- Draw roses by specifying ROSES type and specifying a point, number of petals, and radius
- Draw a flow field by specifying FLOW and specifying vertices
- Fill a polygon with circles by specifying POLYGON and specifying vertices and a palette
- OPTIONAL: specify blending type when also beginning drawing

## Results

Circle:

![circle](https://user-images.githubusercontent.com/72237791/221095216-8dfab288-9a50-4650-be0f-967a3b6a40db.png)

![circle-out-of-bounds](https://user-images.githubusercontent.com/72237791/221095304-922fc897-2813-4025-aec7-b7df94732cab.png)


Roses:

![roses](https://user-images.githubusercontent.com/72237791/221096656-6835b5b1-87cc-41a1-9480-aed6c7438737.png)



Flow (Perlin Noise):

Basic random flow:

![flow-test](https://user-images.githubusercontent.com/72237791/221095804-9cf37876-c641-4f56-af43-3612129f37c1.png)

Blending a bit of yellow:

![flow-add-blend](https://user-images.githubusercontent.com/72237791/221095842-3e858e73-96fb-4eef-a31e-2643c814aa09.png)


Showing the first flow field using replace

![flow-add](https://user-images.githubusercontent.com/72237791/221095339-d3cb86b3-0554-4e21-b6af-0c63ca9b5fd8.png)

Blending the color blue

![flow-blend-1](https://user-images.githubusercontent.com/72237791/221095357-e669e4af-9d68-468b-ad7d-e88e6c3ed512.png)

Blending the color red

![flow-blend-2](https://user-images.githubusercontent.com/72237791/221095365-53ec50bc-ed05-44ab-8e63-dae346a3f9e8.png)

--- NEW ---

Red base:

![flow-add-red](https://user-images.githubusercontent.com/72237791/221095599-d3dddd9f-cf78-40c8-af47-a9d027d24936.png)

Green addition:

![flow-add-green](https://user-images.githubusercontent.com/72237791/221095620-95eff276-ae63-4a6c-aec7-f9c40b1e9427.png)

Blue Addition:

![flow-add-blue](https://user-images.githubusercontent.com/72237791/221095640-d252f65c-3009-41be-bd68-bd022c52d750.png)

Less stepLength for the curves:

![flow-pal-0](https://user-images.githubusercontent.com/72237791/221095760-82aa60af-4dc5-4992-a015-83d6472b3623.png)



Polygon:

Using the circle packing algorithm

![circle-fill](https://user-images.githubusercontent.com/72237791/221095485-98b8fac6-b8e8-468a-95b9-e251e5e1cf94.png)

![circle-triangle](https://user-images.githubusercontent.com/72237791/221095517-65d82bec-dccf-4923-b7cd-83d2f8766e0e.png)

![circle-triangle](https://user-images.githubusercontent.com/72237791/221104743-443eb73d-3162-4d4a-bca9-c96d2a84460e.png)

![circle-pentagon](https://user-images.githubusercontent.com/72237791/221104768-922c11b0-2c29-4a34-80ea-e27633bc8e04.png)

![circle-random-9-vertex](https://user-images.githubusercontent.com/72237791/221104806-426bc895-cd08-45f1-a496-a3a217f7238c.png)

![pack-circle-polygon](https://user-images.githubusercontent.com/72237791/221104830-3a0818e2-b282-44a6-87bf-1e16710a88ed.png)











