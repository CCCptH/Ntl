export module ntl.ranges.concepts;
import ntl.ranges.begin;
import ntl.ranges.end;
import ntl.iterator;
import ntl.ranges.properties;
import ntl.ranges.type_iterator;
import ntl.ranges.type_sentinel;
import ntl.ranges.enable_borrowed_range;

export namespace ne::ranges
{
	template<class T>
	concept ConceptRange = requires (T & t)
	{
		ranges::Begin(t);
		ranges::End(t);
	};

	template<class T>
	concept ConceptSizedRange = ConceptRange<T> and requires (T & t) {
		ranges::Size(t);
	};

	template<class T>
	concept ConceptBorrowedRange = ConceptRange<T> && (TestIsLRef<T> || ENABLE_BORROWED_RANGE<TypeUnCVRef<T>> );

	template<class R>
	concept ConceptInputRange = ConceptRange<R> and ConceptInputIterator<TypeIterator<R>>;

	template<class R>
	concept ConceptOutputRange = ConceptRange<R> and ConceptOutputIterator<TypeIterator<R>>;

	template<class R>
	concept ConceptForwardRange = ConceptRange<R> and ConceptForwardIterator<TypeIterator<R>>;

	template<class R>
	concept ConceptBidirectionalRange = ConceptRange<R> and ConceptBidirectionalIterator<TypeIterator<R>>;

	template<class R>
	concept ConceptRandomAccessRange = ConceptRange<R> and ConceptRandomAccessIterator<TypeIterator<R>>;

	template<class R>
	concept ConceptContinguousRange = ConceptRange<R> and ConceptContinguousIterator<TypeIterator<R>>;

	template<class R>
	concept ConceptCommonRange = ConceptRange<R> and ConceptSameAs<TypeIterator<R>, TypeSentinel<R>>;

}
