*********************************************************************************
* Bus access macros for ST/TT/Falcon Cartridge Port for register base hardware	*
* Version for Lyndon Amsdon's and mine NE2000 interface hardware for the	*
* Cartridge Port. This hardware supports only fast "newer" NE2000 (clone) cards	*
*										*
* Version to use only CP address line and avoids /UDS	(slower, arrgh!)	*
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

CPU020		EQU 0

		.INCLUDE "busenec.i"
