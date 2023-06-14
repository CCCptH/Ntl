export module ntl.iterator.get_distance;
import ntl.iterator.iterator_traits;
import ntl.iterator.concepts;

namespace ne::iters {
    struct GetDistanceFunctor
    {
        template<ConceptIOIterator I, ConceptSentinelFor<I> S>
        constexpr TypeIteratorDifference<I> operator()(I first, S last) const
        {
            if constexpr (ConceptSizedSentinelFor<S, I>) {
                return last - first;
            }
            else {
                TypeIteratorDifference<I> result = 0;
                while (first != last) {
                    ++first;
                    ++result;
                }
                return result;
            }
        }

    // Todo: Range distance
    };
}
export namespace ne::iters {
    inline namespace Niebloid  {
        inline constexpr auto GetDistance = GetDistanceFunctor{};
    }
}