module fuji.image;

public import fuji.c.MFImage;

public import fuji.fuji;

nothrow:
@nogc:

alias MFImage_Convert = fuji.c.MFImage.MFImage_Convert;
void MFImage_Convert(uint width, uint height, const(void[]) input, MFImageFormat inputFormat, void[] output, MFImageFormat outputFormat)
{
	// Note: Should we assert that the sizes MATCH, rather than just check that they are sufficient?
	assert((width*height*MFImage_GetBitsPerPixel(inputFormat))/8 <= input.length, "Not enough data in input buffer!");
	assert((width*height*MFImage_GetBitsPerPixel(outputFormat))/8 <= output.length, "Output buffer is too small!");

	MFImage_Convert(width, height, input.ptr, inputFormat, output.ptr, outputFormat);
}


float MFImage_sRGBToLinear(float s) pure
{
	if(s <= 0.04045f)
		return s / 12.92f;
	else
		return ((s + 0.055f) / 1.055f)^^2.4f;
}

float MFImage_LinearTosRGB(float s) pure
{
	if(s <= 0.0031308f)
		return 12.92f * s;
	else
		return 1.055f * s^^(1.0f/2.4f) - 0.055f;
}

float MFImage_GammaToLinear(float s) pure
{
	return s^^2.2f;
}

float MFImage_LinearToGamma(float s) pure
{
	return s^^(1.0f/2.2f);
}
