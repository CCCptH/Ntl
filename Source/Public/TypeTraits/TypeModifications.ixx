export module ntl.type_traits.type_modifications;
export import ntl.utils.nint;

namespace ne {
        template<class A>
        struct TypeUnConstHelper {using Type = A;};
        template<class A>
        struct TypeUnConstHelper<const A>  {using Type = A;};

        template<class A>
        struct TypeUnVolatileHelper {using Type=A;};
        template<class A>
        struct TypeUnVolatileHelper<volatile A>{using Type = A;};

        template<class A>
        struct TypeUnRefHelper{ using Type = A; };
        template<class A>
        struct TypeUnRefHelper<A&>{using Type=A;};
        template<class A>
        struct TypeUnRefHelper<A&&>{using Type=A;};

        template<class A>
        struct TypeUnPtrHelper {using Type = A;};
        template<class A>
        struct TypeUnPtrHelper<A*>{using Type = A;};
        template<class A>
        struct TypeUnPtrHelper<A* const>{using Type = A;};
        template<class A>
        struct TypeUnPtrHelper<A* volatile>{using Type = A;};
        template<class A>
        struct TypeUnPtrHelper<A* const volatile>{using Type = A;};

        template<class A>
        struct TypeIdentityHelper {using Type = A;};

        template<class A>
        struct TypeAddConstHelper {using Type = const A;};
        template<class A>
        struct TypeAddVolatileHelper {using Type = volatile A;};

        template<class A>
        auto TryAddPtr(int) -> TypeIdentityHelper<typename TypeUnRefHelper<A>::Type*>;
        template<class A>
        auto TryAddPtr(...)  -> TypeIdentityHelper<A>;

        template<class A>
        struct TypeAddPtrHelper: decltype(TryAddPtr<A>(0)){};

        template<class A>
        auto TryAddLRef(int)->TypeIdentityHelper<A&>;
        template<class A>
        auto TryAddLRef(...)->TypeIdentityHelper<A>;
        template<class A>
        auto TryAddRRef(int)->TypeIdentityHelper<A&&>;
        template<class A>
        auto TryAddRRef(...)->TypeIdentityHelper<A>;

        template<class A>
        struct TypeAddLRefHelper: decltype(TryAddLRef<A>(0)){};
        template<class A>
        struct TypeAddRRefHelper: decltype(TryAddRRef<A>(0)){};

        template<class A>
        struct TypeRemoveExtentHelper
        {
            using Type = A;
        };
        template<class A>
        struct TypeRemoveExtentHelper<A[]>{
            using Type = A;
        };
        template<class A, size_t N>
        struct TypeRemoveExtentHelper<A[N]>{
            using Type = A;
        };

        template<class A>
        struct TypeRemoveAllExtentHelper
        {
            using Type = A;
        };
        template<typename A>
        struct TypeRemoveAllExtentHelper<A[]>{
            using Type = typename TypeRemoveAllExtentHelper<A>::Type;
        };
        template<typename A, size_t N>
        struct TypeRemoveAllExtentHelper<A[N]>{
            using Type = typename TypeRemoveAllExtentHelper<A>::Type ;
        };
    
}
export namespace ne {
    /**
     * @brief Same as std::remove_const
     */
    template<class A>
    using TypeUnConst = typename TypeUnConstHelper<A>::Type;
    /**
     * @breif Same as std::remove_volatile
     */
    template<class A>
    using TypeUnVolatile = typename TypeUnVolatileHelper<A>::Type;
    /**
     * @brief Same as std::remove_cv
     */
    template<class A>
    using TypeUnCV = TypeUnConst<TypeUnVolatile<A>>;
    /**
     * @breif Same as std::remove_reference
     */
    template<class A>
    using TypeUnRef = typename TypeUnRefHelper<A>::Type;
    /**
     * @brief Same as std::remove_cvref
     */
    template<class A>
    using TypeUnCVRef = TypeUnRef<TypeUnCV<A>>;
    /**
     * @brief Same as std::remove_pointer
     */
    template<class A>
    using TypeUnPtr = typename TypeUnPtrHelper<A>::Type;
    template<class A>
    using TypeIdentity = typename TypeIdentityHelper<A>::Type;
    /**
     * @brief Same as std::add_const
     */
    template<class A>
    using TypeAddConst = typename TypeAddConstHelper<A>::Type;
    /**
     * @brief Same as std::add_volatile
     */
    template<class A>
    using TypeAddVolatile = typename TypeAddVolatileHelper<A>::Type;
    /**
     * @brief Same as std::add_cv
     */
    template<class A>
    using TypeAddCV = TypeAddConst<TypeAddVolatile<A>>;
    /**
     * @brief Same as std::add_pointer
     */
    template<class A>
    using TypeAddPointer = typename TypeAddPtrHelper<A>::Type;
    /**
     * @brief Same as std::add_lvalue_reference
     */
    template<class A>
    using TypeAddLRef = typename TypeAddLRefHelper<A>::Type;
    /**
     * @brief Same as std::add_rvalue_reference
     */
    template<class A>
    using TypeAddRRef = typename TypeAddRRefHelper<A>::Type;
    /**
     * @brief Same as std::remove_extent
     */
     template<class A>
     using TypeRemoveExtent = typename TypeRemoveExtentHelper<A>::Type;
    /**
    * @brief Same as std::remove_all_extents
    */
    template<class A>
    using TypeRemoveAllExtents = typename TypeRemoveAllExtentHelper<A>::Type;
}