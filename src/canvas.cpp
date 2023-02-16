#include "canvas.h"
#include <cassert>
#include <cmath>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace agl;

/**
 * This implements the corresponding methods
 * to rasterize lines and triangles.
*/

Canvas::Canvas(int w, int h) : _canvas(w, h)
{
}

Canvas::~Canvas()
{
}

void Canvas::save(const std::string& filename)
{
   _canvas.save(filename);
}

void Canvas::begin(PrimitiveType type)
{
   this->currentType= type;
}

void Canvas::end()
{
  int n= this->myPoints.size();
  switch (this->currentType) {
    case UNDEFINED:
      std::cout << "Should not be drawing in UNDEFINED mode" << std::endl;
      break;
    case LINES:
      if (n % 2 != 0) {
        std::cout << "(NO DRAW) There are an odd number of vertices declared" << std::endl;
        this->myPoints.clear();
        break;
      }

      for (int i= 0; i < n; i+=2) {
        Point p1= this->myPoints[i];
        Point p2= this->myPoints[i+1];
        this->drawLine(p1, p2);
      }
      break; 
    case TRIANGLES:
      if (n % 3 != 0) {
        std::cout << "(NO DRAW) There are not a multiple of three vertices declared." << std::endl;
        this->myPoints.clear();
        break;
      }

      for (int i= 0; i < n; i+=3) {
        Point p0= this->myPoints[i];
        Point p1= this->myPoints[i+1];
        Point p2= this->myPoints[i+2];
        this->drawTriangle(p0, p1, p2);
      }
      break;
  }
  this->currentType= UNDEFINED;
  this->myPoints.clear();
}

// drawLine helper function for when |H| > |W|
void Canvas::_drawLineHigh(const Point& p1, const Point& p2) 
{
   int x= p1.x;
   int W= p2.x - p1.x;
   int H= p2.y - p1.y;
   int dx= 1;
   // make sure we go in the right y-direction
   if (W < 0) {
    dx= -1;
    W= -W;
   }
   int F= 2*W - H;
   for (int y= p1.y; y <= p2.y; y++) {
      Pixel newColor= Canvas::interpolateColor(p1.color, p2.color, (float) (y-p1.y)/p2.y);
      this->_canvas.replaceColor(x, y, newColor);
      if (F > 0) {
        x+= dx;
        F+= 2*(W - H);
      } else {
        F+= 2*W;
      }
   }
}

// drawLine helper function for when |W| > |H|
void Canvas::_drawLineLow(const Point& p1, const Point& p2) 
{
   int y= p1.y;
   int W= p2.x - p1.x;
   int H= p2.y - p1.y;

   int dy= 1;
   // make sure we go in the right y-direction
   if (H < 0) {
      dy= -1;
      H= -H;
   }
   int F= 2*H - W;
   for (int x= p1.x; x <= p2.x; x++) {
      // interpolates color based on x ratio to the end 
      Pixel newColor= Canvas::interpolateColor(p1.color, p2.color, (float) (x - p1.x)/p2.x);
      this->_canvas.replaceColor(x, y, newColor);
      if (F > 0) {
        y+= dy;
        F+= 2*(H-W);
      } else {
        F+= 2*H;
      }
   }
}

void Canvas::drawLine(const Point& p1, const Point& p2) {
   int W= p2.x - p1.x;
   int H= p2.y - p1.y;

   if (std::abs(H) < std::abs(W)) {
      // swap, so we go in the positive x-direction
      if (p1.x > p2.x) this->_drawLineLow(p2, p1);
      else this->_drawLineLow(p1, p2);
   } else {
      // swap, so we go in the positive y-direction
      if (p1.y > p2.y) this->_drawLineHigh(p2, p1);
      else this->_drawLineHigh(p1, p2);
   }
}


void Canvas::vertex(int x, int y)
{
  // clips vertex to image sizes
  this->myPoints.push_back(Point { max(min(x, this->_canvas.width()-1), 0), 
    max(min(y, this->_canvas.height()-1), 0), this->currentColor });
}

void Canvas::color(unsigned char r, unsigned char g, unsigned char b)
{
  this->currentColor.r= r;
  this->currentColor.g= g;
  this->currentColor.b= b;
}

void Canvas::background(unsigned char r, unsigned char g, unsigned char b)
{
   Pixel p {r, g, b};
   for (int i= 0; i < this->_canvas.pixelCount(); i++) {
      this->_canvas.set(i, p);
   }
}

Pixel Canvas::interpolateColor(const Pixel& p1, const Pixel& p2, float alpha) 
{
  assert(0.0f <= alpha && alpha <= 1.0f);
  Pixel result;
  result.r= p1.r * (1 - alpha) + p2.r * alpha;
  result.g= p1.g * (1 - alpha) + p2.g * alpha;
  result.b= p1.b * (1 - alpha) + p2.b * alpha;

  return result;
}

float Canvas::implicitLineFn(float linePt1_x, float linePt1_y, 
  float linePt2_x, float linePt2_y,
  float otherPt_x, float otherPt_y) 
{
  int H= linePt2_y - linePt1_y;
  int W= linePt2_x - linePt1_x;
  return H * (otherPt_x - linePt1_x) - W * (otherPt_y - linePt1_y);
}

void Canvas::findBoundingBox(const Point& p0, const Point& p1, const Point& p2,
  int& x_min, int& y_min, int& x_max, int& y_max) 
{
  x_min= std::min(std::min(p0.x, p1.x), p2.x);
  x_max= std::max(std::max(p0.x, p1.x), p2.x);
  y_min= std::min(std::min(p0.y, p1.y), p2.y);
  y_max= std::max(std::max(p0.y, p1.y), p2.y);
}

/**
 * Since the canvas goes from top-left to bottom-right,
 * we put the left-most point as p0, then we choose from
 * the remaining two points with the higher y-value
 * as p1 to get the CCW orientation of points. 
 *
*/
void Canvas::rearrangeCCW(Point& p0, Point& p1, Point& p2) {
  if (p1.x < p0.x && p1.x < p2.x) { // p1 is left-most
    std::swap(p1, p0);
  } else if (p2.x < p0.x && p2.x < p1.x) { // p2 is left-most
    std::swap(p2, p0);
  }

  // See which has the bigger y-value and swap if its p2
  if (p2.y > p1.y) std::swap(p1, p2);
}

void Canvas::drawTriangle(Point& p0, Point& p1, Point& p2) {
  int x_min, y_min, x_max, y_max;

  Canvas::findBoundingBox(p0, p1, p2, x_min, y_min, x_max, y_max);

  Canvas::rearrangeCCW(p0, p1, p2);

  float f_alpha= this->implicitLineFn(p1.x, p1.y, p2.x, p2.y, p0.x, p0.y);
  float f_beta= this->implicitLineFn(p2.x, p2.y, p0.x, p0.y, p1.x, p1.y);
  float f_gamma= this->implicitLineFn(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);

  // point to determine if an edge is part of the respective triangle
  float offPoint_x= -1.1f;
  float offPoint_y= -1.2f; 

  for (int y= y_min; y <= y_max; y++) {
    for (int x= x_min; x <= x_max; x++) {
      float alpha= this->implicitLineFn(p1.x, p1.y, p2.x, p2.y, (float) x, (float) y) / f_alpha;
      float beta= this->implicitLineFn(p2.x, p2.y, p0.x, p0.y, (float) x, (float) y) / f_beta;
      float gamma= this->implicitLineFn(p0.x, p0.y, p1.x, p1.y, (float) x, (float) y) / f_gamma;
      if (alpha >= 0 && beta >= 0 && gamma >= 0) {
        // this either checks if the point is inside the triangle or on an edge
        // if it is an edge, we check it with the offscreen point to see if the
        // edge should be part of this triangle
        if ((alpha > 0 || f_alpha * this->implicitLineFn(p1.x, p1.y, p2.x, p2.y, offPoint_x, offPoint_y) > 0)
        && (beta > 0 || f_beta * this->implicitLineFn(p2.x, p2.y, p0.x, p0.y, offPoint_x, offPoint_y) > 0)
        && (gamma > 0 || f_gamma * this->implicitLineFn(p0.x, p0.y, p1.x, p1.y, offPoint_x, offPoint_y) > 0))
        {
          Pixel newColor= Pixel{0, 0, 0};
          newColor.r= alpha * p0.color.r + beta * p1.color.r + gamma * p2.color.r;
          newColor.g= alpha * p0.color.g + beta * p1.color.g + gamma * p2.color.g;
          newColor.b= alpha * p0.color.b + beta * p1.color.b + gamma * p2.color.b;

          this->_canvas.replaceColor(x, y, newColor);
        }
      }

    }
  } 


}