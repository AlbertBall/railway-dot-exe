// DisplayUnit.h
/*
      Comments in .h files are believed to be accurate and up to date

      This is a source code file for "railway.exe", a railway operation
      simulator, written originally in Borland C++ Builder 4 Professional with
      later updates in Embarcadero C++Builder 10.2.
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
// ---------------------------------------------------------------------------
#ifndef DisplayUnitH
#define DisplayUnitH
// ---------------------------------------------------------------------------
#include <vcl.h>
#include <memory> //for smart pointer

class TUserGraphicItem
{
public:
    AnsiString FileName; // this file is in the Graphics folder
    int HPos, VPos;
    int Width, Height;
    TPicture *UserGraphic;
};

// ---------------------------------------------------------------------------
/// Class that manages all aspects of the display
///
/// There are 2 objects - Display, which is the screen, and HiddenDisplay,
/// which is an internal object used when building a new display and transferred
/// to Display when complete (avoids the flicker that would be visible if Display
/// was built directly)

class TDisplay
{
private:

    TImage* &Output;
///< pointer to the 60 x 36 element railway display area
    TLabel* &OutputLog1;
///< Pointers to the warning message logs (appear above the railway display during operation)
    TLabel* &OutputLog2;
    TLabel* &OutputLog3;
    TLabel* &OutputLog4;
    TLabel* &OutputLog5;
    TLabel* &OutputLog6;
    TLabel* &OutputLog7;
    TLabel* &OutputLog8;
    TLabel* &OutputLog9;
    TLabel* &OutputLog10;
    TMemo* &PerformanceMemo;
///< pointer to the panel that displays the performance log

public:

    bool ZoomOutFlag;
///< true when zoomed-out

// the following offset values relate horizontal and vertical positions on the display area to the positions on the railway as a whole
// as follows:-  overall railway position = display position + offset.  They represent track elements, so to obtain pixel positions the
// element values must be multiplied by 16.  They are static so they are the same for both Display and HiddenDisplay

    static int DisplayOffsetH;
///< the horizontal offset of the displayed screen
    static int DisplayOffsetV;
///< the vertical offset of the displayed screen
    static int DisplayOffsetHHome;
///< the horizontal offset of the 'Home' display
    static int DisplayOffsetVHome;
///< the vertical offset of the 'Home' display
    static int DisplayZoomOutOffsetH;
///< the horizontal offset of the zoomed-out display
    static int DisplayZoomOutOffsetV;
///< the verticalal offset of the zoomed-out display
    static int DisplayZoomOutOffsetHHome;
///< the horizontal offset of the zoomed-out 'Home' display
    static int DisplayZoomOutOffsetVHome;
///< the vertical offset of the zoomed-out 'Home' display

// inline functions

/// Return the name of the default screen font
    AnsiString GetFontName()
    {
        return(Output->Canvas->Font->Name);
    }

/// Return the size of the default screen font
    int GetFontSize()
    {
        return(Output->Canvas->Font->Size);
    }

/// Return the height of the screen
    int Height()
    {
        return(Output->Height);
    }

/// Return the left pixel position of the screen
    int Left()
    {
        return(Output->Left);
    }

/// Return the top pixel position of the screen
    int Top()
    {
        return(Output->Top);
    }

/// Return the width of the screen
    int Width()
    {
        return(Output->Width);
    }

/// Return the current screen font
    TFont* GetFont()
    {
        return(Output->Canvas->Font);
    }

/// Return a pointer to the screen image
    TImage *GetImage()
    {
        return(Output);
    }

    TLabel* GetOutputLog1()
    {
        return(OutputLog1);
    }

///< Return pointers to warning message logs (appear above the railway display during operation)
    TLabel* GetOutputLog2()
    {
        return(OutputLog2);
    }

    TLabel* GetOutputLog3()
    {
        return(OutputLog3);
    }

    TLabel* GetOutputLog4()
    {
        return(OutputLog4);
    }

    TLabel* GetOutputLog5()
    {
        return(OutputLog5);
    }

    TLabel* GetOutputLog6()
    {
        return(OutputLog6);
    }

    TLabel* GetOutputLog7()
    {
        return(OutputLog7);
    }

    TLabel* GetOutputLog8()
    {
        return(OutputLog8);
    }

    TLabel* GetOutputLog9()
    {
        return(OutputLog9);
    }

    TLabel* GetOutputLog10()
    {
        return(OutputLog10);
    }

/// Set the screen cursor to 'Cur' (used to select an arrow or an hourglass)
    void Cursor(TCursor Cur)
    {
        Output->Cursor = Cur;
    }

/// Reset the zoomed-in screen display to the 'Home' position
    void ResetZoomInOffsets()
    {
        DisplayOffsetH = DisplayOffsetHHome;
        DisplayOffsetV = DisplayOffsetVHome;
    }

/// Reset the zoomed-out screen display to the 'Home' position
    void ResetZoomOutOffsets()
    {
        DisplayZoomOutOffsetH = DisplayZoomOutOffsetHHome;
        DisplayZoomOutOffsetV = DisplayZoomOutOffsetVHome;
    }

/// Set the screen font to 'Font'
    void SetFont(TFont* Font)
    {
        Output->Canvas->Font->Assign(Font);
    }

/// Repaint the screen display
    void Update()
    {
        Output->Update();
    }

// functions defined in .cpp file

/// Constructor, sets the screen image (MainScreen or HiddenScreen), the
/// performance log panel and the warning message labels
    TDisplay::TDisplay(TImage* &Image, TMemo* &MemoBox, TLabel* &L1, TLabel* &L2, TLabel* &L3, TLabel* &L4, TLabel* &L5, TLabel* &L6, TLabel* &L7, TLabel* &L8,
                       TLabel* &L9, TLabel* &L10);
/// Empty the rectangle defined by Rect
    void Clear(int Caller, TRect Rect);
/// Empty the display
    void ClearDisplay(int Caller);
/// Plot an ellipse at the defined position and with the defined colour
    void Ellipse(int Caller, int HPos, int VPos, TColor Col);
/// Used in TGraphicElement::LoadOriginalScreenGraphic in TrackUnit to obtain OriginalGraphic from an area of the screen
/// defined by SourceRect
    void GetRectangle(int Caller, TRect DestRect, TRect SourceRect, Graphics::TBitmap *&OriginalGraphic);
/// Hide all the warnings from the top part of the screen - for timetable clock adjustment
    void HideWarningLog(int Caller);
/// Display the track element at HPos & VPos inverted (used to show an offending element
/// when trying to connect track
    void InvertElement(int Caller, int HPos, int VPos);
/// Send Statement to the performance log on screen and to the file
    void PerformanceLog(int Caller, AnsiString Statement);
/// Plot the graphic at the railway (not screen) position
/// set by HPos & VPos
    void PlotAbsolute(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem);
/// Plot a blank track element at HLoc & VLoc
    void PlotBlank(int Caller, int HLoc, int VLoc);
/// Plot a dashed rectangle for the area defined by Rect, used when selecting display areas
    void PlotDashedRect(int Caller, TRect Rect);
/// Plot the graphic at screen position HPos & VPos
    void PlotOutput(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem);
/// Plot user graphic
    void PlotAndAddUserGraphic(int Caller, TUserGraphicItem UserGraphicItem);
/// Plot a small blank rectangle in the centre of a set of points at HLoc & VLoc
/// prior to plotting one or both fillets (the movable section)
    void PlotPointBlank(int Caller, int HLoc, int VLoc);
/// Plot a small blank rectangle over the signal aspect area at HLoc & VLoc
/// prior to plotting the current signal aspect
    void PlotSignalBlank(int Caller, int HLoc, int VLoc, int SpeedTag, bool RHSFlag);
/// As PlotSignalBlank but plot on the Bitmap that is supplied
/// - for writing operating images to a bitmap file
    void PlotSignalBlankOnBitmap(int HLoc, int VLoc, int SpeedTag, Graphics::TBitmap *Bitmap, bool RHSFlag);
/// Plot small (4x4) graphic PlotItem on the zoomed-out display at HPos & Vpos
    void PlotSmallOutput(int Caller, int HPos, int VPos, Graphics::TBitmap *PlotItem);
/// Plot a rectangle at the defined position with colour Col & size defined by Size
    void Rectangle(int Caller, int HPos, int VPos, TColor Col, int Size, int Width);
/// Show the warnings after timetable clock adjusted
    void ShowWarningLog(int Caller);
/// Display TextString at the defined position with the defined font
    void TextOut(int Caller, int HPos, int VPos, AnsiString TextString, TFont *Font);
/// Display warning message Statement in the bottom left hand warning position and scroll other messages up
    void WarningLog(int Caller, AnsiString Statement);
};

// ---------------------------------------------------------------------------
/// The object pointer for the on-screen display, object created in InterfaceUnit
extern TDisplay *Display;
/// The object pointer for the internal hidden display, object created in InterfaceUnit
extern TDisplay *HiddenDisplay;

// ---------------------------------------------------------------------------

#endif
