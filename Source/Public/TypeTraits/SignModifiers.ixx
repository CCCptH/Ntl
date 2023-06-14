export module ntl.type_traits.sign_modifiers;
import ntl.utils.nint;
import ntl.type_traits.primary_type_categories;
import ntl.type_traits.operation_on_traits;
import ntl.type_traits.type_properties;
import ntl.type_traits.type_modifications;
namespace ne
{
    template <class A, class B, bool = TestIsConst<TypeUnRef<A>>,
            bool = TestIsVolatile<TypeUnRef<A>> >
    struct TypeApplyCVHelper {
        using Type = B;
    };
    template <class A, class B> struct TypeApplyCVHelper<A, B, true, false> {
        using Type = const B;
    };
    template <class A, class B> struct TypeApplyCVHelper<A, B, false, true> {
        using Type = volatile B;
    };
    template <class A, class B> struct TypeApplyCVHelper<A, B, true, true> {
        using Type = const volatile B;
    };
    template <class A, class B> struct TypeApplyCVHelper<A &, B, true, false> {
        using Type = const B &;
    };
    template <class A, class B> struct TypeApplyCVHelper<A &, B, false, true> {
        using Type = volatile B &;
    };
    template <class A, class B> struct TypeApplyCVHelper<A &, B, true, true> {
        using Type = const volatile B &;
    };
    template <class A, class B> struct TypeApplyCVHelper<A &, B, false, false> {
        using Type = B &;
    };

    template <class A>
    requires(TestIsIntegral<A> || TestIsEnum<A>) && (!TestIsSame<A, bool>) &&
            (sizeof(A) == 1 || sizeof(A) == 2 || sizeof(A) == 4 || sizeof(A) == 8)
    struct TypeMakeSignedNoCVHelper {
        using Type = TypeConditional<
        sizeof(A) == 1, int8,
        TypeConditional<sizeof(A) == 2, int16,
                        TypeConditional<sizeof(A) == 4, int32, int64>>>;
    };
    // template <> struct TypeMakeSignedNoCVHelper<int8> {
    //     using Type = int8;
    // };
    // template <> struct TypeMakeSignedNoCVHelper<int16> {
    //     using Type = int16;
    // };
    // template <> struct TypeMakeSignedNoCVHelper<int32> {
    //     using Type = int32;
    // };
    // template <> struct TypeMakeSignedNoCVHelper<int64> {
    //     using Type = int64;
    // };
    // template <> struct TypeMakeSignedNoCVHelper<uint8> {
    //     using Type = int8;
    // };
    // template <> struct TypeMakeSignedNoCVHelper<uint16> {
    //     using Type = int16;
    // };
    // template <> struct TypeMakeSignedNoCVHelper<uint32> {
    //     using Type = int32;
    // };
    // template <> struct TypeMakeSignedNoCVHelper<uint64> {
    //     using Type = int64;
    // };

    template <class A>
    requires(TestIsIntegral<A> || TestIsEnum<A>) && (!TestIsSame<A, bool>) &&
            (sizeof(A) == 1 || sizeof(A) == 2 || sizeof(A) == 4 || sizeof(A) == 8)
    struct TypeMakeUnsignedNoCVHelper {
        using Type = TypeConditional<
            sizeof(A) == 1, uint8,
            TypeConditional<sizeof(A) == 2, uint16,
                            TypeConditional<sizeof(A) == 4, uint32, uint64>>>;
    };

    // template <> struct TypeMakeUnsignedNoCVHelper<int8> {
    // using Type = uint8;
    // };
    // template <> struct TypeMakeUnsignedNoCVHelper<int16> {
    // using Type = uint16;
    // };
    // template <> struct TypeMakeUnsignedNoCVHelper<int32> {
    // using Type = uint32;
    // };
    // template <> struct TypeMakeUnsignedNoCVHelper<int64> {
    // using Type = uint64;
    // };
    // template <> struct TypeMakeUnsignedNoCVHelper<uint8> {
    // using Type = uint8;
    // };
    // template <> struct TypeMakeUnsignedNoCVHelper<uint16> {
    // using Type = uint16;
    // };
    // template <> struct TypeMakeUnsignedNoCVHelper<uint32> {
    // using Type = uint32;
    // };
    // template <> struct TypeMakeUnsignedNoCVHelper<uint64> {
    // using Type = uint64;
    // };

    template <class A> struct TypeMakeSignedHelper {
        using Type = typename TypeApplyCVHelper<A, typename TypeMakeSignedNoCVHelper<TypeUnCV<A>>::Type >::Type;
    };

    template <class A> struct TypeMakeUnsignedHelper {
        using Type = typename TypeApplyCVHelper<A, typename TypeMakeUnsignedNoCVHelper<TypeUnCV<A>>::Type >::Type;
    };
}
export namespace ne {
    template<class T>
    using TypeMakeSigned = typename TypeMakeSignedHelper<T>::Type;
    template<class T>
    using TypeMakeUnsigned = typename TypeMakeUnsignedHelper<T>::Type;
}