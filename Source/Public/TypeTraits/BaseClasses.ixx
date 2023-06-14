export module ntl.type_traits.base_classes;

export namespace ne {
    template<class T>
    inline constexpr bool AlWAYS_FALSE = false;
    template<class T>
    inline constexpr bool ALWAYS_TRUE = true;
}
export namespace ne {
    /**
     * @brief Same as std::integral_constant
     * @tparam T
     * @tparam V
     */
    template<class T, T V>
    struct IntegralConstant {
        constexpr static T VALUE = V;
        using ValueType = T;
        using Type = IntegralConstant;
        constexpr operator ValueType() const noexcept { return VALUE; }
        constexpr ValueType operator()() const noexcept { return VALUE; }
    };
    /**
     * @brief Same as std::bool_constant
     */
    template<bool B>
    using BooleanConstant = IntegralConstant<bool, B>;
    /**
     * @brief Same as std::true_type
     */
    using TrueConstant = BooleanConstant<true>;
    /**
     * @brief Same as std::false_type
     */
    using FalseConstant = BooleanConstant<false>;

    /**
     * @brief Same as std::void_t
     */
    template<class ...A>
    using TypeVoid = void;
}