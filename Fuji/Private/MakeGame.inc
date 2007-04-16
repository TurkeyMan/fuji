ifeq ($(TARGET),)
TARGET = $(TARGETNAME)
endif

ifeq ($(PLATFORM),)
#$(error $$(PLATFORM) is undefined.
PLATFORM = LINUX
endif
ifeq ($(BUILD),)
#$(error $$(BUILD) is undefined.
BUILD = Release
endif

ifeq ($(BUILD),Debug)
CFLAGS := $(CFLAGS) -D_DEBUG -O0
endif
ifeq ($(BUILD),Release)
CFLAGS := $(CFLAGS) -D_RELEASE -O1
endif
ifeq ($(BUILD),Retail)
CFLAGS := $(CFLAGS) -D_RETAIL -O3
endif

CONFIG = $(BUILD)_$(PLATFORM)
TARGET := $(TARGET)_$(CONFIG)

INTDIR = Build/$(CONFIG)

FUJILIB = $(FUJIPATH)/Lib/libFuji_$(CONFIG).a

INCDIR = $(FUJIPATH)/Include Source
CFLAGS = -Wall -D_$(PLATFORM)
CXXFLAGS = -fno-exceptions -fno-rtti
ASFLAGS =

LIBDIR =
LDFLAGS =
LIBS =


ifeq ($(PLATFORM),PSP)

ifneq ($(TARGET),)
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = $(TARGETNAME) $(PLATFORM)
endif

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

LIBS := $(LIBS) -lpspusb -lpspusbstor -lpsprtc -lpspgu -lm -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk -lstdc++ -lc -lpsppower -lpspaudio -lpspaudiolib -lpspwlan -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility -lpspuser -lpspkernel -logg -lvorbisfile -lvorbis

# Add in PSPSDK includes and libraries.
INCDIR   := $(INCDIR) $(PSPSDK)/include
LIBDIR   := $(LIBDIR) $(PSPSDK)/lib

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
ifeq ($(PLATFORM),PS2)

CC       = ee-gcc
CXX      = ee-g++
AS       = ee-as
LD       = ee-ld
AR       = ee-ar
RANLIB   = ee-ranlib
STRIP    = ee-strip

CFLAGS := $(CFLAGS) -G0

LIBS := $(LIBS) -ldraw -lgraph -lmath3d -lmf -lpacket -ldma -lstdc++ -lm -lg -lmc -lm -lpad -lc -lkernel

OBJS := $(OBJS) /usr/local/ps2dev/ps2sdk/ee/startup/crt0.o
CFLAGS := -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(CFLAGS)

# Add in PS2SDK includes and libraries.
INCDIR   := $(INCDIR) $(PS2SDK)/ee/include $(PS2SDK)/common/include
LIBDIR   := $(LIBDIR) $(PS2SDK)/ee/lib

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

LIBS := $(LIBS)

# Add in OGC includes and libraries.
INCDIR   := $(INCDIR) $(OGC)/include
LIBDIR   := $(LIBDIR) $(OGC)/lib

# endGC
endif

ifeq ($(PLATFORM),LINUX)

CC       = gcc
CXX      = g++
AS       = as
LD       = ld
AR       = ar
RANLIB   = ranlib
STRIP    = strip

LIBDIR   := $(LIBDIR) /usr/X11R6/lib
LIBS := -lc -lm -lstdc++ -lpthread -lGL -lGLU -lXxf86vm

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

LIBDIR   := $(LIBDIR) /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/lib
LIBS := -lc -lm -lstdc++ -lGL -lGLU -lXxf86vm -lX11 -lXext

# end OSX
endif

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
LDFLAGS   := $(addprefix -L,$(LIBDIR)) $(LDFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)


# more PSP stuff...
#--------------------------------------------------------------------------------------------
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
#--------------------------------------------------------------------------------------------


# Linux and OSX dont have file extensions...
ifeq ($(PLATFORM),LINUX)
FINAL_TARGET = $(TARGET)
else
ifeq ($(PLATFORM),OSX)
FINAL_TARGET = $(TARGET)
else

# consoles produce an .elf
ifeq ($(BUILD_PRX),1)
FINAL_TARGET = $(TARGET).prx
else
FINAL_TARGET = $(TARGET).elf
endif

# OSX/Linux
endif
endif


# General rules

OBJLIST = $(patsubst %,$(INTDIR)/%,$(OBJS))

all: makedirs $(EXTRA_TARGETS) $(FINAL_TARGET)

run: all
	./$(FINAL_TARGET)

$(INTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(INTDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(FUJILIB) $(OBJLIST) $(EXPORT_OBJ)
	$(LINK.c) -Wl,--start-group $^ $(LIBS) -Wl,--end-group -o $@

$(TARGET).elf: $(FUJILIB) $(OBJLIST) $(EXPORT_OBJ)
	$(LINK.c) -Wl,--start-group $^ $(LIBS) -Wl,--end-group -o $@
	$(FIXUP) $@

makedirs:
	@mkdir -p bin
	@mkdir -p $(INTDIR)

clean:
	-rm -f $(FINAL_TARGET) $(EXTRA_CLEAN) $(PSP_EBOOT_SFO) $(PSP_EBOOT) $(EXTRA_TARGETS)
	-rm -f -r $(INTDIR)

$(FUJILIB):
	make -C $(FUJIPATH) PLATFORM=$(PLATFORM) BUILD=$(BUILD)

deletelib:
	-rm -f $(FUJILIB)

deletedata:
	-rm -f Data/Data_$(PLATFORM).zip

deletebin:
	-rm -f $(FINAL_TARGET)

rebuild: deletelib deletedata deletebin all

fujitools:
	make -C $(FUJIPATH) PLATFORM=$(PLATFORM) tools

data: fujitools
	cd Data;./Build_Archive.sh $(PLATFORM)

# PSP specific rules...
#----------------------------------------------------------------------------------------------

kxploit: $(TARGET).elf $(PSP_EBOOT_SFO) $(PSP_EBOOT)
	mkdir -p "$(TARGETNAME)"
	$(STRIP) $(TARGET).elf -o $(TARGETNAME)/$(PSP_EBOOT)
	mkdir -p "$(TARGETNAME)%"
	$(PACK_PBP) "$(TARGETNAME)%/$(PSP_EBOOT)" $(PSP_EBOOT_SFO) $(PSP_EBOOT_ICON)  \
		$(PSP_EBOOT_ICON1) $(PSP_EBOOT_UNKPNG) $(PSP_EBOOT_PIC1)  \
		$(PSP_EBOOT_SND0) NULL $(PSP_EBOOT_PSAR)
	cp $(TARGET).elf $(TARGETNAME).elf

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
