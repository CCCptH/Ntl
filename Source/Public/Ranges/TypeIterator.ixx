export module ntl.ranges.type_iterator;
import ntl.ranges.begin;
import ntl.utils.declval;
export namespace ne::ranges
{
	template<class T>
	using TypeIterator = decltype(ranges::Begin(Declval<T&>()));
}
