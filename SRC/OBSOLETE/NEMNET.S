*********************************************************************************
*										*
*	Low level part of MagicNet MIF NEx000 driver for my ACSI-ISA interface	*
* 	and the Cartridge Port interface developed by Lyndon Amsdon and me.	*
*	Copyright 2001-2002 Dr. Thomas Redelberger				*
*	Use it under the terms of the GNU General Public License		*
*	(See file COPYING.TXT)							*
*										*
*										*
* Features:									*
*	Packets are read/written directly from NEx000 to a MagiCNet datagram	*
*	without	further (double) buffering and memcpy				*
* Limitations:									*
*										*
* Credits:									*
*										*
* Tabsize 8, developed with DEVPAC assembler 2.0.				*
*										*
*********************************************************************************
* $Id$
*
* descriptions:
*


*
* development switches
*

		INCLUDE	DEVSWIT.I

*
* code generation options
*
***		OPT	D+		; switch on symbol info
		OPT	O+		; optimize 0(an) to (an)
		OPT	W-		; warnings off
		OPT	M+		; macro expansion in listings on


* entry points and references in this module
		XDEF	NEInstInt	; (); chain us into the HZ200 interrupt
		XDEF	rtrvPckt	; (); get packet out of the card
		XDEF	myspl7		; (); disable ints.
		XDEF	myspl		; (); enable ints.

* references into ne.s
		XREF	ei_interrupt	; (); the interrupt

* references into enemnet.c
		XREF	netinfo		; pointer to structure; accessed from rtrvPckt
		XREF	if_ENE		; structure; accessed from rtrvPckt
		XREF	ENE_re_xmit	; (); tx queued packets

*
* system variables
*

*
* includes
*

		INCLUDE	UTI.I		; debugging and stack handling macros
		INCLUDE	BUS.I		; ACSI or Cartridge Port hardware macros
		INCLUDE	8390.I		; Symbols for 8390 chip registers



		SECTION	TEXT

*********************************************************************************
* NEInstInt installs our interrupt handler in the HZ200 interrupt.
* As the old vector gets called in a chain like manner, I have to disable all ints
* until the chain is ready
*********************************************************************************

HZ200TrapNo	EQU	$114/4 

NEInstInt	move	sr,-(sp)		; save int. level
		ori	#$700,sr		; disable all ints.

		pea	.myHZ200(pc)		; new vector
		move.w	#HZ200TrapNo,-(sp)
		move.w	#5,-(sp)		; Setexc
		trap	#13			; BIOS
		addq.l	#8,sp
		lea	.oldHZ200(pc),a0
		move.l	d0,(a0)			; save old vector

		move	(sp)+,sr		; restore ints.
		rts


		DC.B	"XBRA"
		DC.L	"TREN"
.oldHZ200	DC.L	0


*********************************************************************************
* .myHZ200 is just a wrapper around ei_interrupt as ei_interrupt does not
* preserve d0-d2 and a0-a1
*********************************************************************************

RmH		REG	d0-d5/a0-a4

.myHZ200	movem.l	RmH,-(sp)

		jsr	ei_interrupt

		jsr	ENE_re_xmit

		movem.l	(sp)+,RmH

* _branch_ to the old vector without using a register
* the old vector will do the rte finally
		move.l	.oldHZ200(pc),-(sp)
		rts



*********************************************************************************
*********************************************************************************

myspl7		move	sr,d0			; return int. level
		ori	#$700,sr		; disable all ints.
		rts


*********************************************************************************
*********************************************************************************

myspl		move	d0,sr			; restore ints.
		rts


*********************************************************************************
*********************************************************************************

*
* Because I use only a few functions and data full blown structure declarations
* are not warranted
*

* offsets in if_ENE

if_in_packets	EQU	130
if_in_errors	EQU	134
if_bpf		EQU	156

* offsets in NETINFO

buf_alloc	EQU	0
if_input	EQU	28
eth_remove_hdr	EQU	48
bpf_input	EQU	56



******** declarations for ethernet **********************************************
N8390Hdr	EQU	4		; the 8390 chip stores a 4 byte header preceeding the packet
NCRC		EQU	4		; 4 trailing CRC of a ethernet packet

		RSRESET
EthDst		RS.B	6		; Ethernet destination address (unused)
EthSrc		RS.B	6		; Ethernet source address (unused)
EthType		RS.W	1		; Ethernet packet type
EthCTypeIP		EQU	$0800		; packet type IP
EthCTypeARP		EQU	$0806		; packet type ARP
EthCtypeRARP		EQU	$8035		; packet type reverse ARP (unused)
EthCTypeIPARPHi		EQU	$08		; Hi byte for both IP and ARP
EthCTypeIPLo		EQU	$00		; Lo byte IP
EthCTypeARPLo		EQU	$06		; Lo byte ARP
EthN		EQU	__RS
***NArpPkt		EQU	64-EthN		; length of Arp packet without ethernet header
					; but with padding and ethernet CRC



******** rtrvPckt ***************************************************************
* This function is called only (statically) once from ei_receive in NE.S; it does 
* special parameter passing different from Turbo-C/Pure-C or cdecl:
*
*
* This function effects to get the packet out of the NEx000 card into the ST RAM
*
* in:	RrxJnk8990	(d1) Junk 8390 header occured from 8990 chip
*	RrxPktLen	(d2) The raw ethernet packet length
*	RrxReadPg	(d4) Page where the packet starts
*
* out:
*	d0.l:	  0=OK
*		Errors:
*		 -1=cannot allocate buf
*		 -2=
*		 -3=
*		 -4=
*		for all errors the packet just gets dropped
*
*	MagicNet statistics get updated
*	dev_* statistics do not get updated because dev_* is opaque to this module
*
* used
*	RxBUS,RyBUS,RcBUS,RdBUS
*
* changed:
*	We destroy d1,d2 although ei_receive appears to use them; they are in fact not
*	used any more after the call to rtrvPckt (this function).
*	We do not use a2 because it may get destroyed by each of the MNet functions
*	called here.
*	rtrvPckt function call
*	We save a4 because this used throughout ei_interrupt as RitDVS
*	We use a3 to point to the new buffer
*	Hence a2-a3 shall be saved by ei_receive to avoid to save them for each call of rtrvPckt
*

* local variables in registers
RrxJnk8990	EQUR	d1		; as in NE.S
RrxPktLen	EQUR	d2		; as in NE.S
RrxReadPg	EQUR	d4		; as in NE.S

RrpPktLen	EQUR	d3		; local copy of RrxPktLen

RrpBuf		EQUR	a3		; points to the new buffer
RrpNif		EQUR	a4		; points to the interface struct if_ENE

* local variables in memory
rxJnk8990	EQU	0		; because d1 gets destroyed

rtrvPckt
	IFGE	RXDEBPRT-999
		PrW	RrxPktLen
		PrA	<" RrxPktLen",13,10>
	ENDC
		move.l	RrpNif,-(sp)		; save used reg.
		move	RrpPktLen,-(sp)		; save used reg.
		move.b	RrxJnk8990,-(sp)	; copy to memory (2 bytes pushed!)
		move	RrxPktLen,RrpPktLen	; copy because d2 gets destroyed

		cmp	#32+NCRC,RrpPktLen	; check packet must not be smaller
		bcs	.err

		lea	if_ENE,RrpNif

* do not know why these extra 50 and 100 are needed
		move	#1,-(sp)		; arg3: mode BUF_ATOMIC
		move.l	#50,-(sp)		; arg2: padding front
		moveq	#100,d0			; more length
		add	RrpPktLen,d0		; .w but cannot overflow...
		move.l	d0,-(sp)		; arg1: length
		move.l	netinfo,a0
		movea.l	buf_alloc(a0),a0
		jsr	(a0)
		lea	10(sp),sp		; pop args

		movea.l	d0,RrpBuf		; buffer
		tst.l	d0
		beq	.err

		moveq	#0,d0
		move	RrpPktLen,d0		; unsigned extend
		add.l	d0,8(RrpBuf)		; move b->dend past packet

* we get the packet out of the card

		move	RrpPktLen,d0		; just full lenght does not hurt (inc. CRC)
		putBUS	d0,EN0_RCNTLO
		lsr.w	#8,d0
		putBUS	d0,EN0_RCNTHI
* we need to skip the 8390 header
		putBUS	#N8390Hdr,EN0_RSARLO	; skip 8390 header (4)
		putBUS	RrxReadPg,EN0_RSARHI	; we start at this page
		putBUS	#E8390_RREAD+E8390_START,E8390_CMD	; go

* note that the data is shifted by one byte in case of a junk header, we need to do one more read
		tst.b	rxJnk8990(sp)
		beq.b	.c1
		getBUS	NE_DATAPORT,d0		; dummy read

.c1		move	RrpPktLen,d1
		sub	#2,d1			; two bytes less we will eat first

		movea.l	4(RrpBuf),a0		; b->dstart
		getBUS	NE_DATAPORT,d0		; get first byte
		move.b	d0,(a0)+
		getMore	NE_DATAPORT,d0		; get second byte
		move.b	d0,(a0)+

		NE2RAM	a0,d1			; both regs get destroyed!
		putBUS	#E8390_NODMA+E8390_START,E8390_CMD	; complete remote DMA
		putBUS	#ENISR_RDC,EN0_ISR	; reset remote DMA ready bit

* pass packet to upper layers

* input filtering?
		move.l	if_bpf(RrpNif),d0
		beq.b	.c2

		move.l	RrpBuf,-(sp)		; arg2: buf 
		move.l	RrpNif,-(sp)		; arg1: nif
		move.l	netinfo,a0
		movea.l	bpf_input(a0),a0
		jsr	(a0)
		addq.l	#8,sp			; pop args

* remove ethernet header
.c2		move.l	RrpBuf,-(sp)		; arg: buf
		move.l	netinfo,a0
		movea.l	eth_remove_hdr(a0),a0	; returns packet type
		jsr	(a0)
		addq.l	#4,sp			; pop arg

		move	d0,-(sp)		; arg4: packet type
		moveq.l	#0,d0
		move.l	d0,-(sp)		; arg3: delay
		move.l	RrpBuf,-(sp)		; arg2: buf
		move.l	RrpNif,-(sp)		; arg1: nif
		move.l	netinfo,a0
		movea.l	if_input(a0),a0
		jsr	(a0)
		lea	14(sp),sp		; pop args
		tst	d0			; error?
		bne.b	.err

		addq.l	#1,if_in_packets(RrpNif)	; statistics
		moveq.l	#0,d0			; signal succes & fall thru to exit

.exit		addq.l	#2,sp			; pop local var.
		move	(sp)+,RrpPktLen		; restore used reg.
		move.l	(sp)+,RrpNif		; restore used reg.
		rts


.err		moveq.l	#-1,d0			; signal error
		addq.l	#1,if_in_errors(RrpNif)	; statistics
		putBUS	#E8390_NODMA+E8390_START,E8390_CMD	; abort remote DMA
		getBUS	NE_DATAPORT,d1		; only this makes for a proper abort !!!
		putBUS	#ENISR_RDC,EN0_ISR	; reset remote DMA ready bit
		bra.b	.exit


******** end of nemnet.s ********************************************************

