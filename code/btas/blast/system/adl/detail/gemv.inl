#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/detail/detail/has_adl.h>

#include <blast/system/generic/gemv.h>

#include <utility>  // std::forward

namespace blast
{
namespace adl
{
namespace detail
{

tag gemv(...);

/** XXX WAR: nvcc/edg bug? **/
#if 0
template <typename... T>
auto _gemv(T&&... t)
    -> has_an_adl<decltype(gemv(std::forward<T>(t)...))>
{
  gemv(std::forward<T>(t)...);
}

template <typename... T>
auto _gemv(T&&... t)
    -> has_no_adl<decltype(gemv(std::forward<T>(t)...))>
{
  blast::system::generic::gemv(std::forward<T>(t)...);
}
#endif


template <typename R>
struct gemv_fn {
  template <typename... T>
  static R call(T&&... t) {
    return gemv(std::forward<T>(t)...);
  }
};

template <>
struct gemv_fn<tag> {
  template <typename... T>
  static auto call(T&&... t)
      -> decltype(blast::system::generic::gemv(std::forward<T>(t)...)) {
    return blast::system::generic::gemv(std::forward<T>(t)...);
  }
};

template <typename... T>
void _gemv(T&&... t) {
  gemv_fn<decltype(gemv(std::declval<T>()...))>::call(std::forward<T>(t)...);
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
_gemv::operator()(T&&... t) const {
  blast::adl::detail::_gemv(std::forward<T>(t)...);
}

} // end namespace adl
} // end namespace system
} // end namespace blast
