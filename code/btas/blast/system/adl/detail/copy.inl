#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/detail/detail/has_adl.h>

#include <blast/system/generic/copy.h>

#include <utility>  // std::forward

namespace blast
{
namespace adl
{
namespace detail
{

tag copy(...);

/** XXX WAR: nvcc/edg bug? **/
#if 0
template <typename... T>
auto _copy(T&&... t)
    -> has_an_adl<decltype(copy(std::forward<T>(t)...))>
{
  copy(std::forward<T>(t)...);
}

template <typename... T>
auto _copy(T&&... t)
    -> has_no_adl<decltype(copy(std::forward<T>(t)...))>
{
  blast::system::generic::copy(std::forward<T>(t)...);
}
#endif


template <typename R>
struct copy_fn {
  template <typename... T>
  static R call(T&&... t) {
    return copy(std::forward<T>(t)...);
  }
};

template <>
struct copy_fn<tag> {
  template <typename... T>
  static auto call(T&&... t)
      -> decltype(blast::system::generic::copy(std::forward<T>(t)...)) {
    return blast::system::generic::copy(std::forward<T>(t)...);
  }
};

template <typename... T>
void _copy(T&&... t) {
  copy_fn<decltype(copy(std::declval<T>()...))>::call(std::forward<T>(t)...);
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
_copy::operator()(T&&... t) const {
  blast::adl::detail::_copy(std::forward<T>(t)...);
}

} // end namespace adl
} // end namespace system
} // end namespace blast
