export module ntl.containers.hash_table;
import ntl.memory.allocator;
import ntl.functional.hash;
import ntl.utils;
import ntl.containers.key_value;
import ntl.iterator;

namespace ne
{
    template<class TheKVType, template<class> typename TheExtractorType, class HasherType, class TheKeyEqual, bool MULTI_KEY>
        requires ConceptKVExtractor<TheExtractorType, TheKVType>
    class HashTable;
    template<class T>
    struct HashNodeBase;
    template<class T>
    struct HashNode;
    template<class T>
    struct HashNodeBase
    {
        HashNode<T>* next;
    };
    template<class T>
    struct HashNode : HashNodeBase<T>
    {
        HashValue hash;
        T value;
    };

    template<class TheHashTable>
    struct BucketList
    {
        using NodeType = typename TheHashTable::BaseNode;
        using NodePtr = NodeType*;
        using SizeType = typename TheHashTable::SizeType;
        // Note that it store the previous node of the bucket
        NodePtr* item;
        SizeType count;
    };

    template<class T>
    void SwapBucketList(BucketList<T>& a, BucketList<T>& b)
    {
        utils::Swap(a.item, b.item);
        utils::Swap(a.count, b.count);
    }

    template<class T>
    BucketList<T> NewBucketList(Allocator& allocator, typename BucketList<T>::SizeType bucket_count)
    {
        BucketList<T> new_bucket_list;
        new_bucket_list.item = allocator.allocate<typename BucketList<T>::NodePtr>(bucket_count);
        ConstructN(new_bucket_list.item, bucket_count, nullptr);
        new_bucket_list.count = bucket_count;
        return new_bucket_list;
    }

    template<class T>
    void SetBucketList(BucketList<T>& bucket_list, typename BucketList<T>::NodePtr* item = nullptr, typename BucketList<T>::SizeType bucket_count = 0)
    {
        bucket_list.item = item;
        bucket_list.count = bucket_count;
    }

    template<class NodePtr>
    void DestroyBucketList(Allocator& allocator, BucketList<NodePtr>& bucket_list)
    {
        allocator.deallocate(bucket_list.item);
        bucket_list.item = nullptr;
        bucket_list.count = 0;
    }

    template<class HashTableType>
    class HashTableConstIterator;

    template<class HashTableType>
    class HashTableIterator
    {
    private:
        using NodePtr = typename HashTableType::Node*;
        NodePtr nd_ptr;
    public:
        using ThisType = HashTableIterator<HashTableType>;
        using IteratorCategory = ForwardIteratorCategory;
        using ValueType = typename HashTableType::ValueType;
        using DifferenceType = typename HashTableType::DifferenceType;
        using Pointer = typename HashTableType::Pointer;
        using Reference = typename HashTableType::Reference;
        using ConstReference = typename HashTableType::ConstReference;

        template<class> friend class HashTableConstIterator;
        template<class TheKVType, template<class> typename TheExtractorType, class HasherType, class TheKeyEqual, bool MULTI_KEY>
            requires ConceptKVExtractor<TheExtractorType, TheKVType>
        friend class HashTable;

        HashTableIterator() : nd_ptr(nullptr) {}
        HashTableIterator(NodePtr ptr) :nd_ptr(ptr) {}
        HashTableIterator(const HashTableIterator&) = default;
        HashTableIterator(HashTableIterator&&) = default;
        ~HashTableIterator() = default;

        ThisType& operator=(const HashTableIterator&) = default;
        ThisType& operator=(HashTableIterator&&) = default;

        Reference operator*() const noexcept
        {
            return nd_ptr->value;
        }
        Pointer operator->() const noexcept
        {
            return GetAddress(nd_ptr->value);
        }
        ThisType& operator++() noexcept
        {
            nd_ptr = nd_ptr->next;
            return *this;
        }
        ThisType& operator++(int) noexcept
        {
            auto tmp = (*this);
            ++(*this);
            return tmp;
        }

        [[nodiscard]] bool operator==(const HashTableIterator& rhs) const noexcept
        {
            return nd_ptr == rhs.nd_ptr;
        }

        [[nodiscard]] bool operator!=(const HashTableIterator& rhs) const noexcept
        {
            return nd_ptr != rhs.nd_ptr;
        }
    };
    template<class HashTableType>
    class HashTableConstIterator
    {
    private:
        using NodePtr = typename HashTableType::Node*;
        NodePtr nd_ptr;
    public:
        using ThisType = HashTableConstIterator<HashTableType>;
        using IteratorCategory = ForwardIteratorCategory;
        using ValueType = typename HashTableType::ValueType;
        using DifferenceType = typename HashTableType::DifferenceType;
        using Pointer = typename HashTableType::ConstPointer;
        using Reference = typename HashTableType::ConstReference;

        template<class> friend class HashTableIterator;
        template<class TheKVType, template<class> typename TheExtractorType, class HasherType, class TheKeyEqual, bool MULTI_KEY>
            requires ConceptKVExtractor<TheExtractorType, TheKVType>
        friend class HashTable;

        HashTableConstIterator() : nd_ptr(nullptr) {}
        HashTableConstIterator(HashTableIterator<HashTableType> it): nd_ptr(it.nd_ptr) {}
        HashTableConstIterator(NodePtr ptr) :nd_ptr(ptr) {}
        HashTableConstIterator(const HashTableConstIterator&) = default;
        HashTableConstIterator(HashTableConstIterator&&) = default;
        ~HashTableConstIterator() = default;

        ThisType& operator=(const HashTableConstIterator&) = default;
        ThisType& operator=(HashTableConstIterator&&) = default;

        Reference operator*() const noexcept
        {
            return nd_ptr->value;
        }
        Pointer operator->() const noexcept
        {
            return GetAddress(nd_ptr->value);
        }
        ThisType& operator++() noexcept
        {
            nd_ptr = nd_ptr->next;
            return *this;
        }
        ThisType& operator++(int) noexcept
        {
            auto tmp = (*this);
            ++(*this);
            return tmp;
        }

        [[nodiscard]] bool operator==(const HashTableConstIterator& rhs) const noexcept
        {
            return nd_ptr == rhs.nd_ptr;
        }

        [[nodiscard]] bool operator!=(const HashTableConstIterator& rhs) const noexcept
        {
            return nd_ptr != rhs.nd_ptr;
        }
    };

}
export namespace ne
{
    template<class TheKVType, template<class> typename TheExtractorType, class HasherType, class TheKeyEqual, bool MULTI_KEY>
        requires ConceptKVExtractor<TheExtractorType, TheKVType>
    class HashTable
    {
    public:
        using ThisType = HashTable<TheKVType, TheExtractorType, HasherType, TheKeyEqual, MULTI_KEY>;
        using Extractor = TheExtractorType<TheKVType>;
        using KeyType = typename Extractor::KeyType;
        using MappedType = typename Extractor::ValueType;
        using ValueType = TheKVType;
        using SizeType = int64;
        using DifferenceType = ptrdiff_t;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using Iterator = HashTableIterator<ThisType>;
        using ConstIterator = HashTableConstIterator<ThisType>;
        using LocalIterator = HashTableIterator<ThisType>;
        using ConstLocalIterator = HashTableConstIterator<ThisType>;
        using Hasher = HasherType;
        using KeyEqual = TheKeyEqual;
    protected:
        template<class T>
        friend struct BucketList;
        template<class T>
        friend class HashTableIterator;
        template<class T>
        friend class HashTableConstIterator;
        using BaseNode = HashNodeBase<ValueType>;
        using Node = HashNode<ValueType>;

        constexpr static float DEFAULT_MAX_LOAD_FACTOR = 1;

        [[no_unique_address]]
        Hasher hasher;
        [[no_unique_address]]
        Extractor extractor;
        [[no_unique_address]]
        KeyEqual key_equal;
        SizeType sz;
        Allocator allocator;
        float max_load_factor;
        // its next points to the first node; 
        BaseNode sentinel;
        /**
         * S->A(1)->B(1)->C(0)->D(2)
         *
         *  Bucket:  0(B)   1(S)   2(C)   
         *            |      |      |
         *            C      A      D
         *                   B
         */
        BucketList<ThisType> bucket_list;

        static SizeType SelectPrime(SizeType target_num)
        {
            static constexpr HashValue PRIME_LIST[] = {
                7, 17,
                37, 61,
                97,
                167, 257,
                409,
                709,
                1031,
                1741, 2357,
                3209, 4349, 5503,
                6949, 8123, 9497,
                11113, 14033,
                19183, 26267,
                33223, 53201,
                99733, //107897, 116731, 126271, 136607, 147793,
                159871, //172933, 187091, 202409, 218971, 236897,
                256279, //277261, 299951, 324503, 351061, 379787,
                410857, //444487, 480881, 520241, 562841, 608903,
                658753, //712697, 771049, 834181, 902483, 976369,
                1056323, //1142821, 1236397, 1337629, 1447153, 1565659,
                1693859, //1832561, 1982627, 2144977, 2320627, 2510653,
                2716249, //2938679, 3179303, 3439651, 3721303, 4026031,
                4355707, //4712381, 5098259, 5515729, 5967347, 6456007,
                6984629, //7556579, 8175383, 8844859, 9569143, 10352717,
                11200489, //12117689, 13109983, 14183539, 15345007,
                16601593, //17961079, 19431899, 21023161, 22744717,
                24607243,// 26622317, 28802401, 31160981, 33712729,
                36473443, //39460231, 42691603, 46187573, 49969847,
                54061849, //58488943, 63278561, 68460391, 74066549,
                80131819, //86693767, 93793069, 101473717, 109783337,
                118773397, //128499677, 139022417, 150406843, 162723577,
                176048909, //190465427, 206062531, 222936881, 241193053,
                260944219, //282312799, 305431229, 330442829, 357502601,
                386778277, //418451333, 452718089, 489790921, 529899637,
                573292817, //620239453, 671030513, 725980837, 785430967,
                849749479, //919334987, 994618837, 1076067617, 1164186217,
                1259520799, //1362662261, 1474249943, 1594975441,
                1725587117, //1866894511, 2019773507, 2185171673,
                2364114217, //2557710269, 2767159799, 2993761039,
                3238918481, //3504151727, 3791104843, 4101556399,
                4294967291
            };
            constexpr SizeType PRIME_LEN = sizeof(PRIME_LIST) / sizeof(HashValue);

            if (target_num < PRIME_LIST[0])
            {
                return PRIME_LIST[0];
            }
            else if (target_num >= PRIME_LIST[PRIME_LEN - 1])
            {
                return PRIME_LIST[PRIME_LEN - 1];
            }

            SizeType left = 0;
            SizeType right = PRIME_LEN;
            while (left < right)
            {
                SizeType mid = (left + right) / 2;
                if (target_num == PRIME_LIST[mid])
                {
                    return PRIME_LIST[mid];
                }
                else if (target_num < PRIME_LIST[mid])
                {
                    right = mid;
                }
                else
                {
                    left = mid + 1;
                }
            }
            return PRIME_LIST[left];
        }

        SizeType getSuitableBucketCount(SizeType elem_num) const noexcept
        {
            auto should_contained = max_load_factor * bucket_list.count;
            if (elem_num<=should_contained && bucket_list.count !=0)
            {
                return bucket_list.count;
            }

            const SizeType target_num = (static_cast<float>(elem_num)/max_load_factor);

            return SelectPrime(target_num);

        }

        // Note that the last node's next pointer is nullptr
        void initSentinel() noexcept
        {
            sentinel.next = nullptr;
        }
        Node* sentinelNodePtr() noexcept
        {
            return static_cast<Node*>(&sentinel);
        }
        const Node* sentinelNodePtr() const noexcept
        {
            return static_cast<const Node*>(&sentinel);
        }
        BaseNode* sentinelBasePtr() noexcept
        {
            return &sentinel;
        }
        const BaseNode* sentinelBasePtr() const noexcept
        {
            return &sentinel;
        }
        inline void destroyNode(Node* ptr, bool key_only=false)
        {
            if (key_only)
            {
                Destruct(GetAddress(getNodeKey(ptr)));
            }
            else
            {
                Destruct(ptr);
            }
            allocator.deallocate(ptr);
        }
        Node* allocateNode()
        {
            return allocator.allocate<Node>();
        }
        const KeyType& getNodeKey(const Node* n) const noexcept
        {
            return extractor.getKey(n->value);
        }

        template<class ...Args>
        void constructAtNode(Node* np, Args&&...args)
        {
            Construct(GetAddress(np->value), Forward<Args>(args)...);
        }
        void calNodeHash(Node* np)
        {
            np->hash = hasher(getNodeKey(np));
        }
        template<class ...Args>
        void constructAtNodeAndSetHash(Node* np, Args&&...args)
        {
            constructAtNode(np, Forward<Args>(args)...);
            calNodeHash(np);
        }

        // attach C at A : A->B ==> A->C->B
        void attachNode(BaseNode* at, Node* target)
        {
            target->next = at->next;
            at->next = target;
        }

        Node* findNodeWithKey(const KeyType& key) const noexcept
        {
            if (bucket_list.count==0)
            {
                return nullptr;
            }
            auto hv = hash(key);
            auto bkl = bucket_list.item;
            auto bptr = bkl[hv];
            if (bptr == nullptr) return nullptr;
            auto ptr = bptr->next;
            while (ptr!=nullptr && modHash(ptr->hash) == hv)
            {
                if (key_equal(getNodeKey(ptr), key))
                {
                    return ptr;
                }
                ptr = ptr->next;
            }
            return nullptr;
        }

        Node* findNodeSameKey(const Node* node) const noexcept
        {
            if (bucket_list.count == 0)
            {
                return nullptr;
            }
            auto hv = hash(node);
            auto bkl = bucket_list.item;
            auto bptr = bkl[hv];
            if (bptr == nullptr)
            {
                return nullptr;
            }
            auto ptr = bptr->next;
            while (ptr != nullptr && modHash(ptr->hash) == hv)
            {
                if (key_equal(getNodeKey(ptr), getNodeKey(node)))
                {
                    return ptr;
                }
                ptr = ptr->next;
            }
            return nullptr;
        }

        // insert node, inc size, rehash if necessary, check if key is existed.
        bool insertNodeIfKeyUnique(Node* node)
            requires (!MULTI_KEY)
        {
            auto fp = findNodeSameKey(node);
            if (fp != nullptr)
            {
                return false;
            }

            insertNode(node);
            return true;
        }

        void insertNodeNoRehash(Node* node)
        {
            ++sz;
            auto target_bucket_id = hash(getNodeKey(node));
            if (bucket_list.item[target_bucket_id] == nullptr)
            {
                attachNode(&sentinel, node);
                bucket_list.item[target_bucket_id] = sentinelBasePtr();
                if (node->next != nullptr)
                {
                    bucket_list.item[hash(node->next)] = node;
                }
            }
            else
            {
                attachNode(bucket_list.item[target_bucket_id], node);
            }
        }

        // insert node, inc size, rehash if necessary, do not check if key is existed.
    	void insertNode(Node* node)
            requires (!MULTI_KEY)
        {
            auto nbc = getSuitableBucketCount(sz);
            if (nbc != bucket_list.count)
            {
                rehash(nbc);
            }
            insertNodeNoRehash(node);
        }

        HashValue modHash(HashValue h) const noexcept
        {
            return h % bucket_list.count;
        }

        // hash value % bucket count
        HashValue hash(const KeyType& key) const noexcept(noexcept(hasher(Declval<KeyType>())))
        {
            return modHash(hasher(key));
        }
        // hash value % bucket count
    	HashValue hash(const Node* node) const
        {
            return hash(getNodeKey(node));
        }

        void rehash(SizeType new_bucket_count)
        {
            auto new_bucket_list = NewBucketList<ThisType>(allocator, new_bucket_count);
            SwapBucketList(bucket_list, new_bucket_list);

            /*for(SizeType i=0; i<bucket_list.count; i++)
            {
                if (bucket_list.item[i] == nullptr)
                {
                    continue;
                }
                auto nodeptr = bucket_list.item[i];
                auto hashval = nodeptr->next->hash;
                auto new_hash = hashval % new_bucket_count;
                new_bucket_list.item[new_hash] = nodeptr;
            }*/
            auto prev_ptr = sentinelBasePtr();
            auto ptr = prev_ptr->next;
            prev_ptr->next = nullptr;
            sz = 0;
            while (ptr != nullptr)
            {
                auto tmp = ptr->next;
                ptr->next = nullptr;
                insertNode(ptr);
                ptr = tmp;
            }

            DestroyBucketList(allocator, new_bucket_list);
        }

        Node* removeNode(Node* node)
			requires !MULTI_KEY
        {
            --sz;
            auto hashval = hash(node);
            auto bp = bucket_list.item[hashval];
            while(bp->next != node)
            {
                bp = bp->next;
            }
            bp->next = node->next;

            if (node->next != nullptr) {
                auto next_hash = hash(node->next);
                if (next_hash != hashval)
                {
                    bucket_list.item[next_hash] = bp;
                }
                if (hash(bucket_list.item[hashval]->next) != hashval)
                {
                    bucket_list.item[hashval] = nullptr;
                }
            }
            destroyNode(node);
            return bp->next;
        }

    public:
        struct InsertResult
        {
            const Iterator  iterator;
            const bool success;

            operator bool() const noexcept
            {
                return success;
            }
        };
        HashTable()
            : hasher()
            , extractor()
            , key_equal()
            , sz()
            , allocator()
            , max_load_factor(DEFAULT_MAX_LOAD_FACTOR)
            , bucket_list{nullptr, 0}
        {
            initSentinel();
        }

        explicit
        HashTable(const Hasher& custom_hash, const KeyEqual& custom_key_equal = KeyEqual(), const Allocator& alloc = Allocator())
	        : hasher(custom_hash)
    		, extractor()
    		, key_equal(custom_key_equal)
    		, sz()
    		, allocator(alloc)
			, max_load_factor(DEFAULT_MAX_LOAD_FACTOR)
			, bucket_list(nullptr, 0)
        {
            initSentinel();
        }

        HashTable(const Hasher& custom_hash, const Allocator& alloc)
	        : HashTable(custom_hash, KeyEqual(), alloc)
        {}

        explicit 
        HashTable(const KeyEqual& custom_key_equal, const Allocator& alloc = Allocator())
            : HashTable(Hasher(), custom_key_equal, alloc)
        {}

        explicit 
    	HashTable(SizeType bucket_count, const Hasher& custom_hash = Hasher(), const KeyEqual& custom_key_equal = KeyEqual(), const Allocator& alloc=Allocator())
	        : hasher(custom_hash)
			, extractor()
			, key_equal(custom_key_equal)
			, sz()
			, allocator(alloc)
			, max_load_factor(DEFAULT_MAX_LOAD_FACTOR)
        {
            bucket_list = NewBucketList<ThisType>(allocator, bucket_count);
            initSentinel();
        }

    	HashTable(SizeType bucket_count, const KeyEqual& custom_key_equal, const Allocator& alloc=Allocator())
            : HashTable(bucket_count, Hasher(), custom_key_equal, alloc)
        {}

        explicit 
        HashTable(const Allocator& allocator)
            : hasher()
            , extractor()
            , key_equal()
            , sz()
            , allocator(allocator)
            , max_load_factor(DEFAULT_MAX_LOAD_FACTOR)
            , bucket_list{ nullptr, 0 }
        {
            initSentinel();
        }

        HashTable(SizeType bucket_count, const Allocator& alloc)
            : HashTable(bucket_count, Hasher(), KeyEqual(), alloc)
        {}

        HashTable(const HashTable& ht)
            : hasher(ht.hasher)
            , extractor(ht.extractor)
            , key_equal(ht.key_equal)
            , sz(ht.sz)
            , allocator(ht.allocator)
            , max_load_factor(ht.max_load_factor)
        {
            initSentinel();
            bucket_list = NewBucketList<ThisType>(allocator, ht.bucket_list.count);
            // TODO:OPTIMIZATION
            auto ptr = ht.sentinel->next;
            while(ptr!=nullptr)
            {
                auto np = allocateNode();
                constructAtNode(np, ptr->value);
                np->hash = ptr->hash;
                insertNode(np);
                ptr = ptr->next;
            }
        }

        HashTable(const HashTable& ht, const Allocator& allocator)
            : hasher(ht.hasher)
            , extractor(ht.extractor)
            , key_equal(ht.key_equal)
            , sz(ht.sz)
            , allocator(allocator)
            , max_load_factor(ht.max_load_factor)
        {
            initSentinel();
            bucket_list = NewBucketList<ThisType>(allocator, ht.bucket_list.count);
            // TODO:OPTIMIZATION
            auto ptr = ht.sentinel->next;
            while (ptr != nullptr)
            {
                auto np = allocateNode();
                constructAtNode(np, ptr->value);
                np->hash = ptr->hash;
                insertNode(np);
                ptr = ptr->next;
            }
        }

        HashTable(HashTable&& ht) noexcept
            : hasher(Move(ht.hasher))
            , extractor(Move(ht.extractor))
            , key_equal(Move(ht.key_equal))
            , sz(ht.sz)
            , allocator(Move(ht.allocator))
            , max_load_factor(ht.max_load_factor)
        {
            bucket_list = ht.bucket_list;
            ht.bucket_list.item = nullptr;
            ht.bucket_list.count = 0;
            ht.sz = 0;
            sentinel->next = ht.sentinel->next;
            ht.initSentinel();
        }

        HashTable(HashTable&& ht, const Allocator& allocator)
            : hasher(Move(ht.hasher))
            , extractor(Move(ht.extractor))
            , key_equal(Move(ht.key_equal))
            , sz(ht.sz)
            , max_load_factor(ht.max_load_factor)
        {
            if (this->allocator == allocator) {
                bucket_list = ht.bucket_list;
                ht.bucket_list.item = nullptr;
                ht.bucket_list.count = 0;
                ht.sz = 0;
                sentinel->next = ht.sentinel->next;
                ht.initSentinel();
            }
            else
            {
                this->allocator = allocator;
                initSentinel();
                bucket_list = NewBucketList<ThisType>(allocator, ht.bucket_list.count);
                // TODO:OPTIMIZATION
                auto ptr = ht.sentinel->next;
                while (ptr != nullptr)
                {
                    auto np = allocateNode();
                    constructAtNode(np, Move(ptr->value));
                    np->hash = ptr->hash;
                    insertNode(np);
                    ptr = ptr->next;
                }

                ht.clear();
            }
        }

        // TODO: operator=

        ~HashTable()
        {
            clear();
        }

        void clear()
        {
            sz = 0;
            auto ptr = sentinelNodePtr()->next;
            while (ptr != nullptr)
            {
                auto tmp = ptr;
                ptr = ptr->next;
                destroyNode(tmp);
            }
            initSentinel();
            DestroyBucketList(allocator, bucket_list);
            //SetBucketList(bucket_list);
        }

        SizeType size() const noexcept { return sz; }
        bool isEmpty() const noexcept { return sz == 0; }

        float getLoadFactor() const noexcept
        {
            auto bc = bucket_list.count;
            return bc == 0 ? 0 : float(sz) / float(bc);
        }

        void swap(ThisType& h) noexcept
        {
            if (allocator == h.allocator)
            {
                utils::Swap(bucket_list, h.bucket_list);
                utils::Swap(sz, h.sz);
                utils::Swap(max_load_factor, h.max_load_factor);
                utils::Swap(sentinel.next, h.sentinel.next);
            }
            else
            {
                Assert(false, "[ntl.containers.hash_table] Not Impl.");
            }
        }

        template<class...Args>
            requires not MULTI_KEY
        InsertResult emplace(Args&&...args)
        {
            auto np = allocateNode();
            constructAtNodeAndSetHash(np, Forward<Args>(args)...);
            auto fp = findNodeSameKey(np);
            if (fp==nullptr)
            {
                insertNode(np);
                return { np, true };
            }
            else
            {
                return { fp, false };
            }
        }

        Iterator remove(ConstIterator it)
        {
            auto np = it.nd_ptr;
            return removeNode(np);
        }

        Iterator find(const KeyType& key)
        {
            auto fp = findNodeWithKey(key);
            return fp;
        }

        ConstIterator find(const KeyType& key) const
        {
            return findNodeWithKey(key);
        }

        Iterator begin() noexcept
        {
            return sentinel.next;
        }
        Iterator end() noexcept
        {
            return nullptr;
        }
        ConstIterator begin() const noexcept
        {
            return sentinel.next;
        }
        ConstIterator end() const noexcept
        {
            return nullptr;
        }
        ConstIterator cbegin() const noexcept
        {
            return sentinel.next;
        }
        ConstIterator cend() const noexcept
        {
            return nullptr;
        }
    };


    
}
