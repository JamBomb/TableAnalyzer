#ifndef COSMOART_FIELDPOWER_NODE_SET_H
#define COSMOART_FIELDPOWER_NODE_SET_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "CFieldNode.h"

namespace CosmoArt_FieldPower
{
    using namespace std;

    class CFieldNodeSet
    {
    private:
        string Name;
        vector <CFieldNode>  FieldNodes;  // Field Node Set

    public:
        CFieldNodeSet()
        {
            Name = "NodeSet";
        }

        void SetName(const string &NodeSetName);
        string  GetName();

        void AddNode(const CFieldNode &FieldNode);
        void DelNode(DWORD dwIndex);
        void ClearFieldNodes();
        void AddFieldNodes(vector <CFieldNode> vctFieldNodes);

        vector <CFieldNode> GetFieldNodes();

        inline DWORD GetLength()
        {
            DWORD dwLength = 0;
            for (vector <CFieldNode>::iterator itr = FieldNodes.begin();
                 itr != FieldNodes.end(); ++itr)
            {
                dwLength += itr->GetWidth();
            }

            return dwLength;
        }

        inline DWORD GetNodeNumber()
        {
            return FieldNodes.size();
        }

        void ShowFieldNodes();

        string toString();

        ~CFieldNodeSet()
        {
            FieldNodes.clear();
        }
    };

}
#endif
