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
	/*
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

	t1 = high_resolution_clock::now();

	ms.erase(ms.get_root());

	t2 = high_resolution_clock::now();
	time_span1 = duration_cast<nanoseconds>(t2 - t1);
	std::cout << "Erased root in " << time_span1.count() << "ms\n";*/

	std::cout << "Just in order to show that it's balancing, let's insert elements 1-31 in order and print\n";
	avltree<int> t;
	for (int i = 1; i < 7; i++) {
		t.insert(7);
		info(t);
	}

	t.insert(7);
	info(t);

}
