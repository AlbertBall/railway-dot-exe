// TextUnit.h
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
#ifndef TextUnitH
#define TextUnitH
// ---------------------------------------------------------------------------
#include <vector>
#include <fstream>  //for file saving & loading
#include <vcl.h>

class TDisplay; // predeclaration

// ---------------------------------------------------------------------------
/// A single piece of text that can be displayed on the railway
class TTextItem
{
public:
/* should use private members but can't use properties with a stack AnsiString (a VCL class)
      and cumbersome to have Set... & Get... functions for every data member.  Don't seem able to have properties for
      non AnsiString members with non property AnsiString either.  If have an AnsiString* and declare it on the stack with 'new'
      also can't use a property with it, if do then get errors that sound ordinary enough, but can't enter text in either the
      text.h or .cpp files, so it seems to mess up the program altogether.  If try to use properties for others & AnsiString*
      as it is then the program compiles, but when try to load KingsCrossSignals.trk fails on loading 6th or 7th piece of text,
      not at the AnsiString value but at an earlier int value!  Hence leave as is.  Clearly properties & VCL classes don't mix!
*/
    AnsiString TextString;
///< the text string
    int HPos;
///< the horizontal position on the railway
    int VPos;
///< the vertical position on the railway
    TFont *Font;
///< the text font

    TTextItem(); // default constructor
    TTextItem(int H, int V, AnsiString T, TFont *&InputFont); // constructor, sets the values given
};

// ---------------------------------------------------------------------------
/// A single object that handles text management
class TTextHandler
{
public:

    typedef std::vector<TTextItem>TTextVector; // a vector that contains text items
    typedef std::vector<TTextItem>::iterator TTextVectorIterator;
    typedef std::vector<TFont*>TFontVector; // new at v2.3.0

    TTextVector TextVector, SelectTextVector; // TextVector contains all the railway's text items, SelectTextVector is used to store text items
    // that are captured during a screen segment selection
    TFontVector FontVector; // new at v2.3.0

// inline functions
    void SetNewHPos(int Caller, int TextItem, int NewHPos)
    {
        TextPtrAt(24, TextItem)->HPos = NewHPos;
    } // change the value of HPos to NewHPos for

// the text item at position TextItem in TextVector
    void SetNewVPos(int Caller, int TextItem, int NewVPos)
    {
        TextPtrAt(25, TextItem)->VPos = NewVPos;
    } // change the value of VPos to NewVPos for

// the text item at position TextItem in TextVector
    TTextHandler::~TTextHandler() // destructor
    {
        FontVector.clear();
        TextVector.clear();
    }

// functions defined in .cpp file
    bool CheckTextElementsInFile(int Caller, std::ifstream& VecFile);
///< check the validity of text items in VecFile prior to loading, return true for success
    bool FindText(int Caller, AnsiString Name, int &HPos, int &VPos);
///< look in TextVector for text item 'Name', and if found return true and return its position in &HPos and &VPos
    bool FontSame(int Caller, TFont *ExistingFont, TFont *InputFont);
///< checks colour, size, name, charset and style
    bool TextErase(int Caller, int HPosition, int VPosition, AnsiString TextToErase);
///< look for a text item in the vicinity of HPosInput & VPosInput & if TextToErase is null then erase any text that is found, else erase TextToErase
    bool TextFound(int Caller, int HPosInput, int VPosInput, AnsiString& Text);
///< look for a text item in the vicinity of HPosInput & VPosInput, return true if found & return the found text in the AnsiString
    int GetFontStyleAsInt(int Caller, TFont *InputFont);
///< retrieve the style of the font as a coded integer
    TTextItem *SelectTextPtrAt(int Caller, int At);
///< return the text item at position 'At' in SelectTextVector (carries out range checking)
    TTextItem *TextPtrAt(int Caller, int At);
///< return the text item at position 'At' in TextVector (carries out range checking)

    unsigned int SelectTextVectorSize(int Caller);
///< return the number of items in SelectTextVector
    unsigned int TextVectorSize(int Caller);
///< return the number of items in TextVector

    TFontStyles TTextHandler::SetFontStyleFromInt(int Caller, int Input);
///< used in loading from a file
    void EnterAndDisplayNewText(int Caller, TTextItem Text, int HPos, int VPos);
///< add Text to TextVector and display it on the screen
    void LoadText(int Caller, std::ifstream& VecFile);
///< load the railway's text from VecFile
    void RebuildFromTextVector(int Caller, TDisplay *Disp);
///< display all text items in TextVector on the screen
    void SaveText(int Caller, std::ofstream& VecFile);
///< save the railway's text to VecFile
    void TextClear(int Caller);
///< empties TextVector and sets all offsets back to zero if there is no active or inactive track
    void TextMove(int Caller, int HPosInput, int VPosInput, int &TextItem, int &TextMoveHPos, int &TextMoveVPos, bool &TextFoundFlag);
///< look for a text item in the vicinity of HPosInput & VPosInput & if found return its exact position in &TextMoveHPos & &TextMoveVPos, its
///< position in TextVector in TextItem, and set FoundFlag to true
    void TextVectorPush(int Caller, TTextItem Text);
///< push &Text onto TextVector & reset the size of the railway if necessary
    void TextVectorResetPosition(int Caller, int HOffset, int VOffset);
///< change the HPos & VPos values for all items in TextVector by the amount in HOffset and VOffset (unused)
    void WriteTextToImage(int Caller, Graphics::TBitmap *Bitmap);
///< write all items in TextVector to the railway image in 'Bitmap'
};

// ---------------------------------------------------------------------------

extern TTextHandler *TextHandler; // the object pointer, object created in InterfaceUnit

// ---------------------------------------------------------------------------
#endif
