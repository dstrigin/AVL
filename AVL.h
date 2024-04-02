#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <initializer_list>
#include <exception>

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
		T key;				// значение
		int balance;		// разность количества дочерних узлов в левом и правом поддеревьях
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
				while (p->left) {
					p = p->left;
				}
			}
			return *this;
		}

		iterator& operator--() noexcept {
			if (p->left->is_nil) {
				node* temp;
				while (!(temp = p->parent)->is_nil && temp->left == p) {
					p = temp;
				}

				p = temp;
			}
			else {
				p = p->left;
				while (p->right) {
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

	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

};
