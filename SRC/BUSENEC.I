*********************************************************************************
* Bus access macros for ST/TT/Falcon Cartridge Port for register base hardware	*
* Version for Lyndon Amsdon's and mine NE2000 interface hardware for the	*
* Cartridge Port. This hardware supports only fast "newer" NE2000 (clone) cards	*
*										*
* Version to use only CP address line and avoids /UDS	(slower, arrgh!)	*
*										*
*	Copyright 2001 Dr. Thomas Redelberger					*
*	Use it under the terms of the GNU General Public License		*
*	(See file COPYING.TXT)							*
*										*
* processor registers are used as follows:					*
*	d6.w:	will be used temporarily;  do not use it!			*
*	d7.w:	will be used temporarily;  do not use it!			*
*										*
*	a5:	will point to ROM3 at $FB0000;	do not change it!		*
*	a6:	will point to ROM4 at $FA0000;	do not change it!		*
*										*
*										*
* Tabsize 8, developed with DEVPAC assembler 2.0.				*
*										*
*********************************************************************************
* $Id: busenec.i 1.1 2002/03/24 13:53:06 Thomas Exp Thomas $
*

*
* manifest constants
*

BUGGY_HW	EQU	1		; if defined enables code to handle buggy hardware

*
* hardware addresses
*

rom4		EQU	$00fa0000	; ROM4 base address
rom3		EQU	$00fb0000	; ROM3 base address

*
* macros
*

lockBUS		MACRO
		moveq	#-1,d0			; preset error code
		tas	DVS+lcl_irqlock		; check for race about Cartrige Port and
		bne	.doNothing		; if somebody owns the bus we quit
		ENDM

lockBUSWait	MACRO
* there should be a timeout based on _hz_200 (and then branch to .doNothing)

.lt1\@		tas	DVS+lcl_irqlock		; check for race about Cartrige Port and
		beq.b	.lc1\@			; if somebody owns the bus we *wait*

.lwait\@	bsr	_appl_yield		; wait
		bra.b	.lt1\@

.lc1\@						; proceed
		ENDM


unlockBUS	MACRO
		sf	DVS+lcl_irqlock		; let other tasks access Cartridge Port
		ENDM

unlockBUSWait	MACRO
		sf	DVS+lcl_irqlock		; let other tasks access Cartridge Port
		ENDM


RxBUS		EQUR	d6
RyBUS		EQUR	d7
RcBUS		EQUR	a5
RdBUS		EQUR	a6


ldBUSRegs	MACRO				; for faster access to hardware
		lea	rom3,RcBUS
		lea	rom4,RdBUS
		ENDM


putBUS		MACRO
		move.w	#(\2)<<8,RyBUS		; move ISA address to bits 8-15
		move.w	RyBUS,RxBUS		; get a copy
		move.b	\1,RyBUS		; merge in data
		add.w	RyBUS,RyBUS		; shift up one bit to avoid UDS/LDS ***
		tst.b	0(RcBUS,RyBUS.w)	; write by reading
		ENDM


putMore		MACRO
		move.w	RxBUS,RyBUS		; move ISA address to bits 8-15 ***
		move.b	\1,RyBUS		; merge in data
		add.w	RyBUS,RyBUS		; shift up one bit to avoid UDS/LDS ***
		tst.b	0(RcBUS,RyBUS.w)	; write by reading
		ENDM


getBUS		MACRO
		move.b	(\1)<<9(RdBUS),\2	; read from CP ***
		ENDM


getMore		MACRO
		move.b	(\1)<<9(RdBUS),\2	; read from CP ***
		ENDM

*
* macro to deselect an interface
*
deselBUS	MACRO
* empty as the cartridge port does not need deselecting
		ENDM



******** RAM2NE *****************************************************************
* This is a macro for speed
* put # of bytes in register arg2 to NE from location pointed to by arg1.
*
* in:	arg1	address register source address
*	arg2	data register (w) # of bytes to put; must not be d0!
*
* both registers plus d0 get destroyed.
* Assembler inst. REPT does not work inside a macro, we repeate explicitly
	
RAM2NE		MACRO
		ext.l	\2			; clear upper word + test for 0
		subq.w	#2,\2			; first two puts are outside loop
		ble	.doNothing\@
		putBUS	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT

* put the packet; optimized for speed, we do 8 bytes at once
		ror.l	#3,\2			; store remainder in upper word
		bra.b	.Rb1\@

.Rt1\@		putMore	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT
		putMore	(\1)+,NE_DATAPORT
.Rb1\@		dbra	\2,.Rt1\@

		clr.w	\2			; prepare for remainder
		rol.l	#3,\2			; restore remainder bits
		bra.b	.Rb2\@

.Rt2\@		putMore	(\1)+,NE_DATAPORT	; put the remaining bytes
.Rb2\@		dbra	\2,.Rt2\@

.doNothing\@
		ENDM



******** NE2RAM *****************************************************************
* This is a macro for speed
* get # of bytes in register arg2 to RAM location pointed to by arg1.
* *** A first getBUS MUST already have been done! ***
*
* in:	arg1	address register destination address
*	arg2	data register (w) # of bytes to get
*
* Assembler inst. REPT does not work inside a macro, we repeate explicitly

OPTREAD		EQU	1

NE2RAM		MACRO
	IFNE	OPTREAD
		lea	NE_DATAPORT<<9(RdBUS),RdBUS	; adjust register to point to data bus ***
	ENDC

* get the Packet; optimized for speed, we do 8 bytes at once.
		ext.l	\2			; clear upper word
		ror.l	#3,\2			; store remainder in upper word
		bra.b	.Nb1\@

.Nt1\@
	IFNE	OPTREAD
		move.b	(RdBUS),(\1)+
		move.b	(RdBUS),(\1)+
		move.b	(RdBUS),(\1)+
		move.b	(RdBUS),(\1)+
		move.b	(RdBUS),(\1)+
		move.b	(RdBUS),(\1)+
		move.b	(RdBUS),(\1)+
		move.b	(RdBUS),(\1)+
	ELSEIF
		getMore	NE_DATAPORT,(\1)+
		getMore	NE_DATAPORT,(\1)+
		getMore	NE_DATAPORT,(\1)+
		getMore	NE_DATAPORT,(\1)+
		getMore	NE_DATAPORT,(\1)+
		getMore	NE_DATAPORT,(\1)+
		getMore	NE_DATAPORT,(\1)+
		getMore	NE_DATAPORT,(\1)+
	ENDC
.Nb1\@		dbra	\2,.Nt1\@

		clr.w	\2				; prepare for remainder
		rol.l	#3,\2				; restore remainder bits
		bra.b	.Nb2\@

.Nt2\@
	IFNE	OPTREAD
		move.b	(RdBUS),(\1)+
	ELSEIF
		getMore	NE_DATAPORT,(\1)+		; get the remaining bytes
	ENDC
.Nb2\@		dbra	\2,.Nt2\@

	IFNE	OPTREAD
		lea	-(NE_DATAPORT<<9)(RdBUS),RdBUS	; restore register ***
	ENDC
		ENDM



*********************************************************************************

