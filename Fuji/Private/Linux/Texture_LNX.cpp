#include "Common.h"
#include "Texture.h"
#include "FileSystem.h"
#include "Image.h"

Texture* Texture::LoadTexture(const char *filename, bool generateMipChain)
{
	Texture *pTexture = FindTexture(filename);

	if(!pTexture) {
		pTexture = gTextureBank.Create((Texture*)Heap_Alloc(sizeof(Texture)));
		pTexture->refCount = 0;
	}

	if(!pTexture->refCount) {
		GLint internalFormat;
		GLenum format;
		GLenum target;
		Image *image;

		image = LoadTGA(filename, false);

		for(uint32 i=0; i < image->width * image->height; i++) {
			unsigned char *pixel = &(((unsigned char *)image->pixels)[i * image->bytesPerPixel]);
			unsigned char temp;

			temp = pixel[0];
			pixel[0] = pixel[2];
			pixel[2] = temp;
		}

		if(image->bitsPerPixel == 32) {
			internalFormat = GL_RGBA;
			format = GL_RGBA;
		} else { // Must be 24
			internalFormat = GL_RGB;
			format = GL_RGB;
		}

		DBGASSERT(image->width == image->height, "Textures must be square!");

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &(pTexture->textureID));
		glBindTexture(GL_TEXTURE_2D, pTexture->textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		if(generateMipChain) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, image->width, image->height, format, GL_UNSIGNED_BYTE, image->pixels);
		} else {
			glTexImage2D(target, 0, internalFormat, image->width, image->height, 0, format, GL_UNSIGNED_BYTE, image->pixels);
		}

		strcpy(pTexture->name, filename);
		pTexture->width = image->width;
		pTexture->height = image->height;

		delete image;
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
//	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	glActiveTexture(0);
}

void Texture::UseNone(int texUnit)
{
//	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
//	glActiveTexture(0);
}
