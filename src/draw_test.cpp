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
   Canvas drawer(101, 101);

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
   

   

   return 0;
}
