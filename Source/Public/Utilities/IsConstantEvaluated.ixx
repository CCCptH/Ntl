module;
#include <type_traits>
export module ntl.utils.is_constant_evaluated;

export namespace ne
{
	constexpr bool IsConstantEvaluated() noexcept
	{
        // C++ 23
        //if consteval {
        //    return true;
        //}
        //else {
        //    return false;
        //}
		// MSVC
        return __builtin_is_constant_evaluated();
	}
}