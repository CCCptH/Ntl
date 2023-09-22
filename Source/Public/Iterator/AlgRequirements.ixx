export module ntl.iterator.concepts.alg_requirements;
import ntl.concepts.swappable;
import ntl.concepts.core_language;
import ntl.concepts.object;
import ntl.type_traits;
import ntl.iterator.concepts.iterator_concepts;
import ntl.iterator.indirectly_swap;
import ntl.iterator.projected;

export namespace ne {
    template<class In, class Out>
    concept ConceptIndirectlyMovable =
            ConceptIndirectlyReadable<In> &&
            ConceptIndirectlyWritable<Out, TypeIteratorRRef<In>>;

    template<class In, class Out>
    concept ConceptIndirectlyMovableStorable =
        ConceptIndirectlyMovable<In, Out> &&
        ConceptIndirectlyWritable<Out, TypeIteratorValue<In>> &&
        ConceptMovable<TypeIteratorValue<In>> &&
        ConceptConstructibleFrom<TypeIteratorValue<In>, TypeIteratorRRef<In>> &&
        ConceptAssignableFrom<TypeIteratorValue<In>&, TypeIteratorRRef<In>>;

    template<class In, class Out>
    concept ConceptIndirectlyCopyable =
        ConceptIndirectlyReadable<In> &&
            ConceptIndirectlyWritable<Out, TypeIteratorReference<In>>;

    template<class In, class Out>
    concept ConceptIndirectlyCopyableStorable =
        ConceptIndirectlyCopyable<In, Out> &&
        ConceptIndirectlyWritable<Out, TypeIteratorValue<In>&> &&
        ConceptIndirectlyWritable<Out, const TypeIteratorValue<In>&> &&
        ConceptIndirectlyWritable<Out, TypeIteratorValue<In>&&> &&
        ConceptIndirectlyWritable<Out, const TypeIteratorValue<In>&&> &&
        ConceptCopyable<TypeIteratorValue<In>> &&
        ConceptConstructibleFrom<TypeIteratorValue<In>, TypeIteratorReference<In>> &&
        ConceptAssignableFrom<TypeIteratorValue<In>&, TypeIteratorReference<In>>;

        template<class A, class B=A>
        concept IndirectlySwappable = 
            ConceptIndirectlyReadable<A> && ConceptIndirectlyReadable<B> &&
        requires (const A a, const B b) {
            iters::IndirectlySwap(a, a);
            iters::IndirectlySwap(a, b);
            iters::IndirectlySwap(b, a);
            iters::IndirectlySwap(b, b);
        };

    template<class A, class B>
    concept ConceptIndirectlySwappable =
        ConceptIndirectlyReadable<A> and ConceptIndirectlyReadable<B>
        and requires (const A a, const B b)
    {
        iters::IndirectlySwap(a, a);
        iters::IndirectlySwap(a, b);
        iters::IndirectlySwap(b, a);
        iters::IndirectlySwap(b, b);
    };
    // TODO
    template<class I>
    concept ConceptPermutable =
        ConceptForwardIterator<I> &&
        ConceptIndirectlyMovableStorable<I, I> &&
        ConceptIndirectlySwappable<I, I>;
    // TODO
    //template<class I1, class I2, class Out, class R = IsLessThanFunctor,
    //    class P1 = IdentityEqualFunctor, class P2 = IdentityEqualFunctor>
    //concept ConceptMergeable =
    //    ConceptInputIterator<I1> &&
    //    ConceptInputIterator<I2> &&
    //    ConceptWeaklyIncrementable<Out> &&
    //    ConceptIndirectlyCopyable<I1, Out> &&
    //    ConceptIndirectlyCopyable<I2, Out> &&
    //    ConceptIndirectStrictWeakOrderRelation<R, Projected<I1, P1>, Projected<I2, P2>>;

    //template<class I, class R = IsLessEqualFunctor, class P = IdentityEqualFunctor>
    //concept ConceptSortable =
    //    ConceptPermutable<I> &&
    //    ConceptIndirectStrictWeakOrderRelation<R, Projected<I, P>>;
}