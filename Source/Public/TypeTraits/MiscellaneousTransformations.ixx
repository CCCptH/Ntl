module;
#include <type_traits>
export module ntl.type_traits.miscellaneous_transformations;

import ntl.type_traits.type_modifications;
import ntl.type_traits.operation_on_traits;
import ntl.type_traits.primary_type_categories;
import ntl.type_traits.base_classes;
import ntl.type_traits.composite_type_categories;
import ntl.utils.declval;
import ntl.type_traits.type_relationships;

namespace ne {
    template<class A>
    struct TypeDecayHelper {
    private:
        using U = TypeUnRef<A>;
    public:
        using Type = TypeConditional<
            TestIsArray<U>,
            TypeRemoveAllExtents<U>*,
                TypeConditional<
                    TestIsFunction<U>,
                    TypeAddPointer<U>,
                    TypeUnCV<U>
                >
        >;
    };

    template<class ...T>
    struct TypeCommonHelper;
    template<class A>
    struct TypeCommonHelper<A>{
        using Type = typename TypeDecayHelper<A>::Type;
    };
    template<class A, class B>
    struct TypeCommonHelper<A, B>{
        using Type = decltype(ALWAYS_TRUE<bool> ? Declval<A>() : Declval<B>());
    };
    template<class A, class B, class ...C>
    struct TypeCommonHelper<A, B, C...> {
        using Type = typename TypeCommonHelper<typename TypeCommonHelper<A, B>::Type , C...>::Type;
    };

        /**
     * https://zh.cppreference.com/w/cpp/types/common_reference
     * common_reference
     * 1. 若 sizeof...(T) 为零，则无成员 type
     * 2. 若 sizeof...(T) 为一（即 T... 仅含一个类型 T0 ），则成员 type 指名与 T0 相同的类型。
     * 3. 若 sizeof...(T) 为二（即 T... 含二个类型 T1 和 T2 ），则：
     *     a) 若 T1 和 T2 都是引用类型，而 T1 和 T2 的简单共用引用类型 S 存在，则成员类型 type 指名 S ；
     *     b) 否则，若 std::basic_common_reference<std::remove_cvref_t<T1>, std::remove_cvref_t<T2>, T1Q, T2Q>::type 存在，
     *     其中 TiQ 是一元别名模板，满足 TiQ<U> 为 U 带上 Ti 的 cv 及引用限定符，则成员类型 type 指名该类型；
     *     c) 否则，若 decltype(false? val<T1>() : val<T2>()) 是合法类型，其中 val 为函数模板 template<class T> T val(); ，
     *     则成员类型 type 指名该类型；
     *     d) 否则，若 std::common_type_t<T1, T2> 为合法类型，则成员类型 type 代表该类型
     *     e) 否则，无成员 type 。
     *  4. 若 sizeof...(T) 大于二（即 T... 由 T1, T2, R... 组成），则若 std::common_reference_t<T1, T2> 存在，且
     *      std::common_reference_t<std::common_reference_t<T1, T2>, R...> 存在，则成员类型 type 指代它。所有其他情况下，
     *      无成员类型 type 。
     *  二个引用类型 T1 和 T2 的简单共用引用类型定义如下：
     *  1. 若 T1 为 cv1 X & 而 T2 为 cv2 Y & （即都是左值引用）：则其简单共用引用类型为
     *      decltype(false? std::declval<cv12 X &>() : std::declval<cv12 Y &>()) ，其中 cv12 为 cv1 与 cv2 的联合，
     *      若该类型存在且为引用类型；
     *  2. 若 T1 a与 T2 均为右值引用类型：若 T1 & 和 T2 & 的简单共用引用类型（按照前一条确定）存在，则令 C 代表该类型的对应右值引用类型。
     *      若 std::is_convertible_v<T1, C> 与 std::is_convertible_v<T2, C> 均为 true ，则 T1 与 T2 的简单共用引用类型为 C 。
     *  3. 否则，二个类型之一必须为左值引用类型 A & 而另一个必须为右值引用类型 B && （ A 与 B 可为 cv 限定）。令 D 代表
     *      A & 与 B const & 的简单共用引用类型，若它存在。若 D 存在且 std::is_convertible_v<B &&, D> 为 true ，
     *      则简单共用引用类型为 D 。
     *  4. 否则，无简单共用引用类型。
     */
    template<class From, class To>
    struct TypeCopyCVHelper{
        using Type = To;
    };

    template<class From, class To>
    struct TypeCopyCVHelper<const From, To> {
        using Type = TypeAddConst<To>;
    };

    template<class From, class To>
    struct TypeCopyCVHelper<volatile From, To> {
        using Type = TypeAddVolatile<To>;
    };

    template<class From, class To>
    struct TypeCopyCVHelper<const volatile From, To> {
        using Type = TypeAddCV<To>;
    };

    template<class A, class B>
    using TypeSimpleCommonRefUnCVCond1 = decltype(AlWAYS_FALSE<bool> ? Declval<A(&)()>()(): Declval<B(&)()>()());
    template<class A, class B>
    using TypeSimpleCommonRefCond1 = TypeSimpleCommonRefUnCVCond1<typename TypeCopyCVHelper<A, B>::Type &, typename TypeCopyCVHelper<B, A>::Type &>;

    template<class A, class B, class X = TypeUnRef<A>, class Y = TypeUnRef<B>>
    struct TypeSimpleCommonRefHelper;
    template<class A, class B>
    using TypeSimpleCommonRef = typename TypeSimpleCommonRefHelper<A, B>::Type;
    template<class A, class B, class X, class Y>
    requires requires {
        typename TypeSimpleCommonRefCond1<X, Y>;
    } && TestIsRef<TypeSimpleCommonRefCond1<X, Y>>
    struct TypeSimpleCommonRefHelper<A&, B&, X, Y> {
        using Type = TypeSimpleCommonRefCond1<X, Y>;
    };

    template<class A, class B>
    using TypeSimpleCommonRRefC = TypeUnRef<TypeSimpleCommonRef<A&, B&>>&&;

    template<class A, class B, class X, class Y>
    requires requires { typename TypeSimpleCommonRRefC<X, Y>; } &&
        TestIsConvertible<A&&, TypeSimpleCommonRRefC<X, Y>> &&
        TestIsConvertible<B&&, TypeSimpleCommonRRefC<X, Y>>
    struct TypeSimpleCommonRefHelper<A&&, B&&, X, Y> {
        using Type = TypeSimpleCommonRRefC<X, Y>;
    };

    template<class A, class B>
    using TypeSimpleCommonRefD = TypeSimpleCommonRef<const A&, B&>;

    template<class A, class B, class X, class Y>
    requires requires { typename TypeSimpleCommonRefD<X, Y>; } &&
        TestIsConvertible<A&&, TypeSimpleCommonRefD<X, Y>>
    struct TypeSimpleCommonRefHelper<A&&, B&, X, Y> {
        using Type = TypeSimpleCommonRefD<X, Y>;
    };

    template<class A, class B, class X, class Y>
    struct TypeSimpleCommonRefHelper<A&, B&&, X, Y>: TypeSimpleCommonRefHelper<B&&, A&>{};

    template<class A, class B, class X, class Y>
    struct TypeSimpleCommonRefHelper{};

    template<class...Ts>
    struct TypeCommonReferenceHelper{};
    template<>
    struct TypeCommonReferenceHelper<>{};
    template<class T>
    struct TypeCommonReferenceHelper<T>{
        using Type = T;
    };

    template<class A, class B>
    struct TypeCommonRefSubBullet3Helper;
    template<class A, class B>
    struct TypeCommonRefSubBullet2Helper: TypeCommonRefSubBullet3Helper<A, B>{};
    template<class A, class B>
    struct TypeCommonRefSubBullet1Helper: TypeCommonRefSubBullet2Helper<A, B>{};

    template<class A, class B>
    struct TypeCommonReferenceHelper<A, B>: TypeCommonRefSubBullet1Helper<A, B>{};

    template<class A, class B>
    requires TestIsRef<A> && TestIsRef<B> && requires { typename TypeSimpleCommonRef<A, B>; }
    struct TypeCommonRefSubBullet1Helper<A, B>
    {
        using Type = TypeSimpleCommonRef<A, B>;
    };

    template<class A, class B>
        requires requires { typename TypeSimpleCommonRefUnCVCond1<A, B>; }
    struct TypeCommonRefSubBullet3Helper<A, B> {
        using Type = TypeSimpleCommonRefUnCVCond1<A, B>;
    };
    template<class A, class B>
    struct TypeCommonRefSubBullet3Helper: TypeCommonHelper<A, B> {};

    template<class A, class B, class C, class...Ts>
    requires requires {
        typename TypeCommonReferenceHelper<A, B>::Type;
    }
    struct TypeCommonReferenceHelper<A, B, C, Ts...> {
        using Type = typename TypeCommonReferenceHelper<typename TypeCommonReferenceHelper<A, B>::Type, C, Ts...>::Type;
    };
}

export namespace ne
{

    template<class A>
    using TypeDecay = typename TypeDecayHelper<A>::Type ;
    template<class ...A>
    using TypeCommon = typename TypeCommonHelper<A...>::Type;

    template<class, class, template<class>class, template<class>class>
    struct TypeSpecBasicCommonReference{};

    // template<class...Ts>
    // using TypeCommonReference = typename TypeCommonReferenceHelper<Ts...>::Type;
    // Todo:
    // basic_common_reference

    // 有bug
    template<class...Ts>
    using TypeCommonReference = std::common_reference_t<Ts...>;
}