export module ntl.ranges.type_sentinel;
import ntl.utils.declval;
import ntl.ranges.end;
export namespace ne::ranges
{
	template<class T>
	using TypeSentinel = decltype(ranges::End(Declval<T&>()));
}
