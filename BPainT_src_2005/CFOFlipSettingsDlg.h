#if !defined(AFX_CFOFLIPSETTINGSDLG_H__67E04604_8A3D_4EE5_BEEF_EEFF7D6CD06D__INCLUDED_)
#define AFX_CFOFLIPSETTINGSDLG_H__67E04604_8A3D_4EE5_BEEF_EEFF7D6CD06D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TCompositeOpBase.h"

// CFOFlipSettingsDlg.h : header file
//
//	Copyright (c) 2001, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

/////////////////////////////////////////////////////////////////////////////
// CCFOFlipSettingsDlg dialog

class CCFOFlipSettingsDlg : public CDialog
{
// Construction
public:
	CCFOFlipSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCFOFlipSettingsDlg)
	enum { IDD = IDD_CFO_FLIP_DIALOG };
	BOOL	m_bHFlip;
	BOOL	m_bVFlip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCFOFlipSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCFOFlipSettingsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

///////////////////////////////////////////////////////////////////////////////
//
// START EXAMPLE COMPOSITE FRAME OP
//
///////////////////////////////////////////////////////////////////////////////


//
//	TFlipCompositeLayersOp
//
//	If you wanted to perform this operation with something other than
//	a simple copy of the data you would pass in a different template param.
//
//

template< class T = CBpaintDoc::visual_to_editor_copy_type >
class TFlipCompositeLayersOp : public 
	TSimpleCompositeLayersOp<T> {

private:

	BPT::BLITFX m_FlipSettings;

public:

	// These are 'traits' and they simplify access to various types.
	// ------------------------------------------------------------------------

	typedef TFlipCompositeLayersOp<T> this_type;

	typedef TSimpleCompositeLayersOp<T> base_type;

	typedef typename base_type::layer_collection_type layer_collection_type;

	typedef typename base_type::SFrameRangeInfo SFrameRangeInfo;

	typedef typename base_type::ROP ROP;

	// ------------------------------------------------------------------------

	TFlipCompositeLayersOp() : m_FlipSettings( 0 ) {

		// Read settings from the application here so the initial
		// message will be correct.

		m_FlipSettings.dwFlags = GLOBAL_GetSettingIntHelper(
			"CFlipCompositeLayersOp", "dwFlags", 0
		);
	
	}

	// This is the method that determines what the drop down combo says...
	// ------------------------------------------------------------------------

	virtual const char * GetName() const {

		return "Flip image";

	}

	// Build the message string to give feedback to the user
	// ------------------------------------------------------------------------

	virtual CString GetMsg() const {

		CString msg( "Operation will " );

		if ( BPT::BLITFX::HFLIP & m_FlipSettings.dwFlags ) {

			msg += "HFLIP ";

		}

		if ( BPT::BLITFX::VFLIP & m_FlipSettings.dwFlags ) {

			if ( BPT::BLITFX::HFLIP & m_FlipSettings.dwFlags ) {

				msg += "and ";

			}

			msg += "VFLIP ";

		}

		if ( 0 == m_FlipSettings.dwFlags ) {

			msg += "do nothing!";

		} else {

			msg += "the cels";

		}

		return msg;

	}

	// This method is called after perform method is called and it's a useful
	// spot to put simple transformations that are best performed to the results
	// of the perform and not the individual frame layers involved.
	// ------------------------------------------------------------------------

	virtual bool PostFrameOp( CBpaintDoc::editor_bitmap_type & surface ) {

		// Create the working bitmap (this isn't the best way to do this
		// but it is very simple to follow if you wanted to base other transforms
		// on this code...
		// --------------------------------------------------------------------

		CBpaintDoc::editor_bitmap_type transformedSurface;

		if ( !transformedSurface.Create( surface.Size() ) ) {

			TRACE( "Failed to create working surface..." );

			return false;

		}

		// This is where you would perform any sort of transform needed, for
		// this example it's just simply a blit operation that can flip the
		// image...
		// --------------------------------------------------------------------

		BPT::T_Blit(
			transformedSurface, 0, 0, surface,
			CBpaintDoc::paint_copy_op_type(),
			&m_FlipSettings
		);

		// Blit the transformed bitmap over the top of the incoming bitmap
		// --------------------------------------------------------------------

		BPT::T_Blit(
			surface, 0, 0, transformedSurface,
			CBpaintDoc::paint_copy_op_type()
		);

		// we're done
		// --------------------------------------------------------------------

		return true;

	}

	// This is the code to perform the operation on the selected ranges.
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

		// Just call the base perform operation to do a simple composite
		// if you wanted to perform some specific operation during the 
		// render of each frame layer you would need to build a method
		// based on "TSimpleCompositeLayersOp<T>::Perform(...)" which can
		// be found in the "TCompositeOpBase.h".  Since we're interested 
		// in the results of the composite in this example we will use
		// the PostFrameOp() method instead.
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

	// Deal with the settings dialog for this operation
	// --------------------------------------------------------------------

	virtual bool SettingsDialog( CWnd * pParent ) {

		CCFOFlipSettingsDlg settingsDlg( pParent );

		// Init settings

		if ( BPT::BLITFX::HFLIP & m_FlipSettings.dwFlags ) {

			settingsDlg.m_bHFlip = TRUE;

		}

		if ( BPT::BLITFX::VFLIP & m_FlipSettings.dwFlags ) {

			settingsDlg.m_bVFlip = TRUE;

		}

		// do the modal dialog

		if ( IDOK == settingsDlg.DoModal() ) {

			// Convert dialog settings to useful information
			// ----------------------------------------------------------------

			m_FlipSettings.dwFlags = 0;

			if ( settingsDlg.m_bHFlip ) {

				m_FlipSettings.dwFlags |= BPT::BLITFX::HFLIP;

			}

			if ( settingsDlg.m_bVFlip ) {

				m_FlipSettings.dwFlags |= BPT::BLITFX::VFLIP;

			}

			// store current options settings
			// ----------------------------------------------------------------

			GLOBAL_PutSettingIntHelper(
				"CFlipCompositeLayersOp", "dwFlags", m_FlipSettings.dwFlags
			);

		}

		return true;

	}

	virtual bool HasSettingsDialog() {

		return true;

	}

};

///////////////////////////////////////////////////////////////////////////////
//
// END!! EXAMPLE COMPOSITE FRAME OP
//
///////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_CFOFLIPSETTINGSDLG_H__67E04604_8A3D_4EE5_BEEF_EEFF7D6CD06D__INCLUDED_)
