/**
 * @file MFPtrList.h
 * @brief A very fast template class to manage lists of pointers.
 * @author Manu Evans
 * @defgroup MFPtrList Pointer List
 * @{
 */

#if !defined(_MFPTRLIST_H)
#define _MFPTRLIST_H

/**
 * Pointer List.
 * Maintains a list of pointers to objects sorted in no real coherant order.
 */
template<class T>
class MFPtrList
{
public:
	/**
	 * Initialise a pointer list.
	 * Allocates memory for the managed pointer list.
	 * @param pGroupName String representing a name for the list of pointers.
	 * @param maxElements Maximum elements the list can store.
	 * @return None.
	 */
	void Init(char* pGroupName, int maxElements);

	/**
	 * Deinitialise a pointer list.
	 * Destroys and releases memory for the pointer list.
	 * @return None.
	 */
	void Deinit();

	/**
	 * Add an item to the list.
	 * Adds an item to the list.
	 * @param p Pointer to an item to be added to the list.
	 * @return Returns the pointer passed by \a p.
	 */
	T* Create(T* p);

	/**
	 * Destroy an item in the list.
	 * Destroys an item in the list. This simply removes the pointer from the list, it does not destroy the item its self.
	 * @param p Pointer to the item in the list to be removed.
	 * @return None.
	 */
	void Destroy(T* p);

	/**
	 * Destroy an item in the list.
	 * Destroys an item in the list. This simply removes the pointer from the list, it does not destroy the item its self.
	 * @param p Iterator pointing to an item in the list to be removed.
	 * @return None.
	 */
	void Destroy(T** p);

	/**
	 * Get an iterator pointing to the start of the list.
	 * Returns an iterator pointing to the start of the list.
	 * @return Returns a pointer pointing to the first pointer in the list.
	 */
	T** Begin();

	/**
	 * Find an item in the list.
	 * Returns an iterator pointing to the specified item in the list.
	 * @param p Pointer to an item to find.
	 * @return Returns an iterator pointing to the item that was found.
	 */
	T** Find(T* p);

	/**
	 * Clear the list.
	 * Clears all items from the list.
	 * @return None.
	 */
	void Clear();

	/**
	 * Get the length of the list.
	 * Gets the length of the list.
	 * @return Returns the number of items in the list.
	 */
	int GetLength();

	/**
	 * Get the maximum length of the list.
	 * Gets the maximum length of the list.
	 * @return Returns the maximum number of items the list can contain.
	 */
	int GetMaxElements();

	/**
	 * Check if the list is full.
	 * Checks if the list is full.
	 * @return Returns true if the list is full.
	 */
	bool IsFull();

	/**
	 * Check if the list is empty.
	 * Checks if the list is empty.
	 * @return Returns true if the list is empty.
	 */
	bool IsEmpty();

protected:
	T** ppMark;

#if !defined(_RETAIL)
	char *pName;
#endif
};


/**
 * Dead/Live Pointer List.
 * Allocates and maintains a list of pointers to objects in no real coherant order.
 */
template<class T>
class MFPtrListDL
{
public:
	/**
	 * Initialise a pointer list.
	 * Allocates memory for the managed pointer list and the maximum number of item instances.
	 * @param pGroupName String representing a name for the list of pointers.
	 * @param maxElements Maximum number of items the list can store.
	 * @param elementSize Size of a single item in the list.
	 * @param pMem Pointer to an already allocated buffer. The buffer must be a minimum of (maxElements+2)*sizeof(T*) + maxElements*elementSize bytes.
	 * @return None.
	 */
	void Init(char* pGroupName, int maxElements, int elementSize = sizeof(T), void *pMem = NULL);

	/**
	 * Deinitialise a pointer list.
	 * Destroys and releases memory for the pointer list and all items in the list.
	 * @return None.
	 * @remarks Destroying the list destroys every item in the list. Any external pointers to items in the list will remain dangling.
	 */
	void Deinit();

	/**
	 * Create a new item.
	 * Creates a new item in the list.
	 * @return Returns a pointer to a newly created item of type T.
	 */
	T* Create();

	/**
	 * Destroy an item in the list.
	 * Destroys an item in the list.
	 * @param p Pointer to the item to be destroyed.
	 * @return None.
	 */
	void Destroy(T* p);

	/**
	 * Destroy an item in the list.
	 * Destroys an item in the list.
	 * @param p Iterator pointing to an item in the list to be removed.
	 * @return None.
	 */
	void Destroy(T** p);

	/**
	 * Get an iterator pointing to the start of the list.
	 * Returns an iterator pointing to the start of the list.
	 * @return Returns a pointer pointing to the first pointer in the list.
	 */
	T** Begin();

	/**
	 * Find an item in the list.
	 * Returns an iterator pointing to the specified item in the list.
	 * @param p Pointer to an item to find.
	 * @return Returns an iterator pointing to the item that was found.
	 */
	T** Find(T* p);

	/**
	 * Swap 2 items placement in the list.
	 * Swaps 2 items placement in the list.
	 * @param p1 Iterator pointing to the first item pointer.
	 * @param p2 Iterator pointing to the second item pointer to exchange position with the first.
	 * @return None.
	 */
	void Swap(T **p1, T **p2);

	/**
	 * Clear the list.
	 * Clears all items from the list.
	 * @return None.
	 */
	void Clear();

	/**
	 * Get the length of the list.
	 * Gets the length of the list.
	 * @return Returns the number of items in the list.
	 */
	int GetLength();

	/**
	 * Get the maximum length of the list.
	 * Gets the maximum length of the list.
	 * @return Returns the maximum number of items the list can contain.
	 */
	int GetMaxElements();

	/**
	 * Check if the list is full.
	 * Checks if the list is full.
	 * @return Returns true if the list is full.
	 */
	bool IsFull();

	/**
	 * Check if the list is empty.
	 * Checks if the list is empty.
	 * @return Returns true if the list is empty.
	 */
	bool IsEmpty();

	/**
	 * Get the start of the dead item list.
	 * Gets the start of the dead item list.
	 * @return Returns an iterator pointing to the first item in the dead item list.
	 * @remarks The dead item list is where all the non-active items reside.
	 */
	T** BeginDead();

protected:
	T** ppMark;

#if !defined(_RETAIL)
	char *pName;
#endif
};

#include "MFPtrList.inl"

#endif // _MFPTRLIST_H

/** @} */
