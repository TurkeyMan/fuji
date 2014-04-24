module fuji.types;

public import fuji.c.MFTypes;


bool PointInRect(float x, float y, const ref MFRect rect) pure nothrow
{
	return x >= rect.x && x < rect.x + rect.width &&
		y >= rect.y && y < rect.y + rect.height;
}

bool RectOverlap(const ref MFRect rect1, const ref MFRect rect2) pure nothrow
{
	return rect1.x < rect2.right && rect1.right > rect2.x && rect1.y < rect2.bottom && rect1.bottom > rect2.y;
}
