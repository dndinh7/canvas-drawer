#include <iostream>
#include "canvas.h"

using namespace agl;
using namespace std;

void test_line(Canvas& drawer, int ax, int ay, int bx, int by, const std::string& savename)
{
   drawer.background(0, 0, 0);
   drawer.begin(LINES);
   drawer.vertex(ax, ay);
   drawer.vertex(bx, by);
   drawer.end();
   drawer.save(savename);
}

int main(int argc, char** argv)
{
   Canvas drawer(100, 100);

   drawer.color(255, 255, 255);
   test_line(drawer, 0, 50, 100, 50, "horizontal-line.png");
   test_line(drawer, 50, 0, 50, 100, "vertical-line.png");
   test_line(drawer, 0, 0, 100, 100, "diagonal-line-1.png"); // slope = 1
   test_line(drawer, 25, 10, 75, 25,  "h-lessthan-w-line-1.png"); // slope H < W
   test_line(drawer, 25, 25, 75, 75,  "w-lessthan-h-line-1.png"); // slope W < H
   test_line(drawer, 0, 100, 100, 0, "diagonal-line-2.png"); // slope H < W
   test_line(drawer, 25, 90, 75, 75,  "h-lessthan-w-line-2.png"); // slope H < W
   test_line(drawer, 25, 90, 75, 25,  "w-lessthan-h-line-2.png"); // slope W < H

   
   // test line interpolation
   drawer.background(0, 0, 0);
   drawer.begin(LINES);
   drawer.color(255, 0, 255);
   drawer.vertex(0, 0);
   drawer.color(0, 255, 255);
   drawer.vertex(100, 100);
   drawer.end();
   drawer.save("line-color-interpolation.png");


   // Draw primitives with a given type (either LINES or TRIANGLES)
   // For example, the following draws a red line followed by a green line
   drawer.background(0, 0, 0);
   drawer.begin(LINES);
   drawer.color(255,0,0);
   drawer.vertex(0,0);
   drawer.vertex(100,0);
   drawer.color(0,255,0);
   drawer.vertex(0, 0);
   drawer.vertex(0,100);
   drawer.end();
   drawer.save("two-lines.png");


   // draws a box with two diagonal lines through it
   drawer.background(0,0,0);
   drawer.begin(LINES);
   drawer.color(255, 0, 0);
   drawer.vertex(0, 0);
   drawer.vertex(0, 100);
   drawer.color(0, 255, 0);
   drawer.vertex(0,100);
   drawer.vertex(100,100);
   drawer.color(0, 0, 255);
   drawer.vertex(100, 0);
   drawer.vertex(100, 100);
   drawer.color(255, 255, 0);
   drawer.vertex(0, 0);
   drawer.vertex(100, 0);
   drawer.color(255, 0, 255);
   drawer.vertex(100, 0);
   drawer.vertex(0, 100);
   drawer.color(0, 255, 255);
   drawer.vertex(0, 0);
   drawer.vertex(100, 100);
   drawer.end();
   drawer.save("line-boxes.png");
   
   // test triangle with interpolation
   drawer.background(0, 0, 0);
   drawer.begin(TRIANGLES);
   drawer.color(255, 0, 255);
   drawer.vertex(10, 0);
   drawer.color(0, 255, 255);
   drawer.vertex(90, 50);
   drawer.color(255, 255, 0);
   drawer.vertex(10, 90);
   drawer.end();
   drawer.save("triangle.png");


   // test triangle with interpolation
   drawer.background(0, 0, 0);
   drawer.begin(TRIANGLES);
   drawer.color(255, 0, 255);
   drawer.vertex(10, 10);
   drawer.vertex(10, 90);
   drawer.vertex(90, 90);


   drawer.color(255, 255, 0);
   drawer.vertex(90, 90);
   drawer.vertex(90, 10);
   drawer.vertex(10, 10);
   drawer.end();
   drawer.save("quad.png");
   

   // test a quad with interpolation
   drawer.background(0, 0, 0);
   drawer.begin(TRIANGLES);
   drawer.color(0, 0, 0);
   drawer.vertex(5, 5);
   drawer.color(255, 255, 255);
   drawer.vertex(95, 95);
   drawer.color(255, 0, 0);
   drawer.vertex(95, 5);
   
   drawer.vertex(5, 95);
   drawer.color(0, 0, 0);
   drawer.vertex(95, 95);
   drawer.color(255, 255, 255);
   drawer.vertex(5, 5);
   drawer.end();
   drawer.save("quad-interpolate.png");

   // test vertex outside of image size
   drawer.background(255, 255, 255);
   drawer.begin(LINES);
   drawer.color(0, 0, 0);
   drawer.vertex(-35, 50);
   drawer.vertex(50, 186);
   
   drawer.vertex(160, -100);
   drawer.vertex(-100, 400);
   drawer.end();
   drawer.save("lines-out-of-bounds.png");

   // draw circle
   drawer.background(255, 255, 255);
   drawer.begin(CIRCLES);
   drawer.color(0, 0, 0);
   drawer.radius(25);
   drawer.vertex(50, 50);
   drawer.end();
   drawer.save("circle.png");

   // draw circle out of bounds
   drawer.background(255, 255, 255);
   drawer.begin(CIRCLES);
   drawer.color(255, 0, 0);
   drawer.radius(50);
   drawer.vertex(75, 75);
   drawer.end();
   drawer.save("circle-out-of-bounds.png");

   // draw 8 petal rose
   drawer.background(255, 255, 255);
   drawer.begin(ROSES);
   drawer.color(0,0,0);
   drawer.radius(25);
   drawer.petals(4);
   drawer.vertex(50, 50);
   drawer.end();
   drawer.save("8-petal.png");

   // draw a 5 petal rose
   drawer.background(255, 255, 255);
   drawer.begin(ROSES);
   drawer.color(0,0,0);
   drawer.radius(25);
   drawer.petals(5);
   drawer.vertex(50, 50);
   drawer.end();
   drawer.save("5-petal.png");


   Canvas canvas(1000, 1000);

   // draw a curved line in the flow field
   canvas.background(255, 255, 255);
   canvas.begin(FLOW);
   canvas.color(0, 0, 0);
   // this is just a simple arbitrary test of a curved flow field
   for (int i= 0; i < 1000; i++) {
      canvas.vertex(i*24%1000, i*76%1000);
      canvas.vertex(i*55%1000, i*432%1000);
   }
   canvas.end();
   canvas.save("flow-test.png");

   // testing flow with additive blending
   canvas.background(0, 0, 0);
   canvas.begin(FLOW, ADD);
   canvas.color(50,30,10);
   for (int i= 0; i < 1000; i++) {
      canvas.vertex(i*24%1000, i*76%1000);
      canvas.vertex(i*55%1000, i*432%1000);
      canvas.vertex(i*99%1000, i*66%1000);
   }
   canvas.end();
   canvas.save("flow-add-blend.png");

   // Reference: https://www.color-hex.com/color-palette/1022563
   std::vector<Pixel> palette;
   palette.push_back(Pixel{0x2C, 0x2C, 0x54});
   palette.push_back(Pixel{0xAC, 0xC3, 0xA6});
   palette.push_back(Pixel{0xF5, 0xD6, 0xBA});
   palette.push_back(Pixel{0xF4, 0x9D, 0x6E});
   palette.push_back(Pixel{0xDA, 0x62, 0x7D});

   // Creates an arbitrary polygon shape using circles
   canvas.background(255, 255, 255);
   canvas.begin(POLYGON);
   canvas.vertex(100, 100);
   canvas.vertex(150, 123);
   canvas.vertex(300, 700);
   canvas.vertex(650, 203);
   canvas.vertex(321, 800);
   canvas.vertex(789, 900);
   canvas.vertex(43, 999);
   canvas.palette(palette);
   canvas.end();
   canvas.save("pack-circle-polygon.png");

   // Packs the entire screen with circles
   canvas.background(0, 0, 0);
   canvas.begin(POLYGON);
   canvas.vertex(0, 0);
   canvas.vertex(1000, 0);
   canvas.vertex(1000, 1000);
   canvas.vertex(0, 1000);
   canvas.palette(palette);
   canvas.end();
   canvas.save("pack-entire-screen.png");



   return 0;
}
