export module ntl.concepts.core_language;
import ntl.type_traits.type_relationships;
import ntl.type_traits.miscellaneous_transformations;
import ntl.type_traits.type_modifications;
import ntl.type_traits.type_properties;
import ntl.type_traits.primary_type_categories;
import ntl.type_traits.support_operations;
import ntl.utils.forward;
import ntl.utils.declval;

namespace ne
{
    template<class I>
    using WithLRef = I&;
}

export namespace ne {
    template<class A, class B>
    concept ConceptSameAs = TestIsSame<A, B>&&TestIsSame<B, A>;

    template<class Derived, class Base>
    concept ConceptDeriveFrom = TestIsBaseOf<Base, Derived> &&
    TestIsConvertible<const volatile Derived*, const volatile Base*>;

    template<class From, class To>
    concept ConceptConvertibleTo =
    TestIsConvertible<From, To> &&
    requires {
        static_cast<To>(Declval<From>());
    };

    template<class A, class B>
    concept ConceptCommonRefWith =
    ConceptSameAs<
            TypeCommonReference<A, B>, TypeCommonReference<B, A>
    > &&
    ConceptConvertibleTo<
            A, TypeCommonReference<A, B>
    > &&
    ConceptConvertibleTo<
            B, TypeCommonReference<A, B>
    >;


    template<class A, class B>
    concept ConceptCommonWith =
    ConceptSameAs<TypeCommon<A, B>, TypeCommon<B, A>> &&
    requires {
        static_cast<TypeCommon<A, B>>(Declval<A>());
        static_cast<TypeCommon<A, B>>(Declval<B>());
    } &&
    ConceptCommonRefWith<
            TypeAddLRef<const A>, TypeAddLRef<const B>
    > &&
            ConceptCommonRefWith<
            TypeAddLRef<TypeCommon<A, B>>, TypeCommonReference<
    TypeAddLRef<const A>, TypeAddLRef<const B>
    >
    >;

    template<class T>
    concept ConceptIntegral = TestIsIntegral<T>;

    template<class T>
    concept ConceptIntegralLike = ConceptIntegral<T> && (!ConceptSameAs<T, bool>);

    template<class T>
    concept ConceptSignedIntegral = ConceptIntegral<T> && TestIsSigned<T>;

    template<class T>
    concept ConceptUnsignedIntegral = ConceptIntegral<T> && TestIsUnsigned<T>;

    template<class T>
    concept ConceptFloatingPoint = TestIsFloatingPoint<T>;

    template<class Lhs, class Rhs>
    concept ConceptAssignableFrom =
            TestIsLRef<Lhs> &&
            ConceptCommonRefWith<
                const TypeUnRef<Lhs>&,
                const TypeUnRef<Rhs>&
            > &&
            requires (Lhs lhs, Rhs&& rhs) {
                { lhs = Forward<Rhs>(rhs) } -> ConceptSameAs<Lhs>;
            };

    template<class A>
    concept ConceptDestructible = TestIsNothrowDestructible<A>;

    template<class T, class...Args>
    concept ConceptConstructibleFrom =
            ConceptDestructible<T> &&
            TestIsConstructible<T, Args...>;

    template<class A>
    concept ConceptDefaultInitializable =
            ConceptConstructibleFrom<A> &&
            requires { A{}; } &&
            requires { ::new (static_cast<void*>(nullptr)) A; };

    template<class A>
    concept ConceptMoveConstructible =
            ConceptConstructibleFrom<A, A> &&
            ConceptConvertibleTo<A, A>;

    template<class A>
    concept ConceptCopyConstructible =
            ConceptMoveConstructible<A> &&
            ConceptConstructibleFrom<A, A&> &&
            ConceptConvertibleTo<A&, A> &&
            ConceptConstructibleFrom<A, const A&> &&
            ConceptConvertibleTo<const A&, A> &&
            ConceptConstructibleFrom<A, const A> &&
            ConceptConvertibleTo<const A, A>;

    template<class I>
    concept ConceptReferenceable = requires {
        typename WithLRef<I>;
    };
}