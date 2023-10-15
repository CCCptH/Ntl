export module ntl.ranges.range_utils;
import ntl.type_traits;
import ntl.concepts;
import ntl.utils.forward;

export namespace ne::ranges
{
	/**
	 * \brief decay-copy, same as c++23 auto(expr)
	 * \param expr 
	 */
	template<class T>
		requires ConceptConvertibleTo<T, TypeDecay<T>>
	constexpr TypeDecay<T> AutoCast(T&& v) noexcept(TestIsNothrowConvertible<T, TypeDecay<T>>)
	{
		return Forward<T>(v);
	}

	template<class T>
	inline constexpr bool TestIsCUE = TestIsClass<T> or TestIsUnion<T> or TestIsEnum<T>;
}
