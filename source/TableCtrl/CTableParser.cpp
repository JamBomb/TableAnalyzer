#include "CTableParser.h"

using namespace CosmoArt_FieldPower;
using namespace std;


CTableParser::CTableParser()
{

}


CTableParser::~CTableParser()
{

}


bool CTableParser::ParseTableFile(const string& filename)
{
    m_FieldTable.Clear();

    //  sink check
    if(filename.empty() ||
       !m_XmlParser.Load(filename.c_str()) ||
       !m_XmlParser.FindElem("table"))
    {
        return false;
    }

    m_FieldTable.SetName(m_XmlParser.GetAttrib("name"));

    m_XmlParser.IntoElem();

    // get field nodes information
    while(m_XmlParser.FindElem("field"))
    {
        int  iWidth = 0;
        int  iUnit = 0;
        DWORD dwNodeWidth = 0;
        CFieldNode FieldNode;
        CFieldNodeSet FieldNodeSet;
        string strTemp;


        //FieldNode.Text = m_XmlParser.GetData();
        strTemp = m_XmlParser.GetAttrib("width");
        iWidth = atoi(strTemp.c_str());
        strTemp =  m_XmlParser.GetAttrib("unit");
        iUnit = atoi(strTemp.c_str());
        dwNodeWidth = (iWidth / iUnit);

        if(dwNodeWidth > FIELD_NODE_MAX_BIT_WIDTH ||
           0 != (iWidth % iUnit))
        {
            return false;
        }

        strTemp = m_XmlParser.GetAttrib("name");
        FieldNodeSet.SetName(strTemp);

        for(int i = 0; i < iUnit; i++)
        {
            FieldNode.SetName(strTemp);
            FieldNode.SetWidth(dwNodeWidth);
            FieldNodeSet.AddNode(FieldNode);
        }

        m_FieldTable.AddFieldNodeSet(FieldNodeSet);

    }

    return true;

}


bool CTableParser::LoadTableFile(const string& filename)
{
    return ParseTableFile(filename);

}

const string CTableParser::GetTableName()
{
    return m_FieldTable.GetName();
}

int CTableParser::GetNumofTableNodes()
{
    vector <CFieldNodeSet> FieldNodeSet = GetTableNodeSets();

    DWORD NodeNumber = 0;

    for (vector <CFieldNodeSet>::iterator itr = FieldNodeSet.begin();
         itr != FieldNodeSet.end(); ++itr)
    {
        NodeNumber += itr->GetNodeNumber();
    }

    return NodeNumber;
}


vector <CFieldNodeSet> CTableParser::GetTableNodeSets()
{
    return m_FieldTable.GetFieldNodeSets();
}

int CTableParser::GetTableLength()
{
    return m_FieldTable.GetLength();
}

bool CTableParser::ParseFieldData(vector <DWORD> vecFieldData)
{
    bool bRtn = true;
    vector <CFieldNode> FieldNodes;
    FieldNodes = m_FieldTable.GetFieldNodes();
    m_FieldAnalyser.SetFieldNodes(FieldNodes);
    m_FieldAnalyser.SetFieldData(vecFieldData);
    m_FieldAnalyser.ParseFieldData();
    m_FieldAnalyser.GetFieldNodes(FieldNodes);
    bRtn = m_FieldTable.SetFieldNodes(FieldNodes);
    return bRtn;
}

bool CTableParser::BuildFieldData(vector < DWORD > & vecFieldData)
{
    bool bRtn = true;
    vecFieldData.clear();
    vector <CFieldNode> FieldNodes;
    FieldNodes = m_FieldTable.GetFieldNodes();
    m_FieldAnalyser.SetFieldNodes(FieldNodes);
    m_FieldAnalyser.BuildFieldData();
    m_FieldAnalyser.GetFieldData(vecFieldData);
    return bRtn;
}





