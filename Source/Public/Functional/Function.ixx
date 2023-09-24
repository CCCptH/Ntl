export module ntl.functional.function;
export import ntl.functional.bad_function_call;
import ntl.functional.invoke;
import ntl.utils;
import ntl.memory.allocator;
import ntl.type_traits;

/**
 * @brief Small Object optimization
 */
export namespace ne
{
	template<class R, class... Args>
	class Function
	{
		static_assert(AlWAYS_FALSE<R>, "[ntl.functional.function] Invalid template params.");
	};

	template<class R, class ...Args>
	class Function<R(Args...)>
	{
	private:
		template<class F>
		class InternalFunc
		{
		private:
			F f;
		public:
			InternalFunc(const F& func): f(func) {}
			InternalFunc(F&& func) :f(Move(func)) {}
			InternalFunc(InternalFunc&& f) = default;
			InternalFunc(const InternalFunc&) = default;
			~InternalFunc() = default;
			R operator()(Args...args)
			{
				return Invoke(f, Forward<Args>(args)...);
			}
		};

		template<class T>
		using TypeFuncForward = TypeConditional<TestIsScalar<T>, T, T&&>;
		;

		constexpr static uint32 BUF_SIZE = 40;
		constexpr static uint32 PTR_SIZE = sizeof(void*);
		static_assert(PTR_SIZE == 8, "[ntl.functional.function] Only support x64 system");

		struct Storage
		{
			union
			{
				mutable uint8 buf[BUF_SIZE] = {};
				void* ptr;
			};
		};

		template<class Func>
		constexpr static bool TestUsingSmall = sizeof(Func) <= BUF_SIZE and alignof(Func) <= alignof(Storage) and TestIsTriviallyCopyConstructible<Func> and TestIsTriviallyDestructible<Func>;
		using Call = R(*)(const Storage*, TypeFuncForward<Args>...);
		using Copy = void* (*)(const void*);
		using Destroy = void(*)(void*);

		Storage storage;
		Call call;
		Copy copy;
		Destroy destroy;

		template <class F>
		static bool IsNotNull(F const&) { return true; }

		template <class F>
		static bool IsNotNull(F* ptr) { return ptr; }

		template <class Ret, class Class>
		static bool IsNotNull(Ret Class::* ptr) { return ptr; }

		template <class F>
		static bool IsNotNull(Function<F> const& f) { return !!f; }

		template<class Func>
		static R CallInternal(const Storage* storage, TypeFuncForward<Args>...args)
		{
			Func* func = reinterpret_cast<Func*>(
				TestUsingSmall<Func> ? &storage->buf[0] : storage->ptr
				);
			return (*func)(Forward<Args>(args)...);
		}

		template<class Func>
		static void* CopyInternal(const void* s)
		{
			const Func* func = reinterpret_cast<const Func*>(s);
			return AllocateNew<Func>(*func);
		}
		template<class Func>
		static void DestroyInternal(void* s)
		{
			Func* func = reinterpret_cast<Func*>(s);
			//Destruct(func);
			DeallocateDelete(func);
		}

		void init()
		{
			call = nullptr;
			copy = nullptr;
			destroy = nullptr;
		}
		template<class Func>
		void set(Func&& func)
		{
			using InterFn = InternalFunc<TypeDecay<Func>> ;
			//if (IsNotNull(Forward<Func>(func)))
			if (IsNotNull(func))
			{
				call = &CallInternal<InterFn>;
				if constexpr (TestUsingSmall<InterFn>)
				{
					Construct(reinterpret_cast<InterFn*>(&storage.buf[0]), Forward<Func>(func));
					copy = nullptr;
					destroy = nullptr;
				}
				else
				{
					// Todo: allocation
					auto ptr = new InterFn(Forward<Func>(func));
					storage.ptr = ptr;
					copy = &CopyInternal<InterFn>;
					destroy = &DestroyInternal<InterFn>;
				}
			}
			else
			{
				init();
			}
		}
	public:
		Function() noexcept
		{
			init();
		}
		Function(decltype(nullptr)) noexcept
		{
			init();
		}
		Function(const Function& function)
			: storage(function.storage)
			, call(function.call)
			, copy(function.copy)
			, destroy(function.destroy)
		{
			if (function.copy != nullptr)
			{
				storage.ptr = function.copy(function.storage.ptr);
			}
		}
		Function(Function&& function) noexcept
			: storage(function.storage)
			, call(function.call)
			, copy(function.copy)
			, destroy(function.destroy)
		{
			function.init();
		}
		template<class Func>
			requires (not TestIsSame<TypeUnCVRef<Func>, Function>)
		Function(Func&& func)
		{
			set<Func>(Forward<Func>(func));
		}
		~Function()
		{
			if (destroy != nullptr)
			{
				destroy(storage.ptr);
			}
		}

		Function& operator=(const Function& other)
		{
			Function(other).swap(*this);
			return *this;
		}
		Function& operator=(Function&& other) noexcept
		{
			Function(Move(other)).swap(*this);
			return *this;
		}
		template<class Fn>
			requires TestIsInvocable<Fn, Args...>
		Function& operator=(Fn&& f)
		{
			Function(Forward<Fn>(f)).swap(*this);
			return *this;
		}
		template<class F>
		Function& operator=(RefWrapper<F> f)
		{
			Function(f).swap(*this);
			return *this;
		}

		R operator()(Args...args) const
		{
			if (call == nullptr) [[unlikely]]
			{
				throw BadFunctionCall();
			}
			return call(&storage, Forward<Args>(args)...);
		}

		operator bool() const noexcept
		{
			return call != nullptr;
		}

		void swap(Function& f) noexcept
		{
			utils::Swap(storage, f.storage);
			utils::Swap(call, f.call);
			utils::Swap(copy, f.copy);
			utils::Swap(destroy, f.destroy);
		}

		friend void Swap(Function& x, Function& y) noexcept
		{
			x.swap(y);
		}
	};
}
