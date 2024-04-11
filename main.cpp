#include "AVL.h"
#include <iostream>
#include <vector>

int main() {

	avltree<int> t;

	t.insert(1);
	t.insert(3);
	t.insert(2);
	
	auto r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

	t.erase(1);

	r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

}