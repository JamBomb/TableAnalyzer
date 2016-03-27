#ifndef COSMOART_FIELDPOWER_TABLE_H
#define COSMOART_FIELDPOWER_TABLE_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "CFieldNodeSet.h"


namespace CosmoArt_FieldPower
{
    using namespace std;

    class CFieldTable
    {
    private:
        string Name;
        vector <CFieldNodeSet>  FieldNodeSets;  // Field Node Set
        vector <CFieldNode> FieldNodes;
        vector <DWORD>  FieldData;     //  Field Data
        vector <DWORD>  FieldReversedData;     //  Field Reversed Data

        void DelNodeSet(DWORD dwIndex);

        void AddNodeSets(vector <CFieldNodeSet>  vctFieldNodeSets);

        bool UpdateFieldNodeSets();

        bool SetNodes(const vector <CFieldNode> &vecFieldNodes);

        vector <CFieldNodeSet> GetNodeSets();

        inline void CalNodesPosition();

        void SetBitData(DWORD dwBitPos, DWORD dwBitWidth, const DWORD &dwData);
        DWORD GetBitData(DWORD dwBitPos, DWORD dwBitWidth);

        void SetReversedBitData(DWORD dwBitPos, DWORD dwBitWidth, const DWORD &dwData);
        DWORD GetReversedBitData(DWORD dwBitPos, DWORD dwBitWidth);

        void BuildData();
        void ParseData();


    public:
        CFieldTable();

        virtual ~CFieldTable();

        void Clear();

        void SetName(const string &TableName);

        string GetName();

        void AddNodeSet(const CFieldNodeSet FieldNodeSet);

        vector <CFieldNode> GetNodes();

        int GetNumofNodes();

        DWORD GetTableLength();

        void SetNodesData(vector <DWORD> &NodesData);

        vector <DWORD> GetNodesData();

        void SetData(vector <DWORD>  &DataList);

        void GetData(vector <DWORD>  &DataList);

        void GetReversedData(vector <DWORD>  &DataList);

        string toString();

        //==============================================================================
        // the serial showxxx  functions  are for debug
        //==============================================================================
        void ShowNodes();
        void ShowData();
        void Show();
        //==============================================================================


    };

}
#endif
