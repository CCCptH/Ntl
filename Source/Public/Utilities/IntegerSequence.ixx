export module ntl.utils.integer_sequence;
import ntl.utils.nint;
import ntl.type_traits.primary_type_categories;

export namespace ne {
    template<class T, T ...I>
        requires TestIsIntegral<T>
    struct IntegerSequence
    {
        using Type = T;
        static constexpr size_t size() noexcept {
            return sizeof...(I);
        }
    };

    template<size_t ...Is>
    using IndexSequence = IntegerSequence<size_t, Is...>;
}

namespace ne {
    template<size_t N, class IdxSeq>
    struct MakeIndexSequenceImpl;
    template<size_t N, size_t ...Is>
    struct MakeIndexSequenceImpl<N, IntegerSequence<size_t, Is...>> {
        using Type = typename MakeIndexSequenceImpl<N-1, IntegerSequence<size_t, N-1, Is...>>::Type;
    };
    template<size_t ...Is>
    struct MakeIndexSequenceImpl<0, IntegerSequence<size_t, Is...>> {
        using Type = IntegerSequence<size_t , Is...>;
    };

    template<class T, class Seq>
    struct ConvertIntegerSeqImpl;
    template<class T, size_t ...Is>
    struct ConvertIntegerSeqImpl<T, IntegerSequence<size_t , Is...>> {
        using Type = IntegerSequence<T, Is...>;
    };
    template<class T, size_t N>
    struct MakeIntegerSequenceImpl
    {
        using Type = typename ConvertIntegerSeqImpl<T, typename MakeIndexSequenceImpl<N, IntegerSequence<size_t>>::Type>::Type;
    };
}

export namespace ne
{
    template<size_t N>
    using MakeIndexSequence = typename MakeIndexSequenceImpl<N, IntegerSequence<size_t >>::Type ;

    template<class T, size_t N>
        requires TestIsIntegral<T>
    using MakeIntegerSequence = typename MakeIntegerSequenceImpl<T, N>::Type ;
}