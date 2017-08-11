#pragma once

#include <blast/detail/config.h>
#include <blast/detail/static_const.h>

namespace blast
{
namespace system
{
namespace adl
{

struct _gemv {
  template <typename... T>
  void operator()(T&&... t) const;
};

// blast::adl::gemv is a global function object
namespace
{
static const auto gemv = detail::static_const<blast::system::adl::_gemv>::value;
}

} // end namespace adl
} // end namespace system
} // end namespace blast

#include <blast/system/adl/detail/gemv.inl>
