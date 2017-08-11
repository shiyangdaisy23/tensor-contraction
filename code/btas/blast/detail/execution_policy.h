#pragma once

#include "blast/detail/config.h"

#include <type_traits>

namespace blast
{
namespace detail
{

template <typename T>
struct is_execution_policy;

// execution_policy_base serves as a guard against
// inifinite recursion in blast entry points:
//
// template <typename DerivedPolicy>
// void foo(const blast::detail::execution_policy_base<DerivedPolicy> &s)
// {
//   using blast::system::detail::generic::foo;
//
//   foo(blast::detail::derived_cast(blast::detail::strip_const(s));
// }
//
// foo is not recursive when
// 1. DerivedPolicy is derived from blast::execution_policy below
// 2. generic::foo takes blast::execution_policy as a parameter
template <typename DerivedPolicy>
struct execution_policy_base {};


template <typename DerivedPolicy>
inline execution_policy_base<DerivedPolicy>&
strip_const(const execution_policy_base<DerivedPolicy>& x)
{
  return const_cast<execution_policy_base<DerivedPolicy>&>(x);
}


template <typename DerivedPolicy>
inline DerivedPolicy&
derived_cast(execution_policy_base<DerivedPolicy>& x)
{
  return static_cast<DerivedPolicy&>(x);
}


template <typename DerivedPolicy>
inline const DerivedPolicy&
derived_cast(const execution_policy_base<DerivedPolicy> &x)
{
  return static_cast<const DerivedPolicy&>(x);
}

} // end namespace detail


template <typename DerivedPolicy>
struct execution_policy
    : blast::detail::execution_policy_base<DerivedPolicy>
{};


} // end namespace blast
