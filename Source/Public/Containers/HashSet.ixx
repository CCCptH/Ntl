export module ntl.containers.hash_set;
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
		//using LocalIterator = HashTableIterator<ThisType>;
		//using ConstLocalIter = HashTableConstIter<ThisType>;
		using InsertResult = typename BaseType::InsertResult;
	};
};
