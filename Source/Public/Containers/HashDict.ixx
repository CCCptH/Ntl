export module ntl.containers.hash_dict;
import ntl.containers.hash_table;
import ntl.containers.key_value;
import ntl.utils;
import ntl.tuple;
import ntl.functional.hash;
import ntl.type_traits;
import ntl.exceptions;

namespace ne
{
    // TODO
    template<class T>
	struct Eq
	{
		bool operator()(const T& a, const T& b) const
		{
            return a == b;
		}
	};
}

export namespace ne
{
    template<class Key, class Value, class HashType = Hash<Key>, class KeyEqualType = Eq<Key>>
    class HashDict
        : protected HashTable<KeyValue<Key, Value>, DefaultKeyValueExtractor, HashType, KeyEqualType, false>
    {
    public:
        using ValueType = KeyValue<Key, Value>;
        using ThisType = HashDict<Key, Value, HashType, KeyEqualType>;
        using KeyType = Key;
        using MappedType = Value;
        using SizeType = int64;
        using DifferenceType = ptrdiff_t;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using Hasher = HashType;
        using KeyEqual = KeyEqualType;
    private:
        using BaseType = HashTable<ValueType, DefaultKeyValueExtractor, Hasher, KeyEqual, false>;
    public:
        using Iterator = typename BaseType::Iterator;
        using ConstIterator = typename BaseType::ConstIterator;
        //using LocalIterator = HashTableIterator<ThisType>;
        //using ConstLocalIter = HashTableConstIter<ThisType>;
        using InsertResult = typename BaseType::InsertResult;

        using BaseType::BaseType;

        /**
         * @brief Emplace key value with args.
         * @tparam ...Args 
         * @param ...args 
         * @return If key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
        */
        template<class ...Args>
        auto emplace(Args&&...args) -> InsertResult;

        /**
         * @brief If key does not exist, emplace key with value.
         * @tparam ...Args 
         * @param key 
         * @param ...args 
         * @return If key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
        */
        template<class ...Args>
        auto tryEmplace(const KeyType& key, Args&&...args) -> InsertResult;
        /**
         * @brief If key does not exist, emplace key with value.
         * @tparam ...Args
         * @param key
         * @param ...args
         * @return If key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
        */
        template<class ...Args>
        auto tryEmplace(KeyType&& key, Args&&...args) -> InsertResult;

        /**
         * @brief Find node with key.
         * @param key 
         * @return If not exists, return end().
        */
        auto find(const KeyType& key) -> Iterator;
        /**
         * @brief Find node with key.
         * @param key
         * @return If not exists, return end().
        */
        auto find(const KeyType& key) const->ConstIterator;

        /**
         * @brief Remove all items;
        */
        void clear();

        /**
         * @brief Return count of items;
        */
        auto size() const noexcept -> SizeType;

        /**
         * @brief Insert key_value if key does not exist.
         * @param key_value 
         * @return If key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
        */
        auto insert(const ValueType& key_value) -> InsertResult;
        /**
         * @brief Insert key_value if key does not exist.
         * @param key_value
         * @returnIf key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
        */
        auto insert(ValueType&& key_value) -> InsertResult;

        /**
         * @brief Returns a reference to the mapped value of the element with key equivalent to key. If no such element exists, an exception of type OutOfRange is thrown.
         * @param key 
         * @return Reference to the mapped value
        */
        auto at(const KeyType& key) -> MappedType&;
        /**
         * @brief Returns a reference to the mapped value of the element with key equivalent to key. If no such element exists, an exception of type OutOfRange is thrown.
         * @param key
         * @return Reference to the mapped value
        */
        auto at(const KeyType& key) const->const MappedType&;

        auto begin() noexcept -> Iterator;
        auto end() noexcept -> Iterator;
        auto begin() const noexcept -> ConstIterator;
        auto end() const noexcept -> ConstIterator;
        auto cbegin() const noexcept -> ConstIterator;
        auto cend() const noexcept -> ConstIterator;

        /**
         * @brief Remove item
         * @param key 
         * @return Size after remove.
        */
        auto remove(const KeyType& key) -> SizeType;
        /**
         * @brief Remove item at `pos`
         * @param pos
         * @return iterator points to next item
        */
        auto remove(Iterator pos) -> Iterator;
        /**
         * @brief Remove item at `pos`
         * @param pos
         * @return iterator points to next item
        */
        auto remove(ConstIterator pos) -> Iterator;

        /**
         * @brief If key exists, assign, else insert.
         * @tparam M 
         * @param key 
         * @param value 
         * @return 
        */
        template<class M>
			requires TestIsAssignable<typename HashDict<Key, Value, HashType, KeyEqualType>::MappedType&, M&&>
        auto insertOrAssign(const KeyType& key, M&& value) -> InsertResult;

        template<class M>
            requires TestIsAssignable<typename HashDict<Key, Value, HashType, KeyEqualType>::MappedType&, M&&>
        auto insertOrAssign(KeyType&& key, M&& value) -> InsertResult;

        auto operator[](const KeyType& key) ->MappedType& 
            requires TestIsDefaultConstructible<HashDict<Key, Value, HashType, KeyEqualType>::MappedType>;
        auto operator[](KeyType&& key)->MappedType& 
            requires TestIsDefaultConstructible<HashDict<Key, Value, HashType, KeyEqualType>::MappedType>;


    };
}

namespace ne
{
    template<class Key, class Value, class HashType, class KeyEqualType>
    template<class ...Args>
    auto HashDict<Key, Value, HashType, KeyEqualType>::emplace(Args && ...args) -> InsertResult
    {
        static_assert(TestIsConstructible<ValueType, Args...>, "[ntl.type_traits] Can not construct value from args");
        return BaseType::emplace(Forward<Args>(args)...);
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    template<class ...Args>
    auto HashDict<Key, Value, HashType, KeyEqualType>::tryEmplace(const KeyType& key, Args && ...args) -> InsertResult
    {
        static_assert(TestIsConstructible<Value, Args...>, "[ntl.type_traits] Can not construct value from args");
        auto fp = BaseType:: findNodeWithKey(key);
        if (fp != nullptr)
        {
            return { fp, false };
        }
        auto np = BaseType::allocateNode();
        BaseType:: constructAtNodeAndSetHash(np, PIECEWISE, ForwardAsTuple(key), ForwardAsTuple(Forward<Args>(args)...));
        BaseType::insertNode(np);
        return { np, true };
    }
    template<class Key, class Value, class HashType, class KeyEqualType>
    template<class ...Args>
    auto HashDict<Key, Value, HashType, KeyEqualType>::tryEmplace(KeyType&& key, Args && ...args) -> InsertResult
    {
        static_assert(TestIsConstructible<Value, Args...>, "[ntl.type_traits] Can not construct value from args");
        auto fp = BaseType::findNodeWithKey(key);
        if (fp != nullptr)
        {
            return { fp, false };
        }
        auto np = BaseType::allocateNode();
        BaseType::constructAtNodeAndSetHash(np, PIECEWISE, ForwardAsTuple(Move(key)), ForwardAsTuple(Forward<Args>(args)...));
        BaseType::insertNode(np);
        return { np, true };
    }
    template<class Key, class Value, class HashType, class KeyEqualType>
    template<class M>
        requires TestIsAssignable<typename HashDict<Key, Value, HashType, KeyEqualType>::MappedType&, M&&>
    auto HashDict<Key, Value, HashType, KeyEqualType>::insertOrAssign(const KeyType& key, M&& value) -> InsertResult
    {
        auto fp = BaseType::findNodeWithKey(key);
        if (fp==nullptr)
        {
            auto np = BaseType::allocateNode();
            BaseType::constructAtNodeAndSetHash(np, key, Forward<M>(value));
            BaseType::insertNode(np);
            return { np, true };
        }
        else
        {
            fp->value.value = value;
            return { fp, false };
        }
    }
    template<class Key, class Value, class HashType, class KeyEqualType>
    template<class M>
        requires TestIsAssignable<typename HashDict<Key, Value, HashType, KeyEqualType>::MappedType&, M&&>
    auto HashDict<Key, Value, HashType, KeyEqualType>::insertOrAssign(KeyType&& key, M&& value) -> InsertResult
    {
        auto fp = BaseType::findNodeWithKey(key);
        if (fp == nullptr)
        {
            auto np = BaseType::allocateNode();
            BaseType::constructAtNodeAndSetHash(np, Move(key), Forward<M>(value));
            BaseType::insertNode(np);
            return { np, true };
        }
        else
        {
            fp->value.value = value;
            return { fp, false };
        }
    }
    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::find(const KeyType& key) -> Iterator
    {
        return BaseType::find(key);
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::find(const KeyType& key) const -> ConstIterator
    {
        return BaseType::find(key);
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    void HashDict<Key, Value, HashType, KeyEqualType>::clear()
    {
        BaseType::clear();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::size() const noexcept -> SizeType
    {
        return BaseType::size();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::insert(const ValueType& key_value) -> InsertResult
    {
        return tryEmplace(key_value.key, key_value.value);
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::insert(ValueType&& key_value) -> InsertResult
    {
        return tryEmplace(key_value.key, Move(key_value.value));
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::at(const KeyType& key) -> MappedType&
    {
        auto it = find(key);
        if (it != end())
        {
            return it->value;
        }
        else
        {
            throw OutOfRange{"[ntl.containers.hash_dict] Invalid access by key"};
        }
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::at(const KeyType& key) const -> const MappedType&
    {
        auto it = find(key);
        if (it != end())
        {
            return it->value;
        }
        else
        {
            throw OutOfRange{ "[ntl.containers.hash_dict] Invalid access by key" };
        }
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::begin() noexcept -> Iterator
    {
        return BaseType::begin();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::end() noexcept -> Iterator
    {
        return BaseType::end();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::begin() const noexcept -> ConstIterator
    {
        return BaseType::begin();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::end() const noexcept -> ConstIterator
    {
        return BaseType::end();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::cbegin() const noexcept -> ConstIterator
    {
        return BaseType::cbegin();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::cend() const noexcept -> ConstIterator
    {
        return BaseType::cend();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::remove(const KeyType& key) -> SizeType
    {
        auto np = BaseType::findNodeWithKey(key);
        if (np != nullptr) {
            BaseType::removeNode(np);
        }
        return size();
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::remove(Iterator iter) -> Iterator
    {
        return BaseType::remove(iter);
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::remove(ConstIterator iter) -> Iterator
    {
        return BaseType::remove(iter);
    }

    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::operator[](const KeyType& key)->MappedType& 
        requires TestIsDefaultConstructible<HashDict<Key, Value, HashType, KeyEqualType>::MappedType>
    {
        auto fp = BaseType::findNodeWithKey(key);
        if (fp == nullptr)
        {
            auto np = BaseType::allocateNode();
            BaseType::constructAtNodeAndSetHash(np, key, MappedType{});
            BaseType::insertNode(np);
            return np->value.value;
        }
        else
        {
            return fp->value.value;
        }
    }
    template<class Key, class Value, class HashType, class KeyEqualType>
    auto HashDict<Key, Value, HashType, KeyEqualType>::operator[](KeyType&& key)->MappedType& 
        requires TestIsDefaultConstructible<HashDict<Key, Value, HashType, KeyEqualType>::MappedType>
    {
        auto fp = BaseType::findNodeWithKey(key);
        if (fp == nullptr)
        {
            auto np = BaseType::allocateNode();
            BaseType::constructAtNodeAndSetHash(np, Move(key), MappedType{});
            BaseType::insertNode(np);
            return np->value.value;
        }
        else
        {
            return fp->value.value;
        }
    }
    
}
