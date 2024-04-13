#include "AVL.h"
#include <iostream>
#include <vector>

template <typename T>
void info(const avltree<T>& t) {

	auto r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

}

int main() {

	avltree<int> t;
	
	for (int i = 1; i < 16; i++) {
		t.insert(i);
	}

	info(t);

	t.erase(8);
	info(t);

	t.erase(6);
	info(t);

	t.erase(4);
	info(t);

}