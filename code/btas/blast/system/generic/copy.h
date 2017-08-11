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
copy(const ExecutionPolicy& exec,
    int n,
    const T* x, int incX,
    U* y, int incY);

// incX,incY -> 1,1
template <typename ExecutionPolicy,
          typename T, typename U>
void
copy(const ExecutionPolicy& exec,
    int n,
    const T* x,
    T* y);

} // end namespace generic
} // end namespace system
} // end namespace blast

#include <blast/system/generic/detail/copy.inl>
