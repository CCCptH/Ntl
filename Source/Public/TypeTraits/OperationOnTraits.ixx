export module ntl.type_traits.operation_on_traits;

import ntl.type_traits.base_classes;

namespace ne {
    template<bool, class T = void>
    struct EnableIfHelper { };
    template<class T>
    struct EnableIfHelper<true, T> { using Type = T; };

    template<bool Cond, class If, class Then>
    struct ConditionalHelper { using Type = If; };
    template<class If, class Then>
    struct ConditionalHelper<false, If, Then> { using Type = Then; };
}

export namespace ne
{
    template<bool Cond, class T>
    using TypeEnableIf = typename EnableIfHelper<Cond, T>::Type;

    template<bool Cond, class If, class Then>
    using TypeConditional = typename ConditionalHelper<Cond, If, Then>::Type;

    template<bool Cond, class T>
    using EnableIf = EnableIfHelper<Cond, T>;

    template<bool Cond, class If, class Then>
    using Conditional = ConditionalHelper<Cond, If, Then>;

    template<class...>
    struct Conjunction : TrueConstant {};
    template<class B>
        requires requires { B::VALUE; }
    struct Conjunction<B> : B {};
    template<class B, class ...Bs>
        requires requires { B::VALUE; }
    struct Conjunction<B, Bs...> : TypeConditional<(bool)(B::VALUE),
        Conjunction<Bs...>, B> {};

    template<class ...Bs>
    inline constexpr bool ValConjunction = Conjunction<Bs...>::VALUE;

    template<class...>
    struct Disjunction : FalseConstant {};
    template<class B>
        requires requires { B::VALUE; }
    struct Disjunction<B> : B {};
    template<class B, class...Bs>
        requires requires { B::VALUE; }
    struct Disjunction<B, Bs...> : TypeConditional<(bool)(B::VALUE),
        B, Disjunction<Bs...>>{};

    template<class...Bs>
    inline constexpr bool ValDisjunction = Disjunction<Bs...>::VALUE;

    template<class B>
        requires requires { B::VALUE; }
    struct Negation : BooleanConstant<!(B::VALUE)> {};

    template<bool ...Bs>
    inline constexpr bool ValAnd = Conjunction<BooleanConstant<Bs>...>::VALUE;
    template<bool ...Bs>
    inline constexpr bool ValOr = Disjunction<BooleanConstant<Bs>...>::VALUE;
    template<bool B>
    inline constexpr bool ValNot = !B;
}