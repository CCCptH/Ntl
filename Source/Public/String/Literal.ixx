module;
#include <cstring>
export module ntl.string.string_literal;
//import ntl.exceptions;
import ntl.utils;
import ntl.utils.debug;
import ntl.iterator.reverse_iterator;
import ntl.compare;
import ntl.functional.hash;

/*
constexpr inline ne::int64 ConstStrLen(const char* str)
{
	ne::int64 n = 0;
	while (*str!='\0')
	{
		++n;
		++str;
	}
	return n;
}
*/
//export namespace ne
//{
//	class String;
	/**
	 * @brief Utilities for const char* that compute in compile time.
	*/
/*
	class Literal
	{
	private:
		const char* thestr;
		int64 b;
		int64 e;
	public:
		friend class String;
		using SizeType = int64;
		using Iterator = const char*;
		using ConstIterator = const char*;
		using ReverseIterator = ReverseIterator<Iterator>;
		using ConstReverseIterator = ReverseIterator;

		static constexpr SizeType NPOS = -1;

		constexpr Literal() noexcept
		{
			if (! IsConstantEvaluated())
			{
				Assert(false, "[ntl.string.string_literal] Must be constant evaluated.");
			}
			thestr = nullptr;
			b = 0;
			e = 0;
		}
		constexpr Literal(decltype(nullptr)) = delete;
		constexpr Literal(const char* str) noexcept
		{
			if (!IsConstantEvaluated())
			{
				Assert(false, "[ntl.string.string_literal] Must be constant evaluated.");
			}
			thestr = str;
			b = 0;
			e = ConstStrLen(str);
		}
		constexpr Literal(const char* str, int64 count) noexcept
		{
			if (!IsConstantEvaluated())
			{
				Assert(false, "[ntl.string.string_literal] Must be constant evaluated.");
			}
			if constexpr (DEBUG)
			{
				auto len = ConstStrLen(str);
				Assert(len > count, "[ntl.string.literal] Len must be greater than count");
			}
			thestr = str;
			b = 0;
			e = count;
		}
		constexpr ~Literal() = default;
		constexpr Literal& operator=(const Literal&) = default;

		constexpr bool operator==(const Literal& other) const
		{
			if (size() != other.size()) return false;
			auto b1 = thestr + b;
			auto b2 = other.thestr + b;
			auto e1 = thestr + e;
			auto e2 = other.thestr + e;
			while (*b1==*b2 && b1!=e1)
			{
				++b1;
				++b2;
			}
			return b1 == e1;

		}

		constexpr const char* data() const noexcept { return thestr; }

		constexpr SizeType size() const noexcept
		{
			return e - b;
		}

		constexpr bool isEmpty() const noexcept {
			return size() == 0;
		}

		constexpr const char& operator[](SizeType i) const
		{
			return thestr[b+i];
		}
		constexpr const char& at(SizeType i) const
		{
			if (b+i >= e)
			{
				//throw OutOfRange{};
			}
			return thestr[b+i];
		}

		constexpr ConstIterator begin() const { return thestr + b; }
		constexpr ConstIterator end() const { return thestr + e; }
		constexpr ConstIterator cbegin() const { return thestr + b; }
		constexpr ConstIterator cend() const { return thestr + e; }
		constexpr ConstReverseIterator rbegin() const { return ConstReverseIterator(end()); }
		constexpr ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }
		constexpr ConstReverseIterator crbegin() const { return ConstReverseIterator(end()); }
		constexpr ConstReverseIterator crend() const { return ConstReverseIterator(begin()); }

		// TODO: substr, find, ...
		constexpr Literal substr(SizeType pos, SizeType count = NPOS) const
		{
			Assert(count >= 0 || count == NPOS, "[ntl.string.literal] Count must be greater than 0.");
			auto rc = count < size() - pos ? count : size() - pos;
			Literal n = *this;
			n.b += pos;
			n.e = n.b + count;
			return n;
		}
		constexpr Literal& removePrefix(SizeType n) noexcept
		{
			Assert(size() >= n, "[ntl.string.literal] Param n is invalid");
			b += n;
			return *this;
		}
		constexpr Literal& removeSuffix(SizeType n) noexcept
		{
			Assert(size() >= n, "[ntl.string.literal] Param n is invalid");
			e -= n;
			return *this;
		}

		constexpr void swap(Literal& x) noexcept
		{
			utils::Swap(thestr, x.thestr);
			utils::Swap(b, x.b);
			utils::Swap(e, x.e);
		}
		constexpr friend void Swap(Literal& x, Literal& y) noexcept
		{
			x.swap(y);
		}

		constexpr StrongOrdering compare(Literal str) const noexcept
		{
			auto b1 = thestr + b;
			auto b2 = str.thestr + str.b;
			auto e1 = thestr + e;
			auto e2 = str.thestr + str.e;

			while(*b1==*b2)
			{
				++b1;
				++b2;
				if (b1==e1 && b2 != e2)
				{
					return StrongOrdering::less;
				}
				else if (b2 == e2 && b1 != e1)
				{
					return StrongOrdering::greater;
				}
				else if (b1==e1 && b2==e2)
				{
					return StrongOrdering::equivalent;
				}
			}
			if ((unsigned char)*b1 > (unsigned char)*b2) return StrongOrdering::greater;
			else if ((unsigned char)*b1 < (unsigned char)*b2) return StrongOrdering::less;
			else return StrongOrdering::equivalent;
		}

		constexpr StrongOrdering compare(const char* str) const noexcept
		{
			auto b1 = thestr + b;
			auto b2 = str;
			auto e1 = thestr + e;

			while (*b1 == *b2)
			{
				++b1;
				++b2;
				if (b1 == e1 && *b2=='\0')
				{
					return StrongOrdering::less;
				}
				else if (*b2=='\0' && b1 != e1)
				{
					return StrongOrdering::greater;
				}
				else if (b1 == e1 && *b2=='\0')
				{
					return StrongOrdering::equivalent;
				}
			}
			if ((unsigned char)*b1 > (unsigned char)*b2) return StrongOrdering::greater;
			else if ((unsigned char)*b1 < (unsigned char)*b2) return StrongOrdering::less;
			else return StrongOrdering::equivalent;
		}

		constexpr StrongOrdering operator<=>(const Literal& str) const noexcept
		{
			return this->compare(str);
		}
		constexpr StrongOrdering operator<=>(const char* str) const noexcept
		{
			return this->compare(str);
		}

		constexpr bool startWith(Literal str) const noexcept
		{
			auto b1 = thestr + b;
			auto b2 = str.thestr + str.b;
			auto n = str.size();
			if (size() < n) return false;
			for (;n>0;n--)
			{
				if (*b1 != *b2) return false;
				++b1;
				++b2;
			}
			return true;
		}

		constexpr bool startWith(const char* str) const noexcept
		{
			return startWith(Literal{ str });
		}

		constexpr bool startWith(char ch) const noexcept
		{
			if (b == e) return false;
			return thestr[b] == ch;
		}

		constexpr bool endWith(Literal str) const noexcept
		{
			auto b1 = thestr + e;
			auto b2 = str.thestr + str.e;
			auto n = str.size();
			if (size() < n) return false;
			for (; n > 0; n--)
			{
				if (*b1 != *b2) return false;
				--b1;
				--b2;
			}
			return true;
		}

		constexpr bool endWith(const char* str) const
		{
			return endWith(Literal{ str });
		}

		constexpr bool endWith(char ch) const noexcept
		{
			if (b == e) return false;
			return thestr[e-1] == ch;
		}

		constexpr bool contains(Literal str) const noexcept 
		{
			auto n = size();
			auto strsz = str.size();
			if (strsz > n) return false;
			auto b1 = thestr + b;
			auto b2 = str.thestr + str.b;
			for(SizeType i=0; i<n-strsz;i++)
			{
				auto p1 = b1+i;
				auto p2 = b2;
				bool ok = true;
				for(SizeType j=0;j<strsz;j++)
				{
					if (p1[j] != p2[j]) {
						ok = false;
						break;
					}
				}
				if (ok == true)
					return true;
			}
			return false;
		}

		constexpr bool contains(const char* str) const
		{
			return contains(Literal{ str });
		}

		constexpr bool contains(char ch) const noexcept
		{
			return find(ch) != NPOS;
		}

		constexpr SizeType find(Literal str, SizeType pos=0) const noexcept
		{
			auto s1 = size();
			auto s2 = str.size();
			if (s2 > s1-pos) return NPOS;
			auto b1 = thestr + b + pos;
			auto b2 = str.thestr + str.b;

			for(SizeType i=0;i<s1-pos-s2;i++)
			{
				auto p1 = b1 + i;
				auto p2 = b2;
				bool ok = true;
				for(SizeType j=0;j<s2;j++)
				{
					if (p1[j] != p2[j]) {
						ok = false;
						break;
					}
				}
				if (ok == true)
					return i + pos;
			}
			return NPOS;
		}

		constexpr SizeType find(const char* str, SizeType pos=0) const noexcept
		{
			return find(Literal{ str }, pos);
		}

		constexpr SizeType find(const char* str, SizeType pos, SizeType count) const noexcept
		{
			return find(Literal{ str, count }, pos);
		}

		constexpr SizeType find(char ch, SizeType pos=0) const noexcept
		{
			auto bp = thestr + b;
			for (SizeType i=0;i<size();i++)
			{
				if (bp[i] == ch) return i;
			}
			return NPOS;
		}

		constexpr SizeType rfind(Literal str, SizeType pos=NPOS) const noexcept
		{
			auto s1 = size();
			auto s2 = str.size();
			pos = (s1 + pos) % s1;
			if (s2 > pos) return NPOS;
			auto b1 = thestr + b;
			auto b2 = str.thestr + str.b;

			for (SizeType i=pos-s2+1;i>=0;i--)
			{
				auto p1 = b1 + i;
				auto p2 = b2;
				bool ok = true;
				for (SizeType j = 0; j < s2; j++)
				{
					if (p1[j] != p2[j]) {
						ok = false;
						break;
					}
				}
				if (ok == true)
					return i;
			}
			return NPOS;
		}

		constexpr SizeType rfind(const char* str, SizeType pos=NPOS) const noexcept
		{
			return rfind(Literal{ str }, pos);
		}

		constexpr SizeType rfind(const char* str, SizeType pos, SizeType count) const noexcept
		{
			return rfind(Literal{ str, count }, pos);
		}

		constexpr SizeType rfind(char ch, SizeType pos=NPOS) const noexcept
		{
			pos = (size() + pos) % size();
			auto bp = thestr + b;
			for(SizeType i=pos;i>=0;i--)
			{
				if (bp[i] == ch) return i;
			}
			return NPOS;
		}

	};

	template<>
	struct Hash<Literal>
	{
		constexpr HashValue operator()(const Literal& str) const noexcept
		{
			HashValue hv = 0;
			HashValue b = 1;
			uint64 sz = str.size();
			for (uint64 i=0; i<sz;++i)
			{
				hv += (uint8)(str[i]) * b;
				b = b << 1;
			}
			return hv;
		}
	};
}
*/