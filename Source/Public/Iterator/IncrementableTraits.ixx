export module ntl.iterator.incrementable_traits;
import ntl.utils;
import ntl.type_traits;
import ntl.concepts.core_language;

export namespace ne {
    template<class I>
    struct IncrementableTraits{};
    template<class I>
        requires TestIsObject<I>
    struct IncrementableTraits<I*> {
        using DifferenceType = ptrdiff_t;
    };
    template<class I>
    struct IncrementableTraits<const I>: IncrementableTraits<I>{};
    template<class I>
        requires ((requires {typename I::DifferenceType;}))
    struct IncrementableTraits<I> {
        using DifferenceType = typename I::DifferenceType;
    };
    template<class I>
        requires ((not requires { typename I::DifferenceType;}) and (not requires { typename I::difference_type;}))
        && requires (const I& a, const I& b) {{a-b} -> ConceptIntegral;}
    struct IncrementableTraits<I> {
        using DifferenceType = TypeMakeSigned<decltype(Declval<I>()- Declval<I>())>;
    };
}