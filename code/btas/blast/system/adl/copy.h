#pragma once

#include <blast/detail/config.h>
#include <blast/detail/static_const.h>

namespace blast
{
namespace system
{
namespace adl
{

struct _copy {
  template <typename... T>
  void operator()(T&&... t) const;
};

// blast::adl::copy is a global function object
namespace
{
static const auto copy = detail::static_const<blast::system::adl::_copy>::value;
}

} // end namespace adl
} // end namespace system
} // end namespace blast

#include <blast/system/adl/detail/copy.inl>
