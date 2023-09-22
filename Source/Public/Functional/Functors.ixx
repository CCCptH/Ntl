export module ntl.functional.functors;
import ntl.utils;
import ntl.concepts;

export namespace ne::functors
{
	struct Identity
	{
		static constexpr bool IS_TRANSPARENT = true;
		template<class T>
		constexpr
			T&& operator() (T&& t) const {
			return Forward<T>(t);
		}
	}; 

	struct Equal
	{
		constexpr static bool IS_TRANSPARENT = true;
		template<class X, class Y>
			requires ConceptEqualityComparableWith<X, Y>
		bool operator()(X&& x, Y&& y)
		{
			return Forward<X>(x) == Forward<Y>(y);
		}
	};

	struct NotEqual
	{
		constexpr static bool IS_TRANSPARENT = true;
		template<class X, class Y>
			requires ConceptEqualityComparableWith<X, Y>
		bool operator()(X&& x, Y&& y)
		{
			return Forward<X>(x) != Forward<Y>(y);
		}
	};

	struct Less
	{
		constexpr static bool IS_TRANSPARENT = true;
		template<class X, class Y>
			requires ConceptTotallyOrderedWith<X, Y>
		bool operator()(X&& x, Y&& y)
		{
			return Forward<X>(x) < Forward<Y>(y);
		}
	};

	struct LessEqual
	{
		constexpr static bool IS_TRANSPARENT = true;
		template<class X, class Y>
			requires ConceptTotallyOrderedWith<X, Y>
		bool operator()(X&& x, Y&& y)
		{
			return Forward<X>(x) <= Forward<Y>(y);
		}
	};

	struct Greater
	{
		constexpr static bool IS_TRANSPARENT = true;
		template<class X, class Y>
			requires ConceptTotallyOrderedWith<X, Y>
		bool operator()(X&& x, Y&& y)
		{
			return Forward<X>(x) > Forward<Y>(y);
		}
	};

	struct GreaterEqual
	{
		constexpr static bool IS_TRANSPARENT = true;
		template<class X, class Y>
			requires ConceptTotallyOrderedWith<X, Y>
		bool operator()(X&& x, Y&& y)
		{
			return Forward<X>(x) >= Forward<Y>(y);
		}
	};
}
