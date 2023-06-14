export module ntl.memory.default_deleter;
import ntl.type_traits;

export namespace ne
{
    template<class T>
    struct DefaultDeleter
    {
        constexpr void operator()(T* ptr) const {
            delete ptr;
        }
        constexpr DefaultDeleter() noexcept = default;
        constexpr ~DefaultDeleter() noexcept = default;
        constexpr DefaultDeleter(const DefaultDeleter&) noexcept = default;
        constexpr DefaultDeleter(DefaultDeleter&&) noexcept = default;
        template<class U>
            requires TestIsConvertible<U*, T*>
        constexpr DefaultDeleter(const DefaultDeleter<U>&) noexcept {};
    };
    template<class T>
    struct DefaultDeleter<T[]>
    {
        template<class U>
            requires TestIsConvertible<U(*)[], T(*)[]>
        constexpr void operator()(U* ptr) const
        {
            delete[] ptr;
        }
        constexpr DefaultDeleter() noexcept = default;
        constexpr ~DefaultDeleter() noexcept = default;
        constexpr DefaultDeleter(const DefaultDeleter&) noexcept = default;
        constexpr DefaultDeleter(DefaultDeleter&&) noexcept = default;
        template<class U>
            requires TestIsConvertible<U(*)[], T(*)[]>
        constexpr DefaultDeleter(const DefaultDeleter<U[]>&) noexcept {};
    };
}
