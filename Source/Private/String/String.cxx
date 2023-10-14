module;
#include <charconv>
#include <string>
#include <cstring>
module ntl.string.string;


namespace ne
{
    inline void InitLayout(StringLayout& s) noexcept
    {
        std::memset(s.layout.raw, 0, StringLayout::LAYOUT_BYTES);
    }

    inline bool IsSmallLayout(const StringLayout& s) noexcept
    {
        return s.layout.small.tag == 0;
    }
    inline bool IsLargeLayout(const StringLayout& s)  noexcept
    {
        return !IsSmallLayout(s);
    }

    inline void ReleaseLayout(StringLayout& s, Allocator& allocator)
    {
        if (IsLargeLayout(s))
        {
            allocator.deallocate(s.layout.large.data);
            s.layout.large.data = nullptr;
        }
    }

    inline void ToLargeLayout(StringLayout& s) noexcept
    {
        s.layout.large.tag = 1;
    }
    inline void ToShortLayout(StringLayout& s) noexcept
    {
        s.layout.small.tag = 0;
    }

    template<class S>
        requires TestIsSame<StringLayout, TypeUnCVRef<S>>
    auto& LayoutAccessLargeAt(S&& s, int64 i) noexcept
    {

        return	Forward<S>(s).layout.large.data[i];
    }
    template<class S>
        requires TestIsSame<StringLayout, TypeUnCVRef<S>>
    auto& LayoutAccessSmallAt(S&& s, int64 i) noexcept
    {

        return	Forward<S>(s).layout.small.data[i];
    }

    template<class S>
        requires TestIsSame<StringLayout, TypeUnCVRef<S>>
    auto& LayoutAccessAt(S&& s, int64 i) noexcept
    {
        if (IsSmallLayout(s))
        {
            return	Forward<S>(s).layout.small.data[i];
        }
        else
        {
            return	Forward<S>(s).layout.large.data[i];
        }
    }

    inline StringLayout::SizeType LayoutSize(const StringLayout& s) noexcept
    {
        if (IsSmallLayout(s)) { return s.layout.small.sz; }
        else { return s.layout.large.sz; }
    }

    inline StringLayout::SizeType LayoutCapacity(const StringLayout& s) noexcept
    {
        if (IsSmallLayout(s)) return StringLayout::SHORT_BYTES;
        else return s.layout.large.cap;
    }

    inline StringLayout::CharType* LayoutData(StringLayout& s) noexcept
    {
        if (IsSmallLayout(s)) return s.layout.small.data;
        else return s.layout.large.data;
    }
    inline const StringLayout::CharType* LayoutData(const StringLayout& s) noexcept
    {
        if (IsSmallLayout(s)) return s.layout.small.data;
        else return s.layout.large.data;
    }
    inline StringLayout::SizeType LayoutSurplus(const StringLayout& s) noexcept
    {
        return LayoutCapacity(s) - LayoutSize(s);
    }

    // Clear Data, Do not release space
    inline void ClearLayout(StringLayout& s)
    {
        if (IsSmallLayout(s))
        {
            InitLayout(s);
        }
        else
        {
            s.layout.large.sz = 0;
        }
    }

    inline void LayoutAlloc(StringLayout& s, uint64 n, Allocator& allocator)
    {
        s.layout.large.data = allocator.allocate<StringLayout::CharType>(n);
        s.layout.large.cap = n;
    }

    inline void InitLayout(StringLayout& s, size_t cap, Allocator& allocator)
    {
        ++cap;
        InitLayout(s);
        if (cap > StringLayout::SHORT_BYTES)
        {
            s.layout.large.tag = 1;
            LayoutAlloc(s, cap, allocator);
        }
    }

    inline void SetLayoutSize(StringLayout& s, StringLayout::SizeType n)
    {
        if (IsSmallLayout(s)) s.layout.small.sz = n;
        else s.layout.large.sz = n;
    }

    inline void TryExpandLayout(StringLayout& s, uint64 n, Allocator& allocator)
    {
        auto old_cap = LayoutCapacity(s);
        auto old_size = LayoutSize(s);
        auto old_ptr = LayoutData(s);
        // Space for '\0'
        ++n;
        if (n > old_cap) {
            auto new_cap = old_cap;
            while (new_cap < n) {
                new_cap *= 2;
            }
            auto new_ptr = allocator.allocate<StringLayout::CharType>(new_cap);
            memcpy(new_ptr, old_ptr, old_size);
            if (IsLargeLayout(s)) allocator.deallocate(old_ptr);
            else ToLargeLayout(s);
            s.layout.large.sz = old_size;
            s.layout.large.cap = new_cap;
            s.layout.large.data = new_ptr;
        }
    }

    inline void LayoutWrite(StringLayout& s, const char* str, StringLayout::SizeType pos, StringLayout::SizeType n, Allocator& allocator)
    {
        auto sz = LayoutSize(s);
        if (pos + n >= sz)
        {
            TryExpandLayout(s, pos + n, allocator);
        }
        auto ptr = LayoutData(s);
        std::memcpy(ptr + pos, str, n);
        if (pos + n > sz) SetLayoutSize(s, pos + n);
    }

    inline void LayoutWrite(StringLayout& s, String::CharType ch, StringLayout::SizeType pos, StringLayout::SizeType n, Allocator& allocator)
    {
        auto sz = LayoutSize(s);
        if (pos + n >= sz)
        {
            TryExpandLayout(s, pos + n, allocator);
        }
        auto ptr = LayoutData(s);
        std::memset(ptr + pos, ch, n);
        if (pos + n > sz) SetLayoutSize(s, pos + n);
    }

    inline void LayoutWrite(StringLayout& s, const char* str, StringLayout::SizeType pos, StringLayout::SizeType n)
    {
        auto sz = LayoutSize(s);
        auto ptr = LayoutData(s);
        std::memcpy(ptr + pos, str, n);
        if (pos + n > sz) SetLayoutSize(s, pos + n);
    }

    inline void LayoutWrite(StringLayout& s, String::CharType ch, StringLayout::SizeType pos, StringLayout::SizeType n)
    {
        auto sz = LayoutSize(s);
        auto ptr = LayoutData(s);
        std::memset(ptr + pos, ch, n);
        if (pos + n > sz) SetLayoutSize(s, pos + n);
    }

    inline void LayoutReserve(StringLayout& s, String::SizeType n, Allocator& allocator)
    {
        auto old_size = LayoutSize(s);
        auto old_ptr = LayoutData(s);
        // one more space for '\0'
        ++n;
        auto using_small = IsSmallLayout(s);
        if (n > LayoutCapacity(s)) {
                LayoutAlloc(s, n, allocator);
                auto new_sz = old_size;
                LayoutWrite(s, old_ptr, 0, new_sz);
                s.layout.large.sz = new_sz;
                if (!using_small) {
                    allocator.deallocate(old_ptr);
                }
        }
    }

    inline void LayoutSqueeze(StringLayout& s, Allocator& alloc) {
        auto sz = LayoutSize(s);
        if (sz < StringLayout::SHORT_BYTES) {
            if (IsLargeLayout(s)) {
                auto ptr = LayoutData(s);
                ToShortLayout(s);
                LayoutWrite(s, ptr, 0, sz);
                alloc.deallocate(ptr);
            }
        }
        else {
            auto ptr = LayoutData(s);
            LayoutAlloc(s, sz, alloc);
            LayoutWrite(s, ptr, 0, sz);
            alloc.deallocate(s);
        }
    }

    inline void LayoutReplace(StringLayout& s, const char* str, StringLayout::SizeType len, StringLayout::SizeType pos, StringLayout::SizeType n, Allocator allocator)
    {
        auto old_sz = LayoutSize(s);
        auto data = LayoutData(s);
        if (n > len) {
            auto ptr = data + pos + len;
            auto diff = n - len;
            for (String::SizeType i = old_sz - pos - n; i < old_sz - diff; i++) {
                *ptr = *(ptr + diff);
                ++ptr;
            }
            memcpy(data + pos, str, len);
            SetLayoutSize(s, old_sz - diff);
        }
        else {
            auto diff = len - n;
            auto new_sz = old_sz + diff;
            TryExpandLayout(s, new_sz, allocator);
            auto data = LayoutData(s);
            auto ptr = data + diff + old_sz - 1;
            while (ptr != data + pos + len - 1) {
                *ptr = *(ptr - diff);
                --ptr;
            }
            memcpy(data + pos, str, len);
            SetLayoutSize(s, new_sz);
        }
    }

    inline void LayoutReplace(StringLayout& s, char ch, StringLayout::SizeType count, StringLayout::SizeType pos, StringLayout::SizeType n, Allocator allocator)
    {
        auto old_sz = LayoutSize(s);
        auto data = LayoutData(s);
        if (n > count) {
            auto ptr = data + pos + count;
            auto diff = n - count;
            for (StringLayout::SizeType i = old_sz - pos - n; i < old_sz - diff; i++) {
                *ptr = *(ptr + diff);
                ++ptr;
            }
            auto data = LayoutData(s);
            memset(data + pos, ch, count);
            SetLayoutSize(s, old_sz - diff);
        }
        else {
            auto diff = count - n;
            auto new_sz = old_sz + diff;
            TryExpandLayout(s, new_sz, allocator);
            auto data = LayoutData(s);
            auto ptr = data + diff + old_sz - 1;
            while (ptr != data + pos + count - 1) {
                *ptr = *(ptr - diff);
                --ptr;
            }
            memset(data + pos, ch, count);
            SetLayoutSize(s, new_sz);
        }
    }

    // TODO: TO SMALL LAYOUT WHEN REMOVE TOO MUCH CHARACTOR
    inline void LayoutRemove(StringLayout& s, StringLayout::SizeType pos, StringLayout::SizeType n, Allocator& alloc)
    {
        auto sz = LayoutSize(s);
        auto ptr = LayoutData(s);
        for (auto i = n; i > 0; i--) {
            *ptr = *(ptr + n);
            ++ptr;
        }
        SetLayoutSize(s, sz - n);
        if (LayoutSize(s) < StringLayout::SHORT_BYTES) {
            LayoutSqueeze(s, alloc);
        }
    }

    inline void LayoutShallowCopy(StringLayout& dst, const StringLayout& from)
    {
        std::memcpy(dst.layout.raw, from.layout.raw, StringLayout::LAYOUT_BYTES);
    }

    inline void LayoutDeepCopy(StringLayout& dst, const StringLayout& from, Allocator& allocator)
    {
        LayoutWrite(dst, LayoutData(from), 0, LayoutSize(from), allocator);
    }

    void String::setEndTag(SizeType pos)
    {
        LayoutWrite(this->layout, '\0', pos, 1);
    }
    void String::setEndTag()
    {
        setEndTag(LayoutSize(layout));
    }

    String::String()
        : allocator()
    {
        InitLayout(this->layout);
    }

    String::String(const Allocator& allocator)
        : allocator(allocator)
    {
        InitLayout(this->layout);
    }

    String::String(SizeType count, CharType ch)
        : allocator()
    {
        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            InitLayout(this->layout, count, this->allocator);
            LayoutWrite(this->layout, ch, 0, count);
            setEndTag();
    }

    String::String(SizeType count, CharType ch, const Allocator& allocator)
        : allocator(allocator)
    {
        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            InitLayout(this->layout, count, this->allocator);
            LayoutWrite(this->layout, ch, 0, count);
            setEndTag();
    }

    String::String(const String& str)
        : allocator(str.allocator)
    {
        InitLayout(this->layout);
        LayoutDeepCopy(this->layout, str.layout, this->allocator);
    }

    String::String(const String& str, SizeType count)
        : allocator(str.allocator)
    {
        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            auto n = str.size() < count ? str.size() : count;
            InitLayout(this->layout, n, this->allocator);
            LayoutWrite(this->layout, LayoutData(str.layout), 0, n);
            setEndTag();
    }

    String::String(const String& str, SizeType count, const Allocator& allocator)
        : allocator(allocator)
    {
        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            auto n = str.size() < count ? str.size() : count;
            InitLayout(this->layout, n, this->allocator);
            LayoutWrite(this->layout, LayoutData(str.layout), 0, n);
            setEndTag();
    }

    String::String(const char* str)
        : allocator()
    {
        auto n = std::strlen(str);
        InitLayout(this->layout, n, this->allocator);
        LayoutWrite(this->layout, str, 0, n + 1);
    }

    String::String(const char* str, SizeType count)
        : allocator()
    {
        //auto sz = std::strlen(str);
        //sz = sz < count ? sz : count;
        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            auto sz = count;
            InitLayout(this->layout, sz, this->allocator);
            LayoutWrite(this->layout, str, 0, sz, allocator);
            setEndTag();
    }

    String::String(const char* str, const Allocator& allocator)
        : allocator(allocator)
    {
        auto n = std::strlen(str);
        InitLayout(this->layout, n, this->allocator);
        LayoutWrite(this->layout, str, 0, n + 1);
    }

    String::String(const char* str, SizeType count, const Allocator& allocator)
        : allocator(allocator)
    {
        //auto sz = std::strlen(str);
        //sz = sz < count ? sz : count;
        // str like "asda\0asdad"
        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            auto sz = count;
            InitLayout(this->layout, sz, this->allocator);
            LayoutWrite(this->layout, str, 0, sz);
            setEndTag();
    }

    String::String(String&& str) noexcept
        : allocator(Move(str.allocator))
    {
        LayoutShallowCopy(this->layout, str.layout);
        InitLayout(str.layout);
    }
    String::~String()
    {
        ReleaseLayout(this->layout, allocator);
    }

    String& String::assign(SizeType n, char ch)
    {
        if (n < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            clear();
            LayoutWrite(this->layout, ch, 0, n, allocator);
            setEndTag();
            return *this;
    }

    String& String::assign(const String& str)
    {
        clear();
        LayoutWrite(this->layout, str.cstr(), 0, str.size(), allocator);
        setEndTag();
        return *this;
    }
    String& String::assign(const String& str, SizeType count)
    {

        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            clear();
            auto n = count < str.size() ? count : str.size();
            LayoutWrite(this->layout, str.cstr(), 0, n, allocator);
            setEndTag();
            return *this;
    }
    String& String::assign(String&& str)
    {
        clear();
        if (allocator == str.allocator)
        {
            LayoutShallowCopy(this->layout, str.layout);
            InitLayout(str.layout);
        }
        else
        {
            LayoutDeepCopy(this->layout, str.layout, allocator);
            ReleaseLayout(str.layout, str.allocator);
            setEndTag();
        }
        return *this;
    }
    String& String::assign(const char* str)
    {
        clear();
        auto n = std::strlen(str);
        LayoutWrite(this->layout, str, 0, n + 1, allocator);
        return *this;
    }

    String& String::assign(const char* str, SizeType count)
    {
        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            clear();
            auto n = count;
            LayoutWrite(this->layout, str, 0, n, allocator);
            setEndTag();
            return *this;
    }

    String& String::operator=(const String& str)
    {
        return assign(str);
    }

    String& String::operator=(const char* str)
    {
        return assign(str);
    }

    String& String::operator=(String&& str)
    {
        return assign(Move(str));
    }
    auto operator<=>(const String& lhs, const String& rhs) noexcept -> String::OrderType
    {
        return lhs.compare(rhs);
    }


    auto operator<=>(const String& lhs, const char* rhs) noexcept -> String::OrderType
    {
        return lhs.compare(rhs);
    }

    bool operator==(const String& lhs, const String& rhs)  noexcept
    {
        if (lhs.size() != rhs.size()) return false;
        return lhs.compare(rhs) == 0;
    }


    bool operator==(const String& lhs, const char* rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    auto String::compare(const String& str) const noexcept -> OrderType
    {
        auto res = std::strcmp(cstr(), str.cstr());
        if (res == -1) return OrderType::less;
        else if (res == 0) return OrderType::equivalent;
        else return OrderType::greater;
    }

    auto String::compare(const char* str) const noexcept -> OrderType
    {
        auto res = std::strcmp(cstr(), str);
        if (res == -1) return OrderType::less;
        else if (res == 0) return OrderType::equivalent;
        else return OrderType::greater;
    }

    String::SizeType String::size() const noexcept
    {
        auto sz = LayoutSize(layout) - 1;
        return sz > 0 ? sz : 0;
    }
    String::SizeType String::capacity() const noexcept
    {
        return LayoutCapacity(layout) - 1;
    }

    bool String::isEmpty() const noexcept
    {
        return size() == 0;
    }
    bool String::isNull() const noexcept
    {
        return LayoutSize(layout) == 0;
    }

    void String::reserve(SizeType n)
    {
        LayoutReserve(this->layout, n, allocator);
    }

    String::Reference String::at(SizeType i)
    {
        if (i >= size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Index is out of range." };
            }
            return LayoutData(layout)[i];
    }
    String::ConstReference String::at(SizeType i) const
    {
        if (i >= size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Index is out of range." };
            }
            return LayoutData(layout)[i];
    }
    String::Reference String::operator[](SizeType i) noexcept
    {
        return LayoutData(layout)[i];
    }
    String::ConstReference String::operator[](SizeType i) const noexcept
    {
        return LayoutData(layout)[i];
    }

    const char* String::cstr() const noexcept
    {
        return LayoutData(layout);
    }

    auto String::begin() noexcept -> Iterator
    {
        auto ptr = LayoutData(layout);
        return ptr;
    }
    auto String::end() noexcept -> Iterator
    {
        auto ptr = LayoutData(layout);
        return ptr + size();
    }
    auto String::begin() const noexcept -> ConstIterator
    {
        auto ptr = LayoutData(layout);
        return ptr;
    }
    auto String::end() const noexcept -> ConstIterator
    {
        auto ptr = LayoutData(layout);
        return ptr + size();
    }
    auto String::cbegin() const noexcept -> ConstIterator
    {
        return begin();
    }
    auto String::cend() const noexcept -> ConstIterator
    {
        return end();
    }
    auto String::rbegin() noexcept -> ReverseIterator
    {
        return ReverseIterator(end());
    }
    auto String::rend() noexcept -> ReverseIterator
    {
        return ReverseIterator(begin());
    }
    auto String::rbegin() const noexcept -> ConstReverseIterator
    {
        return ConstReverseIterator(end());
    }
    auto String::rend() const noexcept -> ConstReverseIterator
    {
        return ConstReverseIterator(begin());
    }
    auto String::crbegin() const noexcept -> ConstReverseIterator
    {
        return rbegin();
    }
    auto String::crend() const noexcept -> ConstReverseIterator
    {
        return rend();
    }

    void String::clear()
    {
        ClearLayout(layout);
    }

    void String::squeeze()
    {
        LayoutSqueeze(layout, allocator);
    }

    String& String::append(const String& str)
    {
        auto ptr = LayoutData(str.layout);
        LayoutWrite(layout, ptr, size(), str.size(), allocator);
        setEndTag();
        return *this;
    }

    String& String::append(CharType ch)
    {
        LayoutWrite(layout, ch, size(), 1, allocator);
        setEndTag();
        return *this;
    }

    String& String::append(const char* str)
    {
        auto sz = std::strlen(str);
        LayoutWrite(layout, str, size(), sz, allocator);
        setEndTag();
        return *this;
    }

    String& String::append(const char* str, SizeType len)
    {
        /*auto sz = std::strlen(str);
        sz = len < sz ? len : sz;*/

        if (len < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, len can not be less than 0." };
            }
            auto sz = len;
            LayoutWrite(layout, str, size(), sz, allocator);
            setEndTag();
            return *this;
    }

    String& String::operator+=(const String& str)
    {
        return append(str);
    }

    String& String::operator+=(CharType ch)
    {
        return append(ch);
    }

    String& String::operator+=(const char* str)
    {
        return append(str);
    }

    String& String::prepend(const String& str)
    {
        auto ptr = str.cstr();
        LayoutReplace(layout, ptr, 0, 0, str.size(), allocator);
        setEndTag();
        return *this;
    }

    String& String::prepend(CharType ch)
    {
        LayoutReplace(layout, ch, 0, 0, 1, allocator);
        setEndTag();
        return *this;
    }

    String& String::prepend(const char* str)
    {
        auto sz = std::strlen(str);
        LayoutReplace(layout, str, 0, 0, sz, allocator);
        setEndTag();
        return *this;
    }

    String& String::prepend(const char* str, SizeType count)
    {
        //auto sz = std::strlen(str);
        //sz = count < sz ? count : sz;

        if (count < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
            }
            auto sz = count;
            LayoutReplace(layout, str, 0, 0, count, allocator);
            setEndTag();
            return *this;
    }

    bool String::contains(const String& str) const noexcept
    {

        if (str.size() == 0 && size() == 0) [[unlikely]] return true;
        auto it = StringSearch(str.begin(), str.end(), begin(), end());
        return it != end();
    }

    bool String::contains(const char* str) const noexcept
    {
        return contains(str, std::strlen(str));
    }

    bool String::contains(const char* str, SizeType len) const
    {
        //auto sz = std::strlen(str);
        //len = len < sz ? len : sz;

        if (len < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, len can not be less than 0." };
            }

            if (len == 0 && size() == 0) [[unlikely]] return true;
            auto it = StringSearch(str, str + len, begin(), end());
            return it != end();
    }

    bool String::contains(CharType ch) const noexcept
    {
        auto sz = size();
        if (isEmpty()) return false;
        for (auto c : (*this))
        {
            if (c == ch) return true;
        }
        return false;
    }

    bool String::startsWith(const String& str) const noexcept
    {
        auto b1 = cstr();
        auto b2 = str.cstr();
        auto n = str.size();
        if (size() < n) return false;
        for (; n > 0; n--)
        {
            if (*b1 != *b2) return false;
            ++b1;
            ++b2;
        }
        return true;
    }

    bool String::startsWith(const char* str) const noexcept
    {
        auto b1 = cstr();
        auto b2 = str;
        auto n = std::strlen(str);
        if (size() < n) return false;
        for (; n > 0; n--)
        {
            if (*b1 != *b2) return false;
            ++b1;
            ++b2;
        }
        return true;
    }

    bool String::startsWith(const char* str, SizeType len) const
    {

        if (len < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, len can not be less than 0." };
            }
            auto b1 = cstr();
            auto b2 = str;
            //SizeType n = std::strlen(str);
            //n = len < n ? len : n;
            auto n = len;
            if (size() < n) return false;
            for (; n > 0; n--)
            {
                if (*b1 != *b2) return false;
                ++b1;
                ++b2;
            }
            return true;
    }

    bool String::startsWith(CharType ch) const noexcept
    {
        if (isEmpty()) return false;
        auto ptr = cstr();
        return *ptr == ch;
    }

    bool String::endsWith(const String& str) const noexcept
    {
        auto n = str.size();
        auto l = size();

        if (l < n) return false;
        auto b1 = cstr() + l - 1;
        auto b2 = str.cstr() + n - 1;
        for (; n > 0; n--)
        {
            if (*b1 != *b2) return false;
            --b1;
            --b2;
        }
        return true;
    }

    bool String::endsWith(const char* str) const noexcept
    {
        auto n = std::strlen(str);
        auto l = size();

        if (l < n) return false;
        auto b1 = cstr() + l - 1;
        auto b2 = str + n - 1;
        for (; n > 0; n--)
        {
            if (*b1 != *b2) return false;
            --b1;
            --b2;
        }
        return true;
    }

    bool String::endsWith(const char* str, SizeType len) const
    {
        //auto n = std::strlen(str);
        //n = len < n ? len : n;

        if (len < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, len can not be less than 0." };
            }
            auto n = len;
            auto l = size();

            if (l < n) return false;
            auto b1 = cstr() + l - 1;
            auto b2 = str + n - 1;
            for (; n > 0; n--)
            {
                if (*b1 != *b2) return false;
                --b1;
                --b2;
            }
            return true;
    }

    bool String::endsWith(CharType ch) const noexcept
    {
        return *(end() - 1) == ch;
    }

    auto String::count(const String& str) const noexcept -> SizeType
    {
        SizeType i = 0;
        auto sz = str.size();

        if (sz == 0) [[unlikely]] return 0;
        auto b = begin();
        auto e = end();
        auto b2 = str.begin();
        auto e2 = str.end();
        while (true)
        {
            b = StringSearch(b2, e2, b, e);
            if (b != e)
            {
                ++b;
                ++i;
            }
            else
            {
                break;
            }
        }
        return i;
    }

    auto String::count(const char* str) const noexcept-> SizeType
    {
        return count(str, std::strlen(str));
    }

    auto String::count(const char* str, SizeType len) const -> SizeType
    {

        if (len < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, len can not be less than 0." };
            }
            SizeType i = 0;
            //auto sz = std::strlen(str);
            //sz = len < sz ? len : sz;
            auto sz = len;

            if (sz == 0) [[unlikely]] return 0;
            auto b = begin();
            auto e = end();
            auto b2 = str;
            auto e2 = str + sz;
            while (true)
            {
                b = StringSearch(b2, e2, b, e);
                if (b != e)
                {
                    ++b;
                    ++i;
                }
                else
                {
                    break;
                }
            }
            return i;
    }

    auto String::count(CharType ch) const noexcept->SizeType
    {
        auto i = 0;
        for (auto c : (*this))
        {
            if (ch == c) ++i;
        }
        return i;
    }

    auto String::indexOf(CharType ch, SizeType from) const->SizeType
    {
        if (from >= size() || from < -size())
        {
            throw OutOfRange{ "[ntl.string.string] Param from is out of range." };
        }
        return find(ch, from);
    }

    auto String::indexOf(const String& str, SizeType from) const->SizeType
    {
        if (from >= size() || from < -size())
        {
            throw OutOfRange{ "[ntl.string.string] Param from is out of range." };
        }
        return find(str, from);
    }

    auto String::indexOf(const char* str, SizeType from) const -> SizeType
    {
        if (from >= size() || from < -size())
        {
            throw OutOfRange{ "[ntl.string.string] Param from is out of range." };
        }
        return find(str, from);
    }

    auto String::lastIndexOf(CharType ch, SizeType from) const->SizeType
    {
        if (from >= size() || from < -size())
        {
            throw OutOfRange{ "[ntl.string.string] Param from is out of range." };
        }
        return rfind(ch, from);
    }

    auto String::lastIndexOf(const String& str, SizeType from) const->SizeType
    {
        if (from >= size() || from < -size())
        {
            throw OutOfRange{ "[ntl.string.string] Param from is out of range." };
        }
        return rfind(str, from);
    }

    auto String::lastIndexOf(const char* str, SizeType from) const -> SizeType
    {
        if (from >= size() || from < -size())
        {
            throw OutOfRange{ "[ntl.string.string] Param from is out of range." };
        }
        return rfind(str, from);
    }

    auto String::rfind(CharType ch, SizeType from) const noexcept->SizeType
    {
        auto pos = from < 0 ? size() + from : from;
        pos = size() - 1 - pos;
        from = pos;
        auto b = rbegin() + from;
        auto e = rend();
        SizeType i = size() - 1;
        for (; b != e; ++b)
        {
            if (*b == ch) return i;
            --i;
        }
        return NPOS;
    }

    auto String::rfind(const String& str, SizeType from) const noexcept->SizeType
    {
        auto pos = from < 0 ? size() + from : from;
        pos = size() - 1 - pos;
        from = pos;
        auto it = StringSearch(
            str.rbegin(),
            str.rend(),
            rbegin() + from,
            rend());
        if (it == rend()) return NPOS;
        else return rend() - it - 1;
    }

    auto String::rfind(const char* str, SizeType from) const noexcept -> SizeType
    {
        auto pos = from < 0 ? size() + from : from;
        pos = size() - 1 - pos;
        from = pos;
        auto it = StringSearch(
            str + from,
            str + std::strlen(str),
            rbegin() + from,
            rend());
        if (it == rend()) return NPOS;
        else return rend() - it - 1;
    }

    auto String::find(const String& str, SizeType from) const noexcept -> SizeType
    {
        from = from >= 0 ? from : size() + from;
        auto it = StringSearch(str.begin(), str.end(), begin() + from, end());
        if (it == end()) return NPOS;
        else return it - begin();

    }

    auto String::find(const char* str, SizeType from) const noexcept -> SizeType
    {
        from = from >= 0 ? from : size() + from;
        auto it = StringSearch(str, str + std::strlen(str), begin() + from, end());
        if (it == end()) return NPOS;
        else return it - begin();
    }
    auto String::find(CharType ch, SizeType from) const noexcept -> SizeType
    {
        from = from >= 0 ? from : size() + from;
        auto b = begin() + from;
        auto e = end();
        SizeType i = 0;
        for (; b != e; ++b)
        {
            if (*b == ch) return i;
            ++i;
        }
        return NPOS;
    }

    String& String::insert(SizeType pos, const String& str)
    {

        if (pos > size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }
            pos = pos >= 0 ? pos : size() + pos;
            LayoutReplace(layout, str.cstr(), str.size(), pos, 0, allocator);
            return *this;
    }

    String& String::insert(SizeType pos, const char* str)
    {

        if (pos > size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }
            pos = pos >= 0 ? pos : size() + pos;
            LayoutReplace(layout, str, std::strlen(str), pos, 0, allocator);
            return *this;
    }

    String& String::insert(SizeType pos, CharType ch, SizeType count)
    {

        if (pos > size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }

                if (count < 0) [[unlikely]]
                    {
                        throw InvalidArgument{ "[ntl.string.string] Invalid argument, count can not be less than 0." };
                    }
                    pos = pos >= 0 ? pos : size() + pos;
                    LayoutReplace(layout, ch, count, pos, 0, allocator);
                    return *this;
    }

    auto String::insert(ConstIterator it, const String& str)->Iterator
    {
        auto pos = it - begin();
        insert(pos, str);
        return begin() + pos;
    }
    auto String::insert(ConstIterator it, const char* str)->Iterator
    {
        auto pos = it - begin();
        insert(pos, str);
        return begin() + pos;
    }
    auto String::insert(ConstIterator it, CharType ch, SizeType count)->Iterator
    {
        auto pos = it - begin();
        insert(pos, ch, count);
        return begin() + pos;
    }

    String& String::replace(SizeType pos, SizeType n, const String& str)
    {
        auto sz = str.size();

        if (pos >= size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }

                if (n < 0) [[unlikely]]
                    {
                        throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than 0." };
                    }

                        if (n > size() - pos - n) [[unlikely]]
                            {
                                throw OutOfRange{ "[ntl.string.string] Param n is out of range." };
                            }
                            pos = pos >= 0 ? pos : size() + pos;
                            LayoutReplace(layout, str.cstr(), sz, pos, n, allocator);
                            return *this;
    }

    String& String::replace(SizeType pos, SizeType n, CharType ch, SizeType count)
    {

        if (pos >= size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }

                if (n < 0) [[unlikely]]
                    {
                        throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than 0." };
                    }

                        if (n > size() - pos - n) [[unlikely]]
                            {
                                throw OutOfRange{ "[ntl.string.string] Param n is out of range." };
                            }
                            pos = pos >= 0 ? pos : size() + pos;
                            LayoutReplace(layout, ch, count, pos, n, allocator);
                            return *this;
    }

    String& String::replace(SizeType pos, SizeType n, const char* str)
    {
        auto sz = std::strlen(str);

        if (pos >= size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }

                if (n < 0) [[unlikely]]
                    {
                        throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than 0." };
                    }

                        if (n > size() - pos - n) [[unlikely]]
                            {
                                throw OutOfRange{ "[ntl.string.string] Param n is out of range." };
                            }
                            pos = pos >= 0 ? pos : size() + pos;
                            LayoutReplace(layout, str, sz, pos, n, allocator);
                            return *this;
    }

    String& String::remove(SizeType pos, SizeType n)
    {

        if (pos >= size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }

                if (n < 0) [[unlikely]]
                    {
                        throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than 0." };
                    }

                        if (n > size() - pos) [[unlikely]]
                            {
                                throw OutOfRange{ "[ntl.string.string] Param n is out of range." };
                            }
                            pos = pos >= 0 ? pos : size() + pos;
                            LayoutRemove(layout, pos, n, allocator);
                            return *this;
    }

    String& String::remove(const String& str)
    {
        auto pos = indexOf(str);
        if (pos != NPOS) return remove(pos, str.size());
        else return *this;
    }

    String& String::remove(const char* str)
    {
        auto sz = std::strlen(str);
        auto pos = indexOf(str, sz);
        if (pos != NPOS) return remove(pos, sz);
        else return *this;
    }

    String& String::remove(CharType ch)
    {
        auto pos = indexOf(ch);
        if (pos != NPOS) return remove(pos, 1);
        else return *this;
    }

    auto String::remove(ConstIterator it)->Iterator
    {
        auto pos = it - begin();
        remove(pos, 1);
        return begin() + pos;
    }

    auto String::remove(ConstIterator first, ConstIterator last)->Iterator
    {
        auto pos = first - begin();
        auto len = last - first;
        remove(pos, len);
        return begin() + pos;
    }

    String String::left(SizeType n) const
    {

        if (n < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than 0" };
            }

                if (n > size()) [[unlikely]]
                    {
                        throw OutOfRange{ "[ntl.string.string] Param n is out of range" };
                    }
                    return String(cstr(), n, allocator);
    }

    String String::right(SizeType n) const
    {

        if (n < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than 0" };
            }

                if (n > size()) [[unlikely]]
                    {
                        throw OutOfRange{ "[ntl.string.string] Param n is out of range" };
                    }
                    return String(cstr() + size() - n, n, allocator);
    }

    String String::repeat(SizeType n) const
    {
        String str(allocator);
        str.reserve(n * size());
        for (; n > 0; n--)
        {
            str += (*this);
        }
        return str;
    }

    String& String::fill(CharType ch, SizeType num)
    {
        clear();
        LayoutWrite(layout, ch, 0, num, allocator);
        return *this;
    }

    String String::trimmed() const
    {
        auto isSpace = [](char c) -> bool
            {
                return c == ' ' || c == '\t' || c == '\v' || c == '\n' || c == '\f' || c == '\r';
            };

        SizeType b = 0;
        SizeType e = 0;
        if (b + e >= size()) return String(allocator);
        for (; !isSpace(*(begin() + b)); ++b);
        for (; !isSpace(*(rbegin() + e)); ++e);
        return String(cstr() + b, size() - e - b, allocator);
    }

    void String::chop(SizeType n)
    {
        n = n >= 0 ? n : 0;
        n = n < size() ? n : size();
        remove(size() - n, n);
    }

    String String::chopped(SizeType n) const
    {

        if (n < 0) [[unlikely]]
            {
                throw InvalidArgument{ "[ntl.string.string] Invalid Argument, n can not be less than 0." };
            }
            return left(size() - n);
    }

    void String::truncate(SizeType pos)
    {

        if (pos < 0) [[unlikely]] clear();
        pos = pos < size() ? pos : size();
        remove(pos, size() - pos);
    }

    String String::sliced(SizeType pos) const
    {

        if (pos > size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }
            pos = (pos + size()) % size();
            return String(cstr() + pos, size() - pos, allocator);
    }

    String String::sliced(SizeType pos, SizeType n) const
    {
        // Check param

        if (pos > size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }

                if (n < 0) [[unlikely]]
                    {
                        throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than 0." };
                    }

                        if (n > size() - pos) [[unlikely]]
                            {
                                throw OutOfRange{ "[ntl.string.string] Param n is out of range." };
                            }
                            pos = (pos + size()) % size();
                            return String(cstr() + pos, n, allocator);
    }

    String String::substr(SizeType pos, SizeType n) const
    {

        if (pos > size() || pos < -size()) [[unlikely]]
            {
                throw OutOfRange{ "[ntl.string.string] Param pos is out of range." };
            }

                if (n < -1) [[unlikely]]
                    {
                        throw InvalidArgument{ "[ntl.string.string] Invalid argument, n can not be less than -1." };
                    }

                        if (n > size() - pos) [[unlikely]]
                            {
                                throw OutOfRange{ "[ntl.string.string] Param n is out of range." };
                            }
                            pos = (pos + size()) % size();
                            if (n == NPOS)
                            {
                                return String(cstr() + pos, size() - pos, allocator);
                            }
                            return String(cstr() + pos, n, allocator);
    }

    Optional<int> String::toInt()
    {
        int val;
        auto res = std::from_chars(cstr(), cstr() + size(), val);
        if (res.ec == decltype(res.ec)())
        {
            return val;
        }
        else
        {
            return NULLOPT;
        }
    }

    Optional<int32> String::toInt32()
    {
        return toInt();
    }

    Optional<int64> String::toInt64()
    {
        int64 val;
        auto res = std::from_chars(cstr(), cstr() + size(), val);
        if (res.ec == decltype(res.ec)())
        {
            return val;
        }
        else
        {
            return NULLOPT;
        }
    }

    Optional<float> String::toFloat()
    {
        float val;
        auto res = std::from_chars(cstr(), cstr() + size(), val);
        if (res.ec == decltype(res.ec)())
        {
            return val;
        }
        else
        {
            return NULLOPT;
        }
    }

    Optional<double> String::toDouble()
    {
        double val;
        auto res = std::from_chars(cstr(), cstr() + size(), val);
        if (res.ec == decltype(res.ec)())
        {
            return val;
        }
        else
        {
            return NULLOPT;
        }
    }


    String operator ""_s(const char* str)
    {
        return String(str);
    }

    String operator+(const String& str, const String& x)
    {
        String s{ str.allocator };

        if (str.allocator != x.allocator) [[unlikely]]
            {
                throw RuntimeError{ "[ntl.string.string] Expect two string have same allocator" };
            }
            s.reserve(str.size() + x.size());
            s.append(str).append(x);
            return x;
    }
    String operator+(const String& str, const char* x)
    {
        String s{ str.allocator };
        auto xsz = std::strlen(x);
        s.reserve(str.size() + xsz);
        s.append(str).append(x);
        return x;
    }
}