//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "MPHexEditor.hpp"
#include "MPHexEditorEx.hpp"
#include "SUIButton.hpp"
#include "SUIGrid.hpp"
#include "SUISideChannel.hpp"
#include "SUITreeView.hpp"
#include "WinSkinData.hpp"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Registry.hpp>
#include "SUIStatusBar.hpp"

#include "source\Project\CProject.h"
#include "source\TableCtrl\CTableCtrl.h"
#include "CnHint.hpp"
#include "CnClasses.hpp"
#include "CnTrayIcon.hpp"
#include <Menus.hpp>


enum
{
    TABLEANALYSER_PROJECT_ICON_INDEX = 0,
    TABLEANALYSER_PROJECT_SELECTED_ICON_INDEX = 0,
    TABLEANALYSER_MODULE_ICON_INDEX = 1,
    TABLEANALYSER_MODULE_SELECTED_ICON_INDEX = 2,
    TABLEANALYSER_ITEM_ICON_INDEX = 3,
    TABLEANALYSER_ITEM_SELECTED_ICON_INDEX = 4,
};

//---------------------------------------------------------------------------
class TTableForm : public TForm
{
__published:    // IDE-managed Components
    TsuiSideChannel *sdchnl1;
    TsuiTreeView *tvTables;
    TPanel *pnl1;
    TSkinData *skndt1;
    TImageList *il1;
    TSplitter *spl1;
    TsuiStatusBar *stat1;
    TTabControl *tbc2;
    TsuiStringGrid *strngrd1;
    TToolBar *tlb1;
    TToolButton *btn6;
    TToolButton *btnAnalyseData;
    TEdit *edt2;
    TLabel *lbl11;
    TToolButton *btn7;
    TToolButton *btnBuildData;
    TToolButton *btn15;
    TMPHexEditorEx *mphxdtrx1;
    TToolButton *btn1;
    TToolButton *btn2;
    TToolButton *btnAbout;
    TCnHint *cnhnt1;
    TCnTrayIcon *ti1;
    TPopupMenu *pm1;
    TMenuItem *Show1;
    TMenuItem *Exit1;
    TMenuItem *N1;
    TMenuItem *About1;
    TMenuItem *N2;
    TPopupMenu *pm2;
    TMenuItem *paste1;
    TMenuItem *Copy1;
    TMenuItem *Cut1;
    TMenuItem *N3;
    TMenuItem *Redo1;
    TMenuItem *Undo1;
    void __fastcall tvTablesClick(TObject *Sender);
    void __fastcall btn6Click(TObject *Sender);
    void __fastcall strngrd1DrawCell(TObject *Sender, int ACol,
                                     int ARow, TRect &Rect, TGridDrawState State);
    void __fastcall strngrd1KeyPress(TObject *Sender, char &Key);
    void __fastcall strngrd1SelectCell(TObject *Sender, int ACol,
                                       int ARow, bool &CanSelect);
    void __fastcall strngrd1Exit(TObject *Sender);
    void __fastcall btnBuildDataClick(TObject *Sender);
    void __fastcall btnAnalyseDataClick(TObject *Sender);
    void __fastcall ti1Click(TObject *Sender, TMouseButton Button,
                             TShiftState Shift, int X, int Y);
    void __fastcall Show1Click(TObject *Sender);
    void __fastcall Exit1Click(TObject *Sender);
    void __fastcall btnAboutClick(TObject *Sender);
    void __fastcall paste1Click(TObject *Sender);
    void __fastcall Copy1Click(TObject *Sender);
    void __fastcall Cut1Click(TObject *Sender);
    void __fastcall Redo1Click(TObject *Sender);
    void __fastcall Undo1Click(TObject *Sender);
private:    // User declarations

    enum Data_Format
    {
        DECIMAL_DATA_TYPE = 0,
        HEXADECIMAL_DATA_TYPE,
        BINARY_DATA_TYPE,
        DATA_TYPE_COUNT,
    };

	Data_Format m_DataFormat;

    void __fastcall OnMinMaxSize(TMessage &Msg);

    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_SYSCOMMAND, TMessage, OnMinMaxSize)
    END_MESSAGE_MAP(TForm)

    void __fastcall InitTableProject(AnsiString ProjectFileName);
    bool __fastcall InitTableGrid(AnsiString TableFileName);

    bool __fastcall SetFieldDataToHexEditor(vector <BYTE> FieldData);

    vector <DWORD> __fastcall GetFieldDataFromHexEditor();

    TTreeNode *m_pSelectedTreeNode;
    int m_GridCol;


    CProject *m_pTableProject;
    CTableCtrl m_TableCtrl;
    String m_TableDirectory;

    bool m_bShowedApplication;

    bool m_bShowTrayHint;

    void __fastcall UpdateCtrlState(bool bEnable);

    void __fastcall StringToValue();

    void __fastcall ValueToString();

	void __fastcall ChangeDataFormat(Data_Format eDataFormat);


public:     // User declarations

    __fastcall TTableForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTableForm *TableForm;
//---------------------------------------------------------------------------
#endif
