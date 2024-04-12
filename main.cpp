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

	/*t.insert(1);
	info(t);
	t.insert(4);
	info(t);
	t.insert(3);
	info(t);
	t.insert(5);
	info(t);
	t.insert(8);
	info(t);

	t.erase(1);
	info(t);

	t.erase(3);
	info(t);*/

}