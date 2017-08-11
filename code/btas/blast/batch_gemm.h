#pragma once

#include "/home/yang/btas_test_2/blast/detail/config.h"
#include "/home/yang/btas_test_2/blast/system/adl/batch_gemm.h"

namespace blast
{

#if 0
template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
batch_gemm(ExecutionPolicy&& exec,
           int m, int n, int k,
           const Alpha& alpha,
           const MA* A, int ldA, int loA,
           const MB* B, int ldB, int loB,
           const Beta& beta,
           MC* C, int ldC, int loC,
           int p);

template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
batch_gemm(ExecutionPolicy&& exec,
           Transpose transA, Transpose transB,
           int m, int n, int k,
           const Alpha& alpha,
           const MA* A, int ldA, int loA,
           const MB* B, int ldB, int loB,
           const Beta& beta,
           MC* C, int ldC, int loC,
           int p);

template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
batch_gemm(ExecutionPolicy&& exec,
           StorageOrder order, Transpose transA, Transpose transB,
           int m, int n, int k,
           const Alpha& alpha,
           const MA* A, int ldA, int loA,
           const MB* B, int ldB, int loB,
           const Beta& beta,
           MC* C, int ldC, int loC,
           int p);
#endif

using blast::system::adl::batch_gemm;

} // end namespace blast
