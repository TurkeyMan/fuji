using System;
using System.Runtime.InteropServices;

namespace Fuji
{
	public enum MFCallback
	{
		Unknown = -1,			/**< Unknown callback. */

		RegisterModules = 0,	/**< Register modules callback. This is called after the Fuji modules are registered, allowing registration of custom modules. */
		FileSystemInit,			/**< File system init callback. This is called after the filesystem initialises, before Fuji attempts to load any debug resources. */
		InitDone,				/**< Post engine init callback. Called after Fuji has completed initialisation. */
		Deinit,					/**< Deinit callback. */

		Update,					/**< Update callback. */
		Draw,					/**< Draw callback. */

		HandleSystemMessages,	/**< System Message Handler callback. */

		GainedFocus,			/**< Focus Gained callback. */
		LostFocus,				/**< Focus Lost callback. */

		DisplayLost,			/**< Display Lost callback. */
		DisplayReset,			/**< Reset Display callback. */

		DisplayResize,			/**< Display resize callback. */

		VerticalBlank,			/**< Vertical Blank callback. */

		Max						/**< Max callback. */
	}

	public enum MFMonth
	{
		January = 1,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December
	}

	public enum MFDayOfWeek
	{
		Sunday,
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday
	}

	public struct MFSystemTime
	{
		public ushort year;
		public ushort month;
		public ushort dayOfWeek;
		public ushort day;
		public ushort hour;
		public ushort minute;
		public ushort second;
		public ushort tenthMillisecond; // ie, 100 microseconds
	}

	public struct MFInitParams
	{
		public struct DisplayMode
		{
			public MFRect displayRect;		/**< Display rect. If not set, the value returned from MFDisplay_GetDefaultRes() is used. */
			public bool fullscreen;		/**< Fullscreen flag. */
		};

		public string appTitle;			/**< A title used to represent the application */

		public UIntPtr hInstance;			/**< The WIN32 hInstance paramater supplied to WinMain() */
		public UIntPtr hWnd;				/**< An optional hWnd to a WIN32 window that will contain the viewport */

		public string[] args;				/**< The argv paramater supplied to main() */

		public DisplayMode display;		/**< Display settings. */

		public bool hideSystemInfo;		/**< Hide the frame rate and system logo() */
	}

	struct System
	{
		public static MFPlatform Platform
		{
			get { return MFSystem_GetCurrentPlatform(); }
		}
		public static string PlatformID
		{
			get { return MFSystem_GetPlatformString((int)MFSystem_GetCurrentPlatform()); }
		}
		public static string PlatformName
		{
			get { return MFSystem_GetPlatformName((int)MFSystem_GetCurrentPlatform()); }
		}
		public static string SystemName
		{
			get { return MFSystem_GetSystemName(); }
		}
		public static MFEndian Endian
		{
			get { return MFSystem_GetPlatformEndian((int)MFSystem_GetCurrentPlatform()); }
		}

		public static ulong RTC
		{
			get { return MFSystem_ReadRTC(); }
		}
		public static ulong RTCFrequency
		{
			get { return MFSystem_GetRTCFrequency(); }
		}
		public double Clock
		{
			get { return (double)RTC / (double)RTCFrequency; }
		}

		public static float TimeDelta
		{
			get { return MFSystem_GetTimeDelta(); }
		}
		public static float FPS
		{
			get { return MFSystem_GetFPS(); }
		}
		public static uint FrameCounter
		{
			get { return MFSystem_GetFrameCounter(); }
		}

		public static MFSystemTime SystemTime
		{
			get
			{
				MFSystemTime t;
				MFSystem_SystemTime(out t);
				return t;
			}
		}
		public static MFSystemTime LocalTime
		{
			get { return SystemTimeToLocalTime(SystemTime); }
		}
		public static MFFileTime FileTime
		{
			get { return SystemTimeToFileTime(SystemTime); }
		}

		public static int Start(MFInitParams initParams)
		{
			return MFMain(UIntPtr.Zero);
		}

		public static void Quit()
		{
			MFSystem_Quit();
		}

		public static MFSystemTime SystemTimeToLocalTime(MFSystemTime systemTime)
		{
			MFSystemTime t;
			MFSystem_SystemTimeToLocalTime(ref systemTime, out t);
			return t;
		}

		public static MFSystemTime LocalTimeToSystemTime(MFSystemTime localTime)
		{
			MFSystemTime t;
			MFSystem_LocalTimeToSystemTime(ref localTime, out t);
			return t;
		}

		public static MFFileTime SystemTimeToFileTime(MFSystemTime systemTime)
		{
			MFFileTime t;
			MFSystem_SystemTimeToFileTime(ref systemTime, out t);
			return t;
		}

		public static MFSystemTime FileTimeToSystemTime(MFFileTime fileTime)
		{
			MFSystemTime t;
			MFSystem_FileTimeToSystemTime(ref fileTime, out t);
			return t;
		}

		[DllImport (Engine.DllVersion)]
		private static extern int MFMain(UIntPtr initPrams);
		[DllImport (Engine.DllVersion)]
		private static extern void MFSystem_Quit();
//		[DllImport (Engine.DllVersion)]
//		private static extern MFSystemCallbackFunction MFSystem_RegisterSystemCallback(MFCallback callback, MFSystemCallbackFunction pCallbackFunction);
//		[DllImport (Engine.DllVersion)]
//		private static extern MFSystemCallbackFunction MFSystem_GetSystemCallback(MFCallback callback);
//		[DllImport (Engine.DllVersion)]
//		private static extern MFDefaults* MFSystem_GetDefaults();
//		[DllImport (Engine.DllVersion)]
//		[return: MarshalAs(UnmanagedType.LPStr)]
//		private static extern string MFSystem_GetSettingString(int tabDepth);
//		[DllImport (Engine.DllVersion)]
//		private static extern void MFSystem_InitFromSettings(const MFIniLine *pSettings);
		[DllImport (Engine.DllVersion)]
		private static extern MFPlatform MFSystem_GetCurrentPlatform();
		[DllImport (Engine.DllVersion)]
		[return: MarshalAs(UnmanagedType.LPStr)]
		private static extern string MFSystem_GetPlatformString(int platform);
		[DllImport (Engine.DllVersion)]
		[return: MarshalAs(UnmanagedType.LPStr)]
		private static extern string MFSystem_GetPlatformName(int platform);
		[DllImport (Engine.DllVersion)]
		[return: MarshalAs(UnmanagedType.LPStr)]
		private static extern string MFSystem_GetSystemName();
		[DllImport (Engine.DllVersion)]
		private static extern MFEndian MFSystem_GetPlatformEndian(int platform);
		[DllImport (Engine.DllVersion)]
		private static extern void MFSystem_SystemTime(out MFSystemTime systemTime);
		[DllImport (Engine.DllVersion)]
		private static extern void MFSystem_SystemTimeToFileTime(ref MFSystemTime systemTime, out MFFileTime fileTime);
		[DllImport (Engine.DllVersion)]
		private static extern void MFSystem_FileTimeToSystemTime(ref MFFileTime fileTime, out MFSystemTime systemTime);
		[DllImport (Engine.DllVersion)]
		private static extern void MFSystem_SystemTimeToLocalTime(ref MFSystemTime systemTime, out MFSystemTime localTime);
		[DllImport (Engine.DllVersion)]
		private static extern void MFSystem_LocalTimeToSystemTime(ref MFSystemTime localTime, out MFSystemTime systemTime);
		[DllImport (Engine.DllVersion)]
		private static extern ulong MFSystem_ReadRTC();
		[DllImport (Engine.DllVersion)]
		private static extern ulong MFSystem_GetRTCFrequency();
		[DllImport (Engine.DllVersion)]
		private static extern float MFSystem_GetTimeDelta();
		[DllImport (Engine.DllVersion)]
		private static extern float MFSystem_GetFPS();
		[DllImport (Engine.DllVersion)]
		private static extern uint MFSystem_GetFrameCounter();
	}
}
