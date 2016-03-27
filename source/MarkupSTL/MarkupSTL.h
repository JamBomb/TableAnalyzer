// Markup.h: interface for the CMarkupSTL class.
//
// Markup Release 6.5
// Copyright (C) 1999-2003 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#if !defined(AFX_MARKUPSTL_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
#define AFX_MARKUPSTL_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_

//#pragma warning(disable:4786)

#include <string>
#include <map>
#include <vector>
#include <string.h>

#ifdef _DEBUG
#define _DS(i) (i?&(m_strDoc.c_str())[m_aPos[i].nStartL]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else
#define MARKUP_SETDEBUGSTATE
#endif

class CMarkupSTL
{
public:
        CMarkupSTL();
        CMarkupSTL(const char* szDoc);
        CMarkupSTL(const CMarkupSTL& markup);
	void operator=( const CMarkupSTL& markup );
        virtual ~CMarkupSTL();

	// Navigate
	bool Load( const char* szFileName );
	bool SetDoc( const char* szDoc );
	bool IsWellFormed();
	bool FindElem( const char* szName=NULL );
	bool FindChildElem( const char* szName=NULL );
	bool IntoElem();
	bool OutOfElem();
        void ResetChildPos();
        void ResetMainPos();
        void ResetPos();
	std::string GetTagName() const;
        std::string GetChildTagName() const ;
        std::string GetData() const ;
        std::string GetChildData() const ;
        std::string GetAttrib( const char* szAttrib ) const;
        std::string GetChildAttrib( const char* szAttrib ) const ;
	std::string GetAttribName( int n ) const;
	bool SavePos( const char* szPosName="" );
	bool RestorePos( const char* szPosName="" );
        std::string GetError() const;

	enum MarkupNodeType
	{
		MNT_ELEMENT					= 1,  // 0x01
		MNT_TEXT					= 2,  // 0x02
		MNT_WHITESPACE				= 4,  // 0x04
		MNT_CDATA_SECTION			= 8,  // 0x08
		MNT_PROCESSING_INSTRUCTION	= 16, // 0x10
		MNT_COMMENT					= 32, // 0x20
		MNT_DOCUMENT_TYPE			= 64, // 0x40
		MNT_EXCLUDE_WHITESPACE		= 123,// 0x7b
	};

	// Create
	bool Save( const char* szFileName );
        std::string GetDoc() const;
        bool AddElem( const char* szName, const char* szData=NULL );
        bool InsertElem( const char* szName, const char* szData=NULL ) ;
        bool AddChildElem( const char* szName, const char* szData=NULL );
        bool InsertChildElem( const char* szName, const char* szData=NULL ) ;
        bool AddAttrib( const char* szAttrib, const char* szValue ) ;
        bool AddChildAttrib( const char* szAttrib, const char* szValue );
        bool AddAttrib( const char* szAttrib, int nValue ) ;
        bool AddChildAttrib( const char* szAttrib, int nValue );
        bool AddSubDoc( const char* szSubDoc ) ;
        bool InsertSubDoc( const char* szSubDoc );
        std::string GetSubDoc() const ;
        bool AddChildSubDoc( const char* szSubDoc ) ;
        bool InsertChildSubDoc( const char* szSubDoc ) ;
        std::string GetChildSubDoc() const ;

	// Modify
	bool RemoveElem();
	bool RemoveChildElem();
        bool SetAttrib( const char* szAttrib, const char* szValue ) ;
        bool SetChildAttrib( const char* szAttrib, const char* szValue ) ;
        bool SetAttrib( const char* szAttrib, int nValue );
        bool SetChildAttrib( const char* szAttrib, int nValue ) ;
        bool SetData( const char* szData, int nCDATA=0 );
        bool SetChildData( const char* szData, int nCDATA=0 );


protected:

#ifdef _DEBUG
	const char* m_pMainDS;
	const char* m_pChildDS;
#endif

	std::string m_strDoc;
	std::string m_strError;

	struct ElemPos
	{
		ElemPos() { Clear(); };
		ElemPos( const ElemPos& pos ) { *this = pos; };
		bool IsEmptyElement() const { return (nStartR == nEndL + 1); };
		void Clear()
		{
			nStartL=0; nStartR=0; nEndL=0; nEndR=0; nReserved=0;
			iElemParent=0; iElemChild=0; iElemNext=0;
		};
		void AdjustStart( int n ) { nStartL+=n; nStartR+=n; };
		void AdjustEnd( int n ) { nEndL+=n; nEndR+=n; };
		int nStartL;
		int nStartR;
		int nEndL;
		int nEndR;
		int nReserved;
		int iElemParent;
		int iElemChild;
		int iElemNext;
	};

	typedef std::vector<ElemPos> vectorElemPosT;
	vectorElemPosT m_aPos;
	int m_iPosParent;
	int m_iPos;
	int m_iPosChild;
	int m_iPosFree;
	int m_nNodeType;

	struct TokenPos
	{
		TokenPos( const char* sz ) { Clear(); szDoc = sz; };
		void Clear() { nL=0; nR=-1; nNext=0; bIsString=false; };
		bool Match( const char* szName )
		{
			int nLen = nR - nL + 1;
		// To ignore case, define MARKUP_IGNORECASE
		#ifdef MARKUP_IGNORECASE
			return ( (strnicmp( &szDoc[nL], szName, nLen ) == 0)
		#else
			return ( (strncmp( &szDoc[nL], szName, nLen ) == 0)
		#endif
				&& ( szName[nLen] == '\0' || strchr(" =/[",szName[nLen]) ) );
		};
		int nL;
		int nR;
		int nNext;
		const char* szDoc;
		bool bIsString;
	};

	struct SavedPos
	{
		int iPosParent;
		int iPos;
		int iPosChild;
	};
	typedef std::map<std::string,SavedPos> mapSavedPosT;
	mapSavedPosT m_mapSavedPos;

	void x_SetPos( int iPosParent, int iPos, int iPosChild )
	{
		m_iPosParent = iPosParent;
		m_iPos = iPos;
		m_iPosChild = iPosChild;
		m_nNodeType = iPos?MNT_ELEMENT:0;
		MARKUP_SETDEBUGSTATE;
	};

	int x_GetFreePos();
	int x_ReleasePos();

	int x_ParseElem( int iPos );
	int x_ParseError( const char* szError, const char* szName = NULL );
	static bool x_FindChar( const char* szDoc, int& nChar, char c );
	static bool x_FindAny( const char* szDoc, int& nChar );
	static bool x_FindToken( TokenPos& token );
	std::string x_GetToken( const TokenPos& token ) const;
	int x_FindElem( int iPosParent, int iPos, const char* szPath );
	std::string x_GetTagName( int iPos ) const;
	std::string x_GetData( int iPos ) const;
	std::string x_GetAttrib( int iPos, const char* szAttrib ) const;
	bool x_AddElem( const char* szName, const char* szValue, bool bInsert, bool bAddChild );
	std::string x_GetSubDoc( int iPos ) const;
	bool x_AddSubDoc( const char* szSubDoc, bool bInsert, bool bAddChild );
	bool x_FindAttrib( TokenPos& token, const char* szAttrib=NULL ) const;
	bool x_SetAttrib( int iPos, const char* szAttrib, const char* szValue );
	bool x_SetAttrib( int iPos, const char* szAttrib, int nValue );
	bool x_CreateNode( std::string& strNode, int nNodeType, const char* szText );
	void x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags );
	int x_ParseNode( TokenPos& token );
	bool x_SetData( int iPos, const char* szData, int nCDATA );
	int x_RemoveElem( int iPos );
	void x_DocChange( int nLeft, int nReplace, const std::string& strInsert );
	void x_PosInsert( int iPos, int nInsertLength );
	void x_Adjust( int iPos, int nShift, bool bAfterPos = false );
	std::string x_TextToDoc( const char* szText, bool bAttrib = false ) const;
	std::string x_TextFromDoc( int nLeft, int nRight ) const;
};

#endif // !defined(AFX_MARKUPSTL_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
