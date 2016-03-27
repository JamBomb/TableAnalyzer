#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <math.h>
#include "CFieldParser.h"

using namespace CosmoArt_FieldPower;

/*****************************************************************************
 Prototype    : CFieldAnalyse.CalFieldNodesPosition
 Description  : Calculate Field Nodes Position
 Input        : None
 Output       : None
 Return       : void
*****************************************************************************/
inline void CFieldAnalyser::CalFieldNodesPosition()
{
    DWORD dwPos = 0;

    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        itr->SetPosition(dwPos);
        dwPos += itr->GetWidth();
    }
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.GetFieldNodesLength
 Description  : Get Field Nodes Length
 Input        : None
 Output       : None
 Return       : void
*****************************************************************************/
inline DWORD CFieldAnalyser::GetFieldNodesLength()
{
    DWORD dwPos = 0;

    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        dwPos += itr->GetWidth();
    }

    return dwPos;
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.SetFieldNodes
 Description  : Set Field Nodes Info
 Input        : vector <CFieldNode> &vecFieldNodes
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::SetFieldNodes(const vector <CFieldNode> &vecFieldNodes)
{
    if(vecFieldNodes.empty())
    {
        return;
    }

    FieldNodes.clear();

    FieldNodes.assign(vecFieldNodes.begin(), vecFieldNodes.end());

    CalFieldNodesPosition();
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.GetFieldNodes
 Description  : Get  Field Nodes Info
 Input        : vector <CFieldNode> &vecFieldNodes
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::GetFieldNodes(vector <CFieldNode> &vecFieldNodes)
{
    vecFieldNodes.clear();
    vecFieldNodes.assign(FieldNodes.begin(), FieldNodes.end());
    return;
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.SetFieldData
 Description  : set data to be analysed.
 Input        : vector <DWORD>  &vecFieldData
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::SetFieldData(vector <DWORD>  &vecFieldData)
{
    if(vecFieldData.empty())
    {
        return;
    }

    FieldData.clear();

    FieldData.assign(vecFieldData.begin(), vecFieldData.end());

    DWORD dwWidth  = GetFieldNodesWidth() / FIELD_NODE_MAX_BIT_WIDTH + 1;
    int Num  = dwWidth - FieldData.size();

    if(Num > 0)
    {
        for(int i = 0; i < Num; ++i)
        {
            FieldData.push_back(0);
        }
    }

    return ;
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.GetFieldData
 Description  :  CFieldAnalyser::
 Input        : vector <DWORD>  &vecFieldData
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::GetFieldData(vector <DWORD>  &vecFieldData)
{
    vecFieldData.clear();
    vecFieldData.assign(FieldData.begin(), FieldData.end());
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.SetBitData
 Description  : Set Data by bit position & width
 Input        : DWORD dwBitPos
                DWORD dwBitWidth
                const DWORD &dwData
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::SetBitData(DWORD dwBitPos, DWORD dwBitWidth, const DWORD &dwData)
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
/*****************************************************************************
 Prototype    : CFieldAnalyse.GetBitData
 Description  : Get Bit Data by bit pos & bit width
 Input        : DWORD dwBitPos
                DWORD dwBitWidth
 Output       : None
 Return       : DWORD
*****************************************************************************/
DWORD CFieldAnalyser::GetBitData(DWORD dwBitPos, DWORD dwBitWidth)
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

/*****************************************************************************
 Prototype    : CFieldAnalyse.BuildFieldData
 Description  : build data by  m_FieldNodes info
 Input        : None
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::BuildFieldData()
{
    FieldData.clear();

    FieldData.assign(GetFieldNodesLength()/FIELD_NODE_MAX_BIT_WIDTH + 1, 0);

    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        // cout <<__FUNCTION__<<dec<<" Pos: "<<itr->GetPosition()<<" Width:"<<itr->GetWidth()<<" Data:"<<hex<<itr->GetData()<< endl;
        SetBitData(itr->GetPosition(), itr->GetWidth(), itr->GetData());
    }
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.ParseFieldData
 Description  : Parse customer data by m_FieldNodes info
 Input        : None
 Output       : None
 Return       : void
*****************************************************************************/
void  CFieldAnalyser::ParseFieldData()
{
    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        DWORD dwData = GetBitData(itr->GetPosition(), itr->GetWidth());
        itr->SetData(dwData);
    }
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.GetFieldNodesWidth
 Description  : Get Field Nodes Sum Width
 Input        : None
 Output       : None
 Return       : DWORD
 *****************************************************************************/
DWORD  CFieldAnalyser::GetFieldNodesWidth()
{
    DWORD dwWidth = 0;

    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        dwWidth += itr->GetWidth();
    }

    return dwWidth;
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.ShowFieldNodes
 Description  : Show Field Nodes Information
 Input        : None
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::ShowFieldNodes()
{
    cout<<endl<<"====================================================="<<endl;
    cout<<" Name     Position   Width    Decimal     Hex"<<endl;

    cout.setf(ios::left);
    for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
         itr != FieldNodes.end(); ++itr)
    {
        cout<<dec<<" "<<setw(12)<<itr->GetName()<<setw(10)<<itr->GetPosition()
            <<setw(8)<<itr->GetWidth()<<setw(12)<<itr->GetData()<<hex<<itr->GetData()<<endl;
    }
    cout<<"======================================================="<<endl;
}

/*****************************************************************************
 Prototype    : CFieldAnalyse.ShowFieldData
 Description  : Show Field ShowFieldData Information
 Input        : None
 Output       : None
 Return       : void
*****************************************************************************/
void CFieldAnalyser::ShowFieldData()
{
    DWORD i = 0;
    cout<<endl<<"====================================================="<<endl;
    cout<<" Num   DecimalData     HexData "<<endl;

    cout.setf(ios::left);
    for (vector <DWORD>::iterator itr = FieldData.begin();
         itr != FieldData.end(); ++itr)
    {
        cout<<" "<<setw(5)<<i++<<dec<<"  "<<setw(15)<<*itr<<hex<<"0x"<<*itr<<endl;
    }
    cout<<"======================================================="<<endl;
}




