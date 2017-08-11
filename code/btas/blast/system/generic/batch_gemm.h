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
           int p);

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
           int p);

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
           int p);

} // end namespace generic
} // end namespace system
} // end namespace blast

#include <blast/system/generic/detail/batch_gemm.inl>
