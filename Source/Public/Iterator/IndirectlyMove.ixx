export module ntl.iterator.indirectly_move;
import ntl.iterator.iterator_traits;
import ntl.utils;
import ntl.type_traits;

namespace ne::iters {
    namespace CpoImpl {
        void IndirectlyMove();

        template<class T>
        concept ConceptUnqualifiedIndirectlyMove =
            (TestIsClass<TypeUnCVRef<T>> or TestIsEnum<TypeUnCVRef<T>>) &&
        requires (T&& t) {
            IndirectlyMove(Forward<T>(t));
        };
            
        struct IndirectlyMoveFunctor
        {
            template<class T>
                requires (TestIsClass<TypeUnCVRef<T>> or TestIsEnum<TypeUnCVRef<T>>) &&
                ConceptUnqualifiedIndirectlyMove<T>
            [[nodiscard]] constexpr
            decltype(auto) operator()(T&& i) const 
                noexcept(noexcept(IndirectlyMove(Forward<T>(i))))
            {
                return IndirectlyMove(Forward<T>(i));
            }

            template<class T>
            requires (!((TestIsClass<TypeUnCVRef<T>> or TestIsEnum<TypeUnCVRef<T>>) && ConceptUnqualifiedIndirectlyMove<T>))
            && requires(T&& i) { *Forward<T>(i) ;}
            [[nodiscard]] constexpr
            decltype(auto) operator()(T&& i) const
            noexcept(noexcept(*Forward<T>(i)))
            {
                if constexpr (TestIsLRef<decltype(*Forward<T>(i))>) {
                    return Move(*Forward<T>(i));
                }
                else {
                    return *(Forward<T>(i));
                }
            }
        };
    }
}
export namespace ne::iters {
    inline namespace Cpo {
        // 返回解引用迭代器的右值
        inline constexpr auto IndirectlyMove = CpoImpl::IndirectlyMoveFunctor{};
    }
}

export namespace ne {
    template<class I>
    using TypeIteratorRRef = decltype(iters::IndirectlyMove(Declval<I&>()));
}