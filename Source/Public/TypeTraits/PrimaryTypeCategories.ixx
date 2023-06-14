module;
#include <type_traits>
export module ntl.type_traits.primary_type_categories;
import ntl.type_traits.base_classes;
import ntl.type_traits.type_modifications;
import ntl.type_traits.type_relationships;

namespace ne {
    template <class A> struct TestIsNoCVSignedIntegralHelper : FalseConstant {};

    template <>
    struct TestIsNoCVSignedIntegralHelper<signed char> : TrueConstant {};
    template <>
    struct TestIsNoCVSignedIntegralHelper<signed short> : TrueConstant {};
    template <> struct TestIsNoCVSignedIntegralHelper<signed int> : TrueConstant {};
    template <>
    struct TestIsNoCVSignedIntegralHelper<signed long> : TrueConstant {};
    template <>
    struct TestIsNoCVSignedIntegralHelper<signed long long> : TrueConstant {};

    template <class A> struct TestIsNoCVUnsignedIntegralHelper : FalseConstant {};

    template <>
    struct TestIsNoCVUnsignedIntegralHelper<unsigned char> : TrueConstant {};
    template <>
    struct TestIsNoCVUnsignedIntegralHelper<unsigned short> : TrueConstant {};
    template <>
    struct TestIsNoCVUnsignedIntegralHelper<unsigned int> : TrueConstant {};
    template <>
    struct TestIsNoCVUnsignedIntegralHelper<unsigned long> : TrueConstant {};
    template <>
    struct TestIsNoCVUnsignedIntegralHelper<unsigned long long> : TrueConstant {};

    template <class A> struct TestIsNoCVIntegralHelper : FalseConstant {};
    template <> struct TestIsNoCVIntegralHelper<signed char> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<signed short> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<signed int> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<signed long> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<signed long long> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<unsigned char> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<unsigned short> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<unsigned int> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<unsigned long> : TrueConstant {};
    template <>
    struct TestIsNoCVIntegralHelper<unsigned long long> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<char> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<wchar_t> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<char8_t> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<char16_t> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<char32_t> : TrueConstant {};
    template <> struct TestIsNoCVIntegralHelper<bool> : TrueConstant {};

    template <class A> struct TestIsNoCVFloatingPointHelper : FalseConstant {};
    template <> struct TestIsNoCVFloatingPointHelper<float> : TrueConstant {};
    template <> struct TestIsNoCVFloatingPointHelper<double> : TrueConstant {};
    template <> struct TestIsNoCVFloatingPointHelper<long double> : TrueConstant {};

    template <class A> struct TestIsArrayHelper : FalseConstant {};
    template <class A> struct TestIsArrayHelper<A[]> : TrueConstant {};
    template <class A, size_t N> struct TestIsArrayHelper<A[N]> : TrueConstant {};

    template <class> struct TestIsFunctionHelper : FalseConstant {};

    // 对常规函数的特化
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...)> : TrueConstant {};

    // 对变参数函数，如 std::printf 的特化
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...)> : TrueConstant {};

    // 对拥有 cv 限定符的函数类型的特化
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) volatile> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const volatile> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) volatile> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const volatile> : TrueConstant {};

    // 对拥有引用限定符的函数类型的特化
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) &> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const &> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) volatile &> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const volatile &> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) &> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const &> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) volatile &> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const volatile &> : TrueConstant {
    };
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) &&> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const &&> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) volatile &&> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const volatile &&> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) &&> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const &&> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) volatile &&> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const volatile &&>
        : TrueConstant {};

    // 对所有以上的 noexcept 版本的特化（ C++17 及之后）

    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) volatile noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const volatile noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) volatile noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const volatile noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) & noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const & noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) volatile & noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const volatile & noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) & noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const & noexcept> : TrueConstant {
    };
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) volatile & noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const volatile & noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) && noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const && noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) volatile && noexcept> : TrueConstant {
    };
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args...) const volatile && noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) && noexcept> : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const && noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) volatile && noexcept>
        : TrueConstant {};
    template <class Ret, class... Args>
    struct TestIsFunctionHelper<Ret(Args..., ...) const volatile && noexcept>
        : TrueConstant {};

    // 在没有编译器帮助下, 无法知道一个类型是否是union
    template <class A>
    struct TestIsUnionHelper : BooleanConstant<std::is_union_v<A>>{};

    template <class A>
    struct TestIsEnumHelper : BooleanConstant<std::is_enum_v<A>> {};

    template <class T>
    BooleanConstant<(!TestIsUnionHelper<T>::VALUE)> TryClassInvoke(int T::*);
    template <class T> FalseConstant TryClassInvoke(...);

    template <class T>
    struct TestIsClassHelper :
        decltype(TryClassInvoke<T>(nullptr))
    {
    };

    template <class T> struct TestIsLRefHelper : FalseConstant {};
    template <class T> struct TestIsLRefHelper<T &> : TrueConstant {};

    template <class T> struct TestIsRRefHelper : FalseConstant {};
    template <class T> struct TestIsRRefHelper<T &&> : TrueConstant {};

    template <class T> struct TestIsNoCVPointerHelper : FalseConstant {};
    template <class T> struct TestIsNoCVPointerHelper<T *> : TrueConstant {};
    template <class T> struct TestIsNoCVPointerHelper<const T *> : TrueConstant {};
    template <class T> struct TestIsNoCVPointerHelper<volatile T *> : TrueConstant {};
    template <class T> struct TestIsNoCVPointerHelper<const volatile T*> : TrueConstant {};
    template <class T> struct TestIsPointerHelper : TestIsNoCVPointerHelper <TypeUnCV<T>> {};

}

export namespace ne {

    /**
     * @brief Same as std::is_integral
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsIntegral = TestIsNoCVIntegralHelper<TypeUnCV<T>>::VALUE;
    /**
     * @brief Same as std::is_floating_point
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsFloatingPoint = TestIsNoCVFloatingPointHelper<TypeUnCV<T>>::VALUE;
    /**
     * @brief Same as std::is_array
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsArray = TestIsArrayHelper<T>::VALUE;
    /**
     * @brief Same as std::is_void
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsVoid = TestIsSame<void, TypeUnCV<A>>;
    /**
     * @brief Same as std::is_null_pointer
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsNullptr = TestIsSame<decltype(nullptr), TypeUnCV<A>>;
    /**
     * @brief Same as std::is_function
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsFunction = TestIsFunctionHelper<T>::VALUE;
    /**
     * @brief Same as std::is_union
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsUnion = TestIsUnionHelper<T>::VALUE;
    /**
     * @brief Same as std::is_enum
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsEnum = TestIsEnumHelper<T>::VALUE;
    /**
     * @brief Same as std::is_class
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsClass = TestIsClassHelper<T>::VALUE;
    /**
     * @brief Same as std::is_lvalue_reference
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsLRef = TestIsLRefHelper<T>::VALUE;
    /**
     * @brief Same as std::is_rvalue_reference
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsRRef = TestIsRRefHelper<T>::VALUE;
    /**
     * @brief Same as std::is_pointer
     * @tparam T
     */
    template<class T>
    inline constexpr bool TestIsPointer = TestIsPointerHelper<T>::VALUE;

}
