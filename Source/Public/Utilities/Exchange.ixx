export module ntl.utils.exchange;

import ntl.utils.move;
import ntl.utils.forward;
import ntl.type_traits.support_operations;
export namespace ne {
    /**
     * @brief Replaces the value of obj with new_value and returns the old value of obj
     * 
     * @tparam A 
     * @tparam B 
     * @param obj object whose value to replace
     * @param new_value the value to assign to obj
     * @return A 
     */
    template<class A, class B=A>
        requires TestIsMoveConstructible<A> and TestIsAssignable<A&,B>
    constexpr
    A Exchange(A& obj, B&& new_value)
    noexcept(TestIsNothrowMoveConstructible<A> && TestIsNothrowAssignable<A&, B>)
    {
        A old = Move(obj);
        obj = Forward<B>(new_value);
        return old;
    }

}