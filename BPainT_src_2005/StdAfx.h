// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7D0396B8_029A_472C_ABBC_F1929561458D__INCLUDED_)
#define AFX_STDAFX_H__7D0396B8_029A_472C_ABBC_F1929561458D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(WINVER)
#define WINVER 0x0410
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcview.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// ----------------------------------------------------------------------------

#include "mmsystem.h"
#include <afxole.h>

// Disable > 255 char warning -- I hope they fix this in VC++ 7.0!!!

#pragma warning(disable: 4786)

// ----------------------------------------------------------------------------

#include <atlstr.h>
#include <atlimage.h>

// ----------------------------------------------------------------------------
// define this to offer a full true display
// ----------------------------------------------------------------------------

//#define TEST_BPAINT_TRUE_COLOR_TEST

// ----------------------------------------------------------------------------

#include "BPTBitmap.h"

#if (defined(TEST_BPAINT_TRUE_COLOR_TEST) || defined(BPAINT_CHUNKY_32BPP_VERSION)) // bpt 4/24/03

typedef BPT::PIXEL_RGB32 SDisplayPixelType;

#else

typedef BPT::PIXEL_RGB16 SDisplayPixelType;

#endif

//
//	MakeDisplayPixelType()
//

SDisplayPixelType __inline 
MakeDisplayPixelType( const int r, const int g, const int b )
{
#if (defined(TEST_BPAINT_TRUE_COLOR_TEST) || defined(BPAINT_CHUNKY_32BPP_VERSION)) // bpt 4/24/03

	return ((((int)(r) << 16) | ((int)(g) << 8) | (int)(b)) | 0xff000000);

#else

	return (((int)(r>>3) << 10) | ((int)(g>>3) << 5) | (int)(b>>3));

#endif
}

int __inline DisplayPixel_R( const SDisplayPixelType p )
{
#if (defined(TEST_BPAINT_TRUE_COLOR_TEST) || defined(BPAINT_CHUNKY_32BPP_VERSION)) // bpt 4/24/03

	return (p >> 16) & 0xff;

#else

	return ((p & (0x31 << 10)) >> 7);

#endif
}

int __inline DisplayPixel_G( const SDisplayPixelType p )
{
#if (defined(TEST_BPAINT_TRUE_COLOR_TEST) || defined(BPAINT_CHUNKY_32BPP_VERSION)) // bpt 4/24/03

	return (p >> 8) & 0xff;

#else

	return ((p & (0x31 << 5)) >> 2);

#endif
}

int __inline DisplayPixel_B( const SDisplayPixelType p )
{
#if (defined(TEST_BPAINT_TRUE_COLOR_TEST) || defined(BPAINT_CHUNKY_32BPP_VERSION)) // bpt 4/24/03

	return (p) & 0xff;

#else

	return ((p & 0x31) << 3);

#endif
}

COLORREF __inline DisplayPixel_COLORREF( const SDisplayPixelType p )
{
	return RGB(
		DisplayPixel_R( p ), DisplayPixel_G( p ), DisplayPixel_B( p )
	);
}

// ----------------------------------------------------------------------------

BOOL GLOBAL_StencilHackOnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

#endif // !defined(AFX_STDAFX_H__7D0396B8_029A_472C_ABBC_F1929561458D__INCLUDED_)
