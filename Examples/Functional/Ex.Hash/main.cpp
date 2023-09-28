#include <iostream>
import ntl.functional.hash;
import ntl.utils;
using namespace ne;
using namespace std;



int main()
{
	auto strHash = Hash<const char*>{};
	auto stdHash = std::hash<const char*>{};
	cout << strHash("wocao") << endl;
	cout << stdHash("wocao") << endl;
	cout << strHash("fuckasdasdksajdklasjlkdjalkdjlkasdjlkasjlkd") << endl;
	auto i = 0x9ae16a3b2f90404ull;
	union PACK
	{
		char str[8];
		uint64 val;
	};
	PACK p;
	p.val = 0;
	cout << strHash(p.str) << endl;
	return 0;
}