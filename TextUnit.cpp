//TextUnit.cpp
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

#include "TextUnit.h"
#include "TrackUnit.h"
#include "DisplayUnit.h"
#include "Utilities.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------

int TTextItem::GetFontStyleAsInt(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetFontStyleAsInt");
    int Output = 0;
    if(Font->Style.Contains(fsBold)) Output = 1;
    if(Font->Style.Contains(fsItalic)) Output+= 2;
    if(Font->Style.Contains(fsUnderline)) Output+= 4;
    if(Font->Style.Contains(fsStrikeOut)) Output+= 8;
    Utilities->CallLogPop(1306);
    return Output;
}

//---------------------------------------------------------------------------

void TTextItem::SetFontStyleFromInt(int Caller, int Input)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetFontStyleAsInt" + AnsiString(Input));
    TFontStyles TempStyle;
    if(Input >= 8)
    {
        TempStyle << fsStrikeOut;
        Input -= 8;
    }
    if(Input >= 4)
    {
        TempStyle << fsUnderline;
        Input -= 4;
    }
    if(Input >= 2)
    {
        TempStyle << fsItalic;
        Input -= 2;
    }
    if(Input >= 1) TempStyle << fsBold;
    Font->Style = TempStyle; //Font->Style doesn't accept << values directly for some reason??
    Utilities->CallLogPop(1307);
}

//---------------------------------------------------------------------------

void TTextItem::DeleteTextItem(int Caller)
/*
Delete text heap object (font) explicitly by this special function rather than by a destructor, because vectors
erase elements during internal operations & if TTextItem had an explicit destructor that deleted the font then
it would be called when a vector element was erased.  Calling the default TTextItem destructor doesn't matter because all
that does is release the memory of the members (including the pointer to the font), it doesn't destroy the font itself.
It's important therefore to call this function before erasing the vector element, otherwise the pointer to the font
would be lost and the font never destroyed, thereby causing memory leaks.
*/
{
//    if(NoDelete) return;
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DeleteTextItem");
    if(Font == 0)
    {
        throw Exception("Error in attempting to delete Font");
    }
    delete Font;
    Font = 0;
    Utilities->CallLogPop(741);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

TTextHandler *TextHandler;

//---------------------------------------------------------------------------

void TTextHandler::EnterAndDisplayNewText(int Caller, TTextItem &Text, int HPos, int VPos)
{
    Text.TextString = Text.TextString.Trim(); //strip leading & trailing spaces and control characters
    if(Text.TextString == "")
    {
        return; //don't add null text to vector
    }
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EnterAndDisplayNewText," + Text.TextString + ',' + AnsiString(HPos) + "," + AnsiString(VPos));
    if(Text.TextString.Length() > 255) Text.TextString = Text.TextString.SubString(0, 255);  //limited because of char* buffer length on loading
    TextVectorPush(1, Text);
    Display->TextOut(0, HPos, VPos, Text.TextString, Text.Font);
    Utilities->CallLogPop(1308);
}
//---------------------------------------------------------------------------
void TTextHandler::TextMove(int Caller, int HPosInput, int VPosInput, int &TextItem,
                            int &TextMoveHPos, int &TextMoveVPos, bool &TextFoundFlag)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextMove," + AnsiString(HPosInput) + "," + AnsiString(VPosInput));
    TTextVectorIterator TextPtr;
    TextFoundFlag = false;
    if(!TextVector.empty())
    {
        int x = TextVector.size();
        for(TextPtr = (TextVector.end() - 1); TextPtr >= TextVector.begin(); TextPtr--)
        {
            x--;
            if((HPosInput >= (*TextPtr).HPos) &&
               (HPosInput < ((*TextPtr).HPos + abs((*TextPtr).Font->Height))) && (VPosInput >= (*TextPtr).VPos) &&
               (VPosInput < ((*TextPtr).VPos + abs((*TextPtr).Font->Height))))
            {
                TextItem = x;
                TextMoveHPos = (*TextPtr).HPos;
                TextMoveVPos = (*TextPtr).VPos;
                TextFoundFlag = true;
                Utilities->CallLogPop(1309);
                return;
            } //if ....
        } //for TextPtr...
    } //if !TextVector...
    Utilities->CallLogPop(1310);
}

//---------------------------------------------------------------------------

bool TTextHandler::TextFound(int Caller, int HPosInput, int VPosInput)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextFound," + AnsiString(HPosInput) + "," + AnsiString(VPosInput));
    TTextVectorIterator TextPtr;
    if(TextHandler->TextVectorSize(1) == 0)
    {
        Utilities->CallLogPop(1311);
        return false;
    }
    for(TextPtr = (TextHandler->TextVector.end() - 1); TextPtr >= TextHandler->TextVector.begin(); TextPtr--)
    {
        if((HPosInput >= (*TextPtr).HPos) &&
           (HPosInput < ((*TextPtr).HPos + abs((*TextPtr).Font->Height))) && (VPosInput >= (*TextPtr).VPos) &&
           (VPosInput < ((*TextPtr).VPos + abs((*TextPtr).Font->Height))))
        {
            Utilities->CallLogPop(1312);
            return true;
        }
    }
    Utilities->CallLogPop(1313);
    return false;
}

//---------------------------------------------------------------------------
bool TTextHandler::TextErase(int Caller, int HPosInput, int VPosInput)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextErase," + AnsiString(HPosInput) + "," + AnsiString(VPosInput));
    TTextVectorIterator TextPtr;
    if(!TextVector.empty())
    {
        for(TextPtr = (TextVector.end() - 1); TextPtr >= TextVector.begin(); TextPtr--)
        {
            if((HPosInput >= (*TextPtr).HPos) &&
               (HPosInput < ((*TextPtr).HPos + (*TextPtr).Font->Size)) &&
               (VPosInput >= (*TextPtr).VPos) &&
               (VPosInput < ((*TextPtr).VPos + ((*TextPtr).Font->Size)*1.5)))

            {
                TextPtr->DeleteTextItem(1);
                TextVector.erase(TextPtr);
                Track->CalcHLocMinEtc(5);
                Utilities->CallLogPop(1314);
                return true;
            } //if ....
        } //for TextPtr...
    }
    Utilities->CallLogPop(1315);
    return false;
}

//---------------------------------------------------------------------------

void TTextHandler::LoadOld(int Caller, std::ifstream& VecFile)
//VecFile already open and its pointer at right place on calling
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadOld");
    int TempInt;
    char *TempString = new char[256];
    char TempChar;
    while(true)
    {
        VecFile >> TempInt; //1 unless end of text
        if(TempInt == 999999) break;
        TTextItem *TempText = new TTextItem; //needs to persist as referenced by text vector
        VecFile >> TempInt; TempText->HPos = TempInt;
        VecFile >> TempInt; TempText->VPos = TempInt;
        VecFile.get(TempChar); //get rid of '\n' prior to loading string
        VecFile.getline(&TempString[0], 256); //'n' is in file as delimiter, but \0 goes into TempString
        TempText->TextString = TempString; //text string
        VecFile.getline(&TempString[0], 256); //Font name
        TempText->Font->Name = TempString;
        VecFile >> TempInt; //font size
        TempText->Font->Size = TempInt;
        VecFile >> TempInt; //font colour
        TempText->Font->Color = static_cast<TColor>(TempInt);
        VecFile >> TempInt; //font charset
        TempText->Font->Charset = (TFontCharset)TempInt;
        VecFile >> TempInt; //font style as integer
        TempText->SetFontStyleFromInt(0, TempInt);
        TextVectorPush(2, *TempText);
    }
    delete TempString;
    Utilities->CallLogPop(1316);
}

//---------------------------------------------------------------------------

void TTextHandler::LoadText(int Caller, std::ifstream& VecFile)
//VecFile already open and its pointer at right place on calling
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadText");
    int TempInt;
    AnsiString FontNameString = "";
    int NumberOfTextElements=0;
    NumberOfTextElements = Utilities->LoadFileInt(VecFile);
    for(int x=0; x<NumberOfTextElements; x++)
    {
        TTextItem *TempText = new TTextItem; //needs to persist as referenced by text vector
        VecFile >> TempInt; TempText->HPos = TempInt;
        VecFile >> TempInt; TempText->VPos = TempInt;

        TempText->TextString = Utilities->LoadFileString(VecFile);
        TempText->Font->Name = Utilities->LoadFileString(VecFile); //name
        VecFile >> TempInt; //font size
        TempText->Font->Size = TempInt;
        VecFile >> TempInt; //font colour
        TempText->Font->Color = static_cast<TColor>(TempInt);
        VecFile >> TempInt; //font charset
        TempText->Font->Charset = (TFontCharset)TempInt;
        VecFile >> TempInt; //font style as integer
        TempText->SetFontStyleFromInt(1, TempInt);
        TextVectorPush(3, *TempText);
    }
    Utilities->CallLogPop(1317);
}

//---------------------------------------------------------------------------

void TTextHandler::SaveText(int Caller, std::ofstream& VecFile)
//VecFile already open and its pointer at right place on calling
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveText");
    int NumberOfTextElements = TextVectorSize(2);
    Utilities->SaveFileInt(VecFile, NumberOfTextElements);
    for(unsigned int x=0; x<(TextVectorSize(3)); x++)
    {
        VecFile << TextPtrAt(8, x)->HPos << '\n';
        VecFile << TextPtrAt(9, x)->VPos << '\n';
        Utilities->SaveFileString(VecFile, TextPtrAt(10, x)->TextString);
        Utilities->SaveFileString(VecFile, TextPtrAt(11, x)->Font->Name);
        VecFile << TextPtrAt(12, x)->Font->Size << '\n';
        if((int(TextPtrAt(37, x)->Font->Color) < 0) || (int(TextPtrAt(38, x)->Font->Color) > 0xFFFFFF))
        { //if set to any of the special 'windows' colours save it as black
            VecFile << '0' << '\n';
        }
        else
        {
            VecFile << TextPtrAt(13, x)->Font->Color << '\n';
        }
        VecFile << (int)(TextPtrAt(14, x)->Font->Charset) << '\n'; //save as 'int' (would be unsigned char else) so 'n' can act as proper delimiter
        VecFile << TextPtrAt(15, x)->GetFontStyleAsInt(0) << '\n';
    }
    Utilities->CallLogPop(1318);
}

//---------------------------------------------------------------------------

bool TTextHandler::CheckTextElementsInFile(int Caller, std::ifstream& VecFile)
{
//VecFile already open and its pointer at right place on calling
//check text elements
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckTextElementsInFile");
    int TempInt;
//TTextItem TempText;
//char TempString[256];
//char TempChar;
    int NumberOfTextElements=0;
    NumberOfTextElements = Utilities->LoadFileInt(VecFile);
    for(int x=0; x<NumberOfTextElements; x++)
    {
        VecFile >> TempInt;
        if((TempInt < -16000000) || (TempInt > 16000000))
        {
            Utilities->CallLogPop(1319);
            return false; //HPos
        }
        VecFile >> TempInt;
        if((TempInt < -16000000) || (TempInt > 16000000))
        {
            Utilities->CallLogPop(1320);
            return false; //VPos
        }
        if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
        {
            Utilities->CallLogPop(1321);
            return false; //Text string
        }
        if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
        {
            Utilities->CallLogPop(1322);
            return false; //font name
        }
        VecFile >> TempInt; //font point size
        if((TempInt < 1) || (TempInt > 1000))
        {
            Utilities->CallLogPop(1323);
            return false;
        }
        VecFile >> TempInt; //font colour - no point checking as all possible values valid as colours
        VecFile >> TempInt; //font charset
        if((TempInt < 0) || (TempInt > 255))
        {
            Utilities->CallLogPop(1324);
            return false;
        }
        VecFile >> TempInt; //font style as integer
        if((TempInt < 0) || (TempInt > 15))
        {
            Utilities->CallLogPop(1325);
            return false;
        }
    }
    Utilities->CallLogPop(1326);
    return true;
}

//---------------------------------------------------------------------------

void TTextHandler::RebuildFromTextVector(int Caller, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RebuildFromTextVector");
    for(unsigned int x=0; x<(TextHandler->TextVectorSize(4)); x++)
    {
        int HPos = TextHandler->TextPtrAt(16, x)->HPos;
        int VPos = TextHandler->TextPtrAt(17, x)->VPos;
        AnsiString TextString = TextHandler->TextPtrAt(18, x)->TextString;
        TFont *TextFont = TextHandler->TextPtrAt(19, x)->Font;
        Disp->TextOut(1, HPos, VPos, TextString, TextFont); //colour changed to white if was black & dark background in TDisplay::TextOut
    }
    Disp->Update();
    Utilities->CallLogPop(1327);
}

//---------------------------------------------------------------------------

void TTextHandler::WriteTextToImage(int Caller, Graphics::TBitmap *Bitmap)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WriteTextToImage");
    for(unsigned int x=0; x<(TextHandler->TextVectorSize(11)); x++)
    {
        int HPos = TextHandler->TextPtrAt(30, x)->HPos;
        int VPos = TextHandler->TextPtrAt(31, x)->VPos;
        AnsiString TextString = TextHandler->TextPtrAt(32, x)->TextString;
        TFont *TextFont = TextHandler->TextPtrAt(33, x)->Font;
        Bitmap->Canvas->Font->Assign(TextFont);
        Bitmap->Canvas->TextOut(HPos - (Track->GetHLocMin() * 16), VPos - (Track->GetVLocMin() * 16), TextString);
    }
    Utilities->CallLogPop(1534);
}

//---------------------------------------------------------------------------

void TTextHandler::TextVectorResetPosition(int Caller, int HOffset, int VOffset)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextVectorResetPosition," + AnsiString(HOffset) + "," + AnsiString(VOffset));
    if(TextVectorSize(5) > 0)
    {
        for(unsigned int x=0; x<TextVectorSize(6); x++)
        {
            TextPtrAt(20, x)->HPos = TextPtrAt(21, x)->HPos - (HOffset * 16);
            TextPtrAt(22, x)->VPos = TextPtrAt(23, x)->VPos - (VOffset * 16);
        }
    }
    Utilities->CallLogPop(1328);
}

//---------------------------------------------------------------------------

void TTextHandler::TextClear(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextClear");
    TextVector.clear();
    if(Track->NoActiveOrInactiveTrack(10))
    {
        Display->DisplayOffsetH = 0;
        Display->DisplayOffsetV = 0;
        Display->DisplayOffsetHHome = 0;
        Display->DisplayOffsetVHome = 0;
        Display->DisplayZoomOutOffsetH = 0;
        Display->DisplayZoomOutOffsetV = 0;
        Display->DisplayZoomOutOffsetHHome = 0;
        Display->DisplayZoomOutOffsetVHome = 0;
        Track->SetHLocMin(2000000000);
        Track->SetHLocMax(-2000000000);
        Track->SetVLocMin(2000000000);
        Track->SetVLocMax(-2000000000);
    }
    else Track->CalcHLocMinEtc(6);
    Utilities->CallLogPop(1329);
}

//---------------------------------------------------------------------------

void TTextHandler::TextVectorPush(int Caller, TTextItem &Text)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextVectorPush," + Text.TextString);
    int HLoc = (Text.HPos)/16;
    int VLoc = (Text.VPos)/16;
    if(HLoc < Track->GetHLocMin()) Track->SetHLocMin(HLoc);
    if(HLoc > Track->GetHLocMax()) Track->SetHLocMax(HLoc);
    if(VLoc < Track->GetVLocMin()) Track->SetVLocMin(VLoc);
    if(VLoc > Track->GetVLocMax()) Track->SetVLocMax(VLoc);
    TextVector.push_back(Text);
    Utilities->CallLogPop(1330);
}

//---------------------------------------------------------------------------

unsigned int TTextHandler::TextVectorSize(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TextVectorSize");
    int TempSize = TextVector.size();
    Utilities->CallLogPop(1430);
    return TempSize;
}

//---------------------------------------------------------------------------

unsigned int TTextHandler::SelectTextVectorSize(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SelectTextVectorSize");
    int TempSize = SelectTextVector.size();
    Utilities->CallLogPop(1431);
    return TempSize;
}

//---------------------------------------------------------------------------

TTextItem *TTextHandler::TextPtrAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + "," + AnsiString(At) + ",TextPtrAt");
    if((At < 0) || ((unsigned int)At >= TextVector.size()))
    {
        throw Exception("At value outside range of TextVector in TextPtrAt");
    }
    TTextItem *TempItem = &(TextVector.at(At));
    Utilities->CallLogPop(1428);
    return TempItem;
}

//---------------------------------------------------------------------------

TTextItem *TTextHandler::SelectTextPtrAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + "," + AnsiString(At) + ",SelectTextPtrAt");
    if((At < 0) || ((unsigned int)At >= SelectTextVector.size()))
    {
        throw Exception("At value outside range of SelectTextVector in SelectTextPtrAt");
    }
    TTextItem *TempItem = &(SelectTextVector.at(At));
    Utilities->CallLogPop(1429);
    return TempItem;
}

//---------------------------------------------------------------------------

bool TTextHandler::FindText(int Caller, AnsiString Name, int &HPos, int &VPos)
{
//Return true if find name in TextVector, HPos & VPos give location, else return false
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindText," + Name);
    for(unsigned int x=0; x<TextVectorSize(12); x++)
    {
        if(TextVector.at(x).TextString == Name)
        {
            HPos = TextVector.at(x).HPos;
            VPos = TextVector.at(x).VPos;
            Utilities->CallLogPop(1560);
            return true;
        }
    }
    Utilities->CallLogPop(1559);
    return false;
}

//---------------------------------------------------------------------------


