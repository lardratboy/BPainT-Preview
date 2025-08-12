// BPTFileio.h: interface for the BPTFileio class.
//
//	Copyright (c) 2000, Brad P. Taylor, LLC
//
//	All rights reserved, unauthorized reproduction prohibited
//
//	-- FILE NOTES --
//
//	These classes should be replaced with something slick someday!
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BPTFILEIO_H__63B8CD0D_A55B_4DCC_8721_2F90A28B37CF__INCLUDED_)
#define AFX_BPTFILEIO_H__63B8CD0D_A55B_4DCC_8721_2F90A28B37CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LoadingSomethingDlg.h"

// ----------------------------------------------------------------------------

#include <utility>
#include <list>
#include <vector>

#define MAKE_CHUNK_ID( a, b, c, d ) \
	((((d)&0xff )<<0x18) | (((c)&0xff )<<0x10) | \
	(((b)&0xff )<<0x08) | (((a)&0xff )<<0x00))

#define IO_LITTLE_ENDIAN

// ----------------------------------------------------------------------------

namespace BPT {

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Very simplistic fileio wrapper. (bare bones)
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

   	class CFileIO {

		enum {

			LARGEST_PARSABLE_LINE	= 16384

		};

   	private:

		// Private data
		// --------------------------------------------------------------------

   		FILE * m_Handle;

		int m_FileLength;

		bool m_bXMLIOMode;

		int m_nNestingLevel;

		// Overly simplistic text centric cache member variables
		// --------------------------------------------------------------------

		BYTE * m_pWholeFileCacheData;

		std::vector< std::pair<BYTE *, BYTE *> > m_pTextLineOffsets;

		bool m_bLineCache;

		int m_nCurrentPos;

		// Internal read text io methods
		// --------------------------------------------------------------------

	public:

		char * Internal_fgets( char * pString, const int nLen ) {

			if ( m_pWholeFileCacheData ) {

				if ( m_bLineCache ) {

					// coming soon!!!!

				} else {

					char * pWrite = pString;

					bool bPartialNewLine = false;

					for ( int i = 0; i < (nLen - 1); i++ ) {

						if ( m_FileLength == m_nCurrentPos ) {

							break;

						}

						int v = *( m_pWholeFileCacheData + m_nCurrentPos++ );

						*pWrite++ = v;

						if ( '\r' == v ) {

							if ( bPartialNewLine ) break;

							bPartialNewLine = true;

						} else if ( '\n' == v ) {

							if ( bPartialNewLine ) break;

							bPartialNewLine = true;

						} else {

							bPartialNewLine = false;

						}

					}

					*pWrite = '\0';

				}

				return pString;

			}

			return fgets( pString, nLen, m_Handle );

		}

		char * Internal_ReadLine() {

			char * pLine = new char [ LARGEST_PARSABLE_LINE ]; // icky? (YES!)

			if ( pLine ) {

				Internal_fgets( pLine, LARGEST_PARSABLE_LINE );

			}

			return pLine; // need to do something

		}

		void Internal_DisposeOfLine( char * pLine ) {

			if ( pLine ) {

				delete [] pLine;

			}

		}

	private:

		int Internal_ReadTextInteger() {

			char * pLine = Internal_ReadLine();

			if ( !pLine ) {

				return 0;

			}

			char * pFound = strstr( pLine, "<int>" );

			int value = 0;

			if ( pFound ) {

				value = atoi( pFound + 5 );

			}

			Internal_DisposeOfLine( pLine );

			return value;

		}

		void Internal_OutputNesting() {

			if ( m_bXMLIOMode ) {

				for ( int i = 0; i < m_nNestingLevel; i++ ) {

					fputc( '\t', m_Handle );

				}

			}

		}

		void Internal_OutputTextInteger( const int value ) {

			Internal_OutputNesting();

			fprintf( m_Handle, "<int>%d</int>\n", value );

		}

		// --------------------------------------------------------------------

   	public:

   		CFileIO() : 
			m_Handle( 0 ), 
			m_FileLength( 0 ),
			m_bXMLIOMode( false ),
			m_nNestingLevel( 0 ),
			m_pWholeFileCacheData( 0 ),
			m_nCurrentPos( 0 ),
			m_bLineCache( false )
			{ /* Empty */ }

   		~CFileIO() {

			Close();

   		}

   		bool Open( const char * filename, const char * access, const bool bXMLIOMode = false ) {

			Close();

#if 1 // BPT 6/6/01

			// Build a read cache for text mode
			// ----------------------------------------------------------------

			if ( (0 != strstr( access, "rt" )) && bXMLIOMode ) {

				// Read the whole thing into RAM!
				// ------------------------------------------------------------

	   			FILE * inFile = fopen( filename, "rb" );

				if ( 0 == inFile ) {

					Close();

					return false;

				}

				// Find the size of the file and read the data into the cache
				// ------------------------------------------------------------

				fseek( inFile, 0, SEEK_END );

				m_FileLength = ftell( inFile );

				fseek( inFile, 0, SEEK_SET );

				m_pWholeFileCacheData = new BYTE [ m_FileLength ];

				if ( !m_pWholeFileCacheData ) {

					Close(); // Don't even bother; it's too damn slow...

					fclose( inFile );

					return false;

				}

				m_bXMLIOMode = bXMLIOMode;

				m_nNestingLevel = 0;

				fread( (void *)m_pWholeFileCacheData, m_FileLength, 1, inFile );

				fclose( inFile );

				// Finally wez ready.
				// ------------------------------------------------------------

				return true;

			}

#endif

   			m_Handle = fopen( filename, access );

			if ( m_Handle ) {

				m_bXMLIOMode = bXMLIOMode;

				m_nNestingLevel = 0;

				Seek( 0, SEEK_END );

				m_FileLength = Tell();

				Seek( 0, SEEK_SET );

//				setvbuf( m_Handle, NULL, _IOFBF, 1024 );

			} else {

				m_FileLength = 0;

			}
   
   			return (0 != m_Handle);
   
   		}
   
   		void Close() {

			// -----------------------

   			if ( m_Handle ) {

   				fclose( m_Handle );

   				m_Handle = 0;

   			}
   
			// -----------------------

			m_bXMLIOMode = false;

			m_nNestingLevel = 0;

			m_FileLength = 0;

			// -----------------------

			if ( m_pWholeFileCacheData ) {

				delete [] m_pWholeFileCacheData;

				m_pWholeFileCacheData = 0;

				m_pTextLineOffsets.clear();

			}

			m_bLineCache = false;

			m_nCurrentPos = 0;

   		}
   
   		void Seek( const int pos, const int mode ) {
   
#if 1 // BPT 6/6/01

			if ( m_pWholeFileCacheData ) {

				if ( SEEK_SET == mode ) {
	
					m_nCurrentPos = pos;
	
				} else if ( SEEK_CUR == mode ) {
	
					m_nCurrentPos += pos;

				} else if ( SEEK_END == mode ) {

					m_nCurrentPos = m_FileLength;

				}

				// limit to valid positions

				m_nCurrentPos = max( 0, min( m_FileLength, m_nCurrentPos ) );

				return;

			}

#endif
				
			if ( m_Handle ) {

   				fseek( m_Handle, pos, mode );

   			}
   
   		}

		int Tell() {

#if 1 // BPT 6/6/01

			if ( m_pWholeFileCacheData ) {

				return m_nCurrentPos;

			}

#endif

			if ( m_Handle ) {

				return ftell( m_Handle );

			}

			return m_FileLength;

		}

		bool ValidFileReadOffset( const int offset ) {

			return ( (offset >= 0) && (offset <= m_FileLength) ); // should be < vs. <= ???

		}

		bool Eof() {

			return (Tell() == m_FileLength);

		}

		int FileSize() {

			return m_FileLength;

		}

		// read methods
		// --------------------------------------------------------------------
   
   		int ReadByte() { 
   
			if ( m_bXMLIOMode ) {

				return (unsigned)Internal_ReadTextInteger() & 0xff;

			}

			if ( m_pWholeFileCacheData ) {

				if ( !Eof() ) {

					return *( m_pWholeFileCacheData + m_nCurrentPos++ );

				}

				return 0;

			}

   			if ( m_Handle ) {

   				return (unsigned)fgetc( m_Handle ) & 0xff;

   			}

   			return 0;
   
   		}
   
   		int Read_m16() {
   
			if ( m_bXMLIOMode ) {

				return (unsigned)Internal_ReadTextInteger() & 0xffff;

			}

   			int hi = ReadByte();
   			int lo = ReadByte();
   
   			return ((hi << 8) | lo);
   
   		}
   
   		int Read_m32() {
   
			if ( m_bXMLIOMode ) {

				return Internal_ReadTextInteger();

			}

   			int a = ReadByte();
   			int b = ReadByte();
   			int c = ReadByte();
   			int d = ReadByte();
   
   			return (int)((a << 24) | (b << 16) | (c << 8) | d);
   
   		}

   		int Read_16() {

			if ( m_bXMLIOMode ) {

				return (unsigned)Internal_ReadTextInteger() & 0xffff;

			}

   			int lo = ReadByte();
   			int hi = ReadByte();
   
   			return ((hi << 8) | lo);
   
   		}
   
   		int Read_32() {
   
			if ( m_bXMLIOMode ) {

				return Internal_ReadTextInteger();

			}

   			int d = ReadByte();
   			int c = ReadByte();
   			int b = ReadByte();
   			int a = ReadByte();
   
   			return (int)((a << 24) | (b << 16) | (c << 8) | d);
   
   		}

   		BYTE * LoadData( const int nBytes ) {

			if ( m_bXMLIOMode ) {

				TRACE( "Request to read %d raw data bytes in text mode!\n", nBytes );

	   			return static_cast<BYTE *>( 0 );

			}

			void * ptr = ::CoTaskMemAlloc( nBytes );
   
			if ( 0 == ptr ) {
   
	   			return static_cast<BYTE *>( 0 );
   
			}

			if ( m_pWholeFileCacheData ) {

				int dataLeft = (m_FileLength - m_nCurrentPos);

				int copyAmount = min( dataLeft, nBytes );

				if ( copyAmount ) {

					memcpy( ptr, m_pWholeFileCacheData + m_nCurrentPos, copyAmount );

					m_nCurrentPos += copyAmount;

				}

				int leftOver = nBytes - copyAmount;

				if ( leftOver ) {

					memset( (BYTE *)ptr + copyAmount, 0, leftOver );

				}

			} else if ( m_Handle ) {

   
   				fread( ptr, 1, nBytes, m_Handle );

   			}

			return static_cast<BYTE *>( ptr );

   		}

   		void UnloadData( BYTE * ptr ) {

   			if ( ptr ) {

   				::CoTaskMemFree( static_cast<void *>( ptr ) );

   			}

   		}

		template< class T > void T_Read( T & v ) {

			if ( 1 == sizeof( v ) ) {

				v = ReadByte();

			} else if ( 2 == sizeof( v ) ) {

				v = Read_16();

			} else if ( 4 == sizeof( v ) ) {

				v = Read_32();

			} else {

				// This should be handled some how!

			}

		}

		char * ReadString() {

			// Handle text mode read of the string data...
			// ----------------------------------------------------------------

			if ( m_bXMLIOMode ) {

				// read the line of text
				// ------------------------------------------------------------

				char * pLine = Internal_ReadLine();
	
				if ( !pLine ) {

					TRACE( "Failed to read line of data from file?\n" );
	
					return static_cast< char * >( 0 );
	
				}
	
				// Find the <string> tag and convert what's between :)
				// ------------------------------------------------------------

				char * quotedString = 0;

				char * pBegin = strstr( pLine, "<string>" );

				if ( pBegin ) {

					// Adjust the working pointers to where the string is
					// --------------------------------------------------------

					char * pEnd = strstr( pBegin, "</string>" );

					if ( !pEnd ) {

						pEnd = pBegin + strlen( pBegin );

					}

					pBegin += 8; // skip the <string> setting

					// Estimate the size in chars the buffer needs to be
					// this of course may be more storage than is necessary
					// but hey it's only a few bytes...
					// --------------------------------------------------------

					int count = (pEnd - pBegin) + 1;

					// Allocate the memory and then copy the data over...
					// --------------------------------------------------------

					quotedString = reinterpret_cast<char *>( ::CoTaskMemAlloc( count * sizeof(char) ) );

					if ( quotedString ) {

						char * pWrite = quotedString;
	
						for ( char * it = pBegin; it != pEnd; it++ ) {
	
							int c = *it;
	
							if ( '\\' == c ) {
	
								if ( pEnd == ++it ) {
	
									break;
	
								}
	
								c = *it;
	
								if ( 'r' == c ) {
									
									*pWrite++ = '\r';

								} else if ( 'n' == c ) {
									
									*pWrite++ = '\n';

								} else if ( 't' == c ) {
	
									*pWrite++ = '\t';
	
								} else if ( '\\' == c ) {

									*pWrite++ = '\\';

								} else {

									// just write the unknown \? thing

									*pWrite++ = '\\';
									*pWrite++ = c;

								}
	
							} else if ( '&' == c ) { // check for XML friendly 'chars'

								if ( pEnd == ++it ) {
	
									*pWrite++ = c;

									break;
	
								}

								bool bSkipXMLConstant = true;

								// strstr isn't completely valid here because it will compare
								// past pEnd (probably not important but worth noting...)

								if ( strstr( it, "lt;" ) ) {

									*pWrite++ = '<';

								} else if ( strstr( it, "gt;" ) ) {

									*pWrite++ = '>';

								} else if ( strstr( it, "quot;" ) ) {

									*pWrite++ = '\"';

								} else if ( strstr( it, "amp;" ) ) {

									*pWrite++ = '&';

								} else {

									*pWrite++ = c;

									bSkipXMLConstant = false;

								}

								if ( bSkipXMLConstant ) {

									while ( ';' != *it ) {

										if ( pEnd == ++it ) {

											goto QUICK_BAILOUT;

										}

									}

									++it;

								}

							} else {
	
								*pWrite++ = c;
	
							}
	
						}

					QUICK_BAILOUT:

						*pWrite = '\0';

					}

				} else {

					quotedString = reinterpret_cast<char *>( ::CoTaskMemAlloc( 1 ) );

					if ( quotedString ) {

						*quotedString = '\0';

					}

				}

				Internal_DisposeOfLine( pLine );
	
				return quotedString;

			}

			// Figure out the length of the string
			// ----------------------------------------------------------------

			int pos = Tell();

			int length = 0;

			do {

				++length;

			} while ( 0 != ReadByte() );

			Seek( pos, SEEK_SET );

			// Okay now lets read using the data read method
			// ----------------------------------------------------------------

			return reinterpret_cast<char *>( LoadData( length ) );

		}

		// write methods
		// --------------------------------------------------------------------

		void WriteByte( const int value ) {

			if ( m_Handle ) {

				if ( m_bXMLIOMode ) {

					Internal_OutputTextInteger( value );

				}  else {

					fputc( value, m_Handle );

				}

			}

		}

		void Write_16( const int value ) {

			if ( m_bXMLIOMode ) {

				Internal_OutputTextInteger( value );
				return;

			}

			WriteByte( value & 0xff );
			WriteByte( (value >> 8) & 0xff );

		}

		void Write_32( const int value ) {

			if ( m_bXMLIOMode ) {

				Internal_OutputTextInteger( value );
				return;

			}

			Write_16( value & 0xffff );
			Write_16( (value >> 16) & 0xffff );

		}

		void Write_m16( const int value ) {

			if ( m_bXMLIOMode ) {

				Internal_OutputTextInteger( value );
				return;

			}

			WriteByte( (value >> 8) & 0xff );
			WriteByte( value & 0xff );

		}

		void Write_m32( const int value ) {

			if ( m_bXMLIOMode ) {

				Internal_OutputTextInteger( value );
				return;

			}

			Write_16( (value >> 16) & 0xffff );
			Write_16( value & 0xffff );

		}

		void WriteNBytes( const void * data, const int nBytes ) {

			if ( m_bXMLIOMode ) {

				TRACE( "Request to write %d raw data bytes in text mode!\n", nBytes );

				return;

			}

			if ( m_Handle ) {

				fwrite( data, nBytes, 1, m_Handle );

			}

		}

		template< class T > void T_Write( const T & v ) {

			if ( 1 == sizeof( v ) ) {

				WriteByte( v );

			} else if ( 2 == sizeof( v ) ) {

				Write_16( v );

			} else if ( 4 == sizeof( v ) ) {

				Write_32( v );

			} else {

				WriteNBytes( (const void *)(&v), sizeof( v ) );

			}

		}

		bool IsXMLFriendly( const int value ) {

			if ( iscntrl(value) || (!isprint(value)) ) {

				return false;

			}

			if (
				0
				|| ('>' == value)
				|| ('<' == value)
				|| ('&' == value)
				|| ('\"' == value)
				) {

				return false;
				
			}
		

			return true;

		}

		CString ConvertToXMLFriendly( const int value ) {

			CString r;

			if ( IsXMLFriendly( value ) ) {

				r.Format( "%c", value );

			} else {

				if ( '<' == value ) {

					r = "&lt;";

				} else if ( '>' == value ) {

					r = "&gt;";

				} else if ( '&' == value ) {

					r = "&amp;";

				} else if ( '\"' == value ) {

					r = "&quot;";

				} else {

					r = " ";

				}

			}

			return r;

		}

		void WriteString( const char * pStr ) {

			// Convert the string to an friendly format for future reading...
			// ----------------------------------------------------------------

			if ( m_bXMLIOMode ) {

				Internal_OutputNesting();

				fprintf( m_Handle, "<string>" ); // start

				int v;

				while ( v = *pStr++ ) {

					if ( iscntrl(v) || (!isprint(v)) ) {

						if ( '\t' == v ) {

							fprintf( m_Handle, "\\t" );

						} else if ( '\r' == v ) {

							fprintf( m_Handle, "\\r" );

						} else if ( '\n' == v ) {

							fprintf( m_Handle, "\\n" );

						}

					} else if ( IsXMLFriendly( v ) ) {

						if ( '\\' != v ) {

							fputc( v, m_Handle );

						} else {

							fputc( v, m_Handle );
							fputc( v, m_Handle );

						}

					} else {

						fprintf( m_Handle, "%s", ConvertToXMLFriendly( v ) );

					}

				}

				fprintf( m_Handle, "</string>\n" ); // finish

				return;

			}

			// ----------------------------------------------------------------

			WriteNBytes( static_cast<const void *>( pStr ), ((strlen( pStr ) + 1) * sizeof( char )) );

		}

		// Text IO methods
		// --------------------------------------------------------------------

		bool IsTextIOMode() {

			return m_bXMLIOMode;

		}

		void SetTextNesting( const int nNestingLevel ) {

			m_nNestingLevel = nNestingLevel;

		}

		// need low level access methods for outputting various items...

		void TextOutput( const char * pString ) {

			Internal_OutputNesting();

			fprintf( m_Handle, "%s\n", pString );

		}

		// hack search method

		int TextFindTextOffset( const char * pString ) {

			char * pLine = new char [ LARGEST_PARSABLE_LINE ]; // icky? (YES!)

			if ( !pLine ) {

				return -1; // ????

			}

			int initialOffset = Tell();

			int foundOffset = initialOffset;

			// ----------------------------------------------------------------

			while ( !Eof() ) {

				int currentOffset = Tell();

				Internal_fgets( pLine, LARGEST_PARSABLE_LINE );

				if ( 0 != strstr( pLine, pString ) ) {

					foundOffset = currentOffset;

					break;

				}

			}

			delete [] pLine;

			// return to our incoming position
			// ----------------------------------------------------------------

			Seek( initialOffset, SEEK_SET );

			return foundOffset;

		}

		// helper method to skip a whole line

		void TextSkipLine() {

			char * pLine = Internal_ReadLine();

			if ( pLine ) {

				Internal_DisposeOfLine( pLine );

			}

		}

   	};

	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Simplistic chunky file (bare bones)
	// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	class CChunkyFileIO {

	public: // traits

		typedef DWORD id_type;
		typedef DWORD size_type;

		struct chunk_reference {

			const struct chunk_reference * pOutter;

			id_type id;
			size_type size;
			int fileOffset;
			int lastValidReadPos;

		};

		typedef std::list<chunk_reference> inner_chunk_collection;

	private: // data

		typedef std::pair< id_type, int > working_block_type;
		std::list< working_block_type > m_Stack;
		CFileIO m_IO;
		int m_Pad;

#if 1 // status dialog

		CLoadingSomethingDlg * m_pStatusDlg;

		DWORD m_nextUpdateTime;

		bool m_bNeedToShowDlg;

		bool m_bWriteMode;
		int m_nWriteStatusBouncer;
		int m_nWriteStatusBounceDelta;

#endif

	private: // methods

		id_type Read_ID() {

			if ( !m_IO.IsTextIOMode() ) {

				return m_IO.Read_32();

			}

			// Okay let's have some fun!

			char * pLine = m_IO.Internal_ReadLine();

			if ( pLine ) {

				id_type id = id_type( 0 );

				char * pTagStart = strstr( pLine, "<" );

				if ( pTagStart ) {

					// This is lame but hey...
	
					id = MAKE_CHUNK_ID(
						*(pTagStart + 1),
						*(pTagStart + 2),
						*(pTagStart + 3),
						*(pTagStart + 4)
					);

				}

				m_IO.Internal_DisposeOfLine( pLine );

				return id;

			}

			return id_type(0);

		}

		void Write_ID( const id_type id ) {

			if ( !m_IO.IsTextIOMode() ) {

				m_IO.Write_32( id );

			}

		}

		size_type Read_Size() {

			if ( m_IO.IsTextIOMode() ) {

				return 0;

			}

			return m_IO.Read_32();

		}

		void Write_Size( const size_type size ) {

			if ( !m_IO.IsTextIOMode() ) {

				m_IO.Write_32( size );

			} else {

				TRACE( "Trying to write the size in text mode?\n" );

			}

		}

		CFileIO * GetIO() {

#if 1 // status dialog

			if ( m_pStatusDlg ) {

				// Check the last update time...

				DWORD curTime = timeGetTime();

				if ( m_nextUpdateTime <= curTime ) {

					if ( m_bWriteMode ) {

						m_pStatusDlg->UpdateStatus( m_nWriteStatusBouncer, 0, 10 );

						m_nWriteStatusBouncer += m_nWriteStatusBounceDelta;

						if ( (0 >= m_nWriteStatusBouncer) || (10 <= m_nWriteStatusBouncer) ) {

							m_nWriteStatusBounceDelta = -m_nWriteStatusBounceDelta;

						}

					} else {

						m_pStatusDlg->UpdateStatus( m_IO.Tell(), 0, m_IO.FileSize() );

					}

					if ( m_bNeedToShowDlg ) {

						m_pStatusDlg->CenterWindow( CWnd::GetDesktopWindow() );

						m_pStatusDlg->ShowWindow( SW_SHOW );

						m_pStatusDlg->UpdateWindow();

						m_bNeedToShowDlg = false;

					}

					if ( m_bWriteMode ) {

						m_nextUpdateTime = timeGetTime() + 100;

					} else {

						m_nextUpdateTime = timeGetTime() + 250;

					}

				}

			}

#endif

			return &m_IO;
			
		}

		// --------------------------------------------------------------------

		CString GenerateIDCString( const id_type id ) {

			CString str;

			str.Format(
				"%c%c%c%c",
				((id >>   0) & 0xff),
				((id >>   8) & 0xff),
				((id >>  16) & 0xff),
				((id >>  24) & 0xff)
			);

			return str;

		}

	public: // interface

		// -------------------------------------------------------------------

		CChunkyFileIO( const int pad = 2 ) : 
			m_Pad( pad ), 
			m_pStatusDlg( 0 ), 
			m_nextUpdateTime( 0 ),
			m_bWriteMode( false ),
			m_bNeedToShowDlg( false )
			{ /* Empty */ }

		~CChunkyFileIO() {

			Close();

		}

		void Close() {

			if ( m_pStatusDlg ) {

				m_pStatusDlg->DestroyWindow();

				delete m_pStatusDlg;

				m_pStatusDlg = 0;

				m_bNeedToShowDlg = true;

			}

			m_Stack.clear();

			m_IO.Close();

		}

		bool Open( const char * filename, const char * access, const bool bXMLIOMode = false  ) {

			Close();

#if 1 // status dialog

			m_pStatusDlg = new CLoadingSomethingDlg;

			m_bNeedToShowDlg = true;

			m_bWriteMode = (0 != strstr( access, "w" ));
			m_nWriteStatusBouncer = 0;
			m_nWriteStatusBounceDelta = 1;

			if ( m_pStatusDlg ) {

				if ( !m_pStatusDlg->Create( CLoadingSomethingDlg::IDD ) ) {

					delete m_pStatusDlg;

					m_pStatusDlg = 0;

				}

			}

			m_nextUpdateTime = timeGetTime() + 2000; // Initial time until dialog shows up

#endif

			return m_IO.Open( filename, access, bXMLIOMode );

		}

		// -------------------------------------------------------------------

		bool IsTextIOMode() {

			return m_IO.IsTextIOMode();

		}

		// Write code
		// -------------------------------------------------------------------

		CFileIO * BeginBlock( const id_type id ) {

			// Write the partial header to reserve space

			if ( m_IO.IsTextIOMode() ) {

				m_IO.SetTextNesting( m_Stack.size() );

				// NEED TO OUTPUT BLOCK ID AS TEXT "<XXXX>"

				CString blockHeader;

				blockHeader.Format( "<%s>", GenerateIDCString( id ) );

				m_IO.TextOutput( blockHeader );

				// NEED TO OUTPUT BLOCK ID AS TEXT "<XXXX>"

				m_IO.SetTextNesting( m_Stack.size() + 1 );

			} else {

				Write_ID( id );
		
				Write_Size( size_type( 0 ) );

			}

			// store the block position to 

			m_Stack.push_back( working_block_type( id, m_IO.Tell() ) );

			return GetIO();

		}

		bool FinishBlock( const id_type id ) {

			if ( m_Stack.empty() || (id != m_Stack.back().first) ) {

				// This is a bad error should be reported somehow!!!

				return false;

			}

			if ( m_IO.IsTextIOMode() ) {

				m_IO.SetTextNesting( m_Stack.size() - 1 );

				// NEED TO OUTPUT BLOCK ID AS TEXT "</XXXX>"

				CString blockFooter;

				blockFooter.Format( "</%s>", GenerateIDCString( id ) );

				m_IO.TextOutput( blockFooter );

				// NEED TO OUTPUT BLOCK ID AS TEXT "</XXXX>"

				m_Stack.pop_back();

				return true;

			}

			// Get the current position so that the size can be calculated
			// ----------------------------------------------------------------

			int pos = m_IO.Tell();
			int size = pos - m_Stack.back().second;

			// Write the size into the previously reserved chunk header
			// ----------------------------------------------------------------

			m_IO.Seek( m_Stack.back().second - sizeof(size_type), SEEK_SET );

			Write_Size( size_type( size ) );

			// Remove the stack entry and move back to write any padding
			// ----------------------------------------------------------------

			m_Stack.pop_back();

			m_IO.Seek( pos, SEEK_SET );

			// write the pad data
			// ----------------------------------------------------------------

			if ( m_Pad ) {

				int p = size % m_Pad;
	
				if ( p ) {
	
					for ( int i = 0; i < (m_Pad - p); i++ ) {
	
						m_IO.WriteByte( 0 );
	
					}
	
				}

			}

			return true;

		}

		// Helper 'functions'
		// -------------------------------------------------------------------

#if 0

		void WriteDataAsChunk( const id_type id, const void * ptr, const int nBytes ) {

			BeginBlock( id );

			m_IO.WriteNBytes( ptr, nBytes );

			FinishBlock( id );

		}

		void WriteStringAsChunk( const id_type id, const char * pStr ) {

			WriteDataAsChunk( id, static_cast<const void *>( pStr ), strlen( pStr ) + 1 );

		}

#endif

		// Read code
		// -------------------------------------------------------------------

		chunk_reference GetCurrentChunkReference(
			const chunk_reference * pOutter /* = 0 */ // BPT 10/9/02
		) {

			chunk_reference ref;

			ref.pOutter = pOutter;

			ref.fileOffset = m_IO.Tell();

			ref.id = Read_ID();

			if ( m_IO.IsTextIOMode() ) {

				ref.size = 0; // in text mode this isn't valid so set it to 0!

				CString endTag;

				endTag.Format( "</%s>", GenerateIDCString( ref.id ) );

				// this needs to read a file line and find the first tag
				// then search for the ending tag that matches it
				// the real question is what about recursive tag definitions?
				// ------------------------------------------------------------

				ref.lastValidReadPos = m_IO.TextFindTextOffset( endTag );

			} else {

				ref.size = Read_Size();

				ref.lastValidReadPos = ref.fileOffset + (int)ref.size - 1;

			}

			m_IO.Seek( ref.fileOffset, SEEK_SET );

			return ref;

		}

		bool SkipChunk( const chunk_reference * pChunkRef = 0 ) {

			int fileOffset;

			if ( m_IO.IsTextIOMode() ) {

				// If we have a specific chunk to work from do so otherwise build
				// -----------------------------------------------------------------

				const chunk_reference * pTestChunkRef = pChunkRef;

				chunk_reference forcedBuildChunkRef;

				if ( !pChunkRef ) {

					forcedBuildChunkRef = GetCurrentChunkReference( 0 );

					pTestChunkRef = &forcedBuildChunkRef;

				}

				// Okay let's get this party started!
				// -----------------------------------------------------------------

				m_IO.Seek( pTestChunkRef->lastValidReadPos, SEEK_SET );

				// Need to skip the 'ending tag'
				// -----------------------------------------------------------------

				m_IO.TextSkipLine();

				// Where are we now?
				// -----------------------------------------------------------------

				fileOffset = m_IO.Tell();

			} else {

				// Move back to the chunk header file position
				// ----------------------------------------------------------------
	
				if ( pChunkRef ) {
	
					fileOffset = pChunkRef->fileOffset;
	
				} else {
	
					fileOffset = m_IO.Tell();
	
				}
	
				fileOffset += sizeof( id_type );
	
				int chunkSize;
	
				if ( pChunkRef ) {
	
					chunkSize = pChunkRef->size;
	
				} else {
	
					m_IO.Seek( fileOffset, SEEK_SET );
	
					chunkSize = int( Read_Size() );
	
				}
	
				fileOffset += sizeof( size_type );
	
				if ( m_Pad ) {
	
					int p = chunkSize % m_Pad;
		
					if ( p ) {
		
						fileOffset += (m_Pad - p);
		
					}
	
				}
	
				fileOffset += chunkSize;
	
				// Finally move into final position
				// ----------------------------------------------------------------
	
				m_IO.Seek( fileOffset, SEEK_SET );

			}

			// return true if there are more chunks available.
			// ----------------------------------------------------------------

			if ( (!pChunkRef) || (!pChunkRef->pOutter) ) {

				return !m_IO.Eof();

			}

			// Check to see if we are at the end of the outter chunk
			// ----------------------------------------------------------------

#if 0 // EXTREME DEBUGGING
			TRACE(
				"Curr 0x%08x outter 0x%08x offset %d limit %d\n",
				pChunkRef->id, pChunkRef->pOutter->id,
				fileOffset, pChunkRef->pOutter->lastValidReadPos
			);
#endif

			return (fileOffset <= pChunkRef->pOutter->lastValidReadPos);

		}

		// Begin chunk read
		// -------------------------------------------------------------------

		CFileIO * BeginChunkDataRead( const chunk_reference * pChunkRef = 0 ) {

			int fileOffset;

			if ( m_IO.IsTextIOMode() ) {

				// Move to the start of the passed in chunk reference
				// -----------------------------------------------------------------

				if ( pChunkRef ) {

					m_IO.Seek( pChunkRef->fileOffset, SEEK_SET );

				}

				// Need to skip the 'start tag'
				// -----------------------------------------------------------------

				m_IO.TextSkipLine();

			} else {

				// Move the 'file' position to inside the chunk header
				// ----------------------------------------------------------------
	
				if ( pChunkRef ) {
	
					fileOffset = pChunkRef->fileOffset;
	
				} else {
	
					fileOffset = m_IO.Tell();
	
				}
	
				fileOffset += sizeof( id_type ) + sizeof( size_type );

				m_IO.Seek( fileOffset, SEEK_SET );

			}

			// Return the IO class to the client for further processing
			// ----------------------------------------------------------------

			return GetIO();

		}

		// Nested chunk methods
		// -------------------------------------------------------------------

		chunk_reference GetNestedChunkReference( const chunk_reference * pChunkRef = 0 ) {

			// Move the 'file' position to inside the chunk header
			// ----------------------------------------------------------------

			BeginChunkDataRead( pChunkRef );

			// Ask the core chunk reference method to do it's job.
			// ----------------------------------------------------------------

			return GetCurrentChunkReference( pChunkRef );

		}

		// Chunk validity test? 
		// -------------------------------------------------------------------

		bool IsChunkReferenceIDValid( const chunk_reference * pChunkRef ) {

			if ( !m_IO.ValidFileReadOffset( pChunkRef->fileOffset ) ) {

				return false;

			}

			return (
				isalnum( (pChunkRef->id >> 0  ) & 0xff ) &&
				isalnum( (pChunkRef->id >> 8  ) & 0xff ) &&
				isalnum( (pChunkRef->id >> 16 ) & 0xff ) &&
				isalnum( (pChunkRef->id >> 24 ) & 0xff )
			);

		}

	};

}; // namespace BPT

#endif // !defined(AFX_BPTFILEIO_H__63B8CD0D_A55B_4DCC_8721_2F90A28B37CF__INCLUDED_)
