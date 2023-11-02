export module ntl.string.text;
export import ntl.string.string;
import ntl.string.unicode;
import ntl.utils;
import ntl.memory.allocator;
import ntl.functional.hash;
import ntl.iterator.reverse_iterator;
import ntl.containers.dynamic_array;
import <compare>;

namespace ne
{
	class TextIterator;
	class TextConstIterator;
	class TextInternal;
}
export namespace ne
{
	/**
	 * @class Text
	 * @breif Unicode support, encoding by utf32 for simplicity. Optimized by cow. Immutable;
	 */
	class Text
	{
	public:
		enum NormalizeStrategy
		{
			NFC, NFD, NFKD, NFKC
		};

		using SizeType = int64;
		using ValueType = utf32;
		using Reference = utf32&;
		using ConstReference = utf32&;
		using Pointer = utf32*;
		using ConstPointer = const utf32*;
		using Iterator = utf32*;
		using ConstIterator = const utf32*;
		using ReverseIterator = ne::ReverseIterator<Iterator>;
		using ConstReverseIterator = ne::ReverseIterator<ConstIterator>;

		Text();
		Text(const Text& text);
		Text(Text&& text) noexcept;
		Text(const utf32* str);
		Text(const char* str);
		Text(const utf8* str);
		Text(const String& str);
		Text(const Allocator& allocator);
		Text(const Text& text, const Allocator& allocator);
		Text(Text&& text, const Allocator& allocator) noexcept;
		Text(const char* str, const Allocator& allocator);
		Text(const utf8* str, const Allocator& allocator);
		Text(const utf32* str, const Allocator& allocator);
		Text(const char* str, int64 len, const Allocator& allocator);
		Text(const utf8* str, int64 len, const Allocator& allocator);
		Text(const utf32* str, int64 len, const Allocator& allocator);
		Text(const String& str, const Allocator& allocator);
		Text(SizeType n, char ch, const Allocator& allocator = Allocator());
		Text(SizeType n, utf8 ch, const Allocator& allocator = Allocator());
		Text(SizeType n, utf32 ch, const Allocator& allocator = Allocator());
		~Text();

		Text& operator=(const Text& text);
		Text& operator=(Text&& text);
		Text& operator=(const utf32* str);
		Text& operator=(const char* str);
		Text& operator=(const utf8* str);
		Text& operator=(const String& str);

		Text& assign(const Text& text);
		Text& assign(Text&& text);
		Text& assign(const utf32* str);
		Text& assign(const char* str);
		Text& assign(const utf8* str);
		Text& assign(const String& str);

		//Iterator begin() noexcept;
		//Iterator end() noexcept;
		//ConstIterator begin() const noexcept;
		//ConstIterator end() const noexcept;
		//ConstIterator cbegin() const noexcept;
		//ConstIterator cend() const noexcept;
		//ReverseIterator rbegin() noexcept;
		//ReverseIterator rend() noexcept;
		//ConstReverseIterator rbegin() const noexcept;
		//ConstReverseIterator rend() const noexcept;
		//ConstReverseIterator crbegin() const noexcept;
		//ConstReverseIterator crend() const noexcept;

		SizeType size() const noexcept;
		SizeType length() const noexcept;

		String toString() const;

		Text& append(utf32 ch, int64 n = 1);
		Text& append(char ch, int64 n = 1);
		Text& append(utf8 ch, int64 n = 1);
		Text& append(Text text);
		Text& append(const utf32* str);
		Text& append(const char* str);
		Text& append(const utf8* str);
		Text& append(const String& str);

		Text& prepend(utf32 ch, int64 n = 1);
		Text& prepend(char ch, int64 n = 1);
		Text& prepend(utf8 ch, int64 n = 1);
		Text& prepend(Text text);
		Text& prepend(const utf32* str);
		Text& prepend(const char* str);
		Text& prepend(const utf8* str);
		Text& prepend(const String& str);

		Text& insert(SizeType pos, Text text);
		Text& insert(SizeType pos, const char* str);
		Text& insert(SizeType pos, const utf8* str);
		Text& insert(SizeType pos, const utf32* str);
		Text& insert(SizeType pos, const String& str);
		Text& insert(SizeType pos, char ch, SizeType count=1);
		Text& insert(SizeType pos, utf8 ch, SizeType count=1);
		Text& insert(SizeType pos, utf32 ch, SizeType count=1);

		Text& replace(SizeType pos, SizeType n, Text text);
		Text& replace(SizeType pos, SizeType n, const char* str);
		Text& replace(SizeType pos, SizeType n, const utf8* str);
		Text& replace(SizeType pos, SizeType n, const utf32* str);
		Text& replace(SizeType pos, SizeType n, const String& str);
		Text& replace(SizeType pos, SizeType n, char ch, SizeType count = 1);
		Text& replace(SizeType pos, SizeType n, utf8 ch, SizeType count = 1);
		Text& replace(SizeType pos, SizeType n, utf32 ch, SizeType count = 1);

		Text& remove(SizeType pos, SizeType n);

		[[nodiscard]]
		utf32 operator[](int64 i) const;
		[[nodiscard]]
		utf32 at(int64 i) const;

		void normalize(NormalizeStrategy strategy);
		[[nodiscard]]
		Text normalized(NormalizeStrategy strategy);

		[[nodiscard]]
		int64 refcount() const;
		[[nodiscard]]
		HashValue hash() const;

		bool isEmpty() const noexcept;
		bool isNull() const noexcept;

		friend bool operator==(Text lhs, Text rhs);
		friend std::strong_ordering operator<=>(Text lhs, Text rhs);

		void swap(Text& text);
		friend void Swap(Text& lhs, Text& rhs);

		const utf32* data() const noexcept;

		const Allocator& getAllocator() const;
	private:
		friend class TextInternal;
		Allocator allocator;
		TextInternal* internal;
	};

	
}
