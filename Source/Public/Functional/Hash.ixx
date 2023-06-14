export module ntl.functional.hash;
import ntl.concepts;
import ntl.type_traits;
import ntl.utils.nint;
export namespace ne
{
	using HashValue = uint64;
	template<class T>
	struct Hash
	{
		constexpr HashValue operator()(const T& val) const noexcept
		{
			static_assert(AlWAYS_FALSE<T>, "[ntl.functional.hash] Type does not have a hash functor.");
			return 0;
		}
	};

	template<ConceptIntegral T>
	struct Hash<T>
	{
		constexpr HashValue operator()(T val) const noexcept
		{
			return static_cast<HashValue>(val);
		}
	};
}
