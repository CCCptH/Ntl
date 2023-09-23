export module ntl.containers.span;
import ntl.utils;
import ntl.type_traits;
export namespace ne
{
	inline constexpr int64 DYNAMIC_EXTENT = -1;
	template<class T, int64 EXTENT=DYNAMIC_EXTENT>
	class Span
	{
	public:
		using ElementType = T;
		using ValueType = TypeUnCV<T>;
		using SizeType = int64;
		using DifferenceType = ptrdiff_t;
		using Pointer = T*;
		using ConstPointer = T*;

	};
}
