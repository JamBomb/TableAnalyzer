//---------------------------------------------------------------------------

#ifndef AboutFormH
#define AboutFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ctime>
//---------------------------------------------------------------------------
class TAboutForm1 : public TForm
{
__published:    // IDE-managed Components
    TTimer *tmrAbout;
    TTimer *tmr1;
    TImage *img1;
    void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
                                  TShiftState Shift, int X, int Y);
    void __fastcall tmrAboutTimer(TObject *Sender);
    void __fastcall tmr1Timer(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall img1Click(TObject *Sender);
private:    // User declarations
    int m_nAppX;
    int m_nAppY;
    int m_centerX;
    int m_centerY;
    int m_mouseX;
    int m_mouseY;
    int m_sleepTime;
    bool isError;
    bool m_isPaintFinished;
    bool isRunning;
    bool isInitialized;
    bool isStopped;

    int* pix0;
    int pixls;
    int pixls2;

    int bits;
    double* bit_px;
    double* bit_py;
    double* bit_vx;
    double* bit_vy;
    int* bit_sx;
    int* bit_sy;
    int* bit_l;
    int* bit_f;
    int* bit_p;
    int* bit_c;
    int bit_max;
    int bit_sound;
    int ru;
    int rv;
    int dy;

    Graphics::TBitmap* bmp;
    Graphics::TBitmap* testbmp;
public:     // User declarations
    __fastcall TAboutForm1(TComponent* Owner);
    __fastcall virtual ~TAboutForm1();
    void  __fastcall Init();
    void __fastcall run();
    void  __fastcall rend();
    double __fastcall randor();
    void __fastcall bit_set(int i, int j, int k);
    int __fastcall bit_get(int i, int j);
    void __fastcall PlayFire(int X, int Y);
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutForm1 *AboutForm1;
//---------------------------------------------------------------------------
#endif
