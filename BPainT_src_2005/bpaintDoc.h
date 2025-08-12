// bpaintDoc.h : interface of the CBpaintDoc class
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPAINTDOC_H__61A97432_27DA_4ABC_891E_BB7C09803162__INCLUDED_)
#define AFX_BPAINTDOC_H__61A97432_27DA_4ABC_891E_BB7C09803162__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

#include "BPTDib.h"
#include "BPTBlitter.h"
#include "BPTPaintLogic.h"
#include "BPTAnimator.h"

//#define CLUT_USE_C_ARRAY

// ----------------------------------------------------------------------------

class CActiveAnimationChange : public CObject {

public:

	DECLARE_DYNAMIC( CActiveAnimationChange )

	enum {

		NEW_ANIMATION			= 0x0001
		,OLD_ANIMATION			= 0x0002
		,FRAME_COUNT			= 0x0003
		,LAYER_COUNT			= 0x0004
		,NEW_EVERYTHING			= 0x0005
		,FRAME_CHANGE			= 0x0006
		,LAYER_CHANGE			= 0x0007
		,BOTH_CHANGE			= 0x0008
		,NEW_SPOT_ADJUSTMENT	= 0x0009
		,SPOT_DATA_CHANGE		= 0x000a

	};

	POINT m_OldSelection;
	POINT m_NewSelection;

public:

	CActiveAnimationChange() { /* Empty */ }

};

// ----------------------------------------------------------------------------

class CColorChange : public CObject {

private:

	CColorChange(); // Hidden

	int m_Slot;

public:

	DECLARE_DYNAMIC( CColorChange )

	enum {

		WHOLE_PALETTE_CHANGE	= 0
		,L_COLOR_CHANGE			= 1
		,R_COLOR_CHANGE			= 2
		,SLOT_RGB_CHANGE		= 3

	};

public:

	CColorChange( const int slot ) : m_Slot( slot ) { /* empty */ }

	int GetSlot() const {

		return m_Slot;

	}

};

// ----------------------------------------------------------------------------

class CEditorRectChange : public CObject {

public:

	DECLARE_DYNAMIC( CEditorRectChange )

private:

	RECT m_Rect;

public:

	enum {

		INVALIDATE_RECT				= 0
		,SYNC_VIEW_RECT				= 1
		,CENTER_VIEW_REQUEST		= 2
		,SILENT_INVALIDATE_RECT		= 3

	};

	CEditorRectChange() {

		SetRectEmpty( &m_Rect );

	}

	CEditorRectChange( const RECT & rect ) : m_Rect( rect ) {

		/* Empty */

	}

	const RECT & GetRect() const {

		return m_Rect;

	}

};

// ----------------------------------------------------------------------------

class CBpaintDoc : public CDocument
{
protected: // create from serialization only
	CBpaintDoc();
	DECLARE_DYNCREATE(CBpaintDoc)

public: // traits

	typedef CBpaintDoc this_type;

#if defined(BPAINT_CHUNKY_32BPP_VERSION)

	typedef BPT::PIXEL_RGB32 editor_pixel_type;

#elif defined(TEST_BPAINT_TRUE_COLOR_TEST) // attempt to test 32 bpp storage

	typedef BPT::PIXEL_8 editor_pixel_type;

#else

	typedef BPT::PIXEL_8 editor_pixel_type;

#endif

	typedef BPT::TSimpleBitmap<editor_pixel_type> editor_bitmap_type;

	typedef SDisplayPixelType composite_pixel_type;
	typedef BPT::TDIBSection<composite_pixel_type> dib_type;
	typedef BPT::TSimpleBitmap<composite_pixel_type> composite_bitmap_type;

	typedef BPT::TRectChangeMediator<this_type> rect_change_mediator_type;

	typedef BPT::TPaintLogic<
		CBpaintDoc 
		,editor_bitmap_type
		,rect_change_mediator_type
	> paint_logic_type;

	// Transfer operator helper typedef's
	// ------------------------------------------------------------------------

	typedef BPT::TCopyROP<composite_pixel_type> composite_copy_op_type;

	// paint related transfer operators
	// ------------------------------------------------------------------------

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

	// these will change eventually
	// based on alpha channel support. 
	// the ultimate question is chunky vs. planer (which is fastest?)

	typedef BPT::TCopyROP<editor_pixel_type> paint_copy_op_type;
	typedef BPT::TCopyROP<editor_pixel_type> paint_brush_replace_op_type;
	typedef BPT::TMultiModeBrushTransferROP<editor_pixel_type> paint_brush_transfer_op_type;
	typedef BPT::TChromakeyedDifferenceOP<editor_pixel_type> paint_brush_difference_op_type;

#else

	typedef BPT::TCopyROP<editor_pixel_type> paint_copy_op_type;
	typedef BPT::TCopyROP<editor_pixel_type> paint_brush_replace_op_type;
	typedef BPT::TMultiModeBrushTransferROP<editor_pixel_type> paint_brush_transfer_op_type;
	typedef BPT::TChromakeyedDifferenceOP<editor_pixel_type> paint_brush_difference_op_type;

#endif

	// 8bpp to output composite bit-depth transfer op
	// ------------------------------------------------------------------------

#if 1 // BPT 6/15/01

	typedef BPT::CAnnotatedPalette::clut_entry_type composite_clut_entry_type;
	typedef BPT::CAnnotatedPalette::clut_type editor_to_composite_clut_type;

#if defined( CLUT_USE_C_ARRAY )
#pragma message("The edit to composite clut is no longer c array capable!")
#endif

#else

//	typedef composite_pixel_type composite_clut_entry_type;

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

	typedef unsigned composite_clut_entry_type;

#else

	typedef int composite_clut_entry_type;

#endif

#if defined( CLUT_USE_C_ARRAY )

	typedef composite_clut_entry_type * editor_to_composite_clut_type;

#else

	typedef BPT::TPow2Clut<composite_clut_entry_type> editor_to_composite_clut_type;

#endif

#endif
	
	// ------------------------------------------------------------------------

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

	typedef BPT::TClutTransferROP<
		composite_pixel_type
		,editor_pixel_type
		,editor_to_composite_clut_type
	> editor_to_edit_only_op_type;  // is this evil to display the real color?

	typedef BPT::TSrcPredicateClutTransfer<
		composite_pixel_type
		,BPT::TIsNotValue<editor_pixel_type>
		,editor_pixel_type
		,editor_to_composite_clut_type
		,BPT::TAlpha32BppTransferROP<composite_pixel_type>
	> editor_to_composite_op_type;

	typedef BPT::TAlphaClutTransferROP<
		composite_pixel_type
		,BPT::CVisualElement::storage_type
		,editor_to_composite_clut_type
	> visual_to_composite_copy_type;

	typedef BPT::TAlpha32BppTransferROP<
		composite_pixel_type, 
		BPT::CVisualElement::storage_type
	> visual_element_to_composite_type;

	typedef BPT::T_DA_AlphaClutTransferROP<
		composite_pixel_type
		,BPT::CVisualElement::storage_type
		,editor_to_composite_clut_type
	> visual_to_foreground_type;

#else // defined(TEST_BPAINT_TRUE_COLOR_TEST)
	
	typedef BPT::TClutTransferROP<
		composite_pixel_type
		,editor_pixel_type
		,editor_to_composite_clut_type
	> editor_to_edit_only_op_type;

	typedef BPT::TSrcPredicateClutTransfer<
		composite_pixel_type
		,BPT::TIsNotValue<editor_pixel_type>
		,editor_pixel_type
		,editor_to_composite_clut_type
		,BPT::TCopyROP<composite_pixel_type>
	> editor_to_composite_op_type;

	typedef BPT::TClutTransferROP<
		composite_pixel_type
		,BPT::CVisualElement::storage_type
		,editor_to_composite_clut_type
	> visual_to_composite_copy_type;

	typedef BPT::TCopyROP<
		composite_pixel_type, 
		BPT::CVisualElement::storage_type
	> visual_element_to_composite_type;

	typedef BPT::TClutTransferROP<
		composite_pixel_type
		,BPT::CVisualElement::storage_type
		,editor_to_composite_clut_type
	> visual_to_foreground_type;

#endif // defined(TEST_BPAINT_TRUE_COLOR_TEST)

	// ------------------------------------------------------------------------

	typedef BPT::TClut5050TransferROP<
		composite_pixel_type
		,BPT::CVisualElement::storage_type
		,editor_to_composite_clut_type
	> visual_to_composite_5050_type;

	typedef BPT::TCopyROP<
		editor_pixel_type, 
		BPT::CVisualElement::storage_type
	> visual_to_editor_copy_type;

	// Scary designer pen stuff?
	// ------------------------------------------------------------------------

	typedef BPT::TPixelClipperObject<paint_logic_type::render_target_type> clipper_base_type;
	typedef BPT::TRectangleRenderer<paint_logic_type::render_target_type,clipper_base_type> rect_renderer_base_type;
	typedef BPT::TBrushRenderer<paint_logic_type::render_target_type,clipper_base_type> brush_renderer_base_type;

	// even uglier pixel override stuff (might be proof of insanity if you can
	// follow this code... BPT)
	// ----------------------------------------------------------------------------

	struct HACK_pixel_overrider_type { // BPT 6/2/01

	private:

		class CBpaintDoc * m_pDoc;

	public: // traits

		typedef paint_logic_type::render_target_type::surface_type::pixel_type pixel_type;

		typedef paint_logic_type::render_target_type render_target_type;

	public: // interface

		HACK_pixel_overrider_type( ) : m_pDoc( 0 ) { /* Empty */ }

		pixel_type OverridePixelValue(
			const pixel_type pixel, const int x, const int y
		);

		void Prepare( render_target_type & renderTarget );

		void SetDocument( class CBpaintDoc * pDoc ) {

			m_pDoc = pDoc;

		}

	};

	// ----------------------------------------------------------------------------

	typedef paint_logic_type::pen_base_type pen_base_type;

	typedef BPT::TFlexiblePen<
		pen_base_type
		,brush_renderer_base_type
		,rect_renderer_base_type
		,clipper_base_type
		,HACK_pixel_overrider_type
	> flexible_pen_base_type;

	// ----------------------------------------------------------------------------

// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 

	class CPickupBrushMediator * m_pPickupBrushMediator;

	typedef BPT::TOpRectRenderer<
		rect_renderer_base_type,
		paint_copy_op_type
	> basic_op_rect_renderer_type;

	typedef BPT::TEllipseBrushRenderer<
		brush_renderer_base_type,
		paint_copy_op_type
	> basic_ellipse_brush_renderer_type;

	typedef BPT::TRectangleBrushRenderer<
		brush_renderer_base_type,
		paint_copy_op_type
	> basic_rect_brush_renderer_type;

	typedef BPT::TRectangleBrushRenderer<
		brush_renderer_base_type,
		paint_copy_op_type
	> basic_rect_brush_renderer_type;

	typedef BPT::TBitmapBrushRenderer<
		brush_renderer_base_type,
		paint_brush_transfer_op_type
	> basic_bitmap_brush_renderer_type;

	typedef BPT::TBitmapBrushRenderer<
		brush_renderer_base_type,
		paint_brush_replace_op_type
	> replace_bitmap_brush_renderer_type;

	typedef BPT::TBitmapBrushRenderer<
		brush_renderer_base_type,
		paint_brush_difference_op_type
	> difference_bitmap_brush_renderer_type;


// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 

	// These types are used by "BPTTools.h" to implement the basic drawing tools
	// ----------------------------------------------------------------------------

	typedef BPT::TRubberbandToolBase<paint_logic_type> rubberband_tool_base_type;
	typedef BPT::TInstantToolBase<paint_logic_type> instant_tool_base_type;
	typedef BPT::TFreehandToolBase<paint_logic_type> freehand_tool_base_type;

	// ------------------------------------------------------------------------

	typedef BPT::TSRLCompressedImage< BPT::TSRLCompressor< composite_pixel_type > > compressed_composite_image_type;

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

	typedef BPT::TAlpha32BppTransferROP<
		composite_pixel_type, compressed_composite_image_type::storage_type
	> fg_compressed_composite_image_copy_type;

	typedef BPT::TAlpha32BppTransferROP<
		composite_pixel_type, composite_pixel_type
	> fg_composite_image_copy_type;

#else

	typedef BPT::TCopyROP<
		composite_pixel_type, compressed_composite_image_type::storage_type
	> fg_compressed_composite_image_copy_type;

	typedef BPT::TTransparentSrcTransferROP<
		composite_pixel_type, composite_pixel_type
	> fg_composite_image_copy_type;

#endif
	
	// ------------------------------------------------------------------------

public: // Attributes

#if 1 // HACK

	class CMakeStencilDlg * m_pPickupColorHackDlg;

	UINT m_nPickupColorHackMsg;

#endif

	// RONG:
	static BPT::CAnimationShowcase *s_clipboardShowcase;
	
	// ------------------------------------------------------------------------

public: // data (hack)

	// On finish edit mode variables
	// ------------------------------------------------------------------------

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

	enum ON_REPLACE_SIMILAR_ENUM {

		ON_REPLACE_DO_NOTHING		= 0
		,ON_REPLACE_ASK				= 1
		,ON_REPLACE_DONT_ASK		= 2

	};

	bool m_bSimpleEditMode;
	bool m_bReleaseUnusedAfterEachEdit;
	int m_nReplaceVisualElementMode;

#endif

private: // data

	// ------------------------------------------------------------------------

	enum SPOT_EDIT_MODE {

		SPOT_EDIT_NONE			= 0
		,SPOT_EDIT_ANIM_SPOT	= 1
		,SPOT_EDIT_ADJUSTMENT	= 2

	};

	SPOT_EDIT_MODE m_eSetNamedSpotMode;

	BPT::CNamedSpot * m_pCurrentNamedSpot;
	bool m_bShowSpotInfo;
	
	// ------------------------------------------------------------------------

	bool m_bImediateUpdatesForSilentRender;

	CPoint m_RawInputCoordinate;

	bool m_bUseMoreAccurateThumbnailRenderer;

	bool m_bEnableRenderThumbnailsHack;

	bool m_bShowAnimationBackdrop;

	bool m_bUseEditorChromakeyAsFatbitsColor;

	bool m_bAutoEraseEditMode;

	// --------------------------------------------------------------------

	bool m_bRenderLinkPointOverlays;
	bool m_bSetAnimLinkPoint;
	bool m_bSetFrameLinkPoint;
	int m_nSetFrameLinkPoint;

	// ------------------------------------------------------------------------

	struct CompositeModeInfo {

		CPoint m_LButtonLastPoint;
		CPoint m_LButtonDownPoint;
		CPoint m_RButtonLastPoint;
		CPoint m_RButtonDownPoint;

		UINT m_LastMouseFlags;

		bool m_LButtonDown;
		bool m_RButtonDown;

		bool m_bXMovementLocked;
		bool m_bYMovementLocked;

		CompositeModeInfo() : 
			m_LButtonLastPoint(0,0),
			m_LButtonDownPoint(0,0),
			m_RButtonLastPoint(0,0),
			m_RButtonDownPoint(0,0)
		{
			m_LButtonDown = false;
			m_RButtonDown = false;

			m_bXMovementLocked = false;
			m_bYMovementLocked = false;
		}

	} m_CompositeModeInfo;

	// ------------------------------------------------------------------------

public:

	enum MODE {

		NOT_PENDING			= -1
		,UNKNOWN			= 0
		,DRAWING			= 1
		,COMPOSITE			= 2

	};

private:

	MODE m_CurrentMode;
	MODE m_PendingMode;

	// ------------------------------------------------------------------------

	BPT::CAnimationShowcase * m_pShowcase;
	BPT::CAnimation * m_pAnimation;

	// ------------------------------------------------------------------------

	int m_PreviousDrawingTool;

	bool m_AutoFocus;

	bool m_FirstTime;
	bool m_bGridOn;

	BPT::CGridInfo m_InputGrid;

	rect_change_mediator_type * m_pEditorRectChangeMediator;

	CSize m_CanvasSize;
	CSize m_ThumbnailSize;

	paint_logic_type m_EditorLogic;

	pen_base_type * m_pCurrentPen;

	editor_pixel_type m_ChromaKey;
	composite_pixel_type m_CompositeChromaKey;
	composite_pixel_type m_CheckerBoardColorA;
	composite_pixel_type m_CheckerBoardColorB;

	bool m_bFillAcrossLayerBoundaries;

	// --------------------------------------------------------------------

	bool m_bColorStencilDefined;
	bool m_bColorStencilEnabled;
	bool m_bColorStencilTable[ 256 ];

	// --------------------------------------------------------------------

	bool m_bHasBackgroundLayers;
	bool m_bHasForegroundLayers;
	bool m_bUseBackdropSurface;

	// Foreground & Background canvas buffers
	// --------------------------------------------------------------------

	composite_bitmap_type m_BackgroundSurface;
	composite_bitmap_type m_ForegroundSurface;
	composite_bitmap_type m_BackdropSurface;

	compressed_composite_image_type m_CompressedForegroundImage;

	// Editor to composite clut
	// --------------------------------------------------------------------

	editor_to_composite_clut_type m_Edit2CompositeClut;

	// The composite & editor display dib's
	// --------------------------------------------------------------------

	dib_type m_CompositeViewDib;
	dib_type m_EditViewDib;

	// --------------------------------------------------------------------

// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 

#if 1 // HACK

	composite_pixel_type m_AnimationLinkPointColor;
	composite_pixel_type m_FrameLink1Color;
	composite_pixel_type m_FrameLink2Color;
	composite_pixel_type m_FrameLink3Color;
	composite_pixel_type m_FrameLink4Color;

	int m_PendingMultiViewData;

	bool m_bUseBackgroundRendererInEditOnlyViews;
	bool m_bOnionSkinMode;
	bool m_bDrawForeground5050;

	bool m_bHasPerformanceTimer;
	__int64 m_QueryPerformanceFrequency;
	int m_CheeseRenderTookValue;

	CPoint m_ptLastKnownLocation;
	bool m_bUseBitmapBrushRenderer_HACK;

	CPoint m_PenPos;
	CSize m_PenSize;
	CSize m_LastPenSize;

	int m_BrushChromaKey;
	int m_Color_L;
	int m_Color_R;

	bool m_bDrawingButtonIsDown;

#endif

	basic_op_rect_renderer_type m_OpRectRenderer;

	basic_rect_brush_renderer_type m_RectBrushRenderer;
	basic_bitmap_brush_renderer_type m_BitmapBrushRenderer;
	basic_ellipse_brush_renderer_type m_EllipseBrushRenderer;
	difference_bitmap_brush_renderer_type m_DifferenceBitmapBrushRenderer;

	//replace_bitmap_brush_renderer_type m_ReplaceBitmapBrushRenderer;

	flexible_pen_base_type m_FlexiblePen;

	int m_nBrushPaintROP;

	HACK_pixel_overrider_type m_HackPixelOverrider; // BPT 6/2/01

// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 
// DESIGN ************************************************************************* 

	// --------------------------------------------------------------------

private: // internal

	void SyncAnimationPlaybackRate() {

		if ( m_pAnimation ) {

			GLOBAL_PutSettingInt( 
				"AnimationPlaybackDelay", m_pAnimation->GetPlaybackDelay()
			);

		}

	}

	bool ChangeCanvasSizeInternals( const CSize size );

	bool OkayToUseToolAfterPickupBrush( const int tool );

	int Standardize_DTOOL_Command( const int id );
	int Standardize_QBRUSH_Command( const int id );
	int Standardize_COLOR_Command( const int id );

	pen_base_type * GetLButtonPen();
	pen_base_type * GetRButtonPen();

	void RefreshPen();

	// Layout fun
	// --------------------------------------------------------------------

	const char * UILayoutSettingsStringWrapper( const char * str );

	void LoadUILayout( const int layout, const bool destroyExistingUI = true );

	bool UILayout_1( CBpaintApp * pApp );
	bool UILayout_2( CBpaintApp * pApp );
	bool UILayout_3( CBpaintApp * pApp );
	bool UILayout_4( CBpaintApp * pApp );
	bool UILayout_5( CBpaintApp * pApp );
	bool UILayout_6( CBpaintApp * pApp );
	bool UILayout_7( CBpaintApp * pApp );
	bool UILayout_8( CBpaintApp * pApp );
	bool UILayout_9( CBpaintApp * pApp );
	bool UILayout_0( CBpaintApp * pApp );

	// Cursor resources
	// --------------------------------------------------------------------

	HCURSOR m_hDefaultCursor;
	HCURSOR m_hFloodCursor;
	HCURSOR m_hFreehandSingleCursor;
	HCURSOR m_hEyeDropperCursor;
	HCURSOR m_hMoveCelCursor;
	HCURSOR m_hOffCanvasDrawingCursor;

	// Overlay hacks
	// --------------------------------------------------------------------

	bool m_bRenderOverlays;

	paint_logic_type::overlay_info m_CurrentOverlay;

	void ClearOverlayType();
	void SetOverlayInfo( const paint_logic_type::overlay_info * pOverlayInfo );
	void EraseOverlay();
	void RepaintOverlayArea( const bool bRender );
	void SilentRender( const RECT & rect );

	void RenderOverlay(
		dib_type::bitmap_type * pCanvasBitmap, const RECT * pClipRect
	);

	// --------------------------------------------------------------------

	void AutoSelectBrushPickupTool( const bool showMsg = false ) {

		CWnd * pMain = AfxGetMainWnd();

		if ( pMain ) {

			if ( showMsg ) {
	
				pMain->MessageBox( "Pickup a brush", "YO!", MB_OK );
	
			}
	
			pMain->PostMessage( WM_COMMAND, ID_DTOOL_PICKUP_BRUSH, 0 );

		}

	}

	// --------------------------------------------------------------------

	BOOL KickStart();

	// --------------------------------------------------------------------

	bool IsDrawingModePossible() {

		// eventually this will aid in disabling edit mode under certain
		// conditions but now it's hacked to always return true.

		return true;

	}

	// --------------------------------------------------------------------

	class CViewTracker {

		typedef CView * view_pointer;

	private:

		view_pointer & m_rTracker;

		view_pointer m_pView;

		CViewTracker(); // Hidden

	public:

		CViewTracker( view_pointer & rTracker, CView * pView ) : m_rTracker( rTracker ) {

			m_pView = pView;

			if ( !m_rTracker ) {

				m_rTracker = pView;

			}

		}

		~CViewTracker() {

			if ( m_pView == m_rTracker ) {

				m_rTracker = 0;

			}

		}

	};

	CView * m_pTemporaryTrackedView;

	// --------------------------------------------------------------------

	void StoreCurrentLayoutAs( const char * name );
	bool LoadCustomLayoutFrom( const char * name );

	// --------------------------------------------------------------------

public: // interface

	void GetGridInfo( BPT::CGridInfo * pGridInfo ) {

		if ( pGridInfo ) *pGridInfo = m_InputGrid;

	}

	void GetPixelAspectRatio( CSize * pSize ) {

		if ( !pSize) return;

		if ( !m_pShowcase ) {
			
			*pSize = CSize( 1, 1 );

			return;

		}

		*pSize = m_pShowcase->GetPixelAspectRatio();

	}

	// --------------------------------------------------------------------

	editor_pixel_type HACK_PenOverridePixelValue( 
		const editor_pixel_type pixel, const int x, const int y 
	);

	// --------------------------------------------------------------------

	void SetShowLinkPointsAndSpotsMode( const bool bOnOff ) {

		m_bRenderLinkPointOverlays = bOnOff;

	}

	void SetCurrentNamedSpot( BPT::CNamedSpot * pNamedSpot ) {

		m_pCurrentNamedSpot = pNamedSpot;

	}

	BPT::CNamedSpot * GetCurrentNamedSpot() const {

		return m_pCurrentNamedSpot;

	}

	// ------------------------------------------------------------------------

	CPoint GetLastRawInputCoordinate() const {

		return m_RawInputCoordinate;

	}

	// ------------------------------------------------------------------------

	BPT::CAnimationShowcase * GetShowcaseObject() {

		return m_pShowcase;

	}

	// ------------------------------------------------------------------------

	void ForceFullRedraw() {
	
		AddDirtyRect( m_ForegroundSurface.Rect() );
	}

	// ------------------------------------------------------------------------

	bool EnsureProperMenu( CWnd * pWnd, const int nIDResource = IDR_BPAINTTYPE );

	// ------------------------------------------------------------------------

	int GetPendingMultiViewData() const {

		return m_PendingMultiViewData;

	}

	void SetPendingMultiViewData( const int value ) {

		m_PendingMultiViewData = value;

	}

	// ------------------------------------------------------------------------

	bool PickupColor( const int x, const int y, editor_pixel_type * pOptionalOutValue );

	// ------------------------------------------------------------------------

	int CountCanvasViewsUsingDib( dib_type * pQueryDib );
	int ActiveEditViewCount();
	int ActiveCompositeViewCount();

	// Get the paint logic object / brush helper methods
	// ------------------------------------------------------------------------

	paint_logic_type::pointer GetPaintLogicPtr() {

		return &m_EditorLogic;

	}

	void BrushHasChanged( const bool bSetChromakey = false, const int * pOptionalNewChromakey = 0 );

	void SetBitmapBrushMode( const bool bEnable );

	SIZE GetCanvasSize();

	CSize GetThumbnailSize();

	CSize GetDrawingBrushSize();
	void SetDrawingBrushSize( CSize & size );

	bool CanBrushDoubleSize( const bool bWidth, const bool bHeight );

	// Color change
	// --------------------------------------------------------------------

	editor_pixel_type GetClearColor();
	void Request_L_ColorChange( const int slot );
	void Request_R_ColorChange( const int slot );
	void Request_ColorRGBEdit( const int slot, CWnd * pCenterWnd = 0 );

	void Request_FullPaletteChange(
		BPT::CAnnotatedPalette * pFrom, 
		const bool bRepaint, const bool bRepaintThumbnails
	);

	void Request_ColorRGBChange(
		const int slot, const int r, const int g, const int b, 
		const bool bRepaint, const bool bRepaintThumbnails = true
	);

	SDisplayPixelType GetDisplayColorForPaletteSlot( const int slot ) {

#if 1 // BPT 6/16/01

		return (*GetCurrentCLUT())[ slot ];

#else

		return m_Edit2CompositeClut[ slot ];

#endif

	}

	BPT::CAnnotatedPalette * GetCurrentPalette();

	void BuildEditToCompositeClut( const bool bForceRepaint = false );

	int Get_L_Color() const {

		return m_Color_L;

	}

	int Get_R_Color() const {

		return m_Color_R;

	}

	bool ConfirmMultiplePaletteBehavior( // 6/15/01
		const char * pMsgTitle = 0, 
		const bool bUserCanCancel = true,
		const bool bBroadCastPaletteChange = true
	);

	// My oh my what fun this is
	// ------------------------------------------------------------------------

	BPT::CAnnotatedPalette * GetShowcasePalette() {

		if ( !m_pShowcase ) return 0;

		return m_pShowcase->GetDefaultPalette();
		
	}

	BPT::CAnnotatedPalette * GetAnimationPalette() {

		if ( !m_pAnimation ) return 0;

		return m_pAnimation->GetPalette();
		
	}

	editor_to_composite_clut_type * GetShowcaseCLUT() {

#if 1 // BPT 6/15/01

		BPT::CAnnotatedPalette * pPal = GetShowcasePalette();

		if ( !pPal ) {

			return &m_Edit2CompositeClut;

		}

		return pPal->GetDisplayCLUT();

#else

		return &m_Edit2CompositeClut;

#endif

	}

	editor_to_composite_clut_type * GetCurrentCLUT() {

#if 1 // BPT 6/15/01

		BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

		if ( !pPal ) {

			return &m_Edit2CompositeClut;

		}

		return pPal->GetDisplayCLUT();

#else

		return &m_Edit2CompositeClut;

#endif

	}

	CString GetCurrentPaletteSourceName() {

		BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

#if defined(BPT_SHOWCASE_MANAGED_PALETTES)

		if ( pPal ) {

			char * pName = pPal->GetName();

			if ( pName ) {

				return CString( pName );

			}

			CString generatedName;

			generatedName.Format( "Palette %p", pPal );

			return generatedName;

		}

		return CString( "No Palette?" );

#else

		if ( GetShowcasePalette() == pPal ) {
	
			return CString( "SHOWCASE Palette" );
	
		} else if ( GetAnimationPalette() == pPal ) {
	
			return CString( "ANIMATION Palette" );
	
		}
	
		return CString( "LAYER Palette" );

#endif

	}

	// ------------------------------------------------------------------------

	bool UseEditorChromakeyAsFatbitsColor() const { // BPT 6/28/01

		return m_bUseEditorChromakeyAsFatbitsColor;

	}

	editor_pixel_type GetEditorChromakey() const { // BPT 6/28/01

		return m_ChromaKey;

	}

	editor_pixel_type GetBrushChromakey() const { // BPT 10/9/02

		return m_BrushChromaKey;

	}

	composite_pixel_type GetEditorChromakeyDisplayValue() { // BPT 6/28/01

		editor_to_composite_clut_type * pClut = GetShowcaseCLUT();

		if ( !pClut ) {

			return 0;

		}

		return (*pClut)[ m_ChromaKey ];

	}
		
	// this is the 'display' canvas
	// ------------------------------------------------------------------------

	dib_type * M_GetCompositeViewDib() {

		return &m_CompositeViewDib;

	}

	dib_type * M_GetEditViewDib() {

		return &m_EditViewDib;

	}

	dib_type * M_GetAnimationViewDib() {

		return 0; // how should this be handled?

	}

	// --------------------------------------------------------------------

	void SetStatusBarText( const CString & str, const int nPane = 0, const bool bUpdate = true );

	void UpdateStatusBarWithEditMsgInfo( const UINT nFlags, const CPoint point );

	HCURSOR GetViewCursor( CView * pView, const CPoint point, const UINT nHitTest );

	// Paint system logic feedback
	// --------------------------------------------------------------------

	void Feedback(
		paint_logic_type * pLogic
		,const paint_logic_type::FEEDBACK id
		,void * pData
	);

	// Rectangle change handler
	// --------------------------------------------------------------------

	void AddDirtyRect( const RECT & rect );

	// --------------------------------------------------------------------

	void RenderBackgroundRect( dib_type::bitmap_type & surface, const RECT & rect );

	// --------------------------------------------------------------------

	DWORD m_dwSavedDoDadFlags;
	DWORD m_dwDoDadFlags;

	composite_pixel_type m_BoundingRectColor;
	composite_pixel_type m_SelectedBoundingRectColor;

	composite_pixel_type m_HandleColor;
	composite_pixel_type m_SelectedHandleColor;

	composite_pixel_type m_OpaqueColor;
	composite_pixel_type m_SelectedOpaqueColor;

	enum DODADS {

		NONE					= 0x0000
		,BOUNDING_RECTS			= 0x0001
		,EXPAND_BOUNDING_RECT	= 0x0002
		,HANDLES				= 0x0004
		,OPAQUE_MODE			= 0x0008
		,RENDER_SELECTED		= 0x0010
		,DISABLE_IMAGE_RENDER	= 0x0020

	};

	void 
	RenderCompositeModePrimEx(
		dib_type::bitmap_type & surface, const RECT & rect
		,bool bOnlyActiveLayer
		,bool bUseVisibilityInfo
		,DWORD dwDodadFlags
		,int nFirstLayer
		,int nLastLayer
		,bool bRenderBackground
		,bool bForegroundMode
	);

	void RenderCompositeModePrim(
		dib_type::bitmap_type & surface, const RECT & rect, 
		bool bOnlyActiveLayer = false,
		bool bUseVisibilityInfo = true,
		DWORD dwDodadFlags = 0
	);

	// --------------------------------------------------------------------

	void RenderLinkPointPrim( 
		dib_type::bitmap_type & surface, const POINT & pt, 
		const composite_pixel_type color, const RECT * pClipRect,
		CDC * pDC, const char * lpszPrefixText
	);

	void RenderLinkPoints(
		dib_type::bitmap_type & surface, const RECT * pClipRect, CDC * pDC
	);

	bool CalcLinkPointRect( CRect & outRect, const POINT & pt );

	bool SetLinkPointMode();
	void SetLinkPointPrim( POINT & pointStorage, const CPoint point );
	void SetLinkPoint( const CPoint point );

	// Composite / display renderer
	// --------------------------------------------------------------------

	bool IsLayerVisible( const int nLayer );

	void RenderCompositeModeRect( const RECT & rect );
	void RenderEditCompositeRect( const RECT & rect );
	void RenderEditOnlyRect( const RECT & rect );
	void RenderRect( const RECT & rect );

	// Edit outline render helper
	// --------------------------------------------------------------------

	void CBpaintDoc::RenderEditLayerOutline(
		composite_bitmap_type * pCanvasBitmap,
		editor_bitmap_type * pEditorBitmap,
		const RECT * pRect
	);

	// 'mode' setting methods.
	// --------------------------------------------------------------------

	void PENDING_SetDocumentMode( enum MODE mode );

	bool SetDocumentMode( enum MODE mode );

	bool EnsureActiveEditor();

	bool EnsureAnimationModeIsHalted();

	// --------------------------------------------------------------------

	bool FinishEdit();
	void DiscardEdit();
	bool BeginEdit();
	bool ClipCheckConfirmEditRequest();

	// Editor view messages? (mouse etc)
	// --------------------------------------------------------------------

	void HandleAutoFocus( CView * pView );

	void MassageEditMessageInfo( CView * pView, UINT & nFlags, CPoint & point);

	void PostEditToolProcessing();
	
	void Dispatch_OnLButtonDown(CView * pView, UINT nFlags, CPoint point);
	void Dispatch_OnLButtonUp(CView * pView, UINT nFlags, CPoint point);
	void Dispatch_OnMouseMove(CView * pView, UINT nFlags, CPoint point);
	void Dispatch_OnRButtonDown(CView * pView, UINT nFlags, CPoint point);
	void Dispatch_OnRButtonUp(CView * pView, UINT nFlags, CPoint point);
	void Dispatch_OnMButtonDown(CView * pView, UINT nFlags, CPoint point);
	void Dispatch_OnMButtonUp(CView * pView, UINT nFlags, CPoint point);
	void Dispatch_OnCancelMode(CView * pView);

	// --------------------------------------------------------------------

	void Composite_OnLButtonDown(CView * pView, UINT nFlags, CPoint point);
	void Composite_OnLButtonUp(CView * pView, UINT nFlags, CPoint point);
	void Composite_OnMouseMove(CView * pView, UINT nFlags, CPoint point);
	void Composite_OnRButtonDown(CView * pView, UINT nFlags, CPoint point);
	void Composite_OnRButtonUp(CView * pView, UINT nFlags, CPoint point);
	void Composite_OnMButtonDown(CView * pView, UINT nFlags, CPoint point);
	void Composite_OnMButtonUp(CView * pView, UINT nFlags, CPoint point);
	void Composite_OnCancelMode(CView * pView);

	// --------------------------------------------------------------------

	void Composite_ShiftCurrentLayer( const int dx, const int dy );

	// --------------------------------------------------------------------

	bool m_bClickSelectCompositeMode; // BPT 6/2/01
	bool Composite_ClickSelectLayerMode(); // BPT 6/2/01
	void Composite_ClickSelectLayer(CView * pView, UINT nFlags, CPoint point); // BPT 6/2/01

	// --------------------------------------------------------------------

	CFrameWnd * m_pPalFrameWnd;

	void OnDestroyPaletteView();

	// --------------------------------------------------------------------

	CFrameWnd * m_pPopupGridFrameWnd;

	void OnDestroyPopupGridView( CView * pView );

	// --------------------------------------------------------------------

	void Request_PreviousDrawingTool();

	void Request_LayerProperties( const int nLayer );
	void Request_FrameProperties( const int nFrame );
	void Request_AnimationProperties( CWnd * pOwner ); // BPT 6/5/02

	void Request_FullChange( BPT::CAnimation * pNew, const int nFrame, const int nLayer );

	void Request_SmartFrameLayerChange( const int nFrame, const int nLayer );

	void Request_AnimationChange( BPT::CAnimation * pNew );
	void Request_PrevAnimation( BPT::CAnimation * pFrom = 0 );
	void Request_NextAnimation( BPT::CAnimation * pFrom = 0 );

	void Request_FrameChange( const int nFrame );
	void Request_PrevFrame();
	void Request_NextFrame();
	void Request_FirstFrame();
	void Request_LastFrame();

	void Request_LayerChange( const int nLayer );
	void Request_PrevLayer();
	void Request_NextLayer();

	BPT::CAnimation * CurrentAnimation();

	// --------------------------------------------------------------------

	void BuildBackgroundLayersSurface();
	void BuildForegroundLayersSurface();

	// --------------------------------------------------------------------

	void OnCompositeFrameLink( const int nLink );

	// Internal dialog box helpers.
	// --------------------------------------------------------------------

	bool CompositeFramesOperationDialogHandler( CWnd * pParent );
	bool CopyFramesDialogHandler( CWnd * pParent );
	bool ExportFramesDialogHandler( CWnd * pParent );
	bool ImportFramesDialogHandler( CWnd * pParent );
	bool ClearLayersDialogHandler( CWnd * pParent );
	bool TranslateFrameLayersDialogHandler( CWnd * pParent );
	bool AnimationFrameNotesDialogHandler( CWnd * PParent );
	bool TextToBrushDialogHandler( CWnd * pParent );
	
	// Thumbnail helper methods
	// --------------------------------------------------------------------

	template< class SURFACE, class FUNCTOR >
	void __forceinline T_RenderFrameLayersHelper(
		SURFACE & surface, const int nFrame, FUNCTOR & fn
	) {

		if ( m_pAnimation ) {

			visual_to_composite_copy_type top( GetShowcaseCLUT() );

			m_pAnimation->RenderFrame( surface, nFrame, 0, 0, top, fn, 0, 0 );

		}

	}

	bool
	/* CBpaintDoc:: */ RenderThumbnail(
		composite_bitmap_type & canvasSizedWorkBitmap, composite_pixel_type backgroundColor,
		const int nFrame, const int nLayerStart, const int nLayerEnd,
		composite_bitmap_type * pBitmap, const RECT & location, 
		const RECT * pClip
	);

	void 
	/* CBpaintDoc:: */ CopyFrames(
		BPT::CAnimationShowcase * pDstShowcase, // BPT 6/1/01
		BPT::CAnimation *dstAnimation,
		int dstFrameAt, int dstLayerAt,
		BPT::CAnimationShowcase * pSrcShowcase, // BPT 6/1/01
		BPT::CAnimation *srcAnimation,
		int srcFrameFrom, int srcFrameTo,
		int srcLayerFrom, int srcLayerTo
	);

	void OnAnimProperties2( CWnd * pOwner ); // BPT 6/5/02

	void CBpaintDoc::OnEditPalette2(
		CWnd * pOwner
		,BPT::CAnnotatedPalette * pPalette
		,const bool bCanRepaint
		,const bool bModifyDocumentPalette
	);

	void LoadCanvasFromPicture( BPT::C8bppPicture * pPictureHack ); // BPT 9/18/02

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBpaintDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnChangedViewList();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBpaintDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBpaintDoc)
	afx_msg void OnViewAutoFocus();
	afx_msg void OnUpdateViewAutoFocus(CCmdUI* pCmdUI);
	afx_msg void OnEditPalette();
	afx_msg void OnUpdateEditPalette(CCmdUI* pCmdUI);
	afx_msg void OnHackLoadBackground();
	afx_msg void OnHackLoadForeground();
	afx_msg void OnEditStencil();
	afx_msg void OnAnimSelectDialog();
	afx_msg void OnAnimDeleteFrames();
	afx_msg void OnAnimInsertFrames();
	afx_msg void OnAnimInsertLayers();
	afx_msg void OnFrameTransformations();
	afx_msg void OnFrameClear();
	afx_msg void OnLayerClear();
	afx_msg void OnPaintTrimEdge();
	afx_msg void OnUpdatePaintTrimEdge(CCmdUI* pCmdUI);
	afx_msg void OnBrushRestore();
	afx_msg void OnBrushRotate90();
	afx_msg void OnBrushHalveSize();
	afx_msg void OnBrushDoubleSize();
	afx_msg void OnBrushDoubleHeight();
	afx_msg void OnBrushDoubleWidth();
	afx_msg void OnBrushHflip();
	afx_msg void OnBrushVflip();
	afx_msg void OnHelperCycleFreehandTool();
	afx_msg void OnReturnToBuiltInBrush();
	afx_msg void OnBrushCustomDeformation();
	afx_msg void OnHackLoadPicture();
	afx_msg void OnHackSavePicture();
	afx_msg void OnBrushOutline();
	afx_msg void OnUiLayout1();
	afx_msg void OnUiLayout2();
	afx_msg void OnUiLayout3();
	afx_msg void OnUiLayout4();
	afx_msg void OnUiLayout5();
	afx_msg void OnUiLayout6();
	afx_msg void OnUiLayout7();
	afx_msg void OnUiLayout8();
	afx_msg void OnUiLayout9();
	afx_msg void OnUiLayout0();
	afx_msg void OnAnimCreate();
	afx_msg void OnNewSimplePaletteView();
	afx_msg void OnUpdateNewSimplePaletteView(CCmdUI* pCmdUI);
	afx_msg void OnFrameNext();
	afx_msg void OnUpdateFrameNext(CCmdUI* pCmdUI);
	afx_msg void OnFramePrev();
	afx_msg void OnUpdateFramePrev(CCmdUI* pCmdUI);
	afx_msg void OnLayerNext();
	afx_msg void OnUpdateLayerNext(CCmdUI* pCmdUI);
	afx_msg void OnLayerPrev();
	afx_msg void OnUpdateLayerPrev(CCmdUI* pCmdUI);
	afx_msg void OnFrameGoto();
	afx_msg void OnUpdateFrameGoto(CCmdUI* pCmdUI);
	afx_msg void OnPendingMode();
	afx_msg void OnRenderForeground5050();
	afx_msg void OnUpdateRenderForeground5050(CCmdUI* pCmdUI);
	afx_msg void OnRenderCheckerBoardBackdrop();
	afx_msg void OnUpdateRenderCheckerBoardBackdrop(CCmdUI* pCmdUI);
	afx_msg void OnCompositeMode();
	afx_msg void OnUpdateCompositeMode(CCmdUI* pCmdUI);
	afx_msg void OnCompositeOpaque();
	afx_msg void OnUpdateCompositeOpaque(CCmdUI* pCmdUI);
	afx_msg void OnCompositeSetAnimMainLink();
	afx_msg void OnUpdateCompositeSetAnimMainLink(CCmdUI* pCmdUI);
	afx_msg void OnCompositeSetLink1();
	afx_msg void OnUpdateCompositeSetLink1(CCmdUI* pCmdUI);
	afx_msg void OnCompositeSetLink2();
	afx_msg void OnUpdateCompositeSetLink2(CCmdUI* pCmdUI);
	afx_msg void OnCompositeSetLink3();
	afx_msg void OnUpdateCompositeSetLink3(CCmdUI* pCmdUI);
	afx_msg void OnCompositeSetLink4();
	afx_msg void OnUpdateCompositeSetLink4(CCmdUI* pCmdUI);
	afx_msg void OnCompositeShowHandles();
	afx_msg void OnUpdateCompositeShowHandles(CCmdUI* pCmdUI);
	afx_msg void OnCompositeXLock();
	afx_msg void OnUpdateCompositeXLock(CCmdUI* pCmdUI);
	afx_msg void OnCompositeYLock();
	afx_msg void OnUpdateCompositeYLock(CCmdUI* pCmdUI);
	afx_msg void OnCompositeBoundingRects();
	afx_msg void OnUpdateCompositeBoundingRects(CCmdUI* pCmdUI);
	afx_msg void OnCompositeExternalRect();
	afx_msg void OnUpdateCompositeExternalRect(CCmdUI* pCmdUI);
	afx_msg void OnCompositeDisableImageRender();
	afx_msg void OnUpdateCompositeDisableImageRender(CCmdUI* pCmdUI);
	afx_msg void OnCompositeRenderSelected();
	afx_msg void OnUpdateCompositeRenderSelected(CCmdUI* pCmdUI);
	afx_msg void OnAnimFastforward();
	afx_msg void OnUpdateAnimFastforward(CCmdUI* pCmdUI);
	afx_msg void OnAnimRewind();
	afx_msg void OnUpdateAnimRewind(CCmdUI* pCmdUI);
	afx_msg void OnNewPopupGridView();
	afx_msg void OnUpdateNewPopupGridView(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpecialCaseOpaqueEdit(CCmdUI* pCmdUI);
	afx_msg void OnSpecialCaseOpaqueEdit();
	afx_msg void On2FrameNext();
	afx_msg void OnUpdate2FrameNext(CCmdUI* pCmdUI);
	afx_msg void On2FramePrev();
	afx_msg void OnUpdate2FramePrev(CCmdUI* pCmdUI);
	afx_msg void On2LayerNext();
	afx_msg void OnUpdate2LayerNext(CCmdUI* pCmdUI);
	afx_msg void On2LayerPrev();
	afx_msg void OnUpdate2LayerPrev(CCmdUI* pCmdUI);
	afx_msg void On2RenderForeground5050();
	afx_msg void OnUpdate2RenderForeground5050(CCmdUI* pCmdUI);
	afx_msg void OnAnimNext();
	afx_msg void OnUpdateAnimNext(CCmdUI* pCmdUI);
	afx_msg void OnAnimPrev();
	afx_msg void OnUpdateAnimPrev(CCmdUI* pCmdUI);
	afx_msg void OnNewMdiGrid();
	afx_msg void OnNewMultiSplitWindow();
	afx_msg void OnAnimDeleteSingleLayer();
	afx_msg void OnUpdateAnimDeleteSingleLayer(CCmdUI* pCmdUI);
	afx_msg void OnShiftDrawCanvasD();
	afx_msg void OnUpdateShiftDrawCanvasD(CCmdUI* pCmdUI);
	afx_msg void OnShiftDrawCanvasL();
	afx_msg void OnUpdateShiftDrawCanvasL(CCmdUI* pCmdUI);
	afx_msg void OnShiftDrawCanvasR();
	afx_msg void OnUpdateShiftDrawCanvasR(CCmdUI* pCmdUI);
	afx_msg void OnShiftDrawCanvasU();
	afx_msg void OnUpdateShiftDrawCanvasU(CCmdUI* pCmdUI);
	afx_msg void OnHflipDrawCanvas();
	afx_msg void OnUpdateHflipDrawCanvas(CCmdUI* pCmdUI);
	afx_msg void OnVflipDrawCanvas();
	afx_msg void OnUpdateVflipDrawCanvas(CCmdUI* pCmdUI);
	afx_msg void OnPlayAnimNextFrame();
	afx_msg void OnPlayAnimStart();
	afx_msg void OnPlayAnimStop();
	afx_msg void OnStencilOn();
	afx_msg void OnUpdateStencilOn(CCmdUI* pCmdUI);
	afx_msg void OnCompositeShowLinkpoints();
	afx_msg void OnUpdateCompositeShowLinkpoints(CCmdUI* pCmdUI);
	afx_msg void OnLayerProperties();
	afx_msg void OnUpdateLayerProperties(CCmdUI* pCmdUI);
	afx_msg void OnFrameProperties();
	afx_msg void OnUpdateFrameProperties(CCmdUI* pCmdUI);
	afx_msg void OnGridSettings();
	afx_msg void OnFrameCompositeOps();
	afx_msg void OnUpdateFrameCompositeOps(CCmdUI* pCmdUI);
	afx_msg void OnLayerExportFrames();
	afx_msg void OnUpdateLayerExportFrames(CCmdUI* pCmdUI);
	afx_msg void OnLayerImportFrames();
	afx_msg void OnUpdateLayerImportFrames(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTranslateFrameLayers(CCmdUI* pCmdUI);
	afx_msg void OnTranslateFrameLayers();
	afx_msg void OnUpdateAnimProperties(CCmdUI* pCmdUI);
	afx_msg void OnAnimProperties();
	afx_msg void OnAnimLoadBackdrop();
	afx_msg void OnUpdateAnimLoadBackdrop(CCmdUI* pCmdUI);
	afx_msg void OnAnimClearBackdrop();
	afx_msg void OnUpdateAnimClearBackdrop(CCmdUI* pCmdUI);
	afx_msg void OnAnimShowBackdrop();
	afx_msg void OnUpdateAnimShowBackdrop(CCmdUI* pCmdUI);
	afx_msg void OnHackLoadPalette();
	afx_msg void OnLayerCopyFrameToNext();
	afx_msg void OnUpdateLayerCopyFrameToNext(CCmdUI* pCmdUI);
	afx_msg void OnHackGridThumbnails();
	afx_msg void OnAllAnimationFrameNotes();
	afx_msg void OnUpdateAllAnimationFrameNotes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHackGridThumbnails(CCmdUI* pCmdUI);
	afx_msg void OnHackTransparentThumbnailScale();
	afx_msg void OnUpdateHackTransparentThumbnailScale(CCmdUI* pCmdUI);
	afx_msg void OnShowcaseChangeCanvas();
	afx_msg void OnUpdateShowcaseChangeCanvas(CCmdUI* pCmdUI);
	afx_msg void OnBrushTextToBrush();
	afx_msg void OnUpdateBrushTextToBrush(CCmdUI* pCmdUI);
	afx_msg void OnAnimSpecialCaptureFont();
	afx_msg void OnSpecialImediateSilentRender();
	afx_msg void OnUpdateSpecialImediateSilentRender(CCmdUI* pCmdUI);
	afx_msg void OnUiStoreLayout0();
	afx_msg void OnUiStoreLayout8();
	afx_msg void OnUiStoreLayout9();
	afx_msg void OnNewWindowClip();
	afx_msg void OnNewMdiAnimList();
	afx_msg void OnNewMdiAnimSpots();
	afx_msg void OnModifyNamedSpot();
	afx_msg void OnUpdateModifyNamedSpot(CCmdUI* pCmdUI);
	afx_msg void OnModifySpotAdjustment();
	afx_msg void OnUpdateModifySpotAdjustment(CCmdUI* pCmdUI);
	afx_msg void OnNewPopupAnimSpots();
	afx_msg void OnUpdateNewPopupAnimSpots(CCmdUI* pCmdUI);
	afx_msg void OnRopDifference();
	afx_msg void OnUpdateRopDifference(CCmdUI* pCmdUI);
	afx_msg void OnRopMatte();
	afx_msg void OnUpdateRopMatte(CCmdUI* pCmdUI);
	afx_msg void OnRopReplace();
	afx_msg void OnUpdateRopReplace(CCmdUI* pCmdUI);
	afx_msg void OnRopSingleColor();
	afx_msg void OnUpdateRopSingleColor(CCmdUI* pCmdUI);
	afx_msg void OnRopKeepDiff();
	afx_msg void OnUpdateRopKeepDiff(CCmdUI* pCmdUI);
	afx_msg void OnRopKeepSame();
	afx_msg void OnUpdateRopKeepSame(CCmdUI* pCmdUI);
	afx_msg void OnCopyFrames();
	afx_msg void OnUpdateCopyFrames(CCmdUI* pCmdUI);
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnBrushExportGbaData();
	afx_msg void OnUpdateBrushExportGbaData(CCmdUI* pCmdUI);
	afx_msg void OnNewMdiVeInfo();
	afx_msg void OnUpdateNewMdiVeInfo(CCmdUI* pCmdUI);
	afx_msg void OnShowcaseOptimize();
	afx_msg void OnUpdateShowcaseOptimize(CCmdUI* pCmdUI);
	afx_msg void OnNewPopupVeInfo();
	afx_msg void OnUpdateNewPopupVeInfo(CCmdUI* pCmdUI);
	afx_msg void OnClickSelectLayer();
	afx_msg void OnUpdateClickSelectLayer(CCmdUI* pCmdUI);
	afx_msg void OnFillAcrossLayers();
	afx_msg void OnUpdateFillAcrossLayers(CCmdUI* pCmdUI);
	afx_msg void OnTpbXmlExport();
	afx_msg void OnUpdateTpbXmlExport(CCmdUI* pCmdUI);
	afx_msg void OnFatbitsColorUseChromakey();
	afx_msg void OnUpdateFatbitsColorUseChromakey(CCmdUI* pCmdUI);
	afx_msg void OnManagePalettes();
	afx_msg void OnUpdateManagePalettes(CCmdUI *pCmdUI);
	//}}AFX_MSG
public:
	afx_msg void OnDrawingTool( UINT nID );
	afx_msg void OnUpdateDrawingTool( CCmdUI* pCmdUI );
	afx_msg void OnQuickBrush( UINT nID );
	afx_msg void OnUpdateQuickBrush( CCmdUI* pCmdUI );
	afx_msg void OnColorCmd( UINT nID );
	afx_msg void OnUpdateColorCmd( CCmdUI* pCmdUI );
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSpecializedwindowsLayersstripwindow();
	afx_msg void OnUpdateSpecializedwindowsLayersstripwindow(CCmdUI *pCmdUI);
	afx_msg void OnSpecializedwindowsBrushwindow();
	afx_msg void OnUpdateSpecializedwindowsBrushwindow(CCmdUI *pCmdUI);
	afx_msg void OnSpecializedwindowsFramesstripwindow();
	afx_msg void OnUpdateSpecializedwindowsFramesstripwindow(CCmdUI *pCmdUI);
	afx_msg void OnChangePixelAspectRatio();
	afx_msg void OnUpdateChangePixelAspectRatio(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLoadPictureFromClipboard(CCmdUI *pCmdUI);
	afx_msg void OnLoadPictureFromClipboard();
	afx_msg void OnAnimSpecialBrushToAnim();
	afx_msg void OnUpdateAnimSpecialBrushToAnim(CCmdUI *pCmdUI);
	afx_msg void OnAnimSpecialExportToCode();
	afx_msg void OnUpdateAnimSpecialExportToCode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSpecialSuperOptimize(CCmdUI *pCmdUI);
	afx_msg void OnSpecialSuperOptimize();
	afx_msg void OnBrushToLayeredAnim();
	afx_msg void OnUpdateBrushToLayeredAnim(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BPAINTDOC_H__61A97432_27DA_4ABC_891E_BB7C09803162__INCLUDED_)
