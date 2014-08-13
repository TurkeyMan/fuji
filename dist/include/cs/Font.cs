using System;
using System.Runtime.InteropServices;

using Fuji;

namespace Fuji
{
	using MFFont = UIntPtr;

	public enum MFFontJustify
	{
		TopLeft,		/**< Justify top left. */
		TopCenter,		/**< Justify top center. */
		TopRight,		/**< Justify top right. */
		TopFull,		/**< Justify top full. */

		CenterLeft,		/**< Justify center left. */
		Center,			/**< Justify center. */
		CenterRight,	/**< Justify center right. */
		CenterFull,		/**< Justify center full. */

		BottomLeft,		/**< Justify bottom left. */
		BottomCenter,	/**< Justify bottom center. */
		BottomRight,	/**< Justify bottom right. */
		BottomFull,		/**< Justify bottom full. */

		Max				/**< Max justifications. */
	}
	
	public struct Font
	{
		Resource resource;

		private Font(MFFont handle)
		{
			resource.handle = handle;
		}

		public static Font Create(string name)
		{
			return new Font(MFFont_Create(name));
		}

		public static Font DebugFont
		{
			get { return new Font(MFFont_GetDebugFont()); }
		}

		public int Release()
		{
			return resource.Release();
		}

		public MFFont Handle
		{
			get { return resource.handle; }
		}

		public float Height
		{
			get { return MFFont_GetFontHeight(resource.handle); }
		}

		public float CharacterWidth(char character)
		{
			return MFFont_GetCharacterWidth(resource.handle, (int)character);
		}

		public float StringWidth(string text, float height, float lineWidth, out float totalHeight, int maxLen = -1)
		{
			return MFFont_GetStringWidth(resource.handle, text, height, lineWidth, maxLen, out totalHeight);
		}

		public MFVector CharacterPos(string text, int charIndex, float height)
		{
			return MFFont_GetCharPos(resource.handle, text, charIndex, height);
		}

		public int BlitText(string text, int x, int y, MFVector colour, int maxChars = -1)
		{
			return MFFont_BlitText(resource.handle, x, y, ref colour, text, maxChars);
		}

		public float DrawText(string text, ref MFVector pos, float height, ref MFVector colour, int maxChars, MFMatrix ltw)
		{
			return MFFont_DrawText(resource.handle, ref pos, height, ref colour, text, maxChars, ref ltw);
		}

		public float DrawText2(string text, float x, float y, float height, ref MFVector colour, int maxChars, MFMatrix ltw)
		{
			return MFFont_DrawText2(resource.handle, x, y, height, ref colour, text, maxChars, ref ltw);
		}

		public float DrawTextJustified(string text, MFVector pos, float boxWidth, float boxHeight, MFFontJustify justification, float textHeight, MFVector colour, int numChars, MFMatrix ltw)
		{
			return MFFont_DrawTextJustified(resource.handle, text, ref pos, boxWidth, boxHeight, justification, textHeight, ref colour, numChars, ref ltw);
		}

		public float DrawTextAnchored(string text, MFVector pos, MFFontJustify justification, float lineWidth, float textHeight, MFVector colour, int numChars, MFMatrix ltw)
		{
			return MFFont_DrawTextAnchored(resource.handle, text, ref pos, justification, lineWidth, textHeight, ref colour, numChars, ref ltw);
		}

		// native calls
		[DllImport (Engine.DllVersion)]
		private static extern MFFont MFFont_Create([MarshalAs(UnmanagedType.LPStr)] string name);

		[DllImport (Engine.DllVersion)]
		private static extern int MFFont_Release(MFFont font);

		[DllImport (Engine.DllVersion)]
		private static extern float MFFont_GetFontHeight(MFFont font);

		[DllImport (Engine.DllVersion)]
		private static extern float MFFont_GetCharacterWidth(MFFont font, int character);

		[DllImport (Engine.DllVersion)]
		private static extern MFVector MFFont_GetCharPos(MFFont font, [MarshalAs(UnmanagedType.LPStr)] string text, int charIndex, float height);

		[DllImport (Engine.DllVersion)]
		private static extern float MFFont_GetStringWidth(MFFont font, [MarshalAs(UnmanagedType.LPStr)] string text, float height, float lineWidth, int maxLen, out float totalHeight);

		[DllImport (Engine.DllVersion)]
		private static extern int MFFont_BlitText(MFFont font, int x, int y, ref MFVector colour, [MarshalAs(UnmanagedType.LPStr)] string text, int maxChars);

		[DllImport (Engine.DllVersion)]
		private static extern float MFFont_DrawText(MFFont font, ref MFVector pos, float height, ref MFVector colour, [MarshalAs(UnmanagedType.LPStr)] string text, int maxChars, ref MFMatrix ltw);

		[DllImport (Engine.DllVersion)]
		private static extern float MFFont_DrawText2(MFFont font, float x, float y, float height, ref MFVector colour, [MarshalAs(UnmanagedType.LPStr)] string text, int maxChars, ref MFMatrix ltw);

		[DllImport (Engine.DllVersion)]
		private static extern float MFFont_DrawTextJustified(MFFont font, [MarshalAs(UnmanagedType.LPStr)] string text, ref MFVector pos, float boxWidth, float boxHeight, MFFontJustify justification, float textHeight, ref MFVector colour, int numChars, ref MFMatrix ltw);

		[DllImport (Engine.DllVersion)]
		private static extern float MFFont_DrawTextAnchored(MFFont font, [MarshalAs(UnmanagedType.LPStr)] string text, ref MFVector pos, MFFontJustify justification, float lineWidth, float textHeight, ref MFVector colour, int numChars, ref MFMatrix ltw);

		[DllImport (Engine.DllVersion)]
		private static extern MFFont MFFont_GetDebugFont();
	}
}
