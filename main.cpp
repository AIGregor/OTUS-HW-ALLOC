// alloc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <map>
#include <vector>
#include <iostream>

#include "my_list.cpp"

#define ONE
#define VEC

using namespace std;

template <class K, class V> 
using a_map = std::map< K, V, std::less<K>, my_list_alloc<std::pair<const K, V>> >;

constexpr size_t Fuct(size_t number)
{
	return number > 0 ? Fuct(number-1) * number : 1;
}

int main()
{

#ifndef VEC

	auto v = std::vector< int, my_list_alloc<int> >();
	v.reserve(10);

	for (int i = 0; i < 5; ++i) {
		std::cout << "vector size = " << v.size() << std::endl;
		v.emplace_back(i);
		std::cout << std::endl;
	}

	for (auto i : v) {
		std::cout << i << std::endl;
	}

#endif // !VEC

#ifndef ONE 
	
	auto std_m = std::map<int, int>();
	
	for (int i = 0; i < 5; ++i) {
		std::cout << "map size = " << std_m.size() << std::endl;
		
		std_m.insert(std::make_pair(i, i*i));
		std::cout << std::endl;
	}

	for (auto i : std_m) {
		std::cout << i.first << "-" << i.second << std::endl;
	}

#endif // !BOTH

	cout << "--------------------------------------" << endl;

	auto m = a_map<int, int>();

	m.get_allocator().reserve(1);

	for (int i = 0; i < 1; ++i) {
		std::cout << "map size = " << m.size() << std::endl;

		m.insert(std::make_pair( i, Fuct(i) ));
		std::cout << std::endl;
	}

	for (auto i : m) {
		std::cout << i.first << "-" << i.second << std::endl;
	}

   return 0;
}

