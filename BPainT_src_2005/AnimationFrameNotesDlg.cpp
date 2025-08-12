// AnimationFrameNotesDlg.cpp : implementation file
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//

#include "stdafx.h"
#include "bpaint.h"
#include "AnimationFrameNotesDlg.h"
#include "BpaintDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimationFrameNotesDlg dialog


CAnimationFrameNotesDlg::CAnimationFrameNotesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationFrameNotesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnimationFrameNotesDlg)
	//}}AFX_DATA_INIT
	m_strCombinedNotes = _T("");
}


void CAnimationFrameNotesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimationFrameNotesDlg)
	DDX_Control(pDX, IDC_NOTES_EDIT, m_GreedyEditCtrl);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_NOTES_EDIT, m_strCombinedNotes);
}


BEGIN_MESSAGE_MAP(CAnimationFrameNotesDlg, CDialog)
	//{{AFX_MSG_MAP(CAnimationFrameNotesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ============================================================================

//
//	CBpaintDoc::AnimationFrameNotesDialogHandler()
//

bool 
CBpaintDoc::AnimationFrameNotesDialogHandler( CWnd * pParent )
{
	CAnimationFrameNotesDlg dlg( pParent );

	// Fill the string with the notes from the frames
	// ------------------------------------------------------------------------

	dlg.m_strCombinedNotes.Empty();

	int frameCount = m_pAnimation->FrameCount();

	for ( int i = 0; i < frameCount; i++ ) {

		BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( i );

		if ( pFrame && pFrame->HasNote() ) {

			// ----------------------------------------------------------------

			CString framePrefix;

			framePrefix.Format( "frame %d:\r\n", i + 1 );

			dlg.m_strCombinedNotes += framePrefix;

			// ----------------------------------------------------------------

			dlg.m_strCombinedNotes += pFrame->GetNote();

			// ----------------------------------------------------------------

			dlg.m_strCombinedNotes += "\r\n";

		}

	}

	// ------------------------------------------------------------------------

CORRECT_ERRORS:

	if ( IDOK == dlg.DoModal() ) {

		// Make a copy so that the destructive strtok operation can be performed
		// If it fails bail out (ugly but it's unlikely)
		// --------------------------------------------------------------------

		char * pOriginalParseString = strdup( dlg.m_strCombinedNotes );

		if ( !pOriginalParseString ) {

			AfxGetMainWnd()->MessageBox( 
				"Unable to duplicate string for final parse!",
				"ERROR!", MB_ICONERROR | MB_OK
			);

			return false;

		}

		// Clear all the existing notes in the frames
		// --------------------------------------------------------------------

		for ( i = 0; i < frameCount; i++ ) {

			BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( i );

			if ( pFrame && pFrame->HasNote() ) {

				pFrame->ClearNote();

			}

		}

		// Okay now start our parse of the string
		// --------------------------------------------------------------------

		char * pParseString = pOriginalParseString;

		char * pToken = strtok( pParseString, " ,\t\r\n" );

		int nLastFrameNumber = -1;

		bool bSyntaxError = false;

		while ( NULL != pToken ) {

			if ( 0 != strstr( pToken, "frame" ) ) {

				// find the number
				// ------------------------------------------------------------

				char * pNumber = strtok( NULL, ":" );

				if ( !pNumber ) {

					CString msg;

					msg.Format( "\"frame\" found with no \":\" to single the end of the number?" );

					AfxGetMainWnd()->MessageBox( msg, "Syntax error?", MB_OK );

					bSyntaxError = true;

					break;

				}

				// Validate the number
				// ------------------------------------------------------------

				int frameNumber = atoi( pNumber ) - 1;

				if ( frameNumber <= nLastFrameNumber ) {

					CString msg;

					msg.Format( "Found frame %d <= last processed frame %d", frameNumber, nLastFrameNumber );

					AfxGetMainWnd()->MessageBox( msg, "Syntax error?", MB_OK );

					bSyntaxError = true;

					break;

				}

				// Make sure the frame number is in range
				// ------------------------------------------------------------

				if ( (0 > frameNumber) || (frameNumber >= frameCount) ) {

					CString msg;

					msg.Format( "Frame %d invalid limit [%d to %d]!", frameNumber + 1, 1, frameCount );

					AfxGetMainWnd()->MessageBox( msg, "Syntax error?", MB_OK );

					bSyntaxError = true;

					break;

				}

				// Okay now let's try to parse to the end of the "block"...
				// ------------------------------------------------------------

				char * pStartOfBlock = pNumber + strlen( pNumber ) + 2;

				if ( !*pStartOfBlock ) {

					CString msg;

					msg.Format( "Unable to begin frame %d capture?", frameNumber + 1 );

					AfxGetMainWnd()->MessageBox( msg, "Syntax error?", MB_OK );

					bSyntaxError = true;

					break;

				}

				// Try to skip bogus line seperation data
				// ------------------------------------------------------------

				while ( *pStartOfBlock ) {

					if ( ('\n' != *pStartOfBlock ) && ('\r' != *pStartOfBlock ) ) {

						break;

					}

					++pStartOfBlock;

				}

				// Use the strstr() function to find the next "frame"
				// ------------------------------------------------------------

				char * pNextFrameStart = strstr( pStartOfBlock, "frame" );

				// Get the frame to store the note into
				// ------------------------------------------------------------

				BPT::CFrame * pFrame = m_pAnimation->GetFramePtr( frameNumber );

				if ( !pFrame ) {

					int result = AfxGetMainWnd()->MessageBox( "Unable to get frame %d skip and resume processing?", "Failure!", MB_ICONERROR | MB_YESNOCANCEL );

					if ( (IDYES == result) && pNextFrameStart ) {

						pToken = strtok( pNextFrameStart, " ,\t\r\n" );

						continue;

					}

				}

				// ------------------------------------------------------------

				if ( pNextFrameStart ) {

					pParseString = pNextFrameStart;

					// Try to backup the next frame pointer to avoid getting
					// extra white space as a part of the parser.
					// ------------------------------------------------------------
	
					while ( pNextFrameStart > (pStartOfBlock + 1) ) {
	
						if ( 
							('\n' != *pNextFrameStart ) && 
							('\n' != *pNextFrameStart ) && 
							('f' != tolower(*pNextFrameStart) ) 
							) {
	
							break;
	
						}
	
						--pNextFrameStart;
	
					}

					// ------------------------------------------------------------

					int nLength = pNextFrameStart - pStartOfBlock;

					pFrame->SetNote( CString( pStartOfBlock, nLength ) );


				} else {

					// This frame was the last one so break out of the parser

					pFrame->SetNote( CString( pStartOfBlock ) );

					break;

				}

			} else {

				pParseString = NULL;

			}

			pToken = strtok( pParseString, " ,\t\r\n" );

		}

		// Clean up and leave if no errors were found
		// --------------------------------------------------------------------

		free( pOriginalParseString );

		if ( bSyntaxError ) {

			goto CORRECT_ERRORS;

		}

	}

	return true;
}

// ============================================================================

/////////////////////////////////////////////////////////////////////////////
// CAnimationFrameNotesDlg message handlers


