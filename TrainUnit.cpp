//TrainUnit.cpp
//BEWARE OF COMMENTS in .cpp files:  they were accurate when written but have sometimes been overtaken by changes and not updated
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
#pragma hdrstop

#include "TrainUnit.h"
#include "TrackUnit.h"
#include "GraphicUnit.h"
#include "DisplayUnit.h"
#include "Utilities.h"
#include <math.hpp> //for speed calcs

//---------------------------------------------------------------------------
#pragma package(smart_init)

TTrainController *TrainController;

//---------------------------------------------------------------------------

int TTrain::NextTrainID = 0;//has to be initialised outside the class

//---------------------------------------------------------------------------

TTrain::TTrain(int Caller, int RearStartElementIn, int RearStartExitPosIn, AnsiString InputCode, int StartSpeedIn, int MassIn, double MaxRunningSpeedIn,
        double MaxBrakeRateIn, double PowerAtRailIn, TTrainMode TrainModeIn, TTrainDataEntry *TrainDataEntryPtrIn, int RepeatNumberIn,
        int IncrementalMinutesIn, int IncrementalDigitsIn, int SignallerMaxSpeedIn) :
        RearStartElement(RearStartElementIn), RearStartExitPos(RearStartExitPosIn), HeadCode(InputCode), StartSpeed(StartSpeedIn),
        Mass(MassIn), MaxRunningSpeed(MaxRunningSpeedIn), MaxBrakeRate(MaxBrakeRateIn), PowerAtRail(PowerAtRailIn), TrainMode(TrainModeIn),
        TrainDataEntryPtr(TrainDataEntryPtrIn), RepeatNumber(RepeatNumberIn), IncrementalMinutes(IncrementalMinutesIn),
        IncrementalDigits(IncrementalDigitsIn), SignallerMaxSpeed(SignallerMaxSpeedIn)
/*
Construct a new train with general default values and input values for position and headcode.
Create the frontcode, headcode and background graphics here but don't delete them in a destructor.
This is because trains are kept in a vector and vectors erase elements during internal operations.
Deletion is explicit by using a special function.  Increment the static class member NextTrainID
after setting this train's ID.
*/

{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TTrain," +
        AnsiString(RearStartElementIn) + "," + AnsiString(RearStartExitPosIn) + "," + AnsiString(InputCode) +
        "," + AnsiString(StartSpeedIn) + "," + AnsiString(MassIn) + "," + AnsiString(TrainModeIn));
//AutoControl = true;//all trains start in auto control
UpdateCounter = 0;
TimeTimeLocArrived = false;
Derailed = false;
DerailPending = false;
Crashed = false;
StoppedAtBuffers = false;
StoppedAtSignal = false;
StoppedAtLocation = false;
StoppedAfterSPAD = false;
StoppedForTrainInFront = false;
SignallerStoppingFlag = false;
SignallerStopped = false;
SignallerRemoved = false;
NotInService = false;
HoldAtLocationInTTMode = false;
AllowedToPassRedSignal = false;
CallingOnFlag = false;
BeingCalledOn = false;
DepartureTimeSet = false;
BufferZoomOutFlashRequired = false;
TimetableFinished = false;
LastActionDelayFlag = false;
OneLengthAccelDecel = false;
TrainCrashedInto = -1;
BackgroundColour = clNormalBackground;
Plotted = false;
TrainGone = false;
SPADFlag = false;
FrontCodePtr = new Graphics::TBitmap;
FrontCodePtr->PixelFormat = pf8bit;
FrontCodePtr->Height = 8;
FrontCodePtr->Width = 8;
FrontCodePtr->Assign(RailGraphics->TempBackground);
FrontCodePtr->Transparent = false;
AValue = sqrt(2*PowerAtRail/Mass);
TimetableMaxRunningSpeed = MaxRunningSpeed;
TerminatedMessageSent = false;
JoinedOtherTrainFlag = false;
LeavingUnderSigControlAtContinuation = false;
StepForwardFlag = false;
RestoreTimetableLocation = "";
for(int x=0;x<4;x++)
    {
    HeadCodeGrPtr[x] = new Graphics::TBitmap;
    HeadCodeGrPtr[x]->PixelFormat = pf8bit;
    HeadCodeGrPtr[x]->Height = 8;
    HeadCodeGrPtr[x]->Width = 8;
    HeadCodeGrPtr[x]->Assign(RailGraphics->TempBackground);
    HeadCodeGrPtr[x]->Transparent = false;
    }
for(int x=0;x<4;x++)
    {
    BackgroundPtr[x] = new Graphics::TBitmap;
    BackgroundPtr[x]->PixelFormat = pf8bit;
    BackgroundPtr[x]->Height = 8;
    BackgroundPtr[x]->Width = 8;
    BackgroundPtr[x]->Assign(RailGraphics->TempBackground);
    BackgroundPtr[x]->Transparent = false;
    }
for(int x=0;x<4;x++)
    {
    HeadCodePosition[x] = HeadCodeGrPtr[x];//set here to ensure have values
    }
for(int x=0;x<4;x++)
    {
    PlotElement[x] = -1;//marker for not plotted yet
    }
for(int x=0;x<3;x++)
    {
    OldZoomOutElement[x] = -1;//marker for not plotted yet
    }
TrainID = NextTrainID;
NextTrainID++;

//new values added to complete initialisation of all TTrain variables

//ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0)); can't be initialised yet as session trains created with Null
//TrainDataEntryPtr, initialise in AddTrain
FrontElementSpeedLimit = 0;
FrontElementLength = 0;
EntrySpeed = 0;
ExitSpeedHalf = 0;
ExitSpeedFull = 0;
MaxExitSpeed = 0;
BrakeRate = 0;
SignallerStopBrakeRate = 0;
FirstHalfMove = true;
EntryTime = 0;
ExitTimeHalf = 0;
ExitTimeFull = 0;
ReleaseTime = 0;
TRSTime = 0;
LastActionTime = 0;
Straddle = MidLag;
LeadElement = -1;
LeadEntryPos = 0;
LeadExitPos = 0;
MidElement = -1;
MidEntryPos = 0;
MidExitPos = 0;
LagElement = -1;
LagEntryPos = 0;
LagExitPos = 0;
for(int x=0;x<4;x++)
    {
    HOffset[x] = 0;
    VOffset[x] = 0;
    PlotEntryPos[x] = 0;
    }
Utilities->CallLogPop(648);
}

//---------------------------------------------------------------------------

void TTrain::DeleteTrain(int Caller)
/*
Delete train heap objects (bitmaps) explicitly by this special function rather than by a destructor, because vectors
erase elements during internal operations & if TTrain had an explicit destructor that deleted the heap elements then
it would be called when a vector element was erased.  Calling the default TTrain destructor doesn't matter because all that
does is release the memory of the members (including pointers to the bitmaps), it doesn't destroy the bitmaps themselves.
It's important therefore to call this function before erasing the vector element, otherwise the pointers to the bitmaps
would be lost and the bitmaps never destroyed, thereby causing memory leaks.
*/
{
//if(NoDelete) return;//used when a TTrain is created to hold copied values from elsewhere
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DeleteTrain");
if(Display->ZoomOutFlag) UnplotTrainInZoomOutMode(0);
if(FrontCodePtr == 0)
    {
    throw Exception("Error in attempting to delete FrontCodePtr");
    }
delete FrontCodePtr;
FrontCodePtr = 0;
for(int x=0;x<4;x++)
    {
    if(BackgroundPtr[x] == 0)
        {
        throw Exception("Error in attempting to delete BackgroundPtr[" + AnsiString(x) + "]");
        }
    delete BackgroundPtr[x];
    BackgroundPtr[x] = 0;
    }
for(int x=0;x<4;x++)
    {
    if(HeadCodeGrPtr[x] == 0)
        {
        throw Exception("Error in attempting to delete HeadCodeGrPtr[" + AnsiString(x) + "]");
        }
    delete HeadCodeGrPtr[x];
    HeadCodeGrPtr[x] = 0;
    }
Utilities->CallLogPop(649);
}

//---------------------------------------------------------------------------

void TTrain::PlotStartPosition(int Caller)
/*
Plots the train starting position on screen.  Note that the check for starting on straight points &
on wrongly set points is carried out in TrainControllerUnit [but have to allow for starting on points because
ChangeDirection calls this function.].  Train starts on Lead & Mid elements & Straddle = LeadMid unless
entering at a continuation in which case Straddle = MidLag & train not plotted immediately.
Set the headcode graphics pointers from the headcode text, then check whether starting at a
continuation.  If so set Mid & Lag elements to -1 so they won't be plotted, and set Lead values
for the continuation element.  Otherwise set Lead and Mid values,

and Lead element value unless
Mid element is a buffer or continuation.  Set Straddle, then for the Mid element set the graphic
offsets and headcode positions and front code.  Pick up background bitmaps for the Mid element,
then check if a train on either Mid or Lag and if so give a warning message and return false so
that the calling function can delete the train.  Plot the Mid element train values then do similarly
for the Lag element - set offsets, pick up background bitmaps, and plot the rear two segments of
the train.  Finally set the Plotted flag and return true.
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotStartPosition," + HeadCode);
int NextElementPosition, NextEntryPos, ElementLength, SpeedLimit;
SetHeadCodeGraphics(0, HeadCode);
//PlotStartTime = TrainController->TTClockTime;
FirstHalfMove = true;

//if enter at continuation then don't plot anything at start, but set TrainIDOnElement for continuation entry so as to
//'claim' it for this train to prevent any other waiting trains trying to enter
if(Track->TrackElementAt(163, RearStartElement).TrackType == Continuation)
    {
    LagElement = -1;//not to be plotted
    LagExitPos = 0;//not to be plotted
    LagEntryPos = 0;//not to be plotted
    MidElement = -1;//not to be plotted
    MidExitPos = 0;//not to be plotted
    MidEntryPos = 0;//not to be plotted
    LeadElement = RearStartElement;
    LeadExitPos = 1;//will be 1 for continuation entry
    LeadEntryPos = 0;

    EntrySpeed = StartSpeed;
    MaxExitSpeed = StartSpeed;//initial value
    EntryTime = TrainController->TTClockTime;
    ElementLength = Track->TrackElementAt(164, LeadElement).Length01;
    SpeedLimit = Track->TrackElementAt(165, LeadElement).SpeedLimit01;
    if(EntrySpeed > SpeedLimit) EntrySpeed = SpeedLimit;
    if(EntrySpeed > MaxRunningSpeed) EntrySpeed = MaxRunningSpeed;
    FirstHalfMove = true;
    SetTrainMovementValues(0, LeadElement, LeadEntryPos);//LeadElement is the element to be entered

    //Precautionary check - If need to brake EntrySpeed may be too high, so set it to the speed at which
    //can achieve ExitSpeedFull at the half braking rate.
    if(ExitSpeedFull < EntrySpeed)
        {
        double TempEntrySpeed = sqrt((MaxExitSpeed * MaxExitSpeed) + (3.6 * 3.6 * MaxBrakeRate * ElementLength));//half braking
        if(TempEntrySpeed < EntrySpeed)
            {
            EntrySpeed = TempEntrySpeed;
            SetTrainMovementValues(1, LeadElement, LeadEntryPos);
            }
        }
    Straddle = MidLag;//only for starting on a continuation
    Track->TrackElementAt(536, LeadElement).TrainIDOnElement = TrainID;//no need to stop gap flashing if start on continuation
    }
else//not starting at a continuation
    {
    LagElement = -1;
    LagEntryPos = 0;
    LagExitPos = 0;
    MidElement = RearStartElement;
    MidExitPos = RearStartExitPos;
    MidEntryPos = Track->GetAnyElementOppositeLinkPos(0, MidElement, MidExitPos, Derailed);
    LeadElement = Track->TrackElementAt(166, MidElement).Conn[MidExitPos];
    LeadEntryPos = Track->TrackElementAt(167, MidElement).ConnLinkPos[MidExitPos];
    LeadExitPos = Track->GetAnyElementOppositeLinkPos(1, LeadElement, LeadEntryPos, Derailed);

    EntrySpeed = StartSpeed;
    MaxExitSpeed = StartSpeed;//initial value
    EntryTime = TrainController->TTClockTime;
    bool TempDerail = false; //dummy
    NextElementPosition = Track->TrackElementAt(168, LeadElement).Conn[Track->GetAnyElementOppositeLinkPos(2, LeadElement, LeadEntryPos, TempDerail)];
    NextEntryPos = Track->TrackElementAt(169, LeadElement).ConnLinkPos[Track->GetAnyElementOppositeLinkPos(3, LeadElement, LeadEntryPos, TempDerail)];
//  facing buffers check - ignore starting speed if start facing buffers
    StoppedAtBuffers = false;//need to set here as well as in UpdateTrain() in case paused during signaller change direction
    if((Track->TrackElementAt(492, LeadElement).TrackType == Buffers) &&
            (Track->TrackElementAt(493, LeadElement).Config[LeadExitPos] == End))
        {
        FrontElementSpeedLimit = Track->TrackElementAt(494, LeadElement).SpeedLimit01;//use 01 for convenience, not used
        FrontElementLength = Track->TrackElementAt(495, LeadElement).Length01;//use 01 for convenience, not used
        EntrySpeed = 0;
        ExitSpeedHalf = 0;
        ExitSpeedFull = 0;
        MaxExitSpeed = 0;//SetTrainMovementValues not called so set this here
        BrakeRate = 0;
        ExitTimeHalf = EntryTime;
        ExitTimeFull = EntryTime;
        if(!StoppedAtLocation) StoppedAtBuffers = true;//stopped at location takes precedence
        }

//  facing continuation check
    else if((Track->TrackElementAt(513, LeadElement).TrackType == Continuation) &&
            (Track->TrackElementAt(514, LeadElement).Config[LeadExitPos] == End))
        {
        FrontElementSpeedLimit = Track->TrackElementAt(509, LeadElement).SpeedLimit01;//use 01 for convenience, not used
        FrontElementLength = Track->TrackElementAt(510, LeadElement).Length01;//use 01 for convenience, not used
        ExitSpeedHalf = EntrySpeed;
        ExitSpeedFull = EntrySpeed;
        MaxExitSpeed = EntrySpeed;
        BrakeRate = 0;
        ExitTimeHalf = TrainController->TTClockTime + TDateTime(1.8 * (double)FrontElementLength/EntrySpeed/86400);
        ExitTimeFull = TrainController->TTClockTime + TDateTime(3.6 * (double)FrontElementLength/EntrySpeed/86400);
        }

//  Signal check
    else if((NextElementPosition > -1) && (NextEntryPos > -1))//condition check added as precaution after SloughIECC error reported by James U
        {
        if((Track->TrackElementAt(170, NextElementPosition).Config[Track->GetNonPointsOppositeLinkPos(NextEntryPos)] == Signal) &&
            (Track->TrackElementAt(171, NextElementPosition).Attribute == 0))
            {
            FrontElementSpeedLimit = Track->TrackElementAt(172, LeadElement).SpeedLimit01;//use 01 for convenience, not used
            FrontElementLength = Track->TrackElementAt(173, LeadElement).Length01;//use 01 for convenience, not used
            EntrySpeed = 0;
            ExitSpeedHalf = 0;
            ExitSpeedFull = 0;
            MaxExitSpeed = 0;
            BrakeRate = 0;
            ExitTimeHalf = EntryTime;
            ExitTimeFull = EntryTime;
            if(!StoppedAtLocation)//stopped at location takes precedence
                {
                StoppedAtSignal = true;
                PlotTrainWithNewBackgroundColour(33, clSignalStopBackground, Display);
    //            TrainController->LogActionError(39, HeadCode, "", SignalHold, Track->TrackElementAt(754, NextElementPosition).ElementID);
                }
            if(StoppedAtLocation && (TrainMode == Signaller))
                {//set both StoppedAtLocation & StoppedAtSignal, so that 'pass red signal' is offered in popup menu rather than move
                 //forwards, but don't change the background colour so still shows as stopped at location
                StoppedAtSignal = true;
                }
            }
        else
            {
            StoppedAtSignal = false;
            if(NextEntryPos > 1)
                {
                ElementLength = Track->TrackElementAt(174, NextElementPosition).Length23;
                SpeedLimit = Track->TrackElementAt(175, NextElementPosition).SpeedLimit23;
                }
            else
                {
                ElementLength = Track->TrackElementAt(176, NextElementPosition).Length01;
                SpeedLimit = Track->TrackElementAt(177, NextElementPosition).SpeedLimit01;
                }
            if(EntrySpeed > SpeedLimit) EntrySpeed = SpeedLimit;
            if(EntrySpeed > MaxRunningSpeed) EntrySpeed = MaxRunningSpeed;
            MaxExitSpeed = EntrySpeed;
            TDateTime TestTime = TrainController->TTClockTime;//test
            AnsiString TimeString = Utilities->Format96HHMMSS(TestTime);//test
            FirstHalfMove = true;
            SetTrainMovementValues(2, NextElementPosition, NextEntryPos);//NextElement is the element to be entered

            //Precautionary check - If need to brake EntrySpeed may be too high, so set it to the speed at which
            //can achieve ExitSpeedFull at the half braking rate.
            if(ExitSpeedFull < EntrySpeed)
                {
                double TempEntrySpeed = sqrt((MaxExitSpeed * MaxExitSpeed) + (3.6 * 3.6 * MaxBrakeRate * ElementLength));//half braking
                if(TempEntrySpeed < EntrySpeed)
                    {
                    EntrySpeed = TempEntrySpeed;
                    SetTrainMovementValues(3, NextElementPosition, NextEntryPos);
                    }
                }
            }
        }
    if(Track->TrackElementAt(178, LeadElement).Config[LeadExitPos] == NotSet)
        {
        throw Exception("Error, LeadElement Exit Connection is NotSet");
        }
    }

if(MidElement > -1)//will be -1 if start on continuation
    {
    Straddle = LeadMid;
    GetOffsetValues(1, HOffset[0], VOffset[0], Track->TrackElementAt(179, LeadElement).Link[LeadExitPos]);
    GetOffsetValues(2, HOffset[1], VOffset[1], Track->TrackElementAt(180, LeadElement).Link[LeadEntryPos]);
    if(LowEntryValue(Track->TrackElementAt(181, LeadElement).Link[LeadEntryPos]))
        {
        for(int x=0;x<4;x++)
            {
            HeadCodePosition[x] = HeadCodeGrPtr[3 - x];
            }
        }
    else
        {
        for(int x=0;x<4;x++)
            {
            HeadCodePosition[x] = HeadCodeGrPtr[x];
            }
        }
    if(TrainMode == Timetable) RailGraphics->ChangeForegroundColour(19, HeadCodePosition[0], FrontCodePtr, clFrontCodeTimetable, BackgroundColour);
    else RailGraphics->ChangeForegroundColour(20, HeadCodePosition[0], FrontCodePtr, clFrontCodeSignaller, BackgroundColour);
    HeadCodePosition[0] = FrontCodePtr;
//pick up background bitmaps [0] & [1] & plot HeadCodes [0] & [1]

    PickUpBackgroundBitmap(0, HOffset[0], VOffset[0], LeadElement, LeadEntryPos, BackgroundPtr[0]);
    PickUpBackgroundBitmap(1, HOffset[1], VOffset[1], LeadElement, LeadEntryPos, BackgroundPtr[1]);
/*  Move check to AddTrain, also, now that can start on bridges need to check that other train is on same track before refusing
    if((Track->TrackElementAt(182, LeadElement).TrainIDOnElement > -1) || ((MidElement > -1) && (Track->TrackElementAt(183, MidElement).TrainIDOnElement > -1)))
        {
        ShowMessage("Can't place train " + HeadCode + "; another train already present at location");
        Utilities->CallLogPop(651);
        return false;
        }
*/
    PlotElement[0] = LeadElement; PlotEntryPos[0] = LeadEntryPos;
    PlotElement[1] = LeadElement; PlotEntryPos[1] = LeadEntryPos;
    PlotTrainGraphic(8, 0, Display);
    PlotTrainGraphic(9, 1, Display);

    GetOffsetValues(3, HOffset[2], VOffset[2], Track->TrackElementAt(184, MidElement).Link[MidExitPos]);
    GetOffsetValues(4, HOffset[3], VOffset[3], Track->TrackElementAt(185, MidElement).Link[MidEntryPos]);

//pick up background bitmaps [2] & [3]

    PickUpBackgroundBitmap(2, HOffset[2], VOffset[2], MidElement, MidEntryPos, BackgroundPtr[2]);
    PickUpBackgroundBitmap(3, HOffset[3], VOffset[3], MidElement, MidEntryPos, BackgroundPtr[3]);

    PlotElement[2] = MidElement; PlotEntryPos[2] = MidEntryPos;
    PlotElement[3] = MidElement; PlotEntryPos[3] = MidEntryPos;
    PlotTrainGraphic(10, 2, Display);
    PlotTrainGraphic(11, 3, Display);
//    Plotted = true; set in PlotTrainGraphic
    }
Display->Update();// resurrected when Update() dropped from PlotOutput etc
Utilities->CallLogPop(652);
return;
}

//---------------------------------------------------------------------------
void TTrain::UnplotTrain(int Caller)
{
if(!Plotted) return;
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",UnplotTrain");

if(Straddle == MidLag)
    {
    if(MidElement > -1)
        {
        PlotBackgroundGraphic(0, 0);
        PlotBackgroundGraphic(1, 1);
        }
    if(LagElement > -1)
        {
        PlotBackgroundGraphic(2, 2);
        PlotBackgroundGraphic(3, 3);
        }
    }
else if(Straddle == LeadMidLag)
    {
    if(LeadElement > -1)
        {
        PlotBackgroundGraphic(4, 0);
        }
    if(MidElement > -1)
        {
        PlotBackgroundGraphic(5, 1);
        PlotBackgroundGraphic(6, 2);
        }
    if(LagElement > -1)
        {
        PlotBackgroundGraphic(7, 3);
        }
    }
else if(Straddle == LeadMid)
    {
    if(LeadElement > -1)
        {
        PlotBackgroundGraphic(8, 0);
        PlotBackgroundGraphic(9, 1);
      }
    if(MidElement > -1)
        {
        PlotBackgroundGraphic(10, 2);
        PlotBackgroundGraphic(11, 3);
        }
    }

if(LeadElement > -1) ResetTrainElementID(1, LeadElement, LeadEntryPos);
if(MidElement > -1) ResetTrainElementID(2, MidElement, MidEntryPos);
if(LagElement > -1) ResetTrainElementID(3, LagElement, LagEntryPos);
Plotted = false;
BackgroundColour = clNormalBackground;
Display->Update();//without this the screen 'blinks' at next Clearand... prob forces a full repaint for some reason
//resurrected when Update() dropped from PlotOutput etc
Utilities->CallLogPop(653);
}

//----------------------------------------------------------------------------

void TTrain::UpdateTrain(int Caller)
/*
Note:  Some changes made since comments written

Brief:
Enter with Straddle defining train position wrt Lag, Mid & Lead elements.  Is only MidLag at this point
on first entry at a continuation (with no train plotted), in all other cases it is either LeadMid (when train fully
on Lead & Mid elements) or LeadMidLag (when train straddling 3 elements).
Thereafter on entry Straddle = LeadMidLag or LeadMid; LeadMid if train fully on Mid & Lead elements, and
LeadMidLag if on Lag, Mid and Lead elements (back on lag, front on Lead, & middle 2 segments on Mid).
If enter with Straddle = LeadMid, then train is in effect in the first half of the next element, and moves half onto it after
the half time point has been passed.  The values for the next element were set when the train was last updated when Straddle became
LeadMid from LeadMidLag.  After the half time point has been passed Straddle is
changed to MidLag within the function and all elements moved down one, old Mid becomes
the new lag, old Lead becomes the new Mid, and a new Lead is obtained.  Then the new positions are plotted, and finally Straddle is
incremented to reflect the position the train now occupies.

Detail:
Check whether stopped at a non-red signal, and if so reset StoppedAtSignal so train can move.
Check whether buffers at immediate exit, either when first enter the function or later, and set StoppedAtBuffers if so
and return.
If Straddle == LeadMid then train fully on Lead and Mid, so ready for a major update:-
    If there's a LagElement (there will be but include check for good practice - next
    function depends on it) Check whether DerailPending set - set during last GetLeadElement if appropriate but only acted on here when
    train fully on offending point - Derail set and DerailPanding reset, train background
    colour changed (note that BackgroundColour is a property of the train itself) then return.
    If no derail pending reset Lag and Mid elements to the old Mid and Lead values, reset Straddle to MidLag, then set
    the new LeadElement, which will be the next connected element (obtain using GetLeadElement) or -1 if the current
    LeadElement is an exit continuation.  During GetLeadElement the element at LeadElement is checked and if a stop
    signal is found StoppedAtSignal is set to true, otherwise StoppedAtSignal is set to false.  Also Derail is set
    if LeadElement is a fouled trailing point.
Now, the train is moved on by one segment.  Firstly the last BackgroundElement is set to LagElement, then the last
segment of the LagElement is unplotted (if there is a LagElement - may be entering at a continuation), by
replotting the last background segment and checking whether the element is a bridge or crossover with the other
track in a route, in which case the route colour is replotted.
Then, if Straddle == LeadMidLag (train will move completely off the element during this function), and the train
track is in a route, then all the train elements are removed from the route unless it's an autosig route.  Normally only the
LeadElement will be in a route for a moving train, but when originally placed all elements may be in the route so check them all.
Note also that there may be two routes at a given element position, but only one of them is the correct one, so this
is identified prior to the removal.  Also the TrainIDs are reset because the train will be fully off this element at the end of
the function.  If Straddle == LeadMidlag and the element being left is a ContinuationExit the the TrainGone flag is set so the
train can be deleted by the calling function, and the function returns.
If the element is a signal in the train movement direction, then it is reset to red (Attribute = 0) and is replotted
to show the red aspect.  Finally if element is a signal in the other direction it is replotted as it was - need to
plot individually because could have any aspect, the background bitmap that was picked up earlier contains just the
basic red aspect.

Now all the array values are updated, but the [0] values are as yet invalid, these have to be obtained explicitly from
the new LeadElement later.  The headcode graphics are updated so that it reads correctly - left to right & top to bottom,
regardless of direction, and with the correct front code colour.

The new front segment background bitmap is now picked up and the graphic offsets set, and the segments are plotted.
No more unplotting is needed as all but the last segment are overwritten by later segments, and the new front
segment is just plotted, though the background bitmap at that location has to be picked up.  Just where they are
plotted depends on the Straddle value, [0] is always on Lead, [1] is on Lead if Straddle == LeadMidLag or Mid if
Straddle == MidLag; [2] is always on Mid, and [3] is on Mid if Straddle == LeadMidLag or Lag if Straddle == MidLag.
Also prior to plotting the lead segment a crash check is made, and if true the Crashed flag is set and the
TrainCrashedInto value also set to the current TrainID - this is so it too becomes crashed and hence stopped.

The Crashed flag is now checked, and if set the front headcode colour is changed to the same as the rest of the code,
and the background colour changed.  Then the train that is crashed into is also set to Crashed, and its colours
changed similarly.  The function then returns.

If Crashed is not set then Straddle is incremented and the function returns.
*/

{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",UpdateTrain");

AnsiString HC = HeadCode;//to identify the train for diagnostic purposes

UpdateCounter++;
if(UpdateCounter > 100) UpdateCounter = 0;
int LockedVectorNumber;
Graphics::TBitmap *EXGraphicPtr = RailGraphics->bmTransparentBgnd;//default values - these needed for route checker below, though not used here
Graphics::TBitmap *EntryDirectionGraphicPtr = RailGraphics->bmTransparentBgnd;

if(!SignallerStoppingFlag) SignallerStopBrakeRate = 0;

if(Crashed || Derailed)
    {
    Utilities->CallLogPop(1017);
    return;//no further action, user has to remove or work around
    }

if(StoppedForTrainInFront && !StoppedAtLocation && !Crashed && !Derailed)
    {
    PlotTrainWithNewBackgroundColour(42, clStoppedTrainInFront, Display);
    }

if(SignallerStopped && !StoppedForTrainInFront && !StoppedAtBuffers && !StoppedAtLocation && !Crashed && !Derailed)
    {
    PlotTrainWithNewBackgroundColour(43, clSignallerStopped, Display);
    }

if(!Stopped() && !SPADFlag)
    {
    TimeTimeLocArrived = false;
    PlotTrainWithNewBackgroundColour(44, clNormalBackground, Display);
    }

//set or release StoppedAtBuffers if fully on 2 elements depending on LeadElement
//Note that if LeadElement == Buffers train must be facing the buffer so no need to check orientation
/*old version where force a stop at buffers regardless of speed
if((Straddle == LeadMid) && (LeadElement > -1) && (Track->TrackElementAt(, LeadElement).TrackType == Buffers)) StoppedAtBuffers = true;
else StoppedAtBuffers = false;
*/
//new version where crash if run into buffers
if(!Crashed)
    {
    if((Straddle == LeadMid) && (LeadElement > -1) && (Track->TrackElementAt(602, LeadElement).TrackType == Buffers))
        {
        if(ExitSpeedFull > 1)
            {
            Crashed = true;
            RailGraphics->ChangeForegroundColour(24, HeadCodePosition[0], HeadCodePosition[0], clB0G0R0, BackgroundColour);
            PlotTrainWithNewBackgroundColour(40, clCrashedBackground, Display);//red
            TrainController->LogActionError(35, HeadCode, "", FailBufferCrash, Track->TrackElementAt(719,LeadElement).ElementID);
//            SendMissedActionLogs(3, -1, ActionVectorEntryPtr);//-1 is a marker for send messages for all remaining entries, including Fer if present
//no need for missed action logs - will be sent when train removed
            StoppedAtBuffers = false;
            }
        else if(!StoppedAtLocation)//stopped at location takes precedence
            {
            StoppedAtBuffers = true;
            }
        else StoppedAtBuffers = false;
        }
    else StoppedAtBuffers = false;
    }
else StoppedAtBuffers = false;//if crashed don't want stopped at buffers set

if(StoppedAtBuffers && !BufferZoomOutFlashRequired && !StoppedAtLocation && !Crashed && !Derailed)
    {
    PlotTrainWithNewBackgroundColour(37, clBufferStopBackground, Display);
    }

//set or reset HoldAtLocationInTTMode
if((TrainMode == Timetable) && StoppedAtLocation && (ActionVectorEntryPtr->Command != "")) HoldAtLocationInTTMode = true;
else if(TrainMode == Timetable) HoldAtLocationInTTMode = false;
//in Signaller mode HoldAtLocationInTTMode not changed

//check if departure pending & set times unless already set
if(TrainMode == Timetable)
    {
    if(!DepartureTimeSet && StoppedAtLocation)// && !StoppedAtBuffers) - set times whether or not at buffers
        {
        if(ActionVectorEntryPtr->DepartureTime > TDateTime(-1))
            {
            ReleaseTime = TrainController->GetRepeatTime(0, ActionVectorEntryPtr->DepartureTime, RepeatNumber, IncrementalMinutes);
            if(ReleaseTime <= LastActionTime + TDateTime(30.0/86400))
                    ReleaseTime = LastActionTime + TDateTime(30.0/86400);
            TRSTime = ReleaseTime - TDateTime(10.0/86400);
            DepartureTimeSet = true;
            }
        }
    }

//check if being held at location pending any actions & deal with them if time appropriate & >= 30s since LastActionTime
if(TrainMode == Timetable)
    {
    if(HoldAtLocationInTTMode)
        {
        //ignore TimeLoc & TTLoc departures
        //Action logs given in functions
        if((TrainController->TTClockTime >= GetTrainTime(0, ActionVectorEntryPtr->EventTime)) &&
                (TrainController->TTClockTime >= LastActionTime + TDateTime(30.0/86400)))
            {
            if(ActionVectorEntryPtr->Command == "fsp") FrontTrainSplit(0);
            else if(ActionVectorEntryPtr->Command == "rsp") RearTrainSplit(0);
            else if(ActionVectorEntryPtr->Command == "Fjo") FinishJoin(0);
            else if(ActionVectorEntryPtr->Command == "jbo") JoinedBy(0);
            else if(ActionVectorEntryPtr->Command == "cdt") ChangeTrainDirection(0);
            else if(ActionVectorEntryPtr->Command == "Fns") NewTrainService(0);
            else if(ActionVectorEntryPtr->Command == "Frh") RemainHere(0);
            else if(ActionVectorEntryPtr->Command == "Fer")  TimetableFinished = true;//other aspects of 'Fer' dealt with in TTrain::Update()
            else if(ActionVectorEntryPtr->Command == "F-nshs") NewShuttleFromNonRepeatService(0);
            else if(ActionVectorEntryPtr->Command == "Frh-sh") RepeatShuttleOrRemainHere(0);
            else if(ActionVectorEntryPtr->Command == "Fns-sh") RepeatShuttleOrNewNonRepeatService(0);
    /*
    F-nshs (FNSShuttle) = Finish New Service (Shuttle) = finish, form new shuttle service in same direction, details =
        shuttle headcode (no train creation)
    Frh-sh (TimeCmdHeadCode) = Finish then restart as a shuttle using Snt-sh or Sns-sh, when all shuttle repeats done
        remain here
    Fns-sh (FSHNewService) = Finish then restart as a shuttle using Snt-sh or Sns-sh, when all shuttle repeats done
        form new service via Sns-fsh using the NonRepeatingShuttleLinkHeadCode
    */
            }
        }
    else
        {
        if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported == FailLocTooShort)
            {
            TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
            }
        }
    }
if(TrainMode == Timetable)
    {
    if(StoppedAtBuffers)
        {
        //error if buffers (& element before it) not at a location, or if buffer location different to ActionVectorEntryPtr location
        //if buffer location same as ActionVectorEntryPtr location & not Frh then error will be given for inability to depart
        AnsiString BufferLocation = Track->TrackElementAt(604, LeadElement).ActiveTrackElementName;
        if(BufferLocation == "") BufferLocation = Track->TrackElementAt(605, Track->TrackElementAt(606, LeadElement).Conn[1]).ActiveTrackElementName;
        AnsiString ExpectedLocation = ActionVectorEntryPtr->LocationName;
        if((BufferLocation == "") || (BufferLocation != ExpectedLocation))
            {
            BufferZoomOutFlashRequired = true;
            PlotTrainWithNewBackgroundColour(38, clBufferAttentionNeeded, Display);
            if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != FailUnexpectedBuffers)
                {
                TrainController->LogActionError(1, HeadCode, "", FailUnexpectedBuffers, Track->TrackElementAt(603, LeadElement).ElementID);
//SendMissedActionLogs(-1, ActionVectorEntryPtr);//-1 is a marker for send messages for all remaining entries, including Fer if present
//Drop missed actions so user can still use sig mode to get back on track
                TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = FailUnexpectedBuffers;
                }
            Utilities->CallLogPop(1020);
            return;
            }
        else if((BufferLocation != "") && (BufferLocation == ExpectedLocation) && DepartureTimeSet && !StoppedAtLocation && (TrainController->TTClockTime > ReleaseTime))
            {
            BufferZoomOutFlashRequired = true;
            if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != FailBuffersPreventingStart)
                {
                PlotTrainWithNewBackgroundColour(39, clBufferAttentionNeeded, Display);
                TrainController->LogActionError(2, HeadCode, "", FailBuffersPreventingStart, ExpectedLocation);
//                SendMissedActionLogs(-1, ActionVectorEntryPtr);//-1 is a marker for send messages for all remaining entries, including Fer if present
//Drop missed actions so user can still use sig mode to get back on track
                TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = FailBuffersPreventingStart;
                }
            Utilities->CallLogPop(1397);
            return;
            }
        }
    else BufferZoomOutFlashRequired = false;
    }
else BufferZoomOutFlashRequired = false;

if(TrainMode == Timetable)
    {
    if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported == FailUnexpectedBuffers)
        {
        TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
        }
    if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported == FailBuffersPreventingStart)
        {
        TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
        }
    }

//Pick up element next to the train front (if exists) to check for calling-on, restart after a cleared signal, or
//restart after stopped for train in front
int  NextElementPosition, NextEntryPos;
if(LeadElement > -1)//if an exit continuation then not set
    {
    if((Track->TrackElementAt(186, LeadElement).TrackType != Points) || ((LeadEntryPos != 0) && (LeadEntryPos != 2)))
        {
        LeadExitPos = Track->GetNonPointsOppositeLinkPos(LeadEntryPos);
        }
    else if((Track->TrackElementAt(187, LeadElement).TrackType == Points) && ((LeadEntryPos == 0) || (LeadEntryPos == 2)))
	    {
    	if(Track->TrackElementAt(188, LeadElement).Attribute == 0) LeadExitPos = 1;
        else LeadExitPos = 3;
	    }
    NextElementPosition = Track->TrackElementAt(189, LeadElement).Conn[LeadExitPos];
    NextEntryPos = Track->TrackElementAt(190, LeadElement).ConnLinkPos[LeadExitPos];
    }
else
    {
    NextElementPosition = -1;
    NextEntryPos = -1;
    }

if((NextElementPosition > -1) && (NextEntryPos > -1))//may be buffers or continuation
    {
    //check whether calling-on conditions met - a) approaching train has stopped at a signal; b) facing train held
    //appropriately; c) at least 1 platform available for the approaching train; d) points (if any) set
    //for direct route into platform; e) approaching train is to stop at station; f) no more facing signals between train and
    //platform; and g) signal must be within 15 track elements of the stop platform
    if(TrainMode == Timetable)
        {
        if(CallingOnAllowed(0))
            {
            CallingOnFlag = true;
            PlotTrainWithNewBackgroundColour(1, clCallOnBackground, Display);//calling-on background
            }
        else
            {
            if(CallingOnFlag)
                {
                PlotTrainWithNewBackgroundColour(2, clSignalStopBackground, Display);
                }
            CallingOnFlag = false;
            }
        }
    if(StoppedAtSignal && ((Track->TrackElementAt(191, NextElementPosition).Attribute > 0) || AllowedToPassRedSignal))
        {
        //reset PassRedSignal when reached half-way point in next element, if reset here then SetTrainMovementValues
        //sets StoppedAtSignal again & train doesn't move
        StoppedAtSignal = false;
        //need to recalculate exit times since old entry time expired. Straddle now at MidLag with front of train on MidElement
        //hence use MidElement for the calculation so same as would have been used if signal not red, when Straddle was
        //LeadMidLag and front of train was on LeadElement (after the current move)
        PlotTrainWithNewBackgroundColour(36, clNormalBackground, Display);
        EntrySpeed = 0;
        EntryTime = TrainController->TTClockTime;
        FirstHalfMove = true;
        SetTrainMovementValues(4, NextElementPosition, NextEntryPos);//NextElement is the element to be entered
        }
    if(StoppedForTrainInFront)
        {
        if(ClearToNextSignal(0))
            {
            StoppedForTrainInFront = false;
            BeingCalledOn = false;
            EntrySpeed = 0;
            EntryTime = TrainController->TTClockTime;
            FirstHalfMove = true;
            SetTrainMovementValues(16, NextElementPosition, NextEntryPos);
            }
        else
            {
            Utilities->CallLogPop(1097);
            return;
            }
        }
    }

if((Straddle == MidLag) && (LeadElement != -1))
//later check only for Straddle == LeadMid, so need this check here for initial train start
    {
    CheckAndCancelRouteForWrongEndEntry(1, LeadElement, LeadEntryPos);
    }

/*Logic below as follows: This check is made to allow a restart if had StoppedAtLocation or StoppedForTrainInFront or
both but potentially able to restart (i.e. not at buffers, not crashed, not derailed, not held at location, departure
time due, no train in front now & no other stop condition).  Note that can be StoppedForTrainInFront when not at a
location since this is set in SetTrainMovementValues whenever a train has zero EntrySpeed and there is a train in front,
which could be when start as Snt.
If StoppedForTrainInFront but not StoppedAtLocation then need to set TRSTime high so pink not plotted, and ReleaseTime
low so can restart if appropriate.  BeingCalledOn was set so that when train stopped at a station it wouldn't restart
until the line was clear of trains up to the next signal.  Hence check whether BeingCalledOn & if so set
StoppedForTrainInFront, this ensures two things - that the restart check is carried is out at each cycle and also that
a restart won't happen until the line is clear to the next signal, regardless of whether or not the ReleaseTime has been
reached.
Then check if TRS time reached & change background to pink if so, & check if release time reached & if so change
background to white and clear StoppedAtLocation.  Then make check of station name, and recheck StoppedForTrainInFront,
if it's set check if ClearToNextSignal and if so clear StoppedForTrainInFront & BeingCalledOn.  If not ClearToNextSignal
then return.  If either not StoppedForTrainInFront or ClearToNextSignal then restart, calling SetTrainMovementValues &
sending a message to the performancelog.
*/

if(TrainMode == Timetable)
    {
    if(StoppedAtLocation && !StoppedAtBuffers && !Crashed && !Derailed && !HoldAtLocationInTTMode)
        {
        if(BeingCalledOn) StoppedForTrainInFront = true;
        if(TrainController->TTClockTime >= TRSTime)
            {
            PlotTrainWithNewBackgroundColour(19, clTRSBackground, Display);//light pink
            }
        if(TrainController->TTClockTime >= ReleaseTime)
            {
            PlotTrainWithNewBackgroundColour(20, clNormalBackground, Display);
            AnsiString StationName;
            if(Track->TrackElementAt(193, LeadElement).ActiveTrackElementName != "")
                {
                StationName = Track->TrackElementAt(194, LeadElement).ActiveTrackElementName;
                }
            else if(Track->TrackElementAt(195, MidElement).ActiveTrackElementName != "")
                {
                StationName = Track->TrackElementAt(196, MidElement).ActiveTrackElementName;
                }
            else
                {
                throw Exception("Error - Stopped at through station but neither lead nor mid elements have a name");
                }
            EntrySpeed = 0;
            EntryTime = TrainController->TTClockTime;
            int NextElementPosition = Track->TrackElementAt(199, LeadElement).Conn[Track->GetNonPointsOppositeLinkPos(LeadEntryPos)];
            int NextEntryPos = Track->TrackElementAt(200, LeadElement).ConnLinkPos[Track->GetNonPointsOppositeLinkPos(LeadEntryPos)];
            FirstHalfMove = true;
            StoppedAtLocation = false;

            if((NextElementPosition > -1) && (NextEntryPos > -1))//condition check added for SloughIECC error reported by James U
                {
                if((Track->TrackElementAt(720, NextElementPosition).Config[Track->GetNonPointsOppositeLinkPos(NextEntryPos)] == Signal) &&
                        (Track->TrackElementAt(721, NextElementPosition).Attribute == 0))
                    {
                    StoppedAtSignal = true;
                    PlotTrainWithNewBackgroundColour(41, clSignalStopBackground, Display);
    //                TrainController->LogActionError(40, HeadCode, "", SignalHold, Track->TrackElementAt(755, NextElementPosition).ElementID);
                    }
                }
            if(ActionVectorEntryPtr->FormatType == TimeTimeLoc)
                {
                TimeTimeLocArrived = false;
                LogAction(27, HeadCode, "", Depart, StationName, ActionVectorEntryPtr->DepartureTime, false);//no warning for TimeTimeLoc departure
                }
            else
                {
                LogAction(6, HeadCode, "", Depart, StationName, ActionVectorEntryPtr->DepartureTime, ActionVectorEntryPtr->Warning);
                }
            DepartureTimeSet = false;
            //no need to set LastActionTime for a departure
            ActionVectorEntryPtr++;//advance pointer beyond departure action - (this line (& LogAction) used to be at the end -  see
                                   //note
/*
Note:  If train stops at station after call on with a TimeTimeLoc loaded, and before the normal stop point, then when
SetTrainMovementValues called it assumes a stop at the stop point because the ActionVectorEntryPtr points to a name
when NameInTimetableBeforeCDT is called and the stop positions are valid.  So next element train movement is based on
this calculation.  However, when the departure time check is made (it is during this function when SetTrainMovementValues
is called), the ActionVectorEntryPtr is advanced at the end past the departure location, so at the next element when
SetTrainMovementValues is called again, all is normal, i.e. the train doesn't stop again at the location.  But to cure
the problem move the ActionVectorEntryPtr increment to before SetTrainMovementValues.
*/
            if(Track->TrackElementAt(201, LeadElement).TrackType == Buffers)
                {
                StoppedAtBuffers = true;
                }
            else//if buffers don't set values
                {
                if(Track->TrackElementAt(724, LeadElement).TrackType != Continuation)
                    {
                    SetTrainMovementValues(12, NextElementPosition, NextEntryPos);//NextElement is the element to be entered
                    }
                else
                    {
                    SetTrainMovementValues(13, LeadElement, LeadEntryPos);//use LeadElement for an exit continuation
                    }
                }
            }
        }
    }

if(Straddle == LeadMidLag)
    {
    if(TrainController->TTClockTime < ExitTimeFull)
        {
        Utilities->CallLogPop(654);
        return;
        }
    }
else
    {
    if(TrainController->TTClockTime < ExitTimeHalf)
        {
        Utilities->CallLogPop(655);
        return;
        }
    }

if((LeadElement > -1) && (MidElement > -1))
    {
    if((TrainMode == Signaller) && (Track->TrackElementAt(789, LeadElement).TrackType == Continuation))
        {//don't allow to stop if exiting at a continuation as causes problems if try to change direction
         //if entering at continuation & LeadElement is a continuation then MidElement will be -1
        SignallerStoppingFlag = false;
        StepForwardFlag = false;
        }
    }

if(Stopped())//this is what prevents another movement if the train is stopped, and stop conditions are only set when
//the train is fully on two elements - i.e. when this routine is entered with Straddle == LeadMidLag
    {
    BrakeRate = 0;
    Utilities->CallLogPop(656);
    return;
    }

//here when ready for next move

//check for train in front & if so stop at next access (when train fully on element next to train)
if((TrainMode == Signaller) && (Straddle == LeadMidLag))
//SetTrainMovementValues brakes & stops signaller mode train for a train in front using local
//variable TrainInFrontInSignallerModeFlag
    {
    if(LeadElement > -1)
        {
        int NextPos = Track->TrackElementAt(649, LeadElement).Conn[LeadExitPos];
        int NextEntryPos = Track->TrackElementAt(650, LeadElement).ConnLinkPos[LeadExitPos];
        if(Track->OtherTrainOnTrack(1, NextPos, NextEntryPos, TrainID))//true if another train on NextEntryPos track whether bridge or not
            {
            StoppedForTrainInFront = true;
            }
        else
            {
            StoppedForTrainInFront = false;
            }
        }
    }

if((Straddle == LeadMid) && SPADFlag)//give message + plot background when ready to move half past the signal
    {
    if(NextElementPosition > -1)
        {
        if((Track->TrackElementAt(662, NextElementPosition).Config[Track->GetNonPointsOppositeLinkPos(NextEntryPos)] == Signal) &&
                (Track->TrackElementAt(663, NextElementPosition).Attribute == 0))
            {
            AnsiString LocID = AnsiString(Track->TrackElementAt(664, NextElementPosition).ElementID);
            TrainController->LogActionError(3, HeadCode, "", FailSPAD, LocID);
            //if goes past 2 signals then give message twice
            PlotTrainWithNewBackgroundColour(17, clSPADBackground, Display);//Yellow
            }
        }
    }

if(Straddle == LeadMidLag)//During this function train moves fully onto 2 elements, Lead & Mid, so set next 2 moves from here
//for the element after Lead
    {
    //if SPADFlag set allow to keep moving until signal obscured before setting background colour, & stop only when ExitSpeedFull is 0
    if(SPADFlag)
        {
        if(ExitSpeedFull == 0)
            {
            StoppedAfterSPAD = true;//but don't want to stop until have moved fully onto element, hence stop test is before this check
            }
        }

    if((TrainMode == Signaller) && (SignallerStoppingFlag || StepForwardFlag))
        {
        if(ExitSpeedFull == 0)
            {
            //only reach here when stopped, because SetTrainMovementValues called after this (i.e. ExitSpeedFull becomes 0 if not 0 now
            //after this test), and Straddle == LeadMidLag so not accessed at the half-move point, hence only reached at the full move
            //point when the speed is 0.  So, colour change won't occur until fully stopped (early in UpdateTrain()), and the log message
            //is sent at the right time and once only.
            SignallerStopped = true;//but don't want to stop until have moved fully onto element, hence stop test is before this check
            StepForwardFlag = false;
            SignallerStoppingFlag = false;
            TTrackElement TE;
            AnsiString Loc = "";
            bool LocNamed = false;
            if(LeadElement > -1)
                {
                TE = Track->TrackElementAt(782, LeadElement);
                if(TE.ActiveTrackElementName != "")
                    {
                    Loc = TE.ActiveTrackElementName;
                    LocNamed = true;
                    }
                else
                    {
                    Loc = "track element " + TE.ElementID;
                    }
                }
            if((MidElement > -1) && !LocNamed)
                {
                TE = Track->TrackElementAt(783, MidElement);
                if(TE.ActiveTrackElementName != "")
                    {
                    Loc = TE.ActiveTrackElementName;
                    LocNamed = true;
                    }
                else if(Loc == "")
                    {
                    Loc = "track element " + TE.ElementID;
                    }
                }
            if(Loc == "")
                {
                Loc = "outside railway";//must have stopped after left at a continuation (because both lead & mid == -1)
                }
            else Loc = "at " + Loc;
            LogAction(30, HeadCode, "", SignallerStop, Loc, TrainController->TTClockTime, false);//false for warning
            }
        }
    if(LeadElement > -1)//if an exit continuation then not set
        {
        if((Track->TrackElementAt(202, LeadElement).TrackType != Points) || ((LeadEntryPos != 0) && (LeadEntryPos != 2)))
            {
            LeadExitPos = Track->GetNonPointsOppositeLinkPos(LeadEntryPos);
            }
        else if((Track->TrackElementAt(203, LeadElement).TrackType == Points) && ((LeadEntryPos == 0) || (LeadEntryPos == 2)))
    	    {
        	if(Track->TrackElementAt(204, LeadElement).Attribute == 0) LeadExitPos = 1; else LeadExitPos = 3;
	        }
        NextElementPosition = Track->TrackElementAt(205, LeadElement).Conn[LeadExitPos];
        NextEntryPos = Track->TrackElementAt(206, LeadElement).ConnLinkPos[LeadExitPos];
        }
    else
        {
        NextElementPosition = -1;
        NextEntryPos = -1;
        }

    EntryTime = TrainController->TTClockTime;
    EntrySpeed = ExitSpeedFull;
    FirstHalfMove = true;

    if((NextElementPosition > -1) && (NextEntryPos > -1))//may be buffers or continuation
        {
        if((Track->TrackElementAt(207, NextElementPosition).Config[Track->GetNonPointsOppositeLinkPos(NextEntryPos)] == Signal) &&
            (Track->TrackElementAt(208, NextElementPosition).Attribute == 0) && (ExitSpeedFull < 1) && !StoppedAtLocation)
            {
            StoppedAtSignal = true;
            PlotTrainWithNewBackgroundColour(34, clSignalStopBackground, Display);
//            TrainController->LogActionError(41, HeadCode, "", SignalHold, Track->TrackElementAt(756, NextElementPosition).ElementID);
            }
        }

    if(!Stopped())
        {
        if((NextElementPosition > -1) && (NextEntryPos > -1))//may be buffers or continuation (skip SetTrainMovementValues if buffers, if
                                                             //a stop element that isn't buffers - e.g. station, then will skip the calcs
                                                             //during SetTrainMovementValues to avoid trying to divide by zero - see that
                                                             //function for fuller explanation
            {
            SetTrainMovementValues(8, NextElementPosition, NextEntryPos);//NextElement is the element to be entered
            }
        //follow the continuation exits:-
        else if((LeadElement > -1) && (Track->TrackElementAt(209, LeadElement).TrackType == Continuation))
            {
            SetTrainMovementValues(9, LeadElement, LeadEntryPos);//Use LeadElement for calcs if lead is a continuation
            }
        else if((MidElement > -1) && (Track->TrackElementAt(210, MidElement).TrackType == Continuation))
            {
            SetTrainMovementValues(10, MidElement, MidEntryPos);//Use MidElement for calcs if mid is a continuation
            }
        else if((LagElement > -1) && (Track->TrackElementAt(211, LagElement).TrackType == Continuation))
            {
            SetTrainMovementValues(11, LagElement, LagEntryPos);//Use LagElement for calcs if lag is a continuation
            }
        }
    }

//straddle ONLY changed here, check if 'LeadMid' first & if so ready for updating Elements
if(Straddle == LeadMid)
    {
    AllowedToPassRedSignal = false;//if had been allowed to pass then at this point it will move half onto signal so can be reset
//    if(LagElement > -1) ResetTrainElementID(LagElement, LagEntryPos);//train fully off old LagElement so can clear TrainOnElement flags - no, reset at earlier call when lag moves off element
    if(DerailPending)//set during last GetLeadElement, but only act on it when train fully on offending point
                //i.e. next time Straddle reaches LeadMid
        {
        Derailed = true;
        DerailPending = false;
        RailGraphics->ChangeForegroundColour(21, HeadCodePosition[0], HeadCodePosition[0], clB0G0R0, BackgroundColour);
        PlotTrainWithNewBackgroundColour(6, clDerailedBackground, Display);
        TrainController->LogActionError(4, HeadCode, "", FailDerailed, Track->TrackElementAt(595,LeadElement).ElementID);
        Utilities->CallLogPop(657);
        return;
        }
    LagElement = MidElement;
    LagEntryPos = MidEntryPos;
    LagExitPos = MidExitPos;
    MidElement = LeadElement;
    MidEntryPos = LeadEntryPos;
    MidExitPos = LeadExitPos;
    Straddle = MidLag;//train now fully on the updated Lag & Mid, the front segment is going to move onto the new
                      //LeadElement during this function (note that if stopped at signal then won't get this far)
    if(LeadElement > -1)
        {
        if(ContinuationExit(1, LeadElement, LeadExitPos))//i.e an exit continuation only
                //if don't exclude entry continuations then can't progress past it
            {
            LeadElement = -1;
            }
        else
            {
            GetLeadElement(0);//sets or resets DerailPending & StoppedAtSignal, and sets LeadElement values
            CheckAndCancelRouteForWrongEndEntry(2, LeadElement, LeadEntryPos);
            if(Stopped())
                {
                Utilities->CallLogPop(658);
                return;//i.e. don't move forward one step if next element is a red signal
                }
            }
        }
    }

if(Straddle == LeadMidLag)
    {//remove route elements - this section moved from below, was under LagElement > -1 condition but needs to cover LagElement == -1
    if(AllRoutes->GetRouteTypeAndGraphics(2, LeadElement, LeadEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::NotAutoSigsRoute)//Trains may not be in a route
    //If Straddle = LeadMidLag at this point then the train is going to move fully off the existing Lag & fully onto existing Lead
    //element during this function
        {
        //NB if LeadElement == -1 then the above test returns false
        int TempH = Track->TrackElementAt(213, LeadElement).HLoc;
        int TempV = Track->TrackElementAt(214, LeadElement).VLoc;
        int TempELink = Track->TrackElementAt(215, LeadElement).Link[LeadEntryPos];
        TAllRoutes::TRouteElementPair FirstPair, SecondPair;
        FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(10, TempH, TempV, SecondPair);
        if((FirstPair.first > -1) && (AllRoutes->GetFixedRouteAt(143, FirstPair.first).GetFixedPrefDirElementAt(153, FirstPair.second).GetELink() == TempELink))
            {
            AllRoutes->RemoveRouteElement(10, TempH, TempV, TempELink);
            }
        else if((SecondPair.first > -1) && (AllRoutes->GetFixedRouteAt(144, SecondPair.first).GetFixedPrefDirElementAt(154, SecondPair.second).GetELink() == TempELink))
            {
            AllRoutes->RemoveRouteElement(11, TempH, TempV, TempELink);
            }
        }

    if(AllRoutes->GetRouteTypeAndGraphics(3, MidElement, MidEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::NotAutoSigsRoute)//Trains may not be in a route
        {
        int TempH = Track->TrackElementAt(216, MidElement).HLoc;
        int TempV = Track->TrackElementAt(217, MidElement).VLoc;
        int TempELink = Track->TrackElementAt(218, MidElement).Link[MidEntryPos];
        TAllRoutes::TRouteElementPair FirstPair, SecondPair;
        FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(11, TempH, TempV, SecondPair);
        if((FirstPair.first > -1) && (AllRoutes->GetFixedRouteAt(145, FirstPair.first).GetFixedPrefDirElementAt(155, FirstPair.second).GetELink() == TempELink))
            {
            AllRoutes->RemoveRouteElement(12, TempH, TempV, TempELink);
            }
        else if((SecondPair.first > -1) && (AllRoutes->GetFixedRouteAt(146, SecondPair.first).GetFixedPrefDirElementAt(156, SecondPair.second).GetELink() == TempELink))
            {
            AllRoutes->RemoveRouteElement(13, TempH, TempV, TempELink);
            }
        }

    if(AllRoutes->GetRouteTypeAndGraphics(4, LagElement, LagEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::NotAutoSigsRoute)//Trains may not be in a route
        {
        int TempH = Track->TrackElementAt(219, LagElement).HLoc;
        int TempV = Track->TrackElementAt(220, LagElement).VLoc;
        int TempELink = Track->TrackElementAt(221, LagElement).Link[LagEntryPos];
        TAllRoutes::TRouteElementPair FirstPair, SecondPair;
        FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(12, TempH, TempV, SecondPair);
        if((FirstPair.first > -1) && (AllRoutes->GetFixedRouteAt(147, FirstPair.first).GetFixedPrefDirElementAt(157, FirstPair.second).GetELink() == TempELink))
            {
            AllRoutes->RemoveRouteElement(14, TempH, TempV, TempELink);
            }
        else if((SecondPair.first > -1) && (AllRoutes->GetFixedRouteAt(148, SecondPair.first).GetFixedPrefDirElementAt(158, SecondPair.second).GetELink() == TempELink))
            {
            AllRoutes->RemoveRouteElement(15, TempH, TempV, TempELink);
            }

        AllRoutes->CheckMapAndRoutes(8);//test
        }
    }

if(LagElement > -1)//not entering at a continuation so can deal with train leaving the lag element
    {
    if(Straddle == LeadMidLag)
//below are the actions required only when the train is not straddling 3 elements (will move onto 2 elements if enters with LeadMidLag)
        {
        ResetTrainElementID(4, LagElement, LagEntryPos);
//amended below so route elements removed for the complete train (for NotAutoSigsRoutes), so train never standing on a route once it
//starts moving, covers for eliminating route when train reaches buffers, and prevents odd route segments when route extended while
//straddling 3 elements (formerly the last segment was replotted as a route & stayed plotted

/*
//move this section (route element removal) out of the LagElement > -1 condition, as need it when Straddle == LeadMidLag regardless of
//LagElement value
        if(AllRoutes->GetRouteTypeAndGraphics(, LeadElement, LeadEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::NotAutoSigsRoute)//Trains may not be in a route
        //If Straddle = LeadMidLag at this point then the train is going to move fully off the existing Lag & fully onto existing Lead
        //element during this function
            {
    //        if(Track->TrackElementAt(, LeadElement).TrackType == Buffers)//only remove LeadElement from route if buffers (LeadElement can never be on departing buffers)
    //drop above as if truncate nonsig route to & including a signal then signal red, train stops, route element that train front standing on
    //still present, so can't select signal as it's now adjacent to the start of another route!
                {
                int TempH = Track->TrackElementAt(, LeadElement).HLoc;
                int TempV = Track->TrackElementAt(, LeadElement).VLoc;
                int TempELink = Track->TrackElementAt(, LeadElement).Link[LeadEntryPos];
                TAllRoutes::TRouteElementPair FirstPair, SecondPair;
                FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(, TempH, TempV, SecondPair);
                if((FirstPair.first > -1) && (AllRoutes->GetFixedRouteAt(, FirstPair.first).GetFixedPrefDirElementAt(, FirstPair.second).GetELink() == TempELink))
                    {
                    AllRoutes->RemoveRouteElement(, TempH, TempV, TempELink);
                    }
                else if((SecondPair.first > -1) && (AllRoutes->GetFixedRouteAt(, SecondPair.first).GetFixedPrefDirElementAt(, SecondPair.second).GetELink() == TempELink))
                    {
                    AllRoutes->RemoveRouteElement(, TempH, TempV, TempELink);
                    }
                }
            }

        if(AllRoutes->GetRouteTypeAndGraphics(, MidElement, MidEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::NotAutoSigsRoute)//Trains may not be in a route
            {
            int TempH = Track->TrackElementAt(, MidElement).HLoc;
            int TempV = Track->TrackElementAt(, MidElement).VLoc;
            int TempELink = Track->TrackElementAt(, MidElement).Link[MidEntryPos];
            TAllRoutes::TRouteElementPair FirstPair, SecondPair;
            FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(, TempH, TempV, SecondPair);
            if((FirstPair.first > -1) && (AllRoutes->GetFixedRouteAt(, FirstPair.first).GetFixedPrefDirElementAt(, FirstPair.second).GetELink() == TempELink))
                {
                AllRoutes->RemoveRouteElement(, TempH, TempV, TempELink);
                }
            else if((SecondPair.first > -1) && (AllRoutes->GetFixedRouteAt(, SecondPair.first).GetFixedPrefDirElementAt(, SecondPair.second).GetELink() == TempELink))
                {
                AllRoutes->RemoveRouteElement(, TempH, TempV, TempELink);
                }
            }

        if(AllRoutes->GetRouteTypeAndGraphics(, LagElement, LagEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::NotAutoSigsRoute)//Trains may not be in a route
            {
            int TempH = Track->TrackElementAt(, LagElement).HLoc;
            int TempV = Track->TrackElementAt(, LagElement).VLoc;
            int TempELink = Track->TrackElementAt(, LagElement).Link[LagEntryPos];
            TAllRoutes::TRouteElementPair FirstPair, SecondPair;
            FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(, TempH, TempV, SecondPair);
            if((FirstPair.first > -1) && (AllRoutes->GetFixedRouteAt(, FirstPair.first).GetFixedPrefDirElementAt(, FirstPair.second).GetELink() == TempELink))
                {
                AllRoutes->RemoveRouteElement(, TempH, TempV, TempELink);
                }
            else if((SecondPair.first > -1) && (AllRoutes->GetFixedRouteAt(, SecondPair.first).GetFixedPrefDirElementAt(, SecondPair.second).GetELink() == TempELink))
                {
                AllRoutes->RemoveRouteElement(, TempH, TempV, TempELink);
                }

            AllRoutes->CheckMapAndRoutes();//test
            }

*/
        TPrefDirElement PrefDirElement;
        //plot locked route marker if appropriate (i.e. if a locked AutoSigs route) but only when train leaves element completely
        //as this is a 16x16 graphic
        if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(6, LagElement, LagExitPos, PrefDirElement, LockedVectorNumber))
            {
            Display->PlotOutput(22, (Track->TrackElementAt(222, LagElement).HLoc * 16),
                        (Track->TrackElementAt(223, LagElement).VLoc * 16), RailGraphics->LockedRouteCancelPtr[PrefDirElement.GetELink()]);
            }

        if(ContinuationExit(2, LagElement, LagExitPos))
            {
            int RouteNumber;
            TrainGone = true;//flag to indicate train to be deleted - outside this function
            if(AllRoutes->GetRouteTypeAndNumber(10, LagElement, LagExitPos, RouteNumber) == TAllRoutes::AutoSigsRoute)
                {
                TTrainController::TContinuationAutoSigEntry ContinuationAutoSigEntry;
                ContinuationAutoSigEntry.RouteNumber = RouteNumber;
                //calc distance from & inc last signal to exit
                int LastElement = LagElement, LastExitPos = LagExitPos, CumDistance = 0;
                int NewLastElement = 0, NewLastExitPos = 0;
                //need above because can't change LastElement & LastExitPos until both new values obtained
                while((Track->TrackElementAt(684, LastElement).Config[LastExitPos] != Signal) && (CumDistance < 1200))
                    {
                    if(LastExitPos < 2) CumDistance+= Track->TrackElementAt(685, LastElement).Length01;
                    else CumDistance+= Track->TrackElementAt(686, LastElement).Length23;
                    NewLastElement = Track->TrackElementAt(687, LastElement).Conn[Track->GetNonPointsOppositeLinkPos(LastExitPos)];
                    if(NewLastElement == -1)//this will catch buffers or any other connection failure
                        {
                        throw Exception("Error, Connection = -1 in Continuation loop in UpdateTrain");
                        }
                    NewLastExitPos = Track->TrackElementAt(688, LastElement).ConnLinkPos[Track->GetNonPointsOppositeLinkPos(LastExitPos)];
                    if(NewLastExitPos == -1)
                        {
                        throw Exception("Error, ConnLinkPos = -1 in Continuation loop in UpdateTrain");
                        }
                    LastElement = NewLastElement;
                    LastExitPos = NewLastExitPos;
                    }
                //if at signal add this in too
                if(CumDistance < 1200)
                    {
                    CumDistance+= Track->TrackElementAt(689, LastElement).Length01;//only need 01 for signal
                    }
                //now have distance including the signal, if >=1200m use 100m (for a signal immediately after the continuation)
                //else use 1200m - CumDistance
                int FirstDistance = 0;
                if(CumDistance >= 1200) FirstDistance = 100;
                else FirstDistance = 1200 - CumDistance;
                if(FirstDistance < 100) FirstDistance = 100;//don't allow < 100
                //can now calc the time delays in seconds - FirstDelay, SecondDelay & ThirdDelay, these are doubles
                ContinuationAutoSigEntry.FirstDelay = 3.6 * double(FirstDistance)/ExitSpeedFull;//speed in km/h so mult by 3.6 to bring to secs
                ContinuationAutoSigEntry.SecondDelay = ContinuationAutoSigEntry.FirstDelay + 4320.0/ExitSpeedFull; //4320.0 = 3.6 * 1200, .0 to make it a double
                ContinuationAutoSigEntry.ThirdDelay = ContinuationAutoSigEntry.SecondDelay + 4320.0/ExitSpeedFull;
                ContinuationAutoSigEntry.AccessNumber = 0;
                ContinuationAutoSigEntry.PassoutTime = TrainController->TTClockTime;
                if(!TrainController->ContinuationAutoSigVector.empty())
                    {
                    TTrainController::TContinuationAutoSigVectorIterator VectorIT;
                    for(VectorIT = TrainController->ContinuationAutoSigVector.begin(); VectorIT != TrainController->ContinuationAutoSigVector.end(); VectorIT++)
                        {
                        if(VectorIT->RouteNumber == RouteNumber)
                            {
                            //another train has passed out of same route so erase earlier entry
                            TrainController->ContinuationAutoSigVector.erase(VectorIT);
                            break;
                            }
                        }
                    }
                TrainController->ContinuationAutoSigVector.push_back(ContinuationAutoSigEntry);
                }
            PlotBackgroundGraphic(14, 3);//need to plot this as returning early so will miss the later plot
            Display->Update(); //need to keep this since Update() not called for PlotSmallOutput as too slow
            Utilities->CallLogPop(659);
            return;
            }
        //above covers for exiting at continuation, need XLinkPos check to exclude entering at a continuation
        if((LeadElement > -1) && (Track->TrackElementAt(224, LeadElement).Config[LeadExitPos] == Signal))//changed to lead so reset early
            {
            Track->TrackElementAt(225, LeadElement).Attribute = 0; //red
            Track->TrackElementAt(671, LeadElement).CallingOnSet = false;
            //don't plot if zoomed out
            if(!Display->ZoomOutFlag) Track->PlotSignal(4, Track->TrackElementAt(226, LeadElement), Display);
            //covers signal resetting in same direction
            }
        if(Track->TrackElementAt(521, LagElement).Config[LagExitPos] == Signal)
            {
            if(AllRoutes->GetRouteTypeAndGraphics(5, LagElement, LagEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::AutoSigsRoute)
                {
                Display->PlotOutput(23, Track->TrackElementAt(227, LagElement).HLoc * 16, Track->TrackElementAt(228, LagElement).VLoc * 16, EXGraphicPtr);
                Display->PlotOutput(24, Track->TrackElementAt(229, LagElement).HLoc * 16, Track->TrackElementAt(230, LagElement).VLoc * 16, EntryDirectionGraphicPtr);
                TPrefDirElement PrefDirElement;
                //plot locked route marker if appropriate, but only when train leaves element completely as this is a 16x16 graphic (OK - Straddle == LeadMidLag)
                if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(8, LagElement, LagExitPos, PrefDirElement, LockedVectorNumber))
                    {
                    Display->PlotOutput(25, (Track->TrackElementAt(231, LagElement).HLoc * 16),
                                (Track->TrackElementAt(232, LagElement).VLoc * 16), RailGraphics->LockedRouteCancelPtr[PrefDirElement.GetELink()]);
                    }
                if(!(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(9, LagElement, LagExitPos, PrefDirElement, LockedVectorNumber)))
                    {
                    AllRoutes->SetTrailingSignalsOnAutoSigsRoute(0, LagElement, LagExitPos);
                    }
                }
            }
        else if((LeadElement > -1) && (Track->TrackElementAt(233, LeadElement).TrackType == SignalPost))
            {
            Track->TrackElementAt(234, LeadElement).Attribute = 0; //red
            Track->TrackElementAt(672, LeadElement).CallingOnSet = false;
            //don't plot if zoomed out
            if(!Display->ZoomOutFlag) Track->PlotSignal(5, Track->TrackElementAt(235, LeadElement), Display);
            //covers signal passed in opposite direction - replot as red, regardless of what it was before, though should already have been red
            }
        else if(Track->TrackElementAt(522, LagElement).TrackType == SignalPost)
            {
            if(AllRoutes->GetRouteTypeAndGraphics(6, LagElement, LagEntryPos, EXGraphicPtr, EntryDirectionGraphicPtr) == TAllRoutes::AutoSigsRoute)
                {
                Display->PlotOutput(26, Track->TrackElementAt(236, LagElement).HLoc * 16, Track->TrackElementAt(237, LagElement).VLoc * 16, EXGraphicPtr);
                Display->PlotOutput(27, Track->TrackElementAt(238, LagElement).HLoc * 16, Track->TrackElementAt(239, LagElement).VLoc * 16, EntryDirectionGraphicPtr);
                }
            TPrefDirElement PrefDirElement;
            //plot locked route marker if appropriate, but only when train leaves element completely as this is a 16x16 graphic (OK - Straddle == LeadMidLag)
            if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(10, LagElement, LagExitPos, PrefDirElement, LockedVectorNumber))
                {
                Display->PlotOutput(28, (Track->TrackElementAt(240, LagElement).HLoc * 16),
                            (Track->TrackElementAt(241, LagElement).VLoc * 16), RailGraphics->LockedRouteCancelPtr[PrefDirElement.GetELink()]);
                }
            }
        }

//below are the actions required at both half moves for LagElement > -1
    PlotBackgroundGraphic(12, 3);

    //if was in locked route but has timed out when train leaves then plot the normal track graphic over the route graphic that is
    //still in BackgroundGraphic[3], if wasn't in a route then will just replot the same BackgroundGraphic
    //need to do this for each half element

    TPrefDirElement PrefDirElement;
    if(!(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(7, LagElement, LagExitPos, PrefDirElement, LockedVectorNumber)))
        {
        int RouteNumber;//holder for call below - not used
        if(AllRoutes->GetRouteTypeAndNumber(9, LagElement, LagExitPos, RouteNumber) == TAllRoutes::NoRoute)
            {
            if(Utilities->clTransparent == TColor(0xFFFFFF))//change to black for a white background
                {
                RailGraphics->ChangeSpecificColour(0, BackgroundPtr[3], BackgroundPtr[3], clB5G3R0, clB0G0R0);//only applies for AutoSigs Route,
                }
            else//change to white for a dark background
                {
                RailGraphics->ChangeSpecificColour(2, BackgroundPtr[3], BackgroundPtr[3], clB5G3R0, clB5G5R5);//only applies for AutoSigs Route,
                }
            PlotBackgroundGraphic(13, 3);
            }
        }

    PlotAlternativeTrackRouteGraphic(1, LagElement, LagEntryPos, HOffset[3], VOffset[3]);
    //above in case train just moving off a bridge & either alternative track in a route - need to keep its route colour,
    //or a train on the opposite track - needs to be replotted
    }

//update all array values
HOffset[3] = HOffset[2];    HOffset[2] = HOffset[1];    HOffset[1] = HOffset[0];
VOffset[3] = VOffset[2];    VOffset[2] = VOffset[1];    VOffset[1] = VOffset[0];
Graphics::TBitmap *TempPtr = BackgroundPtr[3];
BackgroundPtr[3] = BackgroundPtr[2]; BackgroundPtr[2] = BackgroundPtr[1]; BackgroundPtr[1] = BackgroundPtr[0];
BackgroundPtr[0] = TempPtr;

//update headcode graphics depending on Lead entry value
if(LeadElement > -1)//if Lead is -1 then stays as is
    {
    if(LowEntryValue(Track->TrackElementAt(242, LeadElement).Link[LeadEntryPos]))
        {
        for(int x=0;x<4;x++)
            {
            HeadCodePosition[x] = HeadCodeGrPtr[3 - x];
            }
        }
    else
        {
        for(int x=0;x<4;x++)
            {
            HeadCodePosition[x] = HeadCodeGrPtr[x];
            }
        }
    }

if(TrainMode == Timetable)
    {
    RailGraphics->ChangeForegroundColour(22, HeadCodePosition[0], FrontCodePtr, clFrontCodeTimetable, BackgroundColour);//red
    }
else
    {
    RailGraphics->ChangeForegroundColour(23, HeadCodePosition[0], FrontCodePtr, clFrontCodeSignaller, BackgroundColour);//blue
    }
HeadCodePosition[0] = FrontCodePtr;

//plot new seg [0] on Lead & [2] on Mid ([2] always on Mid)
if(LeadElement > -1)
    {
    if(Straddle == MidLag)
        {
        GetOffsetValues(5, HOffset[0], VOffset[0], Track->TrackElementAt(243, LeadElement).Link[LeadEntryPos]);
        //pick up new background bitmap [0]
        PickUpBackgroundBitmap(4, HOffset[0], VOffset[0], LeadElement, LeadEntryPos, BackgroundPtr[0]);
        int LeadElementTrainID = Track->TrackElementAt(244, LeadElement).TrainIDOnElement;
        if((LeadElementTrainID > -1) && (LeadElementTrainID != TrainID))//check if own ID for entry at continuation, else crashes into itself!
            {
            //OK if crossing on a bridge
            int OtherTrainEntryPos = TrainController->EntryPos(0, LeadElementTrainID, LeadElement);
            if(OtherTrainEntryPos == -1)
                {
                throw Exception("Error - OtherTrainEntryPos not set");
                }
            if((Track->TrackElementAt(246, LeadElement).TrackType != Bridge) || (LeadEntryPos == OtherTrainEntryPos) || //LeadEntryPos for rear end crashes
                    (LeadExitPos == OtherTrainEntryPos))//LeadExitPos for head-on crashes
                {
                TrainCrashedInto = Track->TrackElementAt(247, LeadElement).TrainIDOnElement;
                Crashed = true;//only set if Straddle = MidLag
                CallingOnFlag = false;//in case was set, need to disable call on if call on button had been pressed
                }
            }
        }
    else
        {
        GetOffsetValues(6, HOffset[0], VOffset[0], Track->TrackElementAt(248, LeadElement).Link[LeadExitPos]);
        //pick up new background bitmap [0]
        PickUpBackgroundBitmap(5, HOffset[0], VOffset[0], LeadElement, LeadEntryPos, BackgroundPtr[0]);
        }
    PlotElement[0] = LeadElement; PlotEntryPos[0] = LeadEntryPos;
    PlotTrainGraphic(12, 0, Display);
    }

if(MidElement > -1)
    {
    PlotElement[2] = MidElement; PlotEntryPos[2] = MidEntryPos;
    PlotTrainGraphic(1, 2, Display);
    }

//plot the new positions for [1] & [3] graphics -  [1] on Mid if Straddle = MidLag, on Lead if Straddle = LeadMidLag
//                                                 [3] on Lag if Straddle = MidLag, on Mid if Straddle = LeadMidLag
if(Straddle == MidLag)
    {
    if(MidElement > -1)
        {
        PlotElement[1] = MidElement; PlotEntryPos[1] = MidEntryPos;
        PlotTrainGraphic(2, 1, Display);
        }
    if(LagElement > -1)
        {
        PlotElement[3] = LagElement; PlotEntryPos[3] = LagEntryPos;
        PlotTrainGraphic(3, 3, Display);
        }
    }
else//Straddle == LeadMidLag
    {
    if(LeadElement > -1)
        {
        PlotElement[1] = LeadElement; PlotEntryPos[1] = LeadEntryPos;
        PlotTrainGraphic(4, 1, Display);
        }
    if(MidElement > -1)
        {
        PlotElement[3] = MidElement; PlotEntryPos[3] = MidEntryPos;
        PlotTrainGraphic(5, 3, Display);
        }
    }
if(Crashed)
    {
    RailGraphics->ChangeForegroundColour(0, HeadCodePosition[0], HeadCodePosition[0], clB0G0R0, BackgroundColour);
    PlotTrainWithNewBackgroundColour(7, clCrashedBackground, Display);//red
    TrainController->TrainVectorAtIdent(5, TrainCrashedInto).Crashed = true;
    TrainController->TrainVectorAtIdent(34, TrainCrashedInto).CallingOnFlag = false;//in case was set, need to disable call on if call on button had been pressed
    RailGraphics->ChangeForegroundColour(1, TrainController->TrainVectorAtIdent(6, TrainCrashedInto).HeadCodePosition[0],
                              TrainController->TrainVectorAtIdent(7, TrainCrashedInto).HeadCodePosition[0],
                              clB0G0R0, TrainController->TrainVectorAtIdent(8, TrainCrashedInto).BackgroundColour);
    TrainController->TrainVectorAtIdent(9, TrainCrashedInto).PlotTrainWithNewBackgroundColour(8, clCrashedBackground, Display);//red
    TrainController->LogActionError(5, HeadCode, TrainController->TrainVectorAtIdent(13, TrainCrashedInto).HeadCode, FailCrashed, Track->TrackElementAt(621,LeadElement).ElementID);
    Straddle = LeadMidLag;//was MidLag but plotted as LeadMidLag so change Straddle accordingly
    Display->Update();// resurrected when Update() dropped from PlotOutput etc
    Utilities->CallLogPop(660);
    return;
    }

//if(SPADFlag) PlotTrainWithNewBackgroundColour(9, clB5G0R5);//Magenta - use yellow & just plot when signal half obscured

//deal here with station stops & pass times after all replotting done but before Straddle changed
if(TrainMode == Timetable)
    {
    if(Straddle == LeadMidLag)
        {
        if((LeadElement > -1) && (MidElement > -1) && !TimetableFinished)
            {
            //NameInTimetableBeforeCDT returns the number by which the train ActionVectorEntryPtr needs to be incremented
            //to point to the location arrival entry - before a change of direction
            AnsiString LocName = Track->TrackElementAt(249, LeadElement).ActiveTrackElementName;
            bool StopRequired = false;
            int TTVPos = NameInTimetableBeforeCDT(1, LocName, StopRequired);
            if(TTVPos > -1)  //-1 if can't find it or if name is ""
                {
                //check if at buffers (no, dropped buffer check to allow to crash into buffers) or a through station stop,
                //or a station where next element contains a train or a stop signal, if so
                //stop now, note that for 2nd check, if next element is a bridge then will have stopped by now so no need
                //to test the actual track the train is on since it can't be a platform
                TTrackElement LeadTrackElement = Track->TrackElementAt(258, LeadElement);
                TTrackElement NextTrackElement;//default for now
                bool TrainAtStopLinkPos1 = (LeadTrackElement.StationEntryStopLinkPos1 == LeadEntryPos);
                bool TrainAtStopLinkPos2 = (LeadTrackElement.StationEntryStopLinkPos2 == LeadEntryPos);
                bool ForwardConnection = (LeadTrackElement.Conn[LeadExitPos] > -1);
                int NextElementEntryPos = -1;
                int NextElementExitPos = -1;
                bool TrainOnNextElement = false;
                bool StopSignalAtNextElement = false;
                if(ForwardConnection)//if no forward connection can't derive anything from it without errors
                    {
                    NextTrackElement = Track->TrackElementAt(262, LeadTrackElement.Conn[LeadExitPos]);
                    NextElementEntryPos = LeadTrackElement.ConnLinkPos[LeadExitPos];
                    NextElementExitPos = Track->GetNonPointsOppositeLinkPos(NextElementEntryPos);//this is only for signals so no need to worry about points ambiguity
                    TrainOnNextElement = (NextTrackElement.TrainIDOnElement > -1);
                    StopSignalAtNextElement = ((NextTrackElement.Config[NextElementExitPos] == Signal) && (NextTrackElement.Attribute == 0));
                    }
                //logic here is:  if(train@stoplinkpos1 || train@stoplinkpos2 || (forward connection && (train on next element || stop signal at next element)))
                if(TrainAtStopLinkPos1 || TrainAtStopLinkPos2 || (ForwardConnection && (TrainOnNextElement || StopSignalAtNextElement)))
                    {
                    if(TTVPos > 0)
                        {
                        SendMissedActionLogs(1, TTVPos, ActionVectorEntryPtr);
                        ActionVectorEntryPtr+= TTVPos;
                        }
                    if(StopRequired)
                        {
                        StoppedAtLocation = true;
                        StoppedAtSignal = false;//may have been set earlier at line 925 so need to reset as
                                                //StoppedAtLocation takes precedence and don't want both set at same time or have flashing graphic
                                                //in zoom out mode
                        PlotTrainWithNewBackgroundColour(12, clStationStopBackground, Display);//pale green
                        LogAction(8, HeadCode, "", Arrive, LocName, ActionVectorEntryPtr->ArrivalTime, ActionVectorEntryPtr->Warning);
                        if(ActionVectorEntryPtr->FormatType == TimeTimeLoc)
                            {
                            TimeTimeLocArrived = true;//used in case of later signaller control, when need to know
                                                      //whether had arrived or not, to avoid sending the arrival
                                                      //message twice, see TInterface::TimetableControl1Click
                            }
                        }
                    else
                        {
                        LogAction(25, HeadCode, "", Pass, LocName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
                        }
                    if((ActionVectorEntryPtr->FormatType == TimeLoc) || (ActionVectorEntryPtr->FormatType == PassTime))
                        {
                        ActionVectorEntryPtr++;
                        }
                    //don't alter ActionVectorEntryPtr if at a TimeTimeLoc (& can't be anything else other than TimeLoc or PassTime after calling NameInTimetableBeforeCDT successfully)
                    LastActionTime = TrainController->TTClockTime;
                    }
                }
            }
        }
    }

if(Straddle == MidLag)
    {
    Straddle = LeadMidLag;
    FirstHalfMove = false;
    }
else if(Straddle == LeadMidLag)
    {
    Straddle = LeadMid;
    FirstHalfMove = true;
    }
else if(Straddle == LeadMid)
    {
    throw Exception("Error, Straddle shouldn't be LeadMid at exit from UpdateTrain function");
    }
Display->Update(); //need to keep this since Update() not called for PlotSmallOutput as too slow
Utilities->CallLogPop(661);
}

//----------------------------------------------------------------------------

Graphics::TBitmap *TTrain::SetOneGraphicCode(char CodeChar)
{
switch(CodeChar)
    {
    case '0' : return RailGraphics->Code0;
    case '1' : return RailGraphics->Code1;
    case '2' : return RailGraphics->Code2;
    case '3' : return RailGraphics->Code3;
    case '4' : return RailGraphics->Code4;
    case '5' : return RailGraphics->Code5;
    case '6' : return RailGraphics->Code6;
    case '7' : return RailGraphics->Code7;
    case '8' : return RailGraphics->Code8;
    case '9' : return RailGraphics->Code9;
    case 'A' : return RailGraphics->CodeA;
    case 'B' : return RailGraphics->CodeB;
    case 'C' : return RailGraphics->CodeC;
    case 'D' : return RailGraphics->CodeD;
    case 'E' : return RailGraphics->CodeE;
    case 'F' : return RailGraphics->CodeF;
    case 'G' : return RailGraphics->CodeG;
    case 'H' : return RailGraphics->CodeH;
    case 'I' : return RailGraphics->CodeI;
    case 'J' : return RailGraphics->CodeJ;
    case 'K' : return RailGraphics->CodeK;
    case 'L' : return RailGraphics->CodeL;
    case 'M' : return RailGraphics->CodeM;
    case 'N' : return RailGraphics->CodeN;
    case 'O' : return RailGraphics->CodeO;
    case 'P' : return RailGraphics->CodeP;
    case 'Q' : return RailGraphics->CodeQ;
    case 'R' : return RailGraphics->CodeR;
    case 'S' : return RailGraphics->CodeS;
    case 'T' : return RailGraphics->CodeT;
    case 'U' : return RailGraphics->CodeU;
    case 'V' : return RailGraphics->CodeV;
    case 'W' : return RailGraphics->CodeW;
    case 'X' : return RailGraphics->CodeX;
    case 'Y' : return RailGraphics->CodeY;
    case 'Z' : return RailGraphics->CodeZ;
    case 'a' : return RailGraphics->Code_a;
    case 'b' : return RailGraphics->Code_b;
    case 'c' : return RailGraphics->Code_c;
    case 'd' : return RailGraphics->Code_d;
    case 'e' : return RailGraphics->Code_e;
    case 'f' : return RailGraphics->Code_f;
    case 'g' : return RailGraphics->Code_g;
    case 'h' : return RailGraphics->Code_h;
    case 'i' : return RailGraphics->Code_i;
    case 'j' : return RailGraphics->Code_j;
    case 'k' : return RailGraphics->Code_k;
    case 'l' : return RailGraphics->Code_l;
    case 'm' : return RailGraphics->Code_m;
    case 'n' : return RailGraphics->Code_n;
    case 'o' : return RailGraphics->Code_o;
    case 'p' : return RailGraphics->Code_p;
    case 'q' : return RailGraphics->Code_q;
    case 'r' : return RailGraphics->Code_r;
    case 's' : return RailGraphics->Code_s;
    case 't' : return RailGraphics->Code_t;
    case 'u' : return RailGraphics->Code_u;
    case 'v' : return RailGraphics->Code_v;
    case 'w' : return RailGraphics->Code_w;
    case 'x' : return RailGraphics->Code_x;
    case 'y' : return RailGraphics->Code_y;
    case 'z' : return RailGraphics->Code_z;
    default : return RailGraphics->TempHeadCode;
    }
}

//----------------------------------------------------------------------------

void TTrain::SetHeadCodeGraphics(int Caller, AnsiString Code)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetHeadCodeGraphics");
if(Code.Length() != 4) TrainController->StopTTClockMessage(62, "Headcode Incorrect length");
for(int x=1;x<5;x++)//AnsiString indices start at 1
    {
    HeadCodeGrPtr[x-1]->Assign(SetOneGraphicCode(Code[x]));
    }
if(BackgroundColour != clB5G5R5)//i.e. not the basic graphic colour as loaded from .exe resource
    {
    bool ColourError = false, ColourError2 = false;
    for(int x=0;x<4;x++)
        {
        RailGraphics->ChangeBackgroundColour(0, HeadCodeGrPtr[x], HeadCodeGrPtr[x], BackgroundColour, clB5G5R5, ColourError);
        if(ColourError) ColourError2 = true;
        }
    if(ColourError2)
        {
        TrainController->StopTTClockMessage(68, "ERROR:  Colour depth insufficient to display train colours properly.  Please ensure that the 'safe' (web) palette of 256 colours can be displayed");
        }
    }
Utilities->CallLogPop(1484);
}

//----------------------------------------------------------------------------

void TTrain::GetLeadElement(int Caller)//assumes Mid & Lag already set, sets LeadElement,
//LeadEntryPos, LeadExitPos & DerailPending (don't want to act on it immediately)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetLeadElement");
DerailPending = false;
LeadElement = Track->TrackElementAt(269, MidElement).Conn[MidExitPos];
LeadEntryPos = Track->TrackElementAt(270, MidElement).ConnLinkPos[MidExitPos];
if(Track->TrackElementAt(271, LeadElement).TrackType == Points)
    {
    //attr 0=straight, - links 0 & 1 (0 = lead)
    //attr 1=diverging, - links 2 & 3 (2 = lead)
    //set appropriate next element or derail - use a subroutine & return element & bool for derail
    //points always have links 0 & 2 = lead, link 1 = trailing straight, link 3 = training diverging

    //if enter at lead, exit at whatever attr set at
    //if enter at lag, exit at lead, but set derail wrt attribute
    if((LeadEntryPos == 0) && (Track->TrackElementAt(272, LeadElement).Attribute == 0)) LeadExitPos = 1;

    //strictly speaking shouldn't need to set to 0 and 2 correctly since TrackIsInARoute caters for both, but
    //best to be on safe side
    else if(LeadEntryPos == 0)
        {
        LeadEntryPos = 2;
        LeadExitPos = 3;
        }
    else if((LeadEntryPos == 2) && (Track->TrackElementAt(273, LeadElement).Attribute == 0))
        {
        LeadEntryPos = 0;
        LeadExitPos = 1;
        }
    else if(LeadEntryPos == 2) LeadExitPos = 3;

    else if((LeadEntryPos == 1) && (Track->TrackElementAt(274, LeadElement).Attribute == 0)) LeadExitPos = 0;
    else if(LeadEntryPos == 1)
        {
        LeadExitPos = 0;
        DerailPending = true;
        }
    else if((LeadEntryPos == 3) && (Track->TrackElementAt(275, LeadElement).Attribute == 0))
        {
        LeadExitPos = 0;
        DerailPending = true;
        }
    else if(LeadEntryPos == 3) LeadExitPos = 0;
    }
else if(LeadEntryPos == 0) LeadExitPos = 1;
else if(LeadEntryPos == 1) LeadExitPos = 0;
else if(LeadEntryPos == 2) LeadExitPos = 3;
else if(LeadEntryPos == 3) LeadExitPos = 2;
//TTrackElement TrackElement = Track->TrackElementAt(276, LeadElement);
/*  signal check moved to Update() function
if((TrackElement.TrackType == SignalPost) && (TrackElement.Config[LeadExitPos] == Signal)
        && (TrackElement.Attribute == 0))//0 = red
    {
    StoppedAtSignal = true;  //comment out for test of locked route graphic replot
    }
else
    {
    StoppedAtSignal = false;
    }
*/
Utilities->CallLogPop(662);
}

//----------------------------------------------------------------------------

void TTrain::GetOffsetValues(int Caller, int &HOffset, int &VOffset, int Link) const
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetOffsetValues," + AnsiString(Link));
switch(Link)
    {
    case 1:
        {
        HOffset = 0;
        VOffset = 0;
        break;
        }
    case 2:
        {
        HOffset = 4;
        VOffset = 0;
        break;
        }
    case 3:
        {
        HOffset = 8;
        VOffset = 0;
        break;
        }
    case 4:
        {
        HOffset = 0;
        VOffset = 4;
        break;
        }
    case 6:
        {
        HOffset = 8;
        VOffset = 4;
        break;
        }
    case 7:
        {
        HOffset = 0;
        VOffset = 8;
        break;
        }
    case 8:
        {
        HOffset = 4;
        VOffset = 8;
        break;
        }
    case 9:
        {
        HOffset = 8;
        VOffset = 8;
        break;
        }
    default :
        {
        throw Exception("Error in GetOffsetValues - Link value wrong");
        }
    }
Utilities->CallLogPop(674);
}

//---------------------------------------------------------------------------

bool TTrain::LowEntryValue(int EntryLink) const
{
if((EntryLink==1) || (EntryLink==2) || (EntryLink==4) || (EntryLink==7)) return true;
else return false;
}

//---------------------------------------------------------------------------

void TTrain::PickUpBackgroundBitmap(int Caller, int HOffset, int VOffset, int Element, int EntryPos, Graphics::TBitmap *GraphicPtr) const
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PickUpBackgroundBitmap," + AnsiString(HOffset) + "," + AnsiString(VOffset) + "," + AnsiString(Element) + "," + AnsiString(EntryPos));
TAllRoutes::TRouteType RouteType;
Graphics::TBitmap *EXGraphicPtr = RailGraphics->bmTransparentBgnd;//default values
Graphics::TBitmap *EntryDirectionGraphicPtr = RailGraphics->bmTransparentBgnd;
TRect SourceRect, DestRect;
DestRect.Left = 0;
DestRect.Top = 0;
DestRect.Right = 8; //note right and bottom rect co-ordinates are 1 greater than the pixel area
DestRect.Bottom = 8;
SourceRect.Left = HOffset;//(Track->TrackElementAt(284, Element).HLoc * 16) + HOffset;
SourceRect.Top = VOffset;//(Track->TrackElementAt(285, Element).VLoc * 16) + VOffset;
SourceRect.Right = SourceRect.Left + 8;
SourceRect.Bottom = SourceRect.Top + 8;
Graphics::TBitmap *TempGraphic = new Graphics::TBitmap;
TempGraphic->PixelFormat = pf8bit;
TempGraphic->Width = 16;
TempGraphic->Height = 16;
TTrackElement TempElement = Track->TrackElementAt(286, Element);
if(TempElement.TrackType == Points)
    {
    TempGraphic->Assign(TempElement.GraphicPtr);
    TempGraphic->Transparent = true;
    TempGraphic->TransparentColor = Utilities->clTransparent;
    RouteType = AllRoutes->GetRouteTypeAndGraphics(7, Element, EntryPos, EXGraphicPtr, EntryDirectionGraphicPtr);
    if(RouteType == TAllRoutes::AutoSigsRoute)
        {
        TempGraphic->Canvas->Draw(0, 0, EXGraphicPtr);
        TempGraphic->Canvas->Draw(0, 0, EntryDirectionGraphicPtr);
        }
    else TempGraphic->Canvas->Draw(0, 0, Track->GetFilletGraphic(1, TempElement));//add fillet
    GraphicPtr->Canvas->CopyRect(DestRect, TempGraphic->Canvas, SourceRect);
    }
else if(TempElement.TrackType == GapJump)//plot set gap
    {
    if(TempElement.SpeedTag == 88) TempGraphic->Assign(RailGraphics->gl88set);
    else if(TempElement.SpeedTag == 89) TempGraphic->Assign(RailGraphics->gl89set);
    else if(TempElement.SpeedTag == 90) TempGraphic->Assign(RailGraphics->gl90set);
    else if(TempElement.SpeedTag == 91) TempGraphic->Assign(RailGraphics->gl91set);
    else if(TempElement.SpeedTag == 92) TempGraphic->Assign(RailGraphics->gl92set);
    else if(TempElement.SpeedTag == 93) TempGraphic->Assign(RailGraphics->bm93set);
    else if(TempElement.SpeedTag == 94) TempGraphic->Assign(RailGraphics->bm94set);
    else if(TempElement.SpeedTag == 95) TempGraphic->Assign(RailGraphics->gl95set);
    TempGraphic->Transparent = true;
    TempGraphic->TransparentColor = Utilities->clTransparent;
    RouteType = AllRoutes->GetRouteTypeAndGraphics(10, Element, EntryPos, EXGraphicPtr, EntryDirectionGraphicPtr);
    if(RouteType == TAllRoutes::AutoSigsRoute)
        {
        TempGraphic->Canvas->Draw(0, 0, EXGraphicPtr);
        TempGraphic->Canvas->Draw(0, 0, EntryDirectionGraphicPtr);
        }
    GraphicPtr->Canvas->CopyRect(DestRect, TempGraphic->Canvas, SourceRect);
    }
else
    {
    //first check if there's a NamedNonStationLocation element at that position & if so pick up that as the background
    //can't name points or gaps so 'else' OK
    bool FoundFlag;
    TTrack::TIMPair IMPair = Track->GetVectorPositionsFromInactiveTrackMap(4, TempElement.HLoc, TempElement.VLoc, FoundFlag);
    if(FoundFlag)
        {
        if(Track->InactiveTrackElementAt(25, IMPair.first).TrackType == NamedNonStationLocation)
            {
            GraphicPtr->Canvas->CopyRect(DestRect, Track->InactiveTrackElementAt(26, IMPair.first).GraphicPtr->Canvas, SourceRect);
            TempGraphic->Assign(RailGraphics->bmName);
            TempGraphic->Transparent = true;
            TempGraphic->TransparentColor = Utilities->clTransparent;
            RouteType = AllRoutes->GetRouteTypeAndGraphics(8, Element, EntryPos, EXGraphicPtr, EntryDirectionGraphicPtr);
            if(RouteType == TAllRoutes::AutoSigsRoute)
                {
                TempGraphic->Canvas->Draw(0, 0, EXGraphicPtr);
                TempGraphic->Canvas->Draw(0, 0, EntryDirectionGraphicPtr);
                }
            else TempGraphic->Canvas->Draw(0, 0, TempElement.GraphicPtr);//draw track on top
            GraphicPtr->Canvas->CopyRect(DestRect, TempGraphic->Canvas, SourceRect);
            }
        else
            {
            TempGraphic->Assign(TempElement.GraphicPtr);
            TempGraphic->Transparent = true;
            TempGraphic->TransparentColor = Utilities->clTransparent;
            RouteType = AllRoutes->GetRouteTypeAndGraphics(9, Element, EntryPos, EXGraphicPtr, EntryDirectionGraphicPtr);
            if(RouteType == TAllRoutes::AutoSigsRoute)
                {
                TempGraphic->Canvas->Draw(0, 0, EXGraphicPtr);
                TempGraphic->Canvas->Draw(0, 0, EntryDirectionGraphicPtr);
                }
            GraphicPtr->Canvas->CopyRect(DestRect, TempGraphic->Canvas, SourceRect);
            }
        }
    else
        {
        TempGraphic->Assign(TempElement.GraphicPtr);
        TempGraphic->Transparent = true;
        TempGraphic->TransparentColor = Utilities->clTransparent;
        RouteType = AllRoutes->GetRouteTypeAndGraphics(11, Element, EntryPos, EXGraphicPtr, EntryDirectionGraphicPtr);
        if(RouteType == TAllRoutes::AutoSigsRoute)
            {
            TempGraphic->Canvas->Draw(0, 0, EXGraphicPtr);
            TempGraphic->Canvas->Draw(0, 0, EntryDirectionGraphicPtr);
            }
        GraphicPtr->Canvas->CopyRect(DestRect, TempGraphic->Canvas, SourceRect);
        }
    }
delete TempGraphic;
Utilities->CallLogPop(675);
}

//---------------------------------------------------------------------------

void TTrain::PlotTrainGraphic(int Caller, int ArrayNumber, TDisplay *Disp)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotTrainGraphic," + AnsiString(ArrayNumber));
if(PlotElement[ArrayNumber] == -1)
    {
    Utilities->CallLogPop(676);
    return;//not plotted yet
    }
SetTrainElementID(0, PlotElement[ArrayNumber], PlotEntryPos[ArrayNumber]);//set before plot so gap flashing stops first
Disp->PlotOutput(29, ((Track->TrackElementAt(295, PlotElement[ArrayNumber]).HLoc * 16) + HOffset[ArrayNumber]),
            ((Track->TrackElementAt(296, PlotElement[ArrayNumber]).VLoc * 16) + VOffset[ArrayNumber]), HeadCodePosition[ArrayNumber]);
//Only need to set ID for leading element, stays set until train finally leaves the element
Plotted = true;
Utilities->CallLogPop(677);
}

//---------------------------------------------------------------------------

void TTrain::PlotBackgroundGraphic(int Caller, int ArrayNumber) const
{
Display->PlotOutput(30, ((Track->TrackElementAt(297, PlotElement[ArrayNumber]).HLoc * 16) + HOffset[ArrayNumber]),
            ((Track->TrackElementAt(298, PlotElement[ArrayNumber]).VLoc * 16) + VOffset[ArrayNumber]), BackgroundPtr[ArrayNumber]);
}

//---------------------------------------------------------------------------

bool TTrain::BufferAtExit(int Caller, int Element, int ExitPos) const
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",BufferAtExit," + AnsiString(Element) + "," + AnsiString(ExitPos));
if((Track->TrackElementAt(299, Element).TrackType == Buffers) &&
                (Track->TrackElementAt(300, Element).Config[ExitPos] == End))
    {
    Utilities->CallLogPop(678);
    return true;
    }
else
    {
    Utilities->CallLogPop(679);
    return false;
    }
}

//---------------------------------------------------------------------------

bool TTrain::ContinuationExit(int Caller, int Element, int ExitPos) const
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ContinuationExit," + AnsiString(Element) + "," + AnsiString(ExitPos));
if((Track->TrackElementAt(301, Element).TrackType == Continuation) &&
                (Track->TrackElementAt(302, Element).Config[ExitPos] == End))
    {
    Utilities->CallLogPop(680);
    return true;
    }
else
    {
    Utilities->CallLogPop(681);
    return false;
    }
}

//---------------------------------------------------------------------------

bool TTrain::IsTrainIDOnBridgeTrackPos01(int Caller, unsigned int TrackVectorPosition)
//test whether this train on a bridge on trackpos 0 & 1
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsTrainIDOnBridgeTrackPos01," + AnsiString(TrackVectorPosition));
if(Track->TrackElementAt(303, TrackVectorPosition).TrackType != Bridge)
    {
    Utilities->CallLogPop(682);
    return false;
    }
//if(Track->TrackElementAt(304, TrackVectorPosition).TrainIDOnElement != TrainID) return false; No, if a bridge could be one of 2 TrainIDs
if(Track->TrackElementAt(305, TrackVectorPosition).TrainIDOnBridgeTrackPos01 == TrainID)
    {
    if(Track->TrackElementAt(306, TrackVectorPosition).TrainIDOnBridgeTrackPos23 == TrainID)
        {
        throw Exception("Error, same train on two different bridge tracks");
        }
    else
        {
        Utilities->CallLogPop(684);
        return true;
        }
    }
else
    {
    Utilities->CallLogPop(685);
    return false;
    }
}

//---------------------------------------------------------------------------

bool TTrain::IsTrainIDOnBridgeTrackPos23(int Caller, unsigned int TrackVectorPosition)
//test whether this train on a bridge on trackpos 2 & 3
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsTrainIDOnBridgeTrackPos23," + AnsiString(TrackVectorPosition));
if(Track->TrackElementAt(307, TrackVectorPosition).TrackType != Bridge)
    {
    Utilities->CallLogPop(686);
    return false;
    }
//if(Track->TrackElementAt(308, TrackVectorPosition).TrainIDOnElement != TrainID) return false; No, if a bridge could be one of 2 TrainIDs
if(Track->TrackElementAt(309, TrackVectorPosition).TrainIDOnBridgeTrackPos23 == TrainID)
    {
    //don't carry out check for train on tracks 0 & 1 else will enter an infinite loop if train on both
    Utilities->CallLogPop(687);
    return true;
    }
else
    {
    Utilities->CallLogPop(688);
    return false;
    }
}

//---------------------------------------------------------------------------

void TTrain::SetTrainElementID(int Caller, unsigned int TrackVectorPosition, int EntryPos)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetTrainElementID," + AnsiString(TrackVectorPosition) + "," + AnsiString(EntryPos));
Track->TrackElementAt(310, TrackVectorPosition).TrainIDOnElement = TrainID;

//unplot GapFlash graphics if land on flashing gap (this done before train plotted - see PlotTrainGraphic)
if(Track->GapFlashFlag)
    {
    if((LeadElement == Track->GapFlashGreenPosition) || (LeadElement == Track->GapFlashRedPosition))
        {
        Track->GapFlashGreen->PlotOriginal(12, Display);
        Track->GapFlashRed->PlotOriginal(13, Display);
        Track->GapFlashFlag = false;
        }
    }

if(Track->TrackElementAt(311, TrackVectorPosition).TrackType == Bridge)
    {
    if(EntryPos == -1)
        {
        throw Exception("Error, TrackVectorPosition set but not EntryPos in SetTrainElementID");
        }
    if(EntryPos < 2) Track->TrackElementAt(312, TrackVectorPosition).TrainIDOnBridgeTrackPos01 = TrainID;
    else Track->TrackElementAt(313, TrackVectorPosition).TrainIDOnBridgeTrackPos23 = TrainID;
    }
Utilities->CallLogPop(690);
}

//---------------------------------------------------------------------------

void TTrain::ResetTrainElementID(int Caller, unsigned int TrackVectorPosition, int EntryPos)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetTrainElementID," + AnsiString(TrackVectorPosition) + "," + AnsiString(EntryPos));
if(Track->TrackElementAt(314, TrackVectorPosition).TrackType != Bridge) Track->TrackElementAt(315, TrackVectorPosition).TrainIDOnElement = -1;
else
    {
    if(EntryPos == -1)
        {
        throw Exception("Error, TrackVectorPosition set but not EntryPos in ResetTrainElementID");
        }
    if(EntryPos < 2) Track->TrackElementAt(316, TrackVectorPosition).TrainIDOnBridgeTrackPos01 = -1;
    else Track->TrackElementAt(317, TrackVectorPosition).TrainIDOnBridgeTrackPos23 = -1;
    if((EntryPos < 2) && (Track->TrackElementAt(318, TrackVectorPosition).TrainIDOnBridgeTrackPos23 > -1))//i.e. other train on track 2&3
        {
        Track->TrackElementAt(319, TrackVectorPosition).TrainIDOnElement = Track->TrackElementAt(320, TrackVectorPosition).TrainIDOnBridgeTrackPos23;
        }
    else if((EntryPos > 1) && (Track->TrackElementAt(321, TrackVectorPosition).TrainIDOnBridgeTrackPos01 > -1))//i.e. other train on track 1&2
        {
        Track->TrackElementAt(322, TrackVectorPosition).TrainIDOnElement = Track->TrackElementAt(323, TrackVectorPosition).TrainIDOnBridgeTrackPos01;
        }
    else Track->TrackElementAt(324, TrackVectorPosition).TrainIDOnElement = -1;
    }
Utilities->CallLogPop(691);
}

//---------------------------------------------------------------------------

void TTrain::PlotAlternativeTrackRouteGraphic(int Caller, unsigned int LagElement, int LagEntryPos, int HOffset, int VOffset)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotAlternativeTrackRouteGraphic," + AnsiString(LagElement) + "," + AnsiString(LagEntryPos) + "," + AnsiString(HOffset) + "," + AnsiString(VOffset));
int LockedVectorNumber;
if(Track->TrackElementAt(325, LagElement).TrackType != Bridge)// && (Track->TrackElementAt(326, LagElement).TrackType != Crossover))
    {                                                         //only applies for a bridge as there can't be (or shouldn't be) 2 routes on an element that isn't a bridge
    Utilities->CallLogPop(692);
    return;
    }
if(AllRoutes->TrackIsInARoute(0, LagElement, (3 - LagEntryPos)))// i.e other track is in a marked route
//LinkPos doesn't have to be the entry position for the above check
    {
    TRect SourceRect, DestRect;
    DestRect.Left = 0;
    DestRect.Top = 0;
    DestRect.Right = 8; //note right and bottom rect co-ordinates are 1 greater than the pixel area
    DestRect.Bottom = 8;
    SourceRect.Left = HOffset;
    SourceRect.Top = VOffset;
    SourceRect.Right = SourceRect.Left + 8;
    SourceRect.Bottom = SourceRect.Top + 8;

    //identify the route element for the other track
    TAllRoutes::TRouteElementPair RoutePair1, RoutePair2;
    RoutePair1 = AllRoutes->GetRouteElementDataFromRoute2MultiMap(13, Track->TrackElementAt(327, LagElement).HLoc,
            Track->TrackElementAt(328, LagElement).VLoc, RoutePair2);
    int FirstELink, SecondELink = -1;
    FirstELink = AllRoutes->GetFixedRouteAt(149, RoutePair1.first).GetFixedPrefDirElementAt(159, RoutePair1.second).GetELink();//must be at least one
    if(RoutePair2.first > -1) SecondELink = AllRoutes->GetFixedRouteAt(150, RoutePair2.first).GetFixedPrefDirElementAt(160, RoutePair2.second).GetELink();
    TPrefDirElement RouteElement;
    //Graphics::TBitmap *RouteGraphic;
    if(FirstELink == Track->TrackElementAt(329, LagElement).Link[LagEntryPos])//i.e. other track is in RoutePair2
        {
        if(SecondELink == -1)
            {
            throw Exception("Error - Second ELink should be set but isn't in PlotAlternativeTrackRouteGraphic [1]");
            }
        if(SecondELink == Track->TrackElementAt(330, LagElement).Link[LagEntryPos])//error if both have same Link number
            {
            throw Exception("Error - First & Second ELinks have same value in PlotAlternativeTrackRouteGraphic");
            }
    //    RouteGraphic = AllRoutes->GetFixedRouteAt(151, RoutePair2.first).GetFixedPrefDirElementAt(161, RoutePair2.second).GetEXGraphicPtr();
        RouteElement = AllRoutes->GetFixedRouteAt(152, RoutePair2.first).GetFixedPrefDirElementAt(162, RoutePair2.second);
        }
    else//other track is in RoutePair1
        {
    //    RouteGraphic = AllRoutes->GetFixedRouteAt(153, RoutePair1.first).GetFixedPrefDirElementAt(163, RoutePair1.second).GetEXGraphicPtr();
        RouteElement = AllRoutes->GetFixedRouteAt(154, RoutePair1.first).GetFixedPrefDirElementAt(164, RoutePair1.second);
        }
    Graphics::TBitmap *DestGraphic = new Graphics::TBitmap;
    DestGraphic->PixelFormat = pf8bit;
    DestGraphic->Width = 8;
    DestGraphic->Height = 8;
    DestGraphic->Transparent = true;//has to be transparent or will overwrite the track that the train has just left
    DestGraphic->TransparentColor = Utilities->clTransparent;
    DestGraphic->Canvas->CopyRect(DestRect, RouteElement.GetEXGraphicPtr()->Canvas, SourceRect);
    Display->PlotOutput(31, (Track->TrackElementAt(331, LagElement).HLoc * 16) + HOffset,
                (Track->TrackElementAt(332, LagElement).VLoc * 16) + VOffset, DestGraphic);
    //plot locked route marker for other route if appropriate
    TPrefDirElement PrefDirElement;//holder for next call, unused
    //plot locked route marker if appropriate, but only when train leaves element completely as this is a 16x16 graphic
    if(Straddle == LeadMidLag)
        {
        if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(11, RouteElement.GetTrackVectorPosition(), RouteElement.GetXLinkPos(), PrefDirElement, LockedVectorNumber))
            {
            Display->PlotOutput(32, (Track->TrackElementAt(333, LagElement).HLoc * 16),
                        (Track->TrackElementAt(334, LagElement).VLoc * 16), RailGraphics->LockedRouteCancelPtr[RouteElement.GetELink()]);
            }
        }
    delete DestGraphic;
    }
//but - there may be a train on the other track - if so need to replot it else the section of route overwrites it
//also can only be a bridge or trains either have already or soon will crash
if(Track->TrackElementAt(335, LagElement).TrackType != Bridge)
    {
    Utilities->CallLogPop(695);
    return;
    }
if(LagEntryPos > 1)//other train is on track 01
    {
    if(Track->TrackElementAt(336, LagElement).TrainIDOnBridgeTrackPos01 > -1)
        {
        TrainController->TrainVectorAtIdent(31, Track->TrackElementAt(337, LagElement).TrainIDOnBridgeTrackPos01).PlotTrain(2, Display);
        }
    }
else//other train is on track 23
    {
    if(Track->TrackElementAt(338, LagElement).TrainIDOnBridgeTrackPos23 > -1)
        {
        TrainController->TrainVectorAtIdent(32, Track->TrackElementAt(339, LagElement).TrainIDOnBridgeTrackPos23).PlotTrain(3, Display);
        }
    }
Utilities->CallLogPop(696);
}

//---------------------------------------------------------------------------

void TTrain::CheckAndCancelRouteForWrongEndEntry(int Caller, int Element, int EntryPos)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckAndCancelRouteForWrongEndEntry," + AnsiString(Element) + "," + AnsiString(EntryPos));
int RouteNumber;
bool WrongRoute = false;
TPrefDirElement RouteElement;
if(AllRoutes->GetRouteTypeAndNumber(11, Element, EntryPos, RouteNumber) == TAllRoutes::NoRoute)
//here if single track element & no route, or double track element with no route at EntryPos
//but still need to check if on points or a crossover on non-route track, and force-erase route if so (bridge OK of course)
//note that GetRouteTypeAndNumber allows for points having an EntryPos of 0 or 2 & still returns correct values
    {
    if((Track->TrackElementAt(340, Element).TrackType == Crossover) || (Track->TrackElementAt(341, Element).TrackType == Points))
        {
        if(AllRoutes->GetRouteTypeAndNumber(12, Element, (3 - EntryPos), RouteNumber) != TAllRoutes::NoRoute)
        //(3-EntryPos) guarantees other route (0->3; 1->2; 2->1; 3->0)
            {
            if(AllRoutes->GetFixedRouteAt(179, RouteNumber).PrefDirSize() > 2)
                {//don't call for stub end routes
                TrainController->LogActionError(49, HeadCode, "", RouteForceCancelled, Track->TrackElementAt(799, Element).ElementID);
                }
            AllRoutes->GetModifiableRouteAt(13, RouteNumber).ForceCancelRoute(1);
            Utilities->CallLogPop(697);
            return;
            }
        }
    Utilities->CallLogPop(698);
    return;//no route on other track or no other track
    }
for(unsigned int x = 0;x<AllRoutes->GetFixedRouteAt(155, RouteNumber).PrefDirSize();x++)
    {
    RouteElement = AllRoutes->GetFixedRouteAt(156, RouteNumber).GetFixedPrefDirElementAt(165, x);
    if(RouteElement.GetTrackVectorPosition() == (unsigned int)Element)
        {
        if(RouteElement.GetELinkPos() == EntryPos)
            {
            Utilities->CallLogPop(699);
            return;//right direction
            }
        else if((RouteElement.GetELinkPos() == 2) && (EntryPos == 0))
            {
            Utilities->CallLogPop(700);
            return;//right direction (points)
            }
        else if((RouteElement.GetELinkPos() == 0) && (EntryPos == 2))
            {
            Utilities->CallLogPop(701);
            return;//right direction (points)
            }
        else if(RouteElement.GetXLinkPos() == EntryPos)
            {
            WrongRoute = true;
            break;//wrong direction
            }
        else if((RouteElement.GetXLinkPos() == 2) && (EntryPos == 0))
            {
            WrongRoute = true;
            break;//wrong direction
            }
        else if((RouteElement.GetXLinkPos() == 0) && (EntryPos == 2))
            {
            WrongRoute = true;
            break;//wrong direction
            }
        }
    }
if(!WrongRoute)
    {
    throw Exception("Error, Element in route but no route found in CheckAndCancelRouteForWrongEndEntry");
    }
if(AllRoutes->GetFixedRouteAt(180, RouteNumber).PrefDirSize() > 2)
    {//don't call for stub end routes
    TrainController->LogActionError(50, HeadCode, "", RouteForceCancelled, Track->TrackElementAt(800, Element).ElementID);
    }
AllRoutes->GetModifiableRouteAt(14, RouteNumber).ForceCancelRoute(2);
Utilities->CallLogPop(703);
}

//---------------------------------------------------------------------------

void TTrain::PlotTrainWithNewBackgroundColour(int Caller, TColor NewBackgroundColour, TDisplay *Disp)
{
AnsiString HC = HeadCode;//to identify the train for diagnostic purposes
if(BackgroundColour == NewBackgroundColour) return;//don't replot if already correct
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotTrainWithNewBackgroundColour," + AnsiString(NewBackgroundColour));
bool ColourError = false, ColourError2 = false;
RailGraphics->ChangeBackgroundColour(1, FrontCodePtr, FrontCodePtr, NewBackgroundColour, BackgroundColour, ColourError);
if(ColourError) ColourError2 = true;
for(int x=0;x<4;x++)
    {
    RailGraphics->ChangeBackgroundColour(2, HeadCodeGrPtr[x], HeadCodeGrPtr[x], NewBackgroundColour, BackgroundColour, ColourError);
    if(ColourError) ColourError2 = true;
    }
if(ColourError2)
    {
    TrainController->StopTTClockMessage(63, "ERROR:  Colour depth insufficient to display train colours properly.  Please ensure that the 'safe' (web) palette of 256 colours can be displayed");
    }

//NB need a separate 'for' loop since the plot order can be different from the graphic order depending on the direction
//of motion
for(int x=0;x<4;x++)
    {
    PlotTrainGraphic(6, x, Disp);
    }
BackgroundColour = NewBackgroundColour;
Display->Update(); //need to keep this since Update() not called for PlotSmallOutput as too slow
Utilities->CallLogPop(704);
}

//---------------------------------------------------------------------------

void TTrain::SetTrainMovementValues(int Caller, int TrackVectorPosition, int EntryPos)
/*
Note:  Within the loop BrakeRate can only increase and MaxExitSpeed can only reduce

Summary:  Called during PlotStartPosition to set initial values & during Train.Update() when Straddle is LeadMidLag,
i.e. just as the front of a train is about to move fully onto an element, where TrackVectorPosition is the element immediately in front
of the element the front of the train is moving fully on to.  The function calculates the times and speeds at the next half-element and
full-element moves.

Detail:  TrackVectorPosition & EntryPos correspond to the TrackVector element immediately in front of where the train is at
the end of the current Update().  EntrySpeed is needed but this is a class data member so isn't passed in.  Set the
train BrakeRate to zero (for now, likely to be altered later), & check if zero entry speed with another train directly in front & if so
remain stopped.  Pick up the half length value and speed limit for the EntryPos track, and set FrontElementLength to the length of the
EntryPos track, then set LimitingSpeed to the minimum of the element speed limit or the train's maximum speed.  Check if running past a
red signal and set SPADFlag if so (use 1 for EntrySpeed rather than 0 as this value is a double so could be slightly in excess of 0).
In this case set the brake rate to maximum to stop as soon as possible.

For no SPAD calculate the distance that will be travelled at the maximum speed at which the train can exit the next element at half
MaxBrakeRate, this is DistanceAtHalfBraking (also calculate DistanceAtThreeQuarterBraking - used for stopping under signaller control).
DistanceAtHalfBraking is used as the limiting forward look from the next element (i.e. following EntryPos)
for computing the actual braking rate.  If no more restrictive speed limits or reasons to stop are found within the forward look then the
train can accelerate or stay at its (local) maximum speed for the next element.  The maximum speed on exit from the next element is used
for calculating the forward look because it represents the worst case - i.e. assumes that the train accelerates for the next element.

A loop is now entered where the CumulativeLength is updated and each successive element (if there are any - current element checked
first to see whether buffers or continuation) in turn is examined: first the length of the
current element is added to the cumulative length; then the half length and speedlimit are set for the next element - points are
followed according to their current setting (Attribute), but derailments are ignored as these are dealt with outside this function; checks
are then made to see whether the next element is a red signal (train should stop before it); next element is a buffer (train should stop
at the end of it so the cumulative length has the next element length added); current element is a buffer (train should stop
at the end of the current element so no need to alter the cumulative length); or have reached a named location stop position.  For any of
these reasons, or if stopping under signaller control, there is no more looping, instead the braking rate is calculated to bring the train
to a stop over CumulativeLength.  For all normal purposes the braking rate will be less than half (light braking), or less than three
quarters if stopping under signaller control (heavy braking).  However if signals are reset in front of a train then the train may need
emergency braking (> 90% max brake rate) and a SPAD may result.  Similarly if points are chaged in front of a train that divert it into a
siding then again emeregency braking may be necessary and a crash may result.

If the train is due to stop then the function calculates the half and full times and speeds and returns.  However the calculation depends
on the conditions at entry.  If the EntrySpeed is lower than MaxHalfSpeed and the EntryPos element is the one
that the train has to stop at the end of, as it might be for example if train had been stopped at a signal and the next element is a
buffer, then the train accelerates for half the element and brakes for the ither half.
Now the BrakeRate is calculated (limited to the MaxBrakeRate), but if it is less than a value calculated at an earlier pass round
the loop then it retains its earlier value (may be due to a close speed restriction that requires more braking than a more distant stop
requirement).  The MaxExitSpeedAtHalfBraking (maximum speed at which the train can leave the current element and still stop when required
at half the max braking rate) value is also calculated using EntrySpeed and CumulativeLength, but limiting it to the line speed limit or
train MaxRunningSpeed whichever is the lower.  If EntrySpeed > MaxExitSpeedAtHalfBraking then braking is required, so the half and full
speed and time values for the current element are calculated using BrakeRate, EntrySpeed and CurrentElementHalfLength.  If need to stop
at the end of the current element for other than a red signal (SPADs can occur) then ExitSpeedFull is set to 0.  It should be calculated
as 0 anyway for other than a red signal but this makes sure.  If EntrySpeed <= MaxExitSpeedAtHalfBraking then can calculate the half and
full speed and time values for acceleration over the current element, but limit ExitSpeedHalf & Full to MaxExitSpeedAtHalfBraking or to
the current element speed limit if necessary.  Check whether ExitSpeedHalf <= EntrySpeed (+0.01 since it's a double) and use constant speed
time values for Half & Full if so, but prior to this increase EntrySpeed if necessary to avoid a divide by zero error.

If the train is not due to stop within the DistanceAtHalfBraking from the next element following EntryPos then the next element (if there
is one) is checked to see if its speed limit is less than the current value of LimitingSpeed (which is the minimum of any earlier element's
speed limit that has been examined within the loop and the train's MaxRunning speed), and if so LimitingSpeed is set down to it.  Now
the MaxExitSpeedAtHalfBraking is calculated, limiting it to LimitingSpeed if less, in case need to accelerate in the current element, in
which case the exit speeds need to be limited to MaxExitSpeedAtHalfBraking.  If EntrySpeed > LimitingSpeed then calculate the braking rate
to bring the speed down to LimitingSpeed in CumulativeLength, keeping the existing BrakeRate value if lower and keeping it within
MaxBrakeRate.

Then, providing the current element isn't a buffer or continuation, the 'Current' values are updated from the 'Next' values ready for
the next loop iteration.  The loop is broken out of if the current element is a buffer or continuation, the next element is a
continuation, or (CumulativeLength - FrontElementLength) >= DistanceAtHalfBraking.

Now the final Half and Full values can be set for braking (if BrakeRate > 0.01), or accelerating - limiting the half and full exit speed
values to MaxExitSpeedAtHalfBraking if necessary, and using constant speed time values if the exit speeds aren't much different to
EntrySpeed and EntrySpeed > 0.01 (to avoid a divide by zero error).

Note that in no circumstances will a train stop when straddling 3 elements, it will always be fully on two elements.  This is ensured
by UpdateTrain() which never sets any stop conditions unless the train is fully on 2 elements when that function returns, i.e. entered
when Straddle == LeadMidLag 
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetTrainMovementValues," + AnsiString(TrackVectorPosition) + "," + AnsiString(EntryPos));
int EntryHalfLength, CurrentElementHalfLength, NextElementHalfLength, CumulativeLength = 0, CurrentTrackVectorPosition = TrackVectorPosition;
int DistanceAtHalfBraking, DistanceAtThreeQuarterBraking, ExitPos, NextTrackVectorPosition, NextEntryPos;
bool RedSignalFlag = false, BuffersFlag = false, StationFlag = false, BuffersOrContinuationNowFlag = false,
        ContinuationNextFlag = false, TrainInFrontInSignallerModeFlag = false;
double LimitingSpeed, FrontElementMaxSpeed, MaxExitSpeedAtHalfBrakingSquared, MaxExitSpeedAtHalfBraking, NextSpeedLimit, TempBrakeRate;
double ExitSpeedHalfSquared, ExitSpeedFullSquared;
bool SignallerStopRequired = false;

MaxExitSpeed = MaxRunningSpeed;//set high to begin with to avoid divide by zero errors on restart after stops, will be set lower later

//Member variables:     EntrySpeed, ExitSpeedHalf, ExitSpeedFull, MaxExitSpeed, BrakeRate, ExitTimeHalf, EntryTime, ExitTimeFull;

OneLengthAccelDecel = false;
BrakeRate = 0;

//first check if zero entry speed with another train directly in front & if so remain stopped
if(Track->OtherTrainOnTrack(2, CurrentTrackVectorPosition, EntryPos, TrainID) && (EntrySpeed < 1))
    {
    FrontElementSpeedLimit = Track->TrackElementAt(344, CurrentTrackVectorPosition).SpeedLimit01;
    FrontElementLength = Track->TrackElementAt(345, CurrentTrackVectorPosition).Length01;
    if(EntryPos > 1)
        {
        FrontElementSpeedLimit = Track->TrackElementAt(346, CurrentTrackVectorPosition).SpeedLimit23;
        FrontElementLength = Track->TrackElementAt(347, CurrentTrackVectorPosition).Length23;
        }
    EntrySpeed = 0;
    ExitSpeedHalf = 0;
    ExitSpeedFull = 0;
    MaxExitSpeed = 0;
    BrakeRate = 0;
    ExitTimeHalf = EntryTime;
    ExitTimeFull = EntryTime;
    StoppedForTrainInFront = true;
    Utilities->CallLogPop(705);
    return;
    }

if(CurrentTrackVectorPosition > -1)
    {
    if(EntryPos > 1)
        {
        CurrentElementHalfLength = (Track->TrackElementAt(348, CurrentTrackVectorPosition).Length23)/2;
        FrontElementSpeedLimit = Track->TrackElementAt(349, CurrentTrackVectorPosition).SpeedLimit23;
        }
    else
        {
        CurrentElementHalfLength = (Track->TrackElementAt(350, CurrentTrackVectorPosition).Length01)/2;
        FrontElementSpeedLimit = Track->TrackElementAt(351, CurrentTrackVectorPosition).SpeedLimit01;
        }
    EntryHalfLength = CurrentElementHalfLength;
    FrontElementLength = 2 * CurrentElementHalfLength;
    }
else
    {
    throw Exception("Error - CurrentTrackVectorPosition < 0 in SetTrainMovementValues");
    }
if((CurrentElementHalfLength < 0) || (FrontElementSpeedLimit < 0))
    {
    throw Exception("Error - HalfLength or SpeedLimit < 0 in SetTrainMovementValues");
    }

if(BeingCalledOn)
    {
    LimitingSpeed = CallOnMaxSpeed;
    }
else
    {
    LimitingSpeed = MaximumSpeedLimit;
    }
if(LimitingSpeed > FrontElementSpeedLimit) LimitingSpeed = FrontElementSpeedLimit;
if(LimitingSpeed > MaxRunningSpeed) LimitingSpeed = MaxRunningSpeed;
FrontElementMaxSpeed = LimitingSpeed;

//check if running past a red signal without permission
if((Track->TrackElementAt(352, CurrentTrackVectorPosition).Config[Track->GetNonPointsOppositeLinkPos(EntryPos)] == Signal) &&
        (Track->TrackElementAt(353, CurrentTrackVectorPosition).Attribute == 0) && (EntrySpeed > 1) && !AllowedToPassRedSignal)
    {
    SPADFlag = true;//user has to intervene to reset & restart after spad
    }

if(!SPADFlag)
    {
//for braking the deceleration rate is constant so the following formuli (Newton's Laws) are used:-
// (1)  V^2/(3.6^2) = U^2/(3.6^2) - 2FS; (2)  V/3.6 = U/3.6 - FT; (3)  S = UT/3.6 - 0.5FT^2
//where(V = final speed in kph [km/h/3.6 = m/s], U = initial speed in km/h, F = deceleration rate in m/s/s, S = distance in m & T = time)

//calc max speed that can attain on exit from next element (as could accelerate over next element) and use that speed to calc
//DistanceAtHalfBraking,  if use actual speed may miss a stop requirement just outside look-ahead & accelerate, and at next calc
//be unable to stop or have hard acceleration followed immediately by hard braking, this speed makes for smoother operation
    ExitSpeedFull = 3.6 * Power(((3 * EntryHalfLength * AValue * AValue) + (EntrySpeed * EntrySpeed * EntrySpeed / 3.6 / 3.6 / 3.6)), 0.333334);
    if(ExitSpeedFull > LimitingSpeed) ExitSpeedFull = LimitingSpeed;
    DistanceAtHalfBraking = ExitSpeedFull * ExitSpeedFull / 3.6 / 3.6 / MaxBrakeRate;
    DistanceAtThreeQuarterBraking = ExitSpeedFull * ExitSpeedFull / 3.6 / 3.6 / 1.5 / MaxBrakeRate;//used for signaller stops

    do
        {
        RedSignalFlag = false;
        BuffersFlag = false;
        StationFlag = false;
        BuffersOrContinuationNowFlag = false;
        ContinuationNextFlag = false;//have to reset this after the above test
        //add current element length to CumulativeLength
        CumulativeLength+= (2 * CurrentElementHalfLength);
        if((CumulativeLength >= DistanceAtThreeQuarterBraking) && (TrainMode == Signaller) && SignallerStoppingFlag)
            {
            SignallerStopRequired = true;//once set stays set until SignallerStoppingFlag reset, providing !BuffersOrContinuationNowFlag,
            //set SignallerStopBrakeRate to stop in CumulativeLength unless already higher (i.e. can only increase)
            double TempBR = EntrySpeed * EntrySpeed / 2 / 3.6 / 3.6 / CumulativeLength;
            if(SignallerStopBrakeRate < TempBR)
                {
                SignallerStopBrakeRate = TempBR;
                }
            }
        //first check for stops within the length of the current element, where don't want any more checks & don't want
        //to add in any extra to the CumulativeLength.  Only applies for buffers & station stops as may run past a red
        //signal

        //check if current element is a buffer
        if(Track->TrackElementAt(374, CurrentTrackVectorPosition).TrackType == Buffers)
            {
            //no need to add in the length of this element to CumulativeLength as already included
            BuffersFlag = true;
            }
        //check if current element is a station stop
        if(TrainMode == Timetable)
            {
            bool StopRequired = false;
            if(!TimetableFinished && (NameInTimetableBeforeCDT(12, Track->TrackElementAt(375, CurrentTrackVectorPosition).ActiveTrackElementName, StopRequired) > -1) &&
                        ((Track->TrackElementAt(376, CurrentTrackVectorPosition).StationEntryStopLinkPos1 == EntryPos) ||
                        (Track->TrackElementAt(377, CurrentTrackVectorPosition).StationEntryStopLinkPos2 == EntryPos)))
                {
                //no need to add in the length of element to CumulativeLength
                if(StopRequired) StationFlag = true;
                }
            }
        else
            {
            StationFlag = false;
            }

        //set NextHalfLength & NextSpeedLimit, but only if current element not buffers or exit continuation - no next element for them
        if(((Track->TrackElementAt(354, CurrentTrackVectorPosition).TrackType == Buffers) ||
                (Track->TrackElementAt(355, CurrentTrackVectorPosition).TrackType == Continuation)) && (EntryPos == 1))
            {
            BuffersOrContinuationNowFlag = true;
            }
        if(!BuffersOrContinuationNowFlag && !BuffersFlag && !StationFlag)//skip if buffers or station flags already set
            {
            if(Track->TrackElementAt(356, CurrentTrackVectorPosition).TrackType == Points)
                {
                if((EntryPos == 0) || (EntryPos == 2))
                    {
                    if(Track->TrackElementAt(357, CurrentTrackVectorPosition).Attribute == 0) ExitPos = 1; else ExitPos = 3;
                    }
                else ExitPos = 0;
                }
            else ExitPos = Track->GetNonPointsOppositeLinkPos(EntryPos);
            NextTrackVectorPosition = Track->TrackElementAt(358, CurrentTrackVectorPosition).Conn[ExitPos];
            NextEntryPos = Track->TrackElementAt(359, CurrentTrackVectorPosition).ConnLinkPos[ExitPos];
            if(NextTrackVectorPosition > -1)
                {
                if(NextEntryPos > 1)
                    {
                    NextElementHalfLength = (Track->TrackElementAt(360, NextTrackVectorPosition).Length23)/2;
                    NextSpeedLimit = Track->TrackElementAt(361, NextTrackVectorPosition).SpeedLimit23;
                    }
                else
                    {
                    NextElementHalfLength = (Track->TrackElementAt(362, NextTrackVectorPosition).Length01)/2;
                    NextSpeedLimit = Track->TrackElementAt(363, NextTrackVectorPosition).SpeedLimit01;
                    }
                }
            else
                {
                throw Exception("Error - Trying to access NextTrackVectorPosition when none present in SetTrainMovementValues");
                }
            //now check for stops, first cover those where don't want to add in length of next element
            //check if next element is a red signal - Attr 0,
            //note that this doesn't apply to trains stopped at a red signal since the signal position is
            //CurrentTrackVectorPosition not NextTrackVectorPosition
            bool StopRequired;
            if(Track->TrackElementAt(364, NextTrackVectorPosition).Config[Track->GetNonPointsOppositeLinkPos(NextEntryPos)] == Signal)
                {
                if(Track->TrackElementAt(365, NextTrackVectorPosition).Attribute == 0)
                //no need to add in the length of element to CumulativeLength
                RedSignalFlag = true;
                }
            //check if current element is a station & next element contains a train - trains will always stop without crashing at a
            //station they are due to stop even if there is a train in front blocking the normal stop position - providing there is
            //at least one platform element free
            else if((TrainMode == Timetable) && !TimetableFinished && (NameInTimetableBeforeCDT(10, Track->TrackElementAt(368, CurrentTrackVectorPosition).ActiveTrackElementName, StopRequired) > -1) &&
                    Track->OtherTrainOnTrack(3, NextTrackVectorPosition, NextEntryPos, TrainID))
                {
                //no need to add in the length of element to CumulativeLength
                if(StopRequired) StationFlag = true;
                }
            //check if next element contains a train & in Signaller mode (always stops for train in front if in signaller mode)
            else if((TrainMode == Signaller) && Track->OtherTrainOnTrack(4, NextTrackVectorPosition, NextEntryPos, TrainID))//(Track->TrackElementAt(651, NextTrackVectorPosition).TrainIDOnElement > -1))
                {
                //no need to add in the length of element to CumulativeLength
                TrainInFrontInSignallerModeFlag = true;
                }
            //check if next element is a buffer
            else if(Track->TrackElementAt(366, NextTrackVectorPosition).TrackType == Buffers)
                {
                //need to add in the length of that element to CumulativeLength
                CumulativeLength+= Track->TrackElementAt(367, NextTrackVectorPosition).Length01;
                BuffersFlag = true;
                }
            //check if next element is a station stop
            else if((TrainMode == Timetable) && !TimetableFinished && (NameInTimetableBeforeCDT(11, Track->TrackElementAt(370,
                    NextTrackVectorPosition).ActiveTrackElementName, StopRequired) > -1) && ((Track->TrackElementAt(371, NextTrackVectorPosition).StationEntryStopLinkPos1 ==
                    EntryPos) || (Track->TrackElementAt(372, NextTrackVectorPosition).StationEntryStopLinkPos2 == EntryPos)))
                {
                //need to add in the length of that element to CumulativeLength if a stop required
                if(StopRequired)
                    {
                    StationFlag = true;
                    CumulativeLength+= Track->TrackElementAt(373, NextTrackVectorPosition).Length01;
                    }
                }
            }
        if(RedSignalFlag || BuffersFlag || StationFlag || TrainInFrontInSignallerModeFlag || SignallerStopRequired || StepForwardFlag)//no more loops
            {
            //have to come to a stop over CumulativeLength
            if(CumulativeLength == FrontElementLength)//will be if StepForwardFlag
            //only one length to go before stop so check whether need to accelerate for half length then brake for latter
            //half;  calc speed at halfway point that corresponds to half braking rate for latter half of track element,
            //and if less than EntrySpeed then skip this section (don't need any acceleration)
            //if not calc speed at halfway point & if less than above set half speed to this value;
            //use constant acceleration in calculating half time point
                {
                MaxExitSpeed = 0;
                double MaxHalfSpeed;
                double MaxHalfSpeedAtHalfBraking = 3.6 * sqrt(MaxBrakeRate * FrontElementLength / 2);
                //have to halve the element length, & can't be zero or negative so no need to test
//                if(MaxHalfSpeedAtHalfBraking > LimitingSpeed) MaxHalfSpeed = LimitingSpeed; else MaxHalfSpeed = MaxHalfSpeedAtHalfBraking;
                if(MaxHalfSpeedAtHalfBraking > FrontElementMaxSpeed) MaxHalfSpeed = FrontElementMaxSpeed; else MaxHalfSpeed = MaxHalfSpeedAtHalfBraking;
                if(MaxHalfSpeed > (2 * EntrySpeed))//use 2x to prevent kangarooing at last element when had
                //been braking smoothly at less that 50% braking rate, 2x should prevent all but extreme cases
                    {
                    ExitSpeedHalf = 3.6 * Power(((1.5 * EntryHalfLength * AValue * AValue) + (EntrySpeed * EntrySpeed * EntrySpeed / 3.6 / 3.6 / 3.6)), 0.333334);
                    bool HalfSpeedLimited = false;
                    if(MaxHalfSpeed < ExitSpeedHalf)
                        {
                        ExitSpeedHalf = MaxHalfSpeed;
                        HalfSpeedLimited = true;
                        }
                    ExitTimeHalf = EntryTime + TDateTime(((ExitSpeedHalf * ExitSpeedHalf) - (EntrySpeed * EntrySpeed)) /
                            3.6 / 3.6 / (AValue * AValue) / 86400.0);
                    //the above is the time taken to accelerate to ExitSpeedHalf, so if this is reached before the half
                    //way point (i.e. HalfSpeedLimited is set) then the time will be too short; change later to equal the
                    //braking time; not fully accurate but better to be equal than have a short acceleration period followed
                    //by a long braking period
                    ExitSpeedFull = 0;
                    TempBrakeRate = ExitSpeedHalf * ExitSpeedHalf / 2 / 3.6 / 3.6 / EntryHalfLength;
                    if(TempBrakeRate > MaxBrakeRate) TempBrakeRate = MaxBrakeRate;//shouldn't be but leave in anyway
                    if(TempBrakeRate > BrakeRate) BrakeRate = TempBrakeRate;//BrakeRate may already have been set in an earlier loop so don't want to reduce it
                    ExitTimeFull = ExitTimeHalf + TDateTime(ExitSpeedHalf / 3.6 / BrakeRate / 86400.0);
                    if(HalfSpeedLimited)//this is the change referred to above
                        {
                        TDateTime BrakingTime = ExitTimeFull - ExitTimeHalf;
                        ExitTimeHalf = EntryTime + BrakingTime;
                        ExitTimeFull = ExitTimeHalf + BrakingTime;
                        }
                    OneLengthAccelDecel = true;
                    Utilities->CallLogPop(1095);
                    return;
                    }
                }
            //set braking to achieve speed = 0 @ CumulativeLength up to MaxBrakeRate
            //calc MxES for element at EntryPosition & set to this or existing val if lower,
            //calc th, tf, sh, sf & exit
            TempBrakeRate = EntrySpeed * EntrySpeed / 2 / 3.6 / 3.6 / CumulativeLength;
            if(TempBrakeRate > MaxBrakeRate) TempBrakeRate = MaxBrakeRate;
            if(TempBrakeRate > BrakeRate) BrakeRate = TempBrakeRate;//BrakeRate may already have been set in an earlier loop so don't want to reduce it
            if(SignallerStopRequired)//set BrakeRate to max of its calculated value or SignallerStopBrakeRate
                {
                if(SignallerStopBrakeRate < BrakeRate)
                    {
                    SignallerStopBrakeRate = BrakeRate;//this prevents the brakerate from reducing for a signaller stop
                                                       //regardless of other conditions that may change as progress round the loop
                    }
                }
            if(SignallerStopBrakeRate > BrakeRate)
            //prevents BrakeRate dropping below SignallerStopBrakeRate once it's been set whether or not SignallerStopRequired set
            //SignallerStopRequired may not be set if a red signal found in a later calc, & brakerate may then drop
                {
                BrakeRate = SignallerStopBrakeRate;
                }
            int TempMaxExitSpeed;//calc current value & if less than MaxExitSpeed set that to this
            MaxExitSpeedAtHalfBrakingSquared = 3.6 * 3.6 * MaxBrakeRate * (CumulativeLength - FrontElementLength);
            if(MaxExitSpeedAtHalfBrakingSquared < 10) MaxExitSpeedAtHalfBraking = 0; else MaxExitSpeedAtHalfBraking = sqrt(MaxExitSpeedAtHalfBrakingSquared);
//            if(MaxExitSpeedAtHalfBraking > LimitingSpeed) MaxExitSpeed = LimitingSpeed; else MaxExitSpeed = MaxExitSpeedAtHalfBraking;
//I think the above was dropped because it could cause MaxExitSpeed to increase (MaxExitSpeed is an external variable retained between loops)
            if(MaxExitSpeedAtHalfBraking > FrontElementMaxSpeed) TempMaxExitSpeed = FrontElementMaxSpeed; else TempMaxExitSpeed = MaxExitSpeedAtHalfBraking;
            if(TempMaxExitSpeed < MaxExitSpeed) MaxExitSpeed = TempMaxExitSpeed;

            //here have EntrySpeed & MaxExitSpeed (for the next element), BrakeRate (to bring speed to zero over
            //Cumulativelength, and Cumulativelength

            if((EntrySpeed > MaxExitSpeed) || SignallerStopRequired || (SignallerStopBrakeRate > 0.01))//need to brake
                {
                ExitSpeedHalfSquared = (EntrySpeed * EntrySpeed) - (3.6 * 3.6 * 2 * BrakeRate * EntryHalfLength);
                if(ExitSpeedHalfSquared < 10) ExitSpeedHalf = 0; else ExitSpeedHalf = sqrt(ExitSpeedHalfSquared);
                ExitTimeHalf = EntryTime + TDateTime((EntrySpeed - ExitSpeedHalf) / 3.6 / BrakeRate / 86400.0);
                ExitSpeedFullSquared = (EntrySpeed * EntrySpeed) - (3.6 * 3.6 * 4 * BrakeRate * EntryHalfLength);
                if(ExitSpeedFullSquared < 10) ExitSpeedFull = 0; else ExitSpeedFull = sqrt(ExitSpeedFullSquared);
                if((StationFlag) && (CumulativeLength == FrontElementLength))
                    {
                    ExitSpeedFull = 0; //force a stop for station (not for buffers or red signal)
                    }
                ExitTimeFull = EntryTime + TDateTime((EntrySpeed - ExitSpeedFull) / 3.6 / BrakeRate / 86400.0);
                }
            else //e.g. moving towards a signal or station after a speed limit, so can accelerate
                {
//for accelerating the energy input rate (PowerAtRail) is constant so the following formuli are used:-
// (1)  V^2/(3.6^2) - U^2/(3.6^2) = A^2T;  (2)  V = 3.6 * ((1.5*S*A^2) + U^3/(3.6^3))^0.333334;
//where A is a constant (2*PowerAtRail/Mass)^0.5;   V = final speed in kph, U = initial speed in kph, S = distance & T = time
//This is a bit unrealistic during the early acceleration phase as it will be too rapid, but shouldn't affect the running unduly
                BrakeRate = 0;
                ExitSpeedHalf = 3.6 * Power(((1.5 * EntryHalfLength * AValue * AValue) + (EntrySpeed * EntrySpeed * EntrySpeed / 3.6 / 3.6 / 3.6)), 0.333334);
                ExitSpeedFull = 3.6 * Power(((3 * EntryHalfLength * AValue * AValue) + (EntrySpeed * EntrySpeed * EntrySpeed / 3.6 / 3.6 / 3.6)), 0.333334);
                //above valid for ExitSpeedHalf & Full <= MaxExitSpeed
                if(ExitSpeedHalf <= MaxExitSpeed)//can accelerate continually over the half length
                    {
                    ExitTimeHalf = EntryTime + TDateTime(((ExitSpeedHalf * ExitSpeedHalf) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue) / 86400.0);
                    if(ExitSpeedFull <= MaxExitSpeed)//can accelerate continually over the full length
                    //i.e both (ExitSpeedHalf <= MaxExitSpeed) & (ExitSpeedFull <= MaxExitSpeed)
                        {
                        ExitTimeFull = EntryTime + TDateTime(((ExitSpeedFull * ExitSpeedFull) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue) / 86400.0);
                        }
                    else //(ExitSpeedHalf <= MaxExitSpeed) but (ExitSpeedFull > MaxExitSpeed)
                         //accelerate to MaxExitSpeed then reamin at this speed for rest of element
                        {
                        ExitSpeedFull = MaxExitSpeed;
                        double DeltaExitTimeToMaxInSecs = ((MaxExitSpeed * MaxExitSpeed) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue);
                        double DistanceToMax = ((MaxExitSpeed * MaxExitSpeed * MaxExitSpeed) -
                                (EntrySpeed * EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / 3.6 / (1.5 * AValue * AValue);
                        double RemainingDistance = double(FrontElementLength) - DistanceToMax;
                        double DeltaRemainingTimeInSecs = 3.6 * RemainingDistance / MaxExitSpeed;
                        ExitTimeFull = EntryTime + TDateTime((DeltaExitTimeToMaxInSecs + DeltaRemainingTimeInSecs) / 86400.0);
                        }
                    }
                else //ExitSpeedHalf > MaxExitSpeed, so ExitSpeedFull must also be > MaxExitSpeed
                     //accelerate over first half to MaxExitSpeed then remain at this speed for rest of the first and
                     //second halves of the element
                    {
                    ExitSpeedHalf = MaxExitSpeed;
                    double DeltaExitTimeToMaxInSecs = ((MaxExitSpeed * MaxExitSpeed) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue);
                    double DistanceToMax = ((MaxExitSpeed * MaxExitSpeed * MaxExitSpeed) -
                            (EntrySpeed * EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / 3.6 / (1.5 * AValue * AValue);
                    double RemainingDistance = double(FrontElementLength / 2) - DistanceToMax;//remaining distance to half length
                    double DeltaRemainingTimeInSecs = 3.6 * RemainingDistance / MaxExitSpeed;
                    ExitTimeHalf = EntryTime + TDateTime((DeltaExitTimeToMaxInSecs + DeltaRemainingTimeInSecs) / 86400.0);
                    ExitSpeedFull = MaxExitSpeed;
                    ExitTimeFull = ExitTimeHalf + TDateTime(3.6 * EntryHalfLength / MaxExitSpeed / 86400.0);
                    }
                }
            Utilities->CallLogPop(706);
            return;
            }
        else
            {
            if(!BuffersOrContinuationNowFlag)
                {
                if(NextSpeedLimit < LimitingSpeed) LimitingSpeed = NextSpeedLimit;
                }
            //calc max exit speed for the entry element at half braking to ensure don't accelerate past it in this element if
            //acceleration is required
            int TempMaxExitSpeed;//calc current value & if less than MaxExitSpeed set that to this
            //Note that LimitingSpeed is the max value at the end of CumulativeLength, so MaxExitSpeedAtHalfBrakingSquared will be larger
            MaxExitSpeedAtHalfBrakingSquared = (LimitingSpeed * LimitingSpeed) + (3.6 * 3.6 * MaxBrakeRate * (CumulativeLength - FrontElementLength));
            if(MaxExitSpeedAtHalfBrakingSquared < 10) MaxExitSpeedAtHalfBraking = 0; else MaxExitSpeedAtHalfBraking = sqrt(MaxExitSpeedAtHalfBrakingSquared);
            if(MaxExitSpeedAtHalfBraking > FrontElementMaxSpeed) TempMaxExitSpeed = FrontElementMaxSpeed; else TempMaxExitSpeed = MaxExitSpeedAtHalfBraking;
            if(TempMaxExitSpeed < MaxExitSpeed) MaxExitSpeed = TempMaxExitSpeed; //MaxExitSpeed is an external variable & this can reduce its value
            if(EntrySpeed > LimitingSpeed)//note that LimitingSpeed is more restrictive than MaxExitSpeed
                //calc TempBrakeRate & set BrakeRate to this or keep existing val if higher
                {
                TempBrakeRate = ((EntrySpeed * EntrySpeed) - (LimitingSpeed * LimitingSpeed)) / 3.6 / 3.6 / 2 / CumulativeLength;
                if(TempBrakeRate > MaxBrakeRate) TempBrakeRate = MaxBrakeRate;//shouldn't be for speedlimits since all known in advance, but include anyway
                if(TempBrakeRate > BrakeRate) BrakeRate = TempBrakeRate;//BrakeRate may already have been set in an earlier loop so don't want to reduce it
                }
            }
        if(!BuffersOrContinuationNowFlag)
            {
            CurrentTrackVectorPosition = NextTrackVectorPosition;
            EntryPos = NextEntryPos;
            CurrentElementHalfLength = NextElementHalfLength;
            if(Track->TrackElementAt(378, NextTrackVectorPosition).TrackType == Continuation)
                {
                ContinuationNextFlag = true;
                }
            }
        }
    while(((CumulativeLength - FrontElementLength) < DistanceAtHalfBraking) && ((!BuffersOrContinuationNowFlag && !ContinuationNextFlag) || SignallerStoppingFlag));
    //check from the end of the front element, if include the front element and could brake during it, then will skip further loops
    //& miss a stop requirement just beyond the front element.  happened in Richard Standing's railway where a ne service introduced
    //on a 100m length, with 20m length after & then a red signal - train accelerated over the 100m then caused a SPAD as too short a
    //stopping distance after it.

    //If SignallerStoppingFlag then don't exit loop because of ContinuationNextFlag or BuffersOrContinuationNowFlag, since continuation
    //not immediately in front (if it is then LeadElement will be the continuation & SignallerStoppingFlag will be reset in UpdateTrain()),
    //need to at least give a chance to stop on signaller command, if keep moving until continuation is immediately in front then will
    //exit loop & that is OK as don't want to stop so close to a continuation, if that happens it means that the command to stop was given
    //too late

    //set final braking or acc'n
    if(BrakeRate > 0.01)
        {
        ExitSpeedHalfSquared = (EntrySpeed * EntrySpeed) - (3.6 * 3.6 * 2 * BrakeRate * EntryHalfLength);
        if(ExitSpeedHalfSquared < 10) ExitSpeedHalf = 0; else ExitSpeedHalf = sqrt(ExitSpeedHalfSquared);
        ExitTimeHalf = EntryTime + TDateTime((EntrySpeed - ExitSpeedHalf) / 3.6 / BrakeRate / 86400.0);
        ExitSpeedFullSquared = (EntrySpeed * EntrySpeed) - (3.6 * 3.6 * 4 * BrakeRate * EntryHalfLength);
        if(ExitSpeedFullSquared < 10) ExitSpeedFull = 0; else ExitSpeedFull = sqrt(ExitSpeedFullSquared);
        ExitTimeFull = EntryTime + TDateTime((EntrySpeed - ExitSpeedFull) / 3.6 / BrakeRate / 86400.0);
        }
    else
        {
//for accelerating the energy input rate (PowerAtRail) is constant so the following formuli are used:-
// (1)  V^2/(3.6^2) - U^2/(3.6^2) = A^2T;  (2)  V = 3.6 * ((1.5*S*A^2) + U^3/ (3.6)^3)^0.333334;
//where A is a constant (2*PowerAtRail/Mass)^0.5;   V = final speed in kph, U = initial speed in kph , S = distance in m & T = time
//This is a bit unrealistic during the early acceleration phase as it will be too rapid, but shouldn't affect the running unduly

        BrakeRate = 0;
        ExitSpeedHalf = 3.6 * Power(((1.5 * EntryHalfLength * AValue * AValue) + (EntrySpeed * EntrySpeed * EntrySpeed / 3.6 / 3.6 / 3.6)), 0.333334);
        ExitSpeedFull = 3.6 * Power(((3 * EntryHalfLength * AValue * AValue) + (EntrySpeed * EntrySpeed * EntrySpeed / 3.6 / 3.6 / 3.6)), 0.333334);
        //above valid for ExitSpeedHalf & Full <= MaxExitSpeed
        if(ExitSpeedHalf <= MaxExitSpeed)
            {
            ExitTimeHalf = EntryTime + TDateTime(((ExitSpeedHalf * ExitSpeedHalf) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue) / 86400.0);
            if(ExitSpeedFull <= MaxExitSpeed) //(ExitSpeedHalf <= MaxExitSpeed) & (ExitSpeedFull <= MaxExitSpeed)
                {
                ExitTimeFull = EntryTime + TDateTime(((ExitSpeedFull * ExitSpeedFull) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue) / 86400.0);
                }
            else //(ExitSpeedHalf <= MaxExitSpeed) & (ExitSpeedFull > MaxExitSpeed)
                {
                ExitSpeedFull = MaxExitSpeed;
                double DeltaExitTimeToMaxInSecs = ((MaxExitSpeed * MaxExitSpeed) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue);
                double DistanceToMax = ((MaxExitSpeed * MaxExitSpeed * MaxExitSpeed) -
                        (EntrySpeed * EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / 3.6 / (1.5 * AValue * AValue);
                double RemainingDistance = double(FrontElementLength) - DistanceToMax;
                double DeltaRemainingTimeInSecs = 3.6 * RemainingDistance / MaxExitSpeed;
                ExitTimeFull = EntryTime + TDateTime((DeltaExitTimeToMaxInSecs + DeltaRemainingTimeInSecs) / 86400.0);
                }
            }
        else //ExitSpeedHalf > MaxExitSpeed, ExitSpeedFull must be > MaxExitSpeed
            {
            ExitSpeedHalf = MaxExitSpeed;
            double DeltaExitTimeToMaxInSecs = ((MaxExitSpeed * MaxExitSpeed) - (EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / (AValue * AValue);
            double DistanceToMax = ((MaxExitSpeed * MaxExitSpeed * MaxExitSpeed) -
                    (EntrySpeed * EntrySpeed * EntrySpeed)) / 3.6 / 3.6 / 3.6 / (1.5 * AValue * AValue);
            double RemainingDistance = double(FrontElementLength / 2) - DistanceToMax;//remaining distance to half length
            double DeltaRemainingTimeInSecs = 3.6 * RemainingDistance / MaxExitSpeed;
            ExitTimeHalf = EntryTime + TDateTime((DeltaExitTimeToMaxInSecs + DeltaRemainingTimeInSecs) / 86400.0);
            ExitSpeedFull = MaxExitSpeed;
            ExitTimeFull = ExitTimeHalf + TDateTime(3.6 * EntryHalfLength / MaxExitSpeed / 86400.0);
            }
        }
    }

else//SPADFlag set
    {
    BrakeRate = MaxBrakeRate;
    ExitSpeedHalfSquared = (EntrySpeed * EntrySpeed) - (3.6 * 3.6 * 2 * BrakeRate * EntryHalfLength);
    if(ExitSpeedHalfSquared < 10) ExitSpeedHalf = 0; else ExitSpeedHalf = sqrt(ExitSpeedHalfSquared);
    ExitTimeHalf = EntryTime + TDateTime((EntrySpeed - ExitSpeedHalf) / 3.6 / BrakeRate / 86400.0);
    ExitSpeedFullSquared = (EntrySpeed * EntrySpeed) - (3.6 * 3.6 * 4 * BrakeRate * EntryHalfLength);
    if(ExitSpeedFullSquared < 10) ExitSpeedFull = 0; else ExitSpeedFull = sqrt(ExitSpeedFullSquared);
    ExitTimeFull = EntryTime + TDateTime((EntrySpeed - ExitSpeedFull) / 3.6 / BrakeRate / 86400.0);

//check if the exit speed is < 80% of the stopping speed for the next element, and if so stop at end of this element
//this is because would stop short of end of next element (in reality the time to reach the end of the next element
//would be too short (could be so short as to make the train jump) as time is calculated purely on speed & brake rate);
//80% is used as the brake rate might be set to come to a halt at the end of the next element in which case the speed
//will be the stopping speed.
    if(ExitSpeedFull > 0)
        {
        if(Track->TrackElementAt(746, CurrentTrackVectorPosition).TrackType == Points)
            {
            if((EntryPos == 0) || (EntryPos == 2))
                {
                if(Track->TrackElementAt(747, CurrentTrackVectorPosition).Attribute == 0) ExitPos = 1; else ExitPos = 3;
                }
            else ExitPos = 0;
            }
        else ExitPos = Track->GetNonPointsOppositeLinkPos(EntryPos);
        NextTrackVectorPosition = Track->TrackElementAt(748, CurrentTrackVectorPosition).Conn[ExitPos];
        NextEntryPos = Track->TrackElementAt(749, CurrentTrackVectorPosition).ConnLinkPos[ExitPos];
        if(NextTrackVectorPosition > -1)//not a continuation or buffer
            {
            int NextElementLength;
            if(NextEntryPos > 1)
                {
                NextElementLength = (Track->TrackElementAt(750, NextTrackVectorPosition).Length23);
                }
            else
                {
                NextElementLength = (Track->TrackElementAt(751, NextTrackVectorPosition).Length01);
                }
            double NextStoppingSpeed = sqrt(3.6 * 3.6 * 2 * BrakeRate * NextElementLength);
            if(ExitSpeedFull < (0.8 * NextStoppingSpeed))
                {
                ExitSpeedFull = 0;
                }
            }
        }
    }

TempBrakeRate=MinSingle; TempBrakeRate=MaxSingle; TempBrakeRate=MinDouble; TempBrakeRate=MaxDouble;//included to stop warnings from unused declarations in math.hpp
TempBrakeRate=MinExtended; TempBrakeRate=MaxExtended; TempBrakeRate=MinComp; TempBrakeRate=MaxComp;//included to stop warnings from unused declarations in math.hpp
Utilities->CallLogPop(707);
}

//---------------------------------------------------------------------------
/*
bool TTrain::IsTerminalStation(int TrackVectorPosition, int EntryPos)
{
int NextExitPos;
TTrackElement NextElement = Track->TrackElementAt(379, TrackVectorPosition), TempElement;
if(TimetableVector.empty()) return false;
if(NextElement.ActiveTrackElementName != TimetableVector.begin()->LocationName) return false;
while((NextElement.ActiveTrackElementName == TimetableVector.begin()->LocationName) && (NextElement.TrackType != Buffers))
    {
    //check for points & follow attribute, but don't worry about a derail as that dealt with elsewhere
    if((NextElement.TrackType != Points) || ((EntryPos != 0) && (EntryPos != 2)))
        {
        NextExitPos = Track->GetNonPointsOppositeLinkPos(EntryPos);
        }
   	else if((NextElement.TrackType == Points) && ((EntryPos == 0) || (EntryPos == 2)))
   		{
   		if(NextElement.Attribute == 0) NextExitPos = 1; else NextExitPos = 3;
   		}
    TempElement = Track->TrackElementAt(380, NextElement.Conn[NextExitPos]);//need temp as NextElement used in next step
    NextElement = TempElement;
    }
if(NextElement.ActiveTrackElementName != TimetableVector.begin()->LocationName) return false;
if(NextElement.TrackType == Buffers) return true;
return false;//shouldn't reach here but include to prevent compiler return warning
}
*/
//---------------------------------------------------------------------------

int TTrain::NameInTimetableBeforeCDT(int Caller, AnsiString Name, bool &Stop)
/*
returns the number by which the train ActionVectorEntryPtr needs
to be incremented to point to the location arrival entry or passtime entry before a change of direction.  Used to display missed
actions when a stop or pass location has been reached before other timetabled actions have been carried out.  If can't find it, or Name
is "", -1 is returned.  A change of direction is the limit of the search because a train may not stop at a location on the way out
but stop on way back, and in these circumstances no actions have been missed.  Stop indicates whether the train will stop at (true)
or pass (false) the location.
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NameInTimetableBeforeCDT," + Name);
Stop = false;
if(TimetableFinished || (Name == ""))
    {
    Utilities->CallLogPop(957);
    return -1;
    }
//start looking from current pointer position
for(TActionVectorEntry *Ptr = ActionVectorEntryPtr; Ptr<TrainDataEntryPtr->ActionVector.end();Ptr++)
    {
    if((Ptr->Command == "cdt") || (Ptr->FormatType == Repeat))
        {
        break;
        }
    if((Ptr->ArrivalTime > TDateTime(-1)) && (Ptr->LocationName == Name))
        {
        if((Ptr->FormatType == TimeLoc) || (Ptr->FormatType == TimeTimeLoc))
            {
            Stop = true;
            Utilities->CallLogPop(960);
            return (Ptr - ActionVectorEntryPtr);
            }
        }
    if((Ptr->EventTime > TDateTime(-1)) && (Ptr->LocationName == Name) && (Ptr->Command == "pas"))
        {
        Utilities->CallLogPop(1517);
        return (Ptr - ActionVectorEntryPtr);
        }
    }
Utilities->CallLogPop(959);
return -1;//not found a valid entry
}

//---------------------------------------------------------------------------

bool TTrain::ClearToNextSignal(int Caller)
/*Checks forward from train LeadElement, following leading point attributes but ignoring trailing point attributes,
until finds either a train or a signal/buffers/continuation/loop.  If finds a train returns false, else returns true.
Ignores the call-on signal.
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ClearToNextSignal");
int ReturnVal = 0;
for(unsigned int x=0; x<Track->TrackVector.size();x++)
    {
    Track->TrackVector.at(x).TempTrackMarker01 = false;
    Track->TrackVector.at(x).TempTrackMarker23 = false;
    }
int CurrentTrackVectorPosition = LeadElement, NextTrackVectorPosition;
int EntryPos = LeadEntryPos, ExitPos, NextEntryPos;
while(true)
    {
    if((Track->TrackElementAt(382, CurrentTrackVectorPosition).TrainIDOnElement > -1) &&
            (Track->TrackElementAt(383, CurrentTrackVectorPosition).TrainIDOnElement != TrainID))
        {
        ReturnVal = 1;
        break;
        }
    if(((Track->TrackElementAt(384, CurrentTrackVectorPosition).TrackType == Buffers) ||
            (Track->TrackElementAt(385, CurrentTrackVectorPosition).TrackType == Continuation)) && (EntryPos == 1))
        {
        ReturnVal = 2;
        break;
        }
    if((EntryPos < 2) && (Track->TrackElementAt(386, CurrentTrackVectorPosition).Config[1 - EntryPos] == Signal) &&
            (Track->TrackElementAt(529, CurrentTrackVectorPosition).Attribute != 4))//Attr 4 == call-on signal
        {
        ReturnVal = 3;
        break;
        }
    if((Track->TrackElementAt(387, CurrentTrackVectorPosition).TrackType == Bridge) ||
            (Track->TrackElementAt(388, CurrentTrackVectorPosition).TrackType == Crossover))
        {
        if((EntryPos < 2) && (Track->TrackElementAt(523, CurrentTrackVectorPosition).TempTrackMarker01))
        //must be a loop - reached same point as examined earlier
            {
            ReturnVal = 4;
            break;
            }
        else if((EntryPos > 1) && (Track->TrackElementAt(524, CurrentTrackVectorPosition).TempTrackMarker23))
            {
            ReturnVal = 4;
            break;
            }
        }
    else
        {
        if((Track->TrackElementAt(525, CurrentTrackVectorPosition).TempTrackMarker01) ||
                (Track->TrackElementAt(526, CurrentTrackVectorPosition).TempTrackMarker23))
            {
            ReturnVal = 4;
            break;
            }
        }
    if(EntryPos < 2) Track->TrackElementAt(389, CurrentTrackVectorPosition).TempTrackMarker01 = true;
    else Track->TrackElementAt(527, CurrentTrackVectorPosition).TempTrackMarker23 = true;

    if(Track->TrackElementAt(390, CurrentTrackVectorPosition).TrackType == Points)
        {
        if((EntryPos == 0) || (EntryPos == 2))
            {
            if(Track->TrackElementAt(391, CurrentTrackVectorPosition).Attribute == 0) ExitPos = 1; else ExitPos = 3;
            }
        else ExitPos = 0;
        }
    else ExitPos = Track->GetNonPointsOppositeLinkPos(EntryPos);
    NextTrackVectorPosition = Track->TrackElementAt(392, CurrentTrackVectorPosition).Conn[ExitPos];
    NextEntryPos = Track->TrackElementAt(393, CurrentTrackVectorPosition).ConnLinkPos[ExitPos];
    CurrentTrackVectorPosition = NextTrackVectorPosition;
    EntryPos = NextEntryPos;
    }
if(ReturnVal == 1)
    {
    Utilities->CallLogPop(708);
    return false;
    }
if(ReturnVal == 2)
    {
    Utilities->CallLogPop(709);
    return true;
    }
if(ReturnVal == 3)
    {
    Utilities->CallLogPop(946);
    return true;
    }
if(ReturnVal == 4)
    {
    Utilities->CallLogPop(947);
    return true;
    }
else
    {
    throw Exception("Error - failed to set ReturnVal in ClearToNextSignal()");
    }
}

//---------------------------------------------------------------------------

bool TTrain::CallingOnAllowed(int Caller)
/*
Check whether calling-on conditions met - a) approaching train has stopped at a signal but not at a location;
b) if there is a facing train at the station, it is being held appropriately (must be awaiting a join (Fjo or jbo) or a
change of direction (cdt), remaining here (Frh), or under signaller control);
c) at least 1 platform available for the approaching train; d) points (if any) set for direct route into platform;
e) approaching train is to stop at station; f) no more facing signals between train and platform; g) [dropped g]
h) train in front preventing route being set; i) train in front not exiting at continuation; j) signal must be within 4km of
the stop platform; and k) either an autosigs route already set into station or able to set an unrestricted route into station.
If all OK & autosigs route not already set then set an unrestricted route into the station (just to the first platform)
*/
{
if(AllRoutes->RouteFlashFlag) return false;//don't want to create a new route from the stop signal if one is already in construction as
                                           //some of the callingon route elements may be involved
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CallingOnAllowed");
bool PlatformFoundFlag = false, StopRequired = false, SkipRouteCheck = false, AutoSigs = false;
int CurrentTrackVectorPosition = LeadElement, NextTrackVectorPosition, ElementNumber = 0, Distance = 0;
int RouteStartPosition; //this is the track vector position of the start element for the new unrestricted route - one past the stop signal
int PlatformPosition; // the track vector position of the first stop platfrom
int EntryPos = LeadEntryPos, ExitPos, NextEntryPos;
AnsiString LeadStationName = Track->TrackElementAt(395, LeadElement).ActiveTrackElementName;//still OK even if ""
//must be stopped at a signal but not at a location & still in timetable (a)

if(StoppedAtLocation || !StoppedAtSignal || TimetableFinished || Crashed || Derailed || DerailPending ||
        StoppedAfterSPAD || StoppedForTrainInFront) //no need to check for SignallerStopped as this only called in Timetable mode
    {
    Utilities->CallLogPop(711);
    return false;
    }
while(true)
    {
    TTrackElement &CurrentTrackElement = Track->TrackElementAt(396, CurrentTrackVectorPosition);
    //don't look further than 4km ahead (j)
    if(Distance > 4000)
        {
        Utilities->CallLogPop(967);
        return false;
        }
    //don't look further than 15 elements ahead (g) - dropped, 4km better
/*
    if(Count > 15)
        {
        Utilities->CallLogPop(712);
        return false;
        }
*/
    //if find another train on an element in front, before find a valid platform, return false (c)
    if((CurrentTrackElement.TrainIDOnElement > -1) && (CurrentTrackElement.TrainIDOnElement != TrainID) && !PlatformFoundFlag)
        {
        Utilities->CallLogPop(713);
        return false;
        }
    //if find another train in front when there is a valid platform (keep searching after find a platform as train may still
    //be facing later on)
    if((CurrentTrackElement.TrainIDOnElement > -1) && (CurrentTrackElement.TrainIDOnElement != TrainID) && PlatformFoundFlag)
        {
        //get LeadElement, if -1 return (could be exiting at continuation) (i)
        TTrain OtherTrain = TrainController->TrainVectorAtIdent(12, CurrentTrackElement.TrainIDOnElement);
        if(OtherTrain.LeadElement == -1)
            {
            Utilities->CallLogPop(714);
            return false;
            }
        //if a facing train then make sure it is awaiting a join (Fjo or jbo) or a change of direction (cdt), or remaining here (Frh) (b)
        if(OtherTrain.LeadElement == CurrentTrackVectorPosition)
            {
            AnsiString OtherCommand = OtherTrain.ActionVectorEntryPtr->Command;
            if((OtherCommand == "Fjo") || (OtherCommand == "jbo") || (OtherCommand == "cdt") || (OtherCommand == "Frh") ||
                    (OtherTrain.TrainMode == Signaller))
                {
                break;
                }
            else
                {
                Utilities->CallLogPop(955);
                return false;
                }
            }
        else //(h)
            {
            break;
            }
        }
    //if reach buffers or exit continuation return false (can set route)
    if(((CurrentTrackElement.TrackType == Buffers) ||
            (CurrentTrackElement.TrackType == Continuation)) && (EntryPos == 1))
        {
        Utilities->CallLogPop(716);
        return false;
        }
    //if reach forward signal (other than the one the train is waiting at) return false (can set route) (f)
    if((EntryPos < 2) && (CurrentTrackElement.Config[1 - EntryPos] == Signal) &&
        (CurrentTrackVectorPosition != Track->TrackElementAt(404, LeadElement).Conn[LeadExitPos]))
        {
        Utilities->CallLogPop(717);
        return false;
        }
    //if reach a location that isn't in timetable return false - drop this as still can't set a route
/*
    if((Track->TrackElementAt(405, CurrentTrackVectorPosition).ActiveTrackElementName != "") && (Track->TrackElementAt(406, CurrentTrackVectorPosition).ActiveTrackElementName != LeadStationName) &&
            (NameInTimetableBeforeCDT(14, Track->TrackElementAt(407, CurrentTrackVectorPosition).ActiveTrackElementName) == -1))
        {
        Utilities->CallLogPop(718);
        return false;
        }
*/
    //if reach a location that is in timetable set PlatformFoundFlag (but not if position is points set to diverge) (e)
    if((CurrentTrackElement.ActiveTrackElementName != "") && (CurrentTrackElement.ActiveTrackElementName != LeadStationName) &&
            (NameInTimetableBeforeCDT(15, CurrentTrackElement.ActiveTrackElementName, StopRequired) > -1))
        {
        if(StopRequired)
            {
            if((CurrentTrackElement.TrackType != Points) || ((CurrentTrackElement.TrackType == Points) &&
                    (CurrentTrackElement.Attribute == 0)))
                {
                if(!PlatformFoundFlag) PlatformPosition = CurrentTrackVectorPosition; //ensure this only set once at first valid platform position - the unrestricted route will end here
                PlatformFoundFlag = true;
                }
            }
        }
    //Drop this below - was to prevent call-on if front train had left the station.  Criterion now is not that front
    //train has to be at station but that has to be before the next forward signal
/*
    if((Track->TrackElementAt(411, CurrentTrackVectorPosition).ActiveTrackElementName == "") && (PlatformFoundFlag))
        {
        Utilities->CallLogPop(719);
        return false;
        }
*/
    //make sure points are followed correctly (d) & set ExitPos
    if(CurrentTrackElement.TrackType == Points)
        {
        if((EntryPos == 0) || (EntryPos == 2))
            {
            if(CurrentTrackElement.Attribute == 0) ExitPos = 1; else ExitPos = 3;
            }
        if(EntryPos == 1)
            {
            if(CurrentTrackElement.Attribute == 0) ExitPos = 0;
            else
                {
                Utilities->CallLogPop(720);
                return false;
                }
            }
        if(EntryPos == 3)
            {
            if(CurrentTrackElement.Attribute == 1) ExitPos = 0;
            else
                {
                Utilities->CallLogPop(721);
                return false;
                }
            }
        }
    else ExitPos = Track->GetNonPointsOppositeLinkPos(EntryPos);

    //check existing routes - if element forward of the signal (ElementNumber == 2) is AutoSignals then OK as this route must extend to
    //the next signal so must at least reach the station, otherwise reject if (1) there are any route elements already set from element
    //forward of signal to & including the first platform element (covers crossover with other route set) or (2) a fouled diagonal (k)
    if(ElementNumber < 2) SkipRouteCheck = true;
    else SkipRouteCheck = false;
    if(ElementNumber == 1) //the stop signal
        {
        RouteStartPosition = CurrentTrackVectorPosition;
        }
    if(ElementNumber == 2)
        {
        int RouteID; //not used here
        if(AllRoutes->GetRouteTypeAndNumber(18, CurrentTrackVectorPosition, EntryPos, RouteID) == AllRoutes->AutoSigsRoute) AutoSigs = true;
        else AutoSigs = false;
        }
    if(!SkipRouteCheck && !AutoSigs)
        {
        if(AllRoutes->TrackIsInARoute(16, CurrentTrackVectorPosition, EntryPos))
            {
            Utilities->CallLogPop(1859);
            return false;
            }
        int ExitLink = CurrentTrackElement.Link[ExitPos];
        if((ExitLink == 1) || (ExitLink == 3) || (ExitLink == 7) || (ExitLink == 9))
            {
            if(AllRoutes->FouledDiagonal(6, CurrentTrackElement.HLoc, CurrentTrackElement.VLoc, ExitLink))
                {
                Utilities->CallLogPop(1850);
                return false;
                }
            }
        }

    //finished all checks, now update CurrentTrackVectorPosition & EntryPos for the next iteration
    if(EntryPos < 2) Distance+= CurrentTrackElement.Length01;
    else Distance+= CurrentTrackElement.Length23;
    NextTrackVectorPosition = CurrentTrackElement.Conn[ExitPos];
    NextEntryPos = CurrentTrackElement.ConnLinkPos[ExitPos];
    CurrentTrackVectorPosition = NextTrackVectorPosition;
    EntryPos = NextEntryPos;
    ElementNumber++;
    }//while(true)

//if all OK & autosigs route not already set then set an unrestricted route into the station (just to the first platform)
//from the stop signal (note that it may be last in an autosigs route)
//a single element route at the stop signal should have been removed prior to this function being called (that called before
//this in ClockTimer2)

//now add elements to the CallonVector
TAllRoutes::TCallonEntry CallonEntry(AutoSigs, RouteStartPosition, PlatformPosition);
AllRoutes->CallonVector.push_back(CallonEntry);
Utilities->CallLogPop(1860);
return true; //return false if fail to set route for any reason
}

//---------------------------------------------------------------------------
/*
bool TTrain::TimetableFinished()
{
if((ActionVectorEntryPtr == TrainDataEntryPtr->ActionVector.end()) || (ActionVectorEntryPtr->FormatType == Repeat))//past all actions
    {
    return true;
    }
return false;
}
*/
//---------------------------------------------------------------------------

AnsiString TTrain::GetTrainHeadCode(int Caller)

{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTrainHeadCode");
AnsiString RepeatHeadCode = TrainController->GetRepeatHeadCode(0, HeadCode, RepeatNumber, IncrementalDigits);
Utilities->CallLogPop(1452);
return RepeatHeadCode;
}

//---------------------------------------------------------------------------

TDateTime TTrain::GetTrainTime(int Caller, TDateTime Time)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTrainTime," + Utilities->Format96HHMMSS(Time));
TDateTime RepeatTime = TrainController->GetRepeatTime(1, Time, RepeatNumber, IncrementalMinutes);
Utilities->CallLogPop(1453);
return RepeatTime;
}

//---------------------------------------------------------------------------

void TTrain::LogAction(int Caller, AnsiString OwnHeadCode, AnsiString OtherHeadCode, TActionType ActionType, AnsiString LocationName, TDateTime TimetableNonRepeatTime, bool Warning)
/*
Time = timetable time, the time adjustments for repeat trains is carried out internally
Not all messages need this, if not needed a dummy value is required but not used

Arrive:  06:05:40: 2F46 arrived at Old Street 1 minute late
Pass:  06:05:40: 2F46 passed Old Street 1 minute late
Terminate:  06:05:40: 2F46 terminated at Old Street 1 minute late
//NB for Frh just give terminated message but without event time - don't use this function
Depart:  06:05:15: 3F43 departed from Essex Road 2 minutes late
Create:  06:05:40: 2F46 created at Old Street 1 minute late
Enter:  06:05:40: 2F46 entered railway at Old Street 1 minute late
Leave:  06:05:40: 2F46 left railway at 57-N4 1 minute late
FrontSplit:  06:05:40: 2F46 split from front to 3D54 at Old Street 1 minute late
RearSplit:  06:05:40: 2F46 split from rear to 3D54 at Old Street 1 minute late
JoinedByOther:  06:05:40: 2F46 joined by 3D54 at Old Street 1 minute late
ChangeDirection:  06:05:40: 2F46 changed direction at Old Street 1 minute late
NewService:  06:05:40: 2F46 became new service 3D54 at Old Street 1 minute late
TakeManualControl:  06:05:40: 2F46 taken under signaller control at Old Street
RestoreTimetableControl:  06:05:40: 2F46 restored to timetable control at Old Street
RemoveTrain:  06:05:40: 2F46 REMOVED FROM RAILWAY DUE TO CRASH at Old Street
RemoveTrain:  06:05:40: 2F46 REMOVED FROM RAILWAY DUE TO DERAILMENT at Old Street
RemoveTrain:  06:05:40: 2F46 REMOVED FROM RAILWAY at Old Street
SignallerMoveForwards  06:05:40: 2F46 received signaller authority to proceed
SignallerChangeDirection  06:05:40: 2F46 changed direction under signaller control at Old Street
SignallerPassRedSignal  06:05:40: 2F46 received signaller authority to pass red signal
SignallerControlStop  06:05:40: 2F46 received signaller instruction to stop
SignallerStop  06:05:40: 2F46 stopped on signaller command
SignallerLeave:  06:05:40: 2F46 left railway under signaller control at 57-N4
SignallerStepForward:  06:05:40: 2F46 received signaller authority to step forward
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LogAction," + OwnHeadCode + "," + OtherHeadCode + "," + AnsiString(ActionType) + "," + LocationName);
AnsiString BaseLog="", WarningBaseLog="", PerfLog="", ActionLog="";
int IntMinsLate = 0;//need to set it in case MinsLate == 0, since it isn't tested for that
if(ActionType == Arrive) ActionLog = " arrived at ";
if(ActionType == Terminate)
    {
    if(TerminatedMessageSent)//to avoid it being sent twice
        {
        Utilities->CallLogPop(1104);
        return;
        }
    ActionLog = " terminated at ";
    TerminatedMessageSent = true;
    }
if(ActionType == Depart) ActionLog = " departed from ";
if(ActionType == Pass) ActionLog = " passed ";
if(ActionType == Create) ActionLog = " created at ";
if(ActionType == Enter) ActionLog = " entered railway at ";
if(ActionType == Leave) ActionLog = " left railway at ";
if(ActionType == FrontSplit) ActionLog = " split from front to ";
if(ActionType == RearSplit) ActionLog = " split from rear to ";
if(ActionType == JoinedByOther) ActionLog = " joined by ";
if(ActionType == ChangeDirection) ActionLog = " changed direction at ";
if(ActionType == NewService) ActionLog = " became new service ";
if(ActionType == TakeSignallerControl) ActionLog = " taken under signaller control at ";
if(ActionType == RestoreTimetableControl) ActionLog = " restored to timetable control at ";
if(ActionType == RemoveTrain)
    {
    if(Crashed) ActionLog = " REMOVED FROM RAILWAY DUE TO CRASH at ";
    else if(Derailed) ActionLog = " REMOVED FROM RAILWAY DUE TO DERAILMENT at ";
        else ActionLog = " REMOVED FROM RAILWAY at ";
    }
if(ActionType == SignallerMoveForwards) ActionLog = " received signaller authority to proceed";
if(ActionType == SignallerStepForward) ActionLog = " received signaller authority to step forward";
if(ActionType == SignallerChangeDirection) ActionLog = " changed direction under signaller control at ";
if(ActionType == SignallerPassRedSignal) ActionLog = " received signaller authority to pass red signal";
if(ActionType == SignallerControlStop) ActionLog = " received signaller instruction to stop";
if(ActionType == SignallerStop) ActionLog = " stopped on signaller instruction ";
if(ActionType == SignallerLeave) ActionLog = " left railway under signaller control at ";
if(OtherHeadCode != "") OtherHeadCode+= " at ";
TDateTime ActualTime = TrainController->TTClockTime;
if(Warning)
    {
    BaseLog = Utilities->Format96HHMMSS(ActualTime) + " WARNING: " + HeadCode + ActionLog + OtherHeadCode + LocationName;
    WarningBaseLog = HeadCode + ActionLog + OtherHeadCode + LocationName;
    }
else
    {
    BaseLog = Utilities->Format96HHMMSS(ActualTime) + ": " + HeadCode + ActionLog + OtherHeadCode + LocationName;
    }
bool TimePerformance = true;
if((ActionType == TakeSignallerControl) || (ActionType == RestoreTimetableControl) || (ActionType == RemoveTrain) ||
        (ActionType == SignallerMoveForwards) || (ActionType == SignallerChangeDirection) ||
        (ActionType == SignallerPassRedSignal) || (ActionType == SignallerControlStop) || (ActionType == SignallerStop) ||
        (ActionType == SignallerLeave) || (ActionType == SignallerStepForward))
    {
    TimePerformance = false;
    }
if(TimePerformance)
    {
    double MinsLate = ((double)(ActualTime - GetTrainTime(1, TimetableNonRepeatTime)))*1440;
    if(MinsLate < 0)
        {
        IntMinsLate = int(ceil(MinsLate));
        }
    if(MinsLate > 0)
        {
        IntMinsLate = int(floor(MinsLate));
        }
    if(IntMinsLate == 0)
        {
        PerfLog = " on time";
        }
    else if(IntMinsLate == 1) PerfLog = " 1 minute late";
    else if(IntMinsLate == -1) PerfLog = " 1 minute early";
    else if(IntMinsLate > 1) PerfLog = " " + AnsiString(IntMinsLate) + " minutes late";
    else if(IntMinsLate < -1)
        {
        int PosIntMinsLate = -IntMinsLate;
        PerfLog = " " + AnsiString(PosIntMinsLate) + " minutes early";
        }
    if(LocationName.Pos('-') > 0)
        {
        PerfLog = "," + PerfLog;//if a position add a comma to separate vertical position number from number of minutes (better appearance)
        }
    Display->PerformanceLog(0, BaseLog + PerfLog);
    }
else Display->PerformanceLog(1, BaseLog);
if(Warning)
    {
    Display->WarningLog(0, WarningBaseLog);
    }

//update statistics
if((ActionType == Arrive) && (IntMinsLate == 0))
    {
    TrainController->OnTimeArrivals++;
    }
else if((ActionType == Arrive) && (IntMinsLate > 0))
    {
    TrainController->LateArrivals++;
    TrainController->TotLateArrMins+= IntMinsLate;
    }
else if((ActionType == Arrive) && (IntMinsLate < 0))
    {
    TrainController->EarlyArrivals++;
    TrainController->TotEarlyArrMins+= abs(IntMinsLate);
    }
else if((ActionType == Pass) && (IntMinsLate == 0))
    {
    TrainController->OnTimePasses++;
    }
else if((ActionType == Pass) && (IntMinsLate > 0))
    {
    TrainController->LatePasses++;
    TrainController->TotLatePassMins+= IntMinsLate;
    }
else if((ActionType == Pass) && (IntMinsLate < 0))
    {
    TrainController->EarlyPasses++;
    TrainController->TotEarlyPassMins+= abs(IntMinsLate);
    }
else if((ActionType == Depart) && (IntMinsLate == 0))
    {
    TrainController->OnTimeDeps++;
    }
else if((ActionType == Depart) && (IntMinsLate > 0))
    {
    TrainController->LateDeps++;
    TrainController->TotLateDepMins+= IntMinsLate;
    }
Utilities->CallLogPop(968);
}

//---------------------------------------------------------------------------

void TTrain::FrontTrainSplit(int Caller)
{
/*
Split logic is:-  at least one of 4 final train positions must overlap with one of original train positions, & final 4 positions
will maximise the number at the location.  Note that this function isn't sophisticated enough to account for trains already at the
location in determining the 4 positions, and will give a failure message if a train obstructs any of the 4 positions.  In these
circumstances the other train will need to be move sufficiently away to release all 4 positions, then the train will split.
*/
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FrontTrainSplit");
AnsiString LocationName = Track->TrackElementAt(555, LeadElement).ActiveTrackElementName;
if(LocationName == "") LocationName = Track->TrackElementAt(837, MidElement).ActiveTrackElementName;
int FirstNamedElementPos, SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos;
int RearTrainRearPosition, RearTrainFrontPosition, RearTrainExitPos;
int FrontTrainRearPosition, FrontTrainFrontPosition, FrontTrainExitPos;
AnsiString OtherHeadCode = TrainController->GetRepeatHeadCode(1, ActionVectorEntryPtr->OtherHeadCode, RepeatNumber, IncrementalDigits);
//determine all positions & exits
if(LocationName != "")
    {
    //if message given only call at ~5 sec intervals
    if(!(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported == FailLocTooShort) || (UpdateCounter == 0))
        {
        FirstNamedElementPos = LeadElement;
        if(!Track->ThisNamedLocationLongEnoughForSplit(0, LocationName, FirstNamedElementPos,   //check if possible with LeadElement as First
                SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos))
            {
            FirstNamedElementPos = MidElement;
            if(!Track->ThisNamedLocationLongEnoughForSplit(1, LocationName, FirstNamedElementPos, //if not then accept second if possible (though if Lead no good hard to see how Mid could work, but leave in)
                    SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos))
                {
                if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != FailLocTooShort)
                    {
                    TrainController->LogActionError(6, HeadCode, "", FailLocTooShort, LocationName);
                    TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = FailLocTooShort;
                    }
                Utilities->CallLogPop(1009);
                return;
                }
            }
        else
            {
             //if first is possible then check if all 4 positions at location, and if not try the second
            int LeadPosA = FirstNamedElementPos;
            int LeadPosB = FirstNamedLinkedElementPos;
            int LeadPosC = SecondNamedElementPos;
            int LeadPosD = SecondNamedLinkedElementPos;
            //count number of positions that are at the location
            int LeadNumAtLoc = 0;
            if(Track->TrackElementAt(758, LeadPosA).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(Track->TrackElementAt(759, LeadPosB).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(Track->TrackElementAt(760, LeadPosC).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(Track->TrackElementAt(761, LeadPosD).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(LeadNumAtLoc < 4)
                {
                FirstNamedElementPos = MidElement;
                if(!Track->ThisNamedLocationLongEnoughForSplit(4, LocationName, FirstNamedElementPos,
                    SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos))
                    {//restore originals
                    FirstNamedElementPos = LeadPosA;
                    FirstNamedLinkedElementPos = LeadPosB;
                    SecondNamedElementPos = LeadPosC;
                    SecondNamedLinkedElementPos = LeadPosD;
                    }
                else  //count number at location
                    {
                    int MidNumAtLoc = 0;
                    if(Track->TrackElementAt(762, FirstNamedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(Track->TrackElementAt(763, FirstNamedLinkedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(Track->TrackElementAt(764, FirstNamedLinkedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(Track->TrackElementAt(765, SecondNamedLinkedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(LeadNumAtLoc > MidNumAtLoc)//change back, else keep new values
                        {
                        FirstNamedElementPos = LeadPosA;
                        FirstNamedLinkedElementPos = LeadPosB;
                        SecondNamedElementPos = LeadPosC;
                        SecondNamedLinkedElementPos = LeadPosD;
                        }
                    }
                }
            }
        }
    else
        {
        Utilities->CallLogPop(1791);
        return;
        }
    }
else throw Exception("Error - LocationName not set in FrontTrainSplit");
//set front & rear train parameters
//need RearTrainRearPosition, RearTrainFrontPosition, RearTrainExitPos, FrontTrainRearPosition, FrontTrainFrontPosition & FrontTrainExitPos;
//have LeadElement & MidElement of train defining its direction, & one or other on FirstNamedElementPos
//have FirstNamedElementPos, SecondNamedElementPos, FirstNamedLinkedElementPos & SecondNamedLinkedElementPos from ThisNamedLocationLongEnoughForSplit.
if(LeadElement == FirstNamedElementPos)
    {
    if(MidElement == SecondNamedElementPos)
        {
        FrontTrainFrontPosition = FirstNamedLinkedElementPos;
        FrontTrainRearPosition = FirstNamedElementPos;
        RearTrainFrontPosition = SecondNamedElementPos;
        RearTrainRearPosition = SecondNamedLinkedElementPos;
        }
    else //MidElement must == FirstNamedLinkedElementPos
        {
        FrontTrainFrontPosition = SecondNamedLinkedElementPos;
        FrontTrainRearPosition = SecondNamedElementPos;
        RearTrainFrontPosition = FirstNamedElementPos;
        RearTrainRearPosition = FirstNamedLinkedElementPos;
        }
    }
else //MidElement == FirstNamedElementPos
    {
    if(LeadElement == SecondNamedElementPos)
        {
        FrontTrainFrontPosition = SecondNamedLinkedElementPos;
        FrontTrainRearPosition = SecondNamedElementPos;
        RearTrainFrontPosition = FirstNamedElementPos;
        RearTrainRearPosition = FirstNamedLinkedElementPos;
        }
    else //LeadElement must == FirstNamedLinkedElementPos
        {
        FrontTrainFrontPosition = FirstNamedLinkedElementPos;
        FrontTrainRearPosition = FirstNamedElementPos;
        RearTrainFrontPosition = SecondNamedElementPos;
        RearTrainRearPosition = SecondNamedLinkedElementPos;
        }
    }
RearTrainExitPos = -1;
for(int x=0;x<4;x++)
    {
    if(Track->TrackElementAt(584, RearTrainRearPosition).Conn[x] == RearTrainFrontPosition)
        {
        RearTrainExitPos = x;
        break;
        }
    }
if(RearTrainExitPos == -1) throw Exception("Error - RearTrainRearPosition not linked to RearTrainFrontPosition in FrontTrainSplit");

FrontTrainExitPos = -1;
for(int x=0;x<4;x++)
    {
    if(Track->TrackElementAt(585, FrontTrainRearPosition).Conn[x] == FrontTrainFrontPosition)
        {
        FrontTrainExitPos = x;
        break;
        }
    }
if(FrontTrainExitPos == -1) throw Exception("Error - FrontTrainRearPosition not linked to FrontTrainFrontPosition in FrontTrainSplit");

//check no train (apart from self) on any of the 4 elements & fail if so
int TrainIDOnRearOfRearTrain, TrainIDOnFrontOfRearTrain, TrainIDOnRearOfFrontTrain, TrainIDOnFrontOfFrontTrain;
TTrackElement RearMostElement = Track->TrackElementAt(574, RearTrainRearPosition);
if((RearMostElement.TrackType == Bridge) && (RearTrainExitPos > 1)) TrainIDOnRearOfRearTrain = RearMostElement.TrainIDOnBridgeTrackPos23;
else if((RearMostElement.TrackType == Bridge) && (RearTrainExitPos < 2)) TrainIDOnRearOfRearTrain = RearMostElement.TrainIDOnBridgeTrackPos01;
else TrainIDOnRearOfRearTrain = RearMostElement.TrainIDOnElement;
//RearTrainFrontPosition = RearMostElement.Conn[RearTrainExitPos];
TrainIDOnFrontOfRearTrain = Track->TrackElementAt(575, RearTrainFrontPosition).TrainIDOnElement;//can't be a bridge
TrainIDOnRearOfFrontTrain = Track->TrackElementAt(576,FrontTrainRearPosition).TrainIDOnElement;//can't be a bridge
//FrontTrainFrontPosition = Track->TrackElementAt(578,FrontTrainRearPosition).Conn[FrontTrainExitPos];
TTrackElement FrontMostElement = Track->TrackElementAt(577,FrontTrainFrontPosition);
if((FrontMostElement.TrackType == Bridge) && (FrontTrainExitPos > 1)) TrainIDOnFrontOfFrontTrain = FrontMostElement.TrainIDOnBridgeTrackPos23;
else if((FrontMostElement.TrackType == Bridge) && (FrontTrainExitPos < 2)) TrainIDOnFrontOfFrontTrain = FrontMostElement.TrainIDOnBridgeTrackPos01;
else TrainIDOnFrontOfFrontTrain = FrontMostElement.TrainIDOnElement;

if(((TrainIDOnRearOfRearTrain > -1) && (TrainIDOnRearOfRearTrain != TrainID)) ||
        ((TrainIDOnFrontOfRearTrain > -1) && (TrainIDOnFrontOfRearTrain != TrainID)) ||
        ((TrainIDOnRearOfFrontTrain > -1) && (TrainIDOnRearOfFrontTrain != TrainID)) ||
        ((TrainIDOnFrontOfFrontTrain > -1) && (TrainIDOnFrontOfFrontTrain != TrainID)))
    {
    if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != FailSplitDueToOtherTrain)
        {
        TrainController->LogActionError(8, HeadCode, "", FailSplitDueToOtherTrain, LocationName);
        TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = FailSplitDueToOtherTrain;
        }
    //don't advance ActionVectorEntryPtr as need to keep trying, other train may move off eventually
    Utilities->CallLogPop(1010);
    return;
    }

if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported == FailSplitDueToOtherTrain)
    {
    TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
    }
//reposition existing rear train, need to do this first for 2 reasons - 1) will likely be in the way of the new front train, and 2)
//the new train will likely cause a reallocation of the TrainVector, and if so the reference to the existing train will be invalidated.
//Hence deal with existing train while it references a valid entry in the vector, but retain the Old ActionVectorEntryPtr in a separate
//variable as it is needed for setting up the new train
TActionVectorEntry *OldActionVectorEntryPtr = ActionVectorEntryPtr;
UnplotTrain(0);
StartSpeed = 0;
RearStartElement = RearTrainRearPosition;
RearStartExitPos = RearTrainExitPos;
StoppedAtLocation = true;
PlotStartPosition(3);
PlotTrainWithNewBackgroundColour(14, clStationStopBackground, Display);//pale green
LogAction(9, HeadCode, OtherHeadCode, FrontSplit, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;

Mass = Mass/2;
//TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).Mass = Mass;
MaxBrakeRate = MaxBrakeRate/2;
//TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).MaxBrakeRate = MaxBrakeRate;
PowerAtRail = PowerAtRail/2;
//TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).PowerAtRail = PowerAtRail;
//AValue = sqrt(2*PowerAtRail/Mass) i.e. AValue stays same

//create new front train
/*
TrainController::AddTrain(int RearPosition, int FrontPosition, AnsiString HeadCode, int StartSpeed, int Mass,
        double MaxRunningSpeed, double MaxBrakeRate, double PowerAtRail, AnsiString ModeStr, TTrainDataEntry *TrainDataEntryPtr,
        int RepeatNumber, int IncrementalMinutes, int SignallerSpeed)
*/
 //same Mass, MaxBrakeRate & PowerAtRail as this train's halved values, and same MaxRunningSpeed as this train
if(!TrainController->AddTrain(0, FrontTrainRearPosition, FrontTrainFrontPosition, OtherHeadCode, 0, Mass,
    MaxRunningSpeed, MaxBrakeRate, PowerAtRail, "Timetable", OldActionVectorEntryPtr->LinkedTrainEntryPtr,
    RepeatNumber, IncrementalMinutes, IncrementalDigits, SignallerMaxSpeed, false))//false for SignallerControl
    {
    Utilities->CallLogPop(1721);//if fails either a throw will have been sent in AddTrain or start position failed prob because of
                                //another train, in which case a message will have been sent to the perf log, also might well clear later
                                //when other train moves away
    return;
    }
TrainController->TrainAdded = true;

Utilities->CallLogPop(998);
}

//---------------------------------------------------------------------------

void TTrain::RearTrainSplit(int Caller)
{
/*
Split logic is:-  at least one of 4 final train positions must overlap with one of original train positions, & final 4 positions
will maximise the number at the location.  Note that this function isn't sophisticated enough to account for trains already at the
location in determining the 4 positions, and will give a failure message if a train obstructs any of the 4 positions.  In these
circumstances the other train will need to be moved sufficiently away to release all 4 positions, then the train will split.
*/
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RearTrainSplit");
AnsiString LocationName = Track->TrackElementAt(587, LeadElement).ActiveTrackElementName;
if(LocationName == "") LocationName = Track->TrackElementAt(838, MidElement).ActiveTrackElementName;
int FirstNamedElementPos, SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos;
int RearTrainRearPosition, RearTrainFrontPosition, RearTrainExitPos;
int FrontTrainRearPosition, FrontTrainFrontPosition, FrontTrainExitPos;
AnsiString OtherHeadCode = TrainController->GetRepeatHeadCode(2, ActionVectorEntryPtr->OtherHeadCode, RepeatNumber, IncrementalDigits);
//determine all positions & exits
if(LocationName != "")
    {
    //if message given only call at ~5 sec intervals
    if(!(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported == FailLocTooShort) || (UpdateCounter == 0))
        {
        FirstNamedElementPos = LeadElement;
        if(!Track->ThisNamedLocationLongEnoughForSplit(2, LocationName, FirstNamedElementPos,
                SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos))
            {
            FirstNamedElementPos = MidElement;
            if(!Track->ThisNamedLocationLongEnoughForSplit(3, LocationName, FirstNamedElementPos,
                    SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos))
                {
                if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != FailLocTooShort)
                    {
                    TrainController->LogActionError(9, HeadCode, "", FailLocTooShort, LocationName);
                    TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = FailLocTooShort;
                    }
                Utilities->CallLogPop(1013);
                return;
                }
            }
        else
            {
            //if first is possible then check if all 4 positions at location, and if not try the second
            int LeadPosA = FirstNamedElementPos;
            int LeadPosB = FirstNamedLinkedElementPos;
            int LeadPosC = SecondNamedElementPos;
            int LeadPosD = SecondNamedLinkedElementPos;
            //count number of positions that are at the location
            int LeadNumAtLoc = 0;
            if(Track->TrackElementAt(767, LeadPosA).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(Track->TrackElementAt(768, LeadPosB).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(Track->TrackElementAt(769, LeadPosC).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(Track->TrackElementAt(770, LeadPosD).ActiveTrackElementName == LocationName) LeadNumAtLoc++;
            if(LeadNumAtLoc < 4)
                {
                FirstNamedElementPos = MidElement;
                if(!Track->ThisNamedLocationLongEnoughForSplit(5, LocationName, FirstNamedElementPos,
                    SecondNamedElementPos, FirstNamedLinkedElementPos, SecondNamedLinkedElementPos))
                    {//restore originals
                    FirstNamedElementPos = LeadPosA;
                    FirstNamedLinkedElementPos = LeadPosB;
                    SecondNamedElementPos = LeadPosC;
                    SecondNamedLinkedElementPos = LeadPosD;
                    }
                else  //count number at location
                    {
                    int MidNumAtLoc = 0;
                    if(Track->TrackElementAt(771, FirstNamedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(Track->TrackElementAt(772, FirstNamedLinkedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(Track->TrackElementAt(773, FirstNamedLinkedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(Track->TrackElementAt(774, SecondNamedLinkedElementPos).ActiveTrackElementName == LocationName) MidNumAtLoc++;
                    if(LeadNumAtLoc > MidNumAtLoc)//change back, else keep new values
                        {
                        FirstNamedElementPos = LeadPosA;
                        FirstNamedLinkedElementPos = LeadPosB;
                        SecondNamedElementPos = LeadPosC;
                        SecondNamedLinkedElementPos = LeadPosD;
                        }
                    }
                }
            }
        }
    else
        {
        Utilities->CallLogPop(1792);
        return;
        }
    }
else throw Exception("Error - LocationName not set in RearTrainSplit");
//set front & rear train parameters
//need RearTrainRearPosition, RearTrainFrontPosition, RearTrainExitPos, FrontTrainRearPosition, FrontTrainFrontPosition & FrontTrainExitPos;
//have LeadElement & MidElement of train defining its direction, & one or other on FirstNamedElementPos
//have FirstNamedElementPos, SecondNamedElementPos, FirstNamedLinkedElementPos & SecondNamedLinkedElementPos from ThisNamedLocationLongEnoughForSplit.
if(LeadElement == FirstNamedElementPos)
    {
    if(MidElement == SecondNamedElementPos)
        {
        FrontTrainFrontPosition = FirstNamedLinkedElementPos;
        FrontTrainRearPosition = FirstNamedElementPos;
        RearTrainFrontPosition = SecondNamedElementPos;
        RearTrainRearPosition = SecondNamedLinkedElementPos;
        }
    else //MidElement must == FirstNamedLinkedElementPos
        {
        FrontTrainFrontPosition = SecondNamedLinkedElementPos;
        FrontTrainRearPosition = SecondNamedElementPos;
        RearTrainFrontPosition = FirstNamedElementPos;
        RearTrainRearPosition = FirstNamedLinkedElementPos;
        }
    }
else //MidElement == FirstNamedElementPos
    {
    if(LeadElement == SecondNamedElementPos)
        {
        FrontTrainFrontPosition = SecondNamedLinkedElementPos;
        FrontTrainRearPosition = SecondNamedElementPos;
        RearTrainFrontPosition = FirstNamedElementPos;
        RearTrainRearPosition = FirstNamedLinkedElementPos;
        }
    else //LeadElement must == FirstNamedLinkedElementPos
        {
        FrontTrainFrontPosition = FirstNamedLinkedElementPos;
        FrontTrainRearPosition = FirstNamedElementPos;
        RearTrainFrontPosition = SecondNamedElementPos;
        RearTrainRearPosition = SecondNamedLinkedElementPos;
        }
    }
RearTrainExitPos = -1;
for(int x=0;x<4;x++)
    {
    if(Track->TrackElementAt(588, RearTrainRearPosition).Conn[x] == RearTrainFrontPosition)
        {
        RearTrainExitPos = x;
        break;
        }
    }
if(RearTrainExitPos == -1) throw Exception("Error - RearTrainRearPosition not linked to RearTrainFrontPosition in RearTrainSplit");
FrontTrainExitPos = -1;
for(int x=0;x<4;x++)
    {
    if(Track->TrackElementAt(589, FrontTrainRearPosition).Conn[x] == FrontTrainFrontPosition)
        {
        FrontTrainExitPos = x;
        break;
        }
    }
if(FrontTrainExitPos == -1) throw Exception("Error - FrontTrainRearPosition not linked to FrontTrainFrontPosition in RearTrainSplit");

//check no train (apart from self) on any of the 4 elements & fail if so
int TrainIDOnRearOfRearTrain, TrainIDOnFrontOfRearTrain, TrainIDOnRearOfFrontTrain, TrainIDOnFrontOfFrontTrain;
TTrackElement RearMostElement = Track->TrackElementAt(590,RearTrainRearPosition);
if((RearMostElement.TrackType == Bridge) && (RearTrainExitPos > 1)) TrainIDOnRearOfRearTrain = RearMostElement.TrainIDOnBridgeTrackPos23;
else if((RearMostElement.TrackType == Bridge) && (RearTrainExitPos < 2)) TrainIDOnRearOfRearTrain = RearMostElement.TrainIDOnBridgeTrackPos01;
else TrainIDOnRearOfRearTrain = RearMostElement.TrainIDOnElement;
//RearTrainFrontPosition = RearMostElement.Conn[RearTrainExitPos];
TrainIDOnFrontOfRearTrain = Track->TrackElementAt(591, RearTrainFrontPosition).TrainIDOnElement;//can't be a bridge
TrainIDOnRearOfFrontTrain = Track->TrackElementAt(592,FrontTrainRearPosition).TrainIDOnElement;//can't be a bridge
//FrontTrainFrontPosition = Track->TrackElementAt(593,FrontTrainRearPosition).Conn[FrontTrainExitPos];
TTrackElement FrontMostElement = Track->TrackElementAt(594,FrontTrainFrontPosition);
if((FrontMostElement.TrackType == Bridge) && (FrontTrainExitPos > 1)) TrainIDOnFrontOfFrontTrain = FrontMostElement.TrainIDOnBridgeTrackPos23;
else if((FrontMostElement.TrackType == Bridge) && (FrontTrainExitPos < 2)) TrainIDOnFrontOfFrontTrain = FrontMostElement.TrainIDOnBridgeTrackPos01;
else TrainIDOnFrontOfFrontTrain = FrontMostElement.TrainIDOnElement;

if(((TrainIDOnRearOfRearTrain > -1) && (TrainIDOnRearOfRearTrain != TrainID)) ||
        ((TrainIDOnFrontOfRearTrain > -1) && (TrainIDOnFrontOfRearTrain != TrainID)) ||
        ((TrainIDOnRearOfFrontTrain > -1) && (TrainIDOnRearOfFrontTrain != TrainID)) ||
        ((TrainIDOnFrontOfFrontTrain > -1) && (TrainIDOnFrontOfFrontTrain != TrainID)))
    {
    if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != FailSplitDueToOtherTrain)
        {
        TrainController->LogActionError(11, HeadCode, "", FailSplitDueToOtherTrain, LocationName);
        TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = FailSplitDueToOtherTrain;
        }
    //don't advance ActionVectorEntryPtr as need to keep trying, other train may move off eventually
    Utilities->CallLogPop(1014);
    return;
    }

if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported == FailSplitDueToOtherTrain)
    {
    TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
    }

//reposition existing front train, need to do this first for 2 reasons - 1) will likely be in the way of the new rear train, and 2)
//the new train will likely cause a reallocation of the TrainVector, and if so the reference to the existing train will be invalidated.
//Hence deal with existing train while it references a valid entry in the vector, but retain the Old ActionVectorEntryPtr in a separate
//variable as it is needed for setting up the new train
TActionVectorEntry *OldActionVectorEntryPtr = ActionVectorEntryPtr;
UnplotTrain(1);
StartSpeed = 0;
RearStartElement = FrontTrainRearPosition;
RearStartExitPos = FrontTrainExitPos;
StoppedAtLocation = true;
PlotStartPosition(4);
PlotTrainWithNewBackgroundColour(15, clStationStopBackground, Display);//pale green
LogAction(10, HeadCode, OtherHeadCode, RearSplit, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;

Mass = Mass/2;
//TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).Mass = Mass;
MaxBrakeRate = MaxBrakeRate/2;
//TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).MaxBrakeRate = MaxBrakeRate;
PowerAtRail = PowerAtRail/2;
//TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).PowerAtRail = PowerAtRail;
//AValue = sqrt(2*PowerAtRail/Mass) i.e. AValue stays same

//create new rear train
/*
TrainController::AddTrain(int RearPosition, int FrontPosition, AnsiString HeadCode, int StartSpeed, int Mass,
        double MaxRunningSpeed, double MaxBrakeRate, double PowerAtRail, AnsiString ModeStr, TTrainDataEntry *TrainDataEntryPtr,
        int RepeatNumber, int IncrementalMinutes)
*/
 //same Mass, MaxBrakeRate & PowerAtRail as this train's halved values, and same MaxRunningSpeed as this train
if(!TrainController->AddTrain(1, RearTrainRearPosition, RearTrainFrontPosition, OtherHeadCode, 0, Mass,
    MaxRunningSpeed, MaxBrakeRate, PowerAtRail, "Timetable", OldActionVectorEntryPtr->LinkedTrainEntryPtr,
    RepeatNumber, IncrementalMinutes, IncrementalDigits, SignallerMaxSpeed, false))//false for SignallerControl
    {
    Utilities->CallLogPop(1722);//if fails either a throw will have been sent in AddTrain or start position failed prob because of
                                //another train, in which case a message will have been sent to the perf log, also might well clear later
                                //when other train moves away
    return;
    }
TrainController->TrainAdded = true;
Utilities->CallLogPop(1015);
}

//---------------------------------------------------------------------------

void TTrain::FinishJoin(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FinishJoin");
if(TrainGone)//this means that the train has already joined the other & is awaiting deletion by TrainController
             //without this the 'waiting' message can be given since the other train's ActionVectorEntryPtr has moved
             //on from jbo & TrainToJoinIsAdjacent returns false
    {
    Utilities->CallLogPop(1035);
    return;
    }
TTrain *TrainToJoin;
AnsiString JBOHeadCode = TrainController->GetRepeatHeadCode(3, ActionVectorEntryPtr->OtherHeadCode, RepeatNumber, IncrementalDigits);
if(!TrainToJoinIsAdjacent(0, TrainToJoin))
    {
    if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != WaitingForJBO)
        {
//        Display->PerformanceLog(2, TrainController->TTClockTime.FormatString("hh:nn:ss") + ": " + HeadCode + " waiting to join " + JBOHeadCode + " at " + ActionVectorEntryPtr->LocationName);
        TrainController->LogActionError(51, HeadCode, JBOHeadCode, WaitingForJBO, ActionVectorEntryPtr->LocationName);
        TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = WaitingForJBO;
        }
    Utilities->CallLogPop(1030);
    return;//keep this here in case need to add code before final return
    }
//no need to clear error report flag here, cleared in jbo function
//No need to set TimetableFinished, done in jbo function
Utilities->CallLogPop(1031);
}

//---------------------------------------------------------------------------

void TTrain::JoinedBy(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",JoinedBy");
TTrain *TrainToBeJoinedBy;
AnsiString FJOHeadCode = TrainController->GetRepeatHeadCode(4, ActionVectorEntryPtr->OtherHeadCode, RepeatNumber, IncrementalDigits);
if(!TrainToBeJoinedByIsAdjacent(0, TrainToBeJoinedBy))
    {
    if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != WaitingForFJO)
        {
//        Display->PerformanceLog(3, TrainController->TTClockTime.FormatString("hh:nn:ss") + ": " + HeadCode + " waiting to be joined by " + FJOHeadCode + " at " + ActionVectorEntryPtr->LocationName);
        TrainController->LogActionError(52, HeadCode, FJOHeadCode, WaitingForFJO, ActionVectorEntryPtr->LocationName);
        TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = WaitingForFJO;
        }
    LastActionDelayFlag = true;//need to update LastActionTime if this train first to arrive as need 30s after
                               //both adjacent before the join
    Utilities->CallLogPop(1032);
    return;
    }
//here when other train is adjacent
if(LastActionDelayFlag)
    {
    LastActionTime = TrainController->TTClockTime;//need to update this as need 30s after both adjacent before the join
    LastActionDelayFlag = false;
    Utilities->CallLogPop(1033);
    return;
    }
//here when other train is adjacent & 30 secs elapsed since both adjacent

//set new values for mass etc
if(MaxRunningSpeed > TrainToBeJoinedBy->MaxRunningSpeed) MaxRunningSpeed = TrainToBeJoinedBy->MaxRunningSpeed;
double OtherBrakeForce = TrainToBeJoinedBy->MaxBrakeRate * TrainToBeJoinedBy->Mass;
double OwnBrakeForce = MaxBrakeRate * Mass;
double CombinedBrakeRate = (OtherBrakeForce + OwnBrakeForce)/(TrainToBeJoinedBy->Mass + Mass);
Mass+= TrainToBeJoinedBy->Mass;
MaxBrakeRate = CombinedBrakeRate;
PowerAtRail+= TrainToBeJoinedBy->PowerAtRail;
AValue = sqrt(2*PowerAtRail/Mass);

TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
TrainToBeJoinedBy->TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
TrainToBeJoinedBy->TimetableFinished = true;
TrainToBeJoinedBy->TrainGone = true;//this will cause other train to be deleted
TrainToBeJoinedBy->JoinedOtherTrainFlag = true;
LogAction(11, HeadCode, FJOHeadCode, JoinedByOther, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;
Utilities->CallLogPop(1034);
}

//---------------------------------------------------------------------------

void TTrain::ChangeTrainDirection(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeTrainDirection");
TColor TempColour = BackgroundColour;
UnplotTrain(2);
RearStartElement = LeadElement;
RearStartExitPos = LeadEntryPos;
StartSpeed = 0;
StoppedAtLocation = true;
PlotStartPosition(1);
PlotTrainWithNewBackgroundColour(27, TempColour, Display);//plot same as was - should always be pale green
LogAction(12, HeadCode, "", ChangeDirection, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;
Utilities->CallLogPop(1012);
}

//---------------------------------------------------------------------------

void TTrain::NewTrainService(int Caller)//change to new train, give new service message
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NewTrainService");
AnsiString NewHeadCode = TrainController->GetRepeatHeadCode(5, ActionVectorEntryPtr->OtherHeadCode, RepeatNumber, IncrementalDigits);
LogAction(13, HeadCode, NewHeadCode, NewService, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
UnplotTrain(3);
RearStartElement = MidElement;
RearStartExitPos = MidExitPos;
StartSpeed = 0;
TrainDataEntryPtr = ActionVectorEntryPtr->LinkedTrainEntryPtr;
ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0));
HeadCode = NewHeadCode;
StoppedAtLocation = true;
PlotStartPosition(5);
PlotTrainWithNewBackgroundColour(21, clStationStopBackground, Display);//pale green
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;
TerminatedMessageSent = false;
Utilities->CallLogPop(1022);
}

//---------------------------------------------------------------------------

void TTrain::RemainHere(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + AnsiString(Caller) + ",RemainHere");
if(!TerminatedMessageSent)
    {
    Display->PerformanceLog(5, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": " + HeadCode + " terminated at " + ActionVectorEntryPtr->LocationName);
    LastActionTime = TrainController->TTClockTime;
    TerminatedMessageSent = true;
    }
TimetableFinished = true;
Utilities->CallLogPop(1023);
}

//---------------------------------------------------------------------------

void TTrain::SendMissedActionLogs(int Caller, int IncNum, TActionVectorEntry *Ptr)
/*
Enter with pointer at next expected action, and IncNum the number by which have to increase the pointer
to reach the action that is valid for the train's current position.  i.e. IncNum error messages to be sent
except where an action is a departure, starting at the current value for the pointer
If IncNum is -1, then send messages for all remaining actions, including Fer if present
If IncNum is -2, then send messages for all remaining actions, except Fer if present
*/
{
if((Ptr->Command == "Snt") && Ptr->SignallerControl) return;//if remove train that starts under signaller control no messages needed
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SendMissedActionLogs," + AnsiString(IncNum));
if(IncNum > 0)
    {
    for(int x=0;x<IncNum;x++)
        {
        if(x>0) Ptr++;
        //arrival - no need to test for termination as this section only covers missed actions up to the
        //arrival point - may terminate later but that not missed
        if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime > TDateTime(-1)))
            {
            TrainController->LogActionError(12, HeadCode, "", FailMissedArrival, Ptr->LocationName);
            }
        //arrival & departure
        if(Ptr->FormatType == TimeTimeLoc)
            {
            TrainController->LogActionError(13, HeadCode, "", FailMissedArrival, Ptr->LocationName);
            }
        //departure
        else if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime == TDateTime(-1)))
            {
            continue;//skip TimeLoc departures, message given for arrivals
            }
        //pass
        else if(Ptr->FormatType == PassTime)
            {
            TrainController->LogActionError(36, HeadCode, "", FailMissedPass, Ptr->LocationName);
            }
        //split
        else if((Ptr->Command == "fsp") || (Ptr->Command == "rsp"))
            {
            TrainController->LogActionError(14, HeadCode, "", FailMissedSplit, Ptr->LocationName);
            }
        //jbo
        else if(Ptr->Command == "jbo")
            {
            TrainController->LogActionError(15, HeadCode, "", FailMissedJBO, Ptr->LocationName);
            }
        //Errors - have reached a station stop point (before a cdt) during Train->Update() so intervening actions can't
        //be starts, finishes or cdt
        else if((Ptr->Command == "Fns") || (Ptr->Command == "Frh") || (Ptr->Command == "Fer") || (Ptr->Command == "Fjo") ||
                (Ptr->Command == "Snt") || (Ptr->Command == "Sfs") || (Ptr->Command == "Snt-sh") || (Ptr->Command == "Sns") ||
                (Ptr->Command == "Sns-sh") || (Ptr->Command == "Sns-fsh") || (Ptr->Command == "cdt") || (Ptr->Command == "Frh-sh") ||
                (Ptr->Command == "Fns-sh") || (Ptr->Command == "F-nshs") || (Ptr->FormatType == Repeat))
            {
            throw Exception("Error - illegal command in SendMissedActionLogs for IncNum = " + AnsiString(IncNum) + ", and command = " + Ptr->Command);
            }
        }
    }
else
    {
    bool IncludeFER = false;
    if (IncNum == -1) IncludeFER = true;
    while(true)//finish commands & repeats break out of loop
        {
        //Fer & excluded - send normal exit log to give minutes late or early - no, have already sent an unexpected exit message
        if(!IncludeFER && (Ptr->Command == "Fer"))
            {
            break;
            }
        //Fer & included
        else if(IncludeFER && (Ptr->Command == "Fer"))
            {
            TrainController->LogActionError(19, HeadCode, "", FailMissedExitRailway, "");
            break;
            }
        //Repeat
        else if(Ptr->FormatType == Repeat)
            {
            break;
            }
        //Fjo
        else if(Ptr->Command == "Fjo")
            {
            TrainController->LogActionError(16, HeadCode, "", FailMissedJoinOther, Ptr->LocationName);
            break;
            }
        //Frh
        else if(Ptr->Command == "Frh")
            {
            if(!TerminatedMessageSent)
                {
                TrainController->LogActionError(17, HeadCode, "", FailMissedTerminate, Ptr->LocationName);
                TerminatedMessageSent = true;
                }
            break;
            }
        //Frh-sh
        else if(Ptr->Command == "Frh-sh")
            {
            if(!TerminatedMessageSent)
                {
                TrainController->LogActionError(53, HeadCode, "", FailMissedNewService, Ptr->LocationName);
                TerminatedMessageSent = true;
                }
            break;
            }
        //Fns, F-nshs, Fns-sh
        else if((Ptr->Command == "Fns") || (Ptr->Command == "F-nshs") || (Ptr->Command == "Fns-sh"))
            {
            TrainController->LogActionError(18, HeadCode, "", FailMissedNewService, Ptr->LocationName);
            break;
            }
        //end of breakout actions
        //arrival
        if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime > TDateTime(-1)))
            {
            if(IsTrainTerminating(1))
                {
                TrainController->LogActionError(20, HeadCode, "", FailMissedTerminate, Ptr->LocationName);
                TerminatedMessageSent = true;
                }
            else
                {
                TrainController->LogActionError(21, HeadCode, "", FailMissedArrival, Ptr->LocationName);
                }
            }
        //arrival & departure
        else if(Ptr->FormatType == TimeTimeLoc)
            {
            TrainController->LogActionError(22, HeadCode, "", FailMissedArrival, Ptr->LocationName);
            }
        //departure
        else if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime == TDateTime(-1)))
            {
            Ptr++;
            continue;//skip TimeLoc departures, message given for arrivals
            }
        //pass
        else if(Ptr->FormatType == PassTime)
            {
            TrainController->LogActionError(37, HeadCode, "", FailMissedPass, Ptr->LocationName);
            }
        //split
        else if((Ptr->Command == "fsp") || (Ptr->Command == "rsp"))
            {
            TrainController->LogActionError(23, HeadCode, "", FailMissedSplit, Ptr->LocationName);
            }
        //jbo
        else if(Ptr->Command == "jbo")
            {
            TrainController->LogActionError(24, HeadCode, "", FailMissedJBO, Ptr->LocationName);
            }
        //cdt
        else if(Ptr->Command == "cdt")
            {
            TrainController->LogActionError(25, HeadCode, "", FailMissedChangeDirection, Ptr->LocationName);
            }
        //Errors
        else if((Ptr->Command == "Snt-sh") || (Ptr->Command == "Sfs") || (Ptr->Command == "Sns") ||
                (Ptr->Command == "Sns-sh") || (Ptr->Command == "Sns-fsh") || ((Ptr->Command == "Snt") && !Ptr->SignallerControl))
            {
            throw Exception("Error - illegal command in SendMissedActionLogs for IncNum = " + AnsiString(IncNum) + ", and command = " + Ptr->Command);
            }
        Ptr++;
        }
    TimetableFinished = true;
    }
Utilities->CallLogPop(1021);
}

//---------------------------------------------------------------------------

bool TTrain::TrainToJoinIsAdjacent(int Caller, TTrain* &TrainToJoin)
//ensure same repeatnumber
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrainToJoinIsAdjacent");
TTrainDataEntry *TrainToJoinTDEntry = ActionVectorEntryPtr->LinkedTrainEntryPtr;
if(TrainToJoinTDEntry->TrainOperatingDataVector.at(RepeatNumber).RunningEntry != Running)
    {
    Utilities->CallLogPop(1024);
    return false;
    }
TrainToJoin = &(TrainController->TrainVectorAtIdent(33, TrainToJoinTDEntry->TrainOperatingDataVector.at(RepeatNumber).TrainID));
if(TrainToJoin->StoppedAtLocation && (TrainToJoin->TrainMode == Timetable) && (TrainToJoin->ActionVectorEntryPtr->Command == "jbo"))
    {
    if((Track->TrackElementAt(610,LeadElement).Conn[LeadExitPos] == TrainToJoin->LeadElement) ||
            (Track->TrackElementAt(611,LeadElement).Conn[LeadExitPos] == TrainToJoin->MidElement) ||
            (Track->TrackElementAt(612,MidElement).Conn[MidEntryPos] == TrainToJoin->LeadElement) ||
            (Track->TrackElementAt(613,MidElement).Conn[MidEntryPos] == TrainToJoin->MidElement))
        {
        Utilities->CallLogPop(1025);
        return true;
        }
    }
Utilities->CallLogPop(1026);
return false;
}

//---------------------------------------------------------------------------

bool TTrain::TrainToBeJoinedByIsAdjacent(int Caller, TTrain* &TrainToBeJoinedBy)
//ensure same repeatnumber
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrainToBeJoinedByIsAdjacent");
TTrainDataEntry *TrainToBeJoinedByTDEntry = ActionVectorEntryPtr->LinkedTrainEntryPtr;
if(TrainToBeJoinedByTDEntry->TrainOperatingDataVector.at(RepeatNumber).RunningEntry != Running)
    {
    Utilities->CallLogPop(1027);
    return false;
    }
TrainToBeJoinedBy = &(TrainController->TrainVectorAtIdent(15, TrainToBeJoinedByTDEntry->TrainOperatingDataVector.at(RepeatNumber).TrainID));
if(TrainToBeJoinedBy->StoppedAtLocation && (TrainToBeJoinedBy->TrainMode == Timetable) && (TrainToBeJoinedBy->ActionVectorEntryPtr->Command == "Fjo"))
    {
    if((Track->TrackElementAt(614,LeadElement).Conn[LeadExitPos] == TrainToBeJoinedBy->LeadElement) ||
            (Track->TrackElementAt(615,LeadElement).Conn[LeadExitPos] == TrainToBeJoinedBy->MidElement) ||
            (Track->TrackElementAt(616,MidElement).Conn[MidEntryPos] == TrainToBeJoinedBy->LeadElement) ||
            (Track->TrackElementAt(617,MidElement).Conn[MidEntryPos] == TrainToBeJoinedBy->MidElement))
        {
        Utilities->CallLogPop(1028);
        return true;
        }
    }
Utilities->CallLogPop(1029);
return false;
}

//---------------------------------------------------------------------------

void TTrain::NewShuttleFromNonRepeatService(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NewShuttleFromNonRepeatService");
AnsiString NewHeadCode = ActionVectorEntryPtr->NonRepeatingShuttleLinkHeadCode;
LogAction(15, HeadCode, NewHeadCode, NewService, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
UnplotTrain(4);
RearStartElement = MidElement;
RearStartExitPos = MidExitPos;
StartSpeed = 0;
TrainDataEntryPtr = ActionVectorEntryPtr->LinkedTrainEntryPtr;
ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0));
HeadCode = NewHeadCode;
IncrementalMinutes = TrainDataEntryPtr->ActionVector.back().RearStartOrRepeatMins;
IncrementalDigits = TrainDataEntryPtr->ActionVector.back().FrontStartOrRepeatDigits;
StoppedAtLocation = true;
PlotStartPosition(6);
PlotTrainWithNewBackgroundColour(22, clStationStopBackground, Display);//pale green
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;
TerminatedMessageSent = false;
Utilities->CallLogPop(1078);
}

//---------------------------------------------------------------------------

void TTrain::RepeatShuttleOrRemainHere(int Caller)
//need to check whether all repeats finished or not
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RepeatShuttleOrRemainHere");
if(RepeatNumber >= (TrainDataEntryPtr->NumberOfTrains - 1))//finished all repeats
    {
    if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != ShuttleFinishedRemainingHere)
        {
        LogAction(26, HeadCode, "", Terminate, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
        TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = ShuttleFinishedRemainingHere;
        TerminatedMessageSent = true;
        //no need to clear message as no more actions
        }
    TimetableFinished = true;
    Utilities->CallLogPop(1080);
    return;
    }
int TempRepeatNumber = RepeatNumber + 1;
//need the next repeat value in order to obtain a correct NewHeadCode, but don't increase it
//until after LogAction or the wrong time will be used
AnsiString NewHeadCode = TrainController->GetRepeatHeadCode(6, ActionVectorEntryPtr->OtherHeadCode, TempRepeatNumber, IncrementalDigits);
LogAction(16, HeadCode, NewHeadCode, NewService, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
RepeatNumber++;
UnplotTrain(5);
RearStartElement = MidElement;
RearStartExitPos = MidExitPos;
StartSpeed = 0;
TrainDataEntryPtr = ActionVectorEntryPtr->LinkedTrainEntryPtr;
ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0));
HeadCode = NewHeadCode;
StoppedAtLocation = true;
PlotStartPosition(7);
PlotTrainWithNewBackgroundColour(23, clStationStopBackground, Display);//pale green
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;
TerminatedMessageSent = false;
Utilities->CallLogPop(1079);
}

//---------------------------------------------------------------------------

void TTrain::RepeatShuttleOrNewNonRepeatService(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RepeatShuttleOrNewNonRepeatService");
if(RepeatNumber >= (TrainDataEntryPtr->NumberOfTrains - 1))//finished all repeats
    {
    AnsiString NewHeadCode = ActionVectorEntryPtr->NonRepeatingShuttleLinkHeadCode;
    LogAction(17, HeadCode, NewHeadCode, NewService, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
    RepeatNumber = 0;
    UnplotTrain(6);
    RearStartElement = MidElement;
    RearStartExitPos = MidExitPos;
    StartSpeed = 0;
    TrainDataEntryPtr = ActionVectorEntryPtr->NonRepeatingShuttleLinkEntryPtr;
    ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0));
    HeadCode = NewHeadCode;
    StoppedAtLocation = true;
    PlotStartPosition(9);
    PlotTrainWithNewBackgroundColour(24, clStationStopBackground, Display);//pale green
    ActionVectorEntryPtr++;
    LastActionTime = TrainController->TTClockTime;
    TerminatedMessageSent = false;
    Utilities->CallLogPop(1081);
    return;
    }
int TempRepeatNumber = RepeatNumber + 1;
//need the next repeat value in order to obtain a correct NewHeadCode, but don't increase it
//until after LogAction or the wrong time will be used
AnsiString NewHeadCode = TrainController->GetRepeatHeadCode(7, ActionVectorEntryPtr->OtherHeadCode, TempRepeatNumber, IncrementalDigits);
LogAction(18, HeadCode, NewHeadCode, NewService, ActionVectorEntryPtr->LocationName, ActionVectorEntryPtr->EventTime, ActionVectorEntryPtr->Warning);
RepeatNumber++;
UnplotTrain(7);
RearStartElement = MidElement;
RearStartExitPos = MidExitPos;
StartSpeed = 0;
TrainDataEntryPtr = ActionVectorEntryPtr->LinkedTrainEntryPtr;
ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0));
HeadCode = NewHeadCode;
StoppedAtLocation = true;
PlotStartPosition(8);
PlotTrainWithNewBackgroundColour(25, clStationStopBackground, Display);//pale green
ActionVectorEntryPtr++;
LastActionTime = TrainController->TTClockTime;
TerminatedMessageSent = false;
Utilities->CallLogPop(1082);
}

//---------------------------------------------------------------------------

bool TTrain::IsTrainTerminating(int Caller)
{
//Search ActionVector from the position after the entry value for Ptr to the end, and return true if find a Finish
//entry before Fer or TimeLoc.  No point checking for TimeTimeLoc since at a stop location now so a later TimeTimeLoc
//must be preceded by a TimeLoc departure
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsTrainTerminating");
for(unsigned int x=1;x<TrainDataEntryPtr->ActionVector.size();x++)
    {
    if((ActionVectorEntryPtr + x) < TrainDataEntryPtr->ActionVector.end())
        {
        if(((ActionVectorEntryPtr + x)->Command == "Fer") || ((ActionVectorEntryPtr + x)->FormatType == TimeLoc))
            {
            Utilities->CallLogPop(1083);
            return false;
            }
        else if((ActionVectorEntryPtr + x)->SequenceType == Finish)
            {
            Utilities->CallLogPop(1084);
            return true;
            }
        }
    }
Utilities->CallLogPop(1085);
return false;
}

//---------------------------------------------------------------------------

bool TTrain::AbleToMove(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AbleToMove");
bool Able = true;
if(Crashed || Derailed || StoppedAtBuffers || StoppedAtSignal || StoppedForTrainInFront) Able = false;
if(LeadElement > -1)
    {
    int FrontPos = Track->TrackElementAt(678, LeadElement).Conn[LeadExitPos];
    int FrontEntryPos = Track->TrackElementAt(679, LeadElement).ConnLinkPos[LeadExitPos];
    if((FrontPos > -1) && (TrainMode == Signaller) && StoppedForTrainInFront)
        {
        TTrackElement TrackElement = Track->TrackElementAt(680, FrontPos);
        if((TrackElement.TrackType != Bridge) && (TrackElement.TrainIDOnElement == -1))
            {
            Able = true;
            StoppedForTrainInFront = false;
            }
        else if((TrackElement.TrackType == Bridge) && (FrontEntryPos < 2) && (TrackElement.TrainIDOnBridgeTrackPos01 == -1))
            {
            Able = true;
            StoppedForTrainInFront = false;
            }
        else if((TrackElement.TrackType == Bridge) && (FrontEntryPos > 1) && (TrackElement.TrainIDOnBridgeTrackPos23 == -1))
            {
            Able = true;
            StoppedForTrainInFront = false;
            }
        }
    else
        {
        if(Track->TrackElementAt(801, LeadElement).TrackType == Buffers) Able = false;
        //don't set StoppedAtBuffers as (presumably) StoppedAtLocation & leave it at that 
        }
    }
else
    {//leaving at a continuation so keep going
    Able = true;
    StoppedForTrainInFront = false;
    }
Utilities->CallLogPop(1454);
return Able;
}

//---------------------------------------------------------------------------

bool TTrain::AbleToMoveButForSignal(int Caller)
{
//first check if a train immediately in front (may have moved there since this train stopped so StoppedForTrainInFront
//won't be set; if there is a train then set StoppedForTrainInFront
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AbleToMoveButForSignal");
int VecPos = Track->TrackElementAt(654, LeadElement).Conn[LeadExitPos];
int NextEntryPos = Track->TrackElementAt(655, LeadElement).ConnLinkPos[LeadExitPos];
if(Track->OtherTrainOnTrack(5, VecPos, NextEntryPos, TrainID))
    {
    StoppedForTrainInFront = true;
    Utilities->CallLogPop(1455);
    return false;
    }
else
    {
    Utilities->CallLogPop(1456);
    return StoppedAtSignal && !(Crashed || Derailed || StoppedAtBuffers || StoppedForTrainInFront);
    }
}

//---------------------------------------------------------------------------

void TTrain::SignallerChangeTrainDirection(int Caller)
{
//unplots & replots train, which checks for facing signal and sets StoppedAtSignal if req'd
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SignallerChangeTrainDirection");
TColor TempColour = BackgroundColour;
UnplotTrain(8);
RearStartElement = LeadElement;
RearStartExitPos = LeadEntryPos;
StartSpeed = 0;
PlotStartPosition(2);
PlotTrainWithNewBackgroundColour(26, TempColour, Display);
Utilities->CallLogPop(1102);
}

//---------------------------------------------------------------------------

AnsiString TTrain::FloatingLabelNextString(int Caller, TActionVectorEntry *Ptr)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + "," + AnsiString(Ptr - TrainDataEntryPtr->ActionVector.begin()) + ",FloatingLabelNextString");
AnsiString RetStr = "", LocationName = "";
if((Ptr->Command != "") && (Ptr->Command[1] == 'S'))
    {
    throw Exception("Error - start entry in FloatingLabelNextString");
    }
if(Ptr->FormatType == TimeTimeLoc)
    {
    if(TrainMode == Timetable)
        {
        if(!TrainAtLocation(0, LocationName) || (LocationName != Ptr->LocationName))//not arrived yet in tt mode
            {
            RetStr = "Arrive " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(2, Ptr->ArrivalTime));
            }
        else
            {
            RetStr = "Depart " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(3, Ptr->DepartureTime));
            }
        }
    else//TrainMode == Signaller
        {
        if(!DepartureTimeSet)//not arrived yet
            {
            RetStr = "Arrive " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(45, Ptr->ArrivalTime));
            }
        else
            {
            RetStr = "Depart " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(36, Ptr->DepartureTime));
            }
        }
    }
else if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime != TDateTime(-1)))
    {
    RetStr = "Arrive " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(4, Ptr->ArrivalTime));
    }
else if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime == TDateTime(-1)))
    {
    RetStr = "Depart " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(5, Ptr->DepartureTime));
    }
else if(Ptr->FormatType == PassTime)//new
    {
    RetStr = "Pass " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(31, Ptr->EventTime));
    }
else if(Ptr->Command == "Fns")
    {
    RetStr = "Forms new service " + TrainController->GetRepeatHeadCode(8, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) +
            " at " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(6, Ptr->EventTime));
    }
else if(Ptr->Command == "F-nshs")
    {
    RetStr = "Forms new service " + Ptr->NonRepeatingShuttleLinkHeadCode +
            " at " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(32, Ptr->EventTime));
    }
else if((Ptr->Command == "Fns-sh") && (RepeatNumber < (TrainDataEntryPtr->NumberOfTrains - 1)))//not last repeat number
    {
    RetStr = "Forms new service " + TrainController->GetRepeatHeadCode(9, Ptr->OtherHeadCode, RepeatNumber+1, IncrementalDigits) +
            " at " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(7, Ptr->EventTime));
            //use RepeatNumber+1 as it's the repeat number of the NEXT shuttle service that is relevant
    }
else if((Ptr->Command == "Fns-sh") && (RepeatNumber >= (TrainDataEntryPtr->NumberOfTrains - 1)))//last repeat number
    {
    RetStr = "Forms new service " + Ptr->NonRepeatingShuttleLinkHeadCode, + " at " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(8, Ptr->EventTime));
    }
else if((Ptr->Command == "Frh-sh") && (RepeatNumber < (TrainDataEntryPtr->NumberOfTrains - 1)))//not last repeat number
    {
    RetStr = "Forms new service " + TrainController->GetRepeatHeadCode(10, Ptr->OtherHeadCode, RepeatNumber+1, IncrementalDigits) +
            " at " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(9, Ptr->EventTime));
            //use RepeatNumber+1 as it's the repeat number of the NEXT shuttle service that is relevant
    }
else if((Ptr->Command == "Frh-sh") && (RepeatNumber >= (TrainDataEntryPtr->NumberOfTrains - 1)))//last repeat number
    {
    RetStr = "None, train terminated at " + Ptr->LocationName;
    }
else if(Ptr->Command == "Frh")
    {
    RetStr = "None, train terminated at " + Ptr->LocationName;
    }
else if(Ptr->Command == "Fer")
    {
    RetStr = "Exit railway" + TrainController->GetExitLocationAndAt(1, Ptr->ExitList) + " at " + Utilities->Format96HHMM(GetTrainTime(10, Ptr->EventTime));
    }
else if(Ptr->Command == "Fjo")
    {
    RetStr = "Join " + TrainController->GetRepeatHeadCode(11, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName + " at " +
            Utilities->Format96HHMM(GetTrainTime(11, Ptr->EventTime));
    }
else if(Ptr->Command == "jbo")
    {
    RetStr = "Joined by " + TrainController->GetRepeatHeadCode(12, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName + " at " +
            Utilities->Format96HHMM(GetTrainTime(12, Ptr->EventTime));
    }
else if(Ptr->Command == "fsp")
    {
    RetStr = "Front split to " + TrainController->GetRepeatHeadCode(13, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName + " at " +
            Utilities->Format96HHMM(GetTrainTime(13, Ptr->EventTime));
    }
else if(Ptr->Command == "rsp")
    {
    RetStr = "Rear split to " + TrainController->GetRepeatHeadCode(14, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName + " at " +
            Utilities->Format96HHMM(GetTrainTime(14, Ptr->EventTime));
    }
else if(Ptr->Command == "cdt")
    {
    RetStr = "Change direction at " + Ptr->LocationName + " at " + Utilities->Format96HHMM(GetTrainTime(15, Ptr->EventTime));
    }
Utilities->CallLogPop(1124);
return RetStr;
}

//---------------------------------------------------------------------------
AnsiString TTrain::FloatingTimetableString(int Caller, TActionVectorEntry *Ptr)
//Enter with Ptr pointing to first action to be listed (i.e. next action)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + "," + AnsiString(Ptr - TrainDataEntryPtr->ActionVector.begin()) + ",FloatingTimetableString");
AnsiString RetStr = "", PartStr = "";
int Count = 0;
if((Ptr->Command != "") && (Ptr->Command[1] == 'S') && (TrainMode == Timetable))//can start in signaller control so exclude this
    {
    throw Exception("Error - start entry in FloatingTimetableString");
    }
Ptr--;//because incremented at start of loop
bool FirstPass = true;//different first TimeTimeLoc display if in signaller control
do
    {
    Ptr++;
    if((Ptr->FormatType == Repeat) || TimetableFinished) break;
    if((Ptr->FormatType == TimeTimeLoc) && FirstPass)
        {
        AnsiString TrainLoc="";
        if(TrainMode == Timetable)
            {
            if(TrainAtLocation(1, TrainLoc) && (TrainLoc == Ptr->LocationName))
                {
                PartStr = Utilities->Format96HHMM(GetTrainTime(33, Ptr->DepartureTime)) + ": Depart from " + Ptr->LocationName;
                }
            else if(Ptr->ArrivalTime == Ptr->DepartureTime)
                {
                PartStr = Utilities->Format96HHMM(GetTrainTime(34, Ptr->ArrivalTime)) + ": Arrive & depart from " + Ptr->LocationName;
                }
            else
                {
                PartStr = Utilities->Format96HHMM(GetTrainTime(16, Ptr->ArrivalTime)) + ": Arrive at " + Ptr->LocationName + '\n' +
                        Utilities->Format96HHMM(GetTrainTime(17, Ptr->DepartureTime)) + ": Depart from " + Ptr->LocationName;
                Count++; //because there are 2 entries
                }
            }
        else//TrainMode == Signaller
            {
            if(DepartureTimeSet)
                {
                PartStr = Utilities->Format96HHMM(GetTrainTime(37, Ptr->DepartureTime)) + ": Depart from " + Ptr->LocationName;
                }
            else if(Ptr->ArrivalTime == Ptr->DepartureTime)
                {
                PartStr = Utilities->Format96HHMM(GetTrainTime(38, Ptr->ArrivalTime)) + ": Arrive & depart from " + Ptr->LocationName;
                }
            else
                {
                PartStr = Utilities->Format96HHMM(GetTrainTime(39, Ptr->ArrivalTime)) + ": Arrive at " + Ptr->LocationName + '\n' +
                        Utilities->Format96HHMM(GetTrainTime(40, Ptr->DepartureTime)) + ": Depart from " + Ptr->LocationName;
                Count++; //because there are 2 entries
                }
            }
        }
    else if((Ptr->FormatType == TimeTimeLoc) && !FirstPass)
        {
        AnsiString TrainLoc="";
        if((TrainAtLocation(2, TrainLoc)) && (TrainLoc == Ptr->LocationName))
            {
            PartStr = Utilities->Format96HHMM(GetTrainTime(41, Ptr->DepartureTime)) + ": Depart from " + Ptr->LocationName;
            }
        else if(Ptr->ArrivalTime == Ptr->DepartureTime)
            {
            PartStr = Utilities->Format96HHMM(GetTrainTime(42, Ptr->ArrivalTime)) + ": Arrive & depart from " + Ptr->LocationName;
            }
        else
            {
            PartStr = Utilities->Format96HHMM(GetTrainTime(43, Ptr->ArrivalTime)) + ": Arrive at " + Ptr->LocationName + '\n' +
                    Utilities->Format96HHMM(GetTrainTime(44, Ptr->DepartureTime)) + ": Depart from " + Ptr->LocationName;
            Count++; //because there are 2 entries
            }
        }
    else if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime != TDateTime(-1)))
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(18, Ptr->ArrivalTime)) + ": Arrive at " + Ptr->LocationName;
        }
    else if((Ptr->FormatType == TimeLoc) && (Ptr->ArrivalTime == TDateTime(-1)))
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(19, Ptr->DepartureTime)) + ": Depart from " + Ptr->LocationName;
        }
    else if(Ptr->FormatType == PassTime)//new
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(30, Ptr->EventTime)) + ": Pass " + Ptr->LocationName;
        }
    else if(Ptr->Command == "Fns")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(20, Ptr->EventTime)) + ": Form new service " + TrainController->GetRepeatHeadCode(15, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) +
                " at " + Ptr->LocationName;
        }
    else if(Ptr->Command == "F-nshs")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(35, Ptr->EventTime)) + ": Form new service " + Ptr->NonRepeatingShuttleLinkHeadCode +
                " at " + Ptr->LocationName;
        }
    else if((Ptr->Command == "Fns-sh") && (RepeatNumber < (TrainDataEntryPtr->NumberOfTrains - 1)))//not the last repeat number
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(21, Ptr->EventTime)) + ": Form new service " + TrainController->GetRepeatHeadCode(16, Ptr->OtherHeadCode, RepeatNumber+1, IncrementalDigits) +
                " at " + Ptr->LocationName;//use RepeatNumber+1 because it's the repeat number of the NEXT shuttle service that is relevant
        }
    else if((Ptr->Command == "Fns-sh") && (RepeatNumber >= (TrainDataEntryPtr->NumberOfTrains - 1)))//last repeat number
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(22, Ptr->EventTime)) + ": Form new service " + Ptr->NonRepeatingShuttleLinkHeadCode, + " at " + Ptr->LocationName;
        }
    else if((Ptr->Command == "Frh-sh") && (RepeatNumber < (TrainDataEntryPtr->NumberOfTrains - 1)))//not the last repeat number
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(23, Ptr->EventTime)) + ": Form new service " + TrainController->GetRepeatHeadCode(17, Ptr->OtherHeadCode, RepeatNumber+1, IncrementalDigits) +
                " at " + Ptr->LocationName;//use RepeatNumber+1 because it's the repeat number of the NEXT shuttle service that is relevant
        }
    else if((Ptr->Command == "Frh-sh") && (RepeatNumber >= (TrainDataEntryPtr->NumberOfTrains - 1)))//last repeat number
        {
        PartStr = "Terminate at " + Ptr->LocationName;
        }
    else if(Ptr->Command == "Frh")
        {
        PartStr = "Terminate at " + Ptr->LocationName;
        }
    else if(Ptr->Command == "Fer")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(24, Ptr->EventTime)) + ": Exit railway" + TrainController->GetExitLocationAndAt(2, Ptr->ExitList);
        }
    else if(Ptr->Command == "Fjo")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(25, Ptr->EventTime)) + ": Join " + TrainController->GetRepeatHeadCode(18, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName;
        }
    else if(Ptr->Command == "jbo")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(26, Ptr->EventTime)) + ": Joined by " + TrainController->GetRepeatHeadCode(19, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName;
        }
    else if(Ptr->Command == "fsp")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(27, Ptr->EventTime)) + ": Front split to " + TrainController->GetRepeatHeadCode(20, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName;
        }
    else if(Ptr->Command == "rsp")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(28, Ptr->EventTime)) + ": Rear split to " + TrainController->GetRepeatHeadCode(21, Ptr->OtherHeadCode, RepeatNumber, IncrementalDigits) + " at " + Ptr->LocationName;
        }
    else if(Ptr->Command == "cdt")
        {
        PartStr = Utilities->Format96HHMM(GetTrainTime(29, Ptr->EventTime)) + ": Change direction at " + Ptr->LocationName;
        }
    if(RetStr != "") RetStr = RetStr + '\n' + PartStr;
    else RetStr = PartStr;
    FirstPass = false;
    Count++;
    }
while(!TimetableFinished && (Count < 33) && ((Ptr->Command == "") || ((Ptr->Command != "") && (Ptr->Command[1] != 'F'))));
//limit of 33 allows a max of 34 entries (may have gone from 32 to 34 because of a TimeTimeLoc), which with track and train status gives
//a max of 48 lines, at 13 pixels each, = 624 pixels & screen height has 641 so will fit comfortably.  Also 34 timetable entries is as far
//forward as anyone should wish to see without looking at the full timetable
if(TimetableFinished)
    {
    if(TrainMode == Timetable) RetStr = "Timetable finished";
    else  RetStr = "No timetable";
    }
Utilities->CallLogPop(1125);
return RetStr;
}

//---------------------------------------------------------------------------

void TTrain::SaveOneSessionTrain(int Caller, std::ofstream &OutFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveOneSessionTrain");
Utilities->SaveFileString(OutFile, HeadCode);
Utilities->SaveFileInt(OutFile, RearStartElement);
Utilities->SaveFileInt(OutFile, RearStartExitPos);
Utilities->SaveFileInt(OutFile, StartSpeed);
Utilities->SaveFileInt(OutFile, SignallerMaxSpeed);
Utilities->SaveFileBool(OutFile, HoldAtLocationInTTMode);
Utilities->SaveFileInt(OutFile, RepeatNumber);
Utilities->SaveFileInt(OutFile, IncrementalMinutes);
Utilities->SaveFileInt(OutFile, IncrementalDigits);
Utilities->SaveFileInt(OutFile, Mass);
Utilities->SaveFileInt(OutFile, FrontElementSpeedLimit);
Utilities->SaveFileInt(OutFile, FrontElementLength);
Utilities->SaveFileDouble(OutFile, EntrySpeed);
Utilities->SaveFileDouble(OutFile, ExitSpeedHalf);
Utilities->SaveFileDouble(OutFile, ExitSpeedFull);
Utilities->SaveFileDouble(OutFile, TimetableMaxRunningSpeed);
Utilities->SaveFileDouble(OutFile, MaxRunningSpeed);
Utilities->SaveFileDouble(OutFile, MaxExitSpeed);
Utilities->SaveFileDouble(OutFile, MaxBrakeRate);
Utilities->SaveFileDouble(OutFile, BrakeRate);
Utilities->SaveFileDouble(OutFile, PowerAtRail);
Utilities->SaveFileBool(OutFile, FirstHalfMove);
Utilities->SaveFileBool(OutFile, OneLengthAccelDecel);
Utilities->SaveFileDouble(OutFile, double(EntryTime));
Utilities->SaveFileDouble(OutFile, double(ExitTimeHalf));
Utilities->SaveFileDouble(OutFile, double(ExitTimeFull));
Utilities->SaveFileDouble(OutFile, double(ReleaseTime));
Utilities->SaveFileDouble(OutFile, double(TRSTime));
Utilities->SaveFileDouble(OutFile, double(LastActionTime));
Utilities->SaveFileBool(OutFile, CallingOnFlag);
Utilities->SaveFileBool(OutFile, BeingCalledOn);
Utilities->SaveFileBool(OutFile, DepartureTimeSet);
Utilities->SaveFileInt(OutFile, (short)TrainMode);
Utilities->SaveFileBool(OutFile, TimetableFinished);
Utilities->SaveFileBool(OutFile, LastActionDelayFlag);
Utilities->SaveFileBool(OutFile, SignallerRemoved);
Utilities->SaveFileBool(OutFile, TerminatedMessageSent);
Utilities->SaveFileBool(OutFile, Derailed);
Utilities->SaveFileBool(OutFile, DerailPending);
Utilities->SaveFileBool(OutFile, Crashed);
Utilities->SaveFileBool(OutFile, StoppedAtBuffers);
Utilities->SaveFileBool(OutFile, StoppedAtSignal);
Utilities->SaveFileBool(OutFile, StoppedAtLocation);
Utilities->SaveFileBool(OutFile, SignallerStopped);
Utilities->SaveFileBool(OutFile, StoppedAfterSPAD);
Utilities->SaveFileBool(OutFile, StoppedForTrainInFront);
Utilities->SaveFileBool(OutFile, NotInService);
Utilities->SaveFileBool(OutFile, Plotted);
Utilities->SaveFileBool(OutFile, TrainGone);
Utilities->SaveFileBool(OutFile, SPADFlag);
Utilities->SaveFileBool(OutFile, TimeTimeLocArrived);
Utilities->SaveFileInt(OutFile, HOffset[0]);
Utilities->SaveFileInt(OutFile, HOffset[1]);
Utilities->SaveFileInt(OutFile, HOffset[2]);
Utilities->SaveFileInt(OutFile, HOffset[3]);
Utilities->SaveFileInt(OutFile, VOffset[0]);
Utilities->SaveFileInt(OutFile, VOffset[1]);
Utilities->SaveFileInt(OutFile, VOffset[2]);
Utilities->SaveFileInt(OutFile, VOffset[3]);
Utilities->SaveFileInt(OutFile, PlotElement[0]);
Utilities->SaveFileInt(OutFile, PlotElement[1]);
Utilities->SaveFileInt(OutFile, PlotElement[2]);
Utilities->SaveFileInt(OutFile, PlotElement[3]);
Utilities->SaveFileInt(OutFile, PlotEntryPos[0]);
Utilities->SaveFileInt(OutFile, PlotEntryPos[1]);
Utilities->SaveFileInt(OutFile, PlotEntryPos[2]);
Utilities->SaveFileInt(OutFile, PlotEntryPos[3]);
Utilities->SaveFileInt(OutFile, TrainCrashedInto);
Utilities->SaveFileInt(OutFile, (short)Straddle);
Utilities->SaveFileInt(OutFile, NextTrainID);
Utilities->SaveFileInt(OutFile, TrainID);
Utilities->SaveFileInt(OutFile, LeadElement);
Utilities->SaveFileInt(OutFile, LeadEntryPos);
Utilities->SaveFileInt(OutFile, LeadExitPos);
Utilities->SaveFileInt(OutFile, MidElement);
Utilities->SaveFileInt(OutFile, MidEntryPos);
Utilities->SaveFileInt(OutFile, MidExitPos);
Utilities->SaveFileInt(OutFile, LagElement);
Utilities->SaveFileInt(OutFile, LagEntryPos);
Utilities->SaveFileInt(OutFile, LagExitPos);
int ColourNumber;
if(BackgroundColour == clBufferAttentionNeeded) ColourNumber = 0;
else if(BackgroundColour == clBufferStopBackground) ColourNumber = 1;
else if(BackgroundColour == clCallOnBackground) ColourNumber = 2;
else if(BackgroundColour == clCrashedBackground) ColourNumber = 3;
else if(BackgroundColour == clDerailedBackground) ColourNumber = 4;
else if(BackgroundColour == clFrontCodeSignaller) ColourNumber = 5;
else if(BackgroundColour == clFrontCodeTimetable) ColourNumber = 6;
else if(BackgroundColour == clNormalBackground) ColourNumber = 7;
else if(BackgroundColour == clSignallerStopped) ColourNumber = 8;
else if(BackgroundColour == clSignalStopBackground) ColourNumber = 9;
else if(BackgroundColour == clSPADBackground) ColourNumber = 10;
else if(BackgroundColour == clStationStopBackground) ColourNumber = 11;
else if(BackgroundColour == clStoppedTrainInFront) ColourNumber = 12;
else if(BackgroundColour == clTRSBackground) ColourNumber = 13;
Utilities->SaveFileInt(OutFile, ColourNumber);

//additional data
bool ForwardHeadCode;
if(HeadCodePosition[3] == HeadCodeGrPtr[3])
    {
    ForwardHeadCode = true;
    }
//can't use 'if(HeadCodePosition[0] == HeadCodeGrPtr[0])' as HeadCodePosition[0] is set to FrontCodePtr
else
    {
    ForwardHeadCode = false;
    }
Utilities->SaveFileBool(OutFile, ForwardHeadCode);

int TrainDataEntryValue = TrainDataEntryPtr - &(TrainController->TrainDataVector.at(0));
Utilities->SaveFileInt(OutFile, TrainDataEntryValue);
int ActionVectorEntryValue = ActionVectorEntryPtr - &(TrainDataEntryPtr->ActionVector.at(0));
Utilities->SaveFileInt(OutFile, ActionVectorEntryValue);
if(RestoreTimetableLocation == "") Utilities->SaveFileString(OutFile, "******");//marker
else
    {
    AnsiString CombinedString = RestoreTimetableLocation + "******";
    Utilities->SaveFileString(OutFile, CombinedString);//RestoreTimetableLocation + marker
    }
//Note: including this with the marker is to correct an oversight - RestoreTimetableLocation should have been saved earlier
Utilities->CallLogPop(1457);
}

//---------------------------------------------------------------------------

void TTrain::LoadOneSessionTrain(int Caller, std::ifstream &InFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadOneSessionTrain");
HeadCode = Utilities->LoadFileString(InFile);
RearStartElement = Utilities->LoadFileInt(InFile);
RearStartExitPos = Utilities->LoadFileInt(InFile);
StartSpeed = Utilities->LoadFileInt(InFile);
SignallerMaxSpeed = Utilities->LoadFileInt(InFile);
HoldAtLocationInTTMode = Utilities->LoadFileBool(InFile);
RepeatNumber = Utilities->LoadFileInt(InFile);
IncrementalMinutes = Utilities->LoadFileInt(InFile);
IncrementalDigits = Utilities->LoadFileInt(InFile);
Mass = Utilities->LoadFileInt(InFile);
FrontElementSpeedLimit = Utilities->LoadFileInt(InFile);
FrontElementLength = Utilities->LoadFileInt(InFile);
EntrySpeed = Utilities->LoadFileDouble(InFile);
ExitSpeedHalf = Utilities->LoadFileDouble(InFile);
ExitSpeedFull = Utilities->LoadFileDouble(InFile);
TimetableMaxRunningSpeed = Utilities->LoadFileDouble(InFile);
MaxRunningSpeed = Utilities->LoadFileDouble(InFile);
MaxExitSpeed = Utilities->LoadFileDouble(InFile);
MaxBrakeRate = Utilities->LoadFileDouble(InFile);
BrakeRate = Utilities->LoadFileDouble(InFile);
PowerAtRail = Utilities->LoadFileDouble(InFile);
FirstHalfMove = Utilities->LoadFileBool(InFile);
OneLengthAccelDecel = Utilities->LoadFileBool(InFile);
EntryTime = TDateTime(Utilities->LoadFileDouble(InFile));
ExitTimeHalf = TDateTime(Utilities->LoadFileDouble(InFile));
ExitTimeFull = TDateTime(Utilities->LoadFileDouble(InFile));
ReleaseTime = TDateTime(Utilities->LoadFileDouble(InFile));
TRSTime = TDateTime(Utilities->LoadFileDouble(InFile));
LastActionTime = TDateTime(Utilities->LoadFileDouble(InFile));
CallingOnFlag = Utilities->LoadFileBool(InFile);
BeingCalledOn = Utilities->LoadFileBool(InFile);
DepartureTimeSet = Utilities->LoadFileBool(InFile);
TrainMode  = TTrainMode(Utilities->LoadFileInt(InFile));
TimetableFinished = Utilities->LoadFileBool(InFile);
LastActionDelayFlag = Utilities->LoadFileBool(InFile);
SignallerRemoved = Utilities->LoadFileBool(InFile);
TerminatedMessageSent = Utilities->LoadFileBool(InFile);
Derailed = Utilities->LoadFileBool(InFile);
DerailPending = Utilities->LoadFileBool(InFile);
Crashed = Utilities->LoadFileBool(InFile);
StoppedAtBuffers = Utilities->LoadFileBool(InFile);
StoppedAtSignal = Utilities->LoadFileBool(InFile);
StoppedAtLocation = Utilities->LoadFileBool(InFile);
SignallerStopped = Utilities->LoadFileBool(InFile);
StoppedAfterSPAD = Utilities->LoadFileBool(InFile);
StoppedForTrainInFront = Utilities->LoadFileBool(InFile);
NotInService = Utilities->LoadFileBool(InFile);
Plotted = Utilities->LoadFileBool(InFile);
TrainGone = Utilities->LoadFileBool(InFile);
SPADFlag = Utilities->LoadFileBool(InFile);
TimeTimeLocArrived = Utilities->LoadFileBool(InFile);
HOffset[0] = Utilities->LoadFileInt(InFile);
HOffset[1] = Utilities->LoadFileInt(InFile);
HOffset[2] = Utilities->LoadFileInt(InFile);
HOffset[3] = Utilities->LoadFileInt(InFile);
VOffset[0] = Utilities->LoadFileInt(InFile);
VOffset[1] = Utilities->LoadFileInt(InFile);
VOffset[2] = Utilities->LoadFileInt(InFile);
VOffset[3] = Utilities->LoadFileInt(InFile);
PlotElement[0] = Utilities->LoadFileInt(InFile);
PlotElement[1] = Utilities->LoadFileInt(InFile);
PlotElement[2] = Utilities->LoadFileInt(InFile);
PlotElement[3] = Utilities->LoadFileInt(InFile);
PlotEntryPos[0] = Utilities->LoadFileInt(InFile);
PlotEntryPos[1] = Utilities->LoadFileInt(InFile);
PlotEntryPos[2] = Utilities->LoadFileInt(InFile);
PlotEntryPos[3] = Utilities->LoadFileInt(InFile);
TrainCrashedInto = Utilities->LoadFileInt(InFile);
Straddle = (TStraddle)(Utilities->LoadFileInt(InFile));
NextTrainID = Utilities->LoadFileInt(InFile);//will be same for all but best to save all anyway
TrainID = Utilities->LoadFileInt(InFile);
LeadElement = Utilities->LoadFileInt(InFile);
LeadEntryPos = Utilities->LoadFileInt(InFile);
LeadExitPos = Utilities->LoadFileInt(InFile);
MidElement = Utilities->LoadFileInt(InFile);
MidEntryPos = Utilities->LoadFileInt(InFile);
MidExitPos = Utilities->LoadFileInt(InFile);
LagElement = Utilities->LoadFileInt(InFile);
LagEntryPos = Utilities->LoadFileInt(InFile);
LagExitPos = Utilities->LoadFileInt(InFile);
int ColourNumber  = TColor(Utilities->LoadFileInt(InFile));
if(ColourNumber == 0) BackgroundColour = clBufferAttentionNeeded;
else if(ColourNumber == 1) BackgroundColour = clBufferStopBackground;
else if(ColourNumber == 2) BackgroundColour = clCallOnBackground;
else if(ColourNumber == 3) BackgroundColour = clCrashedBackground;
else if(ColourNumber == 4) BackgroundColour = clDerailedBackground;
else if(ColourNumber == 5) BackgroundColour = clFrontCodeSignaller;
else if(ColourNumber == 6) BackgroundColour = clFrontCodeTimetable;
else if(ColourNumber == 7) BackgroundColour = clNormalBackground;
else if(ColourNumber == 8) BackgroundColour = clSignallerStopped;
else if(ColourNumber == 9) BackgroundColour = clSignalStopBackground;
else if(ColourNumber == 10) BackgroundColour = clSPADBackground;
else if(ColourNumber == 11) BackgroundColour = clStationStopBackground;
else if(ColourNumber == 12) BackgroundColour = clStoppedTrainInFront;
else if(ColourNumber == 13) BackgroundColour = clTRSBackground;

//additional data
SetHeadCodeGraphics(1, HeadCode);//sets the BackgroundColour to the loaded value
bool ForwardHeadCode = Utilities->LoadFileBool(InFile);
if(ForwardHeadCode)
    {
    for(int x=0;x<4;x++)
        {
        HeadCodePosition[x] = HeadCodeGrPtr[x];
        }
    }
else
    {
    for(int x=0;x<4;x++)
        {
        HeadCodePosition[x] = HeadCodeGrPtr[3 - x];
        }
    }

/* Don't need the below because trains plotted in Clearand... after the session loaded
//need to set up the background colour for the graphics, by changing the 'official' background colour to a different colour
//than is to be plotted, so that the plot function PlotTrainWithNewBackgroundColour is positively called, this avoids the front
//headcode character being plotted as a block of one colour and also plots the train
TColor TempBackgroundColour = BackgroundColour;
if(BackgroundColour == clNormalBackground) BackgroundColour = clStationStopBackground;
else BackgroundColour = clNormalBackground;
PlotTrainWithNewBackgroundColour(30, TempBackgroundColour, Display);
*/

//if crashed & in timetable mode then change FrontCodePtr to black, if in signaller mode then change to blue whether crashed or not
if(TrainMode == Timetable)
    {
    if(Crashed) RailGraphics->ChangeForegroundColour(2, HeadCodePosition[0], FrontCodePtr, clB0G0R0, BackgroundColour);
    else RailGraphics->ChangeForegroundColour(3, HeadCodePosition[0], FrontCodePtr, clFrontCodeTimetable, BackgroundColour);
    }
else RailGraphics->ChangeForegroundColour(4, HeadCodePosition[0], FrontCodePtr, clFrontCodeSignaller, BackgroundColour);
HeadCodePosition[0] = FrontCodePtr;
//pick up background bitmaps, none if MidLag as no train plotted - entering at continuation
if(Straddle == LeadMid)
    {
    if(LeadElement > -1) PickUpBackgroundBitmap(6, HOffset[0], VOffset[0], LeadElement, LeadEntryPos, BackgroundPtr[0]);
    if(LeadElement > -1) PickUpBackgroundBitmap(7, HOffset[1], VOffset[1], LeadElement, LeadEntryPos, BackgroundPtr[1]);
    if(MidElement > -1) PickUpBackgroundBitmap(8, HOffset[2], VOffset[2], MidElement, MidEntryPos, BackgroundPtr[2]);
    if(MidElement > -1) PickUpBackgroundBitmap(9, HOffset[3], VOffset[3], MidElement, MidEntryPos, BackgroundPtr[3]);
    }
else if(Straddle == LeadMidLag)
    {
    if(LeadElement > -1) PickUpBackgroundBitmap(10, HOffset[0], VOffset[0], LeadElement, LeadEntryPos, BackgroundPtr[0]);
    if(MidElement > -1) PickUpBackgroundBitmap(11, HOffset[1], VOffset[1], MidElement, MidEntryPos, BackgroundPtr[1]);
    if(MidElement > -1) PickUpBackgroundBitmap(12, HOffset[2], VOffset[2], MidElement, MidEntryPos, BackgroundPtr[2]);
    if(LagElement > -1) PickUpBackgroundBitmap(13, HOffset[3], VOffset[3], LagElement, LagEntryPos, BackgroundPtr[3]);
    }

int TrainDataEntryValue = Utilities->LoadFileInt(InFile);
TrainDataEntryPtr = &(TrainController->TrainDataVector.at(0)) + TrainDataEntryValue;
int ActionVectorEntryValue = Utilities->LoadFileInt(InFile);
ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0)) + ActionVectorEntryValue;

//need to set the TrainID if arriving at a continuation but hasn't been plotted yet
if(LeadElement > -1)//need to include this in case train exiting & no lead element
    {
    if(Track->TrackElementAt(667,LeadElement).TrackType == Continuation)
        {
        Track->TrackElementAt(668,LeadElement).TrainIDOnElement = TrainID;//no need to stop gap flashing if a continuation
        }
    }
AValue = sqrt(2*PowerAtRail/Mass);
AnsiString LocationAndMarker = Utilities->LoadFileString(InFile);//possible RestoreTimetableLocation + "******"
//Note: including this with the marker is to correct an oversight - RestoreTimetableLocation should have been saved earlier
//added at beta v0.2e
if((LocationAndMarker[1] != '*') && (LocationAndMarker.Length() > 6)) //name not allowed to include the '*' character
    {
    AnsiString Location = LocationAndMarker.SubString(1, LocationAndMarker.Length() - 6);
    bool GiveMessagesFalse = false;
    bool CheckLocationsExistInRailwayTrue = true;
    if(TrainController->CheckLocationValidity(3, Location, GiveMessagesFalse, CheckLocationsExistInRailwayTrue))
        {//otherwise take no action
        RestoreTimetableLocation = Location;
        }
    }
Utilities->CallLogPop(1458);
}

//---------------------------------------------------------------------------

bool TTrain::CheckOneSessionTrain(std::ifstream &InFile)
{
if(!Utilities->CheckFileStringZeroDelimiter(InFile)) return false;// HeadCode
if(!Utilities->CheckFileInt(InFile, 0, 1000000)) return false;// RearStartElement
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// RearStartExitPos
if(!Utilities->CheckFileInt(InFile, 0, MaximumSpeedLimit)) return false;// StartSpeed
if(!Utilities->CheckFileInt(InFile, 0, MaximumSpeedLimit)) return false;// SignallerMaxSpeed
if(!Utilities->CheckFileBool(InFile)) return false;// HoldAtLocationInTTMode
if(!Utilities->CheckFileInt(InFile, 0, 5760)) return false;// RepeatNumber (max 96 x 60 at 1 min intervals)
if(!Utilities->CheckFileInt(InFile, 0, 5760)) return false;// IncrementalMinutes (max 96 x 60)
if(!Utilities->CheckFileInt(InFile, 0, 1000000)) return false;// IncrementalDigits
if(!Utilities->CheckFileInt(InFile, 0, 10000000)) return false;// Mass
if(!Utilities->CheckFileInt(InFile, 0, MaximumSpeedLimit)) return false;// FrontElementSpeedLimit
if(!Utilities->CheckFileInt(InFile, 0, 10000000)) return false;// FrontElementLength
if(!Utilities->CheckFileDouble(InFile)) return false;// EntrySpeed
if(!Utilities->CheckFileDouble(InFile)) return false;// ExitSpeedHalf
if(!Utilities->CheckFileDouble(InFile)) return false;// ExitSpeedFull
if(!Utilities->CheckFileDouble(InFile)) return false;// TimetableMaxRunningSpeed
if(!Utilities->CheckFileDouble(InFile)) return false;// MaxRunningSpeed
if(!Utilities->CheckFileDouble(InFile)) return false;// MaxExitSpeed
if(!Utilities->CheckFileDouble(InFile)) return false;// MaxBrakeRate
if(!Utilities->CheckFileDouble(InFile)) return false;// BrakeRate
if(!Utilities->CheckFileDouble(InFile)) return false;// PowerAtRail
if(!Utilities->CheckFileBool(InFile)) return false;// FirstHalfMove
if(!Utilities->CheckFileBool(InFile)) return false;// OneLengthAccelDecel
if(!Utilities->CheckFileDouble(InFile)) return false;// double(EntryTime)
if(!Utilities->CheckFileDouble(InFile)) return false;// double(ExitTimeHalf)
if(!Utilities->CheckFileDouble(InFile)) return false;// double(ExitTimeFull)
if(!Utilities->CheckFileDouble(InFile)) return false;// double(ReleaseTime)
if(!Utilities->CheckFileDouble(InFile)) return false;// double(TRSTime)
if(!Utilities->CheckFileDouble(InFile)) return false;// double(LastActionTime)
if(!Utilities->CheckFileBool(InFile)) return false;// CallingOnFlag
if(!Utilities->CheckFileBool(InFile)) return false;// BeingCalledOn
if(!Utilities->CheckFileBool(InFile)) return false;// DepartureTimeSet
if(!Utilities->CheckFileInt(InFile, 0, 2)) return false;// (short)TrainMode
if(!Utilities->CheckFileBool(InFile)) return false;// TimetableFinished
if(!Utilities->CheckFileBool(InFile)) return false;// LastActionDelayFlag
if(!Utilities->CheckFileBool(InFile)) return false;// SignallerRemoved
if(!Utilities->CheckFileBool(InFile)) return false;// TerminatedMessageSent
if(!Utilities->CheckFileBool(InFile)) return false;// Derailed
if(!Utilities->CheckFileBool(InFile)) return false;// DerailPending
if(!Utilities->CheckFileBool(InFile)) return false;// Crashed
if(!Utilities->CheckFileBool(InFile)) return false;// StoppedAtBuffers
if(!Utilities->CheckFileBool(InFile)) return false;// StoppedAtSignal
if(!Utilities->CheckFileBool(InFile)) return false;// StoppedAtLocation
if(!Utilities->CheckFileBool(InFile)) return false;// SignallerStopped
if(!Utilities->CheckFileBool(InFile)) return false;// StoppedAfterSPAD
if(!Utilities->CheckFileBool(InFile)) return false;// StoppedForTrainInFront
if(!Utilities->CheckFileBool(InFile)) return false;// NotInService
if(!Utilities->CheckFileBool(InFile)) return false;// Plotted
if(!Utilities->CheckFileBool(InFile)) return false;// TrainGone
if(!Utilities->CheckFileBool(InFile)) return false;// SPADFlag
if(!Utilities->CheckFileBool(InFile)) return false;// TimeTimeLocArrived
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// HOffset[0]
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// HOffset[1]
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// HOffset[2]
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// HOffset[3]
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// VOffset[0]
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// VOffset[1]
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// VOffset[2]
if(!Utilities->CheckFileInt(InFile, 0, 15)) return false;// VOffset[3]
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// PlotElement[0]
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// PlotElement[1]
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// PlotElement[2]
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// PlotElement[3]
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// PlotEntryPos[0]
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// PlotEntryPos[1]
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// PlotEntryPos[2]
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// PlotEntryPos[3]
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// TrainCrashedInto
if(!Utilities->CheckFileInt(InFile, 0, 2)) return false;// (short)Straddle
if(!Utilities->CheckFileInt(InFile, 0, 1000000)) return false;// NextTrainID
if(!Utilities->CheckFileInt(InFile, 0, 1000000)) return false;// TrainID
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// LeadElement
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// LeadEntryPos
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// LeadExitPos
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// MidElement
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// MidEntryPos
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// MidExitPos
if(!Utilities->CheckFileInt(InFile, -1, 1000000)) return false;// LagElement
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// LagEntryPos
if(!Utilities->CheckFileInt(InFile, 0, 3)) return false;// LagExitPos
if(!Utilities->CheckFileInt(InFile, 0, 13)) return false;// Background colour number
if(!Utilities->CheckFileBool(InFile)) return false;// ForwardHeadCode
if(!Utilities->CheckFileInt(InFile, 0, 10000)) return false;//TrainDataEntryValue
if(!Utilities->CheckFileInt(InFile, 0, 10000)) return false;//ActionVectorEntryValue
if(!Utilities->CheckFileStringZeroDelimiter(InFile)) return false;//End of train marker + possible RestoreTimetableLocation
return true;
}

//---------------------------------------------------------------------------

void TTrain::PlotTrainInZoomOutMode(int Caller, bool Flash)
{
//order below reflects significance so earlier shows first, as may have more than one flag set
//only plot flashing trains when Flash is true

/*
/clCrashedBackground     (TColor)0x0000FF red
/clDerailedBackground    (TColor)0x0000FF red
/clSPADBackground        (TColor)0x00FFFF yellow
/clCallOnBackground      (TColor)0xFF33FF light magenta
/clSignalStopBackground  (TColor)0x00FF66 green
/clBufferAttentionNeeded (TColor)0xFFFF00 cyan
/clStationStopBackground (TColor)0xCCFFCC pale green
/clTRSBackground         (TColor)0xFFCCFF light pink
/clBufferStopBackground  (TColor)0xFFFFCC pale cyan
/clStoppedTrainInFront   (TColor)0xFF9999 light blue
/clSignallerStopped      (TColor)0x99CCFF caramel
/clNormalBackground      (TColor)0xCCCCCC grey
*/

Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotTrainInZoomOutMode");
bool HideFlashingTrain = false;
Graphics::TBitmap *SmallTrainBitmap;
//NB ensure retain same order as zoomed in order so colours correspond
HideFlashingTrain = true;
if(BackgroundColour == clCrashedBackground)
    {
    TrainController->CrashWarning = true;
    SmallTrainBitmap = RailGraphics->smRed;
    }
else if(BackgroundColour == clDerailedBackground)
    {
    TrainController->DerailWarning = true;
    SmallTrainBitmap = RailGraphics->smRed;
    }
else if(BackgroundColour == clSPADBackground)
    {
    TrainController->SPADWarning = true;
    SmallTrainBitmap = RailGraphics->smYellow;
    }
else if(BackgroundColour == clCallOnBackground)
    {
    TrainController->CallOnWarning = true;
    SmallTrainBitmap = RailGraphics->smMagenta;
    }
else if(BackgroundColour == clSignalStopBackground)
    {
    TrainController->SignalStopWarning = true;
    SmallTrainBitmap = RailGraphics->smBrightGreen;
    }
else if(BackgroundColour == clBufferAttentionNeeded)
    {
    TrainController->BufferAttentionWarning = true;
    SmallTrainBitmap = RailGraphics->smCyan;
    }
else if((BackgroundColour == clStationStopBackground) || (BackgroundColour == clTRSBackground))
    {
    SmallTrainBitmap = RailGraphics->smPaleGreen;
    HideFlashingTrain = false;
    }
else if(BackgroundColour == clBufferStopBackground)
    {
    SmallTrainBitmap = RailGraphics->smCyan;
    HideFlashingTrain = false;
    }
else if(BackgroundColour == clStoppedTrainInFront)
    {
    SmallTrainBitmap = RailGraphics->smLightBlue;
    HideFlashingTrain = false;
    }
else if(BackgroundColour == clSignallerStopped)
    {
    SmallTrainBitmap = RailGraphics->smCaramel;
    HideFlashingTrain = false;
    }
else
    {
    SmallTrainBitmap = RailGraphics->smBlack;//moving
    HideFlashingTrain = false;
    }

//now plot the new train
//just plot lead & mid, unless lead == -1 in which case plot mid & lag
if((LeadElement > -1) && (!HideFlashingTrain || Flash))
    {
    Display->PlotSmallOutput(4, Track->TrackElementAt(441, LeadElement).HLoc * 4,
            Track->TrackElementAt(442, LeadElement).VLoc * 4, SmallTrainBitmap);
    }
if((MidElement > -1) && (!HideFlashingTrain || Flash))
    {
    Display->PlotSmallOutput(5, Track->TrackElementAt(443, MidElement).HLoc * 4,
            Track->TrackElementAt(444, MidElement).VLoc * 4, SmallTrainBitmap);
    }
if((LeadElement == -1) && (LagElement > -1) && (!HideFlashingTrain || Flash))
    {
    Display->PlotSmallOutput(6, Track->TrackElementAt(445, LagElement).HLoc * 4,
            Track->TrackElementAt(446, LagElement).VLoc * 4, SmallTrainBitmap);
    }
OldZoomOutElement[0] = LeadElement;
OldZoomOutElement[1] = MidElement;
OldZoomOutElement[2] = LagElement;
Utilities->CallLogPop(1459);
}

//---------------------------------------------------------------------------

void TTrain::UnplotTrainInZoomOutMode(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",UnplotTrainInZoomOutMode," + AnsiString(TrainID));
if(!Display->ZoomOutFlag)
    {
    Utilities->CallLogPop(1304);
    return;
    }
for(int y=0;y<3;y++)
    {
    if(OldZoomOutElement[y] > -1)
        {
        bool FoundFlag=false;
        TTrackElement ATElement = Track->TrackElementAt(717, OldZoomOutElement[y]);
        TTrackElement IATElement1, IATElement2;//default elements to begin with
        Display->PlotSmallOutput(7, ATElement.HLoc * 4, ATElement.VLoc * 4, RailGraphics->smSolidBgnd);//plot the blank
        TTrack::TIMPair IMPair = Track->GetVectorPositionsFromInactiveTrackMap(14, ATElement.HLoc, ATElement.VLoc, FoundFlag);
//Note, have to plot inactives before track because track has to overwrite NamedLocationElements
        if(FoundFlag)
            {
            IATElement1 = Track->InactiveTrackElementAt(87, IMPair.first);
            Display->PlotSmallOutput(8, IATElement1.HLoc * 4, IATElement1.VLoc * 4, IATElement1.SmallGraphicPtr);
            if(IMPair.first != IMPair.second)
                {
                IATElement2 = Track->InactiveTrackElementAt(88, IMPair.second);
                Display->PlotSmallOutput(9, IATElement2.HLoc * 4, IATElement2.VLoc * 4, IATElement2.SmallGraphicPtr);
                }
            }
        Display->PlotSmallOutput(10, ATElement.HLoc * 4, ATElement.VLoc * 4, ATElement.SmallGraphicPtr);
        }
    }
Utilities->CallLogPop(1305);
}

//---------------------------------------------------------------------------

bool TTrain::TrainAtLocation(int Caller, AnsiString &LocationName)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrainAtLocation");
LocationName = "";
if(!StoppedAtLocation)
    {
    Utilities->CallLogPop(1398);
    return false;
    }
if(LeadElement > -1)
    {
    LocationName = Track->TrackElementAt(681, LeadElement).ActiveTrackElementName;
    }
if((LocationName == "") && (MidElement > -1))
    {
    LocationName = Track->TrackElementAt(682, MidElement).ActiveTrackElementName;
    }
if((LocationName == "") && (LagElement > -1))
    {
    LocationName = Track->TrackElementAt(683, LagElement).ActiveTrackElementName;
    }
if(LocationName == "")
    {
    throw Exception("Error - Location name not set in TrainAtLocation");
    }
Utilities->CallLogPop(1399);
return true;
}

//---------------------------------------------------------------------------

void TTrain::PlotTrain(int Caller, TDisplay *Disp)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotTrain");
for(int x=0;x<4;x++)
    {
    PlotTrainGraphic(7, x, Disp);
    }
Utilities->CallLogPop(647);
}

//---------------------------------------------------------------------------

void TTrain::WriteTrainToImage(int Caller, Graphics::TBitmap *Bitmap)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WriteTrainToImage");
for(int x=0;x<4;x++)
    {
    if(PlotElement[x] > -1)
        {
        Bitmap->Canvas->Draw(((Track->TrackElementAt(744, PlotElement[x]).HLoc - Track->GetHLocMin()) * 16 + HOffset[x]),
            ((Track->TrackElementAt(745, PlotElement[x]).VLoc - Track->GetVLocMin()) * 16 + VOffset[x]), HeadCodePosition[x]);
        }
    }
Utilities->CallLogPop(1708);
}

//---------------------------------------------------------------------------
//TTrainController
//---------------------------------------------------------------------------

TTrainController::TTrainController()
    {
    OnTimeArrivals = 0;
    LateArrivals = 0;
    EarlyArrivals = 0;
    OnTimePasses = 0;
    LatePasses = 0;
    EarlyPasses = 0;
    OnTimeDeps = 0;
    LateDeps = 0;
    MissedStops = 0;
    OtherMissedEvents = 0;
    UnexpectedExits = 0;
    IncorrectExits = 0;
    SPADEvents = 0;
    SPADRisks = 0;
    CrashedTrains = 0;
    Derailments = 0;
    TotLateArrMins = 0;
    TotEarlyArrMins = 0;
    TotLatePassMins = 0;
    TotEarlyPassMins = 0;
    TotLateDepMins = 0;
    }

//---------------------------------------------------------------------------

TTrainController::~TTrainController()
    {
    for(unsigned int x=0;x<TrainVector.size();x++)
        {
        TrainVectorAt(32, x).DeleteTrain(4);
        }
    TrainVector.clear();
    }

//---------------------------------------------------------------------------

void TTrainController::Operate(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",Operate");
bool ClockState = Utilities->Clock2Stopped;
Utilities->Clock2Stopped = true;
//new section dealing with Snt & Snt-sh additions
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    const TActionVectorEntry &AVEntry0 = TDEntry.ActionVector.at(0);
    if(AVEntry0.Command == "Snt")
        {
        //calc below only for Snt & Snt-sh entries rather than all entries to save time
        const TActionVectorEntry &AVEntryLast = TDEntry.ActionVector.at(TDEntry.ActionVector.size() - 1);
        int IncrementalMinutes = 0;
        int IncrementalDigits = 0;
        if(AVEntryLast.FormatType == Repeat)
            {
            IncrementalMinutes = AVEntryLast.RearStartOrRepeatMins;
            IncrementalDigits = AVEntryLast.FrontStartOrRepeatDigits;
            }
        if((AVEntryLast.FormatType == Repeat) && (TDEntry.NumberOfTrains < 2))
            {
            throw Exception("Error - Repeat entry && less than two trains for Snt entry: " + TDEntry.HeadCode);
            }
        //see above note
        for(int y=0;y<TDEntry.NumberOfTrains;y++)
            {
            TTrainOperatingData &TTOD = TDEntry.TrainOperatingDataVector.at(y);
            if(TTOD.RunningEntry != NotStarted) continue;
            if(GetRepeatTime(2, AVEntry0.EventTime, y, IncrementalMinutes) > TTClockTime) break;//all the rest will also be greater
            AnsiString TrainHeadCode = GetRepeatHeadCode(22, TDEntry.HeadCode, y, IncrementalDigits);
            if(AddTrain(2, AVEntry0.RearStartOrRepeatMins, AVEntry0.FrontStartOrRepeatDigits, TrainHeadCode, TDEntry.StartSpeed,
                    TDEntry.Mass, TDEntry.MaxRunningSpeed, TDEntry.MaxBrakeRate, TDEntry.PowerAtRail, "Timetable", &TDEntry, y, IncrementalMinutes,
                    IncrementalDigits, TDEntry.SignallerSpeed, AVEntry0.SignallerControl))
                {
                TTOD.TrainID = TrainVector.back().TrainID;
                TTOD.RunningEntry = Running;
                }
            }
        }

    if(AVEntry0.Command == "Snt-sh")//just start this once, shuttle repeats take care of restarts
        {
        //calc below only for Snt & Snt-sh entries rather than all entries to save time
        const TActionVectorEntry &AVEntryLast = TDEntry.ActionVector.at(TDEntry.ActionVector.size() - 1);
        int IncrementalMinutes = 0;
        int IncrementalDigits = 0;
        if(AVEntryLast.FormatType == Repeat)
            {
            IncrementalMinutes = AVEntryLast.RearStartOrRepeatMins;
            IncrementalDigits = AVEntryLast.FrontStartOrRepeatDigits;
            }
        if((AVEntryLast.FormatType == Repeat) && (TDEntry.NumberOfTrains < 2))
            {
            throw Exception("Error - Repeat entry && less than two trains for Snt-sh entry: " + TDEntry.HeadCode);
            }
        //see above note
        TTrainOperatingData &TTOD = TDEntry.TrainOperatingDataVector.at(0);
        if(TTOD.RunningEntry == NotStarted)
            {
            if(AVEntry0.EventTime <= TTClockTime)
                {
                if(AddTrain(3, AVEntry0.RearStartOrRepeatMins, AVEntry0.FrontStartOrRepeatDigits, TDEntry.HeadCode, TDEntry.StartSpeed,
                        TDEntry.Mass, TDEntry.MaxRunningSpeed, TDEntry.MaxBrakeRate, TDEntry.PowerAtRail, "Timetable", &TDEntry, 0, IncrementalMinutes,
                        IncrementalDigits, TDEntry.SignallerSpeed, false))//false for SignallerControl
                    {
                    TTOD.TrainID = TrainVector.back().TrainID;
                    TTOD.RunningEntry = Running;
                    }
                }
            }
        }
    }
//deal with running trains but abort if any vectors added, would probably be OK but don't risk a vector reallocation disrupting the
//iteration, next cycle will catch up with any other pending updates
if(!TrainVector.empty())
    {
    TrainAdded = false;
    AllRoutes->CallonVector.clear(); //this will be rebuilt during the calls to UpdateTrain
    for(unsigned int x=0;x<TrainVector.size();x++)
        {
        TrainVectorAt(33, x).UpdateTrain(0);
        if(TrainAdded)
            {
            break;
            }
        }
    //set warning flags
    CrashWarning = false;
    DerailWarning = false;
    SPADWarning = false;
    CallOnWarning = false;
    SignalStopWarning = false;
    BufferAttentionWarning = false;
    for(int x = TrainVector.size() - 1; x >= 0; x--)//reverse because of erase
        {
        TTrain &Train = TrainVectorAt(34, x);
        if(Train.Crashed)//can't use background colours for crashed & derailed because same colour
            {
            CrashWarning = true;
            }
        else if(Train.Derailed)//can't use background colours for crashed & derailed because same colour
            {
            DerailWarning = true;
            }
        else if(Train.BackgroundColour == clSPADBackground)//use colour as that changes as soon as passes signal
            {
            SPADWarning = true;
            }
        else if(Train.BackgroundColour == clCallOnBackground)//use colour as also stopped at signal
            {
            CallOnWarning = true;
            }
        else if(Train.BackgroundColour == clSignalStopBackground)//use colour to distinguish from call-on
            {
            SignalStopWarning = true;
            }
        else if(Train.BackgroundColour == clBufferAttentionNeeded)//use colour to distinguish from ordinary buffer stop
            {
            BufferAttentionWarning = true;
            }
        if(Train.HasTrainGone())
            {
            AnsiString Loc = "";
            bool ElementFound = false;
            TTrackElement TE;
            if(Train.LagElement > -1)
                {
                TE = Track->TrackElementAt(531, Train.LagElement);
                ElementFound = true;
                }
            else if(Train.MidElement > -1)
                {
                TE = Track->TrackElementAt(779, Train.MidElement);
                ElementFound = true;
                }
            else if(Train.LeadElement > -1)
                {
                TE = Track->TrackElementAt(780, Train.LeadElement);
                ElementFound = true;
                }
            if(ElementFound)
                {
                if(TE.ActiveTrackElementName != "")
                    {
                    Loc = TE.ActiveTrackElementName + ", track element " + TE.ElementID;
                    }
                else
                    {
                    Loc = "track element " + TE.ElementID;
                    }
                }

            TActionVectorEntry *AVEntryPtr = Train.ActionVectorEntryPtr;
            if((Train.SignallerRemoved) || (Train.JoinedOtherTrainFlag))
            //need above first because may also have ActionVectorEntryPtr == "Fer"
                {
                Train.UnplotTrain(9);
                }
            else if(AVEntryPtr->Command == "Fer")
                {
                bool CorrectExit = false;
                if(!AVEntryPtr->ExitList.empty())
                    {
                    for(TExitListIterator ELIT=AVEntryPtr->ExitList.begin();ELIT != AVEntryPtr->ExitList.end();ELIT++)
                        {
                        if(*ELIT == Train.LagElement) CorrectExit = true;
                        }
                    }
                if(CorrectExit)
                    {
                    Train.LogAction(19, Train.HeadCode, "", Leave, Loc, AVEntryPtr->EventTime, AVEntryPtr->Warning);
                    }
                else
                    {
                    LogActionError(38, Train.HeadCode, "", FailIncorrectExit, Loc);
                    }
                }
            else
                {
                if(!AVEntryPtr->SignallerControl)
                    {
                    LogActionError(26, Train.HeadCode, "", FailUnexpectedExitRailway, Loc);
                    Train.SendMissedActionLogs(2, -2, AVEntryPtr);
                    //-2 is marker for send messages for all remaining actions except Fer if present
                    }
                else
                    {
                    Train.LogAction(31, Train.HeadCode, "", SignallerLeave, Loc, TDateTime(0), false);//false for Warning
                    }
                }
            Train.TrainDataEntryPtr->TrainOperatingDataVector.at(Train.RepeatNumber).RunningEntry = Exited;
            Train.DeleteTrain(1);
            TrainVector.erase(TrainVector.begin() + x);
            ReplotTrains(1, Display);//to reset ElementIDs for remaining trains when have removed a train
            }
        }
    }
else
    {
    //reset all flags in case last train removed with flag set
    CrashWarning = false;
    DerailWarning = false;
    SPADWarning = false;
    CallOnWarning = false;
    SignalStopWarning = false;
    BufferAttentionWarning = false;
    }
Utilities->Clock2Stopped = ClockState;
Utilities->CallLogPop(723);
}

//---------------------------------------------------------------------------
void TTrainController::FinishedOperation(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FinishedOperation");
if(!TrainVector.empty())
    {
    for(int x=TrainVector.size()-1;x>=0;x--)
        {
        TrainVectorAt(50, x).DeleteTrain(2);
        }
    TrainVector.clear();
    }
if(!TrainDataVector.empty())
    {
    for(unsigned int x=0; x<TrainDataVector.size();x++)
        {
        TTrainDataEntry &TDEntry = TrainDataVector.at(x);
        for(int y=0; y<TDEntry.NumberOfTrains;y++)
            {
            TTrainOperatingData &TOD = TDEntry.TrainOperatingDataVector.at(y);
            TOD.RunningEntry = NotStarted;
            TOD.TrainID = -1;
            TOD.EventReported = NoEvent;
            }
        }
    }
Display->GetOutputLog1()->Caption = "";
Display->GetOutputLog2()->Caption = "";
Display->GetOutputLog3()->Caption = "";
Display->GetOutputLog4()->Caption = "";
Display->GetOutputLog5()->Caption = "";
Display->GetOutputLog6()->Caption = "";
Display->GetOutputLog7()->Caption = "";
Display->GetOutputLog8()->Caption = "";
Display->GetOutputLog9()->Caption = "";
Display->GetOutputLog10()->Caption = "";
Utilities->CallLogPop(1352);
}

//---------------------------------------------------------------------------

void TTrainController::ReplotTrains(int Caller, TDisplay *Disp)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ReplotTrains");
if(!TrainVector.empty())
    {
    for(unsigned int x=0;x<TrainVector.size();x++)
        {
        TrainVectorAt(51, x).PlotTrain(4, Disp);
        }
    }
Utilities->CallLogPop(724);
}

//---------------------------------------------------------------------------

void TTrainController::WriteTrainsToImage(int Caller, Graphics::TBitmap *Bitmap)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WriteTrainsToImage");
if(!TrainVector.empty())
    {
    for(unsigned int x=0;x<TrainVector.size();x++)
        {
        TrainVectorAt(61, x).WriteTrainToImage(0, Bitmap);
        }
    }
Utilities->CallLogPop(1707);
}

//---------------------------------------------------------------------------

void TTrainController::UnplotTrains(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",UnplotTrains");
if(!TrainVector.empty())
    {
    for(unsigned int x=0;x<TrainVector.size();x++)
        {
        TrainVectorAt(52, x).UnplotTrain(10);
        }
    }
Track->ResetAllTrainIDElements(0);
Utilities->CallLogPop(725);
}

//---------------------------------------------------------------------------

bool TTrainController::AddTrain(int Caller, int RearPosition, int FrontPosition, AnsiString HeadCode, int StartSpeed, int Mass,
        double MaxRunningSpeed, double MaxBrakeRate, double PowerAtRail, AnsiString ModeStr, TTrainDataEntry *TrainDataEntryPtr,
        int RepeatNumber, int IncrementalMinutes, int IncrementalDigits, int SignallerSpeed, bool SignallerControl)
//No need for a caller here as the call is identified by the headcode.  Also the number of AddTrain calls depends on the timetable
//so it would not be possible to use constants to number the calls.
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AddTrain," + AnsiString(RearPosition) + "," + AnsiString(FrontPosition) + "," + HeadCode + "," + AnsiString(StartSpeed) +
        "," + AnsiString(Mass) + "," + ModeStr);

int RearExitPos = -1;
for(int x=0;x<4;x++)
    {
    if(Track->TrackElementAt(519,RearPosition).Conn[x] == FrontPosition)
        {
        RearExitPos = x;
        }
    }
if(RearExitPos == -1)
    {
    throw Exception("Error, RearExit == -1 in AddTrain");
    }

bool ReportFlag = true;//used to stop repeated messages from CheckStartAllowable when split failed
if(TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported != NoEvent) ReportFlag = false;

TActionEventType EventType;
if(!CheckStartAllowable(0, RearPosition, RearExitPos, HeadCode, ReportFlag, EventType))
    {//messages sent to performance log in CheckStartAllowable if ReportFlag true
    TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = EventType;
    Utilities->CallLogPop(938);
    return false;
    }

TrainDataEntryPtr->TrainOperatingDataVector.at(RepeatNumber).EventReported = NoEvent;
TTrainMode TrainMode = None;
if(ModeStr == "Timetable") TrainMode = Timetable;//all else gives 'None', 'Signaller' set within program

TTrain *NewTrain = new TTrain(0, RearPosition, RearExitPos, HeadCode, StartSpeed, Mass, MaxRunningSpeed, MaxBrakeRate, PowerAtRail,
        TrainMode, TrainDataEntryPtr, RepeatNumber, IncrementalMinutes, IncrementalDigits, SignallerSpeed);
NewTrain->ActionVectorEntryPtr = &(TrainDataEntryPtr->ActionVector.at(0));//initialise here rather than in TTrain constructor as create trains
                                                                //with Null TrainDataEntryPtr when loading session trains
if(SignallerControl)
    {
    NewTrain->TimetableFinished = true;
    NewTrain->SignallerStoppingFlag = false;
    NewTrain->TrainMode = Signaller;
    if(NewTrain->MaxRunningSpeed > NewTrain->SignallerMaxSpeed)
        {
        NewTrain->MaxRunningSpeed = NewTrain->SignallerMaxSpeed;
        }
    RailGraphics->ChangeForegroundColour(17, NewTrain->HeadCodePosition[0], NewTrain->FrontCodePtr, clFrontCodeSignaller, NewTrain->BackgroundColour);
    }

//deal with starting conditions:-
//unlocated Snt: just report entry & advance pointer
//located Snt or Sfs: set station conditions as would if had reached stop point in Update(), & advance the ActionVectorEntryPtr
//Sns doesn't need a new train
if(NewTrain->ActionVectorEntryPtr->LocationName != "")//covers all above located starts
//if location of Snt was a station (that is set as LocationName, i.e. not just any station) that isn't next departure station then
//wouldn't have accepted the timetable
    {
    //first check if LeadElement (can't access LeadElement directly yet as not set, use FrontPosition instead) is buffers, note that
    //StoppedAtBuffers is set in UpdateTrain()
    if(Track->TrackElementAt(520, FrontPosition).TrackType == Buffers)//buffer end must be ahead of train or would have failed start position check
        {
        NewTrain->StoppedAtLocation = true;
        NewTrain->PlotStartPosition(0);
        NewTrain->PlotTrainWithNewBackgroundColour(13, clStationStopBackground, Display);//pale green
        NewTrain->LogAction(20, NewTrain->HeadCode, "", Create, NewTrain->ActionVectorEntryPtr->LocationName, NewTrain->ActionVectorEntryPtr->EventTime, NewTrain->ActionVectorEntryPtr->Warning);
        if(!SignallerControl)//don't advance if SignalControlEntry
            {
            NewTrain->ActionVectorEntryPtr++;//should be a command, could be a location departure but if so can't depart so set 'Hold' anyway
            }
        NewTrain->LastActionTime = TTClockTime;
        }
    //else a through station stop
    else
        {
        NewTrain->StoppedAtLocation = true;
        NewTrain->PlotStartPosition(10);
        NewTrain->PlotTrainWithNewBackgroundColour(18, clStationStopBackground, Display);//pale green
        NewTrain->LogAction(21, NewTrain->HeadCode, "", Create, NewTrain->ActionVectorEntryPtr->LocationName, NewTrain->ActionVectorEntryPtr->EventTime, NewTrain->ActionVectorEntryPtr->Warning);
        if(!SignallerControl)//don't advance if SignalControlEntry
            {
            NewTrain->ActionVectorEntryPtr++;
            }
        NewTrain->LastActionTime = TTClockTime;
        }
    }
else //unlocated entry (i.e. not a stop entry, but could still be at a named location)
    {
    NewTrain->PlotStartPosition(11);
    TTrackElement TE = Track->TrackElementAt(530, NewTrain->RearStartElement);
    AnsiString Loc = "";
    if(TE.ActiveTrackElementName != "")
        {
        Loc = TE.ActiveTrackElementName + ", track element " + TE.ElementID;
        }
    else
        {
        Loc = "track element " + TE.ElementID;
        }
    if(TE.TrackType == Continuation)
        {
        NewTrain->LogAction(22, NewTrain->HeadCode, "", Enter, Loc, NewTrain->ActionVectorEntryPtr->EventTime, NewTrain->ActionVectorEntryPtr->Warning);
        }
    else
        {
        NewTrain->LogAction(23, NewTrain->HeadCode, "", Create, Loc, NewTrain->ActionVectorEntryPtr->EventTime, NewTrain->ActionVectorEntryPtr->Warning);
        }
    if(!SignallerControl)//don't advance if SignalControlEntry
        {
        NewTrain->ActionVectorEntryPtr++;
        }
    NewTrain->LastActionTime = TTClockTime;
    //no need to set LastActionTime for an unlocated entry
    }

//cancel a wrong-direction route if either element of train starts on one
if(NewTrain->LeadElement > -1)
    {
    NewTrain->CheckAndCancelRouteForWrongEndEntry(3, NewTrain->LeadElement, NewTrain->LeadEntryPos);
    }
if(NewTrain->MidElement > -1)
    {
    NewTrain->CheckAndCancelRouteForWrongEndEntry(4, NewTrain->MidElement, NewTrain->MidEntryPos);
    }

//set signals for a right-direction autosigs route for either element of train on one
//erase elements back to start for a non-autosigs route & check if an autosigs route immediately behind it, and if so set its signals
//note that all but autosigs routes become part of a single route, so there can only be an autosigs route behind the non-autosigs route
int RouteNumber = -1;
if(NewTrain->LeadElement > -1)
    {
    if(AllRoutes->GetRouteTypeAndNumber(13, NewTrain->LeadElement, NewTrain->LeadEntryPos, RouteNumber) == TAllRoutes::AutoSigsRoute)
        {
        AllRoutes->GetFixedRouteAt(169, RouteNumber).SetRouteSignalsOnly(0);
        }
    else if(RouteNumber > -1)//non-autosigsroute
        {
        TPrefDirElement TempPDE = AllRoutes->GetFixedRouteAt(181, RouteNumber).GetFixedPrefDirElementAt(194, 0);
        int FirstTVPos = TempPDE.GetTrackVectorPosition();
        int FirstELinkPos = TempPDE.GetELinkPos();
        while(TempPDE.GetTrackVectorPosition() != (unsigned int)(NewTrain->LeadElement))
            {
            AllRoutes->RemoveRouteElement(16, TempPDE.HLoc, TempPDE.VLoc, TempPDE.GetELink());
            TempPDE = AllRoutes->GetFixedRouteAt(182, RouteNumber).GetFixedPrefDirElementAt(195, 0);
            }
        if(TempPDE.GetTrackVectorPosition() == (unsigned int)(NewTrain->LeadElement))
            {
            AllRoutes->RemoveRouteElement(17, TempPDE.HLoc, TempPDE.VLoc, TempPDE.GetELink());//remove the last element under LeadElement
            }
        AllRoutes->ForceCancelRouteFlag = true;//to force ClearandRebuildRailway at next clock tick if not in zoom-out mode
        //now deal with a linked autosigs route
        if(Track->TrackElementAt(820, FirstTVPos).Conn[FirstELinkPos] > -1)
            {
            int LinkedRouteNumber = -1;
            if(AllRoutes->GetRouteTypeAndNumber(17, Track->TrackElementAt(821, FirstTVPos).Conn[FirstELinkPos], Track->TrackElementAt(822, FirstTVPos).ConnLinkPos[FirstELinkPos], LinkedRouteNumber) == TAllRoutes::AutoSigsRoute)
                {
                AllRoutes->GetFixedRouteAt(183, LinkedRouteNumber).SetRouteSignalsOnly(1);
                }
            }
        }
    }
if(NewTrain->MidElement > -1)
    {
    RouteNumber = -1;
    if(AllRoutes->GetRouteTypeAndNumber(14, NewTrain->MidElement, NewTrain->MidEntryPos, RouteNumber) == TAllRoutes::AutoSigsRoute)
        {
        AllRoutes->GetFixedRouteAt(170, RouteNumber).SetRouteSignalsOnly(2);
        }
    else if(RouteNumber > -1)//non-autosigsroute
        {
        TPrefDirElement TempPDE = AllRoutes->GetFixedRouteAt(184, RouteNumber).GetFixedPrefDirElementAt(196, 0);
        int FirstTVPos = TempPDE.GetTrackVectorPosition();
        int FirstELinkPos = TempPDE.GetELinkPos();
        while(TempPDE.GetTrackVectorPosition() != (unsigned int)(NewTrain->MidElement))
            {
            AllRoutes->RemoveRouteElement(18, TempPDE.HLoc, TempPDE.VLoc, TempPDE.GetELink());
            TempPDE = AllRoutes->GetFixedRouteAt(185, RouteNumber).GetFixedPrefDirElementAt(197, 0);
            }
        if(TempPDE.GetTrackVectorPosition() == (unsigned int)(NewTrain->MidElement))
            {
            AllRoutes->RemoveRouteElement(19, TempPDE.HLoc, TempPDE.VLoc, TempPDE.GetELink());//remove the last element under LeadElement
            }
        AllRoutes->ForceCancelRouteFlag = true;//to force ClearandRebuildRailway at next clock tick if not in zoom-out mode
        //now deal with a linked autosigs route
        if(Track->TrackElementAt(823, FirstTVPos).Conn[FirstELinkPos] > -1)
            {
            int LinkedRouteNumber = -1;
            if(AllRoutes->GetRouteTypeAndNumber(19, Track->TrackElementAt(824, FirstTVPos).Conn[FirstELinkPos], Track->TrackElementAt(825, FirstTVPos).ConnLinkPos[FirstELinkPos], LinkedRouteNumber) == TAllRoutes::AutoSigsRoute)
                {
                AllRoutes->GetFixedRouteAt(186, LinkedRouteNumber).SetRouteSignalsOnly(3);
                }
            }
        }
    }

TrainVector.push_back(*NewTrain);
Utilities->CallLogPop(731);
return true;
}

//---------------------------------------------------------------------------

int TTrainController::EntryPos(int Caller, int TrainIDIn, int TrackVectorNumber)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EntryPos," + AnsiString(TrainIDIn) + "," + AnsiString(TrackVectorNumber));
int VecPos = -1;
for(unsigned int x=0;x<TrainVector.size();x++)
    {
    if(TrainVectorAt(1, x).TrainID == TrainIDIn)
        {
        VecPos = x;
        }
    }
if(VecPos == -1)
    {
    throw Exception("Error, VecPos not set in EntryPos");
    }
if(TrainVectorAt(2, VecPos).LeadElement == TrackVectorNumber)
    {
    Utilities->CallLogPop(734);
    return TrainVectorAt(3, VecPos).LeadEntryPos;
    }
else if(TrainVectorAt(4, VecPos).MidElement == TrackVectorNumber)
    {
    Utilities->CallLogPop(735);
    return TrainVectorAt(5, VecPos).MidEntryPos;
    }
else if(TrainVectorAt(6, VecPos).LagElement == TrackVectorNumber)
    {
    Utilities->CallLogPop(736);
    return TrainVectorAt(7, VecPos).LagEntryPos;
    }
Utilities->CallLogPop(737);
return -1;
}

//---------------------------------------------------------------------------

TTrain &TTrainController::TrainVectorAtIdent(int Caller, int TrainID)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrainVectorAtIdent," + AnsiString(TrainID));
for(unsigned int x=0;x<TrainVector.size();x++)
    {
    if(TrainVectorAt(53, x).TrainID == TrainID)
        {
        Utilities->CallLogPop(738);
        return TrainVectorAt(54, x);
        }
    }
throw Exception("Error - No Train identified in TrainVectorAtIdent with ID = " + AnsiString(TrainID));
}

//---------------------------------------------------------------------------
//$$$$$$$$$$$$$$$$$$$$$$ Start of Timetable Functions $$$$$$$$$$$$$$$$$$$$$$$$
/*

Note:  The terms 'action' and 'entry' have been used freely for individual code lines within services in comments & in variable names, but
for messages and in the manual and help files the term Entry is reserved for a complete service or train (i.e. an entry in the timetable),
and 'event' is reserved for and individual code line within a service.  Repeats use the term 'item' if they use any at all.

In references to 'HeadCode' can have an optional prefix - up to 4 additional characters that can be anything, so long as last 4 digits
represent the headcode.  This allows links to be uniquely identified regardless of the headcode - so can have same headcodes as often as
user wishes

Prior to start time, anything except a line beginning with a time [...leading spaces...] HH:MM is ignored - can be
descriptive text or anything user wishes
A time on its own line [HH:MM], with or without leading spaces, but with anything following it before the CR (which will
be ignored) is taken as the timetable start time.
Thereafter there must be text on every line in the timetable, as the first blank line (or end of file) will be taken as the end of the
timetable.  Text can follow the 'end of timetable' blank line if the user wishes.
A line within the timetable beginning with '*', with or without leading spaces, is ignored.  Such lines can add text
within the timetable if required.
Timetable entries consist of one line per headcode (i.e. per service, not necessarily per train, as one train can run several different
services)
Each line starts with HeadCode & full train information for a new train (Snt or Snt-sh), or, for a continuing service
(Sfs, Sns, Sns-sh or Sns-fsh), can have (a) Headcode only or (b) HeadCode + Description, nothing else

All leading & trailing spaces before & after a line or any entry in a line are stripped off - these can be included to make reading a
text timetable file easier

form:-
HeadCode[;Description (plain text, no commas or semicolons)][;StartSpeed(kph); MaxRunningSpeed(kph); Mass(tonnes, prog converts to kg);
MaxBrakeRate(tonnes force, prog converts to m/s/s); & gross power(kW, prog converts to power at rail in w)
then multiple entries, separated by commas, of the form:-

HH:MM;Snt;RearStartIdent FrontStartIdent                    }StartNew                }
HH:MM;Snt-sh;RearStartIdent FrontStartIdent;Fsh HeadCode    }SNTShuttle              }
HH:MM;Sns-sh;Fxx-sh HeadCode;F-nshs HeadCode (non-repeating)}SNSShuttle              }

HH:MM;Command;HeadCode (Sfs Sns jbo fsp rsp Fns Fjo Frh-sh) }TimeCmdHeadCode         }  Train action entries
HH:MM;F-nshs;NonRepeatingShuttleLinkHeadCode                }FNSNonRepeatToShuttle   }
HH:MM;Sns-fsh;NonRepeatingShuttleLinkHeadCode               }SNSNonRepeatFromShuttle }

HH:MM;Command (cdt)                                         }TimeCmd                 }
HH:MM;Location (arr & dep)                                  }TimeLoc                 }
HH:MM;HH:MM;Location                                        }TimeTimeLoc             }
HH:MM;pas;Location                                          }PassTime                }
HH:MM;Fns-sh;Snx-sh HeadCode;Sns-fsh HeadCode (non-rep)     }FSHNewService           }
HH:MM;Fer;set of allowable IDs                              }ExitRailway             }
Command   (Frh only)                                        }FinRemHere              }

R;mm;dd;nn.                                                  Repeat                      Repeat entry

Formats:

Command only:                               Frh
Time;Command:                               cdt
Time;Command;Headcode:                      Sfs Sns jbo fsp rsp Fns Fjo Frh-sh F-nshs Sns-fsh
Time;Command;2 Element IDs:                 Snt
Time;Comand;n Element IDs:                  Fer
Time;Command;rep Headcode;nonrep Headcode:  Sns-sh Fns-sh
Time;Command;2 Element IDs;Headcode         Snt-sh
Time;Command;Location                       pas
Time;Location                               Arr Dep
Time;Time;Location                          Arr & dep together

9  Single entries:    Snt (located or unlocated); pas; cdt; TimeLoc arr & dep; TimeTimeLoc; Fer; Frh

9  1x Linked entries: Non-shuttle:  fsp or rsp -> Sfs; Fns -> Sns; Fjo -> jbo; times must match, headcodes must match
                      Shuttle: F-nshs -> Sns-sh: times match, F-nshs HeadCode matches Sns-sh 2nd Headcode;
                      Fns-sh -> Sns-fsh: Fns-sh time + all repeats = Sns-fsh time, Fns-sh 2nd headcode matches Sns-fsh Headcode

4 2x Linked entries, all shuttles:

Frh-sh -> Snt-sh:  Frh-sh time = Snt-sh time + 1 repeat while repeating, Frh-sh Headcode = Snt-sh Headcode;
       -> Sns-sh:  Frh-sh time = Sns-sh time + 1 repeat while repeating, Frh-sh Headcode = Sns-sh 1st Headcode;
       -> Remain Here (at finish location after all repeats)
Fns-sh -> Snt-sh:  Frh-sh time = Snt-sh time + 1 repeat while repeating, Fns-sh 1st Headcode = Snt-sh Headcode
       -> Sns-sh:  Frh-sh time = Sns-sh time + 1 repeat while repeating, Fns-sh 1st Headcode = Sns-sh 1st Headcode

Allowable successors:-

                Successor state     Type

Snt located         AtLoc         )              AtLoc successors:  TimeLoc dep/jbo/fsp/rsp/cdt/Frh/Fns/Fjo/Frh-sh/Fns-sh/F-nshs;
Snt Unlocated       Moving        )              Moving successors: TimeLoc arr/TimeTimeLoc/pas/Fer;
Sfs                 AtLoc         )
Sns                 AtLoc         ) Start
Sns-fsh             AtLoc         )
Snt-sh              AtLoc         )
Sns-sh              AtLoc         )

pas                 Moving        )
jbo                 AtLoc         )
fsp                 AtLoc         )
rsp                 AtLoc         ) Intermediate
cdt                 AtLoc         )
TimeLoc arr         AtLoc         )
TimeLoc dep         Moving        )
TimeTimeLoc         Moving        )

Fns                 Repeat/Nothing)
Fjo                 Repeat/Nothing)
Frh                 Repeat/Nothing)
Fer                 Repeat/Nothing) Finish
Frh-sh              Repeat        )
Fns-sh              Repeat        )
F-nshs              Nothing       )

Descriptions:
Snt                 New train
Sfs                 New service from split
Sns                 New service from another service
Sns-fsh             New non-repeating service from a shuttle service
Snt-sh              New shuttle train at a timetabled stop
Sns-sh              New shuttle service from a feeder service

pas                 Pass
jbo                 Be joined by another train
fsp                 Front split
rsp                 Rear split
cdt                 Change direction of train
TimeLoc arr         Arrival
TimeLoc dep         Departure
TimeTimeLoc         Arrival and departure

Fns                 Finish & form a new service
Fjo                 Finish & join another train
Frh                 Finish & remain here
Fer                 Finish & exit railway
Frh-sh              Finish & repeat shuttle, finally remain here
Fns-sh              Finish & repeat shuttle, finally form a non-repeating service
F-nshs              Finish & form a shuttle feeder service
*/

bool TTrainController::TimetableIntegrityCheck(int Caller, char *FileName, bool GiveMessages, bool CheckLocationsExistInRailway)//true for success
{
//Error messages mainly given in called functions, five are given here - empty file; inability to find a start time; timetable containing
//a line that is too long; timetable containing too few lines; and timetable failed to open.
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TimetableIntegrityCheck," + AnsiString(FileName));
//new for v0.2b
//compile ActiveTrackElementNameMap
TTrack::TActiveTrackElementNameMapEntry ActiveTrackElementNameMapEntry;
Track->ActiveTrackElementNameMap.clear();
for(unsigned int x=0; x<Track->TrackVector.size(); x++)
    {
//    if((Track->TrackVector.at(x).ActiveTrackElementName != "") && (Track->TrackVector.at(x).TrackType != Continuation))
    if((Track->TrackVector.at(x).ActiveTrackElementName != "") && (Track->ContinuationNameMap.find(Track->TrackVector.at(x).ActiveTrackElementName)) == Track->ContinuationNameMap.end())
        {//exclude any name that appears in a continuation, error message given in tt validation if try to include such a name in a tt
        ActiveTrackElementNameMapEntry.first = Track->TrackVector.at(x).ActiveTrackElementName;
        ActiveTrackElementNameMapEntry.second = 0; //this is a dummy value
        Track->ActiveTrackElementNameMap.insert(ActiveTrackElementNameMapEntry);
        }
    }
Track->ActiveTrackElementNameMapCompiledFlag = true;
//end of new section
std::ifstream TTBLFile(FileName, std::ios_base::binary);//binary mode so the "\r\n" pairs stay as they are rather than being entered as '\n'
if(TTBLFile)
    {
    char *TrainTimetableString = new char[10000];//enough for over 200 stations, should be adequate!
    bool EndOfFile = false;
    int Count = 0;//counts 'relevant' lines, i.e ignores any before the start time on its own line
    TTBLFile.getline(TrainTimetableString, 10000, '\0');//delimiter is '\0' as it's an AnsiString
    if(TTBLFile.eof() && (TrainTimetableString[0] == '\0'))//file empty - stores a null in 1st position if doesn't load any characters
        {                                                  //may still have eof even if read a line (no CRLF at end), and
                                                           //if so need to process it
        TimetableMessage(GiveMessages, "Timetable invalid - file empty");
        TTBLFile.close();
        delete TrainTimetableString;
        Utilities->CallLogPop(1611);
        return false;
        }
    AnsiString OneLine(TrainTimetableString);
    bool FinalCallFalse = false;
    while((Count == 0) && !ProcessOneTimetableLine(5, Count, OneLine, EndOfFile, FinalCallFalse, GiveMessages, CheckLocationsExistInRailway))//get rid of lines before the start time
        {
        //ProcessOneTimetableLine returns true for a valid start time, an EndOfFile &/or a blank entry
        TTBLFile.getline(TrainTimetableString, 10000, '\0');
        if(TTBLFile.eof() && (TrainTimetableString[0] == '\0'))//stores a null in 1st position if doesn't load any characters
            {                                                  //may still have eof even if read a line (no CRLF at end), and
                                                               //if so need to process it
            TimetableMessage(GiveMessages, "Timetable invalid - unable to find a valid start time on its own line");
            TTBLFile.close();
            delete TrainTimetableString;
            Utilities->CallLogPop(772);
            return false;
            }
        OneLine = AnsiString(TrainTimetableString);
        }
    //here when have accepted the start time
    Count++;//increment past the start time
    while(!EndOfFile)
        {
        TTBLFile.getline(TrainTimetableString, 10000, '\0');//get next line after start time
        if(TTBLFile.eof() && (TrainTimetableString[0] == '\0'))//stores a null in 1st position if doesn't load any characters
            {                                                  //may still have eof even if read a line (no CRLF at end), and
                                                               //if so need to process it
            EndOfFile = true;
            OneLine = "";
            }
        else OneLine = AnsiString(TrainTimetableString);
        if(OneLine.Length() > 9999)
            {
            TimetableMessage(GiveMessages, "Timetable contains a line that is too long - 10,000 or more characters!");
            TTBLFile.close();
            delete TrainTimetableString;
            Utilities->CallLogPop(789);
            return false;
            }
        bool FinalCallFalse = false;
        if(!ProcessOneTimetableLine(6, Count, OneLine, EndOfFile, FinalCallFalse, GiveMessages, CheckLocationsExistInRailway))//false for FinalCall - just checking at this stage
            {
            TTBLFile.close();
            delete TrainTimetableString;
            Utilities->CallLogPop(770);
            return false;
            }
        if(EndOfFile && (Count < 2))//Timetable must contain at least two relevant lines, one for start time and at least one train
            {
            TimetableMessage(GiveMessages, "Timetable has too few or no relevant entries - must have a start time on its own line and at least one train");
            TTBLFile.close();
            delete TrainTimetableString;
            Utilities->CallLogPop(771);
            return false;
            }
        Count++;
        }
    delete TrainTimetableString;
    TTBLFile.close();
    }//if(TTBLFile)
else
    {
    TimetableMessage(GiveMessages, "Failed to open timetable file, make sure it's not open in another application");
    }
Utilities->CallLogPop(753);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::ProcessOneTimetableLine(int Caller, int Count, AnsiString OneLine, bool &EndOfFile, bool FinalCall, bool GiveMessages, bool CheckLocationsExistInRailway)//return true for success

/*Format:
Prior to start time, anything except a line beginning with a time [...leading spaces...] HH:MM is ignored - can be
descriptive text or anything user wishes
A time on its own line [HH:MM], with or without leading spaces, but with anything following it before the CR (which will
be ignored) is taken as the timetable start time.
Thereafter there must be text on every line in the timetable, as the first blank line (or end of file) will be taken as the end of the
timetable.  Text can follow the 'end of timetable' blank line if the user wishes.
A line within the timetable beginning with '*', with or without leading spaces, is ignored.  Such lines can add text
within the timetable if required.
Timetable entries consist of one line per headcode (i.e. per service, not necessarily per train, as one train can run several different
services)
Each line starts with HeadCode & full train information for a new train (Snt or Snt-sh), or, for a continuing service
(Sfs, Sns, Sns-sh or Sns-fsh), can have (a) Headcode only or (b) HeadCode + Description, nothing else

All leading & trailing spaces before & after a line or any entry in a line are stripped off - these can be included to make reading a
text timetable file easier

form:-
HeadCode[;Description (plain text, no commas or semicolons)][;StartSpeed(kph); MaxRunningSpeed(kph); Mass(tonnes, prog converts to kg);
MaxBrakeRate(tonnes force, prog converts to m/s/s); & gross power(kW, prog converts to power at rail in w)
then multiple entries, separated by commas, of the form:-

    Format                                                       FormatType
[W]HH:MM;Command (cdt)                                         }TimeCmd                 }
[W]HH:MM;Fer;set of allowable IDs                              }ExitRailway             }
[W]HH:MM;pas;Location                                          }PassTime                }
[W]HH:MM;Snt;RearStartIdent FrontStartIdent                    }StartNew                }
[W]HH:MM;Command;HeadCode (Sfs Sns jbo fsp rsp Fns Fjo Frh-sh) }TimeCmdHeadCode         }
[W]HH:MM;F-nshs;non-repeating headcode                         }FNSNonRepeatToShuttle   }
[W]HH:MM;Sns-fsh;NonRepeatingShuttleLinkHeadCode               }SNSNonRepeatFromShuttle }  Train action entries
[W]HH:MM;Snt-sh;RearStartIdent FrontStartIdent;FSH HeadCode    }SNTShuttle              }
[W]HH:MM;Sns-sh;FSH HeadCode;F-nshs HeadCode (non-repeating)   }SNSShuttle              }
[W]HH:MM;Fns-sh;Details                                        }FSHNewService           }
[W]HH:MM;Location (arr & dep)                                  }TimeLoc                 }
[W]HH:MM;HH:MM;Location                                        }TimeTimeLoc             }
Command   (Frh only)                                           }FinRemHere              }

R;mm;dd;nn.                                                  Repeat                    Repeat entry

Two times represent arrival & departure, without any other events between (if arrival and departure times are the same
then departure is 30 sec after arrival), single time represents (a) event time; (b) arrival time if train not already
at location; or (c) departure time if train already at location (including train started at location either as a new
train or as a continuation service train at that location).  All lines must contain a start entry and a finish entry,
the finish being the last unless there is a repeat entry.  The repeat entry begins with 'R', then the incremental
minutes, incremental train headcode last 2 digits, and number of repeats.

Shuttle entries are where can loop back to an earlier Snt-sh or Sns-sh entry from a Frh-sh or Fns-sh (Finish Shuttle)
entry.  Here the shuttle start can have two entries, one from a set position (Snt-sh, must be located) or from a F-nshs
(Sns-sh) - with NO repeat from this source, and from a Fxx-sh, with repeats.  After all shuttle repeats Frh-sh remains
where it is, and Fns-sh links to a new service (via an Sns entry), but there must be no repeats in this new service
(it's for a shuttle train to return to depot at end of services)

Command/Location & details are as follows:-

Although headcodes can be duplicated, all joins, splits, new services etc give other headcode from both trains' povs, and
these have to match once only, i.e. if 2E44 splits to 2E45 then it can't split to 2E45 anywhere else, and 2E45 must give
2E44 in its Sfs entry.  All these are checked.
***add note re shuttles & their use of otherheadcodes + non-repeating headcodes***

Start commands:-
Snt (StartNew) = Start New Train, i.e. create new train, details = rearstartident, space, frontstartident (can't confuse
    with loc as a start entry can't have a location as details)
Sfs (TimeCmdHeadCode) = Start From Split, create a new train that has split from another train (& listed in other train's
    timetable line), details = other headcode - (can't confuse with loc as start can't be a loc)
Sns (TimeCmdHeadCode) = Start, headcode change from earlier service - no need to create train as already exists, it just
    changes its relevant information, details = old headcode (can't confuse with loc as start can't be a loc)
Snt-sh (SNTShuttle) = Start New Train, i.e. create new train, details = rearstartident, space, frontstartident (can't
    confuse with loc as start can't be a loc) then the Fsh-XX service headcode (OtherHeadCode can't be same headcode)
Sns-sh (SNSShuttle) = Start, headcode change from earlier service - no need to create train as already exists, it just
    changes its relevant information, details = the FSH-XX service headcode (OtherHeadCode, can't be same headcode)
    followed by the non-repeating F-nshs headcode (NonRepeatingShuttleLinkHeadCode)
Sns-fsh (SNSNonRepeatFromShuttle) = Start as a non-repeating service from a shuttle service that has finished all its
    repeats, details = NonRepeatingShuttleLinkHeadCode for the corresponding shuttle Fns-sh service

Intermediate commands:-
Time - Location (TimeLoc), can be arrival or departure depending on context
Time Time location (TimeTimeLoc), arrival and departure
    Location Name (exactly as used in the railway) in TimeLoc & TimeTimeLoc means that the train is required to stop at the location
pas (PassTime), Time;pas;Location
jbo (TimeCmdHeadCode) = Joined By Other = joined by other train, details = new headcode (await other train - may be delayed).  Note that the
    joining train's finish details must correspond or the file check will fail
fsp (TimeCmdHeadCode) = Front Split = a new train splits away from front of this train, both trains in same direction, details = new headcode (create
    new train - that train's starting information must correspond)
rsp (TimeCmdHeadCode) = Rear Split = a new train splits away from rear of this train, both trains in same direction, details = new headcode  (create
    new train - that train's starting information must correspond)
cdt (TimeCmd) = Change Direction of Train = change direction, no details needed & no train creation

Finish commands:-
Fns (TimeCmdHeadCode) = Finish New Service = finish, form new service in same direction, details = new headcode (no train
    creation)
F-nshs (FNSShuttle) = Finish New Service (Shuttle) = finish, form new shuttle service in same direction, details =
    shuttle headcode (no train creation)
Fjo (TimeCmdHeadCode) = Finish Join Other = finish, join other train (which must be on an adjacent element, either end -
    may have to wait for it), details = new headcode (delete train)
Frh (FinRemHere) = Finish Remain Here = stay here indefinitely, no details & no time needed
Fer (ExitRailway) = Finish, exit railway (i.e at a continuation) - details = set of allowable exit IDs
Frh-sh (TimeCmdHeadCode) = Finish then restart as a shuttle using Snt-sh or Sns-sh, when all shuttle repeats done remain
    here
Fns-sh (FSHNewService) = Finish then restart as a shuttle using Snt-sh or Sns-sh, when all shuttle repeats done form new
    service via Sns-fsh using the NonRepeatingShuttleLinkHeadCode

Repeat:-
R;mm;dd;nn (Repeat) where mm = minute increment, dd = 2nd 2 headcode digit increment & nn = no. of repeats (no max as can duplicate
    headcodes - it is up to user to avoid duplicates if he/she wishes to.

Checks carried out with error messages in this function:-
At least one comma in a service line (it's based on a .csv file)
No entries following train information;
At least one comma in remainder after train information (i.e at least a start and a finish entry);
SplitEntry returns false in an intermediate entry - message repeats the entry for information;
First entry not a start entry;
Train information incomplete before a start entry;
Entry follows a finish entry but doesn't begin with 'R';
SplitEntry returns false in a finish entry - message repeats the entry for information;
Last action entry isn't a finish entry.

Function returns false with no message if:-
Timetable start time invalid (no message is given for an invalid time as the line is assumed to be an irrelevant line; if no start
    time is found at all then an error message is given in the calling function);
SplitTrainInfo returns false (message given in called function);
SplitRepeat returns false (message given in called function).
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ProcessOneTimetableLine," + AnsiString(Count) +
        "," + OneLine + "," + AnsiString((short)FinalCall) + "," + AnsiString((short)CheckLocationsExistInRailway));
TTrainDataEntry TempTrainDataEntry;
EndOfFile = false;
StripSpaces(0, OneLine);//strip both leading and trailing spaces at ends of line and spaces before and after all commas and
//semicolons within the line
ServiceReference = "";
if(OneLine != "")
    {
    if(OneLine[1] != '*')
        {
        int SCPos = OneLine.Pos(';');
        if(SCPos == 0) ServiceReference = OneLine.SubString(1,8);
        else ServiceReference = OneLine.SubString(1, (SCPos - 1));
        }
    }
bool AllCommas = true;
for(int x=1; x<OneLine.Length()+1;x++)//check for nothing but commas (may be all commas if created from Excel) or a blank line
    {
    if(OneLine[x] != ',') AllCommas = false;
    }
if(AllCommas || (OneLine == ""))
    {
    if(Count > 0)
        {
        EndOfFile = true;
        Utilities->CallLogPop(1018);
        return true;
        }
    else
        {
        Utilities->CallLogPop(754);
        return false;
        }
    }
AnsiString First="", Second="", Third="", Fourth="";
int RearStartOrRepeatMins = 0, FrontStartOrRepeatDigits = 0, NumberOfRepeats = 0;
TDateTime EventTime(0), ArrivalTime(0), DepartureTime(0);
TDateTime StartTime(0);
TExitList ExitList;
bool Warning = false;
if(Count == 0)
    {
    AnyHeadCodeValid = false;
    if(OneLine.SubString(6,5) == ";0000")
        {
        AnyHeadCodeValid = true;
        }
    if(!CheckTimeValidity(0, OneLine, StartTime))
        {
        //no message is given for an invalid time as it's assumed to be an irrelevant line
        //if no start time is found at all then an error message is given in the calling function
        AnyHeadCodeValid = false;
        Utilities->CallLogPop(755);
        return false;
        }
    if(FinalCall)
        {
        TTClockTime = StartTime;
        TimetableStartTime = StartTime;
        }
    }
else
    {
    AnsiString TrainInfoStr = "", HeadCode = "", Description = "";
    int StartSpeed = 0, MaxRunningSpeed = 0, Mass = 0;
    double MaxBrakeRate = 0;
    int PowerAtRail = 0, SignallerSpeed = 0;
    if(OneLine[1] == '*')
        {
        Utilities->CallLogPop(1581);
        return true;//ignore any line beginning with '*' but return true as there is no error
        }
    int Pos = OneLine.Pos(',');
    if(Pos == 0)
        {
        int SubStringLength = 20;
        if(OneLine.Length() < 20) SubStringLength = OneLine.Length();
        TimetableMessage(GiveMessages, "Error in timetable - entry incomplete: see '" + OneLine.SubString(1, SubStringLength) + "'....");
        Utilities->CallLogPop(766);
        return false;
        }
    TrainInfoStr = OneLine.SubString(1, Pos-1);
    if(!SplitTrainInfo(0, TrainInfoStr, HeadCode, Description, StartSpeed, MaxRunningSpeed, Mass, MaxBrakeRate, PowerAtRail, SignallerSpeed, GiveMessages))
        {//error messages given in SplitTrainInfo
        Utilities->CallLogPop(773);
        return false;
        }
    if(FinalCall)
        {
        //store Train info - conversions done in SplitTrainInfo
        //only headcode mandatory for continuing services
        TempTrainDataEntry.HeadCode = HeadCode;
        TempTrainDataEntry.Description = Description;
        TempTrainDataEntry.StartSpeed = StartSpeed;
        TempTrainDataEntry.Mass = Mass;
        TempTrainDataEntry.MaxRunningSpeed = MaxRunningSpeed;
        TempTrainDataEntry.MaxBrakeRate = MaxBrakeRate;
        TempTrainDataEntry.PowerAtRail = PowerAtRail;
        TempTrainDataEntry.SignallerSpeed = SignallerSpeed;
        TTrainOperatingData TempTrainOperatingData;
        TempTrainDataEntry.TrainOperatingDataVector.push_back(TempTrainOperatingData);//push empty vector for now
        }
    AnsiString NewRemainder = OneLine.SubString(Pos+1, OneLine.Length()-Pos);
    //now left with series of entries for this train, but there may be a string of commas at the end of the line if created by Excel
    //so strip them off
    while(NewRemainder[NewRemainder.Length()] == ',')
        {
        if(NewRemainder.Length() > 1)
            {
            NewRemainder = NewRemainder.SubString(1, NewRemainder.Length()-1);
            }
        else
            {
            NewRemainder = "";
            break;
            }
        }
    //check if zero length & fail if so
    if(NewRemainder == "")
        {
        TimetableMessage(GiveMessages, "Error in timetable - no events following train: '" + OneLine + "'");
        Utilities->CallLogPop(769);
        return false;
        }
    //now have one more entry than there are commas
    int CommaCount = 0;
    for(int x=1;x<NewRemainder.Length()+1;x++)
        {
        if(NewRemainder[x] == ',') CommaCount++;
        }//must have at least 1 comma, for start & finish entries, unless train is entered under signaller control
    if(CommaCount == 0)
        {
        if((NewRemainder.SubString(7,3) != "Snt") || (NewRemainder[NewRemainder.Length()] != 'S'))
            {
            int SubStringLength = 20;
            if(OneLine.Length() < 20) SubStringLength = OneLine.Length();
            TimetableMessage(GiveMessages, "Error in timetable - must have at least a start and a finish event for a train that is not started under signaller control - see line beginning: '" + OneLine.SubString(1, SubStringLength) + "'....");
            Utilities->CallLogPop(783);
            return false;
            }
        }
    AnsiString OneEntry = "";
    TTimetableFormatType FormatType;
    TTimetableSequenceType SequenceType;
    TTimetableLocationType LocationType;
    TTimetableShuttleLinkType ShuttleLinkType;
    bool FinishFlag = false;
    for(int x=0;x<CommaCount+1;x++)
        {
        if((CommaCount == 0) || (x<CommaCount))//i.e. train entered under signaller control with no repeats, or entry is not the last,
                                             //in which case there's a comma & finish element or repeat still to come this entry could
                                             //be a finish but can't be a repeat
            {
            if(CommaCount == 0)
                {
                OneEntry = NewRemainder;
                NewRemainder = "";
                }
            else
                {
                Pos = NewRemainder.Pos(',');
                OneEntry = NewRemainder.SubString(1, Pos-1);
                NewRemainder = NewRemainder.SubString(Pos+1, NewRemainder.Length() - Pos);
                }
            First = ""; Second = ""; Third = ""; Fourth=""; RearStartOrRepeatMins = 0; FrontStartOrRepeatDigits = 0; NumberOfRepeats = 0;
            if(!SplitEntry(0, OneEntry, GiveMessages, CheckLocationsExistInRailway, First, Second, Third, Fourth, RearStartOrRepeatMins,
                    FrontStartOrRepeatDigits, FormatType, LocationType, SequenceType, ShuttleLinkType, ExitList, Warning))
                {
                TimetableMessage(GiveMessages, "Error in timetable - Event: '" + OneEntry + "'");
                Utilities->CallLogPop(756);
                return false;
                }
            //check if warning for Frh or Fjo & reject
            if(Warning && (Second == "Frh"))
                {
                TimetableMessage(GiveMessages, "Error in line - '" + OneEntry + "': warnings cannot be given for 'Frh' events");
                Utilities->CallLogPop(1793);
                return false;
                }
            if(Warning && (Second == "Fjo"))
                {
                TimetableMessage(GiveMessages, "Error in line - '" + OneEntry + "': warnings cannot be given for 'Fjo' events, for a train join warning add a 'W' prefix to the 'jbo' event");
                Utilities->CallLogPop(1794);
                return false;
                }
            if(x == 0)//should be start event
                {
                if(SequenceType != Start)
                    {
                    TimetableMessage(GiveMessages, "Error in timetable - First event not a start event: '" + OneEntry + "'");
                    Utilities->CallLogPop(784);
                    return false;
                    }
                if((Second == "Snt") && (Fourth == 'S') && (NewRemainder != ""))
                    {
                    if(NewRemainder[1] != 'R')
                        {
                        TimetableMessage(GiveMessages, "Error in timetable - a signaller controlled event can only be followed by a repeat, see '" + OneEntry + "'");
                        Utilities->CallLogPop(787);
                        return false;
                        }
                    }
                if((Second == "Snt") || (Second == "Snt-sh"))//need full train information including non-default values for at least HeadCode, Description,
                //MaxRunningSpeed, Mass, MaxBrakeRate, & PowerAtRail
                    {
                    if((HeadCode == "") || (Description == "") || (MaxRunningSpeed == 0) || (Mass == 0) || (MaxBrakeRate == 0) ||
                            (PowerAtRail == 0))
                        {
                        TimetableMessage(GiveMessages, "Error in timetable - train information incomplete before 'Snt' or 'Snt-sh' start event: '" + OneEntry + "'");
                        Utilities->CallLogPop(1783);
                        return false;
                        }
                    }
                if((Second == "Sfs") || (Second == "Sns") || (Second == "Sns-sh") || (Second == "Sns-fsh"))//service continuation - need at least non-default value for HeadCode
                    {
                    if(HeadCode == "")
                        {
                        TimetableMessage(GiveMessages, "Error in timetable - headcode missing before 'Sfs', 'Sns', 'Sns-sh' or 'Sns-fsh' start event: '" + OneEntry + "'");
                        Utilities->CallLogPop(788);
                        return false;
                        }
                    if((StartSpeed != 0) || (MaxRunningSpeed != 0) || (Mass != 0) || (MaxBrakeRate != 0) || (PowerAtRail != 0))
                        {
                        TimetableMessage(GiveMessages, "Error in timetable - information additional to a headcode && optional description given before 'Sfs', 'Sns', 'Sns-sh' or 'Sns-fsh'start event: '" + OneEntry + "'");
                        Utilities->CallLogPop(843);
                        return false;
                        }
                    }
                }
            if(SequenceType == Finish)
                {
                FinishFlag = true;//marker for only permitted additional entry being a repeat, only needed if the
                                  //finish entry is not the last entry
                }
            if(FinalCall)
                {
                //interpret & add to ActionVector
                TDateTime TempTime;
                TActionVectorEntry ActionVectorEntry;
                ActionVectorEntry.FormatType = FormatType;
                ActionVectorEntry.LocationType = LocationType;
                ActionVectorEntry.SequenceType = SequenceType;
                ActionVectorEntry.ShuttleLinkType = ShuttleLinkType;
                ActionVectorEntry.Warning = Warning;
                if(FormatType == TimeLoc)
                    {
                    if(CheckTimeValidity(1, First, ActionVectorEntry.EventTime)) {;}//these will all be true as final call
                    ActionVectorEntry.LocationName = Second;
                    }
                else if(FormatType == PassTime)    //new
                    {
                    if(CheckTimeValidity(17, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.LocationName = Third;
                    }
                else if(FormatType == TimeTimeLoc)
                    {
                    if(CheckTimeValidity(2, First, ActionVectorEntry.ArrivalTime)) {;}
                    if(CheckTimeValidity(3, Second, ActionVectorEntry.DepartureTime)) {;}
                    ActionVectorEntry.LocationName = Third;
                    }
                else if(FormatType == TimeCmd)
                    {
                    if(CheckTimeValidity(4, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    }
                else if(FormatType == ExitRailway)
                    {
                    if(CheckTimeValidity(18, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.ExitList = ExitList;
                    }
                else if(FormatType == StartNew)
                    {
                    if(CheckTimeValidity(5, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.RearStartOrRepeatMins = RearStartOrRepeatMins;
                    ActionVectorEntry.FrontStartOrRepeatDigits = FrontStartOrRepeatDigits;
                    if(Fourth == 'S') ActionVectorEntry.SignallerControl = true;
                    }
                else if(FormatType == SNTShuttle)
                    {
                    if(CheckTimeValidity(6, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.RearStartOrRepeatMins = RearStartOrRepeatMins;
                    ActionVectorEntry.FrontStartOrRepeatDigits = FrontStartOrRepeatDigits;
                    ActionVectorEntry.OtherHeadCode = Fourth;
                    }
                else if(FormatType == SNSShuttle)
                    {
                    if(CheckTimeValidity(7, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.OtherHeadCode = Third;
                    ActionVectorEntry.NonRepeatingShuttleLinkHeadCode = Fourth;
                    }
                else if(FormatType == TimeCmdHeadCode)
                    {
                    if(CheckTimeValidity(8, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.OtherHeadCode = Third;
                    }
                else if((FormatType == FNSNonRepeatToShuttle) || (FormatType == SNSNonRepeatFromShuttle))
                    {
                    if(CheckTimeValidity(9, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.NonRepeatingShuttleLinkHeadCode = Third;
                    }
                else if(FormatType == FSHNewService)
                    {
                    if(CheckTimeValidity(10, First, ActionVectorEntry.EventTime)) {;}
                    ActionVectorEntry.Command = Second;
                    ActionVectorEntry.OtherHeadCode = Third;
                    ActionVectorEntry.NonRepeatingShuttleLinkHeadCode = Fourth;
                    }
                else if(FormatType == FinRemHere)
                    {
                    ActionVectorEntry.Command = Second;
                    }
                TempTrainDataEntry.ActionVector.push_back(ActionVectorEntry);
                }
            }
        else//last entry, & not entered under signaller control with no repeats, i.e. could be finish or repeat
            {
            OneEntry = NewRemainder;
            First = ""; Second = ""; Third = ""; Fourth=""; RearStartOrRepeatMins = 0; FrontStartOrRepeatDigits = 0; NumberOfRepeats = 0;
            if((FinishFlag) && (OneEntry[1] != 'R'))//already had a finish entry
                    {
                    TimetableMessage(GiveMessages, "Error in timetable - Event: '" + OneEntry + "' - only a repeat can follow a finish event");
                    Utilities->CallLogPop(79);
                    return false;
                    }
            if(OneEntry[1] != 'R')//must be finish
                {
                if(!SplitEntry(1, OneEntry, GiveMessages, CheckLocationsExistInRailway, First, Second, Third, Fourth, RearStartOrRepeatMins,
                        FrontStartOrRepeatDigits, FormatType, LocationType, SequenceType, ShuttleLinkType, ExitList, Warning))
                    {
                    TimetableMessage(GiveMessages, "Error in timetable - Event: '" + OneEntry + "'");
                    Utilities->CallLogPop(757);
                    return false;
                    }
                if(SequenceType != Finish)
                    {
                    TimetableMessage(GiveMessages, "Error in timetable - last event should be a finish: '" + OneEntry + "'");
                    Utilities->CallLogPop(785);
                    return false;
                    }
                if(FinalCall)
                    {
                    //interpret & add to ActionVector
                    TDateTime TempTime;
                    TActionVectorEntry ActionVectorEntry;
                    ActionVectorEntry.FormatType = FormatType;
                    ActionVectorEntry.LocationType = LocationType;
                    ActionVectorEntry.SequenceType = SequenceType;
                    ActionVectorEntry.ShuttleLinkType = ShuttleLinkType;
                    ActionVectorEntry.Warning = Warning;
                    if(FormatType == TimeCmd)
                        {
                        if(CheckTimeValidity(11, First, ActionVectorEntry.EventTime)) {;}
                        ActionVectorEntry.Command = Second;
                        }
                    else if(FormatType == TimeCmdHeadCode)
                        {
                        if(CheckTimeValidity(12, First, ActionVectorEntry.EventTime)) {;}
                        ActionVectorEntry.Command = Second;
                        ActionVectorEntry.OtherHeadCode = Third;
                        }
                    else if(FormatType == FNSNonRepeatToShuttle)
                        {
                        if(CheckTimeValidity(13, First, ActionVectorEntry.EventTime)) {;}
                        ActionVectorEntry.Command = Second;
                        ActionVectorEntry.NonRepeatingShuttleLinkHeadCode = Third;
                        }
                    else if(FormatType == FSHNewService)
                        {
                        if(CheckTimeValidity(14, First, ActionVectorEntry.EventTime)) {;}
                        ActionVectorEntry.Command = Second;
                        ActionVectorEntry.OtherHeadCode = Third;
                        ActionVectorEntry.NonRepeatingShuttleLinkHeadCode = Fourth;
                        }
                    else if(FormatType == ExitRailway)
                        {
                        if(CheckTimeValidity(19, First, ActionVectorEntry.EventTime)) {;}
                        ActionVectorEntry.Command = Second;
                        ActionVectorEntry.ExitList = ExitList;
                        }
                    else if(FormatType == FinRemHere)
                        {
                        ActionVectorEntry.Command = Second;
                        }
                    TempTrainDataEntry.ActionVector.push_back(ActionVectorEntry);
                    }
                }
            else//repeat
                {
                if(!SplitRepeat(0, OneEntry, RearStartOrRepeatMins, FrontStartOrRepeatDigits, NumberOfRepeats, GiveMessages))
                    {
                    Utilities->CallLogPop(786);//error messages given in SplitRepeat
                    return false;
                    }
                if(FinalCall)
                    {
                    TActionVectorEntry ActionVectorEntry;
                    ActionVectorEntry.FormatType = Repeat;
                    ActionVectorEntry.LocationType = LocTypeForRepeatEntry;
                    ActionVectorEntry.SequenceType = SequTypeForRepeatEntry;
                    ActionVectorEntry.ShuttleLinkType = ShuttleLinkTypeForRepeatEntry;
                    ActionVectorEntry.RearStartOrRepeatMins = RearStartOrRepeatMins;
                    ActionVectorEntry.FrontStartOrRepeatDigits = FrontStartOrRepeatDigits;
                    ActionVectorEntry.NumberOfRepeats = NumberOfRepeats;
                    TempTrainDataEntry.ActionVector.push_back(ActionVectorEntry);
                    }
                }
            }
        }
    if(FinalCall)
        {
        TrainDataVector.push_back(TempTrainDataEntry);
        }
    }
Utilities->CallLogPop(80);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckTimeValidity(int Caller, AnsiString TimeStr, TDateTime &Time)
//1st 5 chars must be HH:MM, anything else will be ignored
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckTimeValidity," + TimeStr);
if(TimeStr.Length() < 5)
    {
    Utilities->CallLogPop(926);
    return false;
    }
if((TimeStr[1] < '0') || (TimeStr[1] > '9'))
    {
    Utilities->CallLogPop(927);
    return false;
    }
if((TimeStr[2] < '0') || (TimeStr[2] > '9'))
    {
    Utilities->CallLogPop(928);
    return false;
    }
if(TimeStr[3] != ':')
    {
    Utilities->CallLogPop(929);
    return false;
    }
if((TimeStr[4] < '0') || (TimeStr[4] > '5'))
    {
    Utilities->CallLogPop(930);
    return false;
    }
if((TimeStr[5] < '0') || (TimeStr[5] > '9'))
    {
    Utilities->CallLogPop(931);
    return false;
    }
while(TimeStr.Length() > 5) TimeStr = TimeStr.SubString(1, TimeStr.Length()-1);
double WholeHours = (AnsiString(TimeStr[1]) + AnsiString(TimeStr[2])).ToDouble();
double FracHour = ((AnsiString(TimeStr[4]) + AnsiString(TimeStr[5])).ToDouble())/60.0;
if((WholeHours + FracHour) >= 95.98334)
    {
    Utilities->CallLogPop(1817);
    return false;// > 95h 59m
    }
Time = TDateTime((WholeHours + FracHour)/24);
Utilities->CallLogPop(932);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::SplitEntry(int Caller, AnsiString OneEntry, bool GiveMessages, bool CheckLocationsExistInRailway, AnsiString &First, AnsiString &Second,
        AnsiString &Third, AnsiString &Fourth, int &RearStartOrRepeatMins, int &FrontStartOrRepeatDigits, TTimetableFormatType &FormatType,
        TTimetableLocationType &LocationType, TTimetableSequenceType &SequenceType, TTimetableShuttleLinkType &ShuttleLinkType,
        TExitList &ExitList, bool &Warning)
/*This is a train action entry from a single line of the timetable, i.e. not train information and not a repeat entry.
Return false for failure.
See description above under ProcessOneTimetableLinefor details of train action entries
NB all types set except LocationType for Sns as may be located or not
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SplitEntry," + OneEntry);
Warning = false;
TDateTime TempTime;
if(OneEntry.Length() > 0)
    {
    if(OneEntry[1] == 'W')//warning
        {
        Warning = true;
        OneEntry = OneEntry.SubString(2, OneEntry.Length() - 1);//strip it off
        }
    }
if(OneEntry == "Frh")
    {
    FormatType = FinRemHere;
    SequenceType = Finish;
    LocationType = AtLocation;
    ShuttleLinkType = NotAShuttleLink;
    Second = "Frh";
    Utilities->CallLogPop(1016);
    return true;
    }
if(OneEntry.Length() < 7)
    {
    Utilities->CallLogPop(907);
    return false;//'HH:MM;' + at least a one-letter location name
    }
int Pos = OneEntry.Pos(';');//first segment delimiter
if(Pos != 6)
    {
    Utilities->CallLogPop(908);
    return false;//no delimiter or delimiter not in position 6, has to be a time so fail
    }
First = OneEntry.SubString(1, 5);//has to be a time
if(!CheckTimeValidity(16, First, TempTime))
    {
    Utilities->CallLogPop(909);
    return false;
    }
AnsiString Remainder = OneEntry.SubString(Pos+1, OneEntry.Length()-Pos);
if((Remainder[1] >= '0') && (Remainder[1] <= '9'))//next segment is a time so this is a TimeTimeLoc & 3rd seg has to be a location to be valid
    {
    if(Remainder.Length() < 7)
        {
        Utilities->CallLogPop(910);
        return false;//'HH:MM;' + at least a one-letter location name
        }
    Pos = Remainder.Pos(';');//second segment delimiter
    if(Pos == 0)
        {
        Utilities->CallLogPop(911);
        return false;//no delimiter, has to be one between departure time & location
        }
    Second = Remainder.SubString(1, 5);//has to be a time
    if(!CheckTimeValidity(15, Second, TempTime))
        {
        Utilities->CallLogPop(912);
        return false;
        }
    Third = Remainder.SubString(Pos+1, Remainder.Length()-Pos);
    if(!CheckLocationValidity(0, Third, GiveMessages, CheckLocationsExistInRailway))
        {
        Utilities->CallLogPop(913);
        return false;
        }
    FormatType = TimeTimeLoc;
    SequenceType = Intermediate;
    LocationType = AtLocation;
    ShuttleLinkType = NotAShuttleLink;
    Utilities->CallLogPop(914);
    return true;
    }
Pos = Remainder.Pos(';');//second segment delimiter
if(Pos == 0)//no third segment so second must be a location, or cdt
    {
    Second = Remainder;
    if(Second == "cdt")
        {
        FormatType = TimeCmd;
        ShuttleLinkType = NotAShuttleLink;
        LocationType = AtLocation;
        SequenceType = Intermediate;
        Utilities->CallLogPop(915);
        return true;
        }
    if(!CheckLocationValidity(1, Second, GiveMessages, CheckLocationsExistInRailway))
        {
        Utilities->CallLogPop(916);
        return false;
        }
    else
        {
        FormatType = TimeLoc;
        LocationType = AtLocation;
        SequenceType = Intermediate;
        ShuttleLinkType = NotAShuttleLink;
        Utilities->CallLogPop(917);
        return true;
        }
    }
//here if second segment is a command, with a third & maybe fourth segments as details
if((Pos != 4) && (Pos != 7) && (Pos != 8))
    {
    Utilities->CallLogPop(918);
    return false;//no third segement or not in position 4 or 7, & should be since all commands are 3, 6 or 7 letters
    }
Second = Remainder.SubString(1, Pos-1);//command

Remainder = Remainder.SubString(Pos+1, Remainder.Length()-Pos);//details
Pos = Remainder.Pos(';');//third segment delimiter
if(Pos == 0) Third = Remainder;
else
    {
    Third = Remainder.SubString(1, Pos-1);
    Fourth = Remainder.SubString(Pos+1, Remainder.Length()-Pos);
    }
if((Second == "Snt") || (Second == "Snt-sh"))//third has to be 2 element idents with a space between
    {
    int SpacePos = Third.Pos(' ');
    if(SpacePos == 0)
        {
        Utilities->CallLogPop(919);
        return false;//no space
        }
    AnsiString RearStartStr = Third.SubString(1, SpacePos-1);
    AnsiString FrontStartStr = Third.SubString(SpacePos + 1, Third.Length() - SpacePos);
//    int RearPosition=0, FrontPosition=0, RearExitPos=0;
    if(CheckLocationsExistInRailway)
        {
        if(!CheckStartPositionValidity(0, RearStartStr, FrontStartStr, GiveMessages))
            {
            Utilities->CallLogPop(920);
            return false;
            }
        RearStartOrRepeatMins = Track->GetTrackVectorPositionFromString(3, RearStartStr, GiveMessages);
        FrontStartOrRepeatDigits = Track->GetTrackVectorPositionFromString(4, FrontStartStr, GiveMessages);
        }
    if(Second == "Snt")
        {
        FormatType = StartNew;
        SequenceType = Start;
        LocationType = NoLocation;//can't be set until know whether located or not - done in SecondPassActions
        ShuttleLinkType = NotAShuttleLink;
        }
    else//Snt-sh
        {
        FormatType = SNTShuttle;
        LocationType = AtLocation;
        SequenceType = Start;
        ShuttleLinkType = ShuttleLink;
        if(!CheckHeadCodeValidity(0, GiveMessages, Fourth))
            {
            Utilities->CallLogPop(1038);
            return false;
            }
        }
    Utilities->CallLogPop(921);
    return true;
    }

if(Second == "Sns-sh")//third & fourth have to be headcodes
    {
    FormatType = SNSShuttle;
    LocationType = AtLocation;
    SequenceType = Start;
    ShuttleLinkType = ShuttleLink;
    if(!CheckHeadCodeValidity(1, GiveMessages, Third))
        {
        Utilities->CallLogPop(1039);
        return false;
        }
    if(!CheckHeadCodeValidity(2, GiveMessages, Fourth))
        {
        Utilities->CallLogPop(1040);
        return false;
        }
    Utilities->CallLogPop(1041);
    return true;
    }

if(Second == "F-nshs")
    {
    FormatType = FNSNonRepeatToShuttle;
    LocationType = AtLocation;
    SequenceType = Finish;
    ShuttleLinkType = ShuttleLink;
    if(!CheckHeadCodeValidity(3, GiveMessages, Third))
        {
        Utilities->CallLogPop(1047);
        return false;
        }
    Utilities->CallLogPop(1048);
    return true;
    }

if(Second == "Sns-fsh")
    {
    FormatType = SNSNonRepeatFromShuttle;
    LocationType = AtLocation;
    SequenceType = Start;
    ShuttleLinkType = ShuttleLink;
    if(!CheckHeadCodeValidity(4, GiveMessages, Third))
        {
        Utilities->CallLogPop(1098);
        return false;
        }
    Utilities->CallLogPop(1099);
    return true;
    }

if(Second == "Fns-sh")//third & fourth have to be headcodes
    {
    FormatType = FSHNewService;
    LocationType = AtLocation;
    SequenceType = Finish;
    ShuttleLinkType = ShuttleLink;
    if(!CheckHeadCodeValidity(5, GiveMessages, Third))
        {
        Utilities->CallLogPop(1050);
        return false;
        }
    if(!CheckHeadCodeValidity(6, GiveMessages, Fourth))
        {
        Utilities->CallLogPop(1051);
        return false;
        }
    Utilities->CallLogPop(1052);
    return true;
    }

//new segment for 'pas'
if(Second == "pas")//third has to be a location
    {
    FormatType = PassTime;
    LocationType = EnRoute;
    SequenceType = Intermediate;
    ShuttleLinkType = NotAShuttleLink;
    if(!CheckLocationValidity(2, Third, GiveMessages, CheckLocationsExistInRailway))
        {
        Utilities->CallLogPop(1515);
        return false;
        }
    Utilities->CallLogPop(1516);
    return true;
    }

//new segment for revised 'Fer'
if(Second == "Fer")//third has to be a set of IDs separated by spaces, and at least 1
    {
    FormatType = ExitRailway;
    LocationType = EnRoute;
    SequenceType = Finish;
    ShuttleLinkType = NotAShuttleLink;
    if(CheckLocationsExistInRailway)
        {
        if(!CheckAndPopulateListOfIDs(0, Third, ExitList, GiveMessages))
            {
            Utilities->CallLogPop(1519);
            return false;
            }
        }
    Utilities->CallLogPop(1520);
    return true;
    }

//all remainder must be TimeCmdHeadCode types to be valid
if((Second != "Fns") && (Second != "Fjo") && (Second != "jbo") && (Second != "fsp") && (Second != "rsp") &&
        (Second != "Sfs") && (Second != "Sns") && (Second != "Frh-sh"))
    {
    Utilities->CallLogPop(922);
    return false;//all TimeCmdHeadCode types
    }
if(!CheckHeadCodeValidity(7, GiveMessages, Third))
    {
    Utilities->CallLogPop(923);
    return false;
    }
FormatType = TimeCmdHeadCode;
LocationType = AtLocation;
if(Second == "Frh-sh") ShuttleLinkType = ShuttleLink;
else ShuttleLinkType = NotAShuttleLink;
if((Second == "Fns") || (Second == "Fjo") || (Second == "Frh-sh"))
    {
    SequenceType = Finish;
    }
if((Second == "jbo") || (Second == "fsp") || (Second == "rsp"))
    {
    SequenceType = Intermediate;
    }
if((Second == "Sfs") || (Second == "Sns"))
    {
    SequenceType = Start;
    }
Utilities->CallLogPop(924);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckLocationValidity(int Caller, AnsiString LocStr, bool GiveMessages, bool CheckLocationsExistInRailway)
{
//check that the location name exists in the railway (only if CheckLocationsExistInRailway is true), doesn't begin with a number
//and contains no special characters
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckLocationValidity," + LocStr);
if(LocStr == "")
    {
    Utilities->CallLogPop(1353);
    return false;//has to have at least one character
    }
if((LocStr[1] >= '0') && (LocStr[1] <= '9'))
    {
    Utilities->CallLogPop(1354);
    return false;//can't begin with a number
    }
for(int x=1; x<LocStr.Length()+1;x++)
    {
    if(LocStr[x] < ' ')
        {
        Utilities->CallLogPop(1355);
        return false;//contains a special character
        }
    if(LocStr[x] > 'z')
        {
        Utilities->CallLogPop(1356);
        return false;//contains a character outside the standard ASCII set
        }
    }
//check exists in railway location list if CheckLocationsExistInRailway is true
if(CheckLocationsExistInRailway)
    {
    if(!Track->LocationNameAllocated(3, LocStr))
        {
        TimetableMessage(GiveMessages, "Location name '" + LocStr + "' appears in the timetable but is not a valid name.  To be valid there must be track (but not a continuation) at a correspondingly named location.");
        Utilities->CallLogPop(1357);
        return false;
        }
    }
Utilities->CallLogPop(1358);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckHeadCodeValidity(int Caller, bool GiveMessages, AnsiString HeadCode)
{
//if(!AnyHeadCodeValid) up to 8 characters total & last 4 characters must be NLNN where N = number and L = capital or small letter
//if(AnyHeadCodeValid) up to 8 characters total, last 2 chars must be digits & last but 2 can be any alphanumeric, upper or lower case
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + "," + AnsiString((short)GiveMessages) + ",CheckHeadCodeValidity," + HeadCode);
if((HeadCode.Length() < 4) || (HeadCode.Length() > 8))
    {
    TimetableMessage(GiveMessages, "Headcode error in '" + HeadCode + "', length must be between 4 and 8 characters, and last 4 must be a legitimate headcode");
    Utilities->CallLogPop(1359);
    return false;
    }
if((HeadCode[HeadCode.Length() - 1] < '0') || (HeadCode[HeadCode.Length() - 1] > '9'))
    {
    TimetableMessage(GiveMessages, "Headcode error in '" + HeadCode + "', last 2 characters must be digits");
    Utilities->CallLogPop(1806);
    return false;
    }
if((HeadCode[HeadCode.Length()] < '0') || (HeadCode[HeadCode.Length()] > '9'))
    {
    TimetableMessage(GiveMessages, "Headcode error in '" + HeadCode + "', last 2 characters must be digits");
    Utilities->CallLogPop(1807);
    return false;
    }
if(!AnyHeadCodeValid)
    {
    if((HeadCode[HeadCode.Length() - 3] < '0') || (HeadCode[HeadCode.Length() - 3] > '9'))
        {
        TimetableMessage(GiveMessages, "Headcode error in '" + HeadCode + "', for restricted headcodes the first character of the headcode must be a digit");
        Utilities->CallLogPop(1360);
        return false;
        }
    if(((HeadCode[HeadCode.Length() - 2] < 'A') || (HeadCode[HeadCode.Length() - 2] > 'Z')) &&
            ((HeadCode[HeadCode.Length() - 2] < 'a') || (HeadCode[HeadCode.Length() - 2] > 'z')))
        {
        TimetableMessage(GiveMessages, "Headcode error in '" + HeadCode + "', for restricted headcodes the second character of the headcode must be a letter");
        Utilities->CallLogPop(1361);
        return false;
        }
    }
else
    {
    for(int x = 3; x > 1; x--)
        {
        if(((HeadCode[HeadCode.Length() - x] < 'A') || (HeadCode[HeadCode.Length() - x] > 'Z')) &&
                ((HeadCode[HeadCode.Length() - x] < 'a') || (HeadCode[HeadCode.Length() - x] > 'z')) &&
                ((HeadCode[HeadCode.Length() - x] < '0') || (HeadCode[HeadCode.Length() - x] > '9')))
            {
            TimetableMessage(GiveMessages, "Headcode error in '" + HeadCode + "', for unrestricted headcodes the first 2 characters of the headcode must be either letters or digits");
            Utilities->CallLogPop(1790);
            return false;
            }
        }
    }
Utilities->CallLogPop(1364);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckAndPopulateListOfIDs(int Caller, AnsiString IDSet, TExitList &ExitList, bool GiveMessages)
//set of legitimate track element IDs, separated by spaces, and at least 1 present
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckSetOfIDs," + IDSet);
ExitList.clear();
AnsiString CurrentID = "";
if(IDSet.Length() == 0)
    {
    TimetableMessage(GiveMessages, "Must have at least one exit element ID following 'Fer'");
    Utilities->CallLogPop(1521);
    return false;
    }
for(int x=1;x<=IDSet.Length();x++)
    {
    char C = IDSet[x];
    if(((C < '0') || (C > '9')) && (C != ' ') && (C != 'N') && (C != '-'))
        {
        TimetableMessage(GiveMessages, "Illegal character in the set of element IDs following 'Fer' in '" + IDSet + "'");
        Utilities->CallLogPop(1522);
        return false;
        }
    }
int Pos = IDSet.Pos(' ');//look for the first space
while(true)
    {
    if(Pos == 0)
        {
        CurrentID = IDSet;
        IDSet = "";
        }
    else
        {
        CurrentID = IDSet.SubString(1, Pos-1);
        IDSet = IDSet.SubString(Pos+1, IDSet.Length()-Pos);
        }
    int VecPos = Track->GetTrackVectorPositionFromString(7, CurrentID, GiveMessages);
    if(VecPos == -1)
        {
        Utilities->CallLogPop(1523);
        return false;//messages given in GetTrackVectorPositionFromString
        }
    else
        {
        if(Track->TrackElementAt(722, VecPos).TrackType != Continuation)
            {
            TimetableMessage(GiveMessages, "The element ID '" + CurrentID + "' following 'Fer' is not an exit");
            Utilities->CallLogPop(1524);
            return false;
            }
        else
            {
            //first check for duplicates
            if(!ExitList.empty())
                {
                for(TExitListIterator ELIT=ExitList.begin();ELIT != ExitList.end();ELIT++)
                    {
                    if(*ELIT == VecPos)
                        {
                        TimetableMessage(GiveMessages, "The element ID '" + CurrentID + "' following 'Fer' duplicates an earlier element");
                        Utilities->CallLogPop(1532);
                        return false;
                        }
                    }
                }
            //of OK add it to the list
            ExitList.push_back(VecPos);
            }
        }
    if(IDSet == "")
        {
        Utilities->CallLogPop(1525);
        return true;
        }
    else
        {
        Pos = IDSet.Pos(' ');//look for the next space
        }
    }//while(true)
}

//---------------------------------------------------------------------------
bool TTrainController::SplitTrainInfo(int Caller, AnsiString TrainInfoStr, AnsiString &HeadCode, AnsiString &Description, int &StartSpeed,
        int &MaxRunningSpeed, int &Mass, double &MaxBrakeRate, int &PowerAtRail, int &SignallerSpeed, bool GiveMessages)
//7 or 8 items for a new train (6 or 7 semicolons), for a continuing service only need headcode, though can have a description, if other
//data entered for continuing service then will be ignored - message given to warn user, checks appropriate number of items and validity
//of each item
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SplitTrainInfo," + TrainInfoStr);
int Pos = 0;
AnsiString Remainder = "";
int SemiColonCount = 0;
for(int x=1; x<TrainInfoStr.Length()+1;x++)
    {
    if(TrainInfoStr[x] == ';') SemiColonCount++;
    }
if((SemiColonCount != 6) && (SemiColonCount != 7) && (SemiColonCount != 1) && (SemiColonCount != 0))
    {
    TimetableMessage(GiveMessages, "Error in train information in '" + TrainInfoStr + "'.  Should be headcode + optional description for a continuing service;" + " or headcode, description, start speed, max running speed, mass, brake force, power (and optional signaller max. speed) for a new service");
    Utilities->CallLogPop(880);
    return false;
    }
if(SemiColonCount == 0)
    {
    HeadCode = TrainInfoStr;
    if(!CheckHeadCodeValidity(8, GiveMessages, HeadCode))
        {
        Utilities->CallLogPop(881);
        return false;
        }
    Utilities->CallLogPop(882);
    return true;
    }
if(SemiColonCount == 1)
    {
    Pos = TrainInfoStr.Pos(';');//1st delimiter
    HeadCode = TrainInfoStr.SubString(1, Pos-1);
    Description = TrainInfoStr.SubString(Pos+1, TrainInfoStr.Length() - Pos);
    if(!CheckHeadCodeValidity(9, GiveMessages, HeadCode))
        {
        Utilities->CallLogPop(883);
        return false;
        }
    if(Description == "")
        {
        TimetableMessage(GiveMessages, "Train description missing in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(884);
        return false;
        }
    if(Description.Length() > 60)
        {
        TimetableMessage(GiveMessages, "Train description too long, limit of 60 characters '" + TrainInfoStr + "'");
        Utilities->CallLogPop(1157);
        return false;
        }
    for(int x=1; x<Description.Length()+1;x++)
        {
        if((Description[x] < ' ') || (Description[x] > '~'))
            {
            TimetableMessage(GiveMessages, "Train description contains invalid characters in '" + TrainInfoStr + "'");
            Utilities->CallLogPop(885);
            return false;
            }
        }
    Utilities->CallLogPop(886);
    return true;
    }
//if here must have 6 or 7 semicolons
Pos = TrainInfoStr.Pos(';');//1st delimiter
HeadCode = TrainInfoStr.SubString(1, Pos-1);
Remainder = TrainInfoStr.SubString(Pos+1, TrainInfoStr.Length() - Pos);
if(!CheckHeadCodeValidity(10, GiveMessages, HeadCode))
    {
    Utilities->CallLogPop(887);
    return false;
    }
Pos = Remainder.Pos(';');//2nd delimiter
Description = Remainder.SubString(1, Pos-1);
Remainder = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
if(Description == "")
    {
    TimetableMessage(GiveMessages, "Train description missing in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(888);
    return false;
    }
if(Description.Length() > 60)
    {
    TimetableMessage(GiveMessages, "Train description too long, limit of 60 characters '" + TrainInfoStr + "'");
    Utilities->CallLogPop(1158);
    return false;
    }
for(int x=1; x<Description.Length()+1;x++)
    {
    if((Description[x] < ' ') || (Description[x] > 126))
        {
        TimetableMessage(GiveMessages, "Train description contains invalid characters in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(889);
        return false;
        }
    }
Pos = Remainder.Pos(';');//3rd delimiter
AnsiString StartSpeedStr = Remainder.SubString(1, Pos-1);
Remainder = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
if(StartSpeedStr == "")
    {
    TimetableMessage(GiveMessages, "Train starting speed missing in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(890);
    return false;
    }
for(int x=1; x<StartSpeedStr.Length()+1;x++)
    {
    if((StartSpeedStr[x] < '0') || (StartSpeedStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Train start speed contains invalid characters in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(891);
        return false;
        }
    }
StartSpeed = StartSpeedStr.ToInt();
if(StartSpeed > TTrain::MaximumSpeedLimit)//400kph = 250mph
    {
    TimetableMessage(GiveMessages, "Train starting speed > 400km/h in '" + TrainInfoStr + "'.  Setting it to 400km/h");
    StartSpeed = TTrain::MaximumSpeedLimit;
    }
Pos = Remainder.Pos(';');//4th delimiter
AnsiString MaxRunningSpeedStr = Remainder.SubString(1, Pos-1);
Remainder = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
if(MaxRunningSpeedStr == "")
    {
    TimetableMessage(GiveMessages, "Train maximum running speed missing in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(892);
    return false;
    }
for(int x=1; x<MaxRunningSpeedStr.Length()+1;x++)
    {
    if((MaxRunningSpeedStr[x] < '0') || (MaxRunningSpeedStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Train maximum running speed contains invalid characters in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(893);
        return false;
        }
    }
MaxRunningSpeed = MaxRunningSpeedStr.ToInt();
if(MaxRunningSpeed > TTrain::MaximumSpeedLimit)//400kph = 250mph
    {
    TimetableMessage(GiveMessages, "Train maximum running speed > 400km/h in '" + TrainInfoStr + "'.  Setting it to 400km/h");
    MaxRunningSpeed = TTrain::MaximumSpeedLimit;
    }
if(MaxRunningSpeed == 0)
    {
    TimetableMessage(GiveMessages, "Train maximum running speed zero in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(894);
    return false;
    }
Pos = Remainder.Pos(';');//5th delimiter
AnsiString MassStr = Remainder.SubString(1, Pos-1);
Remainder = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
if(MassStr == "")
    {
    TimetableMessage(GiveMessages, "Train mass missing in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(895);
    return false;
    }
for(int x=1; x<MassStr.Length()+1;x++)
    {
    if((MassStr[x] < '0') || (MassStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Train mass contains invalid characters in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(896);
        return false;
        }
    }
Mass = MassStr.ToInt() * 1000;//convert tonnes to kg
if(Mass > TTrain::MaximumMassLimit)//10,000tonnes
    {
    TimetableMessage(GiveMessages, "Train mass > 10,000 tonnes in '" + TrainInfoStr + "'.  Setting it to 10,000 tonnes");
    Mass = TTrain::MaximumMassLimit;
    }
if(Mass == 0)
    {
    TimetableMessage(GiveMessages, "Train mass zero in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(897);
    return false;
    }
Pos = Remainder.Pos(';');//6th delimiter
AnsiString MaxBrakeForceStr = Remainder.SubString(1, Pos-1);
Remainder = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
if(MaxBrakeForceStr == "")
    {
    TimetableMessage(GiveMessages, "Train braking force missing in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(898);
    return false;
    }
for(int x = 1; x < (MaxBrakeForceStr.Length()+1); x++)
    {
    if((MaxBrakeForceStr[x] != '.') && ((MaxBrakeForceStr[x] < '0') || (MaxBrakeForceStr[x] > '9')))
        {
        TimetableMessage(GiveMessages, "Train braking force contains invalid characters in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(899);
        return false;
        }
    }
double MaxBrakeForce = MaxBrakeForceStr.ToDouble() * 1000;//convert to kg force
if((MaxBrakeForce/Mass) > 1)//gives 'g' braking - 9.81m/s/s
    {
    TimetableMessage(GiveMessages, "Train braking force too high in '" + TrainInfoStr + "'.  Setting it to the same as the train mass");
    MaxBrakeForce = Mass;
    }
if((MaxBrakeForce/Mass) < 0.009)
    {
    TimetableMessage(GiveMessages, "Train braking force too low in '" + TrainInfoStr + "'.  Setting it to 1% of the train mass");
    MaxBrakeForce = Mass * 0.01;
    }
//convert to m/s/s
MaxBrakeRate = MaxBrakeForce/Mass * 9.81;
//now may have just a power entry or power and signaller max. speed
AnsiString GrossPowerStr="", SignallerSpeedStr="";
if(SemiColonCount == 6)
    {
    GrossPowerStr = Remainder;
    SignallerSpeedStr = "30";//default value
    }
else//must be 7
    {
    Pos = Remainder.Pos(';');//7th delimiter
    GrossPowerStr = Remainder.SubString(1, Pos-1);
    SignallerSpeedStr = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
    }
//deal with GrossPower
if(GrossPowerStr == "")
    {
    TimetableMessage(GiveMessages, "Train power missing in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(901);
    return false;
    }
for(int x=1; x<GrossPowerStr.Length()+1;x++)
    {
    if((GrossPowerStr[x] < '0') || (GrossPowerStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Train power contains invalid characters in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(902);
        return false;
        }
    }

double GrossPower = GrossPowerStr.ToInt() * 1000;//convert to W
if(GrossPower > TTrain::MaximumPowerLimit)//100MW
    {
    TimetableMessage(GiveMessages, "Train power > 100,000kW in '" + TrainInfoStr + "'.  Setting it to 100,000kW");
    GrossPower = TTrain::MaximumPowerLimit;
    }
if(GrossPower == 0)
    {
    TimetableMessage(GiveMessages, "Train power zero in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(903);
    return false;
    }
PowerAtRail = GrossPower * 0.8;//apply ratio of 80% for rail to gross power (seems about average from an internet search)

//deal with SignallerSpeed
if(SignallerSpeedStr == "")
    {
    TimetableMessage(GiveMessages, "Signaller speed not set in '" + TrainInfoStr + "', either set a value or remove the extra semicolon");
    Utilities->CallLogPop(1771);
    return false;
    }
for(int x=1; x<SignallerSpeedStr.Length()+1;x++)
    {
    if((SignallerSpeedStr[x] < '0') || (SignallerSpeedStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Signaller speed contains invalid characters in '" + TrainInfoStr + "'");
        Utilities->CallLogPop(1769);
        return false;
        }
    }
SignallerSpeed = SignallerSpeedStr.ToInt();
if(SignallerSpeed > TTrain::MaximumSpeedLimit)
    {
    TimetableMessage(GiveMessages, "Signaller speed > 400km/h in '" + TrainInfoStr + "'.  Setting it to 400km/h");
    SignallerSpeed = TTrain::MaximumSpeedLimit;
    }
if(SignallerSpeed == 0)
    {
    TimetableMessage(GiveMessages, "Signaller speed zero in '" + TrainInfoStr + "'");
    Utilities->CallLogPop(1770);
    return false;
    }
Utilities->CallLogPop(904);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::SplitRepeat(int Caller, AnsiString OneEntry, int &RearStartOrRepeatMins, int &FrontStartOrRepeatDigits, int &NumberOfRepeats, bool GiveMessages)
{
//Format must be: R;mm;dd;nn  mm may be 1, 2 or more digits, dd may be 1 or 2 digits, nn may be 1, 2 or more digits, none may be zero
//function checks validity of each item and returns false for error
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SplitRepeat," + OneEntry);
if(OneEntry.Length() < 7)
    {
    TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - should be 'R;m;d;n'");
    Utilities->CallLogPop(865);
    return false;
    }
int SemiColonCount = 0;
for(int x=1; x<OneEntry.Length()+1;x++)
    {
    if(OneEntry[x] == ';') SemiColonCount++;
    }
if(SemiColonCount != 3)
    {
    TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - should be 'R;m;d;n'");
    Utilities->CallLogPop(866);
    return false;
    }
if((OneEntry[1] != 'R') || (OneEntry[2] != ';'))
    {
    TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - should be 'R;m;d;n'");
    Utilities->CallLogPop(867);
    return false;
    }
AnsiString Remainder = OneEntry.SubString(3, OneEntry.Length() - 2);//strip off R;

int Pos = 0;
Pos = Remainder.Pos(';');
AnsiString MinutesStr = Remainder.SubString(1, Pos-1);
Remainder = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
if(MinutesStr == "")
    {
    TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - minute increment segment missing");
    Utilities->CallLogPop(868);
    return false;
    }
for(int x=1; x<MinutesStr.Length()+1;x++)
    {
    if((MinutesStr[x] < '0') || (MinutesStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - non-digit character in minute increment segment");
        Utilities->CallLogPop(869);
        return false;
        }
    }
RearStartOrRepeatMins = MinutesStr.ToInt();
if(RearStartOrRepeatMins == 0)
    {
    TimetableMessage(GiveMessages, "Repeat minute increment is zero in:  '" + OneEntry + "' - can't have a zero value");
    Utilities->CallLogPop(870);
    return false;
    }
Pos = Remainder.Pos(';');
AnsiString DigitsStr = Remainder.SubString(1, Pos-1);
Remainder = Remainder.SubString(Pos+1, Remainder.Length() - Pos);
if(DigitsStr == "")
    {
    TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - headcode increment segment missing");
    Utilities->CallLogPop(871);
    return false;
    }
for(int x=1; x<DigitsStr.Length()+1;x++)
    {
    if((DigitsStr[x] < '0') || (DigitsStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - non-digit character in headcode increment segment");
        Utilities->CallLogPop(872);
        return false;
        }
    }
if(DigitsStr.Length() > 2)
    {
    TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - maximum number of digits for headcode increment is 2");
    Utilities->CallLogPop(873);
    return false;
    }
FrontStartOrRepeatDigits = DigitsStr.ToInt();
/* allow zero digit increments so HC can stay same for repeated services - for many suburban services the headcode digits relate to the
route rather than the service
if(FrontStartOrRepeatDigits == 0)
    {
    TimetableMessage(GiveMessages, "Repeat headcode increment is zero in:  '" + OneEntry + "' - can't have a zero value");
    Utilities->CallLogPop(874);
    return false;
    }
*/
AnsiString NumberStr = Remainder;

if(NumberStr == "")
    {
    TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - number of repeats missing");
    Utilities->CallLogPop(875);
    return false;
    }
for(int x=1; x<NumberStr.Length()+1;x++)
    {
    if((NumberStr[x] < '0') || (NumberStr[x] > '9'))
        {
        TimetableMessage(GiveMessages, "Error in repeat: '" + OneEntry + "' - non-digit character in number of repeats");
        Utilities->CallLogPop(876);
        return false;
        }
    }
NumberOfRepeats = NumberStr.ToInt();
if(NumberOfRepeats == 0)
    {
    TimetableMessage(GiveMessages, "Number of repeats is zero in:  '" + OneEntry + "' - if no repeats are needed the repeat should be omitted");
    Utilities->CallLogPop(877);
    return false;
    }
Utilities->CallLogPop(878);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::SecondPassActions(int Caller, bool GiveMessages)
/*Note that here the TrainDataVector has been compiled with FinalCall true in ProcessOneTimetableLine so work on the
vector rather than the timetable
Note also that for unlocated Snt entries the LocationType hasn't yet been set

Many of the errors caught here duplicate those in the preliminary checks, but leave in for completeness

For info:-
class TActionVectorEntry//contains a single train action - repeat entry is also of this class though no train action is taken for it
    {
    public:
    TTimetableEntryType FormatType;
    TDateTime EventTime, ArrivalTime, DepartureTime;//zeroed on creation so change to -1 as a marker for 'not set'
    AnsiString LocationName, Command, OtherHeadCode, NonRepeatingShuttleLinkHeadCode;//null on creation
    TActionVectorEntry *OtherHeadCodeStartingEntryPtr;
    int RearStartOrRepeatMins, FrontStartOrRepeatDigits;
    int NumberOfRepeats;
    TRunningVector RunningVector;//no of repeats + 1
    TActionVectorEntry() {RearStartOrRepeatMins=0; FrontStartOrRepeatDigits=0; NumberOfRepeats = 0; FormatType = NoFormat;
            EventTime=TDateTime(-1); ArrivalTime=TDateTime(-1); DepartureTime=TDateTime(-1); OtherHeadCodeStartingEntryPtr=0;}
    };

typedef std::vector<TActionVectorEntry> TActionVector;//contains all actions for a single train

class TTrainDataEntry//contains all data for a single train -c opied into train object when becomes active
    {
    public:
    AnsiString HeadCode, Description;//null on creation
    int StartSpeed, MaxRunningSpeed, Mass, MaxBrakeRate, PowerAtRail;//kph;kph;kg;m/s/s;W
    TActionVector ActionVector;
    TTrainDataEntry() {StartSpeed=0; MaxRunningSpeed=0, Mass=0; MaxBrakeRate=0; PowerAtRail=0;}
    };

/Allowable successors:-
/Snt unlocated ->  Fer, TimeLoc (arr), TimeTimeLoc, (new) pas; No others
/Snt located -> No starts, no finishes except Frh, no repeat, pas or TimeTimeLoc; any other cmd or TimeLoc (dep) OK
/Snt-sh -> No starts, finishes, repeats, pas or TimeTimeLoc; any other cmd or TimeLoc (dep) OK
/Sfs ->  No starts, finishes, repeats, pas or TimeTimeLoc; any other cmd or TimeLoc (dep) OK (must have a TimeLoc departure somewhere in sequence to
        set location, else fails)
/Sns ->  No starts, finishes, repeats, pas or TimeTimeLoc; any other cmd or TimeLoc (dep) OK (must have a TimeLoc departure somewhere in sequence to
        set location, else fails)
/Sns-sh -> No starts, finishes, repeats, pas or TimeTimeLoc; any other cmd or TimeLoc (dep) OK (must have a TimeLoc departure somewhere in sequence to
        set location, else fails)
/Sns-fsh -> No starts, finishes, repeats, pas or TimeTimeLoc; any other cmd or TimeLoc (dep) OK (must have a TimeLoc departure somewhere in sequence to
        set location, else fails)
/Fns ->  R only
/F-nshs ->  Nothing (no repeats permitted)
/Fjo ->  R only
/Frh ->  R only
/Fer ->  R only
/Frh-sh ->  R only
/Fns-sh ->  R only
/jbo ->  No starts, finishes, repeats, splits, pas or TimeTimeLoc; TimeLoc (dep), jbo or cdt OK
/fsp ->  No starts, repeats, Fer, pas or TimeTimeLoc; TimeLoc (dep) or any other OK
/rsp ->  No starts, repeats, Fer, pas or TimeTimeLoc; TimeLoc (dep) or any other OK
/cdt ->  No starts, repeats, Fer, pas or TimeTimeLoc; TimeLoc (dep) or any other OK
/TimeLoc (arr) ->  No starts, repeats, Fer, pas or TimeTimeLoc; TimeLoc (dep) or any other OK
/TimeLoc (dep) ->  Fer, TimeLoc (arr), or TimeTimeLoc, (new) pas OK, no others
/TimeTimeLoc ->  Fer, TimeLoc (arr), or TimeTimeLoc, (new) pas OK, no others
/(new) pas -> Fer, TimeLoc (arr), or TimeTimeLoc, (new) pas OK, no others
/Repeat ->  Nothing

/There must be a TimeLoc arrival (or a Sns start at location) in a sequence so successive cmd locations can be set
/Check all Snt's & set Locations if located (located = zero start speed, either element at a location (but if rear element
is a continuation then treated as unlocated), and location listed in the next TimeLoc entry, though needn't be immediately after)
/If Snt entry at a location specified in a following TimeLoc entry but start speed > 0 give error message
/Check all times increase or stay same through ActionVector
/Cycle through all entries in vector setting arr & dep times based on above list
/Add locations to all relevant cmd entries based on earlier arrival location (or earlier reference for Sfs & Sns)
/Check locations match the arr & dep TimeLoc entries
/Check same location doesn't appear twice before a cdt except for separate arr & dep TimeLocs
/Make above valid succession checks
/Check all splits have matching Sfs headcodes (both ways), add locations to SFSs & check times same
/Check all new service headcodes (Sns) have matching headcodes (both ways), add locations to SNHs & check times same
/Check a split to 'x' doesn't again split to 'x' (anywhere, not just for one train, since headcodes can be duplicated)
/Check each Fns has matching Sns headcodes (both ways), add locations to SNHs & check times same
/Check all joins have matching headcodes (both ways), locations & times & don't occur in same sequence
/Check each joined by train not joined by same train again (anywhere, not just for one train, since headcodes can be duplicated)
/Set train info for Sfs & Sns entries
/Check each repeat entry exactly matches any included joins or splits (user has to enter it to show that really wants it)
/Check at least one platform long enough for a split (only need 2 lengths) & disallow if not, need length of 2 & 1 extra
    element at each end, or length of 3 & 1 extra element at either end
/Check all TimeLocs have either Arr or Dep times set and EventTime == -1
/Check all Cmds have EventTime set & Arr & Dep times = -1
/Check all Sfs & Sns entries followed somewhere in sequence by a TimeLoc departure
/Check all locations except unlocated SNTs, FERs and Repeats have a LocationName

Give messages in function if errors detected and clear the vector.  Return false for failure.
*/


/*Earlier checks:-
Checks carried out with error messages in this function:-
At least one comma in the line (it's based on a csv file);
No entries following train information;
At least one comma in remainder after train information (i.e at least a start and a finish entry);
SplitEntry returns false in an intermediate entry - message repeats the entry for information;
First entry not a start entry;
Train information incomplete before a start entry;
Entry follows a finish entry but doesn't begin with 'R';
SplitEntry returns false in a finish entry - message repeats the entry for information;
Last action entry isn't a finish entry.

Function returns false with no message if:-
Timetable start time invalid (no message is given for an invalid time as the line is assumed to be an irrelevant line; if no start
    time is found at all then an error message is given in the calling function);
SplitTrainInfo returns false (message given in called function);
SplitRepeat returns false (message given in called function).
*/
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SecondPassActions,");
if(TrainDataVector.empty())
    {
    SecondPassMessage(GiveMessages, "Error in timetable - there appear to be no train services in the timetable, it must contain at least one");
    TrainDataVector.clear();
    Utilities->CallLogPop(1832);
    return false;
    }

/*new preliminary checks for v0.2b without changing anything, carry each out separately:-
1) must have at least one actionvector entry
2) if first actionvector entry not SignallerControl then must have at least one more actionvector entry
3) if first actionvector entry is SignallerControl then must have no more actionvector entries except a repeat
4) first entry must be a start;
5) a start must be the first entry;
6) a repeat entry must be the last;
7) for other than SignallerControl the last entry must be repeat or finish; if last entry is a repeat the last but one must be a finish;
8) a finish entry must be the last or last but one, and if last but one the last must be a repeat
Other successor errors will be caught later as all 'throws' changed to messages prior to the bulk of the sucessor checks
*/

for(unsigned int x=0;x<TrainDataVector.size();x++) //(1)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    if(TrainDataVector.at(x).ActionVector.empty())
        {
        SecondPassMessage(GiveMessages, "Error in timetable - the following service has no listed events, there must be at least one: " + TDEntry.HeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1833);
        return false;
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++) //(2)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    TActionVectorEntry AVEntry0 = TrainDataVector.at(x).ActionVector.at(0);
    if(!(AVEntry0.SignallerControl))
        {
        if(TrainDataVector.at(x).ActionVector.size() == 1)
            {
            SecondPassMessage(GiveMessages, "Error in timetable - service must have a start event and at least one other for: " + TDEntry.HeadCode);
            TrainDataVector.clear();
            Utilities->CallLogPop(1822);
            return false;
            }
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++) //(3)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    TActionVectorEntry AVEntry0 = TrainDataVector.at(x).ActionVector.at(0);
    if(AVEntry0.SignallerControl)
        {
        if(TrainDataVector.at(x).ActionVector.size() > 2)
            {
            SecondPassMessage(GiveMessages, "Error in timetable - a signaller control service can have no more than one item (a repeat) after the start event, see: " + TDEntry.HeadCode);
            TrainDataVector.clear();
            Utilities->CallLogPop(1837);
            return false;
            }
        if(TrainDataVector.at(x).ActionVector.size() > 1)
            {
            TActionVectorEntry AVEntry1 = TrainDataVector.at(x).ActionVector.at(1);
            if(AVEntry1.FormatType != Repeat)
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a signaller control service cannot have any other than a repeat after the start event, see: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1838);
                return false;
                }
            }
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++) //(4)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    TActionVectorEntry AVEntry0 = TrainDataVector.at(x).ActionVector.at(0);
    if(AVEntry0.SequenceType != Start)
        {
        SecondPassMessage(GiveMessages, "Error in timetable - the first event must be a start for: " + TDEntry.HeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1824);
        return false;
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++) //(5)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.SequenceType == Start) && (y != 0))
            {
            SecondPassMessage(GiveMessages, "Error in timetable - a start event is present that is not the first event for: " + TDEntry.HeadCode);
            TrainDataVector.clear();
            Utilities->CallLogPop(1825);
            return false;
            }
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++) //(6)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.FormatType == Repeat) && (y != (TrainDataVector.at(x).ActionVector.size() - 1)))
            {
            SecondPassMessage(GiveMessages, "Error in timetable - a repeat is present that is not the last item for: " + TDEntry.HeadCode);
            TrainDataVector.clear();
            Utilities->CallLogPop(1826);
            return false;
            }
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++) //(7)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((y == 0) && AVEntry.SignallerControl)
            {
            break;
            }
        if(y == (TrainDataVector.at(x).ActionVector.size() - 1))
            {
            if((AVEntry.FormatType != Repeat) && (AVEntry.SequenceType != Finish))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - the last item must be either a finish event or a repeat for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1827);
                return false;
                }
            if(AVEntry.FormatType == Repeat)
                {
                const TActionVectorEntry &LastAVEntry = TrainDataVector.at(x).ActionVector.at(y-1);
                if(LastAVEntry.SequenceType != Finish)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - the last event before the repeat must be a finish for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(1828);
                    return false;
                    }
                }
            }
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++) //(8)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if(AVEntry.SequenceType == Finish)
            {
            if((y != (TrainDataVector.at(x).ActionVector.size() - 1)) && (y != (TrainDataVector.at(x).ActionVector.size() - 2)))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a finish event must be either the last or last but one for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1829);
                return false;
                }
            if(y == (TrainDataVector.at(x).ActionVector.size() - 2))
                {
                if(TrainDataVector.at(x).ActionVector.at(y+1).FormatType != Repeat)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a finish event can only be followed by a repeat for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(1830);
                    return false;
                    }
                }
            }
        }
    }

//end of new preliminary checks

//check ActionVector present and check start event successor validity
//For Snt & Snt-sh set location if stopped, don't set any times yet
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    TActionVectorEntry &AVEntry0 = TrainDataVector.at(x).ActionVector.at(0);//use reference so can change internals where necessary
    if((AVEntry0.Command == "Snt") || (AVEntry0.Command == "Snt-sh"))
        {
        AnsiString LocationName = "";
        if(IsSNTEntryLocated(0, TDEntry, LocationName))//it is at a location
            {
            if(TDEntry.StartSpeed == 0)//stopped
                {
                AVEntry0.LocationName = LocationName;
                AVEntry0.LocationType = AtLocation;
                //check successor validity for located Snt that isn't a SignallerControl entry
                if(!AVEntry0.SignallerControl)
                    {
                    const TActionVectorEntry &AVEntry1 = TrainDataVector.at(x).ActionVector.at(1);//at least 2 entries present checked in integrity check so (1) valid
                    if(!AtLocSuccessor(AVEntry1))
                        {
                        //Frh following Snt-sh will return false in location check, so no need to check here
                        SecondPassMessage(GiveMessages, "Error in timetable - stopped 'Snt' or 'Snt-sh' followed by an illegal event for: " + TDEntry.HeadCode);
                        TrainDataVector.clear();
                        Utilities->CallLogPop(523);
                        return false;
                        }
                    }
                }
            else
                {
                SecondPassMessage(GiveMessages, "Error in timetable - 'Snt' or 'Snt-sh' event at stop location but start speed not zero for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(791);
                return false;
                }
            }
        else //check not Snt-sh & carry out successor validity checks for unlocated Snt that isn't a SignallerControl entry
            {
            if(AVEntry0.Command == "Snt-sh")
                {
                SecondPassMessage(GiveMessages, "Error in timetable - 'Snt-sh' event not at stop location for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1042);
                return false;
                }
            AVEntry0.LocationType = EnRoute;
            if(!AVEntry0.SignallerControl)
                {
                const TActionVectorEntry &AVEntry1 = TrainDataVector.at(x).ActionVector.at(1);//at least 2 entries checked in integrity check so (1) valid
                if(!MovingSuccessor(AVEntry1))
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - unlocated 'Snt' not followed by 'Fer', 'pas' or an arrival for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(790);
                    return false;
                    }
                }
            }
        }
    //check other start successors
    else if(AVEntry0.SequenceType == Start)
        {
        const TActionVectorEntry &AVEntry1 = TrainDataVector.at(x).ActionVector.at(1);//at least 2 entries present checked in integrity check so (1) valid
        if(!AtLocSuccessor(AVEntry1))
            {
            SecondPassMessage(GiveMessages, "Error in timetable - 'Sfs', 'Sns', 'Sns-sh' or 'Sns-fsh' followed by an illegal event for: " + TDEntry.HeadCode);
            TrainDataVector.clear();
            Utilities->CallLogPop(793);
            return false;
            }
        }
    }

//set Sfs, Sns, Sns-sh & 'Sns-fsh' locations same as following TimeLoc departure entry location, if no departure before end of sequence give error message
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    bool FoundFlag = false;
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(0);//use reference so can change internals
    if((AVEntry.Command == "Sfs") || (AVEntry.Command == "Sns") || (AVEntry.Command == "Sns-sh") || (AVEntry.Command == "Sns-fsh"))
        {
        for(unsigned int y=1;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry2.FormatType == TimeLoc)
                {
                FoundFlag = true;
                AVEntry.LocationName = AVEntry2.LocationName;
                break;
                }
            }
        if(!FoundFlag)
            {
            SecondPassMessage(GiveMessages, "Error in timetable - no location departure following an 'Sfs', 'Sns', 'Sns-sh'or 'Sns-fsh' event for: " + TDEntry.HeadCode);
            TrainDataVector.clear();
            Utilities->CallLogPop(851);
            return false;
            }
        }
    }

//set all cmd locations based on earlier location name in TimeLoc arrival or Sfs/Sns/Sns-sh/Sns-fsh/located Snt/Snt-sh locations
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.FormatType == TimeLoc) || ((AVEntry.SequenceType == Start) && (AVEntry.LocationType == AtLocation)))
            {
            if(AVEntry.LocationName == "")//if TimeLoc turns out to be a TimeLoc departure then will emerge & be rejected in successor checks for TimeLocs
                {
                SecondPassMessage(GiveMessages, "Error in timetable for " + TDEntry.HeadCode + ": an event should have had a location name associated with it but it could not be found");
                TrainDataVector.clear();
                Utilities->CallLogPop(1831);
                return false;
//                throw Exception("Error, entry location null in TimeLoc/Sfs/Sns/Sns-sh/Sns-fsh/Snt-sh/located Snt for Train: " + TDEntry.HeadCode);
                }
            for(unsigned int z=y+1;z<TrainDataVector.at(x).ActionVector.size();z++)
                {
                TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(z);//use reference so can change internals where necessary
                if((AVEntry2.Command != "") && (AVEntry2.LocationType == AtLocation))
                    {
                    AVEntry2.LocationName = AVEntry.LocationName;
                    }
                else break;
                }
            }
        }
    }
//all location names now set

//check remaining successor validity except for TimeLoc arr & dep since those times not set yet
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.SequenceType == Finish) && (AVEntry.Command != "F-nshs"))
            {
            if(y < (TrainDataVector.at(x).ActionVector.size() - 1))//i.e at least one more, must be a repeat
                {
                if(TrainDataVector.at(x).ActionVector.at(y+1).FormatType != Repeat)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - only a repeat can follow a finish entry for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(798);
                    return false;
                    }
                }
            }
        if(AVEntry.Command == "F-nshs")
            {
            if(y != (TrainDataVector.at(x).ActionVector.size() - 1))//i.e has to be the last
                {
                SecondPassMessage(GiveMessages, "Error in timetable - F-nshs (shuttle link) must be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1049);
                return false;
                }
            }
        if(AVEntry.Command == "pas")
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a 'pas' can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1518);
                return false;
                }
            }
        if(AVEntry.Command == "jbo")
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a 'jbo' can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(800);
                return false;
                }
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y+1);
            if(!AtLocSuccessor(AVEntry2))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a jbo event is followed by an illegal event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(801);
                return false;
                }
            }
        if((AVEntry.Command == "fsp") || (AVEntry.Command == "rsp"))
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a train split can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(802);
                return false;
                }
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y+1);
            if(!AtLocSuccessor(AVEntry2))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a train split is followed by an illegal event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(803);
                return false;
                }
            }
        if(AVEntry.Command == "cdt")
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a 'cdt' can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(804);
                return false;
                }
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y+1);
            if(!AtLocSuccessor(AVEntry2))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a 'cdt' is followed by an illegal event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(805);
                return false;
                }
            }
        if(AVEntry.FormatType == TimeTimeLoc)
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed arrival and departure can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(806);
                return false;
                }
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y+1);
            if(!MovingSuccessor(AVEntry2))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed arrival and departure is followed by an illegal event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(807);
                return false;
                }
            }
        if(AVEntry.FormatType == PassTime)
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a pass time can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1530);
                return false;
                }
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y+1);
            if(!MovingSuccessor(AVEntry2))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a pass time is followed by an illegal event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(1531);
                return false;
                }
            }
        if(AVEntry.FormatType == Repeat)
            {
            if(y != (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a repeat is not the last item for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(808);
                return false;
                }
            }
        }
    }

//set arrival & departure times for TimeLocs & set their EventTimes to -1
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    bool LastEntryIsAnArrival = false;
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    //first deal with unlocated Snt entries - so next entry (TimeLoc or TimeTimeLoc) is an arrival, all else stopped so the next TimeLoc is a departure
    const TActionVectorEntry &AVEntry0 = TrainDataVector.at(x).ActionVector.at(0);
    if((AVEntry0.Command == "Snt") && (AVEntry0.LocationType == EnRoute))
    //StartSpeed may or may not be 0, but train will move forwards (if capable of doing so), & next TimeLoc will be an arrival, whether or not after one or more TimeTimeLocs
        {
        LastEntryIsAnArrival = false;
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.FormatType == TimeLoc)
                {
                if((AVEntry.ArrivalTime > TDateTime(-1)) || (AVEntry.DepartureTime > TDateTime(-1)) || (AVEntry.EventTime == TDateTime(-1)))
                    {
                    throw Exception("Timetable error, TimeLoc times not as initially set for " + TDEntry.HeadCode);
                    }
                if(LastEntryIsAnArrival)
                    {
                    AVEntry.DepartureTime = AVEntry.EventTime;
                    AVEntry.EventTime = TDateTime(-1);
                    LastEntryIsAnArrival = false;
                    }
                else //last entry a departure
                    {
                    AVEntry.ArrivalTime = AVEntry.EventTime;
                    AVEntry.EventTime = TDateTime(-1);
                    LastEntryIsAnArrival = true;
                    }
                }
            }
        }
    else//all others stopped at beginning
        {
        LastEntryIsAnArrival = true;
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.FormatType == TimeLoc)
                {
                if((AVEntry.ArrivalTime > TDateTime(-1)) || (AVEntry.DepartureTime > TDateTime(-1)) || (AVEntry.EventTime == TDateTime(-1)))
                    {
                    throw Exception("Timetable error, TimeLoc times not as initially set for " + TDEntry.HeadCode);
                    }
                if(LastEntryIsAnArrival)
                    {
                    AVEntry.DepartureTime = AVEntry.EventTime;
                    AVEntry.EventTime = TDateTime(-1);
                    LastEntryIsAnArrival = false;
                    }
                else //last entry a departure
                    {
                    AVEntry.ArrivalTime = AVEntry.EventTime;
                    AVEntry.EventTime = TDateTime(-1);
                    LastEntryIsAnArrival = true;
                    }
                }
            }
        }
    }
//perform remaining successor checks for TimeLocs
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.FormatType == TimeLoc) && (AVEntry.ArrivalTime >= TDateTime(0)))//arrival
//TimeLoc (arr) ->  No starts, repeats, Fer or TimeTimeLoc; TimeLoc (dep) or any other OK
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed arrival can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(809);
                return false;
                }
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y+1);
            if(!AtLocSuccessor(AVEntry2))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed arrival is followed by an illegal event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(810);
                return false;
                }
            }
        if((AVEntry.FormatType == TimeLoc) && (AVEntry.DepartureTime >= TDateTime(0)))//departure
//TimeLoc (dep) ->  Fer, TimeLoc (arr), TimeTimeLoc, (new) pas OK, no others
            {
            if(y >= (TrainDataVector.at(x).ActionVector.size() - 1))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed departure can't be the last event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(811);
                return false;
                }
            const TActionVectorEntry &AVEntry2 = TrainDataVector.at(x).ActionVector.at(y+1);
            if(!MovingSuccessor(AVEntry2))
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed departure is followed by an illegal event for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(812);
                return false;
                }
            }
        }
    }

//check all TimeLocs have either Arr or Dep time set and EventTime == -1, all Cmds have EventTime set & Arr & Dep times == -1,
//& repeats have no times set
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if(AVEntry.FormatType == TimeLoc)
            {
            if(AVEntry.EventTime != TDateTime(-1))
                {
                throw Exception("Timetable error, TimeLoc entry has EventTime not -1 for " + TDEntry.HeadCode);
                }
            if((AVEntry.ArrivalTime == TDateTime(-1)) && (AVEntry.DepartureTime == TDateTime(-1)))
                {
                throw Exception("Timetable error, TimeLoc entry has neither arrival nor departure time set for " + TDEntry.HeadCode);
                }
            }
        if(AVEntry.FormatType == TimeTimeLoc)
            {
            if(AVEntry.EventTime != TDateTime(-1))
                {
                throw Exception("Timetable error, TimeTimeLoc entry has EventTime not -1 for " + TDEntry.HeadCode);
                }
            if((AVEntry.ArrivalTime == TDateTime(-1)) || (AVEntry.DepartureTime == TDateTime(-1)))
                {
                throw Exception("Timetable error, TimeTimeLoc entry has either arrival or departure time not set for " + TDEntry.HeadCode);
                }
            }
        if((AVEntry.FormatType == TimeCmd) || (AVEntry.FormatType == TimeCmdHeadCode) ||
                (AVEntry.FormatType == StartNew) || (AVEntry.FormatType == SNTShuttle) ||
                (AVEntry.FormatType == SNSShuttle) || (AVEntry.FormatType == FNSNonRepeatToShuttle) ||
                (AVEntry.FormatType == FSHNewService) || (AVEntry.FormatType == PassTime))
            {
            if(AVEntry.EventTime == TDateTime(-1))
                {
                throw Exception("Timetable error, Cmd or PassTime entry has EventTime not set for " + TDEntry.HeadCode);
                }
            if((AVEntry.ArrivalTime != TDateTime(-1)) || (AVEntry.DepartureTime != TDateTime(-1)))
                {
                throw Exception("Timetable error, Cmd or PassTime entry has either arrival or departure time set for " + TDEntry.HeadCode);
                }
            }
        if(AVEntry.FormatType == Repeat)
            {
            if((AVEntry.EventTime != TDateTime(-1)) || (AVEntry.ArrivalTime != TDateTime(-1)) || (AVEntry.DepartureTime != TDateTime(-1)))
                {
                throw Exception("Timetable error, Repeat entry has a time set for " + TDEntry.HeadCode);
                }
            }
        }
    }

//check times stay same or increase, note that can have time of 0 if include midnight
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    TDateTime CurrentTime = TTClockTime;//the timetable start time
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if(AVEntry.FormatType == Repeat) break;
        if(AVEntry.FormatType == FinRemHere) break;
        if(AVEntry.FormatType == TimeTimeLoc)
            {
            if(AVEntry.DepartureTime < AVEntry.ArrivalTime)
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed arrival and departure has an later arrival than departure time for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(813);
                return false;
                }
            if(AVEntry.ArrivalTime < CurrentTime)
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a timed arrival and departure has too early an arrival time for: " + TDEntry.HeadCode);
                TrainDataVector.clear();
                Utilities->CallLogPop(814);
                return false;
                }
            CurrentTime = AVEntry.DepartureTime;
            continue;
            }
        if(AVEntry.FormatType == TimeLoc)
            {
            if(AVEntry.ArrivalTime >= TDateTime(0))
                {
                if(AVEntry.ArrivalTime < CurrentTime)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a timed location event has a time that is too early for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(815);
                    return false;
                    }
                CurrentTime = AVEntry.ArrivalTime;
                }
            else
                {
                if(AVEntry.DepartureTime < CurrentTime)//both may be 0 legitimately so must allow for this
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a timed location event has a time that is too early for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(816);
                    return false;
                    }
                CurrentTime = AVEntry.DepartureTime;
                }
            continue;
            }
        if(AVEntry.EventTime < CurrentTime)//all others have EventTime set
            {
            SecondPassMessage(GiveMessages, "Error in timetable - a train event has a time that is set too early for: " + TDEntry.HeadCode + ", may be before timetable start time");
            TrainDataVector.clear();
            Utilities->CallLogPop(835);
            return false;
            }
        CurrentTime = AVEntry.EventTime;
        continue;
        }
    }

//check locations consistent
AnsiString LastLocationName = "";
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    bool LastEntryIsAnArrival = false;
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    //first deal with moving Snt entries (all else stopped)
    if((TrainDataVector.at(x).ActionVector.at(0).Command == "Snt") && (TrainDataVector.at(x).ActionVector.at(0).LocationType == EnRoute))
        {
        LastEntryIsAnArrival = false;
        LastLocationName = TrainDataVector.at(x).ActionVector.at(0).LocationName;//should be ""
        if(LastLocationName != "")
            {
            throw Exception("Timetable error, moving Snt entry has LocationName set for " + TDEntry.HeadCode);
            }
        for(unsigned int y=1;y<TrainDataVector.at(x).ActionVector.size();y++)//note that immediate successor to a moving Snt can only be a Moving type
                                                                             //if it's a SignallerControl entry then the condition isn't met
            {
            const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.FormatType == Repeat) break;//repeat = reached end (+allows repeat after signaller controlled entry)
            else if((AVEntry.FormatType == TimeCmdHeadCode) || (AVEntry.FormatType == FNSNonRepeatToShuttle))
                {
                if(AVEntry.LocationName != LastLocationName)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event is inconsistent for: " + TDEntry.HeadCode + " && " + AVEntry.Command);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(823);
                    return false;
                    }
                }
            else if(AVEntry.FormatType == TimeCmd) //cdt is the only TimeCmd
                {
                if(AVEntry.LocationName != LastLocationName)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event is inconsistent for: " + TDEntry.HeadCode + " && " + AVEntry.Command);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(824);
                    return false;
                    }
                }
            else if(AVEntry.FormatType == TimeTimeLoc)
                {
                if(AVEntry.LocationName == LastLocationName)//last entry must be a departure or would have failed earlier
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event in a timed arrival and departure is the same as the last location for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(825);
                    return false;
                    }
                LastLocationName = AVEntry.LocationName;
                LastEntryIsAnArrival = false;
                }
            else if(AVEntry.FormatType == TimeLoc)
                {
                if(LastEntryIsAnArrival && (AVEntry.LocationName != LastLocationName))
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event for a timed departure is different from the arrival location for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(826);
                    return false;
                    }
                else if(!LastEntryIsAnArrival && (AVEntry.LocationName == LastLocationName))
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event for a timed arrival is the same as the earlier departure location for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(827);
                    return false;
                    }
                LastLocationName = AVEntry.LocationName;
                LastEntryIsAnArrival = !LastEntryIsAnArrival;
                }
            }
        }
    else//all stationary starting entries
        {
        LastEntryIsAnArrival = true;
        LastLocationName = TrainDataVector.at(x).ActionVector.at(0).LocationName;
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.FormatType == Repeat) break;
            else if((AVEntry.FormatType == TimeCmdHeadCode) || (AVEntry.FormatType == FNSNonRepeatToShuttle))
            //no need to add anything for shuttle starts since they are at loc (0) anyway
                {
                if(AVEntry.LocationName != LastLocationName)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event is inconsistent for: " + TDEntry.HeadCode + " && " + AVEntry.Command);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(828);
                    return false;
                    }
                }
            else if(AVEntry.FormatType == TimeCmd)//cdt is the only TimeCmd
                {
                if(AVEntry.LocationName != LastLocationName)
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event is inconsistent for: " + TDEntry.HeadCode + " && " + AVEntry.Command);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(829);
                    return false;
                    }
                }
            else if(AVEntry.FormatType == TimeTimeLoc)
                {
                if(AVEntry.LocationName == LastLocationName)//last entry must be a departure or would have failed earier
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event in a timed arrival and departure is the same as the last location for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(830);
                    return false;
                    }
                LastLocationName = AVEntry.LocationName;
                LastEntryIsAnArrival = false;
                }
            else if(AVEntry.FormatType == TimeLoc)
                {
                if(LastEntryIsAnArrival && (AVEntry.LocationName != LastLocationName))
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event for a timed departure is different from the arrival location for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(831);
                    return false;
                    }
                if(!LastEntryIsAnArrival && (AVEntry.LocationName == LastLocationName))
                    {
                    SecondPassMessage(GiveMessages, "Error in timetable - a location event for a timed arrival is the same as the earlier departure location for: " + TDEntry.HeadCode);
                    TrainDataVector.clear();
                    Utilities->CallLogPop(832);
                    return false;
                    }
                LastLocationName = AVEntry.LocationName;
                LastEntryIsAnArrival = !LastEntryIsAnArrival;
                }
            }
        }
    }

//Check same location doesn't appear twice before a cdt except for separate arr & dep TimeLocs
//i.e. same location can appear in any number of consecutive entries but once changed can't repeat
//before a direction change
AnsiString LocationNameToBeChecked = "";
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    unsigned int y=0;
    const TActionVectorEntry &AVEntry0 = TDEntry.ActionVector.at(0);
    //first discard unlocated Snt entries as they don't have location name set
    if((AVEntry0.Command == "Snt") && (AVEntry0.LocationType == EnRoute))
        {
        y=1;
        }
    while(y < TDEntry.ActionVector.size())//need to check each location name separately in turn, skipped for SignallerControl entries
                                          //if y == 1
        {
        if((TDEntry.ActionVector.at(y).Command == "Fer") || (TDEntry.ActionVector.at(y).FormatType == Repeat))
            {
            break;//out of the 'while' loop since have reached the end & 'Fer' & 'Repeat' have no location name set
            }
        LocationNameToBeChecked = TDEntry.ActionVector.at(y).LocationName;
        for(unsigned int z=y;z<TDEntry.ActionVector.size();z++)
            {
            const TActionVectorEntry &AVEntry = TDEntry.ActionVector.at(z);
            if((AVEntry.Command == "Fer") || (AVEntry.FormatType == Repeat))
                {
                break;//out of the 'z' loop since have reached the end & 'Fer' & 'Repeat' have no location name set
                }
            if(AVEntry.Command == "cdt")
                {
                break;//out of the 'z' loop since the check is only valid up to a change of direction
                }
            if(AVEntry.LocationName == LocationNameToBeChecked) continue;//keep going while name same
            if(AVEntry.LocationName != LocationNameToBeChecked)          //if name different check forwards to see if repeats
                {
                for(unsigned int a=z;a<TDEntry.ActionVector.size();a++)
                    {
                    if(TDEntry.ActionVector.at(a).Command == "cdt")
                        {
                        break;//out of the 'a' & 'z' loops since the check is only valid up to a change of direction
                        }
                    if(TDEntry.ActionVector.at(a).LocationName == LocationNameToBeChecked)
                        {
                        SecondPassMessage(GiveMessages, "Error in timetable - a location entry appears twice inappropriately for: " + TDEntry.HeadCode);
                        TrainDataVector.clear();
                        Utilities->CallLogPop(833);
                        return false;
                        }
                    }
                break;//out of the 'z' loop since have checked 'a' as far as need to
                }
            }
        y++;
        }
    }

//check all locations except unlocated 'Snt' & 'Fer' have LocationName set
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.LocationName == "") && (AVEntry.Command != "Snt") && (AVEntry.Command != "Fer") && (AVEntry.FormatType != Repeat))
            {
            throw Exception("Error, non- 'Snt', 'Fer' or Repeat entry doesn't have a location name set for " + TDEntry.HeadCode);
            }
        AnsiString LocName = "";//dummy, only used so can call IsSNTEntryLocated
        if((AVEntry.Command == "Snt") && (IsSNTEntryLocated(1, TrainDataVector.at(x), LocName)))
            {
            if(AVEntry.LocationName == "")
                {
                throw Exception("Error, 'Snt' entry at a stop location doesn't have a location name set for " + TDEntry.HeadCode);
                }
            }
        if((AVEntry.Command == "Snt") && !(IsSNTEntryLocated(2, TrainDataVector.at(x), LocName)))
            {
            if(AVEntry.LocationName != "")
                {
                throw Exception("Error, 'Snt' unlocated entry has a location name set for " + TDEntry.HeadCode);
                }
            }
        }
    }

/*Check a split to 'x' doesn't again split to 'x' (anywhere, not just for one train, since headcodes can be duplicated)
Check each joined by train not joined by same train again (anywhere, not just for one train, since headcodes can be duplicated)
Check each change of headcode not repeated anywhere else (anywhere, not just for one train, since headcodes can be duplicated)

i.e. check everywhere where there is an 'OtherHeadCode' that it matches once only with its reference (both ways) + set
the OtherHeadCodeStartingEntryPtr pointers where appropriate + train information for splits & new services

BUT need to separate the shuttles from non-shuttles, because can have two trains reference each other in both forms,
eg 2F44 Sns-sh ends in Fns to 2F45, & Sns 2F45 ends in Fns-sh to 2F44.  Here 2F45 is the 'OtherHeadCode' for both
Sns-sh & Fns in train 2F44, & 2F44 is the 'OtherHeadCode' for both Sns & Fns-sh in train 2F45.
*/
for(unsigned int x=0;x<TrainDataVector.size();x++)//new test to ensure no duplicate links at all, other checks ensure none for shuttles,
                                                  //non-shuttles & non-repeating links separately, but don't check that there isn't a
                                                  //duplicate between a non-repeating shuttle and another - leave original tests in as
                                                  //these also set the pointers
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if(AVEntry.OtherHeadCode != "")
            {
            if(!CheckForDuplicateCrossReferences(0, TDEntry.HeadCode, AVEntry.OtherHeadCode, GiveMessages))
                {
                Utilities->CallLogPop(1584);
                return false;//error message given in called function
                }
            }
        if(AVEntry.NonRepeatingShuttleLinkHeadCode != "")
            {
            if(!CheckForDuplicateCrossReferences(1, TDEntry.HeadCode, AVEntry.NonRepeatingShuttleLinkHeadCode, GiveMessages))
                {
                Utilities->CallLogPop(1585);
                return false;//error message given in called function
                }
            }
        }
    }
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.Command != "Sns-sh") && (AVEntry.Command != "Snt-sh") && (AVEntry.Command != "Fns-sh") && (AVEntry.Command != "Frh-sh"))
            {
            if(AVEntry.OtherHeadCode != "")
                {
                if(!CheckCrossReferencesAndSetData(0, TDEntry.HeadCode, AVEntry.OtherHeadCode, false, GiveMessages))//false = non-shuttle
                    {
                    Utilities->CallLogPop(864);
                    return false;//error message given in called function
                    }
                }
            }
        }
    }
//now repeat the check just for the shuttles
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.Command == "Sns-sh") || (AVEntry.Command == "Snt-sh") || (AVEntry.Command == "Fns-sh") || (AVEntry.Command == "Frh-sh"))
            {
            if(AVEntry.OtherHeadCode != "")
                {
                if(!CheckCrossReferencesAndSetData(1, TDEntry.HeadCode, AVEntry.OtherHeadCode, true, GiveMessages))//true = shuttle
                    {
                    Utilities->CallLogPop(1100);
                    return false;//error message given in called function
                    }
                }
            }
        }
    }

//check for proper non-repeating link cross references and that they have no repeats & that times are consistent
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if(AVEntry.NonRepeatingShuttleLinkHeadCode != "")
            {
            if(!CheckNonRepeatingShuttleLinksAndSetData(0, TDEntry.HeadCode, AVEntry.NonRepeatingShuttleLinkHeadCode, GiveMessages))
                {
                Utilities->CallLogPop(1060);
                return false;//error message given in called function
                }
            }
        }
    }

//check that each shuttle start ends either in Fns or Fxx-sh (though a single service can't end in Fxx-sh), and that
//when the Fxx-sh is reached it references the original start and not another shuttle - not allowed to link two shuttles,
//don't ever need to and as designed would skip repeats
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if((AVEntry.Command == "Sns-sh") || (AVEntry.Command == "Snt-sh"))
            {
            if(!CheckShuttleServiceIntegrity(0, &(TrainDataVector.at(x)), GiveMessages))
                {
                Utilities->CallLogPop(1090);
                return false;//error message given in called function
                }
            }
        }
    }

//check all entries have all types set to something
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        if(AVEntry.FormatType == NoFormat)
            {
            throw Exception("Error - timetable ActionVector entry no. " + AnsiString(y) + " has FormatType unset for: " + TDEntry.HeadCode);
            }
        else if(AVEntry.SequenceType == NoSequence)
            {
            throw Exception("Error - timetable ActionVector entry no. " + AnsiString(y) + " has SequenceType unset for: " + TDEntry.HeadCode);
            }
        else if(AVEntry.LocationType == NoLocation)
            {
            throw Exception("Error - timetable ActionVector entry no. " + AnsiString(y) + " has LocationType unset for: " + TDEntry.HeadCode);
            }
        else if(AVEntry.ShuttleLinkType == NoShuttleLink)
            {
            throw Exception("Error - timetable ActionVector entry no. " + AnsiString(y) + " has ShuttleLinkType unset for: " + TDEntry.HeadCode);
            }
        }
    }

//all OK if reach here, so set up the TrainOperatingDataVector (already has one entry) & NumberOfTrains
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    TTrainDataEntry &TDEntry = TrainDataVector.at(x);//non-const reference so can alter content
    TTrainOperatingData TData;
    const TActionVectorEntry &LastAVEntry = TDEntry.ActionVector.at(TDEntry.ActionVector.size() - 1);
    if(LastAVEntry.FormatType == Repeat)//check if a repeat
        {
        /*
        class TTrainOperatingData
            {
            public:
            /int TrainID; - default, set at construction
            /TRunningEntry RunningEntry; - default, set at construction
            TDateTime StartTime; - calc below from inc minutes
            AnsiString HeadCode; - calc below from inc digits
            TTrainOperatingData() {Mass=0; MaxBrakeRate=0; PowerAtRail=0; RunningEntry=NotStarted; TrainID = -1;}//ID -1 = marker for not running
            };
        */
        TDEntry.NumberOfTrains = LastAVEntry.NumberOfRepeats + 1;
        for(int y=1; y<TDEntry.NumberOfTrains; y++)
            {
            TDEntry.TrainOperatingDataVector.push_back(TData);
            }
        }
    else TDEntry.NumberOfTrains = 1;
    }

//check that don't include any Continuation names
for(unsigned int x=0; x<TrainDataVector.size();x++)
    {
    for(unsigned int y=0; y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        AnsiString LocName = TrainDataVector.at(x).ActionVector.at(y).LocationName;
        AnsiString HC = TrainDataVector.at(x).HeadCode;
        if(LocName != "")
            {
            if(Track->ContinuationNameMap.find(LocName) != Track->ContinuationNameMap.end())
                {
                SecondPassMessage(GiveMessages, "Error in timetable - continuation names (" + LocName + ") must not be included, see service " + HC);
                TrainDataVector.clear();
                Utilities->CallLogPop(1578);
                return false;
                }
            }
        }
    }

//check that all repeat times below 96h
for(unsigned int x=0; x<TrainDataVector.size();x++)
    {
    int NumRepeats = (TrainDataVector.at(x).NumberOfTrains) - 1;
    int IncMinutes = 0;
    if(TrainDataVector.at(x).ActionVector.at(TrainDataVector.at(x).ActionVector.size() - 1).FormatType == Repeat)
        {
        IncMinutes = TrainDataVector.at(x).ActionVector.at(TrainDataVector.at(x).ActionVector.size() - 1).RearStartOrRepeatMins;
        }
    else continue;//basic times already checked in CheckTimeValidity
    AnsiString HC = TrainDataVector.at(x).HeadCode;
    for(unsigned int y=0; y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        if((double)TrainDataVector.at(x).ActionVector.at(y).EventTime > -1)
            {
            if(((double)GetRepeatTime(32, TrainDataVector.at(x).ActionVector.at(y).EventTime, NumRepeats, IncMinutes) >= 3.9994)) //3d 23h 59m = 3.9993055556
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a repeat time exceeds 95h 59m, see service " + HC);
                TrainDataVector.clear();
                Utilities->CallLogPop(1818);
                return false;
                }
            }
        if((double)TrainDataVector.at(x).ActionVector.at(y).ArrivalTime > -1)
            {
            if(((double)GetRepeatTime(33, TrainDataVector.at(x).ActionVector.at(y).EventTime, NumRepeats, IncMinutes) >= 3.9994)) //3d 23h 59m = 3.9993055556
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a repeat entry time exceeds 95h 59m, see service " + HC);
                TrainDataVector.clear();
                Utilities->CallLogPop(1819);
                return false;
                }
            }
        if((double)TrainDataVector.at(x).ActionVector.at(y).DepartureTime > -1)
            {
            if(((double)GetRepeatTime(34, TrainDataVector.at(x).ActionVector.at(y).EventTime, NumRepeats, IncMinutes) >= 3.9994)) //3d 23h 59m = 3.9993055556
                {
                SecondPassMessage(GiveMessages, "Error in timetable - a repeat entry time exceeds 95h 59m, see service " + HC);
                TrainDataVector.clear();
                Utilities->CallLogPop(1820);
                return false;
                }
            }
        }
    }

//Now that all set up change any extended headcodes back to ordinary headcodes
for(unsigned int x=0; x<TrainDataVector.size();x++)
    {
    StripExcessFromHeadCode(0, TrainDataVector.at(x).HeadCode);
    for(unsigned int y=0; y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        StripExcessFromHeadCode(1, TrainDataVector.at(x).ActionVector.at(y).OtherHeadCode);
        StripExcessFromHeadCode(2, TrainDataVector.at(x).ActionVector.at(y).NonRepeatingShuttleLinkHeadCode);
        }
    }

//SaveTrainDataVectorToFile();//test
BuildContinuationTrainExpectationMultiMap(0);
Utilities->CallLogPop(782);
return true;
}

//---------------------------------------------------------------------------
//Moving successors: TimeLoc arr/TimeTimeLoc/pas/Fer;
bool TTrainController::MovingSuccessor(const TActionVectorEntry &AVEntry)
{
return ((AVEntry.FormatType ==  TimeLoc) || (AVEntry.FormatType ==  TimeTimeLoc) || (AVEntry.Command == "pas") ||
        (AVEntry.Command == "Fer"));
}

//---------------------------------------------------------------------------
//AtLoc successors:  TimeLoc dep/jbo/fsp/rsp/cdt/Frh/Fns/Fjo/Frh-sh/Fns-sh/F-nshs;
bool TTrainController::AtLocSuccessor(const TActionVectorEntry &AVEntry)
{
return ((AVEntry.FormatType ==  TimeLoc) || (AVEntry.Command ==  "jbo") || (AVEntry.Command == "fsp") ||
        (AVEntry.Command == "rsp") || (AVEntry.Command == "cdt")  || (AVEntry.Command == "Frh")  ||
        (AVEntry.Command == "Fns")  || (AVEntry.Command == "Fjo")  || (AVEntry.Command == "Frh-sh")  ||
        (AVEntry.Command == "Fns-sh")  || (AVEntry.Command == "F-nshs"));
}

//---------------------------------------------------------------------------

void TTrainController::StripExcessFromHeadCode(int Caller, AnsiString &HeadCode)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",StripExcessFromHeadCode," + HeadCode);
if(HeadCode.Length() > 4)//ignore otherwise
    {
    HeadCode = HeadCode.SubString(HeadCode.Length() - 3, 4);
    }
Utilities->CallLogPop(1593);
}

//---------------------------------------------------------------------------

bool TTrainController::CheckForDuplicateCrossReferences(int Caller, AnsiString MainHeadCode, AnsiString SecondHeadCode, bool GiveMessages)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckForDuplicateCrossReferences," + MainHeadCode + "," + SecondHeadCode);
int ForwardCount = 0;
int ReverseCount = 0;

if(MainHeadCode == SecondHeadCode)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - Service " + MainHeadCode + " has an event that references itself");
    TrainDataVector.clear();
    Utilities->CallLogPop(1594);
    return false;
    }

//forward check
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    if(TDEntry.HeadCode == MainHeadCode)
        {
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.OtherHeadCode == SecondHeadCode)
                {
                ForwardCount++;
                }
            if(AVEntry.NonRepeatingShuttleLinkHeadCode == SecondHeadCode)//need own check in case both 'Other' & 'NonRepeating' have same headcode
                {
                ForwardCount++;
                }
            }
        }
    }
if(ForwardCount == 0)
//this is an exception because the headcodes are selected in the same order as the forward check
    {
    throw Exception("Error, ForwardCount == 0 in CheckForDuplicateCrossReferences after called with found values");
    }

if(ForwardCount > 2)//can have 2 if one is Sns-sh linking from another leg of the shuttle, and Fns links out to that same leg
    {
    SecondPassMessage(GiveMessages, "Error in timetable - found more than two references to " + SecondHeadCode + " from a train whose headcode is " + MainHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(1587);
    return false;
    }

//reverse check
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    if(TDEntry.HeadCode == SecondHeadCode)
        {
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            const TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.OtherHeadCode == MainHeadCode)
                {
                ReverseCount++;
                }
            if(AVEntry.NonRepeatingShuttleLinkHeadCode == MainHeadCode)
                {
                ReverseCount++;
                }
            }
        }
    }

if(ReverseCount == 0)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - failed to find a reference to " + MainHeadCode + " from a train whose headcode is " + SecondHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(1588);
    return false;
    }
if(ReverseCount > 2)//can have 2 if one is a second shuttle leg with a link in from Fns, and it links out to the same service with Fxx-sh
    {
    SecondPassMessage(GiveMessages, "Error in timetable - found more than two references to " + MainHeadCode + " from a train whose headcode is " + SecondHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(1589);
    return false;
    }

if(ForwardCount != ReverseCount)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - " + MainHeadCode + " has a different number of references to " + SecondHeadCode + " than the other way round");
    TrainDataVector.clear();
    Utilities->CallLogPop(1610);
    return false;
    }

Utilities->CallLogPop(1590);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckCrossReferencesAndSetData(int Caller, AnsiString MainHeadCode, AnsiString OtherHeadCode, bool Shuttle, bool GiveMessages)
/*Return false for no find or more than one find, check correct types of link
First run through all trains whose headcode is the MainHeadCode (may be > 1) & for each entry whose
'other' is OtherHeadCode increment a forward counter.  Keep a pointer to the 'OtherHeadCode' entry for use later
Must be exactly 1 forward count.  NB Forward relates to MainHeadCode
Then do the same in reverse.
Using the pointers check the event times, then check that the locations & commands match - if main is a split then other must be Sfs;
if main is Fns other must be Sns; if main is jbo other must be Fjo.
Also check platform lengths OK for a split location (call to Track function for this - at least one platform at location has to be long
enough).  If all succeeds so far set the relevant OtherHeadCodeStartingEntryPtr to the new service starting point + train information
for Sfs & Sns services.  Finally check the repeat entries if present are consistent

Check all except the NonRepeatingShuttleLinkHeadCodes, which only occur from F-nshs to Sns-sh, and from Fns-sh to
Sns-fsh.  All others should check out OK, but check shuttles & non-shuttles separately.

/NB prohibit main & other headcodes being same, causes probs in failing to recognise locations
*/

{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckCrossReferencesAndSetData," + MainHeadCode + "," + OtherHeadCode);
int ForwardCount = 0;
int ReverseCount = 0;
unsigned int ForwardTDVectorNumber, ReverseTDVectorNumber;
TActionVectorEntry *ReverseEntryPtr = 0, *ForwardEntryPtr = 0;
TTrainDataEntry *MainTrainDataPtr = 0;
TTrainDataEntry *OtherTrainDataPtr = 0;

//forward check
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    if(TDEntry.HeadCode == MainHeadCode)
        {
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(!Shuttle && (AVEntry.Command != "Sns-sh") && (AVEntry.Command != "Snt-sh") &&
                    (AVEntry.Command != "Fns-sh") && (AVEntry.Command != "Frh-sh"))
                {
                if(AVEntry.OtherHeadCode == OtherHeadCode)
                    {
                    MainTrainDataPtr = &TrainDataVector.at(x);
                    ForwardEntryPtr = &AVEntry;
                    ForwardCount++;
                    ForwardTDVectorNumber = x;
                    }
                }
            else if(Shuttle && ((AVEntry.Command == "Sns-sh") || (AVEntry.Command == "Snt-sh") ||
                    (AVEntry.Command == "Fns-sh") || (AVEntry.Command == "Frh-sh")))
                {
                if(AVEntry.OtherHeadCode == OtherHeadCode)
                    {
                    MainTrainDataPtr = &TrainDataVector.at(x);
                    ForwardEntryPtr = &AVEntry;
                    ForwardCount++;
                    ForwardTDVectorNumber = x;
                    }
                }
            }
        }
    }
if(ForwardCount == 0)
//this is an exception because the headcodes are selected in the same order as the forward check
    {
    throw Exception("Error, ForwardCount == 0 in CheckCrossReferencesAndSetData after called with found values");
    }
if(ForwardCount > 1)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - found more than one reference to " + OtherHeadCode + " from a train whose headcode is " + MainHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(836);
    return false;
    }

//reverse check
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    if(TDEntry.HeadCode == OtherHeadCode)
        {
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(!Shuttle && (AVEntry.Command != "Sns-sh") && (AVEntry.Command != "Snt-sh") &&
                    (AVEntry.Command != "Fns-sh") && (AVEntry.Command != "Frh-sh"))
                {
                if(AVEntry.OtherHeadCode == MainHeadCode)
                    {
                    OtherTrainDataPtr = &TrainDataVector.at(x);
                    ReverseCount++;
                    ReverseEntryPtr = &AVEntry;
                    ReverseTDVectorNumber = x;
                    }
                }
            else if(Shuttle && ((AVEntry.Command == "Sns-sh") || (AVEntry.Command == "Snt-sh") ||
                    (AVEntry.Command == "Fns-sh") || (AVEntry.Command == "Frh-sh")))
                {
                if(AVEntry.OtherHeadCode == MainHeadCode)
                    {
                    OtherTrainDataPtr = &TrainDataVector.at(x);
                    ReverseCount++;
                    ReverseEntryPtr = &AVEntry;
                    ReverseTDVectorNumber = x;
                    }
                }
            }
        }
    }

if(ReverseCount == 0)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - failed to find a reference to " + MainHeadCode + " from a train whose headcode is " + OtherHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(837);
    return false;
    }
if(ReverseCount > 1)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - found more than one reference to " + MainHeadCode + " from a train whose headcode is " + OtherHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(838);
    return false;
    }

//these will all be false for !Shuttle
bool ForwardShuttleStart = ((ForwardEntryPtr->Command == "Sns-sh") || (ForwardEntryPtr->Command == "Snt-sh"));
bool ForwardShuttleFinish = ((ForwardEntryPtr->Command == "Fns-sh") || (ForwardEntryPtr->Command == "Frh-sh"));
bool ReverseShuttleStart = ((ReverseEntryPtr->Command == "Sns-sh") || (ReverseEntryPtr->Command == "Snt-sh"));
bool ReverseShuttleFinish = ((ReverseEntryPtr->Command == "Fns-sh") || (ReverseEntryPtr->Command == "Frh-sh"));

if(Shuttle && MainTrainDataPtr->ActionVector.back().FormatType != Repeat)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - shuttle train " + MainHeadCode + " does not have a repeat");
    TrainDataVector.clear();
    Utilities->CallLogPop(1058);
    return false;
    }

if(Shuttle && OtherTrainDataPtr->ActionVector.back().FormatType != Repeat)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - shuttle train " + OtherHeadCode + " does not have a repeat");
    TrainDataVector.clear();
    Utilities->CallLogPop(1059);
    return false;
    }

if(ForwardEntryPtr->LocationName == "")
    {
    SecondPassMessage(GiveMessages, "Error in timetable - cross referencing train " + MainHeadCode + ", which references train " + OtherHeadCode + ", does not have a location set");
    TrainDataVector.clear();
    Utilities->CallLogPop(526);
    return false;
    }

if(ReverseEntryPtr->LocationName == "")
    {
    SecondPassMessage(GiveMessages, "Error in timetable - cross referenced train " + OtherHeadCode + ", referenced by " + MainHeadCode + ", does not have a location set");
    TrainDataVector.clear();
    Utilities->CallLogPop(527);
    return false;
    }

if(ForwardEntryPtr->LocationName != ReverseEntryPtr->LocationName)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - cross referenced train " + OtherHeadCode + " is at a different location to the referencing train " + MainHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(842);
    return false;
    }

//ignore shuttle repeat links for first time check
if(!Shuttle)
    {
    if(ForwardEntryPtr->EventTime != ReverseEntryPtr->EventTime)
        {
        SecondPassMessage(GiveMessages, "Error in timetable - cross referenced train " + OtherHeadCode + " has a different event time to the referencing train " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(525);
        return false;
        }
    }

//need to allow for repeat times multiplying up by repeating time for shuttle repeat links
//no need to check from reverse to forward as already checked links consistent, and if include will send message twice
if(ForwardShuttleStart && ReverseShuttleFinish)//Shuttle must be true if these are true
    {
    if(!CheckShuttleRepeatTime(0, ForwardEntryPtr->EventTime, ReverseEntryPtr->EventTime, OtherTrainDataPtr->ActionVector.back().RearStartOrRepeatMins))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - shuttle service " + MainHeadCode + " first repeat restart time not consistent with finish service " + OtherHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1055);
        return false;
        }
    }

if((ReverseEntryPtr->Command == "Sfs") || (ReverseEntryPtr->Command == "Sns"))//doesn't matter about ForwardEntryPtr being Sfs/Sns as called for every occurrence of an 'OtherHeadCode' so won't escape
    {
    if(ReverseTDVectorNumber == ForwardTDVectorNumber)
        {
        SecondPassMessage(GiveMessages, "Error in timetable - an 'Sfs' or 'Sns' event (" + OtherHeadCode + ") appears in the same sequence as the corresponding linked event " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(528);
        return false;
        }
    }

if(ReverseEntryPtr->Command == "Fjo")//doesn't matter about ForwardEntryPtr being Fjo as called for every occurrence of an 'OtherHeadCode' so won't escape
    {
    if(ReverseTDVectorNumber == ForwardTDVectorNumber)
        {
        SecondPassMessage(GiveMessages, "Error in timetable - an 'Fjo' event (" + OtherHeadCode + ") appears in the same sequence as the corresponding linked event " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(862);
        return false;
        }
    }

if(ReverseEntryPtr->Command == "Fns")//doesn't matter about ForwardEntryPtr being Fns as called for every occurrence of an 'OtherHeadCode' so won't escape
    {
    if(ReverseTDVectorNumber == ForwardTDVectorNumber)
        {
        SecondPassMessage(GiveMessages, "Error in timetable - an 'Fns' event (" + OtherHeadCode + ") appears in the same sequence as the corresponding linked event " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(529);
        return false;
        }
    }

if(ForwardEntryPtr->Command == "Sfs")
    {
    if((ReverseEntryPtr->Command != "fsp") && (ReverseEntryPtr->Command != "rsp"))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding split train event for the train that starts from a split whose headcode is " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(530);
        return false;
        }
    }

if((ForwardEntryPtr->Command == "fsp") || (ForwardEntryPtr->Command == "rsp"))
    {
    if(ReverseEntryPtr->Command != "Sfs")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'Sfs' event for the train split whose headcode is " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(839);
        return false;
        }
    else
        {
        if(!(Track->LocationNameAllocated(4, ForwardEntryPtr->LocationName)))
            {
            SecondPassMessage(GiveMessages, "Error in timetable - can't find '" + ForwardEntryPtr->LocationName + "' in railway");
            TrainDataVector.clear();
            Utilities->CallLogPop(849);
            return false;
            }
        if(!(Track->OneNamedLocationElementAtLocation(0, ForwardEntryPtr->LocationName)))
            {
            SecondPassMessage(GiveMessages, "Error in timetable - can't find any named location elements at '" + ForwardEntryPtr->LocationName + "'");
            TrainDataVector.clear();
            Utilities->CallLogPop(850);
            return false;
            }
        if(!(Track->OneNamedLocationLongEnoughForSplit(0, ForwardEntryPtr->LocationName)))
            {
            SecondPassMessage(GiveMessages, "Error in timetable - location too short to split a train at " + ForwardEntryPtr->LocationName);
            TrainDataVector.clear();
            Utilities->CallLogPop(846);
            return false;
            }
        ForwardEntryPtr->LinkedTrainEntryPtr = OtherTrainDataPtr;
        ReverseEntryPtr->LinkedTrainEntryPtr = MainTrainDataPtr;
        if(OtherTrainDataPtr->Description == "") OtherTrainDataPtr->Description = MainTrainDataPtr->Description;
        OtherTrainDataPtr->MaxRunningSpeed = MainTrainDataPtr->MaxRunningSpeed;
        }
    }

if(ForwardEntryPtr->Command == "Sns")
    {
    if(ReverseEntryPtr->Command != "Fns")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'Fns' event for the 'Sns' train whose headcode is " + MainHeadCode + " and forms a new service with headcode " + OtherHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(531);
        return false;
        }
    }

if(ForwardEntryPtr->Command == "Fns")
    {
    if(ReverseEntryPtr->Command != "Sns")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'Sns' event for the train whose headcode is " + MainHeadCode + " and forms a new service with headcode " + OtherHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(840);
        return false;
        }
    else
        {
        ForwardEntryPtr->LinkedTrainEntryPtr = OtherTrainDataPtr;
        ReverseEntryPtr->LinkedTrainEntryPtr = MainTrainDataPtr;
        if(OtherTrainDataPtr->Description == "") OtherTrainDataPtr->Description = MainTrainDataPtr->Description;
        OtherTrainDataPtr->MaxRunningSpeed = MainTrainDataPtr->MaxRunningSpeed;
        }
    }
if(ForwardEntryPtr->Command == "jbo")
    {
    if(ReverseEntryPtr->Command != "Fjo")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'Fjo' event for the train whose headcode is " + MainHeadCode + " and is joined by a train with headcode " + OtherHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(841);
        return false;
        }
    }

if(ForwardEntryPtr->Command == "Fjo")
    {
    if(ReverseEntryPtr->Command != "jbo")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'jbo' event for the train whose headcode is " + MainHeadCode + " and joins a train with headcode " + OtherHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(532);
        return false;
        }
    else
        {
        ForwardEntryPtr->LinkedTrainEntryPtr = OtherTrainDataPtr;
        ReverseEntryPtr->LinkedTrainEntryPtr = MainTrainDataPtr;
        if(MainTrainDataPtr->MaxRunningSpeed < OtherTrainDataPtr->MaxRunningSpeed) OtherTrainDataPtr->MaxRunningSpeed = MainTrainDataPtr->MaxRunningSpeed;
        }
    }

if(ForwardShuttleStart)//(ForwardEntryPtr->Command == "Sns-sh") || (ForwardEntryPtr->Command == "Snt-sh"))
    {
    if(!ReverseShuttleFinish)//(ReverseEntryPtr->Command != "Fns-sh") && (ReverseEntryPtr->Command != "Frh-sh"))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - incorrect shuttle link to train whose headcode is " + MainHeadCode + " from train whose headcode is " + OtherHeadCode + ", has to be Fns-sh, Frh-sh");
        TrainDataVector.clear();
        Utilities->CallLogPop(1056);
        return false;
        }
    }

if(ReverseShuttleStart)//(ReverseEntryPtr->Command == "Sns-sh") || (ReverseEntryPtr->Command == "Snt-sh"))
    {
    if(!ForwardShuttleFinish)//(ForwardEntryPtr->Command != "Fns-sh") && (ForwardEntryPtr->Command != "Frh-sh"))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - incorrect shuttle link to train whose headcode is " + OtherHeadCode + " from train whose headcode is " + MainHeadCode + ", has to be Fns-sh, Frh-sh");
        TrainDataVector.clear();
        Utilities->CallLogPop(1057);
        return false;
        }
    else
        {
        ForwardEntryPtr->LinkedTrainEntryPtr = OtherTrainDataPtr;
        ReverseEntryPtr->LinkedTrainEntryPtr = MainTrainDataPtr;
/*don't need LinkedTrainEntryPtr for 'OtherTrain' & don't need data transfer as this is done in the
  non-repeating link for Sns-sh & is provided at the outset for Snt-sh
*/
        }
    }

//check repeat information consistent if present
//note that won't be affected by the non-repeating shuttle links as these are in NonRepeatingShuttleLinkHeadCode
//and those not accessed here

//still need to check the non-repeating links and that they have no repeats - do that outside this function
bool MainRepeat = false, OtherRepeat = false;
TActionVectorEntry MainRepeatEntry, OtherRepeatEntry;
if(MainTrainDataPtr->ActionVector.at(MainTrainDataPtr->ActionVector.size() - 1).FormatType == Repeat)
    {
    MainRepeat = true;
    MainRepeatEntry = MainTrainDataPtr->ActionVector.at(MainTrainDataPtr->ActionVector.size() - 1);
    }
if(OtherTrainDataPtr->ActionVector.at(OtherTrainDataPtr->ActionVector.size() - 1).FormatType == Repeat)
    {
    OtherRepeat = true;
    OtherRepeatEntry = OtherTrainDataPtr->ActionVector.at(OtherTrainDataPtr->ActionVector.size() - 1);
    }
if((MainRepeat && !OtherRepeat) || (!MainRepeat && OtherRepeat))
    {
    SecondPassMessage(GiveMessages, "Error in timetable - only one repeat is provided for the train whose headcode is " + MainHeadCode + " and the associated train with headcode " + OtherHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(844);
    return false;
    }
if(MainRepeat && OtherRepeat)
    {
    if((MainRepeatEntry.EventTime != OtherRepeatEntry.EventTime) ||
            (MainRepeatEntry.RearStartOrRepeatMins != OtherRepeatEntry.RearStartOrRepeatMins) ||
            (MainRepeatEntry.FrontStartOrRepeatDigits != OtherRepeatEntry.FrontStartOrRepeatDigits) ||
            (MainRepeatEntry.NumberOfRepeats != OtherRepeatEntry.NumberOfRepeats))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - repeat items don't correspond for the train whose headcode is " + MainHeadCode + " and the associated train with headcode " + OtherHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(845);
        return false;
        }
    }
Utilities->CallLogPop(863);
return true;
}

//---------------------------------------------------------------------------

void TTrainController::StripSpaces(int Caller, AnsiString &Input)
//strip both leading and trailing spaces at ends of text and spaces before and after all commas and semicolons within the text
{
//strip spaces from extreme ends of input
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",StripSpaces," + AnsiString(Input));
if(Input == "")
    {
    Utilities->CallLogPop(856);
    return;
    }
while(Input[1] == ' ')
    {
    if(Input.Length() > 1)
        {
        Input = Input.SubString(2, Input.Length()-1);
        }
    else
        {
        Input = "";
        Utilities->CallLogPop(857);
        return;
        }
    }
if(Input == "")
    {
    Utilities->CallLogPop(858);
    return;
    }
while(Input[Input.Length()] == ' ')
    {
    if(Input.Length() > 1)
        {
        Input = Input.SubString(1, Input.Length()-1);
        }
    else
        {
        Input = "";
        Utilities->CallLogPop(859);
        return;
        }
    }
//now strip spaces immediately after all commas and semicolons within the text
AnsiString Output = "";
bool DelimiterFound = false;
for(int x = 1;x < Input.Length() + 1; x++)
    {
    if(DelimiterFound)
        {
        if(Input[x] == ' ') continue;
        }
    if((Input[x] != ',') && (Input[x] != ';'))
        {
        DelimiterFound = false;
        Output = Output + Input[x];
        }
    else
        {
        DelimiterFound = true;
        Output = Output + Input[x];
        }
    }
if(Output == "")
    {
    Input = "";
    Utilities->CallLogPop(860);
    return;
    }
//now strip spaces immediately before all commas and semicolons within the text
Input = Output;
Output = "";
DelimiterFound = false;
for(int x = Input.Length(); x > 0; x--)
    {
    if(DelimiterFound)
        {
        if(Input[x] == ' ') continue;
        }
    if((Input[x] != ',') && (Input[x] != ';'))
        {
        DelimiterFound = false;
        Output = AnsiString(Input[x]) + Output;
        }
    else
        {
        DelimiterFound = true;
        Output = AnsiString(Input[x]) + Output;
        }
    }
Input = Output;
Utilities->CallLogPop(861);
}

//---------------------------------------------------------------------------

bool TTrainController::IsSNTEntryLocated(int Caller, const TTrainDataEntry &TDEntry, AnsiString &LocationName)
//checks if an Snt or Snt-sh entry followed (somewhere, not necessarily immediately) by a TimeLoc has the same LocationName
//and if so returns true.  Also returns true for Snt, not Snt-sh, if at least 1 start element is a location & the entry is either
//a signaller control entry & speed is zero or it is followed immediately by Frh.  Always return false for entry at a continuation (may
//be named but not a stop location).  Note that no successor validity checks are done in this function, they must be done elsewhere.
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsSNTEntryLocated," + AnsiString(TDEntry.HeadCode));
const TActionVectorEntry &AVEntry0 = TDEntry.ActionVector.at(0);
LocationName = "";
if((AVEntry0.Command != "Snt") && (AVEntry0.Command != "Snt-sh"))
    {
    throw Exception("Error, first entry not 'Snt' or 'Snt-sh' in IsSNTEntryLocated");
    }
if(Track->TrackElementAt(506, AVEntry0.RearStartOrRepeatMins).TrackType == Continuation)
    {
    Utilities->CallLogPop(852);
    return false;
    }
AnsiString LocRear = Track->TrackElementAt(507, AVEntry0.RearStartOrRepeatMins).ActiveTrackElementName;
AnsiString LocFront = Track->TrackElementAt(508, AVEntry0.FrontStartOrRepeatDigits).ActiveTrackElementName;
if(LocRear != "") LocationName = LocRear;
else LocationName = LocFront;
if(LocationName == "")
    {
    Utilities->CallLogPop(1036);
    return false;
    }
if((AVEntry0.SignallerControl) && (TDEntry.StartSpeed == 0))
    {
    Utilities->CallLogPop(1773);
    return true;
    }
else if((AVEntry0.SignallerControl) && (TDEntry.StartSpeed > 0))
    {
    LocationName = "";
    Utilities->CallLogPop(1784);
    return false;
    }

//here if not a signaller start entry so must be at least one more entry
const TActionVectorEntry &AVEntry1 = TDEntry.ActionVector.at(1);//has to be at least 2 AV entries to pass the > 1 comma test in the preliminary check
if((AVEntry1.Command == "Frh") && (AVEntry0.Command == "Snt"))
    {
    Utilities->CallLogPop(1037);
    return true;
    }
AnsiString TimeLocLocationName;
bool FoundFlag = false;
for(unsigned int y=0;y<TDEntry.ActionVector.size();y++)
    {
    const TActionVectorEntry &AVEntry = TDEntry.ActionVector.at(y);
    if(AVEntry.FormatType == TimeLoc)
        {
        FoundFlag = true;
        TimeLocLocationName = AVEntry.LocationName;
        break;
        }
    }
if(!FoundFlag)
    {
    Utilities->CallLogPop(853);
    return false;
    }
if(TimeLocLocationName == LocationName)
    {
    Utilities->CallLogPop(854);
    return true;
    }
Utilities->CallLogPop(855);
return false;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckStartPositionValidity(int Caller, AnsiString RearElementStr, AnsiString FrontElementStr, bool GiveMessages)
{
//checks that the new train start elements are valid - both exist & are connected, and that not
//attempting to start on a diverging leg (i.e. one segment on points & other on element connected to diverging leg)
//& not starting with front on a continuation
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckStartPositionValidity," + RearElementStr + "," + FrontElementStr);
int RearPosition=0, FrontPosition=0, RearExitPos=0;
RearPosition = Track->GetTrackVectorPositionFromString(5, RearElementStr, GiveMessages);
if(RearPosition < 0)//error message given in GetTrackVectorPositionFromString
    {
    Utilities->CallLogPop(759);
    return false;
    }
FrontPosition = Track->GetTrackVectorPositionFromString(6, FrontElementStr, GiveMessages);
if(FrontPosition < 0)//error message given in GetTrackVectorPositionFromString
    {
    Utilities->CallLogPop(760);
    return false;
    }
TTrackElement RearTrackElement = Track->TrackElementAt(490, RearPosition);
TTrackElement FrontTrackElement = Track->TrackElementAt(491, FrontPosition);
TTrackType RearType = RearTrackElement.TrackType, FrontType = FrontTrackElement.TrackType;
//check front & rear connected
for(int x=0;x<4;x++)
    {
    if(RearTrackElement.Conn[x] == FrontPosition)
        {
        RearExitPos = x;
        break;
        }
    if(x == 3)
        {
        TimetableMessage(GiveMessages, "Front element: " + FrontTrackElement.ElementID + " not linked to rear element: " + RearTrackElement.ElementID);
        Utilities->CallLogPop(762);
        return false;
        }
    }
//check not starting with front on a continuation
if(FrontType == Continuation)
    {
    TimetableMessage(GiveMessages, "Front of train attempting to start on a continuation at: " + FrontElementStr);
    Utilities->CallLogPop(937);
    return false;
    }
//check if trying to start on diverging leg of points
if((RearType == Points) && (RearExitPos == 3))
    {
    TimetableMessage(GiveMessages, "Front of train attempting to start on element connected to diverging points at: " + RearElementStr);
    Utilities->CallLogPop(936);
    return false;
    }

if((FrontType == Points) && (RearTrackElement.ConnLinkPos[RearExitPos] == 3))
    {
    TimetableMessage(GiveMessages, "Rear of train attempting to start on element connected to diverging points at: " + FrontElementStr);
    Utilities->CallLogPop(1808);
    return false;
    }
Utilities->CallLogPop(905);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckStartAllowable(int Caller, int RearPosition, int RearExitPos, AnsiString HeadCode, bool ReportFlag, TActionEventType &EventType)
//Rear & front element validity already checked in CheckStartPositionValidity
//This checks for points in correct orientation, no train at start position and not starting on a locked route
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckStartAllowable," + AnsiString(RearPosition) + "," + AnsiString(RearExitPos));
TTrackElement RearTrackElement = Track->TrackElementAt(517, RearPosition);
if(RearTrackElement.TrackType == Continuation) EventType = FailTrainEntry;
else EventType = FailCreateTrain;
int FrontPosition = RearTrackElement.Conn[RearExitPos];
TTrackElement FrontTrackElement = Track->TrackElementAt(798, FrontPosition);
int FrontEntryPos = RearTrackElement.ConnLinkPos[RearExitPos];
TTrackType RearType = RearTrackElement.TrackType;
TTrackType FrontType = FrontTrackElement.TrackType;
AnsiString RearName, FrontName;
if(RearTrackElement.ActiveTrackElementName != "") RearName = RearTrackElement.ActiveTrackElementName;
else RearName = RearTrackElement.ElementID;
if(FrontTrackElement.ActiveTrackElementName != "") FrontName = FrontTrackElement.ActiveTrackElementName;
else FrontName = FrontTrackElement.ElementID;

TPrefDirElement PrefDirElement;//needed for next function but not used
int LockedVectorNumber;//needed for next function but not used
if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(12, FrontPosition, FrontEntryPos, PrefDirElement, LockedVectorNumber))
    {
    if(ReportFlag)
        {
        if(EventType == FailCreateTrain) EventType = FailCreateLockedRoute;
        else EventType = FailEnterLockedRoute;
        LogActionError(47, HeadCode, "", EventType, FrontName);
        }
    Utilities->CallLogPop(940);
    return false;
    }

if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(13, RearPosition, RearExitPos, PrefDirElement, LockedVectorNumber))
    {
    if(ReportFlag)
        {
        if(EventType == FailCreateTrain) EventType = FailCreateLockedRoute;
        else EventType = FailEnterLockedRoute;
        LogActionError(48, HeadCode, "", EventType, RearName);
        }
    Utilities->CallLogPop(1809);
    return false;
    }

if((RearType != Bridge) && (RearTrackElement.TrainIDOnElement > -1))
    {
    if(ReportFlag)
        {
        LogActionError(27, HeadCode, "", EventType, RearName);
        }
    Utilities->CallLogPop(1810);
    return false;
    }
if((FrontType != Bridge) && (FrontTrackElement.TrainIDOnElement > -1))
    {
    if(ReportFlag)
        {
        if(EventType == FailCreateTrain) LogActionError(28, HeadCode, "", EventType, FrontName);
        else LogActionError(43, HeadCode, "", EventType, RearName);
        }
    Utilities->CallLogPop(941);
    return false;
    }
if(RearType == Bridge)
    {
    if((RearExitPos > 1) && (RearTrackElement.TrainIDOnBridgeTrackPos23 > -1))
        {
        if(ReportFlag)
            {
            LogActionError(29, HeadCode, "", EventType, RearName);
            }
        Utilities->CallLogPop(942);
        return false;
        }
    if((RearExitPos < 2) && (RearTrackElement.TrainIDOnBridgeTrackPos01 > -1))
        {
        if(ReportFlag)
            {
            LogActionError(30, HeadCode, "", EventType, RearName);
            }
        Utilities->CallLogPop(943);
        return false;
        }
    }
if(FrontType == Bridge)
    {
    if((FrontEntryPos > 1) && (FrontTrackElement.TrainIDOnBridgeTrackPos23 > -1))
        {
        if(ReportFlag)
            {
            if(EventType == FailCreateTrain) LogActionError(31, HeadCode, "", EventType, FrontName);
            else LogActionError(44, HeadCode, "", EventType, RearName);
            }
        Utilities->CallLogPop(944);
        return false;
        }
    if((FrontEntryPos < 2) && (FrontTrackElement.TrainIDOnBridgeTrackPos01 > -1))
        {
        if(ReportFlag)
            {
            if(EventType == FailCreateTrain) LogActionError(45, HeadCode, "", EventType, FrontName);
            else LogActionError(46, HeadCode, "", EventType, RearName);
            }
        Utilities->CallLogPop(945);
        return false;
        }
    }

EventType = FailCreatePoints;
if(RearType == Points)
    {
    if(RearTrackElement.Attribute == 1)
        {
        if(ReportFlag)
            {
            LogActionError(33, HeadCode, "", FailCreatePoints, RearName);
            }
        Utilities->CallLogPop(933);
        return false;
        }
    }
if(FrontType == Points)
    {
    if(FrontTrackElement.Attribute == 1)
        {
        if(ReportFlag)
            {
            LogActionError(34, HeadCode, "", FailCreatePoints, FrontName);
            }
        Utilities->CallLogPop(934);
        return false;
        }
    }
Utilities->CallLogPop(939);
return true;
}

//---------------------------------------------------------------------------

AnsiString TTrainController::GetRepeatHeadCode(int Caller, AnsiString BaseHeadCode, int RepeatNumber, int IncDigits)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetRepeatHeadCode," + BaseHeadCode + "," + AnsiString(RepeatNumber) + "," + AnsiString(IncDigits));
int BaseDigits = BaseHeadCode.SubString(3,2).ToInt();
int NextRepeatDigits = BaseDigits + (IncDigits * RepeatNumber);
while(NextRepeatDigits >= 100)
    {
    NextRepeatDigits-= 100;//rolls over after 99
    }
AnsiString NextRepeatDigitsStr = AnsiString(NextRepeatDigits);
if(NextRepeatDigitsStr.Length() < 2) NextRepeatDigitsStr = AnsiString('0') + NextRepeatDigitsStr;
AnsiString NextRepeatHeadCode = BaseHeadCode.SubString(1,2) + NextRepeatDigitsStr;
Utilities->CallLogPop(1365);
return NextRepeatHeadCode;
}

//---------------------------------------------------------------------------

TDateTime TTrainController::GetRepeatTime(int Caller, TDateTime BasicTime, int RepeatNumber, int IncMinutes)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetRepeatTime," + AnsiString(double(BasicTime)) + "," + AnsiString(RepeatNumber) + "," + AnsiString(IncMinutes));
TDateTime NextRepeatTime = BasicTime + TDateTime(((double)(RepeatNumber * IncMinutes))/1440.0);//1440 = no. of minutes in 24h
Utilities->CallLogPop(1366);
return NextRepeatTime;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckShuttleRepeatTime(int Caller, TDateTime ForwardEventTime, TDateTime ReverseEventTime, int RepeatMinutes)
//For success the ForwardEventTime + repeat time should == ReverseEventTime (allow 10secs either way since converting to doubles)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckShuttleRepeatTime," + AnsiString(double(ForwardEventTime)) + "," + AnsiString(double(ReverseEventTime)) + "," + AnsiString(RepeatMinutes));
int ForwardSecs = int(double(ForwardEventTime) * 86400);
int ReverseSecs = int(double(ReverseEventTime) * 86400);
int RepeatSecs = RepeatMinutes * 60;
if((ForwardSecs > (ReverseSecs - RepeatSecs + 10)) || (ForwardSecs < (ReverseSecs - RepeatSecs - 10)))
    {
    Utilities->CallLogPop(1367);
    return false;
    }
else
    {
    Utilities->CallLogPop(1368);
    return true;
    }
}

//---------------------------------------------------------------------------

bool TTrainController::CheckNonRepeatingShuttleLinksAndSetData(int Caller, AnsiString MainHeadCode, AnsiString NonRepeatingHeadCode, bool GiveMessages)
//check for proper non-repeating link cross references and that they have no repeats & that times are consistent
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckNonRepeatingShuttleLinksAndSetData," + MainHeadCode + "," + NonRepeatingHeadCode);
int ForwardCount = 0;
int ReverseCount = 0;
unsigned int ForwardTDVectorNumber, ReverseTDVectorNumber;
TActionVectorEntry *ReverseEntryPtr = 0, *ForwardEntryPtr = 0;//Forward corresponds to Main, Reverse to Other
TTrainDataEntry *MainTrainDataPtr = 0;
TTrainDataEntry *OtherTrainDataPtr = 0;

//forward check
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    if(TDEntry.HeadCode == MainHeadCode)
        {
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.NonRepeatingShuttleLinkHeadCode == NonRepeatingHeadCode)
                {
                MainTrainDataPtr = &TrainDataVector.at(x);
                ForwardEntryPtr = &AVEntry;
                ForwardCount++;
                ForwardTDVectorNumber = x;
                }
            }
        }
    }
if(ForwardCount == 0)
//this is an exception because the headcodes are selected in the same order as the forward check
    {
    throw Exception("Error, ForwardCount == 0 in CheckNonRepeatingShuttleLinksAndSetData after called with found values");
    }
if(ForwardCount > 1)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - found more than one reference to " + NonRepeatingHeadCode + " from a train whose headcode is " + MainHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(1061);
    return false;
    }

//reverse check
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    if(TDEntry.HeadCode == NonRepeatingHeadCode)
        {
        for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
            {
            TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
            if(AVEntry.NonRepeatingShuttleLinkHeadCode == MainHeadCode)
                {
                OtherTrainDataPtr = &TrainDataVector.at(x);
                ReverseCount++;
                ReverseEntryPtr = &AVEntry;
                ReverseTDVectorNumber = x;
                }
            }
        }
    }

if(ReverseCount == 0)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - failed to find a reference to " + MainHeadCode + " from a train whose headcode is " + NonRepeatingHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(1062);
    return false;
    }
if(ReverseCount > 1)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - found more than one reference to " + MainHeadCode + " from a train whose headcode is " + NonRepeatingHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(1063);
    return false;
    }

if(((ForwardEntryPtr->Command == "F-nshs") || (ForwardEntryPtr->Command == "Sns-fsh")) && (MainTrainDataPtr->ActionVector.back().FormatType == Repeat))
    {
    SecondPassMessage(GiveMessages, "Error in timetable - shuttle connecting train " + MainHeadCode + " shouldn't have a repeat");
    TrainDataVector.clear();
    Utilities->CallLogPop(1064);
    return false;
    }

if((ForwardEntryPtr->Command != "F-nshs") && (ForwardEntryPtr->Command != "Sns-fsh") && (MainTrainDataPtr->ActionVector.back().FormatType != Repeat))
    {
    SecondPassMessage(GiveMessages, "Error in timetable - shuttle train " + MainHeadCode + " does not have a repeat item");
    TrainDataVector.clear();
    Utilities->CallLogPop(1065);
    return false;
    }

if(ForwardEntryPtr->LocationName == "")
    {
    SecondPassMessage(GiveMessages, "Error in timetable - cross referencing train " + MainHeadCode + ", which references train " + NonRepeatingHeadCode + ", does not have a location set");
    TrainDataVector.clear();
    Utilities->CallLogPop(1066);
    return false;
    }

if(ReverseEntryPtr->LocationName == "")
    {
    SecondPassMessage(GiveMessages, "Error in timetable - cross referenced train " + NonRepeatingHeadCode + ", referenced by " + MainHeadCode + ", does not have a location set");
    TrainDataVector.clear();
    Utilities->CallLogPop(1067);
    return false;
    }

if(ForwardEntryPtr->LocationName != ReverseEntryPtr->LocationName)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - cross referenced train " + NonRepeatingHeadCode + " is at a different location to the referencing train " + MainHeadCode);
    TrainDataVector.clear();
    Utilities->CallLogPop(1068);
    return false;
    }

if(ForwardEntryPtr->Command == "F-nshs")//i.e. the non repeating link into the shuttle service
    {
    if(ForwardEntryPtr->EventTime != ReverseEntryPtr->EventTime)
        {
        SecondPassMessage(GiveMessages, "Error in timetable - shuttle in-link service " + MainHeadCode + " finish time not consistent with start time of shuttle service " + NonRepeatingHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1069);
        return false;
        }
    }

if(ForwardEntryPtr->Command == "Fns-sh")//i.e. the non repeating link out from the shuttle service
    {
    if(!CheckNonRepeatingShuttleLinkTime(0, ForwardEntryPtr->EventTime, ReverseEntryPtr->EventTime, MainTrainDataPtr->ActionVector.back().RearStartOrRepeatMins,
        MainTrainDataPtr->ActionVector.back().NumberOfRepeats))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - service " + NonRepeatingHeadCode + ", which links out from shuttle service " + MainHeadCode + ", has the wrong start time.  It should correspond to the finish time of the last shuttle." );
        TrainDataVector.clear();
        Utilities->CallLogPop(1070);
        return false;
        }
    }

if((ForwardEntryPtr->Command == "F-nshs") || (ForwardEntryPtr->Command == "Sns-fsh"))//i.e. a non repeating link to or from the shuttle service
    {
    if(ReverseTDVectorNumber == ForwardTDVectorNumber)
        {
        SecondPassMessage(GiveMessages, "Error in timetable - the non repeating link service " + NonRepeatingHeadCode + " appears in the same sequence as the corresponding shuttle service " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1071);
        return false;
        }
    }
/* it's allowed to have a different description
if((ForwardEntryPtr->Command == "F-nshs") || (ForwardEntryPtr->Command == "Sns-fsh"))//i.e. a non repeating link to or from the shuttle service
    {
    if((MainTrainDataPtr->Description != "") && (OtherTrainDataPtr->Description != "") && (MainTrainDataPtr->Description != OtherTrainDataPtr->Description))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - the non repeating link service " + NonRepeatingHeadCode + " has a different description to the corresponding shuttle service " + MainHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1072);
        return false;
        }
    }
*/
if(ForwardEntryPtr->Command == "Sns-sh")
    {
    if(ReverseEntryPtr->Command != "F-nshs")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'F-nshs' event for the 'Sns-sh' train whose headcode is " + MainHeadCode + " and is a new shuttle service formed from the service with headcode " + NonRepeatingHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1073);
        return false;
        }
    }

if(ForwardEntryPtr->Command == "F-nshs")
    {
    if(ReverseEntryPtr->Command != "Sns-sh")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'Sns-sh' event for the 'F-nshs' train whose headcode is " + MainHeadCode + " and forms a new shuttle service with headcode " + NonRepeatingHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1074);
        return false;
        }
    else
        {
        ForwardEntryPtr->LinkedTrainEntryPtr = OtherTrainDataPtr;
        ReverseEntryPtr->NonRepeatingShuttleLinkEntryPtr = MainTrainDataPtr;
        if(OtherTrainDataPtr->Description == "") OtherTrainDataPtr->Description = MainTrainDataPtr->Description;
        OtherTrainDataPtr->MaxRunningSpeed = MainTrainDataPtr->MaxRunningSpeed;
        }
    }

if(ForwardEntryPtr->Command == "Sns-fsh")
    {
    if(ReverseEntryPtr->Command != "Fns-sh")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'Fns-sh' event for the 'Sns-fsh' non-shuttle service whose headcode is " + MainHeadCode + " formed from a shuttle service with headcode " + NonRepeatingHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1075);
        return false;
        }
    }

if(ForwardEntryPtr->Command == "Fns-sh")
    {
    if(ReverseEntryPtr->Command != "Sns-fsh")
        {
        SecondPassMessage(GiveMessages, "Error in timetable - unable to find a corresponding 'Sns-fsh' event for the 'Fns-sh' shuttle service whose headcode is " + MainHeadCode + " and forms a new non-shuttle service with headcode " + NonRepeatingHeadCode);
        TrainDataVector.clear();
        Utilities->CallLogPop(1076);
        return false;
        }
    else
        {
        ForwardEntryPtr->NonRepeatingShuttleLinkEntryPtr = OtherTrainDataPtr;//links to the non-repeating non-shuttle linked service
        ReverseEntryPtr->LinkedTrainEntryPtr = MainTrainDataPtr;//needed for creating formatted timetable
        if(OtherTrainDataPtr->Description == "") OtherTrainDataPtr->Description = MainTrainDataPtr->Description;
        OtherTrainDataPtr->MaxRunningSpeed = MainTrainDataPtr->MaxRunningSpeed;
        }
    }

Utilities->CallLogPop(1077);
return true;
}

//---------------------------------------------------------------------------

bool TTrainController::CheckNonRepeatingShuttleLinkTime(int Caller, TDateTime ForwardEventTime, TDateTime ReverseEventTime, int RepeatMinutes, int RepeatNumber)
//Forward train is the finish shuttle entry 'Fns-sh'.
//The Reverse (new non-repeating service) time must == Forward time + (RepeatMins * RepeatNumber) but allow 10 secs either side
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckNonRepeatingShuttleLinkTime," +
        AnsiString(double(ForwardEventTime)) + "," + AnsiString(double(ReverseEventTime)) + "," + AnsiString(RepeatMinutes) + "," +
        AnsiString(RepeatNumber));
int ForwardSecs = int(double(ForwardEventTime) * 86400);
int ReverseSecs = int(double(ReverseEventTime) * 86400);
int RepeatSecs = RepeatMinutes * RepeatNumber * 60;
if((ReverseSecs > (ForwardSecs + RepeatSecs + 10)) || (ReverseSecs < (ForwardSecs + RepeatSecs - 10)))
    {
    Utilities->CallLogPop(1369);
    return false;
    }
else
    {
    Utilities->CallLogPop(1370);
    return true;
    }
}

//---------------------------------------------------------------------------

bool TTrainController::CheckShuttleServiceIntegrity(int Caller, TTrainDataEntry *TDEntryPtr, bool GiveMessages)
//check that each shuttle start ends either in Fns or Fxx-sh (though a single service can't end in Fxx-sh), and that
//when the Fxx-sh is reached it references the original start and not another shuttle - not allowed to link two shuttles,
//don't ever need to and as designed would skip repeats.

//enter with TDEntry a shuttle start - Snt-sh or Sns-sh
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckShuttleServiceIntegrity," + AnsiString(TDEntryPtr->HeadCode));
if(TDEntryPtr->ActionVector.back().FormatType != Repeat)
    {
    throw Exception("Error - last entry in " + TDEntryPtr->HeadCode + " service is not a repeat - should have already found this error");
    }
TTrainDataEntry *ShuttleStartAddress = TDEntryPtr;
AnsiString OriginalHeadCode = TDEntryPtr->HeadCode;
AnsiString LastActionCommand = (TDEntryPtr->ActionVector.end() - 2)->Command;
if((LastActionCommand != "Fns") && (LastActionCommand != "Fns-sh") && (LastActionCommand != "Frh-sh"))
    {
    SecondPassMessage(GiveMessages, "Error in timetable - last event in shuttle service " + TDEntryPtr->HeadCode + " is not 'Fns', 'Fns-sh' or 'Frh-sh'");
    TrainDataVector.clear();
    Utilities->CallLogPop(1091);
    return false;
    }
while(LastActionCommand == "Fns")
    {
    TDEntryPtr = (TDEntryPtr->ActionVector.end() - 2)->LinkedTrainEntryPtr;
    LastActionCommand = (TDEntryPtr->ActionVector.end() - 2)->Command;
    if((LastActionCommand != "Fns") && (LastActionCommand != "Fns-sh") && (LastActionCommand != "Frh-sh"))
        {
        SecondPassMessage(GiveMessages, "Error in timetable - last event in a continuation shuttle service (i.e links back to a shuttle) whose headcode is " + TDEntryPtr->HeadCode + " is not 'Fns', 'Fns-sh' or 'Frh-sh'");
        TrainDataVector.clear();
        Utilities->CallLogPop(1092);
        return false;
        }
    }
//exit the 'while' with LastActionCommand FSH-XX
if((TDEntryPtr->ActionVector.end() - 2)->LinkedTrainEntryPtr != ShuttleStartAddress)
    {
    SecondPassMessage(GiveMessages, "Error in timetable - the event that ends service " + TDEntryPtr->HeadCode + " is a shuttle finish, but it doesn't link back to the start of the original shuttle starting service " + OriginalHeadCode + ".  The linking of two or more shuttles is not permitted.");
    TrainDataVector.clear();
    Utilities->CallLogPop(1093);
    return false;
    }
Utilities->CallLogPop(1094);
return true;
}

//---------------------------------------------------------------------------

void TTrainController::TimetableMessage(bool GiveMessages, AnsiString Message)
{
if(!GiveMessages) return;
if(ServiceReference == "") ShowMessage(Message);
else ShowMessage("Service " + ServiceReference + ": " + Message);
}

//---------------------------------------------------------------------------

void TTrainController::SecondPassMessage(bool GiveMessages, AnsiString Message)
{
if(!GiveMessages) return;
ShowMessage(Message);
}

//$$$$$$$$$$$$$$$$$$$$$$$ End of Timetable Functions $$$$$$$$$$$$$$$$$$$$$$$
//---------------------------------------------------------------------------

void TTrainController::LogActionError(int Caller, AnsiString HeadCode, AnsiString OtherHeadCode, TActionEventType ActionEventType, AnsiString LocationID)
//FailTrainEntry: 06:00:10 HELD: 2F43 can't enter railway, train obstructing entry position 57-N5
//FailCreateTrain: 06:00:10 HELD: 2F43 can't be created, train obstructing start position 57-N5
//FailCreateLockedRoute:  06:00:10 HELD: 2F43 can't be created on a locked route - start position 57-N5
//FailEnterLockedRoute:  06:00:10 HELD: 2F43 can't enter on a locked route - start position 57-N5
//FailCreatePoints: 06:00:10 HELD: 2F43 can't be created, points set to diverge at start position 57-N5
//FailUnexpectedExitRailway: 06:00:10 ERROR: 2F43 left railway unexpectedly at position 57-N5
//FailIncorrectExit: 06:00:10 ERROR: 2F43 left railway at an incorrect exit at position 57-N5
//FailSPAD: 06:00:10 ERROR: 2F43 PASSED SIGNAL AT DANGER at position 57-N5
//FailLockedRoute: 06:00:10 ERROR: SPAD Risk! Signals reset ahead of train, at position 57-N5
//FailLocTooShort: 06:00:10 ERROR: 2F43 failed to split - location too short at Essex Road
//FailSplitDueToOtherTrain: 06:00:10 HELD: 2F43 unable to split - another train is obstructing at Essex Road
//FailCrashed: 06:00:10: ERROR: 2F43 CRASHED INTO 3F43 at position 46-N7
//FailDerailed: 06:00:10: ERROR: 2F43 DERAILED at position 46-N7
//FailUnexpectedBuffers: 06:00:10: ERROR: 2F43 stopped at buffers unexpectedly at position 46-N7
//FailMissedArrival: 06:00:10: ERROR: 2F43 failed to stop at Essex Road;
//FailMissedSplit: 06:00:10: ERROR: 2F43 failed to split at Essex Road
//FailMissedJBO: 06:00:10: ERROR: 2F43 failed to be joined by join other train at Essex Road
//FailMissedJoinOther:  06:00:10: ERROR: 2F43 failed to join other train at Essex Road
//FailMissedTerminate:  06:00:10: ERROR: 2F43 failed to terminate at Essex Road
//FailMissedNewService:  06:00:10: ERROR: 2F43 failed to form new service at Essex Road
//FailMissedExitRailway:  06:00:10: ERROR: 2F43 failed to exit railway
//FailMissedChangeDirection:  06:00:10: ERROR: 2F43 failed to change direction at Essex Road
//FailMissedPass:  06:00:10: ERROR: 2F43 failed to pass Essex Road
//FailBuffersPreventingStart:  06:00:10: ERROR: 2F43 facing buffers and unable to start at Essex Road
//FailBufferCrash:  06:00:10: ERROR: 2F43 CRASHED INTO BUFFERS at 46-N7
//RouteForceCancelled:  06:00:10: ERROR: 2F43 forced a route cancellation by entering from wrong end at 46-N7
//WaitingForJBO:  06:00:10: WARNING: 2F43 waiting to join 3F43 at Essex Road
//WaitingForFJO:  06:00:10: WARNING: 2F43 waiting to be joined by 3F43 at Essex Road
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LogActionError," + HeadCode + "," + OtherHeadCode + "," + AnsiString(ActionEventType) + "," + LocationID);
AnsiString BaseLog="", Prefix="", ErrorLog="", WarningStr="";
Prefix = " ERROR: ";
if(ActionEventType == FailTrainEntry)
    {
    Prefix = " HELD: ";
    ErrorLog = " unable to enter railway, train obstructing ";
    WarningStr = " can't enter, train obstructing ";
    Display->WarningLog(1, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == FailCreateTrain)
    {
    Prefix = " HELD: ";
    ErrorLog = " can't be created, train obstructing ";
    WarningStr = " can't be created, train obstructing ";
    Display->WarningLog(2, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == FailCreateLockedRoute)
    {
    Prefix = " HELD: ";
    ErrorLog = " can't be created on a locked route at ";
    WarningStr = " can't be created on a locked route at ";
    Display->WarningLog(4, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == FailEnterLockedRoute)
    {
    Prefix = " HELD: ";
    ErrorLog = " can't enter on a locked route at ";
    WarningStr = " can't enter on a locked route at ";
    Display->WarningLog(5, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == FailCreatePoints)
    {
    Prefix = " HELD: ";
    ErrorLog = " can't be created, diverging points at ";
    WarningStr = " can't be created, diverging points at ";
    Display->WarningLog(3, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == FailUnexpectedExitRailway)
    {
    ErrorLog = " left railway unexpectedly at ";
    UnexpectedExits++;
    }
else if(ActionEventType == FailIncorrectExit)
    {
    ErrorLog = " left railway at an incorrect exit at ";
    IncorrectExits++;
    }
else if(ActionEventType == FailLocTooShort)
    {
    ErrorLog = " failed to split - location too short at ";
    WarningStr = " failed to split, location too short at ";
    Display->WarningLog(6, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == FailSplitDueToOtherTrain)
    {
    Prefix = " HELD: ";
    ErrorLog = " unable to split - other train obstructing at ";
    WarningStr = " unable to split - other train obstructing at ";
    Display->WarningLog(7, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == FailUnexpectedBuffers) ErrorLog = " stopped at buffers unexpectedly at position ";
else if(ActionEventType == FailMissedArrival)
    {
    ErrorLog = " failed to stop at ";
    MissedStops++;
    }
else if(ActionEventType == FailMissedSplit)
    {
    ErrorLog = " failed to split at ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailMissedJBO)
    {
    ErrorLog = " failed to be joined by other train at ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailMissedJoinOther)
    {
    ErrorLog = " failed to join other train at ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailMissedTerminate)
    {
    ErrorLog = " failed to terminate at ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailMissedNewService)
    {
    ErrorLog = " failed to form new service at ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailMissedExitRailway)
    {
    ErrorLog = " failed to exit railway ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailMissedChangeDirection)
    {
    ErrorLog = " failed to change direction at ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailMissedPass)
    {
    ErrorLog = " failed to pass ";
    OtherMissedEvents++;
    }
else if(ActionEventType == FailBuffersPreventingStart) ErrorLog = " facing buffers and unable to start at ";
else if(ActionEventType == FailDerailed)
    {
    ErrorLog = " DERAILED at position ";
    Prefix = " DERAILMENT: ";
    Derailments++;
    }
else if(ActionEventType == FailBufferCrash)
    {
    ErrorLog = " CRASHED INTO BUFFERS at ";
    Prefix = " CRASH: ";
    CrashedTrains++;
    }
else if(ActionEventType == FailCrashed)
    {
    ErrorLog = "  CRASHED INTO " + OtherHeadCode + " at position ";
    Prefix = " CRASH: ";
    CrashedTrains++;
    CrashedTrains++;
    }
else if(ActionEventType == FailSPAD)
    {
    ErrorLog = " PASSED SIGNAL AT DANGER at position ";
    Prefix = " SPAD: ";
    SPADEvents++;
    }
else if(ActionEventType == FailLockedRoute)
    {
    ErrorLog = " Signals reset ahead of train, at position ";
    Prefix = " SPAD RISK: ";
    SPADRisks++;
    }
else if(ActionEventType == RouteForceCancelled)
    {
    ErrorLog = " forced a route cancellation by entering from wrong end at ";
    }
else if(ActionEventType == WaitingForJBO)
    {
    Prefix = " WARNING: ";
    ErrorLog = " waiting to join " + OtherHeadCode + " at ";
    WarningStr = " waiting to join " + OtherHeadCode + " at ";
    Display->WarningLog(8, HeadCode + WarningStr + LocationID);
    }
else if(ActionEventType == WaitingForFJO)
    {
    Prefix = " WARNING: ";
    ErrorLog = " waiting to be joined by " + OtherHeadCode + " at ";
    WarningStr = " waiting to be joined by " + OtherHeadCode + " at ";
    Display->WarningLog(9, HeadCode + WarningStr + LocationID);
    }
TDateTime ActualTime = TrainController->TTClockTime;
BaseLog = Utilities->Format96HHMMSS(ActualTime) + Prefix + HeadCode;
Display->PerformanceLog(4, BaseLog + ErrorLog + LocationID);
Utilities->CallLogPop(1371);
}

//---------------------------------------------------------------------------

void TTrainController::SaveTrainDataVectorToFile(int Caller)//test
{
/*
TrainDataEntry
    AnsiString HeadCode, Description;//null on creation
    int StartSpeed, MaxRunningSpeed;//both kph
    int RepeatNumber;
    TActionVector ActionVector;
    TTrainOperatingDataVector TrainOperatingDataVector;//no of repeats + 1
    TTrainDataEntry() {StartSpeed=0; MaxRunningSpeed=0; RepeatNumber=0;}

ActionVectorEntry
    TTimetableEntryType FormatType;
    TDateTime EventTime, ArrivalTime, DepartureTime;//zeroed on creation so change to -1 as a marker for 'not set'
    AnsiString LocationName, Command, OtherHeadCode;//null on creation
    TActionVectorEntry *OtherHeadCodeStartingEntryPtr;
    int RearStartOrRepeatMins, FrontStartOrRepeatDigits;
    int RepeatNumber;

TrainOperatingData
    int Mass, MaxBrakeRate, PowerAtRail;//kg;m/s/s;W
    int TrainID;
    TRunningEntry RunningEntry;
    TDateTime StartTime;
    AnsiString HeadCode;
*/
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveTrainDataVectorToFile");
std::ofstream OutFile("TrainData.csv");
if(OutFile == 0)
    {
    ShowMessage("Output file TrainData.csv failed to open");
    Utilities->CallLogPop(1372);
    return;
    }
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    const TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    OutFile << "HeadCode" << ',' << "Description" << ',' << "StartSpeed" << ','
    << "MaxRunningSpeed" << ',' << "NumberOfTrains" << '\n' << '\n';

    OutFile << TDEntry.HeadCode.c_str() << ',' << TDEntry.Description.c_str() << ',' << TDEntry.StartSpeed << ','
    << TDEntry.MaxRunningSpeed << ',' << TDEntry.NumberOfTrains << '\n' << '\n';

    OutFile << ',' << "FormatType" << ',' << "EventTime" << ',' <<
    "ArrivalTime" << ',' << "DepartureTime" << ',' << "LocationName" <<
    ',' << "Command" << ',' << "OtherHeadCode" << ',' << "LinkedTrainEntryPtr" << ',' <<
    "RearStartOrRepeatMins" << ',' << "FrontStartOrRepeatDigits" << ',' << "RepeatNumber" << '\n' << '\n';
    for(unsigned int y=0;y<TrainDataVector.at(x).ActionVector.size();y++)
        {
        TActionVectorEntry &AVEntry = TrainDataVector.at(x).ActionVector.at(y);
        AnsiString TimetableEntryTypeStr;
//NoFormat, TimeLoc, TimeTimeLoc, TimeCmd, StartNew, TimeCmdHeadCode, FinRemHere, FNSNonRepeatToShuttle, SNTShuttle, SNSShuttle, SNSNonRepeatFromShuttle, FSHNewService, Repeat
        switch(AVEntry.FormatType)
            {
            case 0:
                {
                TimetableEntryTypeStr = "NoFormat";
                break;
                }
            case 1:
                {
                TimetableEntryTypeStr = "TimeLoc";
                break;
                }
            case 2:
                {
                TimetableEntryTypeStr = "TimeTimeLoc";
                break;
                }
            case 3:
                {
                TimetableEntryTypeStr = "TimeCmd";
                break;
                }
            case 4:
                {
                TimetableEntryTypeStr = "StartNew";
                break;
                }
            case 5:
                {
                TimetableEntryTypeStr = "TimeCmdHeadCode";
                break;
                }
            case 6:
                {
                TimetableEntryTypeStr = "FinRemHere";
                break;
                }
            case 7:
                {
                TimetableEntryTypeStr = "FNSShuttle";
                break;
                }
            case 8:
                {
                TimetableEntryTypeStr = "SNTShuttle";
                break;
                }
            case 9:
                {
                TimetableEntryTypeStr = "SNSShuttle";
                break;
                }
            case 10:
                {
                TimetableEntryTypeStr = "SNSNonRepeatFromShuttle";
                break;
                }
            case 11:
                {
                TimetableEntryTypeStr = "FSHNewService";
                break;
                }
            case 12:
                {
                TimetableEntryTypeStr = "Repeat";
                break;
                }
            default:
                {
                TimetableEntryTypeStr = "Default";
                break;
                }
            }
        OutFile << ',' << TimetableEntryTypeStr.c_str() << ',' << Utilities->Format96HHMM(AVEntry.EventTime).c_str() << ',' <<
        Utilities->Format96HHMM(AVEntry.ArrivalTime).c_str() << ',' << Utilities->Format96HHMM(AVEntry.DepartureTime).c_str() <<
        ',' << AVEntry.LocationName.c_str() << ',' << AVEntry.Command.c_str() << ',' << AVEntry.OtherHeadCode.c_str() << ',' <<
        AVEntry.LinkedTrainEntryPtr << ',' << AVEntry.RearStartOrRepeatMins << ',' << AVEntry.FrontStartOrRepeatDigits <<
        ',' << AVEntry.NumberOfRepeats << '\n';
        }
    OutFile << '\n';
    OutFile << ',' << ',' << "Mass" << ',' << "MaxBrakeRate" << ',' << "PowerAtRail" << ','
    << "TrainID" << ',' << "RunningEntry" << '\n' << '\n';
    for(unsigned int y=0;y<TrainDataVector.at(x).TrainOperatingDataVector.size();y++)
        {
        TTrainOperatingData TOD = TrainDataVector.at(x).TrainOperatingDataVector.at(y);
        AnsiString RunningEntryStr;
//NotStarted, Running, Exited
        switch(TOD.RunningEntry)
            {
            case 0:
                {
                RunningEntryStr = "NotStarted";
                break;
                }
            case 1:
                {
                RunningEntryStr = "Running";
                break;
                }
            case 2:
                {
                RunningEntryStr = "Exited";
                break;
                }
            }
        OutFile << ',' << ',' << TOD.TrainID << ',' << RunningEntryStr.c_str() << ',' << '\n';
        }
    OutFile << '\n';
    }
OutFile.close();
Utilities->CallLogPop(1373);
}

//---------------------------------------------------------------------------

void TTrainController::StopTTClockMessage(int Caller, AnsiString Message)
//ShowMessage stops everything so this function used where a message is needed when may be in Operating mode.
//The timetable Restart and BaseTimes are reset so the timetable clock stops & restarts when 'OK' button pressed
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",StopTTClockMessage," + Message);
StopTTClockFlag = true;//so TTClock stopped during MasterClockTimer function
RestartTime = TTClockTime;
ShowMessage(Message);
BaseTime = TDateTime::CurrentDateTime();
StopTTClockFlag = false;
Utilities->CallLogPop(1374);
}

//---------------------------------------------------------------------------

void TTrainController::SaveSessionTrains(int Caller, std::ofstream &SessionFile)
//save *TrainDataEntryPtr & *ActionVectorEntryPtr as integer offsets
//from the start of the relevant vectors.  Can't save the pointer values
//as these will be different each time the vectors are created
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveSessionTrains");
Utilities->SaveFileInt(SessionFile, TrainVector.size());
for(unsigned int x=0;x<TrainVector.size();x++)
    {
    TrainVectorAt(55, x).SaveOneSessionTrain(0, SessionFile);
    }
Utilities->CallLogPop(1375);
}

//---------------------------------------------------------------------------

void TTrainController::LoadSessionTrains(int Caller, std::ifstream &SessionFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadSessionTrains");
int NumberOfTrains = Utilities->LoadFileInt(SessionFile);
for(int x=0;x<NumberOfTrains;x++)
    {
    TTrain *NewTrain = new TTrain(1, 0, 0, "", 0, 1, 0, 0, 0, (TTrainMode)0, 0, 0, 0, 0, 0);//have to have >0 for mass, else have divide
                                                                                            //by zero error in calculating AValue, use 1
    NewTrain->LoadOneSessionTrain(0, SessionFile);
    TrainVector.push_back(*NewTrain);
    }
Utilities->CallLogPop(1376);
}

//---------------------------------------------------------------------------

bool TTrainController::CheckSessionTrains(int Caller, std::ifstream &InFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckSessionTrains");
int NumberOfTrains;
if(!Utilities->CheckAndReadFileInt(InFile, 0, 10000, NumberOfTrains))
    {
    Utilities->CallLogPop(1377);
    return false;
    }
for(int x=0;x<NumberOfTrains;x++)
    {
    if(!(TTrain::CheckOneSessionTrain(InFile)))
        {
        Utilities->CallLogPop(1378);
        return false;
        }
    }
Utilities->CallLogPop(1379);
return true;
}

//---------------------------------------------------------------------------

void TTrainController::SaveSessionLockedRoutes(int Caller, std::ofstream &SessionFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveSessionLockedRoutes");
Utilities->SaveFileInt(SessionFile, AllRoutes->LockedRouteVector.size());
for(unsigned int x=0; x<AllRoutes->LockedRouteVector.size(); x++)
    {
    Utilities->SaveFileInt(SessionFile, AllRoutes->LockedRouteVector.at(x).RouteNumber);
    Utilities->SaveFileInt(SessionFile, AllRoutes->LockedRouteVector.at(x).TruncateTrackVectorPosition);
    Utilities->SaveFileInt(SessionFile, AllRoutes->LockedRouteVector.at(x).LastTrackVectorPosition);
    Utilities->SaveFileInt(SessionFile, AllRoutes->LockedRouteVector.at(x).LastXLinkPos);
    Utilities->SaveFileDouble(SessionFile, double(AllRoutes->LockedRouteVector.at(x).LockStartTime));
    }
Utilities->CallLogPop(1380);
}

//---------------------------------------------------------------------------

void TTrainController::LoadSessionLockedRoutes(int Caller, std::ifstream &SessionFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadSessionLockedRoutes");
TAllRoutes::TLockedRouteClass LockedRouteObject;
int LockedRouteVectorSize = Utilities->LoadFileInt(SessionFile);
for(int x=0; x<LockedRouteVectorSize; x++)
    {
    LockedRouteObject.RouteNumber = Utilities->LoadFileInt(SessionFile);
    LockedRouteObject.TruncateTrackVectorPosition = Utilities->LoadFileInt(SessionFile);
    LockedRouteObject.LastTrackVectorPosition = Utilities->LoadFileInt(SessionFile);
    LockedRouteObject.LastXLinkPos = Utilities->LoadFileInt(SessionFile);
    double LockStartTimeDouble = Utilities->LoadFileDouble(SessionFile);
    LockedRouteObject.LockStartTime = TDateTime(LockStartTimeDouble);
    AllRoutes->LockedRouteVector.push_back(LockedRouteObject);
    }
Utilities->CallLogPop(1381);
}

//---------------------------------------------------------------------------

bool TTrainController::CheckSessionLockedRoutes(int Caller, std::ifstream &SessionFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckSessionLockedRoutes");
int LockedRouteVectorSize;
if(!Utilities->CheckAndReadFileInt(SessionFile, 0, 10000, LockedRouteVectorSize))
    {
    Utilities->CallLogPop(1382);
    return false;
    }
for(int x=0; x<LockedRouteVectorSize; x++)
    {
    if(!Utilities->CheckFileInt(SessionFile, 0, 1000000))
    {
    Utilities->CallLogPop(1383);
    return false;
    }
    if(!Utilities->CheckFileInt(SessionFile, 0, 1000000))
    {
    Utilities->CallLogPop(1384);
    return false;
    }
    if(!Utilities->CheckFileInt(SessionFile, 0, 1000000))
    {
    Utilities->CallLogPop(1385);
    return false;
    }
    if(!Utilities->CheckFileInt(SessionFile, 0, 3))
    {
    Utilities->CallLogPop(1386);
    return false;
    }
    if(!Utilities->CheckFileDouble(SessionFile))
    {
    Utilities->CallLogPop(1387);
    return false;
    }
    }
Utilities->CallLogPop(1388);
return true;
}

//---------------------------------------------------------------------------

void TTrainController::SaveSessionContinuationAutoSigEntries(int Caller, std::ofstream &SessionFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveSessionContinuationAutoSigEntries");
Utilities->SaveFileInt(SessionFile, ContinuationAutoSigVector.size());
for(unsigned int x=0; x<ContinuationAutoSigVector.size(); x++)
    {
    Utilities->SaveFileInt(SessionFile, ContinuationAutoSigVector.at(x).RouteNumber);
    Utilities->SaveFileInt(SessionFile, ContinuationAutoSigVector.at(x).AccessNumber);
    Utilities->SaveFileDouble(SessionFile, ContinuationAutoSigVector.at(x).FirstDelay);
    Utilities->SaveFileDouble(SessionFile, ContinuationAutoSigVector.at(x).SecondDelay);
    Utilities->SaveFileDouble(SessionFile, ContinuationAutoSigVector.at(x).ThirdDelay);
    Utilities->SaveFileDouble(SessionFile, double(ContinuationAutoSigVector.at(x).PassoutTime));
    }
Utilities->CallLogPop(1389);
}

//---------------------------------------------------------------------------

void TTrainController::LoadSessionContinuationAutoSigEntries(int Caller, std::ifstream &SessionFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadSessionContinuationAutoSigEntries");
TContinuationAutoSigEntry ContinuationAutoSigObject;
int ContinuationAutoSigVectorSize = Utilities->LoadFileInt(SessionFile);
for(int x=0; x<ContinuationAutoSigVectorSize; x++)
    {
    ContinuationAutoSigObject.RouteNumber = Utilities->LoadFileInt(SessionFile);
    ContinuationAutoSigObject.AccessNumber = Utilities->LoadFileInt(SessionFile);
    ContinuationAutoSigObject.FirstDelay = Utilities->LoadFileDouble(SessionFile);
    ContinuationAutoSigObject.SecondDelay = Utilities->LoadFileDouble(SessionFile);
    ContinuationAutoSigObject.ThirdDelay = Utilities->LoadFileDouble(SessionFile);
    double PassoutTimeDouble = Utilities->LoadFileDouble(SessionFile);
    ContinuationAutoSigObject.PassoutTime = TDateTime(PassoutTimeDouble);
    ContinuationAutoSigVector.push_back(ContinuationAutoSigObject);
    }
Utilities->CallLogPop(1390);
}

//---------------------------------------------------------------------------

bool TTrainController::CheckSessionContinuationAutoSigEntries(int Caller, std::ifstream &SessionFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckSessionContinuationAutoSigEntries");
int ContinuationAutoSigVectorSize;
if(!Utilities->CheckAndReadFileInt(SessionFile, 0, 10000, ContinuationAutoSigVectorSize))
    {
    Utilities->CallLogPop(1391);
    return false;
    }
for(int x=0; x<ContinuationAutoSigVectorSize; x++)
    {
    if(!Utilities->CheckFileInt(SessionFile, 0, 1000000))
        {
        Utilities->CallLogPop(1392);
        return false;
        }
    if(!Utilities->CheckFileInt(SessionFile, 0, 3))
        {
        Utilities->CallLogPop(1393);
        return false;
        }
    if(!Utilities->CheckFileDouble(SessionFile))
        {
        Utilities->CallLogPop(1405);
        return false;
        }
    if(!Utilities->CheckFileDouble(SessionFile))
        {
        Utilities->CallLogPop(1406);
        return false;
        }
    if(!Utilities->CheckFileDouble(SessionFile))
        {
        Utilities->CallLogPop(1407);
        return false;
        }
    if(!Utilities->CheckFileDouble(SessionFile))
        {
        Utilities->CallLogPop(1394);
        return false;
        }
    }
Utilities->CallLogPop(1395);
return true;
}

//---------------------------------------------------------------------------

/*
class TContinuationTrainExpectationEntry//for expected trains at continuation entries
    {
    public:
    int VectorPosition;
    int RepeatNumber;
    TTrainDataEntryPtr *TrainDataEntry;
    AnsiString HeadCode;
    };


typedef std::multimap<TDateTime,TContinuationTrainExpectationEntry> TContinuationTrainExpectationMultiMap;
typedef pair<TDateTime, TContinuationTrainExpectationEntry> TContinuationTrainExpectationMultiMapPair;
*/

void TTrainController::BuildContinuationTrainExpectationMultiMap(int Caller)
//build this into timetable load so session loading can use it too
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",BuildContinuationTrainExpectationMultiMap");
ContinuationTrainExpectationMultiMap.clear();//need to clear as this called twice when load a session
for(unsigned int x=0;x<TrainDataVector.size();x++)
    {
    TTrainDataEntry &TDEntry = TrainDataVector.at(x);
    const TActionVectorEntry &AVFirstEntry = TDEntry.ActionVector.at(0);
    const TActionVectorEntry &AVLastEntry = TDEntry.ActionVector.at(TDEntry.ActionVector.size() - 1);

    if(AVFirstEntry.Command == "Snt")//new train (no need to include Snt-sh since they can't start at a continuation)
        {
        TContinuationTrainExpectationMultiMapPair CTEMMP;
        if(Track->TrackElementAt(665, AVFirstEntry.RearStartOrRepeatMins).TrackType == Continuation)
            {
            TContinuationTrainExpectationEntry CTEEntry;
            CTEEntry.VectorPosition = AVFirstEntry.RearStartOrRepeatMins;//retains this value for all repeats
            CTEEntry.RepeatNumber = 0;//for first entry
            CTEEntry.TrainDataEntryPtr = &TDEntry;//retains this value for all repeats
            CTEEntry.HeadCode = TDEntry.HeadCode;
            CTEEntry.Description = TDEntry.Description;
            CTEMMP.first = AVFirstEntry.EventTime;
            CTEMMP.second = CTEEntry;
            ContinuationTrainExpectationMultiMap.insert(CTEMMP);//base entry
            if(TDEntry.NumberOfTrains > 1)
                {
                if(AVLastEntry.FormatType != Repeat)
                    {
                    throw Exception("Error, Last ActionVectorEntry not a repeat in BuildContinuationTrainExpectationMultiMap");
                    }
                for(int y=1; y<TDEntry.NumberOfTrains; y++)
                    {
                    CTEEntry.RepeatNumber = y;
                    CTEEntry.HeadCode = GetRepeatHeadCode(23, TDEntry.HeadCode, y, AVLastEntry.FrontStartOrRepeatDigits);
                    //CTEEntry.VectorPosition stays same
                    CTEMMP.first = GetRepeatTime(3, AVFirstEntry.EventTime, y, AVLastEntry.RearStartOrRepeatMins);
                    CTEMMP.second = CTEEntry;
                    ContinuationTrainExpectationMultiMap.insert(CTEMMP);
                    }
                }
            }
        }
    }
Utilities->CallLogPop(1396);
}

//---------------------------------------------------------------------------

void TTrainController::PlotAllTrainsInZoomOutMode(int Caller, bool Flash)
{
//called when WarningFlashCount == 0 or when press zoomout button
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotTrainsInZoomOutMode");
if(!Display->ZoomOutFlag)
    {
    Utilities->CallLogPop(1156);
    return;
    }
for(unsigned int x=0;x<TrainVector.size();x++)
    {
//plot blanks & track for all train, even if to be overplotted, since when flashing need to overplot all anyway
//if OldPlotElement[x] == -1 then ignore (not plotted)
    TrainVectorAt(56, x).UnplotTrainInZoomOutMode(1);
    TrainVectorAt(57, x).PlotTrainInZoomOutMode(0, Flash);
    }
Display->Update(); //need to keep this since Update() not called for PlotSmallOutput as too slow
Utilities->CallLogPop(742);
}

//---------------------------------------------------------------------------

TTrain &TTrainController::TrainVectorAt(int Caller, int VecPos)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrainVectorAt," + AnsiString(VecPos));
if((VecPos < 0) || (VecPos >= (int)TrainVector.size()))
    {
    throw Exception("Out of Range Error, vector size: " + AnsiString(TrainVector.size()) + ", VecPos: " + AnsiString(VecPos) + " in TrainVectorAt");
    }
Utilities->CallLogPop(740);
return TrainVector.at(VecPos);
}

//---------------------------------------------------------------------------

void TTrainController::CreateFormattedTimetable(int Caller, AnsiString RailwayTitle, AnsiString TimetableTitle, AnsiString CurDir)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CreateFormattedTimetable");
AnsiString RetStr = "", PartStr = "";

/*
Have description & mass etc for train at top - header, then array of actions

class TActionVectorEntry//contains a single train action - repeat entry is also of this class though no train action is taken for it
    {
    public:
    TTimetableFormatType FormatType;
    TTimetableSequenceType SequenceType;
    TTimetableLocationType LocationType;
    TTimetableShuttleLinkType ShuttleLinkType;
    TDateTime EventTime, ArrivalTime, DepartureTime;//zeroed on creation so change to -1 as a marker for 'not set'
    AnsiString LocationName, Command, OtherHeadCode, NonRepeatingShuttleLinkHeadCode;//null on creation
    TTrainDataEntry *LinkedTrainEntryPtr;
    TTrainDataEntry *NonShuttleEntryPtrForFSHNS;//only used for the non-shuttle train
    int RearStartOrRepeatMins, FrontStartOrRepeatDigits;//for Snt & repeat entries
    int RepeatNumber; //for repeat entries
    TExitList ExitList;//for Fer entries (empty to begin with)

//inline function
    TActionVectorEntry() {RearStartOrRepeatMins=0; FrontStartOrRepeatDigits=0; RepeatNumber=0; FormatType=NoFormat;
            SequenceType=NoSequence; LocationType=NoLocation; ShuttleLinkType=NoShuttleLink, EventTime=TDateTime(-1); ArrivalTime=TDateTime(-1);
            DepartureTime=TDateTime(-1); LinkedTrainEntryPtr=0; NonShuttleEntryPtrForFSHNS=0;}
    };

typedef std::vector<TActionVectorEntry> TActionVector;//contains all actions for a single train

enum TRunningEntry {NotStarted, Running, Exited};//contains status info for each train

class TTrainOperatingData
    {
    public:
    int TrainID;
    TActionEventType EventReported;
    TRunningEntry RunningEntry;

//inline function
    TTrainOperatingData() {TrainID = -1; EventReported= NoEvent; RunningEntry=NotStarted;}//ID -1 = marker for not running
    };

typedef std::vector<TTrainOperatingData> TTrainOperatingDataVector;

class TTrainDataEntry//contains all data for a single train
    {
    public:
    AnsiString HeadCode, Description;//null on creation, headcode is the first train's headcode, rest are calculated from repeat information
    int StartSpeed;//kph
    int Mass;//kg
    double MaxRunningSpeed;//kph
    double MaxBrakeRate, PowerAtRail;//m/s/s;W
    int NumberOfTrains;// number of repeats + 1
    TActionVector ActionVector;
    TTrainOperatingDataVector TrainOperatingDataVector;//no of repeats + 1

//inline function
    TTrainDataEntry() {StartSpeed=0; MaxRunningSpeed=0; NumberOfTrains=0;}
    };

typedef std::vector<TTrainDataEntry> TTrainDataVector;//object is a member of TTrainController & contains the whole timetable

//formatted timetable types
class TOneTrainFormattedEntry
    {
    AnsiString Action;//includes location if relevanr
    AnsiString Time;
    };

typedef std::vector<TOneTrainFormattedEntry> TOneFormattedTrainVector;

class TOneCompleteFormattedTrain//headcode + list of actions
    {
    public:
    AnsiString HeadCode;
    TOneFormattedTrainVector OneFormattedTrainVector;
    };

typedef std::vector<TOneCompleteFormattedTrain> TOneCompleteFormattedTrainVector;//list af all repeats

class TTrainFormattedInformation//contains all information for a single TT entry (including repeats)
    {
    public:
    AnsiString Header;//description, mass, power, brake rate etc
    int NumberOfTrains;// number of repeats + 1
    TOneCompleteFormattedTrainVector OneCompleteFormattedTrainVector;//list af all repeats
    };


typedef std::vector<TTrainFormattedInformation> TAllFormattedTrains;//all timetable in formatted form
//end of formatted timetable types

*/

AnsiString TTFileName = TDateTime::CurrentDateTime().FormatString("dd-mm-yyyy hh.nn.ss");
//format "16/06/2009 20:55:17"
// avoid characters in filename:=   / \ : * ? " < > |
TTFileName = CurDir + "\\Formatted timetables\\Timetable " + TTFileName + "; " + RailwayTitle + "; " + TimetableTitle + ".csv";

AnsiString ShortTTName = "";
for(int x=TTFileName.Length();x>0;x--)
    {
    if(TTFileName[x] == '\\')
        {
        ShortTTName = TTFileName.SubString(x+1, TTFileName.Length() - x - 4);
        break;
        }
    }

ShowMessage("Creates two timetables named " + ShortTTName + " in the 'Formatted timetables' folder, one in service order in '.csv' format, and one in chronological order in '.txt' format");

AnsiString FormatNoDPStr = "#######0";
AnsiString TableTitle="", TimetableTimeStr="", MassStr="", PowerStr="", BrakeStr="", MaxSpeedStr="", FirstHeadCode="", Header="";
TimetableTimeStr = Utilities->Format96HHMM(TrainController->TimetableStartTime);
TableTitle = "Railway: " + RailwayTitle + "; Timetable: " + TimetableTitle + ";  Start time: " + TimetableTimeStr;
TAllFormattedTrains *AllTTTrains = new TAllFormattedTrains;//all timetable in formatted form
for(unsigned int x=0; x<TrainDataVector.size();x++)
    {
    MassStr="", PowerStr="", BrakeStr="", MaxSpeedStr="";
    const TTrainDataEntry &TrainDataEntry = TrainDataVector.at(x);
    if(TrainDataEntry.Mass > 0) MassStr = "; Mass " + AnsiString::FormatFloat(FormatNoDPStr, ((double)TrainDataEntry.Mass)/1000) + "Te; ";
    if(TrainDataEntry.PowerAtRail > 0) PowerStr = "Power " + AnsiString::FormatFloat(FormatNoDPStr, TrainDataEntry.PowerAtRail/1000/0.8) + "kW; ";
    if(TrainDataEntry.MaxBrakeRate > 0) BrakeStr = "Brake force " + AnsiString::FormatFloat(FormatNoDPStr, (TrainDataEntry.MaxBrakeRate*TrainDataEntry.Mass/9810)) + "Te; ";
    if(TrainDataEntry.MaxRunningSpeed > 0) MaxSpeedStr = "Maximum speed " + AnsiString::FormatFloat(FormatNoDPStr,TrainDataEntry.MaxRunningSpeed) + " km/h";
    FirstHeadCode = TrainDataEntry.HeadCode;
    int IncDigits=0, IncMinutes=0;
    const TActionVector &ActionVector = TrainDataEntry.ActionVector;
    if(!ActionVector.empty())
        {
        if(ActionVector.at(ActionVector.size() - 1).FormatType == Repeat)
            {
            IncDigits = ActionVector.at(ActionVector.size() - 1).FrontStartOrRepeatDigits;
            IncMinutes = ActionVector.at(ActionVector.size() - 1).RearStartOrRepeatMins;
            }
        }

    TTrainFormattedInformation OneTTLine;//contains all information for a single TT entry (including repeats)
    for(int y=0; y<TrainDataEntry.NumberOfTrains;y++)
        {
        OneTTLine.Header = "";
        if((TrainDataEntry.Description != "") && (MassStr != "")) OneTTLine.Header = TrainDataEntry.Description + MassStr + PowerStr + BrakeStr + MaxSpeedStr;
        else if(TrainDataEntry.Description != "") OneTTLine.Header = TrainDataEntry.Description;
        OneTTLine.NumberOfTrains = TrainDataEntry.NumberOfTrains;
        TOneCompleteFormattedTrain OneTTTrain;//headcode + list of actions
        for(unsigned int z=0; z<ActionVector.size();z++)
            {
            TOneTrainFormattedEntry OneTTEntry;
            OneTTTrain.HeadCode = GetRepeatHeadCode(24, FirstHeadCode, y, IncDigits);
            TActionVectorEntry ActionVectorEntry = ActionVector.at(z);
            AnsiString PartStr="", TimeStr="";
/*
enum TTimetableFormatType {NoFormat, TimeLoc, TimeTimeLoc, TimeCmd, StartNew, TimeCmdHeadCode, FinRemHere,
        FNSNonRepeatToShuttle, SNTShuttle, SNSShuttle, SNSNonRepeatFromShuttle, FSHNewService, Repeat, PassTime,
        ExitRailway};
enum TTimetableSequenceType {NoSequence, Start, Finish, Intermediate, SequTypeForRepeatEntry};
enum TTimetableLocationType {NoLocation, AtLocation, EnRoute, LocTypeForRepeatEntry};
enum TTimetableShuttleLinkType {NoShuttleLink, NotAShuttleLink, ShuttleLink, ShuttleLinkTypeForRepeatEntry};
*/
            if(ActionVectorEntry.SequenceType == Start)
                {
                if(ActionVectorEntry.FormatType == StartNew)
                    {
                    if(ActionVectorEntry.LocationName != "")
                        {
                        if(Track->TrackElementAt(742, ActionVectorEntry.RearStartOrRepeatMins).TrackType == Continuation)
                            {
                            PartStr = "Enters at " + ActionVectorEntry.LocationName;
                            }
                        else
                            {
                            PartStr = "Created at " + ActionVectorEntry.LocationName;
                            }
                        }
                    else
                        {//may be a named continuation or other element, and if so report that
                        AnsiString LocName = Track->TrackElementAt(739, ActionVectorEntry.RearStartOrRepeatMins).ActiveTrackElementName;
                        if(Track->TrackElementAt(740, ActionVectorEntry.RearStartOrRepeatMins).TrackType == Continuation)
                            {
                            if(LocName != "")
                                {
                                PartStr = "Enters at " + LocName;
                                }
                            else
                                {//use rear position if it's a continuation
                                PartStr = "Enters at " + Track->TrackElementAt(737, ActionVectorEntry.RearStartOrRepeatMins).ElementID;
                                }
                            }
                        else//not a continuation
                            {
                            if(LocName != "")//if not a continuation then LocName should be same as ActionVectorEntry.LocationName
                                             //but include anyway
                                {
                                PartStr = "Created at " + LocName;
                                }
                            else
                                {//use rear position again
                                PartStr = "Created at " + Track->TrackElementAt(741, ActionVectorEntry.RearStartOrRepeatMins).ElementID;
                                }
                            }
                        }
                    TimeStr = Utilities->Format96HHMM(GetRepeatTime(20, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.FormatType == SNTShuttle)
                    {
                    if(y == 0)//first train
                        {
                        PartStr = "Enters at " + ActionVectorEntry.LocationName;
                        TimeStr = Utilities->Format96HHMM(GetRepeatTime(21, ActionVectorEntry.EventTime, y, IncMinutes));
                        }
                    else
                        {
                        PartStr = "Repeat shuttle service at " + ActionVectorEntry.LocationName + " from ";
                        TimeStr = GetRepeatHeadCode(45, ActionVectorEntry.OtherHeadCode, y-1, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(26, ActionVectorEntry.EventTime, y, IncMinutes));
                        }//y-1 for headcode above since it is the last repeat value that the train is from
                    }
                else if(ActionVectorEntry.Command == "Sfs")
                    {
                    PartStr = "New service at " + ActionVectorEntry.LocationName + " split from";
                    TimeStr = GetRepeatHeadCode(33, ActionVectorEntry.OtherHeadCode, y, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(24, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.Command == "Sns")
                    {
                    PartStr = "New service at " + ActionVectorEntry.LocationName + " from";
                    TimeStr = GetRepeatHeadCode(34, ActionVectorEntry.OtherHeadCode, y, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(25, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.FormatType == SNSShuttle)
                    {
                    if(y == 0)//first entry from shuttle
                        {
                        PartStr = "New service at " + ActionVectorEntry.LocationName + " from";
                        TimeStr = ActionVectorEntry.NonRepeatingShuttleLinkHeadCode + " at " + Utilities->Format96HHMM(GetRepeatTime(27, ActionVectorEntry.EventTime, y, IncMinutes));
                        }
                    else
                        {
                        PartStr = "Repeat shuttle service at " + ActionVectorEntry.LocationName + " from ";
                        TimeStr = GetRepeatHeadCode(35, ActionVectorEntry.OtherHeadCode, y-1, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(22, ActionVectorEntry.EventTime, y, IncMinutes));
                        }//y-1 for headcode above since it is the last repeat value that the train is from
                    }
                else if(ActionVectorEntry.FormatType == SNSNonRepeatFromShuttle)
                    {
                    PartStr = "New service at " + ActionVectorEntry.LocationName + " from";
                    //need repeat for the non-repeating headcode as it's the last train of the repeating shuttle
                    TTrainDataEntry *TDE = ActionVectorEntry.LinkedTrainEntryPtr;
                    AnsiString FirstHeadCode = TDE->HeadCode;
                    int LastRepeatNumber = TDE->NumberOfTrains - 1;//a shuttle has to have at least 1 repeat
                    int IncrementalDigits = TDE->ActionVector.at(TDE->ActionVector.size() - 1).FrontStartOrRepeatDigits;
                    TimeStr = GetRepeatHeadCode(36, FirstHeadCode, LastRepeatNumber, IncrementalDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(23, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                }
            else if(ActionVectorEntry.SequenceType == Intermediate)
                {
                if(ActionVectorEntry.FormatType == TimeTimeLoc)
                    {
                    //here need 2 entries if times different so push the first right away & the second later
                    //if times same just give the arrival entry
                    if(ActionVectorEntry.DepartureTime != ActionVectorEntry.ArrivalTime)
                        {
                        PartStr = "Arrives at " + ActionVectorEntry.LocationName;
                        TimeStr = Utilities->Format96HHMM(GetRepeatTime(4, ActionVectorEntry.ArrivalTime, y, IncMinutes));
                        OneTTEntry.Action = PartStr;
                        OneTTEntry.Time = TimeStr;
                        OneTTTrain.OneFormattedTrainVector.push_back(OneTTEntry);
                        PartStr = "Departs from " + ActionVectorEntry.LocationName;
                        TimeStr = Utilities->Format96HHMM(GetRepeatTime(5, ActionVectorEntry.DepartureTime, y, IncMinutes));
                        }
                    else
                        {
                        PartStr = "Arrives & departs " + ActionVectorEntry.LocationName;
                        TimeStr = Utilities->Format96HHMM(GetRepeatTime(29, ActionVectorEntry.ArrivalTime, y, IncMinutes));
                        }
                    }
                else if((ActionVectorEntry.FormatType == TimeLoc) && (ActionVectorEntry.ArrivalTime != TDateTime(-1)))
                    {
                    PartStr = "Arrives at " + ActionVectorEntry.LocationName;
                    TimeStr = Utilities->Format96HHMM(GetRepeatTime(6, ActionVectorEntry.ArrivalTime, y, IncMinutes));
                    }
                else if((ActionVectorEntry.FormatType == TimeLoc) && (ActionVectorEntry.ArrivalTime == TDateTime(-1)))
                    {
                    PartStr = "Departs from " + ActionVectorEntry.LocationName;
                    TimeStr = Utilities->Format96HHMM(GetRepeatTime(7, ActionVectorEntry.DepartureTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.FormatType == PassTime)
                    {
                    PartStr = "Passes " + ActionVectorEntry.LocationName;
                    TimeStr = Utilities->Format96HHMM(GetRepeatTime(8, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.Command == "jbo")
                    {
                    PartStr = "Joined at " + ActionVectorEntry.LocationName + " by";
                    TimeStr = GetRepeatHeadCode(37, ActionVectorEntry.OtherHeadCode, y, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(9, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.Command == "fsp")
                    {
                    PartStr = "Splits from front at " + ActionVectorEntry.LocationName + " to form";
                    TimeStr = GetRepeatHeadCode(38, ActionVectorEntry.OtherHeadCode, y, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(10, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.Command == "rsp")
                    {
                    PartStr = "Splits from rear at " + ActionVectorEntry.LocationName + " to form";
                    TimeStr = GetRepeatHeadCode(39, ActionVectorEntry.OtherHeadCode, y, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(11, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.Command == "cdt")
                    {
                    PartStr = "Changes direction at " + ActionVectorEntry.LocationName;
                    TimeStr = Utilities->Format96HHMM(GetRepeatTime(12, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                }
            else if(ActionVectorEntry.SequenceType == Finish)
                {
                if(ActionVectorEntry.Command == "Fns")
                    {
                    PartStr = "At " + ActionVectorEntry.LocationName + " forms new service";
                    TimeStr = GetRepeatHeadCode(40, ActionVectorEntry.OtherHeadCode, y, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(13, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.Command == "F-nshs")
                    {
                    PartStr = "At " + ActionVectorEntry.LocationName + " forms new service";
                    TimeStr = ActionVectorEntry.NonRepeatingShuttleLinkHeadCode + " at " + Utilities->Format96HHMM(GetRepeatTime(17, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if((ActionVectorEntry.Command == "Fns-sh") && (y < (TrainDataEntry.NumberOfTrains - 1)))
                    {
                    PartStr = "At " + ActionVectorEntry.LocationName + " forms new service ";
                    TimeStr = GetRepeatHeadCode(41, ActionVectorEntry.OtherHeadCode, y+1, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(14, ActionVectorEntry.EventTime, y, IncMinutes));
                    //y+1 because it's the NEXT service repeat number that is relevant
                    }
                else if((ActionVectorEntry.Command == "Fns-sh") && (y >= (TrainDataEntry.NumberOfTrains - 1)))
                    {
                    PartStr = "At " + ActionVectorEntry.LocationName + " forms new service";
                    TimeStr = ActionVectorEntry.NonRepeatingShuttleLinkHeadCode + " at " + Utilities->Format96HHMM(GetRepeatTime(15, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if((ActionVectorEntry.Command == "Frh-sh") && (y < (TrainDataEntry.NumberOfTrains - 1)))
                    {
                    PartStr = "At " + ActionVectorEntry.LocationName + " forms new service";
                    TimeStr = GetRepeatHeadCode(43, ActionVectorEntry.OtherHeadCode, y+1, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(16, ActionVectorEntry.EventTime, y, IncMinutes));
                    //y+1 because it's the NEXT service repeat number that is relevant
                    }
                else if((ActionVectorEntry.Command == "Frh-sh") && (y >= (TrainDataEntry.NumberOfTrains - 1)))
                    {
                    PartStr = "Terminates shuttle service at " + ActionVectorEntry.LocationName; //only used in chronological tt
                    TimeStr = "End at " + Utilities->Format96HHMM(GetRepeatTime(28, ActionVectorEntry.EventTime, y, IncMinutes));
                    //the "End at " is stripped out of the chronological tt but displayed in the traditional tt
                    }
                else if(ActionVectorEntry.Command == "Frh")
                    {
                    PartStr = "Terminates at " + ActionVectorEntry.LocationName;
                    //need here to examine the time of the preceding entry, may be ArrivalTime if TimeLoc, or EventTime otherwise
                    if(z > 0)//should be for finish entry but include check for safety
                        {
                        if(ActionVector.at(z-1).EventTime != TDateTime(-1))
                            {
                            TimeStr = Utilities->Format96HHMM(GetRepeatTime(30, ActionVector.at(z-1).EventTime, y, IncMinutes));
                            }
                        else if(ActionVector.at(z-1).ArrivalTime != TDateTime(-1))
                            {
                            TimeStr = Utilities->Format96HHMM(GetRepeatTime(31, ActionVector.at(z-1).ArrivalTime, y, IncMinutes));
                            }
                        else TimeStr = "     ";//shouldn't ever get here
                        }
                    }
                else if(ActionVectorEntry.Command == "Fer")
                    {
                    PartStr = "Exits railway" + GetExitLocationAndAt(0, ActionVectorEntry.ExitList);
                    TimeStr = Utilities->Format96HHMM(GetRepeatTime(18, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                else if(ActionVectorEntry.Command == "Fjo")
                    {
                    PartStr = "At " + ActionVectorEntry.LocationName + " joins";
                    TimeStr = GetRepeatHeadCode(44, ActionVectorEntry.OtherHeadCode, y, IncDigits) + " at " + Utilities->Format96HHMM(GetRepeatTime(19, ActionVectorEntry.EventTime, y, IncMinutes));
                    }
                }
            else if(ActionVectorEntry.SequenceType == SequTypeForRepeatEntry) continue;//no entry needed for a repeat
            OneTTEntry.Action = PartStr;
            OneTTEntry.Time = TimeStr;
            OneTTTrain.OneFormattedTrainVector.push_back(OneTTEntry);//one per action
            }
        OneTTLine.OneCompleteFormattedTrainVector.push_back(OneTTTrain);//one per repeat
        }
    AllTTTrains->push_back(OneTTLine);//one per repeating train
    }
//AllTTTrains now complete

std::ofstream TTFile(TTFileName.c_str());
if(TTFile == 0)
    {
    StopTTClockMessage(64, "Formatted timetable file failed to open - can't be created");
    delete AllTTTrains;
    Utilities->CallLogPop(1567);
    return;
    }

/*formatted timetable types
class TOneTrainFormattedEntry
    {
    AnsiString Action;//includes location if relevant
    AnsiString Time;
    };

typedef std::vector<TOneTrainFormattedEntry> TOneFormattedTrainVector;

class TOneCompleteFormattedTrain//headcode + list of actions
    {
    public:
    AnsiString HeadCode;
    TOneFormattedTrainVector OneFormattedTrainVector;
    };

typedef std::vector<TOneCompleteFormattedTrain> TOneCompleteFormattedTrainVector;//list af all repeats

class TTrainFormattedInformation//contains all information for a single TT entry (including repeats)
    {
    public:
    AnsiString Header;//description, mass, power, brake rate etc
    int NumberOfTrains;// number of repeats + 1
    TOneCompleteFormattedTrainVector OneCompleteFormattedTrainVector;//list af all repeats
    };

typedef std::vector<TTrainFormattedInformation> TAllFormattedTrains;//all timetable in formatted form
//end of formatted timetable types
*/

//new layout using multiple rows
TTFile << TableTitle.c_str() << '\n' << '\n';
for(unsigned int x=0; x<AllTTTrains->size();x++)
    {
    TTFile << AllTTTrains->at(x).Header.c_str();
    TTFile << '\n';
    TTFile << ',';//for the blank line above the action list
    for(int y=0;y<AllTTTrains->at(x).NumberOfTrains;y++)//number of repeating trains
        {
        if(y < (AllTTTrains->at(x).NumberOfTrains - 1))
            {
            TTFile << AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).HeadCode.c_str() << ',';
            }
        else
            {
            TTFile << AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).HeadCode.c_str();
            }
        }
    TTFile << '\n' << '\n';

    for(unsigned int z = 0; z<AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(0).OneFormattedTrainVector.size(); z++)
        {
        TTFile << AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(0).OneFormattedTrainVector.at(z).Action.c_str() << ',';
        for(int y=0;y<AllTTTrains->at(x).NumberOfTrains;y++)//number of repeating trains
            {
            if(y < (AllTTTrains->at(x).NumberOfTrains - 1))
                {
                TTFile << AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).OneFormattedTrainVector.at(z).Time.c_str() << ',';
                }
            else
                {
                TTFile << AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).OneFormattedTrainVector.at(z).Time.c_str();
                }
            }
        TTFile << '\n';
        }
    TTFile << '\n' << '\n';
    }

TTFile.close();

AnsiString TTFileName2 = TDateTime::CurrentDateTime().FormatString("dd-mm-yyyy hh.nn.ss");
TTFileName2 = CurDir + "\\Formatted timetables\\Timetable " + TTFileName2 + "; " + RailwayTitle + "; " + TimetableTitle + ".txt";

std::ofstream TTFile2(TTFileName2.c_str());
if(TTFile2 == 0)
    {
    StopTTClockMessage(67, "Chronological timetable file failed to open - can't be created");
    delete AllTTTrains;
    Utilities->CallLogPop(1710);
    return;
    }

typedef std::multimap<AnsiString, AnsiString> TAnsiMultiMap;
std::multimap<AnsiString, AnsiString>::iterator AMMIT;
std::pair<AnsiString, AnsiString> AnsiMultiMapEntry;

TAnsiMultiMap *TAMM = new TAnsiMultiMap;//multimap of AnsiStrings with TimeString as key (to sort automatically)

TTFile2 << TableTitle.c_str() << '\n' << '\n';
for(unsigned int x=0; x<AllTTTrains->size();x++)
    {
    for(int y=0;y<AllTTTrains->at(x).NumberOfTrains;y++)//number of repeating trains
        {
        for(unsigned int z = 0; z<AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).OneFormattedTrainVector.size(); z++)
            {
            bool GiveMessagesFalse = false;
            AnsiString TimeString = AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).OneFormattedTrainVector.at(z).Time;
            AnsiString HeadCodeString = AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).HeadCode;
            AnsiString ActionString = AllTTTrains->at(x).OneCompleteFormattedTrainVector.at(y).OneFormattedTrainVector.at(z).Action;
            if(CheckHeadCodeValidity(11, GiveMessagesFalse, TimeString.SubString(1,4)))// 'NXNN at HH:MM'    will return true as integ check passed
                {
                AnsiString OtherHeadCode = TimeString.SubString(1,4);
                TimeString = TimeString.SubString(9,5);
                ActionString+= " " + OtherHeadCode;
                }
            if(TimeString.SubString(1,7) == "End at ")//for Frh-sh final entry
                {
                TimeString = TimeString.SubString(8,5);
                }
            AnsiString OneLine = TimeString + ' ' + HeadCodeString + ' ' + ActionString + '\n';
            AnsiMultiMapEntry.first = TimeString;
            AnsiMultiMapEntry.second = OneLine;
            TAMM->insert(AnsiMultiMapEntry);
            }
        }
    }

for(AMMIT = TAMM->begin(); AMMIT != TAMM->end(); AMMIT++)
    {
    TTFile2 << (AMMIT->second).c_str();
    }

delete AllTTTrains;
delete TAMM;
Utilities->CallLogPop(1580);
TTFile2.close();
}

//---------------------------------------------------------------------------

AnsiString TTrainController::GetExitLocationAndAt(int Caller, TExitList &ExitList) const
{
//check all timetable names in list, if all same return " at [name]" else return ""
if(ExitList.empty()) return "";
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetExitLocationAndAt");
AnsiString StartName = Track->TrackElementAt(735, *(ExitList.begin())).ActiveTrackElementName;
if(StartName == "")
    {
    if(ExitList.size() == 1)
        {
        AnsiString ID = Track->TrackElementAt(738, *(ExitList.begin())).ElementID;
        Utilities->CallLogPop(1571);
        return " at " + ID;
        }
    else
        {
        Utilities->CallLogPop(1572);
        return "";
        }
    }
for(TExitListIterator ELIT=ExitList.begin();ELIT != ExitList.end();ELIT++)
    {
    if(Track->TrackElementAt(736, *ELIT).ActiveTrackElementName != StartName)
        {
        Utilities->CallLogPop(1570);
        return "";
        }
    }
Utilities->CallLogPop(1569);
return " at " + StartName;
}

//---------------------------------------------------------------------------
/*can't trust this as locations within a vector may not be contiguous
bool TTrainController::IsServiceTerminating(int Caller, TTrainDataEntry *TDEPtr, TActionVectorEntry *AVPtr)
{
//Search ActionVector from the position after the entry value for Ptr to the end, and return true if find a Finish
//entry before Fer or TimeLoc.  No point checking for TimeTimeLoc since at a stop location now so a later TimeTimeLoc
//must be preceded by a TimeLoc departure
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsServiceTerminating");
for(unsigned int x=1;x<TDEPtr->ActionVector.size();x++)
    {
    if((AVPtr + x) < TDEPtr->ActionVector.end())
        {
        AnsiString xx = (AVPtr + x)->Command;//test
        TTimetableFormatType xy = (AVPtr + x)->FormatType;//test
        TTimetableSequenceType xz = (AVPtr + x)->SequenceType;//test
        if(((AVPtr + x)->Command == "Fer") || ((AVPtr + x)->FormatType == TimeLoc))
            {
            Utilities->CallLogPop();
            return false;
            }
        else if((AVPtr + x)->SequenceType == Finish)
            {
            Utilities->CallLogPop();
            return true;
            }
        }
    }
Utilities->CallLogPop();
return false;
}
*/
//---------------------------------------------------------------------------

void TTrainController::SendPerformanceSummary(int Caller, std::ofstream &PerfFile)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SendPerformanceSummary");
AnsiString FormatStr = "####0.0";
AnsiString AvLateArrMins = "";
AnsiString AvEarlyArrMins = "";
AnsiString AvLatePassMins = "";
AnsiString AvEarlyPassMins = "";
AnsiString AvLateDepMins = "";
if(LateArrivals > 0) AvLateArrMins = FormatFloat(FormatStr, (TotLateArrMins/LateArrivals));
if(EarlyArrivals > 0) AvEarlyArrMins = FormatFloat(FormatStr, (TotEarlyArrMins/EarlyArrivals));
if(LatePasses > 0) AvLatePassMins = FormatFloat(FormatStr, (TotLatePassMins/LatePasses));
if(EarlyPasses > 0) AvEarlyPassMins = FormatFloat(FormatStr, (TotEarlyPassMins/EarlyPasses));
if(LateDeps > 0) AvLateDepMins = FormatFloat(FormatStr, (TotLateDepMins/LateDeps));
PerfFile << '\n' << '\n' << "***************************************";
PerfFile << '\n' << '\n' << "Performance summary:" << '\n';

if(OnTimeArrivals != 1) PerfFile << OnTimeArrivals << " on-time arrivals" << '\n';
else                    PerfFile << OnTimeArrivals << " on-time arrival"  << '\n';

if(LateArrivals > 1)       PerfFile << LateArrivals << " late arrivals (average " << AvLateArrMins.c_str() << " min)" << '\n';
else if(LateArrivals == 1) PerfFile << LateArrivals << " late arrival ("          << AvLateArrMins.c_str() << " min)" << '\n';
else                       PerfFile << LateArrivals << " late arrivals"                                               << '\n';//0

if(EarlyArrivals > 1)       PerfFile << EarlyArrivals << " early arrivals (average " << AvEarlyArrMins.c_str() << " min)" << '\n';
else if(EarlyArrivals == 1) PerfFile << EarlyArrivals << " early arrival ("          << AvEarlyArrMins.c_str() << " min)" << '\n';
else                        PerfFile << EarlyArrivals << " early arrivals"                                                << '\n';//0

if(OnTimePasses != 1) PerfFile << OnTimePasses << " on-time passes" << '\n';
else                  PerfFile << OnTimePasses << " on-time pass" << '\n';//1

if(LatePasses > 1)       PerfFile << LatePasses << " late passes (average " << AvLatePassMins.c_str() << " min)" << '\n';
else if(LatePasses == 1) PerfFile << LatePasses << " late pass ("           << AvLatePassMins.c_str() << " min)" << '\n';
else                     PerfFile << LatePasses << " late passes" << '\n';//0

if(EarlyPasses > 1)       PerfFile << EarlyPasses << " early passes (average " << AvEarlyPassMins.c_str() << " min)" << '\n';
else if(EarlyPasses == 1) PerfFile << EarlyPasses << " early pass ("           << AvEarlyPassMins.c_str() << " min)" << '\n';
else                      PerfFile << EarlyPasses << " early passes" << '\n';//0

if(OnTimeDeps != 1) PerfFile << OnTimeDeps << " on-time departures" << '\n';
else                PerfFile << OnTimeDeps << " on-time departure"  << '\n';//1

if(LateDeps > 1)      PerfFile << LateDeps << " late departures (average " << AvLateDepMins.c_str() << " min)" << '\n';
else if(LateDeps == 1) PerfFile << LateDeps << " late departure ("          << AvLateDepMins.c_str() << " min)" << '\n';
else                   PerfFile << LateDeps << " late departures"                                               << '\n';//0

if(MissedStops != 1) PerfFile << MissedStops << " missed stops" << '\n';
else                 PerfFile << MissedStops << " missed stop"  << '\n';//1

if(OtherMissedEvents != 1) PerfFile << OtherMissedEvents << " other missed events" << '\n';
else                       PerfFile << OtherMissedEvents << " other missed event"  << '\n';//1

if(UnexpectedExits != 1) PerfFile << UnexpectedExits << " unexpected train exits" << '\n';
else                     PerfFile << UnexpectedExits << " unexpected train exit"  << '\n';//1

if(IncorrectExits != 1) PerfFile << IncorrectExits << " incorrect train exits" << '\n';
else                    PerfFile << IncorrectExits << " incorrect train exit"  << '\n';//1

if(SPADEvents != 1) PerfFile << SPADEvents << " SPADs" << '\n';
else                PerfFile << SPADEvents << " SPAD"  << '\n';//1

if(SPADRisks != 1) PerfFile << SPADRisks << " SPAD risks" << '\n';
else               PerfFile << SPADRisks << " SPAD risk"  << '\n';//1

if(CrashedTrains != 1) PerfFile << CrashedTrains << " crashed trains" << '\n';
else                   PerfFile << CrashedTrains << " crashed train"  << '\n';//1

if(Derailments != 1) PerfFile << Derailments << " derailments" << '\n';
else                 PerfFile << Derailments << " derailment"  << '\n';//1

PerfFile << '\n' << "***************************************";
Utilities->CallLogPop(1736);
}

//---------------------------------------------------------------------------

void TTrainController::SetWarningFlags(int Caller)
{
Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetWarningFlags");
for(unsigned int x = 0; x < TrainVector.size(); x++)
    {
    TTrain &Train = TrainVectorAt(58, x);
    if(Train.Crashed)//can't use background colours for crashed & derailed because same colour
        {
        CrashWarning = true;
        }
    else if(Train.Derailed)//can't use background colours for crashed & derailed because same colour
        {
        DerailWarning = true;
        }
    else if(Train.BackgroundColour == clSPADBackground)//use colour as that changes as soon as passes signal
        {
        SPADWarning = true;
        }
    else if(Train.BackgroundColour == clCallOnBackground)//use colour as also stopped at signal
        {
        CallOnWarning = true;
        }
    else if(Train.BackgroundColour == clSignalStopBackground)//use colour to distinguish from call-on
        {
        SignalStopWarning = true;
        }
    else if(Train.BackgroundColour == clBufferAttentionNeeded)//use colour to distinguish from ordinary buffer stop
        {
        BufferAttentionWarning = true;
        }
    }
Utilities->CallLogPop(1796);
}

//---------------------------------------------------------------------------
//end of TTrainController entries
//---------------------------------------------------------------------------

