#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/gemm.h>

namespace blast
{

#if 0
template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
gemm(ExecutionPolicy&& exec,
     int m, int n, int k,
     const Alpha& alpha,
     const MA* A, int ldA,
     const MB* B, int ldB,
     const Beta& beta,
     MC* C, int ldC);

template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
gemm(ExecutionPolicy&& exec,
     Transpose transA, Transpose transB,
     int m, int n, int k,
     const Alpha& alpha,
     const MA* A, int ldA,
     const MB* B, int ldB,
     const Beta& beta,
     MC* C, int ldC);

template <typename ExecutionPolicy,
          typename Alpha, typename MA, typename MB,
          typename Beta, typename MC>
void
gemm(ExecutionPolicy&& exec,
     StorageOrder order, Transpose transA, Transpose transB,
     int m, int n, int k,
     const Alpha& alpha,
     const MA* A, int ldA,
     const MB* B, int ldB,
     const Beta& beta,
     MC* C, int ldC);

#endif

using blast::system::adl::gemm;

} // end namespace blast
