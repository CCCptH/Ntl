

export module ntl.iterator.move_iterator;

import ntl.iterator.iterator_traits;
import ntl.type_traits;
//import ntl.concepts;
import ntl.utils;

export namespace ne{
    template<class Iter>
    class MoveIterator
    {
    private:
        using Traits = IteratorTraits<Iter>;
    public:
        using IteratorType = Iter;
        using DifferenceType = typename Traits::DifferenceType;
        using ValueType = typename Traits::ValueType ;
        using Pointer = typename Traits::Pointer ;
        using Reference = TypeConditional<
                TestIsRef<typename Traits::Reference>,
                TypeUnRef<typename Traits::Reference>&&,
                typename Traits::Reference
                >;
        using IteratorCategory = typename Traits::IteratorCategory;
        
        constexpr
        MoveIterator(): current(){};
        constexpr
        explicit MoveIterator(Iter it)
            noexcept(TestIsNothrowMoveConstructible<Iter>)
            : current(Move(it))
        {}

        template<class U>
            requires (!TestIsSame<Iter, U>) && TestIsConvertible<const U&, Iter> &&  TestIsAssignable<Iter&, const U&>
        constexpr
        MoveIterator(const MoveIterator<U>& u)
            noexcept(TestIsNothrowConstructible<Iter, const U&>)
            : current(u.current)
        {}

        template<class U>
            requires (!TestIsSame<Iter, U>) && TestIsConvertible<const U&, Iter> && TestIsAssignable<Iter&, const U&>
        constexpr
        MoveIterator& operator=(const MoveIterator<U>& u)
            noexcept(TestIsNothrowAssignable<Iter&, const U&>)
        {
            current=u.current;
            return *this;
        }

        [[nodiscard]]
        constexpr const Iter& base() const& noexcept { return current; }
        [[nodiscard]]
        constexpr Reference operator*() const { return static_cast<Reference>(*current); }
        [[nodiscard]]
        constexpr Pointer operator->() const { return current; }
        constexpr MoveIterator& operator++() {
            ++current;
            return *this;
        }

        constexpr MoveIterator operator++(int) {
            auto tmp = *this;
            ++current;
            return tmp;
        }

        constexpr MoveIterator& operator--() {
            --current;
            return *this;
        }

        constexpr MoveIterator operator--(int) {
            auto tmp = *this;
            --current;
            return tmp;
        }

        constexpr MoveIterator operator+(DifferenceType diff) const {
            return MoveIterator(current+diff);
        }

        constexpr MoveIterator operator+=(DifferenceType diff) const {
            current+=diff;
            return *this;
        }

        constexpr MoveIterator operator-(DifferenceType diff) const {
            return MoveIterator(current-diff);
        }

        constexpr MoveIterator operator-=(DifferenceType diff) const {
            current-=diff;
            return *this;
        }

    protected:
        Iter current;
    };

    template<class It1, class It2>
    constexpr bool operator==(const MoveIterator<It1>& lhs, const MoveIterator<It2>& rhs) {
        return lhs.base() == rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator!=(const MoveIterator<It1>& lhs, const MoveIterator<It2>& rhs) {
        return lhs.base() != rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator<(const MoveIterator<It1>& lhs, const MoveIterator<It2>& rhs) {
        return lhs.base() < rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator<=(const MoveIterator<It1>& lhs, const MoveIterator<It2>& rhs) {
        return lhs.base() <= rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator>(const MoveIterator<It1>& lhs, const MoveIterator<It2>& rhs) {
        return lhs.base() > rhs.base();
    }

    template<class It1, class It2>
    constexpr bool operator>=(const MoveIterator<It1>& lhs, const MoveIterator<It2>& rhs) {
        return lhs.base() >= rhs.base();
    }

    template<class It1, class It2>
    constexpr auto operator-(const MoveIterator<It1>& lhs, const MoveIterator<It2>& rhs)
        -> decltype(lhs.base()-rhs.base())
    {
        return lhs.base() - rhs.base();
    }

    template<class Iter>
    constexpr MoveIterator<Iter> operator+(typename MoveIterator<Iter>::difference_type lhs, const MoveIterator<Iter>& rhs) {
        return MoveIterator<Iter>(rhs.base()+lhs);
    }
}