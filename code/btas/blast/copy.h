#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/copy.h>

namespace blast
{

#if 0
template <typename ExecutionPolicy,
          typename T, typename U>
void
copy(ExecutionPolicy&& exec,
     int n,
     const T* x, int incX,
     U* y, int incY);

template <typename ExecutionPolicy,
          typename T, typename U>
void
copy(ExecutionPolicy&& exec,
     int n,
     const T* x,
     U* y);
#endif

using blast::system::adl::copy;

} // end namespace blast
