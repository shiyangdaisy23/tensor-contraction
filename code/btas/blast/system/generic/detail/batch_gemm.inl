#pragma once

#include <blast/detail/config.h>

#include <blast/system/adl/batch_gemm.h>
#include <blast/system/adl/gemm.h>

namespace blast
{
namespace system
{
namespace generic
{

// Swap to remove dependence on StorageOrder
template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
batch_gemm(const ExecutionPolicy& exec,
           StorageOrder order, Transpose transA, Transpose transB,
           int m, int n, int k,
           const Alpha& alpha,
           const MA* A, int ldA, int loA,
           const MB* B, int ldB, int loB,
           const Beta& beta,
           MC* C, int ldC, int loC,
           int p)
{
#if defined(BLAST_USE_DECAY)
  for (int i = 0; i < p; ++i) {
    blast::system::adl::gemm(exec,
                             order, transA, transB,
                             m, n, k,
                             alpha,
                             A + i*loA, ldA,
                             B + i*loB, ldB,
                             beta,
                             C + i*loC, ldC);
  }
#else
  static_assert(sizeof(ExecutionPolicy) == 0, "BLAST UNIMPLEMENTED");
#endif
}

// Default to ColMajor
template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
batch_gemm(const ExecutionPolicy& exec,
           Transpose transA, Transpose transB,
           int m, int n, int k,
           const Alpha& alpha,
           const MA* A, int ldA, int loA,
           const MB* B, int ldB, int loB,
           const Beta& beta,
           MC* C, int ldC, int loC,
           int p)
{
  blast::system::adl::batch_gemm(exec,
                                 ColMajor, transA, transB,
                                 m, n, k,
                                 alpha,
                                 A, ldA, loA,
                                 B, ldB, loB,
                                 beta,
                                 C, ldC, loC,
                                 p);
}

// Default to NoTrans
template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
batch_gemm(const ExecutionPolicy& exec,
           int m, int n, int k,
           const Alpha& alpha,
           const MA* A, int ldA, int loA,
           const MB* B, int ldB, int loB,
           const Beta& beta,
           MC* C, int ldC, int loC,
           int p)
{
  blast::system::adl::batch_gemm(exec,
                                 NoTrans, NoTrans,
                                 m, n, k,
                                 alpha,
                                 A, ldA, loA,
                                 B, ldB, loB,
                                 beta,
                                 C, ldC, loC,
                                 p);
}

} // end namespace generic
} // end namespace system
} // end namespace blast
