#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <math.h>
#include "CFieldNodeSet.h"

using namespace CosmoArt_FieldPower;

void CFieldNodeSet::SetName(const string &NodeSetName)
{
    if(NodeSetName.empty())
    {
        return;
    }
    Name.clear();
    Name = NodeSetName;
}

string CFieldNodeSet::GetName()
{
    return Name;
}

void CFieldNodeSet::AddNode(const CFieldNode &FieldNode)
{
    FieldNodes.push_back(FieldNode);
}

vector <CFieldNode> CFieldNodeSet::GetFieldNodes()
{
    vector <CFieldNode>  RtnFieldNodes;
    RtnFieldNodes.clear();

    RtnFieldNodes.assign(FieldNodes.begin(), FieldNodes.end());

    return RtnFieldNodes;
}

void CFieldNodeSet::ClearFieldNodes()
{
    FieldNodes.clear();
}

void CFieldNodeSet::AddFieldNodes(vector <CFieldNode>  vctFieldNodes)
{
    for (vector <CFieldNode>::iterator itr = vctFieldNodes.begin();
         itr != vctFieldNodes.end(); ++itr)
    {
        FieldNodes.push_back(*itr);
    }
}

void CFieldNodeSet::ShowFieldNodes()
{
    cout << endl << "================" << Name << "================================" << endl;
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
string CFieldNodeSet::toString()
{
    return Name;
}


