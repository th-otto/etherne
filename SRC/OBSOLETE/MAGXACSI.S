*********************************************************************************
* ACSI bus locking functions available in MagiC					*
*	Copyright 2001 Dr. Thomas Redelberger					*
*	Use it under the terms of the GNU General Public License		*
*	(See file COPYING.TXT)							*
*										*
*										*
* Tabsize 8, developed with DEVPAC assembler 2.0.				*
*										*
*********************************************************************************
* $Id$
*

*
* code generation options
*
***		OPT	D+		; switch on symbol info
		OPT	O+		; optimize 0(an) to (an)
		OPT	W-		; warnings off
		OPT	M+		; macro expansion in listings on
*
* manifest constants
*


*
* addresses of system variables
*

flock		EQU	$43e		; (w) semaphor to lock floppy usage of DMA
pcookies	EQU	$5a0		; (l) points to cookie jar


*
* entry points and references in this module
*
		XDEF	instMagiC	; (); must be called in super mode
		XDEF	pDMA_begin	; ; pointer to function
		XDEF	pDMA_end	; ; "


		INCLUDE	UTI.I

		SECTION	TEXT



*********************************************************************************
* Simple semaphor routines to be used in a Single-TOS environment
* to be overriden under MagiC
*********************************************************************************

myDMABegin	tas	flock\w			; check, set flock and
		bne.b	.conflict		; if somebody owns the bus we flag
		tas	flock+1\w		; Holzauge: we do *both* bytes
		bne.b	.conflict
		moveq	#0,d0			; OK
		rts

.conflict	moveq	#-1,d0			; signal conflict
		rts


myDMAEnd	clr.w	flock\w			; allow floppy
		rts


**** instMagiC ******************************************************************
* tests presence of MagiC and overrides DMA semaphor functions 
*********************************************************************************

* structure pointed to by the MagX cookie
		RSRESET
mcConfig	RS.L	1	; unused here
mcDOSvars	RS.L	1	; unused here
mcAESvars	RS.L	1	; unused here
mcRes1		RS.L	1	; unused here
mcHdf		RS.L	1

* function table pointed to by mcHdf
		RSRESET
mhDMA_begin	RS.L	1	; (mcHdf points here)
mhDMA_end	RS.L	1	; 
mhDMA_wait	RS.L	1	; unused here
mhNCR_begin	RS.L	1	; unused here
mhNCR_end	RS.L	1	; unused here
mhNCR_wait	RS.L	1	; unused here



instMagiC	movea.l	pcookies\w,a0		; get pointer to jar (array)
		move.l	a0,d0
		beq.b	.quit			; if NULL pointer quit

.t1		move.l	(a0)+,d0		; get cookie tag
		beq.b	.quit			; last one
		movea.l	(a0)+,a1		; get cookie value
		cmp.l	#'MagX',d0		; MagiC cookie?
		bne.b	.t1			; no, again

		move.l	a1,d0			; value=pointer to MagX structure
		beq.b	.quit			; this should never happen

		move.l	mcHdf(a1),d0		; get hdf pointer
		beq.b	.quit			; this should never happen

		move.l	d0,a0
		move.l	mhDMA_begin(a0),pDMA_begin	; override my pointer to function
		move.l	mhDMA_end(a0),pDMA_end		; override my pointer to function
		PrS	.m1(pc)			; show success

.quit		rts


.m1		DC.B	8,8," MagiC support, ",0
		EVEN

* These point to the ACSI bus semaphor functions. They point to our functions.
* to be used when MagiC is not present. If MagiC is present they get overidden
pDMA_begin	DC.L	myDMABegin
pDMA_end	DC.L	myDMAEnd

*********************************************************************************
