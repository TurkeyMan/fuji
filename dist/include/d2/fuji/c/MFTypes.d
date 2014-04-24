module fuji.c.MFTypes;

nothrow:

// alias fuji types
alias byte int8;
alias ubyte uint8;
alias short int16;
alias ushort uint16;
alias int int32;
alias uint uint32;
alias long int64;
alias ulong uint64;

/**
* Represents a spatial rectangle.
*/
struct MFRect
{
	float x = 0; /**< X coordinate of rectangle */
	float y = 0; /**< Y coordinate of rectangle */
	float width = 0; /**< Width of rectangle */
	float height = 0; /**< Height of rectangle */

	@property float top() const pure nothrow { return x; }
	@property float left() const pure nothrow { return y; }
	@property float right() const pure nothrow { return x + width; }
	@property float bottom() const pure nothrow { return y + height; }
}

bool MFTypes_PointInRect(float x, float y, const ref MFRect rect) pure
{
	return x >= rect.x && x < rect.x + rect.width &&
		y >= rect.y && y < rect.y + rect.height;
}

bool MFTypes_RectOverlap(const ref MFRect rect1, const ref MFRect rect2) pure
{
	return rect1.x < rect2.right && rect1.right > rect2.x && rect1.y < rect2.bottom && rect1.bottom > rect2.y;
}

