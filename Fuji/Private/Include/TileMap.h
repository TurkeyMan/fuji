#if defined(_TILEMAP_H)
#define _TILEMAP_H

class TileMap
{
public:
	void Create(char *pFilename, int xTiles, int yTiles);
	void Release();

	void SetOffset(Vector3 offset);
	void Draw();

protected:
	Texture *pTexture;
	uint16 xTile, yTiles;
	uint16 *pTiles;
};

#endif
