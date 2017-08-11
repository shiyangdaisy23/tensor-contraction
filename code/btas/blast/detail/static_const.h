#pragma once

namespace blast
{
namespace detail
{

template <typename T>
struct static_const
{
  static constexpr T value {};
};
template <typename T>
constexpr T static_const<T>::value;

} // end namespace detail
} // end namespace blast
