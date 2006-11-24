# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#

EE_BIN = Hello_sample.elf
EE_OBJS = main.o 
EE_CXXFLAGS = -D_EE -D_PS2 -D_RELEASE -Iports/include/stlport -I../../Include -fno-exceptions -fno-rtti 
EE_LIBS = -lFuji_Release_PS2 -lstdc++ -ldraw -lgraph -lmath3d -lmf -lpacket -ldma   -lm -lg -lmc -lm 
EE_LDFLAGS = -L$(PS2SDK)/ports/lib -L../../Lib

all: $(EE_BIN)
	#ee-strip --strip-all $(EE_BIN)

clean:
	rm -f *.elf *.o *.a

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
