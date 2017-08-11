#pragma once

#include <blast/detail/config.h>
#include <blast/detail/static_const.h>

namespace blast
{
namespace system
{
namespace adl
{

struct _dot {
  template <typename... T>
  void operator()(T&&... t) const;
};

struct _dotu {
  template <typename... T>
  void operator()(T&&... t) const;
};

// blast::adl::dot is a global function object
namespace
{
static const auto dot = detail::static_const<blast::system::adl::_dot>::value;
}

// blast::adl::dotu is a global function object
namespace
{
static const auto dotu = detail::static_const<blast::system::adl::_dotu>::value;
}

} // end namespace adl
} // end namespace system
} // end namespace blast

#include <blast/system/adl/detail/dot.inl>
