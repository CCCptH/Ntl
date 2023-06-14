export module ntl.iterator.iterator_category;

export namespace ne {
    struct InputIteratorCategory{};
    struct OutputIteratorCategory{};
    struct ForwardIteratorCategory: public InputIteratorCategory{};
    struct BidirectionalIteratorCategory: public ForwardIteratorCategory{};
    struct RandomAccessIteratorCategory: public BidirectionalIteratorCategory{};
    struct ContiguousIteratorCategory: public RandomAccessIteratorCategory{};
}