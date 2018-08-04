// alloc.cpp : Defines the entry point for the console application.
//

#include <map>
#include <vector>
#include <iostream>

#include "my_list.cpp"

#define MAP
#define M_LIST

using namespace std;

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