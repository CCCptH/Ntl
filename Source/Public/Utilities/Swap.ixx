export module ntl.utils.swap;

import ntl.utils.move;
import ntl.type_traits.primary_type_categories;
import ntl.type_traits.type_modifications;
import ntl.type_traits.property_queries;
import ntl.utils.exchange;
import ntl.concepts.core_language;

export namespace ne
{
    //template<class T>
    //constexpr
    //void Swap(T& a, T& b)
    //    noexcept
    //{
    //    T temp = Move(b);
    //    b = Move(a);
    //    a = Move(temp);
    //}
}

namespace ne::utils
{
    namespace CpoImpl {
        template<class A>
        inline constexpr bool TestUnCVRefIsClassOrEnum = TestIsEnum<TypeUnCVRef<A>> or TestIsClass<TypeUnCVRef<A>>;

        template<class A>
        void Swap(A&, A&)=delete;

        template<class A, class B>
        concept ConceptUnqualifiedSwappableWith =
        (TestUnCVRefIsClassOrEnum<A>
         || TestUnCVRefIsClassOrEnum<B>) &&
        requires (A&& a, B&& b) {
            Swap(Forward<A>(a), Forward<B>(b));
        };

        struct SwapFunctor;

        template<class A, class B, size_t N>
        concept ConceptSwappableArrays =
        (!ConceptUnqualifiedSwappableWith<A(&)[N], B(&)[N]>) &&
        (VVExtent<A> == VVExtent<B>) &&
        requires (A(& a)[N], B(&b)[N], const SwapFunctor& swapFunctor) {
            swapFunctor(a[0], b[0]);
        };

        template<class A>
        concept ConceptExchangeable = (!ConceptUnqualifiedSwappableWith<A&, A&>) &&
                                      ConceptMoveConstructible<A> &&
                                      ConceptAssignableFrom<A&, A>;

        struct SwapFunctor
        {
            template<class A, class B>
            requires ConceptUnqualifiedSwappableWith<A, B>
            constexpr void operator()(A&& a, B&& b) const
            noexcept(noexcept(Swap(Forward<A>(a), Forward<B>(b))))
            {
                Swap(Forward<A>(a), Forward<B>(b));
            }

            template<ConceptExchangeable T>
            constexpr void operator()(T& a, T& b) const {
                b = Exchange(a, Move(b));
            }

            template<class A, class B, size_t N>
            requires ConceptSwappableArrays<A, B, N>
            constexpr void operator()(A(&a)[N], B(&b)[N]) const
            noexcept(noexcept((*this)(*a, *b)))
            {
                // Todo: swap_ranges
                for(size_t i=0; i<N; ++i) {
                    (*this)(a[i], b[i]);
                }
            }
        };

    }

}
export namespace ne::utils {
    inline constexpr CpoImpl::SwapFunctor Swap{};
}