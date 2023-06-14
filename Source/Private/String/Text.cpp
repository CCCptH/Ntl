module;
#include <stdlib.h>
#include <cstring>
#include <atomic>
module ntl.string.text;
import ntl.string.character;
import ntl.string.unicode;
import ntl.utils;
import ntl.exceptions;
import ntl.iterator.back_insert_iterator;

namespace ne
{
	void Utf32Cpy(utf32* dst, const utf32* src, size_t len)
	{
		memcpy(dst, src, sizeof(utf32) * len);
	}

	class TextInternal
	{
	public:
		Allocator allocator;
		int64 len;
		std::atomic<int64> shared;
		utf32 data[];

		static TextInternal* New(int64 n, Allocator allocator)
		{
			TextInternal* p = reinterpret_cast<TextInternal*>(allocator.allocate<uint8>(sizeof(TextInternal) + n * sizeof(utf32)));
			p->allocator = allocator;
			p->len = n;
			p->shared = 1;
			return p;
		}

		static TextInternal* New(int64 n)
		{
			return New(n, Allocator());
		}

		static TextInternal* IncRef(TextInternal* ti)
		{
			++ti->shared;
			return ti;
		}

		static TextInternal* DecRef(TextInternal* ti)
		{
			auto a = ti->allocator;
			--ti->shared;
			if (ti->shared==0)
			{
				a.deallocate(ti);
				return nullptr;
			}
			return ti;
		}

		static TextInternal* Fork(TextInternal* ti)
		{
			auto p = New(ti->len, ti->allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			DecRef(ti);
			return p;
		}

		static TextInternal* Fork(TextInternal* ti, Allocator allocator)
		{
			auto p = New(ti->len, allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			DecRef(ti);
			return p;
		}

		static TextInternal* Like(TextInternal* ti)
		{
			auto p = New(ti->len, ti->allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			return p;
		}

		static TextInternal* Like(TextInternal* ti, Allocator allocator)
		{
			auto p = New(ti->len, allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			return p;
		}

		static TextInternal* ExtendBackwardLike(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = New(ti->len + n, allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			return p;
		}
		static TextInternal* ExtendBackwardLike(TextInternal* ti, int64 n)
		{
			return ExtendBackwardLike(ti, n, ti->allocator);
		}

		static TextInternal* ExtendBackwardFork(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = ExtendBackwardLike(ti, n, allocator);
			DecRef(ti);
			return p;
		}
		static TextInternal* ExtendBackwardFork(TextInternal* ti, int64 n)
		{
			return ExtendBackwardFork(ti, n, ti->allocator);
		}

		static TextInternal* ExtendForwardLike(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = New(ti->len + n, allocator);
			Utf32Cpy(p->data+n, ti->data, ti->len);
			return p;
		}
		static TextInternal* ExtendForwardLike(TextInternal* ti, int64 n)
		{
			auto p = New(ti->len + n, ti->allocator);
			Utf32Cpy(p->data + n, ti->data, ti->len);
			return p;
		}

		static TextInternal* ExtendForwardFork(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = ExtendForwardLike(ti, n, allocator);
			DecRef(ti);
			return p;
		}
		static TextInternal* ExtendForwardFork(TextInternal* ti, int64 n)
		{
			return ExtendForwardFork(ti, n, ti->allocator);
		}
	};

	using Ti = TextInternal;

	Text::Text()
		: internal(Ti::New(0))
	{}
	Text::Text(const Text& text)
		: internal(Ti::IncRef(text.internal))
	{}
	Text::Text(Text&& text) noexcept
	{
		internal = text.internal;
		text.internal = nullptr;
	}
	Text::Text(const utf32* str)
		: Text(str, Allocator{})
	{}
	Text::Text(const char* str)
		: Text(str, Allocator{})
	{}
	Text::Text(const utf8* str)
		: Text(str, Allocator{})
	{}
	Text::Text(const String& str)
		: Text(str, Allocator{})
	{}
	Text::Text(const Allocator& allocator)
		: internal(Ti::New(0, allocator))
	{}
	Text::Text(const Text& text, const Allocator& allocator)
	{
		if (allocator == text.internal->allocator)
		{
			this->internal = Ti::IncRef(text.internal);
		}
		else
		{
			this->internal = Ti::Like(text.internal, allocator);
		}
	}
	Text::Text(Text&& text, const Allocator& allocator) noexcept
	{
		if (text.internal->allocator == allocator) {
			internal = text.internal;
			text.internal = nullptr;
		}
		else
		{
			internal = Ti::Fork(text.internal, allocator);
			text.internal == nullptr;
		}
	}
	Text::Text(const utf32* str, const Allocator& allocator)
	{
		SizeType len = 0;
		for (; str[len] == utf32(); ++len);
		internal = Ti::New(len, allocator);
		memmove(internal->data, str, sizeof(utf32) * len);
	}
	Text::Text(const char* str, const Allocator& allocator)
	{
		SizeType slen = std::strlen(str);
		SizeType len = CountCodepointsOfUtf8(str, str + slen);
		if (len < 0) [[unlikely]] throw InvalidArgument{ "[ntl.string.text] Error encoding" };

		internal = Ti::New(len, allocator);
		Utf8ToUtf32(internal->data, str, str + slen);

	}
	Text::Text(const utf8* str, const Allocator& allocator)
	{
		SizeType slen = std::strlen(reinterpret_cast<const char*>(str));
		SizeType len = CountCodepointsOfUtf8(str, str + slen);
		internal = Ti::New(len, allocator);
		Utf8ToUtf32(internal->data, str, str + slen);
	}
	Text::Text(const String& str, const Allocator& allocator)
		: Text(str.cstr(), allocator)
	{}
	Text::~Text()
	{
		if (internal != nullptr)
		{
			Ti::DecRef(internal);
		}
	}

	String Text::toString() const
	{
		auto sz = CountUtf8sOfCodepoints(internal->data, internal->data+internal->len);
		String str;
		str.reserve(sz);
		auto back_inserter = BackInserter(str);
		Utf32ToUtf8(back_inserter, internal->data, internal->data+internal->len);
		return str;
	}

	Text::SizeType Text::size() const noexcept
	{
		return internal->len;
	}
	Text::SizeType Text::length() const noexcept
	{
		return size();
	}

	//Text::Iterator Text::begin() noexcept
	//{
	//	if (control!=nullptr)
	//	{
	//		control = CopyAndDecRef(control, allocator);
	//		return control->data;
	//	}
	//	return nullptr;
	//}
	//Text::Iterator Text::end() noexcept
	//{
	//	if (control != nullptr)
	//	{
	//		control = CopyAndDecRef(control, allocator);
	//		return control->data+control->len;
	//	}
	//	return nullptr;
	//}
	//Text::ConstIterator Text::begin() const noexcept
	//{
	//	return control ? control->data : nullptr;
	//}
	//Text::ConstIterator Text::end() const noexcept
	//{
	//	return control ? control->data + control->len : nullptr;
	//}
	//Text::ConstIterator Text::cbegin() const noexcept
	//{
	//	return begin();
	//}
	//Text::ConstIterator Text::cend() const noexcept
	//{
	//	return end();
	//}
	//Text::ReverseIterator Text::rbegin() noexcept
	//{
	//	return ReverseIterator(end());
	//}
	//Text::ReverseIterator Text::rend() noexcept
	//{
	//	return ReverseIterator(begin());
	//}
	//Text::ConstReverseIterator Text::rbegin() const noexcept
	//{
	//	return ConstReverseIterator(end());
	//}
	//Text::ConstReverseIterator Text::rend() const noexcept
	//{
	//	return ConstReverseIterator(begin());
	//}
	//Text::ConstReverseIterator Text::crbegin() const noexcept
	//{
	//	return rbegin();
	//}
	//Text::ConstReverseIterator Text::crend() const noexcept
	//{
	//	return rend();
	//}

	Text& Text::operator=(const Text& text) { return assign(text); }
	Text& Text::operator=(Text&& text) { return assign(Move(text)); }
	Text& Text::operator=(const utf32* str) { return assign(str); }
	Text& Text::operator=(const char* str) { return assign(str); }
	Text& Text::operator=(const utf8* str) { return assign(str); }
	Text& Text::operator=(const String& str) { return assign(str); }

	Text& Text::assign(const Text& text)
	{
		Ti::DecRef(this->internal);
		if (this->internal->allocator == text.internal->allocator) {
			internal = Ti::IncRef(text.internal);
		}
		else
		{
			this->internal = Ti::Like(text.internal, this->internal->allocator);
		}
		return *this;
	}
	Text& Text::assign(Text&& text)
	{
		Ti::DecRef(this->internal);
		if (this->internal->allocator == text.internal->allocator) {
			internal = text.internal;
			text.internal = nullptr;
		}
		else
		{
			this->internal = Ti::Fork(text.internal, this->internal->allocator);
			text.internal = nullptr;
		}
		return *this;
	}
	Text& Text::assign(const utf32* str)
	{
		*this = Move(Text(str));
		return *this;
	}
	Text& Text::assign(const char* str)
	{
		*this = Move(Text(str));
		return *this;
	}
	Text& Text::assign(const utf8* str)
	{
		*this = Move(Text(str));
		return *this;
	}
	Text& Text::assign(const String& str)
	{
		*this = Move(Text(str));
		return *this;
	}

	Text& Text::append(utf32 ch)
	{
		auto sz = size();
		auto apsz = 1;
		internal = Ti::ExtendBackwardFork(internal, apsz);
		internal->data[sz] = ch;
		return *this;
	}

	Text& Text::append(char ch)
	{
		return append(utf32(ch));
	}

	Text& Text::append(utf8 ch)
	{
		return append(utf32(ch));
	}

	Text& Text::append(const Text& text)
	{
		auto sz = size();
		auto apsz = text.size();
		internal = Ti::ExtendBackwardFork(internal, apsz);
		Utf32Cpy(internal->data+sz, text.internal->data, apsz);
		return *this;
	}

	Text& Text::append(const utf32* str)
	{
		auto sz = size();
		auto apsz = 0;
		for (; str[apsz] != utf32(); ++apsz);
		internal = Ti::ExtendBackwardFork(internal, apsz);
		Utf32Cpy(internal->data+sz, str, apsz);
		return *this;
	}

	Text& Text::append(const char* str)
	{
		auto sz = size();
		auto strsz = std::strlen(str);
		auto apsz = CountCodepointsOfUtf8(str, str + strsz);
		internal = Ti::ExtendBackwardFork(internal, apsz);
		Utf8ToUtf32(internal->data+sz, str, str + apsz);
		return *this;
	}

	Text& Text::append(const utf8* str)
	{
		return append(reinterpret_cast<const char*>(str));
	}

	Text& Text::append(const String& str)
	{
		auto sz = size();
		auto strsz = str.size();
		auto apsz = CountCodepointsOfUtf8(str.cstr(), str.cstr() + strsz);
		internal = Ti::ExtendBackwardFork(internal, apsz);
		Utf8ToUtf32(internal->data + sz, str.cstr(), str.cstr() + apsz);
		return *this;
	}

	Text& Text::prepend(utf32 ch)
	{
		auto sz = size();
		auto apsz = 1;
		internal = Ti::ExtendForwardFork(internal, apsz);
		internal->data[0] = ch;
		return *this;
	}

	Text& Text::prepend(char ch)
	{
		return prepend(utf32(ch));
	}

	Text& Text::prepend(utf8 ch)
	{
		return prepend(utf32(ch));
	}

	Text& Text::prepend(const Text& text)
	{
		auto sz = size();
		auto apsz = text.size();
		internal = Ti::ExtendForwardFork(internal, apsz);
		Utf32Cpy(internal->data, text.internal->data, apsz);
		return *this;
	}

	Text& Text::prepend(const utf32* str)
	{
		auto sz = size();
		auto apsz = 0;
		for (; str[apsz] != utf32(); ++apsz);
		internal = Ti::ExtendForwardFork(internal, apsz);
		Utf32Cpy(internal->data, str, apsz);
		return *this;
	}

	// TODO: APPEND PREPEND CHECK ENCODING
	Text& Text::prepend(const char* str)
	{
		auto sz = size();
		auto strsz = std::strlen(str);
		auto apsz = CountCodepointsOfUtf8(str, str + strsz);
		internal = Ti::ExtendForwardFork(internal, apsz);
		Utf8ToUtf32(internal->data, str, str + apsz);
		return *this;
	}

	Text& Text::prepend(const utf8* str)
	{
		return prepend(reinterpret_cast<const char*>(str));
	}

	Text& Text::prepend(const String& str)
	{
		auto sz = size();
		auto strsz = str.size();
		auto apsz = CountCodepointsOfUtf8(str.cstr(), str.cstr() + strsz);
		internal = Ti::ExtendForwardFork(internal, apsz);
		Utf8ToUtf32(internal->data, str.cstr(), str.cstr() + apsz);
		return *this;
	}
}