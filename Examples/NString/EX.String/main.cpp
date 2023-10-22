#include <iostream>

import ntl.string.string;

using namespace std;
using namespace ne;

static int ALLOC_COUNT = 0;
static int RELEASE_COUNT = 0;

void* operator new(size_t sz) {
	++ALLOC_COUNT;
	return malloc(sz);
}

void operator delete(void* ptr) {
	++RELEASE_COUNT;
	free(ptr);
}

void Print(const String& str)
{
	cout << str.cstr() << endl;
}

void MemInfo()
{
	cout << "Allocated: " << ALLOC_COUNT << endl;
	cout << "Released: " << RELEASE_COUNT << endl;
}


int main()
{
	MemInfo();
	{
		String s1(100, 'a');
		String s2;
		String s3("asdff");
		String s4(s1);
		String s5(s1, 10);
	}
	MemInfo();
	{
		String s1(1, 'b');
		Print(s1);
		cout << s1.size() << endl;
		s1.prepend('a');
		cout << s1.size() << endl;
		Print(s1);
	}
	MemInfo();
}