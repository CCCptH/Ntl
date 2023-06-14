export module ntl.string.text;
export import ntl.string.string;
import ntl.string.unicode;
import ntl.utils;
import ntl.memory.allocator;
import ntl.iterator.reverse_iterator;

namespace ne
{
	class TextIterator;
	class TextConstIterator;
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
		Text(const utf32* str, const Allocator& allocator);
		Text(const char* str, const Allocator& allocator);
		Text(const utf8* str, const Allocator& allocator);
		Text(const String& str, const Allocator& allocator);
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

		Iterator begin() noexcept;
		Iterator end() noexcept;
		ConstIterator begin() const noexcept;
		ConstIterator end() const noexcept;
		ConstIterator cbegin() const noexcept;
		ConstIterator cend() const noexcept;
		ReverseIterator rbegin() noexcept;
		ReverseIterator rend() noexcept;
		ConstReverseIterator rbegin() const noexcept;
		ConstReverseIterator rend() const noexcept;
		ConstReverseIterator crbegin() const noexcept;
		ConstReverseIterator crend() const noexcept;

		SizeType size() const noexcept;
		SizeType length() const noexcept;

		String toString() const;

		Text& append(utf32 ch);
		Text& append(char ch);
		Text& append(utf8 ch);
		Text& append(const Text& text);
		Text& append(const utf32* str);
		Text& append(const char* str);
		Text& append(const utf8* str);
		Text& append(const String& str);

		Text& prepend(utf32 ch);
		Text& prepend(char ch);
		Text& prepend(utf8 ch);
		Text& prepend(const Text& text);
		Text& prepend(const utf32* str);
		Text& prepend(const char* str);
		Text& prepend(const utf8* str);
		Text& prepend(const String& str);

	private:
		friend class TextInternal;
		TextInternal* internal;
	};

	
}
