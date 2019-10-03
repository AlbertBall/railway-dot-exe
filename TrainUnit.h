//TrainUnit.h
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
//---------------------------------------------------------------------------
#ifndef TrainUnitH
#define TrainUnitH
//---------------------------------------------------------------------------
#include <vector>
#include <map>
#include <vcl.h>
#include <list>

//---------------------------------------------------------------------------

/// Predeclared to allow access, full declaration in DisplayUnit
class TDisplay;

/// Used for reporting error conditions & warnings
enum TActionEventType {NoEvent, FailTrainEntry, FailCreateTrain, FailCreatePoints, FailSPAD, FailLockedRoute,
                       FailLocTooShort, FailSplitDueToOtherTrain, FailCrashed, FailDerailed,
                       FailUnexpectedBuffers, FailUnexpectedExitRailway, FailMissedArrival, FailMissedSplit, FailMissedJBO, FailMissedJoinOther,
                       FailMissedTerminate, FailMissedNewService, FailMissedExitRailway, FailMissedChangeDirection, FailMissedPass,
                       FailCreateLockedRoute, FailEnterLockedRoute, WaitingForJBO, WaitingForFJO, FailBuffersPreventingStart, FailBufferCrash,
                       FailLevelCrossingCrash, FailIncorrectExit, ShuttleFinishedRemainingHere, RouteForceCancelled};

/// Used in LogAction when reporting a train action to the performance log & file
enum TActionType {Arrive, Terminate, Depart, Create, Enter, Leave, FrontSplit, RearSplit, JoinedByOther, ChangeDirection,
                  NewService, TakeSignallerControl, RestoreTimetableControl, RemoveTrain, SignallerMoveForwards,
                  SignallerChangeDirection, SignallerPassRedSignal, Pass, SignallerControlStop, SignallerStop, SignallerLeave, SignallerStepForward};


enum TTrainMode {NoMode, Timetable, Signaller}; ///< indicates train operating mode, 'None' for not in use

/// Timetable entry types
enum TTimetableFormatType {NoFormat, TimeLoc, TimeTimeLoc, TimeCmd, StartNew, TimeCmdHeadCode, FinRemHere,
                           FNSNonRepeatToShuttle, SNTShuttle, SNSShuttle, SNSNonRepeatFromShuttle, FSHNewService, Repeat, PassTime,
                           ExitRailway};
enum TTimetableLocationType {NoLocation, AtLocation, EnRoute, LocTypeForRepeatEntry};
enum TTimetableSequenceType {NoSequence, Start, Finish, Intermediate, SequTypeForRepeatEntry};
enum TTimetableShuttleLinkType {NoShuttleLink, NotAShuttleLink, ShuttleLink, ShuttleLinkTypeForRepeatEntry};
enum TRunningEntry {NotStarted, Running, Exited}; ///< contains status info for each train

//---------------------------------------------------------------------------

class TTrainDataEntry; // predeclared so TActionVectorEntry has access to it, description below

typedef std::list<int> TExitList; ///< a list of valid train exit TrackVector positions for 'Fer' entries
typedef TExitList::iterator TExitListIterator;

//---------------------------------------------------------------------------

/// Contains a single train action in a timetable - repeat entry is also of this class though no train action is taken for it
class TActionVectorEntry
{
public:
    AnsiString LocationName, Command, OtherHeadCode, NonRepeatingShuttleLinkHeadCode; ///< string values for timetabled action entries, null
                                                                                      ///< on creation
    bool SignallerControl; ///< indicates a train that is defined by the timetable as under signaller control
    bool Warning; ///< if set triggers an alert in the warning panel when the action is reached
    int NumberOfRepeats; ///< the number of repeating services
    int RearStartOrRepeatMins, FrontStartOrRepeatDigits; ///< dual-purpose variables used for the TrackVectorPositions of the rear and front
                                                         ///< train starting elements (for Snt) or for repeat minute & digit values in repeat entries
    TDateTime EventTime, ArrivalTime, DepartureTime; ///< relevant times at which the action is timetabled, zeroed on creation so change
                                                     ///< to -1 as a marker for 'not set'
    TExitList ExitList; ///< the list of valid train exit TrackVector positions for 'Fer' entries (empty to begin with)
    TTimetableFormatType FormatType; ///< defines the timetable action type
    TTimetableLocationType LocationType; ///< indicates where the train is when the relevant action occurs
    TTimetableSequenceType SequenceType; ///< indicates where in the sequence of codes the action lies
    TTimetableShuttleLinkType ShuttleLinkType; ///< indicates whether or not the action relates to a shuttle service link
    TTrainDataEntry *LinkedTrainEntryPtr; ///< link pointer for use between fsp/rsp & Sfs; Fjo & jbo; Fns & Sns; & all shuttle to shuttle
                                          ///< links
    TTrainDataEntry *NonRepeatingShuttleLinkEntryPtr; ///< pointer used by shuttles for the non-shuttle train links, in & out, the
                                                      ///< corresponding non-shuttle linked trains use LinkedTrainEntryPtr

// inline function

    /// Constructor, sets all values to default states
    TActionVectorEntry() {
        RearStartOrRepeatMins=0; FrontStartOrRepeatDigits=0; NumberOfRepeats=0; FormatType=NoFormat;
        SequenceType=NoSequence; LocationType=NoLocation; ShuttleLinkType=NoShuttleLink, EventTime=TDateTime(-1);
        ArrivalTime=TDateTime(-1); DepartureTime=TDateTime(-1); LinkedTrainEntryPtr=0; NonRepeatingShuttleLinkEntryPtr=0;
        Warning = false; SignallerControl = false;
    }
};

typedef std::vector<TActionVectorEntry> TActionVector; ///< contains all actions for a single train
typedef TActionVector::iterator TActionVectorIterator; ///< iterator

//---------------------------------------------------------------------------

/// Data for a specific train for use during operation
class TTrainOperatingData
{
public:
    int TrainID;
    TActionEventType EventReported;
    TRunningEntry RunningEntry;

//inline function

    /// Constructor, values set to defaults
    TTrainOperatingData() {
        TrainID = -1; EventReported= NoEvent; RunningEntry=NotStarted;
    }
};

typedef std::vector<TTrainOperatingData> TTrainOperatingDataVector; ///< vector containing operational data for each timetabled train
                                                                    ///< including all repeats

//---------------------------------------------------------------------------

/// Contains all data for a single train
class TTrainDataEntry
{
public:
    AnsiString HeadCode, ServiceReference, Description; ///< headcode is the first train's headcode, rest are calculated from repeat
                                                        ///< information; ServiceReference is the full (up to 8 characters) reference
                                                        ///< from the timetable (added at V0.6b)
    double MaxBrakeRate; ///< in metres/sec/sec
    double MaxRunningSpeed; ///< in km/h
    double PowerAtRail; ///< in Watts (taken as 80% of the train's Gross Power, i.e. that entered by the user)
    int Mass; ///< in kg
    int NumberOfTrains; ///< number of repeats + 1
    int SignallerSpeed; ///< in km/h for use when under signaller control
    int StartSpeed; ///< in km/h
    TActionVector ActionVector; ///< all the actions for the train
    TTrainOperatingDataVector TrainOperatingDataVector; ///< operating information for the train including all its repeats

//inline function

    /// Constructor with default values
    TTrainDataEntry() {
        StartSpeed=0; MaxRunningSpeed=0; NumberOfTrains=0;
    }
};

typedef std::vector<TTrainDataEntry> TTrainDataVector; ///< vector class for containing the whole timetable (the object is a member of
                                                       ///< TTrainController)

//---------------------------------------------------------------------------

//formatted timetable types

/// A single train timetable action for use in a formatted timetable
class TOneTrainFormattedEntry
{
public:
    AnsiString Action; ///< includes location if relevant
    AnsiString Time; ///< the time of the action as a string
};

typedef std::vector<TOneTrainFormattedEntry> TOneFormattedTrainVector; ///< vector of formatted timetable actions for a single train

//---------------------------------------------------------------------------

/// A single train with its headcode + list of actions for use in the formatted timetable
class TOneCompleteFormattedTrain
{
public:
    AnsiString HeadCode;
    TOneFormattedTrainVector OneFormattedTrainVector; ///< see above
};

typedef std::vector<TOneCompleteFormattedTrain> TOneCompleteFormattedTrainVector; ///< vector of a timetabled train with all its repeats for
                                                                                  ///< use in the formatted timetable

//---------------------------------------------------------------------------

/// Contains all information for a single timetable entry for use in the formatted timetable
class TTrainFormattedInformation
{
public:
    AnsiString Header; ///< description, mass, power, brake rate etc
    int NumberOfTrains; ///< number of repeats + 1
    TOneCompleteFormattedTrainVector OneCompleteFormattedTrainVector; ///< see above
};

typedef std::vector<TTrainFormattedInformation> TAllFormattedTrains; ///< vector of all timetabled trains for use in the formatted timetable

//end of formatted timetable types

//---------------------------------------------------------------------------

class TTrainController; //declared here so TTrain can access it as a friend
class TInterface; //declared here so TTrain can access it as a friend

/// Defines the train position with respect to the track elements; three consecutive elements are Lead (front),
/// Mid (middle) and Lag (rear), and a train is either fully on two of the elements (LeadMid or MidLag), or is
/// straddling all three elements (LeadMidLag).  As the train moves forwards the element that was Lead becomes
/// Mid and Mid becomes Lag.
enum TStraddle {MidLag, LeadMidLag, LeadMid};

class TTrain
{
private:

    friend TTrainController;
    friend TInterface;

//start data
    static const int CallOnMaxSpeed = 30; ///< km/h
    static const int MaximumMassLimit = 10000000; ///< kg (i.e. 10,000 tonnes)
    static const int MaximumPowerLimit = 100000000; ///< Watts (i.e. 100MW)
    static const int MaximumSpeedLimit = 400; ///< km/h

    AnsiString HeadCode; ///< needs own HeadCode because repeat entries will differ from TrainDataEntry.HeadCode
    bool HoldAtLocationInTTMode; ///< true if actions are needed before train departs
    bool TimeTimeLocArrived; ///< indicates whether has arrived (true) or not when ActionVectorEntryPtr->FormatType == TimeTimeLoc
    bool RemainHereLogNotSent; ///< flag to prevent repeated logs, new at v1.2.0
    int IncrementalDigits; ///< the number of digits to increment by in repeat entries
    int IncrementalMinutes; ///< the number of minutes to increment by in repeat entries
    int RearStartElement; ///< start TrackVectorPosition element for rear of train
    int RearStartExitPos; ///< the LinkPos value for the rear starting element (i.e. links to the front starting element)
    int RepeatNumber; ///< indicates which of the repeating services this train represents (0 = first service)
    int SignallerMaxSpeed; ///< maximum train speed under signaller control (in km/h)
    int StartSpeed; ///< the speed of the train when introduced into the railway (in km/h)
    TTrainDataEntry *TrainDataEntryPtr; ///< points to the current position in the timetable's TrainDataVector
    TActionVectorEntry *ActionVectorEntryPtr; ///< points to the current position in the ActionVector (a member of the TTrainDataEntry class)

//train data
    bool AllowedToPassRedSignal; ///< set when train has been called on, or when under signaller control and instructed to pass a red signal or
                                 ///< to step forwards one element
    bool BeingCalledOn; ///< in course of being called on to a station
    bool BufferZoomOutFlashRequired; ///< set when train is at buffers and is to flash in zoomout mode (i.e. when reaches buffers unexpectedly -
                                     ///< more actions still being due in the timetable)
    bool CallingOnFlag; ///< calling on permitted
    bool DepartureTimeSet; ///< set when stopped at a location and the next action is departure (set in UpdateTrain when ReleaseTime and TRSTime
                           ///< have been established)
    bool FirstHalfMove; ///< true when the train is on the first half of an element when it displays as fully on two elements.  It only displays
                        ///< with the front character of the headcode on the first half of an element when the halfway point of the element has been passed.
    bool JoinedOtherTrainFlag; ///< true when the train has joined another train following an 'Fjo' timetable command (when set the train is
                               ///< removed from the display at the next clock tick)
    bool LastActionDelayFlag; ///< used when trains join to ensure that there is a 30 second delay before the actual join takes place after the
                              ///< two trains are adjacent to each other
    bool LeavingUnderSigControlAtContinuation; ///< set when the train has reached an exit continuation when under signaller control, used to
    ///< prevent the popup menu being given on right clicking (can cause ambiguities in positioning if try to give signaller commands when at
    ///< or close to a continuation
    bool OneLengthAccelDecel; ///< set when a train can only move forwards one element before stopping but needs to accelerate for the first half
                              ///< of the element
    bool SignallerRemoved; ///< set when removed under signaller control to force a removal from the display at the next clock tick
    bool SignallerStoppingFlag; ///< set when the signaller stop command has been given
    bool StepForwardFlag; ///< set when the signaller command to step forward one element has been given
    bool TerminatedMessageSent; ///< set when a 'train terminated' message has been logged, to prevent its being logged more than once
    bool TimetableFinished; ///< set when there are no more timetable actions
    double AValue; ///< this is a useful shorthand value in calculating speeds and transit times in SetTrainMovementValues
                   //< [= sqrt(2*PowerAtRail/Mass)]

//the following values are used in calculating speeds and transit times in SetTrainMovementValues,
//speeds in km/h & brake rates in m/sec/sec
    double EntrySpeed; ///< speed at which the train enters the next element
    double ExitSpeedHalf; ///< speed when half way into the next element
    double ExitSpeedFull; ///< speed when leaving the next element
    double TimetableMaxRunningSpeed; ///< the maximum train running speed when in timetable mode (see int SignallerMaxSpeed for signaller control)
    double MaxRunningSpeed; ///< the current maximum train running speed
    double MaxExitSpeed; ///< the maximum speed that the train can exit the next element
    double MaxBrakeRate; ///< the maximum brake rate that the train can achieve
    double BrakeRate; ///< the current train brake rate
    double SignallerStopBrakeRate; ///< the train brake rate when stopping under signaller control

    double PowerAtRail; ///< in Watts (taken as 80% of the train's Gross Power, i.e. that entered by the user)
    float MinsDelayed; ///<new at v2.2.0 for operator time to act panel. Calculated in UpdateTrain
    float OpTimeToAct;  ///<in minutes: new at v2.2.0 for operator time to act panel. Calculated in UpdateTrain, -1 indicates not to be displayed on panel
    float FirstLaterStopRecoverableTime; //this used to deduct from RecoverableTime when arrive at a location for OperatorActionpanel
    int FrontElementSpeedLimit, FrontElementLength; ///< values associated with the element immediately in front of the train (speed in km/h, length in m)
    int Mass; ///< in kg
    int UpdateCounter; ///< used in train splitting operations to prevent too frequent checks for a location being long enough for a split
    ///< after a failure message has already been given (doesn't need to stay failed as signaller can manoeuvre it to a better location)
    TDateTime EntryTime, ExitTimeHalf, ExitTimeFull; ///< times used in SetTrainMovementValues corresponding to the next element the train runs on
    TDateTime ReleaseTime, TRSTime; ///< location departure time and 'train ready to start' time (TRSTime is 10 seconds before the ReleaseTime)
    TDateTime LastActionTime; ///< time of the last timetabled action, used to ensure at least a 30 second delay before the next action
    TTrainMode TrainMode; ///< mode of operation - either Timetable (running under timetable control) or Signaller (running under signaller control)

//operating data
    AnsiString RestoreTimetableLocation; ///< stores the location name at which signaller control is taken, to ensure that it is back at that
                                         ///< location before timetable control can be restored
    bool Plotted; ///< set when train plotted on screen
    bool TrainGone; ///< set when train has left the railway, so it can be removed from the display at the next clock tick
    bool SPADFlag; ///< set when running past a red signal without permission
///< flags to indicate relevant stop conditions or pending stop conditions
    bool Derailed, DerailPending, Crashed, StoppedAtBuffers, StoppedAtSignal, StoppedAtLocation,
         SignallerStopped, StoppedAfterSPAD, StoppedForTrainInFront, NotInService;

    Graphics::TBitmap *BackgroundPtr[4]; ///< the existing track graphic that the train headcode segment covers up (one for each headcode segment)
    Graphics::TBitmap *FrontCodePtr; ///< points to the front headcode segment, this is set to red or blue depending on TrainMode
    Graphics::TBitmap *HeadCodeGrPtr[4]; ///< points to the headcode segment graphics e.g. 5,A,4,7.

    int HOffset[4], VOffset[4]; ///< each headcode character is an 8x8 pixel graphic and must be placed within a 16x16 pixel element, these
    ///< values set the horizontal & vertical offsets of the top left hand corner character graphic relative to the 16x16 element
    int OldZoomOutElement[3]; ///< stores the Lead, Mid & Lag TrackVectorPositions, used for unplotting trains from the old position in
    ///< zoomed-out mode prior to replotting in a new position (new will be different from old if train moving)
    int PlotElement[4]; ///< the TrackVectorPosition of the element where each of the 4 headcode characters is plotted
    int PlotEntryPos[4]; ///< the LinkPos value corresponding to the train entry link of the element where each of the 4 headcode characters is
                         ///< plotted
    int TrainCrashedInto; ///< the TrainID of the train that this train has crashed into, recorded so that train can be marked and displayed as
                          ///< crashed also
    TStraddle Straddle; ///< the current Straddle value of the train (see TStraddle above)

//functions defined in .cpp file

/// True if Element is a buffer and Exitpos is the buffer end
    bool BufferAtExit(int Caller, int Element, int Exitpos) const;
/// True if the train can be called on at its current position - see detail in .cpp file
    bool CallingOnAllowed(int Caller);
/// True if Element is a continuation and Exitpos is the continuation end
    bool ContinuationExit(int Caller, int Element, int Exitpos) const;
/// True if train is on a bridge on trackpos 0 & 1
    bool IsTrainIDOnBridgeTrackPos01(int Caller, unsigned int TrackVectorPosition);
/// True if train is on a bridge on trackpos 2 & 3
    bool IsTrainIDOnBridgeTrackPos23(int Caller, unsigned int TrackVectorPosition);
/// True if train service terminates at its current location
    bool IsTrainTerminating(int Caller);
/// Returns true if EntryLink is 1, 2, 4 or 7, in these circumstances the front of the train (i.e.
/// the character that is red or blue) is the last character of the headcode, otherwise it's the first character of the headcode
    bool LowEntryValue(int EntryLink) const;
/// True for a train waiting to be joined when the joining train is adjacent
    bool TrainToBeJoinedByIsAdjacent(int Caller, TTrain* &TrainToBeJoinedBy);
/// True for a train waiting to join another when the other train is adjacent
    bool TrainToJoinIsAdjacent(int Caller, TTrain* &TrainToJoin);
/// Return a pointer to the graphic corresponding to the character 'CodeVhar'
    Graphics::TBitmap *SetOneGraphicCode(char CodeChar);
/// Reverses the direction of motion of the train
    void ChangeTrainDirection(int Caller);
///new v2.2.0 for operator action panel.  Calculates the time left for operator action to avoid unnecessary delays
    float CalcTimeToAct(int Caller);
/// Checks whether Element and EntryPos (where train is
/// about to enter) is on an existing route (or crosses or meets an existing route for crossings and points) that isn't the train's own
/// route and cancels it if so - because it has reached it at the wrong point
    void CheckAndCancelRouteForWrongEndEntry(int Caller, int Element, int EntryPos);
/// Carry out the actions needed when a train is waiting to join another train
    void FinishJoin(int Caller);
/// Carry out the actions needed when a train is to split from the front
    void FrontTrainSplit(int Caller);
/// Called when a train is about to leave an element and move onto another, this function obtains the new
/// element that will become the train's LeadElement, the earlier LeadElement having been assigned to MidElement and earlier MidElement
/// assigned to LagElement.  It assumes Mid & Lag already set, sets LeadElement, LeadEntryPos, LeadExitPos & DerailPending (i.e. about
/// to foul points but train only becomes derailed when it moves fully onto the element)
    void GetLeadElement(int Caller);
/// Sets HOffset & VOffset (see above) for a single headcode character depending on the Link value
    void GetOffsetValues(int Caller, int &HOffset, int &VOffset, int Link) const;
/// Carry out the actions needed when a train is waiting to be joined by another train
    void JoinedBy(int Caller);
/// Carry out the actions needed when a new shuttle service is created from a non-repeating (F-nshs) service
    void NewShuttleFromNonRepeatService(int Caller);
/// Carry out the actions needed when a train forms a new service (code Fns)
    void NewTrainService(int Caller);
/// Store the background bitmap pointer (BackgroundPtr - see above) prior to being overwritten by the train's headcode charcter, so that it
/// can be replotted after the train has passed using PlotBackgroundGraphic.
    void PickUpBackgroundBitmap(int Caller, int HOffset, int VOffset, int Element, int EntryPos, Graphics::TBitmap *GraphicPtr) const;
/// When a train moves off a bridge the other track may contain a route or have a train on it that has been obscured by this train.  This function
/// checks and replots the original graphic if necessary
    void PlotAlternativeTrackRouteGraphic(int Caller, unsigned int LagElement, int LagELinkPos, int HOffset, int VOffset, TStraddle StraddleValue);
/// Replot the graphic pointed to by BackgroundPtr (see above) after a train has passed
    void PlotBackgroundGraphic(int Caller, int ArrayNumber, TDisplay *Disp) const;
/// Plot the train's headcode character corresponding to ArrayNumber
    void PlotTrainGraphic(int Caller, int ArrayNumber, TDisplay *Disp);
/// Changes the train's background colour (e.g. to pale green if stopped at a station)
    void PlotTrainWithNewBackgroundColour(int Caller, TColor NewBackgroundColour, TDisplay *Disp);
/// Carry out the actions needed when a train is to split from the rear
    void RearTrainSplit(int Caller);
/// Sends the 'train terminated' message to the performance log and sets TimetableFinished to true
    void RemainHere(int Caller);
/// Carry out the actions needed to create either a new shuttle service or (if
/// all repeats have finished) a non-repeating shuttle finishing service (code is Fns-sh)
    void RepeatShuttleOrNewNonRepeatService(int Caller);
/// Carry out the actions needed to create either a new shuttle service or (if
/// all repeats have finished) to keep train at its current location (code is Frh-sh)
    void RepeatShuttleOrRemainHere(int Caller);
/// After a train has moved off an element that element
/// has its TrainIDOnElement value set back to -1 to indicate that a train is not present on it, but, if the element is a bridge then
/// the action is more complex because the element's TrainIDOnBridgeTrackPos01 &/or TrainIDOnBridgeTrackPos23 values are involved
    void ResetTrainElementID(int Caller, unsigned int TrackVectorPosition, int EntryPos);
/// Set the four HeadCodeGrPtr[4] pointers to the appropriate character graphics
/// by calling SetOneGraphicCode four times, also check the background colour and reset it to the default colour if necessary
    void SetHeadCodeGraphics(int Caller, AnsiString Code);
/// When a train moves onto an element that element has its TrainIDOnElement value set to the TrainID value to indicate that
/// a train is present on it.  If the element is a bridge then the element's TrainIDOnBridgeTrackPos01 or TrainIDOnBridgeTrackPos23
/// value is also set as appropriate
    void SetTrainElementID(int Caller, unsigned int TrackVectorPosition, int EntryPos);

//public:

    static int NextTrainID; ///< the ID value to be used for the next train that is created, static so that it doesn't need an object to call it
                            ///< and its value is independent of the objects
/// TrackVector positions, & entry & exit connection positions for the elements that the train occupies
    int LeadElement, LeadEntryPos, LeadExitPos,
        MidElement, MidEntryPos, MidExitPos,
        LagElement, LagEntryPos, LagExitPos;
    int TrainID; ///< the train's identification number
/// Set from the HeadCodeGrPtr[4] pointer values, HeadCodePosition[0] is always the front, which may be the first or the last headcode
/// character depending on the direction of motion and the entry link value of the element being entered (see also LowEntryValue(int EntryLink) above)
    Graphics::TBitmap *HeadCodePosition[4];
    TColor BackgroundColour; ///< the background colour of the train's headcode graphics

//inline functions

/// Check whether the train has left the railway, so that it can be removed from the display at the next clock tick
    bool HasTrainGone() {
        return TrainGone;
    }

//functions defined in .cpp file

/// Carries out an integrity check for the train section of a session file, if fails
/// a message is given and the file is not loaded.  It is static so that it can be called without needing an object.
    static bool CheckOneSessionTrain(std::ifstream &InFile);
/// Used in the floating window to display the 'Next' action
    AnsiString FloatingLabelNextString(int Caller, TActionVectorEntry *Ptr);
/// Used in the floating window to display the timetable
    AnsiString FloatingTimetableString(int Caller, TActionVectorEntry *Ptr);
/// Returns the train headcode, taking account of the RepeatNumber
    AnsiString GetTrainHeadCode(int Caller);
/// Indicates that a train is not prevented from moving - used to allow appropriate popup menu options when under signaller control
    bool AbleToMove(int Caller);
/// Indicates that a train is only prevented from moving by a signal - used to allow appropriate popup menu options when under signaller control
    bool AbleToMoveButForSignal(int Caller);
/// Checks forward from train LeadElement, following leading point attributes but ignoring trailing
/// point attributes, until finds either a train or a signal/buffers/continuation/loop.  If finds a train returns false, else returns
/// true.  Used when train stopped for a train in front, to ensure it remains stopped until this function returns true.
    bool ClearToNextSignal(int Caller);
/// True when the train is stopped at a timetabled location
    bool TrainAtLocation(int Caller, AnsiString &LocationName);
/// Returns the number by which the train ActionVectorEntryPtr needs to be incremented to point to the location arrival entry or
/// passtime entry before a change of direction.
///
/// Used to display missed actions when a stop or pass location has been reached before other timetabled actions have been carried out.
/// If can't find it, or Name is "", -1 is returned.  A change of direction is the limit of the search because a train may not stop at a
/// location on the way out but stop on way back, and in these circumstances no actions have been missed.  Stop indicates whether the train
/// will stop at (true) or pass (false) the location.
    int NameInTimetableBeforeCDT(int Caller, AnsiString Name, bool &Stop);
/// Returns the timetable action time corresponding to 'Time' for this train, i.e. it
/// adjusts the time value according to the train's RepeatNumber and the incremental minutes between repeats
    TDateTime GetTrainTime(int Caller, TDateTime Time);
/// This is a housekeeping function to delete train heap objects (bitmaps) explicitly rather than by using a
/// destructor, because vectors erase elements during internal operations & if TTrain had an explicit destructor that deleted the heap
/// elements then it would be called when a vector element was erased.
///
/// Calling the default TTrain destructor doesn't matter because all that does is release the memory of the members (including pointers
/// to the bitmaps), it doesn't destroy the bitmaps themselves. It's important therefore to call this function before erasing the vector
/// element, otherwise the pointers to the bitmaps would be lost and the bitmaps never destroyed, thereby causing memory leaks.
    void DeleteTrain(int Caller);
/// Create one train with relevant member values from the sesion file
    void LoadOneSessionTrain(int Caller, std::ifstream &InFile);
/// Send a message to the performance log and performance file, and if the message
/// is flagged as a warning then it is also sent as a warning (in red at the top of the railway display area)
    void LogAction(int Caller, AnsiString HeadCode, AnsiString OtherHeadCode, TActionType ActionType, AnsiString LocationName,
                   TDateTime TimetableNonRepeatTime, bool Warning);
/// Plots the train and sets up all relevant members for a new train when it is introduced into the railway
    void PlotStartPosition(int Caller);
/// Plots the train on the display in normal (zoomed-in) mode
    void PlotTrain(int Caller, TDisplay *Disp);
/// Plots the train on screen in zoomed-out mode, state of 'Flash' determines whether the flashing trains are plotted or not
    void PlotTrainInZoomOutMode(int Caller, bool Flash);
/// Data for a single train is saved to a session file
    void SaveOneSessionTrain(int Caller, std::ofstream &OutFile);
/// Missed actions (see NameInTimetableBeforeCDT above) sent to the performance log and performance file
    void SendMissedActionLogs(int Caller, int IncNum, TActionVectorEntry *Ptr);
/// Calculates train speeds and times for the element that the train is about to enter
    void SetTrainMovementValues(int Caller, int TrackVectorPosition, int EntryPos);
/// Unplots & replots train, which checks for facing signal and sets StoppedAtSignal if req'd
    void SignallerChangeTrainDirection(int Caller);
/// Unplot train from screen in zoomed-in mode
    void UnplotTrain(int Caller);
/// Unplot train from screen in zoomed-out mode
    void UnplotTrainInZoomOutMode(int Caller);
/// Major function called at each clock tick for each train & handles all train movement & associated actions
    void UpdateTrain(int Caller);
/// Called by TTrainController::WriteTrainsToImage (called by
/// TInterface::SaveOperatingImage1Click) to add all a single train graphic to the image file
    void WriteTrainToImage(int Caller, Graphics::TBitmap *Bitmap);

public:

//inline function

/// True if the train has stopped for any reason
    bool Stopped() {
        return (Crashed || Derailed || StoppedAtBuffers || StoppedAtSignal || StoppedAtLocation ||
                SignallerStopped || StoppedAfterSPAD || StoppedForTrainInFront || NotInService);
    }
/// Added at v1.2.0: true if any part of train on specific link, false otherwise, including
/// no link present & no TrackVectorNumber within Lead, Mid or Lag (public so Track->TrainOnLink can access it)
    bool LinkOccupied(int Caller, int TrackVectorPosition, int LinkNumber);
/// Constructor, sets listed member values
    TTrain(int Caller, int RearStartElementIn, int RearStartExitPosIn, AnsiString InputCode, int StartSpeed, int Mass, double MaxRunningSpeed,
           double MaxBrakeRate, double PowerAtRail, TTrainMode TrainMode, TTrainDataEntry *TrainDataEntryPtr, int RepeatNumber,
           int IncrementalMinutes, int IncrementalDigits, int SignallerMaxSpeed);
};


//---------------------------------------------------------------------------

/// Handles all train and timetable activities, only one object created
class TTrainController
{
public:

    TDateTime BaseTime; ///< CurrentDateTime (i.e. real time) when operation restarts after a pause
    TDateTime TTClockTime; ///< the time indicated by the timetable clock
    TDateTime TimetableStartTime; ///< the start time of the current timetable
    TDateTime RestartTime; ///< TTClockTime when operation pauses ( = timetable start time prior to operation)
                           ///< TTClockTime is calculated as follows:- TTClockTime = CurrentDateTime + RestartTime - BaseTime;

/// Turns signals back to green in stages after a train has exited an autosig route at a continuation
    class TContinuationAutoSigEntry
    {
public:
        /// Delays in seconds before consecutive signal changes - these correspond to the times
        /// taken for trains to pass subsequent signals outside the boundaries of the railway.  After the third delay the signal nearest to
        /// the continuation that was red when the train passed it has changed to green
        double FirstDelay, SecondDelay, ThirdDelay;
        int AccessNumber; ///< the number of times the signal changing function has been accessed - starts at 0 and increments after each change
        int RouteNumber; ///< the AllRoutesVector position of the route that the continuation is in
        TDateTime PassoutTime; ///< the timetable clock time at which the train exits from the continuation
    };

    typedef std::vector<TContinuationAutoSigEntry> TContinuationAutoSigVector; ///< vector class for TContinuationAutoSigEntry objects
    typedef TContinuationAutoSigVector::iterator TContinuationAutoSigVectorIterator;

/// Class that stores data for trains expected at continuation entries (kept in a multimap -
/// see below), used to display information in the floating window when mouse hovers over a continuation
    class TContinuationTrainExpectationEntry
    {
public:
        AnsiString Description; ///< service description
        AnsiString HeadCode; ///< service headcode
        int RepeatNumber; ///< service RepeatNumber
        int IncrementalMinutes; ///< Repeat separation in minutes
        int IncrementalDigits; ///< Repeat headcode separation
        int VectorPosition; ///< TrackVectorPosition for the continuation element
        TTrainDataEntry *TrainDataEntryPtr; ///< points to the service entry in the timetable's TrainDataVector
    };

/// Multimap class for TContinuationTrainExpectationEntry objects, where the access key is the expectation time
    typedef std::multimap<TDateTime, TContinuationTrainExpectationEntry> TContinuationTrainExpectationMultiMap;
    typedef TContinuationTrainExpectationMultiMap::iterator TContinuationTrainExpectationMultiMapIterator; ///< iterator for the multimap
    typedef std::pair<TDateTime, TContinuationTrainExpectationEntry> TContinuationTrainExpectationMultiMapPair; ///< a single multimap entry

    typedef std::vector<TTrain> TTrainVector; ///< vector containing all trains that currently exist in the railway

///< these added v2.2.0, for OpTimeToActPanel
    typedef std::pair<AnsiString, int> THCandTrainPosParam; //Headcode + TrainID for running trains & -TrackVectorPosition - 1 for continuation
        //entries neg values for continuation track vector & -1 as could be 0 so all negative.  Was simply the trackvector (TV) position, but
        //since OA panel only rebuilt every 2 secs when mouseup on panel the train could be well past the TV position stored, especially at
        //16x operating speed
    typedef std::multimap<float, THCandTrainPosParam> TOpTimeToActMultiMap;
    typedef std::pair<float, THCandTrainPosParam> TOpTimeToActMultiMapEntry;
    typedef TOpTimeToActMultiMap::iterator TOpTimeToActMultiMapIterator;
    typedef std::vector<int> TContinuationEntryVecPosVector; ///<ensures only one train displayed for a given continuation


    /// String used to display the offending service in timetable error messages
/// bool AnyHeadCodeValid; //flag to indicate valid headcode types for a service - when true can accept xxNN; if false accept only NaNN
/// (N=number, x = any alphanumeric, a= upper or lower case letter) - dropped at v0.6b
    AnsiString ServiceReference;
/// Flags to enable the relevant warning graphics to flash at the left hand side of the screen
    bool CrashWarning, DerailWarning, SPADWarning, CallOnWarning, SignalStopWarning, BufferAttentionWarning;
    bool StopTTClockFlag; ///< when true the timetable clock is stopped, used for messages display and train popup menu display etc
    bool TrainAdded; ///< true when a train has been added by a split (occurs outside the normal train introduction process)
    bool SignallerTrainRemovedOnAutoSigsRoute; ///< true if train was on an AutoSigsRoute when removed by the signaller
    bool OpActionPanelVisible; ///<new v2.2.0 flag to prevent time to act functions when not visible

    float NotStartedTrainLateMins; ///< total late minutes of trains that haven't started yet on exit operation for locations not reached yet
    float OperatingTrainLateMins; ///< total late minutes of operating trains on exit operation for locations not reached yet
    float ExcessLCDownMins; ///< total excess time in minutes over the 3 minutes barriers down allowance for level crossings
    float TotEarlyArrMins; ///< values for performance file summary
    float TotEarlyPassMins;
    float TotLateArrMins;
    float TotLateDepMins;
    float TotLatePassMins;

//values for performance file summary
    int CrashedTrains;
    int Derailments;
    int EarlyArrivals;
    int EarlyPasses;
    int IncorrectExits;
    int LateArrivals;
    int LateDeps;
    int LatePasses;
    int MissedStops;
    int OnTimeArrivals;
    int OnTimeDeps;
    int OnTimePasses;
    unsigned int OpTimeToActUpdateCounter; ///<new v2.2.0, incremented in Interface.cpp, controls updating for OpTimeToActPanel
    unsigned int OpActionPanelHintDelayCounter; ///<new v2.2.0 on start operation delays the op action panle headcode display for about 5 secs while hints shown
    int OtherMissedEvents;                 ///< unsigned as might initialise at any value & need it to be positive
    int SPADEvents;
    int SPADRisks;
    int TotArrDepPass;
    int UnexpectedExits;
    int OperatingTrainArrDep; ///< total number of arrivals & departures for operating trains locations not reached yet
    int NotStartedTrainArrDep; ///< total number of arrivals & departures for trains that haven't started yet for locations not reached yet

    TContinuationAutoSigVector ContinuationAutoSigVector; ///< vector for TContinuationAutoSigEntry objects
/// Multimap for TContinuationTrainExpectationEntry objects, the access key is the expectation time
    TContinuationTrainExpectationMultiMap ContinuationTrainExpectationMultiMap;
    TOpTimeToActMultiMap OpTimeToActMultiMap; ///<added v2.2.0 for Op time to act display
    TOpTimeToActMultiMapIterator OpTimeToActMultiMapIterator; ///<added v2.2.0 for Op time to act display
    TTrainDataVector TrainDataVector; ///< vector containing the internal timetable
    TTrainVector TrainVector; ///< vector containing all trains currently in the railway

    //functions defined in .cpp file

/// Build string for use in floating window for expected trains at continuations
    AnsiString ContinuationEntryFloatingTTString(int Caller, TTrainDataEntry *TTDEPtr, int RepeatNumber, int IncrementalMinutes, int IncrementalDigits);
/// Check all timetable names in ExitList, if all same return " at [name]" else
/// return "".  Used in floating label for Next action and in formatted timetables.
    AnsiString GetExitLocationAndAt(int Caller, TExitList &ExitList) const;
/// Return the service headcode for the repeat service
    AnsiString GetRepeatHeadCode(int Caller, AnsiString BaseHeadCode, int RepeatNumber, int IncDigits);
/// Introduce a new train to the railway, with the characteristics specified, returns true for success
    bool AddTrain(int Caller, int RearPosition, int FrontPosition, AnsiString HeadCode, int StartSpeed, int Mass, double MaxRunningSpeed,
                  double MaxBrakeRate, double PowerAtRail, AnsiString ModeStr, TTrainDataEntry *TrainDataEntryPtr, int RepeatNumber,
                  int IncrementalMinutes, int IncrementalDigits, int SignallerSpeed, bool SignallerControl);
/// A shorthand function that returns true if the successor to a given timetable
/// action command should be (or could be) an 'at location' command, used in timetable validation
    bool AtLocSuccessor(const TActionVectorEntry &AVEntry);
/// Used to compile ExitList from a string list of element IDs, returns true for success or gives a message & returns false for failure,
/// used in timetable validation
    bool CheckAndPopulateListOfIDs(int Caller, AnsiString IDSet, TExitList &ExitList, bool GiveMessages);
/// A timetable validation function where all service cross references are checked for validity and set pointers and train information,
/// return true for success
    bool CheckCrossReferencesAndSetData(int Caller, AnsiString SoughtHeadCode, AnsiString SeekingHeadCode, bool Shuttle, bool GiveMessages);
/// A timetable validation function where referenced services are checked for uniqueness, returns true for success
    bool CheckForDuplicateCrossReferences(int Caller, AnsiString MainHeadCode, AnsiString SecondHeadCode, bool GiveMessages);
/// Returns true if the headcode complies with requirements
    bool CheckHeadCodeValidity(int Caller, bool GiveMessages, AnsiString HeadCode);
/// Returns true if the location name complies with requirements
    bool CheckLocationValidity(int Caller, AnsiString LocStr, bool GiveMessages, bool CheckLocationsExistInRailway);
/// A timetable validation function where cross references are checked for validity for non-repeating shuttle links, pointers and train
/// information are set, returns true for success
    bool CheckNonRepeatingShuttleLinksAndSetData(int Caller, AnsiString MainHeadCode, AnsiString NonRepeatingHeadCode, bool GiveMessages);
/// The forward train is the finish shuttle entry 'Fns-sh', the reverse (new non-repeating service) time must = Forward time +
/// (RepeatMins * RepeatNumber), return true for success
    bool CheckNonRepeatingShuttleLinkTime(int Caller, TDateTime ReverseEventTime, TDateTime ForwardEventTime, int RepeatMins, int RepeatNumber);
/// Part of the session file integrity check for ContinuationAutoSigEntries, true for success
    bool CheckSessionContinuationAutoSigEntries(int Caller, std::ifstream &SessionFile);
/// Part of the session file integrity check for locked routes, true for success
    bool CheckSessionLockedRoutes(int Caller, std::ifstream &SessionFile);
/// Part of the session file integrity check for train entries, true for success
    bool CheckSessionTrains(int Caller, std::ifstream &InFile);
/// Check that shuttle link services have consistent times, true for success
    bool CheckShuttleRepeatTime(int Caller, TDateTime ForwardEventTime, TDateTime ReverseEventTime, int RepeatMinutes);
/// Check that each shuttle service ends either in Fns or Fxx-sh (though a single service can't end in Fxx-sh), and that when the Fxx-sh
/// is reached it references the original start and not another shuttle - not allowed to link two shuttles, true for success
    bool CheckShuttleServiceIntegrity(int Caller, TTrainDataEntry *TDEntryPtr, bool GiveMessages);
/// Called when trying to introduce a new train - checks for points in correct orientation, that no train is already at the train
/// starting position, and not starting on a locked route
    bool CheckStartAllowable(int Caller, int RearPosition, int RearExitPos, AnsiString HeadCode, bool ReportFlag, TActionEventType &EventType);
/// A timetable validation function where train starting positions are checked for validity, true for success
    bool CheckStartPositionValidity(int Caller, AnsiString RearElementStr, AnsiString FrontElementStr, bool GiveMessages);
    bool CheckTimeValidity(int Caller, AnsiString TimeStr, TDateTime &Time); ///< returns true if the time complies with requirements
/// New trains introduced with 'Snt' may be at a timetabled location or elsewhere.
/// This function checks and returns true for at a timetabled location.
    bool IsSNTEntryLocated(int Caller, const TTrainDataEntry &TDEntry, AnsiString &LocationName);
/// Checks the last two characters in HeadCode and returns true if both are digits
    bool Last2CharactersBothDigits(int Caller, AnsiString HeadCode);
/// A shorthand function that returns true if the successor to a given timetable
/// action command should be (or could be) an en-route command, used in timetable validation
    bool MovingSuccessor(const TActionVectorEntry &AVEntry);
/// Carry out preliminary (mainly syntax) validity checks on a single timetable service entry and
/// (if FinalCall true) set the internal timetable data values, return true for success
    bool ProcessOneTimetableLine(int Caller, int Count, AnsiString OneLine, bool &EndOfFile, bool FinalCall, bool GiveMessages,
                                 bool CheckLocationsExistInRailway);
/// Carry out further detailed timetable consistency checks, return true for success
    bool SecondPassActions(int Caller, bool GiveMessages);
/// Parse a single timetable service action, return true for success
    bool SplitEntry(int Caller, AnsiString OneEntry, bool GiveMessages, bool CheckLocationsExistInRailway, AnsiString &First,
                    AnsiString &Second, AnsiString &Third, AnsiString &Fourth, int &RearStartOrRepeatMins, int &FrontStartPosition,
                    TTimetableFormatType &TimetableFormatType, TTimetableLocationType &LocationType, TTimetableSequenceType &SequenceType,
                    TTimetableShuttleLinkType &ShuttleLinkType, TExitList &ExitList, bool &Warning);
/// Parse a timetable repeat entry, return true for success
    bool SplitRepeat(int Caller, AnsiString OneEntry, int &RearStartOrRepeatMins, int &FrontStartOrRepeatDigits, int &RepeatNumber,
                     bool GiveMessages);
/// Parse a train information entry, return true for success
    bool SplitTrainInfo(int Caller, AnsiString TrainInfoStr, AnsiString &HeadCode, AnsiString &Description, int &StartSpeed,
                        int &MaxRunningSpeed, int &Mass, double &MaxBrakeRate, int &PowerAtRail, int &SignallerSpeed, bool GiveMessages);
/// Checks overall timetable integrity, calls many other specific checking functions, returns true for success
    bool TimetableIntegrityCheck(int Caller, char *FileName, bool GiveMessages, bool CheckLocationsExistInRailway);
/// new v2.2.0, calcs distance to red signal, returns -1 for no signal found, for autosigs route after next red signal & other conditions,
/// also totals up location stop times before the red signal and returns value in StopTime
    int CalcDistanceToRedSignalandStopTime(int Caller, int TrackVectorPosition, int TrackVectorPositionEntryPos,
                                           bool SigControlAndCanPassRedSignal, TActionVectorEntry *AVPtr, AnsiString HeadCode, int TrainID,
                                           float &CurrentStopTime, float &LaterStopTime, float &RecoverableTime, int &AvTrackSpeed);
/// Return the track entry link (Link[]) array position for the given train on track element at track vector position TrackVectorNumber
    int EntryPos(int Caller, int TrainIDIn, int TrackVectorNumber);
/// Get the interval betwqeen repeats
    TDateTime GetControllerTrainTime(int Caller, TDateTime Time, int RepeatNumber, int IncrementalMinutes);
/// Return the repeating service time
    TDateTime TTrainController::GetRepeatTime(int Caller, TDateTime BasicTime, int RepeatNumber, int IncMinutes);
/// Return a reference to the train at position VecPos in the TrainVector, carries out range checking on VecPos
    TTrain &TrainVectorAt(int Caller, int VecPos);
/// Return a reference to the train with ID TrainID, carries out validity checking on TrainID
    TTrain &TrainVectorAtIdent(int Caller, int TrainID);
    void BuildContinuationTrainExpectationMultiMap(int Caller); ///< populate the ContinuationTrainExpectationMultiMap during timetable loading
    void CalcOperatingAndNotStartedTrainLateness(int Caller); ///< calculates additional lateness values for trains that haven't reached their destinations yet
/// Examines the internal timetable (TrainDataVector) and creates from it a chronological (.txt) timetable and also a traditional
/// spreadsheet format (.csv) timetable
    void CreateFormattedTimetable(int Caller, AnsiString RailwayTitle, AnsiString TimetableTitle, AnsiString CurDir);
    void FinishedOperation(int Caller); ///< called when exiting operation mode to delete all trains and timetable data etc
    void LoadSessionContinuationAutoSigEntries(int Caller, std::ifstream &SessionFile); ///< load ContinuationAutoSigEntries from a session file
    void LoadSessionLockedRoutes(int Caller, std::ifstream &SessionFile); ///< load locked routes from a session file
    void LoadSessionTrains(int Caller, std::ifstream &SessionFile); ///< load trains from a session file
/// Send an error message to the performance log and file, and as a warning if appropriate
    void LogActionError(int Caller, AnsiString HeadCode, AnsiString OtherHeadCode, TActionEventType ActionEventType, AnsiString LocationID);
    void LogEvent(AnsiString Str); ///< store Str to the event log - moved from TUtilities for v0.6 so can record the tt clock value
    void Operate(int Caller); ///< called every clock tick to introduce new trains and update existing trains
/// Plots all trains on screen in zoomed-out mode, state of 'Flash' determines whether the flashing trains are plotted or not
    void PlotAllTrainsInZoomOutMode(int Caller, bool Flash);
    void RebuildOpTimeToActMultimap(int Caller); //new v2.2.0 for OperatorActionPanel
    void ReplotTrains(int Caller, TDisplay *Disp); ///< plot all trains on the display
    void SaveSessionContinuationAutoSigEntries(int Caller, std::ofstream &SessionFile); ///< save ContinuationAutoSigEntries to a session file
    void SaveSessionLockedRoutes(int Caller, std::ofstream &SessionFile); ///< save locked routes to a session file
    void SaveSessionTrains(int Caller, std::ofstream &SessionFile); ///< save trains to a session file
    void SaveTrainDataVectorToFile(int Caller); ///< diagnostic function to store all train data to a file for examination, not used normally
/// Give a user message during timetable integrity checking if GiveMessages is true, ignore if false
    void SecondPassMessage(bool GiveMessages, AnsiString Message);
/// At the end of operation a summary of overall performance is sent to the performance file by this function
    void SendPerformanceSummary(int Caller, std::ofstream &PerfFile);
/// This sets all the warning flags (CrashWarning, DerailWarning etc) to their required states after a session file has been loaded
    void SetWarningFlags(int Caller);
    void StopTTClockMessage(int Caller, AnsiString Message); ///< sends a message to the user and stops the timetable clock while it is displayed
    void StripExcessFromHeadCode(int Caller, AnsiString &HeadCode); ///< change an extended headcode to an ordinary 4 character headcode
/// Strip both leading and trailing spaces at ends of Input and spaces before and after
/// all commas and semicolons within Input, used to rationalise timetable service entries for internal use
    void StripSpaces(int Caller, AnsiString &Input);
/// Sends a message to the user if GiveMessages is true, including ServiceReference (see above) if not null, used for timetable error messages
    void TimetableMessage(bool GiveMessages, AnsiString Message);
    void UnplotTrains(int Caller); ///< unplot all trains from screen
/// Called by TInterface::SaveOperatingImage1Click) to write all trains to the image file
    void WriteTrainsToImage(int Caller, Graphics::TBitmap *Bitmap);

/// Constructor
    TTrainController::TTrainController();
/// Destructor
    TTrainController::~TTrainController();
};

//---------------------------------------------------------------------------

extern TTrainController *TrainController; ///< the object pointer, one object only - created in InterfaceUnit

//---------------------------------------------------------------------------
#endif
