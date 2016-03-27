#ifndef COSMOART_TABLE_CTRL_H
#define COSMOART_TABLE_CTRL_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "../fieldpower/CFieldTable.h"
#include "../MarkupSTL/MarkupSTL.h"

using namespace CosmoArt_FieldPower;
using namespace std;

class CTableCtrl
{
private:

    CFieldTable m_FieldTable;  // Field Table

    CMarkupSTL m_XmlParser; // xml Parser

    bool ParseTable(const string& filename);

    vector <CFieldNodeSet> GetTableNodeSets();


public:

    CTableCtrl();

    virtual ~CTableCtrl();

    bool LoadTable(const string& filename);

    const string GetTableName();

    vector <CFieldNode> GetTableNodes();

    void SetNodesData(vector <DWORD> NodesDataList);

    vector <DWORD> GetNodesData();

    int GetTableLength();

    int GetNumofNodes();

    bool SetData(vector <DWORD>& Data);

    bool GetData(vector <DWORD> &Data);

    bool GetReversedData(vector <DWORD> &Data);

};


#endif
