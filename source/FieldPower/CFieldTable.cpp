#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <math.h>
#include "CFieldTable.h"


using namespace CosmoArt_FieldPower;

CFieldTable::CFieldTable()
{
    Name = "FieldTable";
}

CFieldTable::~CFieldTable()
{
    Clear();
}

inline void CFieldTable::CalNodesPosition()
{
    DWORD dwPos = 0;

    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        itr->SetPosition(dwPos);
        dwPos += itr->GetWidth();
    }
}


void CFieldTable::SetName(const string &TableName)
{
    if(TableName.empty())
    {
        return;
    }

    Name = TableName;
}

string CFieldTable::GetName()
{
    return Name;
}

void CFieldTable::AddNodeSet(const CFieldNodeSet FieldNodeSet)
{
    FieldNodeSets.push_back(FieldNodeSet);

    vector <CFieldNode> FieldNodeSetNodes = FieldNodeSet.GetFieldNodes();

    for (DWORD i = 0; i < FieldNodeSetNodes.size(); ++i)
    {
        FieldNodes.push_back(FieldNodeSetNodes[i]);
    }

    CalNodesPosition();
}

void CFieldTable::AddNodeSets(vector <CFieldNodeSet>  vctFieldNodeSets)
{
    for (vector <CFieldNodeSet>::iterator itr = vctFieldNodeSets.begin();
         itr != vctFieldNodeSets.end(); ++itr)
    {
        AddNodeSet(*itr);
    }
}

int CFieldTable::GetNumofNodes()
{
    return  FieldNodes.size();
}


void CFieldTable::DelNodeSet(DWORD dwIndex)
{
    if(dwIndex >=  FieldNodeSets.size())
    {
        return;
    }
    else
    {
        FieldNodeSets.erase(FieldNodeSets.begin() + dwIndex);
    }

    CalNodesPosition();
}

void CFieldTable::Clear()
{
    Name = "Field Table";
    FieldNodeSets.clear();
    FieldNodes.clear();
    FieldData.clear();
}

vector <CFieldNodeSet> CFieldTable::GetNodeSets()
{
    return FieldNodeSets;
}

bool CFieldTable::SetNodes(const vector <CFieldNode> &vecFieldNodes)
{
    bool bRtn = true;
    if(vecFieldNodes.empty())
    {
        bRtn = false;
        return bRtn;
    }

    FieldNodes.clear();
    FieldNodes.assign(vecFieldNodes.begin(), vecFieldNodes.end());
    bRtn = UpdateFieldNodeSets();
    return bRtn;
}

vector <CFieldNode> CFieldTable::GetNodes()
{
    return FieldNodes;
}

void CFieldTable::Show()
{
    cout << endl << "================Table " << Name << "================================" << endl;
    cout << " Name | Node Number | length |" << endl;

    cout.setf(ios::left);
    for (vector <CFieldNodeSet>::iterator itr = FieldNodeSets.begin();
         itr != FieldNodeSets.end(); ++itr)
    {
        cout << dec << " " << setw(12) << itr->GetName() << setw(10) << itr->GetNodeNumber()
             << setw(8) << itr->GetLength() << endl;
    }
    cout << "=======================================================" << endl;
}
string CFieldTable::toString()
{
    return Name;
}

DWORD CFieldTable::GetTableLength()
{
    DWORD dwWidth = 0;

    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        dwWidth += itr->GetWidth();
    }

    return dwWidth;
}

void CFieldTable::SetNodesData(vector <DWORD> &NodesData)
{
    for(DWORD i = 0; i < FieldNodes.size(); i++)
    {
        if(i < NodesData.size())
        {
            FieldNodes[i].SetData(NodesData[i]);
        }
        else
        {
            FieldNodes[i].SetData(0);
        }
    }

    BuildData();

}

vector <DWORD> CFieldTable::GetNodesData()
{
    vector <DWORD> NodesDataList;

    for(DWORD i = 0; i < FieldNodes.size(); i++)
    {
        NodesDataList.push_back(FieldNodes[i].GetData());
    }

    return NodesDataList;
}


void CFieldTable::SetData(vector <DWORD>  &DataList)
{
    if(DataList.empty())
    {
        return;
    }

    FieldData.clear();

    FieldData.assign(DataList.begin(), DataList.end());

    DWORD dwWidth  =  GetTableLength() / FIELD_NODE_MAX_BIT_WIDTH + 1;
    int Num  = dwWidth - FieldData.size();

    if(Num > 0)
    {
        for(int i = 0; i < Num; ++i)
        {
            FieldData.push_back(0);
        }
    }

    ParseData();

}


void CFieldTable::GetData(vector <DWORD>  &Data)
{
    Data.clear();
    BuildData();
    Data.assign(FieldData.begin(), FieldData.end());
}

void CFieldTable::GetReversedData(vector <DWORD>  &DataList)
{
    DataList.clear();
    BuildData();
    DataList.assign(FieldReversedData.begin(), FieldReversedData.end());
}


void CFieldTable::SetReversedBitData(DWORD dwBitPos, DWORD dwBitWidth, const DWORD &dwData)
{
    DWORD dwBeginArry = dwBitPos / FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwPos = dwBitPos % FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwValue = dwData;

    FieldReversedData[dwBeginArry] |= (dwData << dwPos);

    if((dwPos + dwBitWidth) > FIELD_NODE_MAX_BIT_WIDTH)
    {
        dwValue = (dwData >> (FIELD_NODE_MAX_BIT_WIDTH - dwPos));

        FieldReversedData[dwBeginArry + 1] |= dwValue;

    }

    return ;
}

DWORD CFieldTable::GetReversedBitData(DWORD dwBitPos, DWORD dwBitWidth)
{
    DWORD dwBeginArry = dwBitPos / FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwPos = dwBitPos % FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwValue = 0;

    dwValue = FieldReversedData[dwBeginArry] >> dwPos;

    if((dwPos + dwBitWidth) > FIELD_NODE_MAX_BIT_WIDTH )
    {
        dwValue |= (FieldReversedData[dwBeginArry + 1] << (FIELD_NODE_MAX_BIT_WIDTH - dwPos));
    }

    dwValue = dwValue & ((int)pow(2, dwBitWidth) - 1);

    //cout <<__FUNCTION__<<" Pos: "<<dwBitPos<<" Width:"<<dwBitWidth<<" Data:"<<dwValue<< endl;
    return dwValue;
}



void CFieldTable::SetBitData(DWORD dwBitPos, DWORD dwBitWidth, const DWORD &dwData)
{
    DWORD dwBeginArry = dwBitPos / FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwPos = dwBitPos % FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwValue = dwData;

    dwValue = dwData << (FIELD_NODE_MAX_BIT_WIDTH - dwBitWidth);
    FieldData[dwBeginArry] |= dwValue >> dwBitPos;

    if((dwPos + dwBitWidth) > FIELD_NODE_MAX_BIT_WIDTH)
    {
        FieldData[dwBeginArry + 1] |= (dwData << (2 * FIELD_NODE_MAX_BIT_WIDTH - dwPos - dwBitWidth));
    }

    // cout <<dec<<__FUNCTION__<<" Pos: "<<dwBitPos<<" Width: "<<dwBitWidth<<" Data: "<<dwData
    //    <<" FieldData["<<dwBeginArry<<"]:" <<FieldData[dwBeginArry]<<" " <<hex<<FieldData[dwBeginArry]<< endl;

    if((dwPos + dwBitWidth) > FIELD_NODE_MAX_BIT_WIDTH)
    {
        // cout <<dec<<" FieldData["<<dwBeginArry + 1<<"]:" <<FieldData[dwBeginArry+1]<<" "<<hex<<FieldData[dwBeginArry+1]<<endl;
    }
}


DWORD CFieldTable::GetBitData(DWORD dwBitPos, DWORD dwBitWidth)
{
    DWORD dwBeginArry = dwBitPos / FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwPos = dwBitPos % FIELD_NODE_MAX_BIT_WIDTH;
    DWORD dwValue = 0;

    dwValue = FieldData[dwBeginArry] << dwPos;
    dwValue = dwValue >> (FIELD_NODE_MAX_BIT_WIDTH - dwBitWidth);

    if((dwPos + dwBitWidth) > FIELD_NODE_MAX_BIT_WIDTH )
    {
        dwValue |= (FieldData[dwBeginArry + 1] >> (2 * FIELD_NODE_MAX_BIT_WIDTH - dwPos - dwBitWidth));
    }

    //cout <<__FUNCTION__<<" Pos: "<<dwBitPos<<" Width:"<<dwBitWidth<<" Data:"<<dwValue<< endl;
    return dwValue;
}


void CFieldTable::BuildData()
{
    FieldData.clear();
    FieldReversedData.clear();
    FieldData.assign(GetTableLength() / FIELD_NODE_MAX_BIT_WIDTH + 1, 0);
    FieldReversedData.assign(GetTableLength() / FIELD_NODE_MAX_BIT_WIDTH + 1, 0);

    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        // cout <<__FUNCTION__<<dec<<" Pos: "<<itr->GetPosition()<<" Width:"<<itr->GetWidth()<<" Data:"<<hex<<itr->GetData()<< endl;
        SetBitData(itr->GetPosition(), itr->GetWidth(), itr->GetData());
        SetReversedBitData(itr->GetPosition(), itr->GetWidth(), itr->GetData());
    }
}

void  CFieldTable::ParseData()
{
    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        DWORD dwData = GetBitData(itr->GetPosition(), itr->GetWidth());
        itr->SetData(dwData);
    }
}


void CFieldTable::ShowNodes()
{
    cout << endl << "=====================================================" << endl;
    cout << " Name     Position   Width    Decimal     Hex" << endl;

    cout.setf(ios::left);
    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        cout << dec << " " << setw(12) << itr->GetName() << setw(10) << itr->GetPosition()
             << setw(8) << itr->GetWidth() << setw(12) << itr->GetData() << hex << itr->GetData() << endl;
    }
    cout << "=======================================================" << endl;
}


void CFieldTable::ShowData()
{
    DWORD i = 0;
    cout << endl << "=====================================================" << endl;
    cout << " Num   DecimalData     HexData " << endl;

    cout.setf(ios::left);
    for (vector <DWORD>::iterator itr = FieldData.begin();
         itr != FieldData.end(); ++itr)
    {
        cout << " " << setw(5) << i++ << dec << "  " << setw(15) << *itr << hex << "0x" << *itr << endl;
    }
    cout << "=======================================================" << endl;
}







