// BPTAnimationFileIO.h: interface for the BPTAnimationFileIO class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	This file was a huge rush job, so it has lots of places where the
//	process of loading / writing could be distilled into primitives
//	which would make the code easier to understand and in general better!
//
//	*******************************************************************
//	** ALL BLOCKS ARE ALIGNED TO 2 BYTEs (not reflected in the size) **
//	*******************************************************************
//
//	[aBPT]<size>										; root header block
//														
//		[CANV]<size>									; canvas size block
//			w:dword										; width of canvas
//			h:dword										; height of canvas
//
//		---- REV START (6/5/02 BPT) ------------------------------------------------------
//
//		[PRAT]<size>									; pixel ratio block
//			x:dword										; x pixel scale
//			y:dword										; y pixel scale
//
//		---- REV FINISH (6/5/02 BPT) -----------------------------------------------------
//														
//		[NAME]<size>									; [OPTIONAL] name of the showcase
//			byte(s)										; zero terminated string 
//
//		[nPAL]<size>									; a palette block
//			
//		---- REV START (6/5/02 BPT) ------------------------------------------------------
//			
//			[aPLI]<size>								; palette internal id
//				id:dword								; id used for pointer hookup
//
//			[NAME]<size>								; [OPTIONAL] name of the palette
//				byte(s)									; zero terminated string 
//			
//		---- REV FINISH (6/5/02 BPT) -----------------------------------------------------
//			
//		---- REV START (6/19/01 BPT) -----------------------------------------------------
//			
//			[PFGs]<size>								; slot flags
//				256 dwords (flags)						; flag values
//			
//			[SNTs]<size>								; slot notes
//			
//				[TEXT]<size>							; text block (multiple-instances)
//					char(s)								; text info for the slot
//
//			[FILE]<size>								; [OPTIONAL] relative filename path
//				bytes(s)								; filename
//			
//		---- REV FINISH (6/19/01 BPT) ----------------------------------------------------
//			
//			[RGBs]<size>								; 256 rgb values 
//				256 dwords (COLORREF's)					; colors
//
//		(*****) REV START (6/5/02 BPT) ---------------------------------------------------
//
//		[nPLS]<size>									; nested palette block
//
//			[nPAL]<size>								; one or more palette's
//				*** SEE [nPAL] DESCRIPTION ABOVE ***
//
//		(*****) REV FINISH (6/5/02 BPT) -------------------------------------------------
//
//		---- REV START (5/22/01 BPT) -----------------------------------------------------
//
//		[NXID]<size>									; next showcase id
//			id:dword									; next id to be given out
//
//		---- REV FINISH (5/22/01 BPT) ----------------------------------------------------
//
//		---------------------------------------------------------------------------------
//														
//		(*****) AS OF 5/22/01 THIS IS WHERE THE VISUAL ELEMENTS ARE STORED (*****)
//														
//		[nVLS]<size>									; nested visuals block (*****)
//
//			[aVEL]<size>								; visual element (multi-instance)
//
//		---- REV START (10/8/02 BPT) -----------------------------------------------------
//
//				[nVRS]<size>							; [OPTIONAL] nested visual references
//
//					[aVRF]<size>						; a visual reference block (see below)
//						(see below)
//
//		---- REV FINISH (10/8/02 BPT) ---------------------------------------------------
//
//		---- REV START (6/5/01 BPT) -----------------------------------------------------
//
//				[FILE]<size>							; [OPTIONAL] relative filename path
//					byte(s)								; filename...
//
//		---- REV FINISH (6/5/01 BPT) -----------------------------------------------------
//
//		---- REV START (5/22/01 BPT) -----------------------------------------------------
//
//				[aVEI]<size>							; visual element internal id
//					id:dword							; id used for pointer hookup
//
//		---- REV FINISH (5/22/01 BPT) ----------------------------------------------------
//
//				[SCID]<size>							; showcase relative id
//					id:dword							; showcase relative id
//
//				[DPTH]<size>							; stored bit depth
//					value:dword							; ""
//
//				[RECT]<size>							; initial creation rectangle
//					left:dword							; left
//					top:dword							; top
//					right:dword							; right
//					bottom:dword						; bottom
//
//				[aS??]<size>							; ?? = (32*,16,08) compressed image block
//	
//					[WWHH]<size>						; size block
//						w:dword							; width
//						h:dword							; height
//
//		---- REV START (5/22/01 BPT) -----------------------------------------------------
//
//					[ESIZ]<size>						; estimated data size
//						size:dword						; used for quick & dirty compare
//
//		---- REV FINISH (5/22/01 BPT) -----------------------------------------------------
//
//					[nSXD]<size>						; nested (h) times
//					
//						[aSCD]<size>					; \ SRL control data block / PAIR (multi-instance)
//							count:dword					; number of control stream elements
//							data:byte(s)				; control stream elements
//						[aSVD[<size>					; / SRL value data block   \ PAIR
//							count:dword					; number of value elements
//							data:??(s)					; value stream elements
//
//		(*****) AS OF 5/22/01 THIS IS WHERE THE VISUAL ELEMENTS ARE STORED (*****)
//
//		-----------------------------------------------------------------------
//
//		[nAMS]<size>									; Nested animations root
//														
//			[aANM]<size>								; an animation block (multi-instance)
//														
//				[aAHD]<size>							; animation header
//					frames:dword						; frame count
//					layers:dword						; layer count
//														
//				[NAME]<size>							; [OPTIONAL] name of animation
//					byte(s)								; ...
//
//				[aBKD]<size>							; [OPTIONAL] animation backdrop
//
//					[aVEL]<size>						; a visual element embedded here
//														
//		 		[nTXT]<size>							; [OPTIONAL] nested text notes
//														
//		 			[TEXT]<size>						; text block (multi-instance)
//						char(s)							; ...
//
//				[RATE]<size>							; frame rate
//					delay:dword							; millisecond delay
//														
//				[LNK1]<size>							; [OPTIONAL] link point 1
//					x:dword								; x value
//					y:dword								; y value
//														
//		---- REV START (6/15/01 BPT) -----------------------------------------------------
//														
//				[nPAL]<size>							; [OPTIONAL] palette block
//					*** SEE [nPAL] DESCRIPTION ABOVE ***
//														
//		---- REV START (6/5/02 BPT) -------------------------------------------------------
//
//				[aPLI]<size>							; palette internal id
//					id:dword							; id used for pointer hookup
//
//		---- REV FINISH (6/5/02 BPT) -------------------------------------------------------
//
//				[aPO1]<size>							; [OPTIONAL] policy flags (set 1)
//					flags:dword							; 32 bits of pure fun!
//														
//		---- REV FINISH (6/15/01 BPT) -----------------------------------------------------
//														
//		---- REV START (6/27/01 BPT) -----------------------------------------------------
//
//				[OUTA]<size>							; outline color A
//					index:dword							; index for outline color
//
//				[OUTB]<size>							; outline color B
//					index:dword							; index for outline color
//
//		---- REV FINISH (6/27/01 BPT) -----------------------------------------------------
//														
//				[nLIS]<size>							; Nested layer information root
//														
//					[aLII]<size>						; Layer information block (multi-instance)
//														
//						[NAME]<size>					; [OPTIONAL] name of layer
//							byte(s)						; ...
//
//						[FLGS]<size>					; [OPTIONAL] flags for the layer
//							flags:dword					; ...
//														
//		---- REV START (6/15/01 BPT) -----------------------------------------------------
//														
//						[nPAL]<size>					; [OPTIONAL] palette block
//							*** SEE [nPAL] DESCRIPTION ABOVE ***
//														
//		---- REV FINISH (6/15/01 BPT) -----------------------------------------------------
//														
//		---- REV START (6/5/02 BPT) -------------------------------------------------------
//
//						[aPLI]<size>					; palette internal id
//							id:dword					; id used for pointer hookup
//
//		---- REV FINISH (6/5/02 BPT) -------------------------------------------------------
//
//						[nTXT]<size>					; [OPTIONAL] nested text notes
//														
//							[TEXT]<size>				; text block (multi-instance)
//								char(s)					; ...
//
//				---- REV START (2/13/01 BPT) ----------------------------------------------------
//
//				[nNSD]<size>							; Nested named spot data
//
//					[aNSP]<size>						; a named spot
//
//						[aNSI]<size>					; name spot internal id
//							id:dword					; id used for pointer hookup
//
//						[NAME]<size>					; name of the spot
//							chars(s)					; ...
//
//						[XXYY]<size>					; xy location block
// 							x:dword						; x value
//							y:dword						; y value
//
//						[nTXT]<size>					; [OPTIONAL] nested text notes
//														
//							[TEXT]<size>				; text block (multi-instance)
//								char(s)					; ...
//														
//				---- REV FINISH (2/13/01 BPT) ---------------------------------------------------
//
//				[nFMS]<size>							; Nested frames root
//														
//					[aFRM]<size>						; Frame block (multi-instance)
//														
//						[aFHD]<size>					; frame header block
//														
//							layers:dword				; layer count used for error checking
//														
//						[nTXT]<size>					; [OPTIONAL] nested text notes
//														
//							[TEXT]<size>				; text block (multi-instance)
//								char(s)					; ...
//														
//				---- REV START (2/13/01 BPT) ----------------------------------------------------
//														
//						[nNSA]<size>					; Nested named spot adjustments
//
//							[aNSA]<size>				; Name spot adjustment
//
//								[aNSI]<size>			; name spot internal id
//									id:dword			; id used for pointer hookup
//
//								[DXDY]<size>			; xy 'offset' block
// 									dx:dword   			; dx value
//									dy:dword   			; dy value
//
//								[nTXT]<size>			; [OPTIONAL] nested text notes
//														
//									[TEXT]<size>		; text block (multi-instance)
//										char(s)			; ...
//														
//				---- REV FINISH (2/13/01 BPT) ---------------------------------------------------
//														
//						[LNKS]<size>					; [OPTIONAL] nested link root
//														
//							[LINK]<size>				; link block (multi-instance)
//								x:dword					; x value
//								y:dword					; y value
//														
//						[LNK1]<size>					; [OPTIONAL] link point 1
// 							x:dword						; x value
// 							y:dword						; y value
//														
//						[LNK2]<size>					; [OPTIONAL] link point 2
// 							x:dword						; x value
// 							y:dword						; y value
//														
//						[LNK3]<size>					; [OPTIONAL] link point 3
// 							x:dword						; x value
// 							y:dword						; y value
//														
//						[LNK4]<size>					; [OPTIONAL] link point 4
// 							x:dword						; x value
// 							y:dword						; y value
//														
//						[nLRS]<size>					; nested layer root
//														
//							[aLYR]<size>				; layer block (multi-instance)
//
//								[nTXT]<size>			; [OPTIONAL] nested text notes
//																
//									[TEXT]<size>		; text block (multi-instance)
//										char(s)			; ...
//										
//								[aVRF]<size>			; a visual reference block
//
//									[XXYY]<size>		; xy location block
//										x:dword			; x value
//										y:dword			; y value
//
//									[aVEL]<size>		; EMBEDED visual element (*****)
//										(*****)			; see above definition
//
//			---- REV START (5/22/01 BPT) -----------------------------------------------------
//
//									[aVEI]<size>		; visual element internal id
//										id:dword		; id used for pointer hookup
//
//			---- REV FINISH (5/22/01 BPT) ----------------------------------------------------
//
//	===========================================================================
//
//	SRL control BYTE format
//		
//		76543210
//		xxxxxxx1 == (0x01) skip of (xxxxxxx + 1) 
//		xxxxxx10 == (0x02) run of (xxxxxx + 1) of the next value in the value stream
//		xxxxxx00 == (0x00) literal run (xxxxxx + 1) of next n values from the stream
//
//	SRL value format varies on storage type (8,16,32*, etc...)
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTANIMATIONFILEIO_H__B851D6A5_2E4A_4369_A11A_5D8A1DD409F2__INCLUDED_)
#define AFX_BPTANIMATIONFILEIO_H__B851D6A5_2E4A_4369_A11A_5D8A1DD409F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

// root block

#define ID_aBPT		MAKE_CHUNK_ID( 'a', 'B', 'P', 'T' )

// common blocks
// ----------------------------------------------------------------------------

#define ID_CANV		MAKE_CHUNK_ID( 'C', 'A', 'N', 'V' )
#define ID_NAME		MAKE_CHUNK_ID( 'N', 'A', 'M', 'E' )
#define ID_XYWH		MAKE_CHUNK_ID( 'X', 'Y', 'W', 'H' )
#define ID_XXYY		MAKE_CHUNK_ID( 'X', 'X', 'Y', 'Y' )
#define ID_WWHH		MAKE_CHUNK_ID( 'W', 'W', 'H', 'H' )
#define ID_DPTH		MAKE_CHUNK_ID( 'D', 'P', 'T', 'H' )
#define ID_RECT		MAKE_CHUNK_ID( 'R', 'E', 'C', 'T' )
#define ID_SCID		MAKE_CHUNK_ID( 'S', 'C', 'I', 'D' )
#define ID_FLGS		MAKE_CHUNK_ID( 'F', 'L', 'G', 'S' )
#define ID_RATE		MAKE_CHUNK_ID( 'R', 'A', 'T', 'E' )
#define ID_DXDY		MAKE_CHUNK_ID( 'D', 'X', 'D', 'Y' )
#define ID_NXID		MAKE_CHUNK_ID( 'N', 'X', 'I', 'D' )
#define ID_aVEI		MAKE_CHUNK_ID( 'a', 'V', 'E', 'I' )
#define ID_ESIZ		MAKE_CHUNK_ID( 'E', 'S', 'I', 'Z' )
#define ID_FILE		MAKE_CHUNK_ID( 'F', 'I', 'L', 'E' )
#define ID_OUTA		MAKE_CHUNK_ID( 'O', 'U', 'T', 'A' )
#define ID_OUTB		MAKE_CHUNK_ID( 'O', 'U', 'T', 'B' )
#define ID_PRAT		MAKE_CHUNK_ID( 'P', 'R', 'A', 'T' )

// text blocks

#define ID_nTXT		MAKE_CHUNK_ID( 'n', 'T', 'X', 'T' )
#define ID_TEXT		MAKE_CHUNK_ID( 'T', 'E', 'X', 'T' )

// link point blocks

#define ID_LNKS		MAKE_CHUNK_ID( 'L', 'N', 'K', 'S' )
#define ID_LINK		MAKE_CHUNK_ID( 'L', 'I', 'N', 'K' )

// Special case links

#define ID_LNK1		MAKE_CHUNK_ID( 'L', 'N', 'K', '1' )
#define ID_LNK2		MAKE_CHUNK_ID( 'L', 'N', 'K', '2' )
#define ID_LNK3		MAKE_CHUNK_ID( 'L', 'N', 'K', '3' )
#define ID_LNK4		MAKE_CHUNK_ID( 'L', 'N', 'K', '4' )

// palette blocks

#define ID_nPAL		MAKE_CHUNK_ID( 'n', 'P', 'A', 'L' )
#define ID_RGBs		MAKE_CHUNK_ID( 'R', 'G', 'B', 's' )
#define ID_PSIZ		MAKE_CHUNK_ID( 'P', 'S', 'I', 'Z' )
#define ID_PFGs		MAKE_CHUNK_ID( 'P', 'F', 'G', 's' )
#define ID_SNTs		MAKE_CHUNK_ID( 'S', 'N', 'T', 's' )
#define ID_aPLI		MAKE_CHUNK_ID( 'a', 'P', 'L', 'I' )
#define ID_nPLS		MAKE_CHUNK_ID( 'n', 'P', 'L', 'S' )

// ----------------------------------------------------------------------------

// visual element blocks

#define ID_nVLS		MAKE_CHUNK_ID( 'n', 'V', 'L', 'S' )
#define ID_aVEL		MAKE_CHUNK_ID( 'a', 'V', 'E', 'L' )

// SRL compressed image blocks

#define ID_aS32		MAKE_CHUNK_ID( 'a', 'S', '3', '2' )
#define ID_aS16		MAKE_CHUNK_ID( 'a', 'S', '1', '6' )
#define ID_aS08		MAKE_CHUNK_ID( 'a', 'S', '0', '8' )
#define ID_nSXD		MAKE_CHUNK_ID( 'n', 'S', 'X', 'D' )
#define ID_aSCD		MAKE_CHUNK_ID( 'a', 'S', 'C', 'D' )
#define ID_aSVD		MAKE_CHUNK_ID( 'a', 'S', 'V', 'D' )

// anim blocks

#define ID_nAMS		MAKE_CHUNK_ID( 'n', 'A', 'M', 'S' )
#define ID_aANM		MAKE_CHUNK_ID( 'a', 'A', 'N', 'M' )
#define ID_aAHD		MAKE_CHUNK_ID( 'a', 'A', 'H', 'D' )
#define ID_aALK		MAKE_CHUNK_ID( 'a', 'A', 'L', 'K' )
#define ID_aBKD		MAKE_CHUNK_ID( 'a', 'B', 'K', 'D' )
#define ID_aPO1		MAKE_CHUNK_ID( 'a', 'P', 'O', '1' )

// Named spots

#define ID_nNSD		MAKE_CHUNK_ID( 'n', 'N', 'S', 'D' )
#define ID_aNSP		MAKE_CHUNK_ID( 'a', 'N', 'S', 'P' )
#define ID_aNSI		MAKE_CHUNK_ID( 'a', 'N', 'S', 'I' )

// frame blocks

#define ID_nFMS		MAKE_CHUNK_ID( 'n', 'F', 'M', 'S' )
#define ID_aFRM		MAKE_CHUNK_ID( 'a', 'F', 'R', 'M' )
#define ID_aFHD		MAKE_CHUNK_ID( 'a', 'F', 'H', 'D' )
#define ID_aFLS		MAKE_CHUNK_ID( 'a', 'F', 'L', 'S' )
#define ID_aFLK		MAKE_CHUNK_ID( 'a', 'F', 'L', 'K' )

// Named spot adjustments

#define ID_nNSA		MAKE_CHUNK_ID( 'n', 'N', 'S', 'A' )
#define ID_aNSA		MAKE_CHUNK_ID( 'a', 'N', 'S', 'A' )

// layer info blocks

#define ID_nLIS		MAKE_CHUNK_ID( 'n', 'L', 'I', 'S' )
#define ID_aLII		MAKE_CHUNK_ID( 'a', 'L', 'I', 'I' )

// layer blocks

#define ID_nLRS		MAKE_CHUNK_ID( 'n', 'L', 'R', 'S' )
#define ID_aLYR		MAKE_CHUNK_ID( 'a', 'L', 'Y', 'R' )

// visual reference blocks

#define ID_nVRS		MAKE_CHUNK_ID( 'n', 'V', 'R', 'S' )
#define ID_aVRF		MAKE_CHUNK_ID( 'a', 'V', 'R', 'F' )

// ----------------------------------------------------------------------------

// Disable > 255 char warning -- I hope they fix this in VC++ 7.0!!!

#pragma warning(disable: 4786)

// ----------------------------------------------------------------------------

#include "BPTpcxio.h"
#include "BPTAnimator.h"

// ----------------------------------------------------------------------------

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Load .BPT file
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool LoadBPTFilePrim( CChunkyFileIO * io, CAnimationShowcase ** out );

	bool LoadBPTFile( const char * filename, CAnimationShowcase ** out );

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Save .BPT file
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool SaveBPTFilePrim( CChunkyFileIO * io, CAnimationShowcase * pShowCase );

	bool SaveBPTFile(
		const char * filename, 
		CAnimationShowcase * pShowCase, 
		const bool bExternalVisualElements,
		const bool bXMLIOMode,
		const bool bBackup
	);

}; // namespace BPT

#endif // !defined(AFX_BPTANIMATIONFILEIO_H__B851D6A5_2E4A_4369_A11A_5D8A1DD409F2__INCLUDED_)
