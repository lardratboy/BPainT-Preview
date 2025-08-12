// ExportFramesDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	The way the codec's image information is managed is beyond gross
//	I just didn't have time to do a proper design.
//
//

#include "stdafx.h"
#include "bpaint.h"
#include "ExportFramesDlg.h"
#include "EnterStringDlg.h"
#include "BpaintDoc.h"

#include "BPTUtility.h"
#include "BPTBitmap.h"
#include "BPTpcxio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportFramesDlg dialog


CExportFramesDlg::CExportFramesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExportFramesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportFramesDlg)
	m_nSrcFrom = 0;
	m_nSrcTo = 0;
	m_strFormat = _T("");
	m_nIncrement = 0;
	m_nStartNumber = 0;
	m_bUsePalette = FALSE;
	//}}AFX_DATA_INIT

	m_strDlgName = "Import / Export Dialog";
	m_strRangePrefix = "Range";

	m_bEnablePaletteCheckBox = false;

	m_nMin = 0;
	m_nMax = 0;

	m_pSrcAnimation = 0;

	m_pSelectedCodec = 0;

}

void CExportFramesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportFramesDlg)
	DDX_Control(pDX, IDC_CODEC_COMBO, m_CodecCombo);
	DDX_Control(pDX, IDC_SRC_LAYERS_LIST, m_SrcLayersListBox);
	DDX_Text(pDX, IDC_SRC_FROM_EDIT, m_nSrcFrom);
	DDX_Text(pDX, IDC_SRC_TO_EDIT, m_nSrcTo);
	DDX_Text(pDX, IDC_FILENAME_FORMAT_EDIT, m_strFormat);
	DDX_Text(pDX, IDC_OUTPUT_INC_EDIT, m_nIncrement);
	DDX_Text(pDX, IDC_OUTPUT_START_EDIT, m_nStartNumber);
	DDX_Check(pDX, IDC_PALETTE_CHECK, m_bUsePalette);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExportFramesDlg, CDialog)
	//{{AFX_MSG_MAP(CExportFramesDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_CBN_SELCHANGE(IDC_CODEC_COMBO, OnSelchangeCodecCombo)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CODEC_SETTINGS, OnBnClickedCodecSettings)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportFramesDlg message handlers

BOOL CExportFramesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Fill in the codec combo box
	// ------------------------------------------------------------------------

	codec_collection_type::iterator it;

	for ( it = m_CodecCollection.begin(); it != m_CodecCollection.end(); it++ ) {

		int item = m_CodecCombo.AddString( (*it)->GetName() );

		if ( CB_ERR != item ) {

			if ( m_pSelectedCodec == (*it) ) {

				m_CodecCombo.SetCurSel( item );

			}

			if ( CB_ERR == m_CodecCombo.SetItemDataPtr( item, *it ) ) {

				return FALSE;

			}

		}

	}

	// ------------------------------------------------------------------------

	if ( m_pSelectedCodec ) {

		CWnd * pCheckBox = GetDlgItem(IDC_PALETTE_CHECK);

		if ( pCheckBox ) {

			pCheckBox->EnableWindow(
				m_pSelectedCodec->LoadsPaletteData() ? TRUE : FALSE
			);

		}

		CWnd * pSettingsButton = GetDlgItem(IDC_CODEC_SETTINGS);
	
		if ( pSettingsButton ) {
	
			pSettingsButton->EnableWindow(
				m_pSelectedCodec->HasSettingsDialog() ? TRUE : FALSE
			);
	
		}

	}

	// Hook up the frame from & to spin controls
	// ------------------------------------------------------------------------
	
	CSpinButtonCtrl * pSrcFromSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SRC_FROM_SPIN);

	if ( pSrcFromSpin ) {

		pSrcFromSpin->SetRange( m_nMin, m_nMax );
		pSrcFromSpin->SetPos( m_nSrcFrom );

	}

	CSpinButtonCtrl * pSrcToSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SRC_TO_SPIN);

	if ( pSrcToSpin ) {

		pSrcToSpin->SetRange( m_nMin, m_nMax );
		pSrcToSpin->SetPos( m_nSrcTo );

	}

	// Set the message
	// ------------------------------------------------------------------------

	CString rangeText;

	rangeText.Format( "%s %d to %d", m_strRangePrefix, m_nMin, m_nMax );

	CWnd * pStaticCtrl = (CWnd *)GetDlgItem(IDC_RANGE_INFO);

	if ( pStaticCtrl ) {

		pStaticCtrl->SetWindowText( rangeText );

	}

	// Fill in the layers list box
	// ------------------------------------------------------------------------

	if ( m_pSrcAnimation ) {

		int layerCount = m_pSrcAnimation->LayerCount();

		for ( int i = 0; i < layerCount; i++ ) {

			BPT::CLayerInfo * pLayerInfo = m_pSrcAnimation->GetLayerInfo( i );

			if ( !pLayerInfo ) {

				return FALSE;

			}

			char * pName = pLayerInfo->GetName();

			if ( !pName ) {

				return FALSE;

			}

			int index;

			if ( LB_ERR == (index = m_SrcLayersListBox.AddString( pName )) ) {

				return FALSE;

			}

			// Initial selection

			selected_layers_collection_type::iterator it =
				std::find( m_SrcSelectedLayers.begin(), m_SrcSelectedLayers.end(), index
			);

			if ( m_SrcSelectedLayers.end() != it ) {

				m_SrcLayersListBox.SetSel( index, TRUE );

			}

		}

	}

	// ------------------------------------------------------------------------

	// TODO: Add extra initialization here

	SetWindowText( m_strDlgName );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CExportFramesDlg::OnOK() 
{

	// ------------------------------------------------------------------------

	CString strFormat;

	GetDlgItemText( IDC_FILENAME_FORMAT_EDIT, strFormat );

	if ( !strFormat.GetLength() ) {

		MessageBox( "You MUST specify a format string", "Yo", MB_ICONEXCLAMATION | MB_OK );

		return;

	}

	// Fill in the selected layers collection with the info from the list box
	// ------------------------------------------------------------------------

	m_SrcSelectedLayers.clear();

	int selCount = m_SrcLayersListBox.GetSelCount();

	if ( selCount ) {

		int itemCount = m_SrcLayersListBox.GetCount();

		for ( int i = 0; i < itemCount; i++ ) {

			if ( 0 < m_SrcLayersListBox.GetSel( i ) ) {

				m_SrcSelectedLayers.push_back( i );

			}

		}

	} else {

		MessageBox( "Select at least one layer", "Yo", MB_ICONEXCLAMATION | MB_OK );

		return;

	}
	
	CDialog::OnOK();
}

CString CExportFramesDlg::Filename2Filespec(CString filename) 
{
	char buffer[ _MAX_PATH ];
	char *dp = buffer;

	char *sp = filename.GetBuffer(_MAX_PATH);

	char *pfilename = strrchr(sp, '\\');
	if (pfilename == NULL) {
		
		pfilename = sp;
		
	}

	bool found = false;

	while( *sp ) {
		
		if ((sp > pfilename) && *sp >= '0' && *sp <= '9') {
			
			if (!found) {
				
				while (*sp && (*sp >= '0' && *sp <= '9')) {
					
					sp++;
					
					*dp++ = '#';

				}
				
				found = true;
				
			} else {
				
				*dp++ = *sp++;
				
			}
			
		} else {
			
			*dp++ = *sp++;
			
		}
		
	}

	*dp = '\0';

	return buffer;
}

void CExportFramesDlg::OnBrowse() 
{
	// ------------------------------------------------------------------------

	if ( m_pSelectedCodec ) {

		CFileDialog opf( 
			TRUE
			,m_pSelectedCodec->GetDefaultExt()
			,NULL
			,OFN_LONGNAMES
			,m_pSelectedCodec->GetFileFilters()
			,this
		);

		if ( IDOK == opf.DoModal() ) {

			CString filename = Filename2Filespec(opf.GetPathName());
					
			SetDlgItemText( IDC_FILENAME_FORMAT_EDIT,  filename );

		}

		return;

	}

	// ------------------------------------------------------------------------

	CFileDialog opf( 
		TRUE, _T("*.bmp"), NULL, OFN_LONGNAMES, 
		_T(
			"Bitmap Files (*.bmp)|*.bmp|"
//			"JPEG Files (*.jpg)|*.jpg|"
//			"All Supported Formats (*.jpg,*.bmp)|*.jpg;*.bmp|"
			"All Files (*.*)|*.*|"
			"|"
		)
		,this
	);

	if ( IDOK == opf.DoModal() ) {

		CString filename = Filename2Filespec(opf.GetPathName());
				
		SetDlgItemText( IDC_FILENAME_FORMAT_EDIT,  filename );

	}

}

void CExportFramesDlg::OnSelchangeCodecCombo() 
{
	int item = m_CodecCombo.GetCurSel();

	if ( CB_ERR != item ) {

		BPT::IImportExportImageCodec * pCodec = 
			(BPT::IImportExportImageCodec *)m_CodecCombo.GetItemDataPtr( item );

		if ( ((BPT::IImportExportImageCodec *)-1) != pCodec ) {

			m_pSelectedCodec = pCodec;

		}

	} else {

		m_pSelectedCodec = 0;

	}

	// ------------------------------------------------------------------------

	CWnd * pCheckBox = GetDlgItem(IDC_PALETTE_CHECK);
	
	if ( pCheckBox ) {
	
		if ( m_pSelectedCodec ) {

			pCheckBox->EnableWindow(
				m_pSelectedCodec->LoadsPaletteData() ? TRUE : FALSE
			);
	
		} else {
	
			pCheckBox->EnableWindow( FALSE );
	
		}

	}

	CWnd * pSettingsButton = GetDlgItem(IDC_CODEC_SETTINGS);
	
	if ( pSettingsButton ) {
	
		if ( m_pSelectedCodec ) {

			pSettingsButton->EnableWindow(
				m_pSelectedCodec->HasSettingsDialog() ? TRUE : FALSE
			);
	
		} else {
	
			pSettingsButton->EnableWindow( FALSE );
	
		}

	}

}

void CExportFramesDlg::OnBnClickedCodecSettings()
{
	if ( m_pSelectedCodec ) {

		m_pSelectedCodec->SettingsDialog( this );

	}
}

// ----------------------------------------------------------------------------

//
//	CImport8BppBMPCodec
//

class CImport8BppBMPCodec : public BPT::IImportExportImageCodec {

public:

	typedef BPT::IImportExportImageCodec base_class;

private:

	SIZE m_CanvasSize;

	char m_pFormatTxt[ _MAX_PATH ];

	// ------------------------------------------------------------------------

	class CImportImage : public base_class::ImageInfo {

	public:

		BPT::C8bppPicture  * m_pPicture;

		CImportImage( base_class * pOwner ) : 
			base_class::ImageInfo( pOwner ), m_pPicture( 0 ) {

			// ???

		}

		virtual ~CImportImage() {

			Destroy();

		}

		// --------------------------------------------------------------------

		void Destroy() {

			if ( m_pPicture ) delete m_pPicture;

			size.cx = 0;

			size.cy = 0;

		}

		bool SetPicture( BPT::C8bppPicture  * pPicture ) {

			Destroy();

			m_pPicture = pPicture;

			if ( m_pPicture ) {

				// Fill in the data for mapping

				pData = (void *)(m_pPicture->Iterator( 0, 0 ));
				size = m_pPicture->Size();
				pitch = m_pPicture->Pitch();
				bpp = sizeof(BPT::C8bppPicture::pixel_type) * 8;

				// copy over the palette data (assume conversion operator if necessary)

				for ( int i = 0; i < 256; i++ ) {

					paletteData[ i ] = m_pPicture->m_PaletteData[ i ];

				}

			}

			return true;
		}

	};

	// ------------------------------------------------------------------------

public:

	// ------------------------------------------------------------------------

	CImport8BppBMPCodec() {

		*m_pFormatTxt = '\0';
		m_CanvasSize.cx = 0;
		m_CanvasSize.cy = 0;

	}

	virtual char * GetName() {

		return "BMP files";
		
	}

	virtual base_class::ImageInfo * CreateImageInfo( const SIZE canvasSize ) {

		return new CImportImage( this );
	
	}

	// ------------------------------------------------------------------------

	virtual bool Start(
		ImageInfo * pImageInfo
		,const char * pFormatTxt
		,const SIZE canvasSize
		,const int nFrames
		,const POINT linkPoint
	) {

		// Convert filename to sprintfable form

		char *p = m_pFormatTxt;
		int changed = false;
		
		while( *pFormatTxt ) {

			if ( !changed && *pFormatTxt == '#' ) {

				int count = 0;

				while( *pFormatTxt++ == '#' ) {

					count++;
				}

				*p++ = '%';
				*p++ = '0';
				*p++ = '0' + (count > 9?9:count);
				*p++ = 'd';

				changed = true;

				pFormatTxt--;

			} else {

				*p++ = *pFormatTxt++;
			}

		}

		*p = '\0';
		
		if ( strchr(m_pFormatTxt, '.') == 0 ) {
			
			strcat(m_pFormatTxt, ".bmp");
			
		}

		m_CanvasSize = canvasSize;

		return true;

	}

	virtual bool Frame( ImageInfo * pImageInfo, const int nFrameIOID ) {

		// --------------------------------------------------------------------

		if ( (*m_pFormatTxt == '\0') || (!pImageInfo) || (this != pImageInfo->pOwnerCodec) ) {

			return false;

		}

		// --------------------------------------------------------------------

		char filename[ _MAX_PATH ];

		sprintf( filename, m_pFormatTxt, nFrameIOID );

		// GROSS/LAME design!
		// --------------------------------------------------------------------

		CImportImage * pImportImage = (CImportImage *)pImageInfo;

		// --------------------------------------------------------------------

		BPT::C8bppPicture * pLoadedPicture = BPT::Load8BppPicture( filename );

		if ( !pLoadedPicture ) {

			TRACE( "Loader didn't like picture \"%s\"\n", filename );

			// This probably should be an error!

			return false;

		}

		pImportImage->SetPicture( pLoadedPicture );

		return true;

	}

	bool Finish( ImageInfo * pImageInfo ) {

		return true;

	}

	// ------------------------------------------------------------------------

	virtual bool LoadsPaletteData() {

		return true;

	}

};

//
//	CBpaintDoc::ImportFramesDialogHandler()
//

bool CBpaintDoc::ImportFramesDialogHandler( CWnd * pParent )
{
	CExportFramesDlg dlg( pParent );

	// Setup the dlg members
	// ------------------------------------------------------------------------

	dlg.m_strRangePrefix = "Dest Range";
	dlg.m_strDlgName = "Import frames dialog";
	dlg.m_nMin = 1;
	dlg.m_nMax = m_pAnimation->FrameCount();
	dlg.m_nSrcFrom = dlg.m_nMin;
	dlg.m_nSrcTo = dlg.m_nMax;
	dlg.m_pSrcAnimation = m_pAnimation;
	dlg.m_SrcSelectedLayers.push_back( m_pAnimation->CurrentLayer() );
	dlg.m_nStartNumber = GLOBAL_GetSettingInt( "ImportDlg.nStart", 1 );
	dlg.m_nIncrement = GLOBAL_GetSettingInt( "ImportDlg.nIncrement", 1 );
	dlg.m_strFormat = "path\\filename####.bmp";
	dlg.m_bUsePalette = GLOBAL_GetSettingInt( "ImportDlg.bUsePal", TRUE );

	// Fill in the codecs collection
	// ------------------------------------------------------------------------

	CImport8BppBMPCodec import8bppBMPCodec;

	dlg.m_CodecCollection.push_back( &import8bppBMPCodec );

	// Do the modal dialog 
	// ------------------------------------------------------------------------

DO_OVER:

	dlg.m_pSelectedCodec = &import8bppBMPCodec;

	if ( IDOK != dlg.DoModal() ) {

		return false;

	}

	GLOBAL_PutSettingInt( "ImportDlg.nStart", dlg.m_nStartNumber );
	GLOBAL_PutSettingInt( "ImportDlg.nIncrement", dlg.m_nIncrement );
	GLOBAL_PutSettingInt( "ImportDlg.bUsePal", dlg.m_bUsePalette );

	// ------------------------------------------------------------------------

	BPT::IImportExportImageCodec * pCodec = dlg.m_pSelectedCodec;

	if ( !pCodec ) {

		goto DO_OVER;

	}

	// ------------------------------------------------------------------------

	BPT::IImportExportImageCodec::ImageInfo * pImageInfo = pCodec->CreateImageInfo(
		m_CanvasSize
	);

	if ( !pImageInfo ) {

		return false;

	}

	// Finally perform the operation.
	// ------------------------------------------------------------------------

	int layerCount = m_pAnimation->LayerCount();

	if ( !pCodec->Start(
		pImageInfo, dlg.m_strFormat, m_CanvasSize,
		dlg.m_nSrcTo - dlg.m_nSrcFrom + 1,
		m_pAnimation->m_Link1
		) ) {

		TRACE( "FAILURE::Unable to start codec\n" );

		delete pImageInfo;

		return false;

	}

	int number = dlg.m_nStartNumber;

	bool bFirstFrame = true;

	bool bPaletteChanged = false;

	// ------------------------------------------------------------------------

	for ( int frame = dlg.m_nSrcFrom; frame <= dlg.m_nSrcTo; frame++ ) {

		// --------------------------------------------------------------------

		if ( !pCodec->Frame( pImageInfo, number ) && bFirstFrame ) {

			::MessageBox(NULL, "There was a problem importing the frames, possibly an incorrect filename.", "Huh?", MB_OK);
			
			TRACE( "FAILURE::Codec process frame %d\n", number );

			delete pImageInfo;

			return false;

		}

		number += dlg.m_nIncrement;

		// --------------------------------------------------------------------

		if ( bFirstFrame ) {

			bFirstFrame = false;

			if ( dlg.m_bUsePalette && pCodec->LoadsPaletteData() ) {

				bPaletteChanged = true;

				BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

				int nTotalColors = min( 256, pPal->TotalColors() );

				for ( int i = 0; i < nTotalColors; i++ ) {

					// Set the display clut
					// --------------------------------------------------------

					m_Edit2CompositeClut[ i ] = MakeDisplayPixelType(
						pImageInfo->paletteData[ i ].rgbRed,
						pImageInfo->paletteData[ i ].rgbGreen,
						pImageInfo->paletteData[ i ].rgbBlue
					);

					// Set the annotated palette
					// --------------------------------------------------------

					if ( pPal ) {

						pPal->SetSlot( i,
							pImageInfo->paletteData[ i ].rgbRed,
							pImageInfo->paletteData[ i ].rgbGreen,
							pImageInfo->paletteData[ i ].rgbBlue
						);

					}

				}

			}

		}
	
		// Create a visual element for this loaded image
		// --------------------------------------------------------------------

		BPT::CVisualElement * pVisualElement = 0;

		if ( pImageInfo->size.cx && pImageInfo->size.cy ) {

// THIS WHOLE PROCESS WILL NEED TO BE REDONE WHEN MULTIPLE BITDEPTHS ARE SUPPORTED!
// THIS WHOLE PROCESS WILL NEED TO BE REDONE WHEN MULTIPLE BITDEPTHS ARE SUPPORTED!

			// For now abort if the loaded image isn't the same as the editor!
			// ----------------------------------------------------------------

			if ( (sizeof(editor_pixel_type) * 8) != pImageInfo->bpp ) {

				TRACE(
					"FAILURE::Incorrect pixel depth loaded (%d need %d)\n",
					pImageInfo->bpp, (sizeof(editor_pixel_type) * 8)
				);

				pCodec->Abort( pImageInfo );

				delete pImageInfo;

				return false;

			}

			// Create a 'mapped' editor surface type from codec data
			// ----------------------------------------------------------------

			BPT::TSimpleBitmap<editor_pixel_type> mappedSurface;

			if ( !mappedSurface.Map(
				(BYTE *)pImageInfo->pData, pImageInfo->size, pImageInfo->pitch, false, true ) ) {

				TRACE( "FAILURE::Unable to map codec loaded bitmap?\n");

				pCodec->Abort( pImageInfo );

				delete pImageInfo;

				return false;

			}

			// Finally create a visual element
			// ----------------------------------------------------------------

			CRect captureRect;
		
			BPT::T_FindBoundingRect(
				captureRect, mappedSurface, BPT::TIsNotValue<editor_pixel_type>( m_ChromaKey ), 0
			);
		
			if ( !captureRect.IsRectEmpty() ) {
		
				// Ask the showcase to create us a new visual element
				// ------------------------------------------------------------
		
				pVisualElement = m_pShowcase->CreateVisualElementFromSurface(
					mappedSurface, m_ChromaKey, &captureRect
				);
			
				if ( !pVisualElement ) {
			
					TRACE( "FAILURE::Visual element create failed?\n");

					pCodec->Abort( pImageInfo );
	
					delete pImageInfo;
	
					return false;

				}
		
			}

		}

		// --------------------------------------------------------------------

		int frameNumber = frame - 1;

		for ( int layer = 0; layer < layerCount; layer++ ) {

			// Is this layer in the selected collection?
			// ----------------------------------------------------------------

			CExportFramesDlg::selected_layers_collection_type::iterator it =
				std::find( dlg.m_SrcSelectedLayers.begin(), dlg.m_SrcSelectedLayers.end(), layer
			);

			if ( dlg.m_SrcSelectedLayers.end() != it ) {

				BPT::CLayer * pLayer = m_pAnimation->GetLayer( frameNumber, layer );

				if ( pLayer ) {

					pLayer->SetVisualElement( pVisualElement, 0 );

				}

			}

		}

	}

	if ( !pCodec->Finish( pImageInfo ) ) {
		
		TRACE( "FAILURE::Codec finish failed?\n");

		delete pImageInfo;

		return false;

	}

	delete pImageInfo;

	// Need to report the palette change if any.
	// ------------------------------------------------------------------------

	if ( bPaletteChanged ) {

		UpdateAllViews( NULL, CColorChange::WHOLE_PALETTE_CHANGE, (CObject *)&CColorChange( 0 ) );

	}

	// Finally perform the operation.
	// ------------------------------------------------------------------------

	return true;

}

// ----------------------------------------------------------------------------

//
//	TSubImageExportCodecBase
//

template< class T >
class TSubImageExportCodecBase : public BPT::IImportExportImageCodec {

protected:

	char m_szFormatTxt[ _MAX_PATH ];

	POINT m_LinkPoint;

	int m_nFrames;

	typename typename T::pixel_type m_Chromakey;

	CRect m_TotalBoundingRect;

	// sub image collection to be written
	// ------------------------------------------------------------------------

	struct CSubImage {

		T surface;

		POINT deltaFromLinkPoint;

		int userValue;

		CSubImage() : userValue(0) {

			deltaFromLinkPoint.x = 0;
			deltaFromLinkPoint.y = 0;

		}

	};

	typedef std::list< CSubImage * > sub_image_collection_type;

	sub_image_collection_type m_SubImages;

public:

	typedef BPT::IImportExportImageCodec base_class;

	// ------------------------------------------------------------------------

	TSubImageExportCodecBase() {

		m_szFormatTxt[0] = '\0';

		m_LinkPoint.x = 0;
		m_LinkPoint.y = 0;

		m_nFrames = 0;
		m_Chromakey = 0;

		m_TotalBoundingRect.SetRectEmpty();

	}

	void ReleaseSubImages() {

		while ( !m_SubImages.empty() ) {

			delete m_SubImages.back();

			m_SubImages.pop_back();

		}

	}

	virtual ~TSubImageExportCodecBase() {

		ReleaseSubImages();

	}

	virtual char * GetName() {

		return "ERROR!::Sub image export base?";
		
	}

	// ------------------------------------------------------------------------

	virtual bool Start(
		ImageInfo * pImageInfo
		,const char * pFormatTxt
		,const SIZE canvasSize
		,const int nFrames
		,const POINT linkPoint
	) {

		strcpy( m_szFormatTxt, pFormatTxt );

		m_LinkPoint = linkPoint;

		return true;

	}

	virtual bool Frame( ImageInfo * pImageInfo, const int nFrameIOID ) {

		// --------------------------------------------------------------------

		if ( (*m_szFormatTxt == '\0') || (!pImageInfo) || (this != pImageInfo->pOwnerCodec) ) {

			return false;

		}

		// --------------------------------------------------------------------

		T mappedSurface;

		if ( !mappedSurface.Map(
			(BYTE *)pImageInfo->pData, pImageInfo->size, pImageInfo->pitch, false, true ) ) {

			TRACE( "FAILURE::Map surface failed?\n" );

			return false;

		}

		// Capture the bounding rectangle into a sub image
		// --------------------------------------------------------------------

		CSubImage * pSubImage = new CSubImage;

		if ( !pSubImage ) {

			TRACE( "FAILURE:Unable to create new sub image class\n" );
			
			return false;

		}

		m_SubImages.push_back( pSubImage );

		// Find the bounding rectangle (assumes 0 chromakey...)
		// --------------------------------------------------------------------

		CRect boundingRect;

		BPT::T_FindBoundingRect(
			boundingRect, mappedSurface, BPT::TIsNotValue<typename T::pixel_type>( m_Chromakey ), 0
		);

		// --------------------------------------------------------------------

		if ( !boundingRect.IsRectEmpty() ) {

			if ( m_TotalBoundingRect.IsRectEmpty() ) {

				m_TotalBoundingRect = boundingRect;

			} else {

				m_TotalBoundingRect |= boundingRect;

			}

			pSubImage->deltaFromLinkPoint.x = boundingRect.left - m_LinkPoint.x;
			pSubImage->deltaFromLinkPoint.y = boundingRect.top - m_LinkPoint.y;

			if ( !pSubImage->surface.CreateFromRect( mappedSurface, boundingRect ) ) {

				TRACE( "FAILURE: failed to create sub image surface\n" );

				return false;

			}

		}

		return true;

	}

	// ------------------------------------------------------------------------

	virtual bool ProcessSubImageCollection() = 0;

	// ------------------------------------------------------------------------

	virtual bool Finish( ImageInfo * pImageInfo ) {

		// m_TotalBoundingRect <- is the overall rectangle... 
		// could be used to do some sort of uniform output.

		ASSERT( m_nFrames == m_SubImages.size() );

		// This is where the actual output/processing happens

		if ( !ProcessSubImageCollection() ) return false;
		
		ReleaseSubImages();

		return true;

	}

	virtual bool Abort( ImageInfo * pImageInfo ) {

		// Handle Abort

		ReleaseSubImages();
		
		return true;

	}

}; // TSubImageExportCodecBase

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THESE OUTPUT HELPERS SHOULD BE IN THEIR OWN FILE!
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace BPT_FILE_OUTPUT_HELPERS {

	//
	//	T_PackedOutput8bppSurface()
	//
	//	-- pads each line with 0's if not enough pixels to flesh out word size
	//	-- (the packed bytes are also padded if needed)
	//

	template< class T > void T_PackedOutput8bppSurface(
		FILE * outFile
		,int & packedOutputCount
		,T & bitmap
		,const bool bHex
		,const int nBitsPerValue
		,const int nBitsPerPackedOutput
	) {

		// ------------------------------------------------------------------------

		int h = bitmap.Height();
		int w = bitmap.Width();
		int next = 0;

		int nPixelsPerByte = 8 / nBitsPerValue;

#if 1 // BPT 4/21/03

		int rawBytesPerLine = w / nPixelsPerByte;

		int total = ((((rawBytesPerLine * 8) + nBitsPerPackedOutput - 1) / nBitsPerPackedOutput) * h);

#else

		int total = ((w * h) * nBitsPerValue) / nBitsPerPackedOutput;

#endif

		packedOutputCount += total;

		// ------------------------------------------------------------------------

		const char * pszFormat;

		int extraCount = 0;

		if ( 8 == nBitsPerPackedOutput ) {

			if ( bHex ) {

				pszFormat = "0x%02x";

			} else {

				pszFormat = "%3d";

			}

		} else if ( 16 == nBitsPerPackedOutput ) {

			extraCount = 1;

			if ( bHex ) {

				pszFormat = "0x%04x";

			} else {

				pszFormat = "%5d";

			}

		} else {

			extraCount = 3;
			
			if ( bHex ) {

				pszFormat = "0x%08x";

			} else {

				pszFormat = "%10d";

			}

		}

		int subBytePixelMask = (1 << nBitsPerValue) - 1;

		// --------------------------------------------------------------------

		for ( int y = 0; y < h; ) {

			fprintf( outFile, "\t" );

			typename T::pixel_iterator it = bitmap.Iterator( 0, y );

			typename T::pixel_iterator end = it + w;

			while ( it != end ) {

				// ------------------------------------------------------------

				DWORD value = (*it++) & subBytePixelMask;

				{
					for ( int jj = 1; jj < nPixelsPerByte; jj++ ) {

						if ( it == end ) break;

						value |= ((*it++) & subBytePixelMask) << (jj * nBitsPerValue);

					}
				}

				for ( int extra = 1; extra <= extraCount; extra++ ) {

					if ( it == end ) break;

					DWORD packed = (*it++) & subBytePixelMask;

					for ( int kk = 1; kk < nPixelsPerByte; kk++ ) {

						if ( it == end ) break;

						packed |= ((*it++) & subBytePixelMask) << (kk * nBitsPerValue);

					}

					value |= (packed << (8 * extra));

				}

				// ------------------------------------------------------------

				if ( bHex ) {

					fprintf( outFile, pszFormat, value );

				} else {

					fprintf( outFile, pszFormat, value );

				}

				if ( ++next != total ) {

					fprintf( outFile, "," );

				}

			}

			if ( ++y != h ) {

				fprintf( outFile, "\n" );

			}

		}

	}

	//
	//	T_SubTilePackedOutput8bppSurface()
	//

	template< class T > bool T_SubTilePackedOutput8bppSurface(
		FILE * outFile
		,int & packedOutputCount
		,T & bitmap
		,const bool bHex
		,const int nBitsPerValue
		,const int nBitsPerPackedOutput
		,CSize & tileSize
	) {

		T tileBitmap;

		if ( !tileBitmap.Create( tileSize.cx, tileSize.cy ) ) {

			return false;

		}

		BPT::TCopyROP< typename T::pixel_type > top;

		int tilesWide = (bitmap.Width() + (tileSize.cx - 1)) / tileSize.cx;

		int tilesTall = (bitmap.Height() + (tileSize.cy - 1)) / tileSize.cy;

		int totalTiles = tilesWide * tilesTall;

		int currentTile = 0;

		for ( int y = 0; y < tilesTall; y++ ) {

			for ( int x = 0; x < tilesWide; x++ ) {

				// get the tile pixels
				// ------------------------------------------------------------

				tileBitmap.ClearBuffer( 0 );

				BPT::T_Blit(
					tileBitmap, (x * -tileSize.cx), (y * -tileSize.cy), 
					bitmap, top
				);

				// output the sub tile
				// ------------------------------------------------------------

				T_PackedOutput8bppSurface(
					outFile
					,packedOutputCount
					,tileBitmap
					,bHex
					,nBitsPerValue
					,nBitsPerPackedOutput
				);

				if ( ++currentTile != totalTiles ) {

					fprintf( outFile, ",\n\n" );

				}

			}

		}

		return true;

	}

}; // BPT_FILE_OUTPUT_HELPERS

// ----------------------------------------------------------------------------

//
//	TExport_C_Codec<>
//

template< class T, const bool HASPALETTE = true >
class TExport_C_Codec : public TSubImageExportCodecBase<T> {

protected:

	char m_szBaseName[ _MAX_PATH ];

public:

	typedef TSubImageExportCodecBase<T> base_class;

	TExport_C_Codec() {

		strcpy( m_szBaseName, "Anim" );

	}

	virtual ~TExport_C_Codec() {}

	virtual char * GetName() {

		return "Code export";
		
	}

	// ------------------------------------------------------------------------

	virtual bool ProcessSubImageCollection() {

		// (FUTURE) These should come from the settings dlg...
		// --------------------------------------------------------------------

		const char * pszImageDataProlog = "IMAGE_DATA_PROLOG_MACRO";

		const char * pszImageDataEpilog = "IMAGE_DATA_EPILOG_MACRO";

		const char * pszFrameDataProlog = "FRAME_DATA_PROLOG_MACRO";

		const char * pszFrameDataEpilog = "FRAME_DATA_EPILOG_MACRO";

		const char * pszDeclareFrameMacroName = "DECLARE_FRAME_DATA_MACRO";

		const char * pszDeclareAnimMacroName = "DECLARE_ANIM_DATA_MACRO";

		int bitsPerPixel = 8;

		int bitsPerOutput = 32;

		bool bHexOutput = true;

		// --------------------------------------------------------------------

		CreateBackup( m_szFormatTxt, ".backup" );

		// okay begin the output phase
		// ---------------------------------------------------------------------

		FILE * outFile = fopen( m_szFormatTxt, "wt" );

		if ( NULL == outFile ) return false;

		// Output the data (collecting packed offsets for each surface)
		// --------------------------------------------------------------------

		std::vector< int > frameOffsets;

		frameOffsets.reserve( m_SubImages.size() );

		{
			// output the data collection prolog
			// ----------------------------------------------------------------

			fprintf( outFile, "\n// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" );
			fprintf( outFile, "%s( %s )\n\n", pszImageDataProlog, m_szBaseName );

			// output the data
			// ----------------------------------------------------------------

			base_class::sub_image_collection_type::iterator it = m_SubImages.begin();
			base_class::sub_image_collection_type::iterator end = m_SubImages.end();

			int packedDataOffset = 0;

			int outputNumber = 0;

			while ( it != end ) {

				// Note the offset for this entry
				// ------------------------------------------------------------

				frameOffsets.push_back( packedDataOffset );

				// output the image data prolog
				// ------------------------------------------------------------

				fprintf(
					outFile
					,"// image %d, size (%dx%d) index %d\n\n"
					,outputNumber
					,(*it)->surface.Width()
					,(*it)->surface.Height()
					,packedDataOffset
				);

				// Output the data
				// ------------------------------------------------------------

				CSize subTileSize( (*it)->surface.Width(), (*it)->surface.Height() );

				// (FUTURE) the output 8, 32 should come from the settings dlg

				if ( !BPT_FILE_OUTPUT_HELPERS::T_SubTilePackedOutput8bppSurface(
					outFile, packedDataOffset, (*it)->surface, bHexOutput, bitsPerPixel, bitsPerOutput, subTileSize
					) ) {

					TRACE( "FAILURE: to output sub tiles\n" );

					fclose( outFile );

					return false;

				}

				// output the data image data epilog
				// ------------------------------------------------------------

				++it;

				++outputNumber;

				if ( it != end ) {

					fprintf( outFile, ",\n\n" );

				} else {

					fprintf( outFile, "\n\n" );

				}

			}

			// output the data collection epilog
			// ----------------------------------------------------------------

			fprintf( outFile, "%s( %s )\n", pszImageDataEpilog, m_szBaseName );

		}

		// Output the frames
		// --------------------------------------------------------------------

		{
			std::vector< int >::iterator offsetIt = frameOffsets.begin();

			// output the frame prolog
			// ----------------------------------------------------------------

			fprintf( outFile, "\n// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" );
			fprintf( outFile, "%s( %s )\n\n\t", pszFrameDataProlog, m_szBaseName );

			// output the frame entries
			// ----------------------------------------------------------------

			base_class::sub_image_collection_type::iterator it = m_SubImages.begin();
			base_class::sub_image_collection_type::iterator end = m_SubImages.end();

			while ( it != end ) {

				// Figure out the pitch for the sub image
				// ------------------------------------------------------------

				int nPixelsPerByte = 8 / bitsPerPixel;

				int rawBytesPerLine = (*it)->surface.Width() / nPixelsPerByte;

				int storagePitch = (((rawBytesPerLine * 8) + bitsPerOutput - 1) / bitsPerOutput);

				storagePitch = (storagePitch * bitsPerOutput) / 8;

				// ------------------------------------------------------------

				fprintf(
					outFile,
					"%s(\n"
					"\t\t%s\t\t// base name\n"
					"\t\t,%-6d\t\t// image data index\n"
					"\t\t,%-6d\t\t// line stride/pitch\n"
					"\t\t,%-6d\t\t// width\n"
					"\t\t,%-6d\t\t// height\n"
					"\t\t,%-6d\t\t// x offset\n"
					"\t\t,%-6d\t\t// y offset\n"
					"\t)\n"
					,pszDeclareFrameMacroName
					,m_szBaseName
					,*offsetIt
					,storagePitch
					,(*it)->surface.Width()
					,(*it)->surface.Height()
					,(*it)->deltaFromLinkPoint.x
					,(*it)->deltaFromLinkPoint.y
				);

				// ------------------------------------------------------------

				++it;

				++offsetIt;

				if ( it != end ) {

					fprintf( outFile, "\n\t," );

				} else {

					fprintf( outFile, "\n" );

				}

			}

			// output the frames epilog
			// ----------------------------------------------------------------

			fprintf( outFile, "%s( %s )\n", pszFrameDataEpilog, m_szBaseName );

		}

		// Output the animation control
		// --------------------------------------------------------------------

		fprintf( outFile, "\n// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n" );

		fprintf(
			outFile,
			"%s(\n"
			"\t%s\t// base name\n"
			"\t,%-6d\t// frame count\n"
			"\t,%-6d\t// link point x\n"
			"\t,%-6d\t// link point y\n"
			")\n"
			,pszDeclareAnimMacroName
			,m_szBaseName
			,m_SubImages.size()
			,m_LinkPoint.x
			,m_LinkPoint.y
		);

		// --------------------------------------------------------------------

		fclose( outFile );

		return true;

	}

	// ------------------------------------------------------------------------

	virtual LPCTSTR GetDefaultExt() {

		static char BASED_CODE szDefaultExt[] = "*.c";

		return szDefaultExt;

	}

	virtual LPCTSTR GetFileFilters() {

		static char BASED_CODE szFilter[] =
			"All Supported Formats (*.c,*.cpp,*.h)|*.c;*.cpp;*.h|"
			"C Files (*.c)|*.c|"
			"CPP Files (*.cpp)|*.cpp|"
			"H Files (*.h)|*.h|"
			"All Files (*.*)|*.*|"
			"|"
			;

		return szFilter;

	}

	// ------------------------------------------------------------------------

	virtual bool SettingsDialog( CWnd * pParent ) {

		CEnterStringDlg dlg( pParent );

		dlg.m_DescriptionString = "Enter base name for output";
		dlg.m_Title = "Base Name";
		dlg.m_EditString = m_szBaseName;

		if ( IDOK == dlg.DoModal() ) {

			strcpy( m_szBaseName, dlg.m_EditString );

		}

		return true;

	}

	virtual bool HasSettingsDialog() {

		return true;

	}

};

// ----------------------------------------------------------------------------

//
//	TExportBMPCodec
//

template< class T, const bool HASPALETTE = true >
class TExportBMPCodec : public BPT::IImportExportImageCodec {

private:

	char m_pFormatTxt[_MAX_PATH];

public:

	typedef BPT::IImportExportImageCodec base_class;

	// ------------------------------------------------------------------------

	TExportBMPCodec() {

		*m_pFormatTxt = '\0';

	}

	virtual char * GetName() {

		return "BMP files";
		
	}

	// ------------------------------------------------------------------------

	virtual bool Start(
		ImageInfo * pImageInfo
		,const char * pFormatTxt
		,const SIZE canvasSize
		,const int nFrames
		,const POINT linkPoint
	) {

		// Convert filename to sprintfable form
		
		char *p = m_pFormatTxt;
		int changed = false;
		
		while( *pFormatTxt ) {
			
			if ( !changed && *pFormatTxt == '#' ) {
				
				int count = 0;
				
				while( *pFormatTxt++ == '#' ) {
					
					count++;
				}
				
				*p++ = '%';
				*p++ = '0';
				*p++ = '0' + (count > 9?9:count);
				*p++ = 'd';
				
				changed = true;
				
				pFormatTxt--;
				
			} else {
				
				*p++ = *pFormatTxt++;
			}
			
		}
		
		*p = '\0';

		if ( strchr(m_pFormatTxt, '.') == 0 ) {

			strcat(m_pFormatTxt, ".bmp");
			
		}

		return true;

	}

	virtual bool Frame( ImageInfo * pImageInfo, const int nFrameIOID ) {

		// --------------------------------------------------------------------

		if ( (*m_pFormatTxt == '\0') || (!pImageInfo) || (this != pImageInfo->pOwnerCodec) ) {

			return false;

		}

		// --------------------------------------------------------------------

		char filename[ _MAX_PATH ];

		sprintf( filename, m_pFormatTxt, nFrameIOID );

		// --------------------------------------------------------------------

		T mappedSurface;

		if ( !mappedSurface.Map(
			(BYTE *)pImageInfo->pData, pImageInfo->size, pImageInfo->pitch, false, true ) ) {

			TRACE( "FAILURE::Map surface failed?\n" );

			return false;

		}

		// --------------------------------------------------------------------

		if ( HASPALETTE ) {

			return BPT::T_SaveSurfaceAsBMP<T>(
				filename, mappedSurface, pImageInfo->paletteData, 256
			);

		}
		
		return BPT::T_SaveSurfaceAsBMP<T>(
			filename, mappedSurface, 0, 0
		);

	}

};

//
//	CBpaintDoc::ExportFramesDialogHandler()
//

bool CBpaintDoc::ExportFramesDialogHandler( CWnd * pParent )
{

#if defined( DEMO_SAVE_DISABLED ) // BPT 7/1/01

	AfxMessageBox( "Save is disabled in this DEMO build", MB_OK );

	return true;

#else


	CExportFramesDlg dlg( pParent );

	// Setup the dlg members
	// ------------------------------------------------------------------------

	dlg.m_strRangePrefix = "Source Range";
	dlg.m_strDlgName = "Export frames dialog";
	dlg.m_nMin = 1;
	dlg.m_nMax = m_pAnimation->FrameCount();
	dlg.m_nSrcFrom = dlg.m_nMin;
	dlg.m_nSrcTo = dlg.m_nMax;
	dlg.m_pSrcAnimation = m_pAnimation;
	dlg.m_SrcSelectedLayers.push_back( m_pAnimation->CurrentLayer() );
	dlg.m_nStartNumber = GLOBAL_GetSettingInt( "ExportDlg.nStart", 1 );
	dlg.m_nIncrement = GLOBAL_GetSettingInt( "ExportDlg.nIncrement", 1 );
	dlg.m_strFormat = "path\\filename####.bmp";
	dlg.m_bUsePalette = FALSE;

	// Fill in the codecs collection
	// ------------------------------------------------------------------------

	TExportBMPCodec<editor_bitmap_type> exportEditorBMPCodec;

	dlg.m_CodecCollection.push_back( &exportEditorBMPCodec );

	TExport_C_Codec<editor_bitmap_type> export_C_Codec;

	dlg.m_CodecCollection.push_back( &export_C_Codec );

	// Do the modal dialog 
	// ------------------------------------------------------------------------

DO_OVER:

	dlg.m_pSelectedCodec = &exportEditorBMPCodec;

	if ( IDOK != dlg.DoModal() ) {

		return false;

	}

	GLOBAL_PutSettingInt( "ExportDlg.nStart", dlg.m_nStartNumber );
	GLOBAL_PutSettingInt( "ExportDlg.nIncrement", dlg.m_nIncrement );

	// ------------------------------------------------------------------------

	BPT::IImportExportImageCodec * pCodec = dlg.m_pSelectedCodec;

	if ( !pCodec ) {

		goto DO_OVER;

	}

	// ------------------------------------------------------------------------

	BPT::IImportExportImageCodec::ImageInfo * pImageInfo = pCodec->CreateImageInfo(
		m_CanvasSize
	);

	if ( !pImageInfo ) {

		return false;

	}

	// Finally perform the operation.
	// ------------------------------------------------------------------------

	int layerCount = m_pAnimation->LayerCount();

	if ( !pCodec->Start(
		pImageInfo, dlg.m_strFormat, m_CanvasSize,
		dlg.m_nSrcTo - dlg.m_nSrcFrom + 1,
		m_pAnimation->m_Link1
		) ) {

		TRACE( "FAILURE::Unable to start codec\n" );

		delete pImageInfo;

		return false;

	}

	int number = dlg.m_nStartNumber;

	// Create the layer selection functor.
	// ------------------------------------------------------------------------

	BPT::TIsInCollection<
		int,CExportFramesDlg::selected_layers_collection_type
	> fnLayerSelection( dlg.m_SrcSelectedLayers );

	// ------------------------------------------------------------------------

	editor_bitmap_type renderSurface;

	if ( !renderSurface.Create( m_CanvasSize.cx, m_CanvasSize.cy ) ) {

		TRACE( "Failure to create rendering surface (%dx%d)\n", m_CanvasSize.cx, m_CanvasSize.cy );

		pCodec->Abort( pImageInfo );
			
		delete pImageInfo;

		return false;

	}

	// Fill in the output image info, once as it will stay the same for
	// the entire output (for now anyway).
	// ------------------------------------------------------------------------

	pImageInfo->pData = (void *)renderSurface.Iterator( 0, 0 );
	pImageInfo->size = renderSurface.Size();
	pImageInfo->pitch = renderSurface.Pitch();

#if 1 
	pImageInfo->bpp = sizeof(editor_pixel_type) * 8; // bpt 4/20/03
#else
	pImageInfo->bpp = sizeof(editor_bitmap_type) * 8;
#endif

	// Fill in the palette information 
	// ------------------------------------------------------------------------

	// could use m_pAnimation->IsMultiPaletteAnimation() to react different...

#if 1 // BPT 6/15/01 

	// What about the layer palette does it have a place here?
	// If there are multiple layers involved then the issue of palette
	// gets a little muddled! 

	BPT::CAnnotatedPalette * pPal = GetCurrentPalette();
	
#else

	BPT::CAnnotatedPalette * pPal = m_pShowcase->GetDefaultPalette();

#endif

	// ------------------------------------------------------------------------

	if ( pPal ) {

		int nTotalColors = min( 256, pPal->TotalColors() );

		for ( int i = 0; i < nTotalColors; i++ ) {

			pImageInfo->paletteData[ i ].rgbReserved = 0;
			pImageInfo->paletteData[ i ].rgbRed = pPal->GetSlot_R( i );
			pImageInfo->paletteData[ i ].rgbGreen= pPal->GetSlot_G( i );
			pImageInfo->paletteData[ i ].rgbBlue = pPal->GetSlot_B( i );

		}

	}

	// ------------------------------------------------------------------------

#if 1 // BPT 6/18/01

	// Use a ROP that takes care to ignore palette's that are pushed at it
	// signs there is something odd going on...
	// ------------------------------------------------------------------------

	typedef BPT::TIgnoreConstructorAdaptorROP<
		editor_to_composite_clut_type *,
		BPT::TCopyROP< editor_pixel_type, BPT::CVisualElement::storage_type >
	> top_type;

#else

	typedef BPT::TCopyROP< editor_pixel_type, BPT::CVisualElement::storage_type > top_type;

#endif

	// ------------------------------------------------------------------------

	for ( int frame = dlg.m_nSrcFrom; frame <= dlg.m_nSrcTo; frame++ ) {

		int frameNumber = frame - 1;

		// Render the frame to hand off to the codec
		// --------------------------------------------------------------------

		renderSurface.ClearBuffer( m_ChromaKey );

		m_pAnimation->RenderFrame(
			renderSurface, frameNumber, 0, 0,
			top_type(), fnLayerSelection, 0, 0
		);

		// --------------------------------------------------------------------

		if ( !pCodec->Frame( pImageInfo, number ) ) {

			TRACE( "FAILURE::Codec process frame %d\n", number );

			pCodec->Abort( pImageInfo );

			delete pImageInfo;

			return false;

		}

		number += dlg.m_nIncrement;

	}

	if ( !pCodec->Finish( pImageInfo ) ) {
		
		TRACE( "FAILURE::Codec finish failed?\n");

		delete pImageInfo;

		return false;

	}

	delete pImageInfo;

	// Finally perform the operation.
	// ------------------------------------------------------------------------

#endif

	return true;
}

