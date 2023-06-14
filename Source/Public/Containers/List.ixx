export module ntl.containers.list;

import ntl.iterator;
import ntl.utils;
import ntl.memory.allocator;
import ntl.concepts;

export namespace ne
{
    template<class ListType>
    struct ListNodeBase;
    template<class ListType>
    struct ListNode;
    template<class ListType>
    class ListIterator;
    template<class ListType>
    class ListConstIter;

    template<class Type>
    class List
    {
    public:
        using ThisType = List<Type>;
        using ValueType = Type;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using SizeType = int64;
        using DifferenceType = ptrdiff_t;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Iterator = ListIterator<ThisType>;
        using ConstIterator = ListConstIter<ThisType>;
        using ReverseIterator = ne::ReverseIterator<Iterator>;
        using ConstReverseIterator = ne::ReverseIterator<ConstIterator>;

        List();
        List(const List& list);
        List(List&& list) noexcept;
        template<ConceptInputIterator It>
        List(It first, It last);
        List(SizeType n, const ValueType& val);
        List(InitList<ValueType> il);

        ~List();

        auto operator=(const ThisType& list) -> ThisType&;
        auto operator=(ThisType&& list) noexcept -> ThisType&;
        auto operator=(InitList<Type> il) -> ThisType&;

        void assign(SizeType n, const ValueType& item);
        template<ConceptInputIterator It>
        void assign(It first, It last);
        void assign(InitList<ValueType> il);

        auto front()-> Reference;
        auto front() const -> ConstReference;
        auto back() -> Reference;
        auto back() const -> ConstReference;

        auto begin() noexcept -> Iterator;
        auto end() noexcept -> Iterator;
        auto begin() const noexcept -> ConstIterator ;
        auto end() const noexcept -> ConstIterator ;
        auto cbegin() const noexcept -> ConstIterator ;
        auto cend() const noexcept -> ConstIterator ;
        auto rbegin() noexcept -> ReverseIterator ;
        auto rend() noexcept -> ReverseIterator ;
        auto rbegin() const noexcept -> ConstReverseIterator ;
        auto rend() const noexcept -> ConstReverseIterator ;
        auto crbegin() const noexcept -> ConstReverseIterator ;
        auto crend() const noexcept -> ConstReverseIterator ;

        [[nodiscard]]
        auto size() const noexcept -> SizeType ;
        [[nodiscard]]
        bool isEmpty() const noexcept;

        void clear();

        template<class ...Args>
            requires ConceptConstructibleFrom<Type, Args...>
        auto emplace(ConstIterator pos, Args&&...args) -> Iterator;
        template<class ...Args>
            requires ConceptConstructibleFrom<Type, Args...>
        auto emplaceFront(Args&&...args) -> ThisType&;
        template<class ...Args>
            requires ConceptConstructibleFrom<Type, Args...>
        auto emplaceBack(Args&&...args) -> ThisType&;

        auto insert(ConstIterator pos, const ValueType& item) -> Iterator;
        auto insert(ConstIterator pos, ValueType&& item) -> Iterator;
        auto insert(ConstIterator pos, SizeType n, const ValueType& item) -> Iterator;
        template<ConceptInputIterator It>
        auto insert(ConstIterator pos, It first, It last) -> Iterator;
        auto insert(ConstIterator pos, InitList<ValueType> initList) -> Iterator;

        auto remove(ConstIterator pos) -> Iterator;
        auto remove(ConstIterator first, ConstIterator last) -> Iterator;

        auto prepend(const ValueType& item) -> ThisType&;
        auto prepend(ValueType&& item) -> ThisType&;
        auto append(const ValueType& item) -> ThisType&;
        auto append(ValueType&& item) -> ThisType&;

        void popBack();
        void popFront();

        void reverse();
//        void sort();

    private:
//    public:
        using Node = ListNode<ThisType>;
        using BaseNode = ListNodeBase<ThisType>;
        BaseNode sentinel;
        SizeType sz;
        Allocator allocator;

        BaseNode* toBasePtr(Node* ptr) const {
            return static_cast<BaseNode*>(ptr);
        }
        Node* toNodePtr(BaseNode* ptr) const {
            return static_cast<Node*>(ptr);
        }
        Node* allocateNode() {
            return allocator.template allocate<Node>();
        }
        template<class ...Args>
        Node* createNode(Args&&...args) {
            auto ptr = allocateNode();
            Construct(ptr, typename Node::Placeholder(), Forward<Args>(args)...);
            return ptr;
        }
        void deleteNode(Node* ptr) {
            ptr->~Node();
            allocator.template deallocate(ptr);
        }

        const BaseNode* getSentinelPtr() const {
            return &this->sentinel;
        }

        BaseNode* getSentinelPtr() {
            return &this->sentinel;
        }

        void linkNodeAtFront(BaseNode* node, Node* target) {
            node->prev->next = target;
            auto tmp_ptr = node->prev;
            node->prev = target;
            target->next = node;
            target->prev = tmp_ptr;
        }

        void linkNodeAtBack(BaseNode* node, Node* target) {
            node->next->prev = target;
            auto tmp_ptr = node->next;
            node->next = target;
            target->prev = node;
            target->next = tmp_ptr;
        }

        void initSentinel() {
            sentinel.prev = getSentinelPtr();
            sentinel.next = getSentinelPtr();
        }
    };

    template<ConceptInputIterator It>
    List(It, It) -> List<typename IteratorTraits<It>::ValueType>;
}

namespace ne
{
    template<class ListType>
    struct ListNodeBase
    {
        using ThisType = ListNodeBase<ListType>;
        using ValueType = typename ListType::ValueType;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;

        ThisType* prev;
        ThisType* next;

        ListNodeBase()
            : prev (nullptr)
            , next (nullptr)
        {}
        ListNodeBase(ThisType* prev, ThisType* next)
            : prev (prev)
            , next (next)
        {}
    };

    template<class ListType>
    struct ListNode: public ListNodeBase<ListType>
    {
        using BaseType = ListNodeBase<ListType>;
        using ThisType = ListNode<ListType>;
        using ValueType = typename ListType::ValueType;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;

        struct Placeholder{};

        ValueType item;

        ListNode()
            : BaseType ()
            , item ()
        {}

        explicit
        ListNode(ValueType&& item)
            : BaseType()
            , item(item)
        {}

        explicit
        ListNode(const ValueType& item)
                : BaseType()
                , item(item)
        {}

        template<class...Args>
        explicit ListNode(Placeholder, Args&&...args)
            : BaseType()
            , item(Forward<Args>(args)...)
        {}


        ListNode(BaseType* prev, BaseType *next, ValueType&& item)
                : BaseType(prev, next)
                , item(item)
        {}

        ListNode(BaseType* prev, BaseType *next, const ValueType& item)
                : BaseType(prev, next)
                , item(item)
        {}

        template<class...Args>
        ListNode(BaseType* prev, BaseType *next, Placeholder, Args&&...args)
                : BaseType(prev, next)
                , item(Forward<Args>(args)...)
        {}
    };

    template<class ListType>
    class ListIteratorBase {
    protected:
        using IteratorCategory = BidirectionalIteratorCategory;
        using ValueType = typename ListType::ValueType;
        using DifferenceType = typename ListType::DifferenceType;
        using Pointer = typename ListType::Pointer;
        using Reference = typename ListType::Reference;

        using BaseNodePtr = ListNodeBase<ListType>*;
        using NodePtr = ListNode<ListType>*;
        using ConstBaseNodePtr = const ListNodeBase<ListType>*;
        using ConstNodePtr = const ListNode<ListType>*;

        static BaseNodePtr ToBasePtr(NodePtr ptr) {
            return static_cast<BaseNodePtr>(ptr);
        }

        static NodePtr ToNodePtr(BaseNodePtr ptr) {
            return static_cast<NodePtr>(ptr);
        }

        static ConstBaseNodePtr ToBasePtr(ConstNodePtr ptr) {
            return static_cast<BaseNodePtr>(ptr);
        }

        static ConstNodePtr ToNodePtr(ConstBaseNodePtr ptr) {
            return static_cast<NodePtr>(ptr);
        }
    };

    template<class ListType>
    class ListIterator: public ListIteratorBase<ListType>
    {
    public:
        using BaseType = ListIteratorBase<ListType>;
        using ThisType = ListIterator<ListType>;
        using IteratorCategory = BidirectionalIteratorCategory;
        using ValueType = typename ListType::ValueType;
        using DifferenceType = typename ListType::DifferenceType;
        using Pointer = typename ListType::Pointer;
        using Reference = typename ListType::Reference;
        using ConstReference = typename ListType::ConstReference;

        template<class >
        friend class ListConstIter;

        template<class Type>
        
        friend
        class List;
    private:
        typename BaseType ::BaseNodePtr ptr;
    public:

        ListIterator(): ptr(nullptr) {}

        ListIterator(typename BaseType::BaseNodePtr ptr) noexcept {
            this->ptr = ptr;
        }

        ListIterator(const ListIterator&) = default;
        ListIterator(ListIterator&&) noexcept = default;
        ThisType& operator=(const ListIterator& it) = default;
        ThisType& operator=(ListIterator&&) noexcept = default;

        Reference operator*() const noexcept {
            return (BaseType::ToNodePtr(ptr))->item;
        }

        Pointer operator->() const noexcept {
            return GetAddress((BaseType ::ToNodePtr(ptr))->item);
        }

        ThisType& operator++() noexcept {
            ptr = ptr->next;
            return *this;
        }

        ThisType operator++(int) noexcept {
            auto it = *this;
            ++(*this);
            return it;
        }

        ThisType& operator--() noexcept {
            ptr = ptr->prev;
            return *this;
        }

        ThisType operator--(int) noexcept {
            auto it = *this;
            --*this;
            return it;
        }

        [[nodiscard]]
        bool operator==(const ThisType& rhs) const noexcept {
            return this->ptr == rhs.ptr;
        }
        [[nodiscard]]
        bool operator!=(const ThisType& rhs) const noexcept {
            return this->ptr != rhs.ptr;
        }
    };

    template<class ListType>
    class ListConstIter: public ListIteratorBase<ListType> {
    public:
        using BaseType = ListIteratorBase<ListType>;
        using ThisType = ListConstIter<ListType>;
        using IteratorCategory = BidirectionalIteratorCategory;
        using ValueType = typename ListType::ValueType;
        using DifferenceType = typename ListType::DifferenceType;
        using Pointer = typename ListType::ConstPointer;
        using Reference = typename ListType::ConstReference;

        template<class Type>
        friend
        class ListIterator;

        template<class Type>
            
        friend
        class List;

    private:
        typename BaseType::BaseNodePtr ptr;
    public:

        ListConstIter(): ptr(nullptr) {}
        ListConstIter(const ListConstIter&) = default;
        ListConstIter(ListConstIter&&) noexcept = default;
        ThisType& operator=(const ListConstIter&) = default;
        ThisType& operator=(ListConstIter&&) noexcept = default;

        ListConstIter(typename BaseType::BaseNodePtr ptr) noexcept {
            this->ptr = ptr;
        }

        ListConstIter(const ListIterator<ListType>& it) noexcept {
            ptr = it.ptr;
        }
        Reference operator*() const noexcept {
            return (BaseType::ToNodePtr(ptr))->item;
        }

        Pointer operator->() const noexcept {
            return &(BaseType::ToNodePtr(ptr))->item;
        }

        ThisType &operator++() noexcept {
            ptr = ptr->next;
            return *this;
        }

        ThisType operator++(int) noexcept {
            auto it = *this;
            ++(*this);
            return it;
        }

        ThisType &operator--() noexcept {
            ptr = ptr->prev;
            return *this;
        }

        ThisType operator--(int) noexcept {
            auto it = *this;
            --*this;
            return it;
        }

        [[nodiscard]]
        bool operator==(const ThisType &rhs) const noexcept {
            return this->ptr == rhs.ptr;
        }

        [[nodiscard]]
        bool operator!=(const ThisType &rhs) const noexcept {
            return this->ptr != rhs.ptr;
        }
    };


    template<class Type>
    List<Type>::List()
        : allocator()
    {
        initSentinel();
        sz = 0;
    }

    template<class Type>
    List<Type>::List(const List &l) {
        this->allocator = l.allocator;
        initSentinel();
        for (auto it = l.begin(); it!=l.end(); ++it) {
            this->append(*it);
        }
    }

    template<class Type>
    List<Type>::List(List &&l) noexcept {
        this->allocator = l.allocator;
        this->sz = l.sz;
        if (sz) {
            this->sentinel.next = l.sentinel.next;
            this->sentinel.prev = l.sentinel.prev;
            this->sentinel.next->prev = this->getSentinelPtr();
            this->sentinel.prev->next = this->getSentinelPtr();
        }
        l.sz = 0;
        l.initSentinel();
    }

    template<class Type>
    template<ConceptInputIterator It>
    List<Type>::List(It first, It last)
        : allocator()
    {
        sz = 0;
        initSentinel();
        for (; first!=last; ++first) {
            this->append(*first);
        }
    }

    template<class Type>
    List<Type>::List(List::SizeType n, const ValueType &val)
        : allocator()
    {
        sz=0;
        initSentinel();
        for (;n>0;n--) this->append(val);
    }

    template<class Type>
    List<Type>::List(InitList<ValueType> il)
        : allocator()
    {
        initSentinel();
        sz = 0;
        for (auto it=il.begin(); it!=il.end(); ++it) {
            this->append(*it);
        }
    }

    template<class Type>
    List<Type>::~List() {
        clear();
    }

    template<class Type>
    auto List<Type>::operator=(const List::ThisType &l) -> List::ThisType & {
        assign(l.begin(), l.end());
        return *this;
    }

    template<class Type>
    auto List<Type>::operator=(List::ThisType &&l) noexcept -> List::ThisType & {
        if (allocator!=l.allocator) {
            this->allocator=l.allocator;
            using It = MoveIterator<Iterator>;
            assign(It(l.begin()), It(l.end()));
        }
        else {
            clear();
            this->sz = l.sz;
            if (l.size() != 0) {
                this->sentinel.prev = l.sentinel.prev;
                this->sentinel.next = l.sentinel.next;
                this->sentinel.prev->next = getSentinelPtr();
                this->sentinel.next->prev = getSentinelPtr();
            }
        }
        l.sz=0;
        l.initSentinel();
        return *this;
    }

    template<class Type>
    auto List<Type>::operator=(InitList<Type> il) -> List::ThisType & {
        assign(il.begin(), il.end());
        return *this;
    }

    template<class Type>
    void List<Type>::assign(List::SizeType n, const ValueType &item) {
        clear();
        for (;n>0;--n) append(item);
    }

    template<class Type>
    template<ConceptInputIterator It>
    void List<Type>::assign(It first, It last) {
        clear();
        for(;first!=last;++first) {
            append(*first);
        }
    }

    template<class Type>
    void List<Type>::assign(InitList<ValueType> il) {
        clear();
        for (auto it=il.begin();it!=il.end();++it) {
            append(Move(il));
        }
    }

    template<class Type>
    auto List<Type>::front() -> Reference {
        //if (sentinel.next == sentinel.prev) throw "List is empty";
        return static_cast<Node*>(sentinel.next)->item;
    }

    template<class Type>
    auto List<Type>::front() const -> ConstReference {
        //if (sentinel.next == sentinel.prev) throw "List is empty";
        return static_cast<Node*>(sentinel.next)->item;
    }

    template<class Type>
    auto List<Type>::back() -> Reference {
        //if (sentinel.next == sentinel.prev) throw "List is empty";
        return static_cast<Node*>(sentinel.prev)->item;
    }

    template<class Type>
    auto List<Type>::back() const -> ConstReference {
        //if (sentinel.next == sentinel.prev) throw "List is empty";
        return static_cast<Node*>(sentinel.prev)->item;
    }

    template<class Type>
    auto List<Type>::begin() noexcept -> List::Iterator {
        return Iterator(sentinel.next);
    }

    template<class Type>
    auto List<Type>::end() noexcept -> List::Iterator {
        return Iterator(getSentinelPtr());
    }

    template<class Type>
    auto List<Type>::begin() const noexcept -> List::ConstIterator {
        return ConstIterator(sentinel.next);
    }

    template<class Type>
    auto List<Type>::end() const noexcept -> List::ConstIterator {
        return ConstIterator(const_cast<BaseNode*>(getSentinelPtr()));
    }

    template<class Type>
    auto List<Type>::cbegin() const noexcept -> List::ConstIterator {
        return begin();
    }

    template<class Type>
    auto List<Type>::cend() const noexcept -> List::ConstIterator {
        return end();
    }

    template<class Type>
    auto List<Type>::rbegin() noexcept -> List::ReverseIterator {
        return ReverseIter(end());
    }

    template<class Type>
    auto List<Type>::rend() noexcept -> List::ReverseIterator {
        return ReverseIter(begin());
    }

    template<class Type>
    auto List<Type>::rbegin() const noexcept -> List::ConstReverseIterator {
        return ConstReverseIter(end());
    }

    template<class Type>
    auto List<Type>::rend() const noexcept -> List::ConstReverseIterator {
        return ConstReverseIter(begin());
    }

    template<class Type>
        
    auto List<Type>::crbegin() const noexcept -> List::ConstReverseIterator {
        return rbegin();
    }

    template<class Type>
    auto List<Type>::crend() const noexcept -> List::ConstReverseIterator {
        return rend();
    }

    template<class Type>
    auto List<Type>::size() const noexcept -> List::SizeType {
        return this->sz;
    }

    template<class Type>
    bool List<Type>::isEmpty() const noexcept {
        return sz==0;
    }

    template<class Type>
    void List<Type>::clear() {
        auto ptr = sentinel.next;
        while(ptr!=getSentinelPtr()) {
            auto tmp = ptr;
            ptr = ptr->next;
            deleteNode(toNodePtr(tmp));
        }
        sz = 0;
        initSentinel();
    }

    template<class Type>
        template<class... Args>
        requires ConceptConstructibleFrom<Type, Args...>
    auto List<Type>::emplace(ConstIterator pos, Args&&... args) -> Iterator {
        auto ptr = pos.ptr;
        auto new_ptr = createNode(Forward<Args>(args)...);
        linkNodeAtFront(ptr, new_ptr);
        ++sz;
        return Iterator(new_ptr);
    }

    template<class Type>
    template<class... Args>
        requires ConceptConstructibleFrom<Type, Args...>
    auto List<Type>::emplaceFront(Args &&... args) -> ThisType& {
        emplace(begin(), Forward<Args>(args)... );
        return *this;
    }

    template<class Type>
    template<class... Args>
    requires ConceptConstructibleFrom<Type, Args...>
    auto List<Type>::emplaceBack(Args &&... args) -> ThisType& {
        emplace(end(), Forward<Args>(args)...);
        return *this;
    }

    template<class Type>
    auto List<Type>::insert(List::ConstIterator pos, const ValueType &item) -> List::Iterator {
        return emplace(pos, item);
    }

    template<class Type>
    auto List<Type>::insert(List::ConstIterator pos, ValueType &&item) -> List::Iterator {
        return emplace(pos, Move(item));
    }

    template<class Type>
    auto List<Type>::insert(List::ConstIterator pos, List::SizeType n, const ValueType &item) -> List::Iterator {
        auto iter = pos;
        for(;n>0;n--) {
            iter = insert(iter, item);
        }
        return iter;
    }

    template<class Type>    
    template<ConceptInputIterator It>
    auto List<Type>::insert(List::ConstIterator pos, It first, It last) -> List::Iterator {
        auto iter = Iterator(pos.ptr);
        for(; first !=last; ++first) {
            iter = insert(iter, (*first));
        }
        return iter;
    }

    template<class Type>
    auto List<Type>::insert(List::ConstIterator pos, InitList<ValueType> il) -> List::Iterator {
        return insert(pos, il.begin(), il.end());
    }

    template<class Type>
    auto List<Type>::remove(List::ConstIterator pos) -> List::Iterator {
        if (pos == end()) throw "Cannot remove end";
        auto ptr = pos.ptr;
        auto next_ptr = ptr->next;
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        deleteNode(toNodePtr(ptr));
        sz--;
        return Iterator(next_ptr);
    }

    template<class Type>
    auto List<Type>::remove(List::ConstIterator first, List::ConstIterator last) -> List::Iterator {
        auto first_ptr = first.ptr;
        auto last_ptr = last.ptr;
        first_ptr->prev->next = last_ptr;
        last_ptr->prev = first_ptr->prev;
        while(first_ptr != last_ptr) {
            auto ptr = first_ptr;
            first_ptr = first_ptr->next;
            deleteNode(toNodePtr(first_ptr));
            --sz;
        }
        return Iterator(last_ptr);
    }

    template<class Type>
    auto List<Type>::prepend(const ValueType &item) -> List::ThisType & {
        insert(begin(), item);
        return *this;
    }

    template<class Type>
    auto List<Type>::prepend(ValueType &&item) -> List::ThisType & {
        insert(begin(), Move(item));
        return *this;
    }

    template<class Type>
    auto List<Type>::append(const ValueType &item) -> List::ThisType & {
        insert(end(), item);
        return *this;
    }

    template<class Type>
    auto List<Type>::append(ValueType &&item) -> List::ThisType & {
        insert(end(), Move(item));
        return *this;
    }

    template<class Type>
    void List<Type>::popBack() {
        remove(--end());
    }

    template<class Type>
    void List<Type>::popFront() {
        remove(begin());
    }

    template<class Type>
    void List<Type>::reverse() {
        ThisType tmp_list{};
        auto bp = begin().ptr;
        auto ep = end().ptr;
        while (bp!=ep) {
            auto tmp = bp;
            bp = bp->next;
            tmp_list.linkNodeAtBack(tmp_list.getSentinelPtr(), toNodePtr(tmp));
        }
        tmp_list.sz = sz;
        initSentinel();
        sz=0;
        (*this)= Move(tmp_list);
    }


}

