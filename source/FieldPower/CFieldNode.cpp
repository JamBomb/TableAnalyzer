#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "CFieldNode.h"

using namespace CosmoArt_FieldPower;

void CFieldNode::SetName(const string &NodeName)
{
    if(NodeName.empty())
    {
        return;
    }

    Name = NodeName;
}

string CFieldNode::GetName()
{
    return Name;
}

bool CFieldNode::SetWidth(DWORD dwWidth)
{
    if(0 == dwWidth || dwWidth > FIELD_NODE_MAX_BIT_WIDTH)
    {
        return false;
    }
    else
    {
        Width = dwWidth;
    }

    return true;
}

DWORD  CFieldNode::GetWidth()
{
    return Width;
}

void CFieldNode::SetPosition(DWORD dwPos)
{
    Position = dwPos;
}

DWORD CFieldNode::GetPosition( )
{
    return Position;
}

bool CFieldNode::SetData(DWORD dwData)
{
    if(dwData > GetMaxData())
    {
        return false;
    }
    else
    {
        Data = dwData;
    }

    return true;
}

DWORD CFieldNode::GetData()
{
    return Data;
}

string CFieldNode::toString()
{
    return Name;
}


