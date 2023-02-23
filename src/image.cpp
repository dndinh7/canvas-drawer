// Copyright 2021, Aline Normoyle, alinen

/**
 * This program defines the methods of the Image class
 * which allows for loading, saving images. It also
 * supports many image manipulation methods. This
 * program loads each image with three channels.
 * 
 * Current Supported Image Manipulation Methods:
 * - Flip Horizontal
 * - Replace
 * - SubImage
 * - Resize
 * - Greyscale
 * - Gamma Correction
 * - Alpha Blend
 * - Rotating 90 degrees
 * - Grid Copying
 * - Gaussian Blur
 * - Box Blur
 * - Unsharp Masking
 * - Sobel Operator
 * - Invert
 * - Bitmap
 * - Ridge Detection
 * - Color Swirl
 * - Color Extraction
 * - Addition
 * - Subtraction
 * - Multiply
 * - Difference 
 * - Lightest
 * - Darkest
 * - Sharpen
 * 
 * @author David Dinh
 * @version Feb 2, 2023
 * 
*/

#include "image.h"
#include <cassert>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <algorithm>
#include <cstring>
#include <cmath>
#include <stdlib.h>
#include <time.h>

#define NUM_CHANNELS 3 // assumes that there will only be three components in an image

namespace agl {

enum Color { RED = 0, GREEN, BLUE };

// Function to clamp value
int clamp(int value, int low, int hi) {
  return std::min(std::max(value, low), hi);
}

Image::Image() {
  this->myData= nullptr;
}

Image::Image(int width, int height): myWidth(width), myHeight(height) {
  this->myData= new unsigned char[width * height * NUM_CHANNELS];
  this->totalBytes= width * height * NUM_CHANNELS;
  this->totalPixels= width * height;
}


Image::Image(const Image& orig) {
  this->myData= nullptr;
  this->set(orig.width(), orig.height(), orig.data());
}

Image& Image::operator=(const Image& orig) {
  if (&orig == this) {
    return *this;
  }
  this->set(orig.width(), orig.height(), orig.data());

  return *this;
}

Image::~Image() {
  if (this->myData != nullptr) delete[] this->myData;
}

int Image::width() const {
  return this->myWidth;
}

int Image::height() const {
  return this->myHeight;
}

unsigned char* Image::data() const {
  return this->myData;
}

int Image::bytes() const {
  return this->totalBytes;
}

int Image::pixelCount() const {
  return this->totalPixels;
}

void Image::set(int width, int height, unsigned char* data) {
  assert(sizeof(data) != width * height * NUM_CHANNELS);
  this->myWidth= width;
  this->myHeight= height;
  this->totalBytes= this->myWidth * this->myHeight * NUM_CHANNELS;
  this->totalPixels= this->myWidth * this->myHeight;

  // Assures that we clean up the data we are replacing to avoid leaks
  if (this->myData != nullptr) {
    delete[] this->myData;
    this->myData= nullptr;
  }
  this->myData= new unsigned char[this->totalBytes];
  std::memcpy(this->myData, data, this->totalBytes);
}

// Assumes that flip is false for now
bool Image::load(const std::string& filename, bool flip) {
  const char* file= filename.c_str();
  unsigned char* data= stbi_load(file, &this->myWidth, &myHeight, nullptr, 3); // force it to have 4 channels

  bool success= data != nullptr;

  // so we don't set if it fails
  if (success) this->set(this->myWidth, this->myHeight, data);

  stbi_image_free(data);

  return success;
}

// Assumes that flip is false for now
bool Image::save(const std::string& filename, bool flip) const {
  const char* file= filename.c_str();
  int success= stbi_write_png(file, this->myWidth, this->myHeight, NUM_CHANNELS, 
    this->myData, this->myWidth * NUM_CHANNELS);

  return success == 1;
}

Pixel Image::get(int row, int col) const {
  this->inImageCheck(row, col);

  int idx= (row * this->myWidth + col) * NUM_CHANNELS;
  //                  red                 green                 blue
  return Pixel{ this->myData[idx + RED], this->myData[idx + GREEN], this->myData[idx + BLUE] };
}

void Image::set(int row, int col, const Pixel& color) {
  this->inImageCheck(row, col);

  int idx= (row * this->myWidth + col) * NUM_CHANNELS;

  this->myData[idx + RED]= color.r;
  this->myData[idx + GREEN] = color.g;
  this->myData[idx + BLUE] = color.b;
}

Pixel Image::get(int i) const
{
  assert(i >= 0 && i < this->totalPixels);
  int idx= i * NUM_CHANNELS;

  return Pixel{ this->myData[idx + RED], this->myData[idx + GREEN], this->myData[idx + BLUE] };
}

void Image::set(int i, const Pixel& c)
{
  assert(i >= 0 && i < this->totalPixels);
  int idx= i * NUM_CHANNELS;
  this->myData[idx + RED]= c.r;
  this->myData[idx + GREEN]= c.g;
  this->myData[idx + BLUE]= c.b;
}

Image Image::resize(int w, int h) const {
  Image result(w, h);
  int i_1;
  int j_1;
  for (int i_2= 0; i_2 < h; i_2++) {
    for (int j_2= 0; j_2 < w; j_2++) {
      float rowRatio_2= (float) i_2 / (float) (h-1);
      float colRatio_2= (float) j_2 / (float) (w-1);

      i_1= rowRatio_2 * (this->myHeight - 1);
      j_1= colRatio_2 * (this->myWidth - 1);
      
      result.set(i_2, j_2, this->get(i_1, j_1));
    }
  }
  return result;
}

Image Image::flipHorizontal() const {
  Image result(this->myWidth, this->myHeight);

  for (int i_start= 0; i_start < this->myHeight; i_start++) {

    // corresponding index of the pixel on the other side of the middle line
    int i_end= this->myHeight - 1 - i_start;
    for (int j= 0; j < this->myWidth; j++) {
      
      result.set(i_start, j, this->get(i_end, j));
    }
  }
  return result;
}

Image Image::flipVertical() const {
  Image result(0, 0);
  return result;
}

Image Image::flipPositiveDiagonal() const {
  // switch dimensions to actually flip them correctly by swapping i,j -> j,i
  Image result(this->myHeight, this->myWidth);

  // invariant is that we only traverse the bottom triangle
  for (int i= 0; i < this->myHeight; i++) {
    for (int j= 0; j < this->myWidth; j++) {
      Pixel curPixel= this->get(i, j);
      result.set(j, i, curPixel); // place the pixel in mirrored position
    }
  }

  return result;
}

Image Image::rotate90() const {
  Image flippedHorizontally= this->flipHorizontal();
  Image result= flippedHorizontally.flipPositiveDiagonal();
  
  return result;
}

Image Image::subimage(int startx, int starty, int w, int h) const {
  // assures that the sub image is actually a subimage
  assert(startx + w < this->myWidth && starty + h < this->myHeight);
  Image sub(w, h);

  for (int i= 0; i < h; i++) {
    for (int j= 0; j < w; j++) {
      sub.set(i, j, this->get(starty + i, startx + j));
    }
  }

  return sub;
}

void Image::replace(const Image& image, int startx, int starty) {
  // loop condition protects against index out of bounds error
  for (int i= 0; i < image.height() && starty + i < this->myHeight; i++) {
    for (int j= 0; j < image.width() && startx + j < this->myWidth; j++) {

      this->set(starty + i, startx + j, image.get(i, j));
    }
  }  
}

void Image::replaceColor(int x, int y, Pixel pixel)
{
  this->set(y, x, pixel);
}

void Image::addColor(int x, int y, Pixel pixel) 
{
  Pixel p= this->get(y, x);
  p.r= clamp(pixel.r + p.r, 0, 255);
  p.g= clamp(pixel.g + p.g, 0, 255);
  p.b= clamp(pixel.b + p.b, 0, 255);
  this->set(y, x, p);
}

void Image::alphaColor(int x, int y, Pixel pixel, float alpha) {
  Pixel blendedPixel {0, 0, 0};
  Pixel p1= this->get(y, x);

  blendedPixel.r= (float) p1.r * (1 - alpha) + (float) pixel.r * alpha;
  blendedPixel.g= (float) p1.g * (1 - alpha) + (float) pixel.g * alpha;
  blendedPixel.b= (float) p1.b * (1 - alpha) + (float) pixel.b * alpha;

  this->set(y, x, blendedPixel);
}

void Image::replaceAlpha(const Image& other, float alpha, int startx, int starty) {
  for (int i= 0; i < other.height() && starty + i < this->myHeight; i++) {
    for (int j= 0; j < other.width() && startx + j < this->myWidth; j++) {
      Pixel blendedPixel {0, 0, 0};
      Pixel pixel1= this->get(starty + i, startx + j);
      Pixel pixel2= other.get(i, j);

      blendedPixel.r= (float) pixel1.r * (1 - alpha) + (float) pixel2.r * alpha;
      blendedPixel.g= (float) pixel1.g * (1 - alpha) + (float) pixel2.g * alpha;
      blendedPixel.b= (float) pixel1.b * (1 - alpha) + (float) pixel2.b * alpha;
      
      this->set(starty + i, startx + j, blendedPixel);
    }
  }  
}

Image Image::swirl() const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->myHeight; i++) {
    for (int j= 0; j < this->myWidth; j++) {
      Pixel pixel= this->get(i, j);
      unsigned char tempRed= pixel.r;
      pixel.r= pixel.g;
      pixel.g= pixel.b;
      pixel.b= tempRed;

      result.set(i, j, pixel);
    }
  }
  return result;
}

Image Image::add(const Image& other) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel1= this->get(i);
    Pixel pixel2= other.get(i);
    Pixel resultPixel;
    resultPixel.r= std::min(pixel1.r + pixel2.r, 255);
    resultPixel.g= std::min(pixel1.g + pixel2.g, 255);
    resultPixel.b= std::min(pixel1.b + pixel2.b, 255);
    result.set(i, resultPixel);
  }
  
  return result;
}

Image Image::subtract(const Image& other) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel1= this->get(i);
    Pixel pixel2= other.get(i);
    Pixel resultPixel;
    resultPixel.r= std::max(pixel1.r - pixel2.r, 0);
    resultPixel.g= std::max(pixel1.g - pixel2.g, 0);
    resultPixel.b= std::max(pixel1.b - pixel2.b, 0);
    result.set(i, resultPixel);
  }
   
  return result;
}

Image Image::multiply(const Image& other) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel1= this->get(i);
    Pixel pixel2= other.get(i);
    Pixel resultPixel;
    resultPixel.r= std::min(pixel1.r * pixel2.r, 255);
    resultPixel.g= std::min(pixel1.g * pixel2.g, 255);
    resultPixel.b= std::min(pixel1.b * pixel2.b, 255);
    result.set(i, resultPixel);
  }
   
  return result;
}

Image Image::difference(const Image& other) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel1= this->get(i);
    Pixel pixel2= other.get(i);
    Pixel resultPixel;
    resultPixel.r= std::abs(pixel1.r - pixel2.r);
    resultPixel.g= std::abs(pixel1.g - pixel2.g);
    resultPixel.b= std::abs(pixel1.b - pixel2.b);
    result.set(i, resultPixel);
  }
  
  return result;
}

Image Image::lightest(const Image& other) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel1= this->get(i);
    Pixel pixel2= other.get(i);
    Pixel resultPixel;
    resultPixel.r= std::max(pixel1.r, pixel2.r);
    resultPixel.g= std::max(pixel1.g, pixel2.g);
    resultPixel.b= std::max(pixel1.b, pixel2.b);
    result.set(i, resultPixel);
  }
  
  return result;
}

Image Image::darkest(const Image& other) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel1= this->get(i);
    Pixel pixel2= other.get(i);
    Pixel resultPixel;
    resultPixel.r= std::min(pixel1.r, pixel2.r);
    resultPixel.g= std::min(pixel1.g, pixel2.g);
    resultPixel.b= std::min(pixel1.b, pixel2.b);
    result.set(i, resultPixel);
  }
  
  return result;
}

Image Image::gammaCorrect(float gamma) const {
  Image result(this->myWidth, this->myHeight);
   

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel= this->get(i);

    pixel.r= std::pow(pixel.r/255.0f, 1.0f/gamma) * 255;
    pixel.g= std::pow(pixel.g/255.0f, 1.0f/gamma) * 255;
    pixel.b= std::pow(pixel.b/255.0f, 1.0f/gamma) * 255;

    result.set(i, pixel);
  }
  
  return result;
}

Image Image::alphaBlend(const Image& other, float alpha) const {
  // assumes that images have the same dimensions
  assert(this->myWidth == other.width() && this->myHeight == other.height());
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->myHeight; i++) {
    for (int j= 0; j < this->myWidth; j++) {
      Pixel blendedPixel= Pixel { 0, 0, 0 };
      Pixel pixel1= this->get(i, j);
      Pixel pixel2= other.get(i, j);

      blendedPixel.r= (float) pixel1.r * (1 - alpha) + (float) pixel2.r * alpha;
      blendedPixel.g= (float) pixel1.g * (1 - alpha) + (float) pixel2.g * alpha;
      blendedPixel.b= (float) pixel1.b * (1 - alpha) + (float) pixel2.b * alpha;

      result.set(i, j, blendedPixel);
    } 
  }

  return result;
}

Image Image::invert() const {
  Image image(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel= this->get(i);

    pixel.r= 255 - pixel.r;
    pixel.g= 255 - pixel.g;
    pixel.b= 255 - pixel.b;    

    image.set(i, pixel);

  }
  return image;
}

Image Image::grayscale() const {
  Image result(this->myWidth, this->myHeight);
  Pixel pixel;
  unsigned char intensity;

  for (int i= 0; i < this->totalPixels; i++) {
    pixel= this->get(i);

    // Hardcoded values to make the greyscale intensity to look pleasing to human eye
    intensity= (float) pixel.r * 0.3f + (float) pixel.g * 0.59f + (float) pixel.b * 0.11f;
    
    pixel.r= intensity;
    pixel.g= intensity;
    pixel.b= intensity;

    result.set(i, pixel);
  }

  return result;
}

Image Image::colorJitter(int size) const {
  Image image(this->myWidth, this->myHeight);

  srand(time(NULL));

  int numCols= this->myWidth  / size + ((this->myWidth  % size != 0) ? 1 : 0);
  int numRows= this->myHeight / size + ((this->myHeight % size != 0) ? 1 : 0);

  for (int i= 0; i < numRows; i++) {
    for (int j= 0; j < numCols; j++) {
      int i_start= i * size;
      int j_start= j * size;
      int i_end= std::min(this->myHeight, (i+1) * size);
      int j_end= std::min(this->myWidth,  (j+1) * size);

      int redJitter= std::rand() % 80 - 40;
      int greenJitter= std::rand() % 80 - 40;
      int blueJitter= std::rand() % 80 - 40;
      for (int row= i_start; row < i_end; row++) {
        for (int col= j_start; col < j_end; col++) {
          Pixel pixel= this->get(row, col);
          pixel.r= clamp(pixel.r + redJitter, 0, 255);
          pixel.g= clamp(pixel.g + greenJitter, 0, 255);
          pixel.b= clamp(pixel.b + blueJitter, 0, 255);

          image.set(row, col, pixel);
        }
      }
    }
  }

 
  return image;
}

Image Image::bitmap(int size) const {
  Image image(this->myWidth, this->myHeight);

  // if it is not easily divisible by size, we need to iterate once more
  // to get the corners
  int numCols= this->myWidth  / size + ((this->myWidth  % size != 0) ? 1 : 0);
  int numRows= this->myHeight / size + ((this->myHeight % size != 0) ? 1 : 0);

  for (int i= 0; i < numRows; i++) {
    for (int j= 0; j < numCols; j++) {
      int i_start= i * size;
      int j_start= j * size;
      int i_end= std::min(this->myHeight, (i+1) * size);
      int j_end= std::min(this->myWidth,  (j+1) * size);
      int accumulatedRed= 0;
      int accumulatedGreen= 0;
      int accumulatedBlue= 0;
      int count= 0; // although size could be size * size, edge cases
      for (int row= i_start; row < i_end; row++) {
        for (int col= j_start; col < j_end; col++) {
          Pixel pixel= this->get(row, col);
          accumulatedRed+= pixel.r;
          accumulatedGreen+= pixel.g;
          accumulatedBlue+= pixel.b;
          count++;
        }
      }

      // this will be assigned to each pixel in the size by size pixels
      unsigned char red= accumulatedRed/count;
      unsigned char green= accumulatedGreen/count;
      unsigned char blue= accumulatedBlue/count;


      Pixel avgPixel {red, green, blue};

      for (int row= i_start; row < i_end; row++) {
        for (int col= j_start; col < j_end; col++) {
          image.set(row, col, avgPixel);
        }
      }

    }
  }


  return image;
}

Image Image::sharpen() const {
  int kernel[] {0, -1, 0,
               -1, 5, -1,
                0, -1, 0};
  
  Image result= this->convolute(kernel, 1, 3);

  return result;

}

Image Image::identity() const {
  int kernel[] {0, 0, 0,
                0, 1, 0,
                0, 0, 0};

  Image result= this->convolute(kernel, 1, 3);
  return result;
}

Image Image::gaussianBlur() const {
  int kernel[] {1, 2, 1,
                2, 4, 2,
                1, 2, 1};
  float scale= 1.0f/16.0f;

  Image result= this->convolute(kernel, scale, 3);

  return result;
}

Image Image::boxBlur() const {
  int kernel[] {1, 1, 1,
                1, 1, 1,
                1, 1, 1};
  float scale= 1.0f/9.0f;

  Image result= this->convolute(kernel, scale, 3);

  return result;
}

Image Image::ridgeDetection() const {
  int kernel[] {-1, -1, -1, -1, 8, -1, -1, -1, -1};

  Image result= this->convolute(kernel, 1, 3);

  return result;
}

Image Image::unsharpMasking() const {
  int kernel[] {1, 4, 6, 4, 1,
                4, 16, 24, 16, 4,
                6, 24, -476, 24, 6,
                4, 16, 24, 16, 4,
                1, 4, 6, 4, 1};
  float scale= -1/256.0f;

  Image result= this->convolute(kernel, scale, 5);
  return result;
}

Image Image::sobel() const {
  int kernel1[] {-1, 0, 1,
                  -2, 0, 2,
                  -1, 0, 1};
  int kernel2[] {1, 2, 1,
                 0, 0, 0,
                 -1, -2, -1};
      
  Image G1= this->convolute(kernel1, 1, 3);
  Image G2= this->convolute(kernel2, 1, 3);

  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel1= G1.get(i);
    Pixel pixel2= G2.get(i);

    unsigned char r= clamp(std::sqrt((float) pixel1.r * (float) pixel1.r + (float) pixel2.r * (float) pixel2.r), 0, 255);
    unsigned char g= clamp(std::sqrt((float) pixel1.g * (float) pixel1.g + (float) pixel2.g * (float) pixel2.g), 0, 255);
    unsigned char b= clamp(std::sqrt((float) pixel1.b * (float) pixel1.b + (float) pixel2.b * (float) pixel2.b), 0, 255);

    result.set(i, Pixel{r, g, b});
  }
  
  return result;
}

Image Image::extract(const Pixel& low, const Pixel& high) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel= this->get(i);
    if (pixel.r < low.r || pixel.g < low.g || pixel.b < low.b ||
        pixel.r > high.r || pixel.g > high.g || pixel.b > high.b) {
      pixel.r= 0;
      pixel.g= 0;
      pixel.b= 0;
    }
    result.set(i, pixel);
  }

  return result;
}

Image Image::extractRed() const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel= this->get(i);
    pixel.g= 0;
    pixel.b= 0;
    
    result.set(i, pixel);
  }

  return result;
}

Image Image::extractGreen() const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel= this->get(i);
    pixel.r= 0;
    pixel.b= 0;
    
    result.set(i, pixel);
  }

  return result;
}

Image Image::extractBlue() const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->totalPixels; i++) {
    Pixel pixel= this->get(i);
    pixel.r= 0;
    pixel.g= 0;
    
    result.set(i, pixel);
  }

  return result;
}

Image Image::gridCopy(int m, int n) const {
  Image result(this->myWidth * n, this->myHeight * m);
  unsigned char* data= result.data();

  int widthBytes= this->myWidth * 3;

  // this iterates row-by-row of our current image
  // and copies the bytes directly over to each grid cell
  // in result
  for (int i= 0; i < m * this->myHeight; i++) {
    for (int j= 0; j < n; j++) {
      memcpy(data + (i * n * widthBytes + j * widthBytes), this->myData + 
        (i % this->myHeight * widthBytes), widthBytes);
    }
  }

  return result;
}

void Image::inImageCheck(int row, int col) const {
  assert(row >= 0 && row < this->myHeight);
  assert(col >= 0 && col < this->myWidth);
  assert(this->myData != nullptr);
}


Image Image::convolute(int kernel[], float kernelScale, int sideLength) const {
  Image result(this->myWidth, this->myHeight);

  for (int i= 0; i < this->myHeight; i++) {
    for (int j= 0; j < this->myWidth; j++) {
      Pixel accumulator= {0, 0, 0};
      float accumulatorRed= 0;
      float accumulatorGreen= 0;
      float accumulatorBlue= 0;

      // convolute operator
      for (int k_i= 0; k_i < sideLength; k_i++) {
        for (int k_j= 0; k_j < sideLength; k_j++) {
          int i_offset= k_i - 1; // so we get -1, 0, or 1
          int j_offset= k_j - 1;

          int pixel_i= clamp(i + i_offset, 0, this->myHeight - 1);
          int pixel_j= clamp(j + j_offset, 0, this->myWidth - 1);


          Pixel pixel= this->get(pixel_i, pixel_j);

          // convolution operator requires us to multiply the index
          // mirrored to the pixel aka (m-i-1, n-j-1)
          int kernel_idx= (sideLength - 1 - k_i) * sideLength + (sideLength - 1 - k_j);
          accumulatorRed += kernelScale * kernel[kernel_idx] * pixel.r;
          accumulatorGreen += kernelScale * kernel[kernel_idx] * pixel.g;
          accumulatorBlue += kernelScale * kernel[kernel_idx] * pixel.b;
        }
      }

      accumulator.r= clamp(accumulatorRed, 0, 255);
      accumulator.g= clamp(accumulatorGreen, 0, 255);
      accumulator.b= clamp(accumulatorBlue, 0, 255);
      
      result.set(i, j, accumulator);

    }
  }

  return result;
}

Image Image::glow(const Pixel& low, const Pixel& high) const {
  return this->add(this->extract(low, high).boxBlur());
}




}  // namespace agl

