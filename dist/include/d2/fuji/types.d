module fuji.types;

/**
* Represents a spatial rectangle.
*/
struct MFRect
{
	float x = 0.0; /**< X coordinate of rectangle */
	float y = 0.0; /**< Y coordinate of rectangle */
	float width = 0.0; /**< Width of rectangle */
	float height = 0.0; /**< Height of rectangle */

	@property float top() const pure nothrow { return x; }
	@property float left() const pure nothrow { return y; }
	@property float right() const pure nothrow { return x + width; }
	@property float bottom() const pure nothrow { return y + height; }
}

bool PointInRect(float x, float y, const ref MFRect rect)
{
	return x >= rect.x && x < rect.x + rect.width &&
		y >= rect.y && y < rect.y + rect.height;
}

bool RectOverlap(const ref MFRect rect1, const ref MFRect rect2)
{
	return rect1.x < rect2.right && rect1.right > rect2.x && rect1.y < rect2.bottom && rect1.bottom > rect2.y;
}

