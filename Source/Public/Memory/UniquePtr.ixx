export module ntl.memory.unique_ptr;

import ntl.utils;
import ntl.memory.allocator;
import ntl.type_traits;

export import ntl.memory.default_deleter;

export namespace ne
{
    template<class T, class Deleter=DefaultDeleter<T>>
    class UniquePtr
    {
    public:
        using ElementType = T;
        using DeleterType = Deleter;
        //using Pointer = TypeConditional <
        //	requires { typename TypeUnRef<Deleter>::Pointer; },
        //	TypeUnRef<Deleter>::Pointer,
        //	T*
        //> ;
        using Pointer = T*;

        constexpr UniquePtr() noexcept
            requires TestIsDefaultConstructible<Deleter> and (not TestIsPointer<Deleter>)
            : ptr(nullptr)
            , deleter()
        {}
        constexpr UniquePtr(decltype(nullptr)) noexcept
            requires TestIsDefaultConstructible<Deleter> and (not TestIsPointer<Deleter>)
            : UniquePtr()
        {}
        constexpr explicit UniquePtr(Pointer p) noexcept
            requires TestIsDefaultConstructible<Deleter> and (not TestIsPointer<Deleter>)
            : ptr(p)
            , deleter()
        {}

        constexpr UniquePtr(Pointer p, const DeleterType& d) noexcept
            requires (not TestIsReference<DeleterType>) and TestIsCopyConstructible<DeleterType> and TestIsConstructible<DeleterType, decltype(d)>
            : ptr(p)
            , deleter(d)
        {}

        constexpr UniquePtr(Pointer p, DeleterType&& d) noexcept
            requires (not TestIsReference<DeleterType>) and TestIsCopyConstructible<DeleterType> and TestIsConstructible<DeleterType, decltype(d)>
            : ptr(p)
            , deleter(Move(d))
        {}

        constexpr UniquePtr(Pointer p, TypeUnRef<DeleterType>& d) noexcept
            requires TestIsLRef<DeleterType> and (not TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
            : ptr(p)
            , deleter(d)
        {}

        constexpr UniquePtr(Pointer p, TypeUnRef<DeleterType>&& d) noexcept
            requires TestIsLRef<DeleterType> and (not TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
        = delete;

        constexpr UniquePtr(Pointer p, const TypeUnRef<DeleterType>& d) noexcept
            requires TestIsLRef<DeleterType> and (TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
            : ptr(p)
            , deleter(d)
        {}

        constexpr UniquePtr(Pointer p, const TypeUnRef<DeleterType>&& d) noexcept
            requires TestIsLRef<DeleterType> and (TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
        = delete;

        constexpr UniquePtr(UniquePtr&& u) noexcept
            requires TestIsMoveConstructible<DeleterType>
            : ptr(Move(u.ptr))
            , deleter(Forward<DeleterType>(deleter))
        {
            u.ptr = nullptr;
        }

        template<class U, class E>
        constexpr UniquePtr(UniquePtr<U,E>&& u) noexcept
            requires TestIsConvertible<typename UniquePtr<U,E>::Pointer, Pointer> and (not TestIsArray<U>)
            and TestIsReference<DeleterType> and TestIsSame<E, DeleterType>
                : ptr(u.ptr)
                , deleter(u.deleter)
        {
            u.ptr = nullptr;
        }

        template<class U, class E>
        constexpr UniquePtr(UniquePtr<U, E>&& u) noexcept
            requires TestIsConvertible<typename UniquePtr<U, E>::Pointer, Pointer> and (not TestIsArray<U>)
            and (not TestIsReference<DeleterType>) and TestIsConvertible<E, DeleterType>
            : ptr(u.ptr)
            , deleter(Move(u.deleter))
        {}

        constexpr ~UniquePtr()
        {
            if (ptr != nullptr)
                deleter(ptr);
        }

        constexpr UniquePtr& operator=(decltype(nullptr)) noexcept
        {
            ptr = nullptr;
            return *this;
        }

        constexpr UniquePtr& operator=(UniquePtr&& u) noexcept
            requires TestIsMoveAssignable<DeleterType>
        {
            reset(u.release());
            getDeleter() = Forward<DeleterType>(u.getDeleter());
            return *this;
        }

        template<class U, class E>
        constexpr UniquePtr& operator=(UniquePtr<U, E>&& u) noexcept
            requires (not TestIsArray<U>) and TestIsConvertible<typename UniquePtr<U,E>::Pointer, Pointer>
            and TestIsAssignable<DeleterType&, E&&>
        {
            reset(u.release());
            getDeleter() = Forward<DeleterType>(u.getDeleter());
            return *this;
        }

        constexpr Pointer release() noexcept
        {
            auto p = ptr;
            ptr = nullptr;
            return p;
        }

        constexpr Pointer get() const noexcept
        {
            return ptr;
        }

        constexpr void reset(Pointer p = Pointer()) noexcept
        {
            Pointer old = ptr;
            ptr = p;
            if (old)
            {
                deleter(old);
            }
        }

        constexpr const DeleterType& getDeleter() const noexcept
        {
            return deleter;
        }

        constexpr DeleterType& getDeleter() noexcept
        {
            return deleter;
        }

        constexpr  TypeAddLRef<T> operator*() const noexcept
        {
            return *ptr;
        }
        constexpr  Pointer operator->() const noexcept
        {
            return ptr;
        }

        constexpr operator bool() const noexcept
        {
            return ptr != nullptr;
        }

        constexpr void swap(UniquePtr& other) noexcept
        {
            utils::Swap(ptr, other.ptr);
            utils::Swap(deleter, other.deleter);
        }

        friend void Swap(UniquePtr& x, UniquePtr& y) noexcept
        {
            x.swap(y);
        }

        template<class A, class B, class C, class D>
        friend decltype(Declval<Pointer>()<=>Declval<Pointer>())
        operator<=> (const UniquePtr<A, B>& a, const UniquePtr<C, D>& b) noexcept
        {
            return a.ptr <=> b.ptr;
        }

        template<class A, class B, class C, class D>
        friend bool operator== (const UniquePtr<A, B>& a, const UniquePtr<C, D>& b) noexcept
        {
            return a.ptr == b.ptr;
        }

        template<class A, class B>
        friend decltype(Declval<Pointer>() <=> nullptr)
            operator<=> (const UniquePtr<A, B>& a, decltype(nullptr)) noexcept
        {
            return a.ptr <=> nullptr;
        }

        template<class A, class B>
        friend bool operator== (const UniquePtr<A, B>& a, decltype(nullptr)) noexcept
        {
            return a.ptr == nullptr;
        }
    private:
        T* ptr;
        [[no_unique_address]]
        Deleter deleter;
    };

    template<class T, class Deleter>
    class UniquePtr<T[], Deleter>
    {
    public:
        using ElementType = T;
        using DeleterType = Deleter;
        //using Pointer = TypeConditional <
        //	requires { typename TypeUnRef<Deleter>::Pointer; },
        //	TypeUnRef<Deleter>::Pointer,
        //	T*
        //> ;
        using Pointer = T*;

        constexpr UniquePtr() noexcept
            requires TestIsDefaultConstructible<Deleter> and (not TestIsPointer<Deleter>)
            : ptr(nullptr)
            , deleter()
        {}

        constexpr UniquePtr(decltype(nullptr)) noexcept
            requires TestIsDefaultConstructible<Deleter> and (not TestIsPointer<Deleter>)
            : ptr(nullptr)
            , deleter()
        {}

        template<class U>
        explicit UniquePtr(U p) noexcept
            requires (TestIsSame<U, Pointer> or 
            (TestIsSame<Pointer, ElementType*> and TestIsConvertible<TypeUnPtr<U>(*)[], ElementType(*)[]>))
            and TestIsDefaultConstructible<Deleter>
            : ptr(p)
            , deleter()
        {}

        template<class U>
        constexpr UniquePtr(Pointer p, const DeleterType& d) noexcept
            requires (not TestIsReference<DeleterType>) and TestIsCopyConstructible<DeleterType> and TestIsConstructible<DeleterType, decltype(d)>
            and (TestIsSame<U, Pointer> or
                (TestIsSame<Pointer, ElementType*> and TestIsConvertible<TypeUnPtr<U>(*)[], ElementType(*)[]>))
        : ptr(p)
            , deleter(d)
        {}

        template<class U>
        constexpr UniquePtr(U p, DeleterType&& d) noexcept
            requires (not TestIsReference<DeleterType>) and TestIsCopyConstructible<DeleterType> and TestIsConstructible<DeleterType, decltype(d)>
        and (TestIsSame<U, Pointer> or
            (TestIsSame<Pointer, ElementType*> and TestIsConvertible<TypeUnPtr<U>(*)[], ElementType(*)[]>))
        : ptr(p)
            , deleter(Move(d))
        {}

        template<class U>
        constexpr UniquePtr(U p, TypeUnRef<DeleterType>& d) noexcept
            requires TestIsLRef<DeleterType> and (not TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
            and (TestIsSame<U, Pointer> or
                (TestIsSame<Pointer, ElementType*> and TestIsConvertible<TypeUnPtr<U>(*)[], ElementType(*)[]>))
        : ptr(p)
            , deleter(d)
        {}

        template<class U>
        constexpr UniquePtr(U p, TypeUnRef<DeleterType>&& d) noexcept
            requires TestIsLRef<DeleterType> and (not TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
            and (TestIsSame<U, Pointer> or
                (TestIsSame<Pointer, ElementType*> and TestIsConvertible<TypeUnPtr<U>(*)[], ElementType(*)[]>))
        = delete;

        template<class U>
        constexpr UniquePtr(U p, const TypeUnRef<DeleterType>& d) noexcept
            requires TestIsLRef<DeleterType> and (TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
            and (TestIsSame<U, Pointer> or
                (TestIsSame<Pointer, ElementType*> and TestIsConvertible<TypeUnPtr<U>(*)[], ElementType(*)[]>))
        : ptr(p)
            , deleter(d)
        {}

        template<class U>
        constexpr UniquePtr(U p, const TypeUnRef<DeleterType>&& d) noexcept
            requires TestIsLRef<DeleterType> and (TestIsConst<TypeUnRef<DeleterType>>) and TestIsConvertible<DeleterType, decltype(d)>
            and (TestIsSame<U, Pointer> or
                (TestIsSame<Pointer, ElementType*> and TestIsConvertible<TypeUnPtr<U>(*)[], ElementType(*)[]>))
        = delete;

        constexpr UniquePtr(UniquePtr&& u) noexcept
            requires TestIsMoveConstructible<DeleterType>
        : ptr(Move(u.ptr))
            , deleter(Forward<DeleterType>(deleter))
        {
            u.ptr = nullptr;
        }

        template<class U, class E>
        constexpr UniquePtr(UniquePtr<U, E>&& u) noexcept
            requires (
                TestIsArray<U> and 
                TestIsSame<Pointer, ElementType*> and 
                TestIsSame<typename UniquePtr<U, E>::Pointer, typename UniquePtr<U, E>::ElementType*> and
                TestIsConvertible<typename UniquePtr<U, E>::ElementType(*)[], ElementType(*)[]> and
                ((TestIsLRef<Deleter> and TestIsSame<E, Deleter>) or (!TestIsRef<Deleter> and TestIsConvertible<E, Deleter>))
                    )
            : ptr(u.ptr)
            , deleter(Forward<DeleterType>(u.deleter))
        {
            u.ptr = nullptr;
        }

        constexpr ~UniquePtr()
        {
            if (ptr!=nullptr)
            {
                deleter(ptr);
            }
        }

        constexpr UniquePtr& operator=(decltype(nullptr)) noexcept
        {
            ptr = nullptr;
            return *this;
        }

        constexpr UniquePtr& operator=(UniquePtr&& u) noexcept
            requires TestIsMoveAssignable<DeleterType>
        {
            ptr = u.ptr;
            u.ptr = nullptr;
            deleter = Forward<DeleterType>(u.deleter);
            return *this;
        }

        template<class U, class E>
        constexpr UniquePtr& operator=(UniquePtr<U, E>&& u) noexcept
        {
            ptr = u.ptr;
            u.ptr = nullptr;
            deleter = Forward<DeleterType>(u.deleter);
            return *this;
        }

        constexpr Pointer release() noexcept
        {
            auto p = ptr;
            ptr = nullptr;
            return p;
        }

        constexpr Pointer get() const noexcept
        {
            return ptr;
        }

        constexpr void reset(Pointer p = Pointer()) noexcept
        {
            Pointer old = ptr;
            ptr = p;
            if (old)
            {
                deleter(old);
            }
        }

        constexpr const DeleterType& getDeleter() const noexcept
        {
            return deleter;
        }

        constexpr DeleterType& getDeleter() noexcept
        {
            return deleter;
        }

        constexpr  TypeAddLRef<T> operator*() const noexcept
        {
            return *ptr;
        }
        constexpr  Pointer operator->() const noexcept
        {
            return ptr;
        }

        constexpr operator bool() const noexcept
        {
            return ptr != nullptr;
        }

        constexpr void swap(UniquePtr& other) noexcept
        {
            utils::Swap(ptr, other.ptr);
            utils::Swap(deleter, other.deleter);
        }

        friend void Swap(UniquePtr& x, UniquePtr& y) noexcept
        {
            x.swap(y);
        }

        constexpr ElementType& operator[](size_t i) noexcept
        {
            return ptr[i];
        }

        constexpr const ElementType& operator[](size_t i) const noexcept
        {
            return ptr[i];
        }

        template<class A, class B>
        friend decltype(Declval<Pointer>() <=> Declval<Pointer>())
            operator<=> (const UniquePtr<A, B>& a, decltype(nullptr)) noexcept
        {
            return a.ptr <=> static_cast<Pointer>(nullptr);
        }

        template<class A, class B, class C, class D>
        friend decltype(Declval<Pointer>() <=> Declval<Pointer>())
            operator<=> (const UniquePtr<A, B>& a, const UniquePtr<C, D>& b) noexcept
        {
            return a.ptr <=> b.ptr;
        }

        template<class A, class B, class C, class D>
        friend bool operator== (const UniquePtr<A, B>& a, const UniquePtr<C, D>& b) noexcept
        {
            return a.ptr == b.ptr;
        }

        template<class A, class B>
        friend bool operator== (const UniquePtr<A, B>& a, decltype(nullptr)) noexcept
        {
            return a.ptr == nullptr;
        }
    private:
        Pointer ptr;
        [[no_unique_address]]
        DeleterType deleter;
    };

    // TODO: MAKE_UNIQUE

}
