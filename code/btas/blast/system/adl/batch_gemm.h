#pragma once

#include </home/yang/btas_test_2/blast/detail/config.h>
#include </home/yang/btas_test_2/blast/detail/static_const.h>

namespace blast
{
namespace system
{
namespace adl
{

struct _batch_gemm {
  template <typename... T>
  void operator()(T&&... t) const;
};

// blast::adl::batch_gemm is a global function object
namespace
{
static const auto batch_gemm = detail::static_const<blast::system::adl::_batch_gemm>::value;
}

} // end namespace adl
} // end namespace system
} // end namespace blast

#include </home/yang/btas_test_2/blast/system/adl/detail/batch_gemm.inl>
