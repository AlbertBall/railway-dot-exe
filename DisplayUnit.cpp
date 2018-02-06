//DisplayUnit.cpp  (No own functions called so no call logs)
//BEWARE OF COMMENTS in .cpp files:  they were accurate when written but have
// sometimes been overtaken by changes and not updated
//Comments in .h files are believed to be accurate and up to date
/*
This is a source code file for "railway.exe", a railway operation
simulator, written in Borland C++ Builder 4 Professional
Copyright (C) 2010 Albert Ball [original development]

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Graphics.hpp>
#include <ComCtrls.hpp>
#include <fstream>
#include <vector>
#include <vcl.h>

#pragma hdrstop

#include "DisplayUnit.h"
#include "GraphicUnit.h"
#include "Utilities.h"
#include <fstream>

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
TDisplay *Display;
TDisplay *HiddenDisplay;


int TDisplay::DisplayOffsetH = 0; //have to be initialised outside the class, these lines aren't called
int TDisplay::DisplayOffsetV = 0;//in the normal way, the members are presumably intialised by the compiler
int TDisplay::DisplayOffsetHHome = 0;//without being executed
int TDisplay::DisplayOffsetVHome = 0;
int TDisplay::DisplayZoomOutOffsetH = 0;
int TDisplay::DisplayZoomOutOffsetV = 0;
int TDisplay::DisplayZoomOutOffsetHHome = 0;
int TDisplay::DisplayZoomOutOffsetVHome = 0;


//---------------------------------------------------------------------------

TDisplay::TDisplay(TImage* &Image, TMemo* &MemoBox, TLabel* &L1,  TLabel* &L2,  TLabel* &L3, TLabel* &L4, TLabel* &L5, TLabel* &L6,
        TLabel* &L7,  TLabel* &L8, TLabel* &L9, TLabel* &L10) : Output(Image), PerformanceMemo(MemoBox), OutputLog1(L1), OutputLog2(L2),
        OutputLog3(L3), OutputLog4(L4), OutputLog5(L5), OutputLog6(L6), OutputLog7(L7), OutputLog8(L8), OutputLog9(L9), OutputLog10(L10)
{
//ensure the font type supports the size chosen or will default to nearest
//arial 7pt is clear & fits between tracks, apart from bottom of descenders;
//Lydian 7pt fits in 8 pixels high & isn't bad for small font
//Kartika 10pt is clear & fits in 8 pixels (height 12.5)
Output->Canvas->Font->Style.Clear();
Output->Canvas->Font->Name = "MS Sans Serif";
Output->Canvas->Font->Size = 10;
Output->Canvas->Font->Color = clB0G0R0;
Output->Canvas->Font->Charset = (TFontCharset)(0);
ZoomOutFlag = false;
}

//---------------------------------------------------------------------------

void TDisplay::PlotOutput(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem)
{
if(Display->ZoomOutFlag) return;
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotOutput," + AnsiString(HPos) + "," + AnsiString(VPos));
Output->Canvas->Draw(HPos - (DisplayOffsetH * 16), VPos - (DisplayOffsetV * 16), PlotItem);
//Update(); dropped as many operations too slow
Utilities->CallLogPop(1461);
}

//---------------------------------------------------------------------------

void TDisplay::PlotSmallOutput(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotSmallOutput," + AnsiString(HPos) + "," + AnsiString(VPos));
Output->Canvas->Draw(HPos - (DisplayZoomOutOffsetH * 4), VPos - (DisplayZoomOutOffsetV * 4), PlotItem);
//Update();  too slow with this
Utilities->CallLogPop(1462);
}

//---------------------------------------------------------------------------

void TDisplay::Ellipse(int Caller, int HPos, int VPos, TColor Col)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",Ellipse," + AnsiString(HPos) + "," + AnsiString(VPos) + "," + AnsiString(Col));
TBrush *TempBrush = Output->Canvas->Brush;
Output->Canvas->Brush->Style = bsClear;
Output->Canvas->Pen->Style = psSolid;
Output->Canvas->Pen->Width = 2;
Output->Canvas->Pen->Color = Col;
Output->Canvas->Ellipse(HPos + 4 - (DisplayOffsetH * 16), VPos + 4 - (DisplayOffsetV * 16),
        HPos + 12 - (DisplayOffsetH * 16), VPos + 12 - (DisplayOffsetV * 16));
Output->Canvas->Brush = TempBrush;
Update();
Utilities->CallLogPop(1463);
}

//---------------------------------------------------------------------------

void TDisplay::InvertElement(int Caller, int HPos, int VPos)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",InvertElement," + AnsiString(HPos) + "," + AnsiString(VPos));
Output->Canvas->CopyMode = cmDstInvert;//invert image
PlotOutput(68, HPos, VPos, RailGraphics->bmSolidBgnd);
Output->Canvas->CopyMode = cmSrcCopy; //restore
Update();
Utilities->CallLogPop(1464);
}

//---------------------------------------------------------------------------

void TDisplay::Rectangle(int Caller, int HPos, int VPos, TColor Col, int Size, int Width)//Size 0,2,4,6 for large to small
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",Rectangle," + AnsiString(HPos) + "," + AnsiString(VPos) + "," + AnsiString(Col) + "," + AnsiString(Size));
TBrush *TempBrush = Output->Canvas->Brush;
Output->Canvas->Brush->Style = bsClear;
Output->Canvas->Pen->Style = psSolid;
Output->Canvas->Pen->Width = Width;
Output->Canvas->Pen->Color = Col;
Output->Canvas->Rectangle(HPos + Size - (DisplayOffsetH * 16), VPos + Size - (DisplayOffsetV * 16),
        HPos + 16 - Size - (DisplayOffsetH * 16), VPos + 16 - Size - (DisplayOffsetV * 16));
Output->Canvas->Brush = TempBrush;
Update();
Utilities->CallLogPop(1465);
}

//---------------------------------------------------------------------------

void TDisplay::Clear(int Caller, TRect Rect)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",Clear," + AnsiString(Rect.left) + "," + AnsiString(Rect.top));
Output->Canvas->Brush->Color = Utilities->clTransparent;
Output->Canvas->FillRect(Rect);
Utilities->CallLogPop(1466);
}

//---------------------------------------------------------------------------

void TDisplay::ClearDisplay(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ClearDisplay");
Output->Canvas->Brush->Color = Utilities->clTransparent;
Output->Canvas->FillRect(Output->ClientRect);
Utilities->CallLogPop(1467);
}

//---------------------------------------------------------------------------

void TDisplay::TextOut(int Caller, int HPos, int VPos, AnsiString TextString, TFont *Font)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextOut," + AnsiString(HPos) + "," + AnsiString(VPos) + "," + TextString + "," + Font->Name);
if((Font->Color < TColor(0)) || (Font->Color > TColor(0xFFFFFF))) Font->Color = clB0G0R0;//set to black for any of special windows colours
TFont *TempInputFont = new TFont;//don't alter the original font or won't print black on images
TFont *TempCanvasFont = new TFont;//store Output->Canvas font to put back later
TempCanvasFont->Assign(Output->Canvas->Font);
TempInputFont->Assign(Font);
if(Utilities->clTransparent != clB5G5R5)//dark background
    {
    if(TempInputFont->Color == clB0G0R0) TempInputFont->Color = clB5G5R5;//if font was black set it to white for for dark backgrounds
    }
Output->Canvas->Font->Assign(TempInputFont);
Output->Canvas->TextOut(HPos - (DisplayOffsetH * 16), VPos - (DisplayOffsetV * 16), TextString);
Output->Canvas->Font->Assign(TempCanvasFont);//restore original
delete TempInputFont;
delete TempCanvasFont;
Utilities->CallLogPop(1469);
}

//---------------------------------------------------------------------------

void TDisplay::GetRectangle(int Caller, TRect DestRect, TRect SourceRect, Graphics::TBitmap* &OriginalGraphic)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetRectangle," + AnsiString(DestRect.left) + "," + AnsiString(DestRect.top) + "," + AnsiString(SourceRect.left) + "," + AnsiString(SourceRect.top));
OriginalGraphic->Canvas->CopyRect(DestRect, Display->Output->Canvas, SourceRect);
Utilities->CallLogPop(1470);
}

//---------------------------------------------------------------------------

void TDisplay::PlotBlank(int Caller, int HLoc, int VLoc)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotBlank," + AnsiString(HLoc) + "," + AnsiString(VLoc));
Output->Canvas->Draw((HLoc - DisplayOffsetH) * 16, (VLoc - DisplayOffsetV) * 16, RailGraphics->bmSolidBgnd);
//Update();
Utilities->CallLogPop(1471);
}

//---------------------------------------------------------------------------

void TDisplay::PlotPointBlank(int Caller, int HLoc, int VLoc)
{
if(Display->ZoomOutFlag) return;
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotPointBlank," + AnsiString(HLoc) + "," + AnsiString(VLoc));
Output->Canvas->Draw(((HLoc - DisplayOffsetH) * 16) + 3, ((VLoc - DisplayOffsetV) * 16) + 3, RailGraphics->bmPointBlank);
//Update();
Utilities->CallLogPop(1473);
}

//---------------------------------------------------------------------------

void TDisplay::PlotSignalBlank(int Caller, int HLoc, int VLoc, int SpeedTag)
{
/*
straight signals:
N	16x7  hoff = 0, voff = 0	68
S	16x7  hoff = 0, voff = 9	69
W	7x16  hoff = 0, voff = 0	70
E	7x16  hoff = 9, voff = 0	71
diagonal signals
SW	11x11  hoff = 0, voff = 5	72
NW	11x11  hoff = 0, voff = 0	73
SE	11x11  hoff = 5, voff = 5	74
NE	11x11  hoff = 5, voff = 0	75
*/

if(Display->ZoomOutFlag) return;
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotSignalBlank," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," + AnsiString(SpeedTag));
if((SpeedTag > 75) || (SpeedTag < 68))
    {
    throw Exception("Error, not a signal in PlotSignalBlank");
    }

/*
SpeedTag    HOffset VOffset Graphic  Direction
68              0       0     NS       W->E        graphic is for the signal north or south of the track
69              0       9     NS       E->W
70              0       0     EW       S->N
71              9       0     EW       N->S
72              0       5    Diag     SE->NW
73              0       0    Diag     SW->NE
74              5       5    Diag     NE->SW
75              5       0    Diag     NW->SE
*/
int HOffset = 0;
if(SpeedTag > 73) HOffset = 5;
else if(SpeedTag == 71) HOffset = 9;
int VOffset = 0;
if(SpeedTag == 69) VOffset = 9;
else if(SpeedTag == 72) VOffset = 5;
else if(SpeedTag == 74) VOffset = 5;
Graphics::TBitmap *GraphicPtr;
if(SpeedTag > 71) GraphicPtr = RailGraphics->bmDiagonalSignalBlank;
else if(SpeedTag < 70) GraphicPtr = RailGraphics->bmStraightNSSignalBlank;
else GraphicPtr = RailGraphics->bmStraightEWSignalBlank;
Output->Canvas->Draw(((HLoc - DisplayOffsetH) * 16) + HOffset, ((VLoc - DisplayOffsetV) * 16) + VOffset, GraphicPtr);
//Update();
Utilities->CallLogPop(1475);
}

//---------------------------------------------------------------------------

void TDisplay::PlotSignalBlankOnBitmap(int HLoc, int VLoc, int SpeedTag, Graphics::TBitmap *Bitmap)
{
/*
straight signals:
N	16x7  hoff = 0, voff = 0	68
S	16x7  hoff = 0, voff = 9	69
W	7x16  hoff = 0, voff = 0	70
E	7x16  hoff = 9, voff = 0	71
diagonal signals
SW	11x11  hoff = 0, voff = 5	72
NW	11x11  hoff = 0, voff = 0	73
SE	11x11  hoff = 5, voff = 5	74
NE	11x11  hoff = 5, voff = 0	75
*/

Utilities->CallLog.push_back(Utilities->TimeStamp() + ",PlotSignalBlankOnBitmap," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," + AnsiString(SpeedTag));
if((SpeedTag > 75) || (SpeedTag < 68))
    {
    throw Exception("Error, not a signal in PlotSignalBlankOnBitmap");
    }

/*
SpeedTag    HOffset VOffset Graphic  Direction
68              0       0     NS       W->E        graphic is for the signal north or south of the track
69              0       9     NS       E->W
70              0       0     EW       S->N
71              9       0     EW       N->S
72              0       5    Diag     SE->NW
73              0       0    Diag     SW->NE
74              5       5    Diag     NE->SW
75              5       0    Diag     NW->SE
*/
int HOffset = 0;
if(SpeedTag > 73) HOffset = 5;
else if(SpeedTag == 71) HOffset = 9;
int VOffset = 0;
if(SpeedTag == 69) VOffset = 9;
else if(SpeedTag == 72) VOffset = 5;
else if(SpeedTag == 74) VOffset = 5;
Graphics::TBitmap *GraphicPtr;
if(SpeedTag > 71) GraphicPtr = RailGraphics->bmDiagonalSignalBlank;
else if(SpeedTag < 70) GraphicPtr = RailGraphics->bmStraightNSSignalBlank;
else GraphicPtr = RailGraphics->bmStraightEWSignalBlank;
Bitmap->Canvas->Draw(((HLoc - DisplayOffsetH) * 16) + HOffset, ((VLoc - DisplayOffsetV) * 16) + VOffset, GraphicPtr);
//Update();
Utilities->CallLogPop(1870);
}

//---------------------------------------------------------------------------

void TDisplay::PlotAbsolute(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotAbsolute," + AnsiString(HPos) + "," + AnsiString(VPos));
Output->Canvas->Draw(HPos, VPos, PlotItem);
//Update();
Utilities->CallLogPop(1477);
}

//---------------------------------------------------------------------------

void TDisplay::PlotDashedRect(int Caller, TRect Rect)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotDashedRect," + AnsiString(Rect.left) + "," + AnsiString(Rect.top));
Output->Canvas->Pen->Style = psDot;
Output->Canvas->Pen->Width = 1;
if(Utilities->clTransparent == clB5G5R5) Output->Canvas->Pen->Color = clB0G0R0;//black
else Output->Canvas->Pen->Color = clB5G5R5;//white
Output->Canvas->Brush->Style = bsClear;
Output->Canvas->Rectangle((Rect.left - DisplayOffsetH) * 16, (Rect.top - DisplayOffsetV) * 16,
        (Rect.right - DisplayOffsetH) * 16, (Rect.bottom - DisplayOffsetV) * 16);
//Update();
Utilities->CallLogPop(1478);
}

//---------------------------------------------------------------------------

void TDisplay::PerformanceLog(int Caller, AnsiString Statement)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PerformanceLog," + Statement);
Utilities->PerformanceFile << Statement.c_str() << '\n';
PerformanceMemo->Lines->Add(Statement);
Utilities->CallLogPop(1479);
}

//---------------------------------------------------------------------------

void TDisplay::WarningLog(int Caller, AnsiString Statement)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WarningLog," + Statement);
OutputLog6->Caption = OutputLog7->Caption;
OutputLog7->Caption = OutputLog8->Caption;
OutputLog8->Caption = OutputLog9->Caption;
OutputLog9->Caption = OutputLog10->Caption;
OutputLog10->Caption = OutputLog1->Caption;
OutputLog1->Caption = OutputLog2->Caption;
OutputLog2->Caption = OutputLog3->Caption;
OutputLog3->Caption = OutputLog4->Caption;
OutputLog4->Caption = OutputLog5->Caption;
OutputLog5->Caption = Statement;
Utilities->CallLogPop(1785);
}

//---------------------------------------------------------------------------

void TDisplay::HideWarningLog(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",HideWarningLog");
OutputLog6->Visible = false;
OutputLog7->Visible = false;
OutputLog8->Visible = false;
OutputLog9->Visible = false;
OutputLog10->Visible = false;
OutputLog1->Visible = false;
OutputLog2->Visible = false;
OutputLog3->Visible = false;
OutputLog4->Visible = false;
OutputLog5->Visible = false;
Utilities->CallLogPop(1873);
}

//---------------------------------------------------------------------------

void TDisplay::ShowWarningLog(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ShowWarningLog");
OutputLog6->Visible = true;
OutputLog7->Visible = true;
OutputLog8->Visible = true;
OutputLog9->Visible = true;
OutputLog10->Visible = true;
OutputLog1->Visible = true;
OutputLog2->Visible = true;
OutputLog3->Visible = true;
OutputLog4->Visible = true;
OutputLog5->Visible = true;
Utilities->CallLogPop(1874);
}

//---------------------------------------------------------------------------

