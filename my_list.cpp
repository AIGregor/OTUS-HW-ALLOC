#pragma once
/*
–еализовать свой контейнер, который по аналогии с контейнерами stl
параметризуетс€ аллокатором.  онтейнер должен иметь две возможности
- добавить новый элемент и обойти контейнер в одном направлении.
—овместимость с контейнерами stl на усмотрение автора.
*/
#include <memory>

/*-----------------------------------------------------------------------------*/
/*
	¬спомогательный класс узлов списка
*/
template <typename T>
class Node
{
public:
	Node();
	~Node();
	Node<T>* getNextNode() { return ptrNextNode; };
	void setNextNode(Node<T>* pNextNode) { ptrNextNode = pNextNode; };
	Node<T>* getPrevNode() { return ptrPrevNode; };
	void setPrevNode(Node<T>* pPrevNode) { ptrPrevNode = pPrevNode; };

	T& getValue() { return value; };
	void setValue(T new_value) { value = new_value; };
private:
	Node<T>* ptrNextNode = nullptr;
	Node<T>* ptrPrevNode = nullptr;
	T value;
};

template<typename T>
Node<T>::Node() : ptrNextNode(nullptr), ptrPrevNode(nullptr)
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

	T getValue(size_t index);
	bool insert(const T value);
	bool remove(size_t index);

	Alloc& getAlloc() { return my_list_allocator; };
	size_t size() { return mSize; };

private:
	my_nodeptr head;
	my_nodeptr tail;

	Alloc my_list_allocator;
	
	size_t mSize;

	my_nodeptr createNode()
	{
		my_nodeptr newNode = getAlloc().allocate(1);
		my_nodeptr nextNode = std::move(newNode->getNextNode());
		my_nodeptr prevNode = std::move(newNode->getPrevNode());

		getAlloc().construct(&newNode, newNode);
		getAlloc().construct(&nextNode, newNode);
		getAlloc().construct(&prevNode, newNode);
	
		newNode->setNextNode(nextNode);
		newNode->setPrevNode(prevNode);

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
	setHead( createNode() );
}

template<typename T, typename Alloc>
my_list<T, Alloc>::~my_list()
{
	// TODO: –асписать удаление элементов списка и освобождение пам€ти.
	clearAll();
}

template<typename T, typename Alloc>
T my_list<T, Alloc>::getValue(size_t index)
{
	if (index > mSize || index < 0)
		return -1;

	my_node* ptrNext = head;
	size_t szIndex = 0;
	while (ptrNext->getNextNode() != nullptr && szIndex <= index)
	{
		ptrNext = static_cast<my_node*>(ptrNext->getNextNode());
		++szIndex;
	}

	return ptrNext->getValue();
}

template<typename T, typename Alloc>
bool my_list<T, Alloc>::insert(const T value)
{
	my_node* ptrNext = head;
	while (ptrNext->getNextNode() != ptrNext && ptrNext->getNextNode() != nullptr)
	{
		ptrNext = static_cast<my_node*>(ptrNext->getNextNode());
	}
	my_nodeptr ptrNewNode = createNode();
	ptrNext->setNextNode( ptrNewNode );

	ptrNewNode->setPrevNode( ptrNext );
	ptrNewNode->setNextNode( nullptr );

	ptrNewNode->setValue( value );

	setTail(ptrNewNode);
	incSize();
	
	return true;
}

template<typename T, typename Alloc>
bool my_list<T, Alloc>::remove(size_t index)
{
	if (index > mSize || index < 0)
		return false;

	my_nodeptr ptrNext = head;
	size_t szIndex = 0;

	while (ptrNext->getNextNode() != nullptr || szIndex != index)
	{
		ptrNext = static_cast<my_nodeptr>(ptrNext->getNextNode());
		++szIndex;
	}
	my_nodeptr ptrPrevNode = static_cast<my_nodeptr>(ptrNext->getPrevNode());
	ptrPrevNode->setNextNode( ptrNext->getNextNode() );

	my_nodeptr ptrNextNode = static_cast<my_nodeptr>(ptrNext->getNextNode());
	ptrNextNode->setPrevNode( ptrNext->getPrevNode() );

	decSize();

	return true;
}

template<typename T, typename Alloc>
void my_list<T, Alloc>::clearAll()
{
	mSize = 0;
	my_nodeptr ptrPrev = tail;

	while (ptrPrev != ptrPrev->getPrevNode())
	{		 
		my_nodeptr ptrPrevNode = static_cast<my_nodeptr>(ptrPrev->getPrevNode());
		freeNode(ptrPrev);
		ptrPrev = ptrPrevNode;
	}

	freeNode(head);
}

template<typename T, typename Alloc>
void my_list<T, Alloc>::freeNode(my_nodeptr node)
{
	if (node == nullptr)
		return;

	getAlloc().destroy(node->getNextNode());
	getAlloc().destroy(node->getPrevNode());
	getAlloc().destroy(node);

	getAlloc().deallocate(node, 1);
}