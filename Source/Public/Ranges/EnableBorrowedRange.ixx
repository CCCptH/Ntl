export module ntl.ranges. enable_borrowed_range;
import ntl.type_traits;
export namespace ne
{
	template<class T>
	inline constexpr bool ENABLE_BORROWED_RANGE = false;

	template<class T>
	concept ConceptCanBorrowRange = TestIsLRef<T> and ENABLE_BORROWED_RANGE<T>;
}
