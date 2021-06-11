// Utilities.cpp
/*
      BEWARE OF COMMENTS in .cpp files:  they were accurate when written but have
      sometimes been overtaken by changes and not updated
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

#include "Utilities.h"

// ---------------------------------------------------------------------------
#pragma package(smart_init)
TUtilities *Utilities;

// ---------------------------------------------------------------------------
void TUtilities::CallLogPop(int Caller)
// use this in case we have too many pops, in which case CallLog seems to be destroyed &
// can't record the error
{
    if(CallLog.empty())
    {
        throw Exception("CallLog pop call when empty! Caller = " + AnsiString(Caller));
    }
    else
    {
// AnsiString TestString = CallLog.back();//test
// Display->FileDiagnostics(TestString);//test
        CallLog.pop_back();
    }
}

// ---------------------------------------------------------------------------
AnsiString TUtilities::DateTimeStamp()
{
    return(TDateTime::CurrentDateTime().FormatString("dd/mm/yyyy hh:nn:ss"));
}

// ---------------------------------------------------------------------------
AnsiString TUtilities::TimeStamp()
{
    return(TDateTime::CurrentTime().FormatString("hh:nn:ss"));
}
// ---------------------------------------------------------------------------
// moved to TTrainController so can record the tt clock value
/*
   void TUtilities::LogEvent(AnsiString Str)
   {
   //restrict to last 1000 entries
   EventLog.push_back(Str);
   if(EventLog.size() > 1000) EventLog.pop_front();
   }
*/
// test
/*
   for(unsigned int x = 0;x<CallLog.size();x++)
    {
    AnsiString TestString = CallLog.at(x);//test
    EventLog.push_back("**" + TestString);
*/
// Display->FileDiagnostics(TestString);//test
// }
// test
// }

// ---------------------------------------------------------------------------
void TUtilities::FileDiagnostics(AnsiString Input) // test function
{
    std::ofstream TestFile("TestFile.csv", std::ios_base::app);
    TestFile << Input.c_str() << '\n';
    TestFile.close();
}

// ---------------------------------------------------------------------------
void TUtilities::SaveFileBool(std::ofstream &OutFile, bool SaveBool)
{
    if(SaveBool)
    {
        OutFile << 1 << '\n';
    }
    else
    {
        OutFile << 0 << '\n';
    }
}

// ---------------------------------------------------------------------------
void TUtilities::SaveFileInt(std::ofstream &OutFile, int SaveInt)
{
    OutFile << SaveInt << '\n';
}

// ---------------------------------------------------------------------------
void TUtilities::SaveFileDouble(std::ofstream &OutFile, double SaveDouble)
// if save directly as a double it is truncated to 6 digits, so convert to a
// string & save that
{
    SaveFileString(OutFile, AnsiString(SaveDouble));
}

// ---------------------------------------------------------------------------
void TUtilities::SaveFileString(std::ofstream &OutFile, AnsiString SaveString)
// If don't send a non- '\n' delimiter (using '\0' for convenience, tested in LoadFileString) then when string is null
// two '\n' characters are placed together.  Then, during reloading, the second one is read because a readable character
// is expected, and on finishing the '\n' that would otherwise be the next character in the file isn't there.  Using '\0'
// allows the '\n' to remain in place.  Also see LoadFileString.
{
    OutFile << SaveString.c_str() << '\0' << '\n';
}

// ---------------------------------------------------------------------------
bool TUtilities::LoadFileBool(std::ifstream &InFile)
// no need to worry about leading '\n' characters as the skipws (skip white space) flag is
// set automatically
{
    int TempVal;
    InFile >> TempVal;
    if(TempVal == 0)
    {
        return(false);
    }
    else
    {
        return(true);
    }
}

// ---------------------------------------------------------------------------
int TUtilities::LoadFileInt(std::ifstream &InFile)
// no need to worry about leading '\n' characters as the skipws (skip white space) flag is
// set automatically
{
    int TempInt;
    InFile >> TempInt;
    return(TempInt);
}

// ---------------------------------------------------------------------------
double TUtilities::LoadFileDouble(std::ifstream &InFile) // modified at v2.4.0 to change the decimal point character if necessary
{
    AnsiString TempString = LoadFileString(InFile);
    if(SetLocaleResultOK) // if false the locale conversion failed so don't change anything, then will work as earlier versions
    {
        for(int x = 1; x <= TempString.Length(); x++)
        {
            if((TempString[x] < '0') && (TempString[x] != '-')) // must be the decimal point character, dot or comma
            {
                TempString[x] = DecimalPoint; // so load the value according to the locale
                break;
            }
        }
    }
    return(TempString.ToDouble());
}

// ---------------------------------------------------------------------------
AnsiString TUtilities::LoadFileString(std::ifstream &InFile)
{
    // see SaveFileString for use of the '\0' & '\n' characters
    char TempChar;
    AnsiString TempString = "";
    InFile.get(TempChar); // get rid of the previous 'n' character, if not '\n' then use as part of string
    if(TempChar == '\n')
    {
        InFile.get(TempChar);
    }
    while(TempChar != '\0') // '\0' is the string delimiter
    {
        TempString = TempString + TempChar;
        InFile.get(TempChar);
    }
    return(TempString);
}

// ---------------------------------------------------------------------------
bool TUtilities::CheckFileBool(std::ifstream &InFile)
// no need to worry about leading '\n' characters as the skipws (skip white space) flag is
// set automatically
{
    AnsiString BoolString;
    if(!CheckAndReadFileString(InFile, BoolString))
    {
        return(false);
    }
    if(InFile.fail())
    {
        return(false);
    }
    if(BoolString == "")
    {
        return(false);
    }
    if((BoolString.Length() > 1) || (BoolString == ""))
    {
        return(false);
    }
    if((BoolString != "0") && (BoolString != "1"))
    {
        return(false);
    }
    return(true);
}

// ---------------------------------------------------------------------------
bool TUtilities::CheckFileInt(std::ifstream &InFile, int Lowest, int Highest)
// no need to worry about leading '\n' characters as the skipws (skip white space) flag is
// set automatically
{
    AnsiString IntString;
    if(!CheckAndReadFileString(InFile, IntString))
    {
        return(false);
    }
    if(InFile.fail())
    {
        return(false);
    }
    if(IntString == "")
    {
        return(false);
    }
    for(int x = 1; x <= IntString.Length(); x++)
    {
        bool CharacterOK = false;
        if((x == 1) && (IntString[x] == '-'))
        {
            CharacterOK = true;
        }
        else if((IntString[x] >= '0') && (IntString[x] <= '9'))
        {
            CharacterOK = true;
        }
        if(!CharacterOK)
        {
            return(false);
        }
    }
    int TempInt = IntString.ToInt();
    if((TempInt < Lowest) || (TempInt > Highest))
    {
        return(false);
    }
    return(true);
}

// ---------------------------------------------------------------------------
bool TUtilities::CheckAndReadFileInt(std::ifstream &InFile, int Lowest, int Highest, int &OutInt)
// no need to worry about leading '\n' characters as the skipws (skip white space) flag is
// set automatically
{
    try
    {
        AnsiString IntString;
        if(!CheckAndReadFileString(InFile, IntString))
        {
            return(false);
        }
        if(InFile.fail())
        {
            return(false);
        }
        if(IntString == "")
        {
            return(false);
        }
        for(int x = 1; x <= IntString.Length(); x++)
        {
            bool CharacterOK = false;
            if((x == 1) && (IntString[x] == '-'))
            {
                CharacterOK = true;
            }
            else if((IntString[x] >= '0') && (IntString[x] <= '9'))
            {
                CharacterOK = true;
            }
            if(!CharacterOK)
            {
                return(false);
            }
        }

        int TempInt = IntString.ToInt();
        if((TempInt < Lowest) || (TempInt > Highest))
        {
            return(false);
        }
        OutInt = TempInt;
        return(true);
    }
    catch(const EConvertError &e)  //non error catch
    {
        return(false);
    }
}
// ---------------------------------------------------------------------------

bool TUtilities::CheckFileDouble(std::ifstream &InFile)
{
    try
    {
        AnsiString DoubleString;
        if(!CheckAndReadFileString(InFile, DoubleString))
        {
            return(false);
        }
        if(InFile.fail())
        {
            return(false);
        }
        if(DoubleString == "")
        {
            return(false);
        }
        if(SetLocaleResultOK) // if false the locale conversion failed so don't change anything, then will work as earlier versions
        {
            for(int x = 1; x <= DoubleString.Length(); x++) // added at v2.4.0 to allow decimal point to be changed to local type
            {
                if((DoubleString[x] < '0') && (DoubleString[x] != '-')) // special character must be the decimal point character, dot or comma
                {
                    DoubleString[x] = DecimalPoint; // so load the value according to the locale
                    break;
                }
            }
        }
        DoubleString.ToDouble(); // throws EConvertError if fails
        return(true);
    }
    catch(const EConvertError &e) //non error catch
    {
        return(false);
    }
}

// ---------------------------------------------------------------------------

bool TUtilities::CheckStringDouble(AnsiString &DoubleString)
{
    try
    {
        if(DoubleString == "")
        {
            return(false);
        }
        if(SetLocaleResultOK) // if false the locale conversion failed so don't change anything, then will work as earlier versions
        {
            for(int x = 1; x <= DoubleString.Length(); x++) // added at v2.4.0 to allow decimal point to be changed to local type
            {
                if((DoubleString[x] < '0') && (DoubleString[x] != '-')) // special character must be the decimal point character, dot or comma
                {
                    DoubleString[x] = DecimalPoint; // so load the value according to the locale
                    break;
                }
            }
        }
        DoubleString.ToDouble(); // throws EConvertError if fails
        return(true);
    }
    catch(const EConvertError &e) //non error catch
    {
        return(false);
    }
}
// ---------------------------------------------------------------------------

bool TUtilities::CheckFileString(std::ifstream &InFile)
// Reads the next item and checks it as a string value up to either the '\0' delimiter
// if there is one, in which case the '\0' is extracted but nothing more, or up to the next '\n',
// in which case the '\n' is extracted.  There may or may not be a '\n' at the start, and if there
// is it is ignored (only one is ignored, a second one is treated as a delimiter).
{
    char TempChar;
    InFile.get(TempChar); // may or may not be '\n'
    if(InFile.fail())
    {
        return(false);
    }
    if(TempChar == '\n')
    {
        InFile.get(TempChar); // get the next one if first was '\n'
        if(InFile.fail())
        {
            return(false);
        }
    }
    while((TempChar != '\0') && (TempChar != '\n'))
    {
        if((TempChar < 32) && (TempChar >= 0))
        {
            return(false);
        }
        InFile.get(TempChar);
        if(InFile.fail())
        {
            return(false);
        }
    }
    return(true);
}
// ---------------------------------------------------------------------------

bool TUtilities::CheckFileStringZeroDelimiter(std::ifstream &InFile)
// Reads the next item and checks it as a legitimate string value up to the '\0' delimiter or end of file
// if there is one, in which case the '\0' is extracted but nothing more.  There may or may not be a '\n' at the start, and if there
// is it is ignored (only one is ignored, a second one is treated as a delimiter).
{
    char TempChar;
    InFile.get(TempChar); // may or may not be '\n'
    if(InFile.fail())
    {
        return(false);
    }
    if(TempChar == '\n')
    {
        InFile.get(TempChar); // get the next one if first was '\n'
        if(InFile.fail())
        {
            return(false);
        }
    }
    while(TempChar != '\0')
    {
        if((TempChar < 32) && (TempChar >= 0))
        {
            return(false);
        }
        InFile.get(TempChar);
        if(InFile.eof())
        {
            return(true); // end of file
        }
        if(InFile.fail())
        {
            return(false);
        }
    }
    return(true);
}
// ---------------------------------------------------------------------------

bool TUtilities::CheckAndCompareFileString(std::ifstream &InFile, AnsiString InString)
// Reads the next item and checks it as a string value up to either the '\0' delimiter
// if there is one, in which case the '\0' is extracted but nothing more, or up to the next '\n',
// in which case the '\n' is extracted.  There may or may not be a '\n' at the start, and if there
// is it is ignored (only one is ignored, a second one is treated as a delimiter).
// The item is then compared with InString and fails if different.
{
    char TempChar;
    char *Buffer = new char[10000];
    int Count = 0;
    InFile.get(TempChar); // may or may not be '\n'
    if(InFile.fail())
    {
        delete[] Buffer;
        return(false);
    }
    if(TempChar == '\n')
    {
        InFile.get(TempChar); // get the next one if first was '\n'
        if(InFile.fail())
        {
            delete[] Buffer;
            return(false);
        }
    }
    while((TempChar != '\0') && (TempChar != '\n'))
    {
        if((TempChar < 32) && (TempChar >= 0))
        {
            delete[] Buffer;
            return(false);
        }
        Buffer[Count] = TempChar;
        Count++;
        InFile.get(TempChar);
        if(InFile.fail())
        {
            delete[] Buffer;
            return(false);
        }
    }
    Buffer[Count] = '\0';
    Count++;
    Buffer[Count] = '\n';
    Count++;
    if(AnsiString(Buffer) != InString)
    {
        delete[] Buffer;
        return(false);
    }
    delete[] Buffer;
    return(true);
}
// ---------------------------------------------------------------------------

bool TUtilities::CheckAndReadFileString(std::ifstream &InFile, AnsiString &OutString)
// Reads the next item and checks it as a string value up to either the '\0' delimiter
// if there is one, in which case the '\0' is extracted but nothing more, or up to the next '\n',
// in which case the '\n' is extracted.  There may or may not be a '\n' at the start, and if there
// is it is ignored (only one is ignored, a second one is treated as a delimiter).
// The item is then returned in OutString.
{
    char TempChar;
    char *Buffer = new char[10000];
    int Count = 0;
    InFile.get(TempChar); // may or may not be '\n'
    if(InFile.fail())
    {
        delete[] Buffer;
        return(false);
    }
    if(TempChar == '\n')
    {
        InFile.get(TempChar); // get the next one if first was '\n'
        if(InFile.fail())
        {
            delete[] Buffer;
            return(false);
        }
    }
    while((TempChar != '\0') && (TempChar != '\n'))
    {
        if((TempChar < 32) && (TempChar >= 0))
        {
            delete[] Buffer;
            return(false);
        }
        Buffer[Count] = TempChar;
        Count++;
        InFile.get(TempChar);
        if(InFile.fail())
        {
            delete[] Buffer;
            return(false);
        }
    }
    Buffer[Count] = '\0';
    Count++;
    Buffer[Count] = '\n';
    Count++;
    OutString = AnsiString(Buffer);
    delete[] Buffer;
    return(true);
}

// ---------------------------------------------------------------------------

AnsiString TUtilities::Format96HHMMSS(TDateTime DateTime)
// Formats a TDateTime into an AnsiString of the form hh:mm:ss where hh runs from 00 to 95 & resets when it reaches 96
{
    AnsiString MinSecString = DateTime.FormatString(":nn:ss");
    int Hours = (int)(((double)(DateTime + 0.000006)) * 24); // for v0.6 round up by ~0.5 sec to prevent undershooting
    while(Hours >= 96)
    {
        Hours -= 96;
    }
    AnsiString HourString = AnsiString(Hours);
    while(HourString.Length() < 2)
    {
        HourString = "0" + HourString;
    }
    return (HourString + MinSecString);
}

// ---------------------------------------------------------------------------

AnsiString TUtilities::Format96HHMM(TDateTime DateTime)
// Formats a TDateTime into an AnsiString of the form hh:mm where hh runs from 00 to 95 & resets when it reaches 96
{
    AnsiString MinString = DateTime.FormatString(":nn");
    int Hours = (int)(((double)(DateTime + 0.0003)) * 24); // for v0.6 round up by ~0.5min to prevent undershooting the hour in formatted tts
    while(Hours >= 96)
    {
        Hours -= 96;
    }
    AnsiString HourString = AnsiString(Hours);
    while(HourString.Length() < 2)
    {
        HourString = "0" + HourString;
    }
    return (HourString + MinString);
}

// ---------------------------------------------------------------------------

AnsiString TUtilities::IncrementAnsiTimeOneMinute(AnsiString TimeVal)
{
    AnsiString MinString = TimeVal.SubString(4,2);
    AnsiString HourString = TimeVal.SubString(1,2);
    int Mins =  MinString.ToInt();
    int Hours = HourString.ToInt();
    Mins++;
    if(Mins > 59)
    {
        Mins = 0;
        Hours++; //don't need to worry about exceeding 96 hours as TT validity checker looks after that
    }
    MinString = AnsiString(Mins);
    if(MinString.Length() < 2)
    {
        MinString = "0" + MinString;
    }
    HourString = AnsiString(Hours);
    if(HourString.Length() < 2)
    {
        HourString = "0" + HourString;
    }
    return(HourString + ":" + MinString);
}

// ---------------------------------------------------------------------------

