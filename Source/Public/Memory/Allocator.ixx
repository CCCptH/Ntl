module;
#include <cstdlib>
export module ntl.memory.allocator;
import ntl.memory.memory_arena;
import ntl.utils.nint;
import ntl.memory.bad_alloc;

export namespace ne {

    class __declspec(dllexport) Allocator
    {
    private:
        MemoryArena* arena;
    public:
//#ifdef NE_DEBUG
        // static int COUNT;
//#endif
        constexpr Allocator()noexcept: arena(nullptr) {};
        constexpr Allocator(MemoryArena* arena) noexcept : arena(arena) {}
        constexpr ~Allocator()noexcept = default;

        constexpr Allocator(const Allocator&) noexcept = default;
        constexpr Allocator(Allocator&&) noexcept = default;
        constexpr Allocator& operator=(const Allocator&) noexcept = default;
        constexpr Allocator& operator=(Allocator&&) noexcept = default;

        // TODO: 1. align \n
        //       2. malloc to mimalloc
        template<typename T>
        constexpr T* allocate(size_t n=1) {
            if (arena==nullptr)
            {
                //T* ptr = (T*)malloc(n * sizeof(T));
                T* ptr = reinterpret_cast<T*>(new uint8[n * sizeof(T)]);
                if (ptr!=nullptr)
                {
					return ptr;
                }
            }
            else
            {
                T* ptr = (T*)arena->alloc(sizeof(T) * n, alignof(T));
                if (ptr!=nullptr)
                {
                    return ptr;
                }
            }
            throw BadAlloc(n * sizeof(T));
        }

        template<typename T>
        T* allocate(size_t n, align_t align) {
            return allocate<T>(n);
        }

        template<typename T>
        constexpr void deallocate(T* t) {
//            arena->doFree(t);
            // --COUNT;
            //free(t);
            delete[] reinterpret_cast<uint8*>(t);
        }

        constexpr friend bool operator==(const Allocator& a, const Allocator& b) {
            return a.arena==b.arena;
        }
    };

    template<typename T>
    T* Allocate(size_t n=1, align_t align=alignof(T))
    {
        Allocator a{};
        return a.allocate<T>(n, align);
    }

    template<typename T>
    void Deallocate(T* ptr)
    {
        Allocator a{};
        a.deallocate(ptr);
    }

    template<typename T, typename ...Args>
    T* ConstructN(T* p, size_t n, Args&&...args) {
        auto pp = p;
        for (; n>0; n--) {
            new(pp++) T(Forward<Args>(args)...);
        }
        return p;
    }

    template<typename T, typename ...Args>
    T* Construct(T* p, Args&&...args) {
        return new(p) T(Forward<Args>(args)...);
    }

    template<typename T>
    void Destruct(T* p) {
        p->~T();
    }
    template<typename T>
    void DestructN(T* p, size_t n) {
        for (; n>0; n--) {
            p->~T();
            ++p;
        }
    }

    template<class T, class ...Args>
    T* AllocateNew(Args&& ...args)
    {
        auto ptr = Allocate<T>(1);
        Construct(ptr, Forward<Args>(args)...);
        return ptr;
    }
    template<class T, class ...Args>
    T* AllocateNewN(size_t n, Args&& ...args)
    {
        auto ptr = Allocate<T>(n);
        ConstructN(ptr, n, Forward<Args>(args)...);
        return ptr;
    }

    template<class T>
    void DeallocateDelete(T* ptr)
    {
        Destruct(ptr);
        Deallocate(ptr);
    }

    template<class T>
    void DeallocateDeleteN(T* ptr, size_t n)
    {
        DestructN(ptr, n);
        Deallocate(ptr);
    }
}