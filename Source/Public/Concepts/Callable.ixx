export module ntl.concepts.callable;

import ntl.concepts.core_language;
import ntl.concepts.comparision;
import ntl.utils.forward;
import ntl.functional.invoke;

export namespace ne {
    template<class Func, class ...Args>
    concept ConceptInvocable =
            requires(Func&& f, Args&&...args) {
                Invoke(Forward<Func>(f), Forward<Args>(args)...);
            };

    // 要求不修改可调用参数
    template<class Func, class...Args>
    concept ConceptRegularInvocable = ConceptInvocable<Func, Args...>;

    template<class Func, class...Args>
    concept ConceptPredicate = ConceptRegularInvocable<Func, Args...> and
            requires (Func&& f, Args&&...args) {
                { Invoke(Forward<Func>(f), Forward<Args>(args)...) } -> ConceptBooleanTestable;
            };

    template<class R, class A, class B>
    concept ConceptRelation = ConceptPredicate<R, A, B> and
            ConceptPredicate<R, A, A> and
            ConceptPredicate<R, B, A> and
            ConceptPredicate<R, B, B> ;

    template<class R, class A, class B>
    concept ConceptEquivalenceRelation = ConceptRelation<R, A, B>;

    template<class R, class A, class B>
    concept ConceptStrictWeakOrder = ConceptRelation<R, A, B>;
}