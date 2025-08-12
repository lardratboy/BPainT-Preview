// bpaintDoc.cpp : implementation of the CBpaintDoc class
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"

#include "MainFrm.h"
#include "bpaintDoc.h"
#include "bpaintview.h"
#include "SimplePaletteView.h"
#include "BPTAnimationFileIO.h"

#include "BPTPrimitives.h"
#include "BPTRasterOps.h"

// (frame window includes) ----------------------------------------------------

#include "ChildFrm.h"
#include "GenericMDIChildFrame.h"
#include "PopFrameWnd.h"
#include "SimpleSplitFrame.h"
#include "WindowClipFrmWnd.h"

// (dialog includes) ----------------------------------------------------------

#include "ChooseSizeDlg.h"
#include "NewAnimationShowcase.h"
#include "EditPaletteDlg.h"
#include "MakeStencilDlg.h"
#include "ClearFrameLayersDlg.h"
#include "DeleteFrameRangeDlg.h"
#include "InsertXDlg.h"
#include "SelectAnimationDlg.h"
#include "SimpleTransformationsDlg.h"
#include "GridSettingsDlg.h"
#include "BrushDeformDlg.h"
#include "QuickRGBDlg.h"
#include "CreateAnimDlg.h"
#include "SimpleSplitFrame.h"
#include "LayerPropertiesDlg.h"
#include "FramePropertiesDlg.h"
#include "GotoFrameDlg.h"
#include "CompositeFrameOpsDlg.h"
#include "AnimationPropertiesDlg.h"
#include "PaletteManager.h"

// ----------------------------------------------------------------------------

// THIS IS WAS A HACK TEST VAR

bool m_bUseOriginalOnBrushSizeChange = false;

// RONG:
BPT::CAnimationShowcase *CBpaintDoc::s_clipboardShowcase = NULL;

// ----------------------------------------------------------------------------

#if 1 // BPT 6/2/01

//
//	NOTE: This code was a quick hack and probably should be ripped out
//	and replaced with something that is easy to follow.
//

//
//	CBpaintDoc::HACK_PenOverridePixelValue()
//

CBpaintDoc::editor_pixel_type
CBpaintDoc::HACK_PenOverridePixelValue( 
	const CBpaintDoc::editor_pixel_type pixel, const int x, const int y 
)
{
	// Cheap hack for now it could be soo much faster it's now even funny!
	// --------------------------------------------------------------------

	if ( m_pTemporaryTrackedView && m_pAnimation && m_bFillAcrossLayerBoundaries ) {
		// Determine best we can if this is a composite view
		// --------------------------------------------------------------------

		if ( !m_pTemporaryTrackedView->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

			return pixel;

		}

		dib_type * pDib = ((CBpaintView*)m_pTemporaryTrackedView)->GetCanvasDib();

		if ( !pDib ) {

			return pixel;

		}

		if ( pDib != M_GetCompositeViewDib() ) {

			return pixel;

		}

		// --------------------------------------------------------------------

		int count = m_pAnimation->LayerCount();

		int currentLayer = m_pAnimation->CurrentLayer();

		int checkPixelSize = sizeof( editor_pixel_type );

		for ( int i = (count - 1); 0 <= i; i-- ) {

			if ( i == currentLayer ) {

				if ( pixel != m_ChromaKey ) {

					return pixel;

				}

				continue;

			}

			BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );
	
			if ( pLayer ) {

				// Need to check visibility then hit test

				if ( IsLayerVisible( i ) ) {

					// Hit test this layer to see if it has a pixel value

					BPT::CVisualElement::storage_type vValue;
	
					if ( pLayer->HitTest( x, y, checkPixelSize, &vValue ) ) {

						return static_cast<editor_pixel_type>( vValue );
	
					}

				}

			}

		}

	}

	return pixel;
}

// ----------

//
//	CBpaintDoc::HACK_pixel_overrider_type::Prepare()
//

void
CBpaintDoc::HACK_pixel_overrider_type::Prepare( render_target_type & renderTarget )
{
	if ( m_pDoc ) {

		// Should prepare for the operation

	}
}

//
//	CBpaintDoc::HACK_pixel_overrider_type::OverridePixelValue()
//

CBpaintDoc::HACK_pixel_overrider_type::pixel_type
CBpaintDoc::HACK_pixel_overrider_type::OverridePixelValue(
	const pixel_type pixel, const int x, const int y
) {

	if ( m_pDoc ) {

		return m_pDoc->HACK_PenOverridePixelValue( pixel, x, y );

	}

	return pixel;

}

#endif // BPT 6/2/01

// ----------------------------------------------------------------------------

#include "BPTTools.h"

#if 1 // HACK
#include "BPTpcxio.h"
#include ".\bpaintdoc.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CEditorRectChange, CObject )
IMPLEMENT_DYNAMIC( CColorChange, CObject )
IMPLEMENT_DYNAMIC( CActiveAnimationChange, CObject )

/////////////////////////////////////////////////////////////////////////////
// CBpaintDoc

IMPLEMENT_DYNCREATE(CBpaintDoc, CDocument)

BEGIN_MESSAGE_MAP(CBpaintDoc, CDocument)
	//{{AFX_MSG_MAP(CBpaintDoc)
	ON_COMMAND(ID_VIEW_AUTO_FOCUS, OnViewAutoFocus)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTO_FOCUS, OnUpdateViewAutoFocus)
	ON_COMMAND(ID_EDIT_PALETTE, OnEditPalette)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PALETTE, OnUpdateEditPalette)
	ON_COMMAND(ID_HACK_LOAD_BACKGROUND, OnHackLoadBackground)
	ON_COMMAND(ID_HACK_LOAD_FOREGROUND, OnHackLoadForeground)
	ON_COMMAND(ID_EDIT_STENCIL, OnEditStencil)
	ON_COMMAND(ID_ANIM_SELECT_DIALOG, OnAnimSelectDialog)
	ON_COMMAND(ID_ANIM_DELETE_FRAMES, OnAnimDeleteFrames)
	ON_COMMAND(ID_ANIM_INSERT_FRAMES, OnAnimInsertFrames)
	ON_COMMAND(ID_ANIM_INSERT_LAYERS, OnAnimInsertLayers)
	ON_COMMAND(ID_FRAME_TRANSFORMATIONS, OnFrameTransformations)
	ON_COMMAND(ID_FRAME_CLEAR, OnFrameClear)
	ON_COMMAND(ID_LAYER_CLEAR, OnLayerClear)
	ON_COMMAND(ID_PAINT_TRIM_EDGE, OnPaintTrimEdge)
	ON_UPDATE_COMMAND_UI(ID_PAINT_TRIM_EDGE, OnUpdatePaintTrimEdge)
	ON_COMMAND(ID_BRUSH_RESTORE, OnBrushRestore)
	ON_COMMAND(ID_BRUSH_ROTATE_90, OnBrushRotate90)
	ON_COMMAND(ID_BRUSH_HALVE_SIZE, OnBrushHalveSize)
	ON_COMMAND(ID_BRUSH_DOUBLE_SIZE, OnBrushDoubleSize)
	ON_COMMAND(ID_BRUSH_DOUBLE_HEIGHT, OnBrushDoubleHeight)
	ON_COMMAND(ID_BRUSH_DOUBLE_WIDTH, OnBrushDoubleWidth)
	ON_COMMAND(ID_BRUSH_HFLIP, OnBrushHflip)
	ON_COMMAND(ID_BRUSH_VFLIP, OnBrushVflip)
	ON_COMMAND(ID_HELPER_CYCLE_FREEHAND_TOOL, OnHelperCycleFreehandTool)
	ON_COMMAND(ID_RETURN_TO_BUILT_IN_BRUSH, OnReturnToBuiltInBrush)
	ON_COMMAND(ID_BRUSH_CUSTOM_DEFORMATION, OnBrushCustomDeformation)
	ON_COMMAND(ID_HACK_LOAD_PICTURE, OnHackLoadPicture)
	ON_COMMAND(ID_HACK_SAVE_PICTURE, OnHackSavePicture)
	ON_COMMAND(ID_BRUSH_OUTLINE, OnBrushOutline)
	ON_COMMAND(ID_UI_LAYOUT_1, OnUiLayout1)
	ON_COMMAND(ID_UI_LAYOUT_2, OnUiLayout2)
	ON_COMMAND(ID_UI_LAYOUT_3, OnUiLayout3)
	ON_COMMAND(ID_UI_LAYOUT_4, OnUiLayout4)
	ON_COMMAND(ID_UI_LAYOUT_5, OnUiLayout5)
	ON_COMMAND(ID_UI_LAYOUT_6, OnUiLayout6)
	ON_COMMAND(ID_UI_LAYOUT_7, OnUiLayout7)
	ON_COMMAND(ID_UI_LAYOUT_8, OnUiLayout8)
	ON_COMMAND(ID_UI_LAYOUT_9, OnUiLayout9)
	ON_COMMAND(ID_UI_LAYOUT_0, OnUiLayout0)
	ON_COMMAND(ID_ANIM_CREATE, OnAnimCreate)
	ON_COMMAND(ID_NEW_SIMPLE_PALETTE_VIEW, OnNewSimplePaletteView)
	ON_UPDATE_COMMAND_UI(ID_NEW_SIMPLE_PALETTE_VIEW, OnUpdateNewSimplePaletteView)
	ON_COMMAND(ID_FRAME_NEXT, OnFrameNext)
	ON_UPDATE_COMMAND_UI(ID_FRAME_NEXT, OnUpdateFrameNext)
	ON_COMMAND(ID_FRAME_PREV, OnFramePrev)
	ON_UPDATE_COMMAND_UI(ID_FRAME_PREV, OnUpdateFramePrev)
	ON_COMMAND(ID_LAYER_NEXT, OnLayerNext)
	ON_UPDATE_COMMAND_UI(ID_LAYER_NEXT, OnUpdateLayerNext)
	ON_COMMAND(ID_LAYER_PREV, OnLayerPrev)
	ON_UPDATE_COMMAND_UI(ID_LAYER_PREV, OnUpdateLayerPrev)
	ON_COMMAND(ID_FRAME_GOTO, OnFrameGoto)
	ON_UPDATE_COMMAND_UI(ID_FRAME_GOTO, OnUpdateFrameGoto)
	ON_COMMAND(ID_PENDING_MODE, OnPendingMode)
	ON_COMMAND(ID_RENDER_FOREGROUND_5050, OnRenderForeground5050)
	ON_UPDATE_COMMAND_UI(ID_RENDER_FOREGROUND_5050, OnUpdateRenderForeground5050)
	ON_COMMAND(ID_RENDER_CHECKER_BOARD_BACKDROP, OnRenderCheckerBoardBackdrop)
	ON_UPDATE_COMMAND_UI(ID_RENDER_CHECKER_BOARD_BACKDROP, OnUpdateRenderCheckerBoardBackdrop)
	ON_COMMAND(ID_COMPOSITE_MODE, OnCompositeMode)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_MODE, OnUpdateCompositeMode)
	ON_COMMAND(ID_COMPOSITE_OPAQUE, OnCompositeOpaque)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_OPAQUE, OnUpdateCompositeOpaque)
	ON_COMMAND(ID_COMPOSITE_SET_ANIM_MAIN_LINK, OnCompositeSetAnimMainLink)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_SET_ANIM_MAIN_LINK, OnUpdateCompositeSetAnimMainLink)
	ON_COMMAND(ID_COMPOSITE_SET_LINK_1, OnCompositeSetLink1)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_SET_LINK_1, OnUpdateCompositeSetLink1)
	ON_COMMAND(ID_COMPOSITE_SET_LINK_2, OnCompositeSetLink2)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_SET_LINK_2, OnUpdateCompositeSetLink2)
	ON_COMMAND(ID_COMPOSITE_SET_LINK_3, OnCompositeSetLink3)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_SET_LINK_3, OnUpdateCompositeSetLink3)
	ON_COMMAND(ID_COMPOSITE_SET_LINK_4, OnCompositeSetLink4)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_SET_LINK_4, OnUpdateCompositeSetLink4)
	ON_COMMAND(ID_COMPOSITE_SHOW_HANDLES, OnCompositeShowHandles)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_SHOW_HANDLES, OnUpdateCompositeShowHandles)
	ON_COMMAND(ID_COMPOSITE_X_LOCK, OnCompositeXLock)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_X_LOCK, OnUpdateCompositeXLock)
	ON_COMMAND(ID_COMPOSITE_Y_LOCK, OnCompositeYLock)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_Y_LOCK, OnUpdateCompositeYLock)
	ON_COMMAND(ID_COMPOSITE_BOUNDING_RECTS, OnCompositeBoundingRects)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_BOUNDING_RECTS, OnUpdateCompositeBoundingRects)
	ON_COMMAND(ID_COMPOSITE_EXTERNAL_RECT, OnCompositeExternalRect)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_EXTERNAL_RECT, OnUpdateCompositeExternalRect)
	ON_COMMAND(ID_COMPOSITE_DISABLE_IMAGE_RENDER, OnCompositeDisableImageRender)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_DISABLE_IMAGE_RENDER, OnUpdateCompositeDisableImageRender)
	ON_COMMAND(ID_COMPOSITE_RENDER_SELECTED, OnCompositeRenderSelected)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_RENDER_SELECTED, OnUpdateCompositeRenderSelected)
	ON_COMMAND(ID_ANIM_FASTFORWARD, OnAnimFastforward)
	ON_UPDATE_COMMAND_UI(ID_ANIM_FASTFORWARD, OnUpdateAnimFastforward)
	ON_COMMAND(ID_ANIM_REWIND, OnAnimRewind)
	ON_UPDATE_COMMAND_UI(ID_ANIM_REWIND, OnUpdateAnimRewind)
	ON_COMMAND(ID_NEW_POPUP_GRID_VIEW, OnNewPopupGridView)
	ON_UPDATE_COMMAND_UI(ID_NEW_POPUP_GRID_VIEW, OnUpdateNewPopupGridView)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_CASE_OPAQUE_EDIT, OnUpdateSpecialCaseOpaqueEdit)
	ON_COMMAND(ID_SPECIAL_CASE_OPAQUE_EDIT, OnSpecialCaseOpaqueEdit)
	ON_COMMAND(ID_2_FRAME_NEXT, On2FrameNext)
	ON_UPDATE_COMMAND_UI(ID_2_FRAME_NEXT, OnUpdate2FrameNext)
	ON_COMMAND(ID_2_FRAME_PREV, On2FramePrev)
	ON_UPDATE_COMMAND_UI(ID_2_FRAME_PREV, OnUpdate2FramePrev)
	ON_COMMAND(ID_2_LAYER_NEXT, On2LayerNext)
	ON_UPDATE_COMMAND_UI(ID_2_LAYER_NEXT, OnUpdate2LayerNext)
	ON_COMMAND(ID_2_LAYER_PREV, On2LayerPrev)
	ON_UPDATE_COMMAND_UI(ID_2_LAYER_PREV, OnUpdate2LayerPrev)
	ON_COMMAND(ID_2_RENDER_FOREGROUND_5050, On2RenderForeground5050)
	ON_UPDATE_COMMAND_UI(ID_2_RENDER_FOREGROUND_5050, OnUpdate2RenderForeground5050)
	ON_COMMAND(ID_ANIM_NEXT, OnAnimNext)
	ON_UPDATE_COMMAND_UI(ID_ANIM_NEXT, OnUpdateAnimNext)
	ON_COMMAND(ID_ANIM_PREV, OnAnimPrev)
	ON_UPDATE_COMMAND_UI(ID_ANIM_PREV, OnUpdateAnimPrev)
	ON_COMMAND(ID_NEW_MDI_GRID, OnNewMdiGrid)
	ON_COMMAND(ID_NEW_MULTI_SPLIT_WINDOW, OnNewMultiSplitWindow)
	ON_COMMAND(ID_ANIM_DELETE_SINGLE_LAYER, OnAnimDeleteSingleLayer)
	ON_UPDATE_COMMAND_UI(ID_ANIM_DELETE_SINGLE_LAYER, OnUpdateAnimDeleteSingleLayer)
	ON_COMMAND(ID_SHIFT_DRAW_CANVAS_D, OnShiftDrawCanvasD)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DRAW_CANVAS_D, OnUpdateShiftDrawCanvasD)
	ON_COMMAND(ID_SHIFT_DRAW_CANVAS_L, OnShiftDrawCanvasL)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DRAW_CANVAS_L, OnUpdateShiftDrawCanvasL)
	ON_COMMAND(ID_SHIFT_DRAW_CANVAS_R, OnShiftDrawCanvasR)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DRAW_CANVAS_R, OnUpdateShiftDrawCanvasR)
	ON_COMMAND(ID_SHIFT_DRAW_CANVAS_U, OnShiftDrawCanvasU)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DRAW_CANVAS_U, OnUpdateShiftDrawCanvasU)
	ON_COMMAND(ID_HFLIP_DRAW_CANVAS, OnHflipDrawCanvas)
	ON_UPDATE_COMMAND_UI(ID_HFLIP_DRAW_CANVAS, OnUpdateHflipDrawCanvas)
	ON_COMMAND(ID_VFLIP_DRAW_CANVAS, OnVflipDrawCanvas)
	ON_UPDATE_COMMAND_UI(ID_VFLIP_DRAW_CANVAS, OnUpdateVflipDrawCanvas)
	ON_COMMAND(ID_PLAY_ANIM_NEXT_FRAME, OnPlayAnimNextFrame)
	ON_COMMAND(ID_PLAY_ANIM_START, OnPlayAnimStart)
	ON_COMMAND(ID_PLAY_ANIM_STOP, OnPlayAnimStop)
	ON_COMMAND(ID_STENCIL_ON, OnStencilOn)
	ON_UPDATE_COMMAND_UI(ID_STENCIL_ON, OnUpdateStencilOn)
	ON_COMMAND(ID_COMPOSITE_SHOW_LINKPOINTS, OnCompositeShowLinkpoints)
	ON_UPDATE_COMMAND_UI(ID_COMPOSITE_SHOW_LINKPOINTS, OnUpdateCompositeShowLinkpoints)
	ON_COMMAND(ID_LAYER_PROPERTIES, OnLayerProperties)
	ON_UPDATE_COMMAND_UI(ID_LAYER_PROPERTIES, OnUpdateLayerProperties)
	ON_COMMAND(ID_FRAME_PROPERTIES, OnFrameProperties)
	ON_UPDATE_COMMAND_UI(ID_FRAME_PROPERTIES, OnUpdateFrameProperties)
	ON_COMMAND(ID_GRID_SETTINGS, OnGridSettings)
	ON_COMMAND(ID_FRAME_COMPOSITE_OPS, OnFrameCompositeOps)
	ON_UPDATE_COMMAND_UI(ID_FRAME_COMPOSITE_OPS, OnUpdateFrameCompositeOps)
	ON_COMMAND(ID_LAYER_EXPORT_FRAMES, OnLayerExportFrames)
	ON_UPDATE_COMMAND_UI(ID_LAYER_EXPORT_FRAMES, OnUpdateLayerExportFrames)
	ON_COMMAND(ID_LAYER_IMPORT_FRAMES, OnLayerImportFrames)
	ON_UPDATE_COMMAND_UI(ID_LAYER_IMPORT_FRAMES, OnUpdateLayerImportFrames)
	ON_UPDATE_COMMAND_UI(ID_TRANSLATE_FRAME_LAYERS, OnUpdateTranslateFrameLayers)
	ON_COMMAND(ID_TRANSLATE_FRAME_LAYERS, OnTranslateFrameLayers)
	ON_UPDATE_COMMAND_UI(ID_ANIM_PROPERTIES, OnUpdateAnimProperties)
	ON_COMMAND(ID_ANIM_PROPERTIES, OnAnimProperties)
	ON_COMMAND(ID_ANIM_LOAD_BACKDROP, OnAnimLoadBackdrop)
	ON_UPDATE_COMMAND_UI(ID_ANIM_LOAD_BACKDROP, OnUpdateAnimLoadBackdrop)
	ON_COMMAND(ID_ANIM_CLEAR_BACKDROP, OnAnimClearBackdrop)
	ON_UPDATE_COMMAND_UI(ID_ANIM_CLEAR_BACKDROP, OnUpdateAnimClearBackdrop)
	ON_COMMAND(ID_ANIM_SHOW_BACKDROP, OnAnimShowBackdrop)
	ON_UPDATE_COMMAND_UI(ID_ANIM_SHOW_BACKDROP, OnUpdateAnimShowBackdrop)
	ON_COMMAND(ID_HACK_LOAD_PALETTE, OnHackLoadPalette)
	ON_COMMAND(ID_LAYER_COPY_FRAME_TO_NEXT, OnLayerCopyFrameToNext)
	ON_UPDATE_COMMAND_UI(ID_LAYER_COPY_FRAME_TO_NEXT, OnUpdateLayerCopyFrameToNext)
	ON_COMMAND(ID_HACK_GRID_THUMBNAILS, OnHackGridThumbnails)
	ON_COMMAND(ID_ALL_ANIMATION_FRAME_NOTES, OnAllAnimationFrameNotes)
	ON_UPDATE_COMMAND_UI(ID_ALL_ANIMATION_FRAME_NOTES, OnUpdateAllAnimationFrameNotes)
	ON_UPDATE_COMMAND_UI(ID_HACK_GRID_THUMBNAILS, OnUpdateHackGridThumbnails)
	ON_COMMAND(ID_HACK_TRANSPARENT_THUMBNAIL_SCALE, OnHackTransparentThumbnailScale)
	ON_UPDATE_COMMAND_UI(ID_HACK_TRANSPARENT_THUMBNAIL_SCALE, OnUpdateHackTransparentThumbnailScale)
	ON_COMMAND(ID_SHOWCASE_CHANGE_CANVAS, OnShowcaseChangeCanvas)
	ON_UPDATE_COMMAND_UI(ID_SHOWCASE_CHANGE_CANVAS, OnUpdateShowcaseChangeCanvas)
	ON_COMMAND(ID_BRUSH_TEXT_TO_BRUSH, OnBrushTextToBrush)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_TEXT_TO_BRUSH, OnUpdateBrushTextToBrush)
	ON_COMMAND(ID_ANIM_SPECIAL_CAPTURE_FONT, OnAnimSpecialCaptureFont)
	ON_COMMAND(ID_SPECIAL_IMEDIATE_SILENT_RENDER, OnSpecialImediateSilentRender)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_IMEDIATE_SILENT_RENDER, OnUpdateSpecialImediateSilentRender)
	ON_COMMAND(ID_UI_STORE_LAYOUT_0, OnUiStoreLayout0)
	ON_COMMAND(ID_UI_STORE_LAYOUT_8, OnUiStoreLayout8)
	ON_COMMAND(ID_UI_STORE_LAYOUT_9, OnUiStoreLayout9)
	ON_COMMAND(ID_NEW_WINDOW_CLIP, OnNewWindowClip)
	ON_COMMAND(ID_NEW_MDI_ANIM_LIST, OnNewMdiAnimList)
	ON_COMMAND(ID_NEW_MDI_ANIM_SPOTS, OnNewMdiAnimSpots)
	ON_COMMAND(ID_MODIFY_NAMED_SPOT, OnModifyNamedSpot)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_NAMED_SPOT, OnUpdateModifyNamedSpot)
	ON_COMMAND(ID_MODIFY_SPOT_ADJUSTMENT, OnModifySpotAdjustment)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_SPOT_ADJUSTMENT, OnUpdateModifySpotAdjustment)
	ON_COMMAND(ID_NEW_POPUP_ANIM_SPOTS, OnNewPopupAnimSpots)
	ON_UPDATE_COMMAND_UI(ID_NEW_POPUP_ANIM_SPOTS, OnUpdateNewPopupAnimSpots)
	ON_COMMAND(ID_ROP_DIFFERENCE, OnRopDifference)
	ON_UPDATE_COMMAND_UI(ID_ROP_DIFFERENCE, OnUpdateRopDifference)
	ON_COMMAND(ID_ROP_MATTE, OnRopMatte)
	ON_UPDATE_COMMAND_UI(ID_ROP_MATTE, OnUpdateRopMatte)
	ON_COMMAND(ID_ROP_REPLACE, OnRopReplace)
	ON_UPDATE_COMMAND_UI(ID_ROP_REPLACE, OnUpdateRopReplace)
	ON_COMMAND(ID_ROP_SINGLE_COLOR, OnRopSingleColor)
	ON_UPDATE_COMMAND_UI(ID_ROP_SINGLE_COLOR, OnUpdateRopSingleColor)
	ON_COMMAND(ID_ROP_KEEP_DIFF, OnRopKeepDiff)
	ON_UPDATE_COMMAND_UI(ID_ROP_KEEP_DIFF, OnUpdateRopKeepDiff)
	ON_COMMAND(ID_ROP_KEEP_SAME, OnRopKeepSame)
	ON_UPDATE_COMMAND_UI(ID_ROP_KEEP_SAME, OnUpdateRopKeepSame)
	ON_COMMAND(ID_COPY_FRAMES, OnCopyFrames)
	ON_UPDATE_COMMAND_UI(ID_COPY_FRAMES, OnUpdateCopyFrames)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_BRUSH_EXPORT_GBA_DATA, OnBrushExportGbaData)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_EXPORT_GBA_DATA, OnUpdateBrushExportGbaData)
	ON_COMMAND(ID_NEW_MDI_VE_INFO, OnNewMdiVeInfo)
	ON_UPDATE_COMMAND_UI(ID_NEW_MDI_VE_INFO, OnUpdateNewMdiVeInfo)
	ON_COMMAND(ID_SHOWCASE_OPTIMIZE, OnShowcaseOptimize)
	ON_UPDATE_COMMAND_UI(ID_SHOWCASE_OPTIMIZE, OnUpdateShowcaseOptimize)
	ON_COMMAND(ID_NEW_POPUP_VE_INFO, OnNewPopupVeInfo)
	ON_UPDATE_COMMAND_UI(ID_NEW_POPUP_VE_INFO, OnUpdateNewPopupVeInfo)
	ON_COMMAND(ID_CLICK_SELECT_LAYER, OnClickSelectLayer)
	ON_UPDATE_COMMAND_UI(ID_CLICK_SELECT_LAYER, OnUpdateClickSelectLayer)
	ON_COMMAND(ID_FILL_ACROSS_LAYERS, OnFillAcrossLayers)
	ON_UPDATE_COMMAND_UI(ID_FILL_ACROSS_LAYERS, OnUpdateFillAcrossLayers)
	ON_COMMAND(ID_TPB_XML_EXPORT, OnTpbXmlExport)
	ON_UPDATE_COMMAND_UI(ID_TPB_XML_EXPORT, OnUpdateTpbXmlExport)
	ON_COMMAND(ID_FATBITS_COLOR_USE_CHROMAKEY, OnFatbitsColorUseChromakey)
	ON_UPDATE_COMMAND_UI(ID_FATBITS_COLOR_USE_CHROMAKEY, OnUpdateFatbitsColorUseChromakey)
	ON_COMMAND(ID_MANAGE_PALETTES, OnManagePalettes)
	ON_UPDATE_COMMAND_UI(ID_MANAGE_PALETTES, OnUpdateManagePalettes)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_DTOOL_FREEHAND_LINE,ID_DTOOL_SOLID_SQUARE,OnDrawingTool)
	ON_COMMAND_RANGE(ID_2_DTOOL_FREEHAND_LINE,ID_2_DTOOL_SOLID_SQUARE,OnDrawingTool)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DTOOL_FREEHAND_LINE,ID_DTOOL_SOLID_SQUARE,OnUpdateDrawingTool)
	ON_UPDATE_COMMAND_UI_RANGE(ID_2_DTOOL_FREEHAND_LINE,ID_2_DTOOL_SOLID_SQUARE,OnUpdateDrawingTool)
	ON_COMMAND_RANGE(ID_QBRUSH_ELLIPSE_0,ID_QBRUSH_ELLIPSE_N,OnQuickBrush)
	ON_COMMAND_RANGE(ID_2_QBRUSH_ELLIPSE_0,ID_2_QBRUSH_ELLIPSE_N,OnQuickBrush)
	ON_UPDATE_COMMAND_UI_RANGE(ID_QBRUSH_ELLIPSE_0,ID_QBRUSH_ELLIPSE_N,OnUpdateQuickBrush)
	ON_UPDATE_COMMAND_UI_RANGE(ID_2_QBRUSH_ELLIPSE_0,ID_2_QBRUSH_ELLIPSE_N,OnUpdateQuickBrush)
	ON_COMMAND_RANGE(ID_COLOR_INC,ID_COLOR_PREVIOUS,OnColorCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COLOR_INC,ID_COLOR_PREVIOUS,OnUpdateColorCmd)
	ON_COMMAND_RANGE(ID_2_COLOR_INC,ID_2_COLOR_PREVIOUS,OnColorCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_2_COLOR_INC,ID_2_COLOR_PREVIOUS,OnUpdateColorCmd)
	ON_COMMAND(ID_SPECIALIZEDWINDOWS_LAYERSSTRIPWINDOW, OnSpecializedwindowsLayersstripwindow)
	ON_UPDATE_COMMAND_UI(ID_SPECIALIZEDWINDOWS_LAYERSSTRIPWINDOW, OnUpdateSpecializedwindowsLayersstripwindow)
	ON_COMMAND(ID_SPECIALIZEDWINDOWS_BRUSHWINDOW, OnSpecializedwindowsBrushwindow)
	ON_UPDATE_COMMAND_UI(ID_SPECIALIZEDWINDOWS_BRUSHWINDOW, OnUpdateSpecializedwindowsBrushwindow)
	ON_COMMAND(ID_SPECIALIZEDWINDOWS_FRAMESSTRIPWINDOW, OnSpecializedwindowsFramesstripwindow)
	ON_UPDATE_COMMAND_UI(ID_SPECIALIZEDWINDOWS_FRAMESSTRIPWINDOW, OnUpdateSpecializedwindowsFramesstripwindow)
	ON_COMMAND(ID_CHANGE_PIXEL_ASPECT_RATIO, OnChangePixelAspectRatio)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_PIXEL_ASPECT_RATIO, OnUpdateChangePixelAspectRatio)
	ON_UPDATE_COMMAND_UI(ID_LOAD_PICTURE_FROM_CLIPBOARD, OnUpdateLoadPictureFromClipboard)
	ON_COMMAND(ID_LOAD_PICTURE_FROM_CLIPBOARD, OnLoadPictureFromClipboard)
	ON_COMMAND(ID_ANIM_SPECIAL_BRUSH_TO_ANIM, OnAnimSpecialBrushToAnim)
	ON_UPDATE_COMMAND_UI(ID_ANIM_SPECIAL_BRUSH_TO_ANIM, OnUpdateAnimSpecialBrushToAnim)
	ON_COMMAND(ID_ANIM_SPECIAL_EXPORT_TO_CODE, OnAnimSpecialExportToCode)
	ON_UPDATE_COMMAND_UI(ID_ANIM_SPECIAL_EXPORT_TO_CODE, OnUpdateAnimSpecialExportToCode)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_SUPER_OPTIMIZE, OnUpdateSpecialSuperOptimize)
	ON_COMMAND(ID_SPECIAL_SUPER_OPTIMIZE, OnSpecialSuperOptimize)
	ON_COMMAND(ID_BRUSH_TO_LAYERED_ANIM, OnBrushToLayeredAnim)
	ON_UPDATE_COMMAND_UI(ID_BRUSH_TO_LAYERED_ANIM, OnUpdateBrushToLayeredAnim)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBpaintDoc construction/destruction

CBpaintDoc::CBpaintDoc()
{
	// TODO: add one-time construction code here

	m_bAutoEraseEditMode = (1 == GLOBAL_GetSettingInt( "bAutoEraseEditMode", 0 )); // BPT 11/26/02

#if defined( DEMO_SAVE_DISABLED ) // BPT 7/1/01

	m_bUseEditorChromakeyAsFatbitsColor = // BPT 6/28/01
		(1 == GLOBAL_GetSettingInt( "bUseEditorChromakeyAsFatbitsColor", 1 ));

#else

	m_bUseEditorChromakeyAsFatbitsColor = // BPT 6/28/01
		(1 == GLOBAL_GetSettingInt( "bUseEditorChromakeyAsFatbitsColor", 0 ));

#endif

#if 1 // BPT 6/2/01 
	
	m_HackPixelOverrider.SetDocument( this );

	m_bFillAcrossLayerBoundaries = false; // (1 == GLOBAL_GetSettingInt( "bFillAcrossLayerBoundaries", 0 ));

#endif

#if defined(BPAINT_PERFORMANCE_STATS)

	LARGE_INTEGER freq;

	m_bHasPerformanceTimer = (TRUE == QueryPerformanceFrequency( &freq ));

	m_QueryPerformanceFrequency = ((__int64)freq.QuadPart) / (__int64)1000;

#endif

	m_bDrawingButtonIsDown = false;
	m_nBrushPaintROP = ID_ROP_MATTE;

	// ------------------------------------------------------------------------

#if defined( DEMO_SAVE_DISABLED ) // BPT 7/1/01

	m_bClickSelectCompositeMode = (1 == GLOBAL_GetSettingInt( "bClickSelectCompositeMode", 1 )); // BPT 6/2/01

#else

	m_bClickSelectCompositeMode = (1 == GLOBAL_GetSettingInt( "bClickSelectCompositeMode", 0 )); // BPT 6/2/01

#endif

	// Read in the on finish edit mode member vars
	// ------------------------------------------------------------------------

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

	m_bSimpleEditMode = (1 == GLOBAL_GetSettingInt( "bSimpleEditMode", 0 ));

	m_bReleaseUnusedAfterEachEdit = (1 == GLOBAL_GetSettingInt( "bReleaseUnusedAfterEachEdit", 1 ));

	m_nReplaceVisualElementMode = GLOBAL_GetSettingInt( "nReplaceVisualElementMode", ON_REPLACE_ASK );

#endif

	// ------------------------------------------------------------------------

	m_eSetNamedSpotMode = SPOT_EDIT_NONE;

	m_pCurrentNamedSpot = 0;

#ifdef HULABEE
	m_bShowSpotInfo = false;
#else
	m_bShowSpotInfo = true;
#endif

	// ------------------------------------------------------------------------

	m_bImediateUpdatesForSilentRender = (1 == GLOBAL_GetSettingInt( "SilentRenderImediateUpdates", 0 ));
	m_bEnableRenderThumbnailsHack = (1 == GLOBAL_GetSettingInt( "GridThumbnailsHack", 0 ));
	m_bUseMoreAccurateThumbnailRenderer = false;

	// ------------------------------------------------------------------------

	m_AnimationLinkPointColor = MakeDisplayPixelType( 128, 128, 128 );
	m_FrameLink1Color = MakeDisplayPixelType( 128, 0, 0 );
	m_FrameLink2Color = MakeDisplayPixelType( 0, 128, 0 );
	m_FrameLink3Color = MakeDisplayPixelType( 0, 0, 128 );
	m_FrameLink4Color = MakeDisplayPixelType( 128, 128, 0 );

	// ------------------------------------------------------------------------

	m_bShowAnimationBackdrop = true;

	// ------------------------------------------------------------------------

	m_bRenderLinkPointOverlays = false;
	m_bSetAnimLinkPoint = false;
	m_bSetFrameLinkPoint = false;
	m_nSetFrameLinkPoint = 0;

	// ------------------------------------------------------------------------

	m_bColorStencilDefined = false;

	m_bColorStencilEnabled = false;

	for ( int sCounter = 0; sCounter < 256; sCounter++ ) {
		
		m_bColorStencilTable[ sCounter ] = false;

	}

	// ------------------------------------------------------------------------

	m_nPickupColorHackMsg = 0;

	m_pPickupColorHackDlg = 0;

	m_PendingMultiViewData = 0;

	m_bUseBackgroundRendererInEditOnlyViews = true;

	m_pPickupBrushMediator = 0;

	m_PreviousDrawingTool = 0;

	m_pTemporaryTrackedView = 0;

	m_dwDoDadFlags = BOUNDING_RECTS | HANDLES | EXPAND_BOUNDING_RECT | RENDER_SELECTED;

	m_BoundingRectColor = MakeDisplayPixelType( 0, 16 * 8, 16 * 8 );
	m_SelectedBoundingRectColor = MakeDisplayPixelType( 0, 31 * 8, 31 * 8 );

	m_HandleColor = MakeDisplayPixelType( 31 * 8, 0, 31 * 8 );
	m_SelectedHandleColor = MakeDisplayPixelType( 0, 31 * 8, 0 );

	m_OpaqueColor = MakeDisplayPixelType( 0, 0, 0 );
	m_SelectedOpaqueColor = MakeDisplayPixelType( 31 * 8, 31 * 8, 31 * 8 );

	// ------------------------------------------------------------------------

	m_pPopupGridFrameWnd = 0;

	m_bDrawForeground5050 = false;
	m_bOnionSkinMode = false;

	m_CurrentMode = CBpaintDoc::MODE::COMPOSITE;

	m_pPalFrameWnd = 0;

	m_pShowcase = 0;
	m_pAnimation = 0;

	m_bUseBitmapBrushRenderer_HACK = false;

	m_bRenderOverlays = true;

	m_hDefaultCursor = 0;
	m_hFloodCursor = 0;
	m_hFreehandSingleCursor = 0;
	m_hEyeDropperCursor = 0;
	m_hMoveCelCursor = 0;
	m_hOffCanvasDrawingCursor = 0;

	m_bHasBackgroundLayers = false;
	m_bHasForegroundLayers = false;

	m_bGridOn = false;
	m_AutoFocus = (1 == GLOBAL_GetSettingInt( "bAutoFocus", 1 ));

	// ---------------

	CPoint gridOffset;
	gridOffset.x = GLOBAL_GetSettingInt( "grid.x", 0 );
	gridOffset.y = GLOBAL_GetSettingInt( "grid.y", 0 );

	CSize gridSize;
	gridSize.cx = GLOBAL_GetSettingInt( "grid.cx", 8 );
	gridSize.cy = GLOBAL_GetSettingInt( "grid.cy", 8 );

	m_InputGrid.SetInfo( &gridOffset, &gridSize );

	// ---------------

	m_CanvasSize.cx = GLOBAL_GetSettingInt( "canvas.cx", 800 );
	m_CanvasSize.cy = GLOBAL_GetSettingInt( "canvas.cy", 600 );

	m_pEditorRectChangeMediator = 0;
	m_pCurrentPen = 0;

	m_ChromaKey = GLOBAL_GetSettingInt( "DefaultChromaKey", 0 ); //

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

	m_CompositeChromaKey = 0x00000000;

	m_CheckerBoardColorA = MakeDisplayPixelType( 204, 204, 204 ); // 230, 230, 230 );
	m_CheckerBoardColorB = MakeDisplayPixelType( 255, 255, 255 );

#else

	m_CompositeChromaKey = 0xefff; // imposible value

#if 1
	m_CheckerBoardColorA = MakeDisplayPixelType( 230, 230, 230 );
	m_CheckerBoardColorB = MakeDisplayPixelType( 255, 255, 255 );
#else
	m_CheckerBoardColorA = 0x739c;
	m_CheckerBoardColorB = ~0;
#endif

#endif

	m_FirstTime = true;

	// ------------------------------------------------------------------------

#if defined( CLUT_USE_C_ARRAY )

	m_Edit2CompositeClut = new composite_clut_entry_type[ 256 ];

#endif

	for ( int i = 0; i < 256; i++ ) {

		m_Edit2CompositeClut.Set( i, rand() * rand() );

	}

	// ------------------------------------------------------------------------

#if 1 // HACK

	m_PenPos.x = 0;
	m_PenPos.y = 0;
	m_PenSize.cx = 0;
	m_PenSize.cy = 0;
	m_LastPenSize = m_PenSize;
	m_Color_L = GLOBAL_GetSettingInt( "LastSelectedLColor", 1 );
	m_Color_R = GLOBAL_GetSettingInt( "LastSelectedRColor", m_ChromaKey );
	m_BrushChromaKey = m_ChromaKey;

#endif

}

CBpaintDoc::~CBpaintDoc()
{
	// ------------------------------------------------------------------------
	
	GLOBAL_PutSettingInt( // BPT 6/28/01
		"bUseEditorChromakeyAsFatbitsColor", 
		((m_bUseEditorChromakeyAsFatbitsColor) ? 1 : 0)
	);

	GLOBAL_PutSettingInt( "bClickSelectCompositeMode", m_bClickSelectCompositeMode ? 1 : 0 ); // BPT 6/2/01

//	GLOBAL_PutSettingInt( "bFillAcrossLayerBoundaries", m_bFillAcrossLayerBoundaries ? 1 : 0 ); // BPT 6/2/01

	// Read in the on finish edit mode member vars
	// ------------------------------------------------------------------------

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

	GLOBAL_PutSettingInt( "bSimpleEditMode", m_bSimpleEditMode ? 1 : 0 );

	GLOBAL_PutSettingInt( "bReleaseUnusedAfterEachEdit", m_bReleaseUnusedAfterEachEdit ? 1 : 0 );

	GLOBAL_PutSettingInt( "nReplaceVisualElementMode", m_nReplaceVisualElementMode );

#endif

	// ------------------------------------------------------------------------

	GLOBAL_PutSettingInt( "SilentRenderImediateUpdates", m_bImediateUpdatesForSilentRender ? 1 : 0 );
	GLOBAL_PutSettingInt( "GridThumbnailsHack", m_bEnableRenderThumbnailsHack ? 1 : 0 );
	GLOBAL_PutSettingInt( "UseBackdropSurfaceMode", m_bUseBackdropSurface ? 1 : 0 );
	GLOBAL_PutSettingInt( "LastSelectedLColor", m_Color_L );
	GLOBAL_PutSettingInt( "LastSelectedRColor", m_Color_R );
	GLOBAL_PutSettingInt( "bAutoFocus", m_AutoFocus ? 1 : 0 );

	if ( m_pEditorRectChangeMediator ) {

		delete m_pEditorRectChangeMediator;

	}

	if ( m_pPickupBrushMediator ) {

		delete m_pPickupBrushMediator;

	}

	if ( m_pShowcase ) {

		delete m_pShowcase;

	}
}

// ============================================================================

//
//	CBpaintDoc::KickStart()
//

BOOL CBpaintDoc::KickStart()
{
	HWND hwndMain = AfxGetMainWnd()->GetSafeHwnd();

	// ------------------------------------------------------------------------

	if ( !m_pShowcase ) {

		MessageBox( hwndMain, "No active showcase!", "Fatal error!", MB_ICONERROR );

		return FALSE;

	}

	m_CanvasSize = m_pShowcase->CanvasSize();

	if ( m_CanvasSize == CSize( 0, 0 ) ) {

		if ( !GetSizeDialog(
			AfxGetMainWnd(), "Choose canvas size!", m_CanvasSize, &CSize( 32767, 32767 ), "SizeCanvasDlg", true ) ) {
	
			return FALSE;
	
		}

	}

	// Load the cursor resources (is this right to have it attched to the doc?)
	// ------------------------------------------------------------------------

	m_hDefaultCursor = AfxGetApp()->LoadCursor( IDC_EDIT_CURSOR );
	m_hFloodCursor = AfxGetApp()->LoadCursor( IDC_FILL_CURSOR );
	m_hFreehandSingleCursor = AfxGetApp()->LoadCursor( IDC_FREEHAND_SINGLE_CURSOR );
	m_hEyeDropperCursor = AfxGetApp()->LoadCursor( IDC_EYEDROP_CURSOR );
	m_hMoveCelCursor = AfxGetApp()->LoadCursor( IDC_MOVE_CURSOR );
	m_hOffCanvasDrawingCursor = AfxGetApp()->LoadCursor( IDC_OFF_CANVAS_CURSOR );

	// Now onto the editor initialization
	// ------------------------------------------------------------------------

	if ( !m_EditorLogic.Initialize() ) {

		MessageBox( hwndMain, "Unable to initialize editor", "Fatal error!", MB_ICONERROR );

		return FALSE;

	}

	if ( !m_pEditorRectChangeMediator ) {

		m_pEditorRectChangeMediator = new rect_change_mediator_type( this );

		if ( !m_pEditorRectChangeMediator ) {

			MessageBox( hwndMain, "Unable to create change mediator", "Fatal error!", MB_ICONERROR );

			return FALSE;

		}

	}

	// setup the canvas internals
	// ------------------------------------------------------------------------

	if ( !ChangeCanvasSizeInternals( m_CanvasSize ) ) {

		MessageBox( hwndMain, "Unable to setup canvas internals", "Fatal error!", MB_ICONERROR );

		return FALSE;

	}

	// ------------------------------------------------------------------------

	m_EditorLogic.SetFeedbackOwner( this );

	if ( m_FirstTime ) {

		m_FirstTime = false;

		// Register the simple tools.
		// --------------------------------------------------------------------
	
		m_EditorLogic.RegisterTool( ID_DTOOL_LINE, new CBrushLineTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_EMPTY_RECT, new CBrushRectTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_EMPTY_ELLIPSE, new CBrushEllipseTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_EMPTY_CIRCLE, new CBrushCircleTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_EMPTY_SQUARE, new CBrushSquareTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_3PT_CURVE, new CBrushCurveTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_FLOOD_FILL, new CFloodFillTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_SOLID_RECT, new CSolidRectTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_SOLID_ELLIPSE, new CSolidEllipseTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_SOLID_CIRCLE, new CSolidCircleTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_SOLID_SQUARE, new CSolidSquareTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_FREEHAND_LINE, new CFreehandBrushLineTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_FREEHAND_SPOTTY, new CFreehandBrushSpottyTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_FREEHAND_SINGLE, new CFreehandBrushSingleTool() );
		m_EditorLogic.RegisterTool( ID_DTOOL_CROP_RECT, new CCropRectTool() );
	
		// Create the more complex tools.
		// --------------------------------------------------------------------
	
		m_pPickupBrushMediator = new CPickupBrushMediator( this );
	
		if ( !m_pPickupBrushMediator ) {
	
			MessageBox( hwndMain, "Unable to initialize brush mediator", "Fatal error!", MB_ICONERROR );
	
			return FALSE;
	
		}
	
		m_EditorLogic.RegisterTool( ID_DTOOL_PICKUP_BRUSH, new CPickupBrushTool( m_pPickupBrushMediator ) );
	
		// Create the eye dropper tool
		// --------------------------------------------------------------------

		m_EditorLogic.RegisterTool( ID_DTOOL_EYE_DROPPER, new CEyedropperTool( this ) );

	}

	// Clear out the editor.
	// ------------------------------------------------------------------------

	m_EditorLogic.Clear( GetClearColor() );

	// Need to select an animation, frame & layer for to kick start things
	// if there isn't an existing animation create one!!!
	// ------------------------------------------------------------------------

	if ( 0 == m_pShowcase->AnimationCount() ) {

		OnAnimCreate();

	}

	// ------------------------------------------------------------------------

	// now select the animation ?

	Request_AnimationChange( m_pShowcase->FirstAnimation() );

	// Handle UI layout
	// ------------------------------------------------------------------------

	AfxGetMainWnd()->PostMessage( WM_COMMAND, GLOBAL_GetSettingInt( "LastUILayout", ID_UI_LAYOUT_6) );

	// Kick start the modes
	// ------------------------------------------------------------------------

	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

	return TRUE;

}

// ============================================================================

bool CBpaintDoc::ChangeCanvasSizeInternals( const CSize size )
{
	// ------------------------------------------------------------------------

	HWND hwndMain = NULL;

	if ( AfxGetMainWnd() ) {

		hwndMain = AfxGetMainWnd()->GetSafeHwnd();

	}

	// Change the document's concept
	// ------------------------------------------------------------------------

	m_CanvasSize = size;

	// Calculate the thumbnail size
	// ------------------------------------------------------------------------

	int ratio = GLOBAL_GetSettingInt( "ThumbnailRatio", 16 );

	CSize smallestThumbnail;

	smallestThumbnail.cx = GLOBAL_GetSettingInt( "SmallestThumbnail.cx", 24 );
	smallestThumbnail.cy = GLOBAL_GetSettingInt( "SmallestThumbnail.cy", 16 );

	m_ThumbnailSize.cx = (((m_CanvasSize.cx / ratio) / 4) * 4);
	m_ThumbnailSize.cy = m_CanvasSize.cy / ratio;

	// Check the w
	// ------------------------------------------------------------------------

	if ( smallestThumbnail.cx > m_ThumbnailSize.cx ) {

		m_ThumbnailSize.cx = smallestThumbnail.cx;

		m_ThumbnailSize.cy = (m_ThumbnailSize.cx * m_CanvasSize.cy) / m_CanvasSize.cx;

	}

	// Check the h
	// ------------------------------------------------------------------------

	if ( smallestThumbnail.cy > m_ThumbnailSize.cy ) {

		m_ThumbnailSize.cy = smallestThumbnail.cx;

		m_ThumbnailSize.cx = (m_ThumbnailSize.cy * m_CanvasSize.cx) / m_CanvasSize.cy;

	}

	// change the showcase's concept of the canvas
	// ------------------------------------------------------------------------

	if ( m_pShowcase ) {

		m_pShowcase->SetCanvasSize( size );

	}

	// Create the main composite view dib
	// ------------------------------------------------------------------------

	if ( !m_CompositeViewDib.BPTDIB_DEBUG_CREATE( m_CanvasSize.cx, m_CanvasSize.cy, 0 ) ) {

		MessageBox( hwndMain, "Unable to create canvas dib", "Fatal error!", MB_ICONERROR );

		return false;

	}

	m_CompositeViewDib.GetBitmapPtr()->ClearBuffer( 0 );

	// Create the main edit view dib
	// ------------------------------------------------------------------------

	if ( !m_EditViewDib.BPTDIB_DEBUG_CREATE( m_CanvasSize.cx, m_CanvasSize.cy, 0 ) ) {

		MessageBox( hwndMain, "Unable to create canvas dib", "Fatal error!", MB_ICONERROR );

		return false;

	}

	m_EditViewDib.GetBitmapPtr()->ClearBuffer( 0 );

	// Setup the backdrop surface
	// ------------------------------------------------------------------------

	m_bUseBackdropSurface = (1 == GLOBAL_GetSettingInt( "UseBackdropSurfaceMode", 1 ));

	if ( !m_BackdropSurface.Create( m_CanvasSize ) ) {

		MessageBox( hwndMain, "Unable to create BACKDROP surface", "Fatal error!", MB_ICONERROR );

		return false;

	}

	// Fill in the checker pattern.
	// ------------------------------------------------------------------------

#if 0

	m_BackdropSurface.ClearBuffer( 0 );

#else

	// this needs to be a function!!!

	SIZE bk = { 8, 8 };

	for ( int bky = 0; bky < m_CanvasSize.cy; bky += bk.cy ) {

		int oddCounter = bky / bk.cy;

		for ( int bkx = 0; bkx < m_CanvasSize.cx; bkx += bk.cx ) {

			RECT rect = { bkx, bky, bkx + bk.cx, bky + bk.cy };

			if ( ++oddCounter & 1 ) {
		
				BPT::T_SolidRectPrim(
					m_BackdropSurface, rect, m_CheckerBoardColorA, composite_copy_op_type()
				);

			} else {

				BPT::T_SolidRectPrim(
					m_BackdropSurface, rect, m_CheckerBoardColorB, composite_copy_op_type()
				);

			}

		}

  	}

#endif


	// Setup the foreground & background "native" format bitmaps
	// ------------------------------------------------------------------------

	if ( !m_BackgroundSurface.Create( m_CanvasSize ) ) {

		MessageBox( hwndMain, "Unable to create background surface", "Fatal error!", MB_ICONERROR );

		return false;

	}

	m_BackgroundSurface.ClearBuffer( m_CompositeChromaKey );

	if ( !m_ForegroundSurface.Create( m_CanvasSize ) ) {

		MessageBox( hwndMain, "Unable to create foreground surface", "Fatal error!", MB_ICONERROR );

		return false;

	}

	m_ForegroundSurface.ClearBuffer( m_CompositeChromaKey );

	// Setup the editor logic
	// ------------------------------------------------------------------------

	if ( !m_EditorLogic.Create( m_CanvasSize, m_pEditorRectChangeMediator ) ) {

		MessageBox( hwndMain, "Unable to create create editor internals", "Fatal error!", MB_ICONERROR );

		return FALSE;

	}

	return true;

}

// ============================================================================

//
//	CBpaintDoc::OnNewDocument()
//

BOOL CBpaintDoc::OnNewDocument()
{

#if 1

	// Ask the app object to prepare for a new document
	// ------------------------------------------------------------------------

	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		if ( !pApp->PrepareForNewDocument( this ) ) {

			return FALSE;

		}

	}

#endif

	if (!CDocument::OnNewDocument())
		return FALSE;

	// Choose the canvas size :)
	// ------------------------------------------------------------------------

	CNewAnimationShowcaseDlg showcaseDlg( AfxGetMainWnd() );

	// These settings should be persistant.

	showcaseDlg.m_pRecentFileList = pApp->GetRecentFileList();

	showcaseDlg.m_CanvasSize = m_CanvasSize;

	showcaseDlg.LoadSettings( CString( "NewShowcaseDlg" ) );

	if ( IDOK != showcaseDlg.DoModal() ) {

		return FALSE;

	}

	showcaseDlg.SaveSettings();

	m_CanvasSize = showcaseDlg.m_CanvasSize;

	// Handle the MRU request
	// ------------------------------------------------------------------------

	if ( 0 != showcaseDlg.m_nMRUElement ) {

		CWnd * pWnd = AfxGetMainWnd();

		if ( pWnd ) {

			pWnd->PostMessage( WM_COMMAND, showcaseDlg.m_nMRUElement, 0 );

			SetModifiedFlag( FALSE );

			return TRUE;

		}

	}

	// ------------------------------------------------------------------------

	m_pShowcase = new BPT::CAnimationShowcase();

	if ( !m_pShowcase ) {

		TRACE( "Unable to create animation showcase.\n" );

		return FALSE;

	}

	m_pShowcase->SetCanvasSize( m_CanvasSize );

	if ( showcaseDlg.m_bAnimCheck ) {

		BPT::CAnimation * pAnim = m_pShowcase->NewAnimation(
			showcaseDlg.m_nFrameCount,
			showcaseDlg.m_nLayerCount
		);

		if ( pAnim ) {

			pAnim->SetName( showcaseDlg.m_AnimationName );

		}

	}

	m_pShowcase->SetPixelAspectRatio( showcaseDlg.m_PixelAspect ); // BPT 9/2/02

	// what about the name of the showcase???
	// what about the name of the showcase???
	// what about the name of the showcase???

	// ------------------------------------------------------------------------

	SetModifiedFlag();

	// ------------------------------------------------------------------------

	return KickStart();
}

// ============================================================================

BOOL CBpaintDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{

#if 1

	// Ask the app object to prepare for a new document
	// ------------------------------------------------------------------------

	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		if ( !pApp->PrepareForNewDocument( this ) ) {

			return FALSE;

		}

	}

#endif

	// ------------------------------------------------------------------------

	if ( m_pShowcase ) {

		delete m_pShowcase;

		m_pShowcase = 0;

	}

	// ------------------------------------------------------------------------

	if ( !BPT::LoadBPTFile( lpszPathName, &m_pShowcase ) ) {

		CString errorMsg;

		errorMsg.Format( "Unable to open \"%s\"", lpszPathName );

		MessageBox( AfxGetMainWnd()->GetSafeHwnd(), errorMsg, "Error!", MB_ICONERROR );

		return FALSE;

	}

	// ------------------------------------------------------------------------

	return KickStart();
}

// ============================================================================

BOOL CBpaintDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	if ( !m_pShowcase ) {

		return FALSE;

	}

	// Need to make sure that edit mode is finished

	FinishEdit();

	// fun!

	if ( BPT::SaveBPTFile( lpszPathName, m_pShowcase, false, false, true ) ) {

		SetModifiedFlag( FALSE );

		return TRUE;

	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// CBpaintDoc serialization

void CBpaintDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBpaintDoc diagnostics

#ifdef _DEBUG
void CBpaintDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBpaintDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBpaintDoc commands

void CBpaintDoc::OnViewAutoFocus() 
{
	m_AutoFocus = !m_AutoFocus;
}

void CBpaintDoc::OnUpdateViewAutoFocus(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_AutoFocus );
}
// ============================================================================

//
//	CBpaintDoc::SetStatusBarText()
//

void
CBpaintDoc::SetStatusBarText(
	const CString & str, const int nPane, const bool bUpdate
)
{
	CMainFrame * pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if ( !pFrame ) return;

	CStatusBar * pStatus = &pFrame->m_wndStatusBar;
	if ( !pStatus ) return;

	pStatus->SetPaneText( nPane, str, bUpdate );
}

//
//	CBpaintDoc::UpdateStatusBarWithEditMsgInfo()
//

void 
CBpaintDoc::UpdateStatusBarWithEditMsgInfo( const UINT nFlags, const CPoint point )
{
	CString str;

	CString colorName;

	colorName.Format( "(L:%d, R:%d)", m_Color_L, m_Color_R );

#if 1

	paint_logic_type::tool_base_type::pointer pTool = m_EditorLogic.GetCurrentToolPtr();
	
	bool bDefault = true;

	SIZE size = { 0, 0 };

	if ( pTool ) {

		if ( int count = pTool->GetPropertyCount() ) {

			for ( int i = 0; i < count; i++ ) {

				if ( BPT::ISupportPropertyTypes::WIDTH == pTool->GetPropertyType( i ) ) {

					int v;

					if ( pTool->GetPropertyValue( i, v ) ) {

						size.cx = v;

						bDefault = false;

					}

				} else if ( BPT::ISupportPropertyTypes::HEIGHT == pTool->GetPropertyType( i ) ) {

					int v;

					if ( pTool->GetPropertyValue( i, v ) ) {

						size.cy = v;

						bDefault = false;

					}

				}

			}

		}

	}

	int frame, layer;

	if ( m_pAnimation ) {

		frame = m_pAnimation->CurrentFrame() + 1;
		layer = m_pAnimation->CurrentLayer() + 1;

	} else {

		frame = 0;
		layer = 0;

	}

	CString prefixStr, layerName;

	layerName.Format( "%d", layer );

#if defined(BPAINT_PERFORMANCE_STATS)

	if ( m_pAnimation ) {

		char * pName = m_pAnimation->GetName();

		if ( pName ) {

			prefixStr.Format( "%d %s", m_CheeseRenderTookValue, pName );

		} else {

			prefixStr.Format( "%d ?", m_CheeseRenderTookValue );

		}

	} else {

		prefixStr.Format( "%d ?", m_CheeseRenderTookValue );

	}

#else

	if ( m_pAnimation ) {

		char * pName = m_pAnimation->GetName();
	
		if ( pName ) {
	
			prefixStr.Format( "%s", pName );
	
		} else {

			prefixStr.Format( "?" );

		}

	} else {

		prefixStr.Format( "?" );

	}

#endif

	if ( bDefault ) {
	
		str.Format(
			"{%s, Frame %d, layer %s } x:%4d, y:%4d color %s", 
			prefixStr, frame, layerName, point.x, point.y, colorName
		);

	} else {

		str.Format(
			"{%s, Frame %d, layer %s } x:%4d, y:%4d [ %dx%d ] color %s", 
			prefixStr, frame, layerName, point.x, point.y, size.cx, size.cy, colorName
		);

	}

#if 1

	if ( m_bUseBitmapBrushRenderer_HACK ) {

		CString brushStr;

		CSize size = GetDrawingBrushSize();

		brushStr.Format( " Brush %dx%d", size.cx, size.cy );

		str += brushStr;

	} else {

		CString penStr;

		penStr.Format( " Pen %dx%d", m_PenSize.cx, m_PenSize.cy );

		str += penStr;

	}

#endif

#else

	switch ( m_CurrentOverlay.nType ) {

	default:
		str.Format( "x:%4d, y:%4d color %s", point.x, point.y, colorName );
		break;

	case paint_logic_type::OVERLAY_RECT:
		str.Format(
			"x:%4d, y:%4d [ %dx%d ] color %s", point.x, point.y, 
			m_CurrentOverlay.rcRect.right - m_CurrentOverlay.rcRect.left,
			m_CurrentOverlay.rcRect.bottom - m_CurrentOverlay.rcRect.top,
			colorName
		);
		break;

	}

#endif

	SetStatusBarText( str );
}

// ============================================================================
// Overlay hack
// ============================================================================

//
//	CBpaintDoc::SilentRender()
//

void
CBpaintDoc::SilentRender( const RECT & rect )
{
	RenderRect( rect );
	CEditorRectChange changedRect( rect );

	if ( m_bImediateUpdatesForSilentRender ) {

		// This causes the display to flicker but it's slightly faster
		// It's just not worth it right now. 

		UpdateAllViews( NULL, CEditorRectChange::INVALIDATE_RECT, (CObject *)&changedRect );

	} else {

		UpdateAllViews( NULL, CEditorRectChange::SILENT_INVALIDATE_RECT, (CObject *)&changedRect );

	}
}

//
//	CBpaintDoc::ClearOverlayType()
//

void CBpaintDoc::ClearOverlayType()
{
	if ( paint_logic_type::OVERLAY_NONE != m_CurrentOverlay.nType ) {

		RepaintOverlayArea( false );

		m_CurrentOverlay.nType = paint_logic_type::OVERLAY_NONE;

	}
}

//
//	CBpaintDoc::SetOverlayInfo()
//

void CBpaintDoc::SetOverlayInfo( const paint_logic_type::overlay_info * pOverlayInfo )
{
	RepaintOverlayArea( false );

	m_CurrentOverlay = *pOverlayInfo;

	RepaintOverlayArea( true );

}

//
//	CBpaintDoc::RepaintOverlayArea()
//

void CBpaintDoc::RepaintOverlayArea( const bool bRender )
{
	bool bOld = m_bRenderOverlays;

	m_bRenderOverlays = bRender;

	RECT rect = m_CurrentOverlay.rcRect;

	switch ( m_CurrentOverlay.nType ) {

	default:
	case paint_logic_type::OVERLAY_NONE:
		break;

	case paint_logic_type::OVERLAY_RECT:
		{
			RECT a = { rect.left, rect.top, rect.right, rect.top + 1 };
			RECT b = { rect.left, rect.bottom - 1, rect.right, rect.bottom };
			RECT c = { rect.left, rect.top, rect.left + 1, rect.bottom };
			RECT d = { rect.right - 1, rect.top, rect.right, rect.bottom };

			SilentRender( a );
			SilentRender( b );
			SilentRender( c );
			SilentRender( d );
		}
		break;

	case paint_logic_type::OVERLAY_CROSS_HAIRS:
		{
			RECT c = { 0, 0, m_CanvasSize.cx, m_CanvasSize.cy };
			RECT a = { rect.left, c.top, rect.left + 1, c.bottom };
			RECT b = { c.left, rect.top, c.right, rect.top + 1 };

			SilentRender( a );
			SilentRender( b );
		}
		break;

	}

	m_bRenderOverlays = bOld;

}

//
//	CBpaintDoc::RenderOverlay()
//

void CBpaintDoc::RenderOverlay(
	dib_type::bitmap_type * pCanvasBitmap, const RECT * pClipRect
)
{
	RECT rect = m_CurrentOverlay.rcRect;

	if ( m_bRenderOverlays ) {

		switch ( m_CurrentOverlay.nType ) {

		default:
		case paint_logic_type::OVERLAY_NONE:
			break;

		case paint_logic_type::OVERLAY_RECT:
			BPT::T_RectPrim(
				*pCanvasBitmap, rect, 0,
				BPT::TInvertDstTransferROP<dib_type::bitmap_type::pixel_type>(),
				pClipRect
			);
			break;

		case paint_logic_type::OVERLAY_CROSS_HAIRS:
			{
				RECT c = { 0, 0, m_CanvasSize.cx, m_CanvasSize.cy };
				RECT a = { rect.left, c.top, rect.left + 1, c.bottom };
				RECT b = { c.left, rect.top, c.right, rect.top + 1 };

				BPT::T_RectPrim(
					*pCanvasBitmap, a, 0,
					BPT::TInvertDstTransferROP<dib_type::bitmap_type::pixel_type>(),
					pClipRect
				);

				BPT::T_RectPrim(
					*pCanvasBitmap, b, 0,
					BPT::TInvertDstTransferROP<dib_type::bitmap_type::pixel_type>(),
					pClipRect
				);

			}
			break;

		}

	}

//	CEditorRectChange changedRect( rect );
//	UpdateAllViews( NULL, CEditorRectChange::SILENT_INVALIDATE_RECT, (CObject *)&changedRect );


}

// ============================================================================

//
//	CBpaintDoc::Feedback()
//

void 
CBpaintDoc::Feedback(
	paint_logic_type * pLogic 
	,const paint_logic_type::FEEDBACK id
	,void * pData
)
{
	if ( (paint_logic_type::OVERLAY_CHANGE == id) && pData ) {

		// get the overlay change info

		const paint_logic_type::overlay_info * pOverlayInfo = 
			static_cast< paint_logic_type::overlay_info * >( pData );

		if ( !pOverlayInfo ) {

			return;

		}

		SetOverlayInfo( pOverlayInfo );

	}
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderBackgroundRect()
//

void 
CBpaintDoc::RenderBackgroundRect( dib_type::bitmap_type & surface, const RECT & rect )
{
	// Try to show the animation backdrop if one exists.
	// --------------------------------------------------------------------

	if ( m_bShowAnimationBackdrop ) {

		if ( m_pAnimation && m_pAnimation->HasBackdrop() ) {

			if ( m_pAnimation->RenderBackdrop(
				surface, visual_element_to_composite_type(), &rect ) ) {

				return;

			}

		}

	}

	// ------------------------------------------------------------------------

#if 1 // BPT 6/18/01

	// always use the showcase chromakey palette slot for the background rect
	// is this a bad idea?
	// ------------------------------------------------------------------------

	editor_to_composite_clut_type * pClut = GetShowcaseCLUT();

	composite_pixel_type bgColor;

	if ( pClut ) {

		bgColor = (*pClut)[ m_ChromaKey ];

	} else {

		bgColor = m_Edit2CompositeClut[ m_ChromaKey ];

	}

#else

	composite_pixel_type bgColor = m_Edit2CompositeClut[ m_ChromaKey ];

#endif

	// ------------------------------------------------------------------------

	if ( m_bUseBackdropSurface ) {

		BPT::T_Blit(
			surface, 0, 0, m_BackdropSurface, 
			composite_copy_op_type(), 0, &rect, 0
		);

	} else {

		BPT::T_SolidRectPrim(
			surface, rect, bgColor, composite_copy_op_type()
		);

	}

	// ------------------------------------------------------------------------

	if ( m_bOnionSkinMode && m_pAnimation ) {

#if 0

		// FUTURE!!!!
		// FUTURE!!!!

		int prevFrame = m_pAnimation->CurrentFrame() - 1;

		if ( -1 != prevFrame ) {

			visual_to_composite_copy_type onionSkinOp(
				&m_EditOnionSkin2CompositeClut
			);
			
			pPrevFrameLayer->Render( surface, 0, 0, onionSkinOp, pOptionalClipRect );

			// This could be by making the RenderCompositeModePrim() use a
			// argument to determine the render frame and an override for the
			// clut used to render.  This would be pretty simple.

		}

		// FUTURE!!!!
		// FUTURE!!!!

#endif

	}

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::IsLayerVisible()
//

bool 
CBpaintDoc::IsLayerVisible( const int nLayer )
{
	if ( m_pAnimation ) {

		BPT::CLayerInfo * pInfo = m_pAnimation->GetLayerInfo( nLayer );

		if ( pInfo ) {

			return (0 == (BPT::CLayerInfo::INVISIBLE & pInfo->GetFlags()) );

		}

	}

	return true;
}

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderCompositeModePrimEx()
//

void 
CBpaintDoc::RenderCompositeModePrimEx(
	dib_type::bitmap_type & surface, const RECT & rect
	,bool bOnlyActiveLayer
	,bool bUseVisibilityInfo
	,DWORD dwDodadFlags
	,int nFirstLayer
	,int nLastLayer
	,bool bRenderBackground
	,bool bForegroundMode
)
{

	if ( bRenderBackground ) {

		RenderBackgroundRect( surface, rect );

	}

	if ( m_pAnimation ) {

		// Determine the editor to composite clut to fallback to if no per layer
		// --------------------------------------------------------------------

		editor_to_composite_clut_type * pFallbackClut = GetShowcaseCLUT(); // BPT 6/15/01
	
#if 1 // BPT 6/15/01

		// check to see if the animation has a clut if so override
		// the showcase clut with it
		// --------------------------------------------------------------------
	
		BPT::CAnnotatedPalette * pPal = m_pAnimation->GetPalette();
	
		if ( pPal ) {
	
			pFallbackClut = pPal->GetDisplayCLUT();
	
		}

#endif // BPT 6/15/01

		// Get the outline colors from the animation
		// ----------------------------------------------------

		int outlineColorA = m_pAnimation->GetOutlineColorA();
		int outlineColorB = m_pAnimation->GetOutlineColorB();

		// --------------------------------------------------------------------

		int currentLayer = m_pAnimation->CurrentLayer();
		int currentFrame = m_pAnimation->CurrentFrame();

		// --------------------------------------------------------------------

		bool bDrawForeground5050 = m_bDrawForeground5050;

		// --------------------------------------------------------------------

		int layerCount = m_pAnimation->LayerCount();

		for ( int i = nFirstLayer; i <= nLastLayer; i++ ) {
	
			BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );
	
			if ( pLayer ) {
	
				BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( i );

				// ------------------------------------------------------------

				bool bVisible = true;

				if ( bUseVisibilityInfo ) {

					bVisible = IsLayerVisible( i );

				}

				// ------------------------------------------------------------

				bool bSelected = ( i == currentLayer );

				if ( bOnlyActiveLayer ) {

					if ( !bSelected ) {

						bVisible = false;

					}

				}

				// ------------------------------------------------------------

				if ( bVisible ) {
	
					// --------------------------------------------------------

					RECT renderedRect;

					if ( pLayer->BoundingRect( renderedRect ) ) {

						editor_to_composite_clut_type * pClut = pFallbackClut; // BPT 6/15/01

#if 1 // BPT 6/15/01

						// Determine which clut to use
						// ------------------------------------------------------------
		
						if ( pLayerInfo ) {
		
							BPT::CAnnotatedPalette * pPal = pLayerInfo->GetPalette();
		
							if ( pPal ) {
		
								pClut = pPal->GetDisplayCLUT();
		
							}
		
						}

#endif

#if 1 // BPT 6/26/01

						// Setup the layer outline color
						// ----------------------------------------------------

						bool bOutlineLayer = false;

						composite_pixel_type layerOutlineColor( 0 );

						if ( pLayerInfo ) {

							if ( pLayerInfo->CheckFlags( BPT::CLayerInfo::OUTLINE_COLOR_A ) ) {

								bOutlineLayer = true;

								if ( pClut ) {

									layerOutlineColor = (*pClut)[ outlineColorA ];

								} else {

									layerOutlineColor = 0;

								}

							} else if ( pLayerInfo->CheckFlags( BPT::CLayerInfo::OUTLINE_COLOR_B ) ) {

								bOutlineLayer = true;

								if ( pClut ) {

									layerOutlineColor = (*pClut)[ outlineColorB ];

								} else {

									layerOutlineColor = ~0;

								}

							}

						}

#endif

						// opaque mode?
						// ----------------------------------------------------

						if ( CBpaintDoc::DODADS::OPAQUE_MODE & dwDodadFlags ) {

							 BPT::T_SolidRectPrim(
							 	surface, renderedRect, 
								bSelected ? m_SelectedOpaqueColor : m_OpaqueColor, 
								composite_copy_op_type(),
								&rect
							 );

						}

						// render the layer visual
						// ----------------------------------------------------

						if ( CBpaintDoc::DODADS::DISABLE_IMAGE_RENDER & dwDodadFlags ) {

							if ( CBpaintDoc::DODADS::RENDER_SELECTED & dwDodadFlags ) {

								bVisible = bSelected;

							} else {

								bVisible = false;

							}

						}

						if ( bVisible ) {

							// Render the layer
							// ------------------------------------------------

							if ( bDrawForeground5050 && (i > currentLayer) ) {

								visual_to_composite_5050_type top_5050( pClut );

								pLayer->Render(
									surface, 0, 0, top_5050, &rect, 0
								);

							} else {

#if 1 // BPT 6/26/01 outline layers

								if ( bOutlineLayer ) {

									BPT::TSingleColorTransferROP<
										composite_pixel_type
										,BPT::CVisualElement::storage_type
									> outline_top( layerOutlineColor );
	
									pLayer->Render( surface, 0, -1, outline_top, &rect, 0 ); // U
									pLayer->Render( surface, -1, 0, outline_top, &rect, 0 ); // L
									pLayer->Render( surface, +1, 0, outline_top, &rect, 0 ); // R
									pLayer->Render( surface, 0, +1, outline_top, &rect, 0 ); // D

								}

#endif

								if ( bForegroundMode ) {

									visual_to_foreground_type top( pClut );

									pLayer->Render( surface, 0, 0, top, &rect, 0 );

								} else {

									visual_to_composite_copy_type top( pClut );

									pLayer->Render( surface, 0, 0, top, &rect, 0 );

								}

							}
	
						}

						// adjust the bounding rectangle?
						// --------------------------------------------------------

						if ( CBpaintDoc::DODADS::EXPAND_BOUNDING_RECT & dwDodadFlags ) {

							InflateRect( &renderedRect, 1, 1 );

						}

						// Render the bounding rect ?
						// --------------------------------------------------------

						if ( CBpaintDoc::DODADS::BOUNDING_RECTS & dwDodadFlags ) {

							bool bRenderBoundingRect;

							if ( bSelected ) {

								bRenderBoundingRect = true;

							} else {

								bRenderBoundingRect = (0 == (CBpaintDoc::DODADS::RENDER_SELECTED & dwDodadFlags));

							}

							if ( bRenderBoundingRect ) {

								 BPT::T_RectPrim(
								 	surface, renderedRect, 
									bSelected ? m_SelectedBoundingRectColor : m_BoundingRectColor, 
									composite_copy_op_type(),
									&rect
								 );

							}
	
						}

						// Render the handles?
						// --------------------------------------------------------

						composite_pixel_type handleColor = 
							bSelected ? m_SelectedHandleColor : m_HandleColor;

						CSize handleSize( 3, 3 );
						
						int xo = handleSize.cx / 2;
						int yo = handleSize.cy / 2;
						
						if ( CBpaintDoc::DODADS::HANDLES & dwDodadFlags ) {
						
							CRect a( CPoint( renderedRect.left - xo, renderedRect.top - yo ), handleSize );
							CRect b( CPoint( renderedRect.right - 1 - xo, renderedRect.top - yo ), handleSize );
							CRect c( CPoint( renderedRect.left - xo, renderedRect.bottom - 1 - yo ), handleSize );
							CRect d( CPoint( renderedRect.right - 1 - xo, renderedRect.bottom - 1 - yo ), handleSize );
													
							BPT::T_RectPrim( surface, a, handleColor, composite_copy_op_type(), &rect );
							BPT::T_RectPrim( surface, b, handleColor, composite_copy_op_type(), &rect );
							BPT::T_RectPrim( surface, c, handleColor, composite_copy_op_type(), &rect );
							BPT::T_RectPrim( surface, d, handleColor, composite_copy_op_type(), &rect );
						
						}

					}

				}
	
			}
	
		}

	}
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderCompositeModePrim()
//

void 
CBpaintDoc::RenderCompositeModePrim(
	dib_type::bitmap_type & surface, const RECT & rect, 
	bool bOnlyActiveLayer /* = false */,
	bool bUseVisibilityInfo /* = true */,
	DWORD dwDodadFlags /* = 0 */
) {

	int nLayers = (0 != m_pAnimation) ? m_pAnimation->LayerCount() : 0;

	RenderCompositeModePrimEx(
		surface, rect, bOnlyActiveLayer, bUseVisibilityInfo, dwDodadFlags,
		0, (nLayers - 1), true, false
	);

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::CalcLinkPointRect()
//

bool 
CBpaintDoc::CalcLinkPointRect( CRect & outRect, const POINT & pt )
{
	// This will be used to help with invalidating only the link point areas

	int w = 16;
	int h = 16;

	int tw = 256; // Hack for text width
	int th = 24; // Hack for text height

	outRect.left = pt.x - w;
	outRect.right = pt.x + max( w, tw ) + 1;
	outRect.top = pt.y - h;
	outRect.bottom = pt.y + max( h, th ) + 1;

	return true;
}

//
//	CBpaintDoc::RenderLinkPointPrim()
//

void 
CBpaintDoc::RenderLinkPointPrim( 
	dib_type::bitmap_type & surface, const POINT & pt, 
	const composite_pixel_type color, const RECT * pClipRect,
	CDC * pDC, const char * lpszPrefixText
)
{
	if ( pt.x || pt.y ) {

		CRect rect;

		CalcLinkPointRect( rect, pt );

		if (m_bShowSpotInfo) {

			BPT::T_HLinePrim( 
				surface, rect.left, pt.y, rect.Width(), color,
				composite_copy_op_type(), pClipRect
			);

			BPT::T_VLinePrim( 
				surface, pt.x, rect.top, rect.Height(), color,
				composite_copy_op_type(), pClipRect
			);

		} else {

			BPT::T_HLinePrim( 
				surface, pt.x - 4, pt.y, 9, color,
				composite_copy_op_type(), pClipRect
			);
			
			BPT::T_VLinePrim( 
				surface, pt.x, pt.y - 4, 9, color,
				composite_copy_op_type(), pClipRect
			);
			
		}

		if ( pDC ) {

			pDC->SetTextColor( DisplayPixel_COLORREF( color ) );

			CString text;

			if (m_bShowSpotInfo) {

				text.Format( "%s%d,%d", lpszPrefixText, pt.x, pt.y );

			}

			if ( pClipRect ) {

				pDC->ExtTextOut( pt.x, pt.y, ETO_CLIPPED, pClipRect, text, NULL );

			} else {

				pDC->ExtTextOut( pt.x, pt.y, 0, NULL, text, NULL );

			}

		}

	}
}

//
//	CBpaintDoc::RenderLinkPoints()
//

void
CBpaintDoc::RenderLinkPoints(
	dib_type::bitmap_type & surface, const RECT * pClipRect, CDC * pDC
)
{
	if ( m_bRenderLinkPointOverlays && m_pAnimation ) {

		if ( pDC ) {

			pDC->SetBkMode( TRANSPARENT );
			pDC->SetTextAlign( TA_LEFT | TA_TOP | TA_NOUPDATECP );

		}

		RenderLinkPointPrim( 
			surface, m_pAnimation->m_Link1, m_AnimationLinkPointColor, 
			pClipRect, pDC, " (a) "
		);

		BPT::CFrame * pFrame = m_pAnimation->ActiveFrame();

		if ( pFrame ) {

			// Render the 4 standard link points
			// ----------------------------------------------------------------

			RenderLinkPointPrim( 
				surface, pFrame->m_Link1, m_FrameLink1Color, pClipRect, pDC, " (1) "
			);
	
			RenderLinkPointPrim( 
				surface, pFrame->m_Link2, m_FrameLink2Color, pClipRect, pDC, " (2) "
			);

			RenderLinkPointPrim( 
				surface, pFrame->m_Link3, m_FrameLink3Color, pClipRect, pDC, " (3) "
			);

			RenderLinkPointPrim( 
				surface, pFrame->m_Link4, m_FrameLink4Color, pClipRect, pDC, " (4) "
			);

			// Render the link point adjustments
			// ----------------------------------------------------------------

			BPT::CAnimation::named_spot_collection_type & spotCollection = m_pAnimation->NamedSpotCollection();

			BPT::CAnimation::named_spot_collection_type::iterator spotIt = spotCollection.begin();

			for ( ;spotIt != spotCollection.end(); spotIt++ ) {

				// Get the spot color
				// ------------------------------------------------------------

				composite_pixel_type spotColor;

				if (m_pCurrentNamedSpot == *spotIt) {

					spotColor = MakeDisplayPixelType( 0, 128, 0 );
					
				} else {

					spotColor = MakeDisplayPixelType( 128, 0, 0 );

				}

				// Render the delta
				// ------------------------------------------------------------

				BPT::CNamedSpotAdjustment * pAdjustment = pFrame->FindSpotAdjusterFor( *spotIt );

				if ( pAdjustment ) {

					// Prep the display info
					// --------------------------------------------------------

					POINT pos = pAdjustment->GetPosition();

					CString spotDescription;

					char * pName = pAdjustment->GetName();

					if ( !pName ) {

						pName = "Error?";

					}

					if (m_bShowSpotInfo) {

						spotDescription.Format( " (*%s) ", pName );

					}
	
					// Finally display it
					// --------------------------------------------------------

					RenderLinkPointPrim( 
						surface, pos, spotColor, 
						pClipRect, pDC, spotDescription
					);

				} else {

					// Render the original location
					// --------------------------------------------------------

					POINT pos = (*spotIt)->GetLocation();

					CString spotDescription;

					char * pName = (*spotIt)->GetName();

					if ( !pName ) {

						pName = "Error?";

					}

					if (m_bShowSpotInfo) {

						spotDescription.Format( " (%s) ", pName );

					}
	
					// Finally display it
					// --------------------------------------------------------

					RenderLinkPointPrim( 
						surface, pos, spotColor, 
						pClipRect, pDC, spotDescription
					);

				}

			}

		}

	}
}

// ----------------------------------------------------------------------------

// FUTURE -- If these functions actually counted the number of views it may
// help speed up composite mode when one of they types isn't actually viewed.

int CBpaintDoc::CountCanvasViewsUsingDib( dib_type * pQueryDib )
{
	int viewCounter = 0;

	POSITION pos = GetFirstViewPosition();

	while ( NULL != pos ) {

		CView * pView = GetNextView( pos );

		if ( pView ) {

			if ( pView->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

				dib_type * pDib = ((CBpaintView*)pView)->GetCanvasDib();

				if ( pDib == pQueryDib ) {

					++viewCounter;

				}

			}

		}

	}

	return viewCounter;

}

//
//	CBpaintDoc::ActiveEditViewCount()
//

int 
CBpaintDoc::ActiveEditViewCount()
{
	return CountCanvasViewsUsingDib( &m_EditViewDib );
}

//
//	CBpaintDoc::ActiveCompositeViewCount()
//

int 
CBpaintDoc::ActiveCompositeViewCount()
{
	return CountCanvasViewsUsingDib( &m_CompositeViewDib );
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderCompositeModeRect()
//

void 
CBpaintDoc::RenderCompositeModeRect( const RECT & rect )
{
	// ------------------------------------------------------------------------

	DWORD dwDoDads = m_dwDoDadFlags;

	if ( CBpaintDoc::MODE::NOT_PENDING != m_PendingMode ) {

		dwDoDads = 0; // turn off do dads while a mode is pending, icky!

	}

	// ------------------------------------------------------------------------

	if ( ActiveEditViewCount() ) {

		dib_type::bitmap_type * pEditSurface = m_EditViewDib.GetBitmapPtr();
	
		if ( pEditSurface ) {
	
			RenderCompositeModePrim( *pEditSurface, rect, true, true, dwDoDads );

			// Render overlays
		
			RenderOverlay( pEditSurface, &rect );

			// Render the link points

			BPT::TDibDC<dib_type> dibDC( &m_EditViewDib );

			RenderLinkPoints( *pEditSurface, &rect, &dibDC );

		}

	}

	// ------------------------------------------------------------------------

	if ( ActiveCompositeViewCount() ) {

		dib_type::bitmap_type * pCompositeSurface = m_CompositeViewDib.GetBitmapPtr();
	
		if ( pCompositeSurface ) {
		
			RenderCompositeModePrim( *pCompositeSurface, rect, false, true, dwDoDads );

			// Render overlays

			RenderOverlay( pCompositeSurface, &rect );

			// Render the link points

			BPT::TDibDC<dib_type> dibDC( &m_CompositeViewDib );
			
			RenderLinkPoints( *pCompositeSurface, &rect, &dibDC );

		}
	
	}

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderEditLayerOutline()
//

void CBpaintDoc::RenderEditLayerOutline(
	composite_bitmap_type * pCanvasBitmap,
	editor_bitmap_type * pEditorBitmap,
	const RECT * pRect
)
{

#if 1 // BPT 6/26/01

	if ( m_pAnimation ) {

		// Get the palette for this layer
		// ----------------------------------------------------------------

		int outlineColorA = m_pAnimation->GetOutlineColorA();
		int outlineColorB = m_pAnimation->GetOutlineColorB();

		BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo(
			m_pAnimation->CurrentLayer()
		);

		// ----------------------------------------------------------------

		BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

		if ( pLayerInfo && pPal ) {

			bool bOutlineLayer = false;
	
			composite_pixel_type layerOutlineColor( 0 );

			// ------------------------------------------------------------

			if ( pLayerInfo->CheckFlags( BPT::CLayerInfo::OUTLINE_COLOR_A ) ) {

				bOutlineLayer = true;

				layerOutlineColor = pPal->GetCLUTValue( outlineColorA );

			} else if ( pLayerInfo->CheckFlags( BPT::CLayerInfo::OUTLINE_COLOR_B ) ) {

				bOutlineLayer = true;

				layerOutlineColor = pPal->GetCLUTValue( outlineColorB );

			}

			// ------------------------------------------------------------

			if ( bOutlineLayer ) {

				BPT::TSingleColorTransparentSrcTransferROP<
					composite_pixel_type, editor_pixel_type
				> outlineTop( layerOutlineColor, m_ChromaKey );
				
				BPT::T_Blit( *pCanvasBitmap, -1, 0, *pEditorBitmap, outlineTop, 0, pRect, 0 );
				BPT::T_Blit( *pCanvasBitmap, 0, -1, *pEditorBitmap, outlineTop, 0, pRect, 0 );
				BPT::T_Blit( *pCanvasBitmap, +1, 0, *pEditorBitmap, outlineTop, 0, pRect, 0 );
				BPT::T_Blit( *pCanvasBitmap, 0, +1, *pEditorBitmap, outlineTop, 0, pRect, 0 );
				
			}

		}

	}

#endif

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderEditOnlyRect()
//

void CBpaintDoc::RenderEditOnlyRect( const RECT & rect )
{
	// Get the editor bitmap and the canvas bitmap
	// ------------------------------------------------------------------------

	dib_type::bitmap_type * pCanvasBitmap = m_EditViewDib.GetBitmapPtr();

	editor_bitmap_type * pEditorBitmap = m_EditorLogic.GetCurentBitmapPtr();

// There needs to be logic about what area's of the view dibs are seen, this
// will help keep the speed snappy.

	if ( pCanvasBitmap && pEditorBitmap ) {

		if ( m_bUseBackgroundRendererInEditOnlyViews && 
			(m_bUseBackdropSurface || m_bOnionSkinMode) ) {

			// This should be either a checker board or a solid color
	
			RenderBackgroundRect( *pCanvasBitmap, rect );

#if 1 // BPT 6/26/01

			// Render the outline of the edit layer
			// ----------------------------------------------------------------

			RenderEditLayerOutline( pCanvasBitmap, pEditorBitmap, &rect );

#endif
	
			// Render editor layer
			// ----------------------------------------------------------------
	
#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

			BPT::TSrcChromakeyAlphaClutTransfer<
				composite_pixel_type, editor_pixel_type, 
				editor_to_composite_clut_type
			> op( *GetCurrentCLUT(), m_ChromaKey );

			BPT::T_Blit( *pCanvasBitmap, 0, 0, *pEditorBitmap, op, 0, &rect, 0 );

#else // defined(TEST_BPAINT_TRUE_COLOR_TEST)

			BPT::TSrcChromakeyClutTransfer<
				composite_pixel_type, editor_pixel_type, 
				editor_to_composite_clut_type
			> op( *GetCurrentCLUT(), m_ChromaKey );
	
			BPT::T_Blit( *pCanvasBitmap, 0, 0, *pEditorBitmap, op, 0, &rect, 0 );

#endif // defined(TEST_BPAINT_TRUE_COLOR_TEST)

		} else {

			// This is the opaque mode for the editor other modes could be easily 
			// added!!!!
	
			// ----------------------------------------------------------------
	
			editor_to_edit_only_op_type op( GetCurrentCLUT() );
	
			// Render editor layer
			// ----------------------------------------------------------------
	
			BPT::T_Blit( *pCanvasBitmap, 0, 0, *pEditorBitmap, op, 0, &rect, 0 );

		}

		// Render overlay
		// --------------------------------------------------------------------

		RenderOverlay( pCanvasBitmap, &rect );

		// Render the link points

		BPT::TDibDC<dib_type> dibDC( &m_EditViewDib );

		RenderLinkPoints( *pCanvasBitmap, &rect, &dibDC );

	}

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderEditCompositeRect()
//

void CBpaintDoc::RenderEditCompositeRect( const RECT & rect )
{
	// Get the editor bitmap and the canvas bitmap
	// ------------------------------------------------------------------------

	dib_type::bitmap_type * pCanvasBitmap = m_CompositeViewDib.GetBitmapPtr();

	editor_bitmap_type * pEditorBitmap = m_EditorLogic.GetCurentBitmapPtr();

// There needs to be logic about what area's of the view dibs are seen, this
// will help keep the speed snappy.

	if ( pCanvasBitmap && pEditorBitmap ) {

		// render previous layers
		// --------------------------------------------------------------------

#if 1

		if ( m_bHasBackgroundLayers ) {

			BPT::T_Blit(
				*pCanvasBitmap, 0, 0, m_BackgroundSurface, 
				composite_copy_op_type(), 0, &rect, 0
			);

		} else {

			// This should be either a checker board or a solid color

			RenderBackgroundRect( *pCanvasBitmap, rect );

		}

#else

		RenderBackgroundRect( *pCanvasBitmap, rect );

#endif

#if 1 // BPT 6/26/01

		// Render the outline of the edit layer
		// ----------------------------------------------------------------

		RenderEditLayerOutline( pCanvasBitmap, pEditorBitmap, &rect );

#endif
		// Render editor layer
		// --------------------------------------------------------------------

//#if 1 // use specialized transfer op to speed up the editor to composite blit
#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

	#if 1 // this makes it faster but the code isn't as clear as it should be!!!

		BPT::TSrcChromakeyAlphaClutTransfer<
			composite_pixel_type, editor_pixel_type, 
			editor_to_composite_clut_type
		> op( *GetCurrentCLUT(), m_ChromaKey );

		BPT::T_Blit( *pCanvasBitmap, 0, 0, *pEditorBitmap, op, 0, &rect, 0 );

	#else

		editor_to_composite_op_type op(
			GetCurrentCLUT()
			,editor_to_composite_op_type::predicate_type( m_ChromaKey )
		);

		BPT::T_Blit( *pCanvasBitmap, 0, 0, *pEditorBitmap, op, 0, &rect, 0 );

	#endif

#else

		BPT::TSrcChromakeyClutTransfer<
			composite_pixel_type, editor_pixel_type, 
			editor_to_composite_clut_type
		> op( *GetCurrentCLUT(), m_ChromaKey );

		BPT::T_Blit( *pCanvasBitmap, 0, 0, *pEditorBitmap, op, 0, &rect, 0 );

#endif // defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

		// Render next layers.
		// --------------------------------------------------------------------

		if ( m_bHasForegroundLayers ) {

			if ( m_CompressedForegroundImage.HasData() ) {

				if ( m_bDrawForeground5050 ) {

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

					ASSERT( false ); // This needs to be fixed!

#endif

					m_CompressedForegroundImage.Blit(
						*pCanvasBitmap, 0, 0, 
						BPT::T16bpp_5050_OP<composite_pixel_type,0x3def>(),
						0, &rect, 0
					);

				} else {

					m_CompressedForegroundImage.Blit(
						*pCanvasBitmap, 0, 0, 
						fg_compressed_composite_image_copy_type(),
						0, &rect, 0
					);

				}

			} else {

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03
				ASSERT( false ); // This needs to be fixed!
#endif

#if 1 // now using a type from the document class instead of inlined types

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03
				BPT::T_Blit(
					*pCanvasBitmap, 0, 0, m_ForegroundSurface, 
					fg_composite_image_copy_type(),
					0, &rect, 0
				);
#else
				BPT::T_Blit(
					*pCanvasBitmap, 0, 0, m_ForegroundSurface, 
					fg_composite_image_copy_type( m_CompositeChromaKey ),
					0, &rect, 0
				);
#endif

#else
				BPT::T_Blit(
					*pCanvasBitmap, 0, 0, m_ForegroundSurface, 
					BPT::TTransparentSrcTransferROP<composite_pixel_type>( m_CompositeChromaKey ),
					0, &rect, 0
				);
#endif

			}
			
		}

		// Render overlay
		// --------------------------------------------------------------------

		RenderOverlay( pCanvasBitmap, &rect );

		// Render the link points

		BPT::TDibDC<dib_type> dibDC( &m_CompositeViewDib );

		RenderLinkPoints( *pCanvasBitmap, &rect, &dibDC );

	}

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::RenderRect()
//

void CBpaintDoc::RenderRect( const RECT & rect )
{

#if defined(BPAINT_PERFORMANCE_STATS)
	LARGE_INTEGER iTime, oTime;

	if ( m_bHasPerformanceTimer ) {

		QueryPerformanceCounter( &iTime );

	}
#endif

	// ------------------------------------------------------------------------

	switch ( m_CurrentMode ) {

	default:
	case CBpaintDoc::MODE::COMPOSITE:
		RenderCompositeModeRect( rect );
		break;

	case CBpaintDoc::MODE::DRAWING:
		if ( ActiveCompositeViewCount() ) { // BPT 6/26/01 speed improvement
			RenderEditCompositeRect( rect );
		}
		if ( ActiveEditViewCount() ) { // BPT 6/26/01 speed improvement
			RenderEditOnlyRect( rect );
		}
		break;

	}

#if defined(BPAINT_PERFORMANCE_STATS)

	if ( m_bHasPerformanceTimer ) {

		QueryPerformanceCounter( &oTime );

		__int64 i = iTime.QuadPart;
		__int64 o = oTime.QuadPart;

		m_CheeseRenderTookValue = (o - i) / m_QueryPerformanceFrequency;

	} else {

		m_CheeseRenderTookValue = 0;

	}
	
#endif

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::AddDirtyRect()
//

void 
CBpaintDoc::AddDirtyRect( const RECT & rect )
{

	RECT adjustedRect = rect;

#if 1 // BPT 6/27/01

	// Make the size of the dirty rect 1 bigger to update any outlines
	// ------------------------------------------------------------------------

	InflateRect( &adjustedRect, 1, 1 );

	if ( !IntersectRect( &adjustedRect, &adjustedRect, &m_ForegroundSurface.Rect() ) ) {

		return; // NOP

	}

#endif

	CEditorRectChange changedRect( adjustedRect );

	// Transfer the editor pixels to the composite view dib
	// ------------------------------------------------------------------------

	RenderRect( adjustedRect );

	// This probably should queue these up and then send them out later.
	// ------------------------------------------------------------------------

// SPEED DESIGN
// SPEED DESIGN

	UpdateAllViews( NULL, CEditorRectChange::INVALIDATE_RECT, (CObject *)&changedRect );

// SPEED DESIGN
// SPEED DESIGN

}

// ============================================================================

//
//	CBpaintDoc::MassageEditMessageInfo()
//

void 
CBpaintDoc::MassageEditMessageInfo( CView * pView, UINT & nFlags, CPoint & point)
{

	// Store off input coordinate for potential later use.
	// ------------------------------------------------------------------------

	m_RawInputCoordinate = point;

	// Massage the incoming point?
	// ------------------------------------------------------------------------

	if ( m_bGridOn ) {

		m_InputGrid.Transform( &point );

	}

	// If this message came from a view and is the correct type
	// then ask that view to keep the logical point in view.
	// ------------------------------------------------------------------------

	if ( pView ) {

		if ( pView->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

			((CBpaintView*)pView)->MakeLogicalPointVisible( point );

		}

	}

	// ------------------------------------------------------------------------

	UpdateStatusBarWithEditMsgInfo( nFlags, point );

	m_ptLastKnownLocation = point;
}

void 
CBpaintDoc::PostEditToolProcessing()
{

#if 0 // HACK!!!

	if ( NULL == ::GetCapture() ) {

		m_EditorLogic.PostToolHack(); // evil???

		m_EditorLogic.SetPen( m_pCurrentPen = GetLButtonPen() );

	}

#endif

}

//
//	CBpaintDoc::Dispatch_OnLButtonDown(
//

void 
CBpaintDoc::Dispatch_OnLButtonDown(CView * pView, UINT nFlags, CPoint point)
{
	CViewTracker vt( m_pTemporaryTrackedView, pView );

	MassageEditMessageInfo( pView, nFlags, point );

	// ------------------------------------------------------------------------

	m_bDrawingButtonIsDown = true;

	// ------------------------------------------------------------------------

	OnPendingMode(); // GOOFY

	switch ( m_CurrentMode ) {

	case CBpaintDoc::MODE::DRAWING:
		if ( EnsureActiveEditor() ) {

#if 1  // BPT 11/26/02

			// monochrome auto erase mode
			// if pixel is the 'L' color use the R brush
			// otherwise use the L brush

			pen_base_type * pUsePen = GetLButtonPen();

			if ( m_bAutoEraseEditMode ) {

				paint_logic_type::bitmap_type::pointer pEditCanvas = 
					m_EditorLogic.GetFinalBitmapPtr(); // GetCurentBitmapPtr();

				if ( pEditCanvas ) {

					if ( (0 <= point.x) && (0 <= point.y) && 
						(pEditCanvas->Width() > point.x) && (pEditCanvas->Height() > point.y) ) {

						editor_pixel_type overColor = *pEditCanvas->Iterator( point.x, point.y );

						if ( overColor != m_ChromaKey ) {

							pUsePen = GetRButtonPen();

						}

					}

				}

			}

			m_EditorLogic.SetPen( m_pCurrentPen = pUsePen );

#else

			m_EditorLogic.SetPen( m_pCurrentPen = GetLButtonPen() );

#endif

			m_EditorLogic.Edit_OnDown( nFlags, point );
		}
		break;

	case CBpaintDoc::MODE::COMPOSITE:
		Composite_OnLButtonDown( pView, nFlags, point );
		break;

	}

}

//
//	CBpaintDoc::Dispatch_OnLButtonUp()
//

void
CBpaintDoc::Dispatch_OnLButtonUp(CView * pView, UINT nFlags, CPoint point)
{
	CViewTracker vt( m_pTemporaryTrackedView, pView );

	MassageEditMessageInfo( pView, nFlags, point );

	// ------------------------------------------------------------------------

	OnPendingMode(); // GOOFY

	switch ( m_CurrentMode ) {

	case CBpaintDoc::MODE::DRAWING:
		if ( EnsureActiveEditor() ) {
			m_EditorLogic.Edit_OnUp( nFlags, point );
			PostEditToolProcessing();
		}
		break;

	case CBpaintDoc::MODE::COMPOSITE:
		Composite_OnLButtonUp( pView, nFlags, point );
		break;

	}

	// ------------------------------------------------------------------------

	m_bDrawingButtonIsDown = false;

}

//
//	CBpaintDoc::HandleAutoFocus()
//

void 
CBpaintDoc::HandleAutoFocus( CView * pView )
{
	if ( m_AutoFocus && (NULL == ::GetCapture()) && pView ) {

		if ( pView->GetSafeHwnd() != GetFocus() ) {

			CFrameWnd * pFrame = pView->GetParentFrame();
	
			if ( pFrame ) {
	
				CFrameWnd * ppFrame = pFrame->GetParentFrame();
	
				if ( ppFrame ) {
	
					ppFrame->ActivateFrame();
	
				}
	
				pFrame->ActivateFrame();
				pFrame->SetFocus();
				pFrame->SetActiveView( pView );

				// 'promote' the view title to the frame 

				CString viewTitle;
				pView->GetWindowText( viewTitle );
				pFrame->SetWindowText( viewTitle );
	
			}

		}

	}

}

//
//	CBpaintDoc::Dispatch_OnMouseMove()
//

void 
CBpaintDoc::Dispatch_OnMouseMove(CView * pView, UINT nFlags, CPoint point)
{
	CViewTracker vt( m_pTemporaryTrackedView, pView );

	MassageEditMessageInfo( pView, nFlags, point );

	// automaticly switch to view that sent the mouse move message if
	// ------------------------------------------------------------------------

	HandleAutoFocus( pView );

	// ------------------------------------------------------------------------

	m_bDrawingButtonIsDown = (0 != ((MK_LBUTTON|MK_RBUTTON) & nFlags));

	// ------------------------------------------------------------------------

	OnPendingMode(); // GOOFY

	switch ( m_CurrentMode ) {

	case CBpaintDoc::MODE::DRAWING:
		if ( EnsureActiveEditor() ) {

#if 1 // hack to make sure a pen is active 

		if ( NULL == ::GetCapture() ) {

			if ( GetLButtonPen() != m_EditorLogic.GetPen() ) {

				m_EditorLogic.SetPen( m_pCurrentPen = GetLButtonPen() );

			}

		}

#endif

			m_EditorLogic.Edit_OnMove( nFlags, point );

		}
		break;

	case CBpaintDoc::MODE::COMPOSITE:
		Composite_OnMouseMove( pView, nFlags, point );
		break;

	}

}

//
//	CBpaintDoc::Dispatch_OnRButtonDown()
//

void 
CBpaintDoc::Dispatch_OnRButtonDown(CView * pView, UINT nFlags, CPoint point)
{
	CViewTracker vt( m_pTemporaryTrackedView, pView );

	MassageEditMessageInfo( pView, nFlags, point );

	// ------------------------------------------------------------------------

	m_bDrawingButtonIsDown = true;

	// ------------------------------------------------------------------------

	OnPendingMode(); // GOOFY

	switch ( m_CurrentMode ) {

	case CBpaintDoc::MODE::DRAWING:
		if ( EnsureActiveEditor() ) {
			m_EditorLogic.SetPen( m_pCurrentPen = GetRButtonPen() );
			m_EditorLogic.Edit_OnDown( nFlags, point );
		}
		break;

	case CBpaintDoc::MODE::COMPOSITE:
		Composite_OnRButtonDown( pView, nFlags, point );
		break;

	}

}

//
//	CBpaintDoc::Dispatch_OnRButtonUp()
//

void 
CBpaintDoc::Dispatch_OnRButtonUp(CView * pView, UINT nFlags, CPoint point)
{
	CViewTracker vt( m_pTemporaryTrackedView, pView );

	MassageEditMessageInfo( pView, nFlags, point );

	// ------------------------------------------------------------------------

	OnPendingMode(); // GOOFY

	switch ( m_CurrentMode ) {

	case CBpaintDoc::MODE::DRAWING:
		if ( EnsureActiveEditor() ) {
			m_EditorLogic.Edit_OnUp( nFlags, point );
			PostEditToolProcessing();
		}
		break;

	case CBpaintDoc::MODE::COMPOSITE:
		Composite_OnRButtonUp( pView, nFlags, point );
		break;

	}

	// ------------------------------------------------------------------------

	m_bDrawingButtonIsDown = false;

}

//
//	CBpaintDoc::Dispatch_OnCancelMode()
//

void 
CBpaintDoc::Dispatch_OnCancelMode(CView * pView)
{
	// ------------------------------------------------------------------------

	m_bDrawingButtonIsDown = false;

	// ------------------------------------------------------------------------

	OnPendingMode(); // GOOFY

	switch ( m_CurrentMode ) {

	case CBpaintDoc::MODE::DRAWING:
		if ( EnsureActiveEditor() ) {
			m_EditorLogic.Edit_OnCancel();
			PostEditToolProcessing();
		}
		break;

	case CBpaintDoc::MODE::COMPOSITE:
		Composite_OnCancelMode( pView );
		break;

	}

}

//
//	CBpaintDoc::Dispatch_OnMButtonDown()
//

void 
CBpaintDoc::Dispatch_OnMButtonDown(CView * pView, UINT nFlags, CPoint point)
{
//	MassageEditMessageInfo( pView, nFlags, point );
}

//
//	CBpaintDoc::Dispatch_OnMButtonUp()
//

void 
CBpaintDoc::Dispatch_OnMButtonUp(CView * pView, UINT nFlags, CPoint point)
{
//	MassageEditMessageInfo( pView, nFlags, point );
}

// ============================================================================

int
CBpaintDoc::Standardize_DTOOL_Command( const int id )
{
	if ( (ID_DTOOL_FREEHAND_LINE <= id) && (ID_DTOOL_SOLID_SQUARE >= id) ) {

		return id;

	}

	return (id - ID_2_DTOOL_FREEHAND_LINE) + ID_DTOOL_FREEHAND_LINE;
}

int 
CBpaintDoc::Standardize_QBRUSH_Command( const int id )
{
	if ( (ID_QBRUSH_ELLIPSE_0 <= id) && (ID_QBRUSH_ELLIPSE_N >= id) ) {

		return id;

	}

	return (id - ID_2_QBRUSH_ELLIPSE_0) + ID_QBRUSH_ELLIPSE_0;
}

int 
CBpaintDoc::Standardize_COLOR_Command( const int id )
{
	if ( (ID_COLOR_INC <= id) && (ID_COLOR_PREVIOUS >= id) ) {

		return id;

	}

	return (id - ID_2_COLOR_INC) + ID_COLOR_INC;
}

// ============================================================================

void CBpaintDoc::OnGridSettings() 
{
	CGridSettingsDlg dlg( AfxGetMainWnd() );

	dlg.m_grid_cx = m_InputGrid.m_GridSize.cx;
	dlg.m_grid_cy = m_InputGrid.m_GridSize.cy;

	dlg.m_offset_x = m_InputGrid.m_GridOffset.x;
	dlg.m_offset_y = m_InputGrid.m_GridOffset.y;

	if ( dlg.DoModal() ) {

		m_InputGrid.m_GridSize.cx = dlg.m_grid_cx;
		m_InputGrid.m_GridSize.cy = dlg.m_grid_cy;
	
		m_InputGrid.m_GridOffset.x = dlg.m_offset_x;
		m_InputGrid.m_GridOffset.y = dlg.m_offset_y;

		GLOBAL_GetSettingInt( "grid.x", dlg.m_offset_x );
		GLOBAL_GetSettingInt( "grid.y", dlg.m_offset_y );
		GLOBAL_GetSettingInt( "grid.cx", dlg.m_grid_cx );
		GLOBAL_GetSettingInt( "grid.cy", dlg.m_grid_cy );

	}
}

// ============================================================================

void CBpaintDoc::Request_PreviousDrawingTool()
{
	if ( m_PreviousDrawingTool ) {

		AfxGetMainWnd()->PostMessage( WM_COMMAND, m_PreviousDrawingTool, 0 );

	}
}

void
CBpaintDoc::OnDrawingTool( UINT nID )
{
	// Match the id to a tool message for distribution to the
	// paint logic.

#if 1 //

	SetDocumentMode( CBpaintDoc::MODE::DRAWING );

#endif

	int activeTool = m_EditorLogic.GetCurrentToolID();

	nID = Standardize_DTOOL_Command( nID );

	if ( ID_DTOOL_PICKUP_BRUSH != activeTool ) {

		m_PreviousDrawingTool = activeTool;

	}

	switch ( nID ) {

	default:
		m_EditorLogic.SelectTool( nID );
		break;

	case ID_DTOOL_GRID:

#if 1

		if ( 0x8000 & GetAsyncKeyState( VK_SHIFT ) ) {

			OnGridSettings();

			m_bGridOn = true;

		} else {

			m_bGridOn = !m_bGridOn;

		}

		if ( m_bGridOn ) {

			m_EditorLogic.SetGridInfo( &m_InputGrid );

		} else {

			m_EditorLogic.SetGridInfo( 0 );

		}

#else

		m_bGridOn = !m_bGridOn;

#endif
		break;

	case ID_DTOOL_UNDO:
		m_EditorLogic.Undo();
		break;

	case ID_DTOOL_CLEAR:
		m_EditorLogic.Clear( GetClearColor() );
		break;

	case ID_DTOOL_STAMP_BRUSH:
		m_EditorLogic.Stamp();
		break;

	}

}

void
CBpaintDoc::OnUpdateDrawingTool( CCmdUI* pCmdUI )
{
	int nID = Standardize_DTOOL_Command( pCmdUI->m_nID );

	BOOL bEnable = FALSE;

	switch ( nID ) {

	default:
		bEnable = ( 0 != m_EditorLogic.GetToolForId( nID ) ) ? TRUE : FALSE;
		pCmdUI->SetCheck( (nID == m_EditorLogic.GetCurrentToolID()) ? 1 : 0 );
		break;

	case ID_DTOOL_GRID:
		bEnable = TRUE;
		pCmdUI->SetCheck( m_bGridOn ? 1 : 0 );
		break;

	case ID_DTOOL_UNDO:
		bEnable = m_EditorLogic.CanUndo();
		break;

	case ID_DTOOL_CLEAR:
		bEnable = m_EditorLogic.CanClear();
		break;

	case ID_DTOOL_STAMP_BRUSH:
		bEnable = TRUE;
		break;

	}

	if ( IsDrawingModePossible() ) {

		pCmdUI->Enable( bEnable );

	} else {

		pCmdUI->Enable( FALSE );

	}

}

// ============================================================================

void
CBpaintDoc::OnQuickBrush( UINT nID )
{
	// Match the id to a quick brush handler for distribution to the
	// paint logic.

#if 1 // HACK!

	int standardID = Standardize_QBRUSH_Command( nID );

	switch ( standardID ) {

	default:
		break;

	case ID_QBRUSH_ELLIPSE_0:
	case ID_QBRUSH_ELLIPSE_1:
	case ID_QBRUSH_ELLIPSE_2:
	case ID_QBRUSH_ELLIPSE_3:
	case ID_QBRUSH_ELLIPSE_4:
		{
			m_LastPenSize = m_PenSize;
			int size = (standardID - ID_QBRUSH_ELLIPSE_0);
			m_PenSize.cx = size;
			m_PenSize.cy = size;

			SetBitmapBrushMode( false ); // hack

		}
		break;

	case ID_QBRUSH_SIZE_INC:

		if ( m_bUseBitmapBrushRenderer_HACK ) {

			CSize t = GetDrawingBrushSize();

			t.cx = (int)((float)t.cx * 1.1f);
			t.cy = (int)((float)t.cy * 1.1f);

			SetDrawingBrushSize( CSize( t.cx, t.cy ) );

		} else {

			m_LastPenSize = m_PenSize;
			++m_PenSize.cx;
			++m_PenSize.cy;

		} 
		break;

	case ID_QBRUSH_SIZE_DEC:
		if ( m_bUseBitmapBrushRenderer_HACK ) {

			CSize t = GetDrawingBrushSize();

			t.cx = (int)((float)t.cx * 0.9f);
			t.cy = (int)((float)t.cy * 0.9f);

			SetDrawingBrushSize( CSize( max( 1, t.cx), max(1, t.cy) ) );

		} else {

			m_LastPenSize = m_PenSize;
			--m_PenSize.cx;
			--m_PenSize.cy;

		}
		break;

	case ID_QBRUSH_ELLIPSE_N:
		{
			if ( m_bUseBitmapBrushRenderer_HACK ) {

				CSize t = GetDrawingBrushSize();
	
				if ( GetSizeDialog( 0, "Brush Size", t, &CSize( 2048, 2048 ) ) ) {

					SetDrawingBrushSize( t );

				}

			} else {

				CSize t = m_PenSize;
	
				if ( GetSizeDialog( 0, "Brush Size", m_PenSize, &CSize( 64, 64 ) ) ) {
	
					m_LastPenSize = t;
	
				}
	
				SetBitmapBrushMode( false ); // hack

			}

		}
		break;

	}

	if ( !m_EditorLogic.GetCurrentToolID() ) {

		AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_LINE, 0 );

	} else {

#if 1 // Make sure we're in edit mode?

		SetDocumentMode( CBpaintDoc::MODE::DRAWING );
	
#endif

	}

	RefreshPen();

#endif

}

void
CBpaintDoc::OnUpdateQuickBrush( CCmdUI* pCmdUI )
{

#if 1 // HACK!

	int standardID = Standardize_QBRUSH_Command( pCmdUI->m_nID );

	int possiblePenID;

	if ( m_PenSize.cx == m_PenSize.cy ) {

		possiblePenID = (m_PenSize.cx + ID_QBRUSH_ELLIPSE_0);

	} else {

		possiblePenID = 0;

	}

	switch ( standardID ) {

	default:
		pCmdUI->Enable();
		break;

	case ID_QBRUSH_ELLIPSE_0:
	case ID_QBRUSH_ELLIPSE_1:
	case ID_QBRUSH_ELLIPSE_2:
	case ID_QBRUSH_ELLIPSE_3:
	case ID_QBRUSH_ELLIPSE_4:
		pCmdUI->Enable();

		if ( m_bUseBitmapBrushRenderer_HACK ) {

			pCmdUI->SetCheck( 0 );

		} else {

			pCmdUI->SetCheck(
				((m_PenSize.cx == m_PenSize.cy) && ((m_PenSize.cx + ID_QBRUSH_ELLIPSE_0) == standardID)) ? 1 : 0
			);

		}
		break;

	case ID_QBRUSH_SIZE_INC:
		if ( m_bUseBitmapBrushRenderer_HACK ) {

			pCmdUI->Enable( TRUE );

		} else {

			if ( (64 > m_PenSize.cx) && (64 > m_PenSize.cy) ) {
				pCmdUI->Enable( TRUE );
			} else {
				pCmdUI->Enable( FALSE );
			}

		}
		break;

	case ID_QBRUSH_SIZE_DEC:
		if ( m_bUseBitmapBrushRenderer_HACK ) {

			pCmdUI->Enable( TRUE );

		} else {

			if ( (1 <= m_PenSize.cx) && (1 <= m_PenSize.cy) ) {
				pCmdUI->Enable( TRUE );
			} else {
				pCmdUI->Enable( FALSE );
			}

		}
		break;

	case ID_QBRUSH_ELLIPSE_N:
		pCmdUI->Enable();
		pCmdUI->SetCheck(
			((ID_QBRUSH_ELLIPSE_0 <= possiblePenID) && (ID_QBRUSH_ELLIPSE_4 >= possiblePenID)) ? 0 : 1
		);
		break;

	}

#endif


}

// ============================================================================

void
CBpaintDoc::OnColorCmd( UINT nID )
{
	int standardID = Standardize_COLOR_Command( nID );

	int newColor;

	switch ( standardID ) {

	case ID_COLOR_INC:
		newColor = m_Color_L + 1;
		if ( 256 <= newColor ) { // hack shoudl have some other logic here
			newColor = 0;
		}
		Request_L_ColorChange( newColor );
		break;

	case ID_COLOR_DEC:
		newColor = m_Color_L - 1;
		if ( 0 > newColor ) { // hack shoudl have some other logic here
			newColor = 255;
		}
		Request_L_ColorChange( newColor );
		break;

	case ID_COLOR_SET:
		break;

	case ID_COLOR_PREVIOUS:
		break;

	}

	RefreshPen();

}

void
CBpaintDoc::OnUpdateColorCmd( CCmdUI* pCmdUI )
{
	pCmdUI->Enable();
}

// ============================================================================

//
//	CBpaintDoc::RefreshPen()
//

void 
CBpaintDoc::RefreshPen()
{

// DESIGN
// DESIGN
// DESIGN

// THIS IS SUCH A HACK!
// THIS IS SUCH A HACK!
// THIS IS SUCH A HACK!
// THIS IS SUCH A HACK!

	GetRButtonPen();
	GetLButtonPen();

	m_EditorLogic.SetPen( m_pCurrentPen );

// DESIGN
// DESIGN
// DESIGN

}

//
//	GetClearColor()
//

CBpaintDoc::editor_pixel_type 
CBpaintDoc::GetClearColor()
{
	// THIS IS SUCH A HACK!

	return m_ChromaKey;
}

//
//	CBpaintDoc::GetLButtonPen()
//

CBpaintDoc::pen_base_type * 
CBpaintDoc::GetLButtonPen()
{
	// THIS IS SUCH A HACK!

	int color = m_Color_L;

	// --------------------

	if ( m_bUseBitmapBrushRenderer_HACK ) {

		switch ( m_nBrushPaintROP ) {

		default:
		case ID_ROP_MATTE:
			m_BitmapBrushRenderer.SetTransferOp(
				paint_brush_transfer_op_type(
					paint_brush_transfer_op_type::SOURCE, color, m_BrushChromaKey
				)
			);
			m_FlexiblePen.SetBrushRenderer( &m_BitmapBrushRenderer );
			break;

		case ID_ROP_SINGLE_COLOR:
			m_BitmapBrushRenderer.SetTransferOp(
				paint_brush_transfer_op_type(
					paint_brush_transfer_op_type::SINGLE, color, m_BrushChromaKey
				)
			);
			m_FlexiblePen.SetBrushRenderer( &m_BitmapBrushRenderer );
			break;

#if 0
		case ID_ROP_REPLACE:
			m_ReplaceBitmapBrushRenderer.SetTransferOp(
				paint_brush_replace_op_type()
			);
			m_FlexiblePen.SetBrushRenderer( &m_ReplaceBitmapBrushRenderer );
			break;
#endif

		case ID_ROP_KEEP_DIFF:
			m_DifferenceBitmapBrushRenderer.SetTransferOp(
				paint_brush_difference_op_type(
					&m_bDrawingButtonIsDown, true, color, m_BrushChromaKey
				)
			);
			m_FlexiblePen.SetBrushRenderer( &m_DifferenceBitmapBrushRenderer );
			break;

		case ID_ROP_KEEP_SAME:
			m_DifferenceBitmapBrushRenderer.SetTransferOp(
				paint_brush_difference_op_type(
					&m_bDrawingButtonIsDown, false, color, m_BrushChromaKey
				)
			);
			m_FlexiblePen.SetBrushRenderer( &m_DifferenceBitmapBrushRenderer );
			break;

		}

	} else {

		m_EllipseBrushRenderer.SetColor( color );
		m_EllipseBrushRenderer.SetSize( m_PenSize.cx, m_PenSize.cy );
		m_FlexiblePen.SetBrushRenderer( &m_EllipseBrushRenderer );

	}

	// --------------------

	m_OpRectRenderer.SetColor( color );
	m_FlexiblePen.SetRectRenderer( &m_OpRectRenderer );

	// --------------------

#if 1 // BPT 6/2/01 

	m_FlexiblePen.SetPixelOverrider( &m_HackPixelOverrider );

#endif

	// --------------------

	return &m_FlexiblePen;
}

//
//	CBpaintDoc::GetRButtonPen()
//

CBpaintDoc::pen_base_type * 
CBpaintDoc::GetRButtonPen()
{
	// THIS IS SUCH A HACK!

	int color = m_Color_R; //m_ChromaKey;

	// --------------------

	if ( m_bUseBitmapBrushRenderer_HACK ) {

		switch ( m_nBrushPaintROP ) {

		default:
		case ID_ROP_MATTE:
		case ID_ROP_SINGLE_COLOR:
			m_BitmapBrushRenderer.SetTransferOp(
				paint_brush_transfer_op_type(
					paint_brush_transfer_op_type::SINGLE, color, m_BrushChromaKey
				)
			);
			m_FlexiblePen.SetBrushRenderer( &m_BitmapBrushRenderer );
			break;

#if 0
		case ID_ROP_REPLACE:
			// THIS NEEDS TO DRAW WITH THE RIGHT COLOR
			// INSTEAD OF THE ACTUAL PIXELS
			m_ReplaceBitmapBrushRenderer.SetTransferOp(
				paint_brush_replace_op_type()
			);
			m_FlexiblePen.SetBrushRenderer( &m_ReplaceBitmapBrushRenderer );
			break;
#endif

		case ID_ROP_KEEP_DIFF:
			m_DifferenceBitmapBrushRenderer.SetTransferOp(
				paint_brush_difference_op_type(
					&m_bDrawingButtonIsDown, true, color, m_BrushChromaKey
				)
			);
			m_FlexiblePen.SetBrushRenderer( &m_DifferenceBitmapBrushRenderer );
			break;

		case ID_ROP_KEEP_SAME:
			m_DifferenceBitmapBrushRenderer.SetTransferOp(
				paint_brush_difference_op_type(
					&m_bDrawingButtonIsDown, false, color, m_BrushChromaKey
				)
			);
			m_FlexiblePen.SetBrushRenderer( &m_DifferenceBitmapBrushRenderer );
			break;

		}

	} else {

		m_EllipseBrushRenderer.SetColor( color );
		m_EllipseBrushRenderer.SetSize( m_PenSize.cx, m_PenSize.cy );
		m_FlexiblePen.SetBrushRenderer( &m_EllipseBrushRenderer );

	}

	// --------------------

	m_OpRectRenderer.SetColor( color );
	m_FlexiblePen.SetRectRenderer( &m_OpRectRenderer );

	// --------------------

#if 1 // BPT 6/2/01 

	m_FlexiblePen.SetPixelOverrider( &m_HackPixelOverrider );

#endif

	// --------------------

	return &m_FlexiblePen;
}

void CBpaintDoc::OnUpdateEditPalette(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

HCURSOR CBpaintDoc::GetViewCursor( CView * pView, const CPoint point, const UINT nHitTest )
{

#if 1

	if ( m_pPickupColorHackDlg ) {

		editor_pixel_type pickupColor;

		CView * pOldTrackedView = m_pTemporaryTrackedView;

		m_pTemporaryTrackedView = pView;
	
		if ( PickupColor( point.x, point.y, &pickupColor ) ) {

			bool bButtonDown = (0 != (0x8000 & GetAsyncKeyState( VK_LBUTTON )));

			if ( bButtonDown ) {
	
				m_pPickupColorHackDlg->Palette_ToggleSlotSelection( pickupColor );
	
			} else {
		
				m_pPickupColorHackDlg->Palette_UpdateInfoMessage( pickupColor );
	
			}

		}

		m_pTemporaryTrackedView = pOldTrackedView;

		return m_hEyeDropperCursor;

	}

#endif

	if ( HTCLIENT != nHitTest ) {

		return 0;

	}

	switch ( m_CurrentMode ) {

	case CBpaintDoc::MODE::COMPOSITE:
		if ( 
			(MK_CONTROL & m_CompositeModeInfo.m_LastMouseFlags)
			|| (MK_RBUTTON & m_CompositeModeInfo.m_LastMouseFlags) // BPT 10/10/02
			) {

			return m_hMoveCelCursor;

		} else {

			return 0; //m_hDefaultCursor;

		}
		break;

	case CBpaintDoc::MODE::DRAWING: {

		// Check to see if we're off the drawing canvas
		// --------------------------------------------------------------------

		bool bOffCanvas = ((0 > point.x) || (0 > point.y) ||
			(point.x >= m_CanvasSize.cx) || (point.y >= m_CanvasSize.cy));

		if ( m_hOffCanvasDrawingCursor && bOffCanvas ) {

			return m_hOffCanvasDrawingCursor;

		}

		// Pick the cursor for the active tool
		// --------------------------------------------------------------------

		int id = m_EditorLogic.GetCurrentToolID();

		if ( id ) switch( id ) {
		
			default:
				return m_hDefaultCursor;
				break;
		
			case ID_DTOOL_FLOOD_FILL:
				return m_hFloodCursor;
				break;
		
			case ID_DTOOL_FREEHAND_SINGLE:
				return m_hFreehandSingleCursor;
				break;

			case ID_DTOOL_EYE_DROPPER:
				return m_hEyeDropperCursor;
				break;
		
			}
		}
		break;

	}


	return NULL;
}

// ============================================================================

void CBpaintDoc::OnHackLoadBackground() 
{
}

void CBpaintDoc::OnHackLoadPalette() 
{

#if 1  // BPT 6/15/01

	// Deal with the multiple potential palettes issue
	// ------------------------------------------------------------------------

	if ( !ConfirmMultiplePaletteBehavior( 0, true, false ) ) {

		return;
		
	}

#endif

#if 1 // HACK

	HWND hwndMain = AfxGetMainWnd()->GetSafeHwnd();

	CFileDialog opf( 
		TRUE, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_LONGNAMES, 
		_T("Bitmap Files (*.bmp)|*.bmp||"),
		AfxGetMainWnd()
	);

	if ( IDOK != opf.DoModal() ) {

		return;

	}

	BPT::C8bppPicture * pPictureHack = BPT::Load8BppPicture( 
		opf.GetFileName( )
	);

	if ( pPictureHack ) {

		BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

		int nTotalColors = min( BPT::C8bppPicture::PALETTE_ENTRIES, pPal->TotalColors() );

		for ( int i = 0; i < nTotalColors; i++ ) {
		
			m_Edit2CompositeClut[ i ] = MakeDisplayPixelType(
				pPictureHack->m_PaletteData[ i ].rgbRed
				,pPictureHack->m_PaletteData[ i ].rgbGreen
				,pPictureHack->m_PaletteData[ i ].rgbBlue
			);
		
			if ( pPal ) {
			
				pPal->SetSlot(
					i
					,pPictureHack->m_PaletteData[ i ].rgbRed
					,pPictureHack->m_PaletteData[ i ].rgbGreen
					,pPictureHack->m_PaletteData[ i ].rgbBlue
				);
			
			}

		}

		SetModifiedFlag();

		// need to ask the editor to init itself with this picture...

		delete pPictureHack;

		// Make sure that the palettes are managed (catch all implmentation...)
		// --------------------------------------------------------------------

		m_pShowcase->ManageSharedPaletteEntries(); // 6/17/01
	
		// force a repaint hack!
		// --------------------------------------------------------------------
	
		ForceFullRedraw();

		UpdateAllViews(
			NULL, CColorChange::WHOLE_PALETTE_CHANGE,
			(CObject *)&CColorChange( 0 )
		);

	} else {

		MessageBox(
			hwndMain, "BMP loader didn't like that file (must be 256 colors)",
			"Oops!", MB_OK | MB_ICONWARNING | MB_TASKMODAL
		);

	}

#endif
	
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::LoadCanvasFromPicture()
//

void CBpaintDoc::LoadCanvasFromPicture( BPT::C8bppPicture * pPictureHack )
{
	if ( pPictureHack ) {

		HWND hwndMain = AfxGetMainWnd()->GetSafeHwnd();

#if 1  // BPT 6/15/01

		// Check to see if the palette is different so the user can make a choice
		// ------------------------------------------------------------------------

		bool bAcceptPalette = false;

		BPT::CAnnotatedPalette * pCurrentPal = GetCurrentPalette();

		if ( pCurrentPal ) {

			int nTotalColors = min( BPT::C8bppPicture::PALETTE_ENTRIES, pCurrentPal->TotalColors() );

			for ( int i = 0; i < nTotalColors; i++ ) {

				if ( 
					(pCurrentPal->GetSlot_R(i) != pPictureHack->m_PaletteData[ i ].rgbRed) ||
					(pCurrentPal->GetSlot_G(i) != pPictureHack->m_PaletteData[ i ].rgbGreen) ||
					(pCurrentPal->GetSlot_B(i) != pPictureHack->m_PaletteData[ i ].rgbBlue) ) {

					bAcceptPalette = true;

					break;

				}
	
			}

		} else {

			bAcceptPalette = true;

		}

		// ------------------------------------------------------------------------

		if ( bAcceptPalette ) {
	
			// Deal with the multiple potential palettes issue
			// --------------------------------------------------------------------

			if ( m_pAnimation->MultiplePaletteSupport() ) {

				if ( !ConfirmMultiplePaletteBehavior( "Use palette from file?", true, false ) ) {
	
					delete pPictureHack;
			
					return;
					
				}

			} else {

				bAcceptPalette = ( IDYES == MessageBox(
					hwndMain, "Use palette from this file?", "Yo!", MB_YESNO
				) );

			}

			// Okay now copy over the palette
			// --------------------------------------------------------------------

			if ( bAcceptPalette ) {

				BPT::CAnnotatedPalette * pPal = GetCurrentPalette();
	
				if ( pPal ) {

#if 1 // 10/30/02 BPT

					bool bMergePalettes = false;

					if ( pPal ) {
					
						bMergePalettes = ( IDYES == MessageBox(
							hwndMain, "Chromakey merge palette slots?", "Yo!", MB_YESNO
						) );

					}

#endif
	
					int nTotalColors = min( BPT::C8bppPicture::PALETTE_ENTRIES, pPal->TotalColors() );
	
					for ( int i = 0; i < nTotalColors; i++ ) {

#if 1 // 10/30/02 BPT

						if ( bMergePalettes ) {

							if (
								(0 == i) ||
								((pPal->GetSlot_R(i) != pPal->GetSlot_R(0)) ||
								(pPal->GetSlot_G(i) != pPal->GetSlot_G(0)) ||
								(pPal->GetSlot_B(i) != pPal->GetSlot_B(0))) ) {

								continue;

							}

						}
#endif
				
						m_Edit2CompositeClut[ i ] = MakeDisplayPixelType(
							pPictureHack->m_PaletteData[ i ].rgbRed
							,pPictureHack->m_PaletteData[ i ].rgbGreen
							,pPictureHack->m_PaletteData[ i ].rgbBlue
						);
				
						pPal->SetSlot(
							i
							,pPictureHack->m_PaletteData[ i ].rgbRed
							,pPictureHack->m_PaletteData[ i ].rgbGreen
							,pPictureHack->m_PaletteData[ i ].rgbBlue
						);
		
					}
	
				}

			}

		}

#else

		if ( IDYES == MessageBox( hwndMain, "Use palette from this file?", "Yo!", MB_YESNO ) ) {

			BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

			int nTotalColors = min( BPT::C8bppPicture::PALETTE_ENTRIES, pPal->TotalColors() );

			for ( int i = 0; i < nTotalColors; i++ ) {
		
				m_Edit2CompositeClut[ i ] = MakeDisplayPixelType(
					pPictureHack->m_PaletteData[ i ].rgbRed
					,pPictureHack->m_PaletteData[ i ].rgbGreen
					,pPictureHack->m_PaletteData[ i ].rgbBlue
				);
		
				if ( pPal ) {
			
					pPal->SetSlot(
						i
						,pPictureHack->m_PaletteData[ i ].rgbRed
						,pPictureHack->m_PaletteData[ i ].rgbGreen
						,pPictureHack->m_PaletteData[ i ].rgbBlue
					);
			
				}

			}

		}

#endif

		// need to ask the editor to init itself with this picture...

#if 1 // BPT 10/14/02

		if ( (pPictureHack->Width() > m_CanvasSize.cx) ||
			(pPictureHack->Height() > m_CanvasSize.cy) ) {

			if ( AfxMessageBox( "Picture larger than canvas increase canvas size?\n", MB_YESNO ) ) {

				CSize size;

				size.cx = max( pPictureHack->Width(), m_CanvasSize.cx );

				size.cy = max( pPictureHack->Height(), m_CanvasSize.cy );

				ChangeCanvasSizeInternals( size );

				ForceFullRedraw();

				// Redo the UI layout to rebuild the various canvas related elements.
				// ------------------------------------------------------------------------

				AfxGetMainWnd()->PostMessage( WM_COMMAND, GLOBAL_GetSettingInt( "LastUILayout", ID_UI_LAYOUT_6) );

			}

		}
#endif

		SetDocumentMode( CBpaintDoc::MODE::DRAWING );

		// need to ask the editor to init itself with this picture...

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) || defined(BPAINT_CHUNKY_32BPP_VERSION) // attempt to test 32 bpp storage

		MessageBox( hwndMain, "CBpaintDoc::LoadCanvasFromPicture", "32bpp fix needed!", MB_OK );

		m_EditorLogic.BeginSession( 0, GetClearColor() );

#else

		m_EditorLogic.BeginSession( pPictureHack, GetClearColor() );

#endif

		SetModifiedFlag();

		// Make sure that the palettes are managed (catch all implmentation...)
		// --------------------------------------------------------------------

		m_pShowcase->ManageSharedPaletteEntries(); // 6/17/01
	
		// force a repaint hack!
		// --------------------------------------------------------------------
	
		ForceFullRedraw();

		UpdateAllViews(
			NULL, CColorChange::WHOLE_PALETTE_CHANGE,
			(CObject *)&CColorChange( 0 )
		);

		// FUN!

	}

}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnHackLoadPicture() 
{

#if 1 // HACK

	HWND hwndMain = AfxGetMainWnd()->GetSafeHwnd();

	CFileDialog opf( 
		TRUE, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_LONGNAMES, 
		_T("Bitmap Files (*.bmp)|*.bmp||"),
		AfxGetMainWnd()
	);

	if ( IDOK != opf.DoModal() ) {

		return;

	}

	BPT::C8bppPicture * pPictureHack = BPT::Load8BppPicture( 
		opf.GetFileName( )
	);

	if ( !pPictureHack ) {

		MessageBox(
			hwndMain, "Loader didn't like that file (must be 256 colors)",
			"Oops!", MB_OK | MB_ICONWARNING | MB_TASKMODAL
		);

		return;

	}

	LoadCanvasFromPicture( pPictureHack );

	delete pPictureHack;

#endif
	
}

void CBpaintDoc::OnHackSavePicture() 
{
}

void CBpaintDoc::OnHackLoadForeground() 
{
}

// ============================================================================

//
//	CBpaintDoc::OnEditStencil()
//

void CBpaintDoc::OnEditStencil() 
{
	// --------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	// --------------------------------------------------------------------

	CMakeStencilDlg stencilDlg( this, AfxGetMainWnd() );

	// Setup the stencil window hack pointer
	// -----------------------------------------------------------------------

	m_pPickupColorHackDlg = &stencilDlg;
	m_nPickupColorHackMsg = 0;

	// fill in the dialog's stencil info
	// -----------------------------------------------------------------------

	for ( int i = 0; i < 256; i++ ) {

		stencilDlg.SelectSlot( i, m_bColorStencilTable[ i ], false );

	}

	// -----------------------------------------------------------------------

	if ( IDOK == stencilDlg.DoModal() ) {

		bool bSomething = false;

		for ( int i = 0; i < 256; i++ ) {
	
			if ( m_bColorStencilTable[ i ] = stencilDlg.IsSlotSelected( i ) ) {

				bSomething = true;

			}
	
		}

		// --------------------------------------------------------------------

		m_bColorStencilDefined = bSomething;
		m_bColorStencilEnabled = bSomething;

		// --------------------------------------------------------------------

#if 1 // hack

		m_EditorLogic.SetStencilMode( 
			m_bColorStencilEnabled, m_bColorStencilTable 
		);

		RefreshPen();

#endif

	}

	// Shutdown the stencil window hack 
	// -----------------------------------------------------------------------

	m_pPickupColorHackDlg = 0;
	m_nPickupColorHackMsg = 0;

}

//
//	CBpaintDoc::OnStencilOn() 
//

void CBpaintDoc::OnStencilOn() 
{

	if ( !m_bColorStencilDefined ) {

		OnEditStencil();

		return;

	}

	if ( m_bColorStencilEnabled ) {

		m_bColorStencilEnabled = false;

	} else {

		for ( int i = 0; i < 256; i++ ) {

			if ( m_bColorStencilTable[ i ] ) {

				break;

			}

		}

		if ( i >= 256 ) {

			OnEditStencil();

			return;

		}

		m_bColorStencilEnabled = true;

	}

	m_EditorLogic.SetStencilMode( m_bColorStencilEnabled, 0, false );

	RefreshPen();

}

void CBpaintDoc::OnUpdateStencilOn(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bColorStencilEnabled ? 1 : 0 );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnAnimSelectDialog() 
{

	// Validate showcase state for the operation
	// ------------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	CSelectAnimationDlg dlg( AfxGetMainWnd() );

	dlg.m_pDoc = this;
	dlg.m_pShowcase = m_pShowcase;
	dlg.m_pAnimation = m_pAnimation;

	if ( IDOK == dlg.DoModal() ) {

		// Anything?

	}
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnAnimDeleteFrames() 
{
	if ( !m_pAnimation ) {

		return;

	}

	CDeleteFrameRangeDlg dlg( AfxGetMainWnd() );

	dlg.m_nFromMin = 1;
	dlg.m_nFromMax = m_pAnimation->FrameCount();
	dlg.m_nToMin = 1;
	dlg.m_nToMax = m_pAnimation->FrameCount();

	dlg.m_nFrom = m_pAnimation->CurrentFrame() + 1;
	dlg.m_nTo = m_pAnimation->CurrentFrame() + 1;

	if ( IDOK == dlg.DoModal() ) {

		// Take care of the 'mode' issues
		// --------------------------------------------------------------------

		CBpaintDoc::MODE oldMode = m_CurrentMode;

		if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

			return;

		}

		// If the user has selected frames that have code warn them
		// --------------------------------------------------------------------

		bool bAlreadyAskedAndAnswered = false;

#if 1 
		{

			int count = (dlg.m_nTo - dlg.m_nFrom) + 1;
	
			int frame = dlg.m_nFrom - 1;

			bool bHasNotes = false;
	
			for ( int i = 0; i < count; i++, frame++ ) {

				BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( frame );

				if ( pFrame ) {

					if ( pFrame->HasNote() ) {

						bHasNotes = true;

						break;

					}

				}

			}

			if ( bHasNotes ) {

				CString warningMsg;

				warningMsg.Format(
					"A frame in the range you selected contains a note "
					"this are you sure you want to delete this range of "
					"frames [%d to %d]? (Frame %d was where the note was found)",
					dlg.m_nFrom, dlg.m_nTo, dlg.m_nFrom + i
				);

				if ( IDYES != MessageBox( AfxGetMainWnd()->GetSafeHwnd(),
					warningMsg, "Are you sure???", MB_ICONQUESTION | MB_YESNO ) ) {
		
					return;
		
				}

				bAlreadyAskedAndAnswered = true;

			}

		}
#endif
		// --------------------------------------------------------------------

		if ( false == bAlreadyAskedAndAnswered ) {

			CString msg;
	
			msg.Format( "Delete Frames [%d to %d]? (This can't be undone!)", dlg.m_nFrom, dlg.m_nTo );
	
			if ( IDYES != MessageBox( AfxGetMainWnd()->GetSafeHwnd(),
				msg, "Are you sure?", MB_ICONQUESTION | MB_YESNO ) ) {
	
				return;
	
			}

		}
	
		// --------------------------------------------------------------------

		int count = (dlg.m_nTo - dlg.m_nFrom) + 1;

		int frame = dlg.m_nFrom - 1;

		for ( int i = 0; i < count; i++ ) {

			if ( !m_pAnimation->DeleteFrame( frame ) ) {
	
				MessageBox( 
					AfxGetMainWnd()->GetSafeHwnd(), 
					"Unable to delete frame", "ERROR!", 
					MB_ICONERROR | MB_OK
				);

				break;

			}

		}

		// Need to tell the views of the change.
		// --------------------------------------------------------------------
	
		CActiveAnimationChange changeInfo;
		
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );
	
		// --------------------------------------------------------------------

		UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );
	
		PENDING_SetDocumentMode( oldMode );

		SetModifiedFlag();

	}
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnAnimDeleteSingleLayer() 
{
	if ( IDYES == MessageBox(
		AfxGetMainWnd()->GetSafeHwnd(), "Delete layer (operation can't be undone!)", "Delete layer?", MB_YESNO ) ) {

		// Take care of the 'mode' issues
		// --------------------------------------------------------------------

		CBpaintDoc::MODE oldMode = m_CurrentMode;

		if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

			return;

		}

		// --------------------------------------------------------------------

		m_pAnimation->DeleteLayer( m_pAnimation->CurrentLayer() );

		// Need to tell the views of the change.
		// --------------------------------------------------------------------
	
		CActiveAnimationChange changeInfo;
		
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );
	
		// --------------------------------------------------------------------

		UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );
	
		PENDING_SetDocumentMode( oldMode );

		SetModifiedFlag();

	}
}

void CBpaintDoc::OnUpdateAnimDeleteSingleLayer(CCmdUI* pCmdUI) 
{
	if ( m_pAnimation ) {

		pCmdUI->Enable( (1 < m_pAnimation->LayerCount()) ? TRUE : FALSE );

	} else {

		pCmdUI->Enable( FALSE );

	}

}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnAnimInsertFrames() 
{
	if ( !m_pAnimation ) {

		return;

	}

	int insertCount = 1;

	DWORD outFlags = 0;

	if ( InsertXDialog( AfxGetMainWnd(), "Insert Frames",
		insertCount, outFlags, 1, 16384,
		"InsertFramesDlg", true ) ) {

		// Take care of the 'mode' issues
		// --------------------------------------------------------------------

		CBpaintDoc::MODE oldMode = m_CurrentMode;

		if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

			return;

		}

		// What is the insert frame?
		// --------------------------------------------------------------------

		int nCurrentFrame = m_pAnimation->CurrentFrame();

		int nInsertFrame = nCurrentFrame;

		BPT::CFrame * pOriginalFrame = m_pAnimation->GetFramePtr( nCurrentFrame );

		if ( CInsertXDlg::INSERT_AFTER & outFlags ) {

			++nInsertFrame;

		}

		if ( !m_pAnimation->InsertNewFramesBefore( insertCount, nInsertFrame) ) {

			MessageBox( 
				AfxGetMainWnd()->GetSafeHwnd(), 
				"Unable to insert frames", "ERROR!", 
				MB_ICONERROR | MB_OK
			);

			return;

		}

		// --------------------------------------------------------------------

		bool bError = false;

		if ( CInsertXDlg::DUPLICATE_CURRENT & outFlags ) {

			int layerCount = m_pAnimation->LayerCount();

			int frameIndex = nInsertFrame;

			for ( int counter = 0; counter < insertCount; counter++, frameIndex++ ) {

				BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( frameIndex );

				for ( int layer = 0; layer < layerCount; layer++ ) {

					// Get the source & dest layer pointers

					BPT::CLayer * pSrcLayer = pOriginalFrame->GetLayerPtr( layer );

					BPT::CLayer * pDstLayer = pFrame->GetLayerPtr( layer );

					// Ask the dst layer to copy the src layers info...

					if ( !pDstLayer->CopyFrom( pSrcLayer ) ) {

						bError = true;

					}

				}

			}

		}

		if ( bError ) {

			MessageBox( 
				AfxGetMainWnd()->GetSafeHwnd(), 
				"Error during layer information transfer", "ERROR!", 
				MB_ICONERROR | MB_OK
			);

		}

		// Need to tell the views of the change.
		// --------------------------------------------------------------------
	
		CActiveAnimationChange changeInfo;
		
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );
	
		// --------------------------------------------------------------------

		UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );
	
		PENDING_SetDocumentMode( oldMode );

		SetModifiedFlag();

	}
}

void CBpaintDoc::OnAnimInsertLayers() 
{
	if ( !m_pAnimation ) {

		return;

	}

	int insertCount = 1;

	DWORD outFlags = 0;

	if ( InsertXDialog( AfxGetMainWnd(), "Insert Layers",
		insertCount, outFlags, 1, 16384,
		"InsertLayersDlg", true ) ) {

		// Take care of the 'mode' issues
		// --------------------------------------------------------------------

		CBpaintDoc::MODE oldMode = m_CurrentMode;

		if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

			return;

		}

		// What is the insert frame?
		// --------------------------------------------------------------------

		int nCurrentLayer = m_pAnimation->CurrentLayer();

		int copyFromLayer = nCurrentLayer;

		int nInsertLayer = nCurrentLayer;

		if ( CInsertXDlg::INSERT_AFTER & outFlags ) {

			++nInsertLayer;

		} else {

			copyFromLayer += insertCount;

		}

		if ( !m_pAnimation->InsertNewLayers( insertCount, nInsertLayer) ) {

			MessageBox( 
				AfxGetMainWnd()->GetSafeHwnd(), 
				"Unable to insert layers", "ERROR!", 
				MB_ICONERROR | MB_OK
			);

			return;

		}

		// --------------------------------------------------------------------

		bool bError = false;

		if ( CInsertXDlg::DUPLICATE_CURRENT & outFlags ) {

			int frameCount = m_pAnimation->FrameCount();

			for ( int frame = 0; frame < frameCount; frame++ ) {

				// Get the source & dest frame pointer

				BPT::CFrame * pSrcFrame = m_pAnimation->GetFramePtr( frame );

				BPT::CLayer * pSrcLayer = pSrcFrame->GetLayerPtr( copyFromLayer );

				int layerIndex = nInsertLayer;

				// perform the operation.

				for ( int counter = 0; counter < insertCount; counter++, layerIndex++ ) {

					BPT::CLayer * pDstLayer = pSrcFrame->GetLayerPtr( layerIndex );

					// Ask the dst layer to copy the src layers info...

					if ( !pDstLayer->CopyFrom( pSrcLayer ) ) {

						bError = true;

					}

				}

			}

		}

		if ( bError ) {

			MessageBox( 
				AfxGetMainWnd()->GetSafeHwnd(), 
				"Error during layer information transfer", "ERROR!", 
				MB_ICONERROR | MB_OK
			);

		}

		// Need to tell the views of the change.
		// --------------------------------------------------------------------
	
		CActiveAnimationChange changeInfo;
		
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );
	
		// --------------------------------------------------------------------

		UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );
	
		PENDING_SetDocumentMode( oldMode );

		SetModifiedFlag();

	}
}

// ============================================================================

void CBpaintDoc::OnLayerCopyFrameToNext() 
{
	if ( m_pAnimation ) {

		// Take care of the 'mode' issues
		// --------------------------------------------------------------------

		CBpaintDoc::MODE oldMode = m_CurrentMode;

		if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

			return;

		}

		// Perform the operation
		// --------------------------------------------------------------------

		int nCurrentFrame = m_pAnimation->CurrentFrame();

		int nCurrentLayer = m_pAnimation->CurrentLayer();

		BPT::CFrame * pSrcFrame = m_pAnimation->GetFramePtr( nCurrentFrame );

		int nDstFrame = m_pAnimation->CurrentFrame() + 1;
	
		if ( m_pAnimation->FrameCount() <= nDstFrame ) {
	
			nDstFrame = 0;
	
		}
	
		BPT::CFrame * pDstFrame = m_pAnimation->GetFramePtr( nDstFrame );

		if ( pSrcFrame && pDstFrame ) {

			BPT::CLayer * pDstLayer = pDstFrame->GetLayerPtr( nCurrentLayer );

			BPT::CLayer * pSrcLayer = pSrcFrame->GetLayerPtr( nCurrentLayer );

			if ( pSrcLayer && pDstLayer ) {

				pDstLayer->CopyFrom( pSrcLayer );

			}

		}

		// Need to tell the views of the change.
		// --------------------------------------------------------------------
	
		CActiveAnimationChange changeInfo;
		
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );
	
		// --------------------------------------------------------------------

		UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );
	
		PENDING_SetDocumentMode( oldMode );

		SetModifiedFlag();

		// Update the screen
		// --------------------------------------------------------------------

		Request_NextFrame();

	}
}

void CBpaintDoc::OnUpdateLayerCopyFrameToNext(CCmdUI* pCmdUI) 
{
	if ( m_pAnimation ) {

		if ( 1 < m_pAnimation->FrameCount() ) {

			pCmdUI->Enable( TRUE );

			return;

		}

	}

	pCmdUI->Enable( FALSE );
}

// ============================================================================

void CBpaintDoc::OnFrameTransformations() 
{
	// TODO: Add your command handler code here
	
	CSimpleTransformationsDlg dlg( AfxGetMainWnd() );

	if ( IDOK == dlg.DoModal() ) {

		SetModifiedFlag();

	}
}

void CBpaintDoc::OnFrameClear() 
{
	OnLayerClear();
}

void CBpaintDoc::OnLayerClear() 
{
	// Validate showcase state for the operation
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// Fill in the dialog information members
	// ------------------------------------------------------------------------

	if ( ClearLayersDialogHandler( AfxGetMainWnd() ) ) {

		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

		ForceFullRedraw();

	}

}

void CBpaintDoc::OnChangedViewList() 
{

	POSITION pos = GetFirstViewPosition();

	while (pos != NULL) {

		CView * pView = GetNextView( pos );

#if 0

		// Do some internal processing?

		if ( pHint->IsKindOf( RUNTIME_CLASS( CEditorRectChange ) ) ) {

		}
#endif

	}
	
	CDocument::OnChangedViewList();
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// LAYOUT handler stuff
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//
//	TDocActiveFramesAndViewsInfo<>
//

template< class DOC >
class TDocActiveFramesAndViewsInfo
{

public: // traits

	typedef std::list< CView * > view_collection;
	typedef std::map< CFrameWnd *, view_collection * > output_map_type;

public: // interface

	static void Destroy( output_map_type * pMap ) {

		if ( pMap ) {

			for ( output_map_type::iterator it = pMap->begin(); it != pMap->end(); ) {

				delete (*it).second;

				it = pMap->erase( it );

			}

			delete pMap;

		}

	}

	static output_map_type * Generate( DOC * pDocument ) {

		// Create the map to hold the info
		// --------------------------------------------------------------------

		output_map_type * pMap = new output_map_type;

		if ( !pMap ) {

			return 0;

		}

		// --------------------------------------------------------------------

		POSITION pos = pDocument->GetFirstViewPosition();

		while (pos != NULL) {

			CView * pView = pDocument->GetNextView( pos );

			if ( pView ) {

				CFrameWnd * pFrame = pView->GetParentFrame();

				if ( pFrame ) {

					output_map_type::iterator it = pMap->find( pFrame );

					if ( it != pMap->end() ) {

						(*it).second->push_back( pView );

					} else {

						if ( (*pMap)[ pFrame ] = new view_collection() ) {

							(*pMap)[ pFrame ]->push_back( pView );

						}

					}

				}

			}

		}

		// --------------------------------------------------------------------

		return pMap;

	}

};

// ----------------------------------------------------------------------------

//
//	UILayout_1()
//

bool CBpaintDoc::UILayout_1( CBpaintApp * pApp )
{

	// ------------------------------------------------------------------------

	CFrameWnd * pEdit1 = 0;
	CFrameWnd * pEdit2 = 0;
	CFrameWnd * pGrid1 = 0;
	CFrameWnd * pPalWnd = 0;

	// Create the edit views
	// ------------------------------------------------------------------------

	CRect rMDIClientRect;

	pEdit1 = GLOBAL_CreateNewWindow(
		pApp->GetDocTemplate(), this
	);

	int width = 0;

	int height = 0;
	
	if ( pEdit1 ) {
	
		// Figure out the first pos...
	
		CWnd * pWnd = pEdit1->GetParent();
	
		if ( pWnd ) {
	
			pWnd->GetClientRect( &rMDIClientRect );
	
		}

		width = rMDIClientRect.Width() / 2;
	
		// position
	
		pEdit1->MoveWindow(
			CRect( CPoint( 0, 0 ), 
				CSize( width, rMDIClientRect.Height() )
			)
		);
	
		// need to send the zoom request.
	
		pEdit1->SendMessage( WM_COMMAND, ID_VZOOM_3X );
	
	} else {

		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	pEdit2 = GLOBAL_CreateNewWindow(
		pApp->GetDocTemplate(), this
	);
	
	height = rMDIClientRect.Height() / 2;

	if ( pEdit2 ) {
	
		// position
	
		pEdit2->MoveWindow(
			CRect( CPoint( width, 0 ), 
				CSize( rMDIClientRect.Width() - width, height )
			)
		);
	
	} else {
	
		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the grid view
	// ----------------------------------------------------------------
	
	pGrid1 = GLOBAL_CreateNewWindow(
		pApp->GetMDIChildGridDocTemplate(), this
	);
	
	if ( pGrid1 ) {
	
		// position
	
		pGrid1->MoveWindow(
			CRect( CPoint( width, height ), 
				CSize( rMDIClientRect.Width() - width, rMDIClientRect.Height() - height )
			)
		);
	
	} else {
	
		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the palette view
	// ----------------------------------------------------------------

	OnNewSimplePaletteView();

#if 1

	pEdit1->PostMessage( WM_COMMAND, ID_CENTER_VIEW );

#endif
	
	return true;

HANDLE_ERROR:

	// ????

	return false;

}

//
//	UILayout_2()
//

bool CBpaintDoc::UILayout_2( CBpaintApp * pApp )
{

	// ------------------------------------------------------------------------

	CFrameWnd * pEdit1 = 0;
	CFrameWnd * pEdit2 = 0;
	CFrameWnd * pGrid1 = 0;

	// Create the edit views
	// ------------------------------------------------------------------------

	CRect rMDIClientRect;

	pEdit1 = GLOBAL_CreateNewWindow(
		pApp->GetDocTemplate(), this
	);

	int width = 0;

	int height = 0;
	
	if ( pEdit1 ) {
	
		// Figure out the first pos...
	
		CWnd * pWnd = pEdit1->GetParent();
	
		if ( pWnd ) {
	
			pWnd->GetClientRect( &rMDIClientRect );
	
		}

		width = rMDIClientRect.Width() / 2;
		height = (rMDIClientRect.Height() * 3) / 4;
	
		// position
	
		pEdit1->MoveWindow(
			CRect( CPoint( 0, 0 ), 
				CSize( width, height )
			)
		);
	
		// need to send the zoom request.
	
		pEdit1->SendMessage( WM_COMMAND, ID_VZOOM_3X );
	
	} else {

		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	pEdit2 = GLOBAL_CreateNewWindow(
		pApp->GetDocTemplate(), this
	);
	
	if ( pEdit2 ) {
	
		// position
	
		pEdit2->MoveWindow(
			CRect( CPoint( width, 0 ), 
				CSize( rMDIClientRect.Width() - width, height )
			)
		);
	
	} else {
	
		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the grid view
	// ----------------------------------------------------------------
	
	pGrid1 = GLOBAL_CreateNewWindow(
		pApp->GetMDIChildGridDocTemplate(), this
	);
	
	if ( pGrid1 ) {
	
		// position
	
		pGrid1->MoveWindow(
			CRect( CPoint( 0, height ), 
				CSize( rMDIClientRect.Width(), rMDIClientRect.Height() - height )
			)
		);
	
	} else {
	
		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the palette view
	// ----------------------------------------------------------------

	OnNewSimplePaletteView();
	
#if 1

	pEdit1->PostMessage( WM_COMMAND, ID_CENTER_VIEW );

#endif
	return true;

HANDLE_ERROR:

	// ????

	return false;

}

//
//	UILayout_3()
//

bool CBpaintDoc::UILayout_3( CBpaintApp * pApp )
{
	// ------------------------------------------------------------------------

	CFrameWnd * pEdit1 = 0;
	CFrameWnd * pGrid1 = 0;
	CFrameWnd * pPalWnd = 0;

	// Create the edit views
	// ------------------------------------------------------------------------

	CRect rMDIClientRect;

	pEdit1 = GLOBAL_CreateNewWindow(
		pApp->GetDocTemplate(), this
	);

	int height = 0;
	
	if ( pEdit1 ) {
	
		// Figure out the first pos...
	
		CWnd * pWnd = pEdit1->GetParent();
	
		if ( pWnd ) {
	
			pWnd->GetClientRect( &rMDIClientRect );
	
		}

		height = (rMDIClientRect.Height() * 3) / 4;
	
		// position
	
		pEdit1->MoveWindow(
			CRect( CPoint( 0, 0 ), 
				CSize( rMDIClientRect.Width(), height )
			)
		);
	
		// need to send the zoom request.
	
		// pEdit1->SendMessage( WM_COMMAND, ID_VZOOM_3X );
	
	} else {

		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the grid view
	// ----------------------------------------------------------------
	
	pGrid1 = GLOBAL_CreateNewWindow(
		pApp->GetMDIChildGridDocTemplate(), this
	);
	
	if ( pGrid1 ) {
	
		// position
	
		pGrid1->MoveWindow(
			CRect(
				CPoint( 0, height ), 
				CSize( rMDIClientRect.Width(), rMDIClientRect.Height() - height )
			)
		);
	
	} else {
	
		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the palette view
	// ----------------------------------------------------------------
	
	OnNewSimplePaletteView();

#if 1

	pEdit1->PostMessage( WM_COMMAND, ID_CENTER_VIEW );

#endif
	return true;

HANDLE_ERROR:

	// ????

	return false;

}

//
//	UILayout_4()
//

bool CBpaintDoc::UILayout_4( CBpaintApp * pApp )
{
	// ------------------------------------------------------------------------

	CFrameWnd * pEdit1 = 0;
	CFrameWnd * pGrid1 = 0;
	CFrameWnd * pPalWnd = 0;

	// Create the edit views
	// ------------------------------------------------------------------------

	CRect rMDIClientRect;

	pEdit1 = GLOBAL_CreateNewWindow(
		pApp->GetDocTemplate(), this
	);

	int width = 0;
	
	if ( pEdit1 ) {
	
		// Figure out the first pos...
	
		CWnd * pWnd = pEdit1->GetParent();
	
		if ( pWnd ) {
	
			pWnd->GetClientRect( &rMDIClientRect );
	
		}

		width = rMDIClientRect.Width() / 2;
	
		// position
	
		pEdit1->MoveWindow(
			CRect( CPoint( 0, 0 ), 
				CSize( width, rMDIClientRect.Height() )
			)
		);
	
		// need to send the zoom request.
	
		pEdit1->SendMessage( WM_COMMAND, ID_VZOOM_3X );
	
	} else {

		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the grid view
	// ----------------------------------------------------------------
	
	pGrid1 = GLOBAL_CreateNewWindow(
		pApp->GetMDIChildGridDocTemplate(), this
	);
	
	if ( pGrid1 ) {
	
		// position
	
		pGrid1->MoveWindow(
			CRect(
				CPoint( width, 0 ), 
				CSize( rMDIClientRect.Width() - width, rMDIClientRect.Height() )
			)
		);
	
	} else {
	
		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the palette view
	// ----------------------------------------------------------------
	
	OnNewSimplePaletteView();

#if 1

	pEdit1->PostMessage( WM_COMMAND, ID_CENTER_VIEW );

#endif
	return true;

HANDLE_ERROR:

	// ????

	return false;

}

//
//	UILayout_5()
//

bool CBpaintDoc::UILayout_5( CBpaintApp * pApp )
{
	// ------------------------------------------------------------------------

	CFrameWnd * pEdit1 = 0;
	CFrameWnd * pGrid1 = 0;
	CFrameWnd * pPalWnd = 0;

	// Create the edit views
	// ------------------------------------------------------------------------

	CRect rMDIClientRect;

	pEdit1 = GLOBAL_CreateNewWindow(
		pApp->GetDocTemplate(), this
	);

	int height = 0;
	
	if ( pEdit1 ) {
	
		// Figure out the first pos...
	
		CWnd * pWnd = pEdit1->GetParent();
	
		if ( pWnd ) {
	
			pWnd->GetClientRect( &rMDIClientRect );
	
		}

		height = rMDIClientRect.Height() / 2;
	
		// position
	
		pEdit1->MoveWindow(
			CRect(
				CPoint( 0, 0 ), 
				CSize( rMDIClientRect.Width(), height )
			)
		);
	
		// need to send the zoom request.
	
		pEdit1->SendMessage( WM_COMMAND, ID_VZOOM_3X );
	
	} else {

		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the grid view
	// ----------------------------------------------------------------
	
	pGrid1 = GLOBAL_CreateNewWindow(
		pApp->GetMDIChildGridDocTemplate(), this
	);
	
	if ( pGrid1 ) {
	
		// position
	
		pGrid1->MoveWindow(
			CRect(
				CPoint( 0, height ), 
				CSize( rMDIClientRect.Width(), rMDIClientRect.Height() - height )
			)
		);
	
	} else {
	
		// NEED TO HANDLE THE ERROR!!!
	
		goto HANDLE_ERROR;

	}
	
	// Create the palette view
	// ----------------------------------------------------------------
	
	OnNewSimplePaletteView();

#if 1

	pEdit1->PostMessage( WM_COMMAND, ID_CENTER_VIEW );

#endif
	return true;

HANDLE_ERROR:

	// ????

	return false;

}

//
//	CBpaintDoc::UILayout_6()
//

bool CBpaintDoc::UILayout_6( CBpaintApp * pApp )
{

	// ------------------------------------------------------------------------

	SetPendingMultiViewData( CSimpleSplitFrame::LAYOUT_1 );

	CFrameWnd * pFrameWnd = GLOBAL_CreateNewWindow( pApp->GetMDIChildMultiView1DocTemplate(), this );

	SetPendingMultiViewData( 0 );

	if ( !pFrameWnd ) {

		return false;

	}

	// Create the palette view
	// ----------------------------------------------------------------
	
	OnNewSimplePaletteView();

	pFrameWnd->PostMessage( WM_SYSCOMMAND, SC_MAXIMIZE );

	return true;

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::UILayout_7()
//

bool CBpaintDoc::UILayout_7( CBpaintApp * pApp )
{

	// ------------------------------------------------------------------------

	SetPendingMultiViewData( CSimpleSplitFrame::LAYOUT_2 );

	CFrameWnd * pFrameWnd = GLOBAL_CreateNewWindow( pApp->GetMDIChildMultiView2DocTemplate(), this );

	SetPendingMultiViewData( 0 );

	if ( !pFrameWnd ) {

		return false;

	}

	// Create the palette view
	// ----------------------------------------------------------------
	
	OnNewSimplePaletteView();

	pFrameWnd->PostMessage( WM_SYSCOMMAND, SC_MAXIMIZE );

	return true;

}

// ----------------------------------------------------------------------------

bool CBpaintDoc::UILayout_8( CBpaintApp * pApp ) { 

	if ( LoadCustomLayoutFrom( "custom.layout.b" ) ) {

		return true;

	}
	
	return UILayout_1( pApp ); 

}

bool CBpaintDoc::UILayout_9( CBpaintApp * pApp ) {

	if ( LoadCustomLayoutFrom( "custom.layout.c" ) ) {

		return true;

	}

	return UILayout_1( pApp );

}

bool CBpaintDoc::UILayout_0( CBpaintApp * pApp ) {
	
	if ( LoadCustomLayoutFrom( "custom.layout.a" ) ) {

		return true;

	}

	return UILayout_1( pApp );

}

// ----------------------------------------------------------------------------

void 
CBpaintDoc::LoadUILayout( const int layout, const bool destroyExistingUI )
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
	
	if ( pApp ) {

		// Make sure that the document doesn't get destroyed while
		// all of it's current views/frames are destroyed.
		// --------------------------------------------------------------------

		BOOL bSaveAutoDelete = m_bAutoDelete;
		m_bAutoDelete = FALSE;

		// Destroy the current frames/views
		// --------------------------------------------------------------------

		typedef TDocActiveFramesAndViewsInfo<CBpaintDoc> info_helper;

		info_helper::output_map_type * pMap = info_helper::Generate( this );

		if ( pMap ) {

			if ( destroyExistingUI ) {

				info_helper::output_map_type::iterator it = pMap->begin();
	
				while ( it != pMap->end() ) {
	
					(it->first)->DestroyWindow();
	
					++it;
	
				}

			}

		}

		// Create new frames/views
		// --------------------------------------------------------------------

		int validatedLayout = layout;

		switch ( layout ) {

		default:
		case ID_UI_LAYOUT_1:
			validatedLayout = ID_UI_LAYOUT_1;
			UILayout_1( pApp );
			break;

		case ID_UI_LAYOUT_2:
			UILayout_2( pApp );
			break;

		case ID_UI_LAYOUT_3:
			UILayout_3( pApp );
			break;

		case ID_UI_LAYOUT_4:
			UILayout_4( pApp );
			break;

		case ID_UI_LAYOUT_5:
			UILayout_5( pApp );
			break;

		case ID_UI_LAYOUT_6:
			UILayout_6( pApp );
			break;

		case ID_UI_LAYOUT_7:
			UILayout_7( pApp );
			break;

		case ID_UI_LAYOUT_8:
			UILayout_8( pApp );
			break;

		case ID_UI_LAYOUT_9:
			UILayout_9( pApp );
			break;

		case ID_UI_LAYOUT_0:
			UILayout_0( pApp );
			break;

		}

		// Store off the last selected UI
		// --------------------------------------------------------------------

		GLOBAL_PutSettingInt( "LastUILayout", validatedLayout );

		// ???
		// --------------------------------------------------------------------

//		UpdateAllViews( 0, 0, &CUILayoutId( layout ) );

		// Restore the auto delete setting and we're done!
		// --------------------------------------------------------------------

		m_bAutoDelete = bSaveAutoDelete;

		// Destroy out helper 'map'
		// --------------------------------------------------------------------

		if ( pMap ) {

			info_helper::Destroy( pMap );

		}

	}

}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnPaintTrimEdge() 
{
	m_EditorLogic.SetTrimEdgeMode( !m_EditorLogic.GetTrimEdgeMode() );
}

void CBpaintDoc::OnUpdatePaintTrimEdge(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_EditorLogic.GetTrimEdgeMode() ? 1 : 0 );
}

// ----------------------------------------------------------------------------

SIZE
CBpaintDoc::GetCanvasSize()
{
	return m_CanvasSize;
}

CSize 
CBpaintDoc::GetThumbnailSize()
{
	return m_ThumbnailSize;
}

CSize
CBpaintDoc::GetDrawingBrushSize()
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		return CSize( 
			m_pPickupBrushMediator->GetBitmapPtr()->Width(),
			m_pPickupBrushMediator->GetBitmapPtr()->Height()
		);

	} 

	return CSize( 0, 0 );
}

void CBpaintDoc::SetDrawingBrushSize( CSize & size )
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		m_pPickupBrushMediator->ForceSize( size, m_BrushChromaKey, m_bUseOriginalOnBrushSizeChange );

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::OkayToUseToolAfterPickupBrush()
//

bool CBpaintDoc::OkayToUseToolAfterPickupBrush( const int tool )
{
	return (
		(ID_DTOOL_LINE == tool) ||
		(ID_DTOOL_EMPTY_RECT == tool) ||
		(ID_DTOOL_EMPTY_ELLIPSE == tool) ||
		(ID_DTOOL_EMPTY_CIRCLE == tool) ||
		(ID_DTOOL_3PT_CURVE == tool) ||
		(ID_DTOOL_FREEHAND_LINE == tool) ||
		(ID_DTOOL_FREEHAND_SPOTTY == tool) ||
		(ID_DTOOL_FREEHAND_SINGLE == tool)
	);
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::BrushHasChanged()
//

void CBpaintDoc::BrushHasChanged( const bool bSetChromakey, const int * pOptionalNewChromakey )
{

	// Keep the right button color as the 'chromakey' for the new brush

	if ( bSetChromakey ) {

		m_BrushChromaKey = m_Color_R; //m_ChromaKey;

	}

	if ( pOptionalNewChromakey ) {

		m_BrushChromaKey = *pOptionalNewChromakey;

	}

	m_BitmapBrushRenderer.SetBitmapInfo( 
		m_pPickupBrushMediator->GetBitmapPtr(),
		&m_pPickupBrushMediator->GetDefaultPos(),
		m_BrushChromaKey
	);


#if 1

	if ( (ID_ROP_KEEP_SAME == m_nBrushPaintROP) || (ID_ROP_KEEP_DIFF == m_nBrushPaintROP) ) {

		m_nBrushPaintROP = ID_ROP_MATTE;

	}

	// ugly design

#if 0
	m_ReplaceBitmapBrushRenderer.SetBitmapInfo( 
		m_pPickupBrushMediator->GetBitmapPtr(),
		&m_pPickupBrushMediator->GetDefaultPos(),
		m_BrushChromaKey
	);
#endif

	m_DifferenceBitmapBrushRenderer.SetBitmapInfo( 
		m_pPickupBrushMediator->GetBitmapPtr(),
		&m_pPickupBrushMediator->GetDefaultPos(),
		m_BrushChromaKey
	);

#endif


	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );

	SetBitmapBrushMode( true );

// DESIGN!!!
// DESIGN!!!
// DESIGN!!!

#if 1

	int activeToolID = m_EditorLogic.GetCurrentToolID();

	if ( ID_DTOOL_PICKUP_BRUSH == activeToolID ) {

		if ( m_PreviousDrawingTool ) {

			// This really should go to the last brush using tool
			// but I don't really know that info currently.

			if ( OkayToUseToolAfterPickupBrush( m_PreviousDrawingTool ) ) {

				AfxGetMainWnd()->PostMessage( WM_COMMAND, m_PreviousDrawingTool, 0 );

			} else {

				AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_LINE, 0 );

			}

		} else {

			if ( (ID_DTOOL_FREEHAND_LINE != activeToolID) || 
				(ID_DTOOL_FREEHAND_SPOTTY != activeToolID) ) {
	
				// Really should choose based on the size of the brush.
		
				AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_LINE, 0 );

			}
	
		}

	}

#endif

// DESIGN!!!
// DESIGN!!!
// DESIGN!!!

}

void CBpaintDoc::SetBitmapBrushMode( const bool bEnable )
{
	m_bUseBitmapBrushRenderer_HACK = bEnable;

	RefreshPen();

}

void CBpaintDoc::OnBrushRestore() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		m_pPickupBrushMediator->RestoreBrush();

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnBrushRotate90() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		m_pPickupBrushMediator->ScaledRotate(
			90.0f, 1.0f, 1.0f, m_BrushChromaKey, false
		);

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnBrushHalveSize() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		m_pPickupBrushMediator->ScaledRotate(
			0.0f, 0.5f, 0.5f, m_BrushChromaKey, m_bUseOriginalOnBrushSizeChange
		);

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

bool CBpaintDoc::CanBrushDoubleSize( const bool bWidth, const bool bHeight )
{
	CSize size = GetDrawingBrushSize();

	if ( bWidth ) {

		if ( (size.cx * 2) > (4 * m_CanvasSize.cx) ) {

			return false;

		}

	}

	if ( bHeight ) {

		if ( (size.cy * 2) > (4 * m_CanvasSize.cy) ) {

			return false;

		}

	}

	return true;

}

void CBpaintDoc::OnBrushDoubleSize() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		if ( CanBrushDoubleSize( true, true ) ) {

			m_pPickupBrushMediator->ScaledRotate(
				0.0f, 2.0f, 2.0f, m_BrushChromaKey, m_bUseOriginalOnBrushSizeChange
			);

		}

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnBrushDoubleHeight() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		if ( CanBrushDoubleSize( false, true ) ) {

			m_pPickupBrushMediator->ScaledRotate(
				0.0f, 1.0f, 2.0f, m_BrushChromaKey, m_bUseOriginalOnBrushSizeChange
			);

		}

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnBrushDoubleWidth() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		if ( CanBrushDoubleSize( true, false ) ) {

			m_pPickupBrushMediator->ScaledRotate(
				0.0f, 2.0f, 1.0f, m_BrushChromaKey, m_bUseOriginalOnBrushSizeChange
			);

		}

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnBrushHflip() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		m_pPickupBrushMediator->BLITFXTransform(
			BPT::BLITFX( BPT::BLITFX::HFLIP ), m_BrushChromaKey
		);

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnBrushVflip() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		m_pPickupBrushMediator->BLITFXTransform(
			BPT::BLITFX( BPT::BLITFX::VFLIP ), m_BrushChromaKey
		);

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnHelperCycleFreehandTool() 
{
	int activeTool = m_EditorLogic.GetCurrentToolID();

	if ( ID_DTOOL_FREEHAND_SPOTTY == activeTool ) {

		AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_LINE, 0 );

	} else if ( ID_DTOOL_FREEHAND_LINE == activeTool ) {

		AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_SINGLE, 0 );

	} else if ( ID_DTOOL_FREEHAND_SINGLE == activeTool ) {

		AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_SPOTTY, 0 );

	} else {

		AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_LINE, 0 );

	}
}

void CBpaintDoc::OnReturnToBuiltInBrush() 
{
	if ( m_bUseBitmapBrushRenderer_HACK ) {

		SetBitmapBrushMode( false );

	} else {

		if ( (0 == m_PenSize.cx) && (0 == m_PenSize.cy) ) {

			m_PenSize = m_LastPenSize;

		} else {

			m_LastPenSize = m_PenSize;

			m_PenSize.cx = 0;
			m_PenSize.cy = 0;

		}

	}

	RefreshPen();
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::Request_L_ColorChange()
//

void 
CBpaintDoc::Request_L_ColorChange( const int slot )
{
	if ( slot == m_Color_L ) {

		return;

	}

	m_Color_L = slot;

	RefreshPen();
	
	UpdateAllViews(
		NULL, CColorChange::L_COLOR_CHANGE,
		(CObject *)&CColorChange( slot )
	);

	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );
}

//
//	CBpaintDoc::Request_R_ColorChange()
//

void 
CBpaintDoc::Request_R_ColorChange( const int slot )
{
	if ( slot == m_Color_R ) {

		return;

	}

	m_Color_R = slot;

	RefreshPen();

	UpdateAllViews(
		NULL, CColorChange::R_COLOR_CHANGE,
		(CObject *)&CColorChange( slot )
	);

	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );
}

//
//	CBpaintDoc::GetCurrentPalette()
//

BPT::CAnnotatedPalette * CBpaintDoc::GetCurrentPalette()
{
	// This may eventually return a palette specific to a layer

#if 1 // BPT 6/15/01

	if ( m_pAnimation ) {

		// Does the layer have a palette?
		// --------------------------------------------------------------------

		BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( 
			m_pAnimation->CurrentLayer()
		);
	
		if ( pLayerInfo ) {

			BPT::CAnnotatedPalette * pPal = pLayerInfo->GetPalette();
	
			if ( pPal ) {
	
				return pPal;
	
			}

		}

		// Does the animation have a palette?
		// --------------------------------------------------------------------

		BPT::CAnnotatedPalette * pPal = m_pAnimation->GetPalette();

		if ( pPal ) {

			return pPal;

		}

	}

#endif

	// Couldn't find an overriding palette use the showcase
	// ------------------------------------------------------------------------

	return m_pShowcase->GetDefaultPalette();
}

//
//	CBpaintDoc::Request_FullPaletteChange()
//

void CBpaintDoc::Request_FullPaletteChange(
	BPT::CAnnotatedPalette * pFrom, 
	const bool bRepaint, const bool bRepaintThumbnails
)
{
	// Set the showcase palette value 
	// ------------------------------------------------------------------------

	BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

	if ( (!pPal) || (!pFrom) ) {

		return;

	}

	// Copy the colors from the incomming palette
	// ------------------------------------------------------------------------

	pPal->CopyColorsFrom( pFrom );

	// Make sure that the palettes are managed (catch all implmentation...)
	// --------------------------------------------------------------------

	m_pShowcase->ManageSharedPaletteEntries(); // 6/17/01
	
	// Build the rendering palettes
	// ------------------------------------------------------------------------

	BuildEditToCompositeClut( bRepaint );

	// Repaint the thumbnails if requested.
	// ------------------------------------------------------------------------

#if 1

	if ( m_bEnableRenderThumbnailsHack && bRepaintThumbnails ) {

		// Force a repaint of the grid views
		// ------------------------------------------------------------------------
	
		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

	}

#endif

	SetModifiedFlag();

}

//
//	CBpaintDoc::Request_ColorRGBChange()
//

void CBpaintDoc::Request_ColorRGBChange(
	const int slot, const int r, const int g, const int b, 
	const bool bRepaint, const bool bRepaintThumbnails
)
{

	int vr = max( 0, min( 255, r ));
	int vg = max( 0, min( 255, g ));
	int vb = max( 0, min( 255, b ));

	// Set the showcase palette value 
	// ------------------------------------------------------------------------

	BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

	if ( pPal ) {

		pPal->SetSlot( slot, vr, vg, vb );

	}

	// Rebuild the 
	// ------------------------------------------------------------------------

	m_Edit2CompositeClut[ slot ] = MakeDisplayPixelType( vr, vg, vb );

	// Make sure that the palettes are managed (catch all implmentation...)
	// --------------------------------------------------------------------

	m_pShowcase->ManageSharedPaletteEntries(); // 6/17/01
	
	// ------------------------------------------------------------------------

	if ( bRepaint ) {

		ForceFullRedraw();

	}

	UpdateAllViews( NULL, CColorChange::SLOT_RGB_CHANGE, (CObject *)&CColorChange( slot ) );

	// ------------------------------------------------------------------------

#if 1

	if ( m_bEnableRenderThumbnailsHack && bRepaintThumbnails ) {

		// Force a repaint of the grid views
		// ------------------------------------------------------------------------
	
		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

	}

#endif

	SetModifiedFlag();

}

//
//	CBpaintDoc::Request_ColorRGBEdit()
//

void 
CBpaintDoc::Request_ColorRGBEdit( const int slot, CWnd * pCenterWnd /* = 0 */ )
{
	OnEditPalette();
}

//
//	CBpaintDoc::BuildEditToCompositeClut()
//

void CBpaintDoc::BuildEditToCompositeClut( const bool bForceRepaint )
{
	BPT::CAnnotatedPalette * pPal = GetCurrentPalette();

	if ( pPal ) {

		bool bChange = false;

		int nTotalColors = pPal->TotalColors();

		for ( int i = 0; i < nTotalColors; i++ ) {

			SDisplayPixelType color = MakeDisplayPixelType(
				pPal->GetSlot_R( i ), pPal->GetSlot_G( i ), pPal->GetSlot_B( i ) 
			);

			if ( color != m_Edit2CompositeClut[ i ] ) {

				m_Edit2CompositeClut[ i ] = color;

				bChange = true;

			}

		}

		if ( bChange ) {

			UpdateAllViews( NULL, CColorChange::WHOLE_PALETTE_CHANGE, (CObject *)&CColorChange(0) );

		}

		if ( bForceRepaint ) { // BPT 6/17/01 moved outside bChange check

			ForceFullRedraw();

		}

	}

}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnBrushCustomDeformation() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		RECT rect = m_pPickupBrushMediator->GetPickupRect();

		int x1 = rect.left;
		int y1 = rect.top;
		int x2 = rect.right;
		int y2 = rect.bottom;

		POINT quad[ 4 ] = {
			{ x1, y1 }, { x2, y1 }, { x2, y2 }, { x1, y2 }
		};

		if ( DeformationDialog( 0, "Brush deformation", quad ) ) {

			m_pPickupBrushMediator->DeformBrush(
				quad, m_BrushChromaKey
			);

		}

	} else {

		AutoSelectBrushPickupTool( true );

	}
}

void CBpaintDoc::OnBrushOutline() 
{
	if ( m_pPickupBrushMediator->GetBitmapPtr() ) {

		m_pPickupBrushMediator->Outline( m_Color_L, m_BrushChromaKey );

	} else {

		AutoSelectBrushPickupTool( true );

	}
	
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnUiLayout1() 
{
	LoadUILayout( ID_UI_LAYOUT_1, true );
}

void CBpaintDoc::OnUiLayout2() 
{
	LoadUILayout( ID_UI_LAYOUT_2, true );
}

void CBpaintDoc::OnUiLayout3() 
{
	LoadUILayout( ID_UI_LAYOUT_3, true );
}

void CBpaintDoc::OnUiLayout4() 
{
	LoadUILayout( ID_UI_LAYOUT_4, true );
}

void CBpaintDoc::OnUiLayout5() 
{
	LoadUILayout( ID_UI_LAYOUT_5, true );
}

void CBpaintDoc::OnUiLayout6() 
{
	LoadUILayout( ID_UI_LAYOUT_6, true );
}

void CBpaintDoc::OnUiLayout7() 
{
	LoadUILayout( ID_UI_LAYOUT_7, true );
}

void CBpaintDoc::OnUiLayout8() 
{
	LoadUILayout( ID_UI_LAYOUT_8, true );
}

void CBpaintDoc::OnUiLayout9() 
{
	LoadUILayout( ID_UI_LAYOUT_9, true );
}

void CBpaintDoc::OnUiLayout0() 
{
	LoadUILayout( ID_UI_LAYOUT_0, true );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnAnimCreate() 
{
	// ------------------------------------------------------------------------

	CCreateAnimDlg createDlg( AfxGetMainWnd() );

	// These settings should be persistant.

	createDlg.LoadSettings( CString( "CreateAnimationDlg" ) );

	if ( IDOK != createDlg.DoModal() ) {

		return ;

	}

	createDlg.SaveSettings();

	// Need to create an animation and select it!
	// ------------------------------------------------------------------------

	BPT::CAnimation * pAnim = m_pShowcase->NewAnimation(
		createDlg.m_nFrameCount,
		createDlg.m_nLayerCount
	);

	if ( pAnim ) {

		pAnim->SetName( createDlg.m_AnimationName );
		pAnim->SetPlaybackDelay( createDlg.m_nFrameRate );
		pAnim->m_Link1.x = createDlg.m_nLinkX;
		pAnim->m_Link1.y = createDlg.m_nLinkY;

		Request_AnimationChange( pAnim );

	} else {

		MessageBox( 
			AfxGetMainWnd()->GetSafeHwnd(), "Failed to create anim?", 
			"Error!", MB_ICONERROR|MB_OK
		);

	}

}

//
//	CBpaintDoc::OnAnimNext()
//

void CBpaintDoc::OnAnimNext() 
{
	Request_AnimationChange( m_pShowcase->NextAnimation( m_pAnimation ) );
}

//
//	CBpaintDoc::OnUpdateAnimNext()
//

void CBpaintDoc::OnUpdateAnimNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pAnimation ) );
}

//
//	CBpaintDoc::OnAnimPrev()
//

void CBpaintDoc::OnAnimPrev() 
{
	Request_AnimationChange( m_pShowcase->PrevAnimation( m_pAnimation ) );
}

//
//	CBpaintDoc::OnUpdateAnimPrev()
//

void CBpaintDoc::OnUpdateAnimPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pAnimation ) );
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::UILayoutSettingsStringWrapper()
//

const char * 
CBpaintDoc::UILayoutSettingsStringWrapper( const char * str )
{
	// This will eventually make the str passed valid for each
	// ui layout setting? Yeah I should have thought of this earlier...

	return str;
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnNewMdiGrid() 
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

	if ( pApp ) {

		GLOBAL_CreateNewWindow(
			pApp->GetMDIChildGridDocTemplate(), this
		);

	}
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnNewMultiSplitWindow() 
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

	if ( pApp ) {

		SetPendingMultiViewData( CSimpleSplitFrame::LAYOUT_1 );

		GLOBAL_CreateNewWindow(
			pApp->GetMDIChildMultiView1DocTemplate(), this
		);

		SetPendingMultiViewData( 0 );

	}
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnDestroyPaletteView() {

	if ( IsWindow( m_pPalFrameWnd->GetSafeHwnd() ) ) {

		CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

		if ( pApp ) {

			pApp->StoreFrameWindowSettings(
				m_pPalFrameWnd, 
				UILayoutSettingsStringWrapper( "PaletteFrameWindow" )
			);

		}

	}

	m_pPalFrameWnd = 0;

}

void CBpaintDoc::OnNewSimplePaletteView() 
{
	if ( m_pPalFrameWnd ) {

		m_pPalFrameWnd->DestroyWindow();

	} else {

		CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

		if ( pApp ) {
	
			m_pPalFrameWnd = GLOBAL_CreateNewWindow(
				pApp->GetPopupSimplePaletteDocTemplate(), this,
				UILayoutSettingsStringWrapper( "PaletteFrameWindow" )
			);
	
		}

	}
}

void CBpaintDoc::OnUpdateNewSimplePaletteView(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true );
	
	pCmdUI->SetCheck( (0 != m_pPalFrameWnd) ? 1 : 0 );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnDestroyPopupGridView( CView * pView )
{
	if ( m_pPopupGridFrameWnd ) {

		if ( pView->GetParentFrame() == m_pPopupGridFrameWnd ) {

			if ( IsWindow( m_pPopupGridFrameWnd->GetSafeHwnd() ) ) {
		
				CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
		
				if ( pApp ) {
		
					pApp->StoreFrameWindowSettings(
						m_pPopupGridFrameWnd, 
						UILayoutSettingsStringWrapper( "PopupGridFrameWindow" )
					);
		
				}
		
			}

			m_pPopupGridFrameWnd = 0;

		}

	}

}

void CBpaintDoc::OnNewPopupGridView() 
{
	if ( m_pPopupGridFrameWnd ) {

		m_pPopupGridFrameWnd->DestroyWindow();

	} else {

		CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

		if ( pApp ) {
	
			m_pPopupGridFrameWnd = GLOBAL_CreateNewWindow(
				pApp->GetPopupGridDocTemplate(), this,
				UILayoutSettingsStringWrapper( "PopupGridFrameWindow" )
			);
	
		}

	}
}

void CBpaintDoc::OnUpdateNewPopupGridView(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( true );
	
	pCmdUI->SetCheck( (0 != m_pPopupGridFrameWnd) ? 1 : 0 );
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::Request_LayerProperties()
//

void CBpaintDoc::Request_LayerProperties( const int nLayer )
{
	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );
	Request_LayerChange( nLayer );
	OnLayerProperties();
}

//
//	CBpaintDoc::Request_FrameProperties()
//

void CBpaintDoc::Request_FrameProperties( const int nFrame )
{
	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );
	Request_FrameChange( nFrame );
	OnFrameProperties();
}

//RONG: Added so it can be called from SelectAnimationDlg (plus others?)

void CBpaintDoc::Request_AnimationProperties( CWnd * pOwner )
{
	OnAnimProperties2( pOwner ); // BPT 6/5/02
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::Request_FullChange()
//

void CBpaintDoc::Request_FullChange( BPT::CAnimation * pNew, const int nFrame, const int nLayer )
{
	// --------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	// --------------------------------------------------------------------

	CBpaintDoc::MODE oldMode = m_CurrentMode;

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	SetCurrentNamedSpot( 0 );

	m_pAnimation = pNew;

	if ( pNew ) {

		// --------------------------------------------------------------------

		pNew->SetFrame( nFrame );
		pNew->SetLayer( nFrame );

		// Hookup the animation frame rate (lame)
		// --------------------------------------------------------------------

		SyncAnimationPlaybackRate();

	}

	// Build the edit to composite clut

	BuildEditToCompositeClut();

	// Need to tell the views of the change.

	CActiveAnimationChange changeInfo;
		
	changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
	changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
	changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
	changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

	UpdateAllViews( 0, CActiveAnimationChange::NEW_EVERYTHING, &changeInfo );

	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );

	// Need to ??

	PENDING_SetDocumentMode( oldMode );
}

//
//	CBpaintDoc::Request_SmartFrameLayerChange()
//

void 
CBpaintDoc::Request_SmartFrameLayerChange( const int nFrame, const int nLayer )
{

	// --------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	// --------------------------------------------------------------------

	if ( m_pAnimation ) {

		if ( (m_pAnimation->CurrentFrame() == nFrame) &&
			(m_pAnimation->CurrentLayer() == nLayer) ) {

			return;

		}

	}

	CBpaintDoc::MODE oldMode = m_CurrentMode;

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	int oldFrame = nFrame;
	int oldLayer = nLayer;

	if ( m_pAnimation ) {

		oldFrame = m_pAnimation->CurrentFrame();
		oldLayer = m_pAnimation->CurrentLayer();

		m_pAnimation->SetFrame( nFrame );
		m_pAnimation->SetLayer( nLayer );

	}

	// Build the edit to composite clut

	BuildEditToCompositeClut();

	// Need to tell the views of the change.

	CActiveAnimationChange changeInfo;

	changeInfo.m_OldSelection.x = oldFrame;
	changeInfo.m_OldSelection.y = oldLayer;
	changeInfo.m_NewSelection.x = nFrame;
	changeInfo.m_NewSelection.y = nLayer;

	UpdateAllViews( 0, CActiveAnimationChange::BOTH_CHANGE, &changeInfo );

	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );

	// Restore the mode

	PENDING_SetDocumentMode( oldMode );
}

//
//	BPaintDoc::Request_AnimationChange()
//

void CBpaintDoc::Request_AnimationChange( BPT::CAnimation * pNew )
{
	// --------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	// --------------------------------------------------------------------

	CBpaintDoc::MODE oldMode = m_CurrentMode;

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	SetCurrentNamedSpot( 0 );

	m_pAnimation = pNew;

	// Hookup the animation frame rate (lame)
	// --------------------------------------------------------------------

	SyncAnimationPlaybackRate();

	// Build the edit to composite clut

	BuildEditToCompositeClut();

	// Need to tell the views of the change.

	CActiveAnimationChange changeInfo;
		
	changeInfo.m_OldSelection.x = m_pAnimation ? m_pAnimation->CurrentFrame() : 0;
	changeInfo.m_OldSelection.y = m_pAnimation ? m_pAnimation->CurrentLayer() : 0;
	changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
	changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

	UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );

	// Restore the mode

	PENDING_SetDocumentMode( oldMode );
}

//
//	BPaintDoc::Request_PrevAnimation()
//

void CBpaintDoc::Request_PrevAnimation( BPT::CAnimation * pFrom )
{
	// Request_AnimationChange( ... )
}

//
//	BPaintDoc::Request_NextAnimation()
//

void CBpaintDoc::Request_NextAnimation( BPT::CAnimation * pFrom )
{
	// Request_AnimationChange( ... )
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::Request_FrameChange()
//

void CBpaintDoc::Request_FrameChange( const int nFrame )
{
	if ( m_pAnimation ) {

		if ( m_pAnimation->CurrentFrame() == nFrame ) {

			return;

		}

	}

	CBpaintDoc::MODE oldMode = m_CurrentMode;

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	int oldFrame = nFrame;
	int oldLayer = 0;

	if ( m_pAnimation ) {

		oldFrame = m_pAnimation->CurrentFrame();
		oldLayer = m_pAnimation->CurrentLayer();

		m_pAnimation->SetFrame( nFrame );

	}

	// Build the edit to composite clut

	BuildEditToCompositeClut();

	// Need to tell the views of the change.

#if 1

	// This is faster because the grid view is optimized for it.
	// it should really be optimized for all modes but time is limited

	CActiveAnimationChange changeInfo;

	changeInfo.m_OldSelection.x = oldFrame;
	changeInfo.m_OldSelection.y = oldLayer;
	changeInfo.m_NewSelection.x = nFrame;
	changeInfo.m_NewSelection.y = oldLayer;

	UpdateAllViews( 0, CActiveAnimationChange::BOTH_CHANGE, &changeInfo );

#else

	UpdateAllViews( 0, CActiveAnimationChange::FRAME_CHANGE, &CActiveAnimationChange() );

#endif

	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );

	// Restore the mode

	PENDING_SetDocumentMode( oldMode );
}

//
//	CBpaintDoc::Request_PrevFrame()
//

void CBpaintDoc::Request_PrevFrame()
{
	if ( m_pAnimation ) {

		int nFrame = m_pAnimation->CurrentFrame() - 1;

		if ( 0 > nFrame ) {

			nFrame = m_pAnimation->FrameCount() - 1;

		}

		Request_FrameChange( nFrame );

	}
}

//
//	CBpaintDoc::Request_NextFrame()
//

void CBpaintDoc::Request_NextFrame()
{
	if ( m_pAnimation ) {

		int nFrame = m_pAnimation->CurrentFrame() + 1;

		if ( m_pAnimation->FrameCount() <= nFrame ) {

			nFrame = 0;

		}

		Request_FrameChange( nFrame );

	}
}

void CBpaintDoc::Request_FirstFrame()
{
	Request_FrameChange( 0 );
}

void CBpaintDoc::Request_LastFrame()
{
	if ( m_pAnimation ) {

		Request_FrameChange( m_pAnimation->FrameCount() - 1 );

	}
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::Request_LayerChange()
//

void CBpaintDoc::Request_LayerChange( const int nLayer )
{
	if ( m_pAnimation ) {

		if ( m_pAnimation->CurrentLayer() == nLayer ) {

			return;

		}

	}

	CBpaintDoc::MODE oldMode = m_CurrentMode;

	if ( !SetDocumentMode( CBpaintDoc::MODE::UNKNOWN ) ) {

		return;

	}

	int oldFrame = 0;
	int oldLayer = nLayer;

	if ( m_pAnimation ) {

		oldFrame = m_pAnimation->CurrentFrame();
		oldLayer = m_pAnimation->CurrentLayer();

		m_pAnimation->SetLayer( nLayer );

	}

	// Build the edit to composite clut

	BuildEditToCompositeClut();

	// Need to tell the views of the change.

#if 1

	// This is faster because the grid view is optimized for it.
	// it should really be optimized for all modes but time is limited

	CActiveAnimationChange changeInfo;

	changeInfo.m_OldSelection.x = oldFrame;
	changeInfo.m_OldSelection.y = oldLayer;
	changeInfo.m_NewSelection.x = oldFrame;
	changeInfo.m_NewSelection.y = nLayer;

	UpdateAllViews( 0, CActiveAnimationChange::BOTH_CHANGE, &changeInfo );

#else

	UpdateAllViews( 0, CActiveAnimationChange::LAYER_CHANGE, &CActiveAnimationChange() );

#endif

	UpdateStatusBarWithEditMsgInfo( 0, m_ptLastKnownLocation );

	// Restore the mode

	PENDING_SetDocumentMode( oldMode );
}

//
//	CBpaintDoc::Request_PrevLayer()
//

void CBpaintDoc::Request_PrevLayer()
{
	if ( m_pAnimation ) {

		int nLayer = m_pAnimation->CurrentLayer() - 1;

		if ( 0 > nLayer ) {

			nLayer = m_pAnimation->LayerCount() - 1;

		}

		Request_LayerChange( nLayer );

	}
}

//
//	CBpaintDoc::Request_NextLayer()
//

void CBpaintDoc::Request_NextLayer()
{
	if ( m_pAnimation ) {

		int nLayer = m_pAnimation->CurrentLayer() + 1;

		if ( m_pAnimation->LayerCount() <= nLayer ) {

			nLayer = 0;

		}

		Request_LayerChange( nLayer );

	}
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::CurrentAnimation()
//

BPT::CAnimation * 
CBpaintDoc::CurrentAnimation()
{
	return m_pAnimation;
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::FinishEdit()
//

bool 
CBpaintDoc::FinishEdit()
{
	// --------------------------------------------------------------------

	if ( CBpaintDoc::MODE::DRAWING != m_CurrentMode ) {

		return true;

	}

	// Make sure we have an animation and a layer to change.
	// --------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return false;

	}

	BPT::CLayer * pLayer = m_pAnimation->ActiveLayer();

	if ( !pLayer ) {

		return false;

	}

	// make sure that the tool is 'finished'
	// --------------------------------------------------------------------

	m_EditorLogic.Edit_OnFinish();

	m_EditorLogic.EndSession();

	// Create a visual element for the current frame.
	// --------------------------------------------------------------------

	editor_bitmap_type * pEditorBitmap = m_EditorLogic.GetFinalBitmapPtr();

	if ( !pEditorBitmap ) {

		return false;

	}

	// find the bounding rectangle to determine if the component was cleared...

	CRect captureRect;

	BPT::T_FindBoundingRect(
		captureRect, *pEditorBitmap, BPT::TIsNotValue<editor_pixel_type>( m_ChromaKey ), 0
	);

	if ( captureRect.IsRectEmpty() ) {

		// clear the visual element for the current frame/layer
		// --------------------------------------------------------------------
	
		pLayer->SetVisualElement( 0, &CPoint( 0, 0 ) );

	} else
		
#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

	if ( m_bSimpleEditMode )

#endif// BPT 5/22/01

	{

		// Ask the showcase to create us a new visual element
		// --------------------------------------------------------------------

		BPT::CVisualElement *pVisualElement = m_pShowcase->CreateVisualElementFromSurface(
			*pEditorBitmap, m_ChromaKey, &captureRect
		);
	
		if ( !pVisualElement ) {

#ifdef _DEBUG

			TRACE( "Failed to create visual element? (Yikes!)\n" );

#endif
			return false;
	
		}

		// Replace the visual element for the current frame/layer
		// --------------------------------------------------------------------
	
		pLayer->SetVisualElement( pVisualElement, &CPoint( 0, 0 ) );

	}

// ============================================================================
// BEGIN New smarter edit logic (BPT 5/22/01)
// ============================================================================
	
#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

	else {

		// Get a reference to any existing visual element for this frame-layer
		// --------------------------------------------------------------------

		BPT::CVisualElement *pOldVisualElement = pLayer->GetVisualElement();

		RECT oldBoundingRect;

		pLayer->BoundingRect( oldBoundingRect );

		POINT originOffset = pLayer->GetPos();

		// Check to see if this was a potentially clipped edit
		// (This could figure out if the cel wasn't edited and avoid
		// this message all together...)
		// ----------------------------------------------------------------

		CRect canvasTestRect( CPoint(0,0), m_CanvasSize ), clipResults;

		clipResults.IntersectRect( &canvasTestRect, &oldBoundingRect );

#if 0 // BPT 6/7/01 

// I moved the test to before the enter edit mode so if the user
// accepts the entering edit mode then it's on them to know what
// they are doing if they get to this point.

		if ( !clipResults.EqualRect( &oldBoundingRect ) ) {

			CString message;

			message.Format(

				"The image you just edited was potentially clipped "
				"off the edge of the drawing canvas did you still "
				"want to proceed with the edit?"

			);

			if ( IDNO == MessageBox(
				AfxGetMainWnd()->GetSafeHwnd(), 
				message, "HEY!", MB_YESNO) ) {

				pOldVisualElement->Release();
	
				return true;

			}

		}

#endif

		// Need to create a new visual element here and check to see if there
		// was no change if so then we ignore nop edits (happens very often)
		// --------------------------------------------------------------------

		BPT::CVisualElement *pNewVisualElement = new BPT::CVisualElement( 0 );

		if ( !pNewVisualElement ) {

#ifdef _DEBUG
			TRACE( "Failed to construct CVisualElement object?\n" );
#endif

			return false;

		}

		// Capture the image 
		// --------------------------------------------------------------------

		if ( !pNewVisualElement->CreateFrom(
			*pEditorBitmap, BPT::TIsNotValue<editor_bitmap_type::pixel_type>( m_ChromaKey ), 
			&captureRect) ) {

			delete pNewVisualElement;

#ifdef _DEBUG
			TRACE( "Failed to create CVisualElement compressed image?\n" );
#endif

			return false;

		}

		// Okay finally check to see if the image didn't change, if it didn't
		// we can leave this function after cleaning up.
		// --------------------------------------------------------------------

		bool bReplaceOtherReferences = false;

		if ( pOldVisualElement ) {

#ifdef _DEBUG

			TRACE( 
				"OldRect( %4d, %4d, %4d, %4d )\n", 
				oldBoundingRect.left, oldBoundingRect.top,
				oldBoundingRect.right, oldBoundingRect.bottom
			);

			TRACE( 
				"NewRect( %4d, %4d, %4d, %4d )\n", 
				captureRect.left, captureRect.top,
				captureRect.right, captureRect.bottom
			);

#endif

			if ( EqualRect( &captureRect, &oldBoundingRect) &&
				pNewVisualElement->CompareVisualElement( pOldVisualElement ) ) {
	
#ifdef _DEBUG
				TRACE( "There was no visual change for this cel :)\n" );
				TRACE( "------------------------------\n\n" );
#endif

				delete pNewVisualElement;

				pOldVisualElement->Release();

				return true;

			}

			// Figure out if we need to replace all other occurrences with the
			// new edit, this is done after we detect no change.
			// ----------------------------------------------------------------

			int oldReferenceCount = pOldVisualElement->RefCount();

			if ( 3 < oldReferenceCount ) {

#ifdef _DEBUG
				TRACE( "Old visual element was used %d times\n", (oldReferenceCount - 2) );
#endif

				int nResult = IDNO;

				switch ( m_nReplaceVisualElementMode ) {

				default:
				case ON_REPLACE_DO_NOTHING:
					break;

				case ON_REPLACE_ASK: {

						CString message;

						message.Format( 
							"Old cel was used %d times, replace other "
							"occurrences with your latest edit?",
							(oldReferenceCount - 2)
						);

						nResult = MessageBox( 
							AfxGetMainWnd()->GetSafeHwnd(), 
							message, "Yo!", MB_YESNOCANCEL 
						);

					}
					break;

				case ON_REPLACE_DONT_ASK:
					nResult = IDYES;
					break;

				}
				
				// Deal with the results of the question
				// ------------------------------------------------------------

				if ( IDYES == nResult ) {

					bReplaceOtherReferences = true;

				} else if ( IDCANCEL == nResult ) {

#ifdef _DEBUG
					TRACE( "User canceled edit change\n" );
					TRACE( "------------------------------\n\n" );
#endif

					delete pNewVisualElement;

					pOldVisualElement->Release();

					return true;

				}

			}

		}

		// Transfer ownership of the new visual element to the showcase, so
		// it can be processed by the showcase helper methods.
		// --------------------------------------------------------------------

		pNewVisualElement->SetID( m_pShowcase->NewID() );

		m_pShowcase->AdoptVisualElement( pNewVisualElement );

		// Replace the visual element for the current frame/layer
		// --------------------------------------------------------------------

		pLayer->SetVisualElement( pNewVisualElement, &CPoint( 0, 0 ) );

		// Check to see if the item was exactly the same as something else...
		// --------------------------------------------------------------------

		if ( pLayer->OptimizeVisualElements( m_pShowcase, 0 ) ) {

#ifdef _DEBUG
			TRACE( "Was exactly the same as an existing cel! :)\n" );
#endif

		}

		// Fetch a new reference to the visual element just incase the
		// optimize changed what object we're using.
		// --------------------------------------------------------------------

		pNewVisualElement = pLayer->GetVisualElement();

#ifdef _DEBUG 

		TRACE( "OldVE = %p NewVE = %p\n", pOldVisualElement, pNewVisualElement );

#endif

		// Replace other references with new visual element with an offset
		// --------------------------------------------------------------------

		if ( bReplaceOtherReferences ) {

			// Get the new bounding rectangle to determine delta
			// ----------------------------------------------------------------

			RECT newBoundingRect;
	
			pLayer->BoundingRect( newBoundingRect );

#if 1

			POINT delta = {
				-(originOffset.x),
				-(originOffset.y)
			};

#else

			POINT delta = {
				-(originOffset.x - (newBoundingRect.left - oldBoundingRect.left)),
				-(originOffset.y - (newBoundingRect.top - oldBoundingRect.top))
			};

#endif

#ifdef _DEBUG

			TRACE( 
				"FinRect( %4d, %4d, %4d, %4d )\n", 
				newBoundingRect.left, newBoundingRect.top,
				newBoundingRect.right, newBoundingRect.bottom
			);

			TRACE( "Delta = %d, %d\n", delta.x, delta.y );

#endif

			// Okay ask the showcase to propagate the change to all the
			// other cels.... (This will need to cause a potentially ugly
			// repaint of the grid view, unless an update aware method is built)
			// ----------------------------------------------------------------

			if ( m_pShowcase->AdjustVisualReferences(
				pOldVisualElement, pNewVisualElement, &delta, 0 ) ) {

#ifdef _DEBUG

				TRACE( "Need to repaint the whole grid\n" );

#endif

				// Force a repaint of the grid views
				// ------------------------------------------------------------

				if ( m_bEnableRenderThumbnailsHack ) {

					CActiveAnimationChange changeInfo;
						
					changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
					changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
					changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
					changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
				
					UpdateAllViews( 0, CActiveAnimationChange::NEW_EVERYTHING, &changeInfo );

				} else {

					// Probably should have a new message for just updating the
					// visual element numbers!!!

					CActiveAnimationChange changeInfo;
						
					changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
					changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
					changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
					changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
				
					UpdateAllViews( 0, CActiveAnimationChange::NEW_EVERYTHING, &changeInfo );

				}

			}

		}

		pNewVisualElement->Release();

		// Finally release our reference on the old visual element (if one)
		// --------------------------------------------------------------------

		if ( pOldVisualElement ) {

			pOldVisualElement->Release();

		}

		// Should we release newly unused visual elements?
		// --------------------------------------------------------------------

		if ( m_bReleaseUnusedAfterEachEdit ) {

			m_pShowcase->ReleaseUnusedVisualElements();

		}

	}

#ifdef _DEBUG

	TRACE( "------------------------------\n\n" );

#endif

#endif // BPT 5/22/01

// ============================================================================
// FINISH New smarter edit logic (BPT 5/22/01)
// ============================================================================

	SetModifiedFlag();

	return true;
}

//
//	CBpaintDoc::DiscardEdit()
//

void 
CBpaintDoc::DiscardEdit()
{
	// ????
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::BuildBackgroundLayersSurface()
//

void
CBpaintDoc::BuildBackgroundLayersSurface()
{
	// Determine if the we need to render anything
	// ------------------------------------------------------------------------

	int nActiveLayer = m_pAnimation->CurrentLayer();

	for ( int i = 0; i < nActiveLayer; i++ ) {

		BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );

		if ( pLayer ) {

			if ( pLayer->HasVisualElement() && IsLayerVisible( i ) ) {

				break;

			}

		}

	}

	if ( i == nActiveLayer ) {

		m_bHasBackgroundLayers = false;

		return;

	}

#if 1 // BPT 6/27/01

	// Use the composite renderer so that any special rendering options
	// can be done in only one place
	// ------------------------------------------------------------------------

	RECT rect = m_BackgroundSurface.Rect();

	RenderCompositeModePrimEx(
		m_BackgroundSurface, rect,
		false, true, 0, 0, (nActiveLayer - 1), true, false
	);

#else

	// Render the background now
	// ------------------------------------------------------------------------

	RECT rect = m_BackgroundSurface.Rect();

	RenderBackgroundRect( m_BackgroundSurface, rect );

	// Determine the editor to composite clut to fallback to if no per layer
	// ------------------------------------------------------------------------

	editor_to_composite_clut_type * pFallbackClut = GetShowcaseCLUT();
	
#if 1 // BPT 6/15/01

	// check to see if the animation has a clut if so override
	// the showcase clut with it
	// ------------------------------------------------------------------------

	if ( m_pAnimation ) {

		BPT::CAnnotatedPalette * pPal = m_pAnimation->GetPalette();

		if ( pPal ) {

			pFallbackClut = pPal->GetDisplayCLUT();

		}

	}

#endif // BPT 6/15/01

	// Okay now that we've determine there is something to render do so.
	// ------------------------------------------------------------------------

	for ( i = 0; i < nActiveLayer; i++ ) {

		BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );

		if ( pLayer ) {

			if ( IsLayerVisible( i ) ) {

				editor_to_composite_clut_type * pClut = pFallbackClut;

#if 1 // BPT 6/15/01

				// Determine which clut to use
				// ------------------------------------------------------------

				BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( i );

				if ( pLayerInfo ) {

					BPT::CAnnotatedPalette * pPal = pLayerInfo->GetPalette();

					if ( pPal ) {

						pClut = pPal->GetDisplayCLUT();

					}

				}

#endif
	
				// finally render something!
				// ------------------------------------------------------------

				visual_to_composite_copy_type top( pClut );

				pLayer->Render( m_BackgroundSurface, 0, 0, top );

			}

		}

	}

#endif // BPT 6/27/01

	m_bHasBackgroundLayers = true;
}

//
//	CBpaintDoc::BuildForegroundLayersSurface()
//

void
CBpaintDoc::BuildForegroundLayersSurface()
{
	// Determine if the we need to render anything
	// ------------------------------------------------------------------------

	int nActiveLayer = m_pAnimation->CurrentLayer();

	int nLayerCount = m_pAnimation->LayerCount();

	for ( int i = (nActiveLayer + 1); i < nLayerCount; i++ ) {

		BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );

		if ( pLayer ) {

			if ( pLayer->HasVisualElement() && IsLayerVisible( i ) ) {

				break;

			}

		}

	}

	if ( i == nLayerCount ) {

		m_bHasForegroundLayers = false;

		return;

	}

#if 1 // BPT 6/27/01

	// Use the composite renderer so that any special rendering options
	// can be done in only one place
	// ------------------------------------------------------------------------

	m_ForegroundSurface.ClearBuffer( m_CompositeChromaKey );

	RECT rect = m_ForegroundSurface.Rect();

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/30/03

	ASSERT( false ); // This needs to be changed, to know about destination alpha...

#endif

	RenderCompositeModePrimEx(
		m_ForegroundSurface, rect,
		false, true, 0, (nActiveLayer + 1), (nLayerCount - 1) , false, true
	);

#else

	// Determine the editor to composite clut to fallback to if no per layer
	// ------------------------------------------------------------------------

	editor_to_composite_clut_type * pFallbackClut = GetShowcaseCLUT(); // BPT 6/15/01
	
#if 1 // BPT 6/15/01

	// check to see if the animation has a clut if so override
	// the showcase clut with it
	// ------------------------------------------------------------------------

	if ( m_pAnimation ) {

		BPT::CAnnotatedPalette * pPal = m_pAnimation->GetPalette();

		if ( pPal ) {

			pFallbackClut = pPal->GetDisplayCLUT();

		}

	}

#endif // BPT 6/15/01

	// Okay now that we've determine there is something to render do so.
	// ------------------------------------------------------------------------

	m_ForegroundSurface.ClearBuffer( m_CompositeChromaKey );

	for ( i = (nActiveLayer + 1); i < nLayerCount; i++ ) {

		BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );

		if ( pLayer ) {

			if ( IsLayerVisible( i ) ) {

				editor_to_composite_clut_type * pClut = pFallbackClut;

#if 1 // BPT 6/15/01

				// Determine which clut to use
				// ------------------------------------------------------------

				BPT::CLayerInfo * pLayerInfo = m_pAnimation->GetLayerInfo( i );

				if ( pLayerInfo ) {

					BPT::CAnnotatedPalette * pPal = pLayerInfo->GetPalette();

					if ( pPal ) {

						pClut = pPal->GetDisplayCLUT();

					}

				}

#endif // BPT 6/15/01
	
				// finally render something!
				// ------------------------------------------------------------

				visual_to_composite_copy_type top( pClut );

				pLayer->Render( m_ForegroundSurface, 0, 0, top );

			}

		}

	}

#endif // BPT 6/27/01

	m_bHasForegroundLayers = true;

	// Build the compressed foreground image.
	// ------------------------------------------------------------------------

	BPT::TIsNotValue<composite_pixel_type> predicate( m_CompositeChromaKey );

	if ( !m_CompressedForegroundImage.Create( m_ForegroundSurface, predicate ) ) {

		TRACE( "Warning: Failed to compress foreground image.\n" );

	}

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::ClipCheckConfirmEditRequest()
//

bool 
CBpaintDoc::ClipCheckConfirmEditRequest()
{

#if 1 // BPT 6/7/01

	static bool bInsideAlready = false;

	if ( bInsideAlready ) {

		return false;

	}

	// This would be nice to enable but it causes 2 question message boxes to
	// show up, and I don't have time to debug why.

	// Make sure we have an animation and a layer to change.
	// --------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return false;

	}

	// --------------------------------------------------------------------

	BPT::CLayer * pLayer = m_pAnimation->ActiveLayer();

	if ( !pLayer ) {

		return false;

	}

	// Check to see if this was a potentially clipped edit
	// ----------------------------------------------------------------

	RECT oldBoundingRect;

	if ( pLayer->BoundingRect( oldBoundingRect ) ) {

		CRect canvasTestRect( CPoint(0,0), m_CanvasSize ), clipResults;
	
		clipResults.IntersectRect( &canvasTestRect, &oldBoundingRect );
	
		if ( !clipResults.EqualRect( &oldBoundingRect ) ) {
	
			bInsideAlready = true; // attempt stop wacky timer recursion

			CString message;
	
			message.Format(
	
				"The image you are about to edit is clipped "
				"off the edge of the drawing canvas did you still "
				"want to proceed with the edit?"
	
			);
	
			if ( IDNO == MessageBox(
				AfxGetMainWnd()->GetSafeHwnd(), 
				message, "HEY!", MB_YESNO) ) {

				bInsideAlready = false; // attempt stop wacky timer recursion

				return false;
	
			}
	
			bInsideAlready = false; // attempt stop wacky timer recursion

		}

	}

#endif

	return true;

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::BeginEdit()
//

bool 
CBpaintDoc::BeginEdit()
{
	// --------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return false;

	}

	// Make sure we have an animation and a layer to change.
	// --------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return false;

	}

	// --------------------------------------------------------------------

	BPT::CLayer * pLayer = m_pAnimation->ActiveLayer();

	if ( !pLayer ) {

		return false;

	}

	// If the stencil isn't on act like the user hasn't defined one on
	// this new 'edit'
	// --------------------------------------------------------------------

	if ( !m_bColorStencilEnabled ) {
		
		m_bColorStencilDefined = 0;

	}

	// Try to create an image to temporarly store the cel to be edited.
	// --------------------------------------------------------------------

	editor_bitmap_type tempImage;

	if ( !tempImage.Create( m_CanvasSize.cx, m_CanvasSize.cy ) ) {

		return false;

	}

	// Clear the image  to the chromakey color then render the visual
	// element into place. Once that is complete then kickstart the editor
	// --------------------------------------------------------------------

	tempImage.ClearBuffer( m_ChromaKey );

//	typedef BPT::TCopyROP< editor_pixel_type, BPT::CVisualElement::storage_type > top_type;

	pLayer->Render( tempImage, 0, 0, visual_to_editor_copy_type() );

	m_EditorLogic.BeginSession( &tempImage, m_ChromaKey );

	m_EditorLogic.SetStencilMode( m_bColorStencilEnabled, m_bColorStencilTable );
	
	// Render the foreground & background precomposited images...
	// --------------------------------------------------------------------

	BuildBackgroundLayersSurface();

	BuildForegroundLayersSurface();
	
	// --------------------------------------------------------------------
	// force a repaint hack!

	ForceFullRedraw();

	SetModifiedFlag();

	return true;
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::PENDING_SetDocumentMode()
//

void 
CBpaintDoc::PENDING_SetDocumentMode( enum MODE mode )
{
	// Drawing mode is delayed so that rapid switching between frames
	// is quick.

	if ( (CBpaintDoc::MODE::DRAWING == mode) && (CBpaintDoc::MODE::NOT_PENDING == m_PendingMode) ) {

#if 0 
		
		// try to speed up edit mode feed back if the user is completing a drawing
		// on a different frame than it was started.

		if ( GetCapture() ) {

			SetDocumentMode( mode );
			ForceFullRedraw();
			return;

		}

#endif

		m_PendingMode = mode;

		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_REQUEST_PENDING_MODE );
	
		SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

		ForceFullRedraw();

	} else {

		if ( CBpaintDoc::MODE::NOT_PENDING == m_PendingMode ) {

			SetDocumentMode( mode );

			ForceFullRedraw();

		} else {

			SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

			ForceFullRedraw();

		}

	}

}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::SetDocumentMode()
//

bool 
CBpaintDoc::SetDocumentMode( enum MODE mode )
{
	// bail out if the current mode is already set to the requested mode
	// ------------------------------------------------------------------------

	if ( mode == m_CurrentMode ) {

		return true;

	}

	// Finish mode?
	// ------------------------------------------------------------------------

	// need to finish the current edit.

	if ( !FinishEdit() ) {

		return false;

	}

	// Choose the new mode
	// ------------------------------------------------------------------------

	if ( CBpaintDoc::MODE::DRAWING == mode ) {

		// Need to engage the editor on the current frame

#if 1 // BPT 5/23/01

		if ( !ClipCheckConfirmEditRequest() ) {

			if ( CBpaintDoc::MODE::NOT_PENDING != m_PendingMode ) {

				m_PendingMode = CBpaintDoc::MODE::COMPOSITE;

			}

			mode = CBpaintDoc::MODE::COMPOSITE;

			goto TRY_COMPOSITE_MODE_SWITCH;

		}

#endif

		if ( !BeginEdit() ) {

			return false;

		}

		m_CurrentMode = mode;

		// this is questionable!

//		ForceFullRedraw();

	} else if ( CBpaintDoc::MODE::COMPOSITE == mode ) {

	TRY_COMPOSITE_MODE_SWITCH:

		m_CurrentMode = mode;

		ClearOverlayType();

		// Need to switch things over to composite mode

		// this is questionable!

//		ForceFullRedraw();

	} else {

		m_CurrentMode = mode;

		// does the unknown mode have anything special about it?

	}

	// hack to force the crsr

	HCURSOR hCrsr = GetViewCursor( 0, m_ptLastKnownLocation, HTCLIENT );

	if ( hCrsr ) {

		SetCursor( hCrsr );

	}

	return true;

}

//
//	CBpaintDoc::EnsureActiveEditor()
//

bool 
CBpaintDoc::EnsureActiveEditor()
{
	if ( CBpaintDoc::MODE::NOT_PENDING != m_PendingMode ) {

		OnPendingMode();

	}
	
	return true; // may eventually be deterimed by something else.
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnFrameNext() 
{
	Request_NextFrame();
}

void CBpaintDoc::OnUpdateFrameNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::OnFramePrev() 
{
	Request_PrevFrame();
}

void CBpaintDoc::OnUpdateFramePrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::On2FrameNext() 
{
	OnFrameNext();
}

void CBpaintDoc::OnUpdate2FrameNext(CCmdUI* pCmdUI) 
{
	OnUpdateFrameNext( pCmdUI );
}

void CBpaintDoc::On2FramePrev() 
{
	OnFramePrev();
}

void CBpaintDoc::OnUpdate2FramePrev(CCmdUI* pCmdUI) 
{
	OnUpdateFramePrev( pCmdUI );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnLayerNext() 
{
	Request_NextLayer();
}

void CBpaintDoc::OnUpdateLayerNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::OnLayerPrev() 
{
	Request_PrevLayer();
}

void CBpaintDoc::OnUpdateLayerPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::On2LayerNext() 
{
	OnLayerNext();
}

void CBpaintDoc::OnUpdate2LayerNext(CCmdUI* pCmdUI) 
{
	OnUpdateLayerNext( pCmdUI );
}

void CBpaintDoc::On2LayerPrev() 
{
	OnLayerPrev();
}

void CBpaintDoc::OnUpdate2LayerPrev(CCmdUI* pCmdUI) 
{
	OnUpdateLayerPrev( pCmdUI );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnFrameGoto() 
{
	if ( m_pAnimation ) {

		CGotoFrameDlg dlg( AfxGetMainWnd() );

		dlg.m_nFrame = m_pAnimation->CurrentFrame() + 1;
		dlg.m_nFrameMin = 1;
		dlg.m_nFrameMax = m_pAnimation->FrameCount();
		dlg.m_strFrameRange.Format( "Frames (%d to %d)", dlg.m_nFrameMin, dlg.m_nFrameMax );

		if ( IDOK == dlg.DoModal() ) {

			Request_FrameChange( dlg.m_nFrame - 1 );

		}

	}
}

void CBpaintDoc::OnUpdateFrameGoto(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::OnAnimFastforward() 
{
	Request_LastFrame();
}

void CBpaintDoc::OnUpdateAnimFastforward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::OnAnimRewind() 
{
	Request_FirstFrame();
}

void CBpaintDoc::OnUpdateAnimRewind(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}
// ----------------------------------------------------------------------------

void CBpaintDoc::OnPendingMode() 
{
	if ( CBpaintDoc::MODE::NOT_PENDING != m_PendingMode ) {

		SetDocumentMode( m_PendingMode );

		m_PendingMode = CBpaintDoc::MODE::NOT_PENDING;

	}
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnRenderForeground5050() 
{
	m_bDrawForeground5050 = !m_bDrawForeground5050;

	ForceFullRedraw();

}

void CBpaintDoc::OnUpdateRenderForeground5050(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (m_bDrawForeground5050) ? 1 : 0 );
}

void CBpaintDoc::On2RenderForeground5050() 
{
	OnRenderForeground5050();
}

void CBpaintDoc::OnUpdate2RenderForeground5050(CCmdUI* pCmdUI) 
{
	OnUpdateRenderForeground5050( pCmdUI );
}

void CBpaintDoc::OnRenderCheckerBoardBackdrop() 
{
	m_bUseBackdropSurface = !m_bUseBackdropSurface;

	BuildBackgroundLayersSurface();

	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateRenderCheckerBoardBackdrop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (m_bUseBackdropSurface) ? 1 : 0 );
}

void CBpaintDoc::OnUpdateSpecialCaseOpaqueEdit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( m_bUseBackgroundRendererInEditOnlyViews ? 0 : 1 );
}

void CBpaintDoc::OnSpecialCaseOpaqueEdit() 
{
	m_bUseBackgroundRendererInEditOnlyViews = !m_bUseBackgroundRendererInEditOnlyViews;
	ForceFullRedraw();
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnCompositeMode() 
{
	if ( CBpaintDoc::MODE::COMPOSITE == m_CurrentMode ) {

		m_PendingMode = CBpaintDoc::MODE::NOT_PENDING;

		SetDocumentMode( CBpaintDoc::MODE::DRAWING );

	} else {

		m_PendingMode = CBpaintDoc::MODE::NOT_PENDING;

		SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

	}

	ForceFullRedraw();

}

void CBpaintDoc::OnUpdateCompositeMode(CCmdUI* pCmdUI) 
{
	if ( m_pAnimation ) {

		pCmdUI->Enable( TRUE );

		pCmdUI->SetCheck( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? 1 : 0 );

	} else {

		pCmdUI->Enable( FALSE );

	}
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnCompositeBoundingRects() 
{
	m_dwDoDadFlags ^= CBpaintDoc::DODADS::BOUNDING_RECTS;
	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateCompositeBoundingRects(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( (CBpaintDoc::DODADS::BOUNDING_RECTS & m_dwDoDadFlags) ? 1 : 0 );
}

void CBpaintDoc::OnCompositeExternalRect() 
{
	m_dwDoDadFlags ^= CBpaintDoc::DODADS::EXPAND_BOUNDING_RECT;
	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateCompositeExternalRect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( (CBpaintDoc::DODADS::EXPAND_BOUNDING_RECT & m_dwDoDadFlags) ? 1 : 0 );
}

void CBpaintDoc::OnCompositeOpaque() 
{
	m_dwDoDadFlags ^= CBpaintDoc::DODADS::OPAQUE_MODE;
	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateCompositeOpaque(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( (CBpaintDoc::DODADS::OPAQUE_MODE & m_dwDoDadFlags) ? 1 : 0 );
	
}

void CBpaintDoc::OnCompositeShowHandles() 
{
	m_dwDoDadFlags ^= CBpaintDoc::DODADS::HANDLES;
	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateCompositeShowHandles(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( (CBpaintDoc::DODADS::HANDLES & m_dwDoDadFlags) ? 1 : 0 );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnCompositeDisableImageRender() 
{
	m_dwDoDadFlags ^= CBpaintDoc::DODADS::DISABLE_IMAGE_RENDER;
	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateCompositeDisableImageRender(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( (CBpaintDoc::DODADS::DISABLE_IMAGE_RENDER & m_dwDoDadFlags) ? 1 : 0 );
}

void CBpaintDoc::OnCompositeRenderSelected() 
{
	m_dwDoDadFlags ^= CBpaintDoc::DODADS::RENDER_SELECTED;
	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateCompositeRenderSelected(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( (CBpaintDoc::DODADS::RENDER_SELECTED & m_dwDoDadFlags) ? 1 : 0 );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnCompositeShowLinkpoints() 
{
	m_bRenderLinkPointOverlays = !m_bRenderLinkPointOverlays;

	if ( !m_bRenderLinkPointOverlays ) {

		m_bSetFrameLinkPoint = false;

		m_bSetAnimLinkPoint = false;

		m_pCurrentNamedSpot = 0;

		m_eSetNamedSpotMode = SPOT_EDIT_NONE;

	}

	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateCompositeShowLinkpoints(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );	
	pCmdUI->SetCheck( m_bRenderLinkPointOverlays ? 1 : 0 );
}

void CBpaintDoc::OnCompositeSetAnimMainLink() 
{
	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	m_bSetAnimLinkPoint = !m_bSetAnimLinkPoint;

	m_bSetFrameLinkPoint = false;

	m_nSetFrameLinkPoint = -1;

	if ( m_bSetAnimLinkPoint ) {

		m_bRenderLinkPointOverlays = true;

		ForceFullRedraw();

	}
}

void CBpaintDoc::OnUpdateCompositeSetAnimMainLink(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );	

	pCmdUI->SetCheck( m_bSetAnimLinkPoint ? 1 : 0 );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnCompositeFrameLink( const int nLink ) {

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	if ( m_bSetFrameLinkPoint && (nLink == m_nSetFrameLinkPoint) ) {

		m_bSetFrameLinkPoint = !m_bSetFrameLinkPoint;

	} else {

		m_bSetFrameLinkPoint = true;

	}

	m_bSetAnimLinkPoint = false;

	if ( m_bSetFrameLinkPoint ) {

		m_bRenderLinkPointOverlays = true;

		m_nSetFrameLinkPoint = nLink;

		ForceFullRedraw();

	} else {

		m_nSetFrameLinkPoint = -1;

	}

}

void CBpaintDoc::OnCompositeSetLink1() 
{
	OnCompositeFrameLink( 0 );
}

void CBpaintDoc::OnUpdateCompositeSetLink1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );	

	pCmdUI->SetCheck(
		((0 == m_nSetFrameLinkPoint) && m_bSetFrameLinkPoint) ? 1 : 0 
	);

}

void CBpaintDoc::OnCompositeSetLink2() 
{
	OnCompositeFrameLink( 1 );
}

void CBpaintDoc::OnUpdateCompositeSetLink2(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );	
	
	pCmdUI->SetCheck(
		((1 == m_nSetFrameLinkPoint) && m_bSetFrameLinkPoint) ? 1 : 0 
	);
}

void CBpaintDoc::OnCompositeSetLink3() 
{
	OnCompositeFrameLink( 2 );
}

void CBpaintDoc::OnUpdateCompositeSetLink3(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );	

	pCmdUI->SetCheck(
		((2 == m_nSetFrameLinkPoint) && m_bSetFrameLinkPoint) ? 1 : 0 
	);
}

void CBpaintDoc::OnCompositeSetLink4() 
{
	OnCompositeFrameLink( 3 );
}

void CBpaintDoc::OnUpdateCompositeSetLink4(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );

	pCmdUI->SetCheck(
		((3 == m_nSetFrameLinkPoint) && m_bSetFrameLinkPoint) ? 1 : 0 
	);
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnCompositeXLock() 
{
	m_CompositeModeInfo.m_bXMovementLocked = !m_CompositeModeInfo.m_bXMovementLocked;
}

void CBpaintDoc::OnUpdateCompositeXLock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_CompositeModeInfo.m_bXMovementLocked ? 1 : 0 );
}

void CBpaintDoc::OnCompositeYLock() 
{
	m_CompositeModeInfo.m_bYMovementLocked = !m_CompositeModeInfo.m_bYMovementLocked;
}

void CBpaintDoc::OnUpdateCompositeYLock(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_CompositeModeInfo.m_bYMovementLocked ? 1 : 0 );
}

// ============================================================================

//
//	CBpaintDoc::SetLinkPointMode()
//

bool CBpaintDoc::SetLinkPointMode()
{
	return m_bSetAnimLinkPoint || m_bSetFrameLinkPoint || (SPOT_EDIT_NONE != m_eSetNamedSpotMode);
}

//
//	CBpaintDoc::SetLinkPointPrim()
//

void CBpaintDoc::SetLinkPointPrim( POINT & pointStorage, const CPoint point )
{
	CRect oldRect, newRect;

	CalcLinkPointRect( oldRect, pointStorage );

	pointStorage = point;

	CalcLinkPointRect( newRect, pointStorage );

	AddDirtyRect( oldRect );

	AddDirtyRect( newRect );

	SetModifiedFlag();
	
}

//
//	CBpaintDoc::SetLinkPoint()
//

void CBpaintDoc::SetLinkPoint( const CPoint point )
{
	if ( !m_pAnimation ) {

		return;

	}

	BPT::CFrame * pFrame = m_pAnimation->ActiveFrame();

	if ( !pFrame ) {

		return;

	}

	if ( m_bSetAnimLinkPoint ) {

		SetLinkPointPrim( m_pAnimation->m_Link1, point );

	} else if ( m_bSetFrameLinkPoint ) {

		if ( 0 == m_nSetFrameLinkPoint ) {
	
			SetLinkPointPrim( pFrame->m_Link1, point );
	
		} else if ( 1 == m_nSetFrameLinkPoint ) {
	
			SetLinkPointPrim( pFrame->m_Link2, point );
	
		} else if ( 2 == m_nSetFrameLinkPoint ) {
	
			SetLinkPointPrim( pFrame->m_Link3, point );
	
		} else if ( 3 == m_nSetFrameLinkPoint ) {
	
			SetLinkPointPrim( pFrame->m_Link4, point );
	
		}

	} else if ( (SPOT_EDIT_NONE != m_eSetNamedSpotMode) && m_pCurrentNamedSpot ) {

		POINT spotPos = m_pCurrentNamedSpot->GetLocation();
		
		int updateEvent = CActiveAnimationChange::SPOT_DATA_CHANGE;
		
		if ( SPOT_EDIT_ANIM_SPOT == m_eSetNamedSpotMode ) {

			// Need to see if there is a spot adjustment here
			// ----------------------------------------------------------------
		
			BPT::CNamedSpotAdjustment * pAdjustment = pFrame->FindSpotAdjusterFor( m_pCurrentNamedSpot );

			POINT oldAdjustmentPos = spotPos;
		
			if ( pAdjustment ) {

				oldAdjustmentPos = pAdjustment->GetPosition();

			}

			// Set the main spot position
			// ----------------------------------------------------------------

			m_pCurrentNamedSpot->SetLocation( point );

			SetLinkPointPrim( spotPos, point );

			// ----------------------------------------------------------------

			if ( pAdjustment ) {


				POINT newAdjustmentPos = pAdjustment->GetPosition();

				SetLinkPointPrim( oldAdjustmentPos, newAdjustmentPos );

			}
	
		} else if ( SPOT_EDIT_ADJUSTMENT == m_eSetNamedSpotMode ) {
		
			BPT::CNamedSpotAdjustment * pAdjustment = pFrame->FindSpotAdjusterFor( m_pCurrentNamedSpot );
		
			if ( !pAdjustment ) {
		
				pAdjustment = new BPT::CNamedSpotAdjustment;
		
				if ( pAdjustment ) {
		
					pAdjustment->SetNamedSpot( m_pCurrentNamedSpot );
		
					pFrame->AttachNamedSpotAdjustment( pAdjustment );
		
					updateEvent = CActiveAnimationChange::NEW_SPOT_ADJUSTMENT;
		
				}
		
			}
		
			if ( pAdjustment ) {
		
				POINT delta;
		
				delta.x = point.x - spotPos.x;
				delta.y = point.y - spotPos.y;

				POINT pos = pAdjustment->GetPosition();
		
				pAdjustment->SetDelta( delta );
		
				SetLinkPointPrim( pos, point );

			}
		
		}
		
		// Update the user interface elements
		// ----------------------------------------------------------------
		
		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
		
		UpdateAllViews( 0, updateEvent, &changeInfo );

	}

}

// ============================================================================

//
//	Composite_ClickSelectLayerMode()
//

bool CBpaintDoc::Composite_ClickSelectLayerMode()
{
	return m_bClickSelectCompositeMode;
}

//
//	Composite_ClickSelectLayer()
//

void CBpaintDoc::Composite_ClickSelectLayer(CView * pView, UINT nFlags, CPoint point)
{

	CView * pTestView = (pView) ? pView : m_pTemporaryTrackedView;

	if ( m_pAnimation && pTestView ) {

		// Determine best we can if this is a composite view
		// --------------------------------------------------------------------

		if ( !pTestView->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

			return;

		}

		dib_type * pDib = ((CBpaintView*)pTestView)->GetCanvasDib();

		if ( (!pDib) || (pDib != M_GetCompositeViewDib()) ) {

			return;

		}

		// Do pixel based selection.
		// --------------------------------------------------------------------

		int count = m_pAnimation->LayerCount();

		int checkPixelSize = sizeof( editor_pixel_type );

		for ( int i = (count - 1); 0 <= i; i-- ) {

			BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );
	
			if ( pLayer ) {

				// Need to check visibility then hit test

				if ( IsLayerVisible( i ) ) {

					// Hit test the layer if the edit type
	
					if ( pLayer->HitTest( point.x, point.y, checkPixelSize ) ) {

						Request_LayerChange( i );

						return;
	
					}

				}

			}

		}

		// Do a simple rectangle check if the pixel test didn't work
		// --------------------------------------------------------------------

		for ( i = (count - 1); 0 <= i; i-- ) {

			BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );
	
			if ( pLayer ) {

				// Need to check visibility then hit test

				if ( IsLayerVisible( i ) ) {

					RECT boundingRect;

					if ( pLayer->BoundingRect( boundingRect ) ) {

						if ( PtInRect( &boundingRect, point ) ) {

							Request_LayerChange( i );

							return;

						}

					}

				}

			}

		}

	}

}

// ============================================================================

void 
CBpaintDoc::Composite_OnLButtonDown(CView * pView, UINT nFlags, CPoint point)
{
	m_CompositeModeInfo.m_LButtonDownPoint = point;
	m_CompositeModeInfo.m_LButtonLastPoint = point;
	m_CompositeModeInfo.m_LButtonDown = true;
	m_CompositeModeInfo.m_LastMouseFlags = nFlags;

	if ( SetLinkPointMode() ) {

		SetLinkPoint( point );

	}

#if 1 // BPT 6/2/01

	else if ( Composite_ClickSelectLayerMode() ) {

		Composite_ClickSelectLayer( pView, nFlags, point );

	}

#endif

}

void 
CBpaintDoc::Composite_OnLButtonUp(CView * pView, UINT nFlags, CPoint point)
{
	m_CompositeModeInfo.m_LButtonDown = false;
	m_CompositeModeInfo.m_LastMouseFlags = nFlags;
}

void 
CBpaintDoc::Composite_OnMouseMove(CView * pView, UINT nFlags, CPoint point)
{
	m_CompositeModeInfo.m_LastMouseFlags = nFlags;

	if ( SetLinkPointMode() && m_CompositeModeInfo.m_LButtonDown ) {

		SetLinkPoint( point );

	} else if (
		((MK_CONTROL & nFlags) && m_CompositeModeInfo.m_LButtonDown)
		|| m_CompositeModeInfo.m_RButtonDown // BPT 10/10/02
		
		) {

		CPoint delta;

		if ( m_CompositeModeInfo.m_LButtonDown ) {

			delta = point - m_CompositeModeInfo.m_LButtonLastPoint;

		} else {
		
			delta = point - m_CompositeModeInfo.m_RButtonLastPoint;

		}

		if ( m_CompositeModeInfo.m_bXMovementLocked ) {

			delta.x = 0;

		}

		if ( m_CompositeModeInfo.m_bYMovementLocked ) {

			delta.y = 0;

		}

		Composite_ShiftCurrentLayer( delta.x, delta.y );

		m_CompositeModeInfo.m_LButtonLastPoint = point;
		m_CompositeModeInfo.m_RButtonLastPoint = point;

	}
}

void 
CBpaintDoc::Composite_OnRButtonDown(CView * pView, UINT nFlags, CPoint point)
{
	m_CompositeModeInfo.m_RButtonDownPoint = point;
	m_CompositeModeInfo.m_RButtonLastPoint = point;
	m_CompositeModeInfo.m_RButtonDown = true;
	m_CompositeModeInfo.m_LastMouseFlags = nFlags;

	Composite_ClickSelectLayer( pView, nFlags, point );
}

void 
CBpaintDoc::Composite_OnRButtonUp(CView * pView, UINT nFlags, CPoint point)
{
	m_CompositeModeInfo.m_RButtonDown = false;
	m_CompositeModeInfo.m_LastMouseFlags = nFlags;
}

void 
CBpaintDoc::Composite_OnMButtonDown(CView * pView, UINT nFlags, CPoint point)
{
}

void 
CBpaintDoc::Composite_OnMButtonUp(CView * pView, UINT nFlags, CPoint point)
{
}

void 
CBpaintDoc::Composite_OnCancelMode(CView * pView)
{
	m_CompositeModeInfo.m_LButtonDown = false;
	m_CompositeModeInfo.m_RButtonDown = false;
	m_CompositeModeInfo.m_LastMouseFlags = 0;
}

// ============================================================================

//
//	CBpaintDoc::PickupColor()
//

bool 
CBpaintDoc::PickupColor( const int x, const int y, editor_pixel_type * pOptionalOutValue )
{
	if ( (0 > x) || (0 > y) || (x >= m_CanvasSize.cx) || (y >= m_CanvasSize.cy) ) {

		return false;

	}

	// Do layered read if we can determine the 'tracked' view
	// ------------------------------------------------------------------------

	if ( m_pTemporaryTrackedView && m_pAnimation ) {

		// Determine best we can if this is a composite view
		// --------------------------------------------------------------------

		if ( !m_pTemporaryTrackedView->IsKindOf(RUNTIME_CLASS(CBpaintView)) ) {

			goto EDIT_ONLY;

		}

		dib_type * pDib = ((CBpaintView*)m_pTemporaryTrackedView)->GetCanvasDib();

		if ( !pDib ) {

			goto EDIT_ONLY;

		}

		if ( pDib != M_GetCompositeViewDib() ) {

			goto EDIT_ONLY;

		}

		// Close enough treat the request as layered.
		// --------------------------------------------------------------------

		int count = m_pAnimation->LayerCount();

		int editLayer = m_pAnimation->CurrentLayer();

		bool bResult = false;

		editor_pixel_type value = editor_pixel_type( 0 );

		int checkPixelSize = sizeof( editor_pixel_type );

		for ( int i = (count - 1); 0 <= i; i-- ) {

			if ( editLayer == i ) {

				editor_bitmap_type * pEditorBitmap = m_EditorLogic.GetCurentBitmapPtr();

				if ( pEditorBitmap ) {

					value = *(pEditorBitmap->Iterator( x, y ));

					if ( m_ChromaKey != value ) {

						bResult = true;
						break;

					}

				}

			} else {

				BPT::CLayer * pLayer = m_pAnimation->GetLayer( i );
	
				if ( pLayer ) {

					// Need to check visibility then hit test

					if ( IsLayerVisible( i ) ) {

						// Hit test the layer if the edit type
	
						BPT::CVisualElement::storage_type vValue;
	
						if ( bResult = pLayer->HitTest( x, y, checkPixelSize, &vValue ) ) {
	
							value = (editor_pixel_type)(vValue);
	
							break;
	
						}

					}

				}

			}

		}

		// --------------------------------------------------------------------

		if ( bResult ) {

			if ( pOptionalOutValue ) {
	
				*pOptionalOutValue = value;
	
			}

		} else {

#if 1

			if ( pOptionalOutValue ) {

				*pOptionalOutValue = m_ChromaKey;

			}

			bResult = true;

#endif

		}

		return bResult;

	}

	// Do simplistic editor only read
	// ------------------------------------------------------------------------

EDIT_ONLY:

	editor_bitmap_type * pEditorBitmap = m_EditorLogic.GetCurentBitmapPtr();

	if ( pEditorBitmap ) {

		if ( pOptionalOutValue ) {

			*pOptionalOutValue = *(pEditorBitmap->Iterator( x, y ));

		}

		return true;

	}

#if 1

	if ( pOptionalOutValue ) {

		*pOptionalOutValue = m_ChromaKey;

	}

	return true;

#else

	return false;

#endif

}

bool 
CBpaintDoc::EnsureProperMenu( CWnd * pWnd, const int nIDResource )
{

#if 0

	CMenu * pMenu = pWnd->GetMenu();

	if ( !pMenu ) {

		while ( pWnd = pWnd->GetParent() ) { // ->GetParentFrame() ) {

			if ( pMenu = pWnd->GetMenu() ) {

				break;

			}

		}

		if ( (!pWnd) || (!pMenu) ) {

			return false;

		}

	}

	CMenu * pNew = new CMenu();

	if ( !pNew ) {

		return false;

	}

	if ( FALSE == pNew->LoadMenu( nIDResource ) ) {

		delete pNew;

		return false;

	}

	if ( FALSE == pWnd->SetMenu( pNew ) ) {

		pWnd->SetMenu( pMenu );

		delete pNew;

		return false;

	}

	pMenu->DestroyMenu();

#endif

	return true;
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnShiftDrawCanvasD() 
{
	switch ( m_CurrentMode ) {

	default:
	case CBpaintDoc::MODE::COMPOSITE:
		Composite_ShiftCurrentLayer( 0, 1 );
		break;

	case CBpaintDoc::MODE::DRAWING:
		m_EditorLogic.Shift( 0, 1, GetClearColor() );
		break;

	}
}

void CBpaintDoc::OnUpdateShiftDrawCanvasD(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
}

void CBpaintDoc::OnShiftDrawCanvasL() 
{
	switch ( m_CurrentMode ) {

	default:
	case CBpaintDoc::MODE::COMPOSITE:
		Composite_ShiftCurrentLayer( -1, 0 );
		break;

	case CBpaintDoc::MODE::DRAWING:
		m_EditorLogic.Shift( -1, 0, GetClearColor() );
		break;

	}
}

void CBpaintDoc::OnUpdateShiftDrawCanvasL(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
}

void CBpaintDoc::OnShiftDrawCanvasR() 
{
	switch ( m_CurrentMode ) {

	default:
	case CBpaintDoc::MODE::COMPOSITE:
		Composite_ShiftCurrentLayer( 1, 0 );
		break;

	case CBpaintDoc::MODE::DRAWING:
		m_EditorLogic.Shift( 1, 0, GetClearColor() );
		break;

	}
}

void CBpaintDoc::OnUpdateShiftDrawCanvasR(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
}

void CBpaintDoc::OnShiftDrawCanvasU() 
{
	switch ( m_CurrentMode ) {

	default:
	case CBpaintDoc::MODE::COMPOSITE:
		Composite_ShiftCurrentLayer( 0, -1 );
		break;

	case CBpaintDoc::MODE::DRAWING:
		m_EditorLogic.Shift( 0, -1, GetClearColor() );
		break;

	}
}

void CBpaintDoc::OnUpdateShiftDrawCanvasU(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnHflipDrawCanvas() 
{
	SetDocumentMode( CBpaintDoc::MODE::DRAWING );
	m_EditorLogic.HFlip();
}

void CBpaintDoc::OnUpdateHflipDrawCanvas(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsDrawingModePossible() );
}

void CBpaintDoc::OnVflipDrawCanvas() 
{
	SetDocumentMode( CBpaintDoc::MODE::DRAWING );
	m_EditorLogic.VFlip();
}

void CBpaintDoc::OnUpdateVflipDrawCanvas(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( IsDrawingModePossible() );
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::Composite_ShiftCurrentLayer()
//

void
CBpaintDoc::Composite_ShiftCurrentLayer( const int dx, const int dy )
{

	// Check for the quick bail out conditions
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	BPT::CLayer * pLayer = m_pAnimation->ActiveLayer();
	
	if ( !pLayer ) {
	
		return;
	
	}

	if ( !pLayer->HasVisualElement() ) {

		return;

	}

	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

	// Where was the cel before the operation.
	// ------------------------------------------------------------------------

	CRect beforeRect;

	pLayer->BoundingRect( beforeRect );

	// Peform the operation
	// ------------------------------------------------------------------------

	pLayer->Shift( CPoint( dx, dy ) );

	// Where is the cel now?
	// ------------------------------------------------------------------------

	CRect afterRect;

	pLayer->BoundingRect( afterRect );

#if 0

	ForceFullRedraw();

#else

	// Need to adjust the rects for the doodads rendered with the cels.
	// ------------------------------------------------------------------------

	beforeRect.InflateRect( 4, 4 );
	afterRect.InflateRect( 4, 4 );

	// ------------------------------------------------------------------------

	if ( (32 > abs(dx)) && (32 > abs(dy)) ) {

		CRect combined;
	
		combined.UnionRect( &afterRect, &beforeRect );
	
		AddDirtyRect( combined );

	} else {

		RECT backgroundRects[ 4 ];
	
		int backgroundCount = BPT::ReturnExternalClipRects( 
			backgroundRects, &beforeRect, &afterRect
		);
	
		AddDirtyRect( afterRect );
	
		for ( int i = 0; i < backgroundCount; i++ ) {
	
			AddDirtyRect( backgroundRects[ i ] );
	
		}

	}

#endif

	// ------------------------------------------------------------------------

	SetModifiedFlag();

}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnPlayAnimNextFrame() 
{
	CWnd * pWnd = AfxGetMainWnd();

	if ( pWnd ) {

		pWnd->SendMessage( WM_COMMAND, ID_FRAME_NEXT );

	}
}

void CBpaintDoc::OnPlayAnimStart() 
{
	m_dwSavedDoDadFlags = m_dwDoDadFlags;

	m_dwDoDadFlags = 0;

	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );
}

void CBpaintDoc::OnPlayAnimStop() 
{
	m_dwDoDadFlags = m_dwSavedDoDadFlags;

	ForceFullRedraw();
}

bool CBpaintDoc::EnsureAnimationModeIsHalted()
{
	CWnd * pWnd = AfxGetMainWnd();

	if ( pWnd->IsKindOf( RUNTIME_CLASS(CMainFrame) ) ) {

		((CMainFrame *)pWnd)->StopPlayingAnimation();

	}

	return true;
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnFrameProperties() 
{
	// error check
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	BPT::CFrame * pFrame = m_pAnimation->ActiveFrame();

	if ( !pFrame ) {

		return;

	}

	// Prepare the dialog info
	// ------------------------------------------------------------------------

	CFramePropertiesDlg dlg( AfxGetMainWnd() );

	dlg.m_strNote = pFrame->GetNote();

	dlg.m_nLink1X = pFrame->m_Link1.x;
	dlg.m_nLink1Y = pFrame->m_Link1.y;
	dlg.m_nLink2X = pFrame->m_Link2.x;
	dlg.m_nLink2Y = pFrame->m_Link2.y;
	dlg.m_nLink3X = pFrame->m_Link3.x;
	dlg.m_nLink4X = pFrame->m_Link3.y;
	dlg.m_nLink3Y = pFrame->m_Link4.x;
	dlg.m_nLink4Y = pFrame->m_Link4.y;

	// ------------------------------------------------------------------------

	if ( IDOK == dlg.DoModal() ) {

		// Set the frame properties
		// --------------------------------------------------------------------

		pFrame->SetNote( dlg.m_strNote );
	
		pFrame->m_Link1.x = dlg.m_nLink1X;
		pFrame->m_Link1.y = dlg.m_nLink1Y;
		pFrame->m_Link2.x = dlg.m_nLink2X;
		pFrame->m_Link2.y = dlg.m_nLink2Y;
		pFrame->m_Link3.x = dlg.m_nLink3X;
		pFrame->m_Link3.y = dlg.m_nLink4X;
		pFrame->m_Link4.x = dlg.m_nLink3Y;
		pFrame->m_Link4.y = dlg.m_nLink4Y;

		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

	}

}

void CBpaintDoc::OnUpdateFrameProperties(CCmdUI* pCmdUI) 
{
	if ( m_pAnimation ) {

		pCmdUI->Enable( (0 != m_pAnimation->ActiveFrame()) );

	} else {

		pCmdUI->Enable( FALSE );

	}
	
}

// ----------------------------------------------------------------------------


void CBpaintDoc::OnFrameCompositeOps() 
{
	// Validate showcase state for the operation
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// Fill in the dialog information members
	// ------------------------------------------------------------------------

	if ( CompositeFramesOperationDialogHandler( AfxGetMainWnd() ) ) {

		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

		ForceFullRedraw();

	}

}

void CBpaintDoc::OnUpdateFrameCompositeOps(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::OnCopyFrames() 
{
	// Validate showcase state for the operation
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// Fill in the dialog information members
	// ------------------------------------------------------------------------

	if ( CopyFramesDialogHandler( AfxGetMainWnd() ) ) {

		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

		ForceFullRedraw();

	}

}

void CBpaintDoc::OnUpdateCopyFrames(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnLayerExportFrames() 
{
	// Validate showcase state for the operation
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// Fill in the dialog information members
	// ------------------------------------------------------------------------

	if ( ExportFramesDialogHandler( AfxGetMainWnd() ) ) {

		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

		ForceFullRedraw();

	}

}

// ============================================================================

void CBpaintDoc::OnUpdateLayerExportFrames(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::OnLayerImportFrames() 
{
	// Validate showcase state for the operation
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// Fill in the dialog information members
	// ------------------------------------------------------------------------

	if ( ImportFramesDialogHandler( AfxGetMainWnd() ) ) {

		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

		ForceFullRedraw();

	}
}

void CBpaintDoc::OnUpdateLayerImportFrames(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

// ============================================================================

void CBpaintDoc::OnTranslateFrameLayers() 
{
	// Validate showcase state for the operation
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// Fill in the dialog information members
	// ------------------------------------------------------------------------

	if ( TranslateFrameLayersDialogHandler( AfxGetMainWnd() ) ) {

		// Update the user interface elements
		// --------------------------------------------------------------------

		CActiveAnimationChange changeInfo;
			
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
	
		UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );

		SetModifiedFlag();

		ForceFullRedraw();

	}

}

void CBpaintDoc::OnUpdateTranslateFrameLayers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

// ============================================================================

// ============================================================================

void CBpaintDoc::OnAnimLoadBackdrop() 
{
	// Error check.
	// ------------------------------------------------------------------------

	if ( !m_pAnimation ) {

		return;

	}

	// What filename
	// ------------------------------------------------------------------------

	CFileDialog opf( 
		TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_LONGNAMES, 
		_T(
			"Bitmap Files (*.bmp)|*.bmp|"
			"JPEG Files (*.jpg)|*.jpg|"
			"All Supported Formats (*.jpg,*.bmp)|*.jpg;*.bmp|"
			"All Files (*.*)|*.*|"
			"|"
		)
		,AfxGetMainWnd()
	);

	if ( IDOK != opf.DoModal() ) {

		return;

	}

	// Do something with the filename...
	// ------------------------------------------------------------------------

	SetModifiedFlag();

	if ( !m_pAnimation->LoadBackdrop( opf.GetFileName(), m_CanvasSize ) ) {

		MessageBox( 
			AfxGetMainWnd()->GetSafeHwnd(), "Unable to load backdrop!", 
			"Error!", MB_ICONERROR | MB_OK 
		);

		m_bShowAnimationBackdrop = false;

		ForceFullRedraw();

		return;

	}

	m_bShowAnimationBackdrop = true;

	ForceFullRedraw();

	// Force a view to have the focus.
	// ------------------------------------------------------------------------

#if 1

	POSITION pos = GetFirstViewPosition();

	while (pos != NULL) {

		CView * pView = GetNextView( pos );

		if ( pView ) {

			CFrameWnd * pFrame = pView->GetParentFrame();
			
			if ( pFrame ) {
				
				pFrame->ActivateFrame();

			}

		}

	}

#endif
	
}

void CBpaintDoc::OnUpdateAnimLoadBackdrop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( 0 != m_pAnimation );
}

void CBpaintDoc::OnAnimClearBackdrop() 
{
	m_bShowAnimationBackdrop = false;

	if ( m_pAnimation ) {

		m_pAnimation->DestroyBackdrop();

	}

	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateAnimClearBackdrop(CCmdUI* pCmdUI) 
{
	if ( !m_pAnimation ) {

		pCmdUI->Enable( FALSE );

	} else {

		if ( m_pAnimation->HasBackdrop() ) {

			pCmdUI->Enable( TRUE );

		} else {

			pCmdUI->Enable( FALSE );

		}

	}
}

void CBpaintDoc::OnAnimShowBackdrop() 
{
	m_bShowAnimationBackdrop = !m_bShowAnimationBackdrop;

	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateAnimShowBackdrop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bShowAnimationBackdrop ? 1 : 0 );

	if ( !m_pAnimation ) {

		pCmdUI->Enable( FALSE );

	} else {

		if ( m_pAnimation->HasBackdrop() ) {

			pCmdUI->Enable( TRUE );

		} else {

			pCmdUI->Enable( FALSE );

		}

	}
}

// ============================================================================

void CBpaintDoc::OnHackGridThumbnails() 
{
	// ------------------------------------------------------------------------

	m_bEnableRenderThumbnailsHack = !m_bEnableRenderThumbnailsHack;

	// Force a repaint of the grid views
	// ------------------------------------------------------------------------

	CActiveAnimationChange changeInfo;
		
	changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
	changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
	changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
	changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

	UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );
}

void CBpaintDoc::OnUpdateHackGridThumbnails(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pAnimation) ? TRUE : FALSE );
	pCmdUI->SetCheck( m_bEnableRenderThumbnailsHack ? 1 : 0 );
}

// ============================================================================

void CBpaintDoc::OnAllAnimationFrameNotes() 
{
	if ( AnimationFrameNotesDialogHandler( AfxGetMainWnd() ) ) {

		SetModifiedFlag();

	}

	// Force a repaint of the grid views
	// ------------------------------------------------------------------------

	CActiveAnimationChange changeInfo;
		
	changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
	changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
	changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
	changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;

	UpdateAllViews( 0, CActiveAnimationChange::NEW_ANIMATION, &changeInfo );
}

void CBpaintDoc::OnUpdateAllAnimationFrameNotes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pAnimation) ? TRUE : FALSE );
}


void CBpaintDoc::OnHackTransparentThumbnailScale() 
{
	m_bUseMoreAccurateThumbnailRenderer = !m_bUseMoreAccurateThumbnailRenderer;
}

void CBpaintDoc::OnUpdateHackTransparentThumbnailScale(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pAnimation) ? TRUE : FALSE );
	pCmdUI->SetCheck( m_bUseMoreAccurateThumbnailRenderer ? 1 : 0 );
}

void CBpaintDoc::OnShowcaseChangeCanvas() 
{
	// ------------------------------------------------------------------------

	if ( !GetSizeDialog(
		AfxGetMainWnd(), "Choose canvas size!", m_CanvasSize, &CSize( 32767, 32767 ), "SizeCanvasDlg", true ) ) {
	
		return ;
	
	}

	// setup the canvas internals
	// ------------------------------------------------------------------------

	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

	if ( !ChangeCanvasSizeInternals( m_CanvasSize ) ) {

		AfxMessageBox( "Unable to setup canvas internals, you should try to save NOW!!!", MB_ICONERROR | MB_OK );

		return;

	}

	ForceFullRedraw();

	// Redo the UI layout to rebuild the various canvas related elements.
	// ------------------------------------------------------------------------

	AfxGetMainWnd()->PostMessage( WM_COMMAND, GLOBAL_GetSettingInt( "LastUILayout", ID_UI_LAYOUT_6) );

}

void CBpaintDoc::OnUpdateShowcaseChangeCanvas(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_pShowcase ? TRUE : FALSE );
}

void CBpaintDoc::OnBrushTextToBrush() 
{
	if ( TextToBrushDialogHandler( AfxGetMainWnd() ) ) {

		AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_DTOOL_FREEHAND_SINGLE, 0 );

	}
}

void CBpaintDoc::OnUpdateBrushTextToBrush(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pAnimation) ? TRUE : FALSE );
}

// ============================================================================

//
//	CBpaintDoc::OnAnimSpecialCaptureFont()
//

void CBpaintDoc::OnAnimSpecialCaptureFont() 
{
	// ------------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// ------------------------------------------------------------------------

	CFontDialog dlg( NULL, CF_EFFECTS | CF_SCREENFONTS, NULL, AfxGetMainWnd() );

	if ( IDOK != dlg.DoModal() ) {

		return;

	}

	LOGFONT fontSettings;

	dlg.GetCurrentFont( &fontSettings );

	// Build the name of the animation
	// ------------------------------------------------------------------------

	CString fontName;
	
	fontName.Format( 
		"Font '%s' style '%s' size %d", 
		dlg.GetFaceName(), 
		dlg.GetStyleName(), 
		dlg.GetSize() 
	);

	// Need to create an animation and select it!
	// ------------------------------------------------------------------------

	BPT::CAnimation * pAnim = m_pShowcase->NewAnimation( 256, 1 );

	if ( pAnim ) {

		SetModifiedFlag();

		// Set the name and select the animation
		// --------------------------------------------------------------------

		pAnim->SetName( fontName );

		Request_AnimationChange( pAnim );

		// Start capturing
		// --------------------------------------------------------------------

		CFont theFont;

		if ( TRUE == theFont.CreateFontIndirect( &fontSettings ) ) {
	
			// Create the dib that the text will be rendered into
			// ----------------------------------------------------------------
	
			dib_type textDib;
	
			if ( !textDib.BPTDIB_DEBUG_CREATE( m_CanvasSize.cx, m_CanvasSize.cy, 0 ) ) {
	
				TRACE( "Failed to create dib section %dx%d\n", m_CanvasSize.cx, m_CanvasSize.cy  );
	
				AfxGetMainWnd()->MessageBox( 
					"Failed to create temporary dib?", "Error!", MB_ICONERROR | MB_OK
				);
	
				return ;
	
			}
	
			// Clear the dib section with black text should be rendered in white
			// ----------------------------------------------------------------
	
			dib_type::bitmap_type * pDibBitmap = textDib.GetBitmapPtr();
	
			if ( !pDibBitmap ) {
		
				AfxGetMainWnd()->MessageBox( 
					"Unable to get dib bitmap?", "Error!", MB_ICONERROR | MB_OK
				);

				return ;
	
			}
	
			// Create our 'capture' canvas
			// ----------------------------------------------------------------

			editor_bitmap_type textCanvasBitmap;
	
			if ( !textCanvasBitmap.Create( m_CanvasSize.cx, m_CanvasSize.cy ) ) {

				TRACE( "Failed to create capture bitmap %dx%d\n", m_CanvasSize.cx, m_CanvasSize.cy  );
	
				AfxGetMainWnd()->MessageBox( 
					"Failed to create temporary bitmap?", "Error!", MB_ICONERROR | MB_OK
				);
	
				return ;
			}

			// Use the center of the canvas as the place to render the text
			// ----------------------------------------------------------------

			int cx = m_CanvasSize.cx / 2;
			int cy = m_CanvasSize.cy / 2;

			pAnim->m_Link1.x = cx;
			pAnim->m_Link1.y = cy;

			// Okay use GDI to render the text
			// ----------------------------------------------------------------

			{

				CWaitCursor();

				// Create the DC for rendering
				// ------------------------------------------------------------
	
				CDC memDC;
		
				if ( !memDC.CreateCompatibleDC( NULL ) ) {
	
					AfxMessageBox( "Failed to create rendering CDC!", MB_ICONERROR | MB_OK );
	
					return ;
	
				}
		
				// Select the bitmap as the render surface
				// ------------------------------------------------------------
	
				CBitmap* pOldBitmap = memDC.SelectObject(
					CBitmap::FromHandle( textDib.GetHBitmap() )
				);
		
				// Select the font and device context settings for rendering
				// ------------------------------------------------------------

				CFont * pOldFont = memDC.SelectObject( &theFont );
				memDC.SetBkMode( TRANSPARENT );
				memDC.SetTextColor( RGB( 255, 255, 255 ) );
				memDC.SetTextAlign( TA_BASELINE | TA_NOUPDATECP );

				// Okay now for each letter draw and then capture
				// ------------------------------------------------------------

				char letter[ 2 ];

				letter[ 1 ] = 0;
	
				CRect captureRect, boundingRectUnion;

				boundingRectUnion.SetRectEmpty();

				captureRect.SetRectEmpty();

				for ( int i = 0; i < 256; i++ ) {

					letter[ 0 ] = (char)i;

					// Clear the capture buffers
					// --------------------------------------------------------
	
					if ( captureRect.IsRectEmpty() ) {

						textCanvasBitmap.ClearBuffer( m_ChromaKey );

						pDibBitmap->ClearBuffer( 0 );

					} else {

						BPT::T_SolidRectPrim(
							textCanvasBitmap, captureRect, 
							m_ChromaKey, paint_copy_op_type()
						);

						BPT::T_SolidRectPrim(
							*pDibBitmap, captureRect, 
							0, composite_copy_op_type()
						);

					}

					// Render the text
					// --------------------------------------------------------

					memDC.TextOut( cx, cy, letter, 1 );
	
					// Transfer the dib contents to the editor bitmap surface
					// --------------------------------------------------------

					BPT::TSingleColorTransparentSrcTransferROP<
						editor_pixel_type, dib_type::bitmap_type::pixel_type
					> top( editor_pixel_type( m_Color_L ), dib_type::bitmap_type::pixel_type( 0 ) );
			
					BPT::T_Blit( textCanvasBitmap, 0, 0, *pDibBitmap, top );

					// Get the layer and capture the
					// --------------------------------------------------------

					BPT::CLayer * pLayer = m_pAnimation->ActiveLayer();
				
					if ( !pLayer ) {

						TRACE( "Unable to get frame %d active layer\n", i + 1 );
				
						AfxMessageBox( "Failed to get layer for a frame?", MB_ICONERROR | MB_OK );

						goto ABRUPT_END;
				
					}

					// --------------------------------------------------------

					BPT::T_FindBoundingRect(
						captureRect, textCanvasBitmap, BPT::TIsNotValue<editor_pixel_type>( m_ChromaKey ), 0
					);
				
					if ( captureRect.IsRectEmpty() ) {
				
						// clear the visual element for the current frame/layer
						// ----------------------------------------------------
					
						pLayer->SetVisualElement( 0, &CPoint( 0, 0 ) );
				
					} else {

						// ----------------------------------------------------

						if ( boundingRectUnion.IsRectEmpty() ) {

							boundingRectUnion = captureRect;

						} else {

							boundingRectUnion |= captureRect;

						}
				
						// Ask the showcase to create us a new visual element
						// --------------------------------------------------------------------
				
						BPT::CVisualElement *pVisualElement = m_pShowcase->CreateVisualElementFromSurface(
							textCanvasBitmap, m_ChromaKey, &captureRect
						);
					
						if ( !pVisualElement ) {
					
							TRACE( "Unable to create visual element for frame %d\n", i + 1 );

							AfxMessageBox( "Failed to create visual element?", MB_ICONERROR | MB_OK );

							goto ABRUPT_END;
					
						}
				
						// Replace the visual element for the current frame/layer
						// --------------------------------------------------------------------
					
						pLayer->SetVisualElement( pVisualElement, &CPoint( 0, 0 ) );
				
						// Need to manage the thumbnail here somehow!
						// --------------------------------------------------------------------
				
					}

					// Force an update and then switch frames
					// --------------------------------------------------------

					ForceFullRedraw();

					Request_NextFrame();

				}

				// Put the dc back the way it was for proper disposal and were done
				// ------------------------------------------------------------

			ABRUPT_END:
	
				memDC.SelectObject( pOldFont );
				memDC.SelectObject( pOldBitmap );

				// Set the first frames link point information
				// ------------------------------------------------------------

				BPT::CFrame * pFrame = pAnim->GetFramePtr( 0 );

				if ( pFrame ) {

					pFrame->m_Link1.x = boundingRectUnion.left;
					pFrame->m_Link1.y = boundingRectUnion.top;

					pFrame->m_Link2.x = boundingRectUnion.right;
					pFrame->m_Link2.y = boundingRectUnion.top;

					pFrame->m_Link3.x = boundingRectUnion.right;
					pFrame->m_Link3.y = boundingRectUnion.bottom;

					pFrame->m_Link4.x = boundingRectUnion.left;
					pFrame->m_Link4.y = boundingRectUnion.bottom;

				}

				m_bRenderLinkPointOverlays = true;

				ForceFullRedraw();
			}
	
		} else {

			AfxGetMainWnd()->MessageBox( 
				"Failed to create CFont from LOGFONT structure?", "Error!", MB_ICONERROR | MB_OK
			);

		}

	} else {

		AfxGetMainWnd()->MessageBox( 
			"Failed to create anim?", "Error!", MB_ICONERROR | MB_OK
		);

	}

	
}

// ============================================================================

void CBpaintDoc::OnSpecialImediateSilentRender() 
{
	m_bImediateUpdatesForSilentRender = !m_bImediateUpdatesForSilentRender;
}

void CBpaintDoc::OnUpdateSpecialImediateSilentRender(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bImediateUpdatesForSilentRender ? 1 : 0 );
}

// ============================================================================

//
//	CBpaintDoc::StoreCurrentLayoutAs()
//

void CBpaintDoc::StoreCurrentLayoutAs( const char * name )
{

	// Bail if there isn't an app object ( should be impossible )
	// ------------------------------------------------------------------------

	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
	
	if ( !pApp ) {

		return;

	}

	// Generate a list of all the current frames/views
	// ------------------------------------------------------------------------

	typedef TDocActiveFramesAndViewsInfo<CBpaintDoc> info_helper;

	info_helper::output_map_type * pMap = info_helper::Generate( this );

	if ( pMap ) {

		info_helper::output_map_type::iterator it = pMap->begin();

		int frameCounter = 0;
	
		while ( it != pMap->end() ) {

			CFrameWnd * pFrameWnd = it->first;

			// Generate the section name
			// ------------------------------------------------------------

			CString sectionName;

			sectionName.Format( "%s.frame%d", name, frameCounter );

			++frameCounter;

			// Store off the frame position
			// ------------------------------------------------------------

			pApp->StoreFrameWindowSettings( pFrameWnd, sectionName );

			// Figure out the type of Frame and get the document template used 
			// to create the frame & views. (All frame window types!!!)
			// ----------------------------------------------------------------

			CDocTemplate * pDocTemplate = 0;

			if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) ) {

				CChildFrame * pChildFrame = (CChildFrame *)pFrameWnd;

				pDocTemplate = pChildFrame->GetDocTemplate();

				pChildFrame->StoreInternalSettings( sectionName, "Internal" );

			} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CGenericMDIChildFrame ) ) ) {

				CGenericMDIChildFrame * pGenericMDIChildFrame = (CGenericMDIChildFrame *)pFrameWnd;

				pDocTemplate = pGenericMDIChildFrame->GetDocTemplate();

				pGenericMDIChildFrame->StoreInternalSettings( sectionName, "Internal" );

			} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CPopFrameWnd ) ) ) {

				CPopFrameWnd * pPopFrameWnd = (CPopFrameWnd *)pFrameWnd;

				pDocTemplate = pPopFrameWnd->GetDocTemplate();

				pPopFrameWnd->StoreInternalSettings( sectionName, "Internal" );

			} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CSimpleSplitFrame ) ) ) {

				CSimpleSplitFrame * pSimpleSplitFrame = (CSimpleSplitFrame *)pFrameWnd;

				pDocTemplate = pSimpleSplitFrame->GetDocTemplate();

				pSimpleSplitFrame->StoreInternalSettings( sectionName, "Internal" );

			} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CWindowClipFrmWnd ) ) ) {

				CWindowClipFrmWnd * pWindowClipFrame = (CWindowClipFrmWnd *)pFrameWnd;

				pDocTemplate = pWindowClipFrame->GetDocTemplate();

				pWindowClipFrame->StoreInternalSettings( sectionName, "Internal" );

			} else {
				
				// Other types go here...

			}

			// For any view types that have settings save them
			// ----------------------------------------------------------------

			info_helper::view_collection * pViewCollection = it->second;

			if ( pViewCollection ) {

				info_helper::view_collection::iterator vit = pViewCollection->begin();

				int viewNumber = 0;

				while ( vit != pViewCollection->end() ) {

					if ( (*vit)->IsKindOf( RUNTIME_CLASS( CBpaintView ) ) ) {

						CString baseEntry;

						baseEntry.Format( "View%d", viewNumber );

						((CBpaintView *)(*vit))->StoreInternalSettings(
							sectionName, baseEntry
						);

						++viewNumber;

					} else {

						// Other types will go here

					}

					++vit;

				}

			}

			// If we were able to determine the document type the store off
			// the data into the layout section.
			// ----------------------------------------------------------------

			if ( pDocTemplate ) {

				// Store off the document template used
				// ------------------------------------------------------------

				sectionName += "DocTempID";

				GLOBAL_PutSettingInt( 
					sectionName, 
					pApp->GetDocTemplateStorageID(pDocTemplate), 
					name
				);

			}

			// ----------------------------------------------------------------

			++it;

		}

		// Okay write out the number of things saved.
		// --------------------------------------------------------------------

		GLOBAL_PutSettingInt( "FrameWindowCount", frameCounter, name );

		info_helper::Destroy( pMap );

	}

}

//
//	CBpaintDoc::LoadCustomLayoutFrom()
//

bool CBpaintDoc::LoadCustomLayoutFrom( const char * name )
{

	// Basic error checking
	// ------------------------------------------------------------------------

	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );
	
	if ( !pApp ) {

		return false;

	}

	// Make sure we have stored something before loading...
	// ------------------------------------------------------------------------

	int frameWindowCount = GLOBAL_GetSettingInt( "FrameWindowCount", 0, name );

	if ( !frameWindowCount ) {

		return false;

	}

	// Make sure that the document doesn't get destroyed while
	// all of it's current views/frames are destroyed.
	// ------------------------------------------------------------------------

	BOOL bSaveAutoDelete = m_bAutoDelete;

	m_bAutoDelete = FALSE;

	// Destroy the current frames/views
	// ------------------------------------------------------------------------

	typedef TDocActiveFramesAndViewsInfo<CBpaintDoc> info_helper;

	info_helper::output_map_type * pMap = info_helper::Generate( this );

	if ( pMap ) {

		info_helper::output_map_type::iterator it = pMap->begin();
	
		while ( it != pMap->end() ) {
	
			(it->first)->DestroyWindow();
	
			++it;

		}

	}

	// Okay let's get to creating the frame windows
	// ------------------------------------------------------------------------

	bool bError = false;

	int createdCount = 0;

	typedef std::pair< CFrameWnd *, CString > frame_window_entry;

	typedef std::list< frame_window_entry > frame_window_collection;

	frame_window_collection frameWindowsCollection;

	for ( int frameCounter = 0; frameCounter < frameWindowCount; frameCounter++ ) {
	
		// Generate the section name
		// --------------------------------------------------------------------

		CString sectionName;

		sectionName.Format( "%s.frame%d", name, frameCounter );

		// Store off the document template used
		// ------------------------------------------------------------

		CString docTypeEntry = sectionName + "DocTempID";

		int docType = GLOBAL_GetSettingInt( docTypeEntry, 0, name );

		CDocTemplate * pDocTemplate = pApp->GetDocTemplateFromStorageID( docType );

		if ( pDocTemplate ) {

			CFrameWnd * pFrameWnd = GLOBAL_CreateNewWindow( pDocTemplate, this );

			if ( !pFrameWnd ) {

				bError = true;

			} else {

				++createdCount;

				frameWindowsCollection.push_back(
					frame_window_entry( pFrameWnd, sectionName )
				);

			}

		} else {

			bError = true;

		}

	}

	// Destroy our 1st helper 'map'
	// ------------------------------------------------------------------------

	if ( pMap ) {

		info_helper::Destroy( pMap );

	}

	// Create another helper 'map' to restore view settings
	// ------------------------------------------------------------------------

	pMap = info_helper::Generate( this );

	// Restore the 'view' saved internal settings
	// ------------------------------------------------------------------------

	frame_window_collection::iterator it = frameWindowsCollection.begin();

	while ( it != frameWindowsCollection.end() ) {

		CFrameWnd * pFrameWnd = it->first;

		// restore the frame position
		// --------------------------------------------------------------------

		pApp->RestoreFrameWindowSettings( pFrameWnd, it->second );

		// Restore the frame internal settings
		// --------------------------------------------------------------------

		if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) ) {

			CChildFrame * pChildFrame = (CChildFrame *)pFrameWnd;

			pChildFrame->RestoreInternalSettings( it->second, "Internal" );

		} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CGenericMDIChildFrame ) ) ) {

			CGenericMDIChildFrame * pGenericMDIChildFrame = (CGenericMDIChildFrame *)pFrameWnd;

			pGenericMDIChildFrame->RestoreInternalSettings( it->second, "Internal" );

		} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CPopFrameWnd ) ) ) {

			CPopFrameWnd * pPopFrameWnd = (CPopFrameWnd *)pFrameWnd;

			pPopFrameWnd->RestoreInternalSettings( it->second, "Internal" );

		} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CSimpleSplitFrame ) ) ) {

			CSimpleSplitFrame * pSimpleSplitFrame = (CSimpleSplitFrame *)pFrameWnd;

			pSimpleSplitFrame->RestoreInternalSettings( it->second, "Internal" );

		} else if ( pFrameWnd->IsKindOf( RUNTIME_CLASS( CWindowClipFrmWnd ) ) ) {

			CWindowClipFrmWnd * pWindowClipFrame = (CWindowClipFrmWnd *)pFrameWnd;

			pWindowClipFrame->RestoreInternalSettings( it->second, "Internal" );

		} else {

			// Other types go here...

		}

		// If we can view info then try to restore the info for them.
		// --------------------------------------------------------------------

		if ( pMap ) {

			info_helper::output_map_type::iterator mapIT = pMap->find( it->first );

			if ( mapIT != pMap->end() ) {

				info_helper::view_collection * pViewCollection = mapIT->second;
	
				if ( pViewCollection ) {
	
					info_helper::view_collection::iterator vit = pViewCollection->begin();
	
					int viewNumber = 0;
	
					while ( vit != pViewCollection->end() ) {
	
						if ( (*vit)->IsKindOf( RUNTIME_CLASS( CBpaintView ) ) ) {
	
							CString baseEntry;
	
							baseEntry.Format( "View%d", viewNumber );
	
							((CBpaintView *)(*vit))->RestoreInternalSettings(
								it->second, baseEntry
							);
	
							++viewNumber;
	
						} else {

							// Other types will go here

						}
	
						++vit;
	
					}
	
				}

			}

		}

		++it;

	}

	// Destroy our 2nd helper 'map'
	// ------------------------------------------------------------------------

	if ( pMap ) {

		info_helper::Destroy( pMap );

	}

	// Restore the auto delete setting and we're done!
	// ------------------------------------------------------------------------

	m_bAutoDelete = bSaveAutoDelete;

	// If there was an error put up a message box etc...
	// ------------------------------------------------------------------------

	if ( bError ) {

		AfxGetMainWnd()->MessageBox( 
			"A frame window must have failed construction?", "Error!",
			MB_ICONERROR | MB_OK 
		);

		return false;

	}

	return true;

}

// ============================================================================

void CBpaintDoc::OnUiStoreLayout0() 
{
	StoreCurrentLayoutAs( "custom.layout.a" );

	GLOBAL_PutSettingInt( "LastUILayout", ID_UI_LAYOUT_0 );
}

void CBpaintDoc::OnUiStoreLayout8() 
{
	StoreCurrentLayoutAs( "custom.layout.b" );

	GLOBAL_PutSettingInt( "LastUILayout", ID_UI_LAYOUT_8 );
}

void CBpaintDoc::OnUiStoreLayout9() 
{
	StoreCurrentLayoutAs( "custom.layout.c" );

	GLOBAL_PutSettingInt( "LastUILayout", ID_UI_LAYOUT_9 );
}

// ============================================================================

void CBpaintDoc::OnNewWindowClip() 
{
	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		CFrameWnd * pWindowClip = GLOBAL_CreateNewWindow(
			pApp->GetWindowClipDocTemplate(), this
		);

		// Should there be a default position for this thing?

	}

}

void CBpaintDoc::OnNewMdiAnimList() 
{
	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		CFrameWnd * pWindow = GLOBAL_CreateNewWindow(
			pApp->GetShowcaseAnimsDocTemplate(), this
		);

		// Should there be a default position for this thing?

	}
}

void CBpaintDoc::OnNewMdiAnimSpots() 
{
	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		CFrameWnd * pWindow = GLOBAL_CreateNewWindow(
			pApp->GetAnimationSpotsDocTemplate(), this
		);

		// Should there be a default position for this thing?

	}
}

// ============================================================================

void CBpaintDoc::OnNewMdiVeInfo() 
{
	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		CFrameWnd * pWindow = GLOBAL_CreateNewWindow(
			pApp->GetVisualElementInfoViewDocTemplate(), this
		);

		// Should there be a default position for this thing?

	}
}

void CBpaintDoc::OnUpdateNewMdiVeInfo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

void CBpaintDoc::OnNewPopupVeInfo() 
{
	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		CFrameWnd * pWindow = GLOBAL_CreateNewWindow(
			pApp->GetPopupVisualElementInfoViewDocTemplate(), this
		);

		// Should there be a default position for this thing?

	}
}

void CBpaintDoc::OnUpdateNewPopupVeInfo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

// ============================================================================


void CBpaintDoc::OnNewPopupAnimSpots() 
{
	CBpaintApp * pApp = (CBpaintApp *)AfxGetApp();

	if ( pApp ) {

		CFrameWnd * pFrame = GLOBAL_CreateNewWindow(
			pApp->GetPopupAnimSpotsDocTemplate(), this
		);

		// Should there be a default position for this thing?

	}
}

void CBpaintDoc::OnUpdateNewPopupAnimSpots(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( TRUE );
}

// ============================================================================

void CBpaintDoc::OnRopMatte() 
{
	m_nBrushPaintROP = ID_ROP_MATTE;
}

void CBpaintDoc::OnUpdateRopMatte(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (ID_ROP_MATTE == m_nBrushPaintROP) ? 1 : 0 );
}

void CBpaintDoc::OnRopReplace() 
{
	m_nBrushPaintROP = ID_ROP_REPLACE;
}

void CBpaintDoc::OnUpdateRopReplace(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (ID_ROP_REPLACE == m_nBrushPaintROP) ? 1 : 0 );
}

void CBpaintDoc::OnRopSingleColor() 
{
	m_nBrushPaintROP = ID_ROP_SINGLE_COLOR;
}

void CBpaintDoc::OnUpdateRopSingleColor(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (ID_ROP_SINGLE_COLOR == m_nBrushPaintROP) ? 1 : 0 );
}

// ============================================================================

void CBpaintDoc::OnRopDifference() 
{
}

void CBpaintDoc::OnUpdateRopDifference(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( FALSE );
}

void CBpaintDoc::OnRopKeepDiff() 
{
	m_nBrushPaintROP = ID_ROP_KEEP_DIFF;
}

void CBpaintDoc::OnUpdateRopKeepDiff(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (ID_ROP_KEEP_DIFF == m_nBrushPaintROP) ? 1 : 0 );
}

void CBpaintDoc::OnRopKeepSame() 
{
	m_nBrushPaintROP = ID_ROP_KEEP_SAME;
}

void CBpaintDoc::OnUpdateRopKeepSame(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( (ID_ROP_KEEP_SAME == m_nBrushPaintROP) ? 1 : 0 );
}

// ============================================================================

void CBpaintDoc::OnCopy() 
{
	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );
	
	if (m_pAnimation->GetBeginSelectedFrames() == -1 ||
		m_pAnimation->GetBeginSelectedLayers() == -1) 
	{
		return;
	}
	
	if (m_pAnimation->GetEndSelectedFrames() == -1 || 
		m_pAnimation->GetEndSelectedLayers() == -1) 
	{

		m_pAnimation->SetSelectedFrames( m_pAnimation->GetBeginSelectedFrames(), m_pAnimation->GetBeginSelectedFrames());
		m_pAnimation->SetSelectedLayers( m_pAnimation->GetBeginSelectedLayers(), m_pAnimation->GetBeginSelectedLayers());

	}

	if (s_clipboardShowcase) {

		delete s_clipboardShowcase;

	}
	
	s_clipboardShowcase = new BPT::CAnimationShowcase();

	if ( !s_clipboardShowcase ) {

		TRACE( "Unable to create animation showcase.\n" );

		return;

	}

	s_clipboardShowcase->SetCanvasSize( m_pShowcase->CanvasSize() );

	BPT::CAnimation * pDstAnim = s_clipboardShowcase->NewAnimation(
		m_pAnimation->GetEndSelectedFrames() - m_pAnimation->GetBeginSelectedFrames() + 1,
		m_pAnimation->GetEndSelectedLayers() - m_pAnimation->GetBeginSelectedLayers() + 1
	);

	if ( !pDstAnim ) {

		TRACE( "Unable to create animation.\n" );

		return;

	}

	pDstAnim->SetName( "Clipboard" );

	CopyFrames(
		s_clipboardShowcase,
		pDstAnim, 0, 0, 
		m_pShowcase,
		m_pAnimation, 
		m_pAnimation->GetBeginSelectedFrames(), 
		m_pAnimation->GetEndSelectedFrames(), 
		m_pAnimation->GetBeginSelectedLayers(), 
		m_pAnimation->GetEndSelectedLayers()
	);
}

void CBpaintDoc::OnPaste() 
{
	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

	if (s_clipboardShowcase) {

		BPT::CAnimation *pClipboardAnim = s_clipboardShowcase->FirstAnimation();

		CopyFrames(
			m_pShowcase,
			m_pAnimation, 
			m_pAnimation->CurrentFrame(), 
			m_pAnimation->CurrentLayer(), 
			s_clipboardShowcase,
			pClipboardAnim, 
			0, 
			pClipboardAnim->FrameCount() - 1, 
			0, 
			pClipboardAnim->LayerCount() - 1
		);

#if 1 // BPT 6/2/01

		ForceFullRedraw(); // BPT 6/17/01 
	
		// Make sure the document is marked dirty so...

		SetModifiedFlag();

		// Need to repaint the most views

		CActiveAnimationChange changeInfo;
						
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
				
		UpdateAllViews( 0, CActiveAnimationChange::NEW_EVERYTHING, &changeInfo );

#endif

	}
}

void CBpaintDoc::OnUpdatePaste(CCmdUI* pCmdUI) 
{

#if 1 // BPT 6/1/01

	pCmdUI->Enable( ((0 != m_pShowcase) && (0 != s_clipboardShowcase)) ? TRUE : FALSE );

#endif

}

//
//	CBpaintDoc::CopyFrames()
//

void CBpaintDoc::CopyFrames(
	BPT::CAnimationShowcase * pDstShowcase,
	BPT::CAnimation *dstAnimation,
	int dstFrameAt, int dstLayerAt,
	BPT::CAnimationShowcase * pSrcShowcase,
	BPT::CAnimation *srcAnimation,
	int srcFrameFrom, int srcFrameTo,
	int srcLayerFrom, int srcLayerTo)
{

	bool bError = false;

	int frameCount = srcFrameTo - srcFrameFrom + 1;
	int layerCount = srcLayerTo - srcLayerFrom + 1;

	int srcFrame = srcFrameFrom;
	int dstFrame = dstFrameAt;

	for ( int f = 0; f < frameCount; f++ ) {

		BPT::CFrame * pDstFrame = dstAnimation->GetFramePtr( dstFrame + f );
		BPT::CFrame * pSrcFrame = srcAnimation->GetFramePtr( srcFrame + f );

		if (pDstFrame == NULL || pSrcFrame == NULL) {

			break;

		}

		int srcLayer = srcLayerFrom;
		int dstLayer = dstLayerAt;

		for ( int l = 0; l < layerCount; l++ ) {

			// Get the source & dest layer pointers

			BPT::CLayer * pDstLayer = pDstFrame->GetLayerPtr( dstLayer + l );
			BPT::CLayer * pSrcLayer = pSrcFrame->GetLayerPtr( srcLayer + l );

			if (pDstLayer == NULL || pSrcLayer == NULL) {
				
				break;
				
			}
			
			if ( !pDstLayer->CopyFrom( pSrcLayer ) ) {

				break;

			}

#if 1 // BPT 6/1/01

			// Attach the visual element to the dst showcase so it is saved 
			// and can be managed properly via the find similar logic.
			// ----------------------------------------------------------------

			BPT::CVisualElement * pVisualElement = pDstLayer->GetVisualElement();

			if ( pVisualElement ) {

				pDstShowcase->AdoptVisualElement( pVisualElement );

				pVisualElement->Release();

			}

#endif

		}

	}

}

void CBpaintDoc::OnShowcaseOptimize() 
{
	if ( m_pShowcase ) {

		// ------------------------------------------------------------------------
	
		if ( !EnsureAnimationModeIsHalted() ) {
	
			return;
	
		}
	
		if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {
	
			return;
	
		}

		// Make sure that the edit is finished...

		m_pShowcase->OptimizeVisualElements();

		// Need to repaint the most views

		CActiveAnimationChange changeInfo;
						
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
				
		UpdateAllViews( 0, CActiveAnimationChange::NEW_EVERYTHING, &changeInfo );

	}

}

void CBpaintDoc::OnUpdateShowcaseOptimize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (0 != m_pShowcase) ? TRUE : FALSE );
}


void CBpaintDoc::OnClickSelectLayer() 
{
	m_bClickSelectCompositeMode = !m_bClickSelectCompositeMode;
}

void CBpaintDoc::OnUpdateClickSelectLayer(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( m_bClickSelectCompositeMode ? 1 : 0 );
}

void CBpaintDoc::OnFillAcrossLayers() 
{
	m_bFillAcrossLayerBoundaries = !m_bFillAcrossLayerBoundaries;
}

void CBpaintDoc::OnUpdateFillAcrossLayers(CCmdUI* pCmdUI) 
{
//	pCmdUI->Enable( (CBpaintDoc::MODE::COMPOSITE == m_CurrentMode) ? TRUE : FALSE );
	pCmdUI->SetCheck( m_bFillAcrossLayerBoundaries ? 1 : 0 );
}

void CBpaintDoc::OnTpbXmlExport() 
{

	// ------------------------------------------------------------------------

	char defaultFilename[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );

	_makepath( defaultFilename, drive, dir, fname, ".tpb" );

	// ------------------------------------------------------------------------

	CFileDialog opf( 
		TRUE, _T("*.tpb"), defaultFilename, OFN_LONGNAMES, 
		_T(
			"TPB Files (*.tpb)|*.tpb|"
			"All Files (*.*)|*.*|"
			"|"
		)
		,AfxGetMainWnd()
	);

	if ( IDOK == opf.DoModal() ) {

		CString filename = opf.GetPathName();

		// check to see if the file already exists and ask if it should
		// be replaced
		// --------------------------------------------------------------------

		FILE * inFile = fopen( filename, "rb" );

		if ( NULL != inFile ) {

			fclose( inFile );

			if ( IDYES != MessageBox( 
				AfxGetMainWnd()->GetSafeHwnd(),
				filename, "File already exisits continue export?",
				MB_YESNOCANCEL ) ) {

				return;

			}

		}

		// --------------------------------------------------------------------

		if ( !BPT::SaveBPTFile( filename, m_pShowcase, true, true, true ) ) {

			AfxMessageBox( "Unable to export?", MB_ICONERROR | MB_OK );

		}
				
	}

}

void CBpaintDoc::OnUpdateTpbXmlExport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

void CBpaintDoc::OnFatbitsColorUseChromakey() 
{
	m_bUseEditorChromakeyAsFatbitsColor = !m_bUseEditorChromakeyAsFatbitsColor;

	ForceFullRedraw();
}

void CBpaintDoc::OnUpdateFatbitsColorUseChromakey(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bUseEditorChromakeyAsFatbitsColor ? 1 : 0 );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnManagePalettes()
{
	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	if ( m_pShowcase ) {

		CPaletteManager palManagerDlg( AfxGetMainWnd() );

		palManagerDlg.m_pDoc = this;
		palManagerDlg.m_pShowcase = m_pShowcase;
		palManagerDlg.m_pSelectedPalette = GetCurrentPalette();

		// --------------------

		palManagerDlg.DoModal();

		// --------------------

		SetModifiedFlag();

		UpdateAllViews(
			NULL, CColorChange::WHOLE_PALETTE_CHANGE,
			(CObject *)&CColorChange( 0 )
		);

		ForceFullRedraw();

	}
	
}

void CBpaintDoc::OnUpdateManagePalettes(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pShowcase ? TRUE : FALSE );
}

// ----------------------------------------------------------------------------

//
//	CBpaintDoc::OnSpecializedwindowsLayersstripwindow()
//

void CBpaintDoc::OnSpecializedwindowsLayersstripwindow()
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

	if ( pApp ) {

		GLOBAL_CreateNewWindow(
			pApp->GetLayerStripViewDocTemplate(), this
		);

	}
}

void CBpaintDoc::OnUpdateSpecializedwindowsLayersstripwindow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pShowcase ? TRUE : FALSE );
}

//
//	CBpaintDoc::OnSpecializedwindowsBrushwindow()
//

void CBpaintDoc::OnSpecializedwindowsBrushwindow()
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

	if ( pApp ) {

		GLOBAL_CreateNewWindow(
			pApp->GetBrushViewDocTemplate(), this
		);

	}
}

void CBpaintDoc::OnUpdateSpecializedwindowsBrushwindow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pShowcase ? TRUE : FALSE );
}

//
//	CBpaintDoc::OnSpecializedwindowsFramesstripwindow()
//

void CBpaintDoc::OnSpecializedwindowsFramesstripwindow()
{
	CBpaintApp * pApp = static_cast<CBpaintApp *>( AfxGetApp() );

	if ( pApp ) {

		GLOBAL_CreateNewWindow(
			pApp->GetFrameStripViewDocTemplate(), this
		);

	}
}

void CBpaintDoc::OnUpdateSpecializedwindowsFramesstripwindow(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pShowcase ? TRUE : FALSE );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnChangePixelAspectRatio()
{
	if ( !m_pShowcase ) return;

	CSize pixelAspect = m_pShowcase->GetPixelAspectRatio();

	if ( !GetSizeDialog(
		AfxGetMainWnd(), "Pixel Aspect Ratio", pixelAspect, 
		&CSize( 32, 32 ), "PixelAspectRatioDlg", true ) ) {

		return;

	}

	// setup the canvas internals
	// ------------------------------------------------------------------------

	SetDocumentMode( CBpaintDoc::MODE::COMPOSITE );

	m_pShowcase->SetPixelAspectRatio( pixelAspect );

	ForceFullRedraw();

	// Redo the UI layout to rebuild the various canvas related elements.
	// ------------------------------------------------------------------------

	AfxGetMainWnd()->PostMessage( WM_COMMAND, GLOBAL_GetSettingInt( "LastUILayout", ID_UI_LAYOUT_6) );
}

void CBpaintDoc::OnUpdateChangePixelAspectRatio(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pShowcase ? TRUE : FALSE );
}

void CBpaintDoc::OnUpdateLoadPictureFromClipboard(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->Enable();
}

void CBpaintDoc::OnLoadPictureFromClipboard()
{
	BPT::C8bppPicture * pPicture = BPT::Load8bppPictureFromClipboard();

	if ( pPicture ) {

		LoadCanvasFromPicture( pPicture );

		delete pPicture;

	}

}

// ============================================================================

//
//	CBpaintDoc::OnAnimSpecialBrushToAnim()
//

void CBpaintDoc::OnAnimSpecialBrushToAnim()
{
	// ------------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// basic prep / error checking
	// ------------------------------------------------------------------------

	CBpaintDoc::editor_bitmap_type::pointer pBrushBitmap = 
		m_pPickupBrushMediator->GetBitmapPtr();

	if ( !pBrushBitmap ) {

		return;

	}

// This probably should have it's own dialog!
// This probably should have it's own dialog!
// This probably should have it's own dialog!
// This probably should have it's own dialog!

	// Ask the user what sized tiles to chop the brush into
	// ------------------------------------------------------------------------

	CSize tileSize;

	CSize limitSize( pBrushBitmap->Width(), pBrushBitmap->Height() );

	if ( !GetSizeDialog(
		AfxGetMainWnd(), "Choose tile size", tileSize, &limitSize, "ChopBrushIntoAnim", false) ) {

		return;

	}

	// Name the animation something clever? (ask user?)
	// ------------------------------------------------------------------------

	CString animName;

	animName.Format( "Chop Brush" );

	// Determine the number of tiles wide/tall for the capture process
	// ------------------------------------------------------------------------

	CSize tileCount;

	tileCount.cx = (pBrushBitmap->Width() + (tileSize.cx - 1)) / tileSize.cx;
	tileCount.cy = (pBrushBitmap->Height() + (tileSize.cy - 1)) / tileSize.cy;

	// Need to create an animation and select it!
	// ------------------------------------------------------------------------

	BPT::CAnimation * pAnim = m_pShowcase->NewAnimation( (tileCount.cx * tileCount.cy), 1 );

	if ( pAnim ) {

		SetModifiedFlag();

		// Set the name and select the animation
		// --------------------------------------------------------------------

		pAnim->SetName( animName );

		Request_AnimationChange( pAnim );

		// Start capturing
		// --------------------------------------------------------------------

		CBpaintDoc::editor_bitmap_type tileBitmap;

		if ( tileBitmap.Create( tileSize.cx, tileSize.cy ) ) {

			// Use the center of the canvas as the place to render the text
			// ----------------------------------------------------------------

			int cx = m_CanvasSize.cx / 2;
			int cy = m_CanvasSize.cy / 2;

			CPoint cornerPoint( cx - (tileSize.cx/2), cy - (tileSize.cy/2) );

#if 1
			pAnim->m_Link1 = cornerPoint;
#else
			pAnim->m_Link1.x = cx;
			pAnim->m_Link1.y = cy;
#endif

			// setup the transfer operator
			// ----------------------------------------------------------------

			CBpaintDoc::paint_brush_transfer_op_type top; 

			top.SetColorKey( m_BrushChromaKey );

			// ----------------------------------------------------------------

			{
				CWaitCursor();

				// Okay now for each letter draw and then capture
				// ------------------------------------------------------------

				CRect captureRect, boundingRectUnion;

				boundingRectUnion.SetRectEmpty();

				captureRect.SetRectEmpty();

				int frameNumber = 0;

				for ( int v = 0; v < tileCount.cy; v++ ) \
					for ( int h = 0; h < tileCount.cx; h++ ) {

					// Clear the capture buffers
					// --------------------------------------------------------
	
					if ( captureRect.IsRectEmpty() ) {

						tileBitmap.ClearBuffer( m_BrushChromaKey );

					} else {

						BPT::T_SolidRectPrim(
							tileBitmap, captureRect, 
							m_BrushChromaKey, paint_copy_op_type()
						);

					}

					// Render the tile
					// --------------------------------------------------------

					BPT::T_Blit(
						tileBitmap, (h * -tileSize.cx), (v * -tileSize.cy), 
						*pBrushBitmap, top
					);

					// Get the layer and capture the
					// --------------------------------------------------------

					BPT::CLayer * pLayer = m_pAnimation->ActiveLayer();
				
					if ( !pLayer ) {

						TRACE( "Unable to get frame %d active layer\n", frameNumber + 1 );
				
						AfxMessageBox( "Failed to get layer for a frame?", MB_ICONERROR | MB_OK );

						goto ABRUPT_END;
				
					}

					// --------------------------------------------------------

					BPT::T_FindBoundingRect(
						captureRect, tileBitmap, BPT::TIsNotValue<editor_pixel_type>( m_BrushChromaKey ), 0
					);
				
					if ( captureRect.IsRectEmpty() ) {
				
						// clear the visual element for the current frame/layer
						// ----------------------------------------------------
					
						pLayer->SetVisualElement( 0, &CPoint( 0, 0 ) );
				
					} else {

						// ----------------------------------------------------

						if ( boundingRectUnion.IsRectEmpty() ) {

							boundingRectUnion = captureRect;

						} else {

							boundingRectUnion |= captureRect;

						}
				
						// Ask the showcase to create us a new visual element
						// --------------------------------------------------------------------
				
						BPT::CVisualElement *pVisualElement = m_pShowcase->CreateVisualElementFromSurface(
							tileBitmap, m_BrushChromaKey, &captureRect
						);
					
						if ( !pVisualElement ) {
					
							TRACE( "Unable to create visual element for frame %d\n", frameNumber + 1 );

							AfxMessageBox( "Failed to create visual element?", MB_ICONERROR | MB_OK );

							goto ABRUPT_END;
					
						}
				
						// Replace the visual element for the current frame/layer
						// --------------------------------------------------------------------

						pLayer->SetVisualElement(
							pVisualElement
							,&cornerPoint
						);
				
						// Need to manage the thumbnail here somehow!
						// --------------------------------------------------------------------
				
					}

					// Force an update and then switch frames
					// --------------------------------------------------------

					ForceFullRedraw();

					Request_NextFrame();

					++frameNumber;

				}

				// Put the dc back the way it was for proper disposal and were done
				// ------------------------------------------------------------

			ABRUPT_END:
	
				// Set the first frames link point information
				// ------------------------------------------------------------

				BPT::CFrame * pFrame = pAnim->GetFramePtr( 0 );

				if ( pFrame ) {

					pFrame->m_Link1.x = boundingRectUnion.left + cornerPoint.x;
					pFrame->m_Link1.y = boundingRectUnion.top + cornerPoint.y;

					pFrame->m_Link2.x = boundingRectUnion.right + cornerPoint.x;
					pFrame->m_Link2.y = boundingRectUnion.top + cornerPoint.y;

					pFrame->m_Link3.x = boundingRectUnion.right + cornerPoint.x;
					pFrame->m_Link3.y = boundingRectUnion.bottom + cornerPoint.y;

					pFrame->m_Link4.x = boundingRectUnion.left + cornerPoint.x;
					pFrame->m_Link4.y = boundingRectUnion.bottom + cornerPoint.y;

				}

				m_bRenderLinkPointOverlays = true;

				ForceFullRedraw();
			}
	
		} else {

			AfxGetMainWnd()->MessageBox( 
				"Failed to create tile sized bitmap?", "Error!", MB_ICONERROR | MB_OK
			);

		}

	} else {

		AfxGetMainWnd()->MessageBox( 
			"Failed to create anim?", "Error!", MB_ICONERROR | MB_OK
		);

	}
}

void CBpaintDoc::OnUpdateAnimSpecialBrushToAnim(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (0 != m_pPickupBrushMediator->GetBitmapPtr()) ? TRUE : FALSE );
}

// ----------------------------------------------------------------------------

void CBpaintDoc::OnUpdateSpecialSuperOptimize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pShowcase ? TRUE : FALSE );
}

void CBpaintDoc::OnSpecialSuperOptimize()
{
	if ( m_pShowcase ) {

		// ------------------------------------------------------------------------
	
		if ( !EnsureAnimationModeIsHalted() ) {
	
			return;
	
		}
	
		if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {
	
			return;
	
		}

		// Make sure that the edit is finished...

		if ( IDNO == AfxMessageBox(
			"You should backup your file before performing this operation"
			"; continue with super optimize operation?", MB_YESNO ) ) {

			return;

		}

		if ( !m_pShowcase->SuperOptimizeVisualElements( true ) ) {

			AfxMessageBox( "There was a problem during the super optimize.", MB_ICONERROR | MB_OK );

		}

		// Need to repaint the most views

		CActiveAnimationChange changeInfo;
						
		changeInfo.m_OldSelection.x = m_pAnimation->CurrentFrame();
		changeInfo.m_OldSelection.y = m_pAnimation->CurrentLayer();
		changeInfo.m_NewSelection.x = changeInfo.m_OldSelection.x;
		changeInfo.m_NewSelection.y = changeInfo.m_OldSelection.y;
				
		UpdateAllViews( 0, CActiveAnimationChange::NEW_EVERYTHING, &changeInfo );

	}

}

// ----------------------------------------------------------------------------


void CBpaintDoc::OnBrushToLayeredAnim()
{
	// ------------------------------------------------------------------------

	if ( !EnsureAnimationModeIsHalted() ) {

		return;

	}

	if ( !SetDocumentMode( CBpaintDoc::MODE::COMPOSITE ) ) {

		return;

	}

	// basic prep / error checking
	// ------------------------------------------------------------------------

	CBpaintDoc::editor_bitmap_type::pointer pBrushBitmap = 
		m_pPickupBrushMediator->GetBitmapPtr();

	if ( !pBrushBitmap ) {

		return;

	}

// This probably should have it's own dialog!
// This probably should have it's own dialog!
// This probably should have it's own dialog!
// This probably should have it's own dialog!

	// Ask the user what sized tiles to chop the brush into
	// ------------------------------------------------------------------------

	CSize tileSize;

	CSize limitSize( pBrushBitmap->Width(), pBrushBitmap->Height() );

	if ( !GetSizeDialog(
		AfxGetMainWnd(), "Choose tile size", tileSize, &limitSize, "ChopBrushIntoLayeredAnim", false) ) {

		return;

	}

	// Name the animation something clever? (ask user?)
	// ------------------------------------------------------------------------

	CString animName;

	animName.Format( "Chop Brush 2" );

	// Determine the number of tiles wide/tall for the capture process
	// ------------------------------------------------------------------------

	CSize tileCount;

	tileCount.cx = (pBrushBitmap->Width() + (tileSize.cx - 1)) / tileSize.cx;
	tileCount.cy = (pBrushBitmap->Height() + (tileSize.cy - 1)) / tileSize.cy;

	// Need to create an animation and select it!
	// ------------------------------------------------------------------------

	BPT::CAnimation * pAnim = m_pShowcase->NewAnimation( 1, (tileCount.cx * tileCount.cy) );

	if ( pAnim ) {

		SetModifiedFlag();

		// Set the name and select the animation
		// --------------------------------------------------------------------

		pAnim->SetName( animName );

		Request_AnimationChange( pAnim );

		// Start capturing
		// --------------------------------------------------------------------

		CBpaintDoc::editor_bitmap_type tileBitmap;

		if ( tileBitmap.Create( tileSize.cx, tileSize.cy ) ) {

			// Use the center of the canvas as the place to render the text
			// ----------------------------------------------------------------

			int cx = m_CanvasSize.cx / 2;
			int cy = m_CanvasSize.cy / 2;

//			CPoint cornerPoint( cx - (tileSize.cx/2), cy - (tileSize.cy/2) );
//			CPoint cornerPoint( 0, 0 );

			pAnim->m_Link1.x = 0;
			pAnim->m_Link1.y = 0;

			// setup the transfer operator
			// ----------------------------------------------------------------

			CBpaintDoc::paint_brush_transfer_op_type top; 

			top.SetColorKey( m_BrushChromaKey );

			// ----------------------------------------------------------------

			{
				CWaitCursor();

				// Okay now for each letter draw and then capture
				// ------------------------------------------------------------

				CRect captureRect, boundingRectUnion;

				boundingRectUnion.SetRectEmpty();

				captureRect.SetRectEmpty();

				int layerNumber = 0;

				for ( int v = 0; v < tileCount.cy; v++ ) \
					for ( int h = 0; h < tileCount.cx; h++ ) {

					// Clear the capture buffers
					// --------------------------------------------------------
	
					if ( captureRect.IsRectEmpty() ) {

						tileBitmap.ClearBuffer( m_BrushChromaKey );

					} else {

						BPT::T_SolidRectPrim(
							tileBitmap, captureRect, 
							m_BrushChromaKey, paint_copy_op_type()
						);

					}

					// Render the tile
					// --------------------------------------------------------

					CPoint capturePoint( (h * tileSize.cx), (v * tileSize.cy) );

					BPT::T_Blit(
						tileBitmap, -capturePoint.x, -capturePoint.y, 
						*pBrushBitmap, top
					);

					// Get the layer and capture the
					// --------------------------------------------------------

					BPT::CLayer * pLayer = m_pAnimation->ActiveLayer();
				
					if ( !pLayer ) {

						TRACE( "Unable to get frame %d active layer\n", layerNumber + 1 );
				
						AfxMessageBox( "Failed to get layer for a frame?", MB_ICONERROR | MB_OK );

						goto ABRUPT_END;
				
					}

					// --------------------------------------------------------

					BPT::T_FindBoundingRect(
						captureRect, tileBitmap, BPT::TIsNotValue<editor_pixel_type>( m_BrushChromaKey ), 0
					);
				
					if ( captureRect.IsRectEmpty() ) {
				
						// clear the visual element for the current frame/layer
						// ----------------------------------------------------
					
						pLayer->SetVisualElement( 0, &CPoint( 0, 0 ) );
				
					} else {

						// ----------------------------------------------------

						if ( boundingRectUnion.IsRectEmpty() ) {

							boundingRectUnion = captureRect;

						} else {

							boundingRectUnion |= captureRect;

						}
				
						// Ask the showcase to create us a new visual element
						// --------------------------------------------------------------------
				
						BPT::CVisualElement *pVisualElement = m_pShowcase->CreateVisualElementFromSurface(
							tileBitmap, m_BrushChromaKey, &captureRect
						);
					
						if ( !pVisualElement ) {
					
							TRACE( "Unable to create visual element for layer %d\n", layerNumber + 1 );

							AfxMessageBox( "Failed to create visual element?", MB_ICONERROR | MB_OK );

							goto ABRUPT_END;
					
						}
				
						// Replace the visual element for the current frame/layer
						// --------------------------------------------------------------------

						CPoint cornerPoint = capturePoint + CPoint(m_pPickupBrushMediator->GetDefaultPos());

						pLayer->SetVisualElement(
							pVisualElement
							,&cornerPoint
						);
				
						// Need to manage the thumbnail here somehow!
						// --------------------------------------------------------------------
				
					}

					// Force an update and then switch frames
					// --------------------------------------------------------

					ForceFullRedraw();

					Request_NextLayer();

					++layerNumber;

				}

				// Put the dc back the way it was for proper disposal and were done
				// ------------------------------------------------------------

			ABRUPT_END:
	
				// ------------------------------------------------------------

				ForceFullRedraw();
			}
	
		} else {

			AfxGetMainWnd()->MessageBox( 
				"Failed to create tile sized bitmap?", "Error!", MB_ICONERROR | MB_OK
			);

		}

	} else {

		AfxGetMainWnd()->MessageBox( 
			"Failed to create anim?", "Error!", MB_ICONERROR | MB_OK
		);

	}
}

void CBpaintDoc::OnUpdateBrushToLayeredAnim(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (0 != m_pPickupBrushMediator->GetBitmapPtr()) ? TRUE : FALSE );
}
