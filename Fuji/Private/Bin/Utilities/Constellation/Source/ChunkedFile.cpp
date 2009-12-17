#include "Global.h"
#include "ChunkedFile.h"
#include <typeinfo.h>

// Globals
const ChunkID rootChunkID((uint8 *)"ROOT");
const uint32 ChunkID::length = 4;

ChunkID::ChunkID()
{
}

ChunkID::ChunkID(const uint8 *_id)
{
	Decode(_id);
}

bool ChunkID::operator==(const ChunkID &other) const
{
	return(id == other.id);
}

bool ChunkID::operator==(const uint8 *other) const
{
	return(*((uint32 *)other) == id);
}

void ChunkID::Encode(uint8 *_id)
{
	*((uint32 *)_id) = id;
}

void ChunkID::Decode(const uint8 *_id)
{
	id = *((uint32 *)_id);
}

Chunk::Chunk(Chunk *_parent, Chunk *_rootChunk, const ChunkID &_id)
{
	id = _id;
	parent = _parent;
	rootChunk = _rootChunk;
}

Chunk::~Chunk()
{
	Clear();
}

uint32 Chunk::GetLength()
{
	uint32 length = 0;

	for(std::list<Chunk *>::iterator i = subChunks.begin(); i != subChunks.end(); ++i)
	{
		length += (*i)->GetLength();
	}

	return(length + ChunkID::length + 4);
}

void Chunk::Clear()
{
	for(std::list<Chunk *>::iterator i = subChunks.begin(); i != subChunks.end(); ++i)
	{
		delete *i;
	}

	subChunks.clear();
}

bool Chunk::FindSubChunk(ChunkID &id, std::list<Chunk *>::iterator *result, const std::list<Chunk *>::iterator *startingPos, const std::list<Chunk *>::iterator *endingPos)
{
	std::list<Chunk *>::iterator newStartingPos, newEndingPos;

	if(startingPos != NULL)
	{
		newStartingPos = *startingPos;
	}
	else
	{
		newStartingPos = subChunks.begin();
	}

	if(endingPos != NULL)
	{
		newEndingPos = *endingPos;
		++newEndingPos;
	}
	else
	{
		newEndingPos = subChunks.end();
	}

	for(std::list<Chunk *>::iterator i = 0; i != newEndingPos; ++i)
	{
		if((*i)->id == id)
		{
			*result = i;
			return(true);
		}
	}

	return(false);
}

void Chunk::ChildrenHandler(const uint8 *data, const uint32 maxLength)
{
	uint32 currentPos = 0;
	uint32 maxChunkSize;
	Chunk *newChunk;

	while(currentPos < maxLength)
	{
		// Minimum chunk size (ID + Length)
		if(currentPos < (ChunkID::length + 4))
		{
			throw CFE_OutOfData("Ran out of data while searching for subChunks");
		}

		uint32 chunkLength;
		chunkLength = *(uint32 *)(data + currentPos + ChunkID::length + 4);

		maxChunkSize = maxLength - currentPos;
		newChunk = ChildFactory(ChunkID(data), maxLength - currentPos);
		if(newChunk != NULL)
		{
			newChunk->Decode(data, maxLength - currentPos);
			subChunks.push_back(newChunk);
		}

		currentPos += chunkLength;
	}
}