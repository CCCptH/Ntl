export module ntl.concepts.swappable;
import ntl.concepts.core_language;
import ntl.utils.swap;
export namespace ne
{
    template<class A>
    concept ConceptSwappable = requires (A& a, A& b) {
        utils::Swap(a, b);
    };

    template<class A, class B>
    concept ConceptSwappableWith = ConceptCommonRefWith<A, B> and 
    requires (A&& a, B&& b) {
        utils::Swap(Forward<A>(a), Forward<A>(a));
        utils::Swap(Forward<A>(a), Forward<B>(b));
        utils::Swap(Forward<B>(b), Forward<A>(a));
        utils::Swap(Forward<B>(b), Forward<B>(b));
    };
} // namespace ne
