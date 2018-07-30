// alloc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <map>
#include <vector>
#include <iostream>

#include "my_list.cpp"
#include <list>

#define ONE
//#define VEC

using namespace std;

template <class K, class V> 
using a_map = std::map< K, V, std::less<K>, my_list_alloc< std::_Tree_node<std::pair<int const, int>, void *>> >;

constexpr size_t Fuct(size_t number)
{
	return number > 0 ? Fuct(number-1) * number : 1;
}

int main()
{
	//std::list<int> list = { 0,1,2 };
	//list.push_back(12);

#ifndef ONE 
	
	auto std_map = std::map<int, int>();
	
	for (int i = 0; i < 10; ++i) {
		std::cout << "map size = " << std_map.size() << std::endl;
		
		std_map.insert(std::make_pair(i, i*i));
		std::cout << std::endl;
	}



	cout << "--------------------------------------" << endl;

	auto my_map = a_map<int, int>();
	my_map.get_allocator().reserve(10);

	for (int i = 0; i < 5; ++i) {
		std::cout << "map size = " << my_map.size() << std::endl;

		my_map.insert(std::make_pair( i, Fuct(i) ));
		std::cout << std::endl;
	}

	for (auto i : my_map) {
		std::cout << i.first << " " << i.second << std::endl;
	}

#endif // !ONE

	cout << "--------------------------------------" << endl;

	my_list<int> myList;

	for (int i = 0; i < 5; ++i) {
		
		myList.insert(i);
		std::cout << std::endl;
	}

	size_t i = 0;
	while (i < myList.size()) {
		std::cout << myList.getValue(i) << std::endl;
		++i;
	}
	
	return 0;
}