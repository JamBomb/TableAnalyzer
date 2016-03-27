/* =====================================================================
TackID		Name        Date         Version                         Command
AJ2D14646   y00159558   2009.11.26   UTPSSDK V200R001B002D01    avoid the Warning of the PC-Lint
======================================================================*/
// MarkupSTL.cpp: implementation of the CMarkupSTL class.
//
// Markup Release 6.5
// Copyright (C) 1999-2003 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.
//#pragma warning(disable:4244)

#include <stdio.h>
#include <stdlib.h>
#include "MarkupSTL.h"


using namespace std;

// Customization
#define x_EOL "\r\n" // can be \r\n or \n or empty
#define x_EOLLEN (sizeof(x_EOL)-1); // string length of x_EOL
#define x_ATTRIBQUOTE "\"" // can be double or single quote


CMarkupSTL::CMarkupSTL()
{
    SetDoc( NULL );
}
CMarkupSTL::CMarkupSTL( const char* szDoc )
{
    SetDoc( szDoc );
}
CMarkupSTL::CMarkupSTL( const CMarkupSTL& markup )
{
    *this = markup;
}

void CMarkupSTL::operator=( const CMarkupSTL& markup )
{
    // Begin AJ2D14646 Modify by y00159558 , m_strError and m_mapSavedPos variable should be assigned value
	if(&markup != this)
	{
		m_iPosParent = markup.m_iPosParent;
		m_iPos = markup.m_iPos;
		m_iPosChild = markup.m_iPosChild;
		m_iPosFree = markup.m_iPosFree;
		m_nNodeType = markup.m_nNodeType;
		m_aPos = markup.m_aPos;
		m_strDoc = markup.m_strDoc;
		m_strError = markup.m_strError;
		m_mapSavedPos = markup.m_mapSavedPos;  
		MARKUP_SETDEBUGSTATE;
	}
	else
	{
		return ;
	}
	//End AJ2D14646 Modify by y00159558 , m_strError and m_mapSavedPos variable should be assigned value
}

CMarkupSTL::~CMarkupSTL()
{

}

bool CMarkupSTL::SetDoc( const char* szDoc )
{
	// Reset indexes
	m_iPosFree = 1;
	ResetPos();
	m_mapSavedPos.clear();

	// Set document text
	if ( szDoc )
		m_strDoc = szDoc;
	else
		m_strDoc.erase();

	// Starting size of position array: 1 element per 64 bytes of document
	// Tight fit when parsing small doc, only 0 to 2 reallocs when parsing large doc
	// Start at 8 when creating new document
	int nStartSize = int(m_strDoc.size()) / 64 + 8;
	if ( (int)m_aPos.size() < nStartSize )
		m_aPos.resize( nStartSize );

	// Parse document
	bool bWellFormed = false;
	if ( m_strDoc.size() )
	{
		m_aPos[0].Clear();
		int iPos = x_ParseElem( 0 );
		if ( iPos > 0 )
		{
			m_aPos[0].iElemChild = iPos;
			bWellFormed = true;
		}
	}

	// Clear indexes if parse failed or empty document
	if ( ! bWellFormed )
	{
		m_aPos[0].Clear();
		m_iPosFree = 1;
	}

	ResetPos();
	return bWellFormed;
};

bool CMarkupSTL::IsWellFormed()
{
	if ( m_aPos.size() && m_aPos[0].iElemChild )
		return true;
	return false;
}

bool CMarkupSTL::Load( const char* szFileName )
{
	// Load document from file
	bool bResult = false;
	FILE* fp = fopen( szFileName, "rb" );
	if ( fp )
	{
		// Determine file length
		fseek( fp, 0L, SEEK_END );
		int nFileLen = ftell(fp);
		fseek( fp, 0L, SEEK_SET );

		// Load string
		allocator<char> mem;
		allocator<char>::pointer pBuffer = mem.allocate(nFileLen+1, NULL);
		if ( fread( pBuffer, nFileLen, 1, fp ) == 1 )
		{
			pBuffer[nFileLen] = '\0';
			bResult = SetDoc( pBuffer );
		}
		fclose(fp);
		mem.deallocate(pBuffer,nFileLen+1);
	}
	if ( ! bResult )
		SetDoc(NULL);
	MARKUP_SETDEBUGSTATE;
	return bResult;
}

bool CMarkupSTL::Save( const char* szFileName )
{
	// Save document to file
	bool bResult = false;
	FILE* fp = fopen( szFileName, "wb" );
	if ( fp )
	{
		// Save string
		int nFileLen = int(m_strDoc.size());
		if ( ! nFileLen )
			bResult = true;
		else if ( fwrite( m_strDoc.c_str(), nFileLen, 1, fp ) == 1 )
			bResult = true;
		fclose(fp);
	}
	return bResult;
}


std::string CMarkupSTL::GetDoc() const
{
    return m_strDoc;
}
bool CMarkupSTL::AddElem( const char* szName, const char* szData )
{
    return x_AddElem(szName,szData,false,false);
}
bool CMarkupSTL::InsertElem( const char* szName, const char* szData )
{
    return x_AddElem(szName,szData,true,false);
}
bool CMarkupSTL::AddChildElem( const char* szName, const char* szData )
{
    return x_AddElem(szName,szData,false,true);
}
bool CMarkupSTL::InsertChildElem( const char* szName, const char* szData )
{
    return x_AddElem(szName,szData,true,true);
}
bool CMarkupSTL::AddAttrib( const char* szAttrib, const char* szValue )
{
    return x_SetAttrib(m_iPos,szAttrib,szValue);
}
bool CMarkupSTL::AddChildAttrib( const char* szAttrib, const char* szValue )
{
    return x_SetAttrib(m_iPosChild,szAttrib,szValue);
}
bool CMarkupSTL::AddAttrib( const char* szAttrib, int nValue )
{
    return x_SetAttrib(m_iPos,szAttrib,nValue);
}
bool CMarkupSTL::AddChildAttrib( const char* szAttrib, int nValue )
{
    return x_SetAttrib(m_iPosChild,szAttrib,nValue);
}
bool CMarkupSTL::AddSubDoc( const char* szSubDoc )
{
    return x_AddSubDoc(szSubDoc,false,false);
}






bool CMarkupSTL::FindElem( const char* szName )
{
	// Change current position only if found
	//
	if ( m_aPos.size() )
	{
		int iPos = x_FindElem( m_iPosParent, m_iPos, szName );
		if ( iPos )
		{
			// Assign new position
			x_SetPos( m_aPos[iPos].iElemParent, iPos, 0 );
			return true;
		}
	}
	return false;
}

bool CMarkupSTL::FindChildElem( const char* szName )
{
	// Change current child position only if found
	//
	// Shorthand: call this with no current main position
	// means find child under root element
	if ( ! m_iPos )
		FindElem();

	int iPosChild = x_FindElem( m_iPos, m_iPosChild, szName );
	if ( iPosChild )
	{
		// Assign new position
		int iPos = m_aPos[iPosChild].iElemParent;
		x_SetPos( m_aPos[iPos].iElemParent, iPos, iPosChild );
		return true;
	}

	return false;
}


string CMarkupSTL::GetTagName() const
{
	// Return the tag name at the current main position
	string strTagName;


	if ( m_iPos )
		strTagName = x_GetTagName( m_iPos );
	return strTagName;
}

bool CMarkupSTL::IntoElem()
{
	// If there is no child position and IntoElem is called it will succeed in release 6.3
	// (A subsequent call to FindElem will find the first element)
	// The following short-hand behavior was never part of EDOM and was misleading
	// It would find a child element if there was no current child element position and go into it
	// It is removed in release 6.3, this change is NOT backwards compatible!
	// if ( ! m_iPosChild )
	//	FindChildElem();

	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		x_SetPos( m_iPos, m_iPosChild, 0 );
		return true;
	}
	return false;
}

bool CMarkupSTL::OutOfElem()
{
	// Go to parent element
	if ( m_iPosParent )
	{
		x_SetPos( m_aPos[m_iPosParent].iElemParent, m_iPosParent, m_iPos );
		return true;
	}
	return false;
}


void CMarkupSTL::ResetChildPos()
{
    x_SetPos(m_iPosParent,m_iPos,0);
}

void CMarkupSTL::ResetMainPos()
{
    x_SetPos(m_iPosParent,0,0);
}
void CMarkupSTL::ResetPos()
{
    x_SetPos(0,0,0);
}

std::string CMarkupSTL::GetChildTagName() const
{
    return x_GetTagName(m_iPosChild);
}
std::string CMarkupSTL::GetData() const
{
    return x_GetData(m_iPos);
}
std::string CMarkupSTL::GetChildData() const
{
    return x_GetData(m_iPosChild);
}
std::string CMarkupSTL::GetAttrib( const char* szAttrib ) const
{
    return x_GetAttrib(m_iPos,szAttrib);
}
std::string CMarkupSTL::GetChildAttrib( const char* szAttrib ) const
{
    return x_GetAttrib(m_iPosChild,szAttrib);
}

string CMarkupSTL::GetAttribName( int n ) const
{
	// Return nth attribute name of main position
	TokenPos token( m_strDoc.c_str() );
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[m_iPos].nStartL + 1;
	else
		return "";

	for ( int nAttrib=0; nAttrib<=n; ++nAttrib )
		if ( ! x_FindAttrib(token) )
			return "";

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkupSTL::SavePos( const char* szPosName )
{
	// Save current element position in saved position map
	if ( szPosName )
	{
		SavedPos savedpos;
		savedpos.iPosParent = m_iPosParent;
		savedpos.iPos = m_iPos;
		savedpos.iPosChild = m_iPosChild;
		string strPosName = szPosName;
		m_mapSavedPos[strPosName] = savedpos;
		return true;
	}
	return false;
}

bool CMarkupSTL::RestorePos( const char* szPosName )
{
	// Restore element position if found in saved position map
	if ( szPosName )
	{
		string strPosName = szPosName;
		mapSavedPosT::const_iterator iterSavePos = m_mapSavedPos.find( strPosName );
		if ( iterSavePos != m_mapSavedPos.end() )
		{
			SavedPos savedpos = (*iterSavePos).second;
			x_SetPos( savedpos.iPosParent, savedpos.iPos, savedpos.iPosChild );
			return true;
		}
	}
	return false;
}

std::string CMarkupSTL::GetError() const
{
    return m_strError;
}


bool CMarkupSTL::RemoveElem()
{
	// Remove current main position element
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		int iPos = x_RemoveElem( m_iPos );
		x_SetPos( m_iPosParent, iPos, 0 );
		return true;
	}
	return false;
}

bool CMarkupSTL::RemoveChildElem()
{
	// Remove current child position element
	if ( m_iPosChild )
	{
		int iPosChild = x_RemoveElem( m_iPosChild );
		x_SetPos( m_iPosParent, m_iPos, iPosChild );
		return true;
	}
	return false;
}
bool CMarkupSTL::SetAttrib( const char* szAttrib, const char* szValue )
{
    return x_SetAttrib(m_iPos,szAttrib,szValue);
}

bool CMarkupSTL::SetChildAttrib( const char* szAttrib, const char* szValue )
{
    return x_SetAttrib(m_iPosChild,szAttrib,szValue);
}

bool CMarkupSTL::SetAttrib( const char* szAttrib, int nValue )
{
    return x_SetAttrib(m_iPos,szAttrib,nValue);
}
bool CMarkupSTL::SetChildAttrib( const char* szAttrib, int nValue )
{
    return x_SetAttrib(m_iPosChild,szAttrib,nValue);
}
bool CMarkupSTL::SetData( const char* szData, int nCDATA )
{
    return x_SetData(m_iPos,szData,nCDATA);
}

bool CMarkupSTL::SetChildData( const char* szData, int nCDATA )
{
    return x_SetData(m_iPosChild,szData,nCDATA);
}



















//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

int CMarkupSTL::x_GetFreePos()
{
	//
	// This returns the index of the next unused ElemPos in the array
	//
        if ( m_iPosFree == (int)m_aPos.size() )
		m_aPos.resize( m_iPosFree + m_iPosFree / 2 );
	++m_iPosFree;
	return m_iPosFree - 1;
}

int CMarkupSTL::x_ReleasePos()
{
	//
	// This decrements the index of the next unused ElemPos in the array
	// allowing the element index returned by GetFreePos() to be reused
	//
	--m_iPosFree;
	return 0;
}

int CMarkupSTL::x_ParseError( const char* szError, const char* szName )
{
	if ( szName )
	{
		char szFormat[300];
		sprintf( szFormat, szError, szName );
		m_strError = szFormat;
	}
	else
		m_strError = szError;
	x_ReleasePos();
	return -1;
}

int CMarkupSTL::x_ParseElem( int iPosParent )
{
	// This is either called by SetDoc, x_AddSubDoc, or itself recursively
	// m_aPos[iPosParent].nEndL is where to start parsing for the child element
	// This returns the new position if a tag is found, otherwise zero
	// In all cases we need to get a new ElemPos, but release it if unused
	//
	int iPos = x_GetFreePos();
	m_aPos[iPos].nStartL = m_aPos[iPosParent].nEndL;
	m_aPos[iPos].iElemParent = iPosParent;
	m_aPos[iPos].iElemChild = 0;
	m_aPos[iPos].iElemNext = 0;

	// Start Tag
	// A loop is used to ignore all remarks tags and special tags
	// i.e. <?xml version="1.0"?>, and <!-- comment here -->
	// So any tag beginning with ? or ! is ignored
	// Loop past ignored tags
	TokenPos token( m_strDoc.c_str() );
	token.nNext = m_aPos[iPosParent].nEndL;
	string strName;
	while ( strName.empty() )
	{
		// Look for left angle bracket of start tag
		m_aPos[iPos].nStartL = token.nNext;
		if ( ! x_FindChar( token.szDoc, m_aPos[iPos].nStartL, '<' ) )
			return x_ParseError( "Element tag not found" );

		// Set parent's End tag to start looking from here (or later)
		m_aPos[iPosParent].nEndL = m_aPos[iPos].nStartL;

		// Determine whether this is an element, or bypass other type of node
		token.nNext = m_aPos[iPos].nStartL + 1;
		if ( x_FindToken( token ) )
		{
			if ( token.bIsString )
				return x_ParseError( "Tag starts with quote" );
			char cFirstChar = m_strDoc[token.nL];
			if ( cFirstChar == '?' || cFirstChar == '!' )
			{
				token.nNext = m_aPos[iPos].nStartL;
				if ( ! x_ParseNode(token) )
					return x_ParseError( "Invalid node" );
			}
			else if ( cFirstChar != '/' )
			{
				strName = x_GetToken( token );
				// Look for end of tag
				if ( ! x_FindChar(token.szDoc, token.nNext, '>') )
					return x_ParseError( "End of tag not found" );
			}
			else
				return x_ReleasePos(); // probably end tag of parent
		}
		else
			return x_ParseError( "Abrupt end within tag" );
	}
	m_aPos[iPos].nStartR = token.nNext;

	// Is ending mark within start tag, i.e. empty element?
	if ( m_strDoc[m_aPos[iPos].nStartR-1] == '/' )
	{
		// Empty element
		// Close tag left is set to ending mark, and right to open tag right
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR-1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nStartR;
	}
	else // look for end tag
	{
		// Element probably has contents
		// Determine where to start looking for left angle bracket of end tag
		// This is done by recursively parsing the contents of this element
		int iInner, iInnerPrev = 0;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR + 1;
		while ( (iInner = x_ParseElem( iPos )) > 0 )
		{
			// Set links to iInner
			if ( iInnerPrev )
				m_aPos[iInnerPrev].iElemNext = iInner;
			else
				m_aPos[iPos].iElemChild = iInner;
			iInnerPrev = iInner;

			// Set offset to reflect child
			m_aPos[iPos].nEndL = m_aPos[iInner].nEndR + 1;
		}
		if ( iInner == -1 )
			return -1;

		// Look for left angle bracket of end tag
		if ( ! x_FindChar( token.szDoc, m_aPos[iPos].nEndL, '<' ) )
			return x_ParseError( "End tag of %s element not found", strName.c_str() );

		// Look through tokens of end tag
		token.nNext = m_aPos[iPos].nEndL + 1;
		int nTokenCount = 0;
		while ( x_FindToken( token ) )
		{
			++nTokenCount;
			if ( ! token.bIsString )
			{
				// Is first token not an end slash mark?
				if ( nTokenCount == 1 && m_strDoc[token.nL] != '/' )
					return x_ParseError( "Expecting end tag of element %s", strName.c_str() );

				else if ( nTokenCount == 2 && ! token.Match(strName.c_str()) )
					return x_ParseError( "End tag does not correspond to %s", strName.c_str() );

				// Else is it a right angle bracket?
				else if ( m_strDoc[token.nL] == '>' )
					break;
			}
		}

		// Was a right angle bracket not found?
		if ( ! token.szDoc[token.nL] || nTokenCount < 2 )
			return x_ParseError( "End tag not completed for element %s", strName.c_str() );
		m_aPos[iPos].nEndR = token.nL;
	}

	// Successfully parsed element (and contained elements)
	return iPos;
}

bool CMarkupSTL::x_FindChar( const char* szDoc, int& nChar, char c )
{
	// static function
	const char* pChar = &szDoc[nChar];
	while ( *pChar && *pChar != c )
		pChar += 1;
	nChar = pChar - szDoc;
	if ( ! *pChar )
		return false;
	/*
	while ( szDoc[nChar] && szDoc[nChar] != c )
		nChar += _tclen( &szDoc[nChar] );
	if ( ! szDoc[nChar] )
		return false;
	*/
	return true;
}

bool CMarkupSTL::x_FindAny( const char* szDoc, int& nChar )
{
	// Starting at nChar, find a non-whitespace char
	// return false if no non-whitespace before end of document, nChar points to end
	// otherwise return true and nChar points to non-whitespace char
	while ( szDoc[nChar] && strchr(" \t\n\r",szDoc[nChar]) )
		++nChar;
	return szDoc[nChar] != '\0';
}

bool CMarkupSTL::x_FindToken( CMarkupSTL::TokenPos& token )
{
	// Starting at token.nNext, bypass whitespace and find the next token
	// returns true on success, members of token point to token
	// returns false on end of document, members point to end of document
	const char* szDoc = token.szDoc;
	int nChar = token.nNext;
	token.bIsString = false;

	// By-pass leading whitespace
	if ( ! x_FindAny(szDoc,nChar) )
	{
		// No token was found before end of document
		token.nL = nChar;
		token.nR = nChar;
		token.nNext = nChar;
		return false;
	}

	// Is it an opening quote?
	char cFirstChar = szDoc[nChar];
	if ( cFirstChar == '\"' || cFirstChar == '\'' )
	{
		token.bIsString = true;

		// Move past opening quote
		++nChar;
		token.nL = nChar;

		// Look for closing quote
		x_FindChar( token.szDoc, nChar, cFirstChar );

		// Set right to before closing quote
		token.nR = nChar - 1;

		// Set nChar past closing quote unless at end of document
		if ( szDoc[nChar] )
			++nChar;
	}
	else
	{
		// Go until special char or whitespace
		token.nL = nChar;
		while ( szDoc[nChar] && ! strchr(" \t\n\r<>=\\/?!",szDoc[nChar]) )
			nChar += 1;

		// Adjust end position if it is one special char
		if ( nChar == token.nL )
			++nChar; // it is a special char
		token.nR = nChar - 1;
	}

	// nNext points to one past last char of token
	token.nNext = nChar;
	return true;
}

string CMarkupSTL::x_GetToken( const CMarkupSTL::TokenPos& token ) const
{
	// The token contains indexes into the document identifying a small substring
	// Build the substring from those indexes and return it
	if ( token.nL > token.nR )
		return "";
	return m_strDoc.substr( token.nL,
		token.nR - token.nL + ((token.nR<(int)m_strDoc.size())? 1:0) );
}

int CMarkupSTL::x_FindElem( int iPosParent, int iPos, const char* szPath )
{
	// If szPath is NULL or empty, go to next sibling element
	// Otherwise go to next sibling element with matching path
	//
	if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[0] )
		return iPos;

	// Search
	TokenPos token( m_strDoc.c_str() );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStartL + 1;
		x_FindToken( token ); // Locate tag name
		if ( token.Match(szPath) )
			return iPos;
		iPos = m_aPos[iPos].iElemNext;
	}
	return 0;
}

int CMarkupSTL::x_ParseNode( CMarkupSTL::TokenPos& token )
{
	// Call this with token.nNext set to the start of the node
	// This returns the node type and token.nNext set to the char after the node
	// If the node is not found or an element, token.nR is not determined
	int nTypeFound = 0;
	const char* szDoc = token.szDoc;
	token.nL = token.nNext;
	if ( szDoc[token.nL] == '<' )
	{
		// Started with <, could be:
		// <!--...--> comment
		// <!DOCTYPE ...> dtd
		// <?target ...?> processing instruction
		// <![CDATA[...]]> cdata section
		// <NAME ...> element
		//
		if ( ! szDoc[token.nL+1] || ! szDoc[token.nL+2] )
			return 0;
		char cFirstChar = szDoc[token.nL+1];
		const char* szEndOfNode = NULL;
		if ( cFirstChar == '?' )
		{
			nTypeFound = MNT_PROCESSING_INSTRUCTION;
			szEndOfNode = "?>";
		}
		else if ( cFirstChar == '!' )
		{
			char cSecondChar = szDoc[token.nL+2];
			if ( cSecondChar == '[' )
			{
				nTypeFound = MNT_CDATA_SECTION;
				szEndOfNode = "]]>";
			}
			else if ( cSecondChar == '-' )
			{
				nTypeFound = MNT_COMMENT;
				szEndOfNode = "-->";
			}
			else
			{
				// Document type requires tokenizing because of strings and brackets
				nTypeFound = 0;
				int nBrackets = 0;
				while ( x_FindToken(token) )
				{
					if ( ! token.bIsString )
					{
						char cChar = szDoc[token.nL];
						if ( cChar == '[' )
							++nBrackets;
						else if ( cChar == ']' )
							--nBrackets;
						else if ( nBrackets == 0 && cChar == '>' )
						{
							nTypeFound = MNT_DOCUMENT_TYPE;
							break;
						}
					}
				}
				if ( ! nTypeFound )
					return 0;
			}
		}
		else if ( cFirstChar == '/' )
		{
			// End tag means no node found within parent element
			return 0;
		}
		else
		{
			nTypeFound = MNT_ELEMENT;
		}

		// Search for end of node if not found yet
		if ( szEndOfNode )
		{
			const char* pEnd = strstr( &szDoc[token.nNext], szEndOfNode );
			if ( ! pEnd )
				return 0; // not well-formed
			token.nNext = (pEnd - szDoc) + int( strlen(szEndOfNode) );
		}
	}
	else if ( szDoc[token.nL] )
	{
		// It is text or whitespace because it did not start with <
		nTypeFound = MNT_WHITESPACE;

		token.nNext = token.nL;
		if ( x_FindAny(szDoc,token.nNext) )
		{
			if ( szDoc[token.nNext] != '<' )
			{
				nTypeFound = MNT_TEXT;
				x_FindChar( szDoc, token.nNext, '<' );
			}
		}
	}
	return nTypeFound;
}

string CMarkupSTL::x_GetTagName( int iPos ) const
{
	// Return the tag name at specified element
	TokenPos token( m_strDoc.c_str() );
	token.nNext = m_aPos[iPos].nStartL + 1;
	if ( ! iPos || ! x_FindToken( token ) )
		return "";

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkupSTL::x_FindAttrib( CMarkupSTL::TokenPos& token, const char* szAttrib ) const
{
	// If szAttrib is NULL find next attrib, otherwise find named attrib
	// Return true if found
	int nAttrib = 0;
	for ( int nCount = 0; x_FindToken(token); ++nCount )
	{
		if ( ! token.bIsString )
		{
			// Is it the right angle bracket?
			char cChar = m_strDoc[token.nL];
			if ( cChar == '>' || cChar == '/' || cChar == '?' )
				break; // attrib not found

			// Equal sign
			if ( cChar == '=' )
				continue;

			// Potential attribute
			if ( ! nAttrib && nCount )
			{
				// Attribute name search?
				if ( ! szAttrib || ! szAttrib[0] )
					return true; // return with token at attrib name

				// Compare szAttrib
				if ( token.Match(szAttrib) )
					nAttrib = nCount;
			}
		}
		else if ( nAttrib && nCount == nAttrib + 2 )
		{
			return true;
		}
	}

	// Not found
	return false;
}

string CMarkupSTL::x_GetAttrib( int iPos, const char* szAttrib ) const
{
	// Return the value of the attrib
	TokenPos token( m_strDoc.c_str() );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStartL + 1;
	else
		return "";

	if ( szAttrib && x_FindAttrib( token, szAttrib ) )
		return x_TextFromDoc( token.nL, token.nR - ((token.nR<(int)m_strDoc.size())?0:1) );
	return "";
}

bool CMarkupSTL::x_SetAttrib( int iPos, const char* szAttrib, int nValue )
{
	// Convert integer to string and call SetChildAttrib
	char szVal[25];
	sprintf( szVal, "%d", nValue );
	return x_SetAttrib( iPos, szAttrib, szVal );
}

bool CMarkupSTL::x_SetAttrib( int iPos, const char* szAttrib, const char* szValue )
{
	// Set attribute in iPos element
	TokenPos token( m_strDoc.c_str() );
	int nInsertAt;
	if ( iPos && m_nNodeType == MNT_ELEMENT )
	{
		token.nNext = m_aPos[iPos].nStartL + 1;
		nInsertAt = m_aPos[iPos].nStartR - (m_aPos[iPos].IsEmptyElement()?1:0);
	}
	else
		return false;

	// Create insertion text depending on whether attribute already exists
	int nReplace = 0;
	string strInsert;
	if ( x_FindAttrib( token, szAttrib ) )
	{
		// Decision: for empty value leaving attrib="" instead of removing attrib
		// Replace value only
		strInsert = x_TextToDoc( szValue, true );
		nInsertAt = token.nL;
		nReplace = token.nR-token.nL+1;
	}
	else
	{
		// Insert string name value pair
		string strFormat;
		strFormat = " ";
		strFormat += szAttrib;
		strFormat += "=" x_ATTRIBQUOTE;
		strFormat += x_TextToDoc( szValue, true );
		strFormat += x_ATTRIBQUOTE;
		strInsert = strFormat;
	}

	x_DocChange( nInsertAt, nReplace, strInsert );
	int nAdjust = int(strInsert.size()) - nReplace;
	m_aPos[iPos].nStartR += nAdjust;
	m_aPos[iPos].AdjustEnd( nAdjust );
	x_Adjust( iPos, nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}


bool CMarkupSTL::x_CreateNode( string& strNode, int nNodeType, const char* szText )
{
	// Set strNode based on nNodeType and szText
	// Return false if szText would jeopardize well-formed document
	//
	switch ( nNodeType )
	{
	case MNT_CDATA_SECTION:
		if ( strstr(szText,"]]>") != NULL )
			return false;
		strNode = "<![CDATA[";
		strNode += szText;
		strNode += "]]>";
		break;
	}
	return true;
}

bool CMarkupSTL::x_SetData( int iPos, const char* szData, int nCDATA )
{
	// Set data at specified position
	// if nCDATA==1, set content of element to a CDATA Section
	string strInsert;


	// Set data in iPos element
	if ( ! iPos || m_aPos[iPos].iElemChild )
		return false;

	// Build strInsert from szData based on nCDATA
	// If CDATA section not valid, use parsed text (PCDATA) instead
	if ( nCDATA != 0 )
		if ( ! x_CreateNode(strInsert, MNT_CDATA_SECTION, szData) )
			nCDATA = 0;
	if ( nCDATA == 0 )
		strInsert = x_TextToDoc( szData );

	// Decide where to insert
	int nInsertAt, nReplace;
	if ( m_aPos[iPos].IsEmptyElement() )
	{
		nInsertAt = m_aPos[iPos].nEndL;
		nReplace = 1;

		// Pre-adjust since <NAME/> becomes <NAME>data</NAME>
		string strTagName = x_GetTagName( iPos );
		m_aPos[iPos].nStartR -= 1;
		m_aPos[iPos].nEndL -= (1 + int(strTagName.size()));
		string strFormat;
		strFormat = ">";
		strFormat += strInsert;
		strFormat += "</";
		strFormat += strTagName;
		strInsert = strFormat;
	}
	else
	{
		nInsertAt = m_aPos[iPos].nStartR+1;
		nReplace = m_aPos[iPos].nEndL - m_aPos[iPos].nStartR - 1;
	}
	x_DocChange( nInsertAt, nReplace, strInsert );
	int nAdjust = int(strInsert.size()) - nReplace;
	x_Adjust( iPos, nAdjust );
	m_aPos[iPos].AdjustEnd( nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}

string CMarkupSTL::x_GetData( int iPos ) const
{

	// Return a string representing data between start and end tag
	// Return empty string if there are any children elements
	if ( ! m_aPos[iPos].iElemChild && ! m_aPos[iPos].IsEmptyElement() )
	{
		// See if it is a CDATA section
		const char* szDoc = m_strDoc.c_str();
		int nChar = m_aPos[iPos].nStartR + 1;
		if ( x_FindAny( szDoc, nChar ) && szDoc[nChar] == '<'
				&& nChar + 11 < m_aPos[iPos].nEndL
				&& strncmp( &szDoc[nChar], "<![CDATA[", 9 ) == 0 )
		{
			nChar += 9;
			int nEndCDATA = int( m_strDoc.find( "]]>", nChar ) );
                        if ( (unsigned int)nEndCDATA != string::npos && nEndCDATA < m_aPos[iPos].nEndL )
			{
				return m_strDoc.substr( nChar, nEndCDATA - nChar );
			}
		}
		return x_TextFromDoc( m_aPos[iPos].nStartR+1, m_aPos[iPos].nEndL-1 );
	}
	return "";
}

string CMarkupSTL::x_TextToDoc( const char* szText, bool bAttrib ) const
{
	//
	// &lt;   less than
	// &amp;  ampersand
	// &gt;   greater than
	//
	// and for attributes:
	//
	// &apos; apostrophe or single quote
	// &quot; double quote
	//
        static const char* szaReplace[] = { "&lt;","&amp;","&gt;","&apos;","&quot;" };
	const char* pFind = bAttrib?"<&>\'\"":"<&>";
	const char* pSource = szText;
	string strResult;
	int nLen = int( strlen( szText ) );
	strResult.reserve( nLen + nLen / 10 );
	char cSource = *pSource;
	char* pFound;
	while ( cSource )
	{
		if ( (pFound=const_cast<char*>(strchr(pFind,cSource))) != NULL )
		{
                        pFound = const_cast<char*>(szaReplace[pFound-pFind]);
			strResult.append(pFound);
		}
		else
		{
			strResult += cSource;
		}
		cSource = *(++pSource);
	}
	return strResult;
}

string CMarkupSTL::x_TextFromDoc( int nLeft, int nRight ) const
{
	// Convert XML friendly text to text as seen outside XML document
	// ampersand escape codes replaced with special characters e.g. convert "6&gt;7" to "6>7"
	// ampersand numeric codes replaced with character e.g. convert &#60; to <
	// Conveniently the result is always the same or shorter in byte length
	//
        static const char* szaCode[] = { "lt;","amp;","gt;","apos;","quot;" };
	static int anCodeLen[] = { 3,4,3,5,5 };
        static const char* szSymbol = "<&>\'\"";
	string strResult;
	strResult.reserve( nRight - nLeft + 1 );
	const char* pSource = m_strDoc.c_str();
	int nChar = nLeft;
	char cSource = pSource[nChar];
	while ( nChar <= nRight )
	{
		if ( cSource == '&' )
		{
			bool bCodeConverted = false;

			// Is it a numeric character reference?
			cSource = pSource[nChar+1];
			if ( cSource == '#' )
			{
				// Is it a hex number?
				int nBase = 10;
				int nNumericChar = nChar + 2;
				cSource = pSource[nNumericChar];
				if ( cSource == 'x' )
				{
					++nNumericChar;
					cSource = pSource[nNumericChar];
					nBase = 16;
				}

				// Look for terminating semi-colon within 7 characters
				int nCodeLen = 0;
				while ( nCodeLen < 7 && cSource && cSource != ';' )
				{
					// only ASCII digits 0-9, A-F, a-f expected
					++nCodeLen;
					cSource = pSource[nNumericChar + nCodeLen];
				}

				// Process unicode
				if ( cSource == ';' )
				{
					int nUnicode = strtol( &pSource[nNumericChar], NULL, nBase );
					if ( nUnicode < 0x80 )
						strResult += (char)nUnicode;
					else if ( nUnicode < 0x800 )
					{
						// Convert to 2-byte UTF-8
						strResult += (char)(((nUnicode&0x7c0)>>6) | 0xc0);
						strResult += (char)((nUnicode&0x3f) | 0x80);
					}
					else
					{
						// Convert to 3-byte UTF-8
						strResult += (char)(((nUnicode&0xf000)>>12) | 0xe0);
						strResult += (char)(((nUnicode&0xfc0)>>6) | 0x80);
						strResult += (char)((nUnicode&0x3f) | 0x80);
					}
					// Increment index past ampersand semi-colon
					nChar = nNumericChar + nCodeLen + 1;
					bCodeConverted = true;
				}
			}
			else // does not start with #
			{
				// Look for matching &code;
				for ( int nMatch = 0; nMatch < 5; ++nMatch )
				{
					if ( nChar <= nRight - anCodeLen[nMatch]
						&& strncmp(szaCode[nMatch],&pSource[nChar+1],anCodeLen[nMatch]) == 0 )
					{
						strResult += szSymbol[nMatch];
						nChar += anCodeLen[nMatch] + 1;
						bCodeConverted = true;
						break;
					}
				}
			}

			// If the code is not converted, leave it as is
			if ( ! bCodeConverted )
			{
				strResult += '&';
				++nChar;
			}
		}
		else // not &
		{
			strResult += cSource;
			nChar++;
		}
		cSource = pSource[nChar];
	}
	return strResult;
}

void CMarkupSTL::x_DocChange( int nLeft, int nReplace, const string& strInsert )
{
	// Insert strInsert int m_strDoc at nLeft replacing nReplace chars
	//
	m_strDoc.replace( nLeft, nReplace, strInsert);
}

void CMarkupSTL::x_Adjust( int iPos, int nShift, bool bAfterPos )
{
	// Loop through affected elements and adjust indexes
	// Algorithm:
	// 1. update children unless bAfterPos
	//    (if no children or bAfterPos is true, end tag of iPos not affected)
	// 2. update next siblings and their children
	// 3. go up until there is a next sibling of a parent and update end tags
	// 4. step 2
	int iPosTop = m_aPos[iPos].iElemParent;
	bool bPosFirst = bAfterPos; // mark as first to skip its children
	while ( iPos )
	{
		// Were we at containing parent of affected position?
		bool bPosTop = false;
		if ( iPos == iPosTop )
		{
			// Move iPosTop up one towards root
			iPosTop = m_aPos[iPos].iElemParent;
			bPosTop = true;
		}

		// Traverse to the next update position
		if ( ! bPosTop && ! bPosFirst && m_aPos[iPos].iElemChild )
		{
			// Depth first
			iPos = m_aPos[iPos].iElemChild;
		}
		else if ( m_aPos[iPos].iElemNext )
		{
			iPos = m_aPos[iPos].iElemNext;
		}
		else
		{
			// Look for next sibling of a parent of iPos
			// When going back up, parents have already been done except iPosTop
			while ( (iPos=m_aPos[iPos].iElemParent) != 0 && iPos != iPosTop )
				if ( m_aPos[iPos].iElemNext )
				{
					iPos = m_aPos[iPos].iElemNext;
					break;
				}
		}
		bPosFirst = false;

		// Shift indexes at iPos
		if ( iPos != iPosTop )
			m_aPos[iPos].AdjustStart( nShift );
		m_aPos[iPos].AdjustEnd( nShift );
	}
}

void CMarkupSTL::x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags )
{
	// Determine where to insert new element or node
	//
	bool bInsert = (nFlags&1)?true:false;
	bool bHonorWhitespace = (nFlags&2)?true:false;

	int nStartL;
	if ( nLength )
	{
		// Located at a non-element node
		if ( bInsert )
			nStartL = nOffset;
		else
			nStartL = nOffset + nLength;
	}
	else if ( iPosRel )
	{
		// Located at an element
		if ( bInsert ) // precede iPosRel
			nStartL = m_aPos[iPosRel].nStartL;
		else // follow iPosRel
			nStartL = m_aPos[iPosRel].nEndR + 1;
	}
	else if ( ! iPosParent )
	{
		// Outside of all elements
		if ( bInsert )
			nStartL = 0;
		else
			nStartL = int( m_strDoc.size() );
	}
	else if ( m_aPos[iPosParent].IsEmptyElement() )
	{
		// Parent has no separate end tag, so split empty element
		nStartL = m_aPos[iPosParent].nStartR;
	}
	else
	{
		if ( bInsert ) // after start tag
			nStartL = m_aPos[iPosParent].nStartR + 1;
		else // before end tag
			nStartL = m_aPos[iPosParent].nEndL;
	}

	// Go up to start of next node, unless its splitting an empty element
	if ( ! bHonorWhitespace && ! m_aPos[iPosParent].IsEmptyElement() )
	{
		const char* szDoc = m_strDoc.c_str();
		int nChar = nStartL;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == '<' )
			nStartL = nChar;
	}

	// Determine iPosBefore
	int iPosBefore = 0;
	if ( iPosRel )
	{
		if ( bInsert )
		{
			// Is iPosRel past first sibling?
			int iPosPrev = m_aPos[iPosParent].iElemChild;
			if ( iPosPrev != iPosRel )
			{
				// Find previous sibling of iPosRel
				while ( m_aPos[iPosPrev].iElemNext != iPosRel )
					iPosPrev = m_aPos[iPosPrev].iElemNext;
				iPosBefore = iPosPrev;
			}
		}
		else
		{
			iPosBefore = iPosRel;
		}
	}
	else if ( m_aPos[iPosParent].iElemChild )
	{
		if ( ! bInsert )
		{
			// Find last element under iPosParent
			int iPosLast = m_aPos[iPosParent].iElemChild;
			int iPosNext = iPosLast;
			while ( iPosNext )
			{
				iPosLast = iPosNext;
				iPosNext = m_aPos[iPosNext].iElemNext;
			}
			iPosBefore = iPosLast;
		}
	}

	nOffset = nStartL;
	iPosRel = iPosBefore;
}

bool CMarkupSTL::x_AddElem( const char* szName, const char* szValue, bool bInsert, bool bAddChild )
{
	if ( bAddChild )
	{
		// Adding a child element under main position
		if ( ! m_iPos )
			return false;
	}
	else if ( m_iPosParent == 0 )
	{
		// Adding root element
		if ( IsWellFormed() )
			return false;


		// Locate after any version and DTD
		m_aPos[0].nEndL = int( m_strDoc.size() );
	}

	// Locate where to add element relative to current node
	int iPosParent, iPosBefore, nOffset = 0, nLength = 0;
	if ( bAddChild )
	{
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
	}
	int nFlags = bInsert?1:0;
	x_LocateNew( iPosParent, iPosBefore, nOffset, nLength, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	if ( bEmptyParent || m_aPos[iPosParent].nStartR + 1 == m_aPos[iPosParent].nEndL )
		nOffset += x_EOLLEN;

	// Create element and modify positions of affected elements
	// If no szValue is specified, an empty element is created
	// i.e. either <NAME>value</NAME> or <NAME/>
	//
	int iPos = x_GetFreePos();
	m_aPos[iPos].nStartL = nOffset;

	// Set links
	m_aPos[iPos].iElemParent = iPosParent;
	m_aPos[iPos].iElemChild = 0;
	m_aPos[iPos].iElemNext = 0;
	if ( iPosBefore )
	{
		// Link in after iPosBefore
		m_aPos[iPos].iElemNext = m_aPos[iPosBefore].iElemNext;
		m_aPos[iPosBefore].iElemNext = iPos;
	}
	else
	{
		// First child
		m_aPos[iPos].iElemNext = m_aPos[iPosParent].iElemChild;
		m_aPos[iPosParent].iElemChild = iPos;
	}

	// Create string for insert
	string strInsert;
	int nLenName = int( strlen(szName) );
	int nLenValue = szValue? int( strlen(szValue) ) : 0;
	if ( ! nLenValue )
	{
		// <NAME/> empty element
		strInsert = "<";
		strInsert += szName;
		strInsert += "/>" x_EOL;
		m_aPos[iPos].nStartR = m_aPos[iPos].nStartL + nLenName + 2;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR - 1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nEndL + 1;
	}
	else
	{
		// <NAME>value</NAME>
		string strValue = x_TextToDoc( szValue );
		nLenValue = int( strValue.size() );
		strInsert = "<";
		strInsert += szName;
		strInsert += ">";
		strInsert += strValue;
		strInsert += "</";
		strInsert += szName;
		strInsert += ">" x_EOL;
		m_aPos[iPos].nStartR = m_aPos[iPos].nStartL + nLenName + 1;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR + nLenValue + 1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nEndL + nLenName + 2;
	}

	// Insert
	int nReplace = 0, nLeft = m_aPos[iPos].nStartL;
	if ( bEmptyParent )
	{
		string strParentTagName = x_GetTagName(iPosParent);
		string strFormat;
		strFormat = ">" x_EOL;
		strFormat += strInsert;
		strFormat += "</";
		strFormat += strParentTagName;
		strInsert = strFormat;
		nLeft = m_aPos[iPosParent].nStartR - 1;
		nReplace = 1;
		// x_Adjust is going to update all affected indexes by one amount
		// This will satisfy all except the empty parent
		// Here we pre-adjust for the empty parent
		// The empty tag slash is removed
		m_aPos[iPosParent].nStartR -= 1;
		// For the newly created end tag, see the following example:
		// <A/> (len 4) becomes <A><B/></A> (len 11)
		// In x_Adjust everything will be adjusted 11 - 4 = 7
		// But the nEndL of element A should only be adjusted 5
		m_aPos[iPosParent].nEndL -= (int(strParentTagName.size()) + 1);
	}
	else if ( m_aPos[iPosParent].nStartR + 1 == m_aPos[iPosParent].nEndL )
	{
		strInsert = x_EOL + strInsert;
		nLeft = m_aPos[iPosParent].nStartR + 1;
	}
	x_DocChange( nLeft, nReplace, strInsert );
	x_Adjust( iPos, int(strInsert.size()) - nReplace );

	if ( bAddChild )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else
		x_SetPos( iPosParent, iPos, 0 );
	return true;
}

string CMarkupSTL::x_GetSubDoc( int iPos ) const
{
	if ( iPos )
	{
		int nL = m_aPos[iPos].nStartL;
		int nR = m_aPos[iPos].nEndR + 1;
		const char* szDoc = m_strDoc.c_str();
		int nChar = nR;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == '<' )
			nR = nChar;
		return m_strDoc.substr( nL, nR - nL );
	}
	return "";
}

bool CMarkupSTL::x_AddSubDoc( const char* szSubDoc, bool bInsert, bool bAddChild )
{
	// Add subdocument, parse, and modify positions of affected elements
	//
	int nOffset = 0, iPosParent, iPosBefore;
	if ( bAddChild )
	{
		// Add a subdocument under main position, before or after child
		if ( ! m_iPos )
			return false;
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		// Add a subdocument under parent position, before or after main
		if ( ! m_iPosParent )
			return false;
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
	}
	int nFlags = bInsert?1:0;
	x_LocateNew( iPosParent, iPosBefore, nOffset, 0, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	if ( bEmptyParent )
		nOffset += x_EOLLEN;

	// if iPosBefore is NULL, insert as first element under parent
	int nParentEndLBeforeAdd = m_aPos[iPosParent].nEndL;
	int iPosFreeBeforeAdd = m_iPosFree;

	// Skip version tag or DTD at start of subdocument
	TokenPos token( szSubDoc );
	int nNodeType = x_ParseNode( token );
	while ( nNodeType && nNodeType != MNT_ELEMENT )
	{
		token.szDoc = &token.szDoc[token.nNext];
		token.nNext = 0;
		nNodeType = x_ParseNode( token );
	}
	string strInsert = token.szDoc;

	// Insert subdocument
	m_aPos[iPosParent].nEndL = nOffset;
	int nReplace = 0, nLeft = nOffset;
	string strParentTagName;
	if ( bEmptyParent )
	{
		strParentTagName = x_GetTagName(iPosParent);
		string strFormat;
		strFormat = ">" x_EOL;
		strFormat += strInsert;
		strFormat += "</";
		strFormat += strParentTagName;
		strInsert = strFormat;
		nLeft = m_aPos[iPosParent].nStartR - 1;
		nReplace = 1;
		m_aPos[iPosParent].nEndL = m_aPos[iPosParent].nStartR + x_EOLLEN;
	}
	x_DocChange( nLeft, nReplace, strInsert );

	// Parse subdocument
	int iPos = x_ParseElem(iPosParent);
	m_aPos[iPosParent].nEndL = nParentEndLBeforeAdd;
	if ( iPos <= 0 )
	{
		// Abort because not well-formed
		string strRevert = bEmptyParent?"/":"";
		x_DocChange( nLeft, int(strInsert.size()), strRevert );
		m_iPosFree = iPosFreeBeforeAdd;
		return false;
	}
	else
	{
		// Link in parent and siblings
		m_aPos[iPos].iElemParent = iPosParent;
		if ( iPosBefore )
		{
			m_aPos[iPos].iElemNext = m_aPos[iPosBefore].iElemNext;
			m_aPos[iPosBefore].iElemNext = iPos;
		}
		else
		{
			m_aPos[iPos].iElemNext = m_aPos[iPosParent].iElemChild;
			m_aPos[iPosParent].iElemChild = iPos;
		}

		// Make empty parent pre-adjustment
		if ( bEmptyParent )
		{
			m_aPos[iPosParent].nStartR -= 1;
			m_aPos[iPosParent].nEndL -= (int(strParentTagName.size()) + 1);
		}

		// Adjust, but don't adjust children of iPos (bAfterPos=true)
		x_Adjust( iPos, int(strInsert.size()) - nReplace, true );
	}

	// Set position to top element of subdocument
	if ( bAddChild )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else // Main
		x_SetPos( m_iPosParent, iPos, 0 );
	return true;
}

int CMarkupSTL::x_RemoveElem( int iPos )
{
	// Remove element and all contained elements
	// Return new position
	//
	int iPosParent = m_aPos[iPos].iElemParent;

	// Find previous sibling and bypass removed element
	// This leaves orphan positions in m_aPos array
	int iPosLook = m_aPos[iPosParent].iElemChild;
	int iPosPrev = 0;
	while ( iPosLook != iPos )
	{
		iPosPrev = iPosLook;
		iPosLook = m_aPos[iPosLook].iElemNext;
	}
	if ( iPosPrev )
		m_aPos[iPosPrev].iElemNext = m_aPos[iPos].iElemNext;
	else
		m_aPos[iPosParent].iElemChild = m_aPos[iPos].iElemNext;

	// Remove from document
	// Links have been changed to go around removed element
	// But element position and links are still valid
	int nAfterEnd = m_aPos[iPos].nEndR + 1;
	const char* szDoc = m_strDoc.c_str();
	int nChar = nAfterEnd;
	if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == '<' )
		nAfterEnd = nChar;
	int nLen = nAfterEnd - m_aPos[iPos].nStartL;
	x_DocChange( m_aPos[iPos].nStartL, nLen, string() );
	x_Adjust( iPos, - nLen, true );
	return iPosPrev;
}


