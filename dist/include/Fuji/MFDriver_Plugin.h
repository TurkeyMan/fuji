#if !defined(_MFDRIVER_PLUGIN_H)
#define _MFDRIVER_PLUGIN_H

// configure video plugins
#if MF_RENDERER == MF_DRIVER_PLUGIN
	#if defined(MF_WINDOWS)
		#if defined(MF_COMPILER_VISUALC)
			#define MF_RENDERPLUGIN_D3D9
			#define MF_RENDERPLUGIN_D3D11
		#endif
		#define MF_RENDERPLUGIN_OPENGL
	#endif
#endif

// configure sound plugins
#if MF_SOUND == MF_DRIVER_PLUGIN
	#if defined(MF_LINUX)
		#define MF_SOUNDPLUGIN_ALSA
//		#define MF_SOUNDPLUGIN_PULSEAUDIO
//		#define MF_SOUNDPLUGIN_JACK
//		#define MF_SOUNDPLUGIN_SDL
		#define MF_SOUNDPLUGIN_OPENAL
		#define MF_SOUNDPLUGIN_PORTAUDIO
	#endif
#endif

#endif
