#include "AVL.h"
#include <iostream>
#include <vector>

int main() {

	avltree<int> t;

	std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7 };

	t.insert(v.begin(), v.end());

	for (auto it = t.begin(); it != t.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << "\n\n";

	auto r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

	t.erase(1);

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

	t.erase(5);

	r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

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