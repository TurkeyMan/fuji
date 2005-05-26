#include "Global.h"
#include "ConstelFile.h"
#include "StatusWindow.h"

static const ChunkID constellationChunkID((uint8 *)"CNST"); // ID for the root chunk
static const ChunkID pntsChunkID((uint8 *)"PNTS");

// *** PntsChunk ***
PntsChunk::PntsChunk(Chunk *_parent, Chunk *_rootChunk) : Chunk(_parent, _rootChunk, pntsChunkID)
{
}

PntsChunk::~PntsChunk()
{
}

uint32 PntsChunk::GetLength()
{
	return(Chunk::GetLength());
}

void PntsChunk::Encode(uint8 *data)
{
	return;
}

void PntsChunk::Decode(const uint8 *data, const uint32 maxLength)
{
	/* The child factory has already checked that there's enough space for the ID 
	 * and the length.  Now we just need to check that there's actually
	 * enough room for all of the required data.
	 */
	uint32 length;
	// The length in the chunk doesn't include the ID and the length itself, just the data
	length = *((uint32 *)(data + 4)) + 4 + ChunkID::length;

	if(length > maxLength) {
		throw CFE_OutOfData("Data too short while decoding PNTS chunk");
	}
}

Chunk * PntsChunk::ChildFactory(ChunkID &_id, const uint32 maxLength)
{
	return(NULL);
}

// *** ConstelRootChunk ***
ConstelRootChunk::ConstelRootChunk(Chunk *_parent, Chunk *_rootChunk) : Chunk(_parent, _rootChunk, constellationChunkID)
{
}

ConstelRootChunk::~ConstelRootChunk()
{
}

uint32 ConstelRootChunk::GetLength()
{
	return(0);
}

void ConstelRootChunk::Encode(uint8 *data)
{
	return;
}

void ConstelRootChunk::Decode(const uint8 *data, const uint32 maxLength)
{
	uint32 length;
	length = *((uint32 *)(data + 4)) + 4 + ChunkID::length;

	if(length > maxLength) {
		throw CFE_OutOfData("Data too short while decoding root chunk");
	}

	return;
}

Chunk * ConstelRootChunk::ChildFactory(ChunkID &_id, const uint32 maxLength)
{
	if(pntsChunkID == _id) {
		return((Chunk *)new PntsChunk(this, this));
	}
	
	return(NULL);
}

// *** ConstelFile ***
ConstelFile::ConstelFile()
{
	status->Print("Constructing F3DFile\r\n");
}