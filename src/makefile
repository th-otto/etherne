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
CC=bin\tcc.ttp
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


OBJS_STNG = enestng.o nestng.o ne.o uti.o
OBJS_MNET = enemnet.o nemnet.o ne.o uti.o
OBJS_HT3  = ht3ene.o ne.o uti.o
OBJS = $(OBJS_STNG) $(OBJS_MNET) $(OBJS_HT3)


INC_STNG= inc_stng
INC_MNET= inc_mnet

default:	all

drivers:	etherne.stx etherne.mif


etherne.stx:	$(OBJS_STNG)
	$(LINK) $(LFLAGS) -s=0 -o=$@ $(OBJS_STNG) $(LIBDIR)\tctoslib.lib


etherne.mif:	$(OBJS_MNET)
	$(LINK) $(LFLAGS) -s=0 -o=$@ $(OBJS_MNET)


ht1ene.tos:	ht1ene.s uti.i bus.i 8390.i uti.s
	$(AS)	$< -oht1ene.o
	$(LINK) $(LFLAGS) -s=0 -o=$@ ht1ene.o

ht2ene.tos:	ht2ene.s uti.i bus.i uti.s
	$(AS)	$< -oht2ene.o
	$(LINK) $(LFLAGS) -s=0 -o=$@ ht2ene.o


ht3ene.tos:	$(OBJS_HT3)
	$(LINK) $(LFLAGS) -s=256 -o=$@ $(OBJS_HT3)


ht4enec.tos:	ht4enec.s uti.i busenec.i uti.s
	$(AS)	$< -oht4enec.o
	$(LINK) $(LFLAGS) -s=0 -o=$@ ht4enec.o


enemnet.o:	$(INC_MNET)\buf.i $(INC_MNET)\if.i $(INC_MNET)\netinfo.i uti.i

enestng.o:	$(INC_STNG)\transprt.h $(INC_STNG)\port.h ne.h uti.h devswit.h

nestng.o nemnet.o ne.o ht3ene.o:	devswit.i uti.i bus.i 8390.i

nemnet.o:	$(INC_MNET)\buf.i $(INC_MNET)\if.i $(INC_MNET)\netinfo.i

ne.o:		netdev.i

uti.o:		uti.i uti.h


#################################################################

clean:
	$(RM) *.tos *.stx *.mif *.o bus.i


package:
	cd ..; $(ZIP) -ar etherne.zip "*.* ~*.o ~*.ppt"


all:	eneaf eneas enec enec3 enem eneh enex


#################################################################
# do the various IP stack &  hardware configurations/combinations

eneaf:
	$(CP)	buseneaf.i bus.i	# replace the bus hardware specific include file
	$(MAKE)	drivers ht1ene.tos ht2ene.tos ht3ene.tos
	$(MV)	etherne.mif ..\bin\eneaf.mif	# save these drivers
	$(MV)	etherne.stx ..\bin\eneaf.stx
	$(MV)	ht1ene.tos ..\bin\ht1eneaf.tos
	$(MV)	ht2ene.tos ..\bin\ht2eneaf.tos
	$(MV)	ht3ene.tos ..\bin\ht3eneaf.tos


eneas:
	$(CP)	buseneas.i bus.i
	$(MAKE)	drivers ht1ene.tos ht2ene.tos ht3ene.tos
	$(MV)	etherne.mif ..\bin\eneas.mif
	$(MV)	etherne.stx ..\bin\eneas.stx
	$(MV)	ht1ene.tos ..\bin\ht1eneas.tos
	$(MV)	ht2ene.tos ..\bin\ht2eneas.tos
	$(MV)	ht3ene.tos ..\bin\ht3eneas.tos


enec:
	$(CP)	busenec.i bus.i
	$(MAKE)	drivers ht1ene.tos ht2ene.tos ht3ene.tos ht4enec.tos
	$(MV)	etherne.mif ..\bin\enec.mif
	$(MV)	etherne.stx ..\bin\enec.stx
	$(MV)	ht1ene.tos ..\bin\ht1enec.tos
	$(MV)	ht2ene.tos ..\bin\ht2enec.tos
	$(MV)	ht3ene.tos ..\bin\ht3enec.tos
	$(MV)	ht4enec.tos ..\bin\ht4enec.tos


enec3:
	$(CP)	busenec3.i bus.i
	$(MAKE)	drivers
	$(MV)	etherne.mif ..\bin\enec3.mif
	$(MV)	etherne.stx ..\bin\enec3.stx


enem:
	$(CP)	busenem.i bus.i
	$(MAKE)	drivers ht2ene.tos
	$(MV)	etherne.mif ..\bin\enem.mif
	$(MV)	etherne.stx ..\bin\enem.stx
	$(MV)	ht2ene.tos ..\bin\ht2enem.tos


eneh:
	$(CP)	buseneh.i bus.i
	$(MAKE)	drivers ht2ene.tos
	$(MV)	etherne.mif ..\bin\eneh.mif
	$(MV)	etherne.stx ..\bin\eneh.stx
	$(MV)	ht2ene.tos ..\bin\ht2eneh.tos

enex:
	$(CP)	busenex.i bus.i
	$(MAKE)	drivers ht2ene.tos
	$(MV)	etherne.mif ..\bin\enex.mif
	$(MV)	etherne.stx ..\bin\enex.stx
	$(MV)	ht2ene.tos ..\bin\ht2enex.tos

# development driver

devlopment:
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
	$(AS) $< $(AFLAGS)

.c.o:
	$(CC) $(CFLAGS) $<

#### end of makefile ############################################

