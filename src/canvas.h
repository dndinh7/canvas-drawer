/*-----------------------------------------------
 * Author:
 * Date:
 * Description:
 ----------------------------------------------*/

#ifndef canvas_H_
#define canvas_H_

#include <string>
#include <vector>
#include "image.h"

namespace agl
{
   struct Point {
      int x;
      int y;
      Pixel color;
   };

   enum PrimitiveType {UNDEFINED, LINES, TRIANGLES};

   class Canvas
   {
   public:
      Canvas(int w, int h);
      virtual ~Canvas();

      // Save to file
      void save(const std::string& filename);

      // Draw primitives with a given type (either LINES or TRIANGLES)
      // For example, the following draws a red line followed by a green line
      // begin(LINES);
      //    color(255,0,0);
      //    vertex(0,0);
      //    vertex(100,0);
      //    color(0,255,0);
      //    vertex(0, 0);
      //    vertex(0,100);
      // end();
      void begin(PrimitiveType type);
      void end();

      // Specify a vertex at raster position (x,y)
      // x corresponds to the column; y to the row
      void vertex(int x, int y);

      // Specify a color. Color components are in range [0,255]
      void color(unsigned char r, unsigned char g, unsigned char b);

      // Fill the canvas with the given background color
      void background(unsigned char r, unsigned char g, unsigned char b);

      // Bresenham's line algorithm
      void drawLine(const Point& p1, const Point& p2);

      // Interpolates pixel colors with a given alpha
      static Pixel interpolateColor(const Pixel& p1, const Pixel& p2, float alpha);

   private:
      // Helper functions for drawLine, so that they can 
      // use _canvas without passing it as a parameter
      void _drawLineLow(const Point& p1, const Point& p2);
      void _drawLineHigh(const Point& p1, const Point& p2);

      Image _canvas;
      PrimitiveType currentType;
      Pixel currentColor;
      std::vector<Point> myPoints;
   };
}

#endif


