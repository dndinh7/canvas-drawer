#include "canvas.h"
#include <cassert>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace agl;

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
        std::cout << "There are an odd number of vertices declared" << std::endl;
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
      this->_canvas.replaceColor(x, y, Canvas::interpolateColor(p1.color, p2.color, 0.5f));
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
      this->_canvas.replaceColor(x, y, Canvas::interpolateColor(p1.color, p2.color, 0.5f));
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
  this->myPoints.push_back(Point { x, y, this->currentColor });
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

Pixel Canvas::interpolateColor(const Pixel& p1, const Pixel& p2, float alpha) {
  assert(0.0f <= alpha && alpha <= 1.0f);
  Pixel result;
  result.r= p1.r * (1 - alpha) + p2.r * alpha;
  result.g= p1.g * (1 - alpha) + p2.g * alpha;
  result.b= p1.b * (1 - alpha) + p2.b * alpha;

  return result;
}