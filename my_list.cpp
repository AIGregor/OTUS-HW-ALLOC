#pragma once
/*
–еализовать свой контейнер, который по аналогии с контейнерами stl
параметризуетс€ аллокатором.  онтейнер должен иметь две возможности
- добавить новый элемент и обойти контейнер в одном направлении.
—овместимость с контейнерами stl на усмотрение автора.
*/
#include "stdafx.h"
#include <memory>

static bool has_initialized = false;
static size_t blockCount = 0;
static void* last_valid_address = nullptr;
static void* managed_memory_start = nullptr;

template< typename T>
class my_list_alloc {
public:
	using value_type = T;

	using reference = T&;
	using const_reference = const T&;

	using pointer = T*;
	using const_pointer = const T*;

	void reserve(size_t n);
	// Default constructor, copy constructor, rebinding constructor, and destructor.
	// Empty for stateless allocators.
	my_list_alloc() = default;

	my_list_alloc(const my_list_alloc&) = default;

	template<typename U> my_list_alloc(const my_list_alloc<U>&) {};

	~my_list_alloc() = default;

	T* allocate(size_t n);
	void deallocate(T* p, size_t n);

	void destroy(T* p) const {

		std::cout << "destroy" << std::endl;
		p->~T();
	}

	template<class T, class... Args>
	void construct(T* p, Args&&... args) {

		std::cout << "construct" << std::endl;
		new (p) T(forward<Args>(args)...);
	}

	template<class U>
	struct rebind {
		using other = my_list_alloc<U>;
	};
private:
	void* ptrTemp = nullptr;

};

template <class T, class U>
bool operator==(const my_list_alloc<T>& t, const my_list_alloc<U>& u) {};

template <class T, class U>
bool operator!=(const my_list_alloc<T>&, const my_list_alloc<U>&) {};


template<typename T>
void my_list_alloc<T>::reserve(size_t n)
{
	has_initialized = true;
	managed_memory_start = this->allocate(n);	
	
	last_valid_address = managed_memory_start;
	blockCount = n;
}

template<typename T>
inline T * my_list_alloc<T>::allocate(std::size_t n)
{
	if (n == 0) 
		return NULL;
	
	T* endBuffer = (static_cast<T*>(managed_memory_start) + blockCount);
	T* last_address = (static_cast<T*>(last_valid_address) + n);

	if (endBuffer >= last_address)
	{
		std::cout << "shift alloc: [n = " << n << "]" << std::endl;

		T* last_valid = static_cast<T*>(last_valid_address);
		last_valid_address = last_valid + n;

		return static_cast<T* >(last_valid);
	}
	else
	{	
		std::cout << "allocate: [n = " << n << "]" << std::endl;

		auto p = std::malloc(n * sizeof(T));
		if (!p)
			throw std::bad_alloc();
		return static_cast<T* >(p);
	}

}

template<typename T>
inline void my_list_alloc<T>::deallocate(T * p, size_t n)
{	
	if (p > managed_memory_start && p <= last_valid_address && has_initialized)
	{
		std::cout << "shift dealloc: [n  = " << n << "] " << std::endl;
		return;
	}
	else 
	{
		std::cout << "deallocate: [n  = " << n << "] " << std::endl;
		std::free(p);
		if (p == managed_memory_start)
		{
			has_initialized = false;
			managed_memory_start = nullptr;
			last_valid_address = nullptr;
		}		
	}
}

/*-----------------------------------------------------------------------------*/

template <typename T>
struct Node
{
	Node();
	~Node();
	Node<T>* ptrNextNode = nullptr;
	Node<T>* ptrPrevNode = nullptr;
	T value;
};

template<typename T>
Node<T>::Node()
{
}

template<typename T>
Node<T>::~Node()
{
}

template <typename T, typename Alloc = std::allocator<Node<T>>>
class my_list
{
	using my_node = Node<T>;
	using my_nodeptr = Node<T>*;

public:
	my_list();
	~my_list();

	T& getValue(size_t index);
	bool insert(const T value);
	bool remove(size_t index);

	Alloc& getAlloc() { return my_list_allocator; };

	my_nodeptr begin() { return head; };
	my_nodeptr end() { return NULL; };

	size_t size() { return mSize; }

private:
	my_nodeptr head;
	my_nodeptr tail;

	Alloc my_list_allocator;
	
	size_t mSize;

	my_nodeptr createNode()
	{
		my_nodeptr newNode = getAlloc().allocate(1);

		try
		{
			getAlloc().construct(&newNode->ptrNextNode, newNode);
			getAlloc().construct(&newNode->ptrPrevNode, newNode);
		}
		catch (...)
		{
			getAlloc().deallocate(newNode, 1);
		}

		return newNode;
	};


};

template<typename T, typename Alloc>
my_list<T, Alloc>::my_list() : mSize(0), tail(nullptr)
{
	head = createNode();
}

template<typename T, typename Alloc>
my_list<T, Alloc>::~my_list()
{
}

template<typename T, typename Alloc>
T& my_list<T, Alloc>::getValue(size_t index)
{
	my_node* ptrNext = head;
	size_t szIndex = 0;
	while (ptrNext->ptrNextNode != nullptr && szIndex <= index)
	{
		ptrNext = static_cast<my_node*>(ptrNext->ptrNextNode);
		++szIndex;
	}

	return ptrNext->value;
}

template<typename T, typename Alloc>
bool my_list<T, Alloc>::insert(const T value)
{
	try 
	{
		my_node* ptrNext = head;
		while (ptrNext->ptrNextNode != ptrNext && ptrNext->ptrNextNode != NULL)
		{
			ptrNext = static_cast<my_node*>(ptrNext->ptrNextNode);
		}
		my_nodeptr ptrNewNode = createNode();
		ptrNext->ptrNextNode = ptrNewNode;

		ptrNewNode->ptrPrevNode = ptrNext;
		ptrNewNode->ptrNextNode = nullptr;

		ptrNewNode->value = value;
		++mSize;
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}

template<typename T, typename Alloc>
bool my_list<T, Alloc>::remove(size_t index)
{
	try
	{
		Node* ptrNext = head;
		size_t szIndex = 0;
		while (ptrNext->ptrNextNode != nullptr || szIndex != index)
		{
			ptrNext = static_cast<Node*>(ptrNext->ptrNextNode);
			++szIndex;
		}
		Node* ptrPrevNode = static_cast<Node*>(ptrNext->ptrPrevNode);
		ptrPrevNode->ptrNextNode = ptrNext->ptrNextNode;

		Node* ptrNextNode = static_cast<Node*>(ptrNext->ptrNextNode);
		ptrNextNode->ptrPrevNode = ptrNext->ptrPrevNode;
				
		delete ptrNewNode;
		--mSize;
	}
	catch (...)
	{
		return false;
	}

	return true;
}


