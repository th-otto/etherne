# Makefile for STinG, MagiCNet, MINTNet network drivers for NE2000
# on the EtherNEA or EtherNEC BUS interface
#
# Copyright 2002 Dr. Thomas Redelberger
# Use it under the terms of the GNU General Public License
# (See file COPYING.TXT)
#
#$Id: makefile 1.3 2002/06/08 16:38:20 Thomas Exp Thomas $
#

#### Pure-C 1.1
CC=bin\pcc.ttp
CFLAGS=-Iinclude

LINK=bin\plink.ttp
LFLAGS=
LIBDIR=lib

#### DEVPAC 2.0
#AS=bin\genst2.ttp
#### DEVPAC 3.10
#AS=bin\gen.ttp
#AFLAGS=-L2
### PASM
AS=bin\pasm.ttp
AFLAGS=-S

#### Mupfel
RM=rm -f
CP=cp
MV=mv
MAKE=make
ZIP=stzip
ECHO=echo


OBJS_STNG = startup.o enestng.o nestng.o ne.o uti.o
OBJS_MNET = enemnet.o nemnet.o ne.o uti.o
OBJS_HT3  = ht3ene.o ne.o uti.o
OBJS = $(OBJS_STNG) $(OBJS_MNET) $(OBJS_HT3)


INC_STNG= inc_stng
INC_MNET= inc_mnet

default:	all

drivers:	etherne.stx etherne.mif


etherne.stx:	$(OBJS_STNG)
	$(LINK) $(LFLAGS) -s=0 -o=$@ $(OBJS_STNG) $(LIBDIR)\pctoslib.lib


etherne.mif:	$(OBJS_MNET)
	$(LINK) $(LFLAGS) -s=0 -o=$@ $(OBJS_MNET)


ht1ene.tos:	ht1ene.o
	$(LINK) $(LFLAGS) -s=256 -o=$@ $<

ht2ene.tos:	ht2ene.o
	$(LINK) $(LFLAGS) -s=256 -o=$@ $<


ht3ene.tos:	$(OBJS_HT3)
	$(LINK) $(LFLAGS) -s=256 -o=$@ $(OBJS_HT3)


ht4ene.tos:	ht4ene.o uti.i bus.i uti.s
	$(LINK) $(LFLAGS) -s=256 -o=$@ $<


enemnet.o:	$(INC_MNET)\buf.i $(INC_MNET)\if.i $(INC_MNET)\netinfo.i uti.i

enestng.o:	$(INC_STNG)\transprt.h $(INC_STNG)\port.h ne.h uti.h devswit.h

nestng.o nemnet.o ne.o:	devswit.i uti.i bus.i 8390.i

nemnet.o:	bus.i $(INC_MNET)\buf.i $(INC_MNET)\if.i $(INC_MNET)\netinfo.i

ne.o:		netdev.i bus.i

uti.o:		uti.i uti.h

ht1ene.o:       ht1ene.s uti.i bus.i 8390.i uti.s 
ht2ene.o:       ht2ene.s uti.i bus.i uti.s 
ht3ene.o:       ht3ene.s uti.i bus.i 8390.i
ht4ene.o:       ht4ene.s uti.i bus.i uti.s 

#################################################################

clean:
	$(RM) *.tos *.stx *.mif *.o


package:
	cd ..; $(ZIP) -ar etherne.zip "*.* ~*.o ~*.ppt"


all:	eneaf eneas enec enec3 enem eneh enep2 enex


#################################################################
# do the various IP stack &  hardware configurations/combinations

eneaf:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=0 EXTRADEFS=-DACSI_SLOW_ACCESS=0 drivers ht1ene.tos ht2ene.tos ht3ene.tos
	$(MV)	etherne.mif ..\bin\eneaf.mif	# save these drivers
	$(MV)	etherne.stx ..\bin\eneaf.stx
	$(MV)	ht1ene.tos ..\bin\ht1eneaf.tos
	$(MV)	ht2ene.tos ..\bin\ht2eneaf.tos
	$(MV)	ht3ene.tos ..\bin\ht3eneaf.tos


eneas:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=0 EXTRADEFS=-DACSI_SLOW_ACCESS=1 drivers ht1ene.tos ht2ene.tos ht3ene.tos
	$(MV)	etherne.mif ..\bin\eneas.mif
	$(MV)	etherne.stx ..\bin\eneas.stx
	$(MV)	ht1ene.tos ..\bin\ht1eneas.tos
	$(MV)	ht2ene.tos ..\bin\ht2eneas.tos
	$(MV)	ht3ene.tos ..\bin\ht3eneas.tos


enec:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=1 EXTRADEFS=-DCPU020=0 drivers ht1ene.tos ht2ene.tos ht3ene.tos ht4ene.tos
	$(MV)	etherne.mif ..\bin\enec.mif
	$(MV)	etherne.stx ..\bin\enec.stx
	$(MV)	ht1ene.tos ..\bin\ht1enec.tos
	$(MV)	ht2ene.tos ..\bin\ht2enec.tos
	$(MV)	ht3ene.tos ..\bin\ht3enec.tos
	$(MV)	ht4ene.tos ..\bin\ht4enec.tos


enec3:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=1 EXTRADEFS=-DCPU020=1 drivers
	$(MV)	etherne.mif ..\bin\enec3.mif
	$(MV)	etherne.stx ..\bin\enec3.stx


enem:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=2 drivers ht2ene.tos
	$(MV)	etherne.mif ..\bin\enem.mif
	$(MV)	etherne.stx ..\bin\enem.stx
	$(MV)	ht2ene.tos ..\bin\ht2enem.tos


eneh:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=3 drivers ht2ene.tos
	$(MV)	etherne.mif ..\bin\eneh.mif
	$(MV)	etherne.stx ..\bin\eneh.stx
	$(MV)	ht2ene.tos ..\bin\ht2eneh.tos


enep2:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=6 drivers ht2ene.tos
	$(MV)	etherne.mif ..\bin\enep2.mif
	$(MV)	etherne.stx ..\bin\enep2.stx
	$(MV)	ht2ene.tos ..\bin\ht2enep2.tos


enex:
	$(RM) *.o *.tos *.stx *.mif
	$(MAKE)	DEFS=-DBUS=7 drivers ht2ene.tos
	$(MV)	etherne.mif ..\bin\enex.mif
	$(MV)	etherne.stx ..\bin\enex.stx
	$(MV)	ht2ene.tos ..\bin\ht2enex.tos


# development driver

development:
	$(RM) $(OBJS)
	$(CP) devswitd.i devswit.i	# replace the relevant include file
	$(MAKE)

# production driver

production:
	$(RM) $(OBJS)
	$(CP) devswitp.i devswit.i	# replace the relevant include file
	$(MAKE)


#### general stuff ##############################################

# order of file and option specific to DEVPAC assembler
.s.o:
	$(AS) $< $(AFLAGS) $(DEFS) $(EXTRADEFS)

.c.o:
	$(CC) $(CFLAGS) $<

#### end of makefile ############################################

