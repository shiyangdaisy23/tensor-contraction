#pragma once

#include <blast/detail/config.h>
#include <blast/detail/execution_policy.h>

namespace blast
{
namespace cblas
{

// this awkward sequence of definitions arise
// from the desire both for tag to derive
// from execution_policy and for execution_policy
// to convert to tag (when execution_policy is not
// an ancestor of tag)

// forward declaration of tag
struct tag;

// forward declaration of execution_policy
template <typename>
struct execution_policy;

// specialize execution_policy for tag
template <>
struct execution_policy<tag>
    : blast::execution_policy<tag>
{};

// tag's definition comes before the
// generic definition of execution_policy
struct tag : execution_policy<tag> {};

// allow conversion to tag when it is not a successor
template <typename Derived>
struct execution_policy
    : blast::execution_policy<Derived>
{
  // allow conversion to tag
  inline operator tag () const {
    return tag();
  }
};

static const tag par{};

} // end namespace cblas

CBLAS_TRANSPOSE
cblas_transpose(Transpose trans) {
  switch (trans) {
    case NoTrans:   return CblasNoTrans;
    case Trans:     return CblasTrans;
    case ConjTrans: return CblasConjTrans;
    default:        assert(false); return CblasNoTrans;
  }
}

CBLAS_LAYOUT
cblas_order(StorageOrder order) {
  switch (order) {
    case ColMajor: return CblasColMajor;
    case RowMajor: return CblasRowMajor;
    default:       assert(false); return CblasColMajor;
  }
}

} // end namespace blast


// Include algorithms
#include <blast/system/cblas/copy.h>
#include <blast/system/cblas/dot.h>

#include <blast/system/cblas/gemv.h>
#include <blast/system/cblas/ger.h>

#include <blast/system/cblas/gemm.h>