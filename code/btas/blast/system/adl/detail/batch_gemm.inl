#pragma once

#include </home/yang/btas_test_2/blast/detail/config.h>
#include </home/yang/btas_test_2/blast/system/adl/detail/detail/has_adl.h>

#include </home/yang/btas_test_2/blast/system/generic/batch_gemm.h>

#include <utility>  // std::forward

namespace blast
{
namespace adl
{
namespace detail
{

tag batch_gemm(...);

/** XXX WAR: nvcc/edg bug? **/
#if 0
template <typename... T>
auto _batch_gemm(T&&... t)
    -> has_an_adl<decltype(batch_gemm(std::forward<T>(t)...))>
{
  batch_gemm(std::forward<T>(t)...);
}

template <typename... T>
auto _batch_gemm(T&&... t)
    -> has_no_adl<decltype(batch_gemm(std::forward<T>(t)...))>
{
  blast::system::generic::batch_gemm(std::forward<T>(t)...);
}
#endif


template <typename R>
struct batch_gemm_fn {
  template <typename... T>
  static R call(T&&... t) {
    return batch_gemm(std::forward<T>(t)...);
  }
};

template <>
struct batch_gemm_fn<tag> {
  template <typename... T>
  static auto call(T&&... t)
      -> decltype(blast::system::generic::batch_gemm(std::forward<T>(t)...)) {
    return blast::system::generic::batch_gemm(std::forward<T>(t)...);
  }
};

template <typename... T>
void _batch_gemm(T&&... t) {
  batch_gemm_fn<decltype(batch_gemm(std::declval<T>()...))>::call(std::forward<T>(t)...);
}

} // end namespace detail
} // end namespace adl
} // end namespace blast


namespace blast
{
namespace system
{
namespace adl
{

template <typename... T>
void
_batch_gemm::operator()(T&&... t) const {
  blast::adl::detail::_batch_gemm(std::forward<T>(t)...);
}

} // end namespace adl
} // end namespace system
} // end namespace blast
