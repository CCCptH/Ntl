export module ntl.ranges. enable_borrowed_range;
import ntl.type_traits;
import ntl.type_traits.primary_type_categories;
export namespace ne::ranges
{
	/**
	 * \brief Indicate that T's iterator will not dangle even if T is destroyed;
	 * \tparam T 
	 */
	template<class T>
	inline constexpr bool ENABLE_BORROWED_RANGE = false;

	template<class T>
	concept ConceptCanBorrowRange = TestIsLRef<T> or ENABLE_BORROWED_RANGE<T>;
}
