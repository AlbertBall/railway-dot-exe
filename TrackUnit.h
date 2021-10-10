// TrackUnit.h
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
#ifndef TrackUnitH
#define TrackUnitH
// ---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#include <map>
#include <fstream>
#include <list>
#include <utility>                       //for pair
#include "DisplayUnit.h"                 //for UserGraphicVector

#define FirstUnusedSpeedTagNumber    147 //defined value for use in array sizing etc
#define DefaultTrackLength           100
#define DefaultTrackSpeedLimit       200

// ---------------------------------------------------------------------------

typedef std::pair<int, int>THVPair;
///< HLoc/VLoc position pair

class TDisplay;
///< type only declared here to allow access, full declaration in DisplayUnit
class TPrefDirElement;
///< forward declaration because needed in TTrack
typedef std::vector<TPrefDirElement> TPrefDirVector;
///< forward declaration because needed in TTrack

// ---------------------------------------------------------------------------

/// Map and multimap comparator based on horizontal & vertical position
class TMapComp
{
public:
/// HLoc VLoc
    bool operator()(const THVPair& lower, const THVPair& higher) const;
};

// ---------------------------------------------------------------------------

enum TTrackType
///< describes the type of track element
{
    Simple, Crossover, Points, Buffers, Bridge, SignalPost, Continuation, Platform, GapJump, FootCrossing, Unused,
    Concourse, Parapet, NamedNonStationLocation, Erase, LevelCrossing
};
// FootCrossing covers footbridge & underpass/surface, 'unused' was marker the for old 'text' number, since disused
// Concourse, Parapet, NamedNonStationLocation, Platform & LevelCrossing are the 5 types of inactive element
// Erase was the default active element used for erasing, not used now (all data members unset)

enum TConfiguration
///< describes the type of track link. 'End' is used for both buffer stop and continuation entry/exit positions
{
    NotSet, Connection, End, Gap, Lead, Trail, CrossConn, Under, Signal
};

// FIXED TRACK :-
// All basic track building blocks & methods

class TFixedTrackPiece
{
public: // everything uses these - should really have Gets & Sets but too many to change now

    bool FixedNamedLocationElement;
///< true for an element that can be named (platforms, concourse, footcrossings & non-station named loactions)
    int SpeedTag;
///< The element identification number - corresponds to the relevant SpeedButton->Tag
    int Link[4];
///< Track connection link values, max. of 4, unused = -1, top lh diag.=1, top=2, top rh diag.=3, left=4, right=6, bottom lh diag.=7, bottom=8, bottom rh diag.=9
    Graphics::TBitmap *GraphicPtr;
///< the track bitmap for display on the zoomed-in railway
    Graphics::TBitmap *SmallGraphicPtr;
///< the track bitmap for display on the zoomed-out railway

    TConfiguration Config[4];
///< the type of link - see TConfiguration above

    TTrackType TrackType;
///< the type of track element

/// Plot the element on the railway display at position HLocInput & VLocInput
    void PlotFixedTrackElement(int Caller, int HLocInput, int VLocInput) const;
/// Constructor for building TTrack.FixedTrackArray - see below
    TFixedTrackPiece(int SpeedTagVal, TTrackType TrackTypeVal, int LkVal[4], TConfiguration ConfigVal[4], Graphics::TBitmap *GraphicPtrVal,
                     Graphics::TBitmap *SmallGraphicPtrVal);
/// Default constructor
    TFixedTrackPiece();
};

// ---------------------------------------------------------------------------

// VARIABLE TRACK :-

// ---------------------------------------------------------------------------

/* Note:  Should probably have used different derived classes for the different track types, to avoid all having
      attributes & other specific data, but by the time this occurred to me as a good idea it seemed likely to be more difficult
      to backtrack than to put up with the extra & unused data.
 */

/// Basic track elements as implemented in the overall railway layout
class TTrackElement : public TFixedTrackPiece
{
public: // everything uses these - should really have Gets & Sets but too many to change now

    AnsiString ActiveTrackElementName;
///< Location name used either in the timetable or for a continuation (continuation names not used in timetable as trains can't stop there).  Only active track elements where there are platforms or non-station named locations have ActiveTrackElementNames
    AnsiString ElementID;
///< the element identifier based on position in the railway
    AnsiString LocationName;
///< location name not used for timetabling, only for identification: platforms, non-station named locations, concourses and footcrossings have LocationNames

    bool CallingOnSet;
///< Used for for signals only when a train is being called on - used to plot the position lights
    bool LCPlotted;
///< Utility marker to avoid plotting every element of a multitrack LC during ClearandRebuildRailway
    bool TempTrackMarker01, TempTrackMarker23;
///< Utility markers for program use
    int Attribute;
///< special variable used only for points, signals & level crossings, ignored otherwise; points 0=set to go straight, 1=set to diverge, where both legs diverge 0=set to left fork; signals:  0=red; 1=yellow; 2=double yellow; 3 = green; Level crossing: 0 = raised barriers = closed to trains; 1 = lowered barriers = open to trains; 2 = changing state = closed to trains
    int Conn[4];
///< Connecting element position in TrackVector, set to -1 if no connecting link or if track not linked
    int ConnLinkPos[4];
///< Connecting element link position (i.e. array positions of the connecting element links, in same order as Link[4])
    int HLoc, VLoc;
///< The h & v locations in the railway (top lh corner of the first build screen = 0,0)
    int Length01, Length23, SpeedLimit01, SpeedLimit23;
///< Element lengths and speed limits, ...01 is for the track with link positions [0] and [1], ...23 for [2] and [3], set to -1 if not used (lengths in m & speed limits in km/h)
    int StationEntryStopLinkPos1, StationEntryStopLinkPos2;
///< Used for track at platforms and non-station named locations to mark the train front element stop position, there are two for the two directions of travel, set to -1 if not used
    int TrainIDOnElement, TrainIDOnBridgeTrackPos01, TrainIDOnBridgeTrackPos23;
///< Set to the TrainID value when a train is present on the element, bridges can have two trains present so the ...01 and ...23 values give the TrainIDs for track with link positions [0] & [1], and [2] & [3] respectively, set to -1 if no train present
    enum
///< added at version 0.6
    {
        FourAspect, ThreeAspect, TwoAspect, GroundSignal
    } SigAspect;

// inline functions

/// Constructor for specific type of element. Use very high neg. numbers as 'unset' values for HLoc & VLoc initially as can go high negatively legitimately, build from existing TTrackPiece with default values for extra members
    TTrackElement(TFixedTrackPiece Input) : TFixedTrackPiece(Input), HLoc(-2000000000), VLoc(-2000000000), LocationName(""), ActiveTrackElementName(""),
        Attribute(0), CallingOnSet(false), Length01(DefaultTrackLength), Length23(-1), SpeedLimit01(DefaultTrackSpeedLimit), SpeedLimit23(-1),
        TrainIDOnElement(-1), TrainIDOnBridgeTrackPos01(-1), TrainIDOnBridgeTrackPos23(-1), StationEntryStopLinkPos1(-1), StationEntryStopLinkPos2(-1),
        SigAspect(FourAspect)
    {
        for(int x = 0; x < 4; x++)
        {
            ConnLinkPos[x] = -1;
            Conn[x] = -1;
        }
        if((TrackType == Points) || (TrackType == Crossover) || (TrackType == Bridge))
        {
            Length23 = DefaultTrackLength;

            SpeedLimit23 = DefaultTrackSpeedLimit;
        }
    }

/// Constructor for non-specific default element. Use high neg numbers for 'unset' h & v as can go high negatively legitimately
    TTrackElement() : TFixedTrackPiece(), HLoc(-2000000000), VLoc(-2000000000), LocationName(""), ActiveTrackElementName(""), Attribute(0), CallingOnSet(false),
        Length01(-1), Length23(-1), SpeedLimit01(-1), SpeedLimit23(-1), TrainIDOnElement(-1), TrainIDOnBridgeTrackPos01(-1), TrainIDOnBridgeTrackPos23(-1),
        StationEntryStopLinkPos1(-1), StationEntryStopLinkPos2(-1), SigAspect(FourAspect)
    {
        for(int x = 0; x < 4; x++)
        {
            ConnLinkPos[x] = -1;
            Conn[x] = -1;
        }
    }

// functions defined in .cpp file

    bool operator == (TTrackElement RHElement);
///< equivalence operator
    bool operator != (TTrackElement RHElement);
///< non-equivalence operator
    AnsiString TTrackElement::LogTrack(int Caller) const;
///< Used to log track parameters for call stack logging
    void PlotVariableTrackElement(int Caller, TDisplay *Disp) const;
///< Plot the element on the display 'variable' indicates that the element may be named and if so may be plotted striped or solid depending on whether the name has been set
};

// ---------------------------------------------------------------------------
// PrefDir & Route elements
// ---------------------------------------------------------------------------

/// Basic preferred direction or route element - track element with additional members
class TPrefDirElement : public TTrackElement
{
protected:

    int ELink, ELinkPos;
///< entry link number & array position
    int XLink, XLinkPos;
///< exit link number & array position
    int EXNumber;
///< used to facilitate identification of the appropriate preferred direction or route graphic
    int TrackVectorPosition;
///< TrackVectorPosition of the corresponding track element
    int CheckCount;
///< internal check value used when building preferred directions
    Graphics::TBitmap *EXGraphicPtr, *EntryDirectionGraphicPtr;
///< pointers to the appropriate entry/exit graphic, or direction marker graphic, for preferred directions and routes

    bool operator == (TPrefDirElement RHElement);
///< equivalence operator
    bool operator != (TPrefDirElement RHElement);
///< non-equivalence operator

public:

    friend class TOnePrefDir;
    friend class TOneRoute;
    friend class TAllRoutes;

    bool IsARoute;
///< false for Pref Dir, true for route
    bool AutoSignals;
///< marker within the route for an AutoSignal route element
    bool PrefDirRoute;
///< marker within the route for preferred direction route element

// inline functions

/// Position check
    bool IsPosition(int Position) const
    {
        if(TrackVectorPosition == Position)
        {
            return(true);
        }
        else
        {
            return(false);
        }
    }

/// Returns SpeedTag  //added at v2.9.2 for clipboard storage
    int GetSpeedTag() const
    {
        return(SpeedTag);
    }

/// Returns HLoc  //added at v2.9.0 for clipboard storage
    int GetHLoc() const
    {
        return(HLoc);
    }

/// Returns VLoc  //added at v2.9.0 for clipboard storage
    int GetVLoc() const
    {
        return(VLoc);
    }

/// Returns ELink
    int GetELink() const
    {
        return(ELink);
    }

/// Returns the ELink array position
    int GetELinkPos() const
    {
        return(ELinkPos);
    }

/// Returns XLink
    int GetXLink() const
    {
        return(XLink);
    }

/// Returns the XLink array position
    int GetXLinkPos() const
    {
        return(XLinkPos);
    }

/// Returns EXNumber     //added at v2.9.0 for clipboard storage
    int GetEXNumber()
    {
        return(EXNumber);
    }

/// Returns CheckCount
    int GetCheckCount() //added at v2.9.1
    {
        return(CheckCount);
    }

/// Returns TrackVectorPosition
    unsigned int GetTrackVectorPosition() const
    {
        return(TrackVectorPosition);
    }

/// Returns signed integer value of TrackVectorPosition  (used in flip, mirror etc for pref dirs) added at v2.9.1
    int GetSignedIntTrackVectorPosition() const
    {
        return(TrackVectorPosition);
    }

/// Returns EXGraphicPtr for preferred directions
    Graphics::TBitmap *GetEXGraphicPtr()
    {
        return(GetPrefDirGraphicPtr());
    }

/// Returns route graphic
    Graphics::TBitmap *GetRouteEXGraphicPtr()
    {
        return(GetRouteGraphicPtr(AutoSignals, PrefDirRoute));
    }

/// Used in pasting pref dirs
    void SetTrackVectorPosition(int TVPos) // added at v2.9.0
    {
        TrackVectorPosition = TVPos;
    }

    void SetCheckCount(int ChkCnt) //added at v2.9.1
    {
        CheckCount = ChkCnt;
    }

/// Used in pasting pref dirs
    void SetELink(int input) // added at v2.9.0
    {
        ELink = input;
    }

/// Used in pasting pref dirs
    void SetELinkPos(int input) // added at v2.9.0
    {
        ELinkPos = input;
    }

/// Used in pasting pref dirs
    void SetXLink(int input) // added at v2.9.0
    {
        XLink = input;
    }

/// Used in pasting pref dirs
    void SetXLinkPos(int input) // added at v2.9.0
    {
        XLinkPos = input;
    }

/// Used in pasting pref dirs
    void SetEXNumber(int input) // added at v2.9.0
    {
        EXNumber = input;
    }

/// Used in pasting pref dirs
    void SetEXGraphicPtr(Graphics::TBitmap *input) //added at v2.9.1
    {
        EXGraphicPtr = input;
    }

/// Used in pasting pref dirs
    void SetEntryDirectionGraphicPtr(Graphics::TBitmap *input) //added at v2.9.1
    {
        EntryDirectionGraphicPtr = input;
    }

/// Default constructor, loads default values
    TPrefDirElement() : TTrackElement(), ELink(-1), ELinkPos(-1), XLink(-1), XLinkPos(-1), EXNumber(-1), TrackVectorPosition(-1), CheckCount(0), EXGraphicPtr(0),
        EntryDirectionGraphicPtr(0), IsARoute(false), AutoSignals(false), PrefDirRoute(false)
    {
        ;
    }

/// Constructs a PrefDirElement from a base TrackElement. Sets up the TrackElement values but leaves others as default values
    TPrefDirElement(TTrackElement Input) : TTrackElement(Input), ELink(-1), ELinkPos(-1), XLink(-1), XLinkPos(-1), EXNumber(-1), TrackVectorPosition(-1),
        CheckCount(0), EXGraphicPtr(0), EntryDirectionGraphicPtr(0), IsARoute(false), AutoSignals(false), PrefDirRoute(false)
    {
        ;
    }

// external functions

    bool EntryExitNumber();
///< determines and loads EXNumber (see above)
    AnsiString LogPrefDir() const;
///< Sends a list of PrefDirElement values to Utilities->CallLog file for debugging purposes
    TPrefDirElement(TTrackElement InputElement, int ELink, int ELinkPos, int XLink, int XLinkPos, int TrackVectorPosition);
///< Constructs a PrefDirElement from supplied values
    Graphics::TBitmap *GetDirectionPrefDirGraphicPtr() const;
///< picks up the EntryDirectionGraphicPtr for preferred directions
    Graphics::TBitmap *GetDirectionRouteGraphicPtr(bool AutoSigsFlag, bool PrefDirRoute) const;
///< picks up the green or red route direction graphic
    Graphics::TBitmap *GetOriginalGraphicPtr();
///< picks up the original (non-flashing) graphic for use during route flashing
    Graphics::TBitmap *GetPrefDirGraphicPtr();
///< picks up the EXGraphicPtr for preferred directions
    Graphics::TBitmap *GetRouteAutoSigsGraphicPtr();
///< picks up the blue route graphic (not used - superseded by GetRouteGraphicPtr)
    Graphics::TBitmap *GetRouteGraphicPtr(bool AutoSigsFlag, bool PrefDirRoute);
///< picks up the appropriate route graphic


};

// ---------------------------------------------------------------------------

/// Allows a single Width x Height graphic to change and change back independently of the remaining display
/**
Used for the flashing green and red gap markers, flashing points and route start graphics.  The code is mostly self-explanatory, but SetScreenHVSource (sets source
rectangle) must be called before the original graphic is loaded, whether or not the graphic is loaded from the screen (using
LoadOriginalScreenGraphic, for point flashing and route start markers) or an existing bitmap (using LoadOriginalExistingGraphic, for red
and green gap flashing), and OverlayGraphic and OriginalGraphic must be loaded before they are plotted.  Checks are
built in for these conditions.  SourceRect is the rectangle on the appropriate canvas where the original graphic is taken from.  The
original graphic can be taken from the screen - LoadOriginalScreenGraphic(), or from a section from an existing bitmap -
LoadOriginalExistingGraphic.  If an existing bitmap is selected then the loading function overrides the size that was set in the
constructor, and SourceRect & HPos & VPos that were set in SetScreenHVSource.
*/
class TGraphicElement
{
private:

    bool OverlayPlotted, OverlayLoaded, OriginalLoaded, ScreenSourceSet, ScreenGraphicLoaded, ExistingGraphicLoaded;
///< state flags
    int HPos, VPos;
///< horizontal and vertical positions
    int Width, Height;
///< dimensions in pixels
    Graphics::TBitmap *OriginalGraphic, *OverlayGraphic;
///< original and temporary overlay graphics
    TRect SourceRect;
///< source rectangle of the original graphic

public:
// inline functions
    int GetHPos()
    {
        return(HPos);
    }

    int GetVPos()
    {
        return(VPos);
    }

/// Set SourceRect member values from those supplied and existing Width & Height - ensure this is only called after Width & Height are set
    void SetSourceRect(int Left, int Top)
    {
        SourceRect.init(Left, Top, Left + Width, Top + Height);
    }

// functions defined in .cpp file


    void LoadOriginalExistingGraphic(int Caller, int HOffset, int VOffset, int WidthIn, int HeightIn, Graphics::TBitmap *Graphic);
///< Load red or green gap flashing graphic from the stored bitmaps
    void LoadOriginalScreenGraphic(int Caller);
///< Load original graphic from the screen for point flashing or route start markers
    void LoadOverlayGraphic(int Caller, Graphics::TBitmap *Overlay);
///< Load the temporary overlay graphic
    void PlotOverlay(int Caller, TDisplay *Disp);
///< Plot the overlay graphic on screen
    void PlotOriginal(int Caller, TDisplay *Disp);
///< Plot the original graphic on screen
    void SetScreenHVSource(int Caller, int HPosIn, int VPosIn);
///< Set HPos, VPos & SourceRect member values from the supplied positions
    TGraphicElement();
///< Default constructor (16 x 16 pixel element)
    TGraphicElement(int WidthIn, int HeightIn);
///< Constructor for specified dimensions
    ~TGraphicElement();
///< Destructor
};

// ---------------------------------------------------------------------------

/** Identification Number:

This was introduced when it was decided to have a route identification number for each route rather than using the vector
position number for identifying existing routes that were being extended during route building.  Using vector position numbers
meant that these identification numbers had to be changed when existing routes were erased by trains passing over them.  IDInt is
used for StartSelectionRouteID and ReqPosRouteID (see TAllRoutes) and ensures that any confusion with the old vector position
numbers is picked up by the compiler. Note that the route's RouteID value is an 'int', not an 'IDInt', 'IDInt' is only used for
StartSelectionRouteID and ReqPosRouteID */
class IDInt
{
private:
    int InternalInt;
///< the internal integer value represented by IDInt

public:
// all inline

/// get the internal integer
    int GetInt() const
    {
        return(InternalInt);
    }

/// Equality comparator
    bool operator == (IDInt Comparator)
    {
        return (InternalInt == Comparator.InternalInt);
    }

/// Greater than comparator
    bool operator > (int Comparator)
    {
        return (InternalInt > Comparator);
    }

/// Constructor that sets the internal integer to the input value. The 'explicit' prefix is used to force a compiler error if the input value is an IDInt, which would be a program error (otherwise it would be implicitly converted to an int)
    explicit IDInt::IDInt(int Int)
    {
        InternalInt = Int;
    }

/// Default constructor, internal integer set to -1
    IDInt::IDInt()
    {
        InternalInt = -1;
    }
};

// ---------------------------------------------------------------------------
// Track
// ---------------------------------------------------------------------------

/**All dynamic track data & methods.  Only one object since only one operating railway

Note:  The TrackMap & InactiveTrackMap were developed well after the TrackVector, to speed up track element
searches.  It was realised at that time that the maps themselves could contain type TTrackElement rather than int
(for TrackVectorPosition), and that the track vectors could be dispensed with completely.  However after an attempt
to remove them it was clear that they were far too embedded throughout the program for easy removal, so they were
left in.  */
class TTrack
{
private:
/// Holds an array of TrackPieces, only accessible to TTrack
    class TFixedTrackArray
    {
    public:

        TFixedTrackPiece FixedTrackPiece[FirstUnusedSpeedTagNumber];
///< the array member

/// Array constructor
        TFixedTrackArray();
    };

    TFixedTrackArray FixedTrackArray;
///< the FixedTrackPiece array object
    TTrackElement DistanceStartElement, DistanceContinuingElement;
///< initially used for track element lengths but since disused

    bool TrackFinished;
///< marker for all Conn & ConnLinkPos values set & track complete

    int GapPos, GapHLoc, GapVLoc;
///< record gap setting info
    int HLocMin, VLocMin, HLocMax, VLocMax;
///< give extent of railway for use in zoomed in and out displays and in saving railway images
    int LinkCheckArray[9][2];
///< array of valid link connecting values, I don't think this is used now
    int LinkHVArray[10][2];
///< array used to determine relative horizontal & vertical track element positions for specific link values
    int Tag76Array[25][3];
///< these arrays give valid adjacent named element relative positions for each type of named element, the numbers - 76, 77 etc - relate to track element element SpeedTag values (76 - 79 = platforms, 96 = concourse, 129 & 130 = footbridges, 145 & 146 underpasses and 131 = non-station named location.
    int Tag77Array[25][3];
    int Tag78Array[25][3];
    int Tag79Array[25][3];
    int Tag96Array[28][3];
    int Tag129Array[8][3];
    int Tag130Array[8][3];
    int Tag131Array[4][3];
    int Tag145Array[8][3];
    int Tag146Array[8][3];

    Set<int, 1, 146>TopPlatAllowed, BotPlatAllowed, LeftPlatAllowed, RightPlatAllowed, NameAllowed, LevelCrossingAllowed;
///< sets of valid TrackElements for placement of platforms and non-station named locations

public:
/*
All LCs begin with barriers raised. i.e. closed to trains, that is the normal state.  When a route is set through an LC an active LC object is created
by SetLCChangeValues (called by ConvertandAdd....  for lowering barriers) and added to the
ChangingLCVector.  Once created 'FlashingGraphics' takes care of the flashing, until the duration is reached.  While flashing no further routes
can be set through that LC and the first route can't be cancelled, hence the flashing only needs to cater for plotting the route on the one track that
started the barrier lowering.  When the duration is reached, the object is transferred to a new vector BarriersDownVector, after the StartTime has been
reset (to time the period for which the barriers are down - penalties are given for > 3 minutes), BarrierState changed to Down, and the object erased
from ChangingLCVector.  When there is no route through an LC and no train on the track then the barriers are raised - in ClockTimer2 - when the
BarriersDownVector object is copied back to ChangingLCVector with a new StartTime, BarrierState and ChangeDuration.  Again FlashingGraphics takes care
of the flashing until the duration is reached, when the object is erased from the vector and the LC reverts to its normal (barriers raised) state.

At v2.6.0 LCs could be lowered and raised manually, manual LCs are shown lowering and down as green and indicated by TypeOfRoute being 2.  A manual LC
can't have a route set while changing; can't be opened while a route is set; and must be opened manually.*/

    enum TBarrierState
///< state of barriers, values for level crossings either changing state or with barriers up or down
    {
        Raising, Lowering, Up, Down
    };

    class TActiveLevelCrossing
    {
    public:

        int TypeOfRoute;
///< route type - 0 = nonsignals, 1 = preferred direction (can't have autosigs), 2 no route, 2 added at v2.6.0 for manual operation
        bool ReducedTimePenalty;
///< marker that is set when a train is present on one of the elements of the LC - used to provide a 3 minute penalty allowance
        TBarrierState BarrierState;
///< state of barriers - Raising, Lowering, Up, Down (an enum - see above)
        float ChangeDuration;
///< duration of the level crossing changing period
        int BaseElementSpeedTag;
///< SpeedTag value for the base element of a level crossing
        int HLoc;
///< HLoc value for found level crossing element
        int VLoc;
///< VLoc value for found level crossing element
        TDateTime StartTime;
///< stores the starting time for level crossing changing
        TActiveLevelCrossing::TActiveLevelCrossing();
///< constructor, sets default values
    };

    typedef std::vector<TActiveLevelCrossing>TActiveLCVector;
///< vector of changing level crossing objects.  Note that although a LC may contain several elements there will be only one in the vector when changing, and it might be any of the individual elements.  This is because when an entry is made all linked elements have their attributes changed to 2 for changing, so no more are found.  This applies both for closing & opening to trains

    typedef std::vector<int>TLCVector;
///< vector of level crossing InactiveTrackVector positions - note that this contains all LC elements whether linked to others or not

    typedef std::vector<TUserGraphicItem>TUserGraphicVector;
///< vector of UserGraphicItems

    typedef std::vector<TTrackElement>TTrackVector;
///< vector of TrackElements
    typedef std::vector<TTrackElement>::iterator TTrackVectorIterator;
///< iterator for TTrackVector
    typedef std::map<AnsiString, TPicture*>TUserGraphicMap;
///< map of filenames as key and TPicture* as value. This holds all the TPicture pointers created when a user graphic is selected
    typedef std::pair<AnsiString, TPicture*>TUserGraphicMapEntry;
///<an entry for TUserGraphicMap

    typedef std::map<THVPair, unsigned int, TMapComp>TTrackMap;
///< map of TrackElement TrackVectorPositions, HLoc & VLoc pair is the key
    typedef TTrackMap::iterator TTrackMapIterator;
    typedef std::pair<THVPair, unsigned int>TTrackMapEntry;

    typedef std::map<THVPair, THVPair, TMapComp>TGapMap;
///< map of matching gap positions as an HLoc/VLoc pair, with the key being
    typedef TGapMap::iterator TGapMapIterator;
///< the first gap HLoc/VLoc pair, contains one entry for each pair of matched gaps
    typedef std::pair<THVPair, THVPair>TGapMapEntry;

    typedef std::multimap<THVPair, unsigned int, TMapComp>TInactiveTrack2MultiMap;
///< multimap of inactive TrackElements (platforms, concourses, non-station named locations, parapets & level crossings) '2' because can have 2 entries (platforms) at a single location
    typedef TInactiveTrack2MultiMap::iterator TInactiveTrack2MultiMapIterator;
///< iterator for TInactiveTrack2MultiMap
    typedef std::pair<TInactiveTrack2MultiMapIterator, TInactiveTrack2MultiMapIterator>TInactiveTrackRange;
///< range for TInactiveTrack2MultiMap

    typedef std::pair<unsigned int, unsigned int>TIMPair;
///< TrackElement pair type used for inactive elements, values are vector positions

    typedef std::list<int>TLNPendingList;
///< type list of location name vector positions (see note below) used during
    typedef TLNPendingList::iterator TLNPendingListIterator;
///< naming of linked named location elements

    typedef std::multimap<THVPair, int, TMapComp>TLNDone2MultiMap;
///< multimap of location name vector positions (see note below) used
    typedef TLNDone2MultiMap::iterator TLNDone2MultiMapIterator;
///< during naming of linked named location elements, '2' because there
    typedef std::pair<THVPair, int>TLNDone2MultiMapEntry;
///< can be up to 2 entries (platforms) at a single location

    typedef std::multimap<AnsiString, int>TLocationNameMultiMap;
///< map of location name vector positions (see note below), one entry for every element that is a FixedNamedLocationElement i.e platforms, concourses, footcrossings & named non-station locations.  Hence the only active track elements included are footcrossings
    typedef TLocationNameMultiMap::iterator TLocationNameMultiMapIterator;
    typedef std::pair<TLocationNameMultiMapIterator, TLocationNameMultiMapIterator> TLocationNameMultiMapRange;
    typedef std::pair<AnsiString, int>TLocationNameMultiMapEntry;

    typedef std::map<THVPair, bool> THVPairsLinkedMap;
///< added at v2.6.1 for use in PopulateHVPairsLinkedMapAndNoDuplicates

// NOTE: the above (TLNPendingList, TLNDone2MultiMap & TLocationNameMultiMap) store adjusted vector positions - adjusted because have
// a single int to represent both active and inactive vector positions.  Use (-1 - Position) for active vector positions & (Position)
// for inactive vector positions (most location elements are in the inactive vector so these are positive).  The '-1' bit is needed
// because the value '0' is used for the first position in the inactive vector

    typedef std::map<AnsiString, int>TActiveTrackElementNameMap;
///< map of ActiveTrackElementNames compiled and used for populating the LocationNameComboBox during timetable creation or editing.  Used in place of LocationNameMultiMap as that can contain concourses and non-station named locations that aren't associated with any track.  The second 'int' entry is a dummy, only the list of AnsiString names is needed, and being a map it is automatically sorted and without duplicates.
    typedef TActiveTrackElementNameMap::iterator TActiveTrackElementNameIterator;
    typedef std::pair<AnsiString, int>TActiveTrackElementNameMapEntry;

/// Used as basic elements in a table of signals - see SigTable below
    struct TSigElement
    {
        // NOTE: Don't alter the order of these members as they are loaded from an array of values in the constructor
        int SpeedTag;
///< the TrackElement SpeedTag value - specifies the signal element
        int Attribute;
///< the signal state - red, yellow, double yellow or green
        Graphics::TBitmap* SigPtr;
///< pointer to the graphic
    };

    TSigElement SigTable[40];
///< original table of signals for four aspect
    TSigElement SigTableThreeAspect[40];
///< new at version 0.6 for three aspect
    TSigElement SigTableTwoAspect[40];
///< new at version 0.6 for two aspect
    TSigElement SigTableGroundSignal[40];
///< new at version 0.6 for ground signals

    AnsiString RouteFailMessage;

    bool ActiveTrackElementNameMapCompiledFlag;
///< indicates that the ActiveTrackElementNameMap has been compiled
    bool CopyFlag;
///< true only when copying a selection, used to prevent location names being copied
    bool DuplicatedLocationName(int Caller, bool GiveMessage);
///< examines LocationNameMultiMap and returns true if there are two or more locations with the same name - added at v2.6.1 to cater for Bill78's new .dev file merge program and used when try to save as a .rly file.
    bool GapFlashFlag;
///< true when a pair of connected gaps is flashing
    bool LCChangeFlag;
///< true when LCs changing
    bool LCFoundInAutoSigsRoute;
///< true if found an LC during an automatic route search
    bool NoPlatsMessageSent;
///< used to send no platforms warning once only
    bool SuppressRouteFailMessage;
///< true if a message has been given in the search routine, to avoid giving multiple times and to avoid other failure messages being given
    bool LCFoundInRouteBuildingFlag;
///< true if a route set through an LC that is closed to trains (& therefore needs to be opened)
    bool PointFlashFlag;
///< true when points are flashing during manual change
    bool RouteFlashFlag;
///< true while a route is flashing prior to being set
    bool SkipLocationNameMultiMapCheck;
///<changed from PastingWithAttributes in v2.4.0 as all pastes are now with attributes - needed to suppress multimap checks while pasting
    bool OverrideAndHideSignalBridgeMessage;
///<if false signals facing bridges are not permitted, but can be set to true using CTRL ALT 5
    float LevelCrossingBarrierUpFlashDuration;
///< duration of the flash period when level crossing closing to trains
    float LevelCrossingBarrierDownFlashDuration;
///< duration of the flash period when level crossing opening

    int FlipArray[FirstUnusedSpeedTagNumber];
///< holds TrackElement SpeedTag values for 'flipping' via menu items 'Edit' & 'Flip'
    int GapFlashGreenPosition, GapFlashRedPosition;
///< TrackVectorPosition of the gap element that is flashing green or red
    int MirrorArray[FirstUnusedSpeedTagNumber];
///< holds TrackElement SpeedTag values for 'mirroring' via menu items 'Edit' & 'Mirror'
    int RotRightArray[FirstUnusedSpeedTagNumber];
///< holds TrackElement SpeedTag values for 'rotating right' via menu items 'Edit' & 'Rotate right'
    int RotLeftArray[FirstUnusedSpeedTagNumber];

///< holds TrackElement SpeedTag values for 'rotating left' via menu items 'Edit' & 'Rotate left'
    std::map<AnsiString, char>ContinuationNameMap;

///< map of all continuation names, char is a dummy
    TActiveTrackElementNameMap ActiveTrackElementNameMap;
///< map of active track element names
    TActiveLCVector ChangingLCVector;
///< vector of values for changing level crossings - i.e. barriers in course of being raised or lowered
    TActiveLCVector BarriersDownVector;
///< vector of LCs with barriers down
    TGapMap GapMap;
///< map of gaps (see type for more information above)
    TGraphicElement *GapFlashGreen, *GapFlashRed;
///< the red & green circle graphics used to show where the gaps are
    TInactiveTrack2MultiMap InactiveTrack2MultiMap;
///< multimap of inactive TrackElements (see type for more information above)
    TLCVector LCVector;
///< vector of level crossing InactiveTrackVector positions
    TLNDone2MultiMap LNDone2MultiMap;
///< multimap of processed location name elements (see type for more information above)
    TLNPendingList LNPendingList;
///< list of location name elements awaiting processing (see type for more information above)
    TLocationNameMultiMap LocationNameMultiMap;
///< multimap of location names (see type for more information above)
    TUserGraphicVector UserGraphicVector, SelectGraphicVector;
    TUserGraphicMap UserGraphicMap;
///<the map of graphic filenames as key and TPicture* as values
    TTrackMap TrackMap;
///< map of track (see type for more information above)
    TTrackVector TrackVector, InactiveTrackVector, NewVector, DistanceVector, DistanceSearchVector, SelectVector;
///< vectors of TrackElements
    TTrackVectorIterator NextTrackElementPtr;
///< track vector iterator used during cycling through a track vector
    TUserGraphicMapEntry UGME;
///<an entry for the UserGraphicMap

// inline functions

/// Return location name for a given inactive track vector position
    AnsiString GetLocationName(unsigned int InactiveTrackVectorPosition)
    {
        return(InactiveTrackElementAt(24, InactiveTrackVectorPosition).LocationName);
    }

/// Indicates whether or not the railway is ready for saving as a '.rly' file and for operation
    bool IsReadyForOperation(bool GiveMessage)
    {
        return (IsTrackFinished() && !LocationsNotNamed(1) && !GapsUnset(8) && !DuplicatedLocationName(0, GiveMessage));
    }

/// Indicates whether or not the track has been successfully linked together
    bool IsTrackFinished()
    {
        return(TrackFinished);
    }

/// checks if a user graphic present
    bool UserGraphicPresentAtHV(int Caller, int HPos, int VPos, int& UGIVectorPos)
    {
        UGIVectorPos = 0;
        for(int x = (UserGraphicVector.size() - 1); x >= 0; x--) // go downwards because may erase the element identified
        {
            if((HPos >= (UserGraphicVectorAt(18, x).HPos - (Display->DisplayOffsetH * 16))) && (HPos < (UserGraphicVectorAt(19,
                                                                                                                            x).HPos + UserGraphicVectorAt(20, x).Width - (Display->DisplayOffsetH * 16))) && (VPos >= (UserGraphicVectorAt(21,
                                                                                                                                                                                                                                           x).VPos - (Display->DisplayOffsetV * 16))) && (VPos < (UserGraphicVectorAt(22, x).VPos + UserGraphicVectorAt(23,
                                                                                                                                                                                                                                                                                                                                                        x).Height - (Display->DisplayOffsetV * 16))))
            {
                UGIVectorPos = x;
                return(true);
            }
        }
        return(false);
    }

    enum
    {
        FourAspectBuild, ThreeAspectBuild, TwoAspectBuild, GroundSignalBuild
    } SignalAspectBuildMode;
///< aspect mode for future signal additions

    int GetGapHLoc()
    {
        return(GapHLoc);
    } // return the respective values

    int GetGapVLoc()
    {
        return(GapVLoc);
    }

    int GetHLocMax()
    {
        return(HLocMax);
    }

    int GetHLocMin()
    {
        return(HLocMin);
    }

    int GetVLocMax()
    {
        return(VLocMax);
    }

    int GetVLocMin()
    {
        return(VLocMin);
    }

/// Return the corresponding link position (track always occupies either links 0 & 1 or 2 & 3)
    int GetNonPointsOppositeLinkPos(int LinkPosIn)
    {
        if(LinkPosIn == 3)
        {
            return(2);
        }
        if(LinkPosIn == 2)
        {
            return(3);
        }
        if(LinkPosIn == 1)
        {
            return(0);
        }
        return(1);
    }

/// Return the number of active track elements
    int TrackVectorSize()
    {
        return(TrackVector.size());
    }

/// Return a basic track element from the SpeedTag   new at v2.2.0 - needed because Interface doesn't have direct access to FixedTrackArray
    TTrackElement BuildBasicElementFromSpeedTag(int Caller, int SpeedTag)
    {
        return(FixedTrackArray.FixedTrackPiece[SpeedTag]);
    }

/// Return the number of selected active and inactive track elements (via menu items 'Edit' and 'Select')
    unsigned int SelectVectorSize()
    {
        return(SelectVector.size());
    }

/// Store a TrackElement in the SelectVector
    void SelectPush(TTrackElement TrackElement)
    {
        SelectVector.push_back(TrackElement);
    }

    void SelectVectorClear()
    {
        SelectVector.clear();
    }

// set member values
    void SetHLocMax(int HLoc)
    {
        HLocMax = HLoc;
    }

    void SetHLocMin(int HLoc)
    {
        HLocMin = HLoc;
    }

    void SetTrackFinished(bool Value)
    {
        TrackFinished = Value;
    }

    void SetVLocMax(int VLoc)
    {
        VLocMax = VLoc;
    }

    void SetVLocMin(int VLoc)
    {
        VLocMin = VLoc;
    }

// externally defined functions

/// Used to check the validity of footcrossing links
    bool ActiveMapCheck(int Caller, int HLoc, int VLoc, int SpeedTag);
/// Checks BarrierDownVector and returns true if there is one that is linked to the LC at H & V positions and is set to manual (TypeOfRoute == 2), and returns the vector position in BDVectorPos
    bool AnyLinkedBarrierDownVectorManual(int Caller, int HLoc, int VLoc, int &BDVectorPos);
/// True if a route or train present on any linked level crossing element
    bool AnyLinkedLevelCrossingElementsWithRoutesOrTrains(int Caller, int HLoc, int VLoc, TPrefDirVector SearchVector, bool &TrainPresent);
/// Used during location naming to check for adjacent named elements to a given element at HLoc & VLoc with a specific SpeedTag, and if found allow that element to be inserted into the LNPendingList for naming similarly
    bool AdjElement(int Caller, int HLoc, int VLoc, int SpeedTag, int &FoundElement);
/// Used in SearchForAndUpdateLocationName to check for adjacent named elements to a given element at HLoc & VLoc with a specific SpeedTag, and if found allow that name to be used for this element and all other named elements that are linked to it
    bool AdjNamedElement(int Caller, int HLoc, int VLoc, int SpeedTag, AnsiString &LocationName, int &FoundElement);
/// True for a blank (SpeedTag == 0) element at a specific Trackvector position, no longer used after TrackErase changed (now EraseTrackElement) so that blank elements aren't used
    bool BlankElementAt(int Caller, int At) const;
// True if BarriersDownVector checks OK in SessionFile
    bool CheckActiveLCVector(int Caller, std::ifstream &VecFile);
/// True if a footcrossing is linked properly at both ends
    bool CheckFootCrossingLinks(int Caller, TTrackElement &TrackElement);
/// True if TrackElements in the file are all valid
    bool CheckTrackElementsInFile(int Caller, int &NumberOfActiveElements, bool &GraphicsFollow, std::ifstream& VecFile);
///checks all user graphics & returns true for success
    bool CheckUserGraphics(int Caller, std::ifstream &InFile, UnicodeString GraphicsPath);
/// As DiagonalFouledByRouteOrTrain (in TAllRoutes) but only checks for a train (may or may not be a route present (new at v1.2.0))
    bool DiagonalFouledByTrain(int Caller, int HLoc, int VLoc, int DiagonalLinkNumber, int &TrainID);
/// True if the element defined by MapPos is present in LNDone2MultiMap, used during location naming
    bool ElementInLNDone2MultiMap(int Caller, int MapPos);
// True if the element defined by MapPos is present in LNPendingList, used during location naming
    bool ElementInLNPendingList(int Caller, int MapPos);
/// Check for track errors prior to gap setting - disused as incorporated a time-consuming double brute force search
    bool ErrorInTrackBeforeSetGaps(int Caller, int &HLoc, int &VLoc);
/// True if there is an unset gap, and if so it is marked with a red circle, used during gap setting
    bool FindAndHighlightAnUnsetGap(int Caller);
/// Used in locating the screen name position for a named location, return true if find an inactive element called 'Name'
    bool FindHighestLowestAndLeftmostNamedElements(int Caller, AnsiString Name, int &VPosHi, int &VPosLo, int &HPos);
/// True if find a non-platform element at HLoc & VLoc, and if so return its TrackVector position and a reference to it in TrackElement
    bool FindNonPlatformMatch(int Caller, int HLoc, int VLoc, int &Position, TTrackElement &TrackElement);
/// True if find an unset gap that matches the gap at HLoc & VLoc, if find one mark it with a green circle
    bool FindSetAndDisplayMatchingGap(int Caller, int HLoc, int VLoc);
/// True if there are gaps in the railway and any are unset
    bool GapsUnset(int Caller);
/// Used to check the validity of footcrossing links
    bool InactiveMapCheck(int Caller, int HLoc, int VLoc, int SpeedTag);
/// New at v1.2.0; true if an inactive track element present
    bool InactiveTrackElementPresentAtHV(int Caller, int HLoc, int VLoc);
/// True if there is an element adjacent to LinkIn for element at HLoc & VLoc
    bool IsATrackElementAdjacentToLink(int Caller, int HLocIn, int VLocIn, int LinkIn);
/// True if there is a vector entry at H & V that is set to manual (TypeOfRoute == 2) and returns the vector position in BDVectorPos
    bool IsBarrierDownVectorAtHVManual(int Caller, int HLoc, int VLoc, int &BDVectorPos);
/// True if track at link positions [0] & [1] if FirstTrack true, else that at [2] & [3] in TrackElement has the default length
/// and speed limit, return true if so
    bool IsElementDefaultLength(int Caller, TTrackElement &TrackElement, bool FirstTrack, bool &LengthDifferent, bool &SpeedDifferent);
/// True if a non-station named location at HLoc & VLoc
    bool IsNamedNonStationLocationPresent(int Caller, int HLoc, int VLoc);
/// True if a level crossing is found at H & V
    bool IsLCAtHV(int Caller, int HLoc, int VLoc);
/// True if an open (to trains) level crossing is found at H & V
    bool IsLCBarrierDownAtHV(int Caller, int HLoc, int VLoc);
/// True if a closed (to trains) level crossing is found at H & V
    bool IsLCBarrierUpAtHV(int Caller, int HLoc, int VLoc);
/// True if barrier is in process of opening or closing at H & V
    bool IsLCBarrierFlashingAtHV(int Caller, int HLoc, int VLoc);
/// True if a non-station named location or platform at HLoc & VLoc
    bool IsPlatformOrNamedNonStationLocationPresent(int Caller, int HLoc, int VLoc);
/// True if track has been successfully linked (not used any more)
    bool IsTrackLinked(int Caller);
/// checks for a route being set across an LC to prevent barriers raising
    bool LCInSearchVector(int Caller, int HLoc, int VLoc, TPrefDirVector SearchVector);  //added at v2.8.0
/// Attempt to link the track and return true if successful, if unsuccessful return error flag and position of the first element that can't be linked together with an appropriate message. This is a link checking (FinalCall false) or linking (FinalCall true) function, with messages, called by TryToConnectTrack, which handles linking and all other associated functions
    bool LinkTrack(int Caller, bool &LocError, int &HLoc, int &VLoc, bool FinalCall);
/// Attempt to link the track and return true if successful, don't issue any screen messages. This is also a link checking (FinalCall false) or linking (FinalCall true) function, without messages,  that is called by TryToConnectTrack, which handles linking and all other associated functions
    bool LinkTrackNoMessages(int Caller, bool FinalCall);
/// True if a non-empty LocationName found in LocationNameMultiMap
    bool LocationNameAllocated(int Caller, AnsiString LocationName);
/// True if there are unnamed NamedLocationElements (includes footcrossings)
    bool LocationsNotNamed(int Caller);
/// True if the two vector positions are points that have a straight and a diverging leg, are linked together by their
/// diverging legs, and both are set either to straight or to diverge
    bool MatchingPoint(int Caller, unsigned int TrackVectorPosition, unsigned int DivergingPosition);
/// True if the active or inactive TrackElement at HLoc & VLoc has its FixedNamedLocationElement member true
    bool NamedLocationElementAt(int Caller, int HLoc, int VLoc);
/// True if there is no active or inactive track in the railway
    bool NoActiveOrInactiveTrack(int Caller);
/// True if there is no active track in the railway
    bool NoActiveTrack(int Caller);
/// True if there are no gaps
    bool NoGaps(int Caller);
/// True if there are no NamedLocationElements (includes footcrossings)
    bool NoNamedLocationElements(int Caller);
/// True if there is a platform, NamedNonStationLocation or Concourse present in the railway
    bool NonFootCrossingNamedLocationExists(int Caller);
/// True if there is at least one named location element with name 'LocationName', used in timetable integrity checking
    bool OneNamedLocationElementAtLocation(int Caller, AnsiString LocationName);
/** check sufficient track elements with same LocationName linked together without any trailing point links to allow a train split. Only one
length is needed to return true, but this doesn't mean that all platforms at the location are long enough.  When a split is required during
operation a specific check is made using ThisNamedLocationLongEnoughForSplit.  Need at least two linked track elements with the same LocationName, with connected
elements at each end, which may or may not be named and no connections via point trailing links.  Note that these conditions
exclude opposed buffers since these not linked.  Used in timetable integrity checking. */
    bool OneNamedLocationLongEnoughForSplit(int Caller, AnsiString LocationName);
/// True if another train on NextEntryPos track of element at NextPos, whether bridge or not, return false if not, or if NextPos == -1, or if only own train on the track
    bool OtherTrainOnTrack(int Caller, int NextPos, int NextEntryPos, int OwnTrainID);
/// Check whether there is a platform present at HLoc & VLoc at the same side as the signal represented by SpeedTag, if so return true, and also return a pointer to the appropriate platform graphic (same as a normal platform graphic but with a bit cut out for the signal)
    bool PlatformOnSignalSide(int Caller, int HLoc, int VLoc, int SpeedTag, Graphics::TBitmap* &SignalPlatformGraphic);
/// Used in checking for duplicate location names after Bill78 (discord name) developed the .dev file merge tool - added at v2.6.1
    bool PopulateHVPairsLinkedMapAndNoDuplicates(int Caller, TLocationNameMultiMapRange LNMMRg);
/// When track is being built it is entered into the TrackVector in the order in which it is built, and the TrackMap reflects that positioning.  When the track is linked, the vector is rebuilt in track element position order, and the map is also rebuilt to reflect the new positions.  Called during track linking, returns true if successful.
    bool RepositionAndMapTrack(int Caller);
/// Sets all Conns and CLks to -1 except for gapjumps that match and are properly set, returns true for any unset gaps
    bool ResetConnClkCheckUnsetGapJumps(int Caller);
/// Called by RepositionAndMapTrack to reset the connecting elements of all set gaps (their TrackVector positions will have changed during the repositioning process), returns true if successful
    bool ResetGapsFromGapMap(int Caller);
/// Return a reference to the inactive track element pointed to by NextTrackElementPtr (during zoomed-in or out track rebuilding, or writing image files), return true if there is a next one or false if not
    bool ReturnNextInactiveTrackElement(int Caller, TTrackElement &Next);
/// Return a reference to the active track element pointed to by NextTrackElementPtr (during zoomed-in or out track rebuilding, or writing image files), return true if there is a next one or false if not
    bool ReturnNextTrackElement(int Caller, TTrackElement &Next);
/// See above under 'OneNamedLocationLongEnoughForSplit'
    bool ThisNamedLocationLongEnoughForSplit(int Caller, AnsiString LocationName, int FirstNamedElementPos, int &SecondNamedElementPos,
                                             int &FirstNamedLinkedElementPos, int &SecondNamedLinkedElementPos);
/// True if a non-empty LocationName found as a timetabled location name i.e. not as a continuation name
    bool TimetabledLocationNameAllocated(int Caller, AnsiString LocationName);
/// New at v1.2.0; true if a track element present (not inactive elements - see InactiveTrackElementPresentAtHV
    bool TrackElementPresentAtHV(int Caller, int HLoc, int VLoc);
/// New at v1.2.0; checks whether a train present at input location and link and returns its ID if so
    bool TrainOnLink(int Caller, int HLoc, int VLoc, int Link, int &TrainID);
/// Handles all tasks associated with track linking, returns true if successful (see also LinkTrack & LinkTrackNoMessages above)
    bool TryToConnectTrack(int Caller, bool &LocError, int &HLoc, int &VLoc, bool GiveMessages);
/// Return a pointer to the point fillet (the bit that appears to move when points are changed) for the point and its Attribute specified in TrackElement
    Graphics::TBitmap *GetFilletGraphic(int Caller, TTrackElement TrackElement);
/// Return a pointer to the striped (i.e. when unnamed) graphic corresponding to TrackElement, if TrackElement isn't a named element just return its normal graphic
    Graphics::TBitmap *RetrieveStripedNamedLocationGraphicsWhereRelevant(int Caller, TTrackElement TrackElement);
/// Return the link array position for the element at StartTVPosition that gives the closest link to the element a EndTVPosition. NB the StartTVPosition is expected to be a single track element as only positions 0 & 1 are checked
    int FindClosestLinkPosition(int Caller, int StartTVPosition, int EndTVPosition);
/// Return the opposite link position for the element at TrackVectorPosition with link position LinkPos, if the element is points and they are set against the exit then &Derail is returned true
    int GetAnyElementOppositeLinkPos(int Caller, int TrackVectorPosition, int LinkPos, bool &Derail);
/// Takes the ElementID value (an AnsiString) (e.g. "8-13", "N43-N127", etc) and returns the corresponding track vector position, if none is found then -1 is returned
    int GetTrackVectorPositionFromString(int Caller, AnsiString String, bool GiveMessages);
/// Returns the track vector position corresponding to the Hloc & VLoc positions, FoundFlag indicates whether an element is found or not, and if not -1 is returned
    int GetVectorPositionFromTrackMap(int Caller, int HLoc, int VLoc, bool &FoundFlag);
/// Returns the number of gaps in the railway
    int NumberOfGaps(int Caller);
/// Returns the number of separate platforms (not platform elements) at a given location, either a station or non station named location
    int NumberOfPlatforms(int Caller, AnsiString LocationName);
/// Similar to GetVectorPositionFromTrackMap but for inactive elements, a pair is returned because there can be up to 2 platforms at a specific position. If nothing found each element of pair is 0 & FoundFlag false
    TIMPair GetVectorPositionsFromInactiveTrackMap(int Caller, int HLoc, int VLoc, bool &FoundFlag);
/// Searches LocationNameMultiMap to check if the element pointed to by the TTrackVectorIterator has the name LocationName. If it finds it the pointer TLocationNameMultiMapIterator is returned.  If it fails ErrorString is set to an appropriate text to allow the calling function to report the error.  Otherwise it is set to "".
    TLocationNameMultiMapIterator FindNamedElementInLocationNameMultiMap(int Caller, AnsiString LocationName, TTrackVectorIterator TrackElement,
                                                                         AnsiString &ErrorString);
/// Return a reference to the inactive element at HLoc & VLoc, if no element is found an error is thrown
    TTrackElement &GetInactiveTrackElementFromTrackMap(int Caller, int HLoc, int VLoc);
/// Return a reference to the element at HLoc & VLoc for any map and any vector (used for SelectPrefDir in clipboard pref dir recovery)
    TTrackElement &GetTrackElementFromAnyTrackMap(int Caller, int HLoc, int VLoc, TTrackMap &Map, TTrackVector &Vector); //new at v2.9.0 for clipboard pref dirs & modified at v2.9.2 to make Map & Vector references
/// Return a reference to the element at HLoc & VLoc, if no element is found an error is thrown
    TTrackElement &GetTrackElementFromTrackMap(int Caller, int HLoc, int VLoc);
/// A range-checked version of InactiveTrackVector.at(At)
    TTrackElement &InactiveTrackElementAt(int Caller, int At);
/// A range-checked version of SelectVector.at(At)
    TTrackElement &SelectVectorAt(int Caller, int At);
/// A range-checked version of TrackVector.at(At)
    TTrackElement &TrackElementAt(int Caller, int At);
/// Takes an adjusted vector position value from either vector (if active, Position = -TruePos -1, if inactive, Position = TruePos) and returns a pointer to the relevant element
    TTrackVectorIterator GetTrackVectorIteratorFromNamePosition(int Caller, int Position);
/// A range-checked version of UserGraphicVector.at(At)
    TUserGraphicItem &UserGraphicVectorAt(int Caller, int At);
/// TrackElement.LocationName becomes 'Name' (for active and inactive elements) and, if TrackElement is a platform or named non-station location, any active element at the same HLoc & VLoc position has its ActiveTrackElementName set to 'Name'.
    void AddName(int Caller, TTrackVectorIterator TrackElement, AnsiString Name);
/// Examine TrackVector and whenever find a new gap pair enter it into GapMap
    void BuildGapMapFromTrackVector(int Caller);
/// Examine TrackVector, InactiveTrackVector and TextVector, and set the values that indicate the extent of the railway (HLocMin, VLocMin, HLocMax & VLocMax) for use in zoomed in and out displays and in saving railway images
    void CalcHLocMinEtc(int Caller);
/// Changes the LocationName in the name multimap to NewName at the location pointed to by the TLocationNameMultiMapIterator from whatever it was before.  Accepts null entries so that a formerly named element can have the name changed to "".
    void ChangeLocationNameMultiMapEntry(int Caller, AnsiString NewName, TLocationNameMultiMapIterator SNIterator);
/// Validity test
    void CheckGapMap(int Caller);
/// Validity test
    void CheckLocationNameMultiMap(int Caller);
/// Validity test
    void CheckMapAndInactiveTrack(int Caller);
/// Validity test
    void CheckMapAndTrack(int Caller);
/// After an element has been erased from the TrackVector, all the later elements are moved down one.  This function decrements the position values for all values above that of the erased element in the gap elements, TrackMap and LocationNameMultiMap.
    void DecrementValuesInGapsAndTrackAndNameMaps(int Caller, unsigned int VecPos);
/// After an element has been erased from the InactiveTrackVector, all the later elements are moved down one.  This function decrements the position values for all values above that of the erased element in both InactiveTrack2MultiMap and LocationNameMultiMap.
    void DecrementValuesInInactiveTrackAndNameMaps(int Caller, unsigned int VecPos);
/// All platform, concourse, footcrossing & non-station named location elements are able to have a LocationName allocated.
/**Track elements (including footcrossings) are able to have an ActiveTrackElementName allocated provided there is an adjacent platform or a NamedNonStationLocation.
To set these names the user selects a single named location element (not a footcrossing), enters the name, and this is then allocated as a LocationName
to all linked platform, concourse and footcrossing elements, and as an ActiveTrackElementName to all track elements adjacent to
platforms (inc footcrossing tracks if (but only if) they have a platform at that location). */
    void EnterLocationName(int Caller, AnsiString LocationName, bool AddingElements);
/// Examines LocationNameMultiMap and if the LocationName is found all elements at that H & V (in both active and inactive vectors) have the name erased both as a LocationName and a ActiveTrackElementName.  The LocationNameMultiMap is also rebuilt to correspond to the new names in the vectors.
    void EraseLocationAndActiveTrackElementNames(int Caller, AnsiString LocationName);
/// Erases all active and inactive track elements at HLocInput & VLocInput from the vectors, and, if any of these elements are named the entries are erased from LocationNameMultiMap and the corresponding name is removed from the display and from all other linked named elements
    void EraseTrackElement(int Caller, int HLocInput, int VLocInput, int &ErasedTrackVectorPosition, bool &TrackEraseSuccessfulFlag, bool InternalChecks);
/// With large railways only part of the railway is displayed on screen, and this function converts true (relative to the whole railway) H & V positions to screen (relative to the displayed screen) H & V positions
    void GetScreenPositionsFromTruePos(int Caller, int &ScreenPosH, int &ScreenPosV, int HPosTrue, int VPosTrue);
/// Converse of GetScreenPositionsFromTruePos except that in this function HLoc & VLoc are expressed in track elements (i.e. 16x16 pixel squares) rather than in single pixels
    void GetTrackLocsFromScreenPos(int Caller, int &HLoc, int &VLoc, int ScreenPosH, int ScreenPosV);
/// Converse of GetScreenPositionsFromTruePos
    void GetTruePositionsFromScreenPos(int Caller, int &HPos, int &VPos, int ScreenPosH, int ScreenPosV);
/// Examine all elements in the TrackVector and if have a valid length mark the relevant track using MarkOneLength
    void LengthMarker(int Caller, TDisplay *Disp);
/// Load all BarriersDownVector values from SessionFile
    void LoadBarriersDownVector(int Caller, std::ifstream &VecFile);
/// new at v2.4.0, load user graphics
    void LoadGraphics(int Caller, std::ifstream &VecFile, UnicodeString GraphicsPath);
/// Load track elements (active & inactive) from the file into the relevant vectors and maps, and try to link the resulting track
    void LoadTrack(int Caller, std::ifstream &VecFile, bool &GraphicsFollow);
/// Mark on screen a track element according to its length and speed limit if either of these differ from their default values
    void MarkOneLength(int Caller, TTrackElement TE, bool FirstTrack, TDisplay *Disp);
/// Called during track building or pasting, when an element identified by CurrentTag (i.e. its SpeedTag value) is to be placed at position HLocInput & VLocInput.  If the element can be placed it is displayed and added to the relevant vector, and if named its name is added to LocationNameMultiMap. At v2.2.0 'Aspect' added so can distinguish between adding and pasting track
    void PlotAndAddTrackElement(int Caller, int CurrentTag, int Aspect, int HLocInput, int VLocInput, bool &TrackPlottedFlag, bool InternalChecks);
///new at v2.2.0 - as PlotAndAddTrackElement but keeping speed & length attributes (for pasting) and also pasting location names
    void PlotPastedTrackElementWithAttributes(int Caller, TTrackElement TempTrackElement, int HLocInput, int VLocInput, bool &TrackLinkingRequiredFlag,
                                              bool InternalChecks);
/// Just replot the basic track elements at a level crossing (for flashing)
    void PlotLCBaseElementsOnly(int Caller, TBarrierState State, int BaseElementSpeedTag, int HLoc, int VLoc, int TypeOfRoute, TDisplay *Disp);
/// Plot & open (to trains) all level crossings linked to TrackElement (Manual true = manually lowered, colour is green)
    void PlotLoweredLinkedLevelCrossingBarriers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, int TypeOfRoute, TDisplay *Disp, bool Manual);
/// Plot LC elements without any base elements, and set LCPlotted true - used in ClearandRebuildRailway (Manual true = manually lowered, colour is green)
    void PlotPlainLoweredLinkedLevelCrossingBarriersAndSetMarkers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, TDisplay *Disp, bool Manual);
/// Plot LC elements without any base elements, and set LCPlotted true - used in ClearandRebuildRailway
    void PlotPlainRaisedLinkedLevelCrossingBarriersAndSetMarkers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, TDisplay *Disp);
/// Plot & close (to trains) all level crossings linked to TrackElement - always plots as red - auto
    void PlotRaisedLinkedLevelCrossingBarriers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, TDisplay *Disp);
/// Plots either a LC or a blank element to flash manual LCs in zoomout mode
    void PlotSmallFlashingLinkedLevelCrossings(int Caller, int HLoc, int VLoc, Graphics::TBitmap *GraphicPtr, TDisplay *Disp);
/// Plots a gap on screen - may be set or unset
    void PlotGap(int Caller, TTrackElement TrackElement, TDisplay *Disp);
/// Plot points on screen according to how they are set (Attribute value), or, with both fillets if BothFillets is true (the fillet is the bit that appears to move when points are changed)
    void PlotPoints(int Caller, TTrackElement TrackElement, TDisplay *Disp, bool BothFillets);
/// Plot signals on screen according to their aspect (Attribute value)
    void PlotSignal(int Caller, TTrackElement TrackElement, TDisplay *Disp);
/// Plot platforms if any for a signal graphic - plotted before signal so shows through transparent signal pixels
    void PlotSignalPlatforms(int Caller, int HLoc, int VLoc, TDisplay *Disp);
/// Plot on screen the zoomed-out railway
    void PlotSmallRailway(int Caller, TDisplay *Disp);
/// Plot on screen in zoomed-out mode and in gap setting mode a small red square corresponding to the gap position that is waiting to have its matching gap selected (see also ShowSelectedGap)
    void PlotSmallRedGap(int Caller);
/// Add all LCs to LCVector - note that this contains all LC elements whether linked to others or not
    void PopulateLCVector(int Caller);
/// Clears the existing LocationNameMultiMap and rebuilds it from TrackVector and InactiveTrackVector. Called after the track is linked as many of the vector positions are likely to change - called from RepositionAndMapTrack(); after names are changed in EraseLocationAndActiveTrackElementNames; and after the name changes in EnterLocationName.
    void RebuildLocationNameMultiMap(int Caller);
/// Called by TInterface::ClearandRebuildRailway to replot all the active and inactive track elements and text, BothPointFillets indicates whether points are to be plotted according to how they are set - for operation, or with both fillets - when not operating or during development (the fillet is the bit that appears to move when points are changed)
    void RebuildTrackAndText(int Caller, TDisplay *Disp, bool BothPointFilletsAndBasicLCs);
/// rebuild user graphics
    void RebuildUserGraphics(int Caller, TDisplay *Disp);
/// Track elements have members that indicates whether and on what track a train is present (TrainIDOnElement, TrainIDOnBridgeTrackPos01 and TrainIDOnBridgeTrackPos23).  This function resets them all to their non-train-present state of -1. Called by TTrainController::UnplotTrains
    void ResetAllTrainIDElements(int Caller);
/// Called by EraseTrackElement after the element has been erased and the vector positions changed, in order to reset a matching gaps if the erased element was a set gap
    void ResetAnyNonMatchingGaps(int Caller);
/// Set all LC attributes to 0 (closed to trains)
    void ResetLevelCrossings(int Caller);
/// Called on exit from operation to reset all points to non-diverging or to left fork (Attribute = 0)
    void ResetPoints(int Caller);
/// Called on exit from operation to reset all signals to red (Attribute = 0)
    void ResetSignals(int Caller);
/// Save all changing vector values (used for error file)
    void SaveChangingLCVector(int Caller, std::ofstream &OutFile);
/// Save all vector values to the session file
    void SaveSessionBarriersDownVector(int Caller, std::ofstream &OutFile);
/// Save all active and inactive track elements to VecFile
    void SaveTrack(int Caller, std::ofstream& VecFile, bool GraphicsFollow);
/// save graphics
    void TTrack::SaveUserGraphics(int Caller, std::ofstream &VecFile);
/// Checks all locations that are adjacent to the one entered for linked named location elements.
/**If any LocationName is found in any of the linked elements, that name is used for all elements that are now linked to it. The location entered doesn't need
to be a FixedNamedLocationElement and there doesn't even need to be an element there.
Used during EraseTrackElement (in which case the SpeedTag is that for the element that is erased) and PlotAndAddTrackElement,
to bring the named location and timetable naming up to date with the deletion or insertion.*/
    void SearchForAndUpdateLocationName(int Caller, int HLoc, int VLoc, int SpeedTag);
/// Work through all elements in TrackVector setting all lengths & speed limits to default values - including both tracks for 2-track elements
    void SetAllDefaultLengthsAndSpeedLimits(int Caller);
/// Set TypeOfRoute value to 2 to indicate barriers manually closed
    void SetBarriersDownLCToManual(int Caller, int HLoc, int VLoc);
/// Convert the position values for the TrackElement into an identification string and load in ElementID
    void SetElementID(int Caller, TTrackElement &TrackElement);
/// Set LC attribute at H & V; 0=closed to trains, 1 = open to trains, 2 = changing state = closed to trains
    void SetLCAttributeAtHV(int Caller, int HLoc, int VLoc, int Attr);
/// Set linked LC attributes; 0=closed to trains, 1 = open to trains, 2 = changing state = closed to trains
    void SetLinkedLevelCrossingBarrierAttributes(int Caller, int HLoc, int VLoc, int Attr);
/// Set all TypeOfRoute values to 2 for all linked LCs to indicate manually lowered
    void SetLinkedManualLCs(int Caller, int HLoc, int VLoc);
/// Called when trying to link track and when a name changed when track already linked.
/**Examines all track elements that have ActiveTrackElementName set, sums the number of consecutive elements with the same name,
and sets the EntryLink values for the front of train stop points for each direction.  For stations (not non-station named
locations) of length n, where n > 1, stop element is [floor((n+1)/2) + 1] from each end (unless buffers at one or both ends in
which case stop points are the end elements).  Note that for a single element the stop point is the element itself (formula
doesn't apply).  For NamedNonStationLocations the stop points are at the end elements to allow trains to stack up. */
    void SetStationEntryStopLinkPosses(int Caller);
/// Called during gap setting to mark a gap with a red circle - after which the program awaits user selection of the matching gap
    void ShowSelectedGap(int Caller, TDisplay *Disp);
/// Empty the track and inactive track vectors, the corresponding track maps, and LocationNameMultiMap
    void TrackClear(int Caller);
/// Insert TrackElement into the relevant vector and map, and, if named, insert the name in LocationNameMultiMap
    void TrackPush(int Caller, TTrackElement TrackElement);
/// handles moving of user graphics
    void UserGraphicMove(int Caller, int HPosInput, int VPosInput, int &UserGraphicItem, int &UserGraphicMoveHPos, int &UserGraphicMoveVPos,
                         bool &UserGraphicFoundFlag);
/// Called by TInterface::SaveOperatingImage1Click to add all track & text to the image file in their operating state
    void WriteOperatingTrackAndTextToImage(int Caller, Graphics::TBitmap *Bitmap);
/// Called by SaveImageNoGridMenuItemClick, SaveImageAndGridMenuItemClick amd SaveImageAndPrefDirsMenuItemClick to add all track element graphics to the image file in their non-operating state
    void WriteGraphicsToImage(int Caller, Graphics::TBitmap *Bitmap);
/// Called by TInterface::SaveImageNoGrid1Click, TInterface::SaveImageAndGrid1Click and TInterface::SaveImageAndPrefDirs1Click to add all track & text to the image file in the non-operating state
    void WriteTrackAndTextToImage(int Caller, Graphics::TBitmap *Bitmap);

/// Constructor, only one object of this class
    TTrack();
/// Destructor
    ~TTrack();
};

// ---------------------------------------------------------------------------

extern TTrack *Track;
///< the object pointer, object created in InterfaceUnit

// ---------------------------------------------------------------------------
// PrefDir & Route functions
// ---------------------------------------------------------------------------

enum TTruncateReturnType
///< a flag used during route truncation to indicate the nature of the selected element, it could be not in a route (NotInRoute), in a route but invalid (InRouteFalse), or in a route and valid (InRouteTrue)
{
    NotInRoute, InRouteTrue, InRouteFalse
};

enum TPrefDirRoute
///< used in TOnePrefDir::PrefDirMarker to indicate whether the function is being called for a preferred direction (PrefDirCall) or a route (RouteCall)
{
    PrefDirCall, RouteCall
};

// ---------------------------------------------------------------------------

/// The basic preferred direction class, consisting of any number of elements with preferred directions set. Used during setting up preferred directions and track lengths (ConstructPrefDir), and for all completed preferred directions in the railway (EveryPrefDir)
class TOnePrefDir
{
public: // don't want descendant (TOneRoute) to access the PrefDir4MultiMap

    typedef std::multimap<THVPair, unsigned int, TMapComp>TPrefDir4MultiMap;
///< HLoc&VLoc as a pair, and PrefDirVectorPosition, can be up to 4 values at any H&V
    typedef std::multimap<THVPair, unsigned int, TMapComp>::iterator TPrefDir4MultiMapIterator;
    typedef std::pair<THVPair, unsigned int>TPrefDir4MultiMapEntry;

    TPrefDir4MultiMap PrefDir4MultiMap;
///< the pref dir multimap - up to 4 values (up to 2 tracks per element each with 2 directions)

private:
// inline functions

/// Empty the existing vectors & map
    void ClearPrefDir()
    {
        PrefDirVector.clear();
        SearchVector.clear();
        PrefDir4MultiMap.clear();
    }

// functions defined in .cpp file

/// Retrieves a PrefDir4MultiMap iterator to the PrefDir element at PrefDirVectorPosition.  Used during ErasePrefDirElementAt to erase the relevant element in the multimap.  If nothing is found this is an error but the error message is given in the calling function.
    TPrefDir4MultiMapIterator GetExactMatchFrom4MultiMap(int Caller, unsigned int PrefDirVectorPosition, bool &FoundFlag);
/// Store a single pref dir element in the vector & map
    void StorePrefDirElement(int Caller, TPrefDirElement LoadPrefDirElement);
/// Erase a single element from PrefDirVector and 4MultiMap, decrementing the remaining PrefDirElementNumbers in 4MultiMap if they are greater than the erased value.
    void ErasePrefDirElementAt(int Caller, int PrefDirVectorPosition);
/// Diagnostic validity check
    void CheckPrefDir4MultiMap(int Caller);
/// Called after ErasePrefDirElementAt to decrement the remaining PrefDirElementNumbers in 4MultiMap if they are greater than the erased value.
    void DecrementPrefDirElementNumbersInPrefDir4MultiMap(int Caller, unsigned int ErasedElementNumber);

protected: // descendant (TOneRoute) can access these

    static const int PrefDirSearchLimit = 30000;
///< limit to the number of elements searched in attempting to find a preferred direction

// [dropped as not a good strategy because gaps interfered with direct line searches - instead introduced TotalSearchCount and now use that
// to limit searches. Leave in though in case rethink strategy later on]  Search limit values - set the H&V limits when searching for
// the next pref dir element (or route as inherited by TOneRoute), all points on search path must lie within 15 elements greater than
// the box of which the line between start and finish is a diagonal (else search takes too long)
    int SearchLimitLowH;
    int SearchLimitHighH;
    int SearchLimitLowV;
    int SearchLimitHighV;

    int TotalSearchCount;
///< counts search elements, used to abort searches (prefdirs or routes) if reaches too high a value

// functions defined in .cpp file

/// Called by GetStartAndEndPrefDirElements...
/** which in turn is called by PresetAutoSigRoutesButtonClick. Checks for a diagonal link in
the autosigsroute being fouled by an adjacent track with a corresponding link that meets at the diagonal link, and if it is it
returns true and prevents the route being set.  Note that adjacent track consisting of buffers, gaps and continuations at the
diagonal link are also excluded though they need not be, but it makes the check code simpler and such adjacent track is untidy
and can be modelled better anyway.  Added at v2.1.0. */
    bool PresetAutoRouteDiagonalFouledByTrack(int Caller, TPrefDirElement ElementIn, int XLink);
/// Checks ElementIn and returns true only if a single prefdir set at that H&V, with EntryPos giving entry position, not points, crossovers, signals with wrong direction set, or buffers. Added at v1.2.0
    bool PresetAutoRouteElementValid(int Caller, TPrefDirElement ElementIn, int EntryPos);
/// Try to find a selected element from a given start position.  Enter with CurrentTrackElement stored in the PrefDirVector, XLinkPos set to the link to search on, & SearchVector cleared unless entered recursively.  Function is a continuous loop that exits when find required element (returns true) or reaches a buffer or continuation or otherwise fails a search condition (returns false).
    bool SearchForPrefDir(int Caller, TTrackElement TrackElement, int XLinkPos, int RequiredPosition);
/// Although there may be up to four entries at one H & V position this function gets just one. It is used in EraseFromPrefDirVectorAnd4MultiMap by being called as many times as there are PrefDir elements at H & V
    int GetOnePrefDirPosition(int Caller, int HLoc, int VLoc);
/// Called after a successful search to add the elements from the search vector to the pref dir vector
    void ConvertPrefDirSearchVector(int Caller);

public:

//    typedef std::vector<TPrefDirElement> TPrefDirVector; //dropped here because used as a forward declaration earlier
    typedef std::vector<TPrefDirElement>::iterator TPrefDirVectorIterator;
    typedef std::vector<TPrefDirElement>::const_iterator TPrefDirVectorConstIterator;

    TPrefDirVector PrefDirVector, SearchVector;
///< pref dir vectors, first is the main vector, second used to store search elements temporarily

// inline functions

/// Return the vector size
    unsigned int PrefDirSize() const
    {
        return(PrefDirVector.size());
    }

/// Return the vector size
    unsigned int SearchVectorSize() const
    {
        return(SearchVector.size());
    }

/// Empty the existing preferred direction vector & map - for use by other classes
    void ExternalClearPrefDirAnd4MultiMap()
    {
        ClearPrefDir();
    }

    // functions defined in .cpp file

    /// Determines whether the preferred direction pointed to has another pref dir in the opposite direction set (returns true) or not
    bool TOnePrefDir::BiDirectionalPrefDir(int Caller, TPrefDir4MultiMapIterator PDPtr);
/// Called before PrefDir loading as part of the FileIntegrityCheck function in case there is an error in the file. Very similar to LoadPrefDir but with value checks instead of storage in PrefDirVector.
    bool CheckOnePrefDir(int Caller, int NumberOfActiveElements, std::ifstream &VecFile);
/// Used when setting preferred directions, true if able to finish at the last selected element (can't finish if there is only one element or if end on leading points)
    bool EndPossible(int Caller, bool &LeadingPoints);
/// Finds a pref dir element that links to another element at given vector number and link number & position, returns true if found with linked vector number, true if buffer or continuation with link at blank end & linked vector number = -1, or false if not found with vector number == -1
    bool FindLinkingPrefDir(int Caller, int PrefDirVectorNumber, int LinkNumberPos, int LinkNumber, int &LinkedPrefDirVectorNumber);
/// Used when continuing a chain of preferred directions or element lengths. Tries to find a set of linked tracks between the last selected element and the one at HLoc & VLoc, and returns true if it finds one.  FinishElement is returned true if the element selected is a buffer or continuation - in which case the chain is complete
    bool GetNextPrefDirElement(int Caller, int HLoc, int VLoc, bool &FinishElement);
/// Called when searching for start and end PrefDirElements when setting up automatic signals routes in PreStart mode
    bool GetStartAndEndPrefDirElements(int Caller, TPrefDirElement &StartElement, TPrefDirElement &EndElement, int &LastIteratorValue);
/// Used when beginning a chain of preferred directions or element lengths. Enter with HLoc & VLoc set to selected element & check if selected element is a valid track element, return false if not, if it is, store it as the first entry in PrefDirVector and return true
    bool GetPrefDirStartElement(int Caller, int HLoc, int VLoc);
/// Called during PrefDir build or distance setting. It truncates at & including the first element in the PrefDir vector that matches H & V. After the truncate the final element of the remaining PrefDir has its data members reset to the same defaults as would be the case if the PrefDir had been built up to that point - i.e. for first element or a leading point.
    bool GetPrefDirTruncateElement(int Caller, int HLoc, int VLoc);
/// Checks that all elements in PrefDirVector have been properly set, i.e. don't have their default values, and that every element is connected to the next element
    bool ValidatePrefDir(int Caller);
/// Return the vector position of the last element in the vector (i.e. one less than the vector size)
    int LastElementNumber(int Caller) const;
/// Return a pointer to the last element in the vector
    TPrefDirVectorIterator LastElementPtr(int Caller);
/// Return a non-modifiable element at PrefDirVector position 'At'
    const TPrefDirElement &GetFixedPrefDirElementAt(int Caller, int At) const;
/// Return a modifiable element at PrefDirVector position 'At'
    TPrefDirElement &GetModifiablePrefDirElementAt(int Caller, int At);
/// Return a non-modifiable element at SearchVector position 'At'
    const TPrefDirElement &GetFixedSearchElementAt(int Caller, int At) const;
/// Return a modifiable element at SearchVector position 'At'
    TPrefDirElement &GetModifiableSearchElementAt(int Caller, int At);
/// Used when setting element lengths, returns in &OverallDistance the overall distance for the selected chain of elements and also the speed limit in &OverallSpeedLimit, which is set to -1 if the speed limits vary over the chain
    void CalcDistanceAndSpeed(int Caller, int &OverallDistance, int &OverallSpeedLimit, bool &LeadingPointsAtLastElement);

/// Store a single pref dir element in the vector & map - used by other classes
    void ExternalStorePrefDirElement(int Caller, TPrefDirElement LoadPrefDirElement)
    {
        StorePrefDirElement(6, LoadPrefDirElement);
    }
/// Return up to 4 vector positions for a given HLoc & VLoc; unused values return -1
    void GetVectorPositionsFromPrefDir4MultiMap(int Caller, int HLoc, int VLoc, bool &FoundFlag, int &PrefDirPos0, int &PrefDirPos1, int &PrefDirPos2,
                                                int &PrefDirPos3);
/// Old version of LoadPrefDir, used during development when the save format changed so the old files could be loaded prior to resaving in the new format
    void LoadOldPrefDir(int Caller, std::ifstream &VecFile);
/// Load a vector and map of preferred directions from the file
    void LoadPrefDir(int Caller, std::ifstream &VecFile);
/// PrefDir and route track marker, including direction markers.
/**  Function used for both PrefDirs (PrefDirRoute == PrefDirCall) and routes (PrefDirRoute == RouteCall).
The graphics for marker colours and direction are already stored in all PrefDirElements in TOnePrefDir and TOneRoute, and this
function is called to display them, all in the case of a PrefDir, but for a route only the first and last elements have direction
markers. No markers are displayed if a train is present on an element.  Also no display if EXGraphicPtr not set.  If building a
PrefDir (BuildingPrefDir true) then the start and end rectangles are also displayed. */
    void PrefDirMarker(int Caller, TPrefDirRoute PrefDirRoute, bool BuildingPrefDir, TDisplay *Disp) const;
/// Save the preferred direction vector to a file
    void SavePrefDirVector(int Caller, std::ofstream &VecFile);
/// Save the search vector to a file
    void SaveSearchVector(int Caller, std::ofstream &VecFile);
/// Used when creating a bitmap image to display preferred directions (as on screen during 'Set preferred direction' mode)
    void WritePrefDirToImage(int Caller, Graphics::TBitmap *Bitmap);

// EveryPrefDir (declared in InterfaceUnit.h) functions (all external)

/// Check loaded PrefDir against loaded track, and if discrepancies found clear EveryPrefDir & PrefDir4MultiMap, messages are given by the calling routine.  Return true for OK
    bool CheckPrefDirAgainstTrackVectorNoMessage(int Caller);
/// Check loaded PrefDir against loaded track, and if discrepancies found give message & clear EveryPrefDir & PrefDir4MultiMap.
    void CheckPrefDirAgainstTrackVector(int Caller);
/// Used when a preferred direction has been set to add all the elements to EveryPrefDir, except when they already exist in EveryPrefDir
    void ConsolidatePrefDirs(int Caller, TOnePrefDir *InputPrefDir);
/// Erase element at HLoc and VLoc from the PrefDirVector and from the 4MultiMap. Note that this entails erasing up to four elements (2 directions and 2 tracks for 4-entry elements).
    void EraseFromPrefDirVectorAnd4MultiMap(int Caller, int HLoc, int VLoc);
/// Similar to PrefDirMarker but used only to mark EveryPrefDir - red for unidirectional PrefDir & green for bidirectional. Colours taken from the route colours. Plot red first so green overwrites for bidirectional points.
    void EveryPrefDirMarker(int Caller, TDisplay *Disp);
/// After a track element is erased the preferred direction elements are likely to be affected. This function erases any preferred direction elements that either correspond to the erased track element, or were linked to it
    void RealignAfterTrackErase(int Caller, int ErasedTrackVectorPosition);
/// Called after the track vector has been rebuilt following linking, to rebuild the preferred direction vector to correspond to the element positions in the rebuilt track vector. Doesn't affect the preferred direction multimap.
    void RebuildPrefDirVector(int Caller);
};

// ---------------------------------------------------------------------------

/// A descendent of TOnePrefDir used for routes.  Used during contruction of a route (ConstructRoute) and also for all completed routes, when each route is saved as an entry in the AllRoutesVector (see TAllRoutes)
class TOneRoute : public TOnePrefDir
{
public:
/// A single flashing element of a route that flashes during setting
    class TRouteFlashElement
    {
public:
        int HLoc, VLoc, TrackVectorPosition;
///< element values
        Graphics::TBitmap *OriginalGraphic, *OverlayGraphic;
///< the two graphics, non route-coloured and route-coloured respectively, these are
        ///< displayed alternately during flashing
    };

/// The flashing route
    class TRouteFlash
    {
public:
        std::vector<TRouteFlashElement>RouteFlashVector;
        bool OverlayPlotted;
///< flag indicating the graphic that is currently displayed, true for the overlay (route-coloured)

// both external
        void PlotRouteOverlay(int Caller);
///< display the overlay (route-coloured) graphic
        void PlotRouteOriginal(int Caller);
///< display the original (non route-coloured) graphic
    };

    static const int RouteSearchLimit = 30000;
///< limit to the number of elements searched in attempting to find a route

    IDInt ReqPosRouteID;
///< the route ID number of the route that is being extended backwards during route building, not needed for
    ///< session saves as routes in build are not saved in sessions
    IDInt StartSelectionRouteID;
///< the route ID number of the route that is being extended forwards during route building, not
    ///< needed for session saves as routes in build are not saved in sessions

    int RouteID;
///< the ID number of the route, this is needed for session saves
    int StartRoutePosition;
///< TrackVectorPosition of the StartElement(s) set when the starting position of a new route is selected, note that although there may be two StartElements (as there can be two preferred directions on a single element), there is only one TrackVectorPosition as the element is the same for both
    TPrefDirElement StartElement1, StartElement2;
///< the two preferred direction elements corresponding to the starting position of a new route
    TRouteFlash RouteFlash;
///< the class member that allows the route to flash during setting up (see TRouteFlash above)

// inline functions

/// Empty the route of any stored elements
    void ClearRoute()
    {
        PrefDirVector.clear();
        SearchVector.clear();
    }

/// Erase a single route element
    void EraseRouteElementAt(TPrefDirElement *RouteElementPtr)
    {
        PrefDirVector.erase(RouteElementPtr);
    }

/// Store a single route element in the PrefDirVector
    void StoreRouteElementInPrefDirVector(TPrefDirElement LoadPrefDirElement)
    {
        LoadPrefDirElement.IsARoute = true;
        PrefDirVector.push_back(LoadPrefDirElement);
    }

// functions defined in .cpp file

/// Used when setting signal aspects for a route by working forwards through the route to see what the next forward signal aspects is, because this determines all the rearward signal aspects.
    bool FindForwardTargetSignalAttribute(int Caller, int &NextForwardLinkedRouteNumber, int &Attribute) const;
/// Set the starting conditions for a non-preferred (i.e. unrestricted) route selection beginning on HLoc & VLoc
    bool GetNonPreferredRouteStartElement(int Caller, int HLoc, int VLoc, bool Callon);
/// Try to find a set of linked tracks between the route start element and the one at HLoc & VLoc.  If find one return true, set &PointsChanged to true if any points need to be changed and &ReqPosRouteID to the route ID of the existing route to attach to, if there is one, and -1 if not
    bool GetNextNonPreferredRouteElement(int Caller, int HLoc, int VLoc, bool Callon, IDInt &ReqPosRouteID, bool &PointsChanged);
/// Set the starting conditions for a preferred direction or automatic signal route selection beginning on HLoc & VLoc
    bool GetPreferredRouteStartElement(int Caller, int HLoc, int VLoc, TOnePrefDir *EveryPrefDir, bool AutoSigsFlag);
/// Try to find a set of linked tracks that lie on preferred directions between the route start element and the one at HLoc & VLoc.  If find one return true, set &PointsChanged to true if any points need to be changed and &ReqPosRouteID to the route ID of the existing route to attach to, if there is one, and -1 if not
    bool GetNextPreferredRouteElement(int Caller, int HLoc, int VLoc, TOnePrefDir *EveryPrefDir, bool ConsecSignals, bool AutoSigsFlag,
                                      IDInt &ReqPosRouteID, bool &PointsChanged);
/// Called by GetNextNonPreferredRouteElement and GetNextPreferredRouteElement to check whether or not any points on the selected route need to be changed
    bool PointsToBeChanged(int Caller) const;
/// Called by GetNextNonPreferredRouteElement to carry out the search for linked track, and also called recursively
    bool SearchForNonPreferredRoute(int Caller, TTrackElement CurrentTrackElement, int XLinkPos, int RequiredPosition, IDInt ReqPosRouteID);
/// Called by GetNextPreferredRouteElement to carry out the search for a valid route, and also called recursively
    bool SearchForPreferredRoute(int Caller, TPrefDirElement PrefDirElement, int XLinkPos, int RequiredPosition, IDInt ReqPosRouteID, TOnePrefDir *EveryPrefDir,
                                 bool ConsecSignals, int EndSelectPosition, bool AutoSigsFlag);
/// Called by TAllRoutes::SetAllRearwardsSignals to set rearwards signals from a specified starting position.  If a train is found during the rearwards search then this function flags the fact so that the calling function can change its behaviour with respect to further rearwards signal aspects.
    bool SetRearwardsSignalsReturnFalseForTrain(int Caller, int &Attribute, int PrefDirVectorStartPosition) const;
/// Called after a non-preferred (i.e. unrestricted) route has been selected and has finished flashing, to add it to the AllRoutesVector
    void ConvertAndAddNonPreferredRouteSearchVector(int Caller, IDInt ReqPosRouteID);
/// Called after a preferred (i.e. preferred direction or automatic signals) route has been selected and has finished flashing, to add it to the AllRoutesVector
    void ConvertAndAddPreferredRouteSearchVector(int Caller, IDInt ReqPosRouteID, bool AutoSigsFlag);
/// Cancel a route immediately if a train occupies it when travelling in the wrong direction (or occupies a crossover on a non-route line when the other track is in a route)
    void ForceCancelRoute(int Caller);
/// Examines the route to see whether the element at H & V is in the route, and if not returns a ReturnFlag value of NotInRoute.
/** If it is in a route but the element selected is invalid, then a message is given and returns with a ReturnFlag value of
InRouteFalse.  Otherwise the route is truncated at and including the element that matches H & V with a ReturnFlag value of
InRouteTrue.  Selection invalid if select a bridge; trying to leave a single element; last
element to be left not a signal (for PrefDirRoute or AutoSigsFlag set); last element to be left a bridge, points or crossover
(for not PrefDirRoute & AutoSigsFlag not set), or part of route locked. */
    void GetRouteTruncateElement(int Caller, int HLoc, int VLoc, bool PrefDirRoute, TTruncateReturnType &ReturnFlag);
/// Used when creating a bitmap image to display the route colours and direction arrows (as on screen during operation) for an operating railway
    void RouteImageMarker(int Caller, Graphics::TBitmap *Bitmap) const;
/// After a route has been selected successfully this function sets all LC change values appropriately for the selected route type and location
    void SetLCChangeValues(int Caller, bool PrefDirRoute);
/// Called when setting unrestricted routes to set the route element values appropriately after a successful search has been conducted.  It isn't needed for preferred routes because the element values are obtained from the already set preferred direction elements
    void SetRemainingSearchVectorValues(int Caller);
/// After a route has been selected successfully this function sets all RouteFlash (see above) values appropriately for the selected route type and location
    void SetRouteFlashValues(int Caller, bool AutoSigsFlag, bool PrefDirRoute);
/// Set values for EXGraphicPtr and EntryDirectionGraphicPtr for all elements in SearchVector so that the route displays with the correct colour
    void SetRouteSearchVectorGraphics(int Caller, bool AutoSigsFlag, bool PrefDirRoute);
/// Called when setting a route to set all points appropriately
    void SetRoutePoints(int Caller) const;
/// Called when setting a route to set all points appropriately.  Also called when a new train is added at a position where a route has been set, when it is necessary to set the next rearwards signal to red, the next yellow etc
    void SetRouteSignals(int Caller) const;
};

// ---------------------------------------------------------------------------

/// Handles data and functions relating to all routes on the railway
class TAllRoutes
{
public:

/// Handles routes that are locked because of approaching trains
    class TLockedRouteClass
    {
    public:
        int RouteNumber;
///< the vector position number of the relevant route in AllRoutesVector
        unsigned int TruncateTrackVectorPosition;
///< the TrackVector position of the element selected for truncation
        unsigned int LastTrackVectorPosition;
///< the TrackVector position of the last (i.e. most forward) element in the route
        int LastXLinkPos;
///< the XLinkPos value of the last (i.e. most forward) element in the route
        TDateTime LockStartTime;
///< the timetable time at which the route is locked, to start the 2 minute clock
    };

    enum TRouteType
    {
        NoRoute, NotAutoSigsRoute, AutoSigsRoute
    } RouteType;
///< distinguishes between automatic signals routes and other types, or no route at all (where this is used there is no need to distinguish between preferred direction and unrestricted routes)

    typedef std::vector<TOneRoute>TAllRoutesVector;
///< the vector class that holds all the railway routes
    typedef std::vector<TOneRoute>::iterator TAllRoutesVectorIterator;

    typedef std::vector<TLockedRouteClass>TLockedRouteVector;
///< the vector class that holds all locked routes
    typedef std::vector<TLockedRouteClass>::iterator TLockedRouteVectorIterator;

    typedef std::pair<int, unsigned int>TRouteElementPair;
///< defines a specific element in a route, the first (int) value is the vector position in the AllRoutesVector, and the second (unsigned int) value is the vector position of the element in the route's PrefDirVector
    typedef std::multimap<THVPair, TRouteElementPair, TMapComp>TRoute2MultiMap;
///< the multimap class holding the elements of all routes in the railway.  The first entry is the HLoc & VLoc pair values of the route element, and the second is the TRouteElementPair defining the element.  There are a maximum of 2 elements per HLoc & VLoc location
    typedef TRoute2MultiMap::iterator TRoute2MultiMapIterator;
    typedef std::pair<THVPair, TRouteElementPair>TRoute2MultiMapEntry;

/// Used to store relevant values when a call-on found, ready for plotting an unrestricted route
    class TCallonEntry
    {
    public:
        bool RouteOrPartRouteSet;
///< whether or not a route or part route already plotted
        int RouteStartPosition;
///< the stop signal trackvectorposition
        int PlatformPosition;
///< the first platform trackvectorposition

/// Constructor
        TCallonEntry::TCallonEntry(bool RouteOrPartRouteSetIP, int RouteStartPositionIP, int PlatformPositionIP)
        {
            RouteOrPartRouteSet = RouteOrPartRouteSetIP;
            RouteStartPosition = RouteStartPositionIP;
            PlatformPosition = PlatformPositionIP;
        }
    };

    std::vector<TCallonEntry>CallonVector;
///< the store of all call-on entries

    bool LockedRouteFoundDuringRouteBuilding;
///< this flags the fact that a locked route has been found during route building in an existing linked route which is erased prior to its elements being added to the new route.

// the following variables store the locked route values for reinstating after a locked route has been found during route building in an
// existing linked route which is erased prior to its elements being added to the new route.  The locked route is erased in
// ClearRouteDuringRouteBuildingAt, and is reinstated in ConvertAndAddPreferredRouteSearchVector or ConvertAndAddNonPreferredRouteSearchVector.
    int LockedRouteLastXLinkPos;
    unsigned int LockedRouteTruncateTrackVectorPosition;
    unsigned int LockedRouteLastTrackVectorPosition;
    TDateTime LockedRouteLockStartTime;
// end of locked route values

    bool RebuildRailwayFlag;
///< this is set whenever a route has to be cancelled forcibly in order to force a ClearandRebuildRailway at the next clock tick if not in zoom-out mode to clear the now cancelled route on the display
    bool RouteTruncateFlag;
///< used to flag the fact that a route is being truncated on order to change the behaviour of signal aspect setting in SetRearwardsSignalsReturnFalseForTrain

    const float LevelCrossingBarrierUpDelay;
///< the full value in seconds for which the level crossing flashes prior to closing to trains
    const float LevelCrossingBarrierDownDelay;
///< the full value in seconds for which the level crossing flashes prior to opening to trains
    const float PointsDelay;
///< the value in seconds for which points flash prior to being changed.  Used for the points flash period when changing points manually and for the route flash period when points have to be changed
    const float SignalsDelay;
///< the value in seconds for which signals flash prior to being changed.  Used for the route flash period when points don't have to be changed
    int NextRouteID;
///< stores the value for the route ID number that is next to be built
    TAllRoutesVector AllRoutesVector;
///< the vector that stores all the routes on the railway
    TLockedRouteVector LockedRouteVector;
///< the vector that stores all the locked routes on the railway
    TOneRoute SignallerRemovedTrainAutoRoute;
///< if train was on an AutoSigsRoute when removed then this stores the route so that signals can be reset
    TRoute2MultiMap Route2MultiMap;
///< the map that stores the elements of all routes on the railway (see TRoute2MultiMap for more info)

// inline functions

/// Returns the number of routes in the railway
    unsigned int AllRoutesSize() const
    {
        return(AllRoutesVector.size());
    }

/// Erases all routes from AllRoutesVector and from Route2MultiMap
    void AllRoutesClear()
    {
        AllRoutesVector.clear();
        Route2MultiMap.clear();
    }
/// Functions defined in .cpp file
    bool CheckForLoopingRoute(int Caller, int EndPosition, int EndXLinkPos, int StartPosition); // return true if route loops back on itself
/// Performs an integrity check on the routes stored in a session file and returns false if there is an error
    bool CheckRoutes(int Caller, int NumberOfActiveElements, std::ifstream &InFile);
/// The track geometry allows diagonals to cross without occupying the same track element, so when route plotting it is necessary to check if there is an existing route or a train on such a crossing diagonal.  Returns true for a fouled (i.e. fouled by a route or a train) diagonal. New at v1.2.0
    bool DiagonalFouledByRouteOrTrain(int Caller, int HLoc, int VLoc, int DiagonalLinkNumber);
/// As above but only checks for a route (may or may not be a train present (new at v1.2.0)
    bool DiagonalFouledByRoute(int Caller, int HLoc, int VLoc, int DiagonalLinkNumber);
/// If a route is present at H, V & Elink returns true with RouteNumber giving vector position in AllRoutes vector.  Returns false for anything else including no element or route at H & V etc. New at v1.2.0
    bool TAllRoutes::FindRouteNumberFromRoute2MultiMapNoErrors(int Caller, int HLoc, int VLoc, int ELink, int &RouteNumber);
/// Examines all routes and for each uses GetRouteTruncateElement to see if the element at H & V is present in that route.
/**  The ReturnFlag value indicates InRouteTrue (success), InRouteFalse (failure), or NotInRoute.  Messages are given in GetRouteTruncateElement.
If successful the route is truncated at and including the element that matches H & V.  If PrefDirRoute ensure only truncate to a signal, else prevent
truncation to a crossover, bridge or points, also prevent route being left less than 2 elements in length.*/
    bool GetAllRoutesTruncateElement(int Caller, int HLoc, int VLoc, bool PrefDirRoute);
/// Checks whether the preferred direction element at TrackVectorPosition with XLinkPos value is in a locked route and returns true if so together with the element itself copied to &PrefDirElement & the LockedRouteVector position in &LockedVectorNumber
    bool IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(int Caller, int TrackVectorPosition, int XLinkPos, TPrefDirElement &PrefDirElement,
                                                                      int &LockedVectorNumber);
/// Returns true if there is a route with the given ID number - added at v1.3.1 (see function for details)
    bool IsThereARouteAtIDNumber(int Caller, IDInt RouteID);
/// Loads the routes from a session file
    bool LoadRoutes(int Caller, std::ifstream &InFile);
/// Route locking is required (returns true) if a moving train is within 3 signals back from the RouteTruncatePosition (on the route itself or on any linked routes, or on the element immediately before the start of the route or linked route - this because train cancels route elements that it touches) unless the first signal is red, then OK
    bool RouteLockingRequired(int Caller, int RouteNumber, int RouteTruncatePosition);
/// Examines Route2MultiMap and if the element at TrackVectorPosition with LinkPos (can be entry or exit) is found it returns true (for crossovers & points returns true whichever track the route is on), else returns false.
    bool TrackIsInARoute(int Caller, int TrackVectorPosition, int LinkPos);
/// Returns a route's position in AllRoutesVector from its ID, throws an error if a matching route isn't found
    int GetRouteVectorNumber(int Caller, IDInt RouteID);
/// Returns a constant reference to the route at AllRoutesVector position 'At', after performing range checking on the 'At' value and throwing an error if out of range
    const TOneRoute &GetFixedRouteAt(int Caller, int At) const;
/// Returns a constant reference to the route with ID number RouteID.  If no route is found with that ID an error is thrown
    const TOneRoute &GetFixedRouteAtIDNumber(int Caller, IDInt RouteID) const;
/// Returns a modifiable reference to the route at AllRoutesVector position 'At', after performing range checking on the 'At' value and throwing an error if out of range
    TOneRoute &GetModifiableRouteAt(int Caller, int At);
/// Returns a modifiable reference to the route with ID number RouteID. If no route is found with that ID an error is thrown
    TOneRoute &GetModifiableRouteAtIDNumber(int Caller, IDInt RouteID);
/// Examines Route2MultiMap and returns a TRouteElementPair if one is found with the passed values of H, V and ELink.
/**Also returned as a reference is an iterator to the found element in the map to assist in erasing it.  Called by
TAllRoutes::RemoveRouteElement.  Note that only need ELink (as well as H & V) to identify uniquely, since only bridges can have
two routes on them & their track ELinks are always different.  Messages are given for failure.*/
    TRouteElementPair FindRoutePairFromRoute2MultiMap(int Caller, int HLoc, int VLoc, int ELink, TRoute2MultiMapIterator &Route2MultiMapIterator);
/// Retrieve up to two TRouteElementPair entries from Route2MultiMap at H & V, the first as a function return and the second in the reference SecondPair.  If there's only one then it's the function return
    TRouteElementPair GetRouteElementDataFromRoute2MultiMap(int Caller, int HLoc, int VLoc, TRouteElementPair &SecondPair);
/// Examines Route2MultiMap for the element at TrackVectorPosition with LinkPos (can be entry or exit).
/**Returns the appropriate route type - NoRoute, NotAutoSigsRoute, or AutoSigsRoute.  If element is in a
route then the EXGraphicPtr is returned, and if either the start or end of a route then the correct EntryDirectionGraphicPtr is
returned, else a transparent element is returned.  Function is used int TrainUnit for retaining AutoSigsRoutes but erasing others
after train passes, and for picking up the correct background graphics for replotting of AutoSigsRoutes; also used in
CallingOnAllowed*/
    TRouteType GetRouteTypeAndGraphics(int Caller, int TrackVectorPosition, int LinkPos, Graphics::TBitmap* &EXGraphicPtr,
                                       Graphics::TBitmap* &EntryDirectionGraphicPtr);
/// Examines Route2MultiMap and if the element at TrackVectorPosition with LinkPos (can be entry or exit) is found returns the appropriate route type - NoRoute, NotAutoSigsRoute, or AutoSigsRoute and number (i.e. its position in AllRoutesVector, -1 if NoRoute).
    TRouteType GetRouteTypeAndNumber(int Caller, int TrackVectorPosition, int LinkPos, int &RouteNumber);
/// A single TPrefDirElement is added to both PrefDirVector (for the route at RouteNumber) and Route2MultiMap.  Called from TAllRoutes::StoreOneRoute.  Note that the IsARoute boolean variable is set in StoreRouteElementInPrefDirVector since that catches all route elements wherever created
    void AddRouteElement(int Caller, int HLoc, int VLoc, int ELink, int RouteNumber, TPrefDirElement RouteElement);
/// Diagnostic function - checks equivalence for each route between entries in PrefDirVector and those in Route2MultiMap, and also that the size of the multimap and the sum of the sizes of all PrefDirVectors is the same.  Throws an error if there is a discrepancy.
    void CheckMapAndRoutes(int Caller);
/// When attaching a new route section to an existing route, it is sometimes necessary to erase the original route and create a new composite route.
/**This function erases all elements in the route at RouteNumber using TAllRoutes->RemoveRouteElement to clear elements from Route2MultiMap
and from the PrefDirVector.  Since all elements for the route are removed RemoveRouteElement also clears the Route from AllRoutesVector.
Route numbers are decremented in the map for route numbers that are greater than the route number that is removed.  The LockedRouteVector
as also searched and if any relate to the route that has been cleared they are erased too, but the fact that one has been found is recorded
so that it can be re-established later.*/
    void ClearRouteDuringRouteBuildingAt(int Caller, int RouteNumber);
/// After a route element has been erased from the relevant PrefDirVector and from Route2MultiMap, this function examines all the remaining entries in Route2MultiMap with the same RouteNumber as that for the erased element.  Where a RouteElementNumber exceeds that for the erased element it is decremented.
    void DecrementRouteElementNumbersInRoute2MultiMap(int Caller, int RouteNumber, unsigned int ErasedElementNumber);
/// After a route has been erased from AllRoutesVector and its entries from Route2MultiMap, this function examines all the remaining entries in Route2MultiMap to see if their RouteNumbers exceed that for the erased route.  Where this is so the RouteNumber is decremented.
    void DecrementRouteNumbersInRoute2MultiMap(int Caller, int RouteNumber);
/// Calls PrefDirMarker for all routes, with RouteCall set to identify a route call, and BuildingPrefDir false.
    void MarkAllRoutes(int Caller, TDisplay *Disp);
/// Erases the route element from Route2MultiMap and from the PrefDirVector.
    void RemoveRouteElement(int Caller, int HLoc, int VLoc, int ELink);
/// Insert an entry in Route2MultiMap.  Called by TAllRoutes::AddRouteElement.
    void Route2MultiMapInsert(int Caller, int HLoc, int VLoc, int ELinkIn, int RouteNumber, unsigned int RouteElementNumber);
/// Save railway route information to a session file or an error file
    void SaveRoutes(int Caller, std::ofstream &OutFile);
/// Set rearwards signals from the specified route starting position
    void SetAllRearwardsSignals(int Caller, int Attribute, int RouteNumber, int RouteStartPosition);
/// Enter with signal at TrackVectorElement already set to red by the passing train.
/**Identify the route that the TrackVectorPosition is in,
carry out validity checks, then call SetAllRearwardsSignals to set signals in this route and all linked rearwards routes, unless find a train (a) in the current
route, in which case the signals behind it are set (and behind any other trains in the current route), but only within the current
route; or (b) in a linked rear route, in which case the function sets no further signals.*/
    void SetTrailingSignalsOnAutoSigsRoute(int Caller, int TrackVectorPosition, int XLinkPos);
/// This is called by the InterfaceUnit at intervals based on entries in the ContinuationAutoSigVector in TrainController.
/**It sets signals on the AutoSigsRoute to correspond to a train having exited the route at a continuation, and passing further signals (outside the simulated railway).
Initially the last passed signal will be red, then at the first call it will change to yellow and earlier signals will change accordingly, then double
yellow, then green.  There are only 3 calls in all for any given route, and the AccessNumber changes from 0 to 1 to 2 for successive calls.*/
    void SetTrailingSignalsOnContinuationRoute(int Caller, int RouteNumber, int AccessNumber);
/// A new (empty apart from RouteID) TOneRoute is added to the AllRoutesVector.
/**The route is the last to be added, and will have a RouteNumber of AllRoutesSize() - 1.  Each element of the new route is added in
turn using AddRouteElement, which uses HLoc, VLoc, ELink and RouteNumber to provide the information necessary to insert it into both PrefDirVector and Route2MultiMap.*/
    void StoreOneRoute(int Caller, TOneRoute *Route);
/// A new (empty apart from RouteID) TOneRoute is added to the AllRoutesVector after a session load. Very similar to StoreOneRoute but here the RouteID that is already in Route is used.
    void StoreOneRouteAfterSessionLoad(int Caller, TOneRoute *Route);
/// Calls RouteImageMarker for each route in turn to display the route colours and direction arrows on the bitmap image (as on screen during operation) for an operating railway
    void WriteAllRoutesToImage(int Caller, Graphics::TBitmap *Bitmap);

/// Constructor
    TAllRoutes::TAllRoutes() : LevelCrossingBarrierUpDelay(10.0), LevelCrossingBarrierDownDelay(30.0), PointsDelay(2.5), SignalsDelay(0.5),
        RebuildRailwayFlag(false)
    {
        ;
    }
};

// ---------------------------------------------------------------------------

extern TAllRoutes *AllRoutes;
///< the object pointer, object created in InterfaceUnit

// ---------------------------------------------------------------------------
#endif
