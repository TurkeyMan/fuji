#if !defined(_CONSTELFILE_H_)
#define _CONSTELFILE_H_

#include "Global.h"
#include "ChunkedFile.h"

class PntsChunk : Chunk {
public:
	PntsChunk(Chunk *_parent, Chunk *_rootChunk);
	~PntsChunk();
	uint32 GetLength();
	void Encode(uint8 *data);
	void Decode(const uint8 *data, const uint32 maxLength);

protected:
	Chunk *ChildFactory(ChunkID &_id, const uint32 maxLength);
};

class ConstelRootChunk : public Chunk {
public:
	ConstelRootChunk(Chunk *_parent, Chunk *_rootChunk);
	~ConstelRootChunk();
	uint32 GetLength();
	void Encode(uint8 *data);
	void Decode(const uint8 *data, const uint32 maxLength);
	
protected:	
	Chunk *ChildFactory(ChunkID &_id, const uint32 maxLength);
};

class ConstelFile : public ChunkedFile<ConstelRootChunk> {
public:	
	ConstelFile();
	~ConstelFile() {};
};

#endif