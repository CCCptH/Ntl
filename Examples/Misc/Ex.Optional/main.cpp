#include <iostream>
#include <optional>
import ntl.optional;
import ntl.type_traits;
import ntl.utils;
import ntl.functional.ref_wrapper;

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

	print(a);
	print(b);

	a.then([](const Optional<int>& o)-> Optional<int>
		{
			cout <<"then: " << o.value() << endl;
			return NULLOPT;
		});

	Optional<int> c = NULLOPT;
	c.orElse([]()
	{
			return Optional<int>(2);
	}).then([](Optional<long> opt)->Optional<int>
	{
		cout << opt.value() << endl;
		return Optional<int>(10);
	}).transform([](int x)
	{
			cout << x << endl;
			return x;
	});

	return 0;
}