#include "Common.h"
#include "Texture.h"
#include "FileSystem.h"

struct TgaHeader {
	uint8 idLength;
	uint8 colourMapType;
	uint8 imageType;
	
	uint8 colourMapStart;
	uint16 colourMapLength;
	uint8 colourMapBits;

	uint16 xStart;
	uint16 yStart;
	uint16 width;
	uint16 height;
	uint8 bpp;
	uint8 flags;
};

Texture* Texture::LoadTexture(const char *filename, bool generateMipChain)
{
	Texture *pTexture = FindTexture(filename);

	if(!pTexture) {
		pTexture = gTextureBank.Create((Texture*)Heap_Alloc(sizeof(Texture)));
		pTexture->refCount = 0;
	}
	
	if(!pTexture->refCount) {
		int32 handle;
		TgaHeader header;
		void *imageData;	

		if((handle = File_Open(filename, OF_Read)) == -1) {
			LOGD(STR("Failed loading texture: %s", filename));
			return(NULL);
		}

		if(File_Read(&header, sizeof(header), handle) < sizeof(header)) {
			LOGD(STR("Failed loading texture: %s", filename));
			return(NULL);
		}

		if(header.imageType != 2) { // Can't handle RLE images
			LOGD(STR("Failed loading texture: %s (Unhandled TGA type (%d))", filename, header.imageType));
			File_Close(handle);
			return(NULL);
		}

		if((header.bpp != 24) && (header.bpp != 32)) {
			LOGD(STR("Failed loading texture: %s (Invalid image depth (%d))", filename, header.bpp));
			File_Close(handle);
		}

		// Not interested in the image id, it's just a comment
		File_Seek(Seek_Current, header.idLength, handle);

		// Not interested in the colour map (oddly, the file format spec seems to indicate that an image can be truecolour, and still have a colourmapi, do this seek just in case. Obviously, nothing happens if colourMapType is 0)
		File_Seek(Seek_Current, header.colourMapLength * header.colourMapBits * header.colourMapType, handle);

		uint32 dataLength;
		
		if(header.bpp == 32) {
			dataLength = header.width * header.height * 4;
		} else {
			dataLength = header.width * header.height * 3;
		}
	
		imageData = Heap_Alloc(dataLength);
//		imageData = new unsigned char[dataLength];

		File_Read(imageData, dataLength, handle);
		File_Close(handle);

		GLint internalFormat;
		GLenum format;

		if(header.bpp == 32) {
			internalFormat = GL_RGBA;
			format = GL_RGBA8;
		
			// OpenGL can only handle RGBA, TGA provides ARGB
			for(int32 i=0; i < (header.width * header.height); i++) {
				unsigned char temp;
			
				temp = ((unsigned char *)imageData)[(i * 4) + 3];
				((unsigned char *)imageData)[(i * 4) + 3] = ((unsigned char *)imageData)[(i * 4)];
				((unsigned char *)imageData)[(i * 4)] = temp;
			}
		} else { // Must be 24
			internalFormat = GL_RGB;
			format = GL_RGBA8;
		}

		glGenTextures(1, &(pTexture->textureID));
		glBindTexture(GL_TEXTURE_2D, pTexture->textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		if(generateMipChain) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, header.width, header.height, format, GL_UNSIGNED_BYTE, imageData);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header.width, header.height, 0, format, GL_UNSIGNED_BYTE, imageData);
		}

//		delete imageData;
		Heap_Free(imageData);

//		if(rv != 0) {
//			LOGD(STR("Failed loading texture: %s (Unable to create OpenGL texture object)", filename));
//			return(NULL);
//		}
	
		strcpy(pTexture->name, filename);
		pTexture->width = header.width;
		pTexture->height = header.height;
	}

	pTexture->refCount++;

	return(pTexture);
}

void Texture::Release()
{
	refCount--;
	
	if(!refCount) {
		glDeleteTextures(1, &textureID);
	}
}

void Texture::SetTexture(int texUnit)
{
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(0);
}

void Texture::UseNone(int texUnit)
{
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(0);
}
