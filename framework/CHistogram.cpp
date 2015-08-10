/*
 *
 * This file is part of the Image Processing Framework.
 *
 * Copyright(c) 2011 Miguel Colom.
 * miguel.colom@cmla.ens-cachan.fr
 *
 * This file may be licensed under the terms of of the
 * GNU General Public License Version 2 (the ``GPL'').
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the GPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the GPL along with this
 * program. If not, go to http://www.gnu.org/licenses/gpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef _CHISTOGRAM_H_
#define	_CHISTOGRAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "CHistogram.cpp"
#include "operations.cpp"

//! Histogram class
template <class T>
class CHistogram {
public:
    //! Creates a histogram
    /*!
      \param bins Number of bins
      \param *data Data that goes inside the bins
      \param *datal Data to used to compute the histogram limits
      \param N Number of elements in data and datal
      \param adaptive True to force the same number of sample in each bin
    */
    CHistogram<T>(int bins, T *data, float *datal,
               int N, bool adaptive);
    virtual ~CHistogram<T>();
    //
    //! Gets bin begining
    /*!
      \param bin Bin number
      \return bin beginning
    */
    float get_limit_begin(int bin);

    //! Gets bin end
    /*!
      \param bin Bin number
      \return bin end
    */
    float get_limit_end(int bin);

    //! Gets the number of samples in the bin
    /*!
      \param bin Bin number
      \return number of samples in the bin
    */
    int get_num_elements_bin(int bin);

    //! Gets the elements (data) inside the bin
    /*!
      \param bin Bin number
      \return pointer to the elements (data) in the bin
    */

    T *get_data_bin(int bin);

    //! Gets the elements (datal) inside the bin
    /*!
      \param bin Bin number
      \return pointer to the elements (datal) in the bin
    */
    float *get_datal_bin(int bin);

    //! Gets the number of bins in the histogram
    /*!
      \return number of bins in the histogram
    */
    int get_num_bins();
private:
    int bins;
    float *limits_begin;
    float *limits_end;
    float *num_elements;
    T **data_bins;
    float **datal_bins;
    bool adaptive;
};

template <class T>
CHistogram<T>::CHistogram(int bins, T *data, float *datal,
                       int N, bool adaptive) {
  this->adaptive = adaptive;
  this->bins = bins;

  int samples_per_bin = (this->adaptive ? (int)floor(N / bins) : -1);

  this->limits_begin = new float[bins];
  this->limits_end   = new float[bins];

  this->num_elements = new float[bins];

  this->data_bins = new T*[this->bins];
  this->datal_bins = new float*[this->bins];
  // Initialize this->data_bins and this->datal_bins to NULL
  for (int i = 0; i < this->bins; i++) {
    this->data_bins[i] = NULL;
    this->datal_bins[i] = NULL;
  }
  
  // Allocate N samples for each bin.
  // If we need to save memory, allocate just N / (bins - 1) if adaptive.
  bool save_memory = (N > 6000*6000);
  int len_buffers;
  if (save_memory && this->adaptive) {
    if (bins > 1)
      len_buffers = N / (bins - 1);
    else
      len_buffers = N;
  }
  else
    len_buffers = N;

  T *buffer = new T[len_buffers];
  float *bufferl = new float[len_buffers];
  
  // Sort data by datal
  int *indices = new int[N];
  argsort(datal, indices, N);
  
  // Min and max
  float min_datal = datal[indices[0]];
  float max_datal = datal[indices[N-1]];

  float step = (this->adaptive ? -1 : (max_datal - min_datal) / bins);

  float lim0 = min_datal;
  int elements_count = 0;

  int bin = 0;
  for (int idx = 0; idx <= N; idx++) {    
      float lim1 = 0;
      bool finished_loading;
      if (idx == N)
        finished_loading = true;
      else {
        lim1 = datal[indices[idx]];
        if (this->adaptive) {
            finished_loading = !(bin == bins - 1) &&
                                (elements_count >= samples_per_bin);
        }
        else
            finished_loading = (lim1-lim0 >= step);    
      }

      //
      if (finished_loading) { // Finished loading current bin
          // Move data in the buffer to the bins
          this->data_bins[bin] = new T[elements_count];
          memcpy(this->data_bins[bin],
                  buffer,
                  sizeof(T) * elements_count);

          this->datal_bins[bin] = new float[elements_count];
          memcpy(this->datal_bins[bin],
                  bufferl,
                  sizeof(float) * elements_count);

          // Update limits and number of elements of the bin
          this->limits_begin[bin] = lim0;
          this->limits_end[bin] = lim1;
          this->num_elements[bin] = elements_count;          

          //printf("B%d) (%.2f, %.2f), %d elements\n", bin, lim0, lim1, elements_count);
          //
          lim0 = lim1;
          //
          bin++;
          elements_count = 0;
      }
      else { // Load...
          buffer[elements_count] = data[indices[idx]];
          bufferl[elements_count] = datal[indices[idx]];
          elements_count++;
      }
  }
  this->limits_end[bins-1] = max_datal;
  //this->limits_end[bins-1] = last_lim1 + last_bin_width; //lim1; //max_datal;
  //printf("%.2f, %.2f, %.2f, %.2f\n", max_datal, lim0, lim1, this->limits_end[bins-1]);
  this->bins = bins;
  
  delete[] indices;
  delete[] buffer;
  delete[] bufferl;
}

template <class T>
CHistogram<T>::~CHistogram() {
  // Delete data bins
  for (int i = 0; i < this->bins; i++) {
      if (this->data_bins[i] != NULL) delete[] this->data_bins[i];
      if (this->datal_bins[i] != NULL) delete[] this->datal_bins[i];
  }
  delete[] this->data_bins;
  delete[] this->datal_bins;
  
  // Delete other arrays
  delete[] this->limits_begin;
  delete[] this->limits_end;
  
  delete[] this->num_elements;
}

template <class T>
float CHistogram<T>::get_limit_begin(int bin) {
    return this->limits_begin[bin];
}

template <class T>
float CHistogram<T>::get_limit_end(int bin) {
    return this->limits_end[bin];
}

template <class T>
int CHistogram<T>::get_num_elements_bin(int bin) {
    return this->num_elements[bin];
}

template <class T>
T *CHistogram<T>::get_data_bin(int bin) {
    return this->data_bins[bin];
}

template <class T>
float *CHistogram<T>::get_datal_bin(int bin) {
    return this->datal_bins[bin];
}

template <class T>
int CHistogram<T>::get_num_bins() {
    return this->bins;
}

#endif	/* CHISTOGRAM_H */

