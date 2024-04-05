#include "AVL.h"
#include <iostream>

int main() {
	avltree<int> t;

	t.insert(5);
	t.insert(3);
	t.insert(8);
	t.insert(5);
	t.insert(4);
	t.insert(1);

	for (auto it = t.begin(); it != t.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << "\n\n";

	auto r = t.get_root();

	t.print(r);


}