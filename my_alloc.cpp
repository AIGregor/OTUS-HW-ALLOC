#pragma once
#include <set>

using namespace std;
/*-----------------------------------------------------------------------------*/
/*
Аллокатор с выделением памяти
*/

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
		//std::cout << "destroy" << std::endl;
		p->~T();
	}

	template<class T, class... Args>
	void construct(T* p, Args&&... args) {
		//std::cout << "construct" << std::endl;
		new (p) T(forward<Args>(args)...);
	}

	template<class U>
	struct rebind {
		using other = my_list_alloc<U>;
	};
private:
	set<void*> alloc_block;
	
	size_t blockCount = 0;
	bool has_initialized = false;
	void* last_valid_address = nullptr;
	void* managed_memory_start = nullptr;
};

template <class T, class U>
bool operator==(const my_list_alloc<T>& t, const my_list_alloc<U>& u) {};

template <class T, class U>
bool operator!=(const my_list_alloc<T>&, const my_list_alloc<U>&) {};


template<typename T>
void my_list_alloc<T>::reserve(size_t n)
{
	// reserve() only one time.
	if (has_initialized)
		return;

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
		//std::cout << "shift alloc: [n = " << n << "]" << std::endl;
		T* last_valid = static_cast<T*>(last_valid_address);
		last_valid_address = last_valid + n;
		
		return static_cast<T* >(last_valid);
	}
	else
	{
		//std::cout << "allocate: [n = " << n << "]" << std::endl;
		auto p = std::malloc(n * sizeof(T));
		if (!p)
			throw std::bad_alloc();
		
		alloc_block.insert(p);
		return static_cast<T* >(p);
	}

}

template<typename T>
inline void my_list_alloc<T>::deallocate(T* p, size_t n)
{
	if (p > managed_memory_start && p <= last_valid_address && has_initialized)
	{
		//std::cout << "shift dealloc: [n  = " << n << "] " << std::endl;
		return;
	}
	else
	{
		//std::cout << "deallocate: [n  = " << n << "] " << std::endl;
		if (alloc_block.find(p) == alloc_block.end())
			return;

		std::free(p);
		if (p == managed_memory_start)
		{
			has_initialized = false;
			managed_memory_start = nullptr;
			last_valid_address = nullptr;
		}
	}
}