#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/detail/detail/has_adl.h>

#include <blast/system/generic/gemm.h>

#include <utility>  // std::forward

namespace blast
{
namespace adl
{
namespace detail
{

tag gemm(...);

/** XXX WAR: nvcc/edg bug? **/
#if 0
template <typename... T>
auto _gemm(T&&... t)
    -> has_an_adl<decltype(gemm(std::forward<T>(t)...))>
{
  gemm(std::forward<T>(t)...);
}

template <typename... T>
auto _gemm(T&&... t)
    -> has_no_adl<decltype(gemm(std::forward<T>(t)...))>
{
  blast::system::generic::gemm(std::forward<T>(t)...);
}
#endif


template <typename R>
struct gemm_fn {
  template <typename... T>
  static R call(T&&... t) {
    return gemm(std::forward<T>(t)...);
  }
};

template <>
struct gemm_fn<tag> {
  template <typename... T>
  static auto call(T&&... t)
      -> decltype(blast::system::generic::gemm(std::forward<T>(t)...)) {
    return blast::system::generic::gemm(std::forward<T>(t)...);
  }
};

template <typename... T>
void _gemm(T&&... t) {
  gemm_fn<decltype(gemm(std::declval<T>()...))>::call(std::forward<T>(t)...);
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
_gemm::operator()(T&&... t) const {
  blast::adl::detail::_gemm(std::forward<T>(t)...);
}

} // end namespace adl
} // end namespace system
} // end namespace blast
