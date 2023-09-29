module;
#include <bit>
#include <stdlib.h>
export module ntl.functional.hash;
import ntl.concepts;
import ntl.type_traits;
import ntl.utils.nint;
import ntl.utils.swap;
import ntl.utils.is_constant_evaluated;

namespace ne
{
	// Code from https://github.com/google/cityhash/
	inline constexpr uint64 K0 = 0xc3a5c85c97cb3127;
	inline constexpr uint64 K1 = 0xb492b66fbe98f273;
	inline constexpr uint64 K2 = 0x9ae16a3b2f90404f;
	inline constexpr uint64 K3 = 0xc949d7c7509e6557;
	inline constexpr uint64 KMUL = 0x9ddfea08eb382d69;

	inline constexpr uint64 Rotate(uint64 val, int shift)
	{
		return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
	}
	inline constexpr uint64 Fetch64(const uint8* bytes)
	{
		union UINT64_PACK
		{
			uint64 v;
			struct {
				uint8 b7;
				uint8 b6;
				uint8 b5;
				uint8 b4;
				uint8 b3;
				uint8 b2;
				uint8 b1;
				uint8 b0;
			};

		} u64pack;

		if constexpr (std::endian::native == std::endian::big)
		{
			u64pack.b0 = bytes[0];
			u64pack.b1 = bytes[1];
			u64pack.b2 = bytes[2];
			u64pack.b3 = bytes[3];
			u64pack.b4 = bytes[4];
			u64pack.b5 = bytes[5];
			u64pack.b6 = bytes[6];
			u64pack.b7 = bytes[7];
		}
		else
		{
			u64pack.b0 = bytes[7];
			u64pack.b1 = bytes[6];
			u64pack.b2 = bytes[5];
			u64pack.b3 = bytes[4];
			u64pack.b4 = bytes[3];
			u64pack.b5 = bytes[2];
			u64pack.b6 = bytes[1];
			u64pack.b7 = bytes[0];
		}

		return u64pack.v;
	}
	inline constexpr uint32 Fetch32(const uint8* bytes)
	{
		union UINT32_PACK
		{
			uint32 v;
			struct {
				uint8 b3;
				uint8 b2;
				uint8 b1;
				uint8 b0;
			};
		} u32pack;

		if constexpr (std::endian::native==std::endian::big)
		{
			u32pack.b0 = bytes[0];
			u32pack.b1 = bytes[1];
			u32pack.b2 = bytes[2];
			u32pack.b3 = bytes[3];
		}
		else
		{
			u32pack.b0 = bytes[3];
			u32pack.b1 = bytes[2];
			u32pack.b2 = bytes[1];
			u32pack.b3 = bytes[0];
		}

		return u32pack.v;
	}

	inline constexpr uint64 ShiftMix(uint64 val) {
		return val ^ (val >> 47);
	}

	inline constexpr void ByteSwap(uint8& a, uint8& b)
	{
		auto t = a;
		a = b;
		b = t;
	}

	inline constexpr uint64 BitSwap64(uint64 x)
	{
		if (IsConstantEvaluated())
		{
			union U64_PACK
			{
				uint64 val;
				struct
				{
					uint8 b7;
					uint8 b6;
					uint8 b5;
					uint8 b4;
					uint8 b3;
					uint8 b2;
					uint8 b1;
					uint8 b0;
				};
			};
			U64_PACK p1;
			p1.val = x;
			ByteSwap(p1.b7, p1.b0);
			ByteSwap(p1.b6, p1.b1);
			ByteSwap(p1.b5, p1.b2);
			ByteSwap(p1.b4, p1.b3);
			return p1.val;
		}
		else
		{
			// MSVC
			return _byteswap_uint64(x);
		}
	}

	inline constexpr uint64 HashLen16(uint64 u, uint64 v, uint64 mul) {
		// Murmur-inspired hashing.
		uint64 a = (u ^ v) * mul;
		a ^= (a >> 47);
		uint64 b = (v ^ a) * mul;
		b ^= (b >> 47);
		b *= mul;
		return b;
	}

	inline constexpr uint64 HashLen0to16(const uint8* bytes, size_t len)
	{
		if (len >= 8)
		{
			uint64 mul = K2 + len * 2;
			uint64 a = Fetch64(bytes) + K2;
			uint64 b = Fetch64(bytes + len - 8);
			uint64 c = Rotate(b, 37) * mul + a;
			uint64 d = (Rotate(a, 25) + b) * mul;
			return HashLen16(c, d, mul);
		}
		if (len >= 4)
		{
			uint64 mul = K2 + len * 2;
			uint64 a = Fetch32(bytes);
			return HashLen16(len + (a << 3), Fetch32(bytes + len - 4), mul);
		}
		if (len >= 0)
		{
			uint8 a = static_cast<uint8>(bytes[0]);
			uint8 b = static_cast<uint8>(bytes[len >> 1]);
			uint8 c = static_cast<uint8>(bytes[len - 1]);
			uint32 y = static_cast<uint32>(a) + (static_cast<uint32>(b) << 8);
			uint32 z = static_cast<uint32>(len) + (static_cast<uint32>(c) << 2);
			return ShiftMix(y * K2 ^ z * K0) * K2;
		}
		return K2;
	}

	inline constexpr uint64 HashLen17to32(const uint8* s, size_t len) {
		uint64 mul = K2 + len * 2;
		uint64 a = Fetch64(s) * K1;
		uint64 b = Fetch64(s + 8);
		uint64 c = Fetch64(s + len - 8) * mul;
		uint64 d = Fetch64(s + len - 16) * K2;
		return HashLen16(Rotate(a + b, 43) + Rotate(c, 30) + d,
			a + Rotate(b + K2, 18) + c, mul);
	}

	inline constexpr uint64 HashLen33to64(const uint8* s, size_t len) {
		uint64 mul = K2 + len * 2;
		uint64 a = Fetch64(s) * K2;
		uint64 b = Fetch64(s + 8);
		uint64 c = Fetch64(s + len - 24);
		uint64 d = Fetch64(s + len - 32);
		uint64 e = Fetch64(s + 16) * K2;
		uint64 f = Fetch64(s + 24) * 9;
		uint64 g = Fetch64(s + len - 8);
		uint64 h = Fetch64(s + len - 16) * mul;
		uint64 u = Rotate(a + g, 43) + (Rotate(b, 30) + c) * 9;
		uint64 v = ((a + g) ^ d) + f + 1;
		uint64 w = BitSwap64((u + v) * mul) + h;
		uint64 x = Rotate(e + f, 42) + c;
		uint64 y = (BitSwap64((v + w) * mul) + g) * mul;
		uint64 z = e + f + c;
		a = BitSwap64((x + z) * mul + y) + b;
		b = ShiftMix((z + a) * mul + d + h) * mul;
		return b + x;
	}

	struct U128
	{
		uint64 low;
		uint64 high;
	};

	inline constexpr U128 WeakHashLen32WithSeeds(uint64 w, uint64 x, uint64 y, uint64 z, uint64 a, uint64 b) {
		a += w;
		b = Rotate(b + a + z, 21);
		uint64 c = a;
		a += x;
		a += y;
		b += Rotate(a, 44);
		return U128{ a + z, b + c };
	}

	inline constexpr U128 WeakHashLen32WithSeeds(const uint8* s, uint64 a, uint64 b) {
		return WeakHashLen32WithSeeds(Fetch64(s),
			Fetch64(s + 8),
			Fetch64(s + 16),
			Fetch64(s + 24),
			a,
			b);
	}

	inline constexpr uint64 Hash128to64(const U128& x) {
		// Murmur-inspired hashing.
		const uint64 kMul = 0x9ddfea08eb382d69ULL;
		uint64 a = (x.low ^ x.high) * kMul;
		a ^= (a >> 47);
		uint64 b = (x.high ^ a) * kMul;
		b ^= (b >> 47);
		b *= kMul;
		return b;
	}

	inline constexpr uint64 HashLen16(uint64 u, uint64 v) {
		return Hash128to64(U128{u, v});
	}

	inline constexpr uint64 CityHash64(const uint8* bytes, size_t len)
	{
		if (len <= 32)
		{
			if (len <=16)
			{
				return HashLen0to16(bytes, len);
			}
			else
			{
				return HashLen17to32(bytes, len);
			}
		} else if ( len <= 64)
		{
			return HashLen33to64(bytes, len);
		}

		uint64 x = Fetch64(bytes + len - 40);
		uint64 y = Fetch64(bytes + len - 16) + Fetch64(bytes + len - 56);
		uint64 z = HashLen16(Fetch64(bytes + len - 48) + len, Fetch64(bytes + len - 24));
		auto v = WeakHashLen32WithSeeds(bytes + len - 64, len, z);
		auto w = WeakHashLen32WithSeeds(bytes + len - 32, y + K1, x);
		x = x * K1 + Fetch64(bytes);

		len = (len - 1) & ~static_cast<size_t>(63);
		do {
			x = Rotate(x + y + v.low + Fetch64(bytes + 8), 37) * K1;
			y = Rotate(y + v.high + Fetch64(bytes + 48), 42) * K1;
			x ^= w.high;
			y += v.low + Fetch64(bytes + 40);
			z = Rotate(z + w.low, 33) * K1;
			v = WeakHashLen32WithSeeds(bytes, v.high * K1, x + w.low);
			w = WeakHashLen32WithSeeds(bytes + 32, z + w.high, y + Fetch64(bytes + 16));
			utils::Swap(z, x);
			bytes += 64;
			len -= 64;
		} while (len != 0);
		return HashLen16(HashLen16(v.low, w.low) + ShiftMix(y) * K1 + z,
			HashLen16(v.high, w.high) + x);
	}
}

export namespace ne
{
	using HashValue = uint64;
	template<class T>
	struct Hash
	{
		constexpr HashValue operator()(const T& val) const noexcept
		{
			static_assert(AlWAYS_FALSE<T>, "[ntl.functional.hash] Type does not have a hash functor.");
			return 0;
		}
	};

	template<ConceptIntegral T>
	struct Hash<T>
	{
		constexpr HashValue operator()(T val) const noexcept
		{
			return static_cast<HashValue>(val);
		}
	};

	template<>
	struct Hash<const char*>
	{
		constexpr HashValue operator()(const char* str)
		{
			size_t len = 0; while (str[len] != '\0') ++len;
			return CityHash64(reinterpret_cast<const uint8*>(str), len);
		}
	};

	template<class T>
	struct Hash<T*>
	{
		constexpr HashValue operator()(T* p)
		{
			union
			{
				T* t;
				size_t a;
			} u;
			u.t = p;
			return CityHash64(&u, sizeof(u));
		}
	};
}
