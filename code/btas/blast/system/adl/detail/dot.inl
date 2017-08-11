#pragma once

#include <blast/detail/config.h>
#include <blast/system/adl/detail/detail/has_adl.h>

#include <blast/system/generic/dot.h>

#include <utility>  // std::forward

namespace blast
{
namespace adl
{
namespace detail
{

tag dot(...);
tag dotu(...);

/** XXX WAR: nvcc/edg bug? **/
#if 0
template <typename... T>
auto _dot(T&&... t)
    -> has_an_adl<decltype(dot(std::forward<T>(t)...))>
{
  dot(std::forward<T>(t)...);
}

template <typename... T>
auto _dot(T&&... t)
    -> has_no_adl<decltype(dot(std::forward<T>(t)...))>
{
  blast::system::generic::dot(std::forward<T>(t)...);
}

template <typename... T>
auto _dotu(T&&... t)
    -> has_an_adl<decltype(dotu(std::forward<T>(t)...))>
{
  dotu(std::forward<T>(t)...);
}

template <typename... T>
auto _dotu(T&&... t)
    -> has_no_adl<decltype(dotu(std::forward<T>(t)...))>
{
  blast::system::generic::dotu(std::forward<T>(t)...);
}
#endif


template <typename R>
struct dot_fn {
  template <typename... T>
  static R call(T&&... t) {
    return dot(std::forward<T>(t)...);
  }
};

template <>
struct dot_fn<tag> {
  template <typename... T>
  static auto call(T&&... t)
      -> decltype(blast::system::generic::dot(std::forward<T>(t)...)) {
    return blast::system::generic::dot(std::forward<T>(t)...);
  }
};

template <typename... T>
void _dot(T&&... t) {
  dot_fn<decltype(dot(std::declval<T>()...))>::call(std::forward<T>(t)...);
}

template <typename R>
struct dotu_fn {
  template <typename... T>
  static R call(T&&... t) {
    return dotu(std::forward<T>(t)...);
  }
};

template <>
struct dotu_fn<tag> {
  template <typename... T>
  static auto call(T&&... t)
      -> decltype(blast::system::generic::dotu(std::forward<T>(t)...)) {
    return blast::system::generic::dotu(std::forward<T>(t)...);
  }
};

template <typename... T>
void _dotu(T&&... t) {
  dotu_fn<decltype(dotu(std::declval<T>()...))>::call(std::forward<T>(t)...);
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
_dot::operator()(T&&... t) const {
  blast::adl::detail::_dot(std::forward<T>(t)...);
}

template <typename... T>
void
_dotu::operator()(T&&... t) const {
  blast::adl::detail::_dotu(std::forward<T>(t)...);
}

} // end namespace adl
} // end namespace system
} // end namespace blast
