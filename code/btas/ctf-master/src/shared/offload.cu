/*Copyright (c) 2014, Edgar Solomonik, all rights reserved.*/

#include <complex>
#include <assert.h>
#include <stdio.h>
#include "int_timer.h"
#include <stdint.h>

#include "offload.h"
#include "../tensor/algstrct.h"
#include "../interface/timer.h"

#ifdef USE_CUDA
#include <cuda_runtime.h>
#include <cublas_v2.h>
#include "device_launch_parameters.h"
#endif

namespace CTF_int{
  volatile static int64_t int64_t_max = INT64_MAX;
  #ifndef PROFILE
  #define TAU_PROFILE(NAME,ARG,USER)
  #define TAU_PROFILE_TIMER(ARG1, ARG2, ARG3, ARG4)
  #define TAU_PROFILER_CREATE(ARG1, ARG2, ARG3, ARG4)
  #define TAU_PROFILE_STOP(ARG)
  #define TAU_PROFILE_START(ARG)
  #define TAU_PROFILE_SET_NODE(ARG)
  #define TAU_PROFILE_SET_CONTEXT(ARG)
  #define TAU_FSTART(ARG)
  #define TAU_FSTOP(ARG)
  #endif
  
  #define ABORT                                   \
    do{                                           \
     assert(0); } while (0)
  
#ifdef USE_CUDA
  int initialized = 0;
  cublasHandle_t cuhandle;
  
  void offload_init(){
    if (!initialized){
      int ndev=0;
      cudaGetDeviceCount(&ndev);
      assert(ndev > 0);
      cublasStatus_t status = cublasCreate(&cuhandle);
      assert(status == CUBLAS_STATUS_SUCCESS);
    }
    initialized = 1;
  }
  
  void offload_exit(){
    if (initialized){
      cublasStatus_t status = cublasDestroy(cuhandle);
      assert(status == CUBLAS_STATUS_SUCCESS);
      initialized = 0;
    }
  }
  
  offload_ptr::offload_ptr(algstrct const * sr_, int64_t size_){
    sr = sr_;
    size = size_;
    cudaError_t err = cudaMalloc((void**)&dev_ptr, size*sr->el_size);
    assert(err == cudaSuccess);
  }
  
  offload_ptr::~offload_ptr(){
    cudaError_t err = cudaFree(dev_ptr);
    assert(err == cudaSuccess);
  }

  LinModel<2> upload_mdl(upload_mdl_init,"upload_mdl");
  LinModel<2> download_mdl(download_mdl_init,"download_mdl");

  double estimate_download_time(int64_t size){
    double ps[] = {1.0, (double)size};
    return download_mdl.est_time(ps);
  }

  double estimate_upload_time(int64_t size){
    double ps[] = {1.0, (double)size};
    return upload_mdl.est_time(ps);
  }
  
  void offload_ptr::download(char * host_ptr){
    assert(initialized);
    TAU_FSTART(cuda_download);
    double st_time = MPI_Wtime();
    cudaError_t err = cudaMemcpy(host_ptr, dev_ptr, size*sr->el_size,
                                 cudaMemcpyDeviceToHost);
    double exe_time = MPI_Wtime()-st_time;
    double tps[] = {exe_time, 1.0, (double)size*sr->el_size};
    download_mdl.observe(tps);
    TAU_FSTOP(cuda_download);
    assert(err == cudaSuccess);
  }
  
  void offload_ptr::upload(char const * host_ptr){
    TAU_FSTART(cuda_upload);
    double st_time = MPI_Wtime();
    cudaError_t err = cudaMemcpy(dev_ptr, host_ptr, size*sr->el_size,
                                 cudaMemcpyHostToDevice);

    double exe_time = MPI_Wtime()-st_time;
    double tps[] = {exe_time, 1.0, (double)size*sr->el_size};
    upload_mdl.observe(tps);
    TAU_FSTOP(cuda_upload);
    assert(err == cudaSuccess);
  }
  
  
  template <typename dtype>
  __global__ void gset_zero(dtype *arr, int64_t size, dtype val) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
  
    for (int i=idx; i<size; i+= gridDim.x*blockDim.x) {
      arr[i]=val;
    }
  }
  
  void offload_ptr::set_zero(){
    int blockSize = 256;
    int numBlocks = (size + blockSize - 1) / (size);
    switch (sr->el_size){
      case 4:
        gset_zero<<<blockSize, numBlocks>>>((float*)dev_ptr, size, ((float*)sr->addid())[0]);
        break;
      case 8:
        gset_zero<<<blockSize, numBlocks>>>((double*)dev_ptr, size, ((double*)sr->addid())[0]);
        break;
      case 16:
        gset_zero<<<blockSize, numBlocks>>>((std::complex<double>*)dev_ptr, size, ((std::complex<double>*)sr->addid())[0]);
        break;
      default:
        assert(0);
        break;
    }
  }
  
  void host_pinned_alloc(void ** ptr, int64_t size){
    cudaError_t err = cudaHostAlloc(ptr, size, cudaHostAllocMapped);
    assert(err == cudaSuccess);
  }
  
  void host_pinned_free(void * ptr){
    cudaError_t err = cudaFreeHost(ptr);
    assert(err == cudaSuccess);
  }
#endif
 

  template 
  void offload_gemm(char          tA,
                    char          tB,
                    int           m,
                    int           n,
                    int           k,
                    double        alpha,
                    offload_ptr & A,
                    int           lda_A,
                    offload_ptr & B,
                    int           lda_B,
                    double        beta,
                    offload_ptr & C,
                    int           lda_C);
  template 
  void offload_gemm(char                 tA,
                    char                 tB,
                    int                  m,
                    int                  n,
                    int                  k,
                    std::complex<double> alpha,
                    offload_ptr &        A,
                    int                  lda_A,
                    offload_ptr &        B,
                    int                  lda_B,
                    std::complex<double> beta,
                    offload_ptr &        C,
                    int                  lda_C);

  template <>
  void offload_gemm<double>(char           tA,
                            char           tB,
                            int            m,
                            int            n,
                            int            k,
                            double         alpha,
                            double const * dev_A,
                            int            lda_A,
                            double const * dev_B,
                            int            lda_B,
                            double         beta,
                            double *       dev_C,
                            int            lda_C){
  #ifdef USE_CUDA
    assert(initialized);
  
    cublasOperation_t cuA;  
    switch (tA){
      case 'n':
      case 'N':
        cuA = CUBLAS_OP_N;
        break;
      case 't':
      case 'T':
        cuA = CUBLAS_OP_T;
        break;
    }  
  
    cublasOperation_t cuB;
    switch (tB){
      case 'n':
      case 'N':
        cuB = CUBLAS_OP_N;
        break;
      case 't':
      case 'T':
        cuB = CUBLAS_OP_T;
        break;
    }  
  
    cublasStatus_t status = 
      cublasDgemm(cuhandle, cuA, cuB, m, n, k, &alpha, 
                  dev_A, lda_A, 
                  dev_B, lda_B, &beta, 
                  dev_C, lda_C);
  #ifdef PROFILE
    cudaDeviceSynchronize();
  #endif
    
    assert(status == CUBLAS_STATUS_SUCCESS);
  #endif  
  }
  
  
  template <>
  void offload_gemm< std::complex<double> >(
                           char                         tA,
                           char                         tB,
                           int                          m,
                           int                          n,
                           int                          k,
                           std::complex<double>         alpha,
                           std::complex<double> const * dev_A,
                           int                          lda_A,
                           std::complex<double> const * dev_B,
                           int                          lda_B,
                           std::complex<double>         beta,
                           std::complex<double> *       dev_C,
                           int                          lda_C){
  #ifdef USE_CUDA
    assert(initialized);
    
    cublasOperation_t cuA;  
    switch (tA){
      case 'n':
      case 'N':
        cuA = CUBLAS_OP_N;
        break;
      case 't':
      case 'T':
        cuA = CUBLAS_OP_T;
        break;
      case 'c':
      case 'C':
        cuA = CUBLAS_OP_C;
        break;
    }  
  
    cublasOperation_t cuB;
    switch (tB){
      case 'n':
      case 'N':
        cuB = CUBLAS_OP_N;
        break;
      case 't':
      case 'T':
        cuB = CUBLAS_OP_T;
        break;
      case 'c':
      case 'C':
        cuB = CUBLAS_OP_C;
        break;
    }  
  
    TAU_FSTART(cublas_zgemm);
    cublasStatus_t status = 
      cublasZgemm(cuhandle, cuA, cuB, m, n, k, 
                  reinterpret_cast<cuDoubleComplex*>(&alpha), 
                  reinterpret_cast<const cuDoubleComplex*>(dev_A), lda_A, 
                  reinterpret_cast<const cuDoubleComplex*>(dev_B), lda_B, 
                  reinterpret_cast<cuDoubleComplex*>(&beta), 
                  reinterpret_cast<cuDoubleComplex*>(dev_C), lda_C);
  #ifdef PROFILE
    cudaDeviceSynchronize();
  #endif
    TAU_FSTOP(cublas_zgemm);
    
    assert(status == CUBLAS_STATUS_SUCCESS);
    assert(status == CUBLAS_STATUS_SUCCESS);
  #endif
  }
 
  template <typename dtype>
  void offload_gemm(char           tA,
                    char           tB,
                    int            m,
                    int            n,
                    int            k,
                    dtype          alpha,
                    offload_ptr &  A,
                    int            lda_A,
                    offload_ptr &  B,
                    int            lda_B,
                    dtype          beta,
                    offload_ptr &  C,
                    int            lda_C){
    TAU_FSTART(cuda_gemm);
    offload_gemm(tA, tB, m, n, k, alpha, (dtype*)A.dev_ptr, lda_A, (dtype*)B.dev_ptr, lda_B, beta, (dtype*)C.dev_ptr, lda_C);
    TAU_FSTOP(cuda_gemm);
  }
}
