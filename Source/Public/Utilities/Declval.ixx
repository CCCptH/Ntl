export module ntl.utils.declval;
import ntl.type_traits.type_modifications;
import ntl.type_traits.base_classes;

export namespace ne
{
    template<class A>
    TypeAddRRef<A> Declval() noexcept {
        static_assert(AlWAYS_FALSE<A>, "[ntl.utils.declval] Declval can only be used in unevaluated contexts.");
    }
}