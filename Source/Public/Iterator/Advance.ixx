export module ntl.iterator.advance;

import ntl.iterator.iterator_traits;
import ntl.iterator.concepts.iterator_concepts;

namespace ne::iters {
    struct AdvanceFunctor
    {
    private:
        template<class I>
        static constexpr void AdvanceForward(I& i, TypeIteratorDifference<I> n) {
            while (n>0) {--n; ++i;}
        }
        template<class I>
        static constexpr void AdvanceBackward(I& i, TypeIteratorDifference<I> n) {
            while (n<0) {++n; --i;}
        }

    public:
        template<ConceptIOIterator I>
        constexpr void operator()(I& i, TypeIteratorDifference<I> n) const {
            if (n<0 && ConceptForwardIterator<I>) throw "n < 0 for forward iterator";
            if constexpr(ConceptRandomAccessIterator<I>) {
                i+=n;
            }
            else if constexpr (ConceptBidirectionalIterator<I>) {
                AdvanceForward(i, n);
                AdvanceBackward(i, n);
            } else {
                AdvanceForward(i, n);
            }
        }

        template<ConceptIOIterator I, ConceptSentinelFor<I> S>
        constexpr void operator()(I& i, S bound) const {
            if constexpr (ConceptAssignableFrom<I&, S>) {
                i=bound;
            }
            else if constexpr (ConceptSizedSentinelFor<S, I>) {
                (*this)(i, bound-i);
            }
            else {
                while(i!=bound) {
                    ++i;
                }
            }
        }

        // 自增给定的迭代器 i n 次，或直至 i == bound ，取决于何者先来到
        template<ConceptIOIterator I, ConceptSentinelFor<I> S>
        constexpr TypeIteratorDifference<I> operator()(I& i, TypeIteratorDifference<I> n, S bound) const {
            if (n<0 && (ConceptForwardIterator<I> ||(!ConceptSameAs<I, S>)))
                throw "If `n < 0`, then `bidirectional_iterator<I> && same_as<I, S>` must be true.";
            if constexpr (ConceptSizedSentinelFor<S, I>) {
                auto geq = [](auto a, auto b)->bool {
                    return a==0  ? b==0 :
                            a > 0 ? a>=b :
                            a <=b ;
                };
                if (const auto m = bound-i; geq(n,m)) {
                    (*this)(i, bound);
                    return n-m;
                }
                (*this)(i,n);
                return 0;
            }
            else {
                while (i!=bound&&n>0) {
                    ++i;
                    --n;
                }
                if constexpr (ConceptBidirectionalIterator<I>&&ConceptSameAs<I, S>) {
                    while(i!=bound&&n<0) {
                        --i;
                        ++n;
                    }
                }
                return n;
            }
        }
    };
}
export namespace ne::iters {
    inline namespace Niebloid {
        // niebloid
        inline constexpr auto Advance = AdvanceFunctor{};
    }
}