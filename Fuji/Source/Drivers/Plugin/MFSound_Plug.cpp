#include "Fuji.h"

#if MF_SOUND == MF_DRIVER_PLUGIN

#include "MFSound.h"
#include "MFSystem_Internal.h"

// macro to declare plugin callbacks
#define DECLARE_PLUGIN_CALLBACKS(driver) \
	void MFShader_InitModulePlatformSpecific_##driver(); \
	void MFShader_DeinitModulePlatformSpecific_##driver(); \

#define DEFINE_PLUGIN(driver) \
	{ \
		#driver, \
		MFShader_InitModulePlatformSpecific_##driver, \
		MFShader_DeinitModulePlatformSpecific_##driver, \
	},

// declare the available plugins
#if defined(MF_SOUNDPLUGIN_DSOUND)
	DECLARE_PLUGIN_CALLBACKS(DSound)
#endif
#if defined(MF_SOUNDPLUGIN_XAUDIO2)
	DECLARE_PLUGIN_CALLBACKS(XAudio2)
#endif
#if defined(MF_SOUNDPLUGIN_ASIO)
	DECLARE_PLUGIN_CALLBACKS(ASIO)
#endif
#if defined(MF_SOUNDPLUGIN_OPENAL)
	DECLARE_PLUGIN_CALLBACKS(OpenAL)
#endif
#if defined(MF_SOUNDPLUGIN_ALSA)
	DECLARE_PLUGIN_CALLBACKS(ALSA)
#endif
#if defined(MF_SOUNDPLUGIN_JACK)
	DECLARE_PLUGIN_CALLBACKS(JACK)
#endif
#if defined(MF_SOUNDPLUGIN_PULSE)
	DECLARE_PLUGIN_CALLBACKS(PulseAudio)
#endif
#if defined(MF_SOUNDPLUGIN_PORTAUDIO)
	DECLARE_PLUGIN_CALLBACKS(PortAudio)
#endif

// list of plugins
struct MFSoundPluginCallbacks
{
	const char *pDriverName;
	void (*pInitModulePlatformSpecific)();
	void (*pDeinitModulePlatformSpecific)();
};

// create an array of actual callbacks to the various enabled plugins
MFSoundPluginCallbacks gSoundPlugins[] =
{
#if defined(MF_SOUNDPLUGIN_DSOUND)
	DEFINE_PLUGIN(DSound)
#endif
#if defined(MF_SOUNDPLUGIN_XAUDIO2)
	DEFINE_PLUGIN(XAudio2)
#endif
#if defined(MF_SOUNDPLUGIN_ASIO)
	DEFINE_PLUGIN(ASIO)
#endif
#if defined(MF_SOUNDPLUGIN_OPENAL)
	DEFINE_PLUGIN(OpenAL)
#endif
#if defined(MF_SOUNDPLUGIN_ALSA)
	DEFINE_PLUGIN(ALSA)
#endif
#if defined(MF_SOUNDPLUGIN_JACK)
	DEFINE_PLUGIN(JACK)
#endif
#if defined(MF_SOUNDPLUGIN_PULSE)
	DEFINE_PLUGIN(PulseAudio)
#endif
#if defined(MF_SOUNDPLUGIN_PORTAUDIO)
	DEFINE_PLUGIN(PortAudio)
#endif
};

// the current active plugin
MFSoundPluginCallbacks *gpCurrentSoundPlugin = NULL;


/*** Function Wrappers ***/

void MFSound_InitModulePlatformSpecific()
{
	// choose the plugin from init settings
	gpCurrentSoundPlugin = &gSoundPlugins[gDefaults.plugin.audioPlugin];

	// init the plugin
	gpCurrentSoundPlugin->pInitModulePlatformSpecific();
}

void MFSound_DeinitModulePlatformSpecific()
{
	gpCurrentSoundPlugin->pDeinitModulePlatformSpecific();
}

#endif
