module;
#include <atomic>
export module ntl.memory.shared_ptr;
import ntl.utils;
import ntl.memory.unique_ptr;
import ntl.type_traits;
import ntl.utils.exception_guard;
import ntl.memory.allocator;
import ntl.functional.ref_wrapper;

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
			this->~RefCount();
			allocator.deallocate(this);
		}
	private:
		T value;
		D deleter;
		Allocator allocator;
	};

	template<class Y, class T>
	struct TestIsCompatibleHelper
	{
		static constexpr bool VALUE = TestIsConvertible<Y*, T*>;
	};
	template<class Y, class T, size_t N>
	struct TestIsCompatibleHelper<Y[N], T>
	{
		static constexpr bool VALUE = TestIsSame<TypeRemoveExtent<Y>, TypeRemoveExtent<TypeUnCV<T>>>;
	};

	template<class Y, class T>
	inline constexpr bool TestIsCompatible = TestIsCompatibleHelper<Y, T>::VALUE;
}

export namespace ne
{
	template<class T>
	class EnableSharedFromThis;
	template<class T>
	class WeakPtr;

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
			requires (TestIsConvertible<Y*, ElementType*>) or (TestIsBoundedArray<T> and TestIsConvertible<Y(*)[VVExtent<T>], T*>) or (TestIsUnboundedArray<T> and TestIsConvertible<Y(*)[], T*>)
		explicit SharedPtr(Y* p) 
		{
			if constexpr (!TestIsArray<T>) {
				alloc(p, DefaultDeleter<Y>{}, Allocator{});
			}
			else
			{
				alloc(p, DefaultDeleter<Y[]>{}, Allocator{});
			}
		}

		template<class Y, class Deleter>
			requires (TestIsConvertible<Y*, ElementType*>) or (TestIsBoundedArray<T> and TestIsConvertible<Y(*)[VVExtent<T>], T*>) or (TestIsUnboundedArray<T> and TestIsConvertible<Y(*)[], T*>)
		SharedPtr(Y* p, Deleter&& deleter)
		{
			alloc(p, Forward<Deleter>(deleter), Allocator{});
		}

		template<class Deleter>
		SharedPtr(decltype(nullptr), Deleter&& deleter)
		{
			alloc(nullptr, Forward<Deleter>(deleter), Allocator{});
		}

		template<class Y, class Deleter>
			requires (TestIsConvertible<Y*, ElementType*>) or (TestIsBoundedArray<T> and TestIsConvertible<Y(*)[VVExtent<T>], T*>) or (TestIsUnboundedArray<T> and TestIsConvertible<Y(*)[], T*>)
		SharedPtr(Y* p, Deleter&& deleter, const Allocator& allocator)
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
			requires TestIsCompatible<Y, T>
		SharedPtr(const SharedPtr<Y>& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			if (rc != nullptr) rc->incRef();
		}

		template<class Y>
			//requires TestIsCompatible<Y, T>
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
			requires TestIsCompatible<T, T>
		SharedPtr(SharedPtr<Y>&& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			other.ptr = nullptr;
			other.rc = nullptr;
		}

		template<class Y>
		SharedPtr(SharedPtr<Y>&& other, ElementType* ptr) noexcept
			//requires TestIsConvertible<Y*, ElementType*>
			: ptr(ptr), rc(other.rc)
		{
			other.ptr = nullptr;
			other.rc = nullptr;
		}

		// TODO: WEAK_PTR? 
		template<class U, class UD>
			requires (not TestIsRef<UD>)
		SharedPtr(UniquePtr<U, UD>&& unique_ptr)
			requires TestIsCompatible<U, T>
			: SharedPtr(unique_ptr.release(), Move(unique_ptr.getDeleter()))
		{}
		template<class U, class UD>
			requires (TestIsRef<UD>)
		SharedPtr(UniquePtr<U, UD>&& unique_ptr)
			requires TestIsCompatible<U, T>
		: SharedPtr(unique_ptr.release(), Ref(unique_ptr.getDeleter()))
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
			requires TestIsCompatible<Y, T>
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
			requires TestIsCompatible<Y, T>
		{
			if (GetAddress(other) != this)
			{
				SharedPtr(Move(other)).swap(*this);
			}
			return *this;
		}

		template<class U, class UD>
		SharedPtr operator=(UniquePtr<U, UD>&& unique_ptr)
			requires TestIsCompatible<U, T>
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
		template<class A, class B>
		void enableSharedFromThis(RefCount* ref_count, A* maybe_enable_shared, B* value);

		template<class U, class D>
		void alloc(U p, D d, Allocator a)
		{
			ExceptionGuard gurad([=]() { d(p); });
			auto prc = a.allocate<RefCountAllocator<U, D>>();
			Construct(prc, p, Move(d), Move(a));
			rc = prc;
			ptr = p;
			gurad.complete();


		}

		ElementType* ptr;
		RefCount* rc;

		template<class Z>
		friend class WeakPtr;
	};

	template<class T>
	class WeakPtr
	{
	public:
		using ElementType = T;

		constexpr WeakPtr() noexcept
			: ptr(nullptr), rc(nullptr)
		{}

		WeakPtr(const WeakPtr& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			if (rc)
			{
				rc->incWRef();
			}
		}

		template<class Y>
			requires TestIsCompatible<Y, T>
		WeakPtr(const WeakPtr<Y>& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			if (rc)
			{
				rc->incWRef();
			}
		}

		template <class Y>
			requires TestIsCompatible<Y, T>
		WeakPtr(const SharedPtr<Y>& sp) noexcept
			: ptr(sp.ptr), rc(sp.rc)
		{
			if (rc)
			{
				rc->incWRef();
			}
		}

		WeakPtr(WeakPtr&& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			other.ptr = nullptr;
			other.rc = nullptr;
		}

		template<class Y>
			requires TestIsCompatible<Y, T>
		WeakPtr(WeakPtr<Y>&& other) noexcept
			: ptr(other.ptr), rc(other.rc)
		{
			other.ptr = nullptr;
			other.rc = nullptr;
		}

		~WeakPtr()
		{
			if (rc)
			{
				rc->decWRef();
			}
		}

		WeakPtr& operator=(const WeakPtr& other) noexcept
		{
			WeakPtr(other).swap(*this);
			return *this;
		}
		template<class Y>
			requires TestIsCompatible<Y, T>
		WeakPtr& operator=(const WeakPtr<Y>& other) noexcept
		{
			WeakPtr(other).swap(*this);
			return *this;
		}

		template<class Y>
			requires TestIsCompatible<Y, T>
		WeakPtr& operator=(const SharedPtr<Y>& other) noexcept {
			WeakPtr(other).swap(*this);
			return *this;
		}

		WeakPtr& operator=(WeakPtr&& other) noexcept
		{
			WeakPtr(Move(other)).swap(*this);
			return *this;
		}

		template<class Y>
			requires TestIsCompatible<Y,T>
		WeakPtr& operator=(WeakPtr<Y>&& other) noexcept
		{
			WeakPtr(Move(other)).swap(*this);
			return *this;
		}

		void swap(WeakPtr& other) noexcept
		{
			utils::Swap(ptr, other.ptr);
			utils::Swap(rc, other.rc);
		}

		friend void Swap(WeakPtr& a, WeakPtr& b)
		{
			a.swap(b);
		}

		void reset()
		{
			if (rc) { rc->decWRef(); }
			ptr = nullptr;
			rc = nullptr;
		}

		int64 useCount() const
		{
			return rc ? rc->useCount() : 0;
		}

		bool expired() const
		{
			return (!rc || rc->useCount() == 0);
		}

		SharedPtr<T> lock() const noexcept
		{
			if (rc)
			{
				SharedPtr<T> temp;
				temp.rc = rc->lock();
				temp.ptr = ptr;
				return temp;
			}
			return nullptr;
		}

	private:
		ElementType* ptr;
		RefCount* rc;
	};

	template<class T>
	class EnableSharedFromThis
	{
	public:
		SharedPtr<T> sharedFromThis()
		{
			return SharedPtr<T>(weak);
		}
		SharedPtr<const T> sharedFromThis() const
		{
			return SharedPtr<const T>(weak);
		}
		WeakPtr<T> weakFromThis()
		{
			return weak;
		}
		WeakPtr<const T> weakFromThis() const
		{
			return weak;
		}
	protected:
		template<class T> friend class SharedPtr;
		constexpr EnableSharedFromThis()noexcept = default;
		EnableSharedFromThis(const EnableSharedFromThis&) noexcept = default;
		EnableSharedFromThis& operator=(const EnableSharedFromThis&) noexcept
		{
			return *this;
		}
		~EnableSharedFromThis() = default;
	private:
		mutable WeakPtr<T> weak;
	};

	template <class T>
	template <class A, class B>
	void SharedPtr<T>::enableSharedFromThis(RefCount* ref_count, A* maybe_enable_shared, B* value)
	{
		if constexpr (TestIsBaseOf<EnableSharedFromThis<T>, A>)
		{
			EnableSharedFromThis<T>* es = maybe_enable_shared;
			es->weak.ptr = value;
			if (es->weak.rc) es->weak.rc->decWRef();
			es->weak.rc = ref_count;
			if (es->weak.rc) es->weak.rc->incWRef();
		}
	}

}
