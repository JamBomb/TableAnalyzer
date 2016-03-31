//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <math>
#include "AboutForm.h"
#include "stdlib.h"
#include "mmsystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutForm1 *AboutForm1;
//---------------------------------------------------------------------------
__fastcall TAboutForm1::TAboutForm1(TComponent* Owner)
    : TForm(Owner)
{

}
//---------------------------------------------------------------------------
__fastcall TAboutForm1::~TAboutForm1()
{

}

void __fastcall TAboutForm1::Init()
{
    m_mouseX = 0;
    m_mouseY = 0;
    m_sleepTime = 1;
    isError = false;
    isInitialized = false;

    bits = 10000;
    bit_px = new double[bits];
    bit_py = new double[bits];
    bit_vx = new double[bits];
    bit_vy = new double[bits];
    bit_sx = new int[bits];
    bit_sy = new int[bits];
    bit_l = new int[bits];
    bit_f = new int[bits];
    bit_p = new int[bits];
    bit_c = new int[bits];
    ru = 50;
    rv = 50;
    dy=0;
    bmp = new Graphics::TBitmap;
    testbmp = new Graphics::TBitmap;
//testbmp->LoadFromFile("test.bmp");
    testbmp = img1->Picture->Bitmap;
    Width=500;
    Height=400;

    m_nAppX = Width;
    m_nAppY = Height;
    m_centerX = m_nAppX * 2;
    m_centerY = m_nAppY * 2;
    m_mouseX = m_centerX;
    m_mouseY = m_centerY;

    pixls = m_nAppX * m_nAppY;
    pixls2 = pixls - m_nAppX * 2;
    pix0 = new int[pixls];

    isRunning = false;

    bits = 10000;   // "para_bits"
    bit_max = 150;  // "para_max"
    ru = 50;        // "para_blendx"
    rv = 50;        // "para_blendy"
    for(int i = 0; i < pixls; i++)
        pix0[i] = 0xff000000;

    for(int j = 0; j < bits; j++)
        bit_f[j] = 0;

    bmp->Width = Width;
    bmp->Height = Height;
    bmp->PixelFormat= pf24bit;

    bit_sound = 2;
    isInitialized = true;
//run();
}
void __fastcall TAboutForm1::run()
{
    do
    {
        for(int j = 0; j < pixls2; j++)
        {
            int k = pix0[j];
            int l = pix0[j + 1];
            int i1 = pix0[j + m_nAppX];
            int j1 = pix0[j + m_nAppX + 1];

            int i = (k & 0xff0000) >> 16;
            int k1 = ((((l & 0xff0000) >> 16) - i) * ru >> 8) + i;
            i = (k & 0xff00) >> 8;
            int l1 = ((((l & 0xff00) >> 8) - i) * ru >> 8) + i;
            i = k & 0xff;
            int i2 = (((l & 0xff) - i) * ru >> 8) + i;
            i = (i1 & 0xff0000) >> 16;
            int j2 = ((((j1 & 0xff0000) >> 16) - i) * ru >> 8) + i;
            i = (i1 & 0xff00) >> 8;
            int k2 = ((((j1 & 0xff00) >> 8) - i) * ru >> 8) + i;
            i = i1 & 0xff;

            int l2 = (((j1 & 0xff) - i) * ru >> 8) + i;
            int i3 = ((j2 - k1) * rv >> 8) + k1;
            int j3 = ((k2 - l1) * rv >> 8) + l1;
            int k3 = ((l2 - i2) * rv >> 8) + i2;
            pix0[j] = i3 << 16 | j3 << 8 | k3 | 0xff000000;
        }

        rend();
        int pix;
        for(int y = 0; y < bmp->Height; y++)
        {
            BYTE * ptr = (BYTE *)bmp->ScanLine[y];
            BYTE * ptr1= (BYTE *)testbmp->ScanLine[y+dy];
            for (int x=0; x < bmp->Width; x++)
            {
                pix = bit_get(x,y);

                ptr[x*3] = GetRValue(pix) | ptr1[x*3];
                ptr[x*3+1] = GetGValue(pix) | ptr1[x*3+1];
                ptr[x*3+2] = GetBValue(pix) | ptr1[x*3+2];
            }
        }
        dy=(dy+1)%Height;
        Canvas->CopyRect(ClientRect, bmp->Canvas, ClientRect);
        Sleep(20);
        Application->ProcessMessages();
    }
    while(isStopped);
}


void __fastcall TAboutForm1::bit_set(int i, int j, int k)
{
    int l = i + j * m_nAppX;
    pix0[l] = k;
}

int __fastcall TAboutForm1::bit_get(int i, int j)
{
    int l = i + j * m_nAppX;
    return pix0[l] ;
}
void __fastcall TAboutForm1::PlayFire(int X, int Y)
{
    m_mouseX = X;
    m_mouseY = Y;

    int k = (int)(randor() * 256.);

    int l = (int)(randor() * 256.);

    int i1 = (int)(randor() * 256.);
    int j1 = k << 16 | l << 8 | i1 | 0xff000000;
    int k1 = 0;

    for(int l1 = 0; l1 < bits; l1++)
    {
        if(bit_f[l1] != 0)
            continue;
        bit_px[l1] = m_mouseX;
        bit_py[l1] = m_mouseY;
        double d = randor() * 6.2800000000000002;
        double d1 = randor();
        bit_vx[l1] = sin(d) * d1;
        bit_vy[l1] = cos(d) * d1;
        bit_l[l1] = (int)(randor() * 100.) + 100;

        bit_p[l1] = (int)(randor() * 3.);
        bit_c[l1] = j1;
        bit_sx[l1] = m_mouseX;
        bit_sy[l1] = m_nAppY - 5;
        bit_f[l1] = 2;
        if(++k1 == bit_max)
            break;
    }
    if(bit_sound > 1)
    {
        //PlaySound("syu.wav",0,SND_ASYNC |SND_FILENAME);
        // PlaySound("SYU",HInstance, SND_ASYNC | SND_RESOURCE);
    }
}

void __fastcall TAboutForm1::FormMouseDown(TObject *Sender, TMouseButton Button,
        TShiftState Shift, int X, int Y)
{
    isStopped = false;
    tmrAbout->Enabled = false;

    delete bit_px;
    delete bit_py;
    delete bit_vx;
    delete bit_vy;
    delete bit_sx;
    delete bit_sy;
    delete bit_l;
    delete bit_f;
    delete bit_p;
    delete bit_c;
    delete bmp;
    delete[] pix0;
    Close();
    tmr1->Enabled = false;
}
//---------------------------------------------------------------------------
void  __fastcall TAboutForm1::rend()
{
    bool flag2 = false;
    for(int k = 0; k < bits; k++)
    {
        switch(bit_f[k])
        {
            default:
                break;

            case 1: // '\001'

                bit_vy[k] += randor() / 50.;
                bit_px[k] += bit_vx[k];
                bit_py[k] += bit_vy[k];
                bit_l[k]--;
                if(bit_l[k] == 0 || bit_px[k] < 0.0 || bit_py[k] < 0.0 || bit_px[k] > (double)m_nAppX || bit_py[k] > (double)(m_nAppY - 3))
                {
                    bit_c[k] = 0xff000000;
                    bit_f[k] = 0;
                }
                else if(bit_p[k] == 0)
                {
                    if((int)(randor() * 2.) == 0)
                        bit_set((int)bit_px[k], (int)bit_py[k], -1);
                }
                else
                {
                    bit_set((int)bit_px[k], (int)bit_py[k], bit_c[k]);
                }
                break;

            case 2: // '\002'
                bit_sy[k] -= 5;
                if((double)bit_sy[k] <= bit_py[k])
                {
                    bit_f[k] = 1;
                    flag2 = true;
                }
                if((int)(randor() * 20.) == 0)
                {
                    int i = (int)(randor() * 2.);

                    int j = (int)(randor() * 5.);
                    bit_set(bit_sx[k] + i, bit_sy[k] + j, -1);
                }
                break;
        }
    }

    if(flag2 && bit_sound > 0)
    {
//   PlaySound("firework.wav",0,SND_ASYNC |SND_FILENAME);
//  PlaySound("FireWork",HInstance, SND_ASYNC | SND_RESOURCE);
    }
}

double __fastcall TAboutForm1::randor()
{
    return (double)rand()/(double) RAND_MAX;

}

void __fastcall TAboutForm1::tmrAboutTimer(TObject *Sender)
{
    srand((unsigned)time(NULL));
    DWORD dwRand = rand();
    PlayFire(dwRand%Width,33);
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm1::tmr1Timer(TObject *Sender)
{
//Init();
    if(isStopped == false && isRunning == false)
    {
        isStopped = true;
        run();
    }

}
//---------------------------------------------------------------------------

void __fastcall TAboutForm1::FormShow(TObject *Sender)
{
    Init();
    tmr1->Enabled = true;
    tmrAbout->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm1::img1Click(TObject *Sender)
{
    isStopped = false;
    tmrAbout->Enabled = false;

    delete bit_px;
    delete bit_py;
    delete bit_vx;
    delete bit_vy;
    delete bit_sx;
    delete bit_sy;
    delete bit_l;
    delete bit_f;
    delete bit_p;
    delete bit_c;
    delete bmp;
    delete[] pix0;
    Close();
    tmr1->Enabled = false;
}
//---------------------------------------------------------------------------

