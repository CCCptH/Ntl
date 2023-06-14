export module ntl.expected;
export import ntl.utils.inplace;
import ntl.concepts;
import ntl.type_traits;
import ntl.utils;
import ntl.memory.allocator;
import ntl.utils.exception_guard;
import ntl.functional.invoke;

namespace ne {
    template<class E>
    class Unexpected;
    template<class T, class E>
    class Expected;
    template<class T>
    struct TestIsExpectedHelper : FalseConstant {};
    template<class T, class E>
    struct TestIsExpectedHelper<Expected<T, E>> : TrueConstant {};
    template<class T>
    struct TestIsUnexpectedHelper : FalseConstant {};
    template<class T>
    struct TestIsUnexpectedHelper<Unexpected<T>> : TrueConstant {};
    template<class T>
    inline constexpr bool TestIsUnexpected = TestIsUnexpectedHelper<T>::VALUE;
    template<class T>
    inline constexpr bool TestIsExpected = TestIsExpectedHelper<T>::VALUE;
    struct Unexpect {};
    template<class E>
    inline constexpr bool TestIsValidUnexptedParam =  TestIsObject<E>
        and (not TestIsArray<E>) and (not TestIsConst<E>) and (not TestIsVolatile<E>)
        and (not TestIsUnexpected<E>);
    template<class T>
    inline constexpr bool TestIsValidExpectedParam =
        (not TestIsReference<T>) and
        (not TestIsFunction<T>) and
        (not TestIsSame<TypeUnCV<T>, Inplace>) and
        (not TestIsSame<TypeUnCV<T>, Unexpect>) and
        (not TestIsUnexpected<TypeUnCV<T>>);

}

export namespace ne {
    inline constexpr Unexpect UNEXPECT;
    template<typename E>
    class Unexpected
    {
        static_assert(TestIsValidUnexptedParam<E>, "[ntl.expeced] Unexpected for a non-object type, an array type, a specialization of unexpected, or a cv-qualified type is ill-formed.");
    private:
        E err;
    public:
        template<class T=E>
            requires (not TestIsSame<TypeUnCVRef<T>, Unexpected>)
        and (not TestIsSame<TypeUnCVRef<T>, Inplace>)
        and TestIsConstructible<TypeUnCVRef<T>, E>
        constexpr explicit Unexpected(T&& e)
            : err(Forward<T>(e))
        {}
        template<class ...Args>
            requires TestIsConstructible<E, Args...>
        constexpr Unexpected(Inplace, Args&& ...args) 
            : err(Forward<Args>(args)...)
        {}

        constexpr Unexpected(const Unexpected&) = default;
        constexpr Unexpected(Unexpected&&) = default;

        constexpr const E& error() const& noexcept {
            return err;
        }
        constexpr E& error() & noexcept {
            return err;
        }
        constexpr const E&& error() const&& noexcept {
            return Move(err);
        }
        constexpr E&& error() && noexcept {
            return Move(err);
        }

        constexpr void swap(Unexpected& other) noexcept(noexcept(utils::Swap(Declval<E>(), Declval<E>()))) {
            utils::Swap(this->err, other.err);
        }
        friend constexpr void Swap(Unexpected& x, Unexpected& y) noexcept(noexcept(x.swap(y))) {
            x.swap(y);
        }

        template<class T>
        friend constexpr bool operator==(const Unexpected& x, const Unexpected<T>& y) {
            return x.err == y.err;
        }

    };

    template<class E> Unexpected(E) -> Unexpected<E>;


    template<typename T, typename E>
    class Expected
    {
        static_assert(TestIsValidExpectedParam<T>, "[ntl.expected] The definition of template Expected<T, E> with a type for the T parameter for reference type, a function type, or for possibly cv-qualified types Inplace, Unexpect, or a specialization of Unexpected specialization of unexpected");
        static_assert(TestIsValidUnexptedParam<E>, "[ntl.expected] The definition of template Expected<T, E> with a type for the E parameter for a non-object type, an array type, a specialization of unexpected, or a cv-qualified type is ill-formed.");

        template<class Tp, class Ep>
        friend class Expected;

    public:
        using ValueType = T;
        using ErrorType = E;
        using UnexpectedType = Unexpected<E>;
    private:
        union {
            ValueType val;
            ErrorType err;
        };
        bool has_value;
    
    public:
        // Constructor
        constexpr Expected()
            noexcept(TestIsNothrowConstructible<T>)
            requires TestIsDefaultConstructible<T>
        : has_value(true)
        {
            Construct(GetAddress(val));
        }

        constexpr Expected(const Expected&) noexcept = delete;
        constexpr Expected(const Expected&)
            requires TestIsCopyConstructible<T> and TestIsCopyConstructible<E> and TestIsTriviallyCopyConstructible<T> and TestIsTriviallyCopyConstructible<E>
        = default;

        constexpr Expected(const Expected& e)
            noexcept(TestIsNothrowCopyConstructible<T> and TestIsNothrowCopyConstructible<E>)
            requires TestIsCopyConstructible<T>and TestIsCopyConstructible<E> and !(TestIsTriviallyCopyConstructible<T>and TestIsTriviallyCopyConstructible<E>)
        : has_value(e.has_value)
        {
            if (has_value) {
                Construct(GetAddress(this->val), e.val);
            }
            else {
                Construct(GetAddress(this->err), e.err);
            }
        }

        constexpr Expected(Expected&&)
            requires TestIsMoveConstructible<T>and TestIsMoveConstructible<E>and TestIsTriviallyMoveConstructible<T>and TestIsTriviallyMoveConstructible<E>
        = default;

        constexpr Expected(Expected&& e)
            noexcept(TestIsNothrowMoveConstructible<T> and TestIsNothrowMoveConstructible<E>)
            requires TestIsMoveConstructible<T>and TestIsMoveConstructible<E> and !(TestIsTriviallyMoveConstructible<T>and TestIsTriviallyMoveConstructible<E>)
        : has_value(e.has_value)
        {
            if (has_value) {
                Construct(GetAddress(this->val), ne::Move(e.val));
            }
            else {
                Construct(GetAddress(this->err), ne::Move(e.err));
            }
        }
        
        template<class U>
            requires (not TestIsSame<TypeUnCVRef<U>, Inplace>) and
                    (not TestIsSame<TypeUnCVRef<U>, Expected>) and 
                    (not TestIsUnexpected<TypeUnCVRef<U>>) and 
                    (TestIsConstructible<T, U>)
        constexpr 
        explicit(not TestIsConvertible<U, T>)
        Expected(U&& v)
            noexcept(TestIsNothrowConstructible<T, U>)
            : has_value(true)
        {
            Construct(GetAddress(val), Forward<U>(v));
        }

        template<class U, class G>
            requires TestIsConstructible<T, const U&> and
            TestIsConstructible<E, const G&> and
            (not TestIsConstructible < T, Expected<U, G>&>) and
            (not TestIsConstructible < T, Expected<U, G>>) and
            (not TestIsConstructible < T, const Expected<U, G>&>) and
            (not TestIsConstructible < T, const Expected<U, G>>) and
            (not TestIsConvertible<Expected<U, G>&, T>) and 
            (not TestIsConvertible<Expected<U, G>, T>) and 
            (not TestIsConvertible<const Expected<U, G>&, T>) and
            (not TestIsConvertible<const Expected<U, G>, T>) and
            (not TestIsConstructible<Unexpected<E>, Expected<U, G>&>) and
            (not TestIsConstructible<Unexpected<E>, Expected<U, G>>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<U, G>&>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<U, G>&>)
        constexpr
        explicit((not TestIsConvertible<const U&, T>) or (not TestIsConvertible<const G&, E>))
        Expected(const Expected<U, G>& e)
                noexcept(TestIsNothrowConstructible<T, const U&>and TestIsNothrowConstructible<E, const G&>)
            : has_value(e.has_value)
        {
            if (has_value) {
                Construct(GetAddress(val), e.val);
            }
            else {
                Construct(GettAddress(err), e.err);
            }
        }

        template<class U, class G>
            requires TestIsConstructible<T, U>and
        TestIsConstructible<E, G> and
            (not TestIsConstructible < T, Expected<U, G>&>) and
            (not TestIsConstructible < T, Expected<U, G>>) and
            (not TestIsConstructible < T, const Expected<U, G>&>) and
            (not TestIsConstructible < T, const Expected<U, G>>) and
            (not TestIsConvertible<Expected<U, G>&, T>) and
            (not TestIsConvertible<Expected<U, G>, T>) and
            (not TestIsConvertible<const Expected<U, G>&, T>) and
            (not TestIsConvertible<const Expected<U, G>, T>) and
            (not TestIsConstructible<Unexpected<E>, Expected<U, G>&>) and
            (not TestIsConstructible<Unexpected<E>, Expected<U, G>>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<U, G>&>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<U, G>&>)
        constexpr
        explicit((not TestIsConvertible<U, T>) or (not TestIsConvertible<G, E>))
        Expected(Expected<U, G>&& e)
            noexcept(TestIsNothrowConstructible<T, const U>and TestIsNothrowConstructible<E, G>)
            : has_value(e.has_value)
        {
            if (has_value) {
                Construct(GetAddress(val), Move(e.val));
            }
            else {
                Construct(GettAddress(err), Move(e.err));
            }
        }

        template<class G>
            requires TestIsConstructible<E, const G&>
        constexpr explicit(not TestIsConvertible<const G&, E>)
            Expected(const Unexpected<G>& e)
            noexcept(TestIsNothrowConstructible<E, const G&>)
            : has_value(false)
        {
            Construct(GetAddress(err), e.error());
        }

        template<class G>
            requires TestIsConstructible<E, G>
        constexpr explicit(not TestIsConvertible<G, E>)
        Expected(Unexpected<G>&& e) 
            noexcept(TestIsNothrowConstructible<E, G>)
            : has_value(false)
        {
            Construct(GetAddress(err), Move(e.error()));
        }

        template<class ...Args>
            requires TestIsConstructible<T, Args...>
        constexpr explicit Expected(Inplace, Args&&...args)
            : has_value(true)
        {
            Construct(GetAddress(val), Forward<Args>(args)...);
        }

        template<class ...Args>
            requires TestIsConstructible<T, Args...>
        constexpr explicit Expected(Unexpect, Args&&...args)
            : has_value(false)
        {
            Construct(GetAddress(err), Forward<Args>(args)...);
        }

        constexpr ~Expected() requires (TestIsTriviallyDestructible<T>and TestIsTriviallyDestructible<E>) = default;
        constexpr ~Expected() requires (not (TestIsTriviallyDestructible<T>and TestIsTriviallyDestructible<E>)) {
            if (has_value) {
                Destruct(GetAddress(val));
            }
            else {
                Destruct(GetAddress(err));
            }
        }

        constexpr Expected& operator=(const Expected&) = delete;

        constexpr Expected& operator=(const Expected& e)
            noexcept(TestIsNothrowCopyAssignable<T> and 
                TestIsNothrowCopyConstructible<T> and
                TestIsNothrowCopyAssignable<E> and 
                TestIsNothrowCopyConstructible<E>)
            requires (
                TestIsCopyAssignable<T> and TestIsCopyConstructible<T> and
                    TestIsCopyAssignable<E>and TestIsCopyConstructible<E> and (
                        TestIsNothrowMoveConstructible<T> or TestIsNothrowMoveConstructible<E>
                    )
            )
        {
            if (has_value && e.has_value) {
                val = e.val;
            }
            else if (has_value)
            {
                // e no val
                reinit(err, val, e.err);
            }
            else if (e.has_value)
            {
                reinit(val, err, e.val);
            }
            else
            {
                err = e.err;
            }
            has_value = e.has_value;
            return (*this);
        }

        constexpr Expected& operator=(Expected&& e)
            noexcept (
                TestIsNothrowMoveConstructible<T> and
                TestIsNothrowMoveAssignable<T> and
                TestIsNothrowMoveConstructible<E>and
                TestIsNothrowMoveAssignable<E>
                )
            requires (
                TestIsMoveAssignable<T> and
                TestIsMoveConstructible<T> and
                TestIsMoveAssignable<E> and
                TestIsMoveConstructible<E> and (
                    TestIsNothrowMoveConstructible<T> or
                    TestIsNothrowMoveConstructible<E>
                    )

            )
        {
            if (has_value && e.has_value) {
                val = Move(e.val);
            }
            else if (has_value)
            {
                // e no val
                reinit(err, val, Move(e.err));
            }
            else if (e.has_value)
            {
                reinit(val, err, Move(e.val));
            }
            else
            {
                err = Move(e.err);
            }
            has_value = e.has_value;
            return (*this);
        }

        template<class U=T>
        constexpr Expected& operator=(U&& new_value)
            requires (
                (not TestIsSame<Expected, TypeUnCVRef<U>>) and
                (not TestIsUnexpected<TypeUnCVRef<U>>) and
                TestIsConstructible<T, U> and
                TestIsAssignable<T&, U> and
                (TestIsNothrowConstructible<T, U> or TestIsNothrowMoveConstructible<T> or TestIsNothrowMoveConstructible<E> )
            )
        {
            if (has_value)
            {
                val = Forward<U>(new_value);
            }
            else
            {
                reinit(val, err, Forward<U>(new_value));
                has_value = true;
            }
            return *this;
        }
    private:
        template<class E2>
        constexpr static bool TestCanAssignFromUnexpected =
            TestIsConstructible<E, E2> and TestIsAssignable<E&, E> and
            (TestIsNothrowConstructible<E, E2> or TestIsNothrowMoveConstructible<T> or TestIsNothrowMoveConstructible<E>);
    public:
        template<class E2> requires TestCanAssignFromUnexpected<E2>
        constexpr Expected& operator=(const Unexpected<E2>& unexpected)
        {
            if (has_value)
            {
                reinit(err, val, unexpected.error());
                has_value = false;
            }
            else
            {
                err = unexpected.error();
            }
            return *this;
        }
        template<class E2> requires TestCanAssignFromUnexpected<E2>
        constexpr Expected& operator=(Unexpected<E2>&& unexpected)
        {
            if (has_value)
            {
                reinit(err, val, Move(unexpected.error()));
                has_value = false;
            }
            else
            {
                err = Move(unexpected.error());
            }
            return *this;
        }
        template<class...Args>	requires TestIsNothrowConstructible<T, Args...>
        constexpr T& emplace(Args&&...args) noexcept
        {
            if (has_value)
            {
                Destruct(GetAddress(val));
            }
            else
            {
                Destruct(GetAddress(err));
            }
            auto ptr = *(Construct(GetAddress(val), Forward<Args>(args)...));
            has_value = true;
            return ptr;
        }

        constexpr void swap(Expected& e)
            noexcept(
                TestIsNothrowMoveConstructible<T>and
                TestIsNothrowSwappable<T>and
                TestIsNothrowMoveConstructible<E>and
                TestIsNothrowSwappable<E>
                )
            requires (
                TestIsSwappable<T>and
                TestIsSwappable<E>and
                TestIsMoveConstructible<T>and
                TestIsMoveConstructible<E> and
                (TestIsNothrowMoveConstructible<T> or TestIsNothrowMoveConstructible<E>)
            )
        {
            auto swapValErr = [&](Expected& ev, Expected& ee)
            {
                if constexpr (TestIsNothrowMoveConstructible<E>)
                {
                    E temp(Move(ee.err));
                    Destruct(GetAddress(ee.err));
                    auto guard = ExceptionGuard([&]()
                        {
                            Construct(GetAddress(ee.err), Move(temp));
                        });
                    Construct(ee.val, Move(ev.val));
                    guard.complete();
                    Destruct(GetAddress(ev.val));
                    Construct(GetAddress(ev.err), Move(temp));
                } else
                {
                    static_assert(TestIsNothrowMoveConstructible<T>, "[ntl.expected] To provide strong exception guarantee, T has to be move constructible");
                    T temp(Move(ev.val));
                    Destruct(GetAddress(ev.val));
                    auto guard = ExceptionGuard([&]()
                        {
                            Construct(GetAddress(ev.val), Move(temp));
                        });
                    Construct(GetAddress(ev.err), Move(ee.err));
                    guard.complete();
                    Destruct(GetAddress(ee.err));
                    Construct(GetAddress(ee.val), Move(temp));
                }
                ev.has_value = false;
                ee.has_value = true;
            };

            if (has_value)
            {
                if (e.has_value)
                {
                    utils::Swap(val, e.val);
                }
                else
                {
                    swapValErr(*this, e);
                }
            }
            else
            {
                if (e.has_value)
                {
                    swapValErr(e, *this);
                }
                else
                {
                    utils::Swap(err, e.err);
                }
            }
        }

        friend constexpr void Swap(Expected& x, Expected& y)
        {
            x.swap(y);
        }

        constexpr const T* operator->() const noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return GetAddress(val);
        }
        constexpr T* operator->() noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return GetAddress(val);
        }
        constexpr const T& operator*() const& noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return val;
        }
        constexpr T& operator*() & noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return val;
        }
        constexpr const T&& operator*() const&& noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return Move(val);
        }
        constexpr T&& operator*() && noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return Move(val);
        }
        constexpr explicit operator bool() const noexcept
        {
            return has_value;
        }
        constexpr bool hasValue() const noexcept
        {
            return has_value;
        }
        constexpr const T& value() const& noexcept
        {
            // Todo: throw an exception
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return val;
        }
        constexpr T& value() & noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return val;
        }
        constexpr const T&& value() const&& noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
            return Move(val);
        }
        constexpr T&& value() && noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a value");
            return Move(val);
        }
        constexpr const T& error() const& noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a value");
            return err;
        }
        constexpr T& error() & noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a value");
            return err;
        }
        constexpr const T&& error() const&& noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a value");
            return Move(err);
        }
        constexpr T&& error() && noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a value");
            return Move(err);
        }
        template<class U>
        constexpr T valueOr(U&& other_value) const&
        {
            static_assert(TestIsCopyConstructible<T>, "[ntl.expected] Value type T must be copy constructible");
            static_assert(TestIsConvertible<U, T>, "[ntl.expected] Type of argument has to be converted to value type");
            if (has_value)
            {
                return val;
            }
            else
            {
                return static_cast<T>(Forward<U>(other_value));
            }
        }
        template<class U>
        constexpr T valueOr(U&& other_value) &&
        {
            static_assert(TestIsCopyConstructible<T>, "[ntl.expected] Value type T must be copy constructible");
            static_assert(TestIsConvertible<U, T>, "[ntl.expected] Type of argument has to be converted to value type");
            if (has_value)
            {
                return Move(val);
            }
            else
            {
                return static_cast<T>(Forward<U>(other_value));
            }
        }

        template<class T2,class E2>
        friend constexpr bool operator==(const Expected& lhs, const Expected<T2, E2>& rhs)
        {
            if (lhs.hasValue()!=rhs.hasValue())
            {
                return false;
            }
            else
            {
                if (lhs.hasValue())
                {
                    return static_cast<bool>(lhs.val == rhs.val);
                }
                else
                {
                    return static_cast<bool>(lhs.err == rhs.err);
                }
            }
        }

        template<class T2>
        friend constexpr bool operator==(const Expected& lhs, const T2& rhs)
        {
            return lhs.hasValue() && static_cast<bool>(lhs.val == rhs);
        }

        template<class Callback>
        constexpr auto then(Callback&& callback) &
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(value())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback), this->value());
            }
            else
            {
                return *this;
            }
        }
        template<class Callback>
        constexpr auto then(Callback&& callback) const&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(value())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback), this->value());
            }
            else
            {
                return *this;
            }
        }

        template<class Callback>
        constexpr auto then(Callback&& callback)&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(value())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback), Move(this->value()));
            }
            else
            {
                return Move(*this);
            }
        }
        template<class Callback>
        constexpr auto then(Callback&& callback) const&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(value())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback), Move(this->value()));
            }
            else
            {
                return Move(*this);
            }
        }

        template<class Callback>
        constexpr auto orElse(Callback&& callback) &
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");
            if (hasValue())
            {
                return *this;
            }
            else
            {
                return Invoke(Forward<Callback>(callback), this->error());
            }
        }
        template<class Callback>
        constexpr auto orElse(Callback&& callback) const&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");
            if (hasValue())
            {
                return *this;
            }
            else
            {
                return Invoke(Forward<Callback>(callback), this->error());
            }
        }

        template<class Callback>
        constexpr auto orElse(Callback&& callback)&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");
            if (hasValue())
            {
                return Move(*this);
            }
            else
            {
                return Invoke(Forward<Callback>(callback), Move(this->error()));
            }
        }
        template<class Callback>
        constexpr auto orElse(Callback&& callback) const&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");
            if (hasValue())
            {
                return Move(*this);
            }
            else
            {
                return Invoke(Forward<Callback>(callback), Move(this->error()));
            }
        }

    private:

        template<class T1, class T2, class...Args>
        static constexpr void reinit(T1& new_val, T2& old_val, Args&&...args) {
            // Strong exception guaranteen;;
            if constexpr (TestIsNothrowConstructible<T1, Args...>) {
                Destruct(GetAddress(old_val));
                Construct(GetAddress(new_val), Forward<Args>(args...));
            }
            else if constexpr (TestIsNothrowMoveConstructible<T1>) {
                auto tmp = T1(Forward<Args>(args)...);
                Destruct(GetAddress(old_val));
                Construct(GetAddress(new_val), Move(tmp));
            }
            else {
                static_assert(TestIsNothrowConstructible<T2>,
                    "[ntl.expected] To provide strong exception guarantee, T2 has to be nothrow constructible"
                    );
                auto t2 = Move(old_val);
                Destruct(GetAddress(old_val));
                auto guard = ExceptionGuard([&]() {
                    Construct(GetAddress(old_val), Move(t2));
                    });
                Construct(GetAddress(new_val), Forward<Args>(args)...);
                guard.complete();
            }
        }
    };

    template<class E>
    class Expected<void, E>
    {
        static_assert(TestIsValidUnexptedParam<E>, "[ntl.expected] The definition of template Expected<T, E> with a type for the E parameter for a non-object type, an array type, a specialization of unexpected, or a cv-qualified type is ill-formed.");
        template<class, class> friend class Expected;
    private:
        bool has_value;
        [[no_unique_address]]
        E err;
    public:
        using ValueType = void;
        using ErrorType = E;
        using UnexpectedType = Unexpected<E>;

        constexpr Expected() :has_value(true) {}

        constexpr Expected(const Expected&) requires TestIsCopyConstructible<E>and TestIsTriviallyCopyConstructible<E> = default;

        constexpr Expected(const Expected&) = delete;

        constexpr Expected(const Expected& e)
	        noexcept(TestIsNothrowCopyConstructible<E>)
	        requires TestIsCopyConstructible<E> and (not TestIsTriviallyCopyConstructible<E>)
		        : has_value(e.has_value)
        {
	        if (!e.has_value)
	        {
                Construct(GetAddress(err), e.err);
	        }
        }

    	constexpr Expected(Expected&&) requires TestIsMoveConstructible<E>and TestIsTriviallyMoveConstructible<E> = default;

        constexpr Expected(Expected&& e)
            noexcept(TestIsNothrowMoveConstructible<E>)
            requires TestIsMoveConstructible<E> and (not TestIsTriviallyMoveConstructible<E>)
        : has_value(e.has_value)
        {
            if (!e.has_value)
            {
                Construct(GetAddress(err), Movc(e.err));
            }
        }

        template<class E2>
			requires TestIsConstructible<E, const E2&> and
			(not TestIsConstructible<Unexpected<E>, Expected<void, E2>&>) and
            (not TestIsConstructible<Unexpected<E>, Expected<void, E2>>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<void, E2>&>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<void, E2>>)
        constexpr explicit (not TestIsConvertible<const E2&, E>) Expected(const Expected<void, E2>& expected)
			noexcept (TestIsNothrowConstructible<E, const E2&>)
                :has_value(expected.has_value)
        {
	        if (!expected.has_value)
	        {
                Construct(GetAddress(err), expected.err);
	        }
        }

        template<class E2>
            requires TestIsConstructible<E, E2> and
        (not TestIsConstructible<Unexpected<E>, Expected<void, E2>&>) and
            (not TestIsConstructible<Unexpected<E>, Expected<void, E2>>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<void, E2>&>) and
            (not TestIsConstructible<Unexpected<E>, const Expected<void, E2>>)
    	constexpr explicit (not TestIsConvertible<const E2&, E>) Expected(Expected<void, E2>&& expected)
            noexcept (TestIsNothrowConstructible<E, E2>)
            :has_value(expected.has_value)
        {
            if (!expected.has_value)
            {
                Construct(GetAddress(err), expected.err);
            }
        }

        template<class E2>
			requires TestIsConstructible<E, const E2&>
        constexpr explicit(!TestIsConvertible<const E2&, E>) Expected(const Unexpected<E2>& unexpected)
			noexcept(TestIsNothrowConstructible<E, const E2&>)
            : has_value(false)
        {
            Construct(GetAddress(err), unexpected.error());
        }
        template<class E2>
            requires TestIsConstructible<E, E2>
        constexpr explicit(!TestIsConvertible<const E2&, E>) Expected(Unexpected<E2>&& unexpected)
            noexcept(TestIsNothrowConstructible<E, E2>)
            : has_value(false)
        {
            Construct(GetAddress(err), Move(unexpected.error()));
        }

        constexpr explicit Expected(Inplace)noexcept :has_value(true){}

        template<class...Args>
        constexpr explicit Expected(Unexpect, Args&&...args) noexcept(TestIsNothrowConstructible<E, Args...>)
            :has_value(false)
        {
            Construct(GetAddress(err), Forward<Args>(args)...);
        }

        constexpr ~Expected() requires TestIsNothrowDestructible<E> = default;
        constexpr ~Expected() requires (not TestIsNothrowDestructible<E>)
        {
	        if (! has_value)
	        {
                Destruct(GetAddress(err));
	        }
        }

        constexpr Expected& operator=(const Expected&) = delete;

        constexpr Expected& operator=(const Expected& e) noexcept(TestIsNothrowCopyAssignable<E> and TestIsNothrowCopyConstructible<E>)
			requires TestIsCopyAssignable<E> and TestIsCopyConstructible<E>
        {
	        if (has_value)
	        {
		        if (!e.has_value)
		        {
			        Construct(GetAddress(err), e.err);
                    has_value = false;
		        }
	        }
            else
            {
	            if (e.has_value)
	            {
                    Destruct(GetAddress(err));
                    has_value = true;
	            }
                else
                {
                    err = e.err;
                }
            }
            return *this;
        }

        constexpr Expected& operator=(Expected&& e) noexcept(TestIsNothrowMoveAssignable<E>and TestIsNothrowMoveConstructible<E>)
            requires TestIsMoveAssignable<E>and TestIsMoveConstructible<E>
        {
            if (has_value)
            {
                if (!e.has_value)
                {
                    Construct(GetAddress(err), Move(e.err));
                    has_value = false;
                }
            }
            else
            {
                if (e.has_value)
                {
                    Destruct(GetAddress(err));
                    has_value = true;
                }
                else
                {
                    err = Move(e.err);
                }
            }
            return *this;
        }

        template<class E2>
        constexpr Expected& operator=(const Unexpected<E2>& unexpected)
			requires (TestIsAssignable<E&, const E2&> and TestIsConstructible<E, const E2&>)
        {
	        if (has_value)
	        {
                Construct(GetAddress(err), unexpected.error());
                has_value = false;
	        }
            else
            {
                err = unexpected.error();
            }
            return *this;
        }
        template<class E2>
        constexpr Expected& operator=(Unexpected<E2>&& unexpected)
            requires (TestIsAssignable<E&, E2>and TestIsConstructible<E, E2>)
        {
            if (has_value)
            {
                Construct(GetAddress(err), Move(unexpected.error()));
                has_value = false;
            }
            else
            {
                err = Move(unexpected.error());
            }
            return *this;
        }

        constexpr void emplace() noexcept
        {
	        if (!has_value)
	        {
                Destruct(GetAddress(err));
                has_value = true;
	        }
        }

        constexpr void swap(Expected& e)
			noexcept (TestIsNothrowMoveConstructible<E> and TestIsNothrowSwappable<E>)
			requires (TestIsSwappable<E> and TestIsMoveConstructible<E>)
        {
            auto swapValErr = [&](Expected& ev, Expected& ee)
            {
                Construct(GetAddress(ev.err), Move(ee.err));
                Destruct(GetAddress(ee.err));
                ev.has_value = false;
                ee.has_value = true;
            };

            if (has_value)
            {
	            if (!e.has_value)
	            {
                    swapValErr(*this, e);
	            }
            }
            else
            {
	            if (e.has_value)
	            {
                    swapValErr(e, *this);
	            }
                else
                {
                    utils::Swap(err, e.err);
                }
            }
        }

        friend constexpr void Swap(Expected& x, Expected& y)
			noexcept(x.swap(y))
            requires requires { x.swap(y); }
        {
            x.swap(y);
        }

        constexpr operator bool() const noexcept { return has_value; }
        constexpr bool hasValue() const noexcept { return has_value; }
        constexpr void operator*() const noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
        }
        constexpr void value() const& noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
        }
        constexpr void value() && noexcept
        {
            Assert(has_value, "[ntl.expected] Expected does not contain a value");
        }
        constexpr const E& error() const & noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a error");
            return err;
        }
        constexpr E& error() & noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a error");
            return err;
        }
        constexpr E&& error() && noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a error");
            return Move(err);
        }
        constexpr const E&& error() const&& noexcept
        {
            Assert(!has_value, "[ntl.expected] Expected does not contain a error");
            return Move(err);
        }
        template<class E2>
        friend constexpr bool operator==(const Expected& lhs, const Expected<void, E2>& rhs)
        {
	        if (lhs.has_value != rhs.has_value)
	        {
                return false;
	        }
            else
            {
                return lhs.has_value || static_cast<bool>(lhs.err == rhs.err);
            }
        }
        template<class E2>
        friend constexpr bool operator==(const Expected& lhs, const Unexpected<E2>& rhs)
        {
        	return !lhs.has_value && static_cast<bool>(lhs.err == rhs.error());
        }

        template<class Callback>
        constexpr auto then(Callback&& callback)&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback));
            }
            else
            {
                return *this;
            }
        }
        template<class Callback>
        constexpr auto then(Callback&& callback) const&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback));
            }
            else
            {
                return *this;
            }
        }

        template<class Callback>
        constexpr auto then(Callback&& callback)&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback), Move(this->value()));
            }
            else
            {
                return Move(*this);
            }
        }
        template<class Callback>
        constexpr auto then(Callback&& callback) const&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ErrorType, ErrorType>, "[ntl.expected] Return type's error type must be same with origin error type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");

            if (hasValue())
            {
                return Invoke(Forward<Callback>(callback), Move(this->value()));
            }
            else
            {
                return Move(*this);
            }
        }

        template<class Callback>
        constexpr auto orElse(Callback&& callback)&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");
            if (hasValue())
            {
                return *this;
            }
            else
            {
                return Invoke(Forward<Callback>(callback), this->error());
            }
        }
        template<class Callback>
        constexpr auto orElse(Callback&& callback) const&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsCopyConstructible<ReturnType>, "[ntl.expected] Return type must be copy constructible");
            if (hasValue())
            {
                return *this;
            }
            else
            {
                return Invoke(Forward<Callback>(callback), this->error());
            }
        }

        template<class Callback>
        constexpr auto orElse(Callback&& callback)&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");
            if (hasValue())
            {
                return Move(*this);
            }
            else
            {
                return Invoke(Forward<Callback>(callback), Move(this->error()));
            }
        }
        template<class Callback>
        constexpr auto orElse(Callback&& callback) const&&
        {
            using ReturnType = TypeUnCVRef< TypeInvokeResult<Callback, decltype(error())>>;
            static_assert(TestIsExpected<ReturnType>, "[ntl.expected] Return type of monadic function must be Expected");
            static_assert(TestIsSame<typename ReturnType::ValueType, ValueType>, "[ntl.expected] Return type's value type must be same with origin value type");
            static_assert(TestIsMoveConstructible<ReturnType>, "[ntl.expected] Return type must be move constructible");
            if (hasValue())
            {
                return Move(*this);
            }
            else
            {
                return Invoke(Forward<Callback>(callback), Move(this->error()));
            }
        }
    };
}