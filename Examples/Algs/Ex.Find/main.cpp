#include <iostream>

import ntl.containers.dynamic_array;
import ntl.algorithms.find;
import ntl.ranges;
import ntl.functional;
import ntl.iterator;

using namespace std;
using namespace ne;

struct Pack
{
	int a;
	int b;
};

int main() {
	DynamicArray<Pack> arr = {
		{1, 2},
		{2, 3},
		{3, 4},
		{4, 5}
	};
	auto it = algs::Find(arr.begin(), arr.end(), 3, &Pack::a);
	return 0;
}