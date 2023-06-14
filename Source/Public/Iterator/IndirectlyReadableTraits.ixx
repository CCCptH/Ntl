export module ntl.iterator.indirectly_readable_traits;
import ntl.type_traits;
import ntl.concepts.core_language;

export namespace ne {
    template<class I>
    struct IndirectlyReadableTraits {};
    template<class I>
    struct IndirectlyReadableTraits<const I>: IndirectlyReadableTraits<I> {};
    template<class Arr>
        requires TestIsArray<Arr>
    struct IndirectlyReadableTraits<Arr> {
        using ValueType = TypeUnCV<TypeRemoveExtent<Arr>>;
    };
    template<class T>
    struct IndirectlyReadableTraits<T*> {
        using ValueType = TypeEnableIf<TestIsObject<T>, TypeUnCV<T>>;
    };

    template<class T>
        requires requires {  typename T::ValueType; }
    struct IndirectlyReadableTraits<T> {
        using ValueType = TypeEnableIf<TestIsObject<typename T::ValueType>, TypeUnCV<typename T::ValueType>>;
    };
    template<class T>
        requires requires { typename T::ElementType; }
    struct IndirectlyReadableTraits<T> {
        using ValueType = TypeEnableIf<TestIsObject<typename T::ElementType>, TypeUnCV<typename T::ElementType>>;
    };

    template<class T>
        requires requires { typename T::ValueType; typename T::ElementType; } and
        ConceptSameAs<TypeUnCV<typename T::ValueType>, TypeUnCV<typename T::ElementType>>
    struct IndirectlyReadableTraits<T> {
        using ValueType = TypeEnableIf<TestIsObject<typename T::ValueType>, TypeUnCV<typename T::ValueType>>;
    };
}