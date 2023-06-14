export module ntl.iterator.indirectly_swap;
import ntl.utils.forward;
import ntl.utils.swap;
import ntl.type_traits;
import ntl.concepts.core_language;
import ntl.concepts.swappable;
import ntl.iterator.iterator_traits;
import ntl.iterator.concepts.iterator_concepts;

namespace ne::iters {
    namespace CpoImpl {
        template<class A, class B>
        void IndirectlySwap(A, B) = delete;

        template<class A, class B>
        concept ConceptUnqualifiedIterSwap =
            ((TestIsClass<TypeUnCVRef<A>> or TestIsEnum<TypeUnCVRef<A>>) || (TestIsClass<TypeUnCVRef<B>> or TestIsEnum<TypeUnCVRef<B>>)) &&
        requires (A&& a, B&& b) {
            IndirectlySwap(Forward<A>(a), Forward<B>(b));
        };

        template<class A, class B>
        concept ConceptReadableSwappable =
            ConceptIndirectlyReadable<A> && ConceptIndirectlyReadable<B> &&
            ConceptSwappableWith<TypeIteratorReference<A>, TypeIteratorReference<B>>;

        struct IndirectlySwapFunctor
        {
            template<class A, class B>
                requires ConceptUnqualifiedIterSwap<A, B>
            constexpr void operator()(A&& a, B&& b)
            noexcept(noexcept(IndirectlySwap(Forward<A>(a), Forward<B>(b))))
            {
                (void)IndirectlySwap(Forward<A>(a), Forward<B>(b));
        
            }

            template<class A, class B>
                requires (!ConceptUnqualifiedIterSwap<A, B>) && ConceptReadableSwappable<A, B>
            constexpr void operator()(A&& a, B&& b)
            noexcept(noexcept(utils::Swap(*Forward<A>(a), *Forward<B>(b)))) {
                utils::Swap(*Forward<A>(a), *Forward<B>(b));
            }

            template<class A, class B>
                requires ((!ConceptUnqualifiedIterSwap<A, B>) && (!ConceptReadableSwappable<A, B>))
                && ConceptIndirectlyMovableStorable<A, B> && ConceptIndirectlyMovableStorable<B, A>
            constexpr void operator()(A&& a, B&& b)
            noexcept(
                noexcept(TypeIteratorValue<B>(IndirectlyMove(b))) &&
                noexcept(*b=IndirectlyMove(a)) &&
                noexcept(*Forward<A>(a) = Declval<TypeIteratorValue<B>>())
            ) {
                TypeIteratorValue<B> old(IndirectlyMove(b));
                *b = IndirectlyMove(a);
                *Forward<A>(a) = Move(old);
            }
        };
    }
}

export namespace ne::iters {
    inline namespace Cpo {
        inline constexpr CpoImpl::IndirectlySwapFunctor IndirectlySwap{};
    }
}