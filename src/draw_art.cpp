#include <iostream>
#include <cmath>
#include "canvas.h"
#include <cstdlib>
#include <time.h>
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
   int width= 640;
   int height= 380;
   Canvas drawer(width, height);
   
   // additive blending
   drawer.background(0, 0, 0);
   drawer.begin(FLOW, ADD);   
   drawer.color(50,30,10);
   for (int y= 0; y < 380; y+=10) {
      for (int x= 0; x < 640; x+=10) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-add.png");

   drawer.begin(FLOW, ALPHA, 0.15f);
   drawer.color(2, 0, 130);
   for (int y= 0; y < 380; y+=7) {
      for (int x= 0; x < 640; x+=15) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-blend-1.png");

   drawer.begin(FLOW, ALPHA, 0.025f);
   drawer.color(188, 23, 20);
   for (int y= 0; y < 380; y+=13) {
      for (int x= 0; x < 640; x+=7) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-blend-2.png");

   std::vector<Pixel> palette2;
   palette2.push_back(Pixel{0x6F, 0xA8, 0xDC});
   palette2.push_back(Pixel{0x74, 0xB7, 0x57});
   palette2.push_back(Pixel{0xD9, 0xEA, 0xD3});
   palette2.push_back(Pixel{0xA6, 0x4D, 0x79});
   palette2.push_back(Pixel{0xC2, 0x7B, 0xA0});

   // this will test flow on a palette
   drawer.background(255, 255, 255);
   drawer.begin(FLOW, ALPHA, 0.15f);
   drawer.stepLength((int) ceil(width * 0.01f));
   drawer.numSteps((int) ceil(width * 0.01f));
   drawer.color(palette2[0].r, palette2[0].g, palette2[0].b);
   for (int y= 0; y < height; y+=5) {
      for (int x= 0; x < width; x+=5) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-pal-0.png");

   drawer.begin(FLOW, ALPHA, 0.15f);
   drawer.color(palette2[1].r, palette2[1].g, palette2[1].b);
   for (int y= 1; y < height; y+=5) {
      for (int x= 1; x < width; x+=5) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-pal-1.png");

   drawer.begin(FLOW, ALPHA, 0.10f);
   drawer.color(palette2[2].r, palette2[2].g, palette2[2].b);
   for (int y= 2; y < height; y+=5) {
      for (int x= 2; x < width; x+=5) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-pal-2.png");

   drawer.begin(FLOW, ALPHA, 0.05f);
   drawer.color(palette2[3].r, palette2[3].g, palette2[3].b);
   for (int y= 3; y < height; y+=5) {
      for (int x= 3; x < width; x+=5) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-pal-3.png");

   drawer.begin(FLOW, ALPHA, 0.025f);
   drawer.color(palette2[4].r, palette2[4].g, palette2[4].b);
   for (int y= 4; y < height; y+=5) {
      for (int x= 4; x < width; x+=5) {
         drawer.vertex(x, y);
      }
   }
   drawer.end();
   drawer.save("flow-pal-4.png");


   drawer.background(0, 0, 0);
   drawer.begin(FLOW, ADD);
   drawer.color(10, 0, 0);
   srand(time(0));
   for (int i= 0; i < width*height*0.33; i++) {
      drawer.vertex(rand()%width, rand()%height);
   }
   drawer.end();
   drawer.save("flow-add-red.png");


   drawer.begin(FLOW, ADD);
   drawer.color(0, 10, 0);
   srand(time(0));
   for (int i= 0; i < width*height*0.33; i++) {
      drawer.vertex(rand()%width, rand()%height);
   }
   drawer.end();
   drawer.save("flow-add-green.png");


   drawer.begin(FLOW, ADD);
   drawer.color(0, 0, 10);
   srand(time(0));
   for (int i= 0; i < width*height*0.33; i++) {
      drawer.vertex(rand()%width, rand()%height);
   }
   drawer.end();
   drawer.save("flow-add-blue.png");

   width= 1000;
   height= 1000;
   Canvas canvas(1000, 1000);
   // Reference: https://www.color-hex.com/color-palette/1022563
   std::vector<Pixel> palette;
   palette.push_back(Pixel{0x2C, 0x2C, 0x54});
   palette.push_back(Pixel{0xAC, 0xC3, 0xA6});
   palette.push_back(Pixel{0xF5, 0xD6, 0xBA});
   palette.push_back(Pixel{0xF4, 0x9D, 0x6E});
   palette.push_back(Pixel{0xDA, 0x62, 0x7D});

   canvas.background(255, 255, 255);
   canvas.begin(POLYGON);
   canvas.vertex(0, 0);
   canvas.vertex(width, 0);
   canvas.vertex(width, height);
   canvas.vertex(0, height);
   canvas.palette(palette);
   canvas.end();
   canvas.save("circle-screen.png");

   canvas.background(255, 255, 255);
   canvas.begin(POLYGON);
   canvas.vertex(250, 250);
   canvas.vertex(250, 750);
   canvas.vertex(750, 750);
   canvas.palette(palette);
   canvas.end();
   canvas.save("circle-triangle.png");

   canvas.background(255, 255, 255);
   canvas.begin(POLYGON);

   canvas.vertex(100, 100);
   canvas.vertex(250, 750);
   canvas.vertex(750, 750);
   canvas.vertex(750, 250);
   
   canvas.palette(palette);
   canvas.end();
   canvas.save("circle-pentagon.png");

   canvas.background(0, 0, 0);
   canvas.begin(ROSES);
   canvas.color(255, 255, 255);
   canvas.radius(100);
   canvas.petals(3);
   canvas.vertex(125, 125);
   canvas.petals(2);
   canvas.vertex(458, 125);
   canvas.petals(5);
   canvas.vertex(791, 125);
   canvas.petals(7);
   canvas.vertex(125, 458);
   canvas.petals(4);
   canvas.vertex(458, 458);
   canvas.petals(9);
   canvas.vertex(791, 458);
   canvas.petals(11);
   canvas.vertex(125, 791);
   canvas.petals(6);
   canvas.vertex(458, 791);
   canvas.petals(13);
   canvas.vertex(791, 791);
   canvas.end();
   canvas.save("roses.png");


}

