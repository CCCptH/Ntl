module;
#include <atomic>
export module ntl.memory.shared_ptr;
import ntl.utils;
import ntl.memory.unique_ptr;
import ntl.type_traits;
import ntl.utils.exception_guard;
import ntl.memory.allocator;

export import ntl.memory.default_deleter;

export namespace ne
{
	template<class T>
	class SharedPtr;
	template<class T>
	class WeakPtr;
}

namespace ne
{
	class RefCount
	{
		using Count = std::atomic<int64>;
		Count ref_count = 1;
		Count weak_ref_count = 1;
	public:
		RefCount(int64 ref=1, int64 weak=1) noexcept
			: ref_count(ref), weak_ref_count(weak)
		{}
		virtual ~RefCount() noexcept = default;

		virtual void destroy() = 0;
		virtual void expire() = 0;

		int64 useCount() const noexcept
		{
			return ref_count.load(std::memory_order_relaxed);
		}

		void incRef() noexcept
		{
			ref_count.fetch_add(1, std::memory_order_relaxed);
			weak_ref_count.fetch_add(1, std::memory_order_relaxed);
		}
		void decRef()
		{
			if (ref_count.fetch_sub(1, std::memory_order_release) == 1)
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				destroy();
			}
			decWRef();
		}

		void incWRef()
		{
			weak_ref_count.fetch_add(1, std::memory_order_relaxed);
		}
		void decWRef()
		{
			if (weak_ref_count.fetch_sub(1, std::memory_order_release) == 1) {
				std::atomic_thread_fence(std::memory_order_acquire);
				expire();
			}
		}

		RefCount* lock() noexcept
		{
			for (int64 temp = ref_count.load(std::memory_order_relaxed); temp!=0;)
			{
				if (ref_count.compare_exchange_weak(temp, temp+1, std::memory_order_relaxed))
				{
					weak_ref_count.fetch_add(1, std::memory_order_relaxed);
					return this;
				}
			}
			return nullptr;
		}
	};

	template<class T, class D>
	class RefCountAllocator: public RefCount
	{
	public:
		RefCountAllocator(T p, D d, const Allocator& a)
			: RefCount(), value(p), deleter(d), allocator(a)
		{}
		void destroy() override
		{
			deleter(value);
			value = nullptr;
		}
		void expire() override
		{
			auto a = allocator;
			this->~RefCountBlock();
			allocator.deallocate(this);
		}
	private:
		T value;
		D deleter;
		Allocator allocator;
	};

	template<class Y, class T>
	struct CompatibleTraits
	{
		static constexpr bool IS_T_ARRAY = TestIsArray<T>;
		static constexpr bool VALUE = (
			((not IS_T_ARRAY) and TestIsConvertible<Y*, T*>) or
			(IS_T_ARRAY and TestIsSame<TypeRemoveExtent<Y>, TypeRemoveExtent<T>>)
			);
	};
}

export namespace ne
{
	template<class T>
	class SharedPtr
	{
	public:
		using WeakType = WeakPtr<T>;
		using ElementType = TypeRemoveExtent<T>;

		SharedPtr() noexcept
			: ptr(nullptr), rc(nullptr)
		{}

		SharedPtr(decltype(nullptr)) noexcept
			: ptr(nullptr), rc(nullptr)
		{}

		template<class Y>
		explicit SharedPtr(Y* p) 
			requires(TestIsConvertible<Y*, ElementType*>)
		{
			alloc(p, DefaultDeleter<Y>{}, Allocator{});
		}

		template<class Y, class Deleter>
		SharedPtr(Y* p, Deleter&& deleter)
			requires(TestIsConvertible<Y*, ElementType*>)
		{
			alloc(p, Forward<Deleter>(deleter), Allocator{});
		}

		template<class Deleter>
		SharedPtr(decltype(nullptr), Deleter&& deleter)
		{
			alloc(nullptr, Forward<Deleter>(deleter), Allocator{});
		}

		template<class Y, class Deleter>
		SharedPtr(Y* p, Deleter&& deleter, const Allocator& allocator)
			requires(TestIsConvertible<Y*, ElementType*>)
		{
			alloc(p, Forward<Deleter>(deleter), allocator);
		}

		template<class Deleter>
		SharedPtr(decltype(nullptr), Deleter&& deleter, const Allocator& allocator)
		{
			alloc(nullptr, Forward<Deleter>(deleter), allocator);
		}

		SharedPtr(const SharedPtr& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			if (rc!=nullptr) rc->incRef();
		}

		template<class Y>
		SharedPtr(const SharedPtr<Y>& other) noexcept
			requires TestIsConvertible<Y*, ElementType*>
			: ptr(other.ptr), rc(other.rc)
		{
			if (rc != nullptr) rc->incRef();
		}

		template<class Y>
		SharedPtr(const SharedPtr<Y>& r, ElementType* ptr)
			: ptr(ptr), rc(r.rc)
		{
			if (rc != nullptr) rc->incRef();
		}

		SharedPtr(SharedPtr&& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			other.ptr = nullptr;
			other.rc = nullptr;
		}

		template<class Y>
		SharedPtr(SharedPtr<Y>&& other) noexcept
			requires TestIsConvertible<Y* , ElementType*>
			: ptr(other.ptr), rc(other.rc)
		{
			other.ptr = nullptr;
			other.rc = nullptr;
		}

		template<class Y>
		SharedPtr(SharedPtr<Y>&& other, ElementType* ptr) noexcept
			requires TestIsConvertible<Y*, ElementType*>
			: ptr(ptr), rc(other.rc)
		{
			other.ptr = nullptr;
			other.rc = nullptr;
		}

		// TODO: WEAK_PTR? UNIQUE_PTR?
		template<class U, class UD>
		SharedPtr(UniquePtr<U, UD>&& unique_ptr)
			requires TestIsConvertible<U*, ElementType*>
			: SharedPtr(unique_ptr.release(), Move(unique_ptr.getDeleter()))
		{}

		~SharedPtr()
		{
			if (rc) { rc->decRef(); }
			rc = nullptr;
			ptr = nullptr;
		}

		// operator=
		SharedPtr& operator=(const SharedPtr& other) noexcept
		{
			if (GetAddress(other) != this)
			{
				SharedPtr(other).swap(*this);
			}
			return *this;
		}

		template<class Y>
		SharedPtr& operator=(const SharedPtr<Y>& other) noexcept
			requires TestIsConvertible<Y*, ElementType*>
		{
			if (rc!=other.rc)
			{
				SharedPtr(other).swap(*this);
			}
			return *this;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept
		{
			if (GetAddress(other) != this)
			{
				SharedPtr(Move(other)).swap(*this);
			}
			return *this;
		}

		template<class Y>
		SharedPtr& operator=(SharedPtr<Y>&& other) noexcept
			requires TestIsConvertible<Y*, ElementType*>
		{
			if (GetAddress(other) != this)
			{
				SharedPtr(Move(other)).swap(*this);
			}
			return *this;
		}

		template<class U, class UD>
		SharedPtr operator=(UniquePtr<U, UD>&& unique_ptr)
			requires TestIsConvertible<U*, ElementType*>
		{
			SharedPtr(Move(unique_ptr)).swap(*this);
			return *this;
		}

		void swap(SharedPtr& other) noexcept
		{
			utils::Swap(ptr, other.ptr);
			utils::Swap(rc, other.rc);
		}

	private:
		template<class U, class D>
		void alloc(U p, D d, Allocator a)
		{
			auto prc = a.allocate<RefCountAllocator<U, D>>();
			ExceptionGuard gurad([=]() { a.deallocate(ptr); });
			Construct(prc, p, Move(d), Move(a));
			rc = prc;
			ptr = p;
			gurad.complete();
			// TODO: ENABLE_SHARED_FROM_THIS?
		}

		ElementType* ptr;
		RefCount* rc;
	};
}
