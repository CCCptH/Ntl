export module ntl.functional.invoke;
import ntl.type_traits;
import ntl.utils.forward;
import ntl.utils.declval;
import ntl.utils.get_address;

namespace ne {
    template<class A>
    A& RefWrapperHelper(A& a) noexcept { return a; }
    template<class A>
    void RefWrapperHelper(A&&) = delete;
}
export namespace ne {

    template<class T>
    class RefWrapper;

    template<class T>
    RefWrapper(T&) -> RefWrapper<T>;
    
    template<class A>
    inline constexpr bool TestIsRefWrapper = false;

    template<class U>
    inline constexpr bool TestIsRefWrapper<RefWrapper<U>> = true;
}

namespace ne {
    template<class C, class Pointed, class T1, class ...Args>
    constexpr decltype(auto) InvokeMemPtrImpl(Pointed C::* f, T1 &&t1, Args &&...args) {
        if constexpr (TestIsFunction<Pointed>) {
            if constexpr(TestIsBaseOf<C, TypeDecay<T1>>) {
                return (Forward<T1>(t1).*f)(Forward<Args>(args)...);
            } else if constexpr (TestIsRefWrapper<TypeDecay<T1>>) {
                return (t1.get().*f)(Forward<Args>(args)...);
            } else {
                return ((*Forward<T1>(t1)).*f)(Forward<Args>(args)...);
            }
        } else {
            static_assert(TestIsObject<Pointed> && sizeof...(args) == 0);
            if constexpr (TestIsBaseOf<C, TypeDecay<T1>>)
                return Forward<T1>(t1).*f;
            else if constexpr (TestIsRefWrapper<TypeDecay<T1>>) {
                return t1.get().*f;
            } else {
                return (Forward<T1>(t1)).*f;
            }
        }
    }
    template<class F, class ...Args>
    void InvokeResultHelper(F&& f, Args&& ...args) = delete;

    template <class Base, class T, class Derived, class... Args>
    requires TestIsFunction<T> && TestIsBaseOf<Base, TypeDecay<Derived>>
    auto InvokeResultHelper(T Base::*pmf, Derived&& ref, Args&&... args)
    noexcept(noexcept((Forward<Derived>(ref).*pmf)(Forward<Args>(args)...)))
    -> decltype((Forward<Derived>(ref).*pmf)(Forward<Args>(args)...));

    template <class Base, class T, class RefWrap, class... Args>
    requires TestIsFunction<T> && TestIsRefWrapper<TypeDecay<RefWrap>>
    auto InvokeResultHelper(T Base::*pmf, RefWrap&& ref, Args&&... args)
    noexcept(noexcept((ref.get().*pmf)(Forward<Args>(args)...)))
    -> decltype((ref.get().*pmf)(Forward<Args>(args)...));

    template <class Base, class T, class Pointer, class... Args>
    requires TestIsFunction<T> and (not TestIsRefWrapper<TypeDecay<Pointer>>) and (not TestIsBaseOf<Base, TypeDecay<Pointer>>)
    auto InvokeResultHelper(T Base::*pmf, Pointer&& ptr, Args&&... args)
    noexcept(noexcept(((*Forward<Pointer>(ptr)).*pmf)(Forward<Args>(args)...)))
    ->  decltype(((*Forward<Pointer>(ptr)).*pmf)(Forward<Args>(args)...));

    template <class Base, class T, class Derived>
    requires (not TestIsFunction<T>) and TestIsBaseOf<Base, TypeDecay<Derived>>
    auto InvokeResultHelper(T Base::*pmd, Derived&& ref)
    noexcept(noexcept(Forward<Derived>(ref).*pmd))
    -> decltype(Forward<Derived>(ref).*pmd);

    template <class Base, class T, class RefWrap>
    requires (not TestIsFunction<T>) and TestIsRefWrapper<TypeDecay<RefWrap>>
    auto InvokeResultHelper(T Base::*pmd, RefWrap&& ref)
    noexcept(noexcept(ref.get().*pmd))
    -> decltype(ref.get().*pmd);

    template <class Base, class T, class Pointer>
    requires (not TestIsFunction<T>) and (not TestIsRef<TypeDecay<Pointer>>) and (not TestIsBaseOf<Base, Pointer>)
    auto InvokeResultHelper(T Base::*pmd, Pointer&& ptr)
    noexcept(noexcept((*Forward<Pointer>(ptr)).*pmd))
    -> decltype((*Forward<Pointer>(ptr)).*pmd);

    template <class F, class... Args>
    requires (not TestIsMemberPointer<TypeDecay<F>>)
    auto InvokeResultHelper(F&& f, Args&&... args)
    noexcept(noexcept((Forward<F>(f))(Forward<Args>(args)...)))
    -> decltype((Forward<F>(f))(Forward<Args>(args)...));

    template<class F, class ...Args>
    // requires (not requires { InvokeResultHelper(Declval<F>(), Declval<Args>()...); })
    struct TypeInvokeResultHelper {};

    template<class F, class ...Args>
    requires (requires { InvokeResultHelper(Declval<F>(), Declval<Args>()...); })
    struct TypeInvokeResultHelper<F, Args...> {
        using Type = decltype(InvokeResultHelper(Declval<F>(), Declval<Args>()...));
    };



    template<class F, class ...Args>
    struct TestIsInvocableHelper: FalseConstant {};
    template<class F, class ...Args>
    requires requires { InvokeResultHelper(Declval <F>(), Declval <Args>()...); }
    struct TestIsInvocableHelper<F, Args...> : TrueConstant {};
    template<class F, class ...Args>
    struct TestIsNothrowInvocableHelper: BooleanConstant<TestIsInvocableHelper<F, Args...>::VALUE and noexcept(InvokeResultHelper(Declval<F>(), Declval<Args>()...))> {};

    template<class R, class F, class ...Args>
    struct TestIsInvocableWithReturnTypeHelper: TypeConditional<
            TestIsInvocableHelper<F, Args...>::VALUE,
            TypeConditional<
                TestIsVoid<R>,
                TrueConstant,
                    TypeConditional<
                            TestIsConvertible<decltype(InvokeResultHelper(Declval<F>(), Declval <Args>()...)), R>,
                            TrueConstant,
                            FalseConstant
                    >
            >,
            FalseConstant
    >{};

    template<class X>
    void NothrowInvokeHelper(X) noexcept {};
}

export namespace ne {
    template<class T>
    class RefWrapper
    {
    private:
        T* ptr;
    public:
        using Type = T;
        template<class U>
        requires (not TestIsSame<RefWrapper, TypeUnCVRef<U>>) and requires { RefWrapperHelper(Declval<U>()); }
        constexpr RefWrapper(U&& u)
    		noexcept
            : ptr(GetAddress(RefWrapperHelper<T>(Forward<U>(u))))
            
        {}

        RefWrapper(const RefWrapper&) noexcept = default;
        RefWrapper& operator=(const RefWrapper&) noexcept = default;

        constexpr operator T& () const noexcept { return *ptr; }
        constexpr T& get() const noexcept { return *ptr; }

        template<class ...Args>
        constexpr auto operator()(Args&&...args) const -> decltype(Invoke(this->get(), Forward<Args>(args)...)) {
            return Invoke(this->get(), Forward<Args>(args)...);
        }
    };

    template<class F, class ...Args>
    inline constexpr bool TestIsInvocable = TestIsInvocableHelper<F, Args...>::VALUE;

    template<class F, class ...Args>
    inline constexpr bool TestIsNothrowInvocable = TestIsNothrowInvocableHelper<F, Args...>::VALUE;

    template<class R, class F, class ...Args>
    inline constexpr bool TestIsInvocableWithReturnType = TestIsInvocableWithReturnTypeHelper<R,F,Args...>::VALUE;

    template<class R, class F, class ...Args>
    inline constexpr bool TestIsNothrowInvocableWithReturnType = TestIsNothrowInvocableWithReturnType<R, F, Args...>
            and noexcept(NothrowInvokeHelper<R>(InvokeResultHelper(Declval<F>(), Declval<Args>()...)));

    template<class F, class ...Args>
    using TypeInvokeResult = typename TypeInvokeResultHelper<F, Args...>::Type ;

    // Todo: nothrow impl
    template<class F, class ...Args>
    constexpr TypeInvokeResult<F, Args...> Invoke(F&& func, Args&&...args)
        noexcept(
            TestIsNothrowInvocable<F, Args...>
        )
    {
        if constexpr (TestIsMemberPointer<TypeDecay<F>>) {
            return InvokeMemPtrImpl(func, Forward<Args>(args)...);
        }
        else {
            return (Forward<F>(func))(Forward<Args>(args)...);
        }
    }

    // Todo: nothrow impl
    template<class R, class F, class ...Args>
    constexpr R InvokeWithReturnType (F&& func, Args&&...args)
        noexcept(
            TestIsNothrowInvocableWithReturnType<R, F, Args...>
        )
    {
        if constexpr (TestIsVoid<R>) {
            Invoke(Forward<F>(func), Forward<Args>(args)...);
        }
        else {
            return Invoke(Forward<F>(func), Forward<Args>(args)...);
        }
    }
}