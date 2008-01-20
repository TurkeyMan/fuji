TARGET = ../../Fuji/Bin/ConvertFNT

ifeq ($(PLATFORM),)
#$(error $$(PLATFORM) is undefined.
PLATFORM = LINUX
endif
ifeq ($(BUILD),)
#$(error $$(BUILD) is undefined.
BUILD = Release
endif


OBJS = main.o


ifeq ($(FUJIPATH),)
FUJIPATH = ../../Fuji
endif

CONFIG = $(BUILD)_$(PLATFORM)
INTDIR = Build/$(CONFIG)

FUJILIB = $(FUJIPATH)/Lib/libFujiUtil_$(CONFIG).a

INCDIR = $(FUJIPATH)/Include $(FUJIPATH)/Include/Util $(FUJIPATH)/Source $(FUJIPATH)/Util
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

ifeq ($(PLATFORM),OSX)
INCDIR   :=$(INCDIR) . /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/include/
endif

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
LDFLAGS   := $(addprefix -L,$(LIBDIR)) $(LDFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)

FINAL_TARGET = $(TARGET)
OBJLIST = $(patsubst %,$(INTDIR)/%,$(OBJS))

all: makedirs $(EXTRA_TARGETS) $(FINAL_TARGET)

$(INTDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(INTDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(FUJILIB) $(OBJLIST) $(EXPORT_OBJ)
	$(LINK.c) -Wl,--start-group $^ $(LIBS) -Wl,--end-group -o $@

makedirs:
	@mkdir -p $(INTDIR)

clean:
	-rm -f $(FINAL_TARGET) $(EXTRA_CLEAN) $(EXTRA_TARGETS)
	-rm -f -r $(INTDIR)

$(FUJILIB):
	make -C $(FUJIPATH) PLATFORM=$(PLATFORM) BUILD=$(BUILD) util

deletelib:
	-rm -f $(FUJILIB)

rebuild: deletelib all
