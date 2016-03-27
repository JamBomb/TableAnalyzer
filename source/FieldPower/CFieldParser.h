#ifndef COSMOART_FIELDPOWER_ANALYSE_H
#define COSMOART_FIELDPOWER_ANALYSE_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>

#include "CFieldNode.h"

namespace CosmoArt_FieldPower
{

    class CFieldAnalyser
    {
    public:// User declarations

        CFieldAnalyser()
        {
            FieldNodes.clear();
            FieldData.clear();
        }

        virtual ~CFieldAnalyser()
        {

        }

        void  Clear()
        {
            FieldNodes.clear();
            FieldData.clear();
        }

        void SetFieldNodes(const vector <CFieldNode> &vecFieldNodes);
        void GetFieldNodes(vector <CFieldNode> &vecFieldNodes);
        void SetFieldData(vector <DWORD>  &vecFieldData);
        void GetFieldData(vector <DWORD>  &vecFieldData);
        void SetBitData(DWORD dwBitPos, DWORD dwBitWidth, const DWORD &dwData);
        DWORD GetBitData(DWORD dwBitPos, DWORD dwBitWidth);
        void BuildFieldData();
        void ParseFieldData();
        DWORD GetFieldNodesWidth();
        void ShowFieldNodes();
        void ShowFieldData();

    private:// User declarations

        vector <CFieldNode>  FieldNodes;  // Field Node Set

        vector <DWORD>  FieldData;     //  Field Data

        inline void CalFieldNodesPosition();

        inline DWORD GetFieldNodesLength();

    };


}

//---------------------------------------------------------------------------
#endif


