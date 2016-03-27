#ifndef COSMOART_TABLE_PARSER_H
#define COSMOART_TABLE_PARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "../fieldpower/CFieldTable.h"
#include "../fieldpower/CFieldParser.h"
#include "../MarkupSTL/MarkupSTL.h"

using namespace CosmoArt_FieldPower;
using namespace std;

class CTableParser
{
private:

    CFieldTable m_FieldTable;  // Field Table

    CFieldAnalyser m_FieldAnalyser;  // Field Analyser

    CMarkupSTL m_XmlParser; // xml Parser

    bool ParseTableFile(const string& filename);


public:

    CTableParser();

    virtual ~CTableParser();

    bool LoadTableFile(const string& filename);

    const string GetTableName();

    vector <CFieldNodeSet> GetTableNodeSets();

    int GetTableLength();

    int GetNumofTableNodes();

    bool ParseFieldData(vector <DWORD> vecFieldData);

    bool BuildFieldData(vector <DWORD> &vecFieldData);


};


#endif
