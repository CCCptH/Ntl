export module ntl.compare;
import <compare>;
import ntl.utils;
import ntl.type_traits;

export namespace ne
{
	using WeakOrdering = std::weak_ordering;
	using StrongOrdering = std::strong_ordering;
	using PartialOrdering = std::partial_ordering;

	template<class X, class Y>
	using TypeCompare3Way = decltype(
		Declval<const TypeUnCVRef<X>&>() <=> Declval<const TypeUnCVRef<Y>&>()
		);
}

//namespace ne
//{
//	class PartialOrdering;
//	class WeakOrdering;
//	class StrongOrdering;
//	enum class OrderResult: int8
//	{
//		LESS = -1,
//		EQUIVALENT = 0,
//		GREATER = 1,
//	};
//	struct UnspecifiedParam
//	{
//		constexpr
//		UnspecifiedParam(int UnspecifiedParam::*) noexcept {}
//		template<class T>
//			requires ( not (TestIsSame<T, int>
//			or TestIsSame<T, PartialOrdering>
//			or TestIsSame<T, WeakOrdering>
//			or TestIsSame<T, StrongOrdering>
//				))
//		UnspecifiedParam(T) = delete;
//	};
//}
//
//export namespace ne
//{
//	template<class X, class Y>
//	using TypeCompare3Way = decltype(
//		Declval<const TypeUnCVRef<X>&>() <=> Declval<const TypeUnCVRef<Y>&>()
//		);
//
//	class WeakOrdering
//	{
//	private:
//		using ValueType = int8;
//		ValueType val;
//	public:
//		explicit constexpr
//		WeakOrdering(OrderResult v)
//			: val(ValueType(v))
//		{}
//
//		const static WeakOrdering less;
//		const static WeakOrdering equivalent;
//		const static WeakOrdering greater;
//
//		friend constexpr bool operator==(WeakOrdering a, WeakOrdering b) noexcept = default;
//		friend constexpr bool operator==(WeakOrdering a, UnspecifiedParam) noexcept {
//			return a.val == 0;
//		}
//		friend constexpr bool operator==(UnspecifiedParam, WeakOrdering a) noexcept {
//			return a.val == 0;
//		}
//		friend constexpr bool operator<(WeakOrdering a, UnspecifiedParam) noexcept {
//			return a.val < 0;
//		}
//		friend constexpr bool operator<=(WeakOrdering a, UnspecifiedParam) noexcept {
//			return a.val <= 0;
//		}
//		friend constexpr bool operator<(UnspecifiedParam, WeakOrdering a) noexcept {
//			return 0 < a.val;
//		}
//		friend constexpr bool operator<=(UnspecifiedParam, WeakOrdering a) noexcept {
//			return 0 <= a.val;
//		}
//		friend constexpr bool operator>(WeakOrdering a, UnspecifiedParam) noexcept {
//			return a.val > 0;
//		}
//		friend constexpr bool operator>=(WeakOrdering a, UnspecifiedParam) noexcept {
//			return a.val >= 0;
//		}
//		friend constexpr bool operator>(UnspecifiedParam, WeakOrdering a) noexcept {
//			return 0 > a.val;
//		}
//		friend constexpr bool operator>=(UnspecifiedParam, WeakOrdering a) noexcept {
//			return 0 >= a.val;
//		}
//		friend constexpr WeakOrdering operator<=> (WeakOrdering a, UnspecifiedParam) noexcept {
//			return a;
//		}
//		friend constexpr WeakOrdering operator<=> (UnspecifiedParam, WeakOrdering a) noexcept {
//			return a;
//		}
//	};
//
//	inline constexpr WeakOrdering WeakOrdering::less(OrderResult::LESS);
//	inline constexpr WeakOrdering WeakOrdering::equivalent(OrderResult::EQUIVALENT);
//	inline constexpr WeakOrdering WeakOrdering::greater(OrderResult::GREATER);
//
//	class StrongOrdering
//	{
//	private:
//		using ValueType = int8;
//		ValueType val;
//	public:
//		explicit constexpr
//			StrongOrdering(OrderResult v)
//			: val(ValueType(v))
//		{}
//
//
//		constexpr operator WeakOrdering() const noexcept
//		{
//			return WeakOrdering(OrderResult(val));
//		}
//
//		const static StrongOrdering less;
//		const static StrongOrdering equivalent;
//		const static StrongOrdering greater;
//
//		friend constexpr bool operator==(StrongOrdering a, StrongOrdering b) noexcept = default;
//		friend constexpr bool operator==(StrongOrdering a, UnspecifiedParam) noexcept {
//			return a.val == 0;
//		}
//		friend constexpr bool operator==(UnspecifiedParam, StrongOrdering a) noexcept {
//			return a.val == 0;
//		}
//		friend constexpr bool operator<(StrongOrdering a, UnspecifiedParam) noexcept {
//			return a.val < 0;
//		}
//		friend constexpr bool operator<=(StrongOrdering a, UnspecifiedParam) noexcept {
//			return a.val <= 0;
//		}
//		friend constexpr bool operator<(UnspecifiedParam, StrongOrdering a) noexcept {
//			return 0 < a.val;
//		}
//		friend constexpr bool operator<=(UnspecifiedParam, StrongOrdering a) noexcept {
//			return 0 <= a.val;
//		}
//		friend constexpr bool operator>(StrongOrdering a, UnspecifiedParam) noexcept {
//			return a.val > 0;
//		}
//		friend constexpr bool operator>=(StrongOrdering a, UnspecifiedParam) noexcept {
//			return a.val >= 0;
//		}
//		friend constexpr bool operator>(UnspecifiedParam, StrongOrdering a) noexcept {
//			return 0 > a.val;
//		}
//		friend constexpr bool operator>=(UnspecifiedParam, StrongOrdering a) noexcept {
//			return 0 >= a.val;
//		}
//		friend constexpr StrongOrdering operator<=> (StrongOrdering a, UnspecifiedParam) noexcept {
//			return a;
//		}
//		friend constexpr StrongOrdering operator<=> (UnspecifiedParam, StrongOrdering a) noexcept {
//			return a;
//		}
//	};
//
//	inline constexpr StrongOrdering StrongOrdering::less(OrderResult::LESS);
//	inline constexpr StrongOrdering StrongOrdering::equivalent(OrderResult::EQUIVALENT);
//	inline constexpr StrongOrdering StrongOrdering::greater(OrderResult::GREATER);
//
//}