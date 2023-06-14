export module ntl.iterator.back_insert_iterator;
import ntl.iterator.iterator_category;
import ntl.utils;

export namespace ne
{
    template<class Container>
    class BackInsertIterator
    {
    protected:
        Container* ptr;
    public:
        using IteratorCategory = OutputIteratorCategory;
        using ValueType = void;
        using DifferenceType = ptrdiff_t;
        using Pointer = void;
        using Reference = void;
        using ContainerType = Container;
        using ThisType = BackInsertIterator<Container>;

        explicit constexpr BackInsertIterator(Container& c) : ptr(GetAddress(c)) {}
        //constexpr BackInsertIterator(const BackInsertIterator&) = default;
        //constexpr BackInsertIterator() = default;
        //constexpr BackInsertIterator(BackInsertIterator&&)noexcept = default;
        constexpr ThisType& operator*() { return *this; }
        constexpr ThisType& operator++() { return *this; } 
        constexpr ThisType operator++(int) { return *this; }

        constexpr ThisType& operator=(const typename ContainerType::ValueType& v)
        {
            ptr->append(v);
            return *this;
        }
        constexpr ThisType& operator=(typename ContainerType::ValueType&& v)
        {
            ptr->append(Move(v));
            return *this;
        }
    };
    template<class Container>
    constexpr BackInsertIterator<Container> BackInserter(Container& c) noexcept
    {
        return BackInsertIterator<Container>(c);
    }
} // namespace ne
