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

	ContainerTemplate<std::string> T1{ "abc", "cde", "123", "AAAAAAAA" };

	info(T1);

	std::vector<std::string> check1{ "123", "AAAAAAAA", "abc", "cde" };
	for (const auto& str : check1) {
		T1.erase(str);
		info(T1);
	}

}