export module ntl.ranges.range_utils;
import ntl.type_traits;
#define AUTO_CAST(expr) static_cast<TypeDecay(decltype(expr))> (expr)

export namespace ne::ranges
{
	template<class T>
	inline constexpr bool TestIsCUE = TestIsClass<T> or TestIsUnion<T> or TestIsEnum<T>;
}
