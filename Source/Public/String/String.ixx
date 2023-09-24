module;
#include <charconv>
#include <string>
#include <cstring>
export module ntl.string.string;
//import ntl.string.byte_memory;
//import ntl.string.literal;
import ntl.memory.allocator;
import ntl.exceptions;
import ntl.compare;
import ntl.iterator;
import ntl.utils;
import ntl.containers.dynamic_array;
import ntl.optional;
import ntl.string.string_search;
import ntl.type_traits;

namespace ne
{
	struct StringLayout
	{
		using CharType = char;
		using SizeType = int64;
		struct LongLayout {
			struct {
				uint64 tag : 1;
				uint64 sz : 63;
			};
			SizeType cap;
			CharType* data;
		};
		constexpr static size_t LAYOUT_BYTES = sizeof(LongLayout);
		struct ShortLayout {
			struct {
				uint8 tag : 1;
				uint8 sz : 7;
			};
			CharType data[LAYOUT_BYTES - 1];
		};
		constexpr static size_t SHORT_BYTES = LAYOUT_BYTES - 1;
		static_assert(sizeof(ShortLayout) == sizeof(LongLayout));
		union {
			LongLayout large;
			ShortLayout small;
			CharType raw[LAYOUT_BYTES];
		} layout;
	};
	class StringIterator;
	class StringConstIter;
}

export namespace ne
{
	// TODO: add constexpr to string

	/**
	 * @class String
	 * @brief Works like std::string
	*/
	class __declspec(dllexport) String
	{
	public:
		using CharType = char;
		using ValueType = CharType;
		using SizeType = int64;
		using Reference = ValueType&;
		using ConstReference = const ValueType&;
		using Pointer = ValueType*;
		using ConstPointer = const ValueType*;
		using Iterator = StringIterator;
		using ConstIterator = StringConstIter;
		using ReverseIterator = ne::ReverseIterator<Iterator>;
		using ConstReverseIterator = ne::ReverseIterator<ConstIterator>;
		using OrderType = StrongOrdering;
	private:
		using Layout = StringLayout;
		Allocator allocator;
		Layout layout;
		void setEndTag(SizeType pos);
		void setEndTag();
	public:
		static constexpr SizeType NPOS = -1;

		/**
		 * @brief Default constructor.
		*/
		String();
		/**
		 * @brief Constructor that specify allocator
		 * @param allocator 
		*/
		explicit String(const Allocator& allocator);
		/**
		 * @brief Constructor, fill with count ch.
		 * @param count
		 * @param ch
		 * @return 
		*/
		String(SizeType count, CharType ch);

		/**
		 * @brief Constructor, fill with count ch.
		 * @param count 
		 * @param ch 
		 * @param allocator
		 * @note Throw if count < 0.
		*/
		String(SizeType count, CharType ch, const Allocator& allocator);
		/**
		 * @brief Copy constructor.
		 * @param str 
		*/
		String(const String& str);

		/**
		 * @brief Construct from str with first count bytes.
		 * @param str 
		 * @param count
		 * @note Throw if count < 0. If count > str.size(), it will construct from the whole of str.
		*/
		String(const String& str, SizeType count);

		/**
		 * @brief Construct from str, specify allocator
		 * @param str
		 * @param allocator
		*/
		String(const String& str, const Allocator& allocator);

		/**
		 * @brief Construct from str with first count bytes.
		 * @param str
		 * @param count
		 * @param allocator
		 * @note Throw if count < 0. If count > str.size(), it will construct from the whole of str.
		*/
		String(const String& str, SizeType count, const Allocator& allocator);

		/**
		 * @brief Construct from c-style str
		 * @param str 
		*/
		String(const char* str);
		/**
		 * @brief Construct by c-style string.
		 * @param str
		 * @param count
		 * @note Throw if count < 0. If count is greater than the length of str, the behavior is undefined.
		*/
		String(const char* str, SizeType count);

		/**
		 * @brief Construct from c-style str
		 * @param str
		 * @param allocator
		*/
		String(const char* str, const Allocator& allocator);

		/**
		 * @brief Construct by c-style string.
		 * @param str
		 * @param count
		 * @param allocator
		 * @note Throw if count < 0. If count is greater than the length of str, the behavior is undefined.
		*/
		String(const char* str, SizeType count, const Allocator& allocator);
		//String(Literal str);
		//String(Literal str, const Allocator& allocator);

		/**
		 * @brief Move constructor
		*/
		String(String&& str) noexcept;

		/**
		 * @brief Copy bytes from first to last
		 * @param first
		 * @param last
		*/
		template<ConceptInputIterator It>
		String(It first, It last)
			: String()
		{
			auto distance = iters::GetDistance(first, last);
			this->reserve(distance);
			for(;first!=last;++first)
			{
				this->append(*first);
			}
		}
		~String();

		String& assign(SizeType n, char ch);
		String& assign(const String& str);
		String& assign(const String& str, SizeType count);
		String& assign(String&& str);
		String& assign(const char* str);
		String& assign(const char* str, SizeType count);
		//String& assign(Literal str);

		// todo: bug
		template<ConceptInputIterator It>
		String& assign(It first, It last)
		{
			clear();
			auto distance = iters::GetDistance(first, last);
			this->reserve(distance);
			for (; first != last; ++first)
			{
				this->append(*first);
			}
			return *this;
		}

		String& operator=(const String& str);
		String& operator=(const char* str);
		String& operator=(String&& str);
		//String& operator=(Literal str);

		friend OrderType operator<=>(const String& lhs, const String& rhs) noexcept;
		friend OrderType operator<=>(const String& lhs, const char* rhs) noexcept;
		friend bool operator==(const String& lhs, const String& rhs) noexcept;
		friend bool operator==(const String& lhs, const char* rhs) noexcept;
		//friend OrderType operator<=>(Literal str);
		OrderType compare(const String& str) const noexcept;
		//OrderType compare(Literal str) const noexcept;
		OrderType compare(const CharType* str) const noexcept;

		SizeType size() const noexcept;
		SizeType capacity() const noexcept;

		bool isEmpty() const noexcept;
		bool isNull() const noexcept;

		const char* cstr() const noexcept;

		auto begin() noexcept -> Iterator;
		auto end() noexcept -> Iterator;
		auto begin() const noexcept -> ConstIterator;
		auto end() const noexcept -> ConstIterator;
		auto cbegin() const noexcept -> ConstIterator;
		auto cend() const noexcept -> ConstIterator;
		auto rbegin() noexcept -> ReverseIterator;
		auto rend() noexcept -> ReverseIterator;
		auto rbegin() const noexcept -> ConstReverseIterator;
		auto rend() const noexcept -> ConstReverseIterator;
		auto crbegin() const noexcept -> ConstReverseIterator;
		auto crend() const noexcept -> ConstReverseIterator;

		/**
		 * @brief Clear content, do not release the memory;
		*/
		void clear();
		void reserve(SizeType n);
		void squeeze();

		Reference at(SizeType i);
		ConstReference at(SizeType i) const;
		Reference operator[](SizeType i) noexcept;
		ConstReference operator[](SizeType i) const noexcept;

		String& append(const String& str);
		String& append(CharType ch);
		//String& append(Literal str);
		String& append(const CharType* str);
		String& append(const CharType* str, SizeType len);

		String& operator+=(const String& str);
		String& operator+=(CharType ch);
		//String& operator+=(Literal str);
		String& operator+=(const CharType* str);

		String& prepend(const String& str);
		String& prepend(CharType ch);
		//String& prepend(Literal str);
		String& prepend(const CharType* str);
		String& prepend(const CharType* str, SizeType len);

		bool contains(const String& str) const noexcept;
		//bool contains(Literal str) const noexcept;
		bool contains(const CharType* str) const noexcept;
		bool contains(const CharType* str, SizeType len) const;
		bool contains(CharType ch) const noexcept;

		bool startsWith(const String& str) const noexcept;
		//bool startsWith(Literal str) const noexcept;
		bool startsWith(const CharType* str) const noexcept;
		bool startsWith(const CharType* str, SizeType len) const;
		bool startsWith(CharType ch) const noexcept;

		bool endsWith(const String& str) const noexcept;
		//bool endsWith(Literal str) const noexcept;
		bool endsWith(const CharType* str) const noexcept;
		bool endsWith(const CharType* str, SizeType len) const;
		bool endsWith(CharType ch) const noexcept;

		SizeType count(const String& str) const noexcept;
		//SizeType count(Literal str) const noexcept;
		SizeType count(const CharType* str) const noexcept;
		SizeType count(const CharType* str, SizeType len) const;
		SizeType count(CharType ch) const noexcept;

		SizeType indexOf(CharType ch, SizeType from=0) const;
		SizeType indexOf(const String& str, SizeType from=0) const;
		SizeType indexOf(const char* str, SizeType from = 0) const;
		//SizeType indexOf(Literal str, SizeType from = 0) const noexcept;
		SizeType lastIndexOf(CharType ch, SizeType from = -1) const;
		SizeType lastIndexOf(const String& str, SizeType from = -1) const;
		SizeType lastIndexOf(const char* str, SizeType from = -1) const;
		//SizeType lastIndexOf(Literal str, SizeType from = NPOS) const noexcept;
		SizeType find(CharType ch, SizeType from = 0) const noexcept;
		SizeType find(const String& str, SizeType from = 0) const noexcept;
		SizeType find(const char* str, SizeType from = 0) const noexcept;
		//SizeType find(Literal str, SizeType from = 0) const noexcept;
		SizeType rfind(CharType ch, SizeType from = -1) const noexcept;
		SizeType rfind(const String& str, SizeType from = -1) const noexcept;
		SizeType rfind(const char* str, SizeType from = -1) const noexcept;
		//SizeType rfind(Literal str, SizeType from = NPOS) const noexcept;

		String& insert(SizeType pos, const String& str);
		String& insert(SizeType pos, CharType ch, SizeType count);
		String& insert(SizeType pos, const char* str);
		//String& insert(SizeType pos, Literal str);
		Iterator insert(ConstIterator iterator, const String& str);
		Iterator insert(ConstIterator iterator, CharType ch, SizeType count);
		Iterator insert(ConstIterator iterator, const char* str);
		//Iterator insert(ConstIterator iterator, Literal str);

		String& replace(SizeType pos, SizeType n, const String& str);
		String& replace(SizeType pos, SizeType n, CharType ch, SizeType count);
		String& replace(SizeType pos, SizeType n, const char* str);
		//String& replace(SizeType pos, SizeType n, Literal str);

		Iterator remove(ConstIterator pos);
		Iterator remove(ConstIterator first, ConstIterator last);
		String& remove(SizeType pos, SizeType n);
		String& remove(CharType ch);
		String& remove(const String& str);
		//String& remove(Literal str);
		String& remove(const char* str);

		String left(SizeType pos) const;
		String right(SizeType pos) const;
		String repeat(SizeType n) const;
		/**
		 * @brief Sets every character in the string to character ch. If size is different from -1 (default), the string is resized to size beforehand.
		 * @param ch 
		 * @param num 
		 * @return 
		*/
		String& fill(CharType ch, SizeType num);
		/**
		 * @brief Returns a string that has whitespace removed from the start and the end.  This includes the ASCII characters '\t', '\n', '\v', '\f', '\r', and ' '.
		 * @return 
		*/
		String trimmed() const;
		/**
		 * @brief Removes n characters from the end of the string.\n
		 * If n is greater than or equal to size(), the result is an empty string; if <b>n</b> is negative, it is equivalent to passing zero.
		 * @param n
		*/
		void chop(SizeType n);
		/**
		 * @brief Returns a substring that contains the size() - len leftmost characters of this string.
		 * @param n
		 * @return
		*/
		String chopped(SizeType len) const;
		/**
		 * @brief Truncates the string at the given position index.
		 * If the specified position index is beyond the end of the string, nothing happens.
		 * If position is negative, it is equivalent to passing zero.
		 * @param pos 
		*/
		void truncate(SizeType pos);

		/**
		 * @brief Returns a string that contains n characters of this string.
		 * @param pos 
		 * @return 
		*/
		String sliced(SizeType pos) const;
		/**
		 * @brief Returns a string that contains n characters of this string, starting at position pos.
		 * @param pos 
		 * @param n 
		 * @return 
		*/
		String sliced(SizeType pos, SizeType n) const;
		String substr(SizeType pos, SizeType n=NPOS) const;

		//DynamicArray<String> split(CharType ch) const;
		Optional<int> toInt();
		Optional<int32> toInt32();
		Optional<int64> toInt64();
		Optional<double> toDouble();
		Optional<float> toFloat();

		friend String operator""_s(const char* str);
		//friend String operator""_s(const utf8* str);

		friend String operator+(const String& lhs, const String& rhs);
		friend String operator+(const String& lhs, const char* str);

		friend String FromStdString(const std::string& str)
		{
			return String(str.c_str(), str.size());
		}
	};
}

namespace ne
{
	class StringIterator
	{
		friend class StringConstIter;
	public:
		using Pointer = typename String::Pointer;
		using Reference = typename String::Reference;
		using ThisType = StringIterator;
		using IteratorCategory = RandomAccessIteratorCategory;
		using ValueType = typename String::ValueType;
		using DifferenceType = decltype(Pointer() - Pointer());

		Pointer ptr;

		StringIterator() = default;
		StringIterator(Pointer ptr) :ptr(ptr) {}
		StringIterator(const StringIterator&) = default;
		StringIterator(StringIterator&&) = default;
		~StringIterator() noexcept = default;

		ThisType& operator=(const ThisType&) = default;
		ThisType& operator=(ThisType&&) = default;

		Pointer operator->() const noexcept
		{
			return ptr;
		}
		Reference operator*() const noexcept
		{
			return *ptr;
		}

		Reference operator[](DifferenceType i)const noexcept
		{
			return *(ptr + i);
		}

		friend StrongOrdering operator<=>(const ThisType&, const ThisType&) = default;
		ThisType& operator++() noexcept
		{
			++ptr;
			return *this;
		}
		ThisType operator++(int) noexcept
		{
			auto tmp = *this;
			++ptr;
			return tmp;
		}
		ThisType& operator--() noexcept
		{
			--ptr;
			return *this;
		}
		ThisType operator--(int) noexcept
		{
			auto tmp = *this;
			--ptr;
			return tmp;
		}
		DifferenceType operator-(const ThisType& x) const noexcept
		{
			return ptr - x.ptr;
		}
		friend ThisType operator+(const ThisType& x, DifferenceType n) noexcept
		{
			return ThisType(x.ptr + n);
		}
		friend ThisType operator-(const ThisType& x, DifferenceType n) noexcept
		{
			return ThisType(x.ptr - n);
		}
		ThisType& operator+=(DifferenceType n) noexcept
		{
			ptr += n;
			return  *this;
		}
		ThisType& operator-=(DifferenceType n) noexcept
		{
			ptr -= n;
			return  *this;
		}
	};
	class StringConstIter
	{
	public:
		using Pointer = typename String::ConstPointer;
		using Reference = typename String::ConstReference;
		using ThisType = StringConstIter;
		using IteratorCategory = RandomAccessIteratorCategory;
		using ValueType = typename String::ValueType;
		using DifferenceType = decltype(Pointer() - Pointer());

		Pointer ptr;
		StringConstIter() = default;
		StringConstIter(Pointer ptr) :ptr(ptr) {}
		StringConstIter(const StringIterator& x) : ptr(x.ptr) {};
		StringConstIter(const StringConstIter&) = default;
		StringConstIter(StringConstIter&&) = default;
		~StringConstIter() noexcept = default;

		ThisType& operator=(const ThisType&) = default;
		ThisType& operator=(ThisType&&) = default;

		Pointer operator->() const noexcept
		{
			return ptr;
		}
		Reference operator*() const noexcept
		{
			return *ptr;
		}

		Reference operator[](DifferenceType i) const noexcept
		{
			return *(ptr + i);
		}

		friend StrongOrdering operator<=>(const ThisType&, const ThisType&) = default;
		ThisType& operator++() noexcept
		{
			++ptr;
			return *this;
		}
		ThisType operator++(int) noexcept
		{
			auto tmp = *this;
			++ptr;
			return tmp;
		}
		ThisType& operator--() noexcept
		{
			--ptr;
			return *this;
		}
		ThisType operator--(int) noexcept
		{
			auto tmp = *this;
			--ptr;
			return tmp;
		}
		DifferenceType operator-(const ThisType& x) const noexcept
		{
			return ptr - x.ptr;
		}
		friend ThisType operator+(const ThisType& x, DifferenceType n) noexcept
		{
			return ThisType(x.ptr + n);
		}
		friend ThisType operator-(const ThisType& x, DifferenceType n) noexcept
		{
			return ThisType(x.ptr - n);
		}
		ThisType& operator+=(DifferenceType n) noexcept
		{
			ptr += n;
			return  *this;
		}
		ThisType& operator-=(DifferenceType n) noexcept
		{
			ptr -= n;
			return  *this;
		}
	};

}


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
        if (n < StringLayout::SHORT_BYTES)
        {
            if (IsLargeLayout(s))
            {
                InitLayout(s);
                s.layout.small.sz = n;
                std::memcpy(s.layout.small.data, old_ptr, n);
            }
        }
        else
        {
            LayoutAlloc(s, n, allocator);
            auto new_sz = old_size < n ? old_size : n;
            LayoutWrite(s, old_ptr, 0, new_sz);
            s.layout.large.sz = new_sz;
            allocator.deallocate(old_ptr);
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
    inline void LayoutRemove(StringLayout& s, StringLayout::SizeType pos, StringLayout::SizeType n)
    {
        auto sz = LayoutSize(s);
        auto ptr = LayoutData(s);
        for (auto i = n; i > 0; i--) {
            *ptr = *(ptr + n);
            ++ptr;
        }
        SetLayoutSize(s, sz - n);
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
        LayoutReserve(layout, size(), allocator);
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
                            LayoutRemove(layout, pos, n);
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