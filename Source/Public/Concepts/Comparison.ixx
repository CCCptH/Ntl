export module ntl.concepts.comparision;
import ntl.concepts.core_language;
import ntl.type_traits;
import ntl.utils.forward;

namespace ne {
    template<class T>
    concept ConceptBoolTestableImpl =
            ConceptConvertibleTo<T, bool>;
}

export namespace ne {
    template<class T>
    concept ConceptBooleanTestable =
            ConceptBoolTestableImpl<T> &&
            requires (T&& b) {
                { !Forward<T>(b) } -> ConceptBoolTestableImpl;
            };

    template<class A, class B>
    concept ConceptWeaklyEqualityComparableWith =
            requires (const TypeUnRef<A>& a, const TypeUnRef<B>& b) {
                { a==b } -> ConceptBooleanTestable;
                { a!=b } -> ConceptBooleanTestable;
                { b==a } -> ConceptBooleanTestable;
                { b!=a } -> ConceptBooleanTestable;
            };

    template<class A>
    concept ConceptEqualityComparable = ConceptWeaklyEqualityComparableWith<A, A>;

    template<class A, class B>
    concept ConceptEqualityComparableWith =
            ConceptEqualityComparable<A> &&
            ConceptEqualityComparable<B> &&
            ConceptCommonRefWith<
                const TypeUnRef<A>&,
                const TypeUnRef<B>&
            > &&
            ConceptEqualityComparable<
                TypeCommonReference<const TypeUnRef<A>&, const TypeUnRef<B>&>
            > &&
            ConceptWeaklyEqualityComparableWith<A, B>;

    template<class A, class B>
    concept ConceptPartiallyOrderedWith =
        requires (const TypeUnRef<A>&a, const TypeUnRef<B>& b) {
            { a > b } -> ConceptBooleanTestable;
            { a >= b } -> ConceptBooleanTestable;
            { a < b } -> ConceptBooleanTestable;
            { a <= b } -> ConceptBooleanTestable;
            { b > a } -> ConceptBooleanTestable;
            { b >= a } -> ConceptBooleanTestable;
            { b < a } -> ConceptBooleanTestable;
            { b <= a } -> ConceptBooleanTestable;
        };

    template<class A>
    concept ConceptTotallyOrdered = ConceptPartiallyOrderedWith<A, A> && ConceptEqualityComparable<A>;

    template<class A, class B>
    concept ConceptTotallyOrderedWith = ConceptTotallyOrdered<A> && ConceptTotallyOrdered<B> &&
            ConceptEqualityComparableWith<A, B> &&
            ConceptTotallyOrdered<
                TypeCommonReference<const TypeUnRef<A>&, const TypeUnRef<B>&>
            > &&
            ConceptPartiallyOrderedWith<A, B>;


}