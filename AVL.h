#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <initializer_list>
#include <exception>
#include <iomanip>

/*
Структуру данных оформить в виде контейнера, с поддержкой итераторов соответствующей категории.
Любые аспекты реализации должны быть инкапсулированы (спрятаны),
работа только посредством итераторов и соответствующих методов шаблона.
Названия методов должны совпадать с таковыми у контейнеров set и multiset стандартной библиотеки.
Должны быть реализованы методы upper_bound и lower_bound.
Замечание: указанные контейнеры в стандартной библиотеке поддерживают различные типы итераторов,
относящиеся к категории двунаправленных итераторов – константные и неконстантные, прямые и обратные.
Константные и неконстантные имеют одинаковую семантику и являются, по сути, константными итераторами.
Соответственно, реализовывать константный итератор либо не нужно, либо сделать это с
минимальными затратами как псевдоним неконстантного. Обратные итераторы реализовывать не требуется.

Обязательно прохождение всех тестов для соответствующего типа контейнера (тестирующий проект размещен в архиве,
также есть ссылка на обучающее видео).

На основе выбранной структуры данных реализовать шаблон множества, поддерживающий следующие операции:

1. Добавление, извлечение элементов, поиск (проверка на вхождение);
2. Вывод содержимого структуры данных на экран «боком» (если явно указана возможность печати);
3. Для каждой операции добавления и удаления реализовать подсчет количества вращений для восстановления балансировки;
4. Сохранение в файл и чтение из файла (текстовый);
5. Сравнить эффективность реализации структуры данных с соответствующим стандартным контейнером – временные тесты на некоторых последовательностях значений.
*/

/*
1. инвалидация после удаления
2. компаратор
*/

template<typename T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
class avltree {

	struct node {
		T data;				// значение
		int balance;		// разность количества дочерних узлов в правом и левом поддеревьях
		node* parent;		// родительский узел
		node* left;			// левый дочерний узел
		node* right;		// правый дочерний узел
		bool is_nil;		// является ли фиктивным узлом
	};

	Compare cmp = Compare();

	using node_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

	node_allocator_type Alc;

public:

	using key_type = T;
	using key_compare = Compare;
	using value_type = T;
	using value_compare = Compare;
	using allocator_type = typename node_allocator_type;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using reference = value_type&;
	using const_reference = const value_type&;
	class iterator;
	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:

	/* В прошлой лабораторной я делал другую структуру дерева. В этой попробую реализовать так,
	чтобы все листы ссылались на фиктивный */

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
	inline node* make_node(T&& val, node* p = nullptr, node* l = nullptr, node* r = nullptr) {
		node* new_node = Alc.allocate(1);

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->parent));
		new_node->parent = p;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->left));
		new_node->left = l;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->right));
		new_node->right = r;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->data), std::forward<T>(val));

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
		node* p;

	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = avltree::value_type;
		using difference_type = avltree::difference_type;
		using pointer = avltree::const_pointer;
		using reference = avltree::const_reference;

		iterator(node* n) : p(n) {}

		reference operator*()  const noexcept {
			return p->data;
		}

		node* operator->() const noexcept {
			return p;
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
		const iterator& operator=(const reverse_iterator& it) = delete;
		bool operator==(const reverse_iterator& it) = delete;
		bool operator!=(const reverse_iterator& it) = delete;
		iterator(const reverse_iterator& it) = delete;
	};

	/* работа с итераторами в дереве */

	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	iterator begin() const noexcept {
		return iterator(dummy->left);
	}

	iterator end() const noexcept {
		return iterator(dummy);
	}

	reverse_iterator rbegin() const	noexcept {
		return reverse_iterator(dummy->right);
	}

	reverse_iterator rend() const noexcept {
		return reverse_iterator(dummy);
	}

	/* создание дерева */
	
	avltree(Compare comparator = Compare(), allocator_type alloc = allocator_type())
		: dummy(make_dummy()), cmp(comparator), Alc(alloc) {}

	/* остальные методы */

	allocator_type get_allocator() const noexcept { return Alc; }

	value_compare value_comp() const noexcept { return cmp; }

	inline bool empty() const noexcept {
		return sz == 0;
	}

	size_type size() const {
		return sz;
	}

	void swap(avltree& other) noexcept {
		std::swap(dummy, other.dummy);
		std::swap(sz, other.sz);
	}

	node* get_root() {
		return dummy->parent;
	}

	void print(node* root, int indent = 0) {
		if (root == nullptr || root->is_nil)
			return;

		print(root->right, indent + 4);

		if (indent > 0)
			std::cout << std::setw(indent) << ' ';
		std::cout << root->data << std::endl;

		print(root->left, indent + 4);
	}

	void print_balance(node* root, int indent = 0) {
		if (root == nullptr || root->is_nil)
			return;

		print_balance(root->right, indent + 4);

		if (indent > 0)
			std::cout << std::setw(indent) << ' ';
		std::cout << root->balance << std::endl;

		print_balance(root->left, indent + 4);
	}

private:

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

	}

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

	}

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
		if (n->balance == 1 && n->parent->balance < n->balance && !n->parent->is_nil) {
			left_right_rotation(n->parent, n);
		}
		if (n->balance == -1 && n->parent->balance > n->balance && !n->parent->is_nil) {
			right_left_rotation(n->parent, n);
		}
		fixup(n->parent);
	}

public:
	
	std::pair<iterator, bool> insert(const T& value) {
		if (sz == 0) {

			node* n = make_node(0, dummy, dummy, dummy);
			n->data = value;
			dummy->right = dummy->left = dummy->parent = n;
			sz++;
			return { dummy->left, true };

		}
		else {

			node* to = dummy->parent;

			while (true) {

				if (cmp(value, to->data) && !to->left->is_nil) {
					to = to->left;
				}
				else if (value > to->data && !to->right->is_nil) {
					to = to->right;
				}
				else if (value == to->data) {
					node* n = make_node(0, to, to->left, dummy);
					n->data = value;

					if (!to->left->is_nil) {
						to->left->parent = n;
					}

					to->left = n;

					if (dummy->left == to) {
						dummy->left = n;
					}

					return { iterator(n), true };

				}
				else if (cmp(value, to->data)) {
					
					node* n = make_node(0, to, dummy, dummy);
					n->data = value;

					to->left = n;

					if (dummy->left == to) {
						dummy->left = n;
					}

					while (!to->is_nil) {
						to->balance--;
						if (to->balance == -2) {
							break;
						}
						to = to->parent;
					}

					fixup(n->parent);

					return { iterator(n), true };

				}
				else {

					node* n = make_node(0, to, dummy, dummy);
					n->data = value;

					to->right = n;

					if (dummy->right == to) {
						dummy->right = n;
					}

					while (!to->is_nil) {
						to->balance++;
						if (to->balance == 2) {
							break;
						}
						to = to->parent;
					}

					fixup(n->parent);

					return { iterator(n), true };

				}
			
			}

		}

	}

};
