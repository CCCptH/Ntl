export module ntl.concepts.object;

import ntl.concepts.core_language;
import ntl.concepts.comparision;
import ntl.type_traits;

export namespace ne {
    template<class A>
    concept ConceptMovable =
            // Todo: ConceptSwappable?
            TestIsObject<A> &&
            TestIsMoveConstructible<A> &&
            ConceptAssignableFrom<A&, A>;

    template<class A>
    concept ConceptCopyable =
            TestIsCopyConstructible<A> &&
            ConceptMovable<A> &&
            ConceptAssignableFrom<A&, A&> &&
            ConceptAssignableFrom<A&, const A&> &&
            ConceptAssignableFrom<A&, const A&>;

    template<class A>
    concept ConceptSemiregular = ConceptCopyable<A> and ConceptDefaultInitializable<A>;

    template<class A>
    concept ConceptRegular = ConceptSemiregular<A> and ConceptEqualityComparable<A>;
}