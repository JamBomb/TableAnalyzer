//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "AboutForm.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MPHexEditor"
#pragma link "MPHexEditorEx"
#pragma link "SUIButton"
#pragma link "SUIGrid"
#pragma link "SUISideChannel"
#pragma link "SUITreeView"
#pragma link "WinSkinData"
#pragma link "SUIStatusBar"
#pragma link "CnHint"
#pragma link "CnClasses"
#pragma link "CnTrayIcon"
#pragma resource "*.dfm"
TTableForm *TableForm;
//---------------------------------------------------------------------------
__fastcall TTableForm::TTableForm(TComponent* Owner)
    : TForm(Owner)
{
    strngrd1->Cells[0][0] = "Table Field";
    strngrd1->Cells[0][1] = "Bits [range]";
    strngrd1->Cells[0][2] = "Data [dec]";

    m_DataFormat = DECIMAL_DATA_TYPE;

    m_bShowedApplication = true;

    m_pTableProject = new CProject();

    m_TableDirectory = ExtractFilePath(Application->ExeName) + "Tables\\";

    InitTableProject(m_TableDirectory + "BCMAradTables.tap");

    UpdateCtrlState(false);

    m_bShowTrayHint = false;

    mphxdtrx1->ClipboardAsHexText = true;

}

DWORD __fastcall TTableForm::StringToValue(String strData)
{
    DWORD dwData = 0;
    strData = strData.Trim();

    if(strData.IsEmpty())
    {
        dwData = 0;
    }
    else
    {
        switch (m_DataFormat)
        {

            case DECIMAL_DATA_TYPE:
            {
                dwData = StrToInt(strData);
            }
            break;

            case HEXADECIMAL_DATA_TYPE:
            {
                strData = "0x"+ strData;
                dwData = StrToInt(strData);
            }
            break;

            case BINARY_DATA_TYPE:
            {
                if( 0 == strData.Pos("1"))
                {
                    dwData = 0;
                }
                else
                {
                    strData = strData.Delete(1, strData.Pos("1") - 1);

                    for(int i = strData.Length(); i > 0; i--)
                    {
                        dwData = dwData + StrToInt(strData[i]) * pow(2, strData.Length() - i);
                    }
                }
            }
            break;

            default:
                dwData = 0;
                break;

        }

    }

    return dwData;
}

String __fastcall TTableForm::ValueToString(DWORD dwData, Data_Format eDataFormat)
{
    String  strData = "";

    if(0 == dwData)
    {
        strData = "0";
        return strData;
    }

    switch (eDataFormat)
    {

        case DECIMAL_DATA_TYPE:
        {
            strData = IntToStr((__int64)dwData);
        }
        break;

        case HEXADECIMAL_DATA_TYPE:
        {
            strData = IntToHex((__int64)dwData, 1);
        }
        break;

        case BINARY_DATA_TYPE:
        {
            String tmpStr = 0;
            DWORD dwQuotient = 0;
            DWORD dwRemainder = 0;


            dwQuotient = dwData / 2 ;
            dwRemainder = dwData % 2;
            tmpStr = IntToStr(dwRemainder);

            while(dwQuotient > 0)
            {
                dwRemainder = dwQuotient % 2;

                tmpStr = tmpStr +  IntToStr(dwRemainder);

                dwQuotient = dwQuotient / 2 ;

            };


            for(int i = tmpStr.Length(); i > 0; i--)
            {
                strData = strData +  tmpStr[i];
            }

        }
        break;

        default:
            strData = "0";
            break;

    }

    return    strData;

}


void __fastcall TTableForm::ChangeDataFormat(Data_Format eDataFormat)
{
    if (eDataFormat >= DATA_TYPE_MAX)
    {
        return;
    }

    String Format = (eDataFormat == DECIMAL_DATA_TYPE) ? "dec" : (eDataFormat == HEXADECIMAL_DATA_TYPE) ? "hex":"bin";

    strngrd1->Cells[0][2] = "Data ["+ Format +"]";

    for (int i = 1; i < strngrd1->ColCount; i++)
    {
        String strData = strngrd1->Cells[i][2];
        strData = strData.Trim();

        if(strData.IsEmpty())
        {
            strData = "0";
        }

        strngrd1->Cells[i][2] = ValueToString(StringToValue(strData), eDataFormat);
    }

    m_DataFormat = eDataFormat;

}


void __fastcall TTableForm::OnMinMaxSize(TMessage& Msg)
{
    if (Msg.WParam == SC_MINIMIZE)
    {
        // to do at minize event.
        ti1->Active = true;
        ti1->HideApplication();
        ti1->ShowIcon();
        m_bShowedApplication = false;

        if(m_bShowTrayHint == false)
        {
            ti1->BalloonHint("Hi:","Table Analyzer is here!", (TBalloonType)0, 3);
            m_bShowTrayHint = true;
        }
    }
    else if (Msg.WParam == SC_MAXIMIZE)
    {
        // to do at maxize event.
    }
    else if(Msg.WParam == SC_CLOSE)
    {

        switch (MessageBox(Handle, "Do you want to close Table Analyzer?", Application->Title.c_str(), MB_YESNO + MB_ICONQUESTION))
        {
            case IDYES:
            {
                //Close();
                //#define SC_CLOSE        0xF060   @winuser.h
                break;
            }

            case IDNO:
            {
                Msg.WParam = 0;
                break;
            }
        }
    }


    TForm::Dispatch(&Msg); //将消息传给VCL以继续处理
}

void __fastcall TTableForm::UpdateCtrlState(bool bEnable)
{
    btnBuildData->Enabled = bEnable;
    btnAnalyseData->Enabled = bEnable;
    strngrd1->Enabled = bEnable;
    edt2->Text = "";


}

//---------------------------------------------------------------------------
bool __fastcall TTableForm::InitTableGrid(AnsiString TableFileName)
{
    for(int i = 1; i < strngrd1->ColCount; i++)
    {
        strngrd1->Cols[i]->Clear();
    }



    if(false == FileExists(TableFileName))
    {
        String WarningInfo = "The Table File doesn't exist.\n" + TableFileName;
        Application->MessageBox(WarningInfo.c_str(),
                                Application->Title.c_str(), MB_OK + MB_ICONWARNING);
        return false;

    }

    // load the table file
    m_TableCtrl.LoadTable(TableFileName.c_str());

    stat1->SimpleText = "Table: " + String(m_TableCtrl.GetTableName().c_str()) +
                        " [length: " + IntToStr(m_TableCtrl.GetTableLength()) + " bit, " +
                        " Node: " + IntToStr(m_TableCtrl.GetNumofNodes()) + "]";

    vector <CFieldNode> Nodelist;

    Nodelist =  m_TableCtrl.GetTableNodes();

    strngrd1->ColCount = Nodelist.size() + 1;
    
    for(DWORD i = 0; i < Nodelist.size(); i++)
    {
        strngrd1->Cells[1 + i][0] = String(Nodelist[i].GetName().c_str());
        strngrd1->Cells[1 + i][1] = IntToStr(Nodelist[i].GetWidth()) + "b";
        strngrd1->Cells[1 + i][1] = strngrd1->Cells[1 + i][1] + " [" + IntToStr(Nodelist[i].GetPosition()) + ":" + IntToStr(Nodelist[i].GetWidth() + Nodelist[i].GetPosition() - 1) + "]";
    }
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TTableForm::InitTableProject(AnsiString ProjectFileName)
{
    m_pTableProject->Load(ProjectFileName.c_str());

    TTreeNode *RootNode = NULL;
    TTreeNode *ChildNode = NULL;
    TTreeNode *Node = NULL;
    DWORD dwImageIdx = 0;
    DWORD dwImageSelectedIdx = 0;
    //CRegModule cRegModule;

    tvTables->Items->Clear();
    for(DWORD i = 0; i < m_pTableProject->NodeList.size(); i++)
    {
        dwImageIdx = (m_pTableProject->NodeList[i].Level == 0 ) ? TABLEANALYSER_PROJECT_ICON_INDEX :
                     (m_pTableProject->NodeList[i].Level == 1) ? TABLEANALYSER_MODULE_ICON_INDEX : TABLEANALYSER_ITEM_ICON_INDEX;

        dwImageSelectedIdx = (m_pTableProject->NodeList[i].Level == 0 ) ? TABLEANALYSER_PROJECT_SELECTED_ICON_INDEX :
                             (m_pTableProject->NodeList[i].Level == 1) ? TABLEANALYSER_MODULE_SELECTED_ICON_INDEX : TABLEANALYSER_ITEM_SELECTED_ICON_INDEX;

        if(0 == m_pTableProject->NodeList[i].Level)
        {
            Node = tvTables->Items->Add(NULL, String(m_pTableProject->NodeList[i].Name.c_str()));
            RootNode = Node;
            RootNode->Expanded = true;

        }
        else if(1 == m_pTableProject->NodeList[i].Level)
        {
            //添加Module节点
            Node = tvTables->Items->AddChild(RootNode, String(m_pTableProject->NodeList[i].Name.c_str()));
            ChildNode = Node;
        }
        else
        {
            Node = tvTables->Items->AddChild(ChildNode, String(m_pTableProject->NodeList[i].Name.c_str()));
        }

        Node->Data = (void *)(&m_pTableProject->NodeList[i]);

        Node->ImageIndex =  dwImageIdx;
        Node->SelectedIndex = dwImageSelectedIdx;
    }

    // tvTables->FullCollapse();
    //tvTables->FullExpand();
    tvTables->Update();

}

void __fastcall TTableForm::tvTablesClick(TObject *Sender)
{
    TPrjNode *pPrjNode = NULL;
    String TableFile;

    if (m_pSelectedTreeNode == tvTables->Selected)
    {
        return;
    }

    m_pSelectedTreeNode = tvTables->Selected;


    if(m_pSelectedTreeNode->Level == 2)
    {
        pPrjNode = (TPrjNode*)(m_pSelectedTreeNode->Data);

        TableFile = pPrjNode->Value.c_str();

        TableFile = m_TableDirectory + TableFile.Trim();

        if(true == InitTableGrid(TableFile))
        {
            UpdateCtrlState(true);
        }
        else
        {
            UpdateCtrlState(false);
        }
    }
    else
    {
        stat1->SimpleText = "";

        for(int i = 1; i < strngrd1->ColCount; i++)
        {
            for(int j = 0; j < strngrd1->RowCount; j++)
            {
                strngrd1->Cells[i][j] = "";
            }
        }

        UpdateCtrlState(false);

    }

}
//---------------------------------------------------------------------------



void __fastcall TTableForm::btn6Click(TObject *Sender)
{
    //
//    vector < DWORD > & Data

// m_TableCtrl.SetData();

}
//---------------------------------------------------------------------------


void __fastcall TTableForm::strngrd1DrawCell(TObject *Sender, int ACol,
        int ARow, TRect &Rect, TGridDrawState State)
{
    AnsiString s = strngrd1->Cells[ACol][ARow].Trim();
    if (State.Contains(gdFixed))
    {
        strngrd1->Canvas->Brush->Color = clSkyBlue;
        strngrd1->Canvas->FillRect(Rect);

        if(ACol > 0)
        {
            DrawText(strngrd1->Canvas->Handle, s.c_str(), -1, &Rect, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
        }
        else
        {
            DrawText(strngrd1->Canvas->Handle, s.c_str(), -1, &Rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

        }
    }
    else if( ARow > 1 && strngrd1->Cells[0][ARow] != "" || State.Contains(gdSelected))
    {
        strngrd1->Canvas->Brush->Color = (TColor) 0x00CAFFFF;
        strngrd1->Canvas->FillRect(Rect);
        //TRect Boundary(Rect.left,Rect.top, Rect.right, Rect.bottom * 10);
        strngrd1->Canvas->Pen->Color = clBlack;

        WORD wFlag = DT_VCENTER | DT_CENTER | DT_WORDBREAK;
        //  wFlag =   DT_LEFT  | DT_WORDBREAK;

        DrawText(strngrd1->Canvas->Handle, s.c_str(), -1, &Rect, wFlag);
    }
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::strngrd1KeyPress(TObject *Sender, char &Key)
{
    String NumberList = "";
    String Tmp = Key;
    Tmp = Tmp.UpperCase();

    if (Key == VK_BACK || Key == VK_DELETE || Key == VK_RETURN)
    {
        return ;
    }

    NumberList = (DECIMAL_DATA_TYPE == m_DataFormat)? ("0123456789") : (HEXADECIMAL_DATA_TYPE == m_DataFormat)? "0123456789ABCDEF" : "01";

    if(!NumberList.Pos(Tmp)) //Hex: !HexList.Pos(Tmp)
    {
        Key = 0;
    }
    else if(m_GridCol > 0)
    {

        String StrTmp = strngrd1->Cells[m_GridCol][1];
        StrTmp = StrTmp.Trim();
        StrTmp = StrTmp.SubString(1, StrTmp.Pos("b") - 1);
        DWORD MaxValue =  pow(2, StrToInt(StrTmp));
        DWORD CurValue = StringToValue(strngrd1->Cells[m_GridCol][2] + Tmp); //   StrToInt(strngrd1->Cells["0x"+m_GridCol][2] + Tmp)

        if(CurValue >= MaxValue)
        {
            Key = 0;
        }
        else
        {
            /*输入时字母自动转换成大写*/
            char cUpcase = *(Tmp.c_str());
            Key = cUpcase;
        }

    }
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::strngrd1SelectCell(TObject *Sender, int ACol,
        int ARow, bool &CanSelect)
{
    if(ARow == 2 && !strngrd1->Cells[m_GridCol][1].IsEmpty())
    {
        strngrd1->Options << goEditing;
        strngrd1->Options << goAlwaysShowEditor;
        m_GridCol = ACol;
        strngrd1->Cells[ACol][ARow] = strngrd1->Cells[ACol][ARow].Trim();
    }
    else
    {
        m_GridCol = 0;
    }

}
//---------------------------------------------------------------------------

void __fastcall TTableForm::strngrd1Exit(TObject *Sender)
{
    strngrd1->Options >> goEditing;
    strngrd1->Options >> goAlwaysShowEditor;
}
//---------------------------------------------------------------------------
bool __fastcall TTableForm::SetFieldDataToHexEditor(vector <BYTE> FieldData)
{
    if(0 == FieldData.size())
    {
        return false;
    }

    DWORD dwBufSize = FieldData.size();

    BYTE *pbtBuffer = new BYTE[dwBufSize];

    for(DWORD i = 0; i < dwBufSize; i++)
    {
        pbtBuffer[i] =  FieldData[i];

    }

    TMemoryStream *pmsData = new TMemoryStream();
    pmsData->Write(pbtBuffer, dwBufSize);

    mphxdtrx1->LoadFromStream(pmsData);// for fix HexEditor some error when it is empty.
    mphxdtrx1->SelectAll();
    mphxdtrx1->DeleteSelection(mphxdtrx1->SelectionAsText);
    mphxdtrx1->LoadFromStream(pmsData);
    //HexEditor->SelectAll();

    delete pmsData;
    delete []pbtBuffer;
    return true;
}

//----------------------------------------------------------------------------
vector <DWORD> __fastcall TTableForm::GetFieldDataFromHexEditor()
{
    vector <DWORD>  FieldData;
    DWORD dwData = 0;
    DWORD dwCol = mphxdtrx1->DataSize / 4;
    DWORD dwRow = mphxdtrx1->DataSize % 4;

    /*if mphxdtrx1 is emtpy, then assign  m_FieldData is all zeros.*/
    if (0 == mphxdtrx1->DataSize)
    {
        FieldData.push_back(0);
        return FieldData;
    }

    // firstly, put
    for (DWORD i = 0; i < dwCol * 4; ++i)
    {
        dwData |= mphxdtrx1->Data[i] << ((3 - i%4) * 8);

        if((i+1)%4 == 0)
        {
            FieldData.push_back(dwData);
            dwData = 0;
        }
    }

    // secondly,
    if(dwRow > 0)
    {
        dwData = 0;
        for(DWORD i = dwCol * 4; i < dwCol * 4 + dwRow; ++i)
        {
            dwData |= mphxdtrx1->Data[i] << ((3 - i%4) * 8);
        }

        FieldData.push_back(dwData);
    }

    return FieldData;
}


//----------------------------------------------------------------------------
void __fastcall TTableForm::btnBuildDataClick(TObject *Sender)
{
    vector<DWORD> NodeDataList;
    vector<DWORD> DataList;


    for (int i = 1; i < strngrd1->ColCount; i++)
    {
        String strData = strngrd1->Cells[i][2];
        strData = strData.Trim();
        DWORD  dwData = 0;

        dwData = StringToValue(strData);

        NodeDataList.push_back(dwData);

    }

    // set every node data in the talbe.
    m_TableCtrl.SetNodesData(NodeDataList);



    edt2->Text = "";

    m_TableCtrl.GetReversedData(DataList);

    for(DWORD i = 0; i < DataList.size(); i++)
    {
        String HexValue = "[0x"+ IntToHex((__int64)DataList[i], 1) +  "]";
        edt2->Text = edt2->Text + IntToStr((__int64)DataList[i]) + HexValue;
        if(i < DataList.size() - 1)
        {
            edt2->Text = edt2->Text + "+";
        }
    }

    DataList.clear();

    m_TableCtrl.GetData(DataList);

    vector <BYTE>  ByteList;

    for(DWORD i = 0; i < DataList.size(); i++)
    {
        for(DWORD j = 0; j < 4; j++)
        {
            ByteList.push_back((DataList[i] >> ((3-j)*8)) & 0xFF);
        }
    }

    SetFieldDataToHexEditor(ByteList);

}
//---------------------------------------------------------------------------


void __fastcall TTableForm::btnAnalyseDataClick(TObject *Sender)
{
    vector <DWORD> FieldData;

    if(0 == mphxdtrx1->DataSize)
    {
        return ;
    }

    FieldData = GetFieldDataFromHexEditor();

    m_TableCtrl.SetData(FieldData);

    FieldData.clear();

    FieldData = m_TableCtrl.GetNodesData();

    for (int i = 1; i < strngrd1->ColCount; i++)
    {
        strngrd1->Cells[i][2] = ValueToString(__int64(FieldData[i - 1]), m_DataFormat);
    }

    // set every node data in the table.
    m_TableCtrl.SetNodesData(FieldData);

    vector<DWORD> DataList;

    edt2->Text = "";

    m_TableCtrl.GetReversedData(DataList);

    for(DWORD i = 0; i < DataList.size(); i++)
    {
        edt2->Text = edt2->Text + IntToStr((__int64)DataList[i]) + "[0x"+ IntToHex((__int64)DataList[i], 1) +  "]";
        if(i < DataList.size() - 1)
        {
            edt2->Text = edt2->Text + "+";
        }
    }

    strngrd1->Invalidate();

}
//---------------------------------------------------------------------------
void __fastcall TTableForm::ti1Click(TObject *Sender, TMouseButton Button,
                                     TShiftState Shift, int X, int Y)
{

    if(m_bShowedApplication)
    {
        ti1->HideApplication();
    }
    else
    {

        ti1->ShowApplication();
    }

    m_bShowedApplication = !m_bShowedApplication;



}
//---------------------------------------------------------------------------


void __fastcall TTableForm::Show1Click(TObject *Sender)
{
    ti1->ShowApplication();
    m_bShowedApplication = true;
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Exit1Click(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::btnAboutClick(TObject *Sender)
{
    AboutForm1->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::paste1Click(TObject *Sender)
{
    mphxdtrx1->CBPaste();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Copy1Click(TObject *Sender)
{
    mphxdtrx1->CBCopy();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Cut1Click(TObject *Sender)
{
    mphxdtrx1->CBCut();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Redo1Click(TObject *Sender)
{
    mphxdtrx1->Redo();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Undo1Click(TObject *Sender)
{
    mphxdtrx1->Undo();
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Dec1Click(TObject *Sender)
{
    ChangeDataFormat(DECIMAL_DATA_TYPE);
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Hex1Click(TObject *Sender)
{
    ChangeDataFormat(HEXADECIMAL_DATA_TYPE);

}
//---------------------------------------------------------------------------

void __fastcall TTableForm::Bin1Click(TObject *Sender)
{
    ChangeDataFormat(BINARY_DATA_TYPE);
}
//---------------------------------------------------------------------------

void __fastcall TTableForm::btn3Click(TObject *Sender)
{
        WinExec(_T("calc.exe"),SW_SHOWNORMAL);        
}
//---------------------------------------------------------------------------

