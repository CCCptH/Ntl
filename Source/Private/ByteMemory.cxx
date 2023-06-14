module;
#include <cstring>
module ntl.string.byte_memory;
import ntl.utils;

namespace ne {
    class ByteMemory;
    void ByteMemory::init() {
        memset(raw, 0, LAYOUT_BYTES);
    }
    void ByteMemory::release() {
        if (usingLong() && long_layout.data != nullptr) {
            deallocate(long_layout.data);
        }
    }

    void ByteMemory::toLong() noexcept {
        short_layout.tag = 1;
    }
    void ByteMemory::toShort() noexcept {
        short_layout.tag = 0;
    }
    bool ByteMemory::usingLong() const noexcept {
        return short_layout.tag == 1;
    }
    bool ByteMemory::usingShort() const noexcept {
        return !usingLong();
    }

    auto ByteMemory::allocate(SizeType n) -> CharType* {
        return allocator.template allocate<CharType>(n);
    }
    void ByteMemory::deallocate(CharType* ptr) {
        allocator.deallocate(ptr);
    }

    void ByteMemory::setSize(SizeType n) {
    #ifdef NE_DEBUG
        if (n<0) throw "size must greater than 0";
    #endif
        if (usingShort()) short_layout.sz = n;
        else long_layout.sz = n;
    }

    ByteMemory::ByteMemory()
        : allocator()
    {
        init();
    }

    ByteMemory::ByteMemory(const Allocator& allocator)
        : allocator(allocator)
    {
        init();
    }

    ByteMemory::ByteMemory(SizeType n, const Allocator& allocator)
        : allocator()
    {
        init();
        if (n > LAYOUT_BYTES) {
            CharType* ptr = allocate(n);
            long_layout.cap = n;
        }
    }

    ByteMemory::ByteMemory(const CharType* str, SizeType len, const Allocator& allocator)
    {
        init();
        write(str, 0, len);
    }

    ByteMemory::~ByteMemory() {
        release();
    }

    ByteMemory& ByteMemory::operator=(const ByteMemory& other)
    {
        clear();
        if (other.usingShort()) {
            memcpy(raw, other.raw, LAYOUT_BYTES);
        }
        else {
            write(other.long_layout.data, 0, other.long_layout.sz);
        }
        return *this;
    }

    ByteMemory& ByteMemory::operator=(ByteMemory&& other)
    {
        clear();
        if (allocator == other.allocator) {
            memcpy(raw, other.raw, LAYOUT_BYTES);
            other.init();
        }
        else {
            if (other.usingShort()) {
                memcpy(raw, other.raw, LAYOUT_BYTES);
            }
            else {
                write(other.long_layout.data, 0, other.long_layout.sz);
            }
            other.init();
        }
        return *this;
    }


    ByteMemory::ByteMemory(const ByteMemory& other)
        : allocator(other.allocator)
    {
        if (other.usingShort()) {
            memcpy(raw, other.raw, LAYOUT_BYTES);
        }
        else {
            long_layout.sz = other.long_layout.sz;
            long_layout.cap = other.long_layout.cap;
            long_layout.data = allocate(long_layout.cap);
            memcpy(long_layout.data, other.long_layout.data, long_layout.sz);
        }
    }
    ByteMemory::ByteMemory(ByteMemory&& other) noexcept {
        memcpy(raw, other.raw, LAYOUT_BYTES);
        other.init();
    }

    auto ByteMemory::size() const noexcept -> SizeType {
        if (usingShort()) {
            return short_layout.sz;
        }
        else {
            return long_layout.sz;
        }
    }

    auto ByteMemory::capacity() const noexcept -> SizeType {
        if (usingShort()) {
            return SHORT_BYTES;
        }
        else {
            return long_layout.cap;
        }
    }

    auto ByteMemory::surplus() const noexcept -> SizeType {
        return capacity() - size();
    }

    bool ByteMemory::isEmpty() const noexcept
    {
        return size()==0;
    }

    void ByteMemory::swap(ByteMemory& other) {
        utils::Swap(allocator, other.allocator);
        utils::Swap(raw, other.raw);
    }

    void ByteMemory::clear() {
        release();
        init();
    }

    auto ByteMemory::data() const noexcept -> const CharType* {
        if (usingShort()) return &short_layout.data[0];
        else return long_layout.data;
    }

    auto ByteMemory::data() noexcept -> CharType* {
        if (usingShort()) return &short_layout.data[0];
        else return long_layout.data;
    }

    void ByteMemory::want(SizeType n) {
        auto old_cap = capacity();
        auto old_size = size();
        auto old_ptr = data();
        if (n > old_cap) {
            auto new_cap = old_cap;
            while (new_cap < n) {
                new_cap *=2;
            }
            auto new_ptr = allocate(new_cap);
            memcpy(new_ptr, old_ptr, old_size);
            if (usingLong()) deallocate(old_ptr);
            else toLong();
            toLong();
            long_layout.sz = old_size;
            long_layout.cap = new_cap;
            long_layout.data = new_ptr;
        }
    }

    void ByteMemory::reserve(SizeType n) {
        if (n < SHORT_BYTES) {
            if (n < short_layout.sz) {
                short_layout.sz = n;
            }
        }
        else {
            auto old_cap = capacity();
            auto old_size = size() < n ? size() : n;
            auto old_ptr = data();
            if (n > old_cap) {
                auto new_cap = n;
                auto new_ptr = allocate(new_cap);
                memcpy(new_ptr, old_ptr, old_size);
                if (usingLong()) deallocate(old_ptr);
                else toLong();
                long_layout.sz = old_size;
                long_layout.cap = new_cap;
                long_layout.data = new_ptr;
            }
        }
    }

    auto ByteMemory::at(SizeType n) const -> const CharType& {
        // TODO: throw
        // if (n >= size()) 
        return *(data() + n);
    }
    auto ByteMemory::at(SizeType n) -> CharType& {
        // TODO: throw
        // if (n >= size()) 
        return *(data() + n);
    }
    auto ByteMemory::operator[](SizeType n) const -> const CharType& {
        return *(data() + n);
    }
    auto ByteMemory::operator[](SizeType n) -> CharType& {
        return *(data() + n);
    }

    void ByteMemory::write(const CharType* str, SizeType pos, SizeType n) {
        auto sz = size();
    #ifdef NE_DEBUG
        if (n<0) throw "n < 0";
        if (pos>sz) throw "invalid pos";
    #endif
        // TODO: EXCEPTIONS
        // TODO: pos+sz may out of range
        pos = pos == sz ? pos : (pos+sz) % sz;
        if (pos > sz || pos < 0) {
            throw "out of range";
        }
        if (pos+n>=sz) {
            want(pos+n);
        }
        auto ptr = data();
        memcpy(ptr+pos, str, n);
        if (pos+n>sz) setSize(pos+n);
    }
    void ByteMemory::write(CharType ch, SizeType pos, SizeType n) {
        auto sz = size();
        // TODO: EXCEPTIONS
    #ifdef NE_DEBUG
        if (n<0) throw "n < 0";
        if (pos>sz) throw "invalid pos";
    #endif
        // TODO: EXCEPTIONS
        // TODO: pos+sz may out of range
        pos = pos == sz ? pos : (pos+sz) % sz;
        if (pos > sz || pos < 0) {
            throw "out of range";
        }
        if (pos+n>=sz) {
            want(pos+n);
        }
        auto ptr = data();
        // memcpy(data+pos, str, n);
        memset(ptr+pos, ch, n);
        if (pos+n>sz) setSize(pos+n);
    }

    void ByteMemory::remove(SizeType pos, SizeType n){
        auto sz = size();
    #ifdef NE_DEBUG
        if (n<0) throw "n < 0";
        if (pos>sz) throw "invalid pos";
    #endif
        // TODO: EXCEPTIONS
        // TODO: pos+sz may out of range
        pos = pos == sz ? pos : (pos+sz) % sz;
        if (pos + n > sz || n > sz) {
            throw "out of range";
        }
        auto ptr = data();
        for (auto i=n;i>0;i--) {
            *ptr = *(ptr+n);
            ++ptr;
        }
        setSize(sz-n);
        
    }
    void ByteMemory::replace(const CharType* str, SizeType len, SizeType pos, SizeType n) {
        auto old_sz = size();
        pos = pos == old_sz ? pos : (pos + old_sz) % old_sz;
    #ifdef NE_DEBUG
        if (n < 0) throw "invalid n<0";
        if (len < 0) throw "invalid len < 0";
        if (pos > sz) throw "invalid pos";
    #endif
        if (n > len) {
            auto ptr = data() + pos + len;
            auto diff = n - len;
            for (SizeType i = old_sz - pos - n; i < old_sz-diff; i++) {
                *ptr = *(ptr + diff);
                ++ptr;
            }
            memcpy(data() + pos, str, len);
            setSize(old_sz - diff);
        }
        else {
            auto diff = len - n;
            auto new_sz = old_sz + diff;
            want(new_sz);
            auto ptr = data() + diff + old_sz - 1;
            while (ptr != data() + pos + len - 1) {
                *ptr = *(ptr - diff);
                --ptr;
            }
            memcpy(data() + pos, str, len);
            setSize(new_sz);
        }
    }
    void ByteMemory::replace(CharType ch, SizeType count, SizeType pos, SizeType n) {
        auto old_sz = size();
        pos = pos == old_sz ? pos : (pos + old_sz) % old_sz;
#ifdef NE_DEBUG
        if (n < 0) throw "invalid n<0";
        if (len < 0) throw "invalid len < 0";
        if (pos > sz) throw "invalid pos";
#endif
        if (n > count) {
            auto ptr = data() + pos + count;
            auto diff = n - count;
            for (SizeType i = old_sz - pos - n; i < old_sz - diff; i++) {
                *ptr = *(ptr + diff);
                ++ptr;
            }
            memset(data() + pos, ch, count);
            setSize(old_sz - diff);
        }
        else {
            auto diff = count - n;
            auto new_sz = old_sz + diff;
            want(new_sz);
            auto ptr = data() + diff + old_sz - 1;
            while (ptr != data() + pos + count - 1) {
                *ptr = *(ptr - diff);
                --ptr;
            }
            memset(data() + pos, ch, count);
            setSize(new_sz);
        }
    }
}