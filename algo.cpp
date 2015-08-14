#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <vector>

//
#include "algo.h"
//
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
