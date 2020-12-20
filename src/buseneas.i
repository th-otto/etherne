*********************************************************************************
* Bus access macros for ST/TT ACSI bus for register base hardware		*
* Version for my ACSI-NE2000 interface hardware in slow mode (with the 74xx74	*
* D-flip flips for old NE1000 and older NE2000 (clone) cards			*
*										*
*	Copyright 2002 Dr. Thomas Redelberger					*
*	Use it under the terms of the GNU General Public License		*
*	(See file COPYING.TXT)							*
*										*
* Tabsize 8, developed with DEVPAC assembler 2.0.				*
*										*
*********************************************************************************

*
* manifest constants
*

	ACSI_SLOW_ACCESS equ 1

	.INCLUDE "busenea.i"
