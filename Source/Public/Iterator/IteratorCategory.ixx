export module ntl.iterator.iterator_category;

export namespace ne {
    struct InputIteratorCategory{};
    struct OutputIteratorCategory{};
    struct ForwardIteratorCategory:  InputIteratorCategory{};
    struct BidirectionalIteratorCategory: ForwardIteratorCategory{};
    struct RandomAccessIteratorCategory: BidirectionalIteratorCategory{};
    struct ContinguousIteratorCategory: RandomAccessIteratorCategory{};
}