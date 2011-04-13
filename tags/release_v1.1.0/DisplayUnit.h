//DisplayUnit.h
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
#ifndef DisplayUnitH
#define DisplayUnitH
//---------------------------------------------------------------------------
#include <vcl.h>

class TDisplay //class that manages all aspects of the display.  There are 2 objects - Display, which is the screen, and HiddenDisplay,
    //which is an internal object used when building a new display and transferred to Display when complete (avoids the flicker that would
    //be visible if Display was built directly
{
private:

TImage* &Output; //pointer to the 60 x 36 element railway display area
TLabel* &OutputLog1; //pointers to the warning message logs (appear above the railway display during operation)
TLabel* &OutputLog2;
TLabel* &OutputLog3;
TLabel* &OutputLog4;
TLabel* &OutputLog5;
TLabel* &OutputLog6;
TLabel* &OutputLog7;
TLabel* &OutputLog8;
TLabel* &OutputLog9;
TLabel* &OutputLog10;
TMemo* &PerformanceMemo; //pointer to the panel that displays the performance log

public:

bool ZoomOutFlag; //true when zoomed-out

//the following offset values relate horizontal and vertical positions on the display area to the positions on the railway as a whole
//as follows:-  overall railway position = display position + offset.  They represent track elements, so to obtain pixel positions the
//element values must be multiplied by 16.  They are static so they are the same for both Display and HiddenDisplay

static int DisplayOffsetH; //the horizontal offset of the displayed screen
static int DisplayOffsetV; //the vertical offset of the displayed screen
static int DisplayOffsetHHome; //the horizontal offset of the 'Home' display
static int DisplayOffsetVHome; //the vertical offset of the 'Home' display
static int DisplayZoomOutOffsetH; //the horizontal offset of the zoomed-out display
static int DisplayZoomOutOffsetV; //the verticalal offset of the zoomed-out display
static int DisplayZoomOutOffsetHHome; //the horizontal offset of the zoomed-out 'Home' display
static int DisplayZoomOutOffsetVHome; //the vertical offset of the zoomed-out 'Home' display

//inline functions
AnsiString GetFontName() {return Output->Canvas->Font->Name;} //return the name of the default screen font

int GetFontSize() {return Output->Canvas->Font->Size;} //return the size of the default screen font
int Height() {return Output->Height;} //return the height of the screen
int Left() {return Output->Left;} //return the left pixel position of the screen
int Top() {return Output->Top;} //return the top pixel position of the screen
int Width() {return Output->Width;} //return the width of the screen

TFont* GetFont() {return Output->Canvas->Font;} //return the current screen font

TImage *GetImage() {return Output;} //return a pointer to the screen image

TLabel* GetOutputLog1() {return OutputLog1;} //return pointers to warning message logs (appear above the railway display during operation)
TLabel* GetOutputLog10() {return OutputLog10;}
TLabel* GetOutputLog2() {return OutputLog2;}
TLabel* GetOutputLog3() {return OutputLog3;}
TLabel* GetOutputLog4() {return OutputLog4;}
TLabel* GetOutputLog5() {return OutputLog5;}
TLabel* GetOutputLog6() {return OutputLog6;}
TLabel* GetOutputLog7() {return OutputLog7;}
TLabel* GetOutputLog8() {return OutputLog8;}
TLabel* GetOutputLog9() {return OutputLog9;}

void Cursor(TCursor Cur) {Output->Cursor = Cur;} //set the screen cursor to 'Cur' (used to select an arrow or an hourglass)
void ResetZoomInOffsets() {DisplayOffsetH = DisplayOffsetHHome; DisplayOffsetV = DisplayOffsetVHome;} //reset the zoomed-in screen display
    //to the 'Home' position
void ResetZoomOutOffsets() {DisplayZoomOutOffsetH = DisplayZoomOutOffsetHHome; DisplayZoomOutOffsetV = DisplayZoomOutOffsetVHome;} //reset
    //the zoomed-in screen display to the 'Home' position
void SetFont(TFont* Font) {Output->Canvas->Font->Assign(Font);} //set the screen font to 'Font'
void Update() {Output->Update();} //repaint the screen display

//functions defined in .cpp file
TDisplay::TDisplay(TImage* &Image, TMemo* &MemoBox, TLabel* &L1,  TLabel* &L2,  TLabel* &L3, TLabel* &L4, TLabel* &L5, TLabel* &L6,
    TLabel* &L7,  TLabel* &L8, TLabel* &L9, TLabel* &L10); //constructor, sets the screen image (MainScreen or HiddenScreen), the
    //performance log panel and the warning message labels
void Clear(int Caller, TRect Rect); //empty the rectangle defined by Rect
void ClearDisplay(int Caller); //empty the display
void Ellipse(int Caller, int HPos, int VPos, TColor Col); //plot an ellipse at the defined position and with the defined colour
void GetRectangle(int Caller, TRect DestRect, TRect SourceRect, Graphics::TBitmap *&OriginalGraphic); //used in
    //TGraphicElement::LoadOriginalScreenGraphic in TrackUnit to obtain OriginalGraphic from an area of the screen defined by SourceRect
void HideWarningLog(int Caller); //hide all the warnings from the top part of the screen - for timetable clock adjustment
void InvertElement(int Caller, int HPos, int VPos); //display the track element at HPos & VPos inverted (used to show an offending element
    //when trying to connect track
void PerformanceLog(int Caller, AnsiString Statement); //send Statement to the performance log on screen and to the file
void PlotAbsolute(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem); //plot the graphic at the railway (not screen) position
    //set by HPos & VPos
void PlotBlank(int Caller, int HLoc, int VLoc); //plot a blank track element at HLoc & VLoc
void PlotDashedRect(int Caller, TRect Rect); //plot a dashed rectangle for the area defined by Rect, used when selecting display areas
void PlotOutput(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem); //plot the graphic at screen position HPos & VPos
void PlotPointBlank(int Caller, int HLoc, int VLoc); //plot a small blank rectangle in the centre of a set of points at HLoc & VLoc
    //prior to plotting one or both fillets (the movable section)
void PlotSignalBlank(int Caller, int HLoc, int VLoc, int SpeedTag); //plot a small blank rectangle over the signal aspect area at HLoc &
    //VLoc prior to plotting the current signal aspect
void PlotSignalBlankOnBitmap(int HLoc, int VLoc, int SpeedTag, Graphics::TBitmap *Bitmap); //as PlotSignalBlank but plot on the Bitmap that is
    //supplied - for writing operating images to a bitmap file
void PlotSmallOutput(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem); //plot small (4x4) graphic PlotItem on the zoomed-out
    //display at HPos & Vpos
void Rectangle(int Caller, int HPos, int VPos, TColor Col, int Size, int Width); //plot a rectangle at the defined position with colour
    //Col & size defined by Size
void ShowWarningLog(int Caller); //show the warnings after timetable clock adjusted
void TextOut(int Caller, int HPos, int VPos, AnsiString TextString, TFont *Font); //display TextString at the defined position with the
    //defined font
void WarningLog(int Caller, AnsiString Statement); //display warning message Statement in the bottom left hand warning position and scroll
    //other messages up
};

//---------------------------------------------------------------------------

extern TDisplay *Display; //the object pointer for the on-screen display, object created in InterfaceUnit
extern TDisplay *HiddenDisplay; //the object pointer for the internal hidden display, object created in InterfaceUnit

//---------------------------------------------------------------------------

#endif

