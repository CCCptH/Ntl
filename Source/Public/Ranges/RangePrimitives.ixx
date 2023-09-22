export module ntl.ranges.range_primitives;

import ntl.utils.declval;
import ntl.iterator;
import ntl.ranges.concepts;
import ntl.ranges.properties;
import ntl.ranges.type_iterator;
import ntl.ranges.type_sentinel;

export namespace ne::ranges
{
	template<ConceptSizedRange R>
	using TypeRangeSize = decltype(ranges::Size(Declval<R&>()));

	template<ConceptRange R>
	using TypeRangeDifference = TypeIteratorDifference<TypeIterator<R>>;

	template<ConceptRange R>
	using TypeRangeValue = TypeIteratorValue<TypeIterator<R>>;

	template<ConceptRange R>
	using TypeRangeReference = TypeIteratorReference<TypeIterator<R>>;

	template<ConceptRange R>
	using TypeRangeRRef = TypeIteratorRRef<TypeIterator<R>>;

	template<ConceptRange R>
	using TypeRangeCommonRef = TypeIteratorCommonRef<TypeIterator<R>>;
}
