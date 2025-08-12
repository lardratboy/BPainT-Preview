/*
Module : IJLBitmap.CPP
Purpose: Implementation for a MFC class encapsulation of the Intel Jpeg Library (IJL)
Created: PJN / 03-04-2000
History: PJN / 10-04-2000 Now includes full support for palettes.
         PJN / 30-07-2000 1. Now supports loading of .gif and .ico. This is 
                          accomplished via the IPicture COM interface. Please note that
                          to release code which uses the GIF format, you will need to
                          obtain a license from Unisys. 
                          2. I have also removed the dependence on the Intel Jpeg Library 
                          as the IPicture interface supports loading Jpegs
         PJN / 31-07-2000 1. Class is now derived from CBitmap as before, Dropped support
                          for ICO format as it was producing a separate code path.

Copyright (c) 2000 by PJ Naughter.  
All rights reserved.

*/

//////////////// Includes ////////////////////////////////////////////
#include "stdafx.h"
#include "IJLBitmap.h"


//////////////// Macros / Locals /////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HIMETRIC_INCH	2540


//////////////// Implementation //////////////////////////////////////

CIJLBitmap::CIJLBitmap()
{
  m_pPicture = NULL;
}

CIJLBitmap::~CIJLBitmap()
{
  DeleteObject();
}

CRect CIJLBitmap::Rect()
{
  CRect r(0, 0, 0, 0);
  if (m_hObject)
  {
    BITMAP bm;
    GetObject(sizeof(BITMAP), &bm);
    r.right = bm.bmWidth;
    r.bottom = bm.bmHeight;
  }

  return r;
}

CSize CIJLBitmap::Size()
{
  CRect r = Rect();
  return CSize(r.Width(), r.Height());
}

BOOL CIJLBitmap::Load(LPCTSTR lpszPathName)
{
  BOOL bSuccess = FALSE;

  //Free up any resource we may currently have
  DeleteObject();

	//open the file
  CFile f;
  if (!f.Open(lpszPathName, CFile::modeRead))
  {
    TRACE(_T("Failed to open file %s, Error:%x\n"), lpszPathName, ::GetLastError());
    return FALSE;
  }

	//get the file size
	DWORD dwFileSize = (DWORD)f.GetLength();

	//Allocate memory based on file size
	LPVOID pvData = NULL;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	if (hGlobal == NULL)
  {
    TRACE(_T("Failed to allocate memory for file %s, Error:%x\n"), lpszPathName, ::GetLastError());
    return FALSE;
  }
	pvData = GlobalLock(hGlobal);
	ASSERT(pvData);

	// read file and store in global memory
  if (f.Read(pvData, dwFileSize) != dwFileSize)
  {
    TRACE(_T("Failed to read in image date from file %s, Error:%x\n"), lpszPathName, ::GetLastError());
    GlobalUnlock(hGlobal);
    GlobalFree(hGlobal);
    return FALSE;
  }

  //Tidy up the memory and close the file handle
	GlobalUnlock(hGlobal);

	//create IStream* from global memory
	LPSTREAM pStream = NULL;
	if (FAILED(CreateStreamOnHGlobal(hGlobal, TRUE, &pStream)))
  {
    TRACE(_T("Failed to create IStream interface from file %s, Error:%x\n"), lpszPathName, ::GetLastError());
    GlobalFree(hGlobal);
    return FALSE;
  }

	// Create IPicture from image file
	if (SUCCEEDED(::OleLoadPicture(pStream, dwFileSize, FALSE, IID_IPicture, (LPVOID*)&m_pPicture)))
  {
    short nType = PICTYPE_UNINITIALIZED;
    if (SUCCEEDED(m_pPicture->get_Type(&nType)) && (nType == PICTYPE_BITMAP))
    {
		  OLE_HANDLE hBitmap;
      OLE_HANDLE hPalette;
		  if (SUCCEEDED(m_pPicture->get_Handle(&hBitmap)) &&
          SUCCEEDED(m_pPicture->get_hPal(&hPalette)))
      {
        Attach((HBITMAP) hBitmap);
        m_Palette.Attach((HPALETTE) hPalette);
        bSuccess = TRUE;
      }
    }
  }

	//Free up the IStream* interface
  pStream->Release();

  return bSuccess;
}

BOOL CIJLBitmap::LoadBmp(HINSTANCE hInst, LPCTSTR lpResourceName)
{
  BOOL bSuccess = FALSE;

  //Free up any resource we may currently have
  DeleteObject();

  //Call the SDK to do the load from the resource
  HBITMAP hBitmap = (HBITMAP) ::LoadImage(hInst, lpResourceName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTCOLOR);
  if (hBitmap)
  {
    m_Palette.Attach(CreatePalette(hBitmap));
    Attach(hBitmap);
    bSuccess = TRUE;
  }
  
  return bSuccess;
}

BOOL CIJLBitmap::LoadBmp(HINSTANCE hInst, UINT uID)
{
  return LoadBmp(hInst, MAKEINTRESOURCE(uID)); 
}

BOOL CIJLBitmap::Draw(CDC* pDC, DWORD dwRop, BOOL bBackground, const CRect* rcDst, const CRect* rcSrc, CPalette* pPal)
{
  //Quick return
  if (m_hObject == NULL)
    return FALSE;

  //Setup the source and dest rectangles
  CRect DCRect(Rect());
  CRect DibRect(DCRect);
  if (rcDst)
    DCRect = *rcDst;
  if (rcSrc)
    DibRect = *rcSrc;

  //Create a mem DC to draw into
  CDC memDC;
  memDC.CreateCompatibleDC(pDC);

  //Select in the bitmap, if the image is a bitmap
  CBitmap* pOldBitmap = memDC.SelectObject(this);

  //Select in the palette into the destination DC (if required)
  CPalette* pOldPalette = NULL;
  CPalette palette;
  if ((pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE))
  {
    if (pPal)
      pOldPalette = pDC->SelectPalette(pPal, bBackground);
    else
      pOldPalette = pDC->SelectPalette(&m_Palette, bBackground);
    pDC->RealizePalette();
  }

  //Do the actual bit blitting
  BOOL bSuccess = FALSE;
  if (rcDst == rcSrc)
    bSuccess = pDC->BitBlt(DCRect.left, DCRect.top, DCRect.Width(), DCRect.Height(), &memDC, 0, 0, dwRop);
  else
    bSuccess = pDC->StretchBlt(DCRect.left, DCRect.top, DCRect.Width(), DCRect.Height(), &memDC, 
                               DibRect.left, DibRect.top, DibRect.Width(), DibRect.Height(), dwRop);

  //Restore the DC
  if (pOldPalette)
    pDC->SelectObject(pOldPalette);
  if (pOldBitmap)
    memDC.SelectObject(pOldBitmap);

  return TRUE;
}

HPALETTE CIJLBitmap::CreatePalette(HBITMAP hBitmap)
{
  HPALETTE hPalette = NULL;

  //Get the color depth of the DIBSection
  CBitmap bitmap;
  bitmap.Attach(hBitmap);

  BITMAP bm;
  VERIFY(bitmap.GetObject(sizeof(BITMAP), &bm));

  //If the DIBSection is 256 color or less, it has a color table
  if ((bm.bmBitsPixel * bm.bmPlanes) <= 8)
  {
    // Create a memory DC and select the DIBSection into it
    CDC memDC;
    memDC.CreateCompatibleDC(NULL);
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

    // Get the DIBSection's color table
    RGBQUAD rgb[256];
    UINT nEntries = GetDIBColorTable(memDC.m_hDC, 0, 256, rgb);

    // Create a palette from the color tabl
    LOGPALETTE* pLogPal = (LOGPALETTE*) new BYTE[sizeof(LOGPALETTE) + (nEntries*sizeof(PALETTEENTRY))];
    pLogPal->palVersion = 0x300;
    pLogPal->palNumEntries = (WORD) nEntries;
    for (UINT i=0; i<nEntries; i++)
    {
      pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
      pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
      pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
      pLogPal->palPalEntry[i].peFlags = 0;
    }
    hPalette = ::CreatePalette(pLogPal);

    // Clean up
    memDC.SelectObject(pOldBitmap);
    delete [] pLogPal;
  }
  else   // It has no color table, so use a halftone palette
  {
    CWnd* pWndDesktop = CWnd::GetDesktopWindow();
    ASSERT(pWndDesktop);
    CDC* pDC = pWndDesktop->GetDC();
    hPalette = ::CreateHalftonePalette(pDC->m_hDC);
    pWndDesktop->ReleaseDC(pDC);
  }

  //Unlock the bitmap pointer
  bitmap.Detach();

  return hPalette;
}

BOOL CIJLBitmap::DeleteObject()
{
  if (m_pPicture)  //Free up the COM IPicture* pointer
  {
    //Detach the bitmap and palette objects from their handles
    Detach();
    m_Palette.Detach();

    m_pPicture->Release();
    m_pPicture = NULL;
  }
  else  //Just call the standard DeleteObject methods
  {
    CBitmap::DeleteObject();
    m_Palette.DeleteObject();
  }

  return TRUE;
}
