#include<iostream>
#include<unordered_map>

import ntl.utils;
import ntl.containers.hash_dict;

using namespace std;
using namespace ne;

template<class T>
void print(const T& m) {
	for (auto& [k, v] : m) {
		cout << "[" << k << ", " << v << "] ";
	}
	cout << endl;
}

int main()
{
	HashDict<int, int> dict{
		KeyValue<int, int>(1, 2),
		KeyValue<int, int>(2, 3)
	};
	print(dict);

	HashDict<int, int> dict2(dict.begin(), dict.end(), 5);
	print(dict2);

	dict2.remove(2);
	print(dict2);

	dict.emplace(5, 6);
	print(dict);

	cout << dict.tryEmplace(5, 0) << endl;
	dict2.insertOrAssign(6, 7);
	dict2.insertOrAssign(6, 6);
	print(dict2);

	dict.swap(dict2);
	print(dict);
	print(dict2);
	return 0;
}