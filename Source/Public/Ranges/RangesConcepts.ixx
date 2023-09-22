export module ntl.ranges.concepts;
import ntl.ranges.begin;
import ntl.ranges.end;
import ntl.iterator;

export namespace ne::ranges
{
	template<class T>
	concept ConceptRange = requires (T & t)
	{
		ranges::Begin(t);
		ranges::End(t);
	};
}
