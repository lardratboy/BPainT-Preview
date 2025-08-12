/*
Module : IJLBitmap.H
Purpose: Defines the interface for an MFC class encapsulation of the Intel Jpeg Library (IJL)
Created: PJN / 03-04-2000

Copyright (c) 2000 by PJ Naughter.  
All rights reserved.

*/


/////////////////////////////// Defines ///////////////////////////////////////
#ifndef __IJLBITMAP_H__
#define __IJLBITMAP_H__

#ifndef __AFXOLE_H__
#pragma message("To avoid this message please put afxole.h into your PCH (normally stdafx.h)")
#include <afxole.h>
#endif
  

/////////////////////////////// Classes ///////////////////////////////////////

class CIJLBitmap : public CBitmap
{
public:
//Constructors / Destructors
  CIJLBitmap();
  virtual ~CIJLBitmap();

//Loading support
  BOOL Load(LPCTSTR lpszPathName);
  BOOL LoadBmp(HINSTANCE hInst, LPCTSTR lpResourceName);
  BOOL LoadBmp(HINSTANCE hInst, UINT uID);
   
//Misc
  BOOL      DeleteObject();                  //Free up any memory used by the bitmap or palette
  CRect     Rect();                          //returns a rect of the bitmap boundary
  CSize     Size();                          //Returns the size (width and Depth) of the bitmap boundary
  CPalette& Palette() { return m_Palette; }; //Return a pointer to the palette

//Drawing support
  BOOL Draw(CDC* pDC, DWORD dwRop = SRCCOPY, BOOL bBackground = FALSE, const CRect* rcDst=NULL, const CRect* rcSrc=NULL, CPalette* pPal=NULL);

protected:
//Methods
  HPALETTE CreatePalette(HBITMAP hBitmap);

//Variables
  LPPICTURE m_pPicture;  
  CPalette  m_Palette;
};

#endif //__IJLBITMAP_H__

