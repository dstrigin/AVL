#include "AVL.h"
#include <iostream>

int main() {
	avltree<int> t;

	t.insert(5);
	t.insert(3);
	t.insert(4);
	t.insert(8);
	t.insert(6);

	for (auto it = t.begin(); it != t.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << "\n\n";

	auto r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	/*std::cout << "\n\n";

	t.insert(8);
	t.insert(10);

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

	t.insert(0);
	t.insert(-1);

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";*/

}