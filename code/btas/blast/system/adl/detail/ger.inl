#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/detail/detail/has_adl.h>

#include <blast/system/generic/ger.h>

#include <utility>  // std::forward

namespace blast
{
namespace adl
{
namespace detail
{

tag ger(...);

/** XXX WAR: nvcc/edg bug? **/
#if 0
template <typename... T>
auto _ger(T&&... t)
    -> has_an_adl<decltype(ger(std::forward<T>(t)...))>
{
  ger(std::forward<T>(t)...);
}

template <typename... T>
auto _ger(T&&... t)
    -> has_no_adl<decltype(ger(std::forward<T>(t)...))>
{
  blast::system::generic::ger(std::forward<T>(t)...);
}
#endif


template <typename R>
struct ger_fn {
  template <typename... T>
  static R call(T&&... t) {
    return ger(std::forward<T>(t)...);
  }
};

template <>
struct ger_fn<tag> {
  template <typename... T>
  static auto call(T&&... t)
      -> decltype(blast::system::generic::ger(std::forward<T>(t)...)) {
    return blast::system::generic::ger(std::forward<T>(t)...);
  }
};

template <typename... T>
void _ger(T&&... t) {
  ger_fn<decltype(ger(std::declval<T>()...))>::call(std::forward<T>(t)...);
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
_ger::operator()(T&&... t) const {
  blast::adl::detail::_ger(std::forward<T>(t)...);
}

} // end namespace adl
} // end namespace system
} // end namespace blast
