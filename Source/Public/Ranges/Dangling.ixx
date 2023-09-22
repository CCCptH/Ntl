export module ntl.ranges.dangling;
import ntl.ranges.enable_borrowed_range;
import ntl.ranges.concepts;
import ntl.type_traits;
import ntl.ranges.type_iterator;
export namespace ne::ranges
{
	struct Dangling {};

	template<class T>
	using TypeBorrowedIterator = TypeConditional<ConceptBorrowedRange<T>, TypeIterator<T>, Dangling>;
}
