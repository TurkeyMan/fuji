#PLATFORM = PSP
#BUILD = Release
#TARGET = libFuji
TARGET_LIB = Lib/libFuji

ifeq ($(PLATFORM),)
#$(error $$(PLATFORM) is undefined.
PLATFORM = PSP
endif
ifeq ($(BUILD),)
#$(error $$(BUILD) is undefined.
BUILD = Release
endif

OBJS = Middleware/zlib/adler32.o
OBJS := $(OBJS) Middleware/zlib/compress.o
OBJS := $(OBJS) Middleware/zlib/crc32.o
OBJS := $(OBJS) Middleware/zlib/deflate.o
OBJS := $(OBJS) Middleware/zlib/gzio.o
OBJS := $(OBJS) Middleware/zlib/infback.o
OBJS := $(OBJS) Middleware/zlib/inffast.o
OBJS := $(OBJS) Middleware/zlib/inflate.o
OBJS := $(OBJS) Middleware/zlib/inftrees.o
OBJS := $(OBJS) Middleware/zlib/trees.o
OBJS := $(OBJS) Middleware/zlib/uncompr.o
OBJS := $(OBJS) Middleware/zlib/zutil.o
OBJS := $(OBJS) Middleware/zlib/minizip/ioapi.o
OBJS := $(OBJS) Middleware/zlib/minizip/mztools.o
OBJS := $(OBJS) Middleware/zlib/minizip/unzip.o
OBJS := $(OBJS) Middleware/zlib/minizip/zip.o
OBJS := $(OBJS) Middleware/angelscript/scriptstring/scriptstring.o
OBJS := $(OBJS) Middleware/angelscript/source/as_anyobject.o
OBJS := $(OBJS) Middleware/angelscript/source/as_arrayobject.o
OBJS := $(OBJS) Middleware/angelscript/source/as_builder.o
OBJS := $(OBJS) Middleware/angelscript/source/as_bytecode.o
OBJS := $(OBJS) Middleware/angelscript/source/as_c.o
OBJS := $(OBJS) Middleware/angelscript/source/as_callfunc.o
OBJS := $(OBJS) Middleware/angelscript/source/as_callfunc_mips.o
OBJS := $(OBJS) Middleware/angelscript/source/as_callfunc_sh4.o
OBJS := $(OBJS) Middleware/angelscript/source/as_callfunc_x86.o
OBJS := $(OBJS) Middleware/angelscript/source/as_compiler.o
OBJS := $(OBJS) Middleware/angelscript/source/as_configgroup.o
OBJS := $(OBJS) Middleware/angelscript/source/as_context.o
OBJS := $(OBJS) Middleware/angelscript/source/as_datatype.o
OBJS := $(OBJS) Middleware/angelscript/source/as_gcobject.o
OBJS := $(OBJS) Middleware/angelscript/source/as_generic.o
OBJS := $(OBJS) Middleware/angelscript/source/as_module.o
OBJS := $(OBJS) Middleware/angelscript/source/as_objecttype.o
OBJS := $(OBJS) Middleware/angelscript/source/as_outputbuffer.o
OBJS := $(OBJS) Middleware/angelscript/source/as_parser.o
OBJS := $(OBJS) Middleware/angelscript/source/as_restore.o
OBJS := $(OBJS) Middleware/angelscript/source/as_scriptcode.o
OBJS := $(OBJS) Middleware/angelscript/source/as_scriptengine.o
OBJS := $(OBJS) Middleware/angelscript/source/as_scriptfunction.o
OBJS := $(OBJS) Middleware/angelscript/source/as_scriptnode.o
OBJS := $(OBJS) Middleware/angelscript/source/as_scriptstruct.o
OBJS := $(OBJS) Middleware/angelscript/source/as_string.o
OBJS := $(OBJS) Middleware/angelscript/source/as_string_util.o
OBJS := $(OBJS) Middleware/angelscript/source/as_thread.o
OBJS := $(OBJS) Middleware/angelscript/source/as_tokenizer.o
OBJS := $(OBJS) Middleware/angelscript/source/as_typeinfo.o
OBJS := $(OBJS) Middleware/angelscript/source/as_variablescope.o

OBJS := $(OBJS) Source/DebugMenu.o
OBJS := $(OBJS) Source/Display.o
OBJS := $(OBJS) Source/Image.o
OBJS := $(OBJS) Source/MFBoundingVolume.o
OBJS := $(OBJS) Source/MFCallstack.o
OBJS := $(OBJS) Source/MFCollision.o
OBJS := $(OBJS) Source/MFCommandLine.o
OBJS := $(OBJS) Source/MFDebug.o
OBJS := $(OBJS) Source/MFFileSystem.o
OBJS := $(OBJS) Source/MFFont.o
OBJS := $(OBJS) Source/MFHeap.o
OBJS := $(OBJS) Source/MFIni.o
OBJS := $(OBJS) Source/MFInput.o
OBJS := $(OBJS) Source/MFMaterial.o
OBJS := $(OBJS) Source/MFMath.o
OBJS := $(OBJS) Source/MFMatrix.o
OBJS := $(OBJS) Source/MFModel.o
OBJS := $(OBJS) Source/MFNetwork.o
OBJS := $(OBJS) Source/MFPrimitive.o
OBJS := $(OBJS) Source/MFRenderer.o
OBJS := $(OBJS) Source/MFScript.o
OBJS := $(OBJS) Source/MFScript_Pawn.o
OBJS := $(OBJS) Source/MFScript_Registration.o
OBJS := $(OBJS) Source/MFSockets.o
OBJS := $(OBJS) Source/MFString.o
OBJS := $(OBJS) Source/MFStringCache.o
OBJS := $(OBJS) Source/MFSystem.o
OBJS := $(OBJS) Source/MFTexture.o
OBJS := $(OBJS) Source/MFTextureUtil.o
OBJS := $(OBJS) Source/MFTranslation.o
OBJS := $(OBJS) Source/MFVector.o
OBJS := $(OBJS) Source/MFView.o
OBJS := $(OBJS) Source/Sprite.o
OBJS := $(OBJS) Source/Timer.o
OBJS := $(OBJS) Source/Util.o
OBJS := $(OBJS) Source/FileSystem/MFFileSystemHTTP.o
OBJS := $(OBJS) Source/FileSystem/MFFileSystemMemory.o
OBJS := $(OBJS) Source/FileSystem/MFFileSystemNative.o
OBJS := $(OBJS) Source/FileSystem/MFFileSystemZipFile.o
OBJS := $(OBJS) Source/Materials/MFMat_Effect.o
OBJS := $(OBJS) Source/Materials/MFMat_Standard.o

ifeq ($(PLATFORM),PSP)
OBJS := $(OBJS) PSP/Display_PSP.o
OBJS := $(OBJS) PSP/MFDebug_PSP.o
OBJS := $(OBJS) PSP/MFHeap_PSP.o
OBJS := $(OBJS) PSP/MFInput_PSP.o
OBJS := $(OBJS) PSP/MFMaterial_PSP.o
OBJS := $(OBJS) PSP/MFPrimitive_PSP.o
OBJS := $(OBJS) PSP/MFRenderer_PSP.o
OBJS := $(OBJS) PSP/MFSockets_PSP.o
OBJS := $(OBJS) PSP/MFSound_PSP.o
OBJS := $(OBJS) PSP/MFSystem_PSP.o
OBJS := $(OBJS) PSP/MFTexture_PSP.o
OBJS := $(OBJS) PSP/MFThread_PSP.o
OBJS := $(OBJS) PSP/MFTranslation_PSP.o
OBJS := $(OBJS) PSP/FileSystem/MFFileSystemNative_PSP.o
OBJS := $(OBJS) PSP/Materials/MFMat_Standard_PSP.o
OBJS := $(OBJS) Null/MFModel_Null.o
OBJS := $(OBJS) Null/Materials/MFMat_Effect_Null.o
endif

ifeq ($(PLATFORM),DC)
OBJS := $(OBJS) DC/Display_DC.o
OBJS := $(OBJS) DC/MFInput_DC.o
OBJS := $(OBJS) DC/MFMaterial_DC.o
OBJS := $(OBJS) DC/MFPrimitive_DC.o
OBJS := $(OBJS) DC/MFRenderer_DC.o
OBJS := $(OBJS) DC/MFSound_DC.o
OBJS := $(OBJS) DC/MFSystem_DC.o
OBJS := $(OBJS) DC/MFTexture_DC.o
OBJS := $(OBJS) DC/FileSystem/MFFileSystemNative_DC.o
OBJS := $(OBJS) DC/Materials/Mat_Standard_DC.o
OBJS := $(OBJS) Null/Materials/MFMat_Effect_Null.o
endif

ifeq ($(PLATFORM),GC)
OBJS := $(OBJS) GC/Display_GC.o
OBJS := $(OBJS) GC/MFInput_GC.o
OBJS := $(OBJS) GC/MFMaterial_GC.o
OBJS := $(OBJS) GC/MFPrimitive_GC.o
OBJS := $(OBJS) GC/MFRenderer_GC.o
OBJS := $(OBJS) GC/MFSound_GC.o
OBJS := $(OBJS) GC/MFSystem_GC.o
OBJS := $(OBJS) GC/MFTexture_GC.o
OBJS := $(OBJS) GC/FileSystem/MFFileSystemNative_GC.o
OBJS := $(OBJS) GC/Materials/Mat_Standard_GC.o
OBJS := $(OBJS) Null/Materials/MFMat_Effect_Null.o
endif

ifeq ($(PLATFORM),Linux)
OBJS := $(OBJS) Linux/Display_Linux.o
OBJS := $(OBJS) Linux/MFDebug_Linux.o
OBJS := $(OBJS) Linux/MFHeap_Linux.o
OBJS := $(OBJS) Linux/MFInput_Linux.o
OBJS := $(OBJS) Linux/MFMaterial_Linux.o
OBJS := $(OBJS) Linux/MFModel_Linux.o
OBJS := $(OBJS) Linux/MFPrimitive_Linux.o
OBJS := $(OBJS) Linux/MFSockets_Linux.o
OBJS := $(OBJS) Linux/MFSound_Linux.o
OBJS := $(OBJS) Linux/MFSystem_Linux.o
OBJS := $(OBJS) Linux/MFTexture_Linux.o
OBJS := $(OBJS) Linux/MFThread_Linux.o
OBJS := $(OBJS) Linux/MFTranslation_Linux.o
OBJS := $(OBJS) Linux/FileSystem/MFFileSystemNative_Linux.o
OBJS := $(OBJS) Linux/Materials/MFMat_Effect_Linux.o
OBJS := $(OBJS) Linux/Materials/MFMat_Standard_Linux.o
endif

ifeq ($(PLATFORM),PS2)
OBJS := $(OBJS) PS2/Display_PS2.o
OBJS := $(OBJS) PS2/MFInput_PS2.o
OBJS := $(OBJS) PS2/MFMaterial_PS2.o
OBJS := $(OBJS) PS2/MFPrimitive_PS2.o
OBJS := $(OBJS) PS2/MFRenderer_PS2.o
OBJS := $(OBJS) PS2/MFSound_PS2.o
OBJS := $(OBJS) PS2/MFSystem_PS2.o
OBJS := $(OBJS) PS2/MFTexture_PS2.o
OBJS := $(OBJS) PS2/FileSystem/MFFileSystemNative_PS2.o
OBJS := $(OBJS) PS2/Materials/Mat_Standard_PS2.o
OBJS := $(OBJS) Null/Materials/MFMat_Effect_Null.o
endif

ifeq ($(PLATFORM),PC)
OBJS := $(OBJS) PC/Display_PC.o
OBJS := $(OBJS) PC/MFDebug_PC.o
OBJS := $(OBJS) PC/MFHeap_PC.o
OBJS := $(OBJS) PC/MFInputMappings_PC.o
OBJS := $(OBJS) PC/MFInput_PC.o
OBJS := $(OBJS) PC/MFMaterial_PC.o
OBJS := $(OBJS) PC/MFModel_PC.o
OBJS := $(OBJS) PC/MFPrimitive_PC.o
OBJS := $(OBJS) PC/MFRenderer_PC.o
OBJS := $(OBJS) PC/MFSockets_PC.o
OBJS := $(OBJS) PC/MFSound_PC.o
OBJS := $(OBJS) PC/MFSystem_PC.o
OBJS := $(OBJS) PC/MFTexture_PC.o
OBJS := $(OBJS) PC/MFThread_PC.o
OBJS := $(OBJS) PC/MFTranslation_PC.o
OBJS := $(OBJS) PC/FileSystem/MFFileSystemNative_PC.o
OBJS := $(OBJS) PC/Materials/MFMat_Effect_PC.o
OBJS := $(OBJS) PC/Materials/MFMat_Standard_PC.o
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
#Util/F3D.o

CONFIG = $(BUILD)_$(PLATFORM)

INCDIR = Include Source $(PLATFORM) Middleware Middleware/zlib Middleware/angelscript/include
CFLAGS = -Wall -D_$(PLATFORM)

ifeq ($(BUILD),Debug)
CFLAGS := $(CFLAGS)  -D_DEBUG -O0
endif
ifeq ($(BUILD),Release)
CFLAGS := $(CFLAGS)  -D_RELEASE -O1
endif
ifeq ($(BUILD),Retail)
CFLAGS := $(CFLAGS)  -D_RETAIL -O3
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

#LIBDIR = Lib/
#LDFLAGS =
#LIBS= -lpspgu -lm

TARGET_LIB := $(TARGET_LIB)_$(CONFIG).a

ifeq ($(PLATFORM),PSP)

ifneq ($(TARGET),)
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = $(TARGET) $(PLATFORM)
endif

PSPSDK=C:/cygwin/usr/local/devkitPSP/psp/sdk

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

CFLAGS = -G0

# Add in PSPSDK includes and libraries.
INCDIR   := $(INCDIR) . $(PSPSDK)/include
LIBDIR   := $(LIBDIR) . $(PSPSDK)/lib

ifeq ($(BUILD_PRX),1)
LDFLAGS  := $(addprefix -L,$(LIBDIR)) -specs=$(PSPSDK)/lib/prxspecs -Wl,-q,-T$(PSPSDK)/lib/linkfile.prx $(LDFLAGS)
EXTRA_CLEAN += $(TARGET).elf
# Setup default exports if needed
ifdef PRX_EXPORTS
EXPORT_OBJ=$(patsubst %.exp,%.o,$(PRX_EXPORTS))
EXTRA_CLEAN += $(EXPORT_OBJ)
else
EXPORT_OBJ=$(PSPSDK)/lib/prxexports.o
endif
else
LDFLAGS  := $(addprefix -L,$(LIBDIR)) $(LDFLAGS)
endif

# end PSP
endif
ifeq ($(PLATFORM),GC)

OGC = C:/cygwin/usr/local/devkitPPC/ogc

CC       = powerpc-gekko-gcc
CXX      = powerpc-gekko-g++
AS       = powerpc-gekko-gcc
LD       = powerpc-gekko-gcc
AR       = powerpc-gekko-ar
RANLIB   = powerpc-gekko-ranlib
STRIP    = powerpc-gekko-strip

# Add in OGC includes and libraries.
INCDIR   := $(INCDIR) . $(OGC)/include
LIBDIR   := $(LIBDIR) . $(OGC)/lib

# end GC
endif

ifeq ($(PLATFORM),Linux)

CC       = gcc
CXX      = g++
AS       = as
LD       = ld
AR       = ar
RANLIB   = ranlib
STRIP    = strip

# end Linux
endif

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)

# more PSP stuff...
ifeq ($(PLATFORM),PSP)
# Library selection.  By default we link with Newlib's libc.  Allow the
# user to link with PSPSDK's libc if USE_PSPSDK_LIBC is set to 1.

ifeq ($(USE_KERNEL_LIBC),1)
# Use the PSP's kernel libc
PSPSDL_LIBC_LIB =
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
else
ifeq ($(USE_PSPSDK_LIBC),1)
# Use the pspsdk libc
PSPSDK_LIBC_LIB = -lpsplibc
CFLAGS := -I$(PSPSDK)/include/libc $(CFLAGS)
else
# Use newlib (urgh)
PSPSDK_LIBC_LIB = -lc
endif
endif


# Link with following default libraries.  Other libraries should be specified in the $(LIBS) variable.
# TODO: This library list needs to be generated at configure time.
#
ifeq ($(USE_KERNEL_LIBS),1)
PSPSDK_LIBS = -lpspdebug -lpspdisplay_driver -lpspctrl_driver -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspkernel
else
PSPSDK_LIBS = -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspnet \
			-lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility \
			-lpspuser -lpspkernel
endif

# Define the overridable parameters for EBOOT.PBP
ifndef PSP_EBOOT_TITLE
PSP_EBOOT_TITLE = $(TARGET)
endif

ifndef PSP_EBOOT_SFO
PSP_EBOOT_SFO = PARAM.SFO
endif

ifndef PSP_EBOOT_ICON
PSP_EBOOT_ICON = ICON.PNG
endif

ifndef PSP_EBOOT_ICON1
PSP_EBOOT_ICON1 = PIC0.PNG
endif

ifndef PSP_EBOOT_UNKPNG
PSP_EBOOT_UNKPNG = NULL
endif

ifndef PSP_EBOOT_PIC1
PSP_EBOOT_PIC1 = PIC1.PNG
endif

ifndef PSP_EBOOT_SND0
PSP_EBOOT_SND0 = NULL
endif

ifndef PSP_EBOOT_PSAR
PSP_EBOOT_PSAR = NULL
endif

ifndef PSP_EBOOT
PSP_EBOOT = EBOOT.PBP
endif

# more PSP stuff
endif

ifeq ($(BUILD_PRX),1)
ifneq ($(TARGET_LIB),)
$(error TARGET_LIB should not be defined when building a prx)
else
FINAL_TARGET = $(TARGET).prx
endif
else
ifneq ($(TARGET_LIB),)
FINAL_TARGET = $(TARGET_LIB)
else
FINAL_TARGET = $(TARGET).elf
endif
endif

all: $(EXTRA_TARGETS) $(FINAL_TARGET)

kxploit: $(TARGET).elf $(PSP_EBOOT_SFO)
	mkdir -p "$(TARGET)"
	$(STRIP) $(TARGET).elf -o $(TARGET)/$(PSP_EBOOT)
	mkdir -p "$(TARGET)%"
	$(PACK_PBP) "$(TARGET)%/$(PSP_EBOOT)" $(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0) NULL $(PSP_EBOOT_PSAR)

$(TARGET).elf: $(OBJS) $(EXPORT_OBJ)
	$(LINK.c) $^ $(LIBS) -o $@
	$(FIXUP) $@

$(TARGET_LIB): $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

$(PSP_EBOOT_SFO):
	$(MKSFO) '$(PSP_EBOOT_TITLE)' $@

$(PSP_EBOOT): $(TARGET).elf $(PSP_EBOOT_SFO)
	$(STRIP) $(TARGET).elf -o $(TARGET)_strip.elf
	$(PACK_PBP) EBOOT.PBP $(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0)  $(TARGET)_strip.elf $(PSP_EBOOT_PSAR)
	-rm -f $(TARGET)_strip.elf

%.prx: %.elf
	psp-prxgen $< $@

%.c: %.exp
	psp-build-exports -b $< > $@

clean:
	-rm -f $(FINAL_TARGET) $(EXTRA_CLEAN) $(OBJS) $(PSP_EBOOT_SFO) $(PSP_EBOOT) $(EXTRA_TARGETS)
