module;
#include <cstddef>
export module ntl.iterator.iterator_traits;
import ntl.type_traits;
import ntl.concepts.core_language;
import ntl.iterator.concepts.legacy_iterators;
export import ntl.iterator.incrementable_traits;
export import ntl.iterator.indirectly_readable_traits;
export import ntl.iterator.iterator_category;
import ntl.utils.declval;

export namespace ne
{
    
    template<ConceptReferenceable I>
    using TypeIteratorReference = decltype(*Declval<I&>());
    
    // Iterator traits
    template<class I>
    struct IteratorTraits;

    namespace details {
        template<class I>
        concept ConceptIterTraitsHasAllMember = requires {
            typename I::DifferenceType;
            typename I::ValueType;
            typename I::Reference;
            typename I::IteratorCategory;
            typename I::Pointer;
        };
    }

    template<class I>
        requires details::ConceptIterTraitsHasAllMember<I>
    struct IteratorTraits<I> {
        using DifferenceType = typename I::DifferenceType;
        using ValueType = typename I::ValueType;
        using Pointer = typename I::Pointer;
        using Reference = typename I::Reference;
        using IteratorCategory = typename I::IteratorCategory;
        using __Primary = IteratorTraits;
    };

    // 若 Iter 无 pointer ，但拥有全部其他四个成员类型，则按如下方式声明成员类型：
    template<class I>
        requires (not requires { typename I::Pointer; }) and
        requires {
            typename I::DifferenceType;
            typename I::ValueType;
            typename I::Reference;
            typename I::IteratorCategory;
        }
    struct IteratorTraits<I> {
        using DifferenceType = typename I::DifferenceType;
        using ValueType = typename I::ValueType;
        using Pointer = void;
        using Reference = typename I::Reference;
        using IteratorCategory = typename I::IteratorCategory;
        using __Primary = IteratorTraits;
    };

    // 若 Iter 无 pointer ，但拥有全部其他四个成员类型，则按如下方式声明成员类型：
    namespace details {
        template<class I>
        struct TypeGetIteratorCategoryHelper;

        template<class I>
        requires requires { typename I::IteratorCategory; }
        struct TypeGetIteratorCategoryHelper<I> {
            using Type = typename I::IteratorCategory;
        };

        template<ConceptLegacyRandomAccessIterator I>
        struct TypeGetIteratorCategoryHelper<I> {
            using Type = RandomAccessIteratorCategory;
        };

        template<ConceptLegacyBidirectionalIterator I>
        struct TypeGetIteratorCategoryHelper<I> {
            using Type = BidirectionalIteratorCategory;
        };

        template<ConceptLegacyForwardIterator I>
        struct TypeGetIteratorCategoryHelper<I> {
            using Type = ForwardIteratorCategory;
        };
    }

    template<ConceptLegacyInputIterator Iter>
    requires (not details::ConceptIterTraitsHasAllMember<Iter>)
    struct IteratorTraits<Iter>
    {
        using DifferenceType = typename IncrementableTraits<Iter>::DifferenceType;
        using ValueType = typename IndirectlyReadableTraits<Iter>::ValueType;
        using Pointer = TypeConditional<
                (requires { typename Iter::Pointer; }),
                TypeConditional<
                        (requires { Declval<Iter&>().operator->(); }),
                        decltype(Declval<Iter&>().operator->()),
                        void>,
                void
                >;
        using Reference = typename Iter::Reference;
        using IteratorCategory = TypeConditional<
                (requires { typename Iter::IteratorCategory; } ),
                typename Iter::IteratorCategory,
                typename details::TypeGetIteratorCategoryHelper<Iter>::Type
                >;
        using __Primary = IteratorTraits;
    };

    template<ConceptLegacyIterator I>
        requires (not details::ConceptIterTraitsHasAllMember<I>)
    struct IteratorTraits<I> {
        using DifferenceType = TypeConditional<
                (requires { typename IncrementableTraits<I>::Type; }),
                typename IncrementableTraits<I>::Type,
                void
                >;
        using ValueType = void;
        using Pointer = void;
        using Reference = void;
        using IteratorCategory = OutputIteratorCategory;
        using __Primary = IteratorTraits;
    };

    template<class Ptr>
        requires TestIsObject<Ptr>
    struct IteratorTraits<Ptr*>
    {
        using DifferenceType = std::ptrdiff_t;
        using ValueType = TypeUnCV<Ptr>;
        using Pointer = Ptr*;
        using Reference = Ptr&;
        using IteratorCategory = ContiguousIteratorCategory;
        using __Primary = IteratorTraits;
    };

    namespace details {
        // @Todo: maybe error

        template<class I>
        struct TypeIteratorValueHelper;

        template<class I>
        requires requires {
            typename IteratorTraits<TypeUnCVRef<I>>::ValueType;
            typename IteratorTraits<TypeUnCVRef<I>>::DifferenceType;
            typename IteratorTraits<TypeUnCVRef<I>>::Pointer;
            typename IteratorTraits<TypeUnCVRef<I>>::Reference;
            typename IteratorTraits<TypeUnCVRef<I>>::IteratorCategory;
        }
        struct TypeIteratorValueHelper<I> {
            using Type = typename IteratorTraits<TypeUnCVRef<I>>::ValueType;
        };

        template<class I>
        requires (not requires {
            typename IteratorTraits<TypeUnCVRef<I>>::ValueType;
            typename IteratorTraits<TypeUnCVRef<I>>::DifferenceType;
            typename IteratorTraits<TypeUnCVRef<I>>::Pointer;
            typename IteratorTraits<TypeUnCVRef<I>>::Reference;
            typename IteratorTraits<TypeUnCVRef<I>>::IteratorCategory;
        })
        and requires {
            typename IndirectlyReadableTraits<I>::ValueType;
        }
        struct TypeIteratorValueHelper<I> {
            using Type = typename IndirectlyReadableTraits<TypeUnCVRef<I>>::ValueType;
        };

        template<class I>
        struct TypeIteratorDifferenceHelper;

        template<class I>
        requires requires {
            typename IteratorTraits<TypeUnCVRef<I>>::__Primary;
        }
        struct TypeIteratorDifferenceHelper<I> {
            using Type = typename IteratorTraits<TypeUnCVRef<I>>::DifferenceType;
        };

        template<class I>
        requires requires {
            typename IncrementableTraits<TypeUnCVRef<I>>::DifferenceType;
        } and (not requires {
            typename IteratorTraits<TypeUnCVRef<I>>::DifferenceType;
        })
        struct TypeIteratorDifferenceHelper<I> {
            using Type = typename IncrementableTraits<TypeUnCVRef<I>>::DifferenceType;
        };
    }

    template<class Iter>
    using TypeIteratorValue = typename details::TypeIteratorValueHelper<Iter>::Type;

    template<class Iter>
    using TypeIteratorDifference = typename  details::TypeIteratorDifferenceHelper<Iter>::Type;

    template<class Iter>
    using TypeIteratorCommonRef = TypeCommonReference<TypeIteratorReference<Iter>, TypeIteratorValue<Iter>&>;
}