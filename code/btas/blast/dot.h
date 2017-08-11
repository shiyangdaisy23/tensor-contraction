#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/dot.h>

namespace blast
{

#if 0
template <typename ExecutionPolicy,
          typename T, typename U>
void
dot(ExecutionPolicy&& exec,
    int n,
    const T* x, int incX,
    const T* y, int incY,
    U& result);

template <typename ExecutionPolicy,
          typename T, typename U>
void
dot(ExecutionPolicy&& exec,
    int n,
    const T* x,
    const T* y,
    U& result);

template <typename ExecutionPolicy,
          typename T, typename U>
void
dotu(ExecutionPolicy&& exec,
     int n,
     const T* x, int incX,
     const T* y, int incY,
     U& result);

template <typename ExecutionPolicy,
          typename T, typename U>
void
dotu(ExecutionPolicy&& exec,
     int n,
     const T* x,
     const T* y,
     U& result);
#endif

using blast::system::adl::dot;
using blast::system::adl::dotu;

} // end namespace blast
