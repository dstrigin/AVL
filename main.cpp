#include "AVL.h"
#include <iostream>
#include <vector>
#include <string>

template <typename T>
void info(const avltree<T>& t) {

	auto r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

}

template<typename T> using Myal = std::allocator<T>;
template<typename T> using Mypred = std::less<T>;
template<typename T> using ContainerTemplate = avltree<T, Mypred<T>, Myal<T>>;

int main() {

	avltree<int> t{ 1, 3, 5 };
	info(t);

	t.insert(5);
	info(t);

	t.insert(5);
	info(t);

	t.insert(6);
	info(t);

}