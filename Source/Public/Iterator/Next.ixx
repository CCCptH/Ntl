export module ntl.iterator.next;
import ntl.iterator.iterator_traits;
import ntl.iterator.concepts;

namespace ne::iters {
    struct NextFunctor{
        template<ConceptIOIterator I>
        constexpr I operator()(I i) const
        {
            return ++i;
        }

        template<ConceptIOIterator I >
        constexpr I operator()(I i, TypeIteratorDifference<I> n) const
        {
            Advance(i, n);
            return i;
        }

        template<ConceptIOIterator I, ConceptSentinelFor<I> S>
        constexpr I operator()(I i, TypeIteratorDifference<I> n, I bound) const
        {
            Advance(i, n, bound);
            return i;
        }
    };
}
export namespace ne::iters {
    inline namespace Niebloid {
        inline constexpr auto Next = NextFunctor{};
    }
}