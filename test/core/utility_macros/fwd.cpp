#include "../../utils/test_utils.hpp"
#include <vrm/core/utility_macros/fwd.hpp>

template <typename... Ts>
void TEST_CONST func(Ts&&... xs)
{
    (void)std::initializer_list<int>{(int)VRM_CORE_FWD(xs)...};
    (void)std::initializer_list<int>{(int)FWD(xs)...};
}

TEST_MAIN()
{
    func(VRM_CORE_FWD(0), VRM_CORE_FWD(1), VRM_CORE_FWD(2));
    func(FWD(0), FWD(1), FWD(2));
    return 0;
}
