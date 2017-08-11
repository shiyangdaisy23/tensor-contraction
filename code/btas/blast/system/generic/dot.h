#pragma once

#include <blast/detail/config.h>
#include <blast/system/generic/execution_policy.h>

namespace blast
{
namespace system
{
namespace generic
{

template <typename ExecutionPolicy,
          typename T, typename U>
void
dot(const ExecutionPolicy& exec,
    int n,
    const T* x, int incX,
    const T* y, int incY,
    U& result);

template <typename ExecutionPolicy,
          typename T, typename U>
void
dotu(const ExecutionPolicy& exec,
     int n,
     const T* x, int incX,
     const T* y, int incY,
     U& result);

// incX,incY -> 1,1
template <typename ExecutionPolicy,
          typename T, typename U>
void
dot(const ExecutionPolicy& exec,
    int n,
    const T* x,
    const T* y,
    U& result);

// incX,incY -> 1,1
template <typename ExecutionPolicy,
          typename T, typename U>
void
dotu(const ExecutionPolicy& exec,
     int n,
     const T* x,
     const T* y,
     U& result);

// sdotu -> sdot
template <typename ExecutionPolicy>
void
dotu(const ExecutionPolicy& exec,
     int n,
     const float* x, int incX,
     const float* y, int incY,
     float& result);

// ddotu -> ddot
template <typename ExecutionPolicy>
void
dotu(const ExecutionPolicy& exec,
     int n,
     const double* x, int incX,
     const double* y, int incY,
     double& result);

} // end namespace generic
} // end namespace system
} // end namespace blast

#include <blast/system/generic/detail/dot.inl>
