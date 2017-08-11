#pragma once

#include <blast/detail/config.h>
#include <blast/detail/static_const.h>

namespace blast
{
namespace system
{
namespace adl
{

struct _gemm {
  template <typename... T>
  void operator()(T&&... t) const;
};

// blast::adl::gemm is a global function object
namespace
{
static const auto gemm = detail::static_const<blast::system::adl::_gemm>::value;
}

} // end namespace adl
} // end namespace system
} // end namespace blast

#include <blast/system/adl/detail/gemm.inl>
