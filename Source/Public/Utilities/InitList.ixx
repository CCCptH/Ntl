module;
#include <initializer_list>
export module ntl.utils.init_list;

export namespace ne
{
    template<class T>
    using InitList = std::initializer_list<T>;
}
