export module ntl.type_traits.property_queries;

import ntl.type_traits.base_classes;
import ntl.utils.nint;

export namespace ne
{
    template<class A>
    struct ValRank: IntegralConstant<size_t , 0>{};
    template<class A>
    struct ValRank<A[]>: IntegralConstant<size_t , ValRank<A>::VALUE+1>{};
    template<class A, size_t N>
    struct ValRank<A[N]>: IntegralConstant<size_t , ValRank<A>::VALUE+1>{};

    template<class A>
    inline constexpr size_t VVRank = ValRank<A>::VALUE;

    template<class A, unsigned N=0>
    struct ValExtent: IntegralConstant<size_t , 0>{};
    template<class A>
    struct ValExtent<A[], 0>: IntegralConstant<size_t ,0>{};
    template <class A, unsigned N>
    struct ValExtent<A[], N>: ValExtent<A, N-1>{};
    template<class A, size_t I>
    struct ValExtent<A[I], 0>: IntegralConstant<size_t, I>{};
    template<class A, size_t I, unsigned N>
    struct ValExtent<A[I], N>: ValExtent<A, N-1>{};

    template<class A, unsigned N=0>
    inline constexpr size_t VVExtent = ValExtent<A, N>::VALUE;
}