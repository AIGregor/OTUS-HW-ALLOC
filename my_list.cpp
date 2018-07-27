#pragma once
/*
–еализовать свой контейнер, который по аналогии с контейнерами stl
параметризуетс€ аллокатором.  онтейнер должен иметь две возможности
- добавить новый элемент и обойти контейнер в одном направлении.
—овместимость с контейнерами stl на усмотрение автора.
*/
#include "stdafx.h"
#include <memory>

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

static bool has_initialized = false;
//static size_t blockCount = 0;
//static void* last_valid_address = nullptr;
//static void* managed_memory_start = nullptr;

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
	//managed_memory_start = this->allocate(n);
	
	ptrTemp = allocate(n);

	//last_valid_address = managed_memory_start;
	//blockCount = n;

	//std::free(managed_memory_start);
}

template<typename T>
inline T * my_list_alloc<T>::allocate(std::size_t n)
{
	if (n == 0) 
		return NULL;
	
	auto sz = sizeof(T);

	//T* endBuffer = (static_cast<T*>(managed_memory_start) + blockCount);
	//T* last_address = (static_cast<T*>(last_valid_address) + n);

	if (ptrTemp != nullptr)
	{
		std::cout << "shift pointer: [n = " << n << "]" << std::endl;

		//T* last_valid = static_cast<T*>(last_valid_address);
		//last_valid_address = last_valid + n;

		return static_cast<T* >(ptrTemp);
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
	std::cout << "deallocate: [n  = " << n << "] " << std::endl;

	//if (p <= managed_memory_start && has_initialized)
	//{
	//	//std::free(managed_memory_start);
	//	std::free(p);
	//	has_initialized = false;
	//}
	//else
	//{
		std::free(p);
	//}	

}

template<typename T, typename alloc>
my_list<T, alloc>::my_list()
{
}

template<typename T, typename alloc>
my_list<T, alloc>::~my_list()
{
}

template<typename T, typename alloc>
T my_list<T, alloc>::getValue(size_t index)
{
	return T();
}

template<typename T, typename alloc>
bool my_list<T, alloc>::insert(size_t index)
{
	return false;
}

template<typename T, typename alloc>
bool my_list<T, alloc>::remove(size_t index)
{
	return false;
}
