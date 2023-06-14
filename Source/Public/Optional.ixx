export module ntl.optional;
export import ntl.utils.inplace;
import ntl.utils;
import ntl.type_traits;
import ntl.type_traits.type_modifications;
import ntl.memory.allocator;
import ntl.functional.invoke;
import ntl.compare;
import ntl.exceptions;

export namespace ne
{
    template<class T>
    class Optional;

    class BadOptionalAccess: RuntimeError
    {
    public:
        using RuntimeError::RuntimeError;
    };
}

namespace ne
{

    template<class T>
    struct TestIsOptionalHelper: FalseConstant {};
    template<class T>
    struct TestIsOptionalHelper<Optional<T>> : TrueConstant {};
    template<class T>
    inline constexpr bool TestIsOptional = TestIsOptionalHelper<T>::VALUE;
    struct Nullopt {};
}

export namespace ne
{
    inline constexpr Nullopt NULLOPT{};
    template<class T>
    class Optional
    {
        static_assert(TestIsObject<T>, "[ntl.optional] Instantiation of optional with a non-object type is undefined behavior");
        static_assert(not TestIsArray<T>, "[ntl.optional] Instantiation of optional with a array type is undefined behavior");
    public:
        using ValueType = T;
    private:
        template<class> friend class Optional;
        bool has_value;
        [[no_unique_address]]
        alignas(T) uint8 layout[sizeof(T)];
        T* ptr() { return reinterpret_cast<T*>(&layout[0]); }
        const T* ptr() const { return reinterpret_cast<const T*>(&layout[0]); }
    public:
        /**
         * @brief Construct a optional that does not contain a value
        */
        constexpr Optional():has_value(false){}
        /**
         * @brief Construct a optional that does not contain a value
         * @param Nullopt: NULLOPT
        */
        constexpr Optional(Nullopt):has_value(false){}

        constexpr Optional(const Optional& o) = delete;
        constexpr Optional(const Optional& o)
            noexcept (TestIsNothrowCopyConstructible<T>)
            requires TestIsCopyConstructible<T> and TestIsTriviallyCopyConstructible<T>
        = default;
        constexpr Optional(const Optional& o)
            noexcept (TestIsNothrowCopyConstructible<T>)
            requires TestIsCopyConstructible<T> and (not TestIsTriviallyCopyConstructible<T>)
                : has_value(o.has_value)
        {
            if(o.has_value)
            {
                Construct(ptr(), *o.ptr());
            }
        }
        constexpr Optional(Optional&& o)
            noexcept (TestIsNothrowMoveConstructible<T>)
            requires TestIsMoveConstructible<T>and TestIsTriviallyMoveConstructible<T>
        = default;
        constexpr Optional(Optional&& o)
            noexcept (TestIsNothrowMoveConstructible<T>)
            requires TestIsMoveConstructible<T> and (not TestIsTriviallyMoveConstructible<T>)
        : has_value(o.has_value)
        {
            if (o.has_value)
            {
                Construct(ptr(), Move(*o.ptr()));
            }
        }

        template<class U>
        constexpr explicit(not TestIsConvertible<const U&, T>)
        Optional(const Optional<U>& o)
            requires TestIsConstructible<T, const U&> and
            (not TestIsConstructible<T, Optional<U>&>) and
            (not TestIsConstructible<T, const Optional<U>&>) and
            (not TestIsConstructible<T, Optional<U>&&>) and
            (not TestIsConstructible<T, const Optional<U>&&>)and
            (not TestIsConvertible<Optional<U>&, T>) and
            (not TestIsConvertible<const Optional<U>&, T>)and
            (not TestIsConvertible<Optional<U>&&, T>)and
            (not TestIsConvertible<const Optional<U>&&, T>)
                : has_value(o.has_value)
        {
            if(has_value)
            {
                Construct(ptr(), *o.ptr());
            }
        }

        template<class U>
        constexpr explicit(not TestIsConvertible<U&&, T>)
            Optional(Optional<U>&& o)
            requires TestIsConstructible<T, const U&> and
        (not TestIsConstructible<T, Optional<U>&>) and
            (not TestIsConstructible<T, const Optional<U>&>) and
            (not TestIsConstructible<T, Optional<U>&&>) and
            (not TestIsConstructible<T, const Optional<U>&&>) and
            (not TestIsConvertible<Optional<U>&, T>) and
            (not TestIsConvertible<const Optional<U>&, T>) and
            (not TestIsConvertible<Optional<U>&&, T>) and
            (not TestIsConvertible<const Optional<U>&&, T>)
            : has_value(o.has_value)
        {
            if (has_value)
            {
                Construct(ptr(), *o.ptr());
            }
        }

        template<class...Args>
            requires TestIsConstructible<T, Args...>
        constexpr Optional(Inplace, Args&...args)
            noexcept(TestIsNothrowConstructible<T, Args...>)
            :has_value(true)
        {
            Construct(ptr(), Forward<Args>(args)...);
        }

        template<class U=T>
            requires (not TestIsOptional<TypeUnCVRef<U>>) and (not TestIsSame<TypeUnCVRef<U>, Inplace>) and
            TestIsConstructible<T, U&&>
        constexpr explicit(not TestIsConvertible<U&&, T>) Optional(U&& u)
            : has_value(true)
        {
            Construct(ptr(), Forward<U>(u));
        }

        constexpr ~Optional() requires TestIsTriviallyDestructible<T> = default;
        constexpr ~Optional() requires (not TestIsTriviallyDestructible<T>)
        {
            if (has_value)
                Destruct(ptr());
            has_value = false;
        }

        constexpr Optional& operator=(Nullopt) noexcept
        {
            has_value = false;
            return *this;
        }

        constexpr Optional& operator=(const Optional& opt) = delete;
        constexpr Optional& operator=(const Optional& opt)
            requires TestIsCopyConstructible<T> and
            TestIsCopyAssignable<T> and
            ((not TestIsTriviallyCopyConstructible<T>) or (not TestIsTriviallyCopyAssignable<T>))
        {
            if (has_value)
            {
                if (opt.has_value)
                {
                    (*ptr()) = (*opt.ptr());
                }
                else
                {
                    Destruct(ptr());
                    has_value = false;
                }
            }
            else
            {
                if (opt.has_value)
                {
                    Construct(ptr(), *opt.ptr());
                    has_value = true;
                }
            }
            return *this;
        }
        constexpr Optional& operator=(const Optional& opt)
        requires (TestIsCopyConstructible<T>and
        TestIsCopyAssignable <T>and
                TestIsTriviallyCopyConstructible<T> and TestIsTriviallyCopyAssignable<T>)
        = default;
        constexpr Optional& operator=(Optional&& opt) noexcept = delete;
        constexpr Optional& operator=(Optional&& opt)
			noexcept(TestIsNothrowMoveAssignable<T> and TestIsNothrowMoveConstructible<T>)
            requires TestIsMoveConstructible<T>and
			TestIsMoveAssignable<T> and
            ((not TestIsTriviallyMoveConstructible<T>) or (not TestIsTriviallyMoveAssignable<T>))
        {
            if (has_value)
            {
                if (opt.has_value)
                {
                    (*ptr()) = Move((*opt.ptr()));
                }
                else
                {
                    Destruct(ptr());
                    has_value = false;
                }
            }
            else
            {
                if (opt.has_value)
                {
                    Construct(ptr(), Move(*(opt.ptr())));
                    has_value = true;
                }
            }
            return *this;
        }
        constexpr Optional& operator=(Optional&& opt)
            requires (TestIsMoveConstructible<T>and
        TestIsMoveAssignable <T>and
            TestIsTriviallyMoveConstructible<T>and TestIsTriviallyMoveAssignable<T>)
    	= default;

        template<class U>
        constexpr Optional& operator=(U&& u)
			requires TestIsConstructible<T, U> and
			TestIsAssignable<T&, U> and
			(not TestIsSame<TypeUnCVRef<U>, Optional>) and
			((not TestIsScalar<T>) or 
			(not TestIsSame<TypeDecay<U>, T>))
        {
	        if (has_value)
	        {
                (*ptr()) = Forward<U>(u);
	        }
            else
            {
                Construct(ptr(), Forward<U>(u));
                has_value = true;
            }
            return *this;
        }

        template<class U>
        constexpr Optional& operator=(const Optional<U>& opt)
			requires TestIsConstructible<T, const U&> and
			TestIsAssignable<T&, const U&> and
			(not TestIsConstructible<T, Optional<U>&>) and
    		(not TestIsConstructible<T, const Optional<U>&>) and
    		(not TestIsConstructible<T, Optional<U>&&>) and
    		(not TestIsConstructible<T, const Optional<U>&&>) and
			(not TestIsAssignable<T&, Optional<U>&>) and
            (not TestIsAssignable<T&, const Optional<U>&>) and
    		(not TestIsAssignable<T&, Optional<U>&&>) and
    		(not TestIsAssignable<T&, const Optional<U>&&>) and
			(not TestIsConvertible<Optional<U>&, T>) and
            (not TestIsConvertible<const Optional<U>&, T>) and
            (not TestIsConvertible<Optional<U>&&, T>) and
    		(not TestIsConvertible<const Optional<U>&&, T>)
        {
            if (has_value)
            {
                if (opt.has_value)
                {
                    (*ptr()) = (*opt.ptr());
                }
                else
                {
                    Destruct(ptr());
                    has_value = false;
                }
            }
            else
            {
                if (opt.has_value)
                {
                    Construct(ptr(), *opt.ptr());
                    has_value = true;
                }
            }
        	return *this;
        }

        template<class U>
        constexpr Optional& operator=(Optional<U>&& opt)
            requires TestIsConstructible<T, U>and
        TestIsAssignable<T&, U> and
            (not TestIsConstructible<T, Optional<U>&>) and
            (not TestIsConstructible<T, const Optional<U>&>) and
            (not TestIsConstructible<T, Optional<U>&&>) and
            (not TestIsConstructible<T, const Optional<U>&&>) and
            (not TestIsAssignable<T&, Optional<U>&>) and
            (not TestIsAssignable<T&, const Optional<U>&>) and
            (not TestIsAssignable<T&, Optional<U>&&>) and
            (not TestIsAssignable<T&, const Optional<U>&&>) and
            (not TestIsConvertible<Optional<U>&, T>) and
            (not TestIsConvertible<const Optional<U>&, T>) and
            (not TestIsConvertible<Optional<U>&&, T>) and
            (not TestIsConvertible<const Optional<U>&&, T>)
        {
            if (has_value)
            {
                if (opt.has_value)
                {
                    (*ptr()) = Move(*opt.ptr());
                }
                else
                {
                    Destruct(ptr());
                    has_value = false;
                }
            }
            else
            {
                if (opt.has_value)
                {
                    Construct(ptr(), Move(*opt.ptr()));
                    has_value = true;
                }
            }
            return *this;
        }

        constexpr const T* operator->() const noexcept
        {
            Assert(has_value, "[ntl.optional] Optional does not contain a value");
            return ptr();
        }
        constexpr T* operator->() noexcept
        {
            Assert(has_value, "[ntl.optional] Optional does not contain a value");
            return ptr();
        }

        constexpr const T& operator*() const noexcept
        {
            Assert(has_value, "[ntl.optional] Optional does not contain a value");
            return *ptr();
        }
        constexpr T& operator*() noexcept
        {
            Assert(has_value, "[ntl.optional] Optional does not contain a value");
            return *ptr();
        }

        constexpr explicit operator bool() const noexcept
        {
            return has_value;
        }
        constexpr bool hasValue() const noexcept
        {
            return has_value;
        }

        constexpr T& value() noexcept
        {
            // TODO: in stl, if no value, throw an exception
            //Assert(has_value, "[ntl.optional] Optional does not contain a value");
            if (!has_value)
            {
                throw BadOptionalAccess{ "[ntl.optional] Optional does not contain a value" };
            }
            return *ptr();
        }
        constexpr const T& value() const noexcept
        {
            // TODO: in stl, if no value, throw an exception
            if (!has_value)
            {
                throw BadOptionalAccess{ "[ntl.optional] Optional does not contain a value" };
            }
            return *ptr();
        }

        /**
         * @brief Returns the contained value if available, another value otherwise
         * @tparam U which can convert to T 
         * @param u value if not available
         * @return value
        */
        template<class U>
        constexpr T valueOr(U&& u) const&
        {
            return hasValue() ? *ptr() : static_cast<T>(Forward<U>(u));
        }
        template<class U>
        constexpr T valueOr(U&& u) &&
        {
            return hasValue() ? Move(*ptr()) : static_cast<T>(Forward<U>(u));
        }

        template<class F>
        constexpr auto then(F&& f) &
        {
            using RT = TypeUnCVRef<TypeInvokeResult<F, decltype(value())>>;
            static_assert(TestIsOptional<RT>, "[ntl.optional] Return type must be optional");
        	if(hasValue())
	        {
                return Invoke(Forward<F>(f), value());
	        }
            else
            {
                return RT();
            }
        }
        template<class F>
        constexpr auto then(F&& f) const &
        {
            using RT = TypeUnCVRef<TypeInvokeResult<F, decltype(value())>>;
            static_assert(TestIsOptional<RT>, "[ntl.optional] Return type must be optional");
            if (hasValue())
            {
                return Invoke(Forward<F>(f), value());
            }
            else
            {
                return RT();
            }
        }
        template<class F>
        constexpr auto then(F&& f)&&
        {
            using RT = TypeUnCVRef<TypeInvokeResult<F, T>>;
            static_assert(TestIsOptional<RT>, "[ntl.optional] Return type must be optional");
            if (hasValue())
            {
                return Invoke(Forward<F>(f), Move(value()));
            }
            else
            {
                return RT();
            }
        }
        template<class F>
        constexpr auto then(F&& f) const&&
        {
            using RT = TypeUnCVRef<TypeInvokeResult<F, const T>>;
            static_assert(TestIsOptional<RT>, "[ntl.optional] Return type must be optional");
            if (hasValue())
            {
                return Invoke(Forward<F>(f), Move(value()));
            }
            else
            {
                return RT();
            }
        }
        template<class F>
        constexpr auto orElse(F&& f) const&
        {
            static_assert(TestIsInvocable<F>, "[ntl.optional] F must be callable");
            using RT = TypeUnCVRef<TypeInvokeResult<F>>;
            static_assert(TestIsSame<RT, Optional>, "[ntl.optional] Return type must be same with optional");
            static_assert(TestIsCopyConstructible<T>, "[ntl.optional] T must be copy constructible");
            if (hasValue())
            {
                return *this;
            }
            else
            {
                return Invoke(Forward<F>(f));
            }
        }
        template<class F>
        constexpr auto orElse(F&& f) &&
        {
            static_assert(TestIsInvocable<F>, "[ntl.optional] F must be callable");
            using RT = TypeUnCVRef<TypeInvokeResult<F>>;
            static_assert(TestIsSame<RT, Optional>, "[ntl.optional] Return type must be same with optional");
            static_assert(TestIsMoveConstructible<T>, "[ntl.optional] T must be move constructible");
            if (hasValue())
            {
                return *this;
            }
            else
            {
                return Invoke(Forward<F>(f));
            }
        }

        constexpr void swap(Optional& opt)
			noexcept(TestIsNothrowMoveConstructible<T> and TestIsNothrowSwappable<T>)
			requires (TestIsSwappable<T> and TestIsMoveConstructible<T>)
    	{
	        if (hasValue())
	        {
		        if (opt.hasValue())
		        {
                    utils::Swap(*ptr(), *opt.ptr());
		        }
                else
                {
                    Construct(opt.ptr(), Move(*ptr()));
                    Destruct(ptr());
                    opt.has_value = true;
                    has_value = false;
                }
	        }
            else
            {
	            if (opt.hasValue())
	            {
                    Construct(ptr(), Move(*opt.ptr()));
                    Destruct(opt.ptr());
                    opt.has_value = false;
                    has_value = true;
	            }
            }
        }

        friend constexpr void Swap(Optional& x, Optional& y)
        {
            x.swap(y);
        }

        constexpr void reset() noexcept
        {
            if (hasValue())
            {
	            Destruct(ptr());
                has_value = false;
            }
        }

        template<class ...Args>
			requires TestIsConstructible<T, Args...>
        constexpr T& emplace(Args&&...args)
			noexcept (TestIsNothrowConstructible<T, Args...>)
        {
	        if(hasValue())
	        {
                Destruct(ptr());
	        }
            has_value = true;
            return *Construct(ptr(), Forward<Args>(args)...);
        }

        template<class X>
        friend constexpr StrongOrdering operator<=>(const Optional<X>& opt, Nullopt) noexcept
        {
            if (opt.hasValue()) return StrongOrdering::greater;
            else if (not opt.hasValue()) return StrongOrdering::equivalent;
            else return StrongOrdering::less;
        }
        template<class X, class Y>
        friend constexpr TypeCompare3Way<X, Y> operator<=>(const Optional<X>& lhs, const Optional<Y>& rhs)
        {
            if (lhs.hasValue())
            {
	            if(rhs.hasValue())
	            {
                    return lhs.value() <=> rhs.value();
	            }
                else
                {
                    return TypeCompare3Way<X, Y>::greater;
                }
            }
            else
            {
	            if (rhs.hasValue())
	            {
		            return TypeCompare3Way<X, Y>::less;
	            }
                else
                {
	                return TypeCompare3Way<X, Y>::equivalent;
                }
            }
        }
        template<class X, class Y>
            requires requires (X x, Y y) { x <=> y; }
        friend constexpr TypeCompare3Way<X, Y> operator<=>(const Optional<X>& lhs, Y&& rhs)
        {
	        if (lhs.hasValue())
	        {
                return lhs.value() <=> Forward<Y>(rhs);
	        }
            else
            {
                return TypeCompare3Way<X, Y>::less;
            }
        }
    };

    template<class T>
    Optional<TypeDecay<T>> MakeOptional(T&& value)
    {
        return Optional<TypeDecay<T>>(Forward<T>(value));
    }
    template<class T, class...Args>
    Optional<T> MakeOptional(Args&&...args)
    {
        return Optional<T>(INPLACE, Forward<Args>(args)...);
    }
}
