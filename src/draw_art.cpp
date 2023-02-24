#include <iostream>
#include "canvas.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
   Canvas drawer(640, 380);
   
   // Reference: https://www.color-hex.com/color-palette/1022563
   std::vector<Pixel> palette;
   palette.push_back(Pixel{0x2C, 0x2C, 0x54});
   palette.push_back(Pixel{0xAC, 0xC3, 0xA6});
   palette.push_back(Pixel{0xF5, 0xD6, 0xBA});
   palette.push_back(Pixel{0xF4, 0x9D, 0x6E});
   palette.push_back(Pixel{0xDA, 0x62, 0x7D});

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
}

