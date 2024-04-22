#include "AVL.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>

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
template<typename T> using Mycont = avltree<T, Mypred<T>, Myal<T>>;

int main() {

	char carr[] = "abcadef";

	avltree<char> t(carr, carr + 7);

	info(t);

	std::pair<avltree<char>::const_iterator, avltree<char>::const_iterator> pcc = t.equal_range('a');

}