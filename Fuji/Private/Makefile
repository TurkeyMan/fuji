ifeq ($(PLATFORM),)
#$(error $$(PLATFORM) is undefined.
PLATFORM = LINUX
endif
ifeq ($(BUILD),)
#$(error $$(BUILD) is undefined.
BUILD = Release
endif

CONFIG = $(BUILD)_$(PLATFORM)
INTDIR = Build/$(CONFIG)

# Objects...
#-------------------------------------------------------------------

# zlib

ZLIB = Middleware/zlib/adler32.o \
	Middleware/zlib/compress.o \
	Middleware/zlib/crc32.o \
	Middleware/zlib/deflate.o \
	Middleware/zlib/gzio.o \
	Middleware/zlib/infback.o \
	Middleware/zlib/inffast.o \
	Middleware/zlib/inflate.o \
	Middleware/zlib/inftrees.o \
	Middleware/zlib/trees.o \
	Middleware/zlib/uncompr.o \
	Middleware/zlib/zutil.o \
	Middleware/zlib/minizip/ioapi.o \
	Middleware/zlib/minizip/mztools.o \
	Middleware/zlib/minizip/unzip.o \
	Middleware/zlib/minizip/zip.o

#-------------------------------------------------------------------

# Angelscript

ANGELSCRIPT = Middleware/angelscript/source/as_anyobject.o \
	Middleware/angelscript/source/as_arrayobject.o \
	Middleware/angelscript/source/as_builder.o \
	Middleware/angelscript/source/as_bytecode.o \
	Middleware/angelscript/source/as_c.o \
	Middleware/angelscript/source/as_callfunc.o \
	Middleware/angelscript/source/as_callfunc_mips.o \
	Middleware/angelscript/source/as_callfunc_ppc.o \
	Middleware/angelscript/source/as_callfunc_sh4.o \
	Middleware/angelscript/source/as_callfunc_x86.o \
	Middleware/angelscript/source/as_compiler.o \
	Middleware/angelscript/source/as_configgroup.o \
	Middleware/angelscript/source/as_context.o \
	Middleware/angelscript/source/as_datatype.o \
	Middleware/angelscript/source/as_gcobject.o \
	Middleware/angelscript/source/as_generic.o \
	Middleware/angelscript/source/as_module.o \
	Middleware/angelscript/source/as_objecttype.o \
	Middleware/angelscript/source/as_outputbuffer.o \
	Middleware/angelscript/source/as_parser.o \
	Middleware/angelscript/source/as_restore.o \
	Middleware/angelscript/source/as_scriptcode.o \
	Middleware/angelscript/source/as_scriptengine.o \
	Middleware/angelscript/source/as_scriptfunction.o \
	Middleware/angelscript/source/as_scriptnode.o \
	Middleware/angelscript/source/as_scriptstruct.o \
	Middleware/angelscript/source/as_string.o \
	Middleware/angelscript/source/as_string_util.o \
	Middleware/angelscript/source/as_thread.o \
	Middleware/angelscript/source/as_tokenizer.o \
	Middleware/angelscript/source/as_typeinfo.o \
	Middleware/angelscript/source/as_variablescope.o \
	Middleware/angelscript/scriptstring/scriptstring.o

#-------------------------------------------------------------------

# libpng

LIBPNG = Middleware/lpng1212/png.o \
	Middleware/lpng1212/pngerror.o \
	Middleware/lpng1212/pngget.o \
	Middleware/lpng1212/pngmem.o \
	Middleware/lpng1212/pngpread.o \
	Middleware/lpng1212/pngread.o \
	Middleware/lpng1212/pngrio.o \
	Middleware/lpng1212/pngrtran.o \
	Middleware/lpng1212/pngrutil.o \
	Middleware/lpng1212/pngset.o \
	Middleware/lpng1212/pngtrans.o \
	Middleware/lpng1212/pngvcrd.o \
	Middleware/lpng1212/pngwio.o \
	Middleware/lpng1212/pngwrite.o \
	Middleware/lpng1212/pngwtran.o \
	Middleware/lpng1212/pngwutil.o

#-------------------------------------------------------------------

# Regular build

OBJS := $(ZLIB) \
	$(ANGELSCRIPT) \
	Source/DebugMenu.o \
	Source/Display.o \
	Source/Image.o \
	Source/MFAnimation.o \
	Source/MFAnimScript.o \
	Source/MFBoundingVolume.o \
	Source/MFCallstack.o \
	Source/MFCollision.o \
	Source/MFCommandLine.o \
	Source/MFDebug.o \
	Source/MFFileSystem.o \
	Source/MFFont.o \
	Source/MFHeap.o \
	Source/MFIni.o \
	Source/MFInput.o \
	Source/MFMaterial.o \
	Source/MFMath.o \
	Source/MFMatrix.o \
	Source/MFModel.o \
	Source/MFNetwork.o \
	Source/MFPrimitive.o \
	Source/MFQuaternion.o \
	Source/MFRenderer.o \
	Source/MFScript.o \
	Source/MFScript_Pawn.o \
	Source/MFScript_Registration.o \
	Source/MFSockets.o \
	Source/MFString.o \
	Source/MFStringCache.o \
	Source/MFSystem.o \
	Source/MFTexture.o \
	Source/MFTextureUtil.o \
	Source/MFTranslation.o \
	Source/MFVector.o \
	Source/MFView.o \
	Source/Sprite.o \
	Source/Timer.o \
	Source/Util.o \
	Source/FileSystem/MFFileSystemHTTP.o \
	Source/FileSystem/MFFileSystemMemory.o \
	Source/FileSystem/MFFileSystemNative.o \
	Source/FileSystem/MFFileSystemZipFile.o \
	Source/Materials/MFMat_Effect.o \
	Source/Materials/MFMat_Standard.o

ifeq ($(PLATFORM),PSP)
OBJS := $(OBJS) PSP/Display_PSP.o \
	PSP/MFDebug_PSP.o \
	PSP/MFHeap_PSP.o \
	PSP/MFInput_PSP.o \
	PSP/MFMaterial_PSP.o \
	PSP/MFModel_PSP.o \
	PSP/MFPrimitive_PSP.o \
	PSP/MFRenderer_PSP.o \
	PSP/MFSockets_PSP.o \
	PSP/MFSound_PSP.o \
	PSP/MFSystem_PSP.o \
	PSP/MFTexture_PSP.o \
	PSP/MFThread_PSP.o \
	PSP/MFTranslation_PSP.o \
	PSP/FileSystem/MFFileSystemNative_PSP.o \
	PSP/Materials/MFMat_Standard_PSP.o \
	Null/Materials/MFMat_Effect_Null.o \
	Null/MFAuxillaryDisplay_Null.o \
	Null/MFParticleSystem_Null.o
endif

ifeq ($(PLATFORM),LINUX)
OBJS := $(OBJS) Linux/Display_Linux.o \
	Linux/MFDebug_Linux.o \
	Linux/MFHeap_Linux.o \
	Linux/MFInput_Linux.o \
	Linux/MFMaterial_Linux.o \
	Linux/MFModel_Linux.o \
	Linux/MFPrimitive_Linux.o \
	Linux/MFSockets_Linux.o \
	Linux/MFSound_Linux.o \
	Linux/MFSystem_Linux.o \
	Linux/MFTexture_Linux.o \
	Linux/MFThread_Linux.o \
	Linux/MFTranslation_Linux.o \
	Linux/FileSystem/MFFileSystemNative_Linux.o \
	Linux/Materials/MFMat_Effect_Linux.o \
	Linux/Materials/MFMat_Standard_Linux.o \
	Null/MFAuxillaryDisplay_Null.o \
	Null/MFParticleSystem_Null.o
endif

ifeq ($(PLATFORM),OSX)
OBJS := $(OBJS) OSX/Display_OSX.o \
	OSX/MFDebug_OSX.o \
	OSX/MFHeap_OSX.o \
	OSX/MFInput_OSX.o \
	OSX/MFMaterial_OSX.o \
	OSX/MFModel_OSX.o \
	OSX/MFPrimitive_OSX.o \
	OSX/MFSockets_OSX.o \
	OSX/MFSound_OSX.o \
	OSX/MFSystem_OSX.o \
	OSX/MFTexture_OSX.o \
	OSX/MFThread_OSX.o \
	OSX/MFTranslation_OSX.o \
	OSX/FileSystem/MFFileSystemNative_OSX.o \
	OSX/Materials/MFMat_Standard_OSX.o \
	Null/Materials/MFMat_Effect_Null.o \
	Null/MFAuxillaryDisplay_Null.o \
	Null/MFParticleSystem_Null.o
endif

ifeq ($(PLATFORM),PC)
OBJS := $(OBJS) PC/Display_PC.o \
	PC/MFAuxillaryDisplay_PC.o \
	PC/MFDebug_PC.o \
	PC/MFHeap_PC.o \
	PC/MFInput_PC.o \
	PC/MFInputMappings_PC.o \
	PC/MFMaterial_PC.o \
	PC/MFModel_PC.o \
	PC/MFPrimitive_PC.o \
	PC/MFRenderer_PC.o \
	PC/MFSockets_PC.o \
	PC/MFSound_PC.o \
	PC/MFSystem_PC.o \
	PC/MFTexture_PC.o \
	PC/MFThread_PC.o \
	PC/MFTranslation_PC.o \
	PC/FileSystem/MFFileSystemNative_PC.o \
	PC/Materials/MFMat_Effect_PC.o \
	PC/Materials/MFMat_Standard_PC.o \
	Null/MFParticleSystem_Null.o
endif

ifeq ($(PLATFORM),DC)
OBJS := $(OBJS) DC/Display_DC.o \
	DC/MFInput_DC.o \
	DC/MFMaterial_DC.o \
	DC/MFPrimitive_DC.o \
	DC/MFRenderer_DC.o \
	DC/MFSound_DC.o \
	DC/MFSystem_DC.o \
	DC/MFTexture_DC.o \
	DC/FileSystem/MFFileSystemNative_DC.o \
	DC/Materials/Mat_Standard_DC.o \
	Null/Materials/MFMat_Effect_Null.o \
	Null/MFAuxillaryDisplay_Null.o \
	Null/MFParticleSystem_Null.o
endif

ifeq ($(PLATFORM),GC)
OBJS := $(OBJS) GC/Display_GC.o \
	GC/MFInput_GC.o \
	GC/MFMaterial_GC.o \
	GC/MFPrimitive_GC.o \
	GC/MFRenderer_GC.o \
	GC/MFSound_GC.o \
	GC/MFSystem_GC.o \
	GC/MFTexture_GC.o \
	GC/FileSystem/MFFileSystemNative_GC.o \
	GC/Materials/MFMat_Standard_GC.o \
	Null/Materials/MFMat_Effect_Null.o \
	Null/MFAuxillaryDisplay_Null.o \
	Null/MFParticleSystem_Null.o
endif

ifeq ($(PLATFORM),PS2)
OBJS := $(OBJS) PS2/Display_PS2.o \
	PS2/MFDebug_PS2.o \
	PS2/MFHeap_PS2.o \
	PS2/MFInput_PS2.o \
	PS2/MFMaterial_PS2.o \
	PS2/MFModel_PS2.o \
	PS2/MFPrimitive_PS2.o \
	PS2/MFRenderer_PS2.o \
	PS2/MFSockets_PS2.o \
	PS2/MFSound_PS2.o \
	PS2/MFSystem_PS2.o \
	PS2/MFTexture_PS2.o \
	PS2/MFThread_PS2.o \
	PS2/MFTranslation_PS2.o \
	PS2/FileSystem/MFFileSystemNative_PS2.o \
	PS2/Materials/MFMat_Standard_PS2.o \
	Null/Materials/MFMat_Effect_Null.o \
	Null/MFAuxillaryDisplay_Null.o \
	Null/MFParticleSystem_Null.o
endif

#XBox/Display_XB.o
#XBox/MFDebug_XB.o
#XBox/MFHeap_XB.o
#XBox/MFInput_XB.o
#XBox/MFMaterial_XB.o
#XBox/MFModel_XB.o
#XBox/MFPrimitive_XB.o
#XBox/MFRenderer_XB.o
#XBox/MFSockets_XB.o
#XBox/MFSound_XB.o
#XBox/MFSystem_XB.o
#XBox/MFTexture_XB.o
#XBox/MFThread_XB.o
#XBox/MFTranslation_XB.o
#XBox/FileSystem/MFFileSystemNative_XB.o
#XBox/Materials/MFMat_Standard_XB.o
#Null/Display_Null.o
#Null/MFDebug_Null.o
#Null/MFHeap_Null.o
#Null/MFInput_Null.o
#Null/MFMaterial_Null.o
#Null/MFModel_Null.o
#Null/MFPrimitive_Null.o
#Null/MFSockets_Null.o
#Null/MFSound_Null.o
#Null/MFSystem_Null.o
#Null/MFTexture_Null.o
#Null/MFThread_Null.o
#Null/MFTranslation_Null.o
#Null/FileSystem/MFFileSystemNative_Null.o
#Null/Materials/MFMat_Effect_Null.o
#Null/Materials/MFMat_Standard_Null.o

#-------------------------------------------------------------------

# Util build

UTIL_OBJS := $(ZLIB) \
	$(LIBPNG) \
	Source/MFCallstack.o \
	Source/MFDebug.o \
	Source/MFHeap.o \
	Source/MFIni.o \
	Source/MFMatrix.o \
	Source/MFString.o \
	Source/MFStringCache.o \
	Source/MFTextureUtil.o \
	Source/MFVector.o \
	Source/MFQuaternion.o \
	Source/Util.o \
	Util/F3D.o \
	Null/MFHeap_Null.o \

ifeq ($(PLATFORM),LINUX)
UTIL_OBJS := $(UTIL_OBJS) Linux/MFDebug_Linux.o
endif
ifeq ($(PLATFORM),OSX)
UTIL_OBJS := $(UTIL_OBJS) OSX/MFDebug_OSX.o
endif
ifeq ($(PLATFORM),PC)
UTIL_OBJS := $(UTIL_OBJS) PC/MFDebug_PC.o
endif

#-------------------------------------------------------------------
# End Objects


TARGET_LIB = Lib/libFuji


ifeq ($(PLATFORM),LINUX)
PLATFORMINCLUDE = Linux
else
PLATFORMINCLUDE = $(PLATFORM)
endif

INCDIR = Include Source $(PLATFORMINCLUDE) Middleware Middleware/zlib Middleware/angelscript/include
CFLAGS = -Wall -D_$(PLATFORM)
CXXFLAGS = -fno-exceptions -fno-rtti
ASFLAGS = 


ifeq ($(BUILD),Debug)
CFLAGS := $(CFLAGS) -D_DEBUG -O0
endif
ifeq ($(BUILD),Release)
CFLAGS := $(CFLAGS) -D_RELEASE -O1
endif
ifeq ($(BUILD),Retail)
CFLAGS := $(CFLAGS) -D_RETAIL -O3
endif


ifeq ($(MAKECMDGOALS),util)
TARGET_LIB := $(TARGET_LIB)Util
INTDIR := $(INTDIR)_Util
OBJS = $(UTIL_OBJS)
CFLAGS := $(CFLAGS) -D_FUJI_UTIL
endif


TARGET_LIB := $(TARGET_LIB)_$(CONFIG).a

ifeq ($(PLATFORM),PSP)

# because my environment variables never seem to work! :(
ifndef ($(PSPSDK))
PSPSDK = /usr/local/pspdev/psp/sdk
endif

CC       = psp-gcc
CXX      = psp-g++
AS       = psp-gcc
LD       = psp-gcc
AR       = psp-ar
RANLIB   = psp-ranlib
STRIP    = psp-strip
MKSFO    = mksfo
PACK_PBP = pack-pbp
FIXUP    = psp-fixup-imports

CFLAGS := $(CFLAGS) -G0

# Add in PSPSDK includes and libraries.
INCDIR   := $(INCDIR) . $(PSPSDK)/include

# end PSP
endif

ifeq ($(PLATFORM),PS2)

CC       = ee-gcc
CXX      = ee-g++
AS       = ee-as
LD       = ee-ld
AR       = ee-ar
RANLIB   = ee-ranlib
STRIP    = ee-strip

CFLAGS := $(CFLAGS) -G0

# Add in PSPSDK includes and libraries.
INCDIR   := $(INCDIR) . $(PS2SDK)/ee/include $(PS2SDK)/common/include

# end PS2
endif

ifeq ($(PLATFORM),GC)

# because my environment variables never seem to work! :(
ifndef ($(PSPSDK))
OGC = /usr/local/devkitPPC/ogc
endif

CC       = powerpc-gekko-gcc
CXX      = powerpc-gekko-g++
AS       = powerpc-gekko-gcc
LD       = powerpc-gekko-gcc
AR       = powerpc-gekko-ar
RANLIB   = powerpc-gekko-ranlib
STRIP    = powerpc-gekko-strip

# Add in OGC includes.
INCDIR   := $(INCDIR) . $(OGC)/include

# end GC
endif

ifeq ($(PLATFORM),LINUX)

CC       = gcc
CXX      = g++
AS       = as
LD       = ld
AR       = ar
RANLIB   = ranlib
STRIP    = strip
INCDIR	:= $(INCDIR) . ./Linux

# end LINUX
endif

ifeq ($(PLATFORM),OSX)

CC       = gcc
CXX      = g++
AS       = as
LD       = ld
AR       = ar
RANLIB   = ranlib
STRIP    = strip
INCDIR   :=$(INCDIR) . /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/include/
# end OSX
endif


OBJLIST = $(patsubst %,$(INTDIR)/%,$(OBJS))


CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)


all: makedirs $(EXTRA_TARGETS) $(TARGET_LIB)

util: makedirs $(TARGET_LIB)

$(INTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(INTDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET_LIB): $(OBJLIST)
	$(AR) cru $@ $(OBJLIST)
	$(RANLIB) $@

makedirs:
	@mkdir -p Lib
	@mkdir -p $(INTDIR)

archiver: Bin/Archiver
	make -C ../Utilities/Archiver PLATFORM=$(PLATFORM) BUILD=$(BUILD)

convertase: Bin/ConvertASE
	make -C ../Utilities/ConvertASE PLATFORM=$(PLATFORM) BUILD=$(BUILD)

converttex: Bin/ConvertTEX
	make -C ../Utilities/ConvertTEX PLATFORM=$(PLATFORM) BUILD=$(BUILD)

convertfnt: Bin/ConvertFNT
	make -C ../Utilities/ConvertFNT PLATFORM=$(PLATFORM) BUILD=$(BUILD)

convertdlg: Bin/ConvertDlg
	make -C ../Utilities/ConvertDlg PLATFORM=$(PLATFORM) BUILD=$(BUILD)

tools: archiver convertase converttex convertfnt convertdlg

clean:
	-rm -f $(TARGET_LIB) $(EXTRA_CLEAN) $(EXTRA_TARGETS)
	-rm -f -r $(INTDIR) $(INTDIR)_Util
