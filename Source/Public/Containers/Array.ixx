export module ntl.containers.array;

import ntl.utils;
import ntl.iterator.reverse_iterator;
import ntl.exceptions;

export namespace ne
{
    template<class Type, size_t N>
    class Array
    {
    public:
        using ValueType = Type;
        using SizeType = int64;
        using DifferenceType = ptrdiff_t;
        using ConstPointer = const ValueType*;
        using Pointer = ValueType*;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Iterator = Pointer;
        using ConstIterator = ConstPointer;
        using ReverseIterator = ReverseIterator<Iterator>;
        using ConstReverseIterator = ne::ReverseIterator<ConstIterator>;

//        constexpr
//        Array() = default;
//        constexpr
//        Array(const Array&)=default;
//        constexpr
//        Array(Array&&) noexcept = default;

        constexpr
        SizeType size() const;

        constexpr
        Reference at(SizeType i);
        constexpr
        ConstReference at(SizeType i) const;

        constexpr
        Reference operator[](SizeType i);
        constexpr
        ConstReference operator[](SizeType i) const;

        constexpr
        Pointer getAddress();
        constexpr
        ConstPointer getAddress() const;

        constexpr
        Iterator begin() noexcept;
        constexpr
        Iterator end() noexcept;
        constexpr
        ConstIterator begin() const noexcept;
        constexpr
        ConstIterator end() const noexcept;
        constexpr
        ConstIterator cbegin() const noexcept;
        constexpr
        ConstIterator cend() const noexcept;
        constexpr
        ReverseIterator rbegin() noexcept;
        constexpr
        ReverseIterator rend() noexcept;
        constexpr
        ConstReverseIterator rbegin() const noexcept;
        constexpr
        ConstReverseIterator rend() const noexcept;
        constexpr
        ConstReverseIterator crbegin() const noexcept;
        constexpr
        ConstReverseIterator crend() const noexcept;

        ValueType __elems[N];
    private:
    };

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::size() const -> SizeType {
        return N;
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::at(Array::SizeType i) -> Reference {
        if (i>=N) throw OutOfRange{"[ntl.containers.array] Index is out of range"};
        return __elems[i];
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::at(Array::SizeType i) const -> ConstReference {
        if (i>=N) throw OutOfRange{ "[ntl.containers.array] Index is out of range" };
        return __elems[i];
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::operator[](Array::SizeType i) -> Reference {
        return __elems[i];
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::operator[](Array::SizeType i) const -> ConstReference {
        return __elems[i];
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::getAddress() -> Pointer {
        return &__elems[0];
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::getAddress() const -> ConstPointer {
        return &__elems[0];
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::begin() noexcept -> Iterator {
        return getAddress();
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::end() noexcept -> Iterator {
        return getAddress()+size();
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::begin() const noexcept -> ConstIterator {
        return getAddress();
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::end() const noexcept -> ConstIterator {
        return getAddress()+size();
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::cbegin() const noexcept -> ConstIterator {
        return begin();
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::cend() const noexcept -> ConstIterator {
        return end();
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::rbegin() noexcept -> ReverseIterator {
        return ReverseIterator(end());
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::rend() noexcept -> ReverseIterator {
        return ReverseIterator(begin());
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::rbegin() const noexcept -> ConstReverseIterator {
        return ConstReverseIterator(end());
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::rend() const noexcept -> ConstReverseIterator {
        return ConstReverseIterator(begin());
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::crbegin() const noexcept -> ConstReverseIterator {
        return rbegin();
    }

    template<class Type, size_t N>
    constexpr
    auto Array<Type, N>::crend() const noexcept -> ConstReverseIterator {
        return rend();
    }


}