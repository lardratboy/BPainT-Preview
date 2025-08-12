// CompositeFrameOpsDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "TCompositeOpBase.h"
#include "BPTRasterOps.h"
#include "CFOFlipSettingsDlg.h"

// include something new here

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Adaptor ROP's
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

template< class T, class O >
class TIgnoreCLUTAdaptor {

private:

	O m_Rop;

public:

	typedef typename O::dst_type dst_type;
	typedef typename O::src_type src_type;

	TIgnoreCLUTAdaptor() { /* empty */ }

	TIgnoreCLUTAdaptor( T param ) { /* empty */ }

	__forceinline dst_type & operator()( dst_type & d, const src_type & s ) {

		return m_Rop( d, s );

	}

	template< class TT > void SetColor( const TT v ) {

		m_Rop.SetColor( v );

	}

	// [first, last) to output (same semantics as std::copy())
	// --------------------------------------------------------------------

	template< class ForwardIt, class OutputIt >
	__forceinline void InputMajor( ForwardIt first, ForwardIt last, OutputIt output ) {

		m_Rop.InputMajor( first, last, output );

	}

	// [first, last) write value 's'
	// --------------------------------------------------------------------

	template< class OutputIt >
	__forceinline void OutputMajor( OutputIt first, OutputIt last, const src_type & s ) const {

		m_Rop.OutputMajor( first, last, s );

	}

};

// ============================================================================

//
//	CAddColorsCompositeOp
//

template<
	class T =
		/* TIgnoreCLUTAdaptor<
			CBpaintDoc::editor_to_composite_clut_type *, */
			BPT::TAddColorsTransferROP<
				CBpaintDoc::editor_pixel_type, 
				BPT::CVisualElement::storage_type
			>
		/* > */
>
class CAddColorsCompositeOp : public TSimpleCompositeLayersOp<T> {

private:

public:

	typedef TSimpleCompositeLayersOp<T> base_type;

	typedef typename base_type::layer_collection_type layer_collection_type;

	typedef typename base_type::SFrameRangeInfo SFrameRangeInfo;

	typedef typename base_type::ROP ROP;

	// ------------------------------------------------------------------------

	virtual const char * GetName() const {

		return "Add Color";

	}

	// ------------------------------------------------------------------------

	virtual bool Perform(
		class CBpaintDoc * pDocument, 
		BPT::CAnimation * pDstAnimation, 
		SFrameRangeInfo & dstRangeInfo, 
		layer_collection_type & dstLayersCollection,
		BPT::CAnimation * pSrcAnimation, 
		SFrameRangeInfo & srcRangeInfo, 
		layer_collection_type & srcLayersCollection,
		const int value
		) {

		// Set the ROP
		// --------------------------------------------------------------------

		ROP change;

		change.SetColor( (ROP::dst_type) value );

		ChangeROP( change );

		// --------------------------------------------------------------------

		return base_type::Perform(
			pDocument, 
			pDstAnimation, 
			dstRangeInfo, 
			dstLayersCollection,
			pSrcAnimation, 
			srcRangeInfo, 
			srcLayersCollection,
			value
		);

	}

	// --------------------------------------------------------------------
};

// ============================================================================

//
//	CCompositeLayersOp
//

template< 
	class T = /* TIgnoreCLUTAdaptor<
		CBpaintDoc::editor_to_composite_clut_type *, */
		BPT::TCopyROP< CBpaintDoc::editor_pixel_type, BPT::CVisualElement::storage_type >
	/* > */
>
class CCompositeLayersOp : public TSimpleCompositeLayersOp<T> {

private:

public:

	typedef TSimpleCompositeLayersOp<T> base_type;

	typedef typename base_type::layer_collection_type layer_collection_type;

	typedef typename base_type::SFrameRangeInfo SFrameRangeInfo;

	typedef typename base_type::ROP ROP;

	// ------------------------------------------------------------------------

	virtual const char * GetName() const {

		return "Combine layers";

	}

};

// ============================================================================

//
//	CSingleColorCompositeLayersOp
//

template<
	class T =
		/* TIgnoreCLUTAdaptor<
			CBpaintDoc::editor_to_composite_clut_type *, */
			BPT::TSingleColorTransferROP<
				CBpaintDoc::editor_pixel_type, 
				BPT::CVisualElement::storage_type
			>
		/* > */
>
class CSingleColorCompositeLayersOp : public TSimpleCompositeLayersOp<T> {

private:

public:

	typedef TSimpleCompositeLayersOp<T> base_type;

	typedef typename base_type::layer_collection_type layer_collection_type;

	typedef typename base_type::SFrameRangeInfo SFrameRangeInfo;

	typedef typename base_type::ROP ROP;

	// ------------------------------------------------------------------------

	virtual const char * GetName() const {

		return "Single Color";

	}

	// ------------------------------------------------------------------------

	virtual bool Perform(
		class CBpaintDoc * pDocument, 
		BPT::CAnimation * pDstAnimation, 
		SFrameRangeInfo & dstRangeInfo, 
		layer_collection_type & dstLayersCollection,
		BPT::CAnimation * pSrcAnimation, 
		SFrameRangeInfo & srcRangeInfo, 
		layer_collection_type & srcLayersCollection,
		const int value
		) {

		// Set the ROP
		// --------------------------------------------------------------------

		ROP change;

		change.SetColor( (ROP::dst_type) value );

		ChangeROP( change );

		// --------------------------------------------------------------------

		return base_type::Perform(
			pDocument, 
			pDstAnimation, 
			dstRangeInfo, 
			dstLayersCollection,
			pSrcAnimation, 
			srcRangeInfo, 
			srcLayersCollection,
			value
		);

	}

	// --------------------------------------------------------------------
};

// ============================================================================

//
//	COutlineCompositeLayersOp
//

template< 
	class T = /* TIgnoreCLUTAdaptor<
		CBpaintDoc::editor_to_composite_clut_type *, */
		BPT::TCopyROP< CBpaintDoc::editor_pixel_type, BPT::CVisualElement::storage_type >
	/* > */
>
class COutlineCompositeLayersOp : public TSimpleCompositeLayersOp<T> {

private:

	CBpaintDoc::editor_pixel_type m_ChromaKeyColor;

	CBpaintDoc::editor_pixel_type m_OutlineColor;

public:

	typedef TSimpleCompositeLayersOp<T> base_type;

	typedef typename base_type::layer_collection_type layer_collection_type;

	typedef typename base_type::SFrameRangeInfo SFrameRangeInfo;

	typedef typename base_type::ROP ROP;

	// ------------------------------------------------------------------------

	COutlineCompositeLayersOp() : m_OutlineColor( 0 ) { /* Empty */ }

	// ------------------------------------------------------------------------

	virtual const char * GetName() const {

		return "Outline Composite";

	}

	// ------------------------------------------------------------------------

	virtual bool PostFrameOp( CBpaintDoc::editor_bitmap_type & surface ) {

		// Ask the outline primitive to create an outlined bitmap
		// --------------------------------------------------------------------

		CBpaintDoc::editor_bitmap_type * pOutline = BPT::T_CreateOutlinedBitmap(
			surface, m_ChromaKeyColor, m_OutlineColor
		);

		if ( !pOutline ) {

			return false;

		}

		// Blit the outlined bitmap over the top of the incoming bitmap
		// --------------------------------------------------------------------

		BPT::T_Blit(
			surface, -1, -1, *pOutline,
			CBpaintDoc::paint_copy_op_type()
		);

		// Free up our temporary bitmap and we're done
		// --------------------------------------------------------------------

		delete pOutline;

		return true;

	}

	// --------------------------------------------------------------------

	virtual bool Perform(
		class CBpaintDoc * pDocument, 
		BPT::CAnimation * pDstAnimation, 
		SFrameRangeInfo & dstRangeInfo, 
		layer_collection_type & dstLayersCollection,
		BPT::CAnimation * pSrcAnimation, 
		SFrameRangeInfo & srcRangeInfo, 
		layer_collection_type & srcLayersCollection,
		const int value
		) {

		// Set the color
		// --------------------------------------------------------------------

		m_OutlineColor = (CBpaintDoc::editor_pixel_type)value;
		m_ChromaKeyColor = pDocument->GetClearColor();

		// --------------------------------------------------------------------

		return base_type::Perform(
			pDocument, 
			pDstAnimation, 
			dstRangeInfo, 
			dstLayersCollection,
			pSrcAnimation, 
			srcRangeInfo, 
			srcLayersCollection,
			value
		);

	}

};

// ============================================================================

/////////////////////////////////////////////////////////////////////////////
// CCompositeFrameOpsDlg dialog


CCompositeFrameOpsDlg::CCompositeFrameOpsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompositeFrameOpsDlg::IDD, pParent)
	, m_bIndependentLayersMode(FALSE)
{
	//{{AFX_DATA_INIT(CCompositeFrameOpsDlg)
	m_nColor = 0;
	m_nDstFrom = 0;
	m_nDstTo = 0;
	m_nSrcFrom = 0;
	m_nSrcTo = 0;
	//}}AFX_DATA_INIT

	m_bFirstTime = true;
	m_pShowcase = 0;
	m_pSrcAnimation = 0;
	m_pDstAnimation = 0;
	m_pSelectedOperation = 0;

}


void CCompositeFrameOpsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompositeFrameOpsDlg)
	DDX_Control(pDX, IDC_OP_COMBO, m_OperationsComboBox);
	DDX_Control(pDX, IDC_SRC_LAYERS_LIST, m_SrcLayersListBox);
	DDX_Control(pDX, IDC_DST_LAYERS_LIST, m_DstLayersListBox);
	DDX_Control(pDX, IDC_DST_ANIM_COMBO, m_DstAnimCombo);
	DDX_Control(pDX, IDC_SRC_ANIM_COMBO, m_SrcAnimCombo);
	DDX_Text(pDX, IDC_COLOR_EDIT, m_nColor);
	DDV_MinMaxInt(pDX, m_nColor, 0, 255);
	DDX_Text(pDX, IDC_DST_FROM_EDIT, m_nDstFrom);
	DDX_Text(pDX, IDC_DST_TO_EDIT, m_nDstTo);
	DDX_Text(pDX, IDC_SRC_FROM_EDIT, m_nSrcFrom);
	DDX_Text(pDX, IDC_SRC_TO_EDIT, m_nSrcTo);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_INDEPENDENT_LAYERS_MODE, m_bIndependentLayersMode);
}


BEGIN_MESSAGE_MAP(CCompositeFrameOpsDlg, CDialog)
	//{{AFX_MSG_MAP(CCompositeFrameOpsDlg)
	ON_CBN_SELCHANGE(IDC_SRC_ANIM_COMBO, OnSelchangeSrcAnimCombo)
	ON_CBN_SELCHANGE(IDC_DST_ANIM_COMBO, OnSelchangeDstAnimCombo)
	ON_CBN_SELCHANGE(IDC_OP_COMBO, OnSelchangeOpCombo)
	ON_BN_CLICKED(IDC_OP_SETTINGS, OnOpSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::CompositeFramesOperationDialogHandler()
//

bool 
CBpaintDoc::CompositeFramesOperationDialogHandler( CWnd * pParent )
{
	CCompositeFrameOpsDlg dlg( pParent );

	// Setup the dlg members
	// ------------------------------------------------------------------------

	dlg.m_pShowcase = m_pShowcase;
	dlg.m_pSrcAnimation = m_pAnimation;
	dlg.m_pDstAnimation = m_pAnimation;

	// Select the current frame & layer by default
	// ------------------------------------------------------------------------

	dlg.m_nSrcFrom = dlg.m_pSrcAnimation->CurrentFrame() + 1;
	dlg.m_nSrcTo = dlg.m_nSrcFrom;
	dlg.m_nDstFrom = dlg.m_pDstAnimation->CurrentFrame() + 1;
	dlg.m_nDstTo = dlg.m_nSrcFrom;
	dlg.m_SrcSelectedLayers.push_back( dlg.m_pSrcAnimation->CurrentLayer() );
	dlg.m_DstSelectedLayers.push_back( dlg.m_pDstAnimation->CurrentLayer() );

	// Add the layer operations to the operations collection
	// ------------------------------------------------------------------------

	dlg.m_Operations.clear();

///////////////////////////////////////////////////////////////////////////////

	// add the new "Add Colors" type

	CAddColorsCompositeOp<> addColorsOp;

	dlg.m_Operations.push_back( &addColorsOp );

	// add the composite type

	CCompositeLayersOp<> compositeLayersOp;

	dlg.m_Operations.push_back( &compositeLayersOp );

///////////////////////////////////////////////////////////////////////////////

	// add the single color composite type

	CSingleColorCompositeLayersOp<> singleColorCompositeOp;

	dlg.m_Operations.push_back( &singleColorCompositeOp );

///////////////////////////////////////////////////////////////////////////////

	// add the outline composite type

	COutlineCompositeLayersOp<> outlineCompositeOp;

	dlg.m_Operations.push_back( &outlineCompositeOp );

///////////////////////////////////////////////////////////////////////////////

	// add the flip composite type

	TFlipCompositeLayersOp<> flipCompositeOp;

	dlg.m_Operations.push_back( &flipCompositeOp );

///////////////////////////////////////////////////////////////////////////////

	// Set the default operation

	dlg.m_pSelectedOperation = &compositeLayersOp;

	// Do the modal dialog 
	// ------------------------------------------------------------------------

	DO_OVER:

	if ( IDOK != dlg.DoModal() ) {

		return false;

	}

	if ( !dlg.m_pSelectedOperation ) {

		goto DO_OVER;

	}

#if 1 // BPT 6/18/01

	if ( dlg.m_pDstAnimation->IsMultiPaletteAnimation() || dlg.m_pSrcAnimation->IsMultiPaletteAnimation() ) {

		HWND hwnd = (pParent) ? pParent->GetSafeHwnd() : AfxGetMainWnd()->GetSafeHwnd();

		UINT nResult = MessageBox( hwnd,
			"One of the animations involved in your operation has multiple palettes. "
			"Would you like to review your settings before proceeding with the operation?",
			"Multiple palette warning!",
			MB_YESNOCANCEL | MB_ICONQUESTION
		);

		if ( IDCANCEL == nResult ) {

			return false;

		}

		if ( IDYES == nResult ) {

			goto DO_OVER;

		}

	}

#endif

	// Finally perform the operation.
	// ------------------------------------------------------------------------

	if ( dlg.m_bIndependentLayersMode ) {

		// --------------------------------------------------------------------

		if ( dlg.m_DstSelectedLayers.size() != dlg.m_SrcSelectedLayers.size() ) {

			goto DO_OVER;

		}

		// The selection counts need to be equal to get here
		// --------------------------------------------------------------------

		CCompositeFrameOpsDlg::selected_layers_collection_type::iterator dstIt = 
			dlg.m_DstSelectedLayers.begin();

		CCompositeFrameOpsDlg::selected_layers_collection_type::iterator srcIt = 
			dlg.m_SrcSelectedLayers.begin();

		while ( srcIt != dlg.m_SrcSelectedLayers.end() ) {

			CCompositeFrameOpsDlg::selected_layers_collection_type srcSingle;
			CCompositeFrameOpsDlg::selected_layers_collection_type dstSingle;

			srcSingle.push_back( *srcIt );
			dstSingle.push_back( *dstIt );

			if ( !dlg.m_pSelectedOperation->Perform(
				this, 
				dlg.m_pDstAnimation, dlg.m_DstRangeInfo, srcSingle,
				dlg.m_pSrcAnimation, dlg.m_SrcRangeInfo, dstSingle,
				dlg.m_nColor
				) ) {

				MessageBox(
					AfxGetMainWnd()->GetSafeHwnd(),
					"An error occured while performing operation",
					"ERROR!", MB_ICONERROR | MB_OK
				);

			}

			dstSingle.clear();
			srcSingle.clear();

			++dstIt;
			++srcIt;

		}

	} else {

		// --------------------------------------------------------------------

		if ( !dlg.m_pSelectedOperation->Perform(
			this, 
			dlg.m_pDstAnimation, dlg.m_DstRangeInfo, dlg.m_DstSelectedLayers,
			dlg.m_pSrcAnimation, dlg.m_SrcRangeInfo, dlg.m_SrcSelectedLayers,
			dlg.m_nColor
			) ) {

			MessageBox(
				AfxGetMainWnd()->GetSafeHwnd(),
				"An error occured while performing operation",
				"ERROR!", MB_ICONERROR | MB_OK
			);

		}

	}

	// If we got this far return true that we did something.
	// ------------------------------------------------------------------------

	return true;

}

// ----------------------------------------------------------------------------
// CCompositeFrameOpsDlg message handlers

BOOL CCompositeFrameOpsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// ------------------------------------------------------------------------

	if ( !m_pShowcase ) {

		return FALSE;

	}

	m_bFirstTime = true;
	
	// Fill in the information areas
	// ------------------------------------------------------------------------

	SetupInformation( 
		m_pSrcAnimation, 
		IDC_SRC_FROM_EDIT, IDC_SRC_FROM_SPIN, 
		IDC_SRC_TO_EDIT, IDC_SRC_TO_SPIN, 
		m_SrcLayersListBox, m_SrcSelectedLayers,
		m_SrcRangeInfo, IDC_SRC_STATIC, "Source"
	);

	SetupInformation( 
		m_pDstAnimation, 
		IDC_DST_FROM_EDIT, IDC_DST_FROM_SPIN, 
		IDC_DST_TO_EDIT, IDC_DST_TO_SPIN, 
		m_DstLayersListBox, m_DstSelectedLayers,
		m_DstRangeInfo, IDC_DST_STATIC, "Dest"
	);

	// Fill in the animation combo boxes
	// ------------------------------------------------------------------------

	if ( !FillComboBoxWithAnimationList( m_SrcAnimCombo, m_pSrcAnimation ) ) {

		return FALSE;

	}

	if ( !FillComboBoxWithAnimationList( m_DstAnimCombo, m_pDstAnimation ) ) {

		return FALSE;

	}

	// Fill in the codec combo box
	// ------------------------------------------------------------------------

	operations_collection_type::iterator it;

	for ( it = m_Operations.begin(); it != m_Operations.end(); it++ ) {

		int item = m_OperationsComboBox.AddString( (*it)->GetName() );

		if ( CB_ERR != item ) {

			m_OperationsComboBox.SetItemDataPtr( item, (void *)*it );

		}

	}

	SelectOperation( m_pSelectedOperation );
	
	// Finish up
	// ------------------------------------------------------------------------

	m_bFirstTime = false;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ----------------------------------------------------------------------------

bool
CCompositeFrameOpsDlg::GetListBoxSelections(
	selected_layers_collection_type & collection, CListBox & listBox
)
{
	collection.clear();

	int selCount = listBox.GetSelCount();

	if ( !selCount ) {

		return true;

	}

	int itemCount = listBox.GetCount();

	for ( int i = 0; i < itemCount; i++ ) {

		if ( 0 < listBox.GetSel( i ) ) {

			collection.push_back( i );

		}

	}

	return true;
	
}

// ----------------------------------------------------------------------------

//
//	CCompositeFrameOpsDlg::SetupInformation()
//

bool CCompositeFrameOpsDlg::SetupInformation(
	BPT::CAnimation * pAnimation,
	const int fromEdit, const int fromSpin,
	const int toEdit, const int toSpin,
	CListBox & listBox, selected_layers_collection_type & collection,
	SFrameRangeInfo & rangeInfo, const int staticInfoID, const char * lpstrName
)
{
	// Fill in the selection collection with the list box selections
	// --------------------------------------------------------------------

	if ( !m_bFirstTime ) {

		if ( !GetListBoxSelections( collection, listBox ) ) {

			return false;

		}

	}

	// Get the frame limits
	// --------------------------------------------------------------------

	rangeInfo.m_nMin = 1;
	rangeInfo.m_nMax = pAnimation->FrameCount();

	// Get the current values and limit them to the animation limits
	// --------------------------------------------------------------------

	rangeInfo.m_nFrom = GetDlgItemInt( fromEdit );
	rangeInfo.m_nTo = GetDlgItemInt( toEdit );

	rangeInfo.m_nFrom = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, rangeInfo.m_nFrom ) );
	rangeInfo.m_nTo = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, rangeInfo.m_nTo ) );

	// --------------------------------------------------------------------

	CString infoText;

	infoText.Format( "%s: Frames %d to %d", lpstrName, rangeInfo.m_nMin, rangeInfo.m_nMax );

	SetDlgItemText( staticInfoID, infoText );

	// Set the edit controls to the limited value
	// --------------------------------------------------------------------

	SetDlgItemInt( fromEdit, rangeInfo.m_nFrom );
	SetDlgItemInt( toEdit, rangeInfo.m_nTo );

	// Fill in the frame information controls
	// --------------------------------------------------------------------

	CSpinButtonCtrl * pSrcFromSpin = (CSpinButtonCtrl *)GetDlgItem(fromSpin);

	if ( pSrcFromSpin ) {

		pSrcFromSpin->SetRange( rangeInfo.m_nMin, rangeInfo.m_nMax );
		pSrcFromSpin->SetPos( rangeInfo.m_nFrom );

	}

	CSpinButtonCtrl * pSrcToSpin = (CSpinButtonCtrl *)GetDlgItem(toSpin);

	if ( pSrcToSpin ) {

		pSrcToSpin->SetRange( rangeInfo.m_nMin, rangeInfo.m_nMax );
		pSrcToSpin->SetPos( rangeInfo.m_nTo );

	}

	// --------------------------------------------------------------------

	listBox.ResetContent();

	if ( pAnimation ) {

		// Validate the selection collection
		// --------------------------------------------------------------------

		int layerCount = pAnimation->LayerCount();

		selected_layers_collection_type::iterator validateIt;

		for ( validateIt = collection.begin(); validateIt != collection.end(); ) {

			if ( (*validateIt) >= layerCount ) {

				validateIt = collection.erase( validateIt );

			} else {

				++validateIt;

			}

		}

		// Fill in the list box
		// --------------------------------------------------------------------

		for ( int i = 0; i < layerCount; i++ ) {

			BPT::CLayerInfo * pLayerInfo = pAnimation->GetLayerInfo( i );

			if ( !pLayerInfo ) {

				return false;

			}

			char * pName = pLayerInfo->GetName();

			if ( !pName ) {

				return false;

			}

			int index;

			if ( LB_ERR == (index = listBox.AddString( pName )) ) {

				return false;

			}

			// Select the items in the selection collection

			selected_layers_collection_type::iterator it =
				std::find( collection.begin(), collection.end(), index
			);

			if ( collection.end() != it ) {

				listBox.SetSel( index, TRUE );

			}

		}

	}

	return true;
}

//
//	CCompositeFrameOpsDlg::FillComboBoxWithAnimationList()
//

bool 
CCompositeFrameOpsDlg::FillComboBoxWithAnimationList(
	CComboBox & comboBox, BPT::CAnimation * pSelectedAnim
)
{
	comboBox.ResetContent();

	int animCount = m_pShowcase->AnimationCount();

	BPT::CAnimation * pFirst = m_pShowcase->FirstAnimation();

	BPT::CAnimation * pAnim = pFirst;

	for ( int i = 0; i < animCount; i++ ) {

		int item = comboBox.AddString( pAnim->GetName() );

		if ( CB_ERR == item ) {

			return FALSE;

		}

		comboBox.SetItemData( item, (DWORD)pAnim );

		if ( pAnim == pSelectedAnim ) {

			comboBox.SetCurSel( item );

		}

		pAnim = m_pShowcase->NextAnimation( pAnim );

		if ( pAnim == pFirst ) {

			break;

		}

	}

	return true;
}

//
//	CCompositeFrameOpsDlg::CheckFrameRangeInfo()
//

bool CCompositeFrameOpsDlg::CheckFrameRangeInfo(
	const int fromEdit, const int toEdit, SFrameRangeInfo & rangeInfo
)
{
	// Get the current values and limit them to the animation limits
	// --------------------------------------------------------------------

	int m_nFrom = GetDlgItemInt( fromEdit );
	int m_nTo = GetDlgItemInt( toEdit );

	rangeInfo.m_nFrom = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, m_nFrom ) );
	rangeInfo.m_nTo = max( rangeInfo.m_nMin, min( rangeInfo.m_nMax, m_nTo ) );

	// Set the edit controls to the limited value
	// --------------------------------------------------------------------

	SetDlgItemInt( fromEdit, rangeInfo.m_nFrom );
	SetDlgItemInt( toEdit, rangeInfo.m_nTo );

	// --------------------------------------------------------------------

	return (m_nFrom == rangeInfo.m_nFrom) && (m_nTo == rangeInfo.m_nTo);

}

//
//	CCompositeFrameOpsDlg::OnOK()
//

void CCompositeFrameOpsDlg::OnOK() 
{

	// --------------------------------------------------------------------

	if ( !CheckFrameRangeInfo( 
		IDC_SRC_FROM_EDIT, IDC_SRC_TO_EDIT, m_SrcRangeInfo) ) {

		MessageBox( "Invalid frame range entered, Please validate change", "Yo!", MB_ICONQUESTION | MB_OK );

		CWnd * pWnd = GetDlgItem( IDC_SRC_FROM_EDIT );

		if ( pWnd ) {

			pWnd->SetFocus();

		}

		return;

	}

	// --------------------------------------------------------------------

	if ( !CheckFrameRangeInfo( 
		IDC_DST_FROM_EDIT, IDC_DST_TO_EDIT, m_DstRangeInfo) ) {

		MessageBox( "Invalid frame range entered, Please validate change", "Yo!", MB_ICONQUESTION | MB_OK );

		CWnd * pWnd = GetDlgItem( IDC_DST_FROM_EDIT );

		if ( pWnd ) {

			pWnd->SetFocus();

		}

		return;

	}

	// Validate the range sizes
	// ------------------------------------------------------------------------

	int srcOpLength = (m_SrcRangeInfo.m_nTo - m_SrcRangeInfo.m_nFrom) + 1;
	int dstOpLength = (m_DstRangeInfo.m_nTo - m_DstRangeInfo.m_nFrom) + 1;

	if ( srcOpLength != dstOpLength ) {

		// --------------------------------------------------------------------

		int sFrom = m_SrcRangeInfo.m_nFrom;

		int sTo = sFrom + dstOpLength - 1;

		if ( sTo > m_SrcRangeInfo.m_nMax ) {

			int delta = (sTo - m_SrcRangeInfo.m_nMax);

			sFrom -= delta;
			sTo -= delta;

		}

		if ( 1 > sFrom ) {

			sFrom = 1;

			sTo = min( m_SrcRangeInfo.m_nMax, (sFrom + dstOpLength - 1) );

		} else {

			sTo = sFrom + dstOpLength - 1;

		}

		// --------------------------------------------------------------------

		int dFrom = m_DstRangeInfo.m_nFrom;

		int dTo = dFrom + srcOpLength - 1;

		if ( dTo > m_DstRangeInfo.m_nMax ) {

			int delta = (dTo - m_DstRangeInfo.m_nMax);

			dFrom -= delta;
			dTo -= delta;

		}

		if ( 1 > dFrom ) {

			dFrom = 1;

			dTo = min( m_DstRangeInfo.m_nMax, (dFrom + srcOpLength - 1) );

		} else {

			dTo = dFrom + srcOpLength - 1;

		}

		// Build the message string
		// --------------------------------------------------------------------

		CString messageTxt;

		messageTxt.Format(
			"Invalid frame range entered, requested frame ranges need "
			"to be the same length (BTW: You either need to change the "
			"source range to [ %d to %d ] or the dest range [ %d to %d ] to "
			"fix this, depending on which range is correct).", sFrom, sTo, dFrom, dTo
		);

		MessageBox( 
			messageTxt, "Yo!", MB_ICONQUESTION | MB_OK 
		);

		CWnd * pWnd = GetDlgItem( IDC_SRC_FROM_EDIT );

		if ( pWnd ) {

			pWnd->SetFocus();

		}

		return;

	}

	// --------------------------------------------------------------------

	if ( !GetListBoxSelections( m_SrcSelectedLayers, m_SrcLayersListBox ) ) {

		MessageBox( "Unable to fetch src selections", "ERROR", MB_ICONERROR | MB_OK );

		CDialog::OnCancel();

		return;

	}

	if ( !GetListBoxSelections( m_DstSelectedLayers, m_DstLayersListBox ) ) {

		MessageBox( "Unable to fetch dst selections", "ERROR", MB_ICONERROR | MB_OK );

		CDialog::OnCancel();

		return;

	}

#if 1 // BPT 6/7/03

	if ( m_bIndependentLayersMode ) {

		if ( m_SrcSelectedLayers.size() != m_DstSelectedLayers.size() ) {

			MessageBox( "Selection count for source and dest layers must be the same", "WARNING", MB_ICONWARNING | MB_OK );

			m_DstLayersListBox.SetFocus();

			return;

		}

	}

#endif

	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CCompositeFrameOpsDlg::OnSelchangeSrcAnimCombo() 
{
	int item = m_SrcAnimCombo.GetCurSel();

	if ( CB_ERR != item ) {

		BPT::CAnimation * pAnim = (BPT::CAnimation *)m_SrcAnimCombo.GetItemDataPtr( item );

		if ( ((BPT::CAnimation *)-1) != pAnim ) {

			m_pSrcAnimation = pAnim;

			SetupInformation( 
				pAnim, 
				IDC_SRC_FROM_EDIT, IDC_SRC_FROM_SPIN, 
				IDC_SRC_TO_EDIT, IDC_SRC_TO_SPIN, 
				m_SrcLayersListBox, m_SrcSelectedLayers,
				m_SrcRangeInfo, IDC_SRC_STATIC, "Source"
			);

		}

	}
}

void CCompositeFrameOpsDlg::OnSelchangeDstAnimCombo() 
{
	int item = m_DstAnimCombo.GetCurSel();

	if ( CB_ERR != item ) {

		BPT::CAnimation * pAnim = (BPT::CAnimation *)m_DstAnimCombo.GetItemDataPtr( item );

		if ( ((BPT::CAnimation *)-1) != pAnim ) {

			m_pDstAnimation = pAnim;

			SetupInformation( 
				pAnim, 
				IDC_DST_FROM_EDIT, IDC_DST_FROM_SPIN, 
				IDC_DST_TO_EDIT, IDC_DST_TO_SPIN, 
				m_DstLayersListBox, m_DstSelectedLayers,
				m_DstRangeInfo, IDC_DST_STATIC, "Dest"
			);

		}

	}
}

void CCompositeFrameOpsDlg::SelectOperation( COperation * pSelectOperation )
{
	m_pSelectedOperation = pSelectOperation;

	int itemCount = m_OperationsComboBox.GetCount();

	if ( CB_ERR != itemCount ) {

		for ( int i = 0; i < itemCount; i++ ) {

			COperation * pOperation = (COperation *)m_OperationsComboBox.GetItemDataPtr( i );

			if ( ((COperation *)-1) != pOperation ) {

				if ( pOperation == m_pSelectedOperation ) {

					m_OperationsComboBox.SetCurSel( i );

					break;

				}

			}

		}

	}

	// Enable / disable the settings button
	// ------------------------------------------------------------------------

	CWnd * pWnd = GetDlgItem( IDC_OP_SETTINGS );

	if ( m_pSelectedOperation && pWnd ) {

		pWnd->EnableWindow( m_pSelectedOperation->HasSettingsDialog() ? TRUE : FALSE );

	} else if ( pWnd ) {

		pWnd->EnableWindow( FALSE );

	}

	// Change the message (this code should be a method...)
	// ------------------------------------------------------------------------

	pWnd = GetDlgItem( IDC_OPERATION_MSG );

	if ( m_pSelectedOperation && pWnd ) {

		pWnd->SetWindowText( m_pSelectedOperation->GetMsg() );

	} else if ( pWnd ) {

		pWnd->SetWindowText( "Error" );

	}

}

void CCompositeFrameOpsDlg::OnSelchangeOpCombo() 
{
	int item = m_OperationsComboBox.GetCurSel();

	if ( CB_ERR != item ) {

		COperation * pOperation = (COperation *)m_OperationsComboBox.GetItemDataPtr( item );

		if ( ((COperation *)-1) != pOperation ) {

			SelectOperation( pOperation );

		}

	}

}

void CCompositeFrameOpsDlg::OnOpSettings() 
{
	if ( m_pSelectedOperation ) {

		if ( m_pSelectedOperation->HasSettingsDialog() ) {

			m_pSelectedOperation->SettingsDialog( this );

			// Change the message (this code should be a method...)
			// ----------------------------------------------------------------
		
			CWnd * pWnd = GetDlgItem( IDC_OPERATION_MSG );
		
			if ( m_pSelectedOperation && pWnd ) {
		
				pWnd->SetWindowText( m_pSelectedOperation->GetMsg() );
		
			} else if ( pWnd ) {
		
				pWnd->SetWindowText( "Error" );
		
			}

		}

	}
}
