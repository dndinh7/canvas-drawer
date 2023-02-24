#include "canvas.h"
#include <cassert>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <time.h>

using namespace std;
using namespace agl;

/**
 * This implements the corresponding methods
 * to rasterize lines and triangles. It also implements
 * drawing circles, flow fields, rose petals, blending.
 * 
 * References:
 * https://tylerxhobbs.com/essays/2016/a-randomized-approach-to-cicle-packing
 * https://tylerxhobbs.com/essays/2020/flow-fields
 * https://en.wikipedia.org/wiki/Perlin_noise
 * https://tylerxhobbs.com/essays/2016/a-randomized-approach-to-cicle-packing
 * 
*/

// Function to clamp value
int clamp(int value, int low, int hi) 
{
  return std::min(std::max(value, low), hi);
}

// Although the original Perlin algorithm
// uses a permutation array to hold the gradients
// I use the wikipedia's pseudo generation one
void pseudoGradientGenerator(int grid_x, int grid_y, float& grad_x, float& grad_y)
{
  const unsigned w= 8 * sizeof(unsigned);
  const unsigned s= w / 2;
  unsigned a= grid_x, b= grid_y;

  a *= 3284157443; b ^= a << s | a >> (w - s);
  b *= 1911520717; a ^= b << s | b >> (w - s);
  a *= 2048419325;
  float random= a * (M_PI / ~(~0u >> 1));
  grad_x= cos(random);
  grad_y= sin(random);
}

// Linear Interpolation of values
float LERP(float a, float b, float alpha) 
{
  return (b - a) * ((alpha * (alpha * 6.0 - 15.0) + 10.0) * 
    alpha * alpha * alpha) + a;
}

// computes the dot product of the point x, y and gradient point
float dotGradient(int grid_x, int grid_y, float x, float y)
{
  float grad_x, grad_y;
  pseudoGradientGenerator(grid_x, grid_y, grad_x, grad_y);


  // calculate distance vector
  float dx= x - (float) grid_x;
  float dy= y - (float) grid_y;


  return dx*grad_x + dy*grad_y;
}

float perlin(float x, float y)
{
  // get the gradient bounding box
  int x0= (int) floor(x);
  int x1= x0 + 1;
  int y0= (int) floor(y);
  int y1= y0 + 1;

  // alpha will be distance
  float sx= x - (float) x0;
  float sy= y - (float) y0;

  float n0= dotGradient(x0, y0, x, y);
  float n1= dotGradient(x1, y0, x, y);
  float aux_x1= LERP(n0, n1, sx);

  n0= dotGradient(x0, y1, x, y);
  n1= dotGradient(x1, y1, x, y);
  float aux_x2= LERP(n0, n1, sx);

  float value= LERP(aux_x1, aux_x2, sy); // [-1, 1]
  // want to return a value between 0 and 1
  return (value+1)/2.0f;

}

float Canvas::mapValue(float value, float start, float end, float newStart, float newEnd) {
  // using ratios, we will map the value
  float ratio= value - start / (end - start);
  float newValue= ratio * (newEnd - newStart) + newStart;
  return newValue;
}

void Canvas::_colorPixel(int x, int y, const Pixel& p) {
  switch (this->currentBlendType) {
    case REPLACE:
      this->_canvas.replaceColor(x, y, p);
      break;
    case ADD:
      this->_canvas.addColor(x, y, p);
      break;
    case ALPHA:
      this->_canvas.alphaColor(x, y, p, this->currentAlpha);
      break;
  }
}

Canvas::Canvas(int w, int h) : _canvas(w, h)
{
  // Allow for a 50% buffer if lines were to loop back
  this->flowField.min_x= (int) (w * -0.5f);
  this->flowField.max_x= (int) (w * 1.5f);
  this->flowField.min_y= (int) (h * -0.5f);
  this->flowField.max_y= (int) (h * 1.5f);


  // this will be the spacing between each angle in the flow field
  this->flowField.resolution= (int) ceil(w * 0.001f);

  // based on the resolution
  this->flowField.nRows= (this->flowField.max_y - this->flowField.min_y)
    / this->flowField.resolution;

  this->flowField.nCols= (this->flowField.max_x - this->flowField.min_x) 
    / this->flowField.resolution;

  this->flowField.field= std::vector<float>(this->flowField.nRows * 
    this->flowField.nCols);


  for (int y= 0; y < this->flowField.nRows; y++) {
    for (int x= 0; x < this->flowField.nCols; x++) {
      float scaled_x= x * 0.005;
      float scaled_y= y * 0.005;
      float angle= perlin(scaled_x, scaled_y);
      angle= mapValue(angle, 0, 1, 0, 2 * M_PI);

      this->flowField.field[y * this->flowField.nCols + x]= angle;
    }
  }

  // this will determine the length of each curve
  this->flowField.numSteps= (int) ceil(this->_canvas.height() * 0.1);
  this->flowField.stepLength= (int) ceil(this->_canvas.width() * 0.01);
}

Canvas::~Canvas()
{
}

void Canvas::save(const std::string& filename)
{
  _canvas.save(filename);
}

void Canvas::begin(PrimitiveType primitiveType, BlendType blendType, float alpha)
{
  // should not be calling begin before ending
  assert(this->currentPrimitiveType == UNDEFINED);
  this->currentPrimitiveType= primitiveType;
  this->currentBlendType= blendType;
  this->currentAlpha= alpha;
}

void Canvas::end()
{
  int n= this->myPoints.size();
  switch (this->currentPrimitiveType) {
    case UNDEFINED:
      std::cout << "Should not be drawing in UNDEFINED mode" << std::endl;
      break;
    case LINES:
      if (n % 2 != 0) {
        std::cout << "(NO DRAW) There are an odd number of vertices declared" << std::endl;
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
        break;
      }

      for (int i= 0; i < n; i+=3) {
        Point p0= this->myPoints[i];
        Point p1= this->myPoints[i+1];
        Point p2= this->myPoints[i+2];
        this->drawTriangle(p0, p1, p2);
      }
      break;
    case CIRCLES:
      if (n != this->myRadii.size()) {
        std::cout << "(NO DRAW) Not a vector of circle points" << std::endl;
        break;
      }
      for (int i= 0; i < n; i++) {
        this->drawCircle(this->myPoints[i], this->myRadii[i]);
      }
      break;
    case ROSES:
      if (n != this->myRadii.size() || n != this->myNumPetals.size()) {
        std::cout << "(NO DRAW) Not a vector of rose points" << std::endl;
        break;
      }

      for (int i= 0; i < n; i++) {
        this->drawRose(this->myPoints[i], this->myRadii[i], this->myNumPetals[i]);
      }
      break;
    case FLOW:
      for (Point p: this->myPoints) {
        this->drawFlow(p);
      }
      break;
    case POLYGON:
      // if less than 2, then we simply have a line, point, or nothing
      if (n < 2) {
        std::cout << "(NO DRAW) Not a valid polygon" << std::endl;
        break;
      }
      if (this->myPalette.size() == 0) {
        std::cout << "(NO DRAW) Please also input a palette of colors" << endl;
        break;
      }
      this->packCircles(this->myPoints, this->myPalette);
      break;
  }
  this->currentPrimitiveType= UNDEFINED;
  this->currentBlendType= REPLACE;
  this->myPoints.clear();
  this->myRadii.clear();
  this->myNumPetals.clear();
  this->myPalette.clear();
}


void Canvas::numSteps(int steps)
{
  if (this->currentPrimitiveType == FLOW) {
    this->flowField.numSteps= steps;
  } else {
    cout << "Cannot change numSteps of flow field when type is not FLOW" << endl;
  }
}

void Canvas::stepLength(int length) 
{
  if (this->currentPrimitiveType == FLOW) {
    this->flowField.stepLength= length;
  } else {
    cout << "Cannot change stepLength of flow field when type is not FLOW" << endl;
  }
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
      // interpolates color   
      float num= sqrt(pow(x - p1.x, 2) + pow(y - p1.y, 2));
      float denom= sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
      // do not want to divide by 0
      float alpha= (denom < 1) ? 0 : num/denom;

      Pixel newColor= Canvas::interpolateColor(p1.color, p2.color, alpha);
      this->_colorPixel(x, y, newColor);
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
      // interpolates color   
      float num= sqrt(pow(x - p1.x, 2) + pow(y - p1.y, 2));
      float denom= sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
      // do not want to divide by 0
      float alpha= (denom < 1) ? 0 : num/denom;

      Pixel newColor= Canvas::interpolateColor(p1.color, p2.color, alpha);
      this->_colorPixel(x, y, newColor);
      if (F > 0) {
        y+= dy;
        F+= 2*(H-W);
      } else {
        F+= 2*H;
      }
   }
}

void Canvas::drawLine(Point& p1, Point& p2) {
  int W= p2.x - p1.x;
  int H= p2.y - p1.y;

  int canvasWidth= this->_canvas.width()-1;
  int canvasHeight= this->_canvas.height()-1;

  // if a points is off screen, then we don't want to draw the line
  if (((p1.x < 0 || p1.x > canvasWidth) || (p1.y < 0 || p1.y > canvasHeight)) ||
      ((p2.x < 0 || p2.x > canvasWidth) || (p2.y < 0 || p2.y > canvasHeight))) {
    return;
  }

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
  Point p;
  p.x= clamp(x, 0, this->_canvas.width()-1);
  p.y= clamp(y, 0, this->_canvas.height()-1);
  p.color= this->currentColor;
  
  this->myPoints.push_back(p);
  
  if (this->currentPrimitiveType == CIRCLES || this->currentPrimitiveType == ROSES) {
    this->myRadii.push_back(this->currentRadius);
  }

  if (this->currentPrimitiveType == ROSES) {
    this->myNumPetals.push_back(this->currentNumPetals);
  }
}

void Canvas::palette(std::vector<Pixel> palette)
{
  if (this->currentPrimitiveType == POLYGON) {
    this->myPalette= palette;
  } else {
    std::cout << "Cannot set palette when the type is not POLYGON" << std::endl;
  }
}

void Canvas::color(unsigned char r, unsigned char g, unsigned char b)
{
  this->currentColor.r= r;
  this->currentColor.g= g;
  this->currentColor.b= b;
}

void Canvas::radius(int r) 
{
  if (this->currentPrimitiveType == CIRCLES || this->currentPrimitiveType == ROSES) {
    this->currentRadius= r;
  } else {
    cout << "Cannot set radius when the type is not CIRCLES or ROSES" << endl;
  }
}

void Canvas::petals(int num)
{
  if (this->currentPrimitiveType == ROSES) {
    this->currentNumPetals= num;
  } else {
    cout << "Cannot set petals when the type is not ROSES" << endl;
  }
}

void Canvas::alpha(float alpha)
{
  if (this->currentBlendType == ALPHA) {
    if (alpha < 0.0f || alpha > 1.0f) {
      cout << "alpha needs to be between 0 and 1" << endl;
    } else {
      this->currentAlpha= alpha;
    }
  } else {
    cout << "Cannot set alpha when the blend type is not ALPHA" << endl;
  }
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

void Canvas::findBoundingBox(std::vector<Point>& points,
  int& x_min, int& y_min, int& x_max, int& y_max) 
{
  int n= points.size();
  if (n == 0) {
    cout << "No points, there is no bounding box" << endl;
    return;
  }
  x_min= points[0].x;
  x_max= points[0].x;
  y_min= points[0].y;
  y_max= points[0].y;

  for (int i= 1; i < n; i++) {
    Point p= points[i];
    if (p.x < x_min) x_min= p.x;
    if (p.x > x_max) x_max= p.x;
    if (p.y < y_min) y_min= p.y;
    if (p.y > y_max) y_max= p.y;
  }
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
  std::vector<Point> points;
  points.push_back(p0);
  points.push_back(p1);
  points.push_back(p2);
  Canvas::findBoundingBox(points, x_min, y_min, x_max, y_max);

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

          this->_colorPixel(x, y, newColor);
        }
      }

    }
  } 
}
/* this only gets the outline
void Canvas::drawCircle(const Point& p, int radius)
{
  const int NUM_POINTS=100;
  float deltaTheta= 2*M_PI/NUM_POINTS;
  int xOffset= p.x;
  int yOffset= p.y;

  for (float theta= 0.1f; theta < 2 * M_PI + 0.1f; theta+= deltaTheta) {
    Point p1 {xOffset + (int) ceil(radius * cos(theta)), yOffset + (int) ceil(radius * sin(theta)), 
      p.color, p.lineWidth};
    Point p2 {xOffset + (int) ceil(radius * cos(theta + deltaTheta)), 
      yOffset + (int) ceil(radius * sin(theta + deltaTheta)),
      p.color, p.lineWidth};
    this->drawLine(p1, p2);
  }
}
*/

void Canvas::drawCircle(const Point& p, int radius)
{
  int x_min= max(p.x - radius, 0);
  int x_max= min(p.x + radius, this->_canvas.width()-1);
  int y_min= max(p.y - radius, 0);
  int y_max= min(p.y + radius, this->_canvas.height()-1);

  for (int y= y_min; y <= y_max; y++) {
    for (int x= x_min; x <= x_max; x++) {
      int r_squared= radius * radius;
      int x_y_squared= (x-p.x) * (x-p.x) + (y-p.y) * (y-p.y);
      // check if x^2 + y^2 <= r^2 to be part of the circle

      // I don't include equal because it creates a jagged scircle
      if (x_y_squared < r_squared) {
        this->_colorPixel(x, y, p.color);
      }
    }
  }
}

bool Canvas::polygonContainsPoint(std::vector<Point>& polygon, const Point& testPoint) {
  int n= polygon.size();
  bool c= false;
  int j= n - 1;

  // this polygon is constructed sequentially from the points
  for (int i= 0; i < n; i++) {
    Point p1= polygon[i];
    Point p2= polygon[j];
    float deltaX= p2.x - p1.x;
    float ySpread= testPoint.y - p1.x;
    float deltaY= p2.y - p1.y;
    if (((p1.y > testPoint.y) != (p2.y > testPoint.y)) &&
      (testPoint.x < (((deltaX * ySpread) / deltaY) + p1.x))) {
        c = !c;
    }
    j= i;
  }
  return c;
}

bool Canvas::collision(const Point& p1, int r1, const Point& p2, int r2) {
  return pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) < pow(r1 + r2, 2);
}


void Canvas::packCircles(std::vector<Point>& polygon, std::vector<Pixel>& palette) {
  srand(time(0));

  int numCircles= 2000;
  int x_min, x_max, y_min, y_max;

  findBoundingBox(polygon, x_min, y_min, x_max, y_max);

  // want to randomly create circles that is <= 10% of the area of the box
  int areaBox= (x_max - x_min) * (y_max - y_min);

  // radius is r < sqrt(area * 0.1 / pi)
  // also don't want a 0 radius circle
  int max_radius= max((int) ceil(sqrt(areaBox * 0.1 / M_PI)), 2);

  int cur_radius;


  // these two vectors keep track of placed circles
  std::vector<Point> locations;
  std::vector<int> radii;

  // current point information
  Point p;
  Pixel cur_color;

  int x_range= x_max - x_min;
  int y_range= y_max - y_min;

  // this counts the number of times 
  // we had circle collisions per circle
  const int max_collisions= 2000;



  int palette_size= palette.size();
  for (int i= 0; i < numCircles; i++) {
    // checks how many times we have collided with another circle
    int num_collisions= 0;


    // checks if the circle has been placed or not
    bool placed= false;
    

    while (num_collisions < max_collisions && !placed) {
      // generates random color and radius
      cur_color= palette[rand() % palette_size];
      cur_radius= rand() % max_radius;

      // determines if circle is in polygon or not
      bool inPolygon= false;   

      // checks if random location for circle is in the polygon
      while (!inPolygon) {
        int center_x= rand() % x_range + x_min;
        int center_y= rand() % y_range + y_min;

        for (int k= 0; k < 16; k++) {
          float theta= M_PI * (k / 8.0f);
          p.x= center_x + (int) (cur_radius * cos(theta));
          p.y= center_y + (int) (cur_radius * sin(theta));

          inPolygon= Canvas::polygonContainsPoint(polygon, p);
          if (!inPolygon) break;
        }
        if (inPolygon) {
          p.x= center_x;
          p.y= center_y;
        }
      }

      // after passing the loop, we now try to place it on the canvas
      // but we also need to check if it collides with any other circle
      int numExistingCircles= locations.size();
      bool doesCollide= false;
      for (int i= 0; i < numExistingCircles; i++) {
        doesCollide= doesCollide || Canvas::collision(p, cur_radius, locations[i], radii[i]);
        if (doesCollide) {
          num_collisions++;
          break;
        }
      }

      // after all the checks, we finally draw it and add it to the existing
      // vector (or after reaching the max number of collisions)
      if (!doesCollide || num_collisions >= max_collisions) {
        placed= true;
        p.color= cur_color;
        drawCircle(p, cur_radius);
        locations.push_back(p);
        radii.push_back(cur_radius);
      }
    }

  }

}

void Canvas::drawRose(const Point& p, int radius, int numPetals) {
  const int NUM_POINTS= 100;
  float deltaTheta= 2*M_PI/NUM_POINTS;
  int xOffset= p.x;
  int yOffset= p.y;

  for (float theta= 0.0f; theta < 2 * M_PI - deltaTheta; theta+= deltaTheta) {
    float r_cur= radius * cos(theta*numPetals);
    float r_next= radius * cos((theta+deltaTheta)*numPetals);

    Point p1 {xOffset + (int) (r_cur * cos(theta)), yOffset + 
    (int) (r_cur * sin(theta)), p.color};
    Point p2 {xOffset + (int) (r_next * cos(theta + deltaTheta)), 
      yOffset + (int) (r_next * sin(theta + deltaTheta)),
      p.color};

    this->drawLine(p1, p2);
  }
}

void Canvas::drawFlow(Point& p) {
  
  Point p1= p;
  Point p2= p;
  
  for (int i= 0; i < this->flowField.numSteps; i++) {
    p1= p2;

    // offset for the angle flow field
    int x_offset= p1.x - this->flowField.min_x;
    int y_offset= p1.y - this->flowField.min_y;

    // get the indexes using ratios
    int x_flow_idx= (int) (x_offset / this->flowField.resolution);
    int y_flow_idx= (int) (y_offset / this->flowField.resolution);

    
    // get the angle and clamp the idx
    float angle= this->flowField.field[clamp(y_flow_idx * 
      this->flowField.nCols + x_flow_idx, 0,
      this->flowField.nCols * this->flowField.nRows - 1)];


    int x_step= this->flowField.stepLength * cos(angle);
    int y_step= this->flowField.stepLength * sin(angle);

    p2= Point {p1.x + x_step, p1.y + y_step, p1.color};

    this->drawLine(p1, p2);
  }
  

/*
  int width= this->_canvas.width();
  int height= this->_canvas.height();
  for (int y= 0; y < height; y++) {
    for (int x= 0; x < width; x++) {
      float ratio_x= x/(float)width;
      float ratio_y= y/(float)height;
      int real_x= ratio_x * this->flowField.nCols;
      int real_y= ratio_y * this->flowField.nRows;
      float angle= this->flowField.field[real_y * this->flowField.nCols + real_x];

      Pixel p {(unsigned char) (angle/(2*M_PI) * 255), (unsigned char) (angle/(2*M_PI) * 255), (unsigned char) (angle/(2*M_PI) * 255)};
      //cout << "pixel: " << (int)p.r << ", " << (int)p.g << ", " << (int)p.b << endl;
      this->_colorPixel(x, y, p);
    }
  }
  */

}