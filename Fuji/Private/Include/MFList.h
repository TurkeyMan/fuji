#if !defined(_MFLIST_H)
#define _MFLIST_H

#if defined(_MSC_VER)
#pragma warning(disable: 4345)
#endif

template<class T>
class MFList
{
public:
	MFList();
	~MFList();

	void push(T *pItem);

	void insert(T *pItem, T *pAfter = NULL);

	void remove(T *pItem);

	T *head();
	T *next(T *pItem);

protected:
	T *pHead;
};

template<class T>
class MFDoubleLinkedList
{
public:
	MFDoubleLinkedList();
	~MFDoubleLinkedList();

	void pushfront(T *pItem);
	void pushback(T *pItem);

	void insert(T *pItem, T *pAfter = NULL);
	void insertbefore(T *pItem, T *pBefore);

	void remove(T *pItem);

	T *head();
	T *tail();

	T *next(T *pItem);
	T *prev(T *pItem);

protected:
	T *pHead, *pTail;
};

#include "MFList.inl"

#endif
