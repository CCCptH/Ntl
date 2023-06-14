export module ntl.memory.memory_arena;

export namespace ne
{
	using size_t = decltype(sizeof(int));
	using align_t = decltype(alignof(int));
	struct MemoryArena
	{
	public:
		MemoryArena() = default;
		MemoryArena(const MemoryArena&) = default;
		MemoryArena(MemoryArena&&) = default;
		~MemoryArena() = default;

		virtual void* alloc(size_t n, align_t align_size) = 0;
		virtual void dealloc(void*) = 0;
	};
};
