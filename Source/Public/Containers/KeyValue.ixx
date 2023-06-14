module;
#include <tuple>
export module ntl.containers.key_value;
export import ntl.utils.piecewise;
import ntl.utils;
import ntl.type_traits;
import ntl.tuple;

export namespace ne
{
    template<template<class>class Extractor, class KV>
    concept ConceptKVExtractor = requires (Extractor<KV> extract,const KV& ckv, KV& kv) {
        typename Extractor<KV>::KeyType;
        typename Extractor<KV>::ValueType;
        Extractor<KV>::HAS_VALUE;
        extract.getValue(ckv);
        extract.getKey(ckv);
        extract.getKeyAddr(kv);
    }
        and ((Extractor<KV>::HAS_VALUE==true and requires (Extractor<KV> extract, KV& kv) {
            extract.refValue(kv);
            extract.getValueAddr(kv);
        }) or (Extractor<KV>::HAS_VALUE==false))
    ;

    template<class T>
    struct DefaultKeyValueExtractor
    {
        using KeyType = typename T::KeyType;
        using ValueType = typename T::ValueType;

        constexpr static bool HAS_VALUE = true;

        [[nodiscard]] constexpr
        const KeyType& getKey(const T& kv) const {
            return kv.key;
        }
        [[nodiscard]] constexpr
        ValueType& refValue(T& kv) const {
            return kv.value;
        }

        [[nodiscard]] constexpr
        const ValueType& getValue(const T& kv) const {
            return kv.value;
        }

        [[nodiscard]] constexpr
        KeyType* getKeyAddr(T& kv) const {
            return const_cast<KeyType*>(GetAddress(kv.key));
        }
        [[nodiscard]]
        ValueType* getValueAddr(T& kv) const {
            return GetAddress(kv.value);
        }
    };

    template<class T>
    struct DefaultIdentityExtractor
    {
        using KeyType = T;
        using ValueType = T;
        constexpr static bool HAS_VALUE = false;
        [[nodiscard]] constexpr
        const KeyType& getKey(const T& x) const {
            return x;
        }
        [[nodiscard]] constexpr
        const ValueType& getValue(const T& x) const {
            return x;
        }

        [[nodiscard]] constexpr
        KeyType* getKeyAddr(T& kv) const {
            return (GetAddress(kv.key));
        }
    };

    // TODO
    template<class TheKeyType, class TheValueType>
    struct KeyValue
    {
    private:
        template<class ...Ks, class...Vs, size_t ...Iks, size_t ...Ivs>
        constexpr KeyValue(Tuple<Ks...>& k_tuple, Tuple<Vs...>&v_tuple, IndexSequence<Iks...>, IndexSequence<Ivs...>)
            : key(Get<Iks>(Move(k_tuple))...)
			, value(Get<Ivs>(Move(v_tuple))...)
    	{}
    public:
        using KeyType = TheKeyType;
        using ValueType = TheValueType;

        const KeyType key;
        ValueType value;

        constexpr
        //explicit(not (TestIsImplicitlyConstructible<KeyType> and TestIsImplicitlyConstructible<ValueType>))
    	KeyValue()
            requires TestIsDefaultConstructible<KeyType> and TestIsDefaultConstructible<ValueType>
    		: key()
			, value()
        {}

        constexpr
        explicit (not (TestIsConvertible<const KeyType&, KeyType> and TestIsConvertible<const ValueType&, ValueType>))
        KeyValue(const KeyType&  k, const ValueType& v)
			requires TestIsCopyConstructible<KeyType> and TestIsCopyConstructible<ValueType>
	        : key(k), value(v)
        {}

        template<class K, class V>
        constexpr
    	explicit (not (TestIsConvertible<K, KeyType> and TestIsConvertible<V, ValueType>))
            KeyValue(K&& k, V&& v)
            requires TestIsConstructible<KeyType, K> and TestIsConstructible<ValueType, V>
        : key(Forward<K>(k)), value(Forward<V>(v))
        {}

        template<class ...Ks, class...Vs>
        KeyValue(PieceWise, Tuple<Ks...> k_tuple, Tuple<Vs...>v_tuple)
            : KeyValue(k_tuple, v_tuple, MakeIndexSequence<sizeof...(Ks)>{}, MakeIndexSequence<sizeof...(Vs)>{})
    	{}

    	constexpr KeyValue(const KeyValue&) = default;
    	constexpr KeyValue(KeyValue&&) = default;
    };
}