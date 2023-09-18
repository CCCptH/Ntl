#include <iostream>
import ntl.functional;

using namespace std;
using namespace ne;

int main()
{
	int a = 5;
	auto b = Ref(a);
	RefWrapper c(a);

	cout << a << " " << b << " " << c << " " << endl;
	a = 10;
	cout << a << " " << b << " " << c << " " << endl;
	return 0;
}