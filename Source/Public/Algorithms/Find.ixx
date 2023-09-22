export module ntl.algorithm.find;
import ntl.iterator;
import ntl.ranges.concepts;
import ntl.ranges.dangling;
import ntl.functional;

namespace ne::algs::NiebloidImpl
{
	struct FindFn
	{
		template<ConceptInputIterator I, ConceptSentinelFor<I> S, class T, class Proj=functors::Identity>
			requires ConceptIndirectBinaryPredicate<functors::Equal, Projected<I, Proj>, const T*>
		constexpr I find(I first, S last, const T& val, Proj proj = {})  const
		{
			for (; first != last; ++first) {
				if (Invoke(proj, *first) == val) {
					return first;
				}
			}
			return first;
		}

		template<ne::ranges::ConceptInputRange R, class T, class Proj = functors::Identity>
			requires ConceptIndirectBinaryPredicate<functors::Equal, Projected<ranges::TypeIterator<R>, Proj>, const T*>
		constexpr ne::ranges::TypeBorrowedIterator<R> find(R&& r, const T& val, Proj proj = {}) const
		{
			return (*this)(ranges::Begin(r), ranges::End(r), val, Ref(proj));
		}
	};

	struct FindIfFn
	{
		template<ConceptInputIterator I, ConceptSentinelFor<I> S, class Proj = functors::Identity, ConceptIndirectUnaryPredicate<Projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = {}) const {
			for (; first != last; ++first) {
				if (Invoke(pred, Invoke(proj, *first))) {
					return first;
				}
			}
			return first;
		}

		template<ranges::ConceptInputRange R, class Proj = functors::Identity, ConceptIndirectUnaryPredicate<Projected<I, Proj>> Pred>
		constexpr ranges::TypeBorrowedIterator<R> operator()(R&& r, Pred pred, Proj proj = {}) const {
			return (*this)(ranges::Begin(r), ranges::End(r), Ref(pred), Ref(proj));
		}
	};

	struct FindIfNotFn
	{
		template<ConceptInputIterator I, ConceptSentinelFor<I> S, class Proj = functors::Identity, ConceptIndirectUnaryPredicate<Projected<I, Proj>> Pred>
		constexpr I operator()(I first, S last, Pred pred, Proj proj = {}) const {
			for (; first != last; ++first) {
				if (!Invoke(pred, Invoke(proj, *first))) {
					return first;
				}
			}
			return first;
		}

		template<ranges::ConceptInputRange R, class Proj = functors::Identity, ConceptIndirectUnaryPredicate<Projected<I, Proj>> Pred>
		constexpr ranges::TypeBorrowedIterator<R> operator()(R&& r, Pred pred, Proj proj = {}) const {
			return (*this)(ranges::Begin(r), ranges::End(r), Ref(pred), Ref(proj));
		}
	};
}

export namespace ne::algs
{
	inline constexpr NiebloidImpl::FindFn Find{};
	inline constexpr NiebloidImpl::FindIfFn FindIf{};
	inline constexpr NiebloidImpl::FindIfNotFn FindIfNot{};
}