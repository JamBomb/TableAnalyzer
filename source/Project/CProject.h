//*****************************************************//
//                                                     //
//      CProject                                       //
//                                                     //
//      ∞Ê»®À˘”– (C) 2016 Coriant                      //
//                                                     //
//*****************************************************//
#ifndef CProjectH
#define CProjectH
//---------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <string>
#include "..\\MarkupSTL\\MarkupSTL.h"

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned int
#endif

//---------------------------------------------------------------------------
using namespace std;


struct TPrjNode
{
    TPrjNode()
    {
        Name = "";
        Value = "";
        Level = 0;
    }
    string Name;
    string Value;
    DWORD  Level;
};

/*
project file format:
    <Project name="ProjectName">ProjectValue
        <Module  name="ModuleName">ModuleValue
            <Item name="ItemName">ItemValue</Item>
        </Module>
    </Project>
*/

class CProject
{
public:     // User declarations
    CProject()
    {
        m_FileName = "";
        m_ProjectCount = 0;
    }

    virtual  ~CProject()
    {

    }
    bool  Load(const char* FileName);
    bool  Save();
    bool  SaveAs(const char* FileName);

    vector<TPrjNode> NodeList;

    string GetPrjName(DWORD PrjIdx);

    bool Add(const char* ParentName, const char* Name, const char* Value=NULL);
    bool Delete(const char* ParentName, const char* Name);
    bool SetValue(const char* ParentName, const char* Name, const char* Value=NULL);
    bool IsExisted(const char* ParentName, const char* Name);



private:    // User declarations
    string m_FileName;
    DWORD m_ProjectCount;
    CMarkupSTL XmlPraser;
    bool UpdateProjectNode();
};


#endif
