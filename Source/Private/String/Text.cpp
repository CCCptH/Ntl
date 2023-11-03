module;
#include <stdlib.h>
#include <cstring>
#include <atomic>
#include "unilib/uninorms.h"
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

	int64 Utf32Len(const utf32* str) {
		int64 i = 0;
		while (*str != utf32()) {
			++i;
			++str;
		}
		return i;
	}

	void Utf32Set(utf32* dst, utf32 ch, size_t n) {
		for (size_t i = 0; i < n; i++) {
			dst[i] = ch;
		}
	}

	class TextInternal
	{
	public:
		int64 len;
		HashValue hash;
		std::atomic<int64> shared;
		utf32 data[];

		static void ComputeHash(TextInternal* ti) {
			Hash<__internal::BytesView> HASHER;
			__internal::BytesView b{ ti->data, ti->len * sizeof(utf32) };
			ti->hash = HASHER(b);
		}

		static TextInternal* New(int64 n, Allocator allocator)
		{
			TextInternal* p = reinterpret_cast<TextInternal*>(allocator.allocate<uint8>(sizeof(TextInternal) + n * sizeof(utf32)));
			p->len = n;
			p->shared = 1;
			return p;
		}

		static TextInternal* IncRef(TextInternal* ti)
		{
			++ti->shared;
			return ti;
		}

		static TextInternal* DecRef(TextInternal* ti, Allocator allocator)
		{
			if (ti->shared.fetch_sub(1) == 0)
			{
				allocator.deallocate(ti);
				return nullptr;
			}
			return ti;
		}

		static TextInternal* Fork(TextInternal* ti, Allocator allocator)
		{
			auto p = New(ti->len, allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			p->hash = ti->hash;
			DecRef(ti, allocator);
			return p;
		}

		static TextInternal* Like(TextInternal* ti, Allocator allocator)
		{
			auto p = New(ti->len, allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			p->hash = ti->hash;
			return p;
		}

		static TextInternal* ExtendBackwardLike(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = New(ti->len + n, allocator);
			Utf32Cpy(p->data, ti->data, ti->len);
			return p;
		}

		static TextInternal* ExtendBackwardFork(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = ExtendBackwardLike(ti, n, allocator);
			DecRef(ti, allocator);
			return p;
		}

		static TextInternal* ExtendForwardLike(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = New(ti->len + n, allocator);
			Utf32Cpy(p->data + n, ti->data, ti->len);
			return p;
		}

		static TextInternal* ExtendForwardFork(TextInternal* ti, int64 n, Allocator allocator)
		{
			auto p = ExtendForwardLike(ti, n, allocator);
			DecRef(ti, allocator);
			return p;
		}

		static TextInternal* ExtendSplitFork(TextInternal* ti, int64 pos, int64 n, Allocator allocator) {
			auto p = New(ti->len + n, allocator);
			Utf32Cpy(p->data, ti->data, pos);
			Utf32Cpy(p->data + n + pos, ti->data + n, p->len - pos);
			DecRef(ti, allocator);
			return p;
		}

	};

	using Ti = TextInternal;

	Text::Text()
		: internal(nullptr)
		, allocator()
	{}
	Text::Text(const Text& text)
		: allocator()
	{
		internal = text.internal == nullptr ? nullptr : Ti::IncRef(text.internal);
	}
	Text::Text(Text&& text) noexcept
		: allocator()
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
		: internal(nullptr)
		, allocator(allocator)
	{}
	Text::Text(const Text& text, const Allocator& allocator)
		: allocator(allocator)
	{
		if (allocator == text.allocator)
		{
			this->internal = Ti::IncRef(text.internal);
		}
		else
		{
			this->internal = Ti::Like(text.internal, allocator);
		}
	}
	Text::Text(Text&& text, const Allocator& allocator) noexcept
		: allocator(allocator)
	{
		if (text.allocator == allocator) {
			internal = text.internal;
			text.internal = nullptr;
		}
		else
		{
			internal = Ti::Fork(text.internal, allocator);
			text.internal = nullptr;
		}
	}
	Text::Text(const utf32* str, const Allocator& allocator)
		: allocator(allocator)
	{
		SizeType len = 0;
		for (; str[len] == utf32(); ++len);
		internal = Ti::New(len, allocator);
		memmove(internal->data, str, sizeof(utf32) * len);
		Ti::ComputeHash(internal);
	}

	Text::Text(const utf32* str, int64 len, const Allocator& allocator)
		: allocator(allocator)
	{
		internal = Ti::New(len, allocator);
		memmove(internal->data, str, sizeof(utf32) * len);
		Ti::ComputeHash(internal);
	}

	Text::Text(const char* str, int64 slen, const Allocator& allocator)
		: allocator(allocator)
	{
		SizeType len = CountCodepointsOfUtf8(str, str + slen);
		if (len < 0) [[unlikely]] throw InvalidArgument{ "[ntl.string.text] Error encoding" };

		internal = Ti::New(len, allocator);
		Utf8ToUtf32(internal->data, str, str + slen);
		Ti::ComputeHash(internal);
	}
	Text::Text(const utf8* str, int64 slen, const Allocator& allocator)
		: allocator(allocator)
	{
		SizeType len = CountCodepointsOfUtf8(str, str + slen);
		internal = Ti::New(len, allocator);
		Utf8ToUtf32(internal->data, str, str + slen);
		Ti::ComputeHash(internal);
	}

	Text::Text(const char* str, const Allocator& allocator)
		: Text(str, strlen(str), allocator)
	{}
	Text::Text(const utf8* str, const Allocator& allocator)
		: Text(str, strlen(reinterpret_cast<const char*>(str)), allocator)
	{}

	Text::Text(const String& str, const Allocator& allocator)
		: Text(str.cstr(), str.size(), allocator)
	{}

	Text::Text(SizeType n, char ch, const Allocator& allocator)
		: Text(n, utf32(ch), allocator)
	{}
	Text::Text(SizeType n, utf8 ch, const Allocator& allocator)
		: Text(n, utf32(ch), allocator)
	{}
	Text::Text(SizeType n, utf32 ch, const Allocator& allocator)
		: allocator(allocator)
	{
		internal = Ti::New(n, allocator);
		Utf32Set(internal->data, ch, n);
	}
	Text::~Text()
	{
		if (internal != nullptr)
		{
			Ti::DecRef(internal, allocator);
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
		return internal ? internal->len : 0;
	}
	Text::SizeType Text::length() const noexcept
	{
		return size();
	}

	int64 Text::refcount() const {
		return internal == nullptr ? 0 : internal->shared.load();
	}

	HashValue Text::hash() const {
		return internal == nullptr ? 0 : internal->hash;
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
		Ti::DecRef(this->internal, allocator);
		if (this->allocator == text.allocator) {
			internal = Ti::IncRef(text.internal);
		}
		else
		{
			this->internal = Ti::Like(text.internal, this->allocator);
		}
		return *this;
	}
	Text& Text::assign(Text&& text)
	{
		Ti::DecRef(this->internal, allocator);
		if (this->allocator == text.allocator) {
			internal = text.internal;
			text.internal = nullptr;
		}
		else
		{
			this->internal = Ti::Fork(text.internal, this->allocator);
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

	Text& Text::append(utf32 ch, int64 n)
	{
		auto sz = size();
		auto apsz = n;
		internal = Ti::ExtendBackwardFork(internal, apsz, allocator);
		for (int64 i = 0; i < n; i++) {
			internal->data[sz+i] = ch;
		}
		return *this;
	}

	Text& Text::append(char ch, int64 n)
	{
		return append(utf32(ch), n);
	}

	Text& Text::append(utf8 ch, int64 n)
	{
		return append(utf32(ch), n);
	}

	Text& Text::append(Text text)
	{
		auto sz = size();
		auto apsz = text.size();
		internal = Ti::ExtendBackwardFork(internal, apsz, allocator);
		Utf32Cpy(internal->data+sz, text.internal->data, apsz);
		Ti::ComputeHash(internal);
		return *this;
	}

	Text& Text::append(const utf32* str)
	{
		auto sz = size();
		auto apsz = 0;
		for (; str[apsz] != utf32(); ++apsz);
		internal = Ti::ExtendBackwardFork(internal, apsz, allocator);
		Utf32Cpy(internal->data+sz, str, apsz);
		Ti::ComputeHash(internal);
		return *this;
	}

	Text& Text::append(const char* str)
	{
		auto sz = size();
		auto strsz = std::strlen(str);
		auto apsz = CountCodepointsOfUtf8(str, str + strsz);
		internal = Ti::ExtendBackwardFork(internal, apsz, allocator);
		Utf8ToUtf32(internal->data+sz, str, str + apsz);
		Ti::ComputeHash(internal);
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
		internal = Ti::ExtendBackwardFork(internal, apsz, allocator);
		Utf8ToUtf32(internal->data + sz, str.cstr(), str.cstr() + apsz);
		Ti::ComputeHash(internal);
		return *this;
	}

	Text& Text::prepend(utf32 ch, int64 n)
	{
		auto sz = size();
		auto apsz = 1;
		internal = Ti::ExtendForwardFork(internal, apsz, allocator);
		for (int64 i = 0; i < n; i++) {
			internal->data[i] = ch;
		}
		Ti::ComputeHash(internal);
		return *this;
	}

	Text& Text::prepend(char ch, int64 n)
	{
		return prepend(utf32(ch));
	}

	Text& Text::prepend(utf8 ch, int64 n)
	{
		return prepend(utf32(ch));
	}

	Text& Text::prepend(Text text)
	{
		auto sz = size();
		auto apsz = text.size();
		internal = Ti::ExtendForwardFork(internal, apsz, allocator);
		Utf32Cpy(internal->data, text.internal->data, apsz);
		Ti::ComputeHash(internal);
		return *this;
	}

	Text& Text::prepend(const utf32* str)
	{
		auto sz = size();
		auto apsz = 0;
		for (; str[apsz] != utf32(); ++apsz);
		internal = Ti::ExtendForwardFork(internal, apsz, allocator);
		Utf32Cpy(internal->data, str, apsz);
		Ti::ComputeHash(internal);
		return *this;
	}

	// TODO: APPEND PREPEND CHECK ENCODING
	Text& Text::prepend(const char* str)
	{
		auto sz = size();
		auto strsz = std::strlen(str);
		auto apsz = CountCodepointsOfUtf8(str, str + strsz);
		internal = Ti::ExtendForwardFork(internal, apsz, allocator);
		Utf8ToUtf32(internal->data, str, str + apsz);
		Ti::ComputeHash(internal);
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
		internal = Ti::ExtendForwardFork(internal, apsz, allocator);
		Utf8ToUtf32(internal->data, str.cstr(), str.cstr() + apsz);
		Ti::ComputeHash(internal);
		return *this;
	}

	Text& Text::insert(SizeType pos, Text text) {
		if (text.data() == nullptr) [[unlikely]]  return *this;
		auto n = text.size();
		auto p = Ti::ExtendSplitFork(internal, pos, n, allocator);
		Utf32Cpy(p->data + pos, text.data(), n);
		internal = p;
		return *this;
	}
	Text& Text::insert(SizeType pos, const char* str) {
		return insert(pos, Text(str, allocator));
	}
	Text& Text::insert(SizeType pos, const utf8* str) {
		return insert(pos, Text(str, allocator));
	}
	Text& Text::insert(SizeType pos, const utf32* str) {
		return insert(pos, Text(str, allocator));
	}
	Text& Text::insert(SizeType pos, const String& str) {
		return insert(pos, Text(str, allocator));
	}
	Text& Text::insert(SizeType pos, utf32 ch, SizeType count) {
		auto p = Ti::ExtendSplitFork(internal, pos, count, allocator);
		Utf32Set(p->data + pos, ch, count);
		internal = p;
		return *this;
	}
	Text& Text::insert(SizeType pos, utf8 ch, SizeType count) {
		return insert(pos, utf32(ch), count);
	}
	Text& Text::insert(SizeType pos, char ch, SizeType count) {
		return insert(pos, utf32(ch), count);
	}

	Text& Text::replace(SizeType pos, SizeType n, Text text)
	{
		auto len = text.size();
		if (n <= len) {
			auto diff = len - n;
			internal = Ti::ExtendSplitFork(internal, pos, diff, allocator);
			Utf32Cpy(internal->data + pos, text.data(), len);
		}
		else {
			auto diff = n - len;
			auto p = Ti::New(size() - diff, allocator);
			Utf32Cpy(p->data, internal->data, pos);
			Utf32Cpy(p->data + pos + n, internal->data + pos + diff, size() - pos - diff);
			Utf32Cpy(p->data + pos, text.data(), len);
			Ti::DecRef(internal, allocator);
			internal = p;
		}
		return *this;
	}
	Text& Text::replace(SizeType pos, SizeType n, const char* str) {
		return replace(pos, n, Text(str, allocator));
	}
	Text& Text::replace(SizeType pos, SizeType n, const utf8* str) {
		return replace(pos, n, Text(str, allocator));
	}
	Text& Text::replace(SizeType pos, SizeType n, const utf32* str) {
		return replace(pos, n, Text(str, allocator));
	}
	Text& Text::replace(SizeType pos, SizeType n, const String& str) {
		return replace(pos, n, Text(str, allocator));
	}
	Text& Text::replace(SizeType pos, SizeType n, char ch, SizeType count) {
		return replace(pos, n, Text(count, ch, allocator));
	}
	Text& Text::replace(SizeType pos, SizeType n, utf8 ch, SizeType count) {
		return replace(pos, n, Text(count, ch, allocator));
	}
	Text& Text::replace(SizeType pos, SizeType n, utf32 ch, SizeType count) {
		return replace(pos, n, Text(count, ch, allocator));
	}


	void Text::swap(Text& text) {
		utils::Swap(internal, text.internal);
	}

	void Swap(Text& x, Text& y) {
		return x.swap(y);
	}

	Text Text::normalized(Text::NormalizeStrategy s) {
		std::u32string buf;
		buf.reserve(size());
		for (int64 i = 0; i < size(); i++) {
			buf.append(1, internal->data[i]);
		}
		if (s == NFC) {
			unilib::uninorms::nfc(buf);
		}
		else if (s == NFD) {
			unilib::uninorms::nfd(buf);
		}
		else if (s == NFKC) {
			unilib::uninorms::nfkc(buf);
		}
		else if (s == NFKD) {
			unilib::uninorms::nfkd(buf);
		}

		return Text(buf.data(), buf.size(), allocator);
	}

	void Text::normalize(Text::NormalizeStrategy s) {
		auto t = normalized(s);
		this->swap(t);
	}

	const utf32* Text::data() const noexcept {
		return internal == nullptr ? nullptr : internal->data;
	}

	bool operator==(Text lhs, Text rhs) {
		if (lhs.hash() == rhs.hash() && lhs.size() == rhs.size()) {
			auto p1 = lhs.data();
			auto p2 = rhs.data();
			auto sz = lhs.size();
			for (int64 i = 0; i < sz; i++) {
				if (p1[i] != p2[i]) {
					return false;
				}
			}
			return true;
		}
		return false;
	}

	utf32 Text::operator[](int64 i) const {
		return internal->data[i];
	}
	utf32 Text::at(int64 i) const {
		return internal->data[i];
	}

	bool Text::isEmpty() const noexcept {
		return size() == 0;
	}
	bool Text::isNull() const noexcept {
		return internal == nullptr;
	}

	std::strong_ordering operator<=>(Text lhs, Text rhs) {
		if (lhs == rhs) return std::strong_ordering::equivalent;
		auto lsz = lhs.size();
		auto rsz = rhs.size();
		auto msz = lsz < rsz ? lsz : rsz;
		auto p1 = lhs.data();
		auto p2 = rhs.data();

		for (int64 i = 0; i < msz; i++) {
			if (p1[i] < p2[i]) {
				return std::strong_ordering::less;
			}
			else if (p1[i] > p2[i]) {
				return std::strong_ordering::greater;
			}
		}

		return lsz <=> rsz;
	}

	const Allocator& Text::getAllocator() const {
		return allocator;
	}
}