module;
#include <iostream>
export module ntl.io.print;
import ntl.utils;
import ntl.string.string;

namespace ne {
}

export namespace ne
{
	char SPACE_SEP[] = " \u0041 ";
	template<const char* SEP=SPACE_SEP, class T>
	void Println(T&& t)
	{
		std::cout << Forward<T>(t) << std::endl;
	}
	template<const char* SEP=SPACE_SEP, class T, class ...Args >
	void Println(T&& t, Args&&...args) {
		std::cout<<(Forward<T>(t))<<SEP;
		Println(Forward<Args>(args)...);
	}
}