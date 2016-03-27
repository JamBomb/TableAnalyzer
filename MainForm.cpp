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
    strngrd1->Cells[0][2] = "Data [hex]";

    m_bShowedApplication = true;

    m_pTableProject = new CProject();

    m_TableDirectory = ExtractFilePath(Application->ExeName) + "Tables\\";

    InitTableProject(m_TableDirectory + "BCMAradTables.tap");

    UpdateCtrlState(false);

    m_bShowTrayHint = false;

    mphxdtrx1->ClipboardAsHexText = true;
     
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
                ti1->BalloonHint("Hi:","Table Analyzer is here!", 0, 3);
                m_bShowTrayHint = true;
        }
    }
    else if (Msg.WParam == SC_MAXIMIZE)
    {
        // to do at maxize event.
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
    for(int i = 0; i < strngrd1->RowCount; i++)
    {
        strngrd1->Rows[i]->Clear();
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

    strngrd1->Cells[0][0] = "Table Field";
    strngrd1->Cells[0][1] = "Bits [range]";
    strngrd1->Cells[0][2] = "Data [hex]";

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
    String HexList = "0123456789ABCDEF";
    String Tmp = Key;
    Tmp = Tmp.UpperCase();

    if(Key != VK_BACK && Key != VK_DELETE && Key != VK_RETURN && !HexList.Pos(Tmp))
    {
        Key = 0;
    }
    else if(m_GridCol > 0)
    {
        BYTE byStrLen = strngrd1->Cells[m_GridCol][2].Length();

        BYTE byFieldWidth = 0; //defult value

        String StrTmp = strngrd1->Cells[m_GridCol][1];

        StrTmp = StrTmp.Trim();

        StrTmp = StrTmp.SubString(1, StrTmp.Pos("b") - 1);

        byFieldWidth = StrToInt(StrTmp);

        // limit inputted number size.
        BYTE byLen = (byFieldWidth - 1) / 4 + 1;

        if(byStrLen >= byLen && Key != VK_BACK && Key != VK_DELETE)
        {
            Key = 0;
        }
        else
        {
            if(Key != VK_BACK && Key != VK_DELETE && Key != VK_RETURN && StrToInt("0x" + strngrd1->Cells[m_GridCol][2] + Tmp) >= pow(2, StrToInt(StrTmp)))
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
        String HexData = strngrd1->Cells[i][2];
        HexData = HexData.Trim();

        if(HexData.IsEmpty())
        {
            HexData = "0";
        }

        HexData = "0x"+ HexData.Trim();
        NodeDataList.push_back(StrToInt(HexData));
    }

    // set every node data in the talbe.
    m_TableCtrl.SetNodesData(NodeDataList);



    edt2->Text = "";

    m_TableCtrl.GetReversedData(DataList);

    for(DWORD i = 0; i < DataList.size(); i++)
    {
        edt2->Text = edt2->Text + IntToStr((__int64)DataList[i]);
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
        strngrd1->Cells[i][2] = IntToHex(__int64(FieldData[i - 1]), 1);
        //String HexData
    }

    // set every node data in the table.
    m_TableCtrl.SetNodesData(FieldData);

    vector<DWORD> DataList;

    edt2->Text = "";

    m_TableCtrl.GetReversedData(DataList);

    for(DWORD i = 0; i < DataList.size(); i++)
    {
        edt2->Text = edt2->Text + IntToStr((__int64)DataList[i]);
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

