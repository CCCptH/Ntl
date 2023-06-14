export module ntl.utils.debug;

// MSVC Macro?
#ifdef _DEBUG
#   define NE_DEBUG
#endif

export namespace ne {
#ifdef NE_DEBUG
    inline constexpr bool DEBUG = true;    
#else
    inline constexpr bool DEBUG = false;
#endif
}