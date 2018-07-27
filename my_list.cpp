#pragma once
/*
–еализовать свой контейнер, который по аналогии с контейнерами stl
параметризуетс€ аллокатором.  онтейнер должен иметь две возможности
- добавить новый элемент и обойти контейнер в одном направлении.
—овместимость с контейнерами stl на усмотрение автора.
*/

#include "stdafx.h"
#include <memory>
#include <typeinfo>

using namespace std;

template <typename T, typename alloc = my_list_alloc<T>>
class my_list
{
public:
	my_list();
	~my_list();

	T getValue(size_t index);
	bool insert(size_t index);
	bool remove(size_t index);

private:
	T* ptrNextNode;
	T* ptrPrevNode;

	T value;
	alloc my_list_allocator;
};

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
	static T* _begin = nullptr;
	static T* _end = nullptr;
};

template <class T, class U>
bool operator==(const my_list_alloc<T>& t, const my_list_alloc<U>& u) {};

template <class T, class U>
bool operator!=(const my_list_alloc<T>&, const my_list_alloc<U>&) {};


template<typename T>
void my_list_alloc<T>::reserve(size_t n)
{
	_begin = this->allocate(n);
	_end = _begin + n;
}

template<typename T>
inline T * my_list_alloc<T>::allocate(std::size_t n)
{

	if (n == 0) {
		return NULL;
	}
	
	std::cout << "allocate: [n = " << n << "]" << std::endl;

	if (_begin != _end)
		cout << "False" << endl;

	auto p = std::malloc(n * sizeof(T));
	if (!p)
		throw std::bad_alloc();
	return static_cast<T* >(p);
}

template<typename T>
inline void my_list_alloc<T>::deallocate(T * p, size_t n)
{
	std::cout << "deallocate: [n  = " << n << "] " << std::endl;

	std::free(p);
}

template<typename T, typename alloc>
my_list<T, alloc>::my_list()
{
}

template<typename T, typename alloc>
my_list<T, alloc>::~my_list()
{
}