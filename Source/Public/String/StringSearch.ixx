export module ntl.string.string_search;
import ntl.utils;

namespace ne
{
	template<class It1, class It2>
	It2 BruteForceSearch(It1 pf, It1 pl, It2 tf, It2 tl);
	template<class It1, class It2>
	It2 BoyerMooreHorspoolSearch(It1 pf, It1 pl, It2 tf, It2 tl);
}

export namespace ne
{
	template<class It1, class It2>
	It2 StringSearch(It1 pattern_first, It1 pattern_last, It2 string_first, It2 string_last)
	{
		auto plen =  pattern_last - pattern_first;
		if (plen==0)
		{
			return string_first;
		}
		else if (plen <= 8 && plen>0)
		{
			return BruteForceSearch(pattern_first, pattern_last, string_first, string_last);
		}
		else if (plen <= 256 && plen>8)
		{
			return BoyerMooreHorspoolSearch(pattern_first, pattern_last, string_first, string_last);
		}
		else
		{
			return string_last;
		}
	}
}

namespace ne
{
	template<class It1, class It2>
	It2 BruteForceSearch(It1 pf, It1 pl, It2 tf, It2 tl)
	{
		auto plen = pl - pf;
		if (plen==1)
		{
			for (; tf != tl && *tf != *pf; ++tf);
			return tf;
		}
		else if (plen==2)
		{
			// glibc code
			uint32 p = ((uint32)(*pf) << 16) | (uint32)(*(pf + 1));
			uint32 t = 0;
			for (uint32 c = *tf; p != t && tf != tl; c = *++tf)
				t = (t  << 16) | c;
			return p == t ? tf - 2 : tl;
		}

		uint64 p = 0;
		uint64 t = 0;
		uint64 m = 0;
		for (auto i = 0ul; i<plen;++i)
		{
			p <<= 8;
			p |= (uint8)(*(pf + i));
			m <<= 8;
			m |= 255;
		}

		for (uint8 c=*tf; p != t && tf!=tl; ++tf)
		{
			c = *tf;
			t <<= 8;
			t |= c;
			t &= m;
		}
		return p == t ? (tf - plen) : tl;

	}

	template<class It1, class It2>
	It2 BoyerMooreHorspoolSearch(It1 pf, It1 pl, It2 tf, It2 tl)
	{
		auto plen = pl - pf;
		auto tlen = tl - tf;
		[[unlikely]]
		if (tlen < plen)
		{
			return tl;
		}
		// TODO: better hash to avoid cache dismiss
		
		auto hash = [](auto x) { return (uint8)(*x); };

		uint8 shift[256] = {0};
		for (auto i=0; i<256;i++)
		{
			shift[i] = plen;
		}
		for(auto i=0;i<plen-1;i++)
		{
			shift[hash(pf + i)] = plen-i-1;
		}

		while(tf <= tl-plen)
		{
			int j = plen-1;
			while(j>=0 && *(tf+j)==*(pf+j))
			{
				--j;
			}
			if (j<0)
			{
				return tf;
			}
			tf += shift[hash(tf + plen - 1)];
		}
		return tl;
		
	}

}

