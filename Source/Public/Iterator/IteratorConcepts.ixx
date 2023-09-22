export module ntl.iterator.concepts.iterator_concepts;
import ntl.concepts.object;
import ntl.iterator.indirectly_move;
import ntl.utils;
import ntl.iterator.iterator_traits;


export namespace ne {

    template<class T>
    concept ConceptWeaklyIncrementable =
        ConceptDefaultInitializable<T> && 
        ConceptMovable<T> &&
    requires (T i) {
        typename TypeIteratorDifference<T>;
        requires ConceptSignedIntegral<TypeIteratorDifference<T>>;
        {++i} -> ConceptSameAs<T&>;
        i++;
    };

    template<class T>
    concept ConceptIncrementable =
        ConceptRegular<T> && ConceptWeaklyIncrementable<T> &&
    requires (T i) {
        {i++} -> ConceptSameAs<T>;
    };
}

namespace ne {
    template<class T>
    concept ConceptIndirectlyReadableImpl =
    requires(const T in) {
        typename TypeIteratorValue<T>;
        typename TypeIteratorReference<T>;
        typename TypeIteratorRRef<T>;
        {*in} -> ConceptSameAs<TypeIteratorReference<T>>;
        { iters::IndirectlyMove(in) }-> ConceptSameAs<TypeIteratorRRef<T>>;
    } and
    ConceptCommonRefWith<TypeIteratorReference<T>&&, TypeIteratorValue<T>&> &&
    ConceptCommonRefWith<TypeIteratorReference<T>&&, TypeIteratorRRef<T>&&> &&
    ConceptCommonRefWith<TypeIteratorRRef<T>&&, const TypeIteratorValue<T>&>;
}
export namespace ne {
    template<class T>
    concept ConceptIndirectlyReadable = ConceptIndirectlyReadableImpl<TypeUnCVRef<T>>;

    template<class OutIterType, class ValueType>
    concept ConceptIndirectlyWritable =
    requires (OutIterType&& it, ValueType&& t) {
        *it = Forward<ValueType>(t);
        *Forward<OutIterType>(it) = Forward<ValueType>(t);
        const_cast<const TypeIteratorReference<OutIterType>&&>(*it) = Forward<ValueType>(t);
        const_cast<const TypeIteratorReference<OutIterType>&&>(Forward<OutIterType>(it)) =
        Forward<ValueType>(t);
    };

    template<class T>
    concept ConceptIOIterator =
    requires (T i) {
        {*i}->ConceptReferenceable;
    }
    && ConceptWeaklyIncrementable<T>;

    template<class S, class T>
    concept ConceptSentinelFor = ConceptSemiregular<S> && ConceptIOIterator<T> && ConceptWeaklyEqualityComparableWith<S, T>;

    template<class A, class B>
    inline constexpr auto DISABLE_SIZED_SENTINEL_FOR = false;

    template<class S, class T>
    concept ConceptSizedSentinelFor =
    ConceptSentinelFor<S, T> && !DISABLE_SIZED_SENTINEL_FOR<TypeUnCV<S>, TypeUnCV<T>> &&
    requires (const T& i, const S& s) {
        { s - i } -> ConceptSameAs<TypeIteratorDifference<T>>;
        { i - s } -> ConceptSameAs<TypeIteratorDifference<T>>;
    };

    namespace details {
        // template<class T>
        // struct iter_traits {
        //     using type = 
        // };

        // Todo: Warning maybe error
        template<class I>
        using TypeIteratorCategoryCache = typename IteratorTraits<I>::IteratorCategory;

        template<class I>
        using TypeIteratorConcept = TypeIteratorCategoryCache<I>;

    }

    template<class I>
    concept ConceptInputIterator = ConceptIOIterator<I> && 
        ConceptIndirectlyReadable<I> &&
    requires {
        typename details::TypeIteratorConcept<I>;
    } &&
    ConceptDeriveFrom<details::TypeIteratorConcept<I>, InputIteratorCategory>;

    template<class I, class T>
    concept ConceptOutputIterator = ConceptIOIterator<I>&& ConceptIndirectlyWritable<I, T> &&
    requires (I i, T&& t) {
        *i++ = Forward<T>(t);
    };

    template<class I>
    concept ConceptForwardIterator = ConceptInputIterator<I>&& ConceptDeriveFrom<details::TypeIteratorConcept<I>, ForwardIteratorCategory>
    && ConceptIncrementable<I> && ConceptSentinelFor<I, I>;

    template<class I>
    concept ConceptBidirectionalIterator = ConceptForwardIterator<I>&&ConceptDeriveFrom<details::TypeIteratorConcept<I>, BidirectionalIteratorCategory>
    && requires (I i) {
        {--i}->ConceptSameAs<I&>;
        {i--}->ConceptSameAs<I>;
    };

    template<class I>
    concept ConceptRandomAccessIterator = ConceptBidirectionalIterator<I> && ConceptDeriveFrom<details::TypeIteratorConcept<I>, RandomAccessIteratorCategory>
    && ConceptTotallyOrdered<I> && ConceptSizedSentinelFor<I, I> &&
    requires (I i, const I j, TypeIteratorReference<I> n) {
        { i += n } -> ConceptSameAs<I&>;
        { j +  n } -> ConceptSameAs<I>;
        { n +  j } -> ConceptSameAs<I>;
        { i -= n } -> ConceptSameAs<I&>;
        { j -  n } -> ConceptSameAs<I>;
        {  j[n]  } -> ConceptSameAs<TypeIteratorReference<I>>;
    };

    template<class I>
    concept ConceptContinguousIterator = ConceptRandomAccessIterator<I> && ConceptDeriveFrom<details::TypeIteratorConcept<I>, ContinguousIteratorCategory>
    && TestIsLRef<TypeIteratorDifference<I>> && ConceptSameAs<TypeIteratorValue<I>, TypeUnCVRef<TypeIteratorReference<I>>> &&
    requires (const I& i) {
        {GetAddress(i)} -> ConceptSameAs<TypeAddPointer<TypeIteratorReference<I>>>;
    };

}