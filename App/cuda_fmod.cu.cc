#ifndef CUDA_FMOD_CU_H_ 
#define CUDA_FMOD_CU_H_ 
#include <cmath> 
using namespace std;
using namespace tensorflow;
using CPUDevice = Eigen::ThreadPoolDevice;

// compute low + q*high (mod p)
void ModCastKernel(const int size, const float* in_low, const float* in_high, const double q, const double p, float* out) {
  for (int i = 0; i < size; i++) {
	  double tmp = in_low[i] + q * in_high[i];
    out[i] = (float) remainder(tmp, p);
  }
}


void ModCastFunctor(int size, const float* in_low, const float* in_high, const double q, const double p, float* out) {
  ModCastKernel(size, in_low, in_high, q, p, out);
}


// compute fmod(inp, p)
template <typename T>
void FmodKernel(const int size, const T* inp, const T p, T* out) {
  for (int i = 0; i < size; i++) {
    out[i] = fmod(inp[i], p);
  }
}

template <typename T>
void FmodFunctor(int size, const T* inp, const T p, T* out) {
  FmodKernel<T>(size, inp, p, out);
}

template<> void FmodFunctor<float>(int size, const float* inp, const float p, float* out);
template<> void FmodFunctor<double>(int size, const double* inp, const double p, double* out);

#endif
