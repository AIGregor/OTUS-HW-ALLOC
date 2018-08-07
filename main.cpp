// alloc.cpp : Defines the entry point for the console application.
//

#include <map>
#include <vector>
#include <iostream>

#define MAP
#define M_LIST

/*
–еализовать свой контейнер, который по аналогии с контейнерами stl
параметризуетс€ аллокатором.  онтейнер должен иметь две возможности
- добавить новый элемент и обойти контейнер в одном направлении.
—овместимость с контейнерами stl на усмотрение автора.
*/
#include <memory>

using namespace std;

/*-----------------------------------------------------------------------------*/
/*
јллокатор с выделением пам€ти
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
	void* ptrTemp = nullptr;

	static size_t blockCount;
	static bool has_initialized;
	static void* last_valid_address;
	static void* managed_memory_start;
};

template<typename T> size_t my_list_alloc<T>::blockCount = 0;
template<typename T> bool  my_list_alloc<T>::has_initialized = false;
template<typename T> void* my_list_alloc<T>::last_valid_address = nullptr;
template<typename T> void* my_list_alloc<T>::managed_memory_start = nullptr;


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
		return static_cast<T* >(p);
	}

}

template<typename T>
inline void my_list_alloc<T>::deallocate(T * p, size_t n)
{
	if (p > managed_memory_start && p <= last_valid_address && has_initialized)
	{
		//std::cout << "shift dealloc: [n  = " << n << "] " << std::endl;
		return;
	}
	else
	{
		//std::cout << "deallocate: [n  = " << n << "] " << std::endl;
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
/*
¬спомогательный класс узлов списка
*/
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

/*-----------------------------------------------------------------------------*/
/*
—амописный контейнер - двунаправленный список
*/
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

	void clearAll();
	void freeNode(my_nodeptr node);

	void incSize() { ++mSize; };
	void decSize() { --mSize; };
	void setTail(my_nodeptr node) { tail = node; };
	void setHead(my_nodeptr node) { head = node; };
};

template<typename T, typename Alloc>
my_list<T, Alloc>::my_list() : mSize(0), tail(nullptr)
{
	setHead(createNode());
}

template<typename T, typename Alloc>
my_list<T, Alloc>::~my_list()
{
	// TODO: –асписать удаление элементов списка и освобождение пам€ти.
	clearAll();
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

		setTail(ptrNewNode);
		incSize();
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
		my_nodeptr ptrNext = head;
		size_t szIndex = 0;
		while (ptrNext->ptrNextNode != nullptr || szIndex != index)
		{
			ptrNext = static_cast<my_nodeptr>(ptrNext->ptrNextNode);
			++szIndex;
		}
		my_nodeptr ptrPrevNode = static_cast<my_nodeptr>(ptrNext->ptrPrevNode);
		ptrPrevNode->ptrNextNode = ptrNext->ptrNextNode;

		my_nodeptr ptrNextNode = static_cast<my_nodeptr>(ptrNext->ptrNextNode);
		ptrNextNode->ptrPrevNode = ptrNext->ptrPrevNode;

		decSize();
	}
	catch (...)
	{
		return false;
	}

	return true;
}

template<typename T, typename Alloc>
void my_list<T, Alloc>::clearAll()
{
	mSize = 0;
	my_nodeptr ptrPrev = tail;

	while (ptrPrev != ptrPrev->ptrPrevNode)
	{
		my_nodeptr ptrPrevNode = static_cast<my_nodeptr>(ptrPrev->ptrPrevNode);
		freeNode(ptrPrev);
		ptrPrev = ptrPrevNode;
	}

	freeNode(head);
}

template<typename T, typename Alloc>
void my_list<T, Alloc>::freeNode(my_nodeptr node)
{
	getAlloc().destroy(node->ptrNextNode);
	getAlloc().destroy(node->ptrPrevNode);
	getAlloc().destroy(node);

	getAlloc().deallocate(node, 1);
}


template <class K, class V> 
using a_map = std::map< K, V, std::less<K>, my_list_alloc< std::_Tree_node<std::pair<int const, int>, void *>> >;

constexpr size_t Fuct(size_t number)
{
	return number > 0 ? Fuct(number-1) * number : 1;
}

template<class K, class V, class C = std::less<K>, class Al = std::allocator<K>>
void generate(std::map<K, V, C, Al>& target, size_t range)
{
	for (size_t i = 0; i < range; ++i) {
		// std::cout << "map size = " << target.size() << std::endl;
		target.insert(std::make_pair(i, Fuct(i)));
	}
}

template<class T, class Al = std::allocator<Node<T>>>
void generate(my_list<T, Al>& target, size_t range)
{
	for (size_t i = 0; i < range; ++i) {
		// std::cout << "map size = " << target.size() << std::endl;
		target.insert(i);
	}
}

int main()
{

#ifdef MAP 
	
	// Standart map with standart allocator
	auto std_map = std::map<int, int>();
	generate(std_map, 10);

	// Stasndart map with my allocator and reservation 10 items
	auto my_map = a_map<int, int>();
	my_map.get_allocator().reserve(10);

	// Fill map
	generate(my_map, 10);

	// Print map
	for (auto i : my_map) {
		std::cout << i.first << " " << i.second << std::endl;
	}

#endif // !MAP

#ifdef M_LIST

	// My list with standart allocator
	my_list<int> std_my_list;
	generate(std_my_list, 10);
	
	// My list with my allocator and reservation 10 items
	my_list<int, my_list_alloc<Node<int>>> my_list_alloc;
	my_list_alloc.getAlloc().reserve(10);
	generate(my_list_alloc, 10);

	// Print list
	size_t i = 0;
	while (i < my_list_alloc.size()) {
		std::cout << my_list_alloc.getValue(i) << std::endl;
		++i;
	}

#endif // !M_LIST

	return 0;
}