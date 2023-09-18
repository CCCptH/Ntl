#include<iostream>
import ntl.containers.dynamic_array;
using namespace std;
using namespace ne;


template<class T>
void print(const T& t)
{
	for (auto& x : t)
	{
		cout << x << " ";
	}
	cout << endl;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	DynamicArray<int> arr{ 1, 2, 3, 4, 5 };
	arr.prepend(0);
	arr.append(1);
	print(arr);
	int* ptr = 0;
	ptr = 5 + ptr;
	arr.remove(arr.begin());
	print(arr);
	return 0;
}