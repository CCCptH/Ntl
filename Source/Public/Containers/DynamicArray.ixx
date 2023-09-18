export module ntl.containers.dynamic_array;
import ntl.iterator;
import ntl.iterator.iterator_category;
import ntl.type_traits;
import ntl.utils;
import ntl.memory.allocator;
import ntl.exceptions;
//import ntl.compare;

namespace ne {
    template<class DArr>
    class DynamicArrayIterator;

    template<class DArr>
    class DynamicArrayConstIterator;

}

export namespace ne
{
    template<class Type>
    class DynamicArray
    {
    public:
        using ThisType = DynamicArray<Type>;
        using ValueType = Type;
        using Pointer = Type*;
        using ConstPointer = const Type*;
        using SizeType = int64;
        using Reference = Type&;
        using ConstReference = const Type&;
        using Iterator = DynamicArrayIterator<ThisType>;
        using ConstIterator = DynamicArrayConstIterator<ThisType>;
        using ReverseIterator = ne::ReverseIterator<Iterator>;
        using ConstReverseIterator = ne::ReverseIterator<ConstIterator>;

        DynamicArray() noexcept;
        /**
         * @brief 使用默认构造函数在容器内构造<b>n</b>个对象
         * @param[in] n - 默认构造的对象数量
         */
        explicit DynamicArray(SizeType n);

        /**
         * @brief 使用<b>n</b>个x构造容器
         * @param[in] n - 数量
         * @param[in] x - 对象的值
         */
        DynamicArray(SizeType n, const Type& x);

        DynamicArray(const DynamicArray& v);

        DynamicArray(DynamicArray&& v) noexcept;

        DynamicArray(InitList<Type> initList);

        DynamicArray& operator=(const DynamicArray& v);

        DynamicArray& operator=(DynamicArray&& v);

        DynamicArray& operator=(InitList<Type> i);

        ~DynamicArray();

        /**
         * @brief 在末尾插入元素
         *
         * @param[in] elem - 插入的元素
         * @return DynamicArray& - 返回自身的引用
         */
        DynamicArray& append(const Type& elem);

        /**
         * @brief 从末尾插入元素(右值)
         *
         * @param[in] elem - 插入的元素
         * @return DynamicArray& - 返回自身的引用
         */
        DynamicArray& append(Type&& elem);

        /**
         * @brief 从开始的位置插入元素
         *
         * @param[in] elem - 插入的元素
         * @return DynamicArray&
         */
        DynamicArray& prepend(const Type& elem);

        /**
         * @brief 从开始的位置插入元素
         *
         * @param[in] elem - 插入的元素
         * @return DynamicArray&
         */
        DynamicArray& prepend(Type&& elem);

        /**
         * @brief 在iter位置插入一个元素
         *
         * @tparam Args
         * @param iter
         * @param args
         * @return Iterator
         */
        template<typename ...Args>
        Iterator emplace(Iterator iter, Args&& ...args);

        /**
         * @brief 在末尾构造一个元素
         *
         * @tparam Args
         * @param args
         */
        template<typename ...Args>
        DynamicArray& emplaceBack(Args&& ...args);

        [[nodiscard]]
        Reference at(SizeType i);

        [[nodiscard]]
        ConstReference at(SizeType i) const;

        template<ConceptInputIterator InputIterator>
        void assign(InputIterator f, InputIterator l);
        void assign(SizeType n, const Type& t);
        void assign(InitList<Type> il);

        [[nodiscard]]
        Reference operator[](SizeType i);

        [[nodiscard]]
        ConstReference operator[](SizeType i) const;

        [[nodiscard]]
        SizeType size() const noexcept;

        [[nodiscard]]
        SizeType capacity() const noexcept;

        [[nodiscard]]
        bool isEmpty() const noexcept;

        /**
         * @brief 第一个元素
         *
         * @return Type&
         */
        [[nodiscard]]
        Reference front();

        /**
         * @brief 第一个元素
         *
         * @return const Type&
         */
        [[nodiscard]]
        ConstReference front() const;

        /**
         * @brief 最后一个元素
         *
         * @return Type&
         */
        [[nodiscard]]
        Reference back();

        /**
         * @brief 最后一个元素
         *
         * @return const Type&
         */
        [[nodiscard]]
        ConstReference back() const;

        /**
         * @brief 将capacity调整到size大小
         *
         */
        void squeeze();

        /**
         * @brief 设置capacity为n
         *
         */
        void reserve(SizeType n);

        /**
         * @brief 清空所有元素
         *
         */
        void clear();

        /**
         * @brief 在位置i插入一个元素
         *
         * @param i
         * @param v
         */
        void insert(SizeType i, Type&& v);

        /**
         * @brief 在位置i插入一个元素
         *
         * @param i
         * @param v
         */
        void insert(SizeType i, const Type& v);

        // void insert(SizeType i, SizeType count, const Type& v);

        /**
         * @brief 在迭代器iter位置插入一个元素
         *
         * @param iter
         * @param v
         * @return Iterator
         */
        Iterator insert(ConstIterator iter, Type&& v);

        /**
         * @brief 在迭代器iter位置插入一个元素
         *
         * @param iter
         * @param v
         * @return Iterator
         */
        Iterator insert(ConstIterator iter, const Type& v);

        // Iterator insert(Iterator iter, SizeType count, const T& v);

        /**
         * @brief 移除元素
         *
         * @param i
         */
        void remove(SizeType i);

        // void remove(SizeType i, SizeType count);

        /**
         * @brief 移除iter位置元素
         *
         * @param iter
         * @return Iterator
         */
        Iterator remove(ConstIterator iter);

        /**
         * @brief 移除iter位置元素
         *
         * @param begin
         * @param end
         * @return Iterator
         */
        Iterator remove(ConstIterator begin, ConstIterator end);

        [[nodiscard]]
        Iterator begin() noexcept;
        [[nodiscard]]
        Iterator end() noexcept;

        [[nodiscard]]
        ConstIterator begin() const noexcept;
        [[nodiscard]]
        ConstIterator end() const noexcept;

        [[nodiscard]]
        ConstIterator cbegin() const noexcept;
        [[nodiscard]]
        ConstIterator cend() const noexcept;

        [[nodiscard]]
        ReverseIterator rbegin() noexcept;
        [[nodiscard]]
        ReverseIterator rend() noexcept;

        [[nodiscard]]
        ConstReverseIterator rbegin() const noexcept;
        [[nodiscard]]
        ConstReverseIterator rend() const noexcept;

        [[nodiscard]]
        ConstReverseIterator crbegin() const noexcept;
        [[nodiscard]]
        ConstReverseIterator crend() const noexcept;

        Type* getAddress() noexcept;

        const Type* getAddress() const noexcept;
    private:
        Pointer p_begin;
        Pointer p_end;
        Pointer p_cap;

        static constexpr SizeType MAX_ITEM_SIZE = static_cast<SizeType>(-1);
        Allocator allocator;

        Pointer allocateAndSetBeginAndCap(SizeType n);
        Pointer reallocAndResetData(SizeType n);

        template<class It>
        void copyFrom(Pointer b, It sb, It se);
        template<class It>
        void moveFrom(Pointer b, It sb, It se);

        SizeType getNewCapacity(SizeType n) const {
            constexpr SizeType lin_growth = 1024*1024;
            [[unlikely]]
            if (n+lin_growth < n) {
                throw OutOfRange{ "[ntl.containers.dynamic_array] Capacity if out of range" };
            }
            [[unlikely]]
            if (n >= lin_growth) {
                return n+lin_growth;
            }
            else {
                return n==0 ? 8 : n*2;
            }
        }

        void shiftRearward(Pointer p);
        void shiftForward(Pointer p);
        Pointer reallocAndShiftRearward(SizeType n, Pointer p);
    };
}

namespace ne
{
    template<class DArr>
    class DynamicArrayIterator
    {
    public:
        using IteratorCategory = RandomAccessIteratorCategory;
        using ThisType = DynamicArrayIterator<DArr>;
        using ValueType = typename DArr::ValueType;
        using Pointer = ValueType*;
        using Reference = ValueType&;
        using DifferenceType = decltype(Declval<Pointer>() - Declval<Pointer>());
    private:
        Pointer ptr;
    public:

        DynamicArrayIterator() :ptr(nullptr) {}
        DynamicArrayIterator(Pointer p) : ptr(p) {};
        DynamicArrayIterator(const ThisType&) noexcept = default;
        DynamicArrayIterator(ThisType&&) noexcept = default;
        ~DynamicArrayIterator() = default;

        ThisType& operator=(const ThisType&) noexcept = default;
        ThisType& operator=(ThisType&&) noexcept = default;

        Reference operator[](DifferenceType n) { return ptr[n]; }

        Reference operator*() const noexcept { return *ptr; };
        Pointer operator->() const noexcept { return ptr; }
        ThisType& operator++() noexcept { ++ptr; return *this; }
        ThisType operator++(int) noexcept { auto old = *this; ++(*this); return old; }
        ThisType& operator--() noexcept { --ptr; return *this; }
        ThisType operator--(int) noexcept { auto old = *this; --(*this); return old; }

        friend decltype(Declval<Pointer>() <=> Declval<Pointer>()) operator<=>(const ThisType& a, const ThisType& b) noexcept { return a.ptr <=> b.ptr; }
        friend bool operator==(const ThisType& a, const ThisType& b) noexcept { return a.ptr == b.ptr; }

        friend ThisType operator+(const ThisType& lhs, DifferenceType rhs) {
            return lhs.ptr + rhs;
        }
        friend ThisType operator+(DifferenceType lhs, const ThisType& rhs) {
            return lhs + rhs.ptr;
        }
        friend ThisType& operator+=(const ThisType& lhs, DifferenceType rhs) {
            lhs.ptr += rhs;
            return lhs;
        }
        friend ThisType operator-(const ThisType& lhs, DifferenceType rhs) {
            return lhs.ptr - rhs;
        }
        friend DifferenceType operator-(const ThisType& lhs, const ThisType& rhs) {
            return lhs.ptr - rhs.ptr;
        }
        friend ThisType& operator-=(const ThisType& lhs, DifferenceType rhs) {
            lhs.ptr -= rhs;
            return lhs;
        }

        template<class DArr>
        friend class DynamicArrayConstIterator;
        template<class T>
        friend class DynamicArray;
    };

    template<class DArr>
    class DynamicArrayConstIterator
    {
    public:
        using ThisType = DynamicArrayConstIterator<DArr>;
        using IteratorCategory = RandomAccessIteratorCategory;
        using ValueType = typename DArr::ValueType;
        using Pointer = const ValueType*;
        using Reference = const ValueType&;
        using DifferenceType = decltype(Declval<Pointer>() - Declval<Pointer>());

        DynamicArrayConstIterator() :ptr(nullptr) {}
        DynamicArrayConstIterator(Pointer p) : ptr(p) {};
        DynamicArrayConstIterator(const DynamicArrayIterator<DArr>& it) : ptr(it.ptr) {}
        DynamicArrayConstIterator(const ThisType&) noexcept = default;
        DynamicArrayConstIterator(ThisType&&) noexcept = default;
        ~DynamicArrayConstIterator() = default;

        ThisType& operator=(const ThisType&) noexcept = default;
        ThisType& operator=(ThisType&&) noexcept = default;

        Reference operator*() const noexcept { return *ptr; };
        Pointer operator->() const noexcept { return ptr; }
        ThisType& operator++() noexcept { ++ptr; return *this; }
        ThisType operator++(int) noexcept { auto old = *this; ++(*this); return old; }
        ThisType& operator--() noexcept { --ptr; return *this; }
        ThisType operator--(int) noexcept { auto old = *this; --(*this); return old; }

        friend decltype(Declval<Pointer>() <=> Declval<Pointer>()) operator<=>(const ThisType& a, const ThisType& b) noexcept { return a.ptr <=> b.ptr; }
        friend bool operator==(const ThisType& a, const ThisType& b) noexcept { return a.ptr == b.ptr; }

        friend ThisType operator+(const ThisType& lhs, DifferenceType rhs) {
            return lhs.ptr + rhs;
        }
        friend ThisType operator+(DifferenceType lhs, const ThisType& rhs) {
            return lhs + rhs.ptr;
        }
        ThisType& operator+=(DifferenceType rhs) {
            ptr += rhs;
            return *this;
        }
        friend ThisType operator-(const ThisType& lhs, DifferenceType rhs) {
            return lhs.ptr - rhs;
        }
        friend DifferenceType operator-(const ThisType& lhs, const ThisType& rhs) {
            return lhs.ptr - rhs.ptr;
        }
        ThisType& operator-=(DifferenceType rhs) {
            ptr -= rhs;
            return *this;
        }

        template<class T>
        friend class DynamicArray;

    private:
        Pointer ptr;
    };

    template<class Type>
    auto DynamicArray<Type>::allocateAndSetBeginAndCap(SizeType n) -> Pointer {
        this->p_begin = allocator.template allocate<Type>(n);
        p_cap = p_begin + n;
        return p_begin;
    }

    template<class Type>
    template<class It>
    void DynamicArray<Type>::copyFrom(Pointer b, It sb, It se) {
        for(; sb != se; ++sb) {
            Construct(b, (*sb));
            ++b;
        }
    }

    template<class Type>
        template<class It>
    void DynamicArray<Type>::moveFrom(Pointer b, It sb, It se) {
        for (; sb<se;++sb) {
            Construct(b, Move(*sb));
            ++b;
        }
    }

    template<class Type>
    auto DynamicArray<Type>::reallocAndResetData(SizeType n) -> Pointer {
        auto new_begin = allocator.template allocate<Type>(n);
        auto diff = p_end - p_begin;
        moveFrom(new_begin, p_begin, p_end);
        DestructN(p_begin, diff);
        allocator.deallocate(p_begin);
        p_begin = new_begin;
        p_end = new_begin+diff;
        p_cap = new_begin+n;
        return new_begin;
    }

    template<class Type>
    void DynamicArray<Type>::shiftRearward(Pointer p) {
        auto next = p_end;
        auto prev = p_end - 1;
        while (next > p) {
            Construct(next, Move(*prev));
            Destruct(prev);
            --next;
            --prev;
        }
        ++p_end;
    }

    template<class Type>
    void DynamicArray<Type>::shiftForward(Pointer p) {
        for(;p<p_end;++p) {
            Construct(p-1, Move(*p));
            Destruct(p);
        }
        --p_end;

    }

    template<class Type>
    auto DynamicArray<Type>::reallocAndShiftRearward(SizeType n, Pointer p) -> Pointer {
        auto new_begin = allocator.template allocate<Type>(n);
        auto diff = p_end - p_begin + 1;
        moveFrom(new_begin, p_begin, p);
        moveFrom(new_begin+(p-p_begin)+1, p, p_end);
        DestructN(p_begin, diff-1);
        allocator.deallocate(p_begin);
        p_begin = new_begin;
        p_end = new_begin+diff;
        p_cap = new_begin+n;
        return p_begin;
    }

    template<class Type>
    DynamicArray<Type>::DynamicArray() noexcept
            : p_begin(nullptr)
            , p_end(nullptr)
            , p_cap(nullptr)
            , allocator()
    {}

    template<class Type>
    DynamicArray<Type>::DynamicArray(SizeType n)
            : allocator()
    {
        static_assert(TestIsDefaultConstructible<Type>, "[ntl.containers.dynamic_array] This constructor expect Type to be default constructible");
        this->allocateAndSetBeginAndCap(n);
        this->p_end = this->p_begin+n;
        ConstructN(this->p_begin, this->p_end);
    }

    template<class Type>
    DynamicArray<Type>::DynamicArray(SizeType n, const Type& x) {
        static_assert(TestIsCopyConstructible<Type>, "[ntl.containers.dynamic_array] This constructor expect Type to be copy constructible");
        this->allocateAndSetBeginAndCap(n);
        this->p_end = this->p_begin+n;
        ConstructN(this->p_begin, this->p_end, x);
    }

    template<class Type>
    DynamicArray<Type>::DynamicArray(const DynamicArray<Type> &v) {
        allocator = v.allocator;
        auto n = v.size();
        allocateAndSetBeginAndCap(n);
        p_end = p_begin+n;
        copyFrom(p_begin, v.begin(), v.end());
    }

    template<class Type>
    DynamicArray<Type>::DynamicArray(DynamicArray<Type> &&v) noexcept {
        this->allocator = Move(v.allocator);
        p_begin = v.p_begin;
        p_end = v.p_end;
        p_cap = v.p_cap;
        v.p_begin = v.p_end = v.p_cap = nullptr;
    }

    template<class Type>
    DynamicArray<Type>::DynamicArray(InitList<Type> initList)
            : allocator()
    {
        allocateAndSetBeginAndCap(initList.size());
        p_end = p_begin+initList.size();
        copyFrom(p_begin, initList.begin(), initList.end());
    }

    template<class Type>
    auto DynamicArray<Type>::operator=(const DynamicArray<Type> &v) -> ThisType& {
        if (this->allocator != v.allocator) {
            clear();
            this->allocator = v.allocator;
            allocateAndSetBeginAndCap(v.size());
            copyFrom(v.begin(), v.end());
        }
        else {
            this->template assign(v.begin(), v.end());
        }
        return *this;
    }

    template<class Type>
    auto DynamicArray<Type>::operator=(DynamicArray<Type> &&v) -> ThisType& {
        clear();
        if (this->allocator == v.allocator) {
            p_begin = v.p_begin;
            p_end = v.p_end;
            p_cap = v.p_cap;
            v.p_begin = v.p_end = v.p_cap;
        }
        else {
            for (auto it = MoveIterator(v.begin()); it != MoveIterator(v.end()); ++it) {
                emplaceBack((*it));
            }
            DestructN(v.p_begin, v.size());
            v.allocator.template deallocate(v.p_begin);
            v.p_begin = v.p_end = v.p_cap = nullptr;
            v.sz = 0;
        }
        return *this;
    }

    template<class Type>
    auto DynamicArray<Type>::operator=(InitList<Type> i) -> ThisType& {
        this->template assign(i.begin(), i.end());
    }

    template<class Type>
    DynamicArray<Type>::~DynamicArray() {
        if (p_begin != nullptr) {
            DestructN(p_begin, this->size());
            allocator.deallocate(p_begin);
        }
    }

    template<class Type>
    auto DynamicArray<Type>::append(Type &&elem) -> ThisType& {
        if (p_end != p_cap) {
            Construct(p_end, Move(elem));
        }
        else {
            auto n = size();
            reallocAndResetData(getNewCapacity(n));
            Construct(p_end, Move(elem));
        }
        ++p_end;
        return *this;
    }

    template<class Type>
    auto DynamicArray<Type>::append(const Type &elem) ->ThisType& {
        if (p_end != p_cap) {
            Construct(p_end, elem);
        }
        else {
            auto n = size();
            reallocAndResetData(getNewCapacity(n));
            Construct(p_end, elem);
        }
        ++p_end;
        return *this;
    }

    template<class Type>
    auto DynamicArray<Type>::prepend(const Type &elem) -> ThisType& {
        if (p_end != p_cap) {
            shiftRearward(p_begin);
            Construct(p_begin, elem);
        }
        else {
            reallocAndShiftRearward(getNewCapacity(size()+1), p_begin);
            Construct(p_begin, elem);
        }
        return *this;
    }

    template<class Type>
    auto DynamicArray<Type>::prepend(Type &&elem) ->ThisType& {
        if (p_end != p_cap) {
            shiftRearward(p_begin);
            Construct(p_begin, Move(elem));
        }
        else {
            reallocAndShiftRearward(getNewCapacity(size()+1), p_begin);
            Construct(p_begin, Move(elem));
        }
        return *this;
    }

    template<class Type>
    template<class ...Args>
    auto DynamicArray<Type>::emplace(Iterator iter, Args &&...args) -> Iterator {
        if (iter.ptr > p_end || iter.ptr < p_begin) {
            throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        }
        if (p_end != p_cap) {
            auto pos = iter.ptr;
            shiftRearward(pos);
            Construct(pos, Forward<Args>(args)...);
            return pos;
        }
        else {
            auto diff = iter.ptr - p_begin;
            reallocAndShiftRearward(getNewCapacity(size()+1), iter);
            auto pos = p_begin+diff;
            Construct(pos, Forward<Args>(args)...);
            return pos;
        }
    }

    template<class Type>
    template<class ...Args>
    auto DynamicArray<Type>::emplaceBack(Args &&...args) -> ThisType& {
        if (p_end != p_cap) {
            Construct(p_end, Forward<Args>(args)...);
        }
        else {
            reallocAndResetData(getNewCapacity(size()));
            Construct(p_end, Forward<Args>(args)...);
        }
        return *this;
    }

    template<class Type>
    auto DynamicArray<Type>::at(SizeType i) -> Reference {
        if (i>=size()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        return p_begin[i];
    }

    template<class Type>
    auto DynamicArray<Type>::at(SizeType i) const -> ConstReference {
        if (i>=size()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        return p_begin[i];
    }

    template<class Type>
    template<ConceptInputIterator InputIterator>
    void DynamicArray<Type>::assign(InputIterator f, InputIterator l) {
        clear();
        for(;f!=l;++f) {
            emplaceBack(*f);
        }
    }

    template<class Type>
    void DynamicArray<Type>::assign(SizeType n, const Type& t) {
        clear();
        for(;n>0;n--) {
            emplaceBack(t);
        }
    }

    template<class Type>
    void DynamicArray<Type>::assign(InitList<Type> il) {
        clear();
        for(auto it = il.begin(); it!=il.end(); ++it){
            emplaceBack((*it));
        }
    }

    template<class Type>
    auto DynamicArray<Type>::operator[](SizeType i) -> Reference {
        return p_begin[i];
    }

    template<class Type>
    auto DynamicArray<Type>::operator[](SizeType i) const -> ConstReference {
        return p_begin[i];
    }

    template<class Type>
    auto DynamicArray<Type>::size() const noexcept -> SizeType {
        return p_end-p_begin;
    }

    template<class Type>
    auto DynamicArray<Type>::capacity() const noexcept -> SizeType {
        return p_cap-p_begin;
    }

    template<class Type>
    bool DynamicArray<Type>::isEmpty() const noexcept {
        return p_end==p_begin;
    }

    template<class Type>
    auto DynamicArray<Type>::front() -> Reference {
        if (isEmpty()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        return *p_begin;
    }

    template<class Type>
    auto DynamicArray<Type>::front() const -> ConstReference {
        if (isEmpty()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        return *p_begin;
    }

    template<class Type>
    auto DynamicArray<Type>::back() -> Reference {
        if (isEmpty()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        return *(p_end-1);
    }

    template<class Type>
    auto DynamicArray<Type>::back() const -> ConstReference {
        if (isEmpty()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        return *(p_end-1);
    }

    template<class Type>
    void DynamicArray<Type>::squeeze() {
        reallocAndResetData(size());
    }

    template<class Type>
    void DynamicArray<Type>::reserve(SizeType n) {
        auto new_begin = allocator.allocate<Type>(n);
        auto it_new = new_begin;
        auto it_old = p_begin;
        if (n >= size())
        {
            while (it_old < p_end) {
                Construct(it_new, Move(*it_old));
                Destruct(it_old);
                ++it_new;
                ++it_old;
            }
        }
        else
        {
	        while(it_new < new_begin+n)
	        {
                Construct(it_new, Move(*it_old));
                Destruct(it_old);
                ++it_new;
                ++it_old;
	        }
            while(it_old < p_end)
            {
                Destruct(it_old);
                ++it_old;
            }
        }
        allocator.deallocate(p_begin);
        p_begin = new_begin;
        p_end = new_begin + n;
        p_cap = new_begin + n;
    }

    template<class Type>
    void DynamicArray<Type>::clear() {
        DestructN(p_begin, this->size());
        p_end = p_begin;
    }

    template<class Type>
    void DynamicArray<Type>::insert(SizeType i, Type &&v) {
        if (i > size()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        auto pos = p_begin+i;
        if (p_end != p_cap) {
            shiftRearward(pos);
            Construct(pos, Move(v));
        }
        else {
            reallocAndShiftRearward(getNewCapacity(size()), pos);
            pos = p_begin + i;
            Construct(pos, Move(v));
        }
    }

    template<class Type>
    void DynamicArray<Type>::insert(SizeType i, const Type &v) {
        if (i > size()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        auto pos = p_begin+i;
        if (p_end != p_cap) {
            shiftRearward(pos);
            Construct(pos, v);
        }
        else {
            reallocAndShiftRearward(getNewCapacity(size()), pos);
            pos = p_begin + i;
            Construct(pos, v);
        }
    }

    template<class Type>
    auto DynamicArray<Type>::insert(ConstIterator iter, Type&& v) -> Iterator
    {
        [[unlikely]]
	    if (iter < begin() || iter > end()) throw OutOfRange{"[ntl.containers.dynamic_array] Index is out of range"};
    	auto ptr = const_cast<Pointer>(iter.ptr);
	    if (p_end != p_cap)
	    {
		    shiftRearward(ptr);
		    Construct(ptr, Move(v));
		    return ptr;
	    }
	    else
	    {
		    auto diff = iter.ptr - p_begin;
		    reallocAndShiftRearward(getNewCapacity(size()), ptr);
		    auto pos = p_begin + diff;
		    Construct(pos, Move(v));
		    return pos;
	    }
    }

    template<class Type>
    auto DynamicArray<Type>::insert(ConstIterator iter, const Type &v) -> Iterator {
        if (iter.ptr<p_begin || iter.ptr > p_end) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        auto ptr = const_cast<Pointer>(iter.ptr);
    	if (p_end != p_cap) {
            shiftRearward(ptr);
            Construct(ptr, v);
            return ptr;
        }
        else {
            auto diff = iter.ptr - p_begin;
            reallocAndShiftRearward(getNewCapacity(size()), iter);
            auto pos = p_begin+diff;
            Construct(pos, v);
            return pos;
        }
    }

    template<class Type>
    void DynamicArray<Type>::remove(SizeType i) {
        if (i >= size()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        auto pos = p_begin+i;
        Destruct(pos);
        shiftForward(pos+1);
    }

    template<class Type>
    auto DynamicArray<Type>::remove(ConstIterator iter) -> Iterator {
        auto ptr = const_cast<Pointer>(iter.ptr);
        [[unlikely]]
        if (iter >= end() or iter < begin()) {
            throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        }

        Destruct(ptr);
        shiftForward(ptr+1);

        return ptr;
    }

    template<class Type>
    auto DynamicArray<Type>::remove(ConstIterator first, ConstIterator last) -> Iterator {
        [[unlikely]]
        if (first>this->end() || first < this->begin()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        [[unlikely]]
    	if (last>this->end() || last < this->begin()) throw OutOfRange{ "[ntl.containers.dynamic_array] Index is out of range" };
        if (first >= last) return const_cast<Pointer>(first.ptr);
        auto n = last-first;
        for (auto it = first; it != last; ++it) {
            Destruct(const_cast<Pointer>(it.ptr));
        }
        //for (auto it = first; it != last; ++it) {
        //    Construct(it, *(it+n));
        //}
        auto ptr = const_cast<Pointer>(first.ptr);
        while(last != end())
        {
            Construct(const_cast<Pointer>(first.ptr), Move(*const_cast<Pointer>(last.ptr)));
            Destruct(const_cast<Pointer>(last.ptr));
            ++first;
            ++last;
        }

        p_end-=n;
        return ptr;
    }

    template<class Type>
    auto DynamicArray<Type>::begin() noexcept -> Iterator {
        return p_begin;
    }

    template<class Type>
    auto DynamicArray<Type>::end() noexcept -> Iterator {
        return p_end;
    }

    template<class Type>
    auto DynamicArray<Type>::begin() const noexcept -> ConstIterator {
        return p_begin;
    }

    template<class Type>
    auto DynamicArray<Type>::end() const noexcept -> ConstIterator {
        return p_end;
    }

    template<class Type>
    auto DynamicArray<Type>::cbegin() const noexcept -> ConstIterator {
        return begin();
    }

    template<class Type>
    auto DynamicArray<Type>::cend() const noexcept -> ConstIterator {
        return end();
    }

    template<class Type>
    auto DynamicArray<Type>::rbegin() noexcept -> ReverseIterator {
        return ReverseIter (p_end);
    }

    template<class Type>
    auto DynamicArray<Type>::rend() noexcept -> ReverseIterator {
        return ReverseIter (p_begin);
    }

    template<class Type>
    auto DynamicArray<Type>::rbegin() const noexcept -> ConstReverseIterator {
        return ConstReverseIter (p_end);
    }

    template<class Type>
    auto DynamicArray<Type>::rend() const noexcept -> ConstReverseIterator {
        return ConstReverseIter (p_begin);
    }

    template<class Type>
    auto DynamicArray<Type>::crbegin() const noexcept -> ConstReverseIterator {
        return rbegin();
    }

    template<class Type>
    auto DynamicArray<Type>::crend() const noexcept -> ConstReverseIterator {
        return rend();
    }

    template<class Type>
    auto DynamicArray<Type>::getAddress() noexcept -> Pointer {
        return p_begin;
    }

    template<class Type>
    auto DynamicArray<Type>::getAddress() const noexcept -> ConstPointer {
        return p_begin;
    }
}