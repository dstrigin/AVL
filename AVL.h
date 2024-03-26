#pragma once

#include <iostream>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <initializer_list>
#include <exception>

/*
��������� ������ �������� � ���� ����������, � ���������� ���������� ��������������� ���������.
����� ������� ���������� ������ ���� ��������������� (��������),
������ ������ ����������� ���������� � ��������������� ������� �������.
�������� ������� ������ ��������� � �������� � ����������� set � multiset ����������� ����������.
������ ���� ����������� ������ upper_bound � lower_bound.
���������: ��������� ���������� � ����������� ���������� ������������ ��������� ���� ����������,
����������� � ��������� ��������������� ���������� � ����������� � �������������, ������ � ��������.
����������� � ������������� ����� ���������� ��������� � ��������, �� ����, ������������ �����������.
��������������, ������������� ����������� �������� ���� �� �����, ���� ������� ��� �
������������ ��������� ��� ��������� ��������������. �������� ��������� ������������� �� ���������.

����������� ����������� ���� ������ ��� ���������������� ���� ���������� (����������� ������ �������� � ������,
����� ���� ������ �� ��������� �����).

�� ������ ��������� ��������� ������ ����������� ������ ���������, �������������� ��������� ��������:

1. ����������, ���������� ���������, ����� (�������� �� ���������);
2. ����� ����������� ��������� ������ �� ����� ������ (���� ���� ������� ����������� ������);
3. ��� ������ �������� ���������� � �������� ����������� ������� ���������� �������� ��� �������������� ������������;
4. ���������� � ���� � ������ �� ����� (���������);
5. �������� ������������� ���������� ��������� ������ � ��������������� ����������� ����������� � ��������� ����� �� ��������� ������������������� ��������.
*/

template<typename T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
class avltree {

	struct node {

		T key; // ��������
		int height; // ������ (��� ������-�������)
		node* parent; // ������������ ����
		node* left; // ����� �������� ����
		node* right;// ������ �������� ����
		int duplicates; // ���������� ���������� (multiset)
		bool is_nil; // �������� �� ��������� �����

		node(const T& k = T(), int h = 1, node* p = nullptr, node* l = nullptr, node* r = nullptr)
			: key(k), height(h), parent(p), left(l), right(r), duplicates(1), is_nil(false) { }

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

	/* � ������� ������������ � ����� ������ ��������� ������. � ���� �������� ����������� ���,
	����� ��� ����� ��������� �� ��������� */

	// ��������� ����
	node* dummy;

	size_t sz = 0;

	// �������� ��������� �������
	inline node* make_dummy() {
		dummy = Alc.allocate(1);

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->parent));
		dummy->parent = dummy;

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->left));
		dummy->left = dummy;

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->right));
		dummy->right = dummy;

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->height), 0);

		std::allocator_traits<allocator_type>::construct(Alc, &(dummy->is_nil), true);

		return dummy;
	}

	// �������� ����
	inline node* make_node(T&& val, node* p = nullptr, node* l = nullptr, node* r = nullptr) {
		node* new_node = Alc.allocate(1);

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->parent));
		new_node->parent = p;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->left));
		new_node->left = l;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->right));
		new_node->right = r;

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->data), std::forward<T>(val));

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->height), new_node->parent->height + 1);

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->duplicates), 1);

		std::allocator_traits<allocator_type>::construct(Alc, &(new_node->is_nil), false);

		return new_node;
	}

	// �������� ��������� �������
	inline void delete_dummy(node* node) {
		std::allocator_traits<allocator_type>::destroy(Alc, &(node->parent));
		std::allocator_traits<allocator_type>::destroy(Alc, &(node->left));
		std::allocator_traits<allocator_type>::destroy(Alc, &(node->right));
		std::allocator_traits<allocator_type>::deallocate(Alc, node, 1);
	}

	// �������� ����
	inline void delete_node(node* node) {
		//  ��� ������� ���� ������ (���������� ����������), � ��������� ������� ��� ��, ��� � ���������
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
				/* TODO */
			}

			iterator& operator--() noexcept {
				/* TODO */
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

			//  ����� ������� ������ ��������
			iterator GetMin() {
				while (p->left && !p->left->is_nil) {
					p = p->left;
				}
				return p;
			}

			//  ����� ������� ������� ��������
			iterator GetMax() {
				while (p->right && !p->right->is_nil) {
					p = p->right;
				}
				return p;
			}

			bool      operator!=(const iterator& other) const {
				return p != other.p;
			}

			bool      operator==(const iterator& other) const {
				return p == other.p;
			}

			//  ��� �������� �� ����������� ����� ������� � ��������� �����������
			const iterator& operator=(const reverse_iterator& it) = delete;
			bool operator==(const reverse_iterator& it) = delete;
			bool operator!=(const reverse_iterator& it) = delete;
			iterator(const reverse_iterator& it) = delete;
		};

		using const_iterator = iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

};
