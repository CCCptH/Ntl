#include <iostream>
import ntl.functional;

using namespace std;
using namespace ne;

struct Functor
{
	Functor() {  };
	Functor(const Functor& x) { cout << "Copy Functor" << endl; }
	Functor(Functor&& f) noexcept {  cout << "Move Functor" << endl; }
	~Functor() noexcept { cout << "Destruct Functor" << endl; }
	int operator()(int z) const { cout << "Functor call: " << z << endl; return z; }
};

int func(int x)
{
	cout << "Function call: " << x << endl;
	return x;
}

int main()
{
	Functor functor;
	Function<int(int)> f1(functor);
	Function<int(int)> f2(Functor{});
	Function<int(int)> f3 = func;
	Function<int(int)> f4 = [](int x) {cout << "Lambda call: " << x << endl; return x; };
	f1(1);
	f2(2);
	f3(3);
	f4(4);
	f1.swap(f3);
	cout << "Invoke..." << endl;
	auto a = Invoke(functor, 1); cout << "Result is " << a << endl;
	auto b = Invoke(f1, 2); cout << "Result is " << b << endl;
	auto c = Invoke(f3, 3); cout << "Result is " << c << endl;
	auto d = Invoke(func, 4); cout << "Result is " << d << endl;
	auto e = Invoke([](int x)->int {cout << "Lamda call2: " << x << endl; return x; }, 5); cout << "Result is " << e << endl;
	return 0;
}