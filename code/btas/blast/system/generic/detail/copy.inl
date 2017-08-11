#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/copy.h>

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
     U* y, int incY)
{
  static_assert(sizeof(ExecutionPolicy) == 0, "BLAST UNIMPLEMENTED");
}

// incX,incY -> 1,1
template <typename ExecutionPolicy,
          typename T, typename U>
void
copy(const ExecutionPolicy& exec,
     int n,
     const T* x,
     U* y)
{
  blast::system::adl::copy(exec, n, x, 1, y, 1);
}

} // end namespace generic
} // end namespace system
} // end namespace blast
