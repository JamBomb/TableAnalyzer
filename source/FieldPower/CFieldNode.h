#ifndef COSMOART_FIELDPOWER_NODE_H
#define COSMOART_FIELDPOWER_NODE_H

#include <iostream>
#include <vector>
#include <string>
#include <math.h>

namespace CosmoArt_FieldPower
{
#ifndef DWORD
#define DWORD unsigned int
#endif

#ifndef WORD
#define WORD unsigned short int
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

    using namespace std;

    const DWORD FIELD_NODE_MAX_BIT_WIDTH = 32;

    class CFieldNode
    {
    private:
        string Name;
        DWORD Data;  // record Node value
        DWORD Width; // Node Width (maybe more than 32bit)
        DWORD Position; // Node position at total field width

        DWORD GetMaxData()
        {
            return pow(2, Width) - 1;
        }

    public:
        CFieldNode()
        {
            Name = "Node";
            Position = 0;
            Width = 0;
            Data = 0;
        }

        void SetName(const string &NodeName);
        string  GetName();

        bool SetWidth(DWORD dwWidth);
        DWORD GetWidth();

        void SetPosition(DWORD dwPos);
        DWORD GetPosition();

        bool SetData(DWORD dwData);
        DWORD GetData();

        string toString();
    };

}
#endif
