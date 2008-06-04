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


# tinyxml

TINYXML = Middleware/tinyxml/tinystr.o \
	Middleware/tinyxml/tinyxml.o \
	Middleware/tinyxml/tinyxmlerror.o \
	Middleware/tinyxml/tinyxmlparser.o

#-------------------------------------------------------------------

# Regular build

OBJS := $(ZLIB) \
	$(LIBPNG) \
	$(ANGELSCRIPT) \
	$(TINYXML) \
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
	Source/MFMesh.o \
	Source/MFModel.o \
	Source/MFNetwork.o \
	Source/MFPrimitive.o \
	Source/MFQuaternion.o \
	Source/MFRenderer.o \
	Source/MFScript.o \
	Source/MFScript_Pawn.o \
	Source/MFScript_Registration.o \
	Source/MFSockets.o \
	Source/MFSound.o \
	Source/MFSound_Vorbis.o \
	Source/MFSound_MAD.o \
	Source/MFSound_PSPAudioCodec.o \
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
	Source/FileSystem/MFFileSystemNative.o \
	Source/FileSystem/MFFileSystemMemory.o \
	Source/FileSystem/MFFileSystemCachedFile.o \
	Source/FileSystem/MFFileSystemZipFile.o \
	Source/FileSystem/MFFileSystemHTTP.o \
	Source/Materials/MFMat_Effect.o \
	Source/Materials/MFMat_Standard.o \
	Source/Drivers/D3D9/MFMaterial_D3D9.o \
	Source/Drivers/D3D9/MFModel_D3D9.o \
	Source/Drivers/D3D9/MFPrimitive_D3D9.o \
	Source/Drivers/D3D9/MFRenderer_D3D9.o \
	Source/Drivers/D3D9/MFTexture_D3D9.o \
	Source/Drivers/D3D9/Materials/MFMat_Effect_D3D9.o \
	Source/Drivers/D3D9/Materials/MFMat_Standard_D3D9.o \
	Source/Drivers/Linux/MFFileSystemNative_Linux.o \
	Source/Drivers/Linux/MFInput_Linux.o \
	Source/Drivers/Linux/MFSound_ALSA.o \
	Source/Drivers/Linux/MFSystem_Linux.o \
	Source/Drivers/Linux/MFThread_Linux.o \
	Source/Drivers/Linux/MFTranslation_Linux.o \
	Source/Drivers/Null/MFAuxillaryDisplay_Null.o \
	Source/Drivers/Null/MFDebug_Null.o \
	Source/Drivers/Null/MFDisplay_Null.o \
	Source/Drivers/Null/MFFileSystemNative_Null.o \
	Source/Drivers/Null/MFHeap_Null.o \
	Source/Drivers/Null/MFInput_Null.o \
	Source/Drivers/Null/MFMaterial_Null.o \
	Source/Drivers/Null/MFModel_Null.o \
	Source/Drivers/Null/MFParticleSystem_Null.o \
	Source/Drivers/Null/MFPrimitive_Null.o \
	Source/Drivers/Null/MFSockets_Null.o \
	Source/Drivers/Null/MFSound_Null.o \
	Source/Drivers/Null/MFSystem_Null.o \
	Source/Drivers/Null/MFTexture_Null.o \
	Source/Drivers/Null/MFThread_Null.o \
	Source/Drivers/Null/MFTranslation_Null.o \
	Source/Drivers/Null/Materials/MFMat_Effect_Null.o \
	Source/Drivers/Null/Materials/MFMat_Standard_Null.o \
	Source/Drivers/OpenGL/MFMaterial_OpenGL.o \
	Source/Drivers/OpenGL/MFModel_OpenGL.o \
	Source/Drivers/OpenGL/MFPrimitive_OpenGL.o \
	Source/Drivers/OpenGL/MFRenderer_OpenGL.o \
	Source/Drivers/OpenGL/MFTexture_OpenGL.o \
	Source/Drivers/OpenGL/Materials/MFMat_Standard_OpenGL.o \
	Source/Drivers/PC/MFAuxillaryDisplay_PC.o \
	Source/Drivers/PC/MFDebug_PC.o \
	Source/Drivers/PC/MFDisplay_PC.o \
	Source/Drivers/PC/MFInputMappings_PC.o \
	Source/Drivers/PC/MFInput_PC.o \
	Source/Drivers/PC/MFSound_PC.o \
	Source/Drivers/PC/MFSystem_PC.o \
	Source/Drivers/PC/MFTranslation_PC.o \
	Source/Drivers/PSP/MFDebug_PSP.o \
	Source/Drivers/PSP/MFDisplay_PSP.o \
	Source/Drivers/PSP/MFFileSystemNative_PSP.o \
	Source/Drivers/PSP/MFHeap_PSP.o \
	Source/Drivers/PSP/MFInput_PSP.o \
	Source/Drivers/PSP/MFMaterial_PSP.o \
	Source/Drivers/PSP/MFModel_PSP.o \
	Source/Drivers/PSP/MFPrimitive_PSP.o \
	Source/Drivers/PSP/MFRenderer_PSP.o \
	Source/Drivers/PSP/MFSockets_PSP.o \
	Source/Drivers/PSP/MFSound_PSP.o \
	Source/Drivers/PSP/MFSystem_PSP.o \
	Source/Drivers/PSP/MFTexture_PSP.o \
	Source/Drivers/PSP/MFThread_PSP.o \
	Source/Drivers/PSP/MFTranslation_PSP.o \
	Source/Drivers/PSP/Materials/MFMat_Standard_PSP.o \
	Source/Drivers/WIN32/MFFileSystemNative_WIN32.o \
	Source/Drivers/WIN32/MFHeap_WIN32.o \
	Source/Drivers/WIN32/MFSockets_WINSOCK.o \
	Source/Drivers/WIN32/MFThread_WIN32.o \
	Source/Drivers/X11/MFDisplay_X11.o \
	Source/Drivers/XBox/MFDisplay_XB.o \
	Source/Drivers/XBox/MFHeap_XB.o \
	Source/Drivers/XBox/MFInput_XB.o \
	Source/Drivers/XBox/MFMaterial_XB.o \
	Source/Drivers/XBox/MFModel_XB.o \
	Source/Drivers/XBox/MFPrimitive_XB.o \
	Source/Drivers/XBox/MFRenderer_XB.o \
	Source/Drivers/XBox/MFSystem_XB.o \
	Source/Drivers/XBox/MFTexture_XB.o \
	Source/Drivers/XBox/MFTranslation_XB.o \
	Source/Drivers/XBox/Materials/MFMat_Standard_XB.o

#-------------------------------------------------------------------

# Util build

UTIL_OBJS := $(ZLIB) \
	$(LIBPNG) \
	$(TINYXML) \
	Source/MFCallstack.o \
	Source/MFDebug.o \
	Source/MFFileSystem.o \
	Source/MFHeap.o \
	Source/MFIni.o \
	Source/MFMatrix.o \
	Source/MFSockets.o \
	Source/MFString.o \
	Source/MFStringCache.o \
	Source/MFSystem.o \
	Source/MFTextureUtil.o \
	Source/MFVector.o \
	Source/MFQuaternion.o \
	Source/Util.o \
	Util/F3D.o \
	Source/FileSystem/MFFileSystemCachedFile.o \
	Source/FileSystem/MFFileSystemHTTP.o \
	Source/FileSystem/MFFileSystemMemory.o \
	Source/FileSystem/MFFileSystemNative.o \
	Source/FileSystem/MFFileSystemZipFile.o \
	Source/Drivers/Linux/MFFileSystemNative_Linux.o \
	Source/Drivers/Linux/MFSystem_Linux.o \
	Source/Drivers/Linux/MFThread_Linux.o \
	Source/Drivers/Null/MFDebug_Null.o \
	Source/Drivers/Null/MFFileSystemNative_Null.o \
	Source/Drivers/Null/MFHeap_Null.o \
	Source/Drivers/Null/MFSockets_Null.o \
	Source/Drivers/Null/MFSystem_Null.o \
	Source/Drivers/Null/MFThread_Null.o \
	Source/Drivers/PC/MFDebug_PC.o \
	Source/Drivers/PC/MFSystem_PC.o \
	Source/Drivers/PSP/MFDebug_PSP.o \
	Source/Drivers/WIN32/MFFileSystemNative_WIN32.o \
	Source/Drivers/WIN32/MFHeap_WIN32.o \
	Source/Drivers/WIN32/MFSockets_WINSOCK.o \
	Source/Drivers/WIN32/MFThread_WIN32.o

#-------------------------------------------------------------------
# End Objects


TARGET_LIB = Lib/libFuji


INCDIR = Include Source Source/Images/$(PLATFORM) Middleware Middleware/zlib Middleware/angelscript/include
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

ifeq ($(PLATFORM),SYMBIAN)

SYMBIANDEV = C:\Symbian\9.3\S60_3rd_FP2_Beta\epoc32

CC       = arm-none-symbianelf-gcc
CXX      = arm-none-symbianelf-g++
AS       = arm-none-symbianelf-as
LD       = arm-none-symbianelf-ld
AR       = arm-none-symbianelf-ar
RANLIB   = arm-none-symbianelf-ranlib
STRIP    = arm-none-symbianelf-strip
INCDIR   :=$(INCDIR) . $(SYMBIANDEV)/include $(SYMBIANDEV)/include/stdapis

# end SYMBIAN
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

archiver:
	make -C ../Utilities/Archiver PLATFORM=$(PLATFORM) BUILD=$(BUILD)

convertase:
	make -C ../Utilities/ConvertASE PLATFORM=$(PLATFORM) BUILD=$(BUILD)

converttex:
	make -C ../Utilities/ConvertTEX PLATFORM=$(PLATFORM) BUILD=$(BUILD)

convertsnd:
	make -C ../Utilities/ConvertSND PLATFORM=$(PLATFORM) BUILD=$(BUILD)

convertfnt:
	make -C ../Utilities/ConvertFNT PLATFORM=$(PLATFORM) BUILD=$(BUILD)

convertdlg:
	make -C ../Utilities/ConvertDlg PLATFORM=$(PLATFORM) BUILD=$(BUILD)

tools: archiver convertase converttex convertsnd convertfnt convertdlg

clean:
	-rm -f $(TARGET_LIB) $(EXTRA_CLEAN) $(EXTRA_TARGETS)
	-rm -f -r $(INTDIR) $(INTDIR)_Util
