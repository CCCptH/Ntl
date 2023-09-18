#include <iostream>
#include <optional>
import ntl.optional;
import ntl.type_traits;
import ntl.utils;

using namespace std;
using namespace ne;

void print(auto const& x)
{
	if (x)
	{
		cout << *x << endl;
	}
	else
	{
		cout << "Null optional" << endl;
	}
}

int main()
{
	Optional<int> a = NULLOPT;
	Optional<int> b(5);
	print(a);
	print(b);

	int x=5;
	int y=6;
	static_assert(TestIsMoveConstructible<int>, "fuck");
	static_assert(TestIsAssignable<int&, int>, "fuck");

	a.swap(b);
	print(a);
	print(b);

	b = Move(a);
	print(a);
	print(b);

	return 0;
}