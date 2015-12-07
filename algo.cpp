#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include "algo.h"
#include "framework/CImage.h"
#include "framework/libparser.h"

void computeHistogram(CImage &input, float *hist){
  long int N = input.get_width() * input.get_height();
  int i = 0;

  // Initialize array
  for(i = 0; i <= 255; i++) hist[i] = 0;

  // Use first channel
  float *in = input.get_channel(0);
  // Iterate image
  for (i = 0; i < N; i++) {
    int value = (int) in[i];
    hist[value] = hist[value] + 1;
  }

  printf("Total # of pixels: %ld\n", N);

  // printf("Printing normalized histogram\n");
  // for (int i = 0; i <= 255; i++){
  //   // printf("%d\t%d\n", i, (int)(hist[i]*N));
  //   // Normalized histogram
  //   printf("%d\t%f\n", i, hist[i]);
  // }
}

void computeOtsusSegmentation(CImage &input, float* hist, CImage *output, int overrided_threshold){
  long int N = input.get_width() * input.get_height();

  float sum = 0;
  for (int i=0; i<256; i++){
    sum += i * ((int)hist[i]);
  }

  float sumB = 0;
  int q1 = 0;
  int q2 = 0;

  float varMax = 0;
  int threshold = 0;

  for (int i = 0 ; i < 256 ; i++) {
    q1 += hist[i];
    if (q1 == 0)
      continue;
    q2 = N - q1;

    if (q2 == 0)
      break;

    sumB += (float) (i * ((int)hist[i]));
    float m1 = sumB / q1;
    float m2 = (sum - sumB) / q2;

    float varBetween = (float) q1 * (float) q2 * (m1 - m2) * (m1 - m2);

    if (varBetween > varMax) {
      varMax = varBetween;
      threshold = i;
    }
  }

  if (overrided_threshold != 0){
    threshold = overrided_threshold;
  }

  // int threshold = optimizedthresh;
  printf("Threshold is: %d\n", threshold);

  // Modify output image
  // Use first channel
  float *in = input.get_channel(0);
  float *out = output->get_channel(0);
  // Iterate image
  for (int i = 0; i < N; i++) {
    int value = (int) in[i];
    if (value > threshold){
      out[i] = 255.0f;
    }else{
      out[i] = 0.0f;
    }
  }
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
    if (overrided_threshold > 255 || overrided_threshold < 0){
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
    printf("Algorithm works only with grayscale images\n");
    exit(-1);
  }
  // Create output
  CImage *output = new CImage(Nx, Ny,
                              bits,
                              num_channels);

  float hist[256];
  computeHistogram(input, hist);

  computeOtsusSegmentation(input, hist, output, overrided_threshold);

  // Save output
  output->save((char*)pout.value, input.get_bits_per_channel());
  delete output;
}
