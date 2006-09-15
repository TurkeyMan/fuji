#BUILD = Release
#PLATFORM = LINUX
TARGET = ConvertDlg

ifeq ($(PLATFORM),)
#$(error $$(PLATFORM) is undefined.
PLATFORM = LINUX
endif
ifeq ($(BUILD),)
#$(error $$(BUILD) is undefined.
BUILD = Release
endif

OBJS = main.o

OBJS := $(OBJS) ../../Fuji/Source/MFCallstack.o
OBJS := $(OBJS) ../../Fuji/Source/MFDebug.o
OBJS := $(OBJS) ../../Fuji/Source/MFHeap.o
OBJS := $(OBJS) ../../Fuji/Source/MFIni.o
OBJS := $(OBJS) ../../Fuji/Source/MFMatrix.o
OBJS := $(OBJS) ../../Fuji/Source/MFString.o
OBJS := $(OBJS) ../../Fuji/Source/MFStringCache.o
OBJS := $(OBJS) ../../Fuji/Source/MFTextureUtil.o
OBJS := $(OBJS) ../../Fuji/Source/MFVector.o
OBJS := $(OBJS) ../../Fuji/Source/Util.o
OBJS := $(OBJS) ../../Fuji/Util/F3D.o
OBJS := $(OBJS) ../../Fuji/Null/MFHeap_Null.o

ifeq ($(PLATFORM),LINUX)
OBJS := $(OBJS) ../../Fuji/Linux/MFDebug_Linux.o
endif
ifeq ($(PLATFORM),OSX)
OBJS := $(OBJS) ../../Fuji/OSX/MFDebug_OSX.o
endif
ifeq ($(PLATFORM),PC)
OBJS := $(OBJS) ../../Fuji/PC/MFDebug_PC.o
endif

OBJS := $(OBJS) ../../Fuji/Middleware/zlib/adler32.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/compress.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/crc32.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/deflate.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/gzio.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/infback.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/inffast.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/inflate.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/inftrees.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/trees.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/uncompr.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/zutil.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/minizip/ioapi.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/minizip/mztools.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/minizip/unzip.o
OBJS := $(OBJS) ../../Fuji/Middleware/zlib/minizip/zip.o


INCDIR = ../../Fuji/Include ../../Fuji/Source ../../Fuji/Util ../../Fuji/Middleware ../../Fuji/Middleware/zlib
CFLAGS = -Wall -D_FUJI_UTIL -D_$(PLATFORM)

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

#LIBDIR = ../../Fuji/Lib
LDFLAGS =
#LIBS = -lFuji_$(CONFIG)

LIBS := -lc -lm -lstdc++

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
LDFLAGS   := $(addprefix -L,$(LIBDIR)) $(LDFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)

FINAL_TARGET = $(TARGET)

all: $(EXTRA_TARGETS) $(FINAL_TARGET)

$(TARGET): $(OBJS) $(EXPORT_OBJ)
	$(LINK.c) $^ $(LIBS) -o $@

clean:
	-rm -f $(FINAL_TARGET) $(EXTRA_CLEAN) $(OBJS) $(EXTRA_TARGETS)
