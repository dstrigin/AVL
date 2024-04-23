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

template<typename T> using ContainerTemplate = avltree<T>;
//template<typename T> using ContainerTemplate = std::multiset<T>;

int main() {

	using namespace std;
	using namespace std::chrono;

	// генератор случайных чисел
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
	std::cout << "Created 10.000 elements container in " << time_span1.count() << "ms\n";

	// copy
	t1 = high_resolution_clock::now();

	ContainerTemplate<int> ms1(ms);
	
	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Copied 10.000 elements container in " << time_span1.count() << "ms\n";

	// creation with range
	t1 = high_resolution_clock::now();

	ContainerTemplate<int> ms2(ms.begin(), ms.end());

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Created 10.000 elements container by range in " << time_span1.count() << "ms\n";

	// find
	t1 = high_resolution_clock::now();

	int sum = 0;
	for (int i = 0; i < 10000; i++) {
		auto it = ms.find(11);
		if (it != ms.end()) {
			sum++;
		}
	}
	std::cout << "Just to test: " << sum << "\n";

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found element in " << time_span1 << " ms\n";

	// find non-existing
	t1 = high_resolution_clock::now();
	
	sum = 0;
	for (int i = 0; i < 10000; i++) {
		auto it = ms.find(103211);
		if (it == ms.end()) {
			sum++;
		}
	}
	std::cout << "Just to test: " << sum << "\n";

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found that element does not exist in " << time_span1.count() << "ms\n";

	// lower_bound
	t1 = high_resolution_clock::now();

	auto lb = ms.lower_bound(327);

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found lower_bound of 327 in " << time_span1.count() << "ms\n";
	std::cout << "It was " << *lb << "\n";

	// upper_bound
	t1 = high_resolution_clock::now();

	auto ub = ms.upper_bound(643);

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Found upper_bound of 643 in " << time_span1.count() << "ms\n";
	std::cout << "It was " << *ub << "\n";

	// erase
	t1 = high_resolution_clock::now();

	ms.erase(*lb);

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Erased lower_bound of 327 in " << time_span1.count() << "ms\n";

	t1 = high_resolution_clock::now();

	ms.erase(*ub);

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Erased upper_bound of 643 in " << time_span1.count() << "ms\n";

	/*t1 = high_resolution_clock::now();

	ms.erase(ms.get_root());

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Erased root in " << time_span1.count() << "ms\n";*/

	std::cout << "Just in order to show that it's balancing, let's insert elements 1-31 in order and print\n";
	avltree<int> t;
	for (int i = 1; i < 32; i++) {
		t.insert(7);
	}
	info(t);

}

/*

	log for own class (avltree<T>):
Created 10.000 elements container in 0.0082682ms
Copied 10.000 elements container in 0.0033563ms
Created 10.000 elements container by range in 0.004912ms
Found existing element in 0.0003607ms
Found that element does not exist in 1e-07ms
Found lower_bound of 327 in 0.0001499ms
It was 327
Found upper_bound of 643 in 0.0001399ms
It was 644
Erased lower_bound of 327 in 0.0001591ms
Erased upper_bound of 643 in 0.0001337ms
Erased root in 3.6e-06ms

	log for std::multiset<T>:
Created 10.000 elements container in 0.0066596ms
Copied 10.000 elements container in 0.0026829ms
Created 10.000 elements container by range in 0.0039554ms
Found existing element in 3.6e-06ms
Found that element does not exist in 0ms
Found lower_bound of 327 in 1.9e-06ms
It was 327
Found upper_bound of 643 in 1.5e-06ms
It was 644
Erased lower_bound of 327 in 1.07e-05ms
Erased upper_bound of 643 in 1.05e-05ms

*/