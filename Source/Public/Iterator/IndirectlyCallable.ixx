export module ntl.iterator.concepts.indirectly_callable;

import ntl.type_traits;
import ntl.functional.invoke;
import ntl.concepts.callable;
import ntl.concepts.core_language;
import ntl.iterator.iterator_traits;
import ntl.utils;
import ntl.iterator.concepts.iterator_concepts;

export namespace ne {
    template< class F, class I >
    concept ConceptIndirectlyUnaryInvocable =
        ConceptIndirectlyReadable<I> &&
            ConceptCopyConstructible<F> &&
            ConceptInvocable<F&, TypeIteratorValue<I>&> &&
            ConceptInvocable<F&, TypeIteratorReference<I>> &&
            ConceptInvocable<F&, TypeCommonReference<I>> &&
            ConceptCommonRefWith<
            TypeInvokeResult<F&, TypeIteratorValue<I>&>,
            TypeInvokeResult<F&, TypeIteratorReference<I>>>;
    
    template< class F, class I >
    concept ConceptIndirectlyRegularUnaryInvocable =
        ConceptIndirectlyReadable<I> &&
        ConceptCopyConstructible<F> &&
        ConceptRegularInvocable<F&, TypeIteratorValue<I>&> &&
        ConceptRegularInvocable<F&, TypeIteratorReference<I>> &&
        ConceptRegularInvocable<F&, TypeIteratorCommonRef<I>> &&
        ConceptCommonRefWith<
            TypeCommonReference<F&, TypeIteratorValue<I>&>,
            TypeInvokeResult<F&, TypeIteratorReference<I>>>;

    template<class F, class I>
    concept ConceptIndirectUnaryPredicate =
        ConceptIndirectlyReadable<I> &&
        ConceptCopyConstructible<F> &&
        ConceptPredicate<F&, TypeIteratorValue<I>&> &&
        ConceptPredicate<F&, TypeIteratorReference<I>> &&
        ConceptPredicate<F&, TypeIteratorCommonRef<I>>;

    template< class F, class I1, class I2 >
    concept ConceptIndirectBinaryPredicate =
        ConceptIndirectlyReadable<I1> &&
        ConceptIndirectlyReadable<I2> &&
        ConceptCopyConstructible<F> &&
        ConceptPredicate<F&, TypeIteratorValue<I1>&, TypeIteratorValue<I2>&> &&
        ConceptPredicate<F&, TypeIteratorValue<I1>&, TypeIteratorReference<I2>> &&
        ConceptPredicate<F&, TypeIteratorReference<I1>, TypeIteratorValue<I2>&> &&
        ConceptPredicate<F&, TypeIteratorReference<I1>, TypeIteratorReference<I2>> &&
        ConceptPredicate<F&, TypeIteratorCommonRef<I1>, TypeIteratorCommonRef<I2>>;

    template<class F, class I1, class I2 = I1>
    concept ConceptIndirectEquivalenceRelation =
        ConceptIndirectlyReadable<I1> &&
        ConceptIndirectlyReadable<I2> &&
        ConceptCopyConstructible<F> &&
        ConceptEquivalenceRelation<F&, TypeIteratorValue<I1>&, TypeIteratorValue<I2>&> &&
        ConceptEquivalenceRelation<F&, TypeIteratorValue<I1>&, TypeIteratorReference<I2>> &&
        ConceptEquivalenceRelation<F&, TypeIteratorReference<I1>, TypeIteratorValue<I2>&> &&
        ConceptEquivalenceRelation<F&, TypeIteratorReference<I1>, TypeIteratorReference<I2>> &&
        ConceptEquivalenceRelation<F&, TypeIteratorCommonRef<I1>, TypeIteratorCommonRef<I2>>;

    template<class F, class I1, class I2 = I1>
    concept ConceptIndirectStrictWeakOrderRelation =
        ConceptIndirectlyReadable<I1> &&
        ConceptIndirectlyReadable<I2> &&
        ConceptCopyConstructible<F> &&
        ConceptStrictWeakOrder<F&, TypeIteratorValue<I1>&, TypeIteratorValue<I2>&> &&
        ConceptStrictWeakOrder<F&, TypeIteratorValue<I1>&, TypeIteratorReference<I2>> &&
        ConceptStrictWeakOrder<F&, TypeIteratorReference<I1>, TypeIteratorValue<I2>&> &&
        ConceptStrictWeakOrder<F&, TypeIteratorReference<I1>, TypeIteratorReference<I2>> &&
        ConceptStrictWeakOrder<F&, TypeIteratorCommonRef<I1>, TypeIteratorCommonRef<I2>>;
}