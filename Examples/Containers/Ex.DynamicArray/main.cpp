#include<iostream>
#include <vector>
import ntl.containers.dynamic_array;
using namespace std;
using namespace ne;

struct Node
{
	int v;
	bool moved;
	Node(int v): v(v), moved(false) {}
	Node(const Node& n) : v(n.v), moved(n.moved) { cout << "\tCopy " << v << endl; }
	Node(Node&& n) noexcept : v(n.v), moved(n.moved) { n.moved = true; n.v = -1; cout << "\tMove " << v << endl; }
	~Node() { if (moved) cout << "\tDestruct moved node" << endl; else cout << "\tDestruct " << v << endl; }

	friend ostream& operator<<(ostream& os, const Node& n)
	{
		os << n.v;
		return os;
	}

};

template<class T>
void print(const T& t)
{
	cout << "[";
	for (int i=0;i<t.size(); i++)
	{
		cout << t[i];
		if (i != t.size() - 1)
		{
			cout << ", ";
		}
	}
	cout << "]" << endl;
}

int main()
{
	{
		cout << "New arr1: [1, 2, 3, 4, 5]" << endl;
		DynamicArray<Node> arr{ 1, 2, 3, 4, 5 };
		cout << "Prepend " << 0 << endl;
		arr.prepend(0);
		print(arr);
		cout << "Append" << 1 << endl;
		arr.append(1);
		print(arr);
		cout << "Remove the first node" << endl;
		arr.remove(arr.begin());
		print(arr);
	}
	{
		cout << "New arr2: [1, 2, 3, 4, 5]" << endl;
		DynamicArray<Node> arr{ 1, 2, 3, 4, 5 };
		cout << "Append" << 1 << endl;
		arr.append(1);
		print(arr);
		cout << "Prepend " << 0 << endl;
		arr.prepend(0);
		print(arr);
		cout << "Remove the first node" << endl;
		arr.remove(arr.begin());
		print(arr);
		cout << "Clear arr" << endl;
		arr.clear();
		print(arr);
	}
	{
		cout << "New arr1: [1, 2, 3, 4, 5]" << endl;
		DynamicArray<Node> arr{ 1, 2, 3, 4, 5 };
		cout << "Insert 0 at pos 0" << endl;
		arr.insert(arr.begin(), 0);
		print(arr);
		cout << "Insert 6 at end" << endl;
		arr.insert(arr.end(), 6);
		print(arr);
		cout << "Remove [0, 3)" << endl;
		arr.remove(arr.begin(), arr.begin()+3);
		print(arr);
		cout << " Insert 10 at pos 1" << endl;
		arr.insert(arr.begin() + 1, 10);
		print(arr);
	}
	{
		cout << "New arr1: [1, 2, 3, 4, 5]" << endl;
		DynamicArray<Node> arr{ 1, 2, 3, 4, 5 };
		cout << "Insert 0 at pos 0" << endl;
		arr.insert(0, 0);
		print(arr);
		cout << "Insert 6 at end" << endl;
		arr.insert(arr.size(), 6);
		print(arr);
		cout << "Remove [0, 3)" << endl;
		arr.remove(arr.begin(), arr.begin() + 3);
		print(arr);
		cout << " Insert 10 at pos 1" << endl;
		arr.insert(1, 10);
		print(arr);

	}

	{
		cout << "New arr1: [1, 2, 3, 4, 5]" << endl;
		DynamicArray<Node> arr{ 1, 2, 3, 4, 5 };
		cout << " Insert 10 at pos 1" << endl;
		arr.insert(1, 10);
		print(arr);
		cout << "Squeeze arr" << endl;
		arr.squeeze();
		print(arr);
		cout << "Insert 100 at end" << endl;
		arr.insert(arr.size(), 100);
		print(arr);
	}
	return 0;
}