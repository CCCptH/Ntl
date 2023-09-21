export module ntl.containers.hash_set;
import ntl.containers.hash_table;
import ntl.containers.key_value;
import ntl.utils;
import ntl.tuple;
import ntl.functional.hash;
import ntl.type_traits;
import ntl.exceptions;
import ntl.iterator;
import ntl.memory.allocator;

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
	template<class Key, class HashType = Hash<Key>, class KeyEqualType=Eq<Key>>
	class HashSet
		: protected HashTable<const Key, DefaultIdentityExtractor, HashType, KeyEqualType, false>
	{
	public:
		using ValueType = Key;
		using ThisType = HashSet<Key, HashType, KeyEqualType>;
		using KeyType = Key;
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
		//using BaseType::BaseType;
		using Iterator = typename BaseType::Iterator;
		using ConstIterator = typename BaseType::ConstIterator;
		using InsertResult = typename BaseType::InsertResult;
		//using LocalIterator = HashTableIterator<ThisType>;
		//using ConstLocalIter = HashTableConstIter<ThisType>;

		using BaseType::BaseType;

		template<ConceptInputIterator It>
		HashSet(It first, It last, const Allocator& alloc = Allocator());
		template<ConceptInputIterator It>
		HashSet(It first, It last, const Hasher& hash, const Allocator& alloc = Allocator());
		template<ConceptInputIterator It>
		HashSet(It first, It last, const KeyEqual& key_equal, const Allocator& alloc = Allocator());
		template<ConceptInputIterator It>
		HashSet(It first, It last, const Hasher& hash, const KeyEqual& key_equal, const Allocator& alloc = Allocator());
		template<ConceptInputIterator It>
		HashSet(It first, It last, SizeType bucket_count, const Hasher& hash = Hasher(), const KeyEqual& key_equal = KeyEqual(), const Allocator& alloc = Allocator());

		HashSet(InitList<ValueType> init);
		HashSet(InitList<ValueType> init, const Allocator& allocator);
		HashSet(InitList<ValueType> init, SizeType bucket_count, const Hasher& hash = Hasher(), const KeyEqual& key_equal = KeyEqual(), const Allocator& alloc = Allocator());
		HashSet(InitList<ValueType> init, SizeType bucket_count, const Hasher& hash = Hasher(), const Allocator& alloc = Allocator());
		HashSet(InitList<ValueType> init, SizeType bucket_count, const Allocator& alloc = Allocator());

		~HashSet() = default;
	
		/**
				 * @brief Emplace key value with args.
				 * @tparam ...Args
				 * @param ...args
				 * @return If key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
				*/
		template<class ...Args>
		auto emplace(Args&&...args) -> InsertResult;

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
		 * @brief Insert key if key does not exist.
		 * @param key
		 * @return If key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
		*/
		auto insert(const ValueType& key) -> InsertResult;
		/**
		 * @brief Insert key if key does not exist.
		 * @param key
		 * @returnIf key is duplicated, return {end(), false}, else return {it, true} where it points to inserted node.
		*/
		auto insert(ValueType&& key) -> InsertResult;

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
		 * @brief Remove all items;
		*/
		void clear();

		SizeType count(const Key& key) const noexcept;
		bool contains(const Key& key) const noexcept;

		void swap(ThisType& other);
		friend void Swap(ThisType& a, ThisType& b);

		/**
		 * @brief Return count of items;
		*/
		auto size() const noexcept -> SizeType;

		auto begin() noexcept -> Iterator { return BaseType::begin(); }
		auto end() noexcept -> Iterator { return BaseType::end(); }
		auto begin() const noexcept -> ConstIterator { return BaseType::begin(); }
		auto end() const noexcept -> ConstIterator { return BaseType::end(); }
		auto cbegin() const noexcept -> ConstIterator { return BaseType::cbegin(); }
		auto cend() const noexcept -> ConstIterator { return BaseType::cend(); }
	};

};

namespace ne
{
	template<class Key, class HashType, class KeyEqualType>
	template<ConceptInputIterator It>
	HashSet<Key, HashType, KeyEqualType>::HashSet(It first, It last, const Allocator& alloc)
		: HashSet(first, last, (ConceptRandomAccessIterator<It> ? BaseType::SelectPrime(last - first) : BaseType::SelectPrime(2)), Hasher(), KeyEqualType(), alloc)
	{}

	
	template<class Key, class HashType, class KeyEqualType>
	template<ConceptInputIterator It>
	HashSet<Key, HashType, KeyEqualType>::HashSet(It first, It last, const Hasher& hash, const Allocator& alloc)
		: HashSet(first, last, (ConceptRandomAccessIterator<It> ? BaseType::SelectPrime(last - first) : BaseType::SelectPrime(2)), hash, KeyEqualType(), alloc)
	{}

	template<class Key, class HashType, class KeyEqualType>
	template<ConceptInputIterator It>
	HashSet<Key, HashType, KeyEqualType>::HashSet(It first, It last, const KeyEqual& key_equal, const Allocator& alloc)
		: HashSet(first, last, (ConceptRandomAccessIterator<It> ? BaseType::SelectPrime(last - first) : BaseType::SelectPrime(2)), HashType(), key_equal, alloc)
	{}

	template<class Key, class HashType, class KeyEqualType>
	template<ConceptInputIterator It>
	HashSet<Key, HashType, KeyEqualType>::HashSet(It first, It last, const Hasher& hash, const KeyEqual& key_equal, const Allocator& alloc)
		: HashSet(first, last, (ConceptRandomAccessIterator<It> ? BaseType::SelectPrime(last - first) : BaseType::SelectPrime(2)), hash, key_equal, alloc)
	{}

	template<class Key, class HashType, class KeyEqualType>
	template<ConceptInputIterator It>
	HashSet<Key, HashType, KeyEqualType>::HashSet(It first, It last, SizeType bucket_count, const Hasher& hash, const KeyEqual& key_equal, const Allocator& alloc)
		: BaseType(bucket_count, hash, key_equal, alloc)
	{
		for (; first != last; ++first)
		{
			if (!emplace(*first))
			{
				clear();
				throw KeyDuplicated("[ntl.containers.hash_dict] Key duplicated");
			}
		}
	}

	template<class Key, class HashType, class KeyEqualType>
	HashSet<Key, HashType, KeyEqualType>::HashSet(InitList<ValueType> init)
		: HashSet(init, BaseType::SelectPrime(init.size()), Hasher(), KeyEqual(), Allocator())
	{}
	template<class Key, class HashType, class KeyEqualType>
	HashSet<Key, HashType, KeyEqualType>::HashSet(InitList<ValueType> init, const Allocator& allocator)
		: HashSet(init, BaseType::SelectPrime(init.size()), Hasher(), KeyEqual(), allocator)
	{}
	template<class Key, class HashType, class KeyEqualType>
	HashSet<Key, HashType, KeyEqualType>::HashSet(InitList<ValueType> init, SizeType bucket_count, const Hasher& hash, const KeyEqual& key_equal, const Allocator& alloc)
		: BaseType(bucket_count, hash, key_equal, alloc)
	{
		for (auto it = init.begin(); it != init.end(); ++it) {
			static_assert(TestIsConvertible<decltype(*it), Key>, "[ntl.containers.hash_dict] Cannot convert into target type.");
			if (!emplace(*it)) {
				clear();
				throw KeyDuplicated("[ntl.containers.hash_dict] Key duplicated");
			}
		}
	}
	template<class Key, class HashType, class KeyEqualType>
	HashSet<Key, HashType, KeyEqualType>::HashSet(InitList<ValueType> init, SizeType bucket_count, const Hasher& hash, const Allocator& alloc)
		: HashSet(init, bucket_count, hash, KeyEqual(), alloc)
	{}
	template<class Key, class HashType, class KeyEqualType>
	HashSet<Key, HashType, KeyEqualType>::HashSet(InitList<ValueType> init, SizeType bucket_count, const Allocator& alloc) 
		: HashSet(init, bucket_count, Hasher(), KeyEqual(), alloc)
	{}

	template<class Key, class HashType, class KeyEqualType>
		template<class ...Args>
	auto HashSet<Key, HashType, KeyEqualType>::emplace(Args&&...args) -> InsertResult {
		static_assert(TestIsConstructible<ValueType, Args...>, "[ntl.type_traits] Can not construct value from args");
		return BaseType::emplace(Forward<Args>(args)...);
	}

	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::find(const Key& key) -> Iterator
	{
		return BaseType::find(Key);
	}

	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::find(const Key& key) const -> ConstIterator
	{
		return BaseType::find(Key);
	}

	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::insert(const ValueType& key) -> InsertResult
	{
		return emplace(key);
	}

	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::insert(ValueType&& key) -> InsertResult
	{
		return emplace(Move(key));
	}

	template<class Key, class HashType, class KeyEqualType>
	auto ne::HashSet<Key, HashType, KeyEqualType>::remove(const KeyType& key) -> SizeType
	{
		auto np = BaseType::findNodeWithKey(key);
		if (np != nullptr) {
			BaseType::removeNode(np);
			return 1;
		}
		return 0;
	}
	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::remove(Iterator pos) -> Iterator
	{
		return BaseType::remove(pos);
	}
	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::remove(ConstIterator pos) -> Iterator
	{
		return BaseType::remove(pos);
	}
	template<class Key, class HashType, class KeyEqualType>
	void HashSet<Key, HashType, KeyEqualType>::clear()
	{
		return BaseType::clear();
	}
	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::count(const Key& key) const noexcept -> SizeType
	{
		return find(key) != end() ? 1 : 0;
	}

	template<class Key, class HashType, class KeyEqualType>
	bool HashSet<Key, HashType, KeyEqualType>::contains(const Key& key) const noexcept
	{
		return find(key)!=end();
	}

	template<class Key, class HashType, class KeyEqualType>
	void HashSet<Key, HashType, KeyEqualType>::swap(ThisType& other)
	{
		BaseType::swap(other);
	}

	template<class Key, class HashType, class KeyEqualType>
	void Swap(HashSet<Key, HashType, KeyEqualType>& a, HashSet<Key, HashType, KeyEqualType>& b)
	{
		a.swap(b);
	}

	template<class Key, class HashType, class KeyEqualType>
	auto HashSet<Key, HashType, KeyEqualType>::size() const noexcept -> SizeType
	{
		return BaseType::size();
	}

}