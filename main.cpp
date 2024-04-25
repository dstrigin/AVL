#include "AVL.h"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <set>

/* prints contents and balance for each node of avl tree */
template <typename T>
void info(const avltree<T>& t) {

	auto r = t.get_root();

	t.print(r);

	std::cout << "\n\n";

	t.print_balance(r);

	std::cout << "\n\n";

}

// uncomment line in order to test

//template<typename T> using ContainerTemplate = avltree<T>;
template<typename T> using ContainerTemplate = std::multiset<T>;

int main() {
	
	using namespace std;
	using namespace std::chrono;

	// random generator
	random_device rd;
	mt19937 engine(rd());
	uniform_int_distribution<int> dist(1, 1000);

	// insertion 
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	ContainerTemplate<int> ms;
	for (int i = 0; i < 10000; i++) {
		ms.insert(dist(engine));
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Created 10.000 elements container in " << time_span1.count() << " s\n\n";

	// copy
	t1 = high_resolution_clock::now();

	int sum{};
	for (int i = 0; i < 10000; i++) {
		ContainerTemplate<int> ms1(ms);
		sum += ms1.size();
	}
	
	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Copied container in " << time_span1.count() << " s\n\n";

	// creation with range
	t1 = high_resolution_clock::now();

	sum = 0;
	for (int i = 0; i < 10000; i++) {
		ContainerTemplate<int> ms2(ms.begin(), ms.end());
		sum += ms2.size();
	}

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Created by range in " << time_span1.count() << " s\n\n";

	// find
	t1 = high_resolution_clock::now();

	sum = 0;
	for (int i = 0; i < 10000; i++) {
		auto it = ms.find(11);
		if (it != ms.end()) {
			sum++;
		}
	}

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found element in " << time_span1 << " s\n\n";

	// find non-existing
	t1 = high_resolution_clock::now();
	
	sum = 0;
	for (int i = 0; i < 10000; i++) {
		auto it = ms.find(103211);
		if (it == ms.end()) {
			sum++;
		}
	}

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found that element does not exist in " << time_span1.count() << " s\n\n";

	// lower_bound
	t1 = high_resolution_clock::now();

	sum = 0;
	for (int i = 0; i < 10000; i++) {
		auto lb = ms.lower_bound(327);
		sum += *lb;
	}

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found lower_bound in " << time_span1.count() << " s\n\n";

	// upper_bound
	t1 = high_resolution_clock::now();

	sum = 0;
	for (int i = 0; i < 10000; i++) {
		auto ub = ms.upper_bound(643);
		sum += *ub;
	};

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found upper_bound in " << time_span1.count() << " s\n\n";

	// erase
	t1 = high_resolution_clock::now();

	ms.erase(*ms.lower_bound(327));

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Erased lower_bound of 327 in " << time_span1.count() << " s\n\n";

	t1 = high_resolution_clock::now();

	ms.erase(*ms.upper_bound(643));

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Erased upper_bound of 643 in " << time_span1.count() << " s\n\n";

	t1 = high_resolution_clock::now();

	std::cout << "Just in order to show that it's balancing, let's insert the same element 31 times and print the tree out\n";
	avltree<int> t;
	for (int i = 1; i < 32; i++) {
		t.insert(7);
	}

	info(t);
		 
}

/*

	avltree:

Created 10.000 elements container in 0.0011876 s

Copied container in 3.55314 s

Created by range in 0.0012173 s

Found element in 0 s

Found that element does not exist in 0 s

Found lower_bound in 1.48e-05 s

Found upper_bound in 1e-07 s

Erased lower_bound of 327 in 2e-07 s

Erased upper_bound of 643 in 1e-07 s

	multiset:

Created 10.000 elements container in 0.0017846 s

Copied container in 8.50776 s

Created by range in 10.47 s

Found element in 1e-07s s

Found that element does not exist in 0 s

Found lower_bound in 1e-07 s

Found upper_bound in 1e-07 s

Erased lower_bound of 327 in 6.4e-06 s

Erased upper_bound of 643 in 3.1e-06 s

*/
