/*-----------------------------------------------
 * Author: David Dinh
 * Date: 16 February 2023
 * Description: This program allows the user
 * to create their own canvas and currently
 * draw lines and triangles.
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
      int lineWidth;
      int r;
   };

   enum PrimitiveType {UNDEFINED, LINES, TRIANGLES, CIRCLES};

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

      // Specify a circle's radius
      void radius(int r);

      // Specify a width
      void width(int w);

      // Specify a color. Color components are in range [0,255]
      void color(unsigned char r, unsigned char g, unsigned char b);

      // Fill the canvas with the given background color
      void background(unsigned char r, unsigned char g, unsigned char b);

      // Bresenham's line algorithm
      void drawLine(const Point& p1, const Point& p2);

      // Draws a triangle from three points, this might
      // change the parameters (which will be cleared anyway if drawn)
      void drawTriangle(Point& p0, Point& p1, Point& p2);

      // Interpolates pixel colors with a given alpha
      static Pixel interpolateColor(const Pixel& p1, const Pixel& p2, float alpha);

      /* Computes if the otherPt is on the same line as the two points or not.
       * Returns a positive number if the otherPt is above the line
       * and a negative number if the otherPt is below the line.
       * I chose to do it in this style, so it can take offscreen floating points
      */
      static float implicitLineFn(float linePt1_x, float linePt1_y, 
         float linePt2_x, float linePt2_y, 
         float otherPt_x, float otherPt_y);

      /**
       * Finds the box that bounds the vector of given points.
       * The values will be returned in the passed references.
       */
      static void findBoundingBox(const Point& p0, const Point& p1, const Point& p2,
        int& x_min, int& y_min, int& x_max, int& y_max);

      /**
       * This method rearranges the points, so that
       * p0 -> p1 -> p2 goes in CCW orientation.
       * 
      */
      static void rearrangeCCW(Point& p0, Point& p1, Point& p2);

      /**
       * Draws a circle at point p (this point object should be CirclePoint)
      */
      void drawCircle(const Point& p);


   private:
      // Helper functions for drawLine, so that they can 
      // use _canvas without passing it as a parameter
      void _drawLineLow(const Point& p1, const Point& p2);
      void _drawLineHigh(const Point& p1, const Point& p2);

      Image _canvas;
      PrimitiveType currentType;
      Pixel currentColor;
      std::vector<Point> myPoints;
      int currentRadius= 1;
      int currentLineWidth= 1;
   };
}

#endif


