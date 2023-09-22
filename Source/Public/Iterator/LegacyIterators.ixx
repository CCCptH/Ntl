export module ntl.iterator.concepts.legacy_iterators;
import ntl.concepts.core_language;
import ntl.concepts.comparision;
import ntl.concepts.object;
import ntl.type_traits;
import ntl.utils;
import ntl.iterator.incrementable_traits;
import ntl.iterator.indirectly_readable_traits;

namespace ne {
    template<ConceptReferenceable I>
    using TypeIteratorReference = decltype(*Declval<I&>());
}

export namespace ne {

    template<class I>
    concept ConceptLegacyIterator = requires (I i) {
        {   *i } -> ConceptReferenceable;
        {  ++i } -> ConceptSameAs<I&>;
        { *i++ } -> ConceptReferenceable;
    } && ConceptCopyable<I>;
    template<class I>
    concept ConceptLegacyInputIterator = ConceptLegacyIterator<I> &&
            ConceptEqualityComparable<I> &&
            requires (I i) {
                typename IncrementableTraits<I>::DifferenceType;
                typename IndirectlyReadableTraits<I>::ValueType;
                typename TypeCommonReference<TypeIteratorReference<I>&&, typename IndirectlyReadableTraits<I>::ValueType>;
                *i++;
                typename TypeCommonReference<decltype(*i++)&&, typename IndirectlyReadableTraits<I>::ValueType>;
                requires ConceptSignedIntegral<typename IncrementableTraits<I>::DifferenceType>;
            };
    template<class I>
    concept ConceptLegacyForwardIterator = ConceptLegacyInputIterator<I> && ConceptConstructibleFrom<I>
            and TestIsLRef<TypeIteratorReference<I>>
            and ConceptSameAs<
                TypeUnCVRef<TypeIteratorReference<I>>, typename IndirectlyReadableTraits<I>::ValueType
            >
            and requires(I i) {
                { i++ } -> ConceptConvertibleTo<const I&>;
                { *i++ } -> ConceptSameAs<TypeIteratorReference<I>>;
            };

    template<class I>
    concept ConceptLegacyBidirectionalIterator = ConceptLegacyForwardIterator<I>
            and requires (I i) {
                { --i } -> ConceptSameAs<I&>;
                { i-- } -> ConceptConvertibleTo<const I&>;
                { *i-- } -> ConceptSameAs<TypeIteratorReference<I>>;
            };

    template<class I>
    concept ConceptLegacyRandomAccessIterator = ConceptLegacyBidirectionalIterator<I>
            and ConceptTotallyOrdered<I>
            and requires (I i, typename IncrementableTraits<I>::DifferenceType n) {
                { i+=n } -> ConceptSameAs<I&>;
                { i-=n } -> ConceptSameAs<I&>;
                { i+n } -> ConceptSameAs<I>;
                { i-n } -> ConceptSameAs<I>;
                { n+i } -> ConceptSameAs<I>;
                { i-i } -> ConceptSameAs<decltype(n)>;
                { i[n] } -> ConceptConvertibleTo<TypeIteratorReference<I>>;
            };
}