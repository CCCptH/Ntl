export module ntl.utils.exception_guard;
import ntl.concepts.callable;
import ntl.utils.forward;
export namespace ne {
	template<ConceptInvocable Rollback>
	class ExceptionGuard
	{
	private:
		Rollback r;
		bool c;
	public:
		ExceptionGuard(Rollback&& rollback) noexcept
			: r(Forward<Rollback>(rollback))
			, c(false)
		{}
		~ExceptionGuard() noexcept {
			if (!c)
				r();
		}
		void complete() noexcept { c = true; }
	};
}