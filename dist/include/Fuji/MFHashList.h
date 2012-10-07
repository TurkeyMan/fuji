/**
 * @file MFHashList.h
 * @brief A fast template class to manage a hashed lists of pointers.
 * @author Manu Evans
 * @defgroup MFHashList Hash List
 * @{
 */

#if !defined(_MFHASHLIST_H)
#define _MFHASHLIST_H

/**
 * Hash List.
 * Maintains a hashed list of pointers to objects.
 */
template<class T>
class MFHashList
{
public:
	/**
	 * Operator overload for the offset operator to index the hash list by string.
	 * Operator overload for the offset operator to index the hash list by string.
	 * @param pHashString String to be hashed and look up in the table.
	 * @return Pointer to the item. NULL if the item is not found.
	 */
	const T* operator [](const char *pHashString) const;

	/**
	 * Const operator overload for the offset operator to index the hash list by string.
	 * Const operator overload for the offset operator to index the hash list by string.
	 * @param pHashString String to be hashed and look up in the table.
	 * @return Const pointer to the item. NULL if the item is not found.
	 */
	T* operator [](const char *pHashString);

	/**
	 * Initialise a hash list.
	 * Allocates memory for the hash list.
	 * @param pGroupName String representing a name for the list of pointers.
	 * @param maxElements Maximum elements the list can store.
	 * @return None.
	 */
	void Init(const char *pGroupName, uint32 maxElements);

	/**
	 * Deinitialise a hash list.
	 * Destroys and releases memory reserved by the hash list.
	 * @return None.
	 */
	void Deinit();

	/**
	 * Add an item to the list.
	 * Adds an item to the list.
	 * @param pItem Pointer to an item to be added to the list.
	 * @param pHashBuffer Buffer to be hashed.
	 * @param len Length of the buffer. If len is set to -1, \a pHashBuffer is treated as a string and len is determined by the terminating NULL character.
	 * @return Returns the pointer passed by \a pItem.
	 */
	T* Create(T* pItem, const char *pHashBuffer, int len = -1);

	/**
	 * Destroy an item in the list.
	 * Destroys an item in the list. This simply removes the pointer from the list, it does not destroy the item its self.
	 * @param pHashBuffer Buffer that identifies the item.
	 * @param len Length of the buffer. If len is set to -1, \a pHashBuffer is treated as a string and len is determined by the terminating NULL character.
	 * @return None.
	 */
	void Destroy(const char *pHashBuffer, int len = -1);

	/**
	 * Destroy an item in the list.
	 * Destroys an item in the list. This simply removes the pointer from the list, it does not destroy the item its self.
	 * @param hash Hash value identifying the item to destroy.
	 * @return None.
	 */
	void Destroy(uint32 hash);

	/**
	 * Destroy an item in the list by item pointer.
	 * Destroys a specific item in the list. This simply removes the pointer from the list, it does not destroy the item its self.
	 * @param pItem Pointer to item to remove from the list.
	 * @return None.
	 * @remarks This performs a linear search for the item through the list. This is the slowest way to access an item.
	 */
	void DestroyItem(T* pItem);

	/**
	 * Get an item in the list.
	 * Gets an item in the list by hash value.
	 * @param pHashBuffer Buffer to be hashed.
	 * @param len Length of the buffer. If len is set to -1, \a pHashBuffer is treated as a string and len is determined by the terminating NULL character.
	 * @return Returns a pointer to the item, or NULL if it was not found in the list.
	 */
	T* Find(const char *pHashBuffer, int len = -1);

	/**
	 * Clear the list.
	 * Clears all items from the list.
	 * @return None.
	 */
	void Clear();

	/**
	 * Get the number of items in the list.
	 * Gets the number of items in the list.
	 * @return Returns the number of items in the list.
	 */
	int GetNumItems();

	/**
	 * Get the maximum length of the list.
	 * Gets the maximum length of the list.
	 * @return Returns the maximum number of items the list can contain.
	 */
	int GetMaxItems();

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
	struct MFHashListItem
	{
		uint32 hash;
		T *pItem;
	};

	MFHashListItem *pList;
	int listSize, numItems;

#if !defined(_RETAIL)
	const char *pName;
#endif
};

#include "MFHashList.inl"

#endif // _MFHASHLIST_H

/** @} */
