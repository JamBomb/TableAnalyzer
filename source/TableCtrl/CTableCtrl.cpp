#include "CTableCtrl.h"

using namespace CosmoArt_FieldPower;
using namespace std;


CTableCtrl::CTableCtrl()
{

}

CTableCtrl::~CTableCtrl()
{

}


bool CTableCtrl::ParseTable(const string& filename)
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
        if(!strTemp.empty())
        {
            iUnit = atoi(strTemp.c_str());
        }
        else
        {
            iUnit = 1;
        }

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

        m_FieldTable.AddNodeSet(FieldNodeSet);

    }

    return true;

}


bool CTableCtrl::LoadTable(const string& filename)
{
    return ParseTable(filename);

}

const string CTableCtrl::GetTableName()
{
    return m_FieldTable.GetName();
}

int CTableCtrl::GetNumofNodes()
{
    return m_FieldTable.GetNumofNodes();
}


vector <CFieldNode> CTableCtrl::GetTableNodes()
{
    return m_FieldTable.GetNodes();
}


int CTableCtrl::GetTableLength()
{
    return m_FieldTable.GetTableLength();
}

bool CTableCtrl::SetData(vector <DWORD>& Data)
{
    bool bRtn = true;

    m_FieldTable.SetData(Data);

    return bRtn;
}

bool CTableCtrl::GetData(vector <DWORD>& Data)
{
    bool bRtn = true;
    Data.clear();
    m_FieldTable.GetData(Data);
    return bRtn;
}

bool CTableCtrl::GetReversedData(vector <DWORD>& Data)
{
    bool bRtn = true;
    Data.clear();
    m_FieldTable.GetReversedData(Data);
    return bRtn;
}


void CTableCtrl::SetNodesData(vector <DWORD> NodesDataList)
{
    m_FieldTable.SetNodesData(NodesDataList);
}

vector <DWORD> CTableCtrl::GetNodesData()
{
    return m_FieldTable.GetNodesData();
}






