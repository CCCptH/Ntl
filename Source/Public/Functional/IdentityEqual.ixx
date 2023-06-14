export module ntl.functional.identity_equal;

namespace ne {
    struct IdentityEqualFunctor
    {
        template<class T>
        constexpr
        T&& operator() (T&& t) const {
            return Forward<T>(t);
        }
    };
    
    export inline constexpr auto IdentityEqual = IdentityEqualFunctor{};
    
}