export module ntl.containers.red_black_tree;

import ntl.containers.key_value;
import ntl.utils;
import ntl.iterator;
import ntl.memory.allocator;
import ntl.type_traits;

export namespace ne
{
    enum class RBTreeColor: uint8 { RED, BLACK };
    /**
     * Sentinel Node, left is root, parent is begin, right is to be determined
     */
    struct RBTreeBaseNode;
    struct RBTreeSentinelNode
    {
        RBTreeBaseNode* left;
        RBTreeBaseNode* right;
        RBTreeBaseNode* parent;
    };
    struct RBTreeBaseNode: public RBTreeSentinelNode
    {
        RBTreeColor color;
    };
    template<class TheKVType, template<class> typename TheExtractorType, class TheCompareType, bool MULTI_KEY>
    requires ConceptKVExtractor<TheExtractorType, TheKVType>
    class RedBlackTree;

    template<class>
    class RBTreeConstIter;
    template<class RBTreeType>
    struct RBTreeNode: RBTreeBaseNode
    {
        using ValueType = typename RBTreeType::ValueType;
        using KeyType = typename RBTreeType::KeyType;
        using MappedType = typename RBTreeType::MappedType;
        using ThisType = RBTreeNode<RBTreeType>;
        using BaseType = RBTreeBaseNode;

        ValueType value;
        RBTreeNode(const RBTreeNode&) = delete;
    };

    template<class RBTreeType>
    class RBTreeIterator
    {
    public:
        using ThisType = RBTreeIterator<RBTreeType>;
        using SizeType = typename RBTreeType::SizeType;
        using DifferenceType = typename RBTreeType::DifferenceType;
        using ValueType = typename RBTreeType::ValueType;
        using Pointer = typename RBTreeType::Pointer;
        using Reference = typename RBTreeType::Reference;
        using NodeType = RBTreeNode<RBTreeType>;
        using IteratorCategory = ForwardIteratorCategory;

        RBTreeIterator() = default;

        template<class TheKVType, template<class> typename TheExtractorType, class TheCompareType, bool MULTI_KEY>
        requires ConceptKVExtractor<TheExtractorType, TheKVType>
        friend class RedBlackTree;

        template<class>
        friend class RBTreeConstIter;

        explicit
        RBTreeIterator(NodeType* ptr)
                : ptr(ptr)
        {}
        RBTreeIterator(const RBTreeIterator& it) {
            ptr = it.ptr;
        }
        RBTreeIterator(RBTreeIterator&& it) noexcept {
            ptr = it.ptr;
        }
        ~RBTreeIterator() = default;
        ThisType& operator=(const RBTreeIterator& it) {
            ptr = it.ptr;
            return *this;
        }
        ThisType& operator=(RBTreeIterator&& it) noexcept {
            ptr = it.ptr;
            return *this;
        }

        Pointer operator->() const noexcept { return &(ptr->value); }
        Reference operator*() const noexcept { return ptr->value; }

        bool operator==(const ThisType& x) const noexcept {
            return ptr == x.ptr;
        }
        bool operator!=(const ThisType& x) const noexcept {
            return ptr != x.ptr;
        }

        ThisType& operator++() {
            ptr = static_cast<NodeType *>(RBTreeType::template IncNode(ptr));
            return *this;
        }
        ThisType operator++(int) {
            auto it = (*this);
            ptr = static_cast<NodeType *>(RBTreeType::template IncNode(ptr));
            return it;
        }
//        ThisType& operator--() {
//            ptr = static_cast<NodeType *>(RBTreeType::template DecNode(ptr));
//            return *this;
//        }
//        ThisType operator--(int) {
//            auto it = (*this);
//            ptr = static_cast<NodeType *>(RBTreeType::template DecNode(ptr));
//            return it;
//        }

    private:
        NodeType *ptr;
    };

    template<typename RBTreeType>
    class RBTreeConstIter
    {
    public:
        using ThisType = RBTreeConstIter<RBTreeType>;
        using SizeType = typename RBTreeType::SizeType;
        using DifferenceType = typename RBTreeType::DifferenceType;
        using ValueType = typename RBTreeType::ValueType;
        using Pointer = typename RBTreeType::ConstPointer;
        using Reference = typename RBTreeType::ConstReference;
        using NodeType = RBTreeNode<RBTreeType>;
        using IteratorCategory = ForwardIteratorCategory;

        RBTreeConstIter() = default;

        template<class TheKVType, template<class> typename TheExtractorType, class TheCompareType, bool MULTI_KEY>
        requires ConceptKVExtractor<TheExtractorType, TheKVType>
        friend class RedBlackTree;

        explicit
        RBTreeConstIter(NodeType* ptr)
                : ptr(ptr)
        {}
        RBTreeConstIter(const RBTreeIterator<RBTreeType>& it) noexcept {
            ptr = it.ptr;
        }
        RBTreeConstIter(const RBTreeConstIter& it) {
            ptr = it.ptr;
        }
        RBTreeConstIter(RBTreeConstIter&& it) noexcept {
            ptr = it.ptr;
        }
        ~RBTreeConstIter() = default;
        ThisType& operator=(const RBTreeConstIter& it) {
            ptr = it.ptr;
            return *this;
        }
        ThisType& operator=(RBTreeConstIter&& it) noexcept {
            ptr = it.ptr;
            return *this;
        }

        bool operator==(const ThisType& x) const noexcept {
            return ptr == x.ptr;
        }
        bool operator!=(const ThisType& x) const noexcept {
            return ptr != x.ptr;
        }

        Pointer operator->() const noexcept { return &(ptr->value); }
        Reference operator*() const noexcept { return ptr->value; }

        ThisType& operator++() {
            ptr = static_cast<NodeType *>(RBTreeType::template IncNode(ptr));
            return *this;
        }
        ThisType operator++(int) {
            auto it = (*this);
            ptr = static_cast<NodeType *>(RBTreeType::template IncNode(ptr));
            return it;
        }
//        ThisType& operator--() {
//            ptr = static_cast<NodeType *>(RBTreeType::template DecNode(ptr));
//            return *this;
//        }
//        ThisType operator--(int) {
//            auto it = (*this);
//            ptr = static_cast<NodeType *>(RBTreeType::template DecNode(ptr));
//            return it;
//        }

    private:
        NodeType *ptr;
    };

    /**
     *
     * @tparam TheKVType
     * @tparam TheExtractorType
     * @tparam TheCompareType
     * @tparam MULTI_KEY
     *
     *
     */
    template<class TheKVType, template<class> typename TheExtractorType, class TheCompareType, bool MULTI_KEY>
        requires ConceptKVExtractor<TheExtractorType, TheKVType>
    class RedBlackTree
    {
    public:
        using ThisType = RedBlackTree<TheKVType, TheExtractorType, TheCompareType, MULTI_KEY>;
        using Extractor = TheExtractorType<TheKVType>;
        using KeyType = typename Extractor::KeyType;
        using MappedType = typename Extractor::ValueType;
        using ValueType = TheKVType;
        using SizeType = int64;
        using DifferenceType = ptrdiff_t;
        using KeyCompare = TheCompareType;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using Iterator = RBTreeIterator<ThisType>;
        using ConstIterator = RBTreeConstIter<ThisType>;
        // using ReverseIter = Iters::ReverseWrapper<Iterator>;
        // using ConstReverseIter = Iters::ReverseWrapper<ConstIter>;

    private:
        struct InsertionResultKeyValue {
            Iterator iterator;
            bool inserted;
        };
    public:
        using InsertionResult = TypeConditional<!MULTI_KEY, InsertionResultKeyValue, Iterator>;
    template<class T>
    friend class RBTreeIterator;
    template<class T>
    friend class RBTreeConstIter;

    protected:
        using Node = RBTreeNode<ThisType>;
        using BaseNode = RBTreeBaseNode;
        using SentinelNode = RBTreeSentinelNode;
       [[no_unique_address]]
        KeyCompare compare;
        Extractor extractor;
        Allocator allocator;
        SizeType sz;
        SentinelNode sentinel;

    protected:
        /**
            性质1. 结点是红色或黑色。
            性质2. 根结点是黑色。
            性质3. 所有叶子都是黑色。（叶子是NIL结点）
            性质4. 每个红色结点的两个子结点都是黑色。（从每个叶子到根的所有路径上不能有两个连续的红色结点）
            性质5. 从任一结点到其每个叶子的所有路径都包含相同数目的黑色结点。
         */

        template<class NodePtr>
        static bool IsLeftChild(NodePtr x) { return x == x->parent->left; }
        template<class NodePtr>
        static bool IsRightChild(NodePtr x) { return x == x->parent->right; }
        template<class NodePtr>
        static bool IsNodeRedColor(NodePtr x) { return x->color == RBTreeColor::RED; }
        template<class NodePtr>
        static bool IsNodeBlackColor(NodePtr x) { return x->color == RBTreeColor::BLACK; }

    public:
        // 检查子树是否是红黑树，不是返回0，是返回黑色节点数量
        template<class NodePtr>
        static uint32 CheckSubInvariant(NodePtr x) {
            if (x == nullptr) return 1;

            // 一致性检查
            if (x->left != nullptr && x->left->parent != x) {
                return 0;
            }
            if (x->right != nullptr && x->right->parent != x) {
                return 0;
            }
            if (x->left == x->right && x->left == nullptr) {
                return 0;
            }

            // 性质四检查
            if (IsNodeRedColor(x)) {
                if (x->left != nullptr && IsNodeRedColor(x->left)) {
                    return 0;
                }
                if (x->right != nullptr && IsNodeRedColor(x->right)) {
                    return 0;
                }
            }

            uint32 h = CheckSubInvariant(x->left);
            if (h==0) return 0;

            if (h!=CheckSubInvariant(x->right)) return 0;

            if (IsNodeBlackColor(x)) {
                return h+1;
            }
            else {
                return h;
            }
        }


        template<class NodePtr>
        static bool IsValidRBTree(NodePtr root) {
            if (root == nullptr) return true;
            if (root->parent == nullptr) return false;
            if (!IsLeftChild(root)) return false;
            if (IsNodeRedColor(root)) return true;
            return CheckSubInvariant(root) !=0;
        }

        template<class NodePtr>
        static NodePtr GetMinNode(NodePtr x) {
            // ASSERT((x != nullptr));
            while (x->left != nullptr) {
                x=static_cast<NodePtr>(x->left);
            }
            return x;
        }
        template<class NodePtr>
        static NodePtr GetMaxNode(NodePtr x) {
            // ASSERT((x != nullptr));
            while (x->right != nullptr) {
                x=static_cast<NodePtr>(x->right);
            }
            return x;
        }

    protected:
        template<class NodePtr>
        static NodePtr IncNode(NodePtr x) {
            if (x->right != nullptr) return GetMinNode(static_cast<NodePtr>(x->right));
            while(!IsLeftChild(x)) {
                x = static_cast<NodePtr>(x->parent);
            }
            return static_cast<NodePtr>(x->parent);
        }
        template<class NodePtr>
        static NodePtr DecNode(NodePtr x) {
            // when x is initial sentinel, x->parent is sentinel, but sentinel is not its left child, so it return sentinel
            if (x->left != nullptr) return GetMaxNode(static_cast<NodePtr>(x->left));
            while(IsLeftChild(x)) {
                x = static_cast<NodePtr>(x->parent);
            }
            return static_cast<NodePtr>(x->parent);
        }

        template<class NodePtr>
        static NodePtr GetLeaf(NodePtr x) {
            while (true) {
                if (x->left != nullptr) {
                    x = x->left;
                    continue;
                }
                if (x->right != nullptr) {
                    x = x->right;
                    continue;
                }
                break;
            }
            return x;
        }

        template<class NodePtr, class ParentPtr>
        static void SetParent(NodePtr x, ParentPtr parent) { x->parent = static_cast<decltype(x->parent)>(parent); }
        template<class NodePtr, class LeftPtr>
        static void SetLeft(NodePtr* x, LeftPtr left) { x->left = static_cast<decltype(x->left)>(left); }
        template<class NodePtr, class RightPtr>
        static void SetRight(NodePtr x, RightPtr right) { x->right = static_cast<decltype(x->right)>(right); }

        template<class NodePtr>
        static void LeftRotate(NodePtr x) {
            NodePtr y = static_cast<decltype(y)>(x->right);
            SetRight(x, y->left);
            if (x->right != nullptr) {
                SetParent(x->right, x);
            }
            SetParent(y, x->parent);
            if (IsLeftChild(x)) {
                SetLeft(x->parent, y);
            }
            else {
                SetRight(x->parent, y);
            }
            SetLeft(y, x);
            SetParent(x, y);
        }
        template<class NodePtr>
        static void RightRotate(NodePtr x) {
            NodePtr y = static_cast<decltype(y)>(x->left);
            SetLeft(x, y->right);
            if (x->left != nullptr) {
                SetParent(x->left, x);
            }
            SetParent(y, x->parent);
            if (IsLeftChild(x)) {
                SetLeft(x->parent, y);
            } else {
                SetRight(x->parent, y);
            }
            SetRight(y, x);
            SetParent(x, y);
        }

        template<class NodePtr>
        static void TurnBlack(NodePtr x) { x->color = RBTreeColor::BLACK; }
        template<class NodePtr>
        static void TurnRed(NodePtr x) { x->color = RBTreeColor ::RED; }

        template<class NodePtr>
        static bool IsRed(NodePtr x) { return x->color == RBTreeColor ::RED; }
        template <class NodePtr>
        static bool IsBlack(NodePtr x) { return x->color == RBTreeColor ::BLACK; }

        template<class NodePtr>
        static void BalanceAfterInsertion(NodePtr root, NodePtr x) {
            if (x == root) {
                TurnBlack(x);
            }
            // Balance
            /**
             * 1. 空树
             *    直接插入
             * 2. 父节点为黑色
             *     不影响平衡，直接插入
             * 3. 父节点为红色
             *     3.1 叔叔节点为红色
             *         叔叔节点和父节点改为黑色，爷爷节点改为红色，继续递归处理
             *     3.2 叔叔结点不存在或为黑结点,并且插入结点的父亲结点是祖父结点的左子结点
             *         3.2.1 新插入结点,为其父结点的左子结点
             *               父亲设为黑色，爷爷设为红色， 右旋
             *         3.2.2 新插入结点,为其父结点的右子结点
             *              对父亲节点左旋，然后按3.2.1处理
             *     3.3 叔叔结点不存在或为黑结点,并且插入结点的父亲结点是祖父结点的右子结点
             *         同上
             */
            while (x != root && IsRed(x->parent)) {
                if (IsLeftChild(x->parent)) {
                    auto uncle = x->parent->parent->right;
                    if (uncle != nullptr && IsRed(uncle)) {
                        x = x->parent;
                        TurnBlack(x);
                        x = x->parent;
                        TurnRed(x);
                        TurnBlack(uncle);
                    } else {
                        if (IsRightChild(x)) {
                            x = x->parent;
                            LeftRotate(x);
                        }
                        x = x->parent;
                        TurnBlack(x);
                        x = x->parent;
                        TurnRed(x);
                        RightRotate(x);
                        break;
                    }

                }
                else {
                    auto uncle = x->parent->parent->left;
                    if (uncle != nullptr && IsRed(uncle)) {
                        x = x->parent;
                        TurnBlack(x);
                        x = x->parent;
                        TurnRed(x);
                        TurnBlack(uncle);
                    }
                    else {
                        if (IsLeftChild(x)) {
                            x = x->parent;
                            RightRotate(x);
                        }
                        x = x->parent;
                        TurnBlack(x);
                        x = x->parent;
                        TurnRed(x);
                        LeftRotate(x);
                        break;
                    }
                }
            }
            TurnBlack(root);
        }

        template<class NodePtr>
        static BaseNode* ToBasePtr(NodePtr x) { return static_cast<BaseNode*>(x); }
        template<class NodePtr>
        static Node* ToNodePtr(NodePtr x) { return static_cast<Node*>(x); }

        template<class NodePtr, class ChildPtr = BaseNode*>
        ChildPtr& findInsertionPlace(NodePtr& parent, const KeyType & k)
            requires Extractor::HAS_VALUE and (not MULTI_KEY)
        {
            BaseNode* nd = getRootPtr();
            BaseNode** ndpp = &sentinel.left;

            if (nd != nullptr) {
                while(true) {
                    if (compare(k, extractor.getKey(ToNodePtr(nd)->value))) {
                        if (nd->left != nullptr) {
                            ndpp = GetAddress(nd->left);
                            nd = nd->left;
                        }
                        else {
                            parent = static_cast<NodePtr>(nd);
                            return (parent->left);
                        }
                    }
                    else if (compare(extractor.getKey(ToNodePtr(nd)->value), k)) {
                        if (nd->right != nullptr) {
                            ndpp = GetAddress(nd->right);
                            nd = nd->right;
                        }
                        else {
                            parent = static_cast<NodePtr>(nd);
                            return ( parent->right);
                        }
                    }
                    else {
                        parent = nd;
                        return (*ndpp);
                    }
                }
            }

            parent = static_cast<NodePtr>(getSentinelPtr());
            return parent->left;
        }

        template<class NodePtr>
        BaseNode*& findInsertionPlace(NodePtr& parent, const KeyType& k, Node*& hint) {
            if (hint == getSentinelPtr() || compare(k, extractor.getKey(hint->value))) {
                Node* prior = hint;
                prior = DecNode(prior);

                if (hint == getBeginPtr() || compare(extractor.getKey(prior->value), k)) {
                    if (hint->left == nullptr) {
                        parent = static_cast<NodePtr>(hint);
                        return parent->left;
                    }
                    else {
                        parent = static_cast<NodePtr>(prior);
                        return prior->right;
                    }
                }
                return findInsertionPlace(parent, k);
            }
            else if (compare(extractor.getKey(hint->value), k)) {
                Node* next = hint;
                next = IncNode(next);
                if (hint->right== nullptr) {
                    parent = static_cast<NodePtr>(hint);
                    return hint->right;
                }
                else {
                    parent = static_cast<NodePtr>(next);
                    return parent->left;
                }
                return findInsertionPlace(parent, k);
            }
            parent = static_cast<NodePtr>(hint);
            BaseNode** x = reinterpret_cast<BaseNode**>(GetAddress(hint));
            return *x;
        }

        template<class NodePtr>
        void setBeginPtr(NodePtr x) {
            sentinel.parent = static_cast<BaseNode*>(x);
        }
        
        Node* findNode(const KeyType& key) const {
            Node* ptr = getRootPtr();
            while (true) {
                if (compare(key, extractor.getKey(ptr->value))) {
                    ptr = ToNodePtr(ptr->left);
                    if (ptr == nullptr) {
                        return getSentinelPtr();
                    }
                }
                else if (compare(extractor.getKey(ptr->value), key)) {
                    ptr = ToNodePtr(ptr->right);
                    if (ptr == nullptr) {
                        return (getSentinelPtr());
                    }
                }
                else {
                    return (ptr);
                }
            };
        }

        void insertNodeAt(BaseNode* parent, BaseNode*& child, BaseNode* new_node) {
            ++sz;
            TurnRed(new_node);
            new_node->left = nullptr;
            new_node->right = nullptr;
            new_node->parent = parent;
            child = new_node;
            if (getBeginPtr()->left != nullptr) {
                setBeginPtr(getBeginPtr()->left);
            }
            BalanceAfterInsertion(ToBasePtr(getRootPtr()), child);
        }

        // 从树中移除节点，但不释放内存，不进行析构
        template<class NodePtr>
        static void RemoveNode(NodePtr root, NodePtr z) {
            // y is to be deleted
            NodePtr y = (z->left == nullptr || z->right == nullptr) ? z : IncNode(z);

            NodePtr x = y->left != nullptr ? static_cast<decltype(x)>(y->left) : static_cast<decltype(x)>(y->right);
            // uncle
            NodePtr w = nullptr;

            if (x != nullptr) {
                x->parent = y->parent;
            }
            if (IsLeftChild(y)) {
                SetLeft(y->parent, x);
                if (y != root) {
                    w = static_cast<decltype(w)>(y->parent->right);
                }
                else {
                    // w is nullptr, root has no uncle
                    root = static_cast<decltype(root)>(x);
                }
            }
            else {
                SetRight(y->parent, x);
                w = static_cast<decltype(w)>(y->parent->left);
            }

            bool is_remove_black = IsBlack(y);

            if (y!=z) {
                y->parent = z->parent;
                if (IsLeftChild(z)) {
                    SetLeft(y->parent, y);
                }
                else {
                    SetRight(y->parent, y);
                }
                y->left = z->left;
                if (y->left != nullptr)
                    SetParent(y->left, y);
                y->right = z->right;
                if (y->right != nullptr)
                    SetParent(y->right, y);
                y->color = z->color;
                if (root == z) {
                    root = y;
                }
            }

            // If remove red node, no need to rebalance
            if (is_remove_black && root != nullptr) {
                if (x!= nullptr) {
                    TurnBlack(x);
                }
                else {
                    while(true) {
                        if (IsRightChild(x)) {
                            if (IsRed(w)) {
                                TurnBlack(w);
                                TurnRed(w->parent);
                                LeftRotate(w->parent);
                                if (root == w->left) {
                                    root = w;
                                }
                                w = static_cast<decltype(w)>(w->left->right);
                            }
                            if ((w->left== nullptr || IsBlack(w->left)) && (w->right== nullptr || IsBlack(w->right))) {
                                TurnRed(w);
                                x = static_cast<decltype(x)>(w->parent);
                                if (x==root || IsRed(x)) {
                                    TurnBlack(x);
                                    break;
                                }
                                w = IsLeftChild(x) ? static_cast<decltype(w)>(x->parent->right) : static_cast<decltype(w)>(x->parent->left);
                            }
                            else {
                                if (w->right== nullptr || IsBlack(w->right)) {
                                    TurnBlack(w->left);
                                    TurnRed(w);
                                    RightRotate(w);
                                    w=static_cast<decltype(w)>(w->parent);
                                }
                                w->color = w->parent->color;
                                TurnBlack(w->parent);
                                TurnBlack(w->right);
                                LeftRotate(w->parent);
                                break;
                            }
                        }
                        else {
                            if (IsRed(w)) {
                                TurnBlack(w);
                                TurnRed(w->parent);
                                RightRotate(w->parent);
                                if (root == w->right) {
                                    root = w;
                                }
                                w = static_cast<decltype(w)>(w->left->left);
                            }
                            if ((w->left== nullptr || IsBlack(w->left)) && (w->right== nullptr || IsBlack(w->right))) {
                                TurnRed(w);
                                TurnRed(w);
                                x = static_cast<decltype(x)>(w->parent);
                                if (x==root || IsRed(x)) {
                                    TurnBlack(x);
                                    break;
                                }
                                w = IsLeftChild(x) ? static_cast<decltype(x)>(x->parent->right) : static_cast<decltype(x)>(x->parent->left);
                            }
                            else {
                                if (w->left== nullptr || IsBlack(w->left)) {
                                    TurnBlack(w->right);
                                    TurnRed(w);
                                    LeftRotate(w);
                                    w=static_cast<decltype(w)>(w->parent);
                                }
                                w->color = w->parent->color;
                                TurnBlack(w->parent);
                                TurnBlack(w->left);
                                RightRotate(w->parent);
                                break;
                            }
                        }
                    }
                }
            }
        }

        Node* removeNode(Node* ptr) {
            --sz;
            auto r = IncNode(ptr);
            if (getBeginPtr() == ptr) {
                setBeginPtr(r);
            }
            RemoveNode(getRootPtr(), ptr);
            return r;
        }

        Node* allocateNode() {
            auto ptr = allocator.template allocate<Node>();
            return ptr;
        }
        template<class ...Args>
        void constructValueOnNode(Node* x, Args&&...args) {
            Construct(GetAddress(x->value), Forward<Args>(args)...);
        }
        template<class KeyOther, class ...Args>
        void constructKeyValueOnNode(Node* x, KeyOther&& key, Args&&...args) {
            Construct(extractor.getKeyAddr(x->value), Forward<KeyOther>(key));
            Construct(extractor.getValueAddr(x->value), Forward<Args>(args)...);
        }
        void destructKeyValueOnNode(Node* x) {
            Destruct(GetAddress(x->value));
        }
        void deleteNode(Node* ptr) {
            destructKeyValueOnNode(ptr);
            allocator.template deallocate(ptr);
        }

        Node* getBeginPtr() const {
            return const_cast<Node*>(static_cast<const Node*>(sentinel.parent));
        }
        Node* getRootPtr() const {
            return const_cast<Node*>(static_cast<const Node*>(sentinel.left));
        }

        Node* getSentinelPtr() const {
            return const_cast<Node*>(static_cast<const Node*>(&sentinel));
        }

        void initSentinel() {
            sentinel.parent = getSentinelPtr();
            sentinel.left = nullptr;
            sentinel.right = nullptr;
        }

        static void DestroyTree(Node* root, Allocator& alloc) {
            if (root->left != nullptr) {
                DestroyTree(static_cast<Node*>(root->left), alloc);
            }
            if (root->right != nullptr) {
                DestroyTree(static_cast<Node*>(root->right), alloc);
            }
            Destruct(GetAddress(root->value));
            alloc.template deallocate(root);
        }

        void detachTo(RBTreeSentinelNode& s) {
            BaseNode* sptr = static_cast<BaseNode*>(&s);
            if (isEmpty()) {
                s.left = nullptr;
                s.right = nullptr;
                s.parent = sptr;
            }
            else {
                s.left = sentinel.left;
                s.right = sentinel.right;
                s.parent = sentinel.parent;
                s.left->parent = sptr;
            }
            initSentinel();
        }
    public:

        RedBlackTree()
            : compare()
            , extractor ()
            , allocator()
            , sz(0)
        {
                initSentinel();
        }

        explicit RedBlackTree(const KeyCompare& compare)
            : compare(compare)
            , extractor()
            , allocator()
            , sz(0)
        {
                initSentinel();
        }

        RedBlackTree(const RedBlackTree& other)
            : compare(other.compare)
            , extractor()
            , allocator(other.allocator)
        {
            sz=0;
            initSentinel();
            auto hint = begin();
            for (auto it = other.begin(); it != other.end(); ++it) {
                hint = hintEmplace(hint, *it).iterator;
            }
        }

        RedBlackTree(RedBlackTree&& other) noexcept
            : compare(Move(other.compare))
            , extractor(Move(other.extractor))
            , allocator(other.allocator)
        {
            sz = other.sz;
            other.detachTo(sentinel);
        }

        ~RedBlackTree() {
            clear();
        }

        auto begin() noexcept -> Iterator {
            return Iterator(getBeginPtr());
        }
        auto end() noexcept -> Iterator {
            return Iterator(getSentinelPtr());
        }
        auto begin() const noexcept -> ConstIterator {
            return ConstIterator(getBeginPtr());
        }
        auto end() const noexcept -> ConstIterator {
            return ConstIterator(getSentinelPtr());
        }
        auto cbegin() const noexcept -> ConstIterator {
            return begin();
        }
        auto cend() const noexcept -> ConstIterator {
            return end();
        }

        void clear() {
            auto root = getRootPtr();
            initSentinel();
            sz = 0;
            if (root != nullptr)
                DestroyTree(root, allocator);
        }

        auto size() const noexcept-> SizeType { return sz; }
        bool isEmpty() const noexcept { return sz==0; }

        template<class KeyOther, class ...Args>
        InsertionResult emplaceKeyArgs(const KeyOther& key, Args&&...args)
            requires Extractor::HAS_VALUE and (not MULTI_KEY)
        {
            BaseNode* parent;
            auto& child = findInsertionPlace(parent, key);
            if (child == nullptr) {
                Node* ptr = allocateNode();
                constructKeyValueOnNode(ptr, (key), Forward<Args>(args)...);
                insertNodeAt(parent, child, ptr);
                return { Iterator(ToNodePtr(ptr)), true };
            }
            return {Iterator(getSentinelPtr()), false};
        }

        template<class ...Args>
        InsertionResult emplace(Args&&...args)
            requires Extractor::HAS_VALUE and (not MULTI_KEY)
        {
            BaseNode* parent;
            Node* ptr = allocateNode();
            constructValueOnNode(ptr, Forward<Args>(args)...);
            auto& child = findInsertionPlace(parent, extractor.getKey(ptr->value));
            if (child == nullptr) {
                insertNodeAt(parent, child, ptr);
                return { Iterator(ToNodePtr(ptr)), true};
            }
            else {
                deleteNode(ptr);
                return { Iterator(getSentinelPtr()), false};
            }
        }

        template<class ...Args>
            requires Extractor::HAS_VALUE and (not MULTI_KEY)
        InsertionResult hintEmplace(ConstIterator hint, Args&&...args) {
            BaseNode* parent;
            Node* ptr = allocateNode();
            constructValueOnNode(ptr, Forward<Args>(args)...);
            auto& child = findInsertionPlace(parent, extractor.getKey(ptr->value), hint.ptr);
            if (child== nullptr) {
                insertNodeAt(parent, child, ptr);
                return { Iterator(ToNodePtr(ptr)), true};
            }
            else {
                deleteNode(ptr);
                return { Iterator(getSentinelPtr()), true};
            }
        }


        Iterator find(const KeyType& key)
            requires Extractor::HAS_VALUE and (not MULTI_KEY)
        {
            return Iterator(findNode(key));
        }

        ConstIterator find(const KeyType& key) const
            requires Extractor::HAS_VALUE and (not MULTI_KEY)
        {
            return ConstIterator(findNode(key));
        }

        Iterator remove(ConstIterator pos) {
            return Iterator(removeNode(pos.ptr));
        }
    };
}