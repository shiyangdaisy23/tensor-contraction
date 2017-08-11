#pragma once

#include "extern.hpp"
//#include "/opt/intel/mkl/include/mkl.h"
//#include "/opt/intel/mkl/include/mkl_cblas.h"

void
cblas_batch_sgemm(CBLAS_ORDER order,
                  CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                  CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
                  float alpha,
                  const float *A, CBLAS_INT ldA, CBLAS_INT loA,
                  const float *B, CBLAS_INT ldB, CBLAS_INT loB,
                  float beta,
                  float *C, CBLAS_INT ldC, CBLAS_INT loC,
                  CBLAS_INT batch_size)
{
// #pragma omp parallel for  
 for (CBLAS_INT i = 0; i < batch_size; ++i)
    cblas_sgemm(order,
                transA, transB,
                m, n, k,
                alpha,
                A + i*loA, ldA,
                B + i*loB, ldB,
                beta,
                C + i*loC, ldC);
}

void
cblas_batch_dgemm(CBLAS_ORDER order,
                  CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                  CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
                  double alpha,
                  const double *A, CBLAS_INT ldA, CBLAS_INT loA,
                  const double *B, CBLAS_INT ldB, CBLAS_INT loB,
                  double beta,
                  double *C, CBLAS_INT ldC, CBLAS_INT loC,
                  CBLAS_INT batch_size)
{
// #pragma omp parallel for 
  for (CBLAS_INT i = 0; i < batch_size; ++i)
    cblas_dgemm(order,
                transA, transB,
                m, n, k,
                alpha,
                A + i*loA, ldA,
                B + i*loB, ldB,
                beta,
                C + i*loC, ldC);
}

void
cblas_batch_cgemm(CBLAS_ORDER order,
                  CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                  CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
                  const float *alpha,
                  const float *A, CBLAS_INT ldA, CBLAS_INT loA,
                  const float *B, CBLAS_INT ldB, CBLAS_INT loB,
                  const float *beta,
                  float *C, CBLAS_INT ldC, CBLAS_INT loC,
                  CBLAS_INT batch_size)
{
  loA *= 2;
  loB *= 2;
  loC *= 2;


// #pragma omp parallel for
  for (CBLAS_INT i = 0; i < batch_size; ++i)
    cblas_cgemm(order,
                transA, transB,
                m, n, k,
                alpha,
                A + i*loA, ldA,
                B + i*loB, ldB,
                beta,
                C + i*loC, ldC);
}

void
cblas_batch_zgemm(CBLAS_ORDER order,
                  CBLAS_TRANSPOSE transA, CBLAS_TRANSPOSE transB,
                  CBLAS_INT m, CBLAS_INT n, CBLAS_INT k,
                  const double *alpha,
                  const double *A, CBLAS_INT ldA, CBLAS_INT loA,
                  const double *B, CBLAS_INT ldB, CBLAS_INT loB,
                  const double *beta,
                  double *C, CBLAS_INT ldC, CBLAS_INT loC,
                  CBLAS_INT batch_size)
{
  loA *= 2;
  loB *= 2;
  loC *= 2;


// #pragma omp parallel for
  for (CBLAS_INT i = 0; i < batch_size; ++i)
    cblas_zgemm(order,
                transA, transB,
                m, n, k,
                alpha,
                A + i*loA, ldA,
                B + i*loB, ldB,
                beta,
                C + i*loC, ldC);
}
