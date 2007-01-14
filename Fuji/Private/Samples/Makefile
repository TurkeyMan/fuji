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
	make -C ModelTest/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD)

data:
	make -C 3DBox/ PLATFORM=$(PLATFORM) BUILD=$(BUILD) data
	make -C InputTest/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD) data
	make -C HelloWorld/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD) data
	make -C ModelTest/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD) data

clean:
	make -C 3DBox/ PLATFORM=$(PLATFORM) BUILD=$(BUILD) clean
	make -C InputTest/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD) clean
	make -C HelloWorld/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD) clean
	make -C ModelTest/ PLATFORM=$(PLATFORM)  BUILD=$(BUILD) clean
