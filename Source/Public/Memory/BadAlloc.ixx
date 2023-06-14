module;
#include <cstdio>
#include <exception>
export module ntl.memory.bad_alloc;
import ntl.utils.nint;
export namespace ne
{
	class BadAlloc: public std::exception
	{
	private:
		uint64 alloc_size;
		mutable char buf[128]={'\0'};
	public:
		BadAlloc(): alloc_size(0) {}
		BadAlloc(uint64 try_alloc_size): alloc_size(try_alloc_size) {}
		BadAlloc(const BadAlloc&) = default;
		BadAlloc(BadAlloc&&) = default;
		~BadAlloc() noexcept = default;
		const char* what() const noexcept
		{
			sprintf_s(buf, 128, "[ntl.memory] Allocation failed. Try to allocate " "[ntl.memory] Allocation failed.Try to allocate %llu bytes", alloc_size);
			return buf;
		}
	};
}