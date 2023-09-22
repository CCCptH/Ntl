export module ntl.ranges. enable_borrowed_range;
import ntl.type_traits;
export namespace ne::ranges
{
	/**
	 * \brief Indicate that T's iterator will not dangle even if T is destroyed;
	 * \tparam T 
	 */
	template<class T>
	inline constexpr bool ENABLE_BORROWED_RANGE = false;
}
