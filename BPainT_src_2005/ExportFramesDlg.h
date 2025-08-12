#if !defined(AFX_EXPORTFRAMESDLG_H__278956FB_96B5_4AF1_AFFB_0FBDA28DB1E0__INCLUDED_)
#define AFX_EXPORTFRAMESDLG_H__278956FB_96B5_4AF1_AFFB_0FBDA28DB1E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportFramesDlg.h : header file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include <list>
#include "BPTAnimator.h"

// ----------------------------------------------------------------------------

namespace BPT {

	class IImportExportImageCodec {

	public:

		class ImageInfo {

		public:

			// ----------------------------------------------------------------

			IImportExportImageCodec * pOwnerCodec;

			void * pData;

			SIZE size;

			int pitch;

			int bpp;

			RGBQUAD paletteData[ 256 ];

			// ----------------------------------------------------------------

			ImageInfo( IImportExportImageCodec * pOwner ) {

				pOwnerCodec = pOwner;

				pData = 0;
				size.cx = 0;
				size.cy = 0;
				pitch = 0;
				bpp = 0;

				memset( (void *)paletteData, 0, sizeof( paletteData ) );

			}

			virtual ~ImageInfo() {}

		};

	private:

	public:

		// --------------------------------------------------------------------

		IImportExportImageCodec() { /* Empty */ }

		virtual ~IImportExportImageCodec() { /* Empty */ }

		// --------------------------------------------------------------------

		virtual char * GetName() {

			return "ERROR!";

		}

		virtual ImageInfo * CreateImageInfo(
			const SIZE canvasSize
		) {

			return new ImageInfo( this );
	
		}

		// --------------------------------------------------------------------

		virtual bool Start(
			 ImageInfo * pImageInfo, const char * pFormatTxt, 
			 const SIZE canvasSize, const int nFrames
			 ,const POINT linkPoint
		) {

			return true;

		}

		virtual bool Frame( ImageInfo * pImageInfo, const int nFrameIOID ) {

			return true;

		}

		virtual bool Finish( ImageInfo * pImageInfo ) {
			
			return true;

		}

		virtual bool Abort( ImageInfo * pImageInfo ) {
			
			return true;

		}

		// --------------------------------------------------------------------

		virtual bool SettingsDialog( CWnd * pParent ) {

			return true;

		}

		virtual bool HasSettingsDialog() {

			return false;

		}

		// --------------------------------------------------------------------

		virtual bool LoadsPaletteData() {

			return false;

		}

		// backup exiting file and remove prior backup
		// ---------------------------------------------------------------------

		bool DoesFileExist( const char * pszFilename ) {

			FILE * f = fopen( pszFilename, "rb" );

			if ( 0 == f ) return false;

			fclose( f );

			return true;

		}

		bool CreateBackup( const char * pszFilename, const char * pszBackupExt ) {

			if ( !DoesFileExist( pszFilename ) ) return true;

			char backupFilename[ _MAX_PATH ];

			strcpy( backupFilename, pszFilename );
			strcat( backupFilename, pszBackupExt );

			if ( DoesFileExist( backupFilename ) ) {

				DeleteFile( backupFilename );

			}

			return (FALSE != MoveFile( pszFilename, backupFilename ));

		}

		// --------------------------------------------------------------------

		virtual LPCTSTR GetDefaultExt() {

			static char BASED_CODE szDefaultExt[] = "*.bmp";

			return szDefaultExt;

		}

		virtual LPCTSTR GetFileFilters() {

			static char BASED_CODE szFilter[] =
				"Bitmap Files (*.bmp)|*.bmp|"
				"All Files (*.*)|*.*|"
				"|"
				;

			return szFilter;

		}

	};

};

// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CExportFramesDlg dialog

class CExportFramesDlg : public CDialog
{
// Construction
public:

	CExportFramesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportFramesDlg)
	enum { IDD = IDD_EXPORT_FRAMES };
	CComboBox	m_CodecCombo;
	CListBox	m_SrcLayersListBox;
	int		m_nSrcFrom;
	int		m_nSrcTo;
	CString	m_strFormat;
	int		m_nIncrement;
	int		m_nStartNumber;
	BOOL	m_bUsePalette;
	//}}AFX_DATA

	int m_SelectedCodec;

	int m_nMin;
	int m_nMax;

	BPT::CAnimation * m_pSrcAnimation;

	typedef std::list<int> selected_layers_collection_type;

	selected_layers_collection_type m_SrcSelectedLayers;

	bool m_bEnablePaletteCheckBox;

	CString m_strDlgName;
	CString m_strRangePrefix;

	typedef std::list< BPT::IImportExportImageCodec * > codec_collection_type;

	std::list< BPT::IImportExportImageCodec * > m_CodecCollection;

	BPT::IImportExportImageCodec * m_pSelectedCodec;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportFramesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString CExportFramesDlg::Filename2Filespec(CString filename);

	// Generated message map functions
	//{{AFX_MSG(CExportFramesDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBrowse();
	afx_msg void OnSelchangeCodecCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCodecSettings();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTFRAMESDLG_H__278956FB_96B5_4AF1_AFFB_0FBDA28DB1E0__INCLUDED_)
