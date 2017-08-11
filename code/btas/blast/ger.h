#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/ger.h>

namespace blast
{

#if 0
template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
ger(ExecutionPolicy&& exec,
    StorageOrder order, int m, int n,
    const Alpha& alpha,
    const T* x, int incX,
    const T* y, int incY,
    U* A, int ldA);

template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
geru(ExecutionPolicy&& exec,
     StorageOrder order, int m, int n,
     const Alpha& alpha,
     const T* x, int incX,
     const T* y, int incY,
     U* A, int ldA);

template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
ger(ExecutionPolicy&& exec,
    int m, int n,
    const Alpha& alpha,
    const T* x, int incX,
    const T* y, int incY,
    U* A, int ldA);

template <typename ExecutionPolicy,
          typename Alpha, typename T, typename U>
void
geru(ExecutionPolicy&& exec,
     int m, int n,
     const Alpha& alpha,
     const T* x, int incX,
     const T* y, int incY,
     U* A, int ldA);
#endif

using blast::system::adl::ger;

} // end namespace blast
