export module ntl.iterator.reverse_iterator;

import ntl.iterator.iterator_traits;
import ntl.type_traits;
//import ntl.concepts;

export namespace ne {
    template<class Iter>
    class ReverseIterator
    {
    protected:
        using Traits = IteratorTraits<Iter>;
    public:
        using IteratorType = Iter;
        using DifferenceType = typename Traits::DifferenceType;
        using ValueType = typename Traits::ValueType;
        using Pointer = typename Traits::Pointer;
        using Reference = typename Traits::Reference ;
        using IteratorCategory = typename Traits::IteratorCategory;
        
        constexpr ReverseIterator() : current() {}
        constexpr explicit ReverseIterator(Iter x)
            noexcept(TestIsNothrowMoveConstructible<Iter>)
            : current(x)
        {}

        template<class Other>
            requires (!TestIsSame<Iter, Other>)
            && ConceptConvertibleTo<const Other&, Iter>
        constexpr ReverseIterator(const ReverseIterator<Other>& x)
            noexcept(TestIsNothrowConstructible<Iter, const Other&>)
            : current(x.current)
        {}

        template<class Other>
            requires (!TestIsSame<Iter, Other>)
            && ConceptConvertibleTo<const Other&, Iter>
            && ConceptAssignableFrom<Iter&, const Other&>
        constexpr ReverseIterator& operator=(const ReverseIterator<Other>& x)
        {
            current=x.current;
            return *this;
        }

        [[nodiscard]] constexpr Iter base() const { return current; }
        [[nodiscard]] constexpr Reference operator*() { auto t = current; return *(--t); }
        [[nodiscard]] constexpr Pointer operator->() const
            requires (TestIsPointer<Iter>
                || requires (const Iter i){i.operator->();})
        {
            Iter t=current;
            --t;
            if constexpr(TestIsPointer<Iter>) {
                return t;
            }
            else {
                return t.operator->();
            }
        }

        constexpr ReverseIterator& operator++() {
            --current;
            return *this;
        }

        constexpr ReverseIterator operator++(int) {
            auto tmp = *this;
            --current;
            return tmp;
        }

        constexpr ReverseIterator& operator--() {
            ++current;
            return *this;
        }

        constexpr ReverseIterator operator--(int) {
            auto tmp = *this;
            ++current;
            return tmp;
        }

        constexpr ReverseIterator operator+(DifferenceType n) const {
            return ReverseIterator(current-n);
        }

        constexpr ReverseIterator& operator+=(DifferenceType n) {
            current-=n;
            return *this;
        }

        constexpr ReverseIterator operator-(DifferenceType n) const {
            return ReverseIterator(current+n);
        }

        constexpr ReverseIterator& operator-=(DifferenceType n) {
            current+=n;
            return *this;
        }

        constexpr Reference operator[](DifferenceType n) {
            return *((*this)+n);
        }

    protected:
        Iter current;
    };

    template<class It1, class It2>
    constexpr bool operator==(const ReverseIterator<It1>& lhs, const ReverseIterator<It2>& rhs) {
        return lhs.base()==rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator!=(const ReverseIterator<It1>& lhs, const ReverseIterator<It2>& rhs) {
        return lhs.base()!=rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator<(const ReverseIterator<It1>& lhs, const ReverseIterator<It2>& rhs) {
        return lhs.base()>rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator<=(const ReverseIterator<It1>& lhs, const ReverseIterator<It2>& rhs) {
        return lhs.base()>=rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator>(const ReverseIterator<It1>& lhs, const ReverseIterator<It2>& rhs) {
        return lhs.base()<rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator>=(const ReverseIterator<It1>& lhs, const ReverseIterator<It2>& rhs) {
        return lhs.base()<=rhs.base();
    }

    template<class It1, class It2>
    constexpr auto operator-(const ReverseIterator<It1>& lhs, const ReverseIterator<It2>& rhs)
        ->decltype(lhs.base()-rhs.base())
    {
        return rhs.base() - lhs.base();
    }
}