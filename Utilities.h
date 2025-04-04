// Utilities.h
/*
      Comments in .h files are believed to be accurate and up to date

      This is a source code file for "railway.exe", a railway operation
      simulator, written originally in Borland C++ Builder 4 Professional with
      later updates in Embarcadero C++Builder.
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
#ifndef UtilitiesH
#define UtilitiesH
// ---------------------------------------------------------------------------

#include <deque>
#include <vcl.h>
#include <fstream>
#include <locale.h> //to check local decimal point character, added at v2.4.0
#include <windows.h>            //needed for 64 bit compilation

// ---------------------------------------------------------------------------

enum TDelayMode //added at v2.13.0.  Here so DelayMode retains value when ClearEverything called
{
    Nil, Minor, Moderate, Major
};

enum TFailureMode //added at v2.14.0.  Here so FailureMode retains value when ClearEverything called
{
    FNil, FMinor, FModerate, FMajor
};

class TUtilities // single object incorporating general purpose data & functions for all units to access
{
public:
//delay & track element failure parameters
    float MinorDelayCutoff;
    float ModerateDelayCutoff;
    float MajorDelayCutoff;
    float MinorDelayFactor;
    float ModerateDelayFactor;
    float MajorDelayFactor;

    int NilPointChangeEventsPerFailure;
    int NilSignalChangeEventsPerFailure;
    int NilMTBTSRs;
    int MinorPointChangeEventsPerFailure;
    int MinorSignalChangeEventsPerFailure;
    int MinorMTBTSRs;
    int ModeratePointChangeEventsPerFailure;
    int ModerateSignalChangeEventsPerFailure;
    int ModerateMTBTSRs;
    int MajorPointChangeEventsPerFailure;
    int MajorSignalChangeEventsPerFailure;
    int MajorMTBTSRs;

    float RepairDiagnosisTime;  //these are all in minutes
    int MaxRandomRepairTime;
    int FixedMinRepairTime;

//other variables
    bool Clock2Stopped;
    ///< when true the main loop - Interface->ClockTimer2 - is stopped
    bool RedLowFlag;
    ///<Sets Red = low values for heatmaps
    bool RHSignalFlag;
    ///< new at v2.3.0   false=LH signals
    bool SetLocaleResultOK;
    ///< flag to indicate whether the call to setlocale() in InterfaceUnit.cpp succeeded or not
    bool ShowLongServRefsFlag;
    ///< when set long service references show on screen, initialised in Interface constructor
    bool NoPlatsMessageSent; //moved here from Track class v2.23.0
    ///< used to send no platforms warning once only
    bool OverrideAndHideSignalBridgeMessage; // added at v2.5.1 to allow facing signals before bridges - with a warning (moved here from Track class v2.23.0)
    ///<if false signals facing bridges are not permitted, but can be set to true using CTRL ALT 5
    char DecimalPoint;
    ///< added at v2.4.0 so can use the local value in loaded session files
    double LastDelayTTClockTime;
    ///< Clock time at which the latest delay for any train occurred. Used to prevent new delays within 5 minutes of the last one, added at v2.13.0
    double MTBTSRs;
    ///<temporary speed restriction, units /day/simple element added at v2.13.0
    int DefaultTrackLength;
    ///< length of each track element before being changed within the program (can be changed in config.txt) (moved here at v2.13.1 so doesn't change)
    int DefaultTrackSpeedLimit;
    ///< speed limit of each track element before being changed within the program (can be changed in config.txt)
    int PointChangeEventsPerFailure;
    ///< number of points changes between failures - reciprocal of failure probability per change
    int SignalChangeEventsPerFailure;
    ///< number of signal changes between failures - reciprocal of failure probability per change
    int ScreenElementWidth;
    ///< width of display screen in elements
    int ScreenElementHeight;
    ///< height of display screen in elements
    int CumulativeDelayedRandMinsAllTrains;
    ///< the running total of all random delays including knock-on delays for all trains, used to reduce total late mins in performance summary (added at v2.13.0)
    std::ofstream PerformanceFile;
    ///< the file where the performance log for a particular period of operation is saved
    std::deque<AnsiString>CallLog;
    ///< call stack store, saved to the errorlog for diagnostic purposes
    std::deque<AnsiString>EventLog;
    ///< event store, saved to the errorlog for diagnostic purposes
    TDateTime LastTSRCheckTime;
    ///< time of last TSR check, used every minute, added at v2.13.0  //change from 5 mins to 1 min at v2.14.0
    AnsiString DateTimeStamp();
    ///< creates a string of the form 'dd/mm/yyyy hh:mm:ss' for use in call & event logging
    AnsiString TimeStamp();
    ///< creates a string of the form 'hh:mm:ss' for use in call & event logging
    TColor clTransparent;
    ///< the display background colour, can be white, black or dark blue
    TDelayMode DelayMode;
    ///< specifies whether no delays or minor, moderate or major random delays are to be applied (added at v2.13.0)
    TFailureMode FailureMode;
    ///< specifies whether no failures or minor, moderate or major random failures are to be applied (added at v2.14.0)

//functions
// void LogEvent(AnsiString Str); //store Str to the event log - moved to TTrainController for v0.6 so can record the tt clock value
    void CallLogPop(int Caller);
    ///< pops the last entry off the call stack, throws an error if called when empty
    void FileDiagnostics(AnsiString Input);
    ///< sends Input to 'TestFile.csv', this is a diagnostic function used during development
    void Pause(double Msec);
    ///gives a delay od Msec value;
    void SaveFileBool(std::ofstream &OutFile, bool SaveBool);
    ///< stores '1' if the bool is true or '0' if false to the file, then a CR
    void SaveFileInt(std::ofstream &OutFile, int SaveInt);
    ///< stores the int value to the file, then a CR
    void SaveFileDouble(std::ofstream &OutFile, double SaveDouble);
    ///< converts the double value to a string (if double stored directly it is truncated to 6 digits) then stores to the file, then a CR, uses the local decimal point character
    void SaveFileString(std::ofstream &OutFile, AnsiString SaveString);
    ///< stores the string value to the file, then a '0' delimiter then a CR
    bool LoadFileBool(std::ifstream &InFile);
    ///< loads a bool value from the file
    int LoadFileInt(std::ifstream &InFile);
    ///< loads an int value from the file
    double LoadFileDouble(std::ifstream &InFile);
    ///< loads a double value from the file (converts from a string to a double) and uses the local decimal point character regardless of what appears in the string
    AnsiString LoadFileString(std::ifstream &InFile);
    ///< loads a string value from the file
    bool CheckFileBool(std::ifstream &InFile);
    ///< checks that the value is a bool returns true for success
    bool CheckFileInt(std::ifstream &InFile, int Lowest, int Highest);
    ///< checks that the value is an int lying between Lowest & Highest (inclusive), returns true for success
    bool CheckAndReadFileInt(std::ifstream &InFile, int Lowest, int Highest, int &OutInt);
    ///< checks that the value is an int lying between Lowest & Highest (inclusive), returns true for success and returns the value in OutInt
    bool CheckFileDouble(std::ifstream &InFile);
    ///< checks that the value is a double, returns true for success
    bool CheckStringDouble(AnsiString &DoubleString);
    ///< checks the string represents a valid double value, returns true for success. Added at v2.4.0 for checking ExcessLCDownMins DoubleString is returned as a reference because the decimal point is potentially modified by the function
    bool CheckFileString(std::ifstream &InFile);
    ///< checks that the value is a string ('0' or CR accepted as delimiters), returns true for success
    bool CheckFileStringZeroDelimiter(std::ifstream &InFile);
    ///< checks that the value is a string ('0' only accepted as the delimiter), returns true for success
    bool CheckAndCompareFileString(std::ifstream &InFile, AnsiString InString);
    ///< checks that the value is a string ('0' or CR accepted as delimiters) and is the same as InString, returns true for success
    bool CheckAndReadFileString(std::ifstream &InFile, AnsiString &OutString);
    ///< checks that the value is a string ('0' or '\n' (CRLF) accepted as delimiters), returns true for success and returns the value in OutString
    bool CheckAndReadOneLineFromConfigFile(std::ifstream &InFile, AnsiString &OutString);
    ///< similar to CheckAndReadFileString but allows tab characters and doesn't ignore initial \n characters
    bool ReadOneLineFromCouplingFile(std::ifstream &InFile, AnsiString &OutString);
    ///< reads a single line from the multiplayer coupling file, returns true for success with OutString containing the line
    AnsiString Format96HHMMSS(TDateTime DateTime);
    ///< formats a TDateTime into an AnsiString of the form hh:mm:ss where hh runs from 00 to 95 & resets when it reaches 96
    AnsiString Format96HHMM(TDateTime DateTime);
    ///< formats a TDateTime into an AnsiString of the form hh:mm where hh runs from 00 to 95 & resets when it reaches 96
    AnsiString IncrementAnsiTimeOneMinute(AnsiString TimeVal);
    ///< takes "HH:MM" and increments it to "HH:MX", where MX == MM + 1, incrementing the hour if necessary
//constructor
    TUtilities()
    {
        MinorDelayCutoff = 2000.0;  //if random(10000) > 2000 there is no delay
        ModerateDelayCutoff = 3000.0;
        MajorDelayCutoff = 3500.0;
        MinorDelayFactor = 1.5; //multiplication factor for log(MinorDelayCutoff/randval) where randval is 1 to 1999
        ModerateDelayFactor = 3.0;
        MajorDelayFactor = 6.0;
        NilPointChangeEventsPerFailure = 1000000; //these set high but not used for no delays
        NilSignalChangeEventsPerFailure = 1000000;
        NilMTBTSRs = 1000000;
        MinorPointChangeEventsPerFailure = 1000;
        MinorSignalChangeEventsPerFailure = 2000;
        MinorMTBTSRs = 200; //once every 200 days per simple element
        ModeratePointChangeEventsPerFailure = 400;
        ModerateSignalChangeEventsPerFailure = 1000;
        ModerateMTBTSRs = 100;
        MajorPointChangeEventsPerFailure = 200;
        MajorSignalChangeEventsPerFailure = 500;
        MajorMTBTSRs = 50;
        RepairDiagnosisTime = 5.0;
        MaxRandomRepairTime = 170;
        FixedMinRepairTime = 10;

    }
};

// ---------------------------------------------------------------------------

extern TUtilities *Utilities; // the object pointer, object created in InterfaceUnit

// ---------------------------------------------------------------------------
#endif
