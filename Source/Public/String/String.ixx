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
	class String
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
		String& append(SizeType n, CharType ch);
		//String& append(Literal str);
		String& append(const CharType* str);
		String& append(const CharType* str, SizeType len);

		String& operator+=(const String& str);
		String& operator+=(CharType ch);
		//String& operator+=(Literal str);
		String& operator+=(const CharType* str);

		String& prepend(const String& str);
		String& prepend(CharType ch);
		String& prepend(SizeType n, CharType ch);
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
		Iterator insert(ConstIterator iterator, CharType ch, SizeType count=1);
		Iterator insert(ConstIterator iterator, const char* str);
		//Iterator insert(ConstIterator iterator, Literal str);

		String& replace(SizeType pos, SizeType n, const String& str);
		String& replace(SizeType pos, SizeType n, CharType ch, SizeType count=1);
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

		void swap(String& str);
		friend void Swap(String& s1, String& s2) { s1.swap(s2); }
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

