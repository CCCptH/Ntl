export module ntl.ranges.begin;
import ntl.type_traits;
import ntl.ranges.enable_borrowed_range;
import ntl.iterator;
import ntl.ranges.range_utils;

// Begin
export namespace ne::ranges::CpoImpl
{
	void Begin(auto&) = delete;
	void Begin(const auto&) = delete;

	template<class T>
	concept ConceptHasMemberBegin = ConceptCanBorrowRange<T> and requires (T && t)
	{
		{ AutoCast(t.begin()) } -> ConceptIOIterator;
	};
	template<class T>
	concept ConceptUnqualifiedBegin = (!ConceptHasMemberBegin<T>) and ConceptCanBorrowRange<T>
		and TestIsCUE<T>
		and requires (T&& t)
	{
		{ AutoCast(Begin(t)) } -> ConceptIOIterator;
	};

	struct BeginCpo
	{
		template<class T>
		[[nodiscard]]
		constexpr auto operator()(T(&arr)[]) const noexcept
			requires (sizeof(T) >= 0)
		{
			return arr + 0;
		}
		template<class T, size_t N>
		[[nodiscard]]
		constexpr auto operator()(T(&arr)[N]) const noexcept
			requires (sizeof(T) >= 0)
		{
			return arr + 0;
		}
		template<class T>
			requires ConceptHasMemberBegin<T>
		[[nodiscard]]
		constexpr auto operator()(T&& t) const
			noexcept(noexcept(AutoCast(t.begin())))
		{
			return AutoCast(t.begin());
		}
		template<class T>
			requires ConceptUnqualifiedBegin<T>
		[[nodiscard]]
		constexpr auto operator()(T&& t) const 
			noexcept(noexcept(AutoCast(Begin(t))))
		{
			return AutoCast(Begin(t));
		}
		void operator()(auto&&) const = delete;
	};
}


export namespace ne::ranges
{
	inline namespace Cpo
	{
		inline constexpr auto Begin = CpoImpl::BeginCpo{};
	}
}
