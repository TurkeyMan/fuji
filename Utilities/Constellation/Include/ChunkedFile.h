#if !defined(_CHUNKEDFILE_H)
#define _CHUNKEDFILE_H

#include "Global.h"
#include <list>

class ChunkID {
public:	
	ChunkID();
	ChunkID(const uint8 *_id);
	bool operator==(const ChunkID &other) const;
	bool operator==(const uint8 *other) const;
	void Encode(uint8 *_id);
	void Decode(const uint8 *_id);
	
	const static uint32 length;

private:
	uint32 id;
};

class Chunk {
public:
	Chunk(Chunk *_parent, Chunk *_rootChunk, const ChunkID &_id);
	virtual ~Chunk();
	virtual uint32 GetLength();
	virtual void Clear();
	virtual void Encode(uint8 *data) = 0;
	virtual void Decode(const uint8 *data, const uint32 maxLength) = 0;
	
	bool FindSubChunk(ChunkID &_id, std::list<Chunk *>::iterator *result, const std::list<Chunk *>::iterator *startingPos = NULL, const std::list<Chunk *>::iterator *endingPos = NULL);

	ChunkID id;
	Chunk *parent, *rootChunk;
	std::list<Chunk *> subChunks;

protected:
	virtual Chunk *ChildFactory(ChunkID &_id, const uint32 maxLength) = 0;
	
	void ChildrenHandler(const uint8 *data, const uint32 maxLength);
};

// *** Exceptions ***
class ChunkedFileException {
public:
	const char *message;
	
	ChunkedFileException()
	{
		message = NULL;
	}

	ChunkedFileException(const char *_message)
	{
		message = _message;
	}
};

class CFE_FileError : public ChunkedFileException {
public:
	CFE_FileError() {}
	CFE_FileError(const char *_message) : ChunkedFileException(_message) {}
};

class CFE_Open : public CFE_FileError {
public:	
	CFE_Open() {}
	CFE_Open(const char *_message) : CFE_FileError(_message) {}
};

class CFE_DataError : public ChunkedFileException {
public:
	CFE_DataError() {}
	CFE_DataError(const char *_message) : ChunkedFileException(_message) {}
};

class CFE_OutOfData : public CFE_DataError {
public:	
	CFE_OutOfData() {}
	CFE_OutOfData(const char *_message) : CFE_DataError(_message) {}
};

class CFE_Read : public CFE_FileError {
public:
	CFE_Read() {}
	CFE_Read(const char *_message) : CFE_FileError(_message) {}
};

class CFE_DecodingError : public CFE_DataError {
public:
	CFE_DecodingError() {}
	CFE_DecodingError(const char *_message) : CFE_DataError(_message) {}
};

// Only relevant for saving
class CFE_Write : public ChunkedFileException {
public:
	CFE_Write() {}
	CFE_Write(const char *_message) : ChunkedFileException(_message) {}
};

class CFE_EncodingError : public CFE_DataError {
public:
	CFE_EncodingError() {}
	CFE_EncodingError(const char *_message) : CFE_DataError(_message) {}
};

// *** ChunkedFile ***
template <class RootType> class ChunkedFile {
public:	
	ChunkedFile();
	virtual ~ChunkedFile();
	void Load(const char *filename);
	void Save(const char *filename);

	RootType rootChunk;

protected:
	void LoadData(const char *filename, uint32 *length, uint8 **data);
	void SaveData(const char *filename, const uint32 length, uint8 *data);
	void FreeData(uint8 *data);
};

// *** Globals ***
extern const ChunkID rootChunkID;

template <class RootType> ChunkedFile<RootType>::ChunkedFile() : rootChunk(NULL, NULL)
{
}

template <class RootType> ChunkedFile<RootType>::~ChunkedFile()
{
}

template <class RootType> void ChunkedFile<RootType>::Load(const char *filename)
{
	uint8 *data = NULL;
	uint32 length;

	// If this screws up, it'll free the data first, then throw an exception.
	// If not, it's up to me to free it.
	LoadData(filename, &length, &data);
	
	try {
		/* Check the length's big enough for the root chunk, and that the ID in the file
		 * header matches.
		 */
		if(length < ChunkID::GetLength() + 4) {
			throw CFE_OutOfData("Data too short while decoding main file");
		}

		// Parsing the chunks goes here
	}
	catch(ChunkedFileException) {
		FreeData(data);
		throw;
	}
}

template <class RootType> void ChunkedFile<RootType>::Save(const char *filename)
{
	uint32 length;
	uint8 *data;

	length = rootChunk.GetLength();
	data = new uint8[length];
	try { // Free the data if an exception is thrown
		rootChunk.Encode(data);
		SaveData(filename, length, data);
	}
	catch(ChunkedFileException) {
		delete[] data;
		throw;
	}
	
	delete[] data;
}

template <class RootType> void ChunkedFile<RootType>::LoadData(const char *filename, uint32 *length, uint8 **data)
{
	HANDLE fileHandle;
	uint8 *tempData;

	fileHandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(fileHandle == INVALID_HANDLE_VALUE) {
		throw CFE_Open("Unable to open file for reading");
	}

	LARGE_INTEGER fileSize64;
	uint32 fileSize32;

	if(!GetFileSizeEx(fileHandle, &fileSize64)) {
		CloseHandle(fileHandle);		
		throw CFE_Open("Unable to determine file size");
	}

	if(fileSize64 > 0xFFFFFFFF || fileSize64 < 0) {
		CloseHandle(fileHandle);
		throw CFE_Open("File too big");
	}

	fileSize = (uint32)fileSize64;

	tempData = new uint8[fileSize];	

	uint32 amountRead;
	if(!ReadFile(fileHandle, (LPVOID)tempData, fileSize, &amountRead, NULL)) {
		CloseHandle(fileHandle);
		delete[] tempData;
		throw CFE_Read("Error while reading file");
	}

	if(amountRead < fileSize) {
		CloseHandle(fileHandle);
		delete[] tempData;
		throw CFE_Read("Data too short while reading file");
	}

	CloseHandle(fileHandle);
	*data = tempData;
	*length = fileSize;
}

template <class RootType> void ChunkedFile<RootType>::SaveData(const char *filename, const uint32 length, uint8 *data)
{
	HANDLE fileHandle;

	fileHandle = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if(fileHandle == INVALID_HANDLE_VALUE) {
		throw CFE_Open("Unable to open file for writing");
	}

	uint32 amountWritten;
	if(!WriteFile(fileHandle, (LPVOID)data, length, &amountWritten, NULL)) {
		CloseHandle(fileHandle);
		DeleteFile(filename);
		throw CFE_Write("Error while writing file");
	}

	if(amountWritten < length) {
		CloseHandle(fileHandle);
		DeleteFile(filename);
		throw CFE_Write("Unable to write all data");
	}

	CloseHandle(fileHandle);
}

// Frees the data allocated with LoadData
template <class RootType> void ChunkedFile<RootType>::FreeData(uint8 *data)
{
	delete[] data;
}

#endif