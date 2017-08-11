#pragma once

#include <blast/detail/config.h>

#include <blast/system/adl/dot.h>
//#include <blast/system/adl/axpy.h>

namespace blast
{
namespace system
{
namespace generic
{

template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
ger(const ExecutionPolicy& exec,
    StorageOrder order, int m, int n,
    const Alpha& alpha,
    const T* x, int incX,
    const T* y, int incY,
    U* A, int ldA)
{
#if defined(BLAST_USE_DECAY)
  // axpy
#else
  static_assert(sizeof(ExecutionPolicy) == 0, "BLAST UNIMPLEMENTED");
#endif
}

template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
geru(const ExecutionPolicy& exec,
     StorageOrder order, int m, int n,
     const Alpha& alpha,
     const T* x, int incX,
     const T* y, int incY,
     U* A, int ldA)
{
#if defined(BLAST_USE_DECAY)
  // axpy
#else
  static_assert(sizeof(ExecutionPolicy) == 0, "BLAST UNIMPLEMENTED");
#endif
}

// Default to ColMajor
template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
ger(const ExecutionPolicy& exec,
    int m, int n,
    const Alpha& alpha,
    const T* x, int incX,
    const T* y, int incY,
    U* A, int ldA)
{
  blast::system::adl::ger(exec,
                          ColMajor, m, n,
                          alpha,
                          x, incX,
                          y, incY,
                          A, ldA);
}

// Default to ColMajor
template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
geru(const ExecutionPolicy& exec,
     int m, int n,
     const Alpha& alpha,
     const T* x, int incX,
     const T* y, int incY,
     U* A, int ldA)
{
  blast::system::adl::geru(exec,
                           ColMajor, m, n,
                           alpha,
                           x, incX,
                           y, incY,
                           A, ldA);
}

// sgeru -> sger
template <typename ExecutionPolicy>
void
geru(const ExecutionPolicy& exec,
     StorageOrder order, int m, int n,
     const float& alpha,
     const float* x, int incX,
     const float* y, int incY,
     float* A, int ldA)
{
  blast::system::adl::ger(exec,
                          order, m, n,
                          alpha,
                          x, incX,
                          y, incY,
                          A, ldA);
}

// dgeru -> dger
template <typename ExecutionPolicy>
void
geru(const ExecutionPolicy& exec,
     StorageOrder order, int m, int n,
     const double& alpha,
     const double* x, int incX,
     const double* y, int incY,
     double* A, int ldA)
{
  blast::system::adl::ger(exec,
                          order, m, n,
                          alpha,
                          x, incX,
                          y, incY,
                          A, ldA);
}

} // end namespace generic
} // end namespace system
} // end namespace blast
