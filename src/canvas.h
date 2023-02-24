/*-----------------------------------------------
 * Author: David Dinh
 * Date: 23 February 2023
 * Description: This program allows the user
 * to create their own canvas and currently
 * draw lines, triangles, roses of n (if odd) or 2n petals,
 * circles, flow field using perlin noise, and additive and
 * alpha blending.
 ----------------------------------------------*/

#ifndef canvas_H_
#define canvas_H_

#include <string>
#include <vector>
#include "image.h"

namespace agl
{
  enum BlendType { ALPHA, ADD, REPLACE };

  enum PrimitiveType {UNDEFINED, LINES, TRIANGLES, CIRCLES, ROSES, FLOW, POLYGON};

  struct Point {
    int x;
    int y;
    Pixel color;
  };

  struct FlowField {
    std::vector<float> field;
    int resolution;
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int nRows;
    int nCols;
    int numSteps;
    int stepLength;
  };

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
    void begin(PrimitiveType primitiveType, BlendType blendType= REPLACE, float alpha= 0.0f);

    // This will draw the shapes, then clear the vectors, and reset
    // the primitive type back to UNDEFINED and blendType back to REPLACE
    void end();

    // Specify a vertex at raster position (x,y)
    // x corresponds to the column; y to the row
    void vertex(int x, int y);

    // Specify the alpha for alpha blending
    void alpha(float alpha);

    // Specify a circle's radius
    void radius(int r);

    // Specify number of petals
    void petals(int num);

    // Specify a palette
    void palette(std::vector<Pixel> palette);

    // Specify a color. Color components are in range [0,255]
    void color(unsigned char r, unsigned char g, unsigned char b);

    // Fill the canvas with the given background color
    void background(unsigned char r, unsigned char g, unsigned char b);

    // WARNING: the next two methods will change the
    // the curves of the flow field drawing...
    // changing it may cause errors...

    // Specify a flow field's numSteps
    void numSteps(int steps);
    
    // Specify a flow field's stepLength
    void stepLength(int length);

    // Bresenham's line algorithm
    void drawLine(Point& p1, Point& p2);

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
    static void findBoundingBox(std::vector<Point>& points,
      int& x_min, int& y_min, int& x_max, int& y_max);

    /**
     * This method rearranges the points, so that
     * p0 -> p1 -> p2 goes in CCW orientation.
     * 
    */
    static void rearrangeCCW(Point& p0, Point& p1, Point& p2);

    /**
     * Draws a circle at point p
    */
    void drawCircle(const Point& p, int radius);

    /**
     * Fills the polygon with different sized circles
    */
    void packCircles(std::vector<Point>& polygon, std::vector<Pixel>& palette);

    /**
     * Draws a rose at point p
    */
    void drawRose(const Point& p, int radius, int numPetals);

    /**
     * Draws a flow at point p
    */
    void drawFlow(Point& p);

    /**
     * Maps a value in a range [start, end],
     * to a new range [newStart, newEnd]
     */
    static float mapValue(float value, float start, float end, float newStart, float newEnd);

    /**
     * Returns whether the point is in the polygon or not
    */
    static bool polygonContainsPoint(std::vector<Point>& polygon, const Point& testPoint);

    // Returns whether two circles are colliding
    static bool collision(const Point& p1, int r1, const Point& p2, int r2);

  private:
    // Helper functions for drawLine, so that they can 
    // use _canvas without passing it as a parameter
    void _drawLineLow(const Point& p1, const Point& p2);
    void _drawLineHigh(const Point& p1, const Point& p2);

    // This will color the pixel at x and y
    // based on the blend type
    void _colorPixel(int x, int y, const Pixel& p);



    Image _canvas;
    PrimitiveType currentPrimitiveType= UNDEFINED;
    Pixel currentColor;
    std::vector<Point> myPoints;
    std::vector<int> myRadii;     // determines the radius of circle points
    std::vector<int> myNumPetals; // determines number of petals
    std::vector<Pixel> myPalette;   // palette for packing circles
    FlowField flowField;
    BlendType currentBlendType= REPLACE;
    int currentRadius= 1;
    int currentNumPetals= 1; // for rose curve
    float currentAlpha= 0.0f;

  };
}

#endif


