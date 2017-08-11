#pragma once

#include <blast/detail/config.h>

#include <blast/system/adl/gemv.h>
//#include <blast/system/adl/dot.h>

namespace blast
{
namespace system
{
namespace generic
{

template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename VX,
          typename Beta, typename VY>
void
gemv(const ExecutionPolicy& exec,
     StorageOrder order, Transpose trans,
     int m, int n,
     const Alpha& alpha,
     const MA* A, int ldA,
     const VX* x, int incX,
     const Beta& beta,
     VY* y, int incY)
{
#if defined(BLAST_USE_DECAY)
  // dot
#else
  static_assert(sizeof(ExecutionPolicy) == 0, "BLAST UNIMPLEMENTED");
#endif
}

template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename VX,
          typename Beta, typename VY>
void
gemv(const ExecutionPolicy& exec,
     int m, int n,
     const Alpha& alpha,
     const MA* A, int ldA,
     const VX* x, int incX,
     const Beta& beta,
     VY* y, int incY)
{
  blast::system::adl::gemv(exec, NoTrans,
                           m, n
                           alpha,
                           A, ldA,
                           x, incX,
                           beta,
                           y, incY);
}

template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename VX,
          typename Beta, typename VY>
void
gemv(const ExecutionPolicy& exec,
     Transpose trans,
     int m, int n,
     const Alpha& alpha,
     const MA* A, int ldA,
     const VX* x, int incX,
     const Beta& beta,
     VY* y, int incY)
{
  blast::system::adl::gemv(exec, ColMajor, NoTrans,
                           m, n
                           alpha,
                           A, ldA,
                           x, incX,
                           beta,
                           y, incY);
}

} // end namespace generic
} // end namespace system
} // end namespace blast
