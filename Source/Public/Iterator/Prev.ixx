export module ntl.iterator.prev;

import ntl.iterator.iterator_traits;
import ntl.iterator.concepts;
import ntl.iterator.advance;

namespace ne::iters {
        struct PrevFunctor{
            template<ConceptBidirectionalIterator I>
            constexpr I operator()(I i) const
            {
                return --i;
            }

            template<ConceptBidirectionalIterator I >
            constexpr I operator()(I i, TypeIteratorDifference<I> n) const
            {
                Advance(i, -n);
                return i;
            }

            template<ConceptBidirectionalIterator I, ConceptSentinelFor<I> S>
            constexpr I operator()(I i, TypeIteratorDifference<I> n, I bound) const
            {
                Advance(i, -n, bound);
                return i;
            }
        };
}
export namespace ne::iters {
    inline namespace Cpo {
        inline constexpr auto Prev = PrevFunctor{};
    }
}