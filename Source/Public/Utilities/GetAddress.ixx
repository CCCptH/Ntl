export module ntl.utils.get_address;

export namespace ne {
    template<class T>
    [[nodiscard]]
    constexpr
    T* GetAddress(T& t) {
        return __builtin_addressof(t);
    }
    template<class T>
    const T* GetAddress(const T&& t) = delete;
}