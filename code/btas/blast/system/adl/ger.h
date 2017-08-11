#pragma once

#include <blast/detail/config.h>
#include <blast/detail/static_const.h>

namespace blast
{
namespace system
{
namespace adl
{

struct _ger {
  template <typename... T>
  void operator()(T&&... t) const;
};

// blast::adl::ger is a global function object
namespace
{
static const auto ger = detail::static_const<blast::system::adl::_ger>::value;
}

} // end namespace adl
} // end namespace system
} // end namespace blast

#include <blast/system/adl/detail/ger.inl>
