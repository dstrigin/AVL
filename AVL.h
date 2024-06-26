﻿#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <initializer_list>
#include <exception>
#include <iomanip>

template<typename T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
class avltree {

	struct node {
		T data;					// значение
		unsigned char balance;	// разность количества дочерних узлов в правом и левом поддеревьях
		node* parent;			// родительский узел
		node* left;				// левый дочерний узел
		node* right;			// правый дочерний узел
		bool is_nil;			// является ли фиктивным узлом
	};

	Compare cmp = Compare();

	using node_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

	node_allocator_type Alc;

public:

	using key_type					= T;
	using key_compare				= Compare;
	using value_type				= T;
	using value_compare				= Compare;
	using allocator_type			= typename node_allocator_type;
	using size_type					= size_t;
	using difference_type			= ptrdiff_t;
	using pointer					= value_type*;
	using const_pointer				= const value_type*;
	using reference					= value_type&;
	using const_reference			= const value_type&;
	class iterator;
	using const_iterator			= iterator;
	using reverse_iterator			= std::reverse_iterator<iterator>;
	using const_reverse_iterator	= std::reverse_iterator<const_iterator>;

private:

	// фиктивный узел
	node* dummy;

	size_t sz = 0;

	// Создание фиктивной вершины
	inline node* make_dummy() {

		dummy = Alc.allocate(1);

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->parent));
		dummy->parent = dummy;

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->left));
		dummy->left = dummy;

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->right));
		dummy->right = dummy;

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->balance), 0);

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->is_nil), true);

		return dummy;
	}

	// Создание узла
	template <typename Q>
	inline node* make_node(Q&& val, node* p = nullptr, node* l = nullptr, node* r = nullptr) {

		node* new_node = Alc.allocate(1);

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->parent));
		new_node->parent = p;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->left));
		new_node->left = l;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->right));
		new_node->right = r;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->data), std::forward<Q>(val));

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->balance), 0);

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->is_nil), false);

		return new_node;
	}

	// Удаление фиктивной вершины
	inline void delete_dummy(node* node) {
		std::allocator_traits<allocator_type>::destroy(Alc, &(node->parent));
		std::allocator_traits<allocator_type>::destroy(Alc, &(node->left));
		std::allocator_traits<allocator_type>::destroy(Alc, &(node->right));
		std::allocator_traits<allocator_type>::deallocate(Alc, node, 1);
	}

	// Удаление узла
	inline void delete_node(node* node) {
		//  Тут удаляем поле данных (вызывается деструктор), а остальное удаляем так же, как и фиктивную
		std::allocator_traits<allocator_type>::destroy(Alc, &(node->data));
		delete_dummy(node);
	}

public:

	/* Итератор */

	class iterator
	{

		node* p; // узел, на который указывает

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type		= avltree::value_type;
		using difference_type	= avltree::difference_type;
		using pointer			= avltree::const_pointer;
		using reference			= avltree::const_reference;

		iterator(node* n) : p(n) {}

		iterator() : p(nullptr) {}

		reference operator*() const noexcept {
			return p->data;
		}

		node* operator->() const noexcept {
			return p;
		}

		node* _node() {
			return this->p;
		}

		iterator& operator++() noexcept {
			if (p->right->is_nil) {
				node* temp;
				while (!(temp = p->parent)->is_nil && temp->right == p) {
					p = temp;
				}

				p = temp;
			}
			else {
				p = p->right;
				while (!p->left->is_nil) {
					p = p->left;
				}
			}
			return *this;
		}

		iterator& operator--() noexcept {
			if (p->is_nil) {
				p = p->right;
			}
			else if (p->left->is_nil) {
				node* temp;
				while (!(temp = p->parent)->is_nil && temp->left == p) {
					p = temp;
				}

				if (!p->is_nil) {
					p = temp;
				}
			}
			else {
				p = p->left;
				while (!p->right->is_nil) {
					p = p->right;
				}
			}
			return *this;
		}

		iterator& operator++(int) noexcept {
			iterator temp = *this;
			++*this;
			return temp;
		}

		iterator& operator--(int) noexcept {
			iterator temp = *this;
			--*this;
			return temp;
		}

		bool operator!=(const iterator& other) const {
			return p != other.p;
		}

		bool operator==(const iterator& other) const {
			return p == other.p;
		}

		//  Эти операции не допускаются между прямыми и обратными итераторами
		const iterator& operator=(const reverse_iterator& it)	= delete;
		bool operator==(const reverse_iterator& it)				= delete;
		bool operator!=(const reverse_iterator& it)				= delete;
		iterator(const reverse_iterator& it)					= delete;
	};

	/* работа с итераторами в дереве */

	using const_iterator			= iterator;
	using reverse_iterator			= std::reverse_iterator<iterator>;
	using const_reverse_iterator	= std::reverse_iterator<const_iterator>;

	// начало коллекции
	iterator begin() const noexcept {
		return iterator(dummy->left);
	}

	// конец коллекции
	iterator end() const noexcept {
		return iterator(dummy);
	}

	// обратный итератор начала
	reverse_iterator rbegin() const	noexcept {
		return reverse_iterator(dummy->right);
	}

	// обратный итератор конца
	reverse_iterator rend() const noexcept {
		return reverse_iterator(dummy);
	}

	/* создание дерева */

	// default 
	avltree(Compare comparator = Compare(), allocator_type alloc = allocator_type())
		: dummy(make_dummy()), cmp(comparator), Alc(alloc) { }

	// конструктор по диапазону
	template <class InputIterator>
	avltree(InputIterator first, InputIterator last, Compare comparator = Compare(), allocator_type alloc = allocator_type())
		: dummy(make_dummy()), cmp(comparator), Alc(alloc) {

		while (first != last) {
			insert(*first++);
		}

	}

	// конструктор через список инициализации
	avltree(const std::initializer_list<T>& il) : dummy(make_dummy()) {
		for (const T& element : il) {
			insert(element);
		}
	}

	// конструктор копии
	avltree(const avltree& tree) : dummy(make_dummy()) {

		//  Размер задаём
		sz = tree.sz;
		if (tree.empty()) return;

		dummy->parent = recur_copy_tree(tree.dummy->parent, tree.dummy);
		dummy->parent->parent = dummy;

		//  Осталось установить min и max
		node* min = dummy->parent;
		while (!min->is_nil) {
			min = min->left;
		}
		dummy->left = min;

		node* max = dummy->parent;
		while (!max->is_nil) {
			max = max->right;
		}
		dummy->right = max;
	}

private:

	//  Рекурсивное копирование дерева
	node* recur_copy_tree(node* source, const node* source_dummy)
	{
		//  Сначала создаём дочерние поддеревья
		node* left_sub_tree;
		if (!source->left->is_nil)
			left_sub_tree = recur_copy_tree(source->left, source_dummy);
		else
			left_sub_tree = dummy;

		node* right_sub_tree;
		if (!source->right->is_nil)
			right_sub_tree = recur_copy_tree(source->right, source_dummy);
		else
			right_sub_tree = dummy;

		//  Теперь создаём собственный узел
		node* current = make_node(source->data, dummy, left_sub_tree, right_sub_tree);

		//  Устанавливаем родителей
		if (!source->right->is_nil)
			current->right->parent = current;
		if (!source->left->is_nil)
			current->left->parent = current;
		//  Ну и всё, можно возвращать
		return current;
	}

public:

	/* остальные методы */

	// копирующее =
	const avltree& operator=(const avltree& other) {
		if (this == &other) {
			return *this;
		}

		avltree tmp{ other };
		swap(tmp);

		return *this;
	}

	// move =
	const avltree& operator=(avltree&& other) {
		std::swap(dummy, other.dummy);
		std::swap(sz, other.sz);
		other.clear();
		return *this;
	}

	allocator_type get_allocator() const noexcept { return Alc; }

	value_compare value_comp() const noexcept { return cmp; }

	// проверка на пустоту контейнера
	inline bool empty() const noexcept {
		return sz == 0;
	}

	// количество элементов в дереве
	size_type size() const {
		return sz;
	}

	// смена значений двух деревьев
	void swap(avltree& other) noexcept {
		std::swap(dummy, other.dummy);
		std::swap(sz, other.sz);
	}

	// получение корня дерева
	node* get_root() const {
		return dummy->parent;
	}

	// печать дерева, заваленого набок
	void print(node* root, int indent = 0) const {

		if (sz == 0) {
			std::cout << "<empty tree>";
			return;
		}

		if (root == nullptr || root->is_nil)
			return;

		print(root->right, indent + 4);

		if (indent > 0)
			std::cout << std::setw(indent) << ' ';
		std::cout << root->data << std::endl;

		print(root->left, indent + 4);
	}

	// печать баланса каждого узла, заваленная набок
	void print_balance(node* root, int indent = 0) const {

		if (sz == 0) {
			std::cout << "<empty tree>";
			return;
		}

		if (root == nullptr || root->is_nil)
			return;

		print_balance(root->right, indent + 4);

		if (indent > 0)
			std::cout << std::setw(indent) << ' ';
		std::cout << (int)root->balance << std::endl;

		print_balance(root->left, indent + 4);
	}

private:

	/* вращения */

	// левое вращение
	void left_rotation(node* x, node* z) {

		node* n = z->left;
		x->right = n;
		if (!n->is_nil) {
			n->parent = x;
		}

		z->parent = x->parent;

		if (x->parent == dummy) {
			dummy->parent = z;
		}
		else if (x == x->parent->left) {
			x->parent->left = z;
		}
		else {
			x->parent->right = z;
		}

		z->left = x;
		x->parent = z;

		z->balance--;
		x->balance -= 2;
	}

	// правое вращение
	void right_rotation(node* x, node* z) {

		node* n = z->right;
		x->left = n;
		if (!n->is_nil) {
			n->parent = x;
		}

		z->parent = x->parent;

		if (x->parent == dummy) {
			dummy->parent = z;
		}
		else if (x == x->parent->left) {
			x->parent->left = z;
		}
		else {
			x->parent->right = z;
		}

		z->right = x;
		x->parent = z;

		z->balance++;
		x->balance += 2;
	}

	// большое левое вращение
	void left_right_rotation(node* x, node* z) {

		node* n = z->right;
		node* t = n->left;
		z->right = t;

		if (!t->is_nil) {
			t->parent = z;
		}

		n->left = z;
		z->parent = n;

		node* t2 = n->right;

		x->left = t2;
		if (!t2->is_nil) {
			t2->parent = x;
		}

		n->parent = x->parent;

		if (x->parent == dummy) {
			dummy->parent = n;
		}
		else if (x == x->parent->left) {
			x->parent->left = n;
		}
		else {
			x->parent->right = n;
		}

		n->right = x;
		x->parent = n;

		if (n->balance == 0) {
			x->balance = z->balance = 0;
		}
		else if (n->balance > 0) {
			x->balance = -1;
			z->balance = 0;
		}
		else {
			x->balance = 0;
			z->balance = 1;
		}
		n->balance = 0;

	}

	// большое правое вращение
	void right_left_rotation(node* x, node* z) {

		node* n = z->left;
		node* t = n->right;
		z->left = t;

		if (!t->is_nil) {
			t->parent = z;
		}

		n->right = z;
		z->parent = n;

		node* t2 = n->left;

		x->right = t2;
		if (!t2->is_nil) {
			t2->parent = x;
		}

		n->parent = x->parent;

		if (x->parent == dummy) {
			dummy->parent = n;
		}
		else if (x == x->parent->left) {
			x->parent->left = n;
		}
		else {
			x->parent->right = n;
		}

		n->left = x;
		x->parent = n;

		if (n->balance == 0) {
			x->balance = z->balance = 0;
		}
		else if (n->balance > 0) {
			x->balance = -1;
			z->balance = 0;
		}
		else {
			x->balance = 0;
			z->balance = 1;
		}
		n->balance = 0;

	}

	// перебалансировка
	void fixup(node* n) {

		if (n->is_nil) {
			return;
		}

		if (n->balance == -2) {
			right_rotation(n, n->left);
		}
		if (n->balance == 2) {
			left_rotation(n, n->right);
		}
		if (n->balance == 1 && n->parent->balance == -2) {
			left_right_rotation(n->parent, n);
		}
		if (n->balance == -1 && n->parent->balance == 2) {
			right_left_rotation(n->parent, n);
		}
		fixup(n->parent);
	}

public:

	// вставка элемента
	std::pair<iterator, bool> insert(const T& value) {

		if (sz == 0) {
			node* n = make_node(value, dummy, dummy, dummy);
			dummy->right = dummy->left = dummy->parent = n;
			sz++;
			return { iterator(n), true };
		}

		node* to = dummy->parent;
		node* parent = to->parent;

		while (!to->is_nil) {
			parent = to;
			if (cmp(value, to->data)) {
				to = to->left;
			}
			else if (cmp(to->data, value)) {
				to = to->right;
			}
			else { // дубликат

				while (!to->right->is_nil && !cmp(to->data, value) && !cmp(value, to->data)) {
					to = to->right;
				}

				node* n = make_node(value, to, dummy, dummy);

				if (!to->right->is_nil) {
					to->right->parent = n;
					n->right = to->right;
				}

				if (dummy->right == to) {
					dummy->right = n;
				}

				to->right = n;

				sz++;

				node* temp = n;
				parent = n->parent;

				while (!parent->is_nil) {
					if (temp == parent->left) {
						parent->balance--;
					}
					else {
						parent->balance++;
					}
					if (abs(parent->balance) == 2) {
						break;
					}
					temp = parent;
					parent = parent->parent;
				}

				fixup(n);

				return { iterator(n), true };
			}
		}

		// Вставляем узел
		node* n = make_node(value, parent, dummy, dummy);
		if (cmp(n->data, parent->data)) {
			parent->left = n;
			if (dummy->left == parent) {
				dummy->left = n;
			}
		}
		else {
			parent->right = n;
			if (dummy->right == parent) {
				dummy->right = n;
			}
		}
		sz++;

		// обновляем баланс узлов вверх по дереву
		node* temp = n;

		while (!parent->is_nil) {
			if (temp == parent->left) {
				parent->balance--;
			}
			else {
				parent->balance++;
			}
			if (std::abs(parent->balance) == 2) {
				break;
			}
			temp = parent;
			parent = parent->parent;
		}

		fixup(n);

		return { iterator(n), true };
	}

	// вставка элемента
	std::pair<iterator, bool> insert(T&& value) {

		if (sz == 0) {
			node* n = make_node(std::move(value), dummy, dummy, dummy);
			dummy->right = dummy->left = dummy->parent = n;
			sz++;
			return { iterator(n), true };
		}

		node* to = dummy->parent;
		node* parent = to->parent;

		while (!to->is_nil) {
			parent = to;
			if (cmp(value, to->data)) {
				to = to->left;
			}
			else if (cmp(to->data, value)) {
				to = to->right;
			}
			else { // дубликат

				while (!to->right->is_nil && !cmp(to->data, value) && !cmp(value, to->data)) {
					to = to->right;
				}

				node* n = make_node(std::move(value), to, dummy, dummy);

				if (!to->right->is_nil) {
					to->right->parent = n;
					n->right = to->right;
				}

				if (dummy->right == to) {
					dummy->right = n;
				}

				to->right = n;

				sz++;
				
				node* temp = n;
				parent = n->parent;

				while (!parent->is_nil) {
					if (temp == parent->left) {
						parent->balance--;
					}
					else {
						parent->balance++;
					}
					if (abs(parent->balance) == 2) {
						break;
					}
					temp = parent;
					parent = parent->parent;
				}
				
				fixup(n);

				return { iterator(n), true };
			}
		}

		// Вставляем узел
		node* n = make_node(std::move(value), parent, dummy, dummy);
		if (cmp(n->data, parent->data)) {
			parent->left = n;
			if (dummy->left == parent) {
				dummy->left = n;
			}
		}
		else {
			parent->right = n;
			if (dummy->right == parent) {
				dummy->right = n;
			}
		}
		sz++;

		// обновляем баланс узлов вверх по дереву
		node* temp = n;

		while (!parent->is_nil) {
			if (temp == parent->left) {
				parent->balance--;
			}
			else {
				parent->balance++;
			}
			if (std::abs(parent->balance) == 2) {
				break;
			}
			temp = parent;
			parent = parent->parent;
		}

		fixup(n);

		return { iterator(n), true };
	}

	// вставка диапазона элементов
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		while (first != last) {
			insert(*first++);
		}
	}

	// вставка в конкретную позицию
	iterator insert(const_iterator position, const value_type& x) {
		return std::get<0>(insert(x));
	}

	// поиск элемента
	iterator find(const value_type& value) const {

		iterator root(dummy->parent);

		while (!root->is_nil) {
			if (cmp(value, *root)) {
				--root;
			}
			else if (cmp(*root, value)) {
				++root;
			}
			else {
				return iterator(root);
			}
		}

		return dummy;
	}

	// возвращает итератор на первый элемент, по значению равный заданному или больший 
	iterator lower_bound(const value_type& key) {

		iterator root(dummy->parent);

		while (!root->is_nil) {
			if (cmp(key, *root)) {
				--root;
			}
			else if (!cmp(*root, key)) {
				return root;
			}
			else {

				while (cmp(*root, key)) {
					++root;
				}

				return root;
			}
		}

		return root;

	}

	// возвращает итератор на первый элемент, по значению равный заданному или больший 
	const_iterator lower_bound(const value_type& key) const {
		return const_iterator(const_cast<avltree*>(this)->lower_bound(key));
	}

	// возвращает итератор на первый элемент, по значению больший заданного
	iterator upper_bound(const value_type& key) {

		iterator root(dummy->parent);

		while (!root->is_nil) {
			if (cmp(key, *root)) {
				--root;
			}
			else if (!cmp(*root, key)) {
				return ++root;
			}
			else {

				while (cmp(*root, key)) {
					++root;
				}

				if (!cmp(*root, key) && !cmp(key, *root)) {
					return ++root;
				}
				else {
					return root;
				}
			}
		}

		return root;
	}

	// возвращает итератор на первый элемент, по значению больший заданного
	const_iterator upper_bound(const value_type& key) const {
		return const_iterator(const_cast<avltree*>(this)->upper_bound(key));
	}

	// подсчет количества элементов с заданным ключом в дереве
	size_type count(const value_type& key) const {

		size_type res{ 0 };

		iterator it = find(key);

		if (it != end()) {
			res++;

			iterator temp_left(it);
			iterator temp_right(it);

			while (*(--temp_left) == key) {
				res++;
			}
			while (*(++temp_right) == key) {
				res++;
			}
		}

		return res;
	}

	// диапазон, в котором все элементы равны
	std::pair<const_iterator, const_iterator> equal_range(const value_type& key) const {

		const_iterator first(find(key));

		if (first->is_nil) {
			return { end(), end() };
		}
		else {
			const_iterator left = first;
			const_iterator right = first;

			while (!cmp(*left, key) && !cmp(key, *left)) {
				--left;
			}
			// после выхода из цикла на позиции слева от элемента с искомым значением
			// поэтому передвинем
			if (left->is_nil) { // если на фиктивном узле - значит ушли из начала
				left = begin();
			}
			else {
				++left;
			}

			while (!cmp(*right, key) && !cmp(key, *right)) {
				++right;
			} // здесь не передвигаем, так как конец диапазона находится за ним (канон STL)

			return { left, right };

		}

	}

	// удаление элемента по итератору
	iterator erase(iterator elem) {

		if (elem->is_nil) {
			return end();
		}

		node* p = elem->parent;

		iterator res(elem);
		++res;

		if (elem->left->is_nil && elem->right->is_nil) { // удаляем лист

			if (p->is_nil) {

				dummy->parent = dummy->left = dummy->right = dummy;
				sz--;
				return end();
			}
			else if (elem == p->left) {

				p->left = dummy;
				if (dummy->left == elem._node()) {
					dummy->left = p;
				}

				p->balance++;

			}
			else {

				p->right = dummy;
				if (dummy->right == elem._node()) {
					dummy->right = p;
				}

				p->balance--;

			}

		}
		else if (elem->left->is_nil || elem->right->is_nil) { // удаляем узел с 1 потомком

			node* child = elem->left->is_nil ? elem->right : elem->left;

			if (!p->is_nil) {
				if (p->left == elem._node()) {
					p->left = child;
					p->balance++;
				}
				else {
					p->right = child;
					p->balance--;
				}
			}
			else { // ситуация, когда в дереве 2 элемента
				p->parent = child;
			}

			child->parent = p;

		}
		else { // есть оба потомка

			// найдем ближайший по значению
			iterator closest(elem._node());

			closest = closest->left;
			while (!closest->right->is_nil) {
				closest = closest->right;
			}

			std::swap(closest._node()->data, elem._node()->data);

			return erase(closest);

		}

		node* broken(p);
		node* pp(p->parent);

		while (!pp->is_nil) {
			if (pp->balance == 0) {
				break;
			}
			if (pp->left == p) {
				pp->balance--;
			}
			else {
				pp->balance++;
			}
			if (abs(pp->balance) == 2) {
				break;
			}
			p = pp;
			pp = pp->parent;
		}

		sz--;
		delete_node(elem._node());

		fixup(broken);

		return res;

	}

	// удаление элемента по ключу
	size_type erase(const value_type& elem) {
		iterator it = find(elem);
		if (!it->is_nil) {
			erase(it);
			return 1;
		}
		else {
			return 0;
		}
	}

	// удаление диапазона элементов
	iterator erase(const_iterator first, const_iterator last) {
		while (first != last) {
			first = erase(first);
		}
		return first;
	}

private:

	// вспомогательное для сравнения

	// равенство по структуре и значениям
	bool equals(node* curr, node* other) const {
		if (curr->is_nil && other->is_nil) {
			return true;
		}
		if (!curr->is_nil && !other->is_nil) {
			return curr->data == other->data
				&& equals(curr->left, other->left)
				&& equals(curr->right, other->right);
		}
		return false;
	}

	// <
	bool less(node* curr, node* other) const {
		if (curr->is_nil && other->is_nil) {
			return true;
		}
		else if (curr->is_nil && !other->is_nil) {
			return true;
		}
		else if (!curr->is_nil && other->is_nil) {
			return false;
		}
		else {
			return curr->data < other->data
				&& less(curr->left, other->left)
				&& less(curr->right, other->right);
		}
		return false;
	}

	// <=
	bool less_equal(node* curr, node* other) const {
		if (curr->is_nil && other->is_nil) {
			return true;
		}
		else if (curr->is_nil && !other->is_nil) {
			return true;
		}
		else if (!curr->is_nil && other->is_nil) {
			return false;
		}
		else {
			return curr->data <= other->data
				&& less_equal(curr->left, other->left)
				&& less_equal(curr->right, other->right);
		}
	}

public:
	// сравнение содержимого

	// равенство содержимого
	bool content_equal(const avltree& other) const {

		if (sz != other.sz) {
			return false;
		}

		bool res = true;

		auto it1 = begin();
		auto it2 = other.begin();

		while (it1 != end()) {
			if (*it1++ != *it2++) {
				res = false;
				break;
			}
		}

		return res;

	}

	// операции сравнения

	bool operator==(const avltree& other) const {
		return equals(dummy->parent, other.dummy->parent);
	}

	bool operator!=(const avltree& other) const {
		return !(*this == other);
	}

	bool operator>=(const avltree& other) const {
		return !(*this < other);
	}

	bool operator<=(const avltree& other) const {
		return less_equal(dummy->parent, other.dummy->parent);
	}

	bool operator> (const avltree& other) const {
		return !(*this <= other);
	}

	bool operator< (const avltree& other) const {
		return less(dummy->parent, other.dummy->parent);
	}

	//  очистка дерева (без удаления фиктивной вершины)
	void clear() {
		Free_nodes(dummy->parent);
		sz = 0;
		dummy->parent = dummy->left = dummy->right = dummy;
	}

private:

	//  рекурсивное удаление узлов дерева, не включая фиктивную вершину
	void Free_nodes(node* n) {
		if (n->is_nil) {
			return;
		}
		if (!n->is_nil) {
			Free_nodes(n->left);
			Free_nodes(n->right);
			delete_node(n);
		}
	}

public:

	~avltree() {
		clear(); // рекурсивный деструктор
		delete_dummy(dummy);
	}

};
