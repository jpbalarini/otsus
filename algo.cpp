/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Juan Pablo Balarini, Sergio Nesmachnow
 * jpbalarini@fing.edu.uy, sergion@fing.edu.uy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include "algo.h"
#include "framework/CImage.h"
#include "framework/libparser.h"

int MAX_INTENSITY = 255;

/*! Computes image histogram
    \param input Input image
    \param hist Image histogram that is returned
*/
void computeHistogram(CImage &input, unsigned *hist){
  // Compute number of pixels
  long int N = input.get_width() * input.get_height();
  int i = 0;

  // Initialize array
  for(i = 0; i <= MAX_INTENSITY; i++) hist[i] = 0;

  // Use first channel
  float *in = input.get_channel(0);
  // Iterate image
  for (i = 0; i < N; i++) {
    int value = (int) in[i];
    hist[value]++;
  }

  printf("Total # of pixels: %ld\n", N);
}

/*! Segments the image using the computed threshold
    \param input Input image
    \param output Output segmented image
    \param threshold Threshold used for segmentation
*/
void segmentImage(CImage &input, CImage *output, int threshold) {
  // Compute number of pixels
  long int N = input.get_width() * input.get_height();
  // Modify output image
  // Use first channel
  float *in = input.get_channel(0);
  float *out = output->get_channel(0);
  // Iterate image
  for (int i = 0; i < N; i++) {
    int value = (int) in[i];
    // Build the segmented image
    if (value > threshold){
      out[i] = 255.0f;
    }else{
      out[i] = 0.0f;
    }
  }
}

/*! Computes Otsus segmentation
    \param input Input image
    \param hist Image histogram
    \param output Output segmented image
    \param overrided_threshold Input param that overrides threshold
*/
void computeOtsusSegmentation(CImage &input, unsigned* hist, CImage *output, int overrided_threshold){
  // Compute number of pixels
  long int N = input.get_width() * input.get_height();
  int threshold = 0;

  if (overrided_threshold != 0){
    // If threshold was manually entered
    threshold = overrided_threshold;
  }else{
    // Compute threshold
    // Init variables
    float sum = 0;
    float sumB = 0;
    int q1 = 0;
    int q2 = 0;
    float varMax = 0;

    // Auxiliary value for computing m2
    for (int i = 0; i <= MAX_INTENSITY; i++){
      sum += i * ((int)hist[i]);
    }

    for (int i = 0 ; i <= MAX_INTENSITY ; i++) {
      // Update q1
      q1 += hist[i];
      if (q1 == 0)
        continue;
      // Update q2
      q2 = N - q1;

      if (q2 == 0)
        break;
      // Update m1 and m2
      sumB += (float) (i * ((int)hist[i]));
      float m1 = sumB / q1;
      float m2 = (sum - sumB) / q2;

      // Update the between class variance
      float varBetween = (float) q1 * (float) q2 * (m1 - m2) * (m1 - m2);

      // Update the threshold if necessary
      if (varBetween > varMax) {
        varMax = varBetween;
        threshold = i;
      }
    }
  }

  // Perform the segmentation
  segmentImage(input, output, threshold);
}

//! Otsus segmentation algorithm.
/*!
  \param argc Number of arguments of the program
  \param **argv Arguments of the program
*/
void algorithm(int argc, char **argv) {
  // Optional parameters
  vector <OptStruct *> options;
  // Mandatory parameters
  vector <ParStruct *> parameters;

  OptStruct oThreshold = {"t:", 0,  NULL, NULL, "override threshold"};
  options.push_back(&oThreshold);

  ParStruct pinput = {"input", NULL, "input image"}; parameters.push_back(&pinput);
  ParStruct pout = {"output", NULL, "output image"}; parameters.push_back(&pout);

  if (!parsecmdline("otsus", "otsu's segmentation",
                    argc, argv, options, parameters)) {
    printf("\n");
    exit(-1);
  }

  // Optional parameters
  bool override_thres = oThreshold.flag;
  int overrided_threshold = 0;
  if (override_thres){
    overrided_threshold = atoi(oThreshold.value);
    // Check threshold bounds
    if (overrided_threshold > MAX_INTENSITY || overrided_threshold < 0){
      printf("Invalid threshold value\n");
      exit(-1);
    }
  }

  // Load input image
  CImage input;
  input.load((char*)pinput.value);

  // Input image properties
  int bits = input.get_bits_per_channel();
  int num_channels = input.get_num_channels();
  int Nx = input.get_width();
  int Ny = input.get_height();
  long int N = Nx * Ny;

  if (num_channels > 1){
    fprintf(stderr, "Algorithm works only with grayscale images\n");
    exit(-1);
  }
  // Create output
  CImage *output = new CImage(Nx, Ny,
                              bits,
                              num_channels);

  unsigned hist[MAX_INTENSITY + 1];
  computeHistogram(input, hist);

  computeOtsusSegmentation(input, hist, output, overrided_threshold);

  // Save output
  output->save((char*)pout.value, bits);
  delete output;
}
