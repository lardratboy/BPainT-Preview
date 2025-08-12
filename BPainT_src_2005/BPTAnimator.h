// BPTAnimator.h: interface for the BPTAnimator class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTANIMATOR_H__1A7F1BD5_6A97_449C_94FF_7C1F8BCD9737__INCLUDED_)
#define AFX_BPTANIMATOR_H__1A7F1BD5_6A97_449C_94FF_7C1F8BCD9737__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------------

// Disable > 255 char warning -- I hope they fix this in VC++ 7.0!!!

#pragma warning(disable: 4786)

#define BPT_OPTIMIZE_VISUAL_ELEMENTS
#define BPT_SHOWCASE_MANAGED_PALETTES

// ----------------------------------------------------------------------------

#include <list>
#include <map>

#include "BPTUtility.h"
#include "BPTPrimitives.h"
#include "BPTSRL.h"
#include "BPTFileio.h"
#include "BPTBreakdown.h"

// ----------------------------------------------------------------------------

namespace BPT {

	// ID type for a visual element should this be a GUID?
	// ------------------------------------------------------------------------

	typedef DWORD CIDType;

	// Forward reference
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CShowcaseFileIO;

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Annotation abstract
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class AAnnotation : public TISupportProperties<> {

	public: // Traits

	private: // Data

	public: // Interface

	};

	// text annotation
	// ------------------------------------------------------------------------

	class CTextAnnotation : public AAnnotation {

	public: // Traits

	private: // Data

	public: // Interface

	};

	// Canvas Relative 'Point' annotation
	// ------------------------------------------------------------------------

	class CPointAnnotation : public CTextAnnotation {

	public: // Traits

	private: // Data

	public: // Interface

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Base object 
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CAnnotatedObject /* : public CObject */ {

	public: // Traits

		// this should probably be a multi-map, that uses the prefix
		// of the annotation as the key, if there isn't one then
		// a generic one could be assumed it would probably make a whole
		// lot of sense if these are going to be used as storage of things
		// such as the name, and link points rapid retrieval is important!

		typedef std::list< AAnnotation * > annotation_collection_type;

	private: // Data

		annotation_collection_type m_Annotations;

		CString m_Str;

	private: // methods

	public: // Interface

		CAnnotatedObject() {

			m_Str.Empty();
		
		}

		// --------------------------------------------------------------------

		int AnnotationCount() {

			return m_Annotations.size();

		}

		annotation_collection_type::iterator AnnotationBegin() {

			return m_Annotations.begin();

		}

		annotation_collection_type::iterator AnnotationEnd() {

			return m_Annotations.end();

		}

		// Ugly temporary solution for the annotations
		// --------------------------------------------------------------------

		bool SetNote( CString & str ) {

			m_Str = str;

			return (m_Str == str);

		}

		CString & GetNote() {

			return m_Str;

		}

		bool HasNote() const {

			return ( 0 != m_Str.GetLength() );

		}

		void ClearNote() {

			m_Str.Empty();

		}

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// A visual element reference + data
	// ------------------------------------------------------------------------

	class CVisualElement; // BPT 9/25/02

	//
	//	CVisualElementReference
	//

	class CVisualElementReference : public CAnnotatedObject {

	public: // Traits

	private: // Data

		CVisualElement * m_pVisualElement;

		POINT m_At;

	public: // Interface

		CVisualElementReference() : m_pVisualElement( 0 ) {

			m_At.x = 0;
			m_At.y = 0;

		}

		~CVisualElementReference();

		// --------------------------------------------------------------------

		int GetX() const {

			return m_At.x;

		}

		int GetY() const {

			return m_At.y;

		}

		POINT GetPos() const {

			return m_At;

		}

		void MoveTo( const int x, const int y ) {

			m_At.x = x;
			m_At.y = y;

		}

		void Shift( const int dx, const int dy ) {

			MoveTo( m_At.x + dx, m_At.y + dy );

		}

		bool HasVisualElement() const {

			return (0 != m_pVisualElement );

		}

		// --------------------------------------------------------------------

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01 

		RECT BoundingRect(
			const BLITFX * pOptionalBlitFX = 0,
			const SIZE * pAlignmentCanvas = 0
		);

#endif // BPT 5/22/01

		bool CheckStoredPixelSize( const int value );

		template< class ST >
		bool HitTest( const int x, const int y, ST * pValue = 0 ) { // BPT 9/25/02

			if ( !m_pVisualElement ) return false;

			return m_pVisualElement->HitTest(
				x - m_At.x, y - m_At.y, pValue
			);

		}

		template< class SURFACE, class TOP >
		bool Render(
			SURFACE & dstSurface, const int x, const int y, TOP op,
			const RECT * pOptionalClipRect = 0, const BLITFX * pOptionalBlitFX = 0
		) {

			if ( !m_pVisualElement ) return false;

			// What about BLITFX features embedded inside this visual reference?

			return m_pVisualElement->Render(
				dstSurface, x + m_At.x, y + m_At.y, op,
				pOptionalClipRect, pOptionalBlitFX
			);

		}

		// --------------------------------------------------------------------
		// Caller MUST call release after using this object. This should ref
		// counting really should be done with a class this will be changed in
		// the future.
		// --------------------------------------------------------------------

		CVisualElement * GetVisualElement() const;

		void SetVisualElement( CVisualElement * pVisualElement );

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Visual element
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CVisualElement : public CAnnotatedObject {

	public: // Traits

		typedef unsigned int storage_type;

		typedef TSRLCompressor< storage_type > compressor_type;

		typedef TSRLCompressedImage< compressor_type > compressed_image_type;

#if 1 // BPT 6/1/01

		typedef CVisualElement * pointer;

		typedef CVisualElement & reference;

#endif

#if 1 // BPT 9/25/02

		typedef std::list< CVisualElementReference * > nested_collection_type;

#endif

	private: // Data

		unsigned long m_ReferenceCount;

		compressed_image_type m_CompressedImage;

		int m_StoredPixelSize;

		RECT m_CaptureRect;

		int m_ID;
		int m_LastID;

#if 1 // BPT 9/25/02

		nested_collection_type m_NestedVisuals;

#endif

	private:

		// --------------------------------------------------------------------

		template< class SURFACE, class PREDICATE >
		bool CreateThumbnail(
			SURFACE & surface, PREDICATE predicate, 
			const RECT * pRect, const SIZE * pOptionalThumbnailSize
		) {

			// This will eventually need to do something!

			return true;

		}

	public: // Interface

		CVisualElement( const int id ) :
			m_ID( id ), m_LastID( id ), 
			m_ReferenceCount( 0 ), m_StoredPixelSize( 0 ) {
		
			SetRectEmpty( &m_CaptureRect );
		
		}

		virtual ~CVisualElement() {

			DestroyCurrentVisuals();
		
		}

		void DestroyNestedVisuals() {

			while ( !m_NestedVisuals.empty() ) { // BPT 9/25/02

				delete m_NestedVisuals.back();

				m_NestedVisuals.pop_back();

			}

		}

		void DestroyCurrentVisuals() {

			m_CompressedImage.Destroy();

			DestroyNestedVisuals();

			SetRectEmpty( &m_CaptureRect );

		}

		// ID
		// --------------------------------------------------------------------

		int ID() {

			return m_ID;

		}

		int LastID() {

			return m_LastID;

		}

		void SetID( const int id ) {

			m_LastID = m_ID;

			m_ID = id;

		}

		// reference counting
		// --------------------------------------------------------------------

		void AddRef() {

			++m_ReferenceCount;

		}

		void Release() {

			if ( 0 == --m_ReferenceCount ) {

				delete this;

			}

		}

		int RefCount() {

			return m_ReferenceCount;

		}

		// 
		// --------------------------------------------------------------------

		bool HasNestedVisualReferences() const { // BPT 9/25/02

			return !m_NestedVisuals.empty();

		}

		int NestedVisualsCount() const { // BPT 10/14/02

			return m_NestedVisuals.size();

		}

		CVisualElementReference * GetNestedVisualReference( const int nth ) {

			nested_collection_type::iterator it = m_NestedVisuals.begin();

			for ( int place = 0; it != m_NestedVisuals.end(); it++ ) {

				if ( nth == place ) return (*it);

				++place;

			}

			return 0;

		}

		// Information query
		// --------------------------------------------------------------------

		RECT Rect(
			const POINT * pPoint = 0, const BLITFX * pOptionalBlitFX = 0,
			const SIZE * pAlignmentCanvas = 0
		) {

			// If this is a nested visual element then the rect is the 
			// combination of all the nested rects.
			// ----------------------------------------------------------------

			if ( HasNestedVisualReferences() ) {

				nested_collection_type::iterator it = m_NestedVisuals.begin();

				CRect boundingRect;

				boundingRect.SetRectEmpty();

				while ( it != m_NestedVisuals.end() ) {

					CRect rect = (*it)->BoundingRect( pOptionalBlitFX, pAlignmentCanvas );

					if ( pPoint ) {

						rect.OffsetRect( pPoint->x, pPoint->y );

					}

					if ( boundingRect.IsRectEmpty() ) {

						boundingRect = rect;

					} else if ( !rect.IsRectEmpty() ) {

						boundingRect |= rect;

					}

					++it;

				}

				return boundingRect;

			}

			// ----------------------------------------------------------------

			int x = ( pPoint ) ? pPoint->x : 0;
			int y = ( pPoint ) ? pPoint->y : 0;

			int xx = x + m_CaptureRect.left;
			int yy = y + m_CaptureRect.top;

			if ( pOptionalBlitFX ) {

				ASSERT( pAlignmentCanvas );

				if ( BLITFX::HFLIP & pOptionalBlitFX->dwFlags ) {

					xx = (pAlignmentCanvas->cx - (x + m_CaptureRect.right));

				}

				if ( BLITFX::VFLIP & pOptionalBlitFX->dwFlags ) {

					yy = (pAlignmentCanvas->cy - (y + m_CaptureRect.bottom));

				}

			}
			
			RECT rect = m_CaptureRect;

			SetRect( 
				&rect, xx, yy, 
				xx + (m_CaptureRect.right - m_CaptureRect.left), 
				yy + (m_CaptureRect.bottom - m_CaptureRect.top)
			);

			return rect;

		}

		RECT BoundingRect(
			const POINT * pPoint = 0, const BLITFX * pOptionalBlitFX = 0,
			const SIZE * pAlignmentCanvas = 0
		) {

			// Eventually there might be a difference between the bounding
			// rect and the capture rect but for now lets simplify the issue.

			return Rect( pPoint, pOptionalBlitFX, pAlignmentCanvas );

		}

		SIZE Size() {

			if ( HasNestedVisualReferences() ) {

				CRect bounding( BoundingRect() );

				return bounding.Size();

			}

			return m_CompressedImage.Size();

		}

		// Creation interface
		// --------------------------------------------------------------------

		template< class SURFACE, class PREDICATE >
		bool CreateFrom(
			SURFACE & surface, PREDICATE predicate, 
			const RECT * pRect = 0, const SIZE * pOptionalThumbnailSize = 0
		 ) {

			// Clear out any currently stored visual(s).
			// ----------------------------------------------------------------

			DestroyCurrentVisuals();

			// find the bounding rectangle using the predicate
			// ----------------------------------------------------------------

			RECT captureRect = surface.Rect();

			if ( pRect ) {

				captureRect = *pRect;

			} else {

				T_FindBoundingRect(
					captureRect, surface, predicate, 0
				);

			}

			// make sure we have a valid capture rect
			// ----------------------------------------------------------------

			if ( IsRectEmpty( &captureRect ) ) {

				return false;

			}

			// compress the area into a SRL object
			// ----------------------------------------------------------------

			if ( !m_CompressedImage.Create( surface, predicate, &captureRect ) ) {

				return false;

			}

			// Record some very useful information 
			// ----------------------------------------------------------------

			m_StoredPixelSize = sizeof( typename SURFACE::pixel_type );

			m_CaptureRect = captureRect;

			// create the thumbnail
			// ----------------------------------------------------------------

			if ( !CreateThumbnail( surface, predicate, 
				&m_CaptureRect, pOptionalThumbnailSize ) ) {

				TRACE( "Failed to create thumbnail.\n" );

				// Failed to create the thumbnail

				// Should this be an error?
				// Should this be an error?
				// Should this be an error?

			}

			return true;

		}

		// Render interface
		// --------------------------------------------------------------------

		template< class SURFACE, class TOP >
		bool Render(
			SURFACE & dstSurface, const int x, const int y, TOP op,
			const RECT * pOptionalClipRect = 0, const BLITFX * pOptionalBlitFX = 0
		) {

			// Render the nested visuals
			// ----------------------------------------------------------------

			if ( HasNestedVisualReferences() ) {

				nested_collection_type::iterator it = m_NestedVisuals.begin();

				for ( ; it != m_NestedVisuals.end(); it++ ) {

					(*it)->Render( dstSurface, x, y, op, pOptionalClipRect, pOptionalBlitFX );

				}

				return true;

			}

			// ----------------------------------------------------------------

			RECT rect = Rect(
				&CPoint( x, y ), pOptionalBlitFX, &dstSurface.Size()
			);

			// ----------------------------------------------------------------

			m_CompressedImage.Blit(
				dstSurface, rect.left, rect.top, 
				op, pOptionalBlitFX, pOptionalClipRect, 0
			);

			return true;

		}

		// Render interface
		// --------------------------------------------------------------------

		template< class SURFACE, class TOP >
		bool RenderThumbnail(
			SURFACE & dstSurface, const int x, const int y, TOP op,
			const RECT * pOptionalClipRect = 0, const BLITFX * pOptionalBlitFX = 0
		) {

			// This will eventually render the thumbnail.

			return true;
		
		}

		// --------------------------------------------------------------------

		void ChangeStoredPixelSize( const int value ) {

			m_StoredPixelSize = value;

		}

		int GetStoredPixelSize() const {

			return m_StoredPixelSize;

		}

		bool CheckStoredPixelSize( const int value ) {

			// ----------------------------------------------------------------

			if ( HasNestedVisualReferences() ) {

				nested_collection_type::iterator it = m_NestedVisuals.begin();

				for ( ; it != m_NestedVisuals.end(); it++ ) {

					if ( !(*it)->CheckStoredPixelSize( value ) ) {

						return false;

					}

				}

				return true;

			}

			return (value == GetStoredPixelSize());

		}

		// --------------------------------------------------------------------

		bool HitTest( const int x, const int y, storage_type * pValue = 0 ) {

			// ----------------------------------------------------------------

			if ( HasNestedVisualReferences() ) {

				nested_collection_type::iterator it = m_NestedVisuals.begin();

				for ( ; it != m_NestedVisuals.end(); it++ ) {

					if ( (*it)->HitTest( x, y, pValue ) ) {

						return true;

					}

				}

				return false;

			}

			// ----------------------------------------------------------------

			return m_CompressedImage.HitTest(
				x - m_CaptureRect.left, y - m_CaptureRect.top, pValue
			);

		}

		// --------------------------------------------------------------------

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01 

		bool _CompareCompressedImage_( const CVisualElement * pOther ) const {

			// ----------------------------------------------------------------

			if ( GetStoredPixelSize() != pOther->GetStoredPixelSize() ) {

#if 0
				TRACE(
					"VE: %p == %p? pixel depth different (%d vs. %d)\n",
					this, pOther, GetStoredPixelSize(), pOther->GetStoredPixelSize()
				);
#endif

				return false;

			}

			return m_CompressedImage.SameAs( &pOther->m_CompressedImage );

		}

		bool CompareVisualElement( const CVisualElement * pOther ) { // BPT 9/25/02

			// ----------------------------------------------------------------

			if ( HasNestedVisualReferences() ) {
				
				if ( !pOther->HasNestedVisualReferences() ) {

					return false;

				}

				// Need to be able to find duplicates even if the order of the
				// visual element references don't match exactly as long as they
				// match the layering info.

				return false; // for now just return no match!

			}

			// ----------------------------------------------------------------

			return _CompareCompressedImage_( pOther );

		}

#if 1	// BPT 9/28/02

		template< class SHOWCASE >
		bool OptimizeNestedVisualElements(
			SHOWCASE * pShowcase, typename SHOWCASE::optimize_map_type * pOptimizeMap
		) {

			if ( !HasNestedVisualReferences() ) return false;

			bool bDidSomething = false;

			nested_collection_type::iterator it = m_NestedVisuals.begin();

			for ( ; it != m_NestedVisuals.end(); it++ ) {

				if ( pShowcase->OptimizeVisualElementReference( *it, pOptimizeMap ) ) {

					bDidSomething = true;

				}

			}

			return bDidSomething;
		}

#endif

#endif // BPT 5/22/01

		// --------------------------------------------------------------------

#if 1 // BPT 9/27/02

	private:

		template< class SHOWCASE, class SURFACE >
		class TBreakdownContainer {

		public:

			typedef std::list< CVisualElement * > element_collection_type;

		private:

			SHOWCASE * m_pShowcase;

		public:

			element_collection_type m_NestedImages;

		public:

			TBreakdownContainer( SHOWCASE * pShowcase ) : m_pShowcase( pShowcase ) {}

			bool operator()( SURFACE & surface, RECT & rect, typename SURFACE::pixel_type chromakey ) {

				TRACE(
					"Capture (%4d, %4d, %4d, %4d)[%4dx%4d]\n",
					rect.left, rect.top, rect.right, rect.bottom,
					rect.right - rect.left, rect.bottom - rect.top
				);

				CVisualElement * pNew = m_pShowcase->CreateVisualElementFromSurface(
					surface, chromakey, &rect
				);

				if ( !pNew ) {

					return false;

				}

				m_NestedImages.push_back( pNew );

				return true;

			}

			element_collection_type::iterator begin() {

				return m_NestedImages.begin();

			}

			element_collection_type::iterator end() {

				return m_NestedImages.end();

			}

			int size() {

				return m_NestedImages.size();

			}

			element_collection_type::reference back() {

				return m_NestedImages.back();

			}

			void pop_back() {

				m_NestedImages.pop_back();

			}

		};

	public:

		template< class SHOWCASE >
		bool BreakdownVisualElement(
			SHOWCASE * pShowcase
			,const int adjacentSearchWidth
			,const int adjacentSearchHeight
		) {

			// simple rejection cases

			if ( !pShowcase ) return false;
	
			if ( HasNestedVisualReferences() ) return true;

			if ( IsRectEmpty( &m_CaptureRect ) ) return true;

			if ( 0 >= m_CaptureRect.right ) return false;

			if ( 0 >= m_CaptureRect.bottom ) return false;

			// create the 'working' surface

			storage_type chromakey( 0 );

			typedef TSimpleBitmap<storage_type> surface_type;

			surface_type surface;

			if ( !surface.Create( m_CaptureRect.right, m_CaptureRect.bottom ) ) {

#ifdef _DEBUG
				TRACE( "Unable to create working surface (%dx%d) for breakdown operation?\n", m_CaptureRect.right, m_CaptureRect.bottom );
#endif

				return false;

			}

			surface.ClearBuffer( chromakey );

			// render the image into our working surface

			BPT::TCopyROP<storage_type> copyOp;

			Render( surface, 0, 0, copyOp );

			// create the breakdown container

			typedef TBreakdownContainer<SHOWCASE,surface_type> breakdown_container;

			breakdown_container collector( pShowcase );

			// finally call the surface breakdown

			BPT::TBreakDownSurface<surface_type> breakdown;

			if ( !breakdown.BreakDown(
				collector, surface, chromakey, true, true, &m_CaptureRect
				,adjacentSearchWidth, adjacentSearchHeight
				) ) {

#ifdef _DEBUG
				TRACE( "VisualElement breakdown failed?\n" );
#endif

				return false;

			}

			// replace the compressed image with the nested images

			bool bSucceeded = true;
			bool bReturnValue = true;

			if ( 1 < collector.size() ) {

				typename breakdown_container::element_collection_type::iterator it = collector.begin();

				for ( ; it != collector.end(); it++ ) {

					CVisualElementReference * pNew = new CVisualElementReference();

					if ( !pNew ) {

#ifdef _DEBUG
						TRACE( "Unable to create CVisualElementReference for nested image?\n" );
#endif

						bSucceeded = false;
						bReturnValue = false;
						break;

					}

					// force the stored pixel size to match the current setting

					(*it)->ChangeStoredPixelSize( GetStoredPixelSize() ); 

					pNew->SetVisualElement( *it );

					m_NestedVisuals.push_back( pNew );

				}

			} else {

				bSucceeded = false;

			}

			// if all the visual references were created then destroy the
			// compressed image, otherwise destroy the incomplete nested
			// visuals collection.

			if ( bSucceeded ) {

				m_CompressedImage.Destroy();

			} else {

				DestroyNestedVisuals();

				pShowcase->ReleaseUnusedVisualElements();

			}

			return bReturnValue;

		}

#endif

#if 1 // BPT 10/3/02

		template< class SHOWCASE > bool AdoptNestedVisuals( SHOWCASE * pShowcase ) {

			if ( !HasNestedVisualReferences() ) return true;

			nested_collection_type::iterator it = m_NestedVisuals.begin();

			for ( ; it != m_NestedVisuals.end(); it++ ) {

				CVisualElement * pVisualElement = (*it)->GetVisualElement();

				if ( pVisualElement ) {

					pShowcase->AdoptVisualElement( pVisualElement );

					pVisualElement->Release();

				}

			}

			return true;

		}

#endif

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

		// --------------------------------------------------------------------

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Layer class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CLayer : public CAnnotatedObject {

	public: // Traits

	private: // Data

		CVisualElementReference m_VisualReference;

	public: // Interface

		// --------------------------------------------------------------------

		CLayer() { /* empty */ }

		~CLayer() {

		}

		// --------------------------------------------------------------------

		bool Init( const int id ) {

			// vestage from when layers controlled frames...

			return true;

		}

		// --------------------------------------------------------------------

		POINT GetPos() const {

			return m_VisualReference.GetPos();

		}

		void Shift( const int dx, const int dy ) {

			m_VisualReference.Shift( dx, dy );

		}

		void Shift( const POINT & delta ) {

			Shift( delta.x, delta.y );

		}

		void MoveTo( const int x, const int y ) {

			m_VisualReference.MoveTo( x, y );
		}

		void MoveTo( const POINT & pos ) {

			MoveTo( pos.x, pos.y );

		}

		void SetVisualElement(
			CVisualElement * pVisualElement, const POINT * pPos = 0
		) {

			m_VisualReference.SetVisualElement( pVisualElement );

			if ( pPos ) {

				MoveTo( *pPos );

			}

		}

		// --------------------------------------------------------------------

		bool CopyFrom( const CLayer * pLayer ) {

			CVisualElement * pVisualElement = pLayer->GetVisualElement();

			SetVisualElement( pVisualElement, &pLayer->GetPos() );

			if ( pVisualElement ) {

				pVisualElement->Release();

			}

			return true;

		}

		// --------------------------------------------------------------------

		// client must call release...

		CVisualElement * GetVisualElement() const {

			return m_VisualReference.GetVisualElement();

		}

		bool HasVisualElement() const {

			return m_VisualReference.HasVisualElement();

		}

		// Render interface
		// --------------------------------------------------------------------

		template< class SURFACE, class TOP > void 
		Render(
			SURFACE & dstSurface, const int x, const int y,
			TOP op, const RECT * pOptionalClipRect = 0,
			const BLITFX * pOptionalBlitFX = 0
		) {

#if 1 // BPT 9/27/02

			m_VisualReference.Render(
				dstSurface,
				x,
				y,
				op,
				pOptionalClipRect,
				pOptionalBlitFX
			);

#else

			CVisualElement * pVisualElement = GetVisualElement();

			if ( pVisualElement ) {

				// Ask the element to render itself
				// ------------------------------------------------------------

				pVisualElement->Render(
					dstSurface, 
					m_VisualReference.GetX() + x,
					m_VisualReference.GetY() + y,
					op, pOptionalClipRect, 
					pOptionalBlitFX
				);

				// release our hold on the visual element

				pVisualElement->Release();

			}

#endif

		}

		// --------------------------------------------------------------------

		template< class SURFACE, class TOP > void 
		RenderThumbnail(
			SURFACE & dstSurface, const int x, const int y,
			TOP op, const RECT * pOptionalClipRect = 0,
			const BLITFX * pOptionalBlitFX = 0
		) {

			CVisualElement * pVisualElement = GetVisualElement();

			if ( pVisualElement ) {

				// Need to scale the coordinates to make sense...
				// Should the visual element expose a method?
				// ------------------------------------------------------------

// FIX!!!
// FIX!!!

				CPoint pt(
					m_VisualReference.GetX() + x, 
					m_VisualReference.GetY() + y
				);

// FIX!!!
// FIX!!!

				// Ask the element to render itself
				// ------------------------------------------------------------

				pVisualElement->RenderThumbnail(
					dstSurface, pt.x, pt.y,
					op, pOptionalClipRect, 
					pOptionalBlitFX
				);

				// release our hold on the visual element

				pVisualElement->Release();

			}

		}

		// --------------------------------------------------------------------

		bool BoundingRect( RECT & rect ) {

#if 1 // BPT 5/22/01

			rect = m_VisualReference.BoundingRect();

			return HasVisualElement();

#else

			CVisualElement * pVisualElement = GetVisualElement();

			if ( pVisualElement ) {

// FIX!!!
// FIX!!!

				rect = pVisualElement->BoundingRect( &m_VisualReference.GetPos() );

// FIX!!!
// FIX!!!

				// release our hold on the visual element

				pVisualElement->Release();

				return true;

			}

			SetRectEmpty( &rect );

			return false;

#endif

		}

		// --------------------------------------------------------------------

		bool HitTest(
			const int x, const int y, const int requiredStorageSize,
			CVisualElement::storage_type * pValue = 0 ) {

#if 1 // BPT 10/10/02

#if 1
			if ( requiredStorageSize ) {

				if ( !m_VisualReference.CheckStoredPixelSize( requiredStorageSize ) ) {

					return false;

				}

			}
#endif

			return m_VisualReference.HitTest( x, y, pValue );

#else

			CVisualElement * pVisualElement = GetVisualElement();

			if ( pVisualElement ) {

				// validate the storage type for the hit test.

				if ( requiredStorageSize && 
					(requiredStorageSize != pVisualElement->GetStoredPixelSize()) ) {

					pVisualElement->Release();

					return false;

				}

				// Need to ask the visual element for a hit test...

				bool bResult = pVisualElement->HitTest(
					x - m_VisualReference.GetX(), 
					y - m_VisualReference.GetY(), 
					pValue
				);

				pVisualElement->Release();

				return bResult;

			}

			return false;
#endif

		}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		template< class SHOWCASE >
		bool OptimizeVisualElements(
			SHOWCASE * pShowcase, typename SHOWCASE::optimize_map_type * pOptimizeMap
		) {

			if ( !HasVisualElement() ) return false;

			return pShowcase->OptimizeVisualElementReference( &m_VisualReference, pOptimizeMap );

		}

		bool AdjustVisualReferences(
			CVisualElement * pOldElement
	    	,CVisualElement * pNewElement
			,const POINT * pDelta
			,const POINT * pMoveTo
		);

#endif // BPT 5/22/01

#if 1 // BPT 6/1/01

		// Added this to help me build a gather use information for
		// visual elements

		template< class FN > bool TVisualElementFunctorDispatch( FN & functor ) {

			functor( this );

			CVisualElement * pVisualElement = GetVisualElement();

			if ( pVisualElement ) {

				bool bDidSomething = functor( pVisualElement );

				pVisualElement->Release();

				return bDidSomething;

			}

			return false;

		}

#endif

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CSimpleName {

	private: // Data

		char * m_pzName;

	private: // methods

		void DestroyName() {

			if ( m_pzName ) {

				free( m_pzName );

				m_pzName = 0;

			}

		}

	public: // Interface

		CSimpleName() : m_pzName( 0 ) { /* empty */ }

		~CSimpleName() {

			DestroyName();

		}

		// --------------------------------------------------------------------

		char * GetName() {

			return m_pzName;

		}

		bool SetName( const char * pzName ) {

			DestroyName();

			m_pzName = strdup( pzName );

			return (0 != m_pzName);

		}

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	CNamedSpot
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CNamedSpot : public CAnnotatedObject {

	public: // Traits

	private: // Data

		CSimpleName m_Name;

		POINT m_Location;

	public: // Interface

		CNamedSpot() {

			m_Location.x = 0;
			m_Location.y = 0;

		}

		// --------------------------------------------------------------------

		POINT GetLocation() const {

			return m_Location;

		}

		bool SetLocation( POINT location ) {

			m_Location = location;

			return true;

		}

		// --------------------------------------------------------------------

		char * GetName() {

			return m_Name.GetName();

		}

		bool SetName( const char * pszName ) {

			return m_Name.SetName( pszName );

		}

		// --------------------------------------------------------------------

		CNamedSpot * Copy() {

			CNamedSpot * pNew = new CNamedSpot;

			if ( !pNew ) { // BPT 6/26/01

				return 0; 

			}

			pNew->SetLocation( GetLocation() );
			pNew->SetName( GetName() );

			return pNew;
		}

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	CNamedSpotAdjustment
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CNamedSpotAdjustment : public CAnnotatedObject {

	public: // Traits

	private: // Data

		CNamedSpot * m_pNamedSpot;

		POINT m_Delta;

	public: // Interface

		CNamedSpotAdjustment() : m_pNamedSpot( 0 ) {

			m_Delta.x = 0;
			m_Delta.y = 0;

		}

		// --------------------------------------------------------------------

		CNamedSpot * GetNamedSpot() const {

			return m_pNamedSpot;

		}

		void SetNamedSpot( CNamedSpot * pNamedSpot ) {

			m_pNamedSpot = pNamedSpot;

		}

		// --------------------------------------------------------------------

		POINT GetDelta() const {

			return m_Delta;

		}

		void SetDelta( POINT delta ) {

			m_Delta = delta;

		}

		// --------------------------------------------------------------------

		POINT GetPosition() const {

			POINT pos = m_Delta;

			if ( m_pNamedSpot ) {

				POINT loc = m_pNamedSpot->GetLocation();

				pos.x += loc.x;
				pos.y += loc.y;

			}

			return pos;

		}

		// --------------------------------------------------------------------

		char * GetName() const {

			if ( !m_pNamedSpot ) {

				return 0;

			}

			return m_pNamedSpot->GetName();

		}

		// --------------------------------------------------------------------

		CNamedSpotAdjustment * Copy( CNamedSpot * namedSpot ) {

			CNamedSpotAdjustment *pNew = new CNamedSpotAdjustment;
			
			if ( !pNew ) { // BPT 6/26/01

				return 0; 

			}

			pNew->SetDelta( GetDelta() );
			pNew->SetNamedSpot( namedSpot );

			return pNew;
		}
		
		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Frame class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CFrame : public CAnnotatedObject {

	public: // Traits

		typedef std::list< CLayer * > layer_collection_type;
		typedef std::list< CNamedSpotAdjustment * > named_spot_adjustment_collection_type;

	private: // Data

		layer_collection_type m_Layers;

		named_spot_adjustment_collection_type m_NamedSpotAdjustments;

		// special case link points

	public:

		POINT m_Link1;
		POINT m_Link2;
		POINT m_Link3;
		POINT m_Link4;

	public: // Interface

		// --------------------------------------------------------------------

		int NamedSpotAdjustmentsCount() const {

			return m_NamedSpotAdjustments.size();

		}

		named_spot_adjustment_collection_type & NamedSpotAdjustmentsCollection() {

			return m_NamedSpotAdjustments;

		}

		void AttachNamedSpotAdjustment( CNamedSpotAdjustment * pNamedSpotAdjustment ) {

			m_NamedSpotAdjustments.push_back( pNamedSpotAdjustment );

		}

		void DeleteNamedSpotAdjustment( CNamedSpotAdjustment * pNamedSpotAdjustment ) {

			// Remove the named spot from the spot collection
			// ----------------------------------------------------------------

			named_spot_adjustment_collection_type::iterator adjustmentIt = m_NamedSpotAdjustments.begin();

			for ( ; adjustmentIt != m_NamedSpotAdjustments.end(); adjustmentIt++ ) {

				if ( (*adjustmentIt) == pNamedSpotAdjustment ) {

					delete (*adjustmentIt);

					m_NamedSpotAdjustments.erase( adjustmentIt );

					break;

				}

			}

		}

		CNamedSpotAdjustment * FindSpotAdjusterFor( CNamedSpot * pNamedSpot ) {

			named_spot_adjustment_collection_type::iterator adjustmentIt = m_NamedSpotAdjustments.begin();

			for ( ; adjustmentIt != m_NamedSpotAdjustments.end(); adjustmentIt++ ) {

				if ( (*adjustmentIt)->GetNamedSpot() == pNamedSpot ) {

					return (*adjustmentIt);

				}

			}

			return 0;

		}

		// --------------------------------------------------------------------

		CFrame() {
		
			m_Link1.x = 0;
			m_Link1.y = 0;

			m_Link2.x = 0;
			m_Link2.y = 0;

			m_Link3.x = 0;
			m_Link3.y = 0;

			m_Link4.x = 0;
			m_Link4.y = 0;

		}

		~CFrame() {

			// Delete the named spot adjustments
			// ----------------------------------------------------------------

			while ( !m_NamedSpotAdjustments.empty() ) {

				delete m_NamedSpotAdjustments.back();

				m_NamedSpotAdjustments.pop_back();

			}

			// Delete the layers
			// ----------------------------------------------------------------

			while ( !m_Layers.empty() ) {

				delete m_Layers.back();

				m_Layers.pop_back();

			}

		}

		// --------------------------------------------------------------------

		bool HasLinkPointInfo() {

			return 
				(m_Link1.x || m_Link1.y) ||
				(m_Link2.x || m_Link2.y) ||
				(m_Link3.x || m_Link3.y) ||
				(m_Link4.x || m_Link4.y);

		}

		// --------------------------------------------------------------------

		int LayerCount() const {

			return m_Layers.size();

		}

		layer_collection_type::iterator GetLayer( const int nLayer ) {

			layer_collection_type::iterator it = m_Layers.begin();

			for ( int counter = 0 ; it != m_Layers.end(); counter++ ) {

				if ( nLayer == counter ) {

					break;

				}

				++it;

			}

			return it;

		}

		CLayer * GetLayerPtr( const int nLayer ) {

			layer_collection_type::iterator it = GetLayer( nLayer );

			if ( it == m_Layers.end() ) {

				return 0;

			}

			return *it;

		}

#if 1 // BPT 6/1/01

		int LayerIndexFromPointer( CLayer * pLayer ) {

			layer_collection_type::iterator it = m_Layers.begin();

			for ( int counter = 0 ; it != m_Layers.end(); counter++ ) {

				if ( (*it) == pLayer ) {

					return counter;

				}

				++it;

			}

			return -1;

		}

#endif

		// --------------------------------------------------------------------

		bool DeleteLayer( layer_collection_type::iterator it ) {

			delete *it;

			m_Layers.erase( it );

			return true;

		}

		bool DeleteLayer( const int nLayer ) {

			layer_collection_type::iterator it = GetLayer( nLayer );

			if ( it == m_Layers.end() ) {

				return false;

			}

			return DeleteLayer( it );

		}

		// --------------------------------------------------------------------

		bool InsertNewLayersBefore( const int nLayers, layer_collection_type::iterator * pIT = 0 ) {

			// find where to insert the new layers
			// ----------------------------------------------------------------

			layer_collection_type::iterator it;

			if ( pIT ) {

				it = *pIT;

			} else {

				it = m_Layers.end();
			}

			// ----------------------------------------------------------------

			int tempID = LayerCount() + nLayers;

			for ( int i = 0; i < nLayers; i++ ) {

				CLayer * pNew = new CLayer();

				if ( !pNew ) {

					TRACE( "Unable to create %d of %d layers\n", i + 1, nLayers );

					return false;

				}

				if ( !pNew->Init( --tempID ) ) {

					TRACE( "Unable to Init() %d of %d layers\n", i + 1, nLayers );

					delete pNew;

					return false;

				}

				it = m_Layers.insert( it, pNew );

			}

			return true;

		}

		bool InsertNewLayersBefore( const int nLayers, const int nLayer ) {

			layer_collection_type::iterator it = GetLayer( nLayer );

			return InsertNewLayersBefore( nLayers, &it );

		}

		// --------------------------------------------------------------------

		template< class SURFACE, class TOP, class FUNCTOR > 
		void __forceinline RenderSingleTOP(
			SURFACE & dstSurface, const int x, const int y,
			TOP op, FUNCTOR & fn, const RECT * pOptionalClipRect = 0,
			const BLITFX * pOptionalBlitFX = 0
		) {

			layer_collection_type::iterator it = m_Layers.begin();

			for ( int i = 0; it != m_Layers.end(); it++, i++ ) {

				if ( fn( i ) ) {

					if ( (*it)->HasVisualElement() ) {

						(*it)->Render( dstSurface, x, y, op, pOptionalClipRect );

					}

				}

			}

		}

		template< class SURFACE, class GETLAYERTOPFN, class FUNCTOR > 
		void __forceinline Render(
			SURFACE & dstSurface, const int x, const int y,
			GETLAYERTOPFN & fnGetLayerTop, FUNCTOR & fn, const RECT * pOptionalClipRect = 0,
			const BLITFX * pOptionalBlitFX = 0
		) {

			layer_collection_type::iterator it = m_Layers.begin();

			for ( int i = 0; it != m_Layers.end(); it++, i++ ) {

				if ( fn( i ) ) {

					if ( (*it)->HasVisualElement() ) {

						(*it)->Render( 
							dstSurface, x, y, 
							fnGetLayerTop( i ), 
							pOptionalClipRect
						);

					}

				}

			}

		}

		// for_each
		// --------------------------------------------------------------------

		template< class T >
		void for_each( T fn ) {

			std::for_each( m_Layers.begin(), m_Layers.end(), fn );

		}

		template< class P, class T >
		void predicate_for_each( P pred, T fn ) {

			layer_collection_type::iterator it = m_Layers.begin();

			for ( ; it != m_Layers.end(); it++ ) {

				if ( pred( *it ) ) {

					fn( *it );

				}

			}

		}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		template< class SHOWCASE >
		bool OptimizeVisualElements(
			SHOWCASE * pShowcase, typename SHOWCASE::optimize_map_type * pOptimizeMap
		) {

			bool bDidSomething = false;

			layer_collection_type::iterator it = m_Layers.begin();

			for ( ; it != m_Layers.end(); it++ ) {

				if ( (*it)->OptimizeVisualElements( pShowcase, pOptimizeMap ) ) {

					bDidSomething = true;

				}

			}

			return bDidSomething;

		}

		bool AdjustVisualReferences(
			CVisualElement * pOldElement
	    	,CVisualElement * pNewElement
			,const POINT * pDelta
			,const POINT * pMoveTo
		);

#endif // BPT 5/22/01

#if 1 // BPT 6/1/01

		// Added this to help me build a gather use information for
		// visual elements

		template< class FN > bool TVisualElementFunctorDispatch( FN & functor ) {

			bool bDidSomething = false;

			layer_collection_type::iterator it = m_Layers.begin();

			for ( ; it != m_Layers.end(); it++ ) {

				functor( *it );

				if ( (*it)->TVisualElementFunctorDispatch( functor ) ) {

					bDidSomething = true;

				}

			}

			return bDidSomething;

		}

#endif

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Animation context class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CAnimationContext {
	
	private:
	
		int m_frame;
		int m_min_frame;
		int m_max_frame;
	
		int m_layer;
		int m_min_layer;
		int m_max_layer;
			
		int m_beginSelectedFrames, m_endSelectedFrames;
		int m_beginSelectedLayers, m_endSelectedLayers;
		
	public:
	
		CAnimationContext() {
	
			m_frame = 0;
			m_min_frame = 0;
			m_max_frame = 0;
			
			m_layer = 0;
			m_min_layer = 0;
			m_max_layer = 0;
	
		}
	
		// --------------------------------------------------------------------
	
		int Frame() {
	
			return m_frame;
	
		}

		void SetSelectedFrames(int begin, int end) {

			m_beginSelectedFrames = begin;
			m_endSelectedFrames = end;

		}

		void SetSelectedLayers(int begin, int end) {
			
			m_beginSelectedLayers = begin;
			m_endSelectedLayers = end;
			
		}

		int GetBeginSelectedFrames() {

			return m_beginSelectedFrames;

		}

		int GetEndSelectedFrames() {
			
			return m_endSelectedFrames;
			
		}

		int GetBeginSelectedLayers() {
			
			return m_beginSelectedLayers;
			
		}

		int GetEndSelectedLayers() {
			
			return m_endSelectedLayers;
			
		}

		int SetFrameRange( const int minFrame, const int maxFrame ) {
	
			m_min_frame = minFrame;
			m_max_frame = maxFrame;
	
			m_frame = max( m_min_frame, min( m_frame, m_max_frame ) );
	
			return m_frame;
	
		}
	
		int FirstFrame() {

			m_frame = m_min_frame;

			return m_frame;
		}
	
		int LastFrame() {

			m_frame = m_max_frame;

			return m_frame;

		}

 		void SetFrame( const int nFrame ) {

			m_frame = max( m_min_frame, min( nFrame, m_max_frame ) );

		}

		int NextFrame( const bool bWrap = true ) {
	
			if ( ++m_frame > m_max_frame ) {
	
				m_frame = m_min_frame;
	
			}
	
			return m_frame;
	
		}
	
		int PrevFrame( const bool bWrap = true ) {
	
			if ( --m_frame < m_min_frame ) {
	
				m_frame = m_max_frame;
	
			}
	
			return m_frame;
	
		}
	
		// --------------------------------------------------------------------
	
		int Layer() {
	
			return m_layer;
	
		}
	
		int SetLayerRange( const int minLayer, const int maxLayer ) {
	
			m_min_layer = minLayer;
			m_max_layer = maxLayer;
	
			m_layer = max( m_min_layer, min( m_layer, m_max_layer ) );
	
			return m_layer;
	
		}

		int FirstLayer() {

			m_layer = m_min_layer;

			return m_layer;

		}
	
		int LastLayer() {

			m_layer = m_max_layer;

			return m_layer;

		}

 		void SetLayer( const int nLayer ) {

			m_layer = max( m_min_layer, min( nLayer, m_max_layer ) );

		}

		int NextLayer( const bool bWrap = true ) {
	
			if ( ++m_layer > m_max_layer ) {
	
				m_layer = m_min_layer;
	
			}
	
			return m_layer;
	
		}
	
		int PrevLayer( const bool bWrap = true ) {
	
			if ( --m_layer < m_min_layer ) {
	
				m_layer = m_max_layer;
	
			}
	
			return m_layer;
	
		}
	
	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	Palette class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CAnnotatedPalette : public CAnnotatedObject {

	public: // traits

		enum {

			FIX_PAL_SIZE_POW2 = 8,
			FIXED_PAL_SIZE = (1 << FIX_PAL_SIZE_POW2)

		};

		typedef COLORREF color_storage_type;

#if defined(TEST_BPAINT_TRUE_COLOR_TEST) // bpt 4/24/03

		typedef unsigned clut_entry_type;

#else

		typedef int clut_entry_type;

#endif

		typedef BPT::TPow2Clut<clut_entry_type,FIX_PAL_SIZE_POW2> clut_type;

		enum PFLAGS {

			UNKNOWN					= 0x00000000
			,PFLAG_LOCKED			= 0x80000000
			,PFLAG_FROM_PARENT		= 0x00000001

		};

	private: // data

		color_storage_type m_Colors[ FIXED_PAL_SIZE ];

		clut_type m_Clut; // BPT 6/15/01

		DWORD m_Flags[ FIXED_PAL_SIZE ]; // BPT 6/19/01

		CString m_Notes[ FIXED_PAL_SIZE ]; // BPT 6/19/01

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		int m_ReferenceCount; // BPT 6/5/02

#endif // defined(BPT_SHOWCASE_MANAGED_PALETTES) BPT 6/5/02

	public: // interface

		// constructor
		// --------------------------------------------------------------------

		CAnnotatedPalette() {
			
#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			m_ReferenceCount = 0;

#endif // defined(BPT_SHOWCASE_MANAGED_PALETTES) BPT 6/5/02

			Default( 0 );
		
		}

		// managed palette related code 
		// --------------------------------------------------------------------

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

	private:

		CSimpleName m_Name;

	public:

		void AddRef() {

			++m_ReferenceCount;

		}

		void Release() {

			if ( 0 >= --m_ReferenceCount ) {

				delete this;

			}

		}

		int RefCount() {

			return m_ReferenceCount;

		}

		// --------------------------------------------------------------------

		char * GetName() {

			return m_Name.GetName();

		}

		bool SetName( const char * pzName ) {

			return m_Name.SetName( pzName );

		}

#endif // defined(BPT_SHOWCASE_MANAGED_PALETTES) BPT 6/5/02

		// --------------------------------------------------------------------

		void SetSlot( const int slot, const int r, const int g, const int b ) {

			m_Colors[ slot ] = RGB( r, g, b );

#if 1 && defined(TEST_BPAINT_TRUE_COLOR_TEST) // hack to test per slot alpha

			unsigned t = static_cast<clut_entry_type>(
				MakeDisplayPixelType( r, g, b )
			); // BPT 6/15/01

			if ( 127 < slot ) {

				t = (t & (~0xff000000)) | (slot << 24);

			}

			m_Clut[ slot ] = t;

#else

			m_Clut[ slot ] = static_cast<clut_entry_type>(
				MakeDisplayPixelType( r, g, b )
			); // BPT 6/15/01

#endif

#if defined(TEST_BPAINT_TRUE_COLOR_TEST)

			// The chromakey is special it always is transparent!

			if ( 0 == slot ) {

				m_Clut[ slot ] &= ~0xFF000000;

			}

#endif

		}

		// --------------------------------------------------------------------

		CString & GetSlotNote( const int slot ) {

			return m_Notes[ slot ];

		}

		void ClearSlotNote( const int slot ) {

			m_Notes[ slot ].Empty();

		}

		void SetSlotNote( const int slot, CString & string ) {

			if ( string.IsEmpty() ) {

				ClearSlotNote( slot );

			} else {

				m_Notes[ slot ] = string;

			}

		}

		DWORD GetSlotFlags( const int slot ) {

			return m_Flags[ slot ];

		}

		void ChangeSlotFlags( const int slot, const DWORD set, const DWORD clear ) {

			m_Flags[ slot ] &= ~clear;
			m_Flags[ slot ] |= set;

		}

		// --------------------------------------------------------------------

		int TotalColors() const {

			return FIXED_PAL_SIZE;

		}

		// Helper methods
		// --------------------------------------------------------------------

		int GetSlot_R( const int slot ) { return GetRValue( m_Colors[slot] ); }
		int GetSlot_G( const int slot ) { return GetGValue( m_Colors[slot] ); }
		int GetSlot_B( const int slot ) { return GetBValue( m_Colors[slot] ); }

		color_storage_type GetSlot( const int slot ) { return m_Colors[ slot ]; }

		// Display color table related methods.
		// --------------------------------------------------------------------

		clut_entry_type GetCLUTValue( const int slot ) {

			return m_Clut[ slot ];

		}

		clut_type * GetDisplayCLUT() {

			return &m_Clut;

		}

		// --------------------------------------------------------------------

		bool RebuildDisplayCLUT() {

			int nTotalColors = TotalColors();

			for ( int i = 0; i < nTotalColors; i++ ) {

				COLORREF temp = GetSlot( i );

				SetSlot( i, GetRValue(temp), GetGValue(temp), GetBValue(temp) );

			}

			return true;

		}

		// --------------------------------------------------------------------

		COLORREF fHSVTest( float h0t1, float s0t1, float v0t2, float c = 255.0f )
		{
			float h = h0t1;// * 360.0f;
			float s = s0t1;
			float v = s0t1;
		
			if ( 360.0f == h ) h = 0.0f;
		
			h = h / 60.0f;
		
			float i = floorf( h );
			float f = h - i;
			float p = v * (1.0f - s);
			float q = v * (1 - (s * f));
			float t = v * (1 - (s * (1 - f)));
		
			switch ( (int)i ) {
		
			case 0: return RGB( v * c, t * c, p * c ); break;
			case 1: return RGB( q * c, v * c, p * c ); break;
			case 2: return RGB( p * c, v * c, t * c ); break;
			case 3: return RGB( p * c, q * c, v * c ); break;
			case 4: return RGB( t * c, p * c, v * c ); break;
			case 5: return RGB( v * c, p * c, q * c ); break;
		
			}
		
			return 0;
		}

		void Default( const int id ) {

			// set the flags and strings
			// ----------------------------------------------------------------

			for ( int ic = 0; ic < TotalColors(); ic++ ) {

				m_Flags[ ic ] = 0;
				m_Notes[ ic ].Empty();

			}

			//
			// ----------------------------------------------------------------

#if 1

			int colorSlot = 0;

			for ( int ir = 0; ir < 8; ir++ ) {

				int r = (ir * 255) / 7;

				for ( int ig = 0; ig < 8; ig++ ) {

					int g = (ig * 255) / 7;

					for ( int ib = 0; ib < 4; ib++ ) {

						int b = (ib * 255) / 3;

						SetSlot( colorSlot++, r, g, b );

					}

				}

			}

#else

			// THIS WILL NEED TO CHANGE WHEN THE NUMBER OF COLORS DOES!!!
			// THIS WILL NEED TO CHANGE WHEN THE NUMBER OF COLORS DOES!!!
			// THIS WILL NEED TO CHANGE WHEN THE NUMBER OF COLORS DOES!!!
			// THIS WILL NEED TO CHANGE WHEN THE NUMBER OF COLORS DOES!!!

			float h = 0.0f;
			float hd = 360.0f / 16.0f;
		
			int n = 16;
		
			for ( int hc = 0; hc < 16; hc++ ) {
		
				// core
		
				int i = hc * 16;
		
				int z = 0;
		
				for ( int vc = 0; vc < 8; vc++ ) {
					
					float v = (float)vc / 7.0f;
		
					for ( int sc = 0; sc < 2; sc++ ) {
		
						float s = (float) z / (float)(n - 1 );

	#if 1 // BPT 6/15/01

						COLORREF temp = fHSVTest( h, 1.0f - s, 1.0f - v );

						SetSlot( i++, GetRValue(temp), GetGValue(temp), GetBValue(temp) );

	#else
		
						m_Colors[ i++ ] = fHSVTest( h, 1.0f - s, 1.0f - v );

	#endif
		
						++z;
		
					}
		
				}
		
				h = h + hd;
		
			}

#endif

		}

		// --------------------------------------------------------------------

		void CopySlotFrom(
			const int srcSlot, const int dstSlot, 
			CAnnotatedPalette * pFrom, const bool bCopyFlags
		) {

			m_Colors[ dstSlot ] = pFrom->m_Colors[ srcSlot ];
			m_Clut[ dstSlot ] = pFrom->m_Clut[ srcSlot ];
			m_Notes[ dstSlot ] = pFrom->m_Notes[ srcSlot ];

			if ( bCopyFlags ) {

				m_Flags[ dstSlot ] = pFrom->m_Flags[ srcSlot ];

			}

		}

		// --------------------------------------------------------------------

		void CopyColorsFrom( CAnnotatedPalette * pFrom, const bool bCopyFlags = true ) {

			// ----------------------------------------------------------------

			if ( TotalColors() != pFrom->TotalColors() ) {

				return /* nop */;

			}

			// ----------------------------------------------------------------

			for ( int i = 0; i < TotalColors(); i++ ) {

				CopySlotFrom( i, i, pFrom, bCopyFlags );

			}

		}

		// --------------------------------------------------------------------

		bool PaletteRangeTheSameColors( CAnnotatedPalette * pOther, const int nMin, const int nMax ) {

			// safety check
			// ----------------------------------------------------------------

			if ( TotalColors() != pOther->TotalColors() ) {

				return false;

			}

			if ( (0 > nMin) || (TotalColors() < nMin) || 
				(0 > nMax) || (TotalColors() < nMax) ) {

				return false;

			}

			// ----------------------------------------------------------------

			for ( int i = nMin; i <= nMax; i++ ) {

				if ( GetSlot( i ) != pOther->GetSlot( i ) ) {

					return false;

				}

			}

			return true;

		}

		bool PalettesTheSameColors( CAnnotatedPalette * pOther ) {

			return PaletteRangeTheSameColors( pOther, 0, TotalColors() );

		}

		// --------------------------------------------------------------------

#if !defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		CAnnotatedPalette * Duplicate() {

			CAnnotatedPalette * pPal = new CAnnotatedPalette;

			if ( !pPal ) {

				return 0;

			}

			pPal->CopyColorsFrom( this );

			return pPal;

		}

#endif // !defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		// Shared palette entries
		// --------------------------------------------------------------------

		void ManageSharedPaletteEntries( CAnnotatedPalette * pParentPalette ) {

			if ( pParentPalette ) {

				for ( int i = 0; i < TotalColors(); i++ ) {

					if ( PFLAG_FROM_PARENT & m_Flags[ i ] ) {

						CopySlotFrom( i, i, pParentPalette, false );

					}

				}

			} else {

				for ( int i = 0; i < TotalColors(); i++ ) {

					if ( PFLAG_FROM_PARENT & m_Flags[ i ] ) {

						SetSlot( i, 255, 255, 255 );

						ClearSlotNote( i );

					}

				}

			}

		}

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//	CLayerInfo
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CLayerInfo : public CAnnotatedObject {

	public: // Traits

		typedef std::list< CFrame * > frame_collection_type;

		enum {

			INVISIBLE			= 0x00000001
			,LOCKED				= 0x00000002
			,OUTLINE_COLOR_A	= 0x00010000
			,OUTLINE_COLOR_B	= 0x00020000

		};

	private: // Data

		CSimpleName m_Name;
		DWORD m_dwFlags;
		CAnnotatedPalette * m_pPalette;

	private: // methods

	public: // Interface

		CLayerInfo() : m_dwFlags( 0 ) {

			m_pPalette = 0; // BPT 6/15/01

		
		}

		~CLayerInfo() {

			DestroyPalette();
		
		}

		// --------------------------------------------------------------------

		char * GetName() {

			return m_Name.GetName();

		}

		bool SetName( const char * pzName ) {

			return m_Name.SetName( pzName );

		}

		// --------------------------------------------------------------------

		DWORD GetFlags() const {

			return m_dwFlags;

		}

		void SetFlags( const DWORD dwFlags ) {

			m_dwFlags = dwFlags;

		}

		void ModifyFlags( DWORD dwRemove, DWORD dwAdd ) {

			m_dwFlags = (m_dwFlags & (~dwRemove)) | dwAdd;

		}

		bool CheckFlags( const DWORD dwFlags, const bool bAnyCheck = true ) {

			if ( bAnyCheck ) {

				return ( m_dwFlags & dwFlags ) ? true : false;

			}

			return ( dwFlags == (m_dwFlags & dwFlags) ) ? true : false;

		}

		// --------------------------------------------------------------------

#if 1 // BPT 6/15/01

		CAnnotatedPalette * GetPalette() {

			return m_pPalette;

		}

		void DestroyPalette() {

			if ( m_pPalette ) {

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

				m_pPalette->Release();

#else

				delete m_pPalette;

#endif

				m_pPalette = 0;

			}

		}

		bool AdoptPalette( CAnnotatedPalette * pNewPalette ) {

			DestroyPalette();

			m_pPalette = pNewPalette;

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			if ( pNewPalette ) {

				pNewPalette->AddRef();

			}

#endif
			// other processing here...

			return true;

		}

#if !defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		bool CreatePalette() {

			if ( m_pPalette ) {

				return true;

			}

			CAnnotatedPalette * pNew = new CAnnotatedPalette;

			if ( !pNew ) {

				return false;

			}

			return AdoptPalette( pNew );

		}

		bool CreatePaletteFrom( CAnnotatedPalette * pSource ) {

			CAnnotatedPalette * pNew = pSource->Duplicate();

			if ( !pNew ) {

				return false;

			}

			return AdoptPalette( pNew );

		}

#endif // !defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		void ManageSharedPaletteEntries( CAnnotatedPalette * pParentPalette ) {

			if ( m_pPalette ) {

				m_pPalette->ManageSharedPaletteEntries( pParentPalette );

			}

		}

#endif // BPT 6/15/01

		// --------------------------------------------------------------------

		bool CopyFrom( CLayerInfo * pLayerInfo ) { // BPT 6/27/01

			SetFlags( pLayerInfo->GetFlags() );

			SetName( pLayerInfo->GetName() );

			SetNote( pLayerInfo->GetNote() );

			DestroyPalette();

			CAnnotatedPalette * pPalette = pLayerInfo->GetPalette();

			if ( pPalette ) {

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

				if ( !AdoptPalette( pPalette ) ) {

					return false;

				}

#else

				if ( !CreatePaletteFrom( pPalette ) ) {

					return false;

				}

#endif

			}

			return true;

		}

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Animation backdrop
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CAnimationBackdrop : public CAnnotatedObject {

	public: // Traits

	private: // Data

		CVisualElement * m_pVisualElement;

	private: // methods

	public: // Interface

		// --------------------------------------------------------------------

		CAnimationBackdrop() : m_pVisualElement( 0 ) { /* Empty */ }

		~CAnimationBackdrop() {

			Destroy();

		}

		// --------------------------------------------------------------------

		void Destroy() {

			if ( m_pVisualElement ) {

				delete m_pVisualElement;

				m_pVisualElement = 0;

			}

		}

		// --------------------------------------------------------------------

		bool HasVisualElement() {

			return (0 != m_pVisualElement);

		}

		// Need to make a load composite compatible backdrop.
		// --------------------------------------------------------------------

		bool LoadFromFilename( const char * filename, const SIZE & canvasSize );

		// Need to make a rendering method!
		// --------------------------------------------------------------------

		template< class SURFACE, class TOP >
		bool __forceinline Render(
			SURFACE & dstSurface, TOP op, const RECT * pOptionalClipRect = 0
		) {

			// Error check
			// ----------------------------------------------------------------

			if ( !HasVisualElement() ) {

				return false;

			}

			// Finally
			// ----------------------------------------------------------------

			return m_pVisualElement->Render(
				dstSurface, 0, 0, op, pOptionalClipRect
			);

		}

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Animation class
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CAnimation : public CAnnotatedObject {

	public: // Traits

		typedef std::list< CFrame * > frame_collection_type;
		typedef std::list< CLayerInfo * > layer_info_collection_type;
		typedef std::list< CNamedSpot * > named_spot_collection_type;

		enum POLICY_FLAGS { // BPT 6/17/01

			UNKNOWN							= 0x00000000
			,ENABLE_ANIMATION_PALETTE		= 0x00000001
			,ENABLE_LAYER_PALETTES			= 0x00000002

		};

	private: // Data

		layer_info_collection_type m_LayerInfo;

		frame_collection_type m_Frames;

		int m_LayerCount;

		CSimpleName m_Name;

		CAnimationBackdrop m_Backdrop;

		int m_PlaybackDelay;

		named_spot_collection_type m_NamedSpots;

		// special case link points

		CAnnotatedPalette * m_pPalette; // BPT 6/15/01

		DWORD m_Policies; // BPT 6/17/01

#if 1 // BPT 6/27/01

		int m_nOutlineColorA;
		int m_nOutlineColorB;

#endif

	public:

		POINT m_Link1;

	private: // methods

	public: // Interface

		// Policy methods (BPT 6/17/01)
		// --------------------------------------------------------------------

		void POLICY_ChangeSet1( const DWORD set, const DWORD clear ) {

			m_Policies &= ~clear;
			m_Policies |= set;

		}

		void POLICY_AnimationPalette( const bool bEnable ) {

			if ( bEnable ) {

				POLICY_ChangeSet1( ENABLE_ANIMATION_PALETTE, 0 );

			} else {

				POLICY_ChangeSet1( 0, ENABLE_ANIMATION_PALETTE );

			}

		}

		void POLICY_LayerPalettes( const bool bEnable ) {

			if ( bEnable ) {

				POLICY_ChangeSet1( ENABLE_LAYER_PALETTES, 0 );

			} else {

				POLICY_ChangeSet1( 0, ENABLE_LAYER_PALETTES );

			}

		}

		bool AnimationPaletteEnabled() const {

			return (m_Policies & ENABLE_ANIMATION_PALETTE) ? true : false;

		}

		bool LayerPalettesEnabled() const {

			return (m_Policies & ENABLE_LAYER_PALETTES) ? true : false;

		}

		bool MultiplePaletteSupport() const {

			return (AnimationPaletteEnabled() || LayerPalettesEnabled());

		}

		// --------------------------------------------------------------------

		int NamedSpotCount() const {

			return m_NamedSpots.size();

		}

		named_spot_collection_type & NamedSpotCollection() {

			return m_NamedSpots;

		}

		void AttachNamedSpot( CNamedSpot * pNamedSpot ) {

			m_NamedSpots.push_back( pNamedSpot );

		}

		CNamedSpot *FindNamedSpot( const char * spotName ) {

			named_spot_collection_type::iterator spotIt = m_NamedSpots.begin();
			
			for ( ; spotIt != m_NamedSpots.end(); spotIt++ ) {
				
				if ( stricmp((*spotIt)->GetName(), spotName) == 0) {
					
					return *spotIt;			
					
				}
				
			}

			return NULL;
		}

		void DeleteNamedSpot( CNamedSpot * pNamedSpot ) {

			// Remove references to this named spot used in frames
			// ----------------------------------------------------------------

			frame_collection_type::iterator frameIt = m_Frames.begin();

			for ( ; frameIt != m_Frames.end(); frameIt++ ) {

				CNamedSpotAdjustment * pSpotAdjustment = (*frameIt)->FindSpotAdjusterFor( pNamedSpot );

				if ( pSpotAdjustment ) {

					(*frameIt)->DeleteNamedSpotAdjustment( pSpotAdjustment );

				}

			}

			// Remove the named spot from the spot collection
			// ----------------------------------------------------------------

			named_spot_collection_type::iterator spotIt = m_NamedSpots.begin();

			for ( ; spotIt != m_NamedSpots.end(); spotIt++ ) {

				if ( (*spotIt) == pNamedSpot ) {

					delete (*spotIt);

					m_NamedSpots.erase( spotIt );

					break;

				}

			}

		}

		// --------------------------------------------------------------------

		CAnimation() : m_LayerCount( 0 ) {
		
		// special case link points

			m_Link1.x = 0;
			m_Link1.y = 0;

			m_PlaybackDelay = 100;

			m_pPalette = 0; // BPT 6/15/01

			m_Policies = 0; // BPT 6/17/01

#if 1 // BPT 6/27/01

			m_nOutlineColorA = 0;
			m_nOutlineColorB = 1;

#endif

		}

		~CAnimation() {

			DestroyPalette(); // BPT 6/15/01

			// Delete the named spots
			// ----------------------------------------------------------------

			while ( !m_NamedSpots.empty() ) {

				delete m_NamedSpots.back();

				m_NamedSpots.pop_back();

			}

			// delete the 'frames'
			// ----------------------------------------------------------------

			while ( !m_Frames.empty() ) {

				delete m_Frames.back();

				m_Frames.pop_back();

			}

			// delete the 'layer info'
			// ----------------------------------------------------------------

			while ( !m_LayerInfo.empty() ) {

				delete m_LayerInfo.back();

				m_LayerInfo.pop_back();

			}

		}

		// --------------------------------------------------------------------

		int GetPlaybackDelay() const {

			return m_PlaybackDelay;

		}

		void SetPlaybackDelay( const int delay ) {

			m_PlaybackDelay = delay;

		}

		// --------------------------------------------------------------------

		bool HasBackdrop() {

			return m_Backdrop.HasVisualElement();

		}

		void DestroyBackdrop() {

			m_Backdrop.Destroy();

		}

		bool LoadBackdrop( const char * filename, const SIZE & canvasSize ) {

			return m_Backdrop.LoadFromFilename( filename, canvasSize );

		}

		template< class SURFACE, class TOP >
		bool __forceinline RenderBackdrop( 
			SURFACE & dstSurface, TOP op, const RECT * pOptionalClipRect = 0
		) {

			return m_Backdrop.Render( dstSurface, op, pOptionalClipRect );

		}

		// --------------------------------------------------------------------

		char * GetName() {

			return m_Name.GetName();

		}

		bool SetName( const char * pzName ) {

			return m_Name.SetName( pzName );

		}

		// --------------------------------------------------------------------

		int LayerCount() const {

			return m_LayerCount;

		}

		int FrameCount() const {

			return m_Frames.size();

		}

		// --------------------------------------------------------------------

		frame_collection_type & GetFrameCollection() {

			return m_Frames;

		}

		frame_collection_type::iterator GetFrame( const int nFrame ) {

			frame_collection_type::iterator it = m_Frames.begin();

			for ( int counter = 0 ; it != m_Frames.end(); counter++ ) {

				if ( nFrame == counter ) {

					break;

				}

				++it;

			}

			return it;

		}

		CFrame * GetFramePtr( const int nFrame ) {

			frame_collection_type::iterator it = GetFrame( nFrame );

			if ( it == m_Frames.end() ) {

				return 0;

			}

			return *it;

		}

		CFrame * ActiveFrame() {

			return GetFramePtr( m_AnimationContext.Frame() );

		}

#if 1 // BPT 6/1/01

		int FrameIndexFromPointer( CFrame * pFrame ) {

			frame_collection_type::iterator it = m_Frames.begin();

			for ( int counter = 0 ; it != m_Frames.end(); counter++ ) {

				if ( (*it) == pFrame ) {

					return counter;

				}

				++it;

			}

			return -1;

		}

#endif

		// Interaction interface this is lame!
		// --------------------------------------------------------------------

		CAnimationContext m_AnimationContext;

		CLayer * GetLayer( const int nFrame, const int nLayer ) {

			CFrame * pFrame = GetFramePtr( nFrame );

			if ( !pFrame ) {

				return 0;

			}

			return pFrame->GetLayerPtr( nLayer );

		}

		CLayer * GetLayer( const int nLayer ) {

			return GetLayer( m_AnimationContext.Frame(), nLayer );

		}

		CLayer * ActiveLayer() {

			return GetLayer( m_AnimationContext.Layer() );

		}

		// -----------------------

		layer_info_collection_type::iterator GetLayerInfoIterator( const int nLayer ) {

			layer_info_collection_type::iterator it = m_LayerInfo.begin();

			for ( int counter = 0 ; it != m_LayerInfo.end(); counter++ ) {

				if ( nLayer== counter ) {

					return it;

				}

				++it;

			}

			return m_LayerInfo.end();

		}

		CLayerInfo * GetLayerInfo( const int nLayer ) {

			layer_info_collection_type::iterator it = GetLayerInfoIterator( nLayer );

			if ( it != m_LayerInfo.end() ) {

				return *it;

			}

			return 0;

		}

		// -----------------------

		int CurrentFrame() {

			return m_AnimationContext.Frame();

		}

		void SetFrame( const int nFrame ) {

			m_AnimationContext.SetFrame( nFrame );

		}

		void FirstFrame() {

			m_AnimationContext.FirstFrame();

		}

		void PrevFrame() {

			m_AnimationContext.PrevFrame();

		}

		void NextFrame() {

			m_AnimationContext.NextFrame();

		}

		void LastFrame() {

			m_AnimationContext.LastFrame();

		}

		// -----------------------

		int CurrentLayer() {

			return m_AnimationContext.Layer();

		}

		void SetLayer( const int nLayer ) {

			m_AnimationContext.SetLayer( nLayer );

		}

		void FirstLayer() {

			m_AnimationContext.FirstLayer();

		}

		void PrevLayer() {

			m_AnimationContext.PrevLayer();

		}

		void NextLayer() {

			m_AnimationContext.NextLayer();

		}

		void LastLayer() {

			m_AnimationContext.LastLayer();

		}

		// -----------------------

		void SetSelectedFrames(int begin, int end) {
			
			m_AnimationContext.SetSelectedFrames(begin, end);
			
		}
		
		void SetSelectedLayers(int begin, int end) {
			
			m_AnimationContext.SetSelectedLayers(begin, end);
			
		}

		int GetBeginSelectedFrames() {
			
			return m_AnimationContext.GetBeginSelectedFrames();
			
		}

		int GetEndSelectedFrames() {
			
			return m_AnimationContext.GetEndSelectedFrames();
			
		}

		int GetBeginSelectedLayers() {
			
			return m_AnimationContext.GetBeginSelectedLayers();
			
		}

		int GetEndSelectedLayers() {
			
			return m_AnimationContext.GetEndSelectedLayers();
			
		}

		// -----------------------

		void SyncInternals() {

			m_AnimationContext.SetLayerRange( 0, LayerCount() - 1 );
			m_AnimationContext.SetFrameRange( 0, FrameCount() - 1 );

		}

		// Animation layer operations (affects all frames...)
		// --------------------------------------------------------------------

		bool DeleteLayer( const int nLayer ) {

			// error check

			if ( (0 > nLayer) || (nLayer >= LayerCount()) ) {

				return false;

			}

			// pefrorm the operation on the frames
			// ----------------------------------------------------------------

			frame_collection_type::iterator it = m_Frames.begin();

			for ( ; it != m_Frames.end(); it++ ) {

				if ( !(*it)->DeleteLayer( nLayer ) ) {

					return false;

				}

			}

			// Manage the layer information collection
			// ----------------------------------------------------------------

			layer_info_collection_type::iterator layerIt = m_LayerInfo.begin();

			for ( int counter = 0 ; layerIt != m_LayerInfo.end(); counter++ ) {

				if ( nLayer == counter ) {

					delete *layerIt;

					m_LayerInfo.erase( layerIt );

					--m_LayerCount;

					break;

				}

				++layerIt;

			}

			// ----------------------------------------------------------------

			SyncInternals();

			return true;

		}

		bool InsertNewLayers( const int nLayers, const int nLayer ) {

			// pefrorm the frame operations
			// ----------------------------------------------------------------

			frame_collection_type::iterator it = m_Frames.begin();

			for ( ; it != m_Frames.end(); it++ ) {

				if ( !(*it)->InsertNewLayersBefore( nLayers, nLayer ) ) {

					return false;

				}

			}

			// Manage the layer information collection
			// ----------------------------------------------------------------

			layer_info_collection_type::iterator insertPos = GetLayerInfoIterator( nLayer );

			char layerName[ _MAX_PATH ];

			m_LayerCount = m_LayerInfo.size();

			int potentialNumber = m_LayerCount + nLayers - 1;

			for ( int i = 0; i < nLayers; i++, m_LayerCount++ ) {

				CLayerInfo * pNew = new CLayerInfo();

				if ( !pNew ) {

					return false;

				}

				// generate name
				// ------------------------------------------------------------

				int set = (potentialNumber / 26);
		
				if ( set ) {
			
					sprintf( layerName, "%c%d", 'a' + (potentialNumber % 26), set );
		
				} else {
		
					sprintf( layerName, "%c", 'a' + (potentialNumber % 26) );
		
				}

				--potentialNumber;

				if ( !pNew->SetName( layerName ) ) {

					return false;

				}

				// ------------------------------------------------------------

				insertPos = m_LayerInfo.insert( insertPos, pNew );

			}

			// ----------------------------------------------------------------

			SyncInternals();

			return true;

		}

		// --------------------------------------------------------------------

		bool ExpandNestedVisuals() { // BPT 10/14/02

			// pefrorm the frame operations
			// ----------------------------------------------------------------

			frame_collection_type::iterator it = m_Frames.begin();

			for ( ; it != m_Frames.end(); it++ ) {

				bool bRepeat = false;

				do {

					for ( int layer = 0; layer < m_LayerCount; layer++ ) {

						// get the layer and check for nested visuals
						// ----------------------------------------------------

						CFrame * pFrame = *it;

						CVisualElement * pVisualElement = 0;

						POINT containerPosition;

						{
							CLayer * pLayer = pFrame->GetLayerPtr( layer );

							if ( !pLayer ) continue;

							pVisualElement = pLayer->GetVisualElement();

							if ( !pVisualElement ) continue;

							if ( !pVisualElement->HasNestedVisualReferences() ) {
								
								pVisualElement->Release();

								continue;

							}

							containerPosition = pLayer->GetPos();

						}

						// count the empty layers above this layer
						// ----------------------------------------------------

						int nEmptyLayersAbove = 0;

						for ( int above = layer; 0 <= --above; ) {

							CLayer * pAboveLayer = pFrame->GetLayerPtr( above );

							if ( !pAboveLayer ) break;

							if ( pAboveLayer->HasVisualElement() ) break;

							++nEmptyLayersAbove;

						}

						// check to see if we need to insert new layers
						// ----------------------------------------------------

						int nestedVisualCount = pVisualElement->NestedVisualsCount();

						int newLayersNeededCount = (nestedVisualCount - nEmptyLayersAbove - 1);

						if ( 0 < newLayersNeededCount ) {

							if ( !InsertNewLayers(
								newLayersNeededCount, layer - nEmptyLayersAbove
							) ) {

								pVisualElement->Release();

								return false;

							}

							layer += newLayersNeededCount;

						}

						// pull the nested visuals out into the 
						// ----------------------------------------------------

						for ( int i = 0; i < nestedVisualCount; i++ ) {

							// Get the visual reference...

							CVisualElementReference * pNestedReference = pVisualElement->GetNestedVisualReference( i );

							if ( !pNestedReference ) continue; // error????

							CVisualElement * pNestedVisual = pNestedReference->GetVisualElement();

							if ( !pNestedVisual ) continue; // error???

							if ( pNestedVisual->HasNestedVisualReferences() ) bRepeat = true;

							// now 

							CLayer * pLayer = pFrame->GetLayerPtr( layer - i );

							if ( !pLayer ) {

								// ERROR!!!

								TRACE( "ERROR: Expand unable to get layer %d\n", layer - i );

								pNestedVisual->Release();

								pVisualElement->Release();

								return false;

							}

							if ( i ) { ASSERT( !pLayer->HasVisualElement() ); }

							POINT pos = pNestedReference->GetPos();

							pos.x += containerPosition.x;
							pos.y += containerPosition.y;

							pLayer->SetVisualElement( pNestedVisual, &pos );

							pNestedVisual->Release();

						}

						pVisualElement->Release();

					}

				} while ( bRepeat );

			}

			return true;

		}

		// --------------------------------------------------------------------

		bool DeleteFrame( frame_collection_type::iterator it ) {

			delete *it;

			m_Frames.erase( it );

			SyncInternals();

			return true;

		}

		bool DeleteFrame( const int nFrame ) {

			frame_collection_type::iterator it = GetFrame( nFrame );

			if ( it == m_Frames.end() ) {

				return false;

			}

			return DeleteFrame( it );

		}

		// --------------------------------------------------------------------

		CFrame * CreateFrame() {

			CFrame * pNew = new CFrame();

			if ( !pNew ) {

				return 0;

			}

			if ( ! pNew->InsertNewLayersBefore( m_LayerCount ) ) {

				delete pNew;

				return 0;

			}

			return pNew;

		}

		bool InsertNewFramesBefore( const int nFrames, frame_collection_type::iterator * pIT = 0 ) {

			// find where to insert the new layers
			// ----------------------------------------------------------------

			frame_collection_type::iterator it;

			if ( pIT ) {

				it = *pIT;

			} else {

				it = m_Frames.end();
			}

			// ----------------------------------------------------------------

			for ( int i = 0; i < nFrames; i++ ) {

				CFrame * pNew = CreateFrame();

				if ( ! pNew ) {

					return false;

				}

				it = m_Frames.insert( it, pNew );

			}

			SyncInternals();

			return true;

		}

		// --------------------------------------------------------------------

		bool InsertNewFramesBefore( const int nFrames, const int nFrame ) {

			frame_collection_type::iterator it = GetFrame( nFrame );

			return InsertNewFramesBefore( nFrames, &it );

		}

		// Layer visibility class
		// --------------------------------------------------------------------

		struct CLayerVisFunctor_true {

			bool operator()( const int nNumber ) {

				return true;

			}

		};

		// Render frame
		// --------------------------------------------------------------------

		template< class SURFACE, class TOP > class TRenderAnimationFrameHelper {

		private:

			CAnimation * m_pAnimation;

			TOP m_DefaultTop;

			TRenderAnimationFrameHelper(); // Hidden

		public:

			TRenderAnimationFrameHelper(
				CAnimation * pAnimation, TOP & defaultTop
			) : m_pAnimation( pAnimation ), m_DefaultTop( defaultTop ) {}
	
			TOP operator()( const int nLayer ) {
				
				if ( m_pAnimation ) {

					CAnnotatedPalette * pPal = m_pAnimation->GetLayerPalette( nLayer );

					if ( !pPal ) {

						pPal = m_pAnimation->GetPalette();

					}

					if ( pPal ) {

						return TOP( pPal->GetDisplayCLUT() );

					}

				}

				return m_DefaultTop;

			}

		};

		template< class SURFACE, class TOP, class FUNCTOR > void 
		RenderFramePtr(
			SURFACE & dstSurface, CFrame * pFrame, const int x, const int y,
			TOP & op, FUNCTOR & fn, const RECT * pOptionalClipRect = 0,
			const BLITFX * pOptionalBlitFX = 0
		) {

			TRenderAnimationFrameHelper<SURFACE,TOP> getOp( this, op );

			pFrame->Render(
				dstSurface, x, y, getOp, fn, pOptionalClipRect, pOptionalBlitFX
			);

		}

		template< class SURFACE, class TOP, class FUNCTOR > void 
		RenderFrame(
			SURFACE & dstSurface, const int nFrame, const int x, const int y,
			TOP & op, FUNCTOR & fn, const RECT * pOptionalClipRect = 0,
			const BLITFX * pOptionalBlitFX = 0
		) {

			frame_collection_type::iterator it = GetFrame( nFrame );

			if ( it != m_Frames.end() ) {

				RenderFramePtr(
					dstSurface, *it, x, y, op, fn, pOptionalClipRect, pOptionalBlitFX
				);

			}

		}

		// for_each
		// --------------------------------------------------------------------

		template< class T >
		void for_each( T fn ) {

			std::for_each( m_Frames.begin(), m_Frames.end(), fn );

		}

		template< class T >
		void for_each_frame_and_layer( T fn ) {

			frame_collection_type::iterator it = m_Frames.begin();

			for ( ; it != m_Frames.end(); it++ ) {

				it->for_each( fn );

			}

		}

		template< class P, class T >
		void predicate_for_each_frame_and_layer( P pred, T fn ) {

			frame_collection_type::iterator it = m_Frames.begin();

			for ( ; it != m_Frames.end(); it++ ) {

				it->predicate_for_each( pred, fn );

			}

		}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		template< class SHOWCASE >
		bool OptimizeVisualElements(
			SHOWCASE * pShowcase, typename SHOWCASE::optimize_map_type * pOptimizeMap
		) {

			bool bDidSomething = false;

			frame_collection_type::iterator it = m_Frames.begin();

			for ( ; it != m_Frames.end(); it++ ) {

				if ( (*it)->OptimizeVisualElements( pShowcase, pOptimizeMap ) ) {

					bDidSomething = true;

				}

			}

			return bDidSomething;
		}

		bool AdjustVisualReferences(
			CVisualElement * pOldElement
	    	,CVisualElement * pNewElement
			,const POINT * pDelta
			,const POINT * pMoveTo
		);

#endif // BPT 5/22/01

#if 1 // BPT 6/1/01

		// Added this to help me build a gather use information for
		// visual elements

		template< class FN > bool TVisualElementFunctorDispatch( FN & functor ) {

			bool bDidSomething = false;

			frame_collection_type::iterator it = m_Frames.begin();

			for ( ; it != m_Frames.end(); it++ ) {

				functor( *it );

				if ( (*it)->TVisualElementFunctorDispatch( functor ) ) {

					bDidSomething = true;

				}

			}

			return bDidSomething;

		}

#endif

		// --------------------------------------------------------------------

#if 1 // BPT 6/15/01

		CAnnotatedPalette * GetLayerPalette( const int nLayer ) {

			CLayerInfo * pLayerInfo = GetLayerInfo( nLayer );

			if ( pLayerInfo ) {

				return pLayerInfo->GetPalette();

			}

			return 0;

		}

		CAnnotatedPalette * GetPalette() {

			return m_pPalette;

		}

		void DestroyPalette() {

			if ( m_pPalette ) {

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

				m_pPalette->Release();

#else

				delete m_pPalette;

#endif

				m_pPalette = 0;

			}

		}

		bool AdoptPalette( CAnnotatedPalette * pNewPalette ) {

			DestroyPalette();

			m_pPalette = pNewPalette;

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			if ( m_pPalette ) {

				m_pPalette->AddRef();

			}

#endif
			// other processing here...

			return true;

		}

#if !defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		bool CreatePalette() {

			if ( m_pPalette ) {

				return true;

			}

			CAnnotatedPalette * pNew = new CAnnotatedPalette;

			if ( !pNew ) {

				return false;

			}

			return AdoptPalette( pNew );

		}

		bool CreatePaletteFrom( CAnnotatedPalette * pSource ) {

			CAnnotatedPalette * pNew = pSource->Duplicate();

			if ( !pNew ) {

				return false;

			}

			return AdoptPalette( pNew );

		}

#endif // !defined(BPT_SHOWCASE_MANAGED_PALETTES) BPT 6/5/02

		// Layer palette helpers

		bool LayersHavePalettes() {

			layer_info_collection_type::iterator layerIt = m_LayerInfo.begin();

			for ( ; layerIt != m_LayerInfo.end(); layerIt++ ) {

				if ( (*layerIt)->GetPalette() ) {

					return true;

				}

			}

			return false;

		}

		bool IsMultiPaletteAnimation() {

			return ((0 != GetPalette()) || LayersHavePalettes());

		}

		void DestroyLayerPalettes() {

			layer_info_collection_type::iterator layerIt = m_LayerInfo.begin();

			for ( ; layerIt != m_LayerInfo.end(); layerIt++ ) {

				if ( (*layerIt)->GetPalette() ) {

					(*layerIt)->DestroyPalette();

				}

			}

		}

		void ManageSharedPaletteEntries( CAnnotatedPalette * pParentPalette ) {

			layer_info_collection_type::iterator layerIt = m_LayerInfo.begin();

			CAnnotatedPalette * pActiveParentPalette;

			if ( m_pPalette ) {

				pActiveParentPalette = m_pPalette;

				m_pPalette->ManageSharedPaletteEntries( pParentPalette );

			} else {

				pActiveParentPalette = pParentPalette;

			}

			for ( ; layerIt != m_LayerInfo.end(); layerIt++ ) {

				if ( (*layerIt)->GetPalette() ) {

					(*layerIt)->ManageSharedPaletteEntries( pActiveParentPalette );

				}

			}

		}

#endif // BPT 6/15/01

		// --------------------------------------------------------------------

#if 1 // BPT 6/27/01

		void SetOutlineColorA( const int nOutlineColor ) {

			m_nOutlineColorA = nOutlineColor;

		}
			
		void SetOutlineColorB( const int nOutlineColor ) {

			m_nOutlineColorB = nOutlineColor;

		}

		int GetOutlineColorA() const {

			return m_nOutlineColorA;

		}
			
		int GetOutlineColorB() const {

			return m_nOutlineColorB;

		}

#endif

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Animation collection, need better name?
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CAnimationShowcase : public CAnnotatedObject {

	public: // Traits

		typedef std::list< CAnimation * > anim_collection_type;

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		typedef std::list< typename CVisualElement::pointer > visual_element_collection_type; 

		typedef std::map< CVisualElement *, CVisualElement * > optimize_map_type;

#endif // BPT 5/22/01

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		typedef std::list<CAnnotatedPalette *> palette_collection_type;

#endif	// BPT 6/5/02

	private: // Data

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		CAnnotatedPalette * m_pDefaultPalette;

#else

		CAnnotatedPalette m_DefaultPalette;

#endif // !defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		anim_collection_type m_AnimationCollection;

		CIDType m_NextID;

		CSize m_CanvasSize;

		CSimpleName m_Name;

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		visual_element_collection_type m_VisualElementsCollection;

		bool m_bNeedToMaintainVisualElementList;

#endif // BPT 5/22/01

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		palette_collection_type m_PaletteCollection;

#endif	// BPT 6/5/02

#if 1 // BPT 9/2/02
		
		CSize m_PixelAspectRatio;

#endif // BPT 9/2/02

	public: // Interface

		CAnimationShowcase() : m_NextID( 1 ), m_CanvasSize( 0, 0 ), m_PixelAspectRatio(1,1) {

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

			m_bNeedToMaintainVisualElementList = false;

#endif

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			m_pDefaultPalette = new CAnnotatedPalette;

			if ( m_pDefaultPalette ) {

				m_pDefaultPalette->SetName( ".showcase global palette" );

				m_pDefaultPalette->AddRef();

			} else {

				// THIS IS FATAL ERROR!!!!

			}

#endif

		}

		// --------------------------------------------------------------------

		~CAnimationShowcase() {

			// destroy all the animations contained in this showcase
			// ----------------------------------------------------------------

			while ( !m_AnimationCollection.empty() ) {

				delete m_AnimationCollection.back();

				m_AnimationCollection.pop_back();

			}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

#if 1 // BPT 10/3/02

			{

				// Need to deal with the nested elements first
				// ------------------------------------------------------------

				bool bReleaseNested;

				do {

					bReleaseNested = false;

					SortVisualElementCollection();

					visual_element_collection_type::iterator it = 
						m_VisualElementsCollection.begin();

					for ( ; it != m_VisualElementsCollection.end(); it++ ) {

						if ( (*it)->HasNestedVisualReferences() ) {

							(*it)->Release();

							m_VisualElementsCollection.erase( it );

							bReleaseNested = true;

							break;

						}

					}

				} while ( bReleaseNested );

			}

#endif

			// Now remove the remaining visual elements
			// ----------------------------------------------------------------

			while ( !m_VisualElementsCollection.empty() ) {

				int referenceCount = m_VisualElementsCollection.back()->RefCount();

#ifdef _DEBUG

				if ( 1 != referenceCount ) {

					TRACE( 
						"CVisualElement %p reference count (1 != %d)?\n", 
						m_VisualElementsCollection.back(), referenceCount
					);

				}

#endif

				// Only release this visual element once so that the clipboard
				// thing can work properly...

				m_VisualElementsCollection.back()->Release();

				m_VisualElementsCollection.pop_back();

			}

#endif // BPT 5/22/01

			// destroy all the palettes contained in this showcase
			// ----------------------------------------------------------------

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			while ( !m_PaletteCollection.empty() ) {

				m_PaletteCollection.back()->Release();

				m_PaletteCollection.pop_back();

			}

			if ( m_pDefaultPalette ) {

				m_pDefaultPalette->Release();

			}

#endif // BPT 6/5/02

		}

		// --------------------------------------------------------------------

		char * GetName() {

			return m_Name.GetName();

		}

		bool SetName( const char * pzName ) {

			return m_Name.SetName( pzName );

		}

		// Misc
		// --------------------------------------------------------------------

		void SetCanvasSize( const CSize size ) {

			m_CanvasSize = size;

		}

		CSize CanvasSize() const {

			return m_CanvasSize;

		}

#if 1 // BPT 9/2/02

		CSize GetPixelAspectRatio() const {

			return m_PixelAspectRatio;

		}

		void SetPixelAspectRatio( CSize & ratio ) {

			m_PixelAspectRatio = ratio;

		}

#endif

		// Showcase relative id (
		// --------------------------------------------------------------------

		CIDType NewID() {

			return CIDType( m_NextID++ );

		}

		// --------------------------------------------------------------------
		// Visual Elements methods
		// --------------------------------------------------------------------

		template< class T >
		struct TCompareReferenceCountLT : std::binary_function<T,T,bool> {

			bool operator()( const T & p, const T & q ) const {

				return p->RefCount() < q->RefCount();

			}

		};

		template< class T >
		struct TCompareReferenceCountGT : std::binary_function<T,T,bool> {

			bool operator()( const T & p, const T & q ) const {

				return p->RefCount() > q->RefCount();

			}

		};

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		bool AdoptVisualElement( CVisualElement * pVisualElement ) {

#if 1 // BPT 6/1/01

			// Check to see if the element was already in the list and
			// don't put another instance in the list if so.
			// ----------------------------------------------------------------

			visual_element_collection_type::iterator found = 
				std::find( 
					m_VisualElementsCollection.begin(),
					m_VisualElementsCollection.end(),
					pVisualElement
				);

			if ( found != m_VisualElementsCollection.end() ) {

				return true;

			}

#endif

#if 1 // BPT 10/3/02

			// if this is a nested visual element then we need to
			// adopt all the nested visual elements as well
			// ----------------------------------------------------------------

			if ( pVisualElement->HasNestedVisualReferences() ) {

				pVisualElement->AdoptNestedVisuals( this );

			}

#endif

			// Hook up the visual element to the showcase for future 
			// management.
			// ----------------------------------------------------------------

			pVisualElement->AddRef();

			m_VisualElementsCollection.push_back( pVisualElement );

			m_bNeedToMaintainVisualElementList = true;

			return true;

		}

#endif // BPT 5/22/01

		template< class T > CVisualElement * 
		CreateVisualElementFromSurface(
			T & surface, const typename T::pixel_type chromaKey,
			const RECT * pRect = 0, const SIZE * pOptionalThumbnailSize = 0
		) {

			// Create the object
			// ----------------------------------------------------------------

			CIDType id = NewID();

			CVisualElement * pVisualElement = new CVisualElement( id );

			if ( !pVisualElement ) {

				return 0;

			}

			// Capture the data
			// ----------------------------------------------------------------

			if ( !pVisualElement->CreateFrom(
				surface, TIsNotValue<typename T::pixel_type>( chromaKey ), 
				pRect, pOptionalThumbnailSize )) {

				delete pVisualElement;

				return 0;

			}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

			AdoptVisualElement( pVisualElement );

#endif // BPT 5/22/01

			return pVisualElement;

		}

#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/22/01

		//
		//	ReleaseUnusedVisualElements()
		//

		void ReleaseUnusedVisualElements() {

#ifdef _DEBUG
			int removeCount = 0;
#endif

			bool bRemovedNestedElement;

			do {

				bRemovedNestedElement = false;

				visual_element_collection_type::iterator it = 
					m_VisualElementsCollection.begin();

				for ( ; it != m_VisualElementsCollection.end(); ) {

					if ( 1 == (*it)->RefCount() ) {

						if ( (*it)->HasNestedVisualReferences() ) {

#ifdef _DEBUG
							TRACE( "Unused nested reference removed.\n" );
#endif

							bRemovedNestedElement = true;

						}

						(*it)->Release();

						it = m_VisualElementsCollection.erase( it );

#ifdef _DEBUG
						++removeCount;
#endif

					} else {

						++it;

					}

				}

			} while ( bRemovedNestedElement );

#ifdef _DEBUG
			TRACE( "Removed %d unused visual elements\n", removeCount );
#endif

		}

		//
		//	MaintainVisualElements()
		//

		void MaintainVisualElements(
			const bool bReleaseUnused = true, 
			const bool bBuildThumbnails = true // future processing...
		) {

			if ( !(bReleaseUnused || bBuildThumbnails) ) {

				return /* NOP */;

			}

			// ----------------------------------------------------------------

			if ( bReleaseUnused ) {

				ReleaseUnusedVisualElements();

			}

			// ----------------------------------------------------------------

			if ( bBuildThumbnails ) {

				// future processing...
				// need to determine which elements need a thumbnail :)

			}

			m_bNeedToMaintainVisualElementList = false;

		}

		//
		//	OptimizeVisualElementReference()
		//

		bool OptimizeVisualElementReference(
			CVisualElementReference * pVisualReference
		) {

			// Check to see if the visual reference has a visual element before
			// doing any further processing...
			// ----------------------------------------------------------------

			CVisualElement * pVisualElement = pVisualReference->GetVisualElement();

			if ( !pVisualElement ) {

				return false;

			}

			ReleaseUnusedVisualElements(); // BPT 10/14/02

#if 0

			// Check to see if this visual element is already 'optimized'
			// you can tell that it's optimized if it the visual element that
			// it references has a reference count > 4.
			// ----------------------------------------------------------------

			int referenceCount = pVisualElement->RefCount();

			if ( 5 < referenceCount ) {

				pVisualElement->Release();

				return false;

			}

#endif

			// Okay now search through the existing visual elements to see
			// if there is another visual element that looks exactly like 
			// this one.
			// ----------------------------------------------------------------

			bool bDidSomething = false;

			RECT currentBoundingRect = pVisualReference->BoundingRect();

			visual_element_collection_type::iterator it = 
					m_VisualElementsCollection.begin();

			for ( ; it != m_VisualElementsCollection.end(); it++ ) {

				if ( (*it) == pVisualElement ) continue;

				if ( pVisualElement->CompareVisualElement( *it ) ) {

					// Get the bounding rect for the current search element
					// this will help set the new position :)
					// ----------------------------------------------------
					
					RECT boundingRect = (*it)->BoundingRect();

					// Okay replace the reference with the match
					// ----------------------------------------------------

					pVisualReference->SetVisualElement( *it );

					pVisualReference->MoveTo(
						(currentBoundingRect.left - boundingRect.left),
						(currentBoundingRect.top - boundingRect.top)
					);

					// No further search is necessary
					// ----------------------------------------------------

#ifdef _DEBUG
					TRACE(
						"( Found duplicate %d replaced with %d, delta (%d,%d) )\n\t"
						,pVisualElement->ID()
						,(*it)->ID()
						,(currentBoundingRect.left - boundingRect.left)
						,(currentBoundingRect.top - boundingRect.top)
					);
#endif

					bDidSomething = true;

					break;

				}

			}
			
			// Cleanup and return
			// ----------------------------------------------------------------

			pVisualElement->Release();

			// This should make things interesting
			// ----------------------------------------------------------------

			if ( bDidSomething ) {

				ReleaseUnusedVisualElements();

			}

			return bDidSomething;

		}

		//
		//	OptimizeVisualElementReference()
		//

		bool OptimizeVisualElementReference(
			CVisualElementReference * pVisualReference,
			optimize_map_type * pOptimizeMap
		) {

			if ( !pOptimizeMap ) {

				return OptimizeVisualElementReference( pVisualReference );

			}

			// Check to see if the visual reference has a visual element before
			// doing any further processing...
			// ----------------------------------------------------------------

			CVisualElement * pVisualElement = pVisualReference->GetVisualElement();

			if ( !pVisualElement ) { return false; }

			optimize_map_type::iterator found = pOptimizeMap->find( pVisualElement );

			pVisualElement->Release();

			if ( found == pOptimizeMap->end() ) { return false; }

			// remap the visual element reference moving to keep the alignment
			// ----------------------------------------------------------------

			CVisualElement * pFoundElement = found->second;

			RECT currentBoundingRect = pVisualReference->BoundingRect();

			RECT boundingRect = pFoundElement->BoundingRect();

			pVisualReference->SetVisualElement( pFoundElement );

			pVisualReference->MoveTo(
				(currentBoundingRect.left - boundingRect.left),
				(currentBoundingRect.top - boundingRect.top)
			);

			return true;

		}

		//
		//	CompactVisualElementIDs()
		//

		void CompactVisualElementIDs() {

			CIDType id = 1;

			visual_element_collection_type::iterator it = 
				m_VisualElementsCollection.begin();

			for ( ; it != m_VisualElementsCollection.end(); it++ ) {

				(*it)->SetID( id );

				++id;

			}

			m_NextID = id;

		}

		//
		//	SortVisualElementCollection()
		//

		void SortVisualElementCollection() {

			// BPT 6/1/01: This will eventually need to do something
			// I was unable to force VC++ to compile my predicate
			// for the std::list::sort method so I just removed all
			// references to it.

#if 1 // 10/3/02 BPT

			m_VisualElementsCollection.sort(
				TCompareReferenceCountGT<CVisualElement *>()
			);

#endif


		}

#if 1 // BPT 6/1/01

		// Added this to help me build a gather use information for
		// visual elements

		template< class FN > bool TVisualElementFunctorDispatch( FN & functor ) {

			bool bDidSomething = false;

			anim_collection_type::iterator it = m_AnimationCollection.begin();

			for ( ; it != m_AnimationCollection.end(); it++ ) {

				functor( *it );

				if ( (*it)->TVisualElementFunctorDispatch( functor ) ) {

					bDidSomething = true;

				}

			}

			return bDidSomething;

		}

#endif

		bool AdjustVisualReferences(
			CVisualElement * pOldElement
	    	,CVisualElement * pNewElement
			,const POINT * pDelta
			,const POINT * pMoveTo
		) {

			// call optimize visual elements for each animation
			// ----------------------------------------------------------------

			bool bDidSomething = false;

			anim_collection_type::iterator it = m_AnimationCollection.begin();

			for ( ; it != m_AnimationCollection.end(); it++ ) {

				if ( (*it)->AdjustVisualReferences(
					pOldElement, pNewElement, pDelta, pMoveTo ) ) {

					bDidSomething = true;

				}

			}

			return bDidSomething;
		}

		int VisualElementCount() const {

			return m_VisualElementsCollection.size();

		}

		bool OptimizeVisualElements() {

			bool bDidSomething = false;

			TRACE( "START: OptimizeVisualElements() count = %d\n", VisualElementCount() );

			// Need to build the remap map
			// ----------------------------------------------------------------

#if 1 // BPT 10/15/02

			optimize_map_type sameAsMap;

			// ---------

			ReleaseUnusedVisualElements();

			SortVisualElementCollection();

			// ---------

			{
				visual_element_collection_type::iterator outter = 
						m_VisualElementsCollection.begin();

				for ( ; outter != m_VisualElementsCollection.end(); outter++ ) {

					// check to see if this visual element is remapped and
					// search for others using the remap to element

					CVisualElement * pSearchVisual = *outter;

					optimize_map_type::iterator found = sameAsMap.find( pSearchVisual );

					if ( sameAsMap.end() != found ) {

						pSearchVisual = found->second;

					}

					visual_element_collection_type::iterator inner = outter;

					while ( ++inner != m_VisualElementsCollection.end() ) {

						// make sure this element wasn't found already

						if ( sameAsMap.end() != sameAsMap.find( *inner ) ) continue;

						if ( pSearchVisual->CompareVisualElement( *inner ) ) {

							// Increase the reference count so they don't 
							// go away while we're remapping them.

							pSearchVisual->AddRef();

							(*inner)->AddRef();

							// set the map entry

							sameAsMap[ *inner ] = pSearchVisual;

						}

					}

				}
			}
#endif

			// try to optimize nested visual elements
			// ----------------------------------------------------------------

#if 1 // BPT 9/28/02

			{

#ifdef _DEBUG
				TRACE( "Optimizing nested visual elements\n" );
#endif

				visual_element_collection_type::iterator it = 
						m_VisualElementsCollection.begin();

				for ( ; it != m_VisualElementsCollection.end(); it++ ) {

					if ( (*it)->OptimizeNestedVisualElements( this, &sameAsMap ) ) {

						bDidSomething = true;

					}

				}

			}

#endif

			// call optimize visual elements for each animation
			// ----------------------------------------------------------------

#ifdef _DEBUG
			TRACE( "Optimizing animation based visual elements\n" );
#endif

			anim_collection_type::iterator it = m_AnimationCollection.begin();

			for ( ; it != m_AnimationCollection.end(); it++ ) {

				if ( (*it)->OptimizeVisualElements( this, &sameAsMap ) ) {

					bDidSomething = true;

				}

			}

#if 1 // BPT 10/15/02

			// Need to release our references on the visual elements in the map

			{

				optimize_map_type::iterator final = sameAsMap.begin(); 

				while ( final != sameAsMap.end() ) {

					final->first->Release();

					final->second->Release();

					++final;

				}

			}

#endif

			// Now finish up the optimize process by renumbering the id's
			// ----------------------------------------------------------------

			ReleaseUnusedVisualElements(); // BPT 6/1/01

			SortVisualElementCollection();

			CompactVisualElementIDs();

#ifdef _DEBUG

			// dump out the visual elements information
			// ----------------------------------------------------------------

			visual_element_collection_type::iterator dbugIT = 
				m_VisualElementsCollection.begin();

			int elementCounter = 1;

			TRACE( "BEGIN: Visual Elements sumary (count = %d)\n\n", VisualElementCount() );

			for ( ; dbugIT != m_VisualElementsCollection.end(); dbugIT++ ) {

				TRACE( "\t%4d used %d times\n", elementCounter, (*dbugIT)->RefCount() );

				++elementCounter;

			}

			TRACE( "\nFINISH: Visual Elements sumary\n\n" );
#endif

			return bDidSomething;

		}

#endif // BPT 5/22/01

		// Animation methods
		// --------------------------------------------------------------------

		CAnimation * FirstAnimation() {

			if ( !m_AnimationCollection.size() ) {

				return 0;

			}

			return *m_AnimationCollection.begin();

		}

		CAnimation * NextAnimation( CAnimation * pAnimation ) {

			anim_collection_type::iterator it = m_AnimationCollection.begin();

			for ( ; it != m_AnimationCollection.end(); it++ ) {

				if ( pAnimation == (*it) ) {

					++it;

					if ( it == m_AnimationCollection.end() ) {

						return *m_AnimationCollection.begin();

					} else {

						return *it;

					}

				}

			}

			return pAnimation;

		}

		CAnimation * PrevAnimation( CAnimation * pAnimation ) {

			anim_collection_type::reverse_iterator it = m_AnimationCollection.rbegin();

			for ( ; it != m_AnimationCollection.rend(); it++ ) {

				if ( pAnimation == (*it) ) {

					++it;

					if ( it == m_AnimationCollection.rend() ) {

						return *m_AnimationCollection.rbegin();

					} else {

						return *it;

					}

				}

			}

			return pAnimation;

		}

		// --------------------------------------------------------------------

		int AnimationCount() const {

			return m_AnimationCollection.size();

		}

		bool AttachAnimation( CAnimation * pAnimation ) {

			m_AnimationCollection.push_back( pAnimation );

			return true;

		}

		bool UnattachAnimation( CAnimation * pAnimation ) {

			anim_collection_type::iterator it = m_AnimationCollection.begin();

			for ( ; it != m_AnimationCollection.end(); it++ ) {

				if ( pAnimation == (*it) ) {

					m_AnimationCollection.erase( it );

					return true;

				}

			}

			return false;

		}

		// RONG: Copy an animation
		// BPT: removed unecessary BPT::'s and made the dialog optional + added code to
		// not return a partially copied animation as god only knows what didn't get copied

		CAnimation * CopyAnimation(
			CAnimation * pSrcAnim, const bool bShowErrorDialog = true ) {

			CAnimation * pDstAnim = NewAnimation(
				pSrcAnim->FrameCount(), 
				pSrcAnim->LayerCount() 
			);

			if ( !pDstAnim ) { // BPT 6/26/01

				return false;

			}

			CString name;

			name.Format( "Copy of %s", pSrcAnim->GetName() );

			pDstAnim->SetName( name );
			pDstAnim->SetPlaybackDelay( pSrcAnim->GetPlaybackDelay() );
			pDstAnim->m_Link1 = pSrcAnim->m_Link1;

			// Duplicate the named spots for the source animation
			// ----------------------------------------------------------------

			BPT::CAnimation::named_spot_collection_type::iterator spotIt = pSrcAnim->NamedSpotCollection().begin();
			
			for ( ; spotIt != pSrcAnim->NamedSpotCollection().end(); spotIt++ ) {

				CNamedSpot * pSpotCopy = (*spotIt)->Copy();

				// Only attach valid pointers to the list (BPT 6/26/01)

				if ( pSpotCopy ) {

					pDstAnim->AttachNamedSpot( pSpotCopy );

				} else {

					TRACE( "Unable to Copy() spot \"%s\"\n", (*spotIt)->GetName() );

				}

			}

			bool bError = false;

			int layerCount = pSrcAnim->LayerCount();

#if 1 // BPT 6/27/01

			// Outline colors used for layer rendering
			// ------------------------------------------------------------

			pDstAnim->SetOutlineColorA( pSrcAnim->GetOutlineColorA() );
			pDstAnim->SetOutlineColorB( pSrcAnim->GetOutlineColorB() );

			// Deal with the animation palette
			// ------------------------------------------------------------

			CAnnotatedPalette * pPalette = pSrcAnim->GetPalette();

			if ( pPalette ) {

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

				if ( !pDstAnim->AdoptPalette( pPalette ) ) {

#else

				if ( !pDstAnim->CreatePaletteFrom( pPalette ) ) {

#endif

					bError = false;

				}

			}

			// Copy the layer info settings (palette's etc...)
			// ------------------------------------------------------------
				
			for ( int layerInfoCounter = 0; layerInfoCounter < layerCount; layerInfoCounter++ ) {

				// Get the source & dest layer info pointers

				CLayerInfo * pSrcLayerInfo = pSrcAnim->GetLayerInfo( layerInfoCounter );

				CLayerInfo * pDstLayerInfo = pDstAnim->GetLayerInfo( layerInfoCounter );

				// Ask the dst layer to copy the src layers info...

				if ( !pDstLayerInfo->CopyFrom( pSrcLayerInfo ) ) {

					bError = true;

				}

			}

#endif

			// Copy over each frame's data to the new animation
			// ----------------------------------------------------------------

			int frameCount = pSrcAnim->FrameCount();

			for ( int frame = 0; frame < frameCount; frame++ ) {

				CFrame * pSrcFrame = pSrcAnim->GetFramePtr( frame );

				CFrame * pDstFrame = pDstAnim->GetFramePtr( frame );

				// Copy over the spot adjustments
				// ------------------------------------------------------------

				BPT::CFrame::named_spot_adjustment_collection_type::iterator spotIt = pSrcFrame->NamedSpotAdjustmentsCollection().begin();
				
				for ( ; spotIt != pSrcFrame->NamedSpotAdjustmentsCollection().end(); spotIt++ ) {
					
					CNamedSpot * pSpot = pDstAnim->FindNamedSpot( (*spotIt)->GetName() );

					if ( pSpot ) {

						CNamedSpotAdjustment * pAdjustmentCopy = (*spotIt)->Copy( pSpot );

						// Only attach valid pointers to the list (BPT 6/26/01)

						if ( pAdjustmentCopy ) {

							pDstFrame->AttachNamedSpotAdjustment( pAdjustmentCopy );

						} else {

							TRACE( "Unable to Copy() spot adjustment for Spot \"%s\"\n", pSpot->GetName() );

						}

					}
						
				}
				
				// Okay now the layers
				// ------------------------------------------------------------
				
				for ( int layer = 0; layer < layerCount; layer++ ) {

					// Get the source & dest layer pointers

					CLayer * pSrcLayer = pSrcFrame->GetLayerPtr( layer );

					CLayer * pDstLayer = pDstFrame->GetLayerPtr( layer );

					// Ask the dst layer to copy the src layers info...

					if ( !pDstLayer->CopyFrom( pSrcLayer ) ) {

						bError = true;

					}

				}

			}

			if ( bError ) {

				if ( bShowErrorDialog ) {

					MessageBox( 
						AfxGetMainWnd()->GetSafeHwnd(), 
						"Error during layer information transfer", "ERROR!", 
						MB_ICONERROR | MB_OK
					);

				}

				// BPT: added
				// ------------------------------------------------------------

				UnattachAnimation( pDstAnim );

				delete pDstAnim;

				return 0;

			}
			
			return pDstAnim;
		}

		CAnimation * NewAnimation( const int nFrames, const int nLayers ) {

			// Create the controlling object
			// ----------------------------------------------------------------

			CAnimation * pNew = new CAnimation();

			if ( !pNew ) {

				TRACE0( "Unable to create CAnimation instance.\n" );

				return 0;

			}

			// ----------------------------------------------------------------

			if ( !pNew->InsertNewLayers( nLayers, 0 ) ) {

				delete pNew;

				TRACE(
					"Unable to create CAnimation : %d frames with %d layers.\n",
					nFrames, nLayers
				);

				return 0;

			}

			// ----------------------------------------------------------------

			if ( !pNew->InsertNewFramesBefore( nFrames ) ) {

				delete pNew;

				TRACE(
					"Unable to create CAnimation : %d frames with %d layers.\n",
					nFrames, nLayers
				);

				return 0;

			}

			// ----------------------------------------------------------------

			AttachAnimation( pNew );

			return pNew;

		}

		// --------------------------------------------------------------------

		CAnnotatedPalette * GetDefaultPalette() {

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

			return m_pDefaultPalette;

#else

			return &m_DefaultPalette;

#endif

		}

		// This should be called anytime a palette has changed to ensure
		// proper parent/child palette updating...
		// --------------------------------------------------------------------

		void ManageSharedPaletteEntries() {

			anim_collection_type::iterator it = m_AnimationCollection.begin();

			for ( ; it != m_AnimationCollection.end(); it++ ) {

				(*it)->ManageSharedPaletteEntries( GetDefaultPalette() );

			}

		}

		// New managed palette system.
		// --------------------------------------------------------------------

#if defined(BPT_SHOWCASE_MANAGED_PALETTES) // BPT 6/5/02

		// --------------------------------------------------------------------

		int PaletteCount() const {

			return m_PaletteCollection.size();

		}

		palette_collection_type::iterator BeginPaletteIterator() {

			return m_PaletteCollection.begin();

		}

		palette_collection_type::iterator EndPaletteIterator() {

			return m_PaletteCollection.end();

		}

		// --------------------------------------------------------------------

		bool AdoptPalette( CAnnotatedPalette * pPal ) {

			m_PaletteCollection.push_back( pPal );

			pPal->AddRef();

			return true;

		}

		bool AbandonPalette( CAnnotatedPalette * pPal ) {

			palette_collection_type::iterator it = m_PaletteCollection.begin();

			while ( it != m_PaletteCollection.end() ) {

				if ( (*it) == pPal ) {

					m_PaletteCollection.erase( it );

					break;

				}

				++it;

			}

			pPal->Release();

			return true;

		}

		CAnnotatedPalette * CreatePalette( const bool bCopyDefault = true ) {

			CAnnotatedPalette * pPal = new CAnnotatedPalette;

			if ( !pPal ) {

				return 0;

			}

			// give the palette a simple name

			char paletteName[ _MAX_PATH ];

			sprintf( paletteName, "Palette (%04d)", PaletteCount() + 1 );

			if ( !pPal->SetName( paletteName ) ) {

				return false;

			}

			if ( bCopyDefault && m_pDefaultPalette ) {

				pPal->CopyColorsFrom( m_pDefaultPalette );

			}

			// finally adopt the palette

			if ( !AdoptPalette( pPal ) ) {

				pPal->Release();

				return 0;

			}

			return pPal;

		}

		CAnnotatedPalette * DuplicatePalette( CAnnotatedPalette * pSrc ) {

			CAnnotatedPalette * pPal = CreatePalette( false );

			if ( !pPal ) {

				return 0;

			}

			pPal->CopyColorsFrom( pSrc );

			return pPal;

		}

#endif // defined(BPT_SHOWCASE_MANAGED_PALETTES) BPT 6/5/02


#if 1 // BPT 6/7/03

		//
		//	SuperOptimizeVisualElements()
		//

		bool SuperOptimizeVisualElements( const bool bExpandVisuals )
		{
			// okay handle the breakdown
			// ----------------------------------------------------------------

			int visualElements = VisualElementCount();

			TRACE( "%d visual elements before breakdown\n", visualElements );

			{
				visual_element_collection_type::iterator it = 
					m_VisualElementsCollection.begin();

				TRACE( "Starting breakdown test.\n" );

				for ( int i = 0; i < visualElements; i++ ) {

					if ( !(*it)->BreakdownVisualElement( this, 2, 2 ) ) {

						TRACE( "\tBreakdown failed (%d)\n", i );

						return false;

					}

					++it;

				}

				TRACE( "finish breakdown test.\n" );

			}
		
			visualElements = VisualElementCount();

			TRACE( "%d visual elements before breakdown\n", visualElements );

			// ----------------------------------------------------------------

			anim_collection_type::iterator it = m_AnimationCollection.begin();

			for ( ; it != m_AnimationCollection.end(); it++ ) {

				if ( !(*it)->ExpandNestedVisuals() ) {

					TRACE( "ERROR processing animation \"%s\"!\n", (*it)->GetName() );

					return false;

				} else {

					TRACE( "Yeah ExpandNestedVisuals() worked!\n" );

				}

			}

			return true;

		}

#endif // BPT 6/7/03


#if defined(BPT_OPTIMIZE_VISUAL_ELEMENTS) // BPT 5/23/01

		/*

			This really needs to have a layer for each 'palette' so that the
			context of the visual elements can be tracked a little better!!!!

			-BPT 6/5/02

		*/

		CAnimation * CreateVisualElementsAnim( const char * pName ) {

			int visualElements = VisualElementCount();

#if 0 // Breakdown test 9/27/02

			{

				visual_element_collection_type::iterator it = 
					m_VisualElementsCollection.begin();

				TRACE( "Starting breakdown test.\n" );

				for ( int i = 0; i < visualElements; i++ ) {

					if ( !(*it)->BreakdownVisualElement( this, 2, 2 ) ) {

						TRACE( "\tBreakdown failed (%d)\n", i );

					}

					++it;

				}

				TRACE( "finish breakdown test.\n" );

			}
		
			visualElements = VisualElementCount();

#endif

			int nLayers = 1;
			
#if defined(BPT_SHOWCASE_MANAGED_PALETTES)

			nLayers += PaletteCount();

#endif

			CAnimation * pNew = NewAnimation( visualElements, nLayers );

			if ( !pNew ) {

				return 0;

			}

			// Do we have a name to set?
			// ----------------------------------------------------------------

			if ( pName ) {

				pNew->SetName( pName );

			} else {

				pNew->SetName( "Visual Elements Anim" );

			}

			// Name the layers
			// ----------------------------------------------------------------

#if defined(BPT_SHOWCASE_MANAGED_PALETTES)

			{
				int whichLayer = 0;

				palette_collection_type::iterator palIT = BeginPaletteIterator();

				for ( int layer = 0; layer < nLayers; layer++ ) {

					CLayerInfo * pLayerInfo = pNew->GetLayerInfo( layer );

					if ( !pLayerInfo ) {

#ifdef _DEBUG 
						TRACE( "Failure to get layer %d?\n", layer );
#endif
						continue;

					}

					// --------------------------------------------------------

					CAnnotatedPalette * pPalette;

					if ( layer ) {

						pPalette = *palIT++;

					} else {

						pPalette = GetDefaultPalette();

					}

					// Give the layer the palette and the palette name
					// --------------------------------------------------------

					pLayerInfo->AdoptPalette( pPalette );

					char * pPalName = pPalette->GetName();

					if ( pPalName ) {

						pLayerInfo->SetName( pPalName );

					} else {

						char generatedName[ _MAX_PATH ];

						sprintf( generatedName, "Untitled palette %p", pPalette );

						pLayerInfo->SetName( generatedName );

					}

				}

			}
#endif

			// We doggie this is fun :)
			// ----------------------------------------------------------------
			visual_element_collection_type::iterator it = 
				m_VisualElementsCollection.begin();

			for ( int frame = 0; frame < visualElements; frame++ ) {

				if ( it == m_VisualElementsCollection.end() ) {

					break;

				}

#if 0 // BPT 10/11/02

				while ( (*it)->HasNestedVisualReferences() ) {

					if ( ++it == m_VisualElementsCollection.end() ) {

						break;

					}

				}

#endif

				// Get the layer pointer
				// ------------------------------------------------------------

				CFrame * pFrame = pNew->GetFramePtr( frame );

				if ( !pFrame ) {

#ifdef _DEBUG 
					TRACE( "Failure to get frame %d?\n", frame + 1 );
#endif

					break;

				}

				// every 'layer' gets the same visual element

				for ( int layer = 0; layer < nLayers; layer++ ) {

					CLayer * pLayer = pFrame->GetLayerPtr( layer );

					if ( !pLayer ) {

#ifdef _DEBUG 
						TRACE( "Failure to get layer %d?\n", layer );
#endif

						break;

					}

					// Okay now we're talking! set the visual element
					// --------------------------------------------------------

#if 1 // BPT 10/11/02

					// center the visual element

					RECT boundingRect = (*it)->BoundingRect();

					int cx = boundingRect.right - boundingRect.left;
					int cy = boundingRect.bottom - boundingRect.top;

					pLayer->SetVisualElement(
						(*it), 
						&CPoint(
							((m_CanvasSize.cx/2) - cx/2) - boundingRect.left,
							((m_CanvasSize.cy/2) - cy/2) - boundingRect.top
						)
					);

#else

					pLayer->SetVisualElement( (*it), &CPoint(0,0) );

#endif

				}

				// ------------------------------------------------------------

				++it;

			}

			// ----------------------------------------------------------------

#if 1 // HACK
			if ( pNew ) {

				if ( !pNew->ExpandNestedVisuals() ) {

					TRACE( "ERROR!\n" );

				} else {

					TRACE( "Yeah ExpandNestedVisuals() worked!\n" );

				}

			}
#endif

			return pNew;

		}

#endif // BPT 5/23/01

		// --------------------------------------------------------------------

		bool SaveTo( CShowcaseFileIO * io );
		bool LoadFrom( CShowcaseFileIO * io );

	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// CShowcaseFileIO
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CShowcaseFileIO : public CChunkyFileIO {

	public: // reflected traits

		typedef CChunkyFileIO base_type;
		typedef base_type::id_type id_type;
		typedef base_type::size_type size_type;
		typedef base_type::chunk_reference chunk_reference;
		typedef base_type::inner_chunk_collection inner_chunk_collection;

	public: // traits

		typedef int io_internal_id_type;

		typedef std::map< io_internal_id_type, void * > internal_id_map_type;

	private:

		CString m_BaseFilename;

		char m_FilenameDrive[_MAX_DRIVE];
		char m_FilenameDir[_MAX_DIR];
		char m_FilenameFname[_MAX_FNAME];
		char m_FilenameExt[_MAX_EXT];

		internal_id_map_type m_InternalIDMap;
		CAnimationShowcase * m_pShowcase;
		CAnimation * m_pAnimation;
		CFrame * m_pFrame;
		CAnnotatedPalette * m_pPalette; // BPT 6/15/01

		io_internal_id_type m_NextIOID;

		bool m_bExternalVisualElements;

	public:

		CShowcaseFileIO() : 
			m_pAnimation(0), 
			m_pShowcase(0), 
			m_pFrame(0), 
			m_NextIOID( 0 ),
			m_pPalette( 0 ),
			m_bExternalVisualElements( false )
		{

			m_FilenameDrive[ 0 ] = '\0';
			m_FilenameDir[ 0 ] = '\0';
			m_FilenameFname[ 0 ] = '\0';
			m_FilenameExt[ 0 ] = '\0';

		}

		// --------------------------------------------------------------------

		bool Open( 
			const char * filename, 
			const char * access,
			const bool bExternalVisualElements = false,
			const bool bXMLIOMode = false
		) {

			// Need to set other flags

			if ( bXMLIOMode ) {

				m_bExternalVisualElements = true;

			} else {

				m_bExternalVisualElements = bExternalVisualElements;

			}

			if ( filename ) {

				// Get the components that make up the filename
				// ------------------------------------------------------------

				m_BaseFilename = filename;

			} else {

				m_BaseFilename = ".\\Error.Ouch";

			}

			_splitpath( 
				m_BaseFilename, 
				m_FilenameDrive, 
				m_FilenameDir, 
				m_FilenameFname, 
				m_FilenameExt
			);

			return base_type::Open( filename, access, bXMLIOMode );

		}

		// --------------------------------------------------------------------

		bool StoreVisualElementsExternally() const {

			return m_bExternalVisualElements;

		}

		CString MakeSubordinateFilename(
			CString & fnamePrefix, 
			CString & fnamePostfix, 
			CString & newExtension,
			const bool bFullPath = true
		) {

			char filename[ _MAX_PATH ];

			CString combinedName = fnamePrefix;
			combinedName += m_FilenameFname;
			combinedName += fnamePostfix;

			if ( bFullPath ) {

				_makepath( 
					filename, 
					m_FilenameDrive, 
					m_FilenameDir,
					combinedName,
					newExtension
				);

			} else {

				_makepath( 
					filename, 
					"", 
					"",
					combinedName,
					newExtension
				);

			}

			return CString( filename );

		}

		// --------------------------------------------------------------------

		CAnimation * CurrentAnimation() const {

			return m_pAnimation;

		}

		void SetCurrentAnimation( CAnimation * pAnimation ) {

			m_pAnimation = pAnimation;

		}

		// --------------------------------------------------------------------


		CAnimationShowcase * CurrentShowcase() const {

			return m_pShowcase;

		}

		void SetCurrentShowcase( CAnimationShowcase * pShowcase ) {

			m_pShowcase = pShowcase;

		}

		// --------------------------------------------------------------------

		CFrame * CurrentFrame() const {

			return m_pFrame;

		}

		void SetCurrentFrame( CFrame * pFrame ) {

			m_pFrame = pFrame;

		}

		// --------------------------------------------------------------------

		CAnnotatedPalette * CurrentPalette() const {

			// Overriding palette option?
			// ----------------------------------------------------------------

			if ( m_pPalette ) {

				return m_pPalette;

			}

			// Palette from the animation?
			// ----------------------------------------------------------------

			if ( m_pAnimation ) {

				CAnnotatedPalette * pPal = m_pAnimation->GetPalette();

				if ( pPal ) {

					return pPal;

				}

			}

			// Palette from the showcase?
			// ----------------------------------------------------------------

			if ( m_pShowcase ) {

				CAnnotatedPalette * pPal = m_pShowcase->GetDefaultPalette();

				if ( pPal ) {

					return pPal;

				}

			}

			return 0;

		}

		void SetCurrentPalette( CAnnotatedPalette * pPalette ) {

			m_pPalette = pPalette;

		}

		// This should probably be made into a class so that some type 
		// safety is available unless the file is corrupt this shouldn't ever
		// become an issue, but things happen...
		// --------------------------------------------------------------------

#if 1 // BPT 10/8/02

		bool PointerHasID( void * pVoid ) {

			internal_id_map_type::iterator it = m_InternalIDMap.begin();

			for ( ; it != m_InternalIDMap.end(); it++ ) {

				if ( pVoid == (*it).second ) {

					return true;

				}

			}

			return false;
		}

#endif // BPT 10/8/02

		bool AssociatePointerWithID( void * pVoid, const io_internal_id_type id ) {

			m_InternalIDMap[ id ] = pVoid;

			internal_id_map_type::iterator found = m_InternalIDMap.find( id );

			return ( m_InternalIDMap.end() != found );

		}

		io_internal_id_type SaveIDForPointer( void * pVoid ) {

			internal_id_map_type::iterator it = m_InternalIDMap.begin();

			for ( ; it != m_InternalIDMap.end(); it++ ) {

				if ( pVoid == (*it).second ) {

					return (*it).first;

				}

			}

			++m_NextIOID;

			AssociatePointerWithID( pVoid, m_NextIOID );

			return m_NextIOID;

		}

		void * PointerFromSaveID( const io_internal_id_type id ) {

			internal_id_map_type::iterator found = m_InternalIDMap.find( id );

			if ( m_InternalIDMap.end() != found ) {

				return (*found).second;

			}

			return 0;

		}


	};

}; // namespace BPT

#endif // !defined(AFX_BPTANIMATOR_H__1A7F1BD5_6A97_449C_94FF_7C1F8BCD9737__INCLUDED_)
