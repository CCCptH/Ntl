export module ntl.functional.ref_wrapper;
export import ntl.functional.invoke;

export namespace ne
{
	template<class T>
	constexpr RefWrapper<T> Ref(T& object) noexcept
	{
		return RefWrapper<T>(object);
	}

	template<class T>
	constexpr RefWrapper<T> Ref(RefWrapper<T> object) noexcept
	{
		return RefWrapper<T>(object);
	}

	template<class T>
	constexpr void Ref(const T&&) = delete;

	template<class T>
	constexpr RefWrapper<const T> CRef(const T& object) noexcept
	{
		return RefWrapper<const T>(object);
	}

	template<class T>
	constexpr RefWrapper<const T> CRef(RefWrapper<T> object) noexcept
	{
		return RefWrapper<const T>(object);
	}

	template<class T>
	constexpr void CRef(const T&&) = delete;
}
