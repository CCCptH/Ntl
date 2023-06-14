export module ntl.string.unicode;
import ntl.utils;
import ntl.optional;
import ntl.iterator;
import ntl.expected;

export namespace ne
{
	template<class It>
	concept ConceptCodepointIterator = ConceptInputIterator<It> and (TestIsSame<TypeUnCVRef<TypeIteratorValue<It>>, utf32> or TestIsSame<TypeUnCVRef<TypeIteratorValue<It>>, uint32>);
	template<class It>
	concept ConceptByteIterator = ConceptInputIterator<It> and (TestIsSame<TypeUnCVRef<TypeIteratorValue<It>>, char> or TestIsSame<TypeUnCVRef<TypeIteratorValue<It>>, utf8> or TestIsSame<TypeUnCVRef<TypeIteratorValue<It>>, uint8>);


	enum class GraphemeClusterBreakProperty : uint8
	{
		CR = 0,
		LF = 1,
		CONTROL = 2,
		EXTEND = 3,
		ZWJ = 4,
		REGIONAL_INDICATOR = 5,
		PREPEND = 6,
		SPACINGMARK = 7,
		L = 8,
		V = 9,
		T = 10,
		LV = 11,
		LVT = 12,
		EXTENDED_PICTOGRAPHIC = 13,
		NONE,
		//SOT,
		//EOF,
	};

	/**
	 * @brief Query grapheme cluster break property for text segmentation. \n
	 * Check https://www.unicode.org/reports/tr29/tr29-39.html#State_Machines
	 * @param codepoint 
	 * @return 
	*/
	GraphemeClusterBreakProperty QueryGraphemeClusterBreakProperty(utf32 codepoint);

	/**
	 * @brief Count how many codepoints from a range of utf8 encoding bytes. If is not a valid utf8 encoding sequence, return -1;
	 * @param first 
	 * @param last 
	 * @return 
	*/
	int64 CountCodepointsOfUtf8(ConceptByteIterator auto first, ConceptByteIterator auto last);

	int64 CountUtf8sOfCodepoints(ConceptCodepointIterator auto first, ConceptCodepointIterator auto last);

	int64 IsValidUnicode8Encoding(ConceptByteIterator auto first, ConceptByteIterator auto last);

	void Utf8ToUtf32(auto back_inserter, ConceptByteIterator auto first, ConceptByteIterator auto last);

	void Utf32ToUtf8(auto back_inserter, ConceptCodepointIterator auto first, ConceptCodepointIterator auto last);

	void Utf8ToUtf32(utf32* buf, ConceptByteIterator auto first, ConceptByteIterator auto last);

	void Utf32ToUtf8(char* buf, ConceptCodepointIterator auto first, ConceptCodepointIterator auto last);
}

namespace ne
{
	int64 CountCodepointsOfUtf8(ConceptByteIterator auto first, ConceptByteIterator auto last)
	{
		int64 len = 0;
		while(first < last)
		{
			uint8 c = static_cast<uint8>(*first);
			if (c <= 0b01111111)
			{
				++first;
				++len;
			}
			else if (c >= 0b11000000 && c <= 0b11011111)
			{
				iters::Advance(first, 2);
				++len;
			}
			else if (c >= 0b11100000 && c <= 0b11101111)
			{
				iters::Advance(first, 3);
				++len;
			}
			else if (c >= 0b11110000 && c <= 0b11110111)
			{
				iters::Advance(first, 4);
				++len;
			}
			else
			{
				return -1;
			}
		}
		return len;
	}

	int64 CountUtf8sOfCodepoints(ConceptCodepointIterator auto first, ConceptCodepointIterator auto last)
	{
		int64 len = 0;
		while (first<last)
		{
			utf32 c = *first;
			if (c < 0x80)
			{
				len += 1;
			}
			else if (c<0x800)
			{
				len += 2;
			}
			else if (c<0x8000)
			{
				len += 3;
			}
			else if (c<=0x10ffff)
			{
				len += 4;
			}
			else
			{
				return -1;
			}
			++first;
		}
		return len;
	}

	int64 IsValidUnicode8Encoding(ConceptByteIterator auto first, ConceptByteIterator auto last)
	{
		return CountCodepointsOfUtf8(first, last) != -1;
	}

	void Utf8ToUtf32(auto back_inserter, ConceptByteIterator auto first, ConceptByteIterator auto last)
	{
		while(first < last)
		{
			uint8 c = static_cast<uint8>(*first);
			utf32 code = 0;
			if (c <= 0b01111111)
			{
				code = c;
			}
			else if (c >= 0b11000000 && c <= 0b11011111)
			{
				code = uint32(0b00011111 & c) << 6;
				c = *first; ++first;
				code |= uint32(0b00111111 & c);
			}
			else if (c >= 0b11100000 && c <= 0b11101111)
			{
				code = uint32(0b00001111 & c) << 12;
				c = *first; ++first;
				code |= uint32(0b00111111 & c) << 6;
				c = *first; ++first;
				code |= uint32(0b00111111 & c);
			}
			//else if (c >= 0b11110000 && c <= 0b11110111)
			else
			{
				code = uint32(0b00000111 & c) << 18;
				c = *first; ++first;
				code |= uint32(0b00111111 & c) << 12;
				c = *first; ++first;
				code |= uint32(0b00111111 & c) << 6;
				c = *first; ++first;
				code |= uint32(0b00111111 & c);
			}
			*back_inserter = code;
		}
	}

	void Utf32ToUtf8(auto back_inserter, ConceptCodepointIterator auto first, ConceptCodepointIterator auto last)
	{
		while (first < last)
		{
			uint32 codepoint = static_cast<uint32>(*first);
			++first;
			if (codepoint < 0x80)
			{
				*back_inserter = codepoint & 0x7f;
			}
			else if (codepoint < 0x800)
			{
				*back_inserter = 0xC0 | (codepoint >> 6);
				*back_inserter = 0x80 | (codepoint & 0x3F);
			}
			else if (codepoint < 0x8000)
			{
				*back_inserter = 0xE0 | (codepoint >> 12);
				*back_inserter = 0x80 | ((codepoint >> 6) & 0x3F);
				*back_inserter = 0x80 | (codepoint & 0x3F);
			}
			//else if (codepoint <= 0x10FFFF)
			else
			{
				*back_inserter = 0xF0 | (codepoint >> 18);
				*back_inserter = 0x80 | ((codepoint >> 12) & 0x3F);
				*back_inserter = 0x80 | ((codepoint >> 6) & 0x3F);
				*back_inserter = 0x80 | (codepoint & 0x3F);
			}
		}
	}

	void Utf8ToUtf32(utf32* buf, ConceptByteIterator auto first, ConceptByteIterator auto last)
	{
		int64 n=0;
		while (first < last)
		{
			uint8 c = static_cast<uint8>(*first);
			++first;
			utf32 code = 0;
			if (c <= 0b01111111)
			{
				code = c;
			}
			else if (c >= 0b11000000 && c <= 0b11011111)
			{
				code = uint32(0b00011111 & c) << 6;
				c = *first; ++first;
				code |= uint32(0b00111111 & c);
			}
			else if (c >= 0b11100000 && c <= 0b11101111)
			{
				code = uint32(0b00001111 & c) << 12;
				c = *first; ++first;
				code |= uint32(0b00111111 & c) << 6;
				c = *first; ++first;
				code |= uint32(0b00111111 & c);
			}
			//else if (c >= 0b11110000 && c <= 0b11110111)
			else
			{
				code = uint32(0b00000111 & c) << 18;
				c = *first; ++first;
				code |= uint32(0b00111111 & c) << 12;
				c = *first; ++first;
				code |= uint32(0b00111111 & c) << 6;
				c = *first; ++first;
				code |= uint32(0b00111111 & c);
			}
			*(buf+n) = code;
			++n;
		}
	}

	void Utf32ToUtf8(char* buf, ConceptCodepointIterator auto first, ConceptCodepointIterator auto last)
	{
		int64 n;
		while (first < last)
		{
			uint32 codepoint = static_cast<uint32>(*first);
			if (codepoint < 0x80)
			{
				*(buf+n) = codepoint & 0x7f;
				++n;
			}
			else if (codepoint < 0x800)
			{
				*(buf + n) = 0xC0 | (codepoint >> 6);
				++n;
				*(buf + n) = 0x80 | (codepoint & 0x3F);
				++n;
			}
			else if (codepoint < 0x8000)
			{
				*(buf + n) = 0xE0 | (codepoint >> 12);
				++n;
				*(buf + n) = 0x80 | ((codepoint >> 6) & 0x3F);
				++n;
				*(buf + n) = 0x80 | (codepoint & 0x3F);
				++n;
			}
			//else if (codepoint <= 0x10FFFF)
			else
			{
				*(buf + n) = 0xF0 | (codepoint >> 18);
				++n;
				*(buf + n) = 0x80 | ((codepoint >> 12) & 0x3F);
				++n;
				*(buf + n) = 0x80 | ((codepoint >> 6) & 0x3F);
				++n;
				*(buf + n) = 0x80 | (codepoint & 0x3F);
				++n;
			}
		}
	}

}

export namespace ne::deprecated
{
	enum class GraphemeClusterBreakProperty: uint8
	{
		CR = 0,
		LF = 1,
		CONTROL = 2,
		EXTEND = 3,
		ZWJ = 4,
		REGIONAL_INDICATOR = 5,
		PREPEND = 6,
		SPACINGMARK = 7,
		L = 8,
		V = 9,
		T = 10,
		LV = 11,
		LVT = 12,
		EXTENDED_PICTOGRAPHIC = 13,
		NONE,
		//SOT,
		//EOF,
	};

	GraphemeClusterBreakProperty QueryGraphemeClusterProperty(utf32 code_point);

	// Now it will terminate when encoding is invalid in debug mode.
	// TODO: delete assert
	template<class It>
	struct NextCodePointResult
	{
		It iterator;
		utf32 codepoint;
	};
	template<class It>
		requires TestIsSame<TypeIteratorValue<It>, char>
	Optional<NextCodePointResult<It>> NextCodePoint(It it)
	{
		uint8 c = (uint8)(*it);
		++it;
		utf32 code = 0;
		if (c<=0b01111111)
		{
			code = c;
		}
		else if (c>=0b11000000 && c<=0b11011111)
		{
			code = uint32(0b00011111 & c) << 6;
			c = *it; ++it;
			//DAssert((c & 0b11000000)==0b10000000, "[ntl.string.unicode] Invalid encoding");
			code |= uint32(0b00111111 & c);
		}
		else if (c>=0b11100000 && c<=0b11101111)
		{
			code = uint32(0b00001111 & c) << 12;
			c = *it; ++it;
			//DAssert((c & 0b11000000) == 0b10000000, "[ntl.string.unicode] Invalid encoding");
			code |= uint32(0b00111111 & c) << 6;
			c = *it; ++it;
			//DAssert((c & 0b11000000) == 0b10000000, "[ntl.string.unicode] Invalid encoding");
			code |= uint32(0b00111111 & c);
		}
		else if (c>=0b11110000 && c<=0b11110111)
		{
			code = uint32(0b00000111 & c) << 18;
			c = *it; ++it;
			//DAssert((c & 0b11000000) == 0b10000000, "[ntl.string.unicode] Invalid encoding");
			code |= uint32(0b00111111 & c) << 12;
			c = *it; ++it;
			//DAssert((c & 0b11000000) == 0b10000000, "[ntl.string.unicode] Invalid encoding");
			code |= uint32(0b00111111 & c) << 6;
			c = *it; ++it;
			//DAssert((c & 0b11000000) == 0b10000000, "[ntl.string.unicode] Invalid encoding");
			code |= uint32(0b00111111 & c);
		}
		else
		{
			return NULLOPT;
		}
		return NextCodePointResult<It>{ it, code };
	}

	enum class GraphemeClusterBreakError
	{
		ERROR_ENCODEING,
		BREAK_OUT_OF_EOT
	};

	template<class It>
		requires TestIsSame<TypeIteratorValue<It>, char>
	Expected<It, GraphemeClusterBreakError> NextGraphemeClusterBreak(It sot, It eot)
	{
		using GCBP = GraphemeClusterBreakProperty;
		using GCBE = GraphemeClusterBreakError;
		auto it = sot;
		int8 curr_stat = -1;
		utf32 curr_code = 0;
		auto opt = NextCodePoint(it);
		if (!opt) [[unlikely]] return Unexpected<GraphemeClusterBreakError>{GCBE::ERROR_ENCODEING};

		it = opt->iterator;
		curr_code = opt->codepoint;
		curr_stat = (int8)QueryGraphemeClusterProperty(curr_code);

		while (true)
		{
			// eot
			if (it>=eot) 
			{
				if (it==eot)
				{
					return eot;
				}
				return Unexpected<GraphemeClusterBreakError>{GCBE::BREAK_OUT_OF_EOT};
			}

			auto next_opt = NextCodePoint(it);
			if (!next_opt) return Unexpected<GraphemeClusterBreakError>{GCBE::ERROR_ENCODEING};

			auto next_code = next_opt->codepoint;
			auto next_prop = QueryGraphemeClusterProperty(next_code);

			if (curr_stat == (int8)GCBP::CR)
			{
				if (next_prop == GCBP::LF)
				{
					curr_stat = (int8)GCBP::LF;
					goto next_iter;
				}
			}
			if (curr_stat==(int8)GCBP::CONTROL ||
				curr_stat==(int8)GCBP::CR ||
				curr_stat==(int8)GCBP::LF
				)
			{
				return it;
			}
			if (true)
			{
				if (next_prop == GCBP::CONTROL ||
					next_prop == GCBP::CR ||
					next_prop == GCBP::LF)
				{
					return it;
				}
			}
			if (curr_stat==(int8)GCBP::L)
			{
				if (next_prop==GCBP::L ||
					next_prop==GCBP::LV ||
					next_prop==GCBP::LVT ||
					next_prop==GCBP::V)
				{
					curr_stat = (int)next_prop;
					goto next_iter;
				}
			}
			if (curr_stat==(int)GCBP::LV ||
				curr_stat==(int)GCBP::V)
			{
				if (next_prop == GCBP::T ||
					next_prop == GCBP::V)
				{
					curr_stat = (int)next_prop;
					goto next_iter;
				}
			}
			if (curr_stat == (int)GCBP::LVT ||
				curr_stat == (int)GCBP::T)
			{
				if (next_prop == GCBP::T)
				{
					curr_stat = (int)next_prop;
					goto next_iter;
				}
			}

			if (true)
			{
				if (next_prop==GCBP::SPACINGMARK)
				{
					curr_stat = (int8)next_prop;
					goto next_iter;;
				}
			}
			if (curr_stat==(int8)GCBP::PREPEND)
			{
				curr_stat = (int8)next_prop;
				goto next_iter;
			}
			if (curr_stat==(int8)GCBP::EXTENDED_PICTOGRAPHIC)
			{
				if (next_prop==GCBP::EXTEND)
				{
					curr_stat = 100;
					goto next_iter;
				}
				if (next_prop==GCBP::ZWJ)
				{
					curr_stat = 101;
					goto next_iter;
				}
			}
			if (curr_stat==100)
			{
				if (next_prop==GCBP::EXTEND)
				{
					curr_stat = 100;
					goto next_iter;
				}
				if (next_prop==GCBP::ZWJ)
				{
					curr_stat = 101;
					goto next_iter;
				}
			}
			if (curr_stat==101)
			{
				if (next_prop==GCBP::EXTENDED_PICTOGRAPHIC)
				{
					curr_stat = (int8)next_prop;
					goto next_iter;
				}
			}
			// GB9 After GB11
			if (true)
			{
				if (next_prop == GCBP::EXTEND ||
					next_prop == GCBP::ZWJ)
				{
					curr_stat = (int8)next_prop;
					goto next_iter;
				}
			}
			if (curr_stat==(int8)GCBP::REGIONAL_INDICATOR)
			{
				if (next_prop==GCBP::REGIONAL_INDICATOR)
				{
					curr_stat = (int8)GCBP::NONE;
					goto next_iter;
				}
			}
			return it;

			next_iter:
			{
				curr_code = next_code;
				it = next_opt->iterator;
			}

		}
	}
}
