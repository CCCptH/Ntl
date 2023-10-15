export module ntl.ranges.end;
import ntl.type_traits;
import ntl.ranges.enable_borrowed_range;
import ntl.iterator;
import ntl.ranges.range_utils;
import ntl.ranges.type_iterator;

namespace ne::ranges::CpoImpl
{
	void End(auto&) = delete;
	void End(const auto&) = delete;

	template<class T>
	concept ConceptHasMemberEnd = ConceptCanBorrowRange<T> and requires (T && t)
	{
		{ AutoCast(t.end()) } -> ConceptIOIterator;
	};
	template<class T>
	concept ConceptUnqualifiedEnd = (!ConceptHasMemberEnd<T>) and ConceptCanBorrowRange<T>
		and (TestIsCUE<T>)
		and requires (T && t)
	{
		{ AutoCast(End(t)) } -> ConceptSentinelFor<TypeIterator<T>>;
	};

	struct EndCpo
	{
		template<class T, size_t N>
		[[nodiscard]]
		constexpr auto operator()(T(&arr)[N]) const noexcept
			requires (sizeof(T) >= 0)
		{
			return arr + N;
		}
		template<class T>
			requires ConceptUnqualifiedEnd<T>
		[[nodiscard]]
		constexpr auto operator()(T&& t) const 
			noexcept(noexcept(AutoCast(End(t))))
		{
			return AutoCast(End(t));
		}
		template<class T>
			requires ConceptHasMemberEnd<T>
		[[nodiscard]]
		constexpr auto operator()(T&& t) const 
			noexcept(noexcept(AutoCast(t.end())))
		{
			return AutoCast(t.end());
		}
		void operator()(auto&&) const = delete;
	};
}

export namespace ne::ranges
{
	// TODO: CBEGIN, CEND, RBEGIN, REND, CRBEGIN, CREND
	inline namespace Cpo
	{
		inline constexpr CpoImpl::EndCpo End{};
	}
}
