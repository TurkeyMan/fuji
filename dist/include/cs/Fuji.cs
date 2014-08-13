using System;
using System.Runtime.InteropServices;

namespace Fuji
{
	using MFEngineInstance = UIntPtr;

	public enum MFPlatform
	{
		Unknown = -1,	/**< Unknown platform */

		Windows = 0,	/**< Windows */
		XBox,			/**< XBox */
		Linux,			/**< Linux */
		PSP,			/**< Playstation Portable */
		PS2,			/**< Playstation 2 */
		DC,				/**< Dreamcast */
		GC,				/**< Gamecube */
		OSX,			/**< MacOSX */
		Amiga,			/**< Amiga */
		XBox360,		/**< XBox360 */
		PS3,			/**< Playstation 3 */
		Wii,			/**< Nintendo Wii */
		Symbian,		/**< Symbian OS */
		IPhone,			/**< IPhone OS */
		Android,		/**< Android */
		WindowsMobile,	/**< Windows Mobile */
		NativeClient,	/**< Native Client (NaCL) */
		Web,			/**< Web JavaScript (Emscripten) */

		Max				/**< Max platform */
	}

	public enum MFEndian
	{
		Unknown = -1,		/**< Unknown endian */

		LittleEndian = 0,	/**< Little Endian */
		BigEndian			/**< Big Endian */
	}

	public struct MFRect
	{
		float x, y;
		float width, height;
	}

	public struct Engine
	{
		public const string DllVersion = "libFuji.so.0.7.1";

		MFEngineInstance instance;

		public static Engine Instance
		{
			get { return new Engine() { instance = Fuji_GetCurrentEngineInstance() }; }
			set { Fuji_SetCurrentEngineInstance(value.instance); }
		}

		public static Engine Create()
		{
			return new Engine() { instance = Fuji_CreateEngineInstance() };
		}

		public void Destroy()
		{
			Fuji_DestroyEngineInstance(instance);
			instance = MFEngineInstance.Zero;
		}

		public void Start(MFInitParams initParams)
		{
			System.Start(initParams);
		}

		public void Quit()
		{
			System.Quit();
		}

		// native calls
		[DllImport (DllVersion)]
		private static extern MFEngineInstance Fuji_CreateEngineInstance();

		[DllImport (DllVersion)]
		private static extern void Fuji_DestroyEngineInstance(MFEngineInstance instance);

		[DllImport (DllVersion)]
		private static extern MFEngineInstance Fuji_GetCurrentEngineInstance();

		[DllImport (DllVersion)]
		private static extern MFEngineInstance Fuji_SetCurrentEngineInstance(MFEngineInstance instance);
	}
}
