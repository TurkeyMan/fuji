ifeq ($(PLATFORM),)
#$(error $$(PLATFORM) is undefined.
PLATFORM = LINUX
endif
ifeq ($(BUILD),)
#$(error $$(BUILD) is undefined.
BUILD = Release
endif

all:
	make -C 3DBox/ PLATFORM=$(PLATFORM) BUILD=$(BUILD)
	make -C InputTest/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD)
	make -C HelloWorld/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD)
