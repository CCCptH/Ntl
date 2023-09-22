export module ntl.ranges.properties;
import ntl.iterator;
import ntl.ranges.range_utils;
import ntl.concepts;
import ntl.type_traits;
import ntl.ranges.type_iterator;
import ntl.ranges.type_sentinel;

export namespace ne::ranges
{
	template<class T> inline constexpr bool DISABLE_SIZED_RANGE = false;
}

namespace ne::ranges::CpoImpl
{
	void Size(auto&) = delete;
	void Size(const auto&) = delete;

	template<class T>
	concept ConceptHasMemberSize = requires (T && t) { {AUTO_CAST(t.size())} -> ConceptIntegral; };
	template<class T>
	concept ConceptUnqualifiedSize = (!ConceptHasMemberSize<T>) and TestIsCUE<T> and requires (T && t)
	{
		{AUTO_CAST(Size(t))} -> ConceptIntegral;
	};

	struct SizeCpo
	{
		template<class T, size_t N>
		[[nodiscard]]
		constexpr auto operator()(T(&arr)[N]) const noexcept
		{
			return AUTO_CAST(VVExtent<T>);
		}

		template<class T>
			requires ConceptHasMemberSize<T>
		[[nodiscard]]
		constexpr auto operator()(T&& t) const noexcept(noexcept(AUTO_CAST(t.size()))) {
			return AUTO_CAST(t.size());
		}

		template<class T>
			requires ConceptUnqualifiedSize<T>
		[[nodiscard]]
		constexpr auto operator()(T&& t) const noexcept(noexcept(AUTO_CAST(Size(t)))) {
			return AUTO_CAST(Size(t));
		}

		template<class T>
			requires ConceptForwardIterator<TypeIterator<T>> 
		and ConceptSizedSentinelFor<TypeSentinel<T>, TypeIterator<T>>
		constexpr int64 operator()(T&& t) const noexcept(noexcept(static_cast<int64>(ranges::End(t) - ranges::Begin(t)))) {
			return ranges::End(t) - ranges::Begin(t);
		}

		void operator()(auto&&) = delete;
	};
}

export namespace ne::ranges
{
	inline namespace Cpo {
		inline constexpr CpoImpl::SizeCpo Size{};
	}
}
