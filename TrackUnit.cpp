// TrackUnit.cpp
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
#include <algorithm>  //for std::find
#include <vcl.h>

#pragma hdrstop

#include "TrackUnit.h"
#include "TrainUnit.h"
#include "GraphicUnit.h"
//#include "DisplayUnit.h" included in TrackUnit.h
#include "TextUnit.h"
#include "PerfLogUnit.h"
#include "Utilities.h"

#pragma package(smart_init)
// ---------------------------------------------------------------------------

TTrack *Track;
TAllRoutes *AllRoutes;

// ---------------------------------------------------------------------------

// FIXED TRACK :-

// Constructor to build TrackPieces from array

TFixedTrackPiece::TFixedTrackPiece(int SpeedTagVal, TTrackType TrackTypeVal, int LkVal[4], TConfiguration ConfigVal[4], Graphics::TBitmap* GraphicPtrVal,
                                   Graphics::TBitmap* SmallGraphicPtrVal) : SpeedTag(SpeedTagVal), TrackType(TrackTypeVal), GraphicPtr(GraphicPtrVal), SmallGraphicPtr(SmallGraphicPtrVal)
{
    for(int x = 0; x < 4; x++)
    {
        Link[x] = LkVal[x];
        Config[x] = ConfigVal[x];
    }
// NamedLocationElements 76, 77, 78, 79, 96, 129, 130, 131, 145 & 146  (platforms, concourses, footcrossings & named non-station locations)
// underpasses (144 & 145 added at v2.3.1

	const std::vector<int> named_locations_{76, 77, 78, 79, 96, 129, 130, 131, 145, 146};

	FixedNamedLocationElement = std::find(named_locations_.begin(), named_locations_.end(), SpeedTagVal) != named_locations_.end();
}

// ---------------------------------------------------------------------------

TFixedTrackPiece::TFixedTrackPiece() : SpeedTag(0), TrackType(Erase), GraphicPtr(RailGraphics->bmSolidBgnd), SmallGraphicPtr(RailGraphics->smSolidBgnd),
    FixedNamedLocationElement(false) // default values
{
    for(int x = 0; x < 4; x++)
    {
        Link[x] = -1; // -1 & NotSet are the markers for 'unused' respectively
        Config[x] = NotSet;
    }
}

// ---------------------------------------------------------------------------
void TFixedTrackPiece::PlotFixedTrackElement(int Caller, int HLocInput, int VLocInput) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotFixedTrackElement," + AnsiString(HLocInput) + "," +
                                 AnsiString(VLocInput));
    Display->PlotOutput(33, HLocInput * 16, VLocInput * 16, GraphicPtr);
    Utilities->CallLogPop(1331);
}

// ---------------------------------------------------------------------------

// VARIABLE TRACK :-

// ---------------------------------------------------------------------------

bool TTrackElement:: operator == (TTrackElement RHElement)
{
	return
		this->HLoc == RHElement.HLoc &&
		this->VLoc == RHElement.VLoc &&
		this->SpeedTag == RHElement.SpeedTag;

}

// ---------------------------------------------------------------------------

bool TTrackElement:: operator != (TTrackElement RHElement)
{
	return
		this->HLoc != RHElement.HLoc ||
		this->VLoc != RHElement.VLoc ||
		this->SpeedTag != RHElement.SpeedTag;
}

// ---------------------------------------------------------------------------

void TTrackElement::PlotVariableTrackElement(int Caller, TDisplay *Disp) const
// 'Variable' in the sense that element might be striped or non-striped
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotVariableTrackElement");
    Graphics::TBitmap *GraphicOutput = GraphicPtr;

    if(LocationName == "")
    {
        switch(SpeedTag)
        {
        case 76: // t platform
            GraphicOutput = RailGraphics->gl76Striped;
            break;

        case 77: // h platform
            GraphicOutput = RailGraphics->bm77Striped;
            break;

        case 78: // v platform
            GraphicOutput = RailGraphics->bm78Striped;
            break;

        case 79: // r platform
            GraphicOutput = RailGraphics->gl79Striped;
            break;

        case 96: // concourse
            GraphicOutput = RailGraphics->ConcourseStriped;
            break;

        case 129: // v footbridge
            GraphicOutput = RailGraphics->gl129Striped;
            break;

        case 130: // h footbridge
            GraphicOutput = RailGraphics->gl130Striped;
            break;

        case 131: // non-station named loc
            GraphicOutput = RailGraphics->bmNameStriped;
            break;

        case 145: // v underpass
            GraphicOutput = RailGraphics->gl145Striped;
            break;

        case 146: // h underpass
            GraphicOutput = RailGraphics->gl146Striped;
            break;

        default:
            GraphicOutput = GraphicPtr;
            break;
        }
    }
    Disp->PlotOutput(34, HLoc * 16, VLoc * 16, GraphicOutput);
    //deal with TSRs
    if((TrackType == Simple) && Failed) //added at v2.13.0
    {
       Disp->GetImage()->Canvas->Draw((HLoc - Display->DisplayOffsetH) * 16, (VLoc - Display->DisplayOffsetV) * 16, RailGraphics->BlackOctagon); //indicates that it has failed
    }
    Utilities->CallLogPop(1332);
}

// ---------------------------------------------------------------------------

AnsiString TTrackElement::LogTrack(int Caller) const
// for debugging when passes as a call parameter
{
    AnsiString LogString = "TrkEl:-," + AnsiString(ElementID) + "," + LocationName + "," + AnsiString(TrainIDOnElement) + "," +
        AnsiString(TrainIDOnBridgeOrFailedPointOrigSpeedLimit01) + "," + AnsiString(TrainIDOnBridgeOrFailedPointOrigSpeedLimit23) + ",EndTrkEl,";

    return(LogString);
}

// ---------------------------------------------------------------------------

/// Constructor for specific type of element. Use very high neg. numbers as 'unset' values for HLoc & VLoc initially as can go high negatively legitimately, build from existing TTrackPiece with default values for extra members
    TTrackElement::TTrackElement(TFixedTrackPiece Input) : TFixedTrackPiece(Input), HLoc(-2000000000), VLoc(-2000000000), LocationName(""), ActiveTrackElementName(""),
        Attribute(0), CallingOnSet(false), Length01(Utilities->DefaultTrackLength), Length23(-1), SpeedLimit01(Utilities->DefaultTrackSpeedLimit), SpeedLimit23(-1),
        TrainIDOnElement(-1), TrainIDOnBridgeOrFailedPointOrigSpeedLimit01(-1), TrainIDOnBridgeOrFailedPointOrigSpeedLimit23(-1), StationEntryStopLinkPos1(-1), StationEntryStopLinkPos2(-1),
        SigAspect(FourAspect)
    {
        Failed = false; //added at v2.13.0
        for(int x = 0; x < 4; x++)
        {
            ConnLinkPos[x] = -1;
            Conn[x] = -1;
        }
        if((TrackType == Points) || (TrackType == Crossover) || (TrackType == Bridge))
        {
            Length23 = Utilities->DefaultTrackLength;
            SpeedLimit23 = Utilities->DefaultTrackSpeedLimit;
        }
    }

// ---------------------------------------------------------------------------

bool TMapComp:: operator()(const THVPair& lower, const THVPair& higher) const // HLoc  VLoc
{
    if(lower.second < higher.second)
    {
        return(true);
    }
    else if(lower.second > higher.second)
    {
        return(false);
    }
    else if(lower.second == higher.second)
    {
        if(lower.first < higher.first)
        {
            return(true);
        }
    }
    return(false);
}

// ---------------------------------------------------------------------------
// PrefDirElement Functions
// ---------------------------------------------------------------------------

TPrefDirElement::TPrefDirElement(TTrackElement ElementIn, int ELinkIn, int ELinkPosIn, int XLinkIn, int XLinkPosIn, int TrackVectorPositionIn)
    : TTrackElement(ElementIn), ELink(ELinkIn), ELinkPos(ELinkPosIn), XLink(XLinkIn), XLinkPos(XLinkPosIn), TrackVectorPosition(TrackVectorPositionIn),
    CheckCount(9), IsARoute(false), AutoSignals(false), PrefDirRoute(false)
{
    if(!EntryExitNumber())
    {
        throw Exception("EXNumber failure in TPrefDirElement constructor");
    }
    EXGraphicPtr = GetPrefDirGraphicPtr();
    EntryDirectionGraphicPtr = GetDirectionPrefDirGraphicPtr();
}

// ---------------------------------------------------------------------------

AnsiString TPrefDirElement::LogPrefDir() const
// for debugging when passed as a call parameter
{
    AnsiString LogString = "PthEl:-," + AnsiString(ELink) + "," + AnsiString(ELinkPos) + "," + AnsiString(XLink) + "," + AnsiString(XLinkPos) + "," +
        AnsiString(EXNumber) + "," + AnsiString(TrackVectorPosition) + "," + AnsiString((short)AutoSignals) + "," + AnsiString((short)PrefDirRoute) +
        ",ElementID," + AnsiString(ElementID) + "," + LocationName + "," + AnsiString(TrainIDOnElement) + "," + AnsiString(TrainIDOnBridgeOrFailedPointOrigSpeedLimit01) + "," +
        AnsiString(TrainIDOnBridgeOrFailedPointOrigSpeedLimit23);

// Track->TrackElementAt(73, TrackVectorPosition).LogTrack(12);
    return(LogString);
}

// ---------------------------------------------------------------------------

bool TPrefDirElement::EntryExitNumber() // true for valid number
/*
          Computes a number corresponding to ELink & Xlink if set, or to the entry and exit link values for the track
          at Link[0] and Link[1], or, if ELink or XLink not set, and a complex (4-entry) element, return false for error message.
          This should be OK because only elements for which ELink & XLink not set are PrefDir/route start elements and leading points
          as temporary end of PrefDir, and in both cases this function is not called as the direction is not displayed for these elements.
          Uses simple links between any 2 entry & exit points for use in displaying PrefDir or route graphics, or original graphic during
          route flashing.  Should only be called when ELink & XLink set, or when ELinkPos & XLinkPos set deliberately from a
          TTrackElement during route setting functions.  If a bridge then an additional check is made in case the graphic needed
          corresponds to an undebridge, i.e a gap needed between entry and exit.  In this case the EXNumber is increased by
          16 so as to be unique.  Returns true if valid and sets EXNumber to the selected value.
*/

{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",EntryExitNumber");
    int EXArray[16][2] =
    {{4, 6}, {2, 8}, // horizontal & vertical
     {2, 4}, {6, 2}, {8, 6}, {4, 8},    // sharp curves
     {1, 6}, {3, 8}, {9, 4}, {7, 2}, {1, 8}, {3, 4}, {9, 2}, {7, 6},    // loose curves
     {1, 9}, {3, 7}};    // forward & reverse diagonals

    int EXNum = -1;
    int Entry, Exit;

    if(ELink > -1)
    {
        Entry = ELink; // pick up simple elements even if ELink &/or XLink not set, as no ambiguity
    }
    else if(Link[2] == -1)
    {
        Entry = Link[0];
    }
    else
    {
        Utilities->CallLogPop(122);
        return(false);
    }
    if(XLink > -1)
    {
        Exit = XLink;
    }
    else if(Link[2] == -1)
    {
        Exit = Link[1];
    }
    else
    {
        Utilities->CallLogPop(123);
        return(false);
    }
    for(int x = 0; x < 16; x++)
    {
        if(((Entry == EXArray[x][0]) && (Exit == EXArray[x][1])) || ((Entry == EXArray[x][1]) && (Exit == EXArray[x][0]))) //added extra brackets round && segments at v2.9.1
        {
            EXNum = x;
        }
    }
    if(EXNum == -1)
    {
        Utilities->CallLogPop(124);
        return(false);
    }
    int BrNum = -1;

/* The order for bridge entries & exits is as below.  Note that there are 3 of each type,
       the graphic for each of which is different because of the shape of the overbridge.  The basic
       entry/exit value is computed above, and this used to select only from elements with that entry/exit
       value that is an underbridge, i.e overbridges ignored as the normal graphic is OK for them.
       int BrEXArray[24][2] = {
       {4,6},{2,8},{1,9},{3,7},
       {1,9},{3,7},{1,9},{3,7},
       {2,8},{4,6},{2,8},{4,6}
*/

    if(TrackType == Bridge)
    {
        if(EXNum == 1)
        {
            if(SpeedTag == 49)
            {
                BrNum = 1 + 16;
            }
            else if(SpeedTag == 54)
            {
                BrNum = 8 + 16;
            }
            else if(SpeedTag == 55)
            {
                BrNum = 10 + 16;
            }
        }
        else if(EXNum == 0)
        {
            if(SpeedTag == 48)
            {
                BrNum = 0 + 16;
            }
            else if(SpeedTag == 58)
            {
                BrNum = 11 + 16;
            }
            else if(SpeedTag == 59)
            {
                BrNum = 9 + 16;
            }
        }
        else if(EXNum == 14)
        {
            if(SpeedTag == 50)
            {
                BrNum = 2 + 16;
            }
            else if(SpeedTag == 52)
            {
                BrNum = 4 + 16;
            }
            else if(SpeedTag == 57)
            {
                BrNum = 6 + 16;
            }
        }
        else if(EXNum == 15)
        {
            if(SpeedTag == 51)
            {
                BrNum = 3 + 16;
            }
            else if(SpeedTag == 53)
            {
                BrNum = 7 + 16;
            }
            else if(SpeedTag == 56)
            {
                BrNum = 5 + 16;
            }
        }
    }
    if(BrNum == -1)
    {
        EXNumber = EXNum;
    }
    else
    {
        EXNumber = BrNum;
    }
    Utilities->CallLogPop(125);
    return(true);
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TPrefDirElement::GraphicFromSpeedTag(int speed_tag)
{
   switch(SpeedTag)
	{
		case 64:
			return(RailGraphics->LinkGraphicsPtr[16]); // intercept diagonal buffers
		case 65:
			return(RailGraphics->LinkGraphicsPtr[17]);
		case 66:
			return(RailGraphics->LinkGraphicsPtr[18]);
		case 67:
			return(RailGraphics->LinkGraphicsPtr[19]);
		case 80:
			return(RailGraphics->LinkGraphicsPtr[20]); // intercept continuations
		case 81:
			return(RailGraphics->LinkGraphicsPtr[21]);
		case 82:
			return(RailGraphics->LinkGraphicsPtr[22]);
		case 83:
			return(RailGraphics->LinkGraphicsPtr[23]);
		case 84:
			return(RailGraphics->LinkGraphicsPtr[24]);
		case 85:
			return(RailGraphics->LinkGraphicsPtr[25]);
		case 86:
			return(RailGraphics->LinkGraphicsPtr[26]);
		case 87:
			return(RailGraphics->LinkGraphicsPtr[27]);
		case 129:
			return(RailGraphics->LinkGraphicsPtr[28]); // intercept under footbridges
		case 130:
			return(RailGraphics->LinkGraphicsPtr[29]);
		default:
			return nullptr;
	}
}

Graphics::TBitmap *TPrefDirElement::GetOriginalGraphicPtr()
/*
      This is the basic track graphic for use in plotting the original graphic during route flashing.
      Enter with all set apart from EXGraphic & EntryDirectionGraphic
*/
{

	Graphics::TBitmap *graphic_{GraphicFromSpeedTag(SpeedTag)};

	if(graphic_) return graphic_;

    if(XLinkPos == -1) // not set, could be first element or last element = leading point
	{
// check if just a simple one in & one out & if so set graphic (otherwise Bridge, Crossover or
// Points & don't want to display these)
        if(Link[2] != -1)
        {
            return(0); // i.e. complex element, don't display
        }
        else
        {
            if(!EntryExitNumber())
            {
                throw Exception("Error in EntryExitNumber 4");
            }
            else
            {
                return(RailGraphics->LinkGraphicsPtr[EXNumber]);
            }
        }
    }
    if(EXNumber > 15) // underbridge
    {
        return(RailGraphics->BridgeGraphicsPtr[EXNumber - 16]);
    }
    else
    {
        return(RailGraphics->LinkGraphicsPtr[EXNumber]);
    }
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TPrefDirElement::GetPrefDirGraphicPtr()
/*
      As above but for PrefDir graphics.
*/
{
    Graphics::TBitmap *graphic_{GraphicFromSpeedTag(SpeedTag)};

	if(graphic_) return graphic_;

    if(XLinkPos == -1) // not set, could be first element or last element = leading point
    {
// check if just a simple one in & one out & if so set graphic (otherwise Bridge, Crossover or Points)
        if(Link[2] != -1)
        {
            return(0); // i.e. complex element, don't display
        }
        else
        {
            if(!EntryExitNumber())
            {
                throw Exception("Error in EntryExitNumber 5");
            }
            else
            {
                return(RailGraphics->LinkPrefDirGraphicsPtr[EXNumber]);
            }
        }
    }
    if(EXNumber > 15) // underbridge
    {
        return(RailGraphics->BridgePrefDirGraphicsPtr[EXNumber - 16]);
    }
    else
    {
        return(RailGraphics->LinkPrefDirGraphicsPtr[EXNumber]);
    }
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TPrefDirElement::GetRouteGraphicPtr(bool AutoSigsFlag, bool PrefDirRoute)
/*
      As above but for route graphics.
*/
{
    Graphics::TBitmap *graphic_{GraphicFromSpeedTag(SpeedTag)};

	if(graphic_) return graphic_;

	if(!AutoSigsFlag && !PrefDirRoute)
	{

        if(XLinkPos == -1) // not set, could be first element or last element = leading point
        {
            // check if just a simple one in & one out & if so set graphic (otherwise Bridge, Crossover or Points)
            if(Link[2] != -1)
            {
                return(0); // i.e. complex element, don't display
            }
            else
            {
                if(!EntryExitNumber())
                {
                    throw Exception("Error in EntryExitNumber 6");
                }
                else
                {
                    return(RailGraphics->LinkNonSigRouteGraphicsPtr[EXNumber]);
                }
            }
        }
        if(EXNumber > 15) // underbridge
        {
            return(RailGraphics->BridgeNonSigRouteGraphicsPtr[EXNumber - 16]);
		}
		else
		{
			return(RailGraphics->LinkNonSigRouteGraphicsPtr[EXNumber]);
		}
	}

	else if(!AutoSigsFlag && PrefDirRoute)
	{
        if(XLinkPos == -1) // not set, could be first element or last element = leading point
        {
            // check if just a simple one in & one out & if so set graphic (otherwise Bridge, Crossover or Points)
            if(Link[2] != -1)
            {
                return(0); // i.e. complex element, don't display
            }
            else
            {
                if(!EntryExitNumber())
                {
                    throw Exception("Error in EntryExitNumber 10");
                }
                else
                {
                    return(RailGraphics->LinkSigRouteGraphicsPtr[EXNumber]);
                }
            }
        }
        if(EXNumber > 15) // underbridge
        {
            return(RailGraphics->BridgeSigRouteGraphicsPtr[EXNumber - 16]);
        }
        else
        {
            return(RailGraphics->LinkSigRouteGraphicsPtr[EXNumber]);
        }
    }

    else
    {
        if(XLinkPos == -1) // not set, could be first element or last element = leading point
        {
            // check if just a simple one in & one out & if so set graphic (otherwise Bridge, Crossover or Points)
            if(Link[2] != -1)
            {
                return(0); // i.e. complex element, don't display
            }
            else
            {
                if(!EntryExitNumber())
                {
                    throw Exception("Error in EntryExitNumber 11");
                }
                else
                {
                    return(RailGraphics->LinkRouteAutoSigsGraphicsPtr[EXNumber]);
                }
            }
        }
        if(EXNumber > 15) // underbridge
        {
            return(RailGraphics->BridgeRouteAutoSigsGraphicsPtr[EXNumber - 16]);
        }
        else
        {
            return(RailGraphics->LinkRouteAutoSigsGraphicsPtr[EXNumber]);
        }
    }
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TPrefDirElement::GetRouteAutoSigsGraphicPtr()
/*
      As above but for route flashing graphics.  (Disused - now combined with above)
*/
{
    Graphics::TBitmap *graphic_{GraphicFromSpeedTag(SpeedTag)};

	if(graphic_) return graphic_;

    if(XLinkPos == -1) // not set, could be first element or last element = leading point
    {
// check if just a simple one in & one out & if so set graphic (otherwise Bridge, Crossover or Points)
        if(Link[2] != -1)
        {
            return(0); // i.e. complex element, don't display
        }
        else
        {
            if(!EntryExitNumber())
            {
                throw Exception("Error in EntryExitNumber 7");
            }
            else
            {
                return(RailGraphics->LinkRouteAutoSigsGraphicsPtr[EXNumber]);
            }
        }
    }
    if(EXNumber > 15) // underbridge
    {
        return(RailGraphics->BridgeRouteAutoSigsGraphicsPtr[EXNumber - 16]);
    }
    else
    {
        return(RailGraphics->LinkRouteAutoSigsGraphicsPtr[EXNumber]);
    }
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TPrefDirElement::GetDirectionPrefDirGraphicPtr() const
/*
      Get PrefDir direction graphic.  Enter with all set apart from EXGraphic & EntryDirectionGraphic
*/
{
    if((ELink > 0) && (ELink < 10) && (ELink != 5))
    {
        return(RailGraphics->DirectionPrefDirGraphicsPtr[ELink]);
    }
    else
    {
        throw Exception("Error in EntryExitNumber 8");
    }
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TPrefDirElement::GetDirectionRouteGraphicPtr(bool AutoSigsFlag, bool PrefDirRoute) const
/*
      Get route direction graphic.  Enter with all set apart from EXGraphic & EntryDirectionGraphic
*/
{
    if((ELink > 0) && (ELink < 10) && (ELink != 5))
    {
        if(!AutoSigsFlag && !PrefDirRoute)
        {
            return(RailGraphics->DirectionNonSigRouteGraphicsPtr[ELink]);
        }
        else if(!AutoSigsFlag && PrefDirRoute)
        {
            return(RailGraphics->DirectionSigRouteGraphicsPtr[ELink]);
        }
        else
        {
            return(RailGraphics->DirectionRouteAutoSigsGraphicsPtr[ELink]);
        }
    }
    else
    {
        throw Exception("Error in EntryExitNumber 9");
    }
}

// ---------------------------------------------------------------------------

bool TPrefDirElement:: operator == (TPrefDirElement RHElement)
/*
      Set == operator when TrackVectorPosition, ELink & XLink all same
*/
{
	return
		this->TrackVectorPosition == RHElement.TrackVectorPosition &&
		this->ELink == RHElement.ELink &&
		this->XLink == RHElement.XLink;
}

// ---------------------------------------------------------------------------

bool TPrefDirElement:: operator != (TPrefDirElement RHElement)
/*
      Set != operator when any of TrackVectorPosition, ELink or XLink different
*/
{
	return
		this->TrackVectorPosition == RHElement.TrackVectorPosition &&
		this->ELink == RHElement.ELink &&
		this->XLink == RHElement.XLink;
}

// ---------------------------------------------------------------------------

int TPrefDirElement::GetRouteColour(Graphics::TBitmap *EXG)
{   //returns 1 for red, 2 for green & 3 for blue , or 0 for no match (i.e. using it other than on a route)
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",GetRouteColour");
    if(GetRouteGraphicPtr(0,0) == EXG)  //AutoSignals, PrefDirRoute
    {
        Utilities->CallLogPop(2566);
        return(1);
    }
    if(GetRouteGraphicPtr(0,1) == EXG)
    {
        Utilities->CallLogPop(2567);
        return(2);
    }
    if(GetRouteGraphicPtr(1,0) == EXG)
    {
        Utilities->CallLogPop(2568);
        return(3);
    }
    if(GetRouteGraphicPtr(1,1) == EXG)
    {
        Utilities->CallLogPop(2569);
        return(3);
    }
    Utilities->CallLogPop(2570);
    return(0);
}

// ---------------------------------------------------------------------------
// Track functions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

TTrack::TActiveLevelCrossing::TActiveLevelCrossing()
{
    TypeOfRoute = 0;
    ReducedTimePenalty = false;
    BarrierState = Up;
    ChangeDuration = 0.0;
    BaseElementSpeedTag = 1;
    HLoc = 0;
    VLoc = 0;
    StartTime = TDateTime(0);
}

// ---------------------------------------------------------------------------

TTrack::TTrack()
{
// CurrentSpeedButtonTag = 0; //not assigned yet

    HLocMin = 2000000000;
    VLocMin = 2000000000;
    HLocMax = -2000000000;
    VLocMax = -2000000000;
    SkipLocationNameMultiMapCheck = false; // new at v2.2.0, false is default value
    CopyFlag = false; // only true for copying, so names aren't copied
    AnsiString NL = '\n';

    RouteFailMessage = "Unable to set a route:" + NL + NL + "it may be unreachable, perhaps because of failed points; " + NL + NL +
        "reachable but with too many different directions leading away from the start point  - set some points on the route required; " + NL + NL +
        "blocked by a train, another route or a changing level crossing; " + NL + NL +
        "or invalid - possibly due to a preferred direction mismatch, or a missed signal in a blue route or green route restricted to consecutive signals.";

    GapFlashGreen = new TGraphicElement;
    GapFlashRed = new TGraphicElement;
    GapFlashGreen->LoadOverlayGraphic(1, RailGraphics->bmGreenEllipse);
    GapFlashRed->LoadOverlayGraphic(2, RailGraphics->bmRedEllipse);

    int InternalLinkCheckArray[9][2] =
    {{1, 9}, {4, 6}, {7, 3}, {2, 8}, {0, 0}, {8, 2}, {3, 7}, {6, 4}, {9, 1}};

/* array of valid link values for 'old' location and 'new' location, where
      array number = (((Hnew - Hold)+1)*3) + ((Vnew - Vold)+1) */

    for(int x = 0; x < 9; x++)
    {
        for(int y = 0; y < 2; y++)
        {
            LinkCheckArray[x][y] = InternalLinkCheckArray[x][y];
        }
    }

// Platform and default track element values
    TopPlatAllowed << 1 << 9 << 10 << 30 << 31 << 60 << 61 << 68 << 69 << 77 << 125 << 126 << 129 << 145;
// top & bot sigs, straights, straight points, buffers, signal, vert footcrossing, bot plat
    BotPlatAllowed << 1 << 7 << 8 << 28 << 29 << 60 << 61 << 68 << 69 << 76 << 125 << 126 << 129 << 145;
    LeftPlatAllowed << 2 << 12 << 14 << 33 << 35 << 62 << 63 << 70 << 71 << 79 << 127 << 128 << 130 << 146;
    RightPlatAllowed << 2 << 11 << 13 << 32 << 34 << 62 << 63 << 70 << 71 << 78 << 127 << 128 << 130 << 146;
    NameAllowed << 1 << 2 << 3 << 4 << 5 << 6 << 20 << 21 << 22 << 23 << 24 << 25 << 26 << 27 // disallow diagonals, points, crossovers, bridges, gaps,
    << 60 << 61 << 62 << 63  << 68  << 69  << 70  << 71 << 80 << 81 << 82 << 83 << 125 << 126 << 127 << 128;     // diag continuations, diag buffers, footcrossings (diagonals may be OK
    // but as can't link diagonal locations would need solid blocks to allow linkage & that would look untidy except for single
    // elements, & can always use straights so leave out.) Allow horiz & vert signals as from v2.6.0
    LevelCrossingAllowed << 1 << 2; // only allow on straight tracks without direction markers
// Note platforms not allowed at continuations, but named non-station locations OK, though not allowed in timetables

    int HVArray[10][2] =
    {{0, 0}, {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};

    for(int x = 0; x < 10; x++)
    {
        for(int y = 0; y < 2; y++)
        {
            LinkHVArray[x][y] = HVArray[x][y];
        }
    }
    TrackFinished = false;
// DistancesSet = false;

    TSigElement TempSigTable[40] = // original four aspect
    {{68, 0, RailGraphics->gl68}, {69, 0, RailGraphics->gl69}, {70, 0, RailGraphics->gl70}, {71, 0, RailGraphics->gl71}, {72, 0, RailGraphics->gl72},
     {73, 0, RailGraphics->bm73}, {74, 0, RailGraphics->bm74}, {75, 0, RailGraphics->gl75},

     {68, 1, RailGraphics->bm68yellow}, {69, 1, RailGraphics->bm69yellow}, {70, 1, RailGraphics->bm70yellow}, {71, 1, RailGraphics->bm71yellow},
     {72, 1, RailGraphics->bm72yellow}, {73, 1, RailGraphics->bm73yellow}, {74, 1, RailGraphics->bm74yellow}, {75, 1, RailGraphics->bm75yellow},

     {68, 2, RailGraphics->bm68dblyellow}, {69, 2, RailGraphics->bm69dblyellow}, {70, 2, RailGraphics->bm70dblyellow}, {71, 2, RailGraphics->bm71dblyellow},
     {72, 2, RailGraphics->bm72dblyellow}, {73, 2, RailGraphics->bm73dblyellow}, {74, 2, RailGraphics->bm74dblyellow}, {75, 2, RailGraphics->bm75dblyellow},

     {68, 3, RailGraphics->bm68green}, {69, 3, RailGraphics->bm69green}, {70, 3, RailGraphics->bm70green}, {71, 3, RailGraphics->bm71green},
     {72, 3, RailGraphics->bm72green}, {73, 3, RailGraphics->bm73green}, {74, 3, RailGraphics->bm74green}, {75, 3, RailGraphics->bm75green},

     {68, 4, RailGraphics->gl68}, {69, 4, RailGraphics->gl69},    // Attr 4 disused but leave in case re-instate
     {70, 4, RailGraphics->gl70}, {71, 4, RailGraphics->gl71}, {72, 4, RailGraphics->gl72}, {73, 4, RailGraphics->bm73}, {74, 4, RailGraphics->bm74},
     {75, 4, RailGraphics->gl75}};

    for(int x = 0; x < 40; x++)
    {
        SigTable[x] = TempSigTable[x];
    }

    TSigElement TempSigTableThreeAspect[40] =
    {{68, 0, RailGraphics->gl68}, {69, 0, RailGraphics->gl69}, {70, 0, RailGraphics->gl70}, {71, 0, RailGraphics->gl71}, {72, 0, RailGraphics->gl72},
     {73, 0, RailGraphics->bm73}, {74, 0, RailGraphics->bm74}, {75, 0, RailGraphics->gl75},

     {68, 1, RailGraphics->bm68yellow}, {69, 1, RailGraphics->bm69yellow}, {70, 1, RailGraphics->bm70yellow}, {71, 1, RailGraphics->bm71yellow},
     {72, 1, RailGraphics->bm72yellow}, {73, 1, RailGraphics->bm73yellow}, {74, 1, RailGraphics->bm74yellow}, {75, 1, RailGraphics->bm75yellow},

     {68, 2, RailGraphics->bm68green}, {69, 2, RailGraphics->bm69green}, {70, 2, RailGraphics->bm70green}, {71, 2, RailGraphics->bm71green},
     {72, 2, RailGraphics->bm72green}, {73, 2, RailGraphics->bm73green}, {74, 2, RailGraphics->bm74green}, {75, 2, RailGraphics->bm75green},

     {68, 3, RailGraphics->bm68green}, {69, 3, RailGraphics->bm69green}, {70, 3, RailGraphics->bm70green}, {71, 3, RailGraphics->bm71green},
     {72, 3, RailGraphics->bm72green}, {73, 3, RailGraphics->bm73green}, {74, 3, RailGraphics->bm74green}, {75, 3, RailGraphics->bm75green},

     {68, 4, RailGraphics->gl68}, {69, 4, RailGraphics->gl69},    // Attr 4 disused but leave in case re-instate
     {70, 4, RailGraphics->gl70}, {71, 4, RailGraphics->gl71}, {72, 4, RailGraphics->gl72}, {73, 4, RailGraphics->bm73}, {74, 4, RailGraphics->bm74},
     {75, 4, RailGraphics->gl75}};

    for(int x = 0; x < 40; x++)
    {
        SigTableThreeAspect[x] = TempSigTableThreeAspect[x];
    }

    TSigElement TempSigTableTwoAspect[40] =
    {{68, 0, RailGraphics->gl68}, {69, 0, RailGraphics->gl69}, {70, 0, RailGraphics->gl70}, {71, 0, RailGraphics->gl71}, {72, 0, RailGraphics->gl72},
     {73, 0, RailGraphics->bm73}, {74, 0, RailGraphics->bm74}, {75, 0, RailGraphics->gl75},

     {68, 1, RailGraphics->bm68green}, {69, 1, RailGraphics->bm69green}, {70, 1, RailGraphics->bm70green}, {71, 1, RailGraphics->bm71green},
     {72, 1, RailGraphics->bm72green}, {73, 1, RailGraphics->bm73green}, {74, 1, RailGraphics->bm74green}, {75, 1, RailGraphics->bm75green},

     {68, 2, RailGraphics->bm68green}, {69, 2, RailGraphics->bm69green}, {70, 2, RailGraphics->bm70green}, {71, 2, RailGraphics->bm71green},
     {72, 2, RailGraphics->bm72green}, {73, 2, RailGraphics->bm73green}, {74, 2, RailGraphics->bm74green}, {75, 2, RailGraphics->bm75green},

     {68, 3, RailGraphics->bm68green}, {69, 3, RailGraphics->bm69green}, {70, 3, RailGraphics->bm70green}, {71, 3, RailGraphics->bm71green},
     {72, 3, RailGraphics->bm72green}, {73, 3, RailGraphics->bm73green}, {74, 3, RailGraphics->bm74green}, {75, 3, RailGraphics->bm75green},

     {68, 4, RailGraphics->gl68}, {69, 4, RailGraphics->gl69},    // Attr 4 disused but leave in case re-instate
     {70, 4, RailGraphics->gl70}, {71, 4, RailGraphics->gl71}, {72, 4, RailGraphics->gl72}, {73, 4, RailGraphics->bm73}, {74, 4, RailGraphics->bm74},
     {75, 4, RailGraphics->gl75}};

    for(int x = 0; x < 40; x++)
    {
        SigTableTwoAspect[x] = TempSigTableTwoAspect[x];
    }

    TSigElement TempSigTableGroundSignal[40] =
    {{68, 0, RailGraphics->bm68grounddblred}, {69, 0, RailGraphics->bm69grounddblred}, {70, 0, RailGraphics->bm70grounddblred},
     {71, 0, RailGraphics->bm71grounddblred}, {72, 0, RailGraphics->bm72grounddblred}, {73, 0, RailGraphics->bm73grounddblred},
     {74, 0, RailGraphics->bm74grounddblred}, {75, 0, RailGraphics->bm75grounddblred},

     {68, 1, RailGraphics->bm68grounddblwhite}, {69, 1, RailGraphics->bm69grounddblwhite}, {70, 1, RailGraphics->bm70grounddblwhite},
     {71, 1, RailGraphics->bm71grounddblwhite}, {72, 1, RailGraphics->bm72grounddblwhite}, {73, 1, RailGraphics->bm73grounddblwhite},
     {74, 1, RailGraphics->bm74grounddblwhite}, {75, 1, RailGraphics->bm75grounddblwhite},

     {68, 2, RailGraphics->bm68grounddblwhite}, {69, 2, RailGraphics->bm69grounddblwhite}, {70, 2, RailGraphics->bm70grounddblwhite},
     {71, 2, RailGraphics->bm71grounddblwhite}, {72, 2, RailGraphics->bm72grounddblwhite}, {73, 2, RailGraphics->bm73grounddblwhite},
     {74, 2, RailGraphics->bm74grounddblwhite}, {75, 2, RailGraphics->bm75grounddblwhite},

     {68, 3, RailGraphics->bm68grounddblwhite}, {69, 3, RailGraphics->bm69grounddblwhite}, {70, 3, RailGraphics->bm70grounddblwhite},
     {71, 3, RailGraphics->bm71grounddblwhite}, {72, 3, RailGraphics->bm72grounddblwhite}, {73, 3, RailGraphics->bm73grounddblwhite},
     {74, 3, RailGraphics->bm74grounddblwhite}, {75, 3, RailGraphics->bm75grounddblwhite},

     {68, 4, RailGraphics->bm68grounddblred}, {69, 4, RailGraphics->bm69grounddblred},    // Attr 4 disused but leave in case re-instate
     {70, 4, RailGraphics->bm70grounddblred}, {71, 4, RailGraphics->bm71grounddblred}, {72, 4, RailGraphics->bm72grounddblred},
     {73, 4, RailGraphics->bm73grounddblred}, {74, 4, RailGraphics->bm74grounddblred}, {75, 4, RailGraphics->bm75grounddblred}};

    for(int x = 0; x < 40; x++)
    {
        SigTableGroundSignal[x] = TempSigTableGroundSignal[x];
    }

    TSigElement TempFailedSigTable[8] = // added at v2.13.0
    {{68, 0, RailGraphics->FSig68}, {69, 0, RailGraphics->FSig69}, {70, 0, RailGraphics->FSig70}, {71, 0, RailGraphics->FSig71}, {72, 0, RailGraphics->FSig72},
     {73, 0, RailGraphics->FSig73}, {74, 0, RailGraphics->FSig74}, {75, 0, RailGraphics->FSig75}};

    for(int x = 0; x < 8; x++)
    {
        FailedSigTable[x] = TempFailedSigTable[x];
    }

    TSigElement TempFailedGroundSigTable[8] = // added at v2.14.0 to allow ground signals to fail
    {{68, 0, RailGraphics->FGSig68}, {69, 0, RailGraphics->FGSig69}, {70, 0, RailGraphics->FGSig70}, {71, 0, RailGraphics->FGSig71}, {72, 0, RailGraphics->FGSig72},
     {73, 0, RailGraphics->FGSig73}, {74, 0, RailGraphics->FGSig74}, {75, 0, RailGraphics->FGSig75}};

    for(int x = 0; x < 8; x++)
    {
        FailedGroundSigTable[x] = TempFailedGroundSigTable[x];
    }

/*
      Named Location Arrays:  Set out the adjacent positions and tracktypes that are accepted as valid connections for
      a single location.  These are as follows:-
      Directly Adjacent = up, down, left or right - NOT diagonal.
      There are two separate groups, platforms, concourses & footcrossings (providing the crossing part touches or overlaps the other relevant
      named location) all link with each other providing directly adjacent, but not to NamedNonStationLocations.
      NamedNonStationLocation link to other NamedNonStationLocations providing directly adjacent, but not to anything else.

      //t     76
      //b     77
      //l     78
      //r     79
      //c     96
      //v fb  129
      //h fb  130
      //v underpass 145
      //h underpass 146
      //n     131
*/

    int Tag76[25][3] =
    {{-1, 0, 96}, // c    top plat
     {1, 0, 96}, {0, -1, 96}, {0, 1, 96}, {-1, 0, 76},    // t
     {1, 0, 76}, {0, -1, 76}, {0, 1, 76}, {-1, 0, 77},    // b
     {1, 0, 77}, {0, -1, 77}, {0, 1, 77}, {0, 0, 77}, {-1, 0, 78},    // l
     {1, 0, 78}, {0, -1, 78}, {0, 1, 78}, {-1, 0, 79},    // r
     {1, 0, 79}, {0, -1, 79}, {0, 1, 79}, {0, -1, 129},    // v fb
     {0, 0, 129}, {0, -1, 145},    // v up
     {0, 0, 145}};

    for(int x = 0; x < 25; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag76Array[x][y] = Tag76[x][y];
        }
    }

    int Tag77[25][3] =
    {{-1, 0, 96}, // c     bot plat
     {1, 0, 96}, {0, -1, 96}, {0, 1, 96}, {-1, 0, 76},    // t
     {1, 0, 76}, {0, -1, 76}, {0, 1, 76}, {0, 0, 76}, {-1, 0, 77},    // b
     {1, 0, 77}, {0, -1, 77}, {0, 1, 77}, {-1, 0, 78},    // l
     {1, 0, 78}, {0, -1, 78}, {0, 1, 78}, {-1, 0, 79},    // r
     {1, 0, 79}, {0, -1, 79}, {0, 1, 79}, {0, 1, 129},    // v fb
     {0, 0, 129}, {0, 1, 145},    // v up
     {0, 0, 145}};

    for(int x = 0; x < 25; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag77Array[x][y] = Tag77[x][y];
        }
    }

    int Tag78[25][3] =
    {{-1, 0, 96}, // c     left plat
     {1, 0, 96}, {0, -1, 96}, {0, 1, 96}, {-1, 0, 76},    // t
     {1, 0, 76}, {0, -1, 76}, {0, 1, 76}, {-1, 0, 77},    // b
     {1, 0, 77}, {0, -1, 77}, {0, 1, 77}, {-1, 0, 78},    // l
     {1, 0, 78}, {0, -1, 78}, {0, 1, 78}, {-1, 0, 79},    // r
     {1, 0, 79}, {0, -1, 79}, {0, 1, 79}, {0, 0, 79}, {-1, 0, 130},    // h fb
     {0, 0, 130}, {-1, 0, 146},    // h up
     {0, 0, 146}};

    for(int x = 0; x < 25; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag78Array[x][y] = Tag78[x][y];
        }
    }

    int Tag79[25][3] =
    {{-1, 0, 96}, // c     right plat
     {1, 0, 96}, {0, -1, 96}, {0, 1, 96}, {-1, 0, 76},    // t
     {1, 0, 76}, {0, -1, 76}, {0, 1, 76}, {-1, 0, 77},    // b
     {1, 0, 77}, {0, -1, 77}, {0, 1, 77}, {-1, 0, 78},    // l
     {1, 0, 78}, {0, -1, 78}, {0, 1, 78}, {0, 0, 78}, {-1, 0, 79},    // r
     {1, 0, 79}, {0, -1, 79}, {0, 1, 79}, {1, 0, 130},    // h fb
     {0, 0, 130}, {1, 0, 146},    // h up
     {0, 0, 146}};

    for(int x = 0; x < 25; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag79Array[x][y] = Tag79[x][y];
        }
    }

    int Tag96[28][3] =
    {{-1, 0, 96}, // c       //concourse
     {1, 0, 96}, {0, -1, 96}, {0, 1, 96}, {-1, 0, 76},    // t
     {1, 0, 76}, {0, -1, 76}, {0, 1, 76}, {-1, 0, 77},    // b
     {1, 0, 77}, {0, -1, 77}, {0, 1, 77}, {-1, 0, 78},    // l
     {1, 0, 78}, {0, -1, 78}, {0, 1, 78}, {-1, 0, 79},    // r
     {1, 0, 79}, {0, -1, 79}, {0, 1, 79}, {0, 1, 129},    // v fb
     {0, -1, 129}, {1, 0, 130},    // h fb
     {-1, 0, 130}, {0, 1, 145},    // v up
     {0, -1, 145}, {1, 0, 146},    // h up
     {-1, 0, 146}};

    for(int x = 0; x < 28; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag96Array[x][y] = Tag96[x][y];
        }
    }

    int Tag129[8][3] = // vert fb
    {{0, -1, 96}, // c
     {0, -1, 77},    // b
     {0, -1, 129},    // v fb

     {0, 1, 96},    // c
     {0, 1, 76},    // t
     {0, 1, 129},    // v fb

     {0, 0, 76},    // t
     {0, 0, 77}};    // b

    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag129Array[x][y] = Tag129[x][y];
        }
    }

    int Tag145[8][3] = // vert up
    {{0, -1, 96}, // c
     {0, -1, 77},    // b
     {0, -1, 145},    // v fb

     {0, 1, 96},    // c
     {0, 1, 76},    // t
     {0, 1, 145},    // v fb

     {0, 0, 76},    // t
     {0, 0, 77}};    // b

    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag145Array[x][y] = Tag145[x][y];
        }
    }

    int Tag130[8][3] = // hor fb
    {{-1, 0, 96}, // c
     {-1, 0, 79},    // r
     {-1, 0, 130},    // h fb

     {1, 0, 96},    // c
     {1, 0, 78},    // l
     {1, 0, 130},    // h fb

     {0, 0, 78},    // l
     {0, 0, 79}};    // r

    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag130Array[x][y] = Tag130[x][y];
        }
    }

    int Tag146[8][3] = // hor up
    {{-1, 0, 96}, // c
     {-1, 0, 79},    // r
     {-1, 0, 146},    // h fb

     {1, 0, 96},    // c
     {1, 0, 78},    // l
     {1, 0, 146},    // h fb

     {0, 0, 78},    // l
     {0, 0, 79}};    // r

    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag146Array[x][y] = Tag146[x][y];
        }
    }

    int Tag131[4][3] =
    {{-1, 0, 131}, // n
     {1, 0, 131}, {0, -1, 131}, {0, 1, 131}};

    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            Tag131Array[x][y] = Tag131[x][y];
        }
    }

    int InternalFlipArray[FirstUnusedSpeedTagNumber] =
    {
        0, 1, 2, 5, 6, 3, 4, 9, 10, 7, 8, 13, 14, 11, 12, 15, 16, 17, 19, 18, 22, 23, 20, 21, 26, 27, 24, 25, 30, 31, 28, 29, 34, 35, 32, 33, 38, 39, 36, 37, 42,
        43, 40, 41, 45, 44, 47, 46, 48, 49, 51, 50, 53, 52, 55, 54, 57, 56, 59, 58, 60, 61, 63, 62, 66, 67, 64, 65, 68, 69, 71, 70, 74, 75, 72, 73, 77, 76, 78,
        79, 80, 81, 83, 82, 86, 87, 84, 85, 88, 89, 91, 90, 94, 95, 92, 93, 96, 99, 100, 97, 98, 103, 104, 101, 102, 106, 105, 109, 110, 107, 108, 113, 114,
        111, 112, 117, 118, 115, 116, 119, 120, 121, 123, 122, 124, 125, 126, 128, 127, 129, 130, 131, 134, 133, 132, 135, 139, 138, 137, 136, 143, 142, 141,
        140, 144, 145, 146
    };

    int InternalMirrorArray[FirstUnusedSpeedTagNumber] =
    {
        0, 1, 2, 4, 3, 6, 5, 8, 7, 10, 9, 12, 11, 14, 13, 15, 16, 17, 19, 18, 21, 20, 23, 22, 25, 24, 27, 26, 29, 28, 31, 30, 33, 32, 35, 34, 37, 36, 39, 38, 41,
        40, 43, 42, 45, 44, 47, 46, 48, 49, 51, 50, 53, 52, 55, 54, 57, 56, 59, 58, 61, 60, 62, 63, 65, 64, 67, 66, 69, 68, 70, 71, 73, 72, 75, 74, 76, 77, 79,
        78, 81, 80, 82, 83, 85, 84, 87, 86, 89, 88, 90, 91, 93, 92, 95, 94, 96, 98, 97, 100, 99, 102, 101, 104, 103, 106, 105, 108, 107, 110, 109, 112, 111,
        114, 113, 116, 115, 118, 117, 119, 120, 124, 122, 123, 121, 126, 125, 127, 128, 129, 130, 131, 132, 135, 134, 133, 137, 136, 139, 138, 142, 143, 140,
        141, 144, 145, 146
    };

    int InternalRotRightArray[FirstUnusedSpeedTagNumber] =
    {
        0, 2, 1, 4, 6, 3, 5, 14, 12, 13, 11, 7, 9, 8, 10, 15, 16, 17, 19, 18, 25, 27, 24, 26, 21, 23, 20, 22, 35, 33, 34, 32, 28, 30, 29, 31, 41, 43, 40, 42, 37,
        39, 36, 38, 46, 47, 44, 45, 49, 48, 51, 50, 56, 57, 58, 59, 52, 53, 54, 55, 63, 62, 60, 61, 65, 67, 64, 66, 71, 70, 68, 69, 73, 75, 72, 74, 79, 78, 76,
        77, 83, 82, 80, 81, 85, 87, 84, 86, 91, 90, 88, 89, 93, 95, 92, 94, 96, 102, 104, 101, 103, 98, 100, 97, 99, 106, 105, 108, 110, 107, 109, 116, 118,
        115, 117, 112, 114, 111, 113, 120, 119, 122, 124, 121, 123, 127, 128, 126, 125, 130, 129, 131, 133, 134, 135, 132, 137, 138, 139, 136, 143, 142, 140,
        141, 144, 146, 145
    };

    int InternalRotLeftArray[FirstUnusedSpeedTagNumber] =
    {
        0, 2, 1, 5, 3, 6, 4, 11, 13, 12, 14, 10, 8, 9, 7, 15, 16, 17, 19, 18, 26, 24, 27, 25, 22, 20, 23, 21, 32, 34, 33, 35, 31, 29, 30, 28, 42, 40, 43, 41, 38,
        36, 39, 37, 46, 47, 44, 45, 49, 48, 51, 50, 56, 57, 58, 59, 52, 53, 54, 55, 62, 63, 61, 60, 66, 64, 67, 65, 70, 71, 69, 68, 74, 72, 75, 73, 78, 79, 77,
        76, 82, 83, 81, 80, 86, 84, 87, 85, 90, 91, 89, 88, 94, 92, 95, 93, 96, 103, 101, 104, 102, 99, 97, 100, 98, 106, 105, 109, 107, 110, 108, 117, 115,
        118, 116, 113, 111, 114, 112, 120, 119, 123, 121, 124, 122, 128, 127, 125, 126, 130, 129, 131, 135, 132, 133, 134, 139, 136, 137, 138, 142, 143, 141,
        140, 144, 146, 145
    };

    for(int x = 0; x < FirstUnusedSpeedTagNumber; x++)
    {
        FlipArray[x] = InternalFlipArray[x];
        MirrorArray[x] = InternalMirrorArray[x];
        RotRightArray[x] = InternalRotRightArray[x];
        RotLeftArray[x] = InternalRotLeftArray[x];
    }
}

// ---------------------------------------------------------------------------
TTrack::~TTrack()
{
// delete TrackVectorPtr;
// delete FixedTrackArrayPtr;
    TTrack::TUserGraphicMap::iterator UGMIt = Track->UserGraphicMap.begin();

    while(UGMIt != Track->UserGraphicMap.end()) // delete all the TPictures in the map
    {
        delete UGMIt->second;
        UGMIt++;
    }
    delete GapFlashGreen;
    delete GapFlashRed;
    // all the rest are cleared by the relevant automatic destructors
}

// ---------------------------------------------------------------------------

TTrack::TFixedTrackArray::TFixedTrackArray()
{
    Graphics::TBitmap *TrackImageArray[FirstUnusedSpeedTagNumber] =
    {
// loc 0 not used, set to bmSolidBgnd
        RailGraphics->bmSolidBgnd, RailGraphics->gl1, RailGraphics->gl2, RailGraphics->gl3, RailGraphics->gl4, RailGraphics->gl5, RailGraphics->gl6,
        RailGraphics->bm7, RailGraphics->bm8, RailGraphics->bm9, RailGraphics->bm10, RailGraphics->bm11, RailGraphics->bm12, RailGraphics->bm13,
        RailGraphics->bm14, RailGraphics->gl15, RailGraphics->bm16, RailGraphics->bmSolidBgnd,
// no 17 not used (was used for text in early phases), set to bmSolidBgnd
        RailGraphics->bm18, RailGraphics->gl19, RailGraphics->bm20, RailGraphics->gl21, RailGraphics->gl22, RailGraphics->gl23, RailGraphics->gl24,
        RailGraphics->gl25, RailGraphics->gl26, RailGraphics->bm27, RailGraphics->bm28, RailGraphics->bm29, RailGraphics->bm30, RailGraphics->bm31,
        RailGraphics->bm32, RailGraphics->bm33, RailGraphics->bm34, RailGraphics->bm35, RailGraphics->bm36, RailGraphics->bm37, RailGraphics->bm38,
        RailGraphics->bm39, RailGraphics->bm40, RailGraphics->bm41, RailGraphics->bm42, RailGraphics->bm43, RailGraphics->gl44, RailGraphics->bm45,
        RailGraphics->bm46, RailGraphics->gl47, RailGraphics->gl48, RailGraphics->gl49, RailGraphics->bm50, RailGraphics->bm51, RailGraphics->gl52,
        RailGraphics->bm53, RailGraphics->bm54, RailGraphics->gl55, RailGraphics->bm56, RailGraphics->gl57, RailGraphics->gl58, RailGraphics->bm59,
        RailGraphics->gl60, RailGraphics->gl61, RailGraphics->gl62, RailGraphics->gl63, RailGraphics->gl64, RailGraphics->bm65, RailGraphics->gl66,
        RailGraphics->gl67, RailGraphics->gl68, RailGraphics->gl69, RailGraphics->gl70, RailGraphics->gl71, RailGraphics->gl72, RailGraphics->bm73,
        RailGraphics->bm74, RailGraphics->gl75, RailGraphics->gl76, RailGraphics->bm77, RailGraphics->bm78, RailGraphics->gl79, RailGraphics->gl80,
        RailGraphics->gl81, RailGraphics->gl82, RailGraphics->gl83, RailGraphics->gl84, RailGraphics->bm85, RailGraphics->gl86, RailGraphics->gl87,
        RailGraphics->gl88unset, RailGraphics->gl89unset, RailGraphics->gl90unset, RailGraphics->gl91unset, RailGraphics->gl92unset, RailGraphics->bm93unset,
        RailGraphics->bm94unset, RailGraphics->gl95unset, RailGraphics->Concourse, RailGraphics->gl97, RailGraphics->gl98, RailGraphics->gl99,
        RailGraphics->bm100, RailGraphics->bm101, RailGraphics->gl102, RailGraphics->gl103, RailGraphics->gl104, RailGraphics->gl105, RailGraphics->bm106,
        RailGraphics->gl107, RailGraphics->gl108, RailGraphics->gl109, RailGraphics->gl110, RailGraphics->gl111, RailGraphics->gl112, RailGraphics->gl113,
        RailGraphics->gl114, RailGraphics->gl115, RailGraphics->gl116, RailGraphics->gl117, RailGraphics->gl118, RailGraphics->gl119, RailGraphics->gl120,
        RailGraphics->gl121, RailGraphics->gl122, RailGraphics->gl123, RailGraphics->gl124, RailGraphics->gl125, RailGraphics->gl126, RailGraphics->gl127,
        RailGraphics->gl128, RailGraphics->gl129, RailGraphics->gl130, RailGraphics->bmName, RailGraphics->bm132, RailGraphics->bm133, RailGraphics->bm134,
        RailGraphics->bm135, RailGraphics->bm136, RailGraphics->bm137, RailGraphics->bm138, RailGraphics->bm139, RailGraphics->bm140, RailGraphics->bm141,
        RailGraphics->gl142, RailGraphics->gl143, RailGraphics->LCBothHor, RailGraphics->gl145, RailGraphics->gl146
    };

    Graphics::TBitmap *SmallTrackImageArray[FirstUnusedSpeedTagNumber] =
    {
// loc 0 not used, set to smSolidBgnd
        RailGraphics->smSolidBgnd, RailGraphics->sm1, RailGraphics->sm2, RailGraphics->sm3, RailGraphics->sm4, RailGraphics->sm5, RailGraphics->sm6,
        RailGraphics->sm7, RailGraphics->sm8, RailGraphics->sm9, RailGraphics->sm10, RailGraphics->sm11, RailGraphics->sm12, RailGraphics->sm13,
        RailGraphics->sm14, RailGraphics->sm15, RailGraphics->sm16, RailGraphics->smSolidBgnd,
// no 17 not used (was used for text in early phases), set to smSolidBgnd
        RailGraphics->sm18, RailGraphics->sm19, RailGraphics->sm20, RailGraphics->sm21, RailGraphics->sm22, RailGraphics->sm23, RailGraphics->sm24,
        RailGraphics->sm25, RailGraphics->sm26, RailGraphics->sm27, RailGraphics->sm28, RailGraphics->sm29, RailGraphics->sm30, RailGraphics->sm31,
        RailGraphics->sm32, RailGraphics->sm33, RailGraphics->sm34, RailGraphics->sm35, RailGraphics->sm36, RailGraphics->sm37, RailGraphics->sm38,
        RailGraphics->sm39, RailGraphics->sm40, RailGraphics->sm41, RailGraphics->sm42, RailGraphics->sm43, RailGraphics->sm44, RailGraphics->sm45,
        RailGraphics->sm46, RailGraphics->sm47, RailGraphics->sm48, RailGraphics->sm49, RailGraphics->sm50, RailGraphics->sm51, RailGraphics->sm52,
        RailGraphics->sm53, RailGraphics->sm54, RailGraphics->sm55, RailGraphics->sm56, RailGraphics->sm57, RailGraphics->sm58, RailGraphics->sm59,
        RailGraphics->sm60, RailGraphics->sm61, RailGraphics->sm62, RailGraphics->sm63, RailGraphics->sm64, RailGraphics->sm65, RailGraphics->sm66,
        RailGraphics->sm67, RailGraphics->sm68, RailGraphics->sm69, RailGraphics->sm70, RailGraphics->sm71, RailGraphics->sm72, RailGraphics->sm73,
        RailGraphics->sm74, RailGraphics->sm75, RailGraphics->sm76, RailGraphics->sm77, RailGraphics->sm78, RailGraphics->sm79, RailGraphics->sm80,
        RailGraphics->sm81, RailGraphics->sm82, RailGraphics->sm83, RailGraphics->sm84, RailGraphics->sm85, RailGraphics->sm86, RailGraphics->sm87,
        RailGraphics->sm88, RailGraphics->sm89, RailGraphics->sm90, RailGraphics->sm91, RailGraphics->sm92, RailGraphics->sm93, RailGraphics->sm94,
        RailGraphics->sm95, RailGraphics->sm96, RailGraphics->sm97, RailGraphics->sm98, RailGraphics->sm99, RailGraphics->sm100, RailGraphics->sm101,
        RailGraphics->sm102, RailGraphics->sm103, RailGraphics->sm104, RailGraphics->sm105, RailGraphics->sm106, RailGraphics->sm107, RailGraphics->sm108,
        RailGraphics->sm109, RailGraphics->sm110, RailGraphics->sm111, RailGraphics->sm112, RailGraphics->sm113, RailGraphics->sm114, RailGraphics->sm115,
        RailGraphics->sm116, RailGraphics->sm117, RailGraphics->sm118, RailGraphics->sm119, RailGraphics->sm120, RailGraphics->sm121, RailGraphics->sm122,
        RailGraphics->sm123, RailGraphics->sm124, RailGraphics->sm125, RailGraphics->sm126, RailGraphics->sm127, RailGraphics->sm128, RailGraphics->sm129,
        RailGraphics->sm130, RailGraphics->smName, RailGraphics->sm132, RailGraphics->sm133, RailGraphics->sm134, RailGraphics->sm135, RailGraphics->sm136,
        RailGraphics->sm137, RailGraphics->sm138, RailGraphics->sm139, RailGraphics->sm18, RailGraphics->sm18, RailGraphics->sm19, RailGraphics->sm19,
        RailGraphics->smLC, RailGraphics->sm129, RailGraphics->sm130 // use small footbridges for underpasses
    };

// track types
    TTrackType TrackTypeArray[FirstUnusedSpeedTagNumber] =
    {
        Erase, // 1    0
        Simple, Simple, Simple, Simple, Simple, Simple, // 6    1-6
        Points, Points, Points, Points, Points, Points, Points, Points, // 8    7-14
        Crossover, Crossover, // 2    15-16
        Unused, // 17 (was for text in earlier development)                     //1    17
        Simple, Simple, Simple, Simple, Simple, Simple, Simple, Simple, Simple, Simple, // 10   18-27
        Points, Points, Points, Points, Points, Points, Points, Points, Points, Points, Points, Points, Points, Points, Points, Points, // 16   28-43
        Crossover, Crossover, Crossover, Crossover, // 4    44-47
        Bridge, Bridge, Bridge, Bridge, Bridge, Bridge, Bridge, Bridge, Bridge, Bridge, Bridge, Bridge, // 12   48-59
        Buffers, Buffers, Buffers, Buffers, Buffers, Buffers, Buffers, Buffers, // 8    60-67
        SignalPost, SignalPost, SignalPost, SignalPost, SignalPost, SignalPost, SignalPost, SignalPost, // 8    68-75
        Platform, Platform, Platform, Platform, // 4    76-79
        Continuation, Continuation, Continuation, Continuation, Continuation, Continuation, Continuation, Continuation, // 8    80-87
        GapJump, GapJump, GapJump, GapJump, GapJump, GapJump, GapJump, GapJump, // 8    88-95
        Concourse, // 1    96
        Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet,
        Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, Parapet, // 28   97-124
        Simple, Simple, Simple, Simple, // 4    125-128
        FootCrossing, FootCrossing, // 2    129-130
        NamedNonStationLocation, // 1    131
        Points, Points, Points, Points, Points, Points, Points, Points, // 8    132-139
        Simple, Simple, Simple, Simple, // 4    140-143
        LevelCrossing, // 1    144
        FootCrossing, FootCrossing // 2    145 & 146
    };

// links
    int Links[FirstUnusedSpeedTagNumber][4] =
    {{-1, -1, -1, -1}, // erase element
     {4, 6, -1, -1}, {2, 8, -1, -1}, {6, 8, -1, -1}, {4, 8, -1, -1}, {2, 6, -1, -1}, {2, 4, -1, -1},    // simple
     {4, 6, 4, 2}, {6, 4, 6, 2}, {4, 6, 4, 8}, {6, 4, 6, 8}, {8, 2, 8, 4}, {8, 2, 8, 6}, {2, 8, 2, 4}, {2, 8, 2, 6},    // points
// points always have links 0 & 2 = lead, link 1 = trailing straight, link 3 = trailing diverging
     {4, 6, 2, 8}, {1, 9, 3, 7},    // crossover links 0 & 1 = diagonal top left to Bottom right, then horizontal, then vertical
     {-1, -1, -1, -1},    // unused
     {3, 7, -1, -1}, {1, 9, -1, -1}, {7, 6, -1, -1}, {4, 9, -1, -1}, {1, 6, -1, -1}, {4, 3, -1, -1}, {3, 8, -1, -1}, {1, 8, -1, -1}, {2, 9, -1, -1},
     {2, 7, -1, -1},    // simple
     {4, 6, 4, 3}, {6, 4, 6, 1}, {4, 6, 4, 9}, {6, 4, 6, 7}, {8, 2, 8, 1}, {8, 2, 8, 3}, {2, 8, 2, 7}, {2, 8, 2, 9}, {9, 1, 9, 2}, {7, 3, 7, 2}, {3, 7, 3, 8}, {1, 9, 1, 8}, {9, 1, 9, 4}, {7, 3, 7, 6}, {3, 7, 3, 4}, {1, 9, 1, 6}, // points
// points always have links 0 & 2 = lead, link 1 = trailing straight (or left diverging if no straight), link 3 = trailing diverging
// (or right diverging if no straight)
     {1, 9, 2, 8}, {2, 8, 3, 7}, {4, 6, 3, 7}, {1, 9, 4, 6},    // crossover links 0 & 1 = diagonal top left to Bottom right, then horizontal, then vertical
     {2, 8, 4, 6}, {4, 6, 2, 8}, {3, 7, 1, 9}, {1, 9, 3, 7}, {2, 8, 1, 9}, {2, 8, 3, 7}, {3, 7, 2, 8}, {1, 9, 2, 8}, {4, 6, 3, 7}, {4, 6, 1, 9}, {1, 9, 4, 6}, {3, 7, 4, 6}, // bridge, links 2 & 3 = underbridge
     {4, 6, -1, -1}, {6, 4, -1, -1}, {8, 2, -1, -1}, {2, 8, -1, -1}, {1, 9, -1, -1}, {3, 7, -1, -1}, {7, 3, -1, -1}, {9, 1, -1, -1}, // buffers - position 0 = buffer
     {4, 6, -1, -1}, {4, 6, -1, -1}, {2, 8, -1, -1}, {2, 8, -1, -1}, {1, 9, -1, -1}, {3, 7, -1, -1}, {3, 7, -1, -1}, {1, 9, -1, -1}, // signals (need Config to determine signal end, see below)
     {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1},    // platform
     {4, 6, -1, -1}, {6, 4, -1, -1}, {8, 2, -1, -1}, {2, 8, -1, -1}, {1, 9, -1, -1}, {3, 7, -1, -1}, {7, 3, -1, -1}, {9, 1, -1, -1}, // continuation - position 0 = continuation
     {4, 6, -1, -1}, {6, 4, -1, -1}, {8, 2, -1, -1}, {2, 8, -1, -1}, {1, 9, -1, -1}, {3, 7, -1, -1}, {7, 3, -1, -1}, {9, 1, -1, -1}, // gapjump - position 0 = gap
     {-1, -1, -1, -1},    // Concourse
     {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1},
     {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1},
     {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1},
     {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1}, {-1, -1, -1, -1},    // Parapets
     {4, 6, -1, -1}, {4, 6, -1, -1}, {2, 8, -1, -1}, {2, 8, -1, -1},    // arrows
     {4, 6, -1, -1}, {2, 8, -1, -1},    // footbridges
     {-1, -1, -1, -1},    // NamedNonStationLocation
     {8, 1, 8, 3}, {4, 3, 4, 9}, {2, 9, 2, 7}, {6, 7, 6, 1}, {9, 4, 9, 2}, {7, 2, 7, 6}, {1, 6, 1, 8}, {3, 8, 3, 4},    // points without straight legs
// these points have links 0 & 2 = lead, link 1 = LH trailing, link 3 = RH trailing
     {3, 7, -1, -1}, {3, 7, -1, -1}, {1, 9, -1, -1}, {1, 9, -1, -1},    // arrowed diagonals
     {-1, -1, -1, -1},    // level crossing
     {4, 6, -1, -1}, {2, 8, -1, -1},    // underpasses/surface crossings
    };

    TConfiguration Configs[FirstUnusedSpeedTagNumber][4] =
    {{NotSet, NotSet, NotSet, NotSet}, // unused
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},    // simple
     {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail},
     {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail},    // points
     {CrossConn, CrossConn, CrossConn, CrossConn}, {CrossConn, CrossConn, CrossConn, CrossConn},    // crossover
     {NotSet, NotSet, NotSet, NotSet},    // unused
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},
     {Connection, Connection, NotSet, NotSet},    // simple
     {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail},
     {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail},
     {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail},
     {Lead, Trail, Lead, Trail},    // points
     {CrossConn, CrossConn, CrossConn, CrossConn}, {CrossConn, CrossConn, CrossConn, CrossConn}, {CrossConn, CrossConn, CrossConn, CrossConn},
     {CrossConn, CrossConn, CrossConn, CrossConn},    // crossover
     {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under},
     {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under},
     {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under},
     {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under}, {Connection, Connection, Under, Under},    // bridge
     {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet},
     {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet},    // buffers
     {Connection, Signal, NotSet, NotSet}, {Signal, Connection, NotSet, NotSet}, {Signal, Connection, NotSet, NotSet}, {Connection, Signal, NotSet, NotSet},
     {Signal, Connection, NotSet, NotSet}, {Signal, Connection, NotSet, NotSet}, {Connection, Signal, NotSet, NotSet}, {Connection, Signal, NotSet, NotSet}, // signals (signal at exit end in forward direction)
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},    // platform
     {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet},
     {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, {End, Connection, NotSet, NotSet}, // continuation
     {Gap, Connection, NotSet, NotSet}, {Gap, Connection, NotSet, NotSet}, {Gap, Connection, NotSet, NotSet}, {Gap, Connection, NotSet, NotSet},
     {Gap, Connection, NotSet, NotSet}, {Gap, Connection, NotSet, NotSet}, {Gap, Connection, NotSet, NotSet}, {Gap, Connection, NotSet, NotSet},    // gapjump
     {NotSet, NotSet, NotSet, NotSet},    // Concourse
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},
     {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet}, {NotSet, NotSet, NotSet, NotSet},    // Parapets
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},
     {Connection, Connection, NotSet, NotSet},    // Arrows
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},    // Footbridges
     {NotSet, NotSet, NotSet, NotSet},    // NamedNonStationLocation
     {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail},
     {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail}, {Lead, Trail, Lead, Trail},    // points
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet},
     {Connection, Connection, NotSet, NotSet},    // Arrowed diagonals
     {NotSet, NotSet, NotSet, NotSet},    // Level crossing
     {Connection, Connection, NotSet, NotSet}, {Connection, Connection, NotSet, NotSet}    // Underpasses/surface crossings
    };

    for(int x = 0; x < 17; x++)
    {
        FixedTrackPiece[x] = TFixedTrackPiece(x, TrackTypeArray[x], Links[x], Configs[x], TrackImageArray[x], SmallTrackImageArray[x]);
    }
    FixedTrackPiece[17] = TFixedTrackPiece(17, TrackTypeArray[17], Links[17], Configs[17], 0, 0);
// 17 was the old text value so don't want any graphics (now disused)
    for(int x = 18; x < FirstUnusedSpeedTagNumber; x++)
    {
        FixedTrackPiece[x] = TFixedTrackPiece(x, TrackTypeArray[x], Links[x], Configs[x], TrackImageArray[x], SmallTrackImageArray[x]);
    }
}

// ---------------------------------------------------------------------------
TGraphicElement::TGraphicElement() : OverlayPlotted(false), OriginalLoaded(false), ScreenSourceSet(false), ScreenGraphicLoaded(false),
    ExistingGraphicLoaded(false), Width(16), Height(16)
{
    OriginalGraphic = new Graphics::TBitmap;
    OriginalGraphic->PixelFormat = pf8bit;
    OriginalGraphic->Width = Width;
    OriginalGraphic->Height = Height;
    OriginalGraphic->Transparent = false; // seems to default to false but set it to be sure, so no need to plot a blank before each replot
}

// ---------------------------------------------------------------------------

TGraphicElement::TGraphicElement(int WidthIn, int HeightIn) : OverlayPlotted(false), OriginalLoaded(false), ScreenSourceSet(false), ScreenGraphicLoaded(false),
    ExistingGraphicLoaded(false), Width(WidthIn), Height(HeightIn)
{
    OriginalGraphic = new Graphics::TBitmap;
    OriginalGraphic->PixelFormat = pf8bit;
    OriginalGraphic->Width = Width;
    OriginalGraphic->Height = Height;
    OriginalGraphic->Transparent = false; // seems to default to false but set it to be sure, so no need to plot a blank before each replot
}

// ---------------------------------------------------------------------------

TGraphicElement::~TGraphicElement()
{
    delete OriginalGraphic;
}

// ---------------------------------------------------------------------------

void TGraphicElement::SetScreenHVSource(int Caller, int HPosIn, int VPosIn)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetScreenHVSource," + AnsiString(HPosIn) + "," + AnsiString(VPosIn));
    HPos = HPosIn; // HPos & VPos are members of TGraphicElement
    VPos = VPosIn;
    int Left, Top; // can't use e.g. PointFlash.SourceRect.Left & Top directly as references as don't exist as objects in their own right

    Track->GetScreenPositionsFromTruePos(2, Left, Top, HPos, VPos);
    SourceRect.init(Left, Top, Left + Width, Top + Height);
    ScreenSourceSet = true;
    Utilities->CallLogPop(422);
}

// ---------------------------------------------------------------------------

void TGraphicElement::LoadOriginalScreenGraphic(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadOriginalScreenGraphic");
    if(!OverlayLoaded)
    {
        throw Exception("Overlay not loaded in TGraphicElement::LoadOriginalScreenGraphic()");
    }
    if((OverlayGraphic->Width != 16) || (OverlayGraphic->Height != 16))
    {
        throw Exception("Overlay not 16x16 in TGraphicElement::LoadOriginalScreenGraphic()");
    }
    if(!ScreenSourceSet)
    {
        throw Exception("Source not set in TGraphicElement::LoadOriginalScreenGraphic()");
    }
    if(ExistingGraphicLoaded) // can only call one of the load functions
    {
        throw Exception("ExistingGraphicLoaded in TGraphicElement::LoadOriginalScreenGraphic()");
    }
    if(OverlayPlotted) // don't load from screen if overlay plotted
    {
        Utilities->CallLogPop(775);
        return;
    }
    TRect DestRect(0, 0, Width, Height);

    Display->GetRectangle(0, DestRect, SourceRect, OriginalGraphic);
    OriginalLoaded = true;
    ScreenGraphicLoaded = true;
    Utilities->CallLogPop(423);
}

// ---------------------------------------------------------------------------

void TGraphicElement::LoadOriginalExistingGraphic(int Caller, int HOffset, int VOffset, int WidthIn, int HeightIn, Graphics::TBitmap *Graphic)
/*
      Overrides size set in the constructor, SourceRect & HPos & VPos in SetScreenHVSource
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadOriginalExistingGraphic," + AnsiString(HOffset) + "," +
                                 AnsiString(VOffset) + "," + AnsiString(WidthIn) + "," + AnsiString(HeightIn));
    if(!OverlayLoaded)
    {
        throw Exception("Overlay not loaded in TGraphicElement::LoadOriginalExistingGraphic()");
    }
    if(!ScreenSourceSet) // has to be called to set HPos & VPos
    {
        throw Exception("Source not set in TGraphicElement::LoadOriginalExistingGraphic()");
    }
    if(ScreenGraphicLoaded) // can only call one of the load functions
    {
        throw Exception("ScreenGraphicLoaded in TGraphicElement::LoadOriginalExistingGraphic()");
    }
    Width = WidthIn;
    Height = HeightIn;
    OriginalGraphic->Width = Width;
    OriginalGraphic->Height = Height;
    HPos += HOffset; // originally set in SetScreenHVSource to position of H & V locations
    VPos += VOffset;
    TRect DestRect(0, 0, Width, Height);

    SourceRect.init(HOffset, VOffset, HOffset + Width, VOffset + Height);
    OriginalGraphic->Canvas->CopyRect(DestRect, Graphic->Canvas, SourceRect);
    OriginalLoaded = true;
    ExistingGraphicLoaded = true;
    Utilities->CallLogPop(424);
}

// ---------------------------------------------------------------------------

void TGraphicElement::LoadOverlayGraphic(int Caller, Graphics::TBitmap *Overlay)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadOverlayGraphic,");
    OverlayGraphic = Overlay;
    OverlayLoaded = true;
    Utilities->CallLogPop(425);
}

// ---------------------------------------------------------------------------

void TGraphicElement::PlotOverlay(int Caller, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotOverlay,");
    if(!OverlayLoaded)
    {
        throw Exception("Overlay not loaded in TGraphicElement::PlotOverlay()");
    }
    if(!OverlayPlotted)
    {
        Disp->PlotOutput(35, HPos, VPos, OverlayGraphic); // plot overlay
        Disp->Update();
        OverlayPlotted = true;
    }
    Utilities->CallLogPop(426);
}

// ---------------------------------------------------------------------------

void TGraphicElement::PlotOriginal(int Caller, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotOriginal,");
    if(OverlayPlotted)
    {
        if(!OriginalLoaded) // this comes after OverlayPlotted because may wish to 'try' to plot original even
                            // when it isn't loaded in case it had been plotted - e.g. when change user modes
        {
            throw Exception("Original not loaded in TGraphicElement::PlotOriginal()");
        }
        Disp->PlotOutput(36, HPos, VPos, OriginalGraphic); // replot original
        Disp->Update(); // This was commented out originally but when in flashes much less frequent when points changing manually
        OverlayPlotted = false;
    }
    Utilities->CallLogPop(427);
}

// ---------------------------------------------------------------------------

bool TTrack::NoActiveOrInactiveTrack(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NoTrack");
    bool TrackPresent = false;

    if(InactiveTrackVector.size() != 0)
    {
        Utilities->CallLogPop(1333);
        return(false);
    }
    else if(TrackVector.size() == 0)
    {
        Utilities->CallLogPop(1334);
		return(true);
    }

	auto iterator_ = std::find_if(
		TrackVector.begin(),
		TrackVector.end(),
		[](auto x){return x.SpeedTag != 0;}
	);

    return iterator_ == TrackVector.end();
}

// ---------------------------------------------------------------------------

bool TTrack::NoActiveTrack(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NoActiveTrack");
    bool TrackPresent = false;

    if(TrackVector.empty())
    {
        Utilities->CallLogPop(1582);
        return(true);
    }


	auto iterator_ = std::find_if(
		TrackVector.begin(),
		TrackVector.end(),
		[](auto x){return x.SpeedTag != 0;}
	);

    Utilities->CallLogPop(1583);
    return iterator_ == TrackVector.end();
}

// ---------------------------------------------------------------------------

void TTrack::EraseTrackElement(int Caller, int HLocInput, int VLocInput, int &ErasedTrackVectorPosition, bool &TrackEraseSuccessfulFlag, bool InternalChecks)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EraseTrackElement," + AnsiString(HLocInput) + "," +
                                 AnsiString(VLocInput) + "," + AnsiString((short)InternalChecks));
    TrackEraseSuccessfulFlag = false;
// TrackEraseSuccessfulFlag used for both track element and inactive element erase,
// since have to match platforms as well as track
// used to set TrackFinished to false if an element erased

    ErasedTrackVectorPosition = -1; // marker for no element erased
    AnsiString SName = "", ErrorString;
    TLocationNameMultiMapIterator SNIt;
    THVPair TrackMapKeyPair, InactiveTrackMapKeyPair;
    TTrackMapIterator TrackMapPtr;
    TInactiveTrack2MultiMapIterator InactiveTrack2MultiMapIterator;

    if(TrackVector.empty())
    {
        TrackMapKeyPair.first = HLocInput;
        TrackMapKeyPair.second = VLocInput;
        TrackMapPtr = TrackMap.find(TrackMapKeyPair);
        if(TrackMapPtr != TrackMap.end())
        {
            bool FoundFlag;
            int VecPos = GetVectorPositionFromTrackMap(37, HLocInput, VLocInput, FoundFlag);
            if(FoundFlag) // should find it as it's in the map
            {
                if(TrackElementAt(629, VecPos).FixedNamedLocationElement) // footcrossings only
                {
                    SName = TrackElementAt(1, VecPos).LocationName;
                    SNIt = FindNamedElementInLocationNameMultiMap(7, SName, TrackVector.begin() + VecPos, ErrorString);
                    if(ErrorString != "")
                    {
                        throw Exception(ErrorString + " for EraseTrackElement 1");
                    }
                    LocationNameMultiMap.erase(SNIt);
                }
                TrackVector.erase(TrackVector.begin() + TrackMapPtr->second);
                // ensure erase vector element before map element as iterator no longer valid after a map erase
                TrackMap.erase(TrackMapPtr);
                FixedTrackArray.FixedTrackPiece[0].PlotFixedTrackElement(2, HLocInput, VLocInput); // plot a blank element
                // need to decrement all map element position values that lie above VecPos, since vector positions above this have all moved down one
                DecrementValuesInGapsAndTrackAndNameMaps(0, VecPos);
                ResetAnyNonMatchingGaps(1); // in case the deleted element was a set gap
                if(SName != "")
                {
                    EraseLocationAndActiveTrackElementNames(5, SName); // this instead of SearchForAndUpdateLocationName later (saves time)
                    int HPos, VPos;
                    if(TextHandler->FindText(1, SName, HPos, VPos))
                    {
                        if(TextHandler->TextErase(5, HPos, VPos, SName))
                        {
                            ;
                        } // condition not used

                    }
                }
                ErasedTrackVectorPosition = VecPos;
                TrackEraseSuccessfulFlag = true;
            }
        }
    }
    if(InactiveTrackVector.empty())
    {
        unsigned int VecPos;
        InactiveTrackMapKeyPair.first = HLocInput;
        InactiveTrackMapKeyPair.second = VLocInput;
        InactiveTrack2MultiMapIterator = InactiveTrack2MultiMap.find(InactiveTrackMapKeyPair);
        if(InactiveTrack2MultiMapIterator != InactiveTrack2MultiMap.end())
        {
            SName = "";
            VecPos = InactiveTrack2MultiMapIterator->second;
            if(InactiveTrackElementAt(0, VecPos).FixedNamedLocationElement)
            {
                SName = InactiveTrackElementAt(1, VecPos).LocationName;
                SNIt = FindNamedElementInLocationNameMultiMap(2, SName, InactiveTrackVector.begin() + VecPos, ErrorString);
                if(ErrorString != "")
                {
                    throw Exception(ErrorString + " for EraseTrackElement 2A");
                }
                LocationNameMultiMap.erase(SNIt);
            }
            InactiveTrackVector.erase(InactiveTrackVector.begin() + InactiveTrack2MultiMapIterator->second); // if inactive can erase immediately
            // ensure erase vector element before map element as iterator no longer valid after a map erase
            InactiveTrack2MultiMap.erase(InactiveTrack2MultiMapIterator);
            FixedTrackArray.FixedTrackPiece[0].PlotFixedTrackElement(1, HLocInput, VLocInput); // plot a blank element
            // need to decrement all map element position values that lie above VecPos, since vector positions above this have all moved down one
            DecrementValuesInInactiveTrackAndNameMaps(1, VecPos);
            TrackEraseSuccessfulFlag = true;
            if(SName != "")
            {
                EraseLocationAndActiveTrackElementNames(3, SName); // this instead of SearchForAndUpdateLocationName later (saves time)
                int HPos, VPos;
                if(TextHandler->FindText(2, SName, HPos, VPos))
                {
                    if(TextHandler->TextErase(6, HPos, VPos, SName))
                    {
                        ;
                    } // condition not used

                }
            }
        }
        if(InactiveTrackVector.size() != 0) // need to check again as last access may have erased the last element
        {
            InactiveTrack2MultiMapIterator = InactiveTrack2MultiMap.find(InactiveTrackMapKeyPair); // may be up to 2 elements (platforms) at same location
            if(InactiveTrack2MultiMapIterator != InactiveTrack2MultiMap.end())
            {
                SName = "";
                VecPos = InactiveTrack2MultiMapIterator->second;
                if(InactiveTrackElementAt(2, VecPos).FixedNamedLocationElement)
                {
                    SName = InactiveTrackElementAt(3, VecPos).LocationName;
                    SNIt = FindNamedElementInLocationNameMultiMap(3, SName, InactiveTrackVector.begin() + VecPos, ErrorString);
                    if(ErrorString != "")
                    {
                        throw Exception(ErrorString + " for EraseTrackElement 2B");
                    }
                    LocationNameMultiMap.erase(SNIt);
                }
                InactiveTrackVector.erase(InactiveTrackVector.begin() + InactiveTrack2MultiMapIterator->second); // if inactive can erase immediately
                InactiveTrack2MultiMap.erase(InactiveTrack2MultiMapIterator);
                // need to decrement all map element position values that lie above VecPos, since vector positions above this have all moved down one
                DecrementValuesInInactiveTrackAndNameMaps(2, VecPos);
                if(SName != "")
                {
                    EraseLocationAndActiveTrackElementNames(4, SName); // this instead of SearchForAndUpdateLocationName later (saves time)
                    int HPos, VPos;
                    if(TextHandler->FindText(3, SName, HPos, VPos))
                    {
                        if(TextHandler->TextErase(7, HPos, VPos, SName))
                        {
                            ;
                        } // condition not used

                    }
                }
            }
        }
    }
    if(TrackEraseSuccessfulFlag)
    {
        CalcHLocMinEtc(2);
        SetTrackFinished(false);
    }
    if(InternalChecks)
    {
        CheckMapAndTrack(1); // test
        CheckMapAndInactiveTrack(1); // test
        CheckLocationNameMultiMap(6); // test
    }
    Utilities->CallLogPop(428);
}

// ---------------------------------------------------------------------------

void TTrack::PlotAndAddTrackElement(int Caller, int CurrentTag, int Aspect, int HLocInput, int VLocInput, bool &TrackLinkingRequiredFlag, bool InternalChecks)
// TrackLinkingRequiredFlag only relates to elements that require track linking after plotting - used to set TrackFinished
// to false in calling function. New at v2.2.0 new parameter 'Aspect' to ensure signals plotted with correct number of aspects (for pasting)
// and also when zero and combined with SignalPost to indicate that adding track rather than pasting
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotAndAddTrackElement," + AnsiString(CurrentTag) + "," +
                                 AnsiString(HLocInput) + "," + AnsiString(VLocInput) + "," + AnsiString((short)InternalChecks));
    bool PlatAllowedFlag = false;

    TrackLinkingRequiredFlag = false;
/*
      Not erase, that covered separately.
      First check if Current SpeedButton assigned, then check if a platform and only
      permit if an appropriate trackpiece already there & not a same platform there.
      - can't enter a platform without track first.
      Then for non-platforms, check if a track piece already present at location &
      reject if so.
*/

    TLocationNameMultiMapEntry LocationNameEntry;

    LocationNameEntry.first = "";
    if(CurrentTag == 0)
    {
        Utilities->CallLogPop(429);
        return; // not assigned yet
    }
    TTrackElement TempTrackElement(FixedTrackArray.FixedTrackPiece[CurrentTag]);

    TempTrackElement.HLoc = HLocInput;
    TempTrackElement.VLoc = VLocInput;
    SetElementID(1, TempTrackElement); // TempTrackElement is the one to be added
// new at version 0.6 - set signal aspect depending on build mode

    if(TempTrackElement.TrackType == SignalPost)
    {
        if(Aspect == 0) // new at v2.2.0, '0' and SignalPost together means that track being added & not pasted, because when
                        // pasting a SignalPost can only have values 1 to 4
        {
            if(SignalAspectBuildMode == ThreeAspectBuild)
            {
                TempTrackElement.SigAspect = TTrackElement::ThreeAspect;
            }
            else if(SignalAspectBuildMode == TwoAspectBuild)
            {
                TempTrackElement.SigAspect = TTrackElement::TwoAspect;
            }
            else if(SignalAspectBuildMode == GroundSignalBuild)
            {
                TempTrackElement.SigAspect = TTrackElement::GroundSignal;
            }
            else
            {
                TempTrackElement.SigAspect = TTrackElement::FourAspect;
            }
        }
        else if(Aspect == 1)
        {
            TempTrackElement.SigAspect = TTrackElement::GroundSignal;
        }
        else if(Aspect == 2)
        {
            TempTrackElement.SigAspect = TTrackElement::TwoAspect;
        }
        else if(Aspect == 3)
        {
            TempTrackElement.SigAspect = TTrackElement::ThreeAspect;
        }
        else
        {
            TempTrackElement.SigAspect = TTrackElement::FourAspect;
        }
    }
    bool FoundFlag = false, InactiveFoundFlag = false, NonStationOrLevelCrossingPresent = false, PlatformPresent = false;
    int VecPos = GetVectorPositionFromTrackMap(12, HLocInput, VLocInput, FoundFlag); // active track already there
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(5, HLocInput, VLocInput, InactiveFoundFlag); // inactive track already there
    int InactiveSpeedTag1 = 0, InactiveSpeedTag2 = 0;

    if(InactiveFoundFlag) // check if a LocationName already there & if so disallow platform
    {
        if(InactiveTrackElementAt(4, IMPair.first).TrackType == NamedNonStationLocation)
        {
            NonStationOrLevelCrossingPresent = true;
        }
        if(InactiveTrackElementAt(117, IMPair.first).TrackType == LevelCrossing)
        {
            NonStationOrLevelCrossingPresent = true;
        }
        if(InactiveTrackElementAt(5, IMPair.first).TrackType == Platform)
        {
            PlatformPresent = true;
        }
        // no need to check IMPair.second since if that exists it is because .first is a platform
        InactiveSpeedTag1 = InactiveTrackElementAt(6, IMPair.first).SpeedTag;
        InactiveSpeedTag2 = InactiveTrackElementAt(7, IMPair.second).SpeedTag; // note .first & .second will be same if only one present
    }
// check platforms
    if(TempTrackElement.TrackType == Platform)
    {
        if(FoundFlag) // active track element already there
        {
            if(InactiveFoundFlag && ((TempTrackElement.SpeedTag == InactiveSpeedTag1) || (TempTrackElement.SpeedTag == InactiveSpeedTag2)))
            {
                ;
            }
            // same platform type already there so above keeps PlatAllowedFlag false
            else if((TempTrackElement.SpeedTag == 76) && (TopPlatAllowed.Contains(TrackElementAt(1044, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            // won't allow a same platform, as TopPlatAllowed not valid for a same platform <--NO, only checks active track, same plat disallowed by first line after if(FoundFlag)
            {
                PlatAllowedFlag = true;
            }
            else if((TempTrackElement.SpeedTag == 77) && (BotPlatAllowed.Contains(TrackElementAt(1045, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            {
                PlatAllowedFlag = true;
            }
            else if((TempTrackElement.SpeedTag == 78) && (LeftPlatAllowed.Contains(TrackElementAt(1046, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            {
                PlatAllowedFlag = true;
            }
            else if((TempTrackElement.SpeedTag == 79) && (RightPlatAllowed.Contains(TrackElementAt(1047, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            {
                PlatAllowedFlag = true;
            }
            if(PlatAllowedFlag)
            {
                TrackLinkingRequiredFlag = true; // needed in order to call LinkTrack
                TrackPush(1, TempTrackElement);
                SearchForAndUpdateLocationName(1, TempTrackElement.HLoc, TempTrackElement.VLoc, TempTrackElement.SpeedTag);
                // checks all adjacent locations and if any name found that one is used for all elements that are now linked to it
                // Must be called AFTER TrackPush
                // No need to plot the element - Clearand ... called after this function called
                // set corresponding track element length to 100m & give message if was different    drop in v2.4.0
                // note can only be Length01 since even if points then only the straight part can be adjacent to the platform
// drop in v2.4.0                if(TrackElementAt(2, VecPos).Length01 != DefaultTrackLength) ShowMessage("Note:  The track element at this location has a length of " +
// AnsiString(TrackElementAt(3, VecPos).Length01) + "m.  It will be reset to 100m since all platform track lengths are fixed at 100m");
// TrackElementAt(4, VecPos).Length01 = DefaultTrackLength;
                if(InternalChecks)
                {
                    CheckMapAndInactiveTrack(5); // test
                    CheckLocationNameMultiMap(4); // test
                }
                Utilities->CallLogPop(430);
                return;
            }
        } // if(FoundFlag)

        Utilities->CallLogPop(431);
        return;
    } // if platform

// check if element is a LocationName - OK if placed on an allowable track element, or on a blank element
    if(TempTrackElement.TrackType == NamedNonStationLocation)
    {
        if((FoundFlag && (NameAllowed.Contains(TrackElementAt(1048, VecPos).SpeedTag)) && !PlatformPresent && !InactiveFoundFlag) ||
           (!FoundFlag && !InactiveFoundFlag))
        // need to add && !NonStationOrLevelCrossingPresent, or better - !InactiveFoundFlag to above FoundFlag condition <-- OK done
        {
            TrackLinkingRequiredFlag = true; // needed in case have named a continuation, need to check if adjacent element named
            TrackPush(2, TempTrackElement);
            SearchForAndUpdateLocationName(2, TempTrackElement.HLoc, TempTrackElement.VLoc, TempTrackElement.SpeedTag);
            // checks all adjacent locations and if any name found that one is used for all elements that are now linked to it
            if(VecPos > -1) // need to allow for non-station named locations that aren't on tracks
            {
// drop in v2.4.0                if(TrackElementAt(830, VecPos).Length01 != DefaultTrackLength) ShowMessage("Note:  The track element at this location has a length of " +
// AnsiString(TrackElementAt(831, VecPos).Length01) + "m.  It will be reset to 100m since all named location track lengths are fixed at 100m");
// TrackElementAt(832, VecPos).Length01 = DefaultTrackLength; //NB named locations can only be placed at one track elements
            }
            if(InternalChecks)
            {
                CheckMapAndInactiveTrack(11); // test
                CheckLocationNameMultiMap(12); // test
            }
            Utilities->CallLogPop(432);
            return;
        }
        else
        {
            Utilities->CallLogPop(433);
            return;
        }
    }
// check if a level crossing - OK if placed on a plain straight track
    if(TempTrackElement.TrackType == LevelCrossing)
    {
        if(FoundFlag && (LevelCrossingAllowed.Contains(TrackElementAt(1049, VecPos).SpeedTag)) && !PlatformPresent && !InactiveFoundFlag)
        {
            TrackPush(11, TempTrackElement);
            PlotRaisedLinkedLevelCrossingBarriers(0, TrackElementAt(1050, VecPos).SpeedTag, TempTrackElement.HLoc, TempTrackElement.VLoc, Display); //always plots red
// no need for reference to LC element as can't be open
            TrackLinkingRequiredFlag = true;
            Utilities->CallLogPop(1907);
            return;
        }
        else
        {
            Utilities->CallLogPop(1906);
            return; // was a level crossing but can't place it for some reason
        }
    }

// check if another element already there
    else if(FoundFlag || InactiveFoundFlag)
    {
        Utilities->CallLogPop(434);
        return; // something already there (active or inactive track)
    }
// add LocationName if a FixedNamedLocationElement by checking for any adjacent names, then give all linked named location
// elements the same name - in case had linked 2 separately named locations - all get the one name that it finds
// first from an adjacent element search, also non-named location elements at platform locations have timetable name set
// do this after pushed into vector so that can use EnterLocationName

    if(TempTrackElement.FixedNamedLocationElement) // concourse or footcrossing (platforms & named non-station locations already dealt with)
    {
        TrackPush(3, TempTrackElement);
        SearchForAndUpdateLocationName(3, TempTrackElement.HLoc, TempTrackElement.VLoc, TempTrackElement.SpeedTag);
        // checks all adjacent locations and if any name found that one is used for all elements that are now linked to it
    }
    else if(TempTrackElement.TrackType == Points)
    {
        TrackPush(4, TempTrackElement);
        bool BothPointFillets = true;
        PlotPoints(6, TempTrackElement, Display, BothPointFillets);
    }
    else if(TempTrackElement.TrackType == SignalPost)
    {
        TrackPush(10, TempTrackElement);
        PlotSignal(12, TempTrackElement, Display);
    }
    else
    {
        TrackPush(5, TempTrackElement);
        TempTrackElement.PlotVariableTrackElement(1, Display); // all named locations already dealt with so no ambiguity between striped & non-striped
    }
    if((TempTrackElement.TrackType != Concourse) && (TempTrackElement.TrackType != Parapet))
    {
        TrackLinkingRequiredFlag = true; // plats & NamedLocs aleady dealt with
    }
    if(InternalChecks)
    {
        CheckMapAndTrack(2); // test
        CheckMapAndInactiveTrack(2); // test
        CheckLocationNameMultiMap(5); // test
    }
    Utilities->CallLogPop(2062);
}

// ---------------------------------------------------------------------------

void TTrack::PlotPastedTrackElementWithAttributes(int Caller, TTrackElement TempTrackElement, int HLocInput, int VLocInput, bool &TrackLinkingRequiredFlag,
                                                  bool InternalChecks)
// new at v2.2.0 - similar to above but keeping speed & length attributes (for pasting) and also pastes location names
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotPastedTrackElementWithAttributes," + AnsiString(HLocInput) + "," +
                                 AnsiString(VLocInput) + "," + AnsiString((short)InternalChecks));
    bool PlatAllowedFlag = false;

    TrackLinkingRequiredFlag = false;
    TLocationNameMultiMapEntry LocationNameEntry;

    LocationNameEntry.first = "";
	if(TempTrackElement.SpeedTag == 0)
    {
        Utilities->CallLogPop(2063);
        return; // not assigned yet
    }
    TempTrackElement.HLoc = HLocInput;
	TempTrackElement.VLoc = VLocInput;

    for(int x = 0; x < 4; x++) // unset any gaps
    {
        if(TempTrackElement.Config[x] == Gap)
        {
			TempTrackElement.ConnLinkPos[x] = -1;
		}
        TempTrackElement.Conn[x] = -1;
    }
    SetElementID(5, TempTrackElement); // TempTrackElement is the one to be added
// new at version 0.6 - set signal aspect depending on build mode
    bool FoundFlag = false, InactiveFoundFlag = false, NonStationOrLevelCrossingPresent = false, PlatformPresent = false;
    int VecPos = GetVectorPositionFromTrackMap(56, HLocInput, VLocInput, FoundFlag); // active track already there

    // if find an active track element (as has been pasted into track vector when dealing with inactive elements in SelectVector)
    // )set its ActiveTrackElementName to same name as the inactive element (from SelectVector). Note that can't use LocationName
    // for the active track element because these aren't set
    // if don't do this then get a mismatch error during map checks later

    // if(FoundFlag) TrackElementAt(xx, VecPos).ActiveTrackElementName = TempTrackElement.LocationName; //doesn't work!!

    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(26, HLocInput, VLocInput, InactiveFoundFlag); // inactive track already there
    int InactiveSpeedTag1 = 0, InactiveSpeedTag2 = 0;

    if(InactiveFoundFlag) // check if a LocationName already there & if so disallow platform
    {
        if(InactiveTrackElementAt(119, IMPair.first).TrackType == NamedNonStationLocation)
        {
            NonStationOrLevelCrossingPresent = true;
        }
        if(InactiveTrackElementAt(120, IMPair.first).TrackType == LevelCrossing)
        {
            NonStationOrLevelCrossingPresent = true;
        }
        if(InactiveTrackElementAt(121, IMPair.first).TrackType == Platform)
        {
            PlatformPresent = true;
        }
        // no need to check IMPair.second since if that exists it is because .first is a platform
        InactiveSpeedTag1 = InactiveTrackElementAt(122, IMPair.first).SpeedTag;
        InactiveSpeedTag2 = InactiveTrackElementAt(123, IMPair.second).SpeedTag; // note .first & .second will be same if only one present
    }
// check platforms
    if(TempTrackElement.TrackType == Platform)
    {
        if(FoundFlag) // active track element already there
        {
            if(InactiveFoundFlag && ((TempTrackElement.SpeedTag == InactiveSpeedTag1) || (TempTrackElement.SpeedTag == InactiveSpeedTag2)))
            {
                ;
            }
            // same platform type already there so above keeps PlatAllowedFlag false
            else if((TempTrackElement.SpeedTag == 76) && (TopPlatAllowed.Contains(TrackElementAt(1051, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            // won't allow a same platform, as TopPlatAllowed not valid for a same platform <--NO, only checks active track, same plat disallowed by first line after if(FoundFlag)
            {
                PlatAllowedFlag = true;
            }
            else if((TempTrackElement.SpeedTag == 77) && (BotPlatAllowed.Contains(TrackElementAt(1052, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            {
                PlatAllowedFlag = true;
            }
            else if((TempTrackElement.SpeedTag == 78) && (LeftPlatAllowed.Contains(TrackElementAt(1053, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            {
                PlatAllowedFlag = true;
            }
            else if((TempTrackElement.SpeedTag == 79) && (RightPlatAllowed.Contains(TrackElementAt(1054, VecPos).SpeedTag)) && !NonStationOrLevelCrossingPresent)
            {
                PlatAllowedFlag = true;
            }
            if(PlatAllowedFlag)
            {
                TrackLinkingRequiredFlag = true; // needed in order to call LinkTrack
                TrackPush(12, TempTrackElement);
//                if(!CopyFlag) // don't need this for copy - yes we do, this is so a location will be named if pasted next to a named location - condition removed at v2.6.0
                {
                    SearchForAndUpdateLocationName(4, TempTrackElement.HLoc, TempTrackElement.VLoc, TempTrackElement.SpeedTag);
                    // checks all adjacent locations and if any name found that one is used for all elements that are now linked to it
                }
                // Must be called AFTER TrackPush
// No need to plot the element - Clearand ... called after this function called
                // set corresponding track element length to 100m & give message if was different  drop in v2.4.0
                // note can only be Length01 since even if points then only the straight part can be adjacent to the platform
// drop in v2.4.0                if(TrackElementAt(907, VecPos).Length01 != DefaultTrackLength) ShowMessage("Note:  The track element at this location has a length of " +
// AnsiString(TrackElementAt(908, VecPos).Length01) + "m.  It will be reset to 100m since all platform track lengths are fixed at 100m");
// TrackElementAt(909, VecPos).Length01 = DefaultTrackLength;
                if(InternalChecks)
                {
                    CheckMapAndInactiveTrack(12); // test
                    CheckLocationNameMultiMap(20); // test
                }
                Utilities->CallLogPop(2064);
                return;
            }
        } // if(FoundFlag)

        Utilities->CallLogPop(2065);
        return;
    } // if platform

// check if element is a LocationName - OK if placed on an allowable track element, or on a blank element
    if(TempTrackElement.TrackType == NamedNonStationLocation)
    {
        if((FoundFlag && (NameAllowed.Contains(TrackElementAt(1055, VecPos).SpeedTag)) && !PlatformPresent && !InactiveFoundFlag) ||
           (!FoundFlag && !InactiveFoundFlag))
        // need to add && !NonStationOrLevelCrossingPresent, or better - !InactiveFoundFlag to above FoundFlag condition <-- OK done
        {
            TrackLinkingRequiredFlag = true; // needed in case have named a continuation, need to check if adjacent element named
            TrackPush(13, TempTrackElement);
//            if(!CopyFlag) // don't need this for copy - yes we do, this is so a location will be named if pasted next to a named location - condition removed at v2.6.0
            {
                SearchForAndUpdateLocationName(5, TempTrackElement.HLoc, TempTrackElement.VLoc, TempTrackElement.SpeedTag);
                // checks all adjacent locations and if any name found that one is used for all elements that are now linked to it
            }
            if(VecPos > -1) // need to allow for non-station named locations that aren't on tracks
            {
// drop in v2.4.0                if(TrackElementAt(910, VecPos).Length01 != DefaultTrackLength) ShowMessage("Note:  The track element at this location has a length of " +
// AnsiString(TrackElementAt(911, VecPos).Length01) + "m.  It will be reset to 100m since all named location track lengths are fixed at 100m");
// TrackElementAt(912, VecPos).Length01 = DefaultTrackLength; //NB named locations can only be placed at one track elements
            }
            if(InternalChecks)
            {
                CheckMapAndInactiveTrack(13); // test
                CheckLocationNameMultiMap(21); // test
            }
            Utilities->CallLogPop(2066);
            return;
        }
        else
        {
            Utilities->CallLogPop(2067);
            return;
        }
    }
// check if a level crossing - OK if placed on a plain straight track
    if(TempTrackElement.TrackType == LevelCrossing)
    {
        if(FoundFlag && (LevelCrossingAllowed.Contains(TrackElementAt(1056, VecPos).SpeedTag)) && !PlatformPresent && !InactiveFoundFlag)
        {
            TrackPush(14, TempTrackElement);
            PlotRaisedLinkedLevelCrossingBarriers(3, TrackElementAt(1057, VecPos).SpeedTag, TempTrackElement.HLoc, TempTrackElement.VLoc, Display); //always plots red
// no need for reference to LC element as can't be open
            TrackLinkingRequiredFlag = true;
            Utilities->CallLogPop(2068);
            return;
        }
        else
        {
            Utilities->CallLogPop(2069);
            return; // was a level crossing but can't place it for some reason
        }
    }

// check if another element already there
    else if(FoundFlag || InactiveFoundFlag)
    {
        Utilities->CallLogPop(2070);
        return; // something already there (active or inactive track)
    }
// add LocationName if a FixedNamedLocationElement by checking for any adjacent names, then give all linked named location
// elements the same name - in case had linked 2 separately named locations - all get the one name that it finds
// first from an adjacent element search, also non-named location elements at platform locations have timetable name set
// do this after pushed into vector so that can use EnterLocationName

    if(TempTrackElement.FixedNamedLocationElement) // concourse or footcrossing (platforms & named non-station locations already dealt with)
    {
        TrackPush(15, TempTrackElement);
        SearchForAndUpdateLocationName(6, TempTrackElement.HLoc, TempTrackElement.VLoc, TempTrackElement.SpeedTag);
        // checks all adjacent locations and if any name found that one is used for all elements that are now linked to it
    }
    else if(TempTrackElement.TrackType == Points)
    {
        TrackPush(16, TempTrackElement);
        bool BothPointFillets = true;
        PlotPoints(7, TempTrackElement, Display, BothPointFillets);
    }
    else if(TempTrackElement.TrackType == SignalPost)
    {
        TrackPush(17, TempTrackElement);
        PlotSignal(14, TempTrackElement, Display);
    }
    else
    {
        TrackPush(18, TempTrackElement);
        TempTrackElement.PlotVariableTrackElement(6, Display); // all named locations already dealt with so no ambiguity between striped & non-striped
    }
    if((TempTrackElement.TrackType != Concourse) && (TempTrackElement.TrackType != Parapet))
    {
        TrackLinkingRequiredFlag = true; // plats & NamedLocs aleady dealt with
    }
    if(InternalChecks)
    {
        CheckMapAndTrack(12); // test
        CheckMapAndInactiveTrack(14); // test
        CheckLocationNameMultiMap(22); // test
    }
    Utilities->CallLogPop(2071);
}

// ---------------------------------------------------------------------------

bool TTrack::TryToConnectTrack(int Caller, bool &LocError, int &HLoc, int &VLoc, bool GiveMessages)
// GiveMessages relates to the call to LinkTrack or  LinkTrackNoMessages
// return bool = true for success
// LocError = true for location error & HLoc & VLoc to be inverted
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TryToConnectTrack," + AnsiString((short)GiveMessages));
    LocError = false;
    SetTrackFinished(false);
    if(TrackVector.empty())
    {
        Utilities->CallLogPop(437);
        return(false);
    }
    if(GapsUnset(7))
    {
        if(GiveMessages)
        {
            ShowMessage("Gaps must be set before track can be validated");
        }
        Utilities->CallLogPop(1135);
        return(false);
    }
// below sets all Conns and CLks to -1 except for gapjumps that match and are properly set,
// returns true for any unset gaps
    if(ResetConnClkCheckUnsetGapJumps(1))
    {
        // can keep this exception as protected by the GapsUnset call above
        throw Exception("Error, gaps unset when TryToConnectTrack called");
    }
    BuildGapMapFromTrackVector(1);
    CheckGapMap(1); // test
// Gap connections now securely defined

    CheckMapAndTrack(8); // test

// Perform a pre-check prior to TrackMap being compiled
    if(GiveMessages)
    {
        if(!LinkTrack(1, LocError, HLoc, VLoc, false))
        {
            Utilities->CallLogPop(439);
            return(false);
        }
    }
    else
    {
        if(!LinkTrackNoMessages(1, false))
        {
            Utilities->CallLogPop(1131);
            return(false);
        }
    }
// here if pre-check successful
    if(!RepositionAndMapTrack(0))
    {
        ShowMessage("Error in RepositionAndMapTrack during TryToConnectTrack.  Railway file is corrupt, further use may cause a system crash");
        Utilities->CallLogPop(1138);
        return(false);
    }
// now perform the final assembly - FinalCall = true
    if(GiveMessages)
    {
        if(!LinkTrack(2, LocError, HLoc, VLoc, true))
        {
            Utilities->CallLogPop(1116);
            return(false);
        }
    }
    else
    {
        if(!LinkTrackNoMessages(2, true))
        {
            Utilities->CallLogPop(1132);
            return(false);
        }
    }
// success

    PopulateLCVector(0);
    CheckGapMap(2); // test
    CheckMapAndTrack(3); // test
    CheckMapAndInactiveTrack(3); // test
    CheckLocationNameMultiMap(9); // test
    SetTrackFinished(true);

// Build ContinuationNameMap
    std::pair<AnsiString, char>TempMapPair;

    ContinuationNameMap.clear();
    for(int x = 0; x < Track->TrackVectorSize(); x++)
    {
        if((Track->TrackElementAt(1058, x).TrackType == Continuation) && (Track->TrackElementAt(1059, x).ActiveTrackElementName != ""))
        {
            TempMapPair.first = Track->TrackElementAt(1060, x).ActiveTrackElementName;
            TempMapPair.second = 'x'; // unused
            ContinuationNameMap.insert(TempMapPair);
        }
    }

//check (provided TrackFinished is true) if any named (red) locations are without platforms, ie concourses only or concourses and foot crossings
//(don't report blue areas without track as these unlikely to be mistakes)

    if(TrackFinished)
    {
        AnsiString Name = "";
        typedef std::list<AnsiString> TNoPlatsList;
        TNoPlatsList::iterator NPLIt;
        TNoPlatsList NoPlatsList;
        typedef std::list<AnsiString> TLocNameList;
        TLocNameList LocNameList; //single entry for each name
        TLocationNameMultiMapRange MMRange;
        TLocationNameMultiMapIterator TempIt;
        for(TLocationNameMultiMapIterator LNMMIt = LocationNameMultiMap.begin(); LNMMIt != LocationNameMultiMap.end(); LNMMIt++)
        {
            LocNameList.push_back(LNMMIt->first);
        }
        LocNameList.sort();
        LocNameList.unique();
        for(TLocNameList::iterator LNLIt = LocNameList.begin(); LNLIt != LocNameList.end(); LNLIt++)
        {
            Name = *LNLIt;
            MMRange = LocationNameMultiMap.equal_range(Name);
            if(MMRange.first == MMRange.second) //can't find it - should always do but include as a safeguard
            {
                continue;
            }
            for(TLocationNameMultiMapIterator LNMMIt = MMRange.first; LNMMIt != MMRange.second; LNMMIt++)
            {
                if((LNMMIt->second) < 0) //active track element
                {
                    if(TrackElementAt(1401, -1 - LNMMIt->second).TrackType != FootCrossing)
                    {
                        break;
                    }
                }
                else //inactive
                {
                    if(InactiveTrackElementAt(1402, LNMMIt->second).TrackType != Concourse)
                    {
                        break;
                    }
                }
                TempIt = MMRange.second;
                if(LNMMIt == --TempIt) //reached last named element & all concourses or foot crossings
                {
                    NoPlatsList.push_back(Name);
                }
            }
        }
        if(!NoPlatsList.empty())
        {
            AnsiString NoPlatsAnsiList = "";
            for(NPLIt = NoPlatsList.begin(); NPLIt != NoPlatsList.end(); NPLIt++)
            {
                NoPlatsAnsiList += *NPLIt + '\n';
            }
            if(!NoPlatsMessageSent)
            {
                if(NoPlatsList.size() > 1)
                {
                    ShowMessage("Please note: the following locations have no platforms, trains won't be able to stop or pass there:-\n\n" + NoPlatsAnsiList + "\nThis message will not be shown again.");
                }
                else
                {
                    ShowMessage("Please note: the following location has no platforms, trains won't be able to stop or pass there:-\n\n" + NoPlatsAnsiList + "\nThis message will not be shown again.");
                }
                NoPlatsMessageSent = true;
            }
        }
    }
    Utilities->CallLogPop(440);
    return(true);
}

// ---------------------------------------------------------------------------
bool TTrack::ErrorInTrackBeforeSetGaps(int Caller, int &HLoc, int &VLoc)
// unused - too time-consuming - double brute force search
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ErrorInTrackBeforeSetGaps");
    int NewHLoc, NewVLoc;
    bool ConnectionFoundFlag, LinkFoundFlag;

    for(unsigned int x = 0; x < TrackVector.size(); x++) // check all elements in turn
    {
        for(unsigned int y = 0; y < 4; y++) // check all links for each element
        {
            if(TrackElementAt(1061, x).Link[y] <= 0)
            {
                continue; // no link
            }
            if(TrackElementAt(1062, x).Config[y] == End)
            {
                continue; // buffer or continuation
            }
            if(TrackElementAt(1063, x).Config[y] == Gap)
            {
                continue; // gap jump
            }
            // get required H & V for track element joining link 'y'
            NewHLoc = TrackElementAt(1064, x).HLoc + LinkHVArray[TrackElementAt(1065, x).Link[y]][0];
            NewVLoc = TrackElementAt(1066, x).VLoc + LinkHVArray[TrackElementAt(1067, x).Link[y]][1];
            // find track element if present
            ConnectionFoundFlag = false;
            for(unsigned int z = 0; z < TrackVector.size(); z++)
            {
// if(TrackElementAt(5, z).TrackType == Platform)
// continue;   //skip platforms
                if((TrackElementAt(1068, z).HLoc == NewHLoc) && (TrackElementAt(1069, z).VLoc == NewVLoc))
                {
                    ConnectionFoundFlag = true;
                    // find connecting link in the newly found track element if there is one
                    LinkFoundFlag = false;
                    for(unsigned int a = 0; a < 4; a++)
                    {
                        if(TrackElementAt(1070, z).Link[a] == (10 - TrackElementAt(1071, x).Link[y]))
                        {
                            LinkFoundFlag = true;
                        }
                    }
                    // if there isn't a corresponding link set the invert values for the offending element
                    if(!LinkFoundFlag)
                    {
                        HLoc = TrackElementAt(1072, x).HLoc;
                        VLoc = TrackElementAt(1073, x).VLoc;
                        Utilities->CallLogPop(441);
                        return(true);
                    }
                    break; // success, so break out of 'z' loop
                } // if((TrackElementAt(, z).HLoc== NewHLoc) &&....

            } // for z...
              // if there isn't a connection set the invert values for the offending element
            if(!ConnectionFoundFlag)
            {
                HLoc = TrackElementAt(1074, x).HLoc;
                VLoc = TrackElementAt(1075, x).VLoc;
                Utilities->CallLogPop(442);
                return(true);
            }
        } // for y....
    } // for x...
    Utilities->CallLogPop(443);
    return(false); // all OK
}

// ---------------------------------------------------------------------------

bool TTrack::FindNonPlatformMatch(int Caller, int HLoc, int VLoc, int &Position, TTrackElement &TrackElement) // true if find one
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindNonPlatformMatch," + AnsiString(HLoc) + "," + AnsiString(VLoc) +
                                 TrackElement.LogTrack(0));
    bool FoundFlag;

    Position = GetVectorPositionFromTrackMap(13, HLoc, VLoc, FoundFlag);
    if(FoundFlag)
    {
        TrackElement = TrackElementAt(1076, Position);
    }
    Utilities->CallLogPop(444);
    return(FoundFlag);
}

// ---------------------------------------------------------------------------

bool TTrack::ReturnNextTrackElement(int Caller, TTrackElement &Next)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ReturnNextTrackElement");
    if(NextTrackElementPtr >= TrackVector.end())
    {
        Utilities->CallLogPop(1336);
        return(false);
    }
    Next = *NextTrackElementPtr;
    NextTrackElementPtr++;
    Utilities->CallLogPop(1337);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::ReturnNextInactiveTrackElement(int Caller, TTrackElement &Next)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ReturnNextInactiveTrackElement");
    if(NextTrackElementPtr >= InactiveTrackVector.end())
    {
        Utilities->CallLogPop(1338);
        return(false);
    }
    Next = *NextTrackElementPtr;
    NextTrackElementPtr++;
    Utilities->CallLogPop(1339);
    return(true);
}

// ---------------------------------------------------------------------------

int TTrack::NumberOfGaps(int Caller)

{
	Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NumberOfGaps");

	if(TrackVector.empty())
	{
		Utilities->CallLogPop(1340);
		return(0);
	}

	const int Count = std::count_if(
		TrackVector.begin(),
		TrackVector.end(),
		[](auto x){return x.TrackType == GapJump;}
	);

    Utilities->CallLogPop(1341);
    return(Count);
}

// ---------------------------------------------------------------------------
bool TTrack::ResetConnClkCheckUnsetGapJumps(int Caller)
// above sets all Conns and CLks to -1 except for gapjumps that match and are properly set
// returns true for any unset gaps
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetConnClkCheckUnsetGapJumps");
    bool UnsetGaps = false;

    if(TrackVector.empty())
    {
        Utilities->CallLogPop(445);
        return(false);
    }
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(1078, x).TrackType != GapJump)
        {
            for(unsigned int y = 0; y < 4; y++)
            {
                TrackElementAt(1079, x).Conn[y] = -1;
                TrackElementAt(1080, x).ConnLinkPos[y] = -1;
            }
        }
        else // GapJump
        {
// int tempint = TrackElementAt(, x).Conn[0);

            if(TrackElementAt(1081, x).Conn[0] == -1) // unset if -1
            {
                for(unsigned int y = 0; y < 4; y++)
                {
                    TrackElementAt(1082, x).Conn[y] = -1;
                    TrackElementAt(1083, x).ConnLinkPos[y] = -1;
                }
                UnsetGaps = true;
                continue; // to next 'x'
            }
            else // set, but may not have matching element, or that element may not be set
            {
                for(unsigned int y = 1; y < 4; y++) // reset the non-gap values anyway, gap always at position 0
                {
                    TrackElementAt(1084, x).Conn[y] = -1;
                    TrackElementAt(1085, x).ConnLinkPos[y] = -1;
                }

                if(TrackElementAt(1086, TrackElementAt(1104, x).Conn[0]).TrackType != GapJump)
                // check that the element pointed to by the gap link is a GapJump & if not clear Conns & CLks & reset Lk[0]
                {
                    for(unsigned int y = 0; y < 4; y++)
                    {
                        TrackElementAt(1087, x).Conn[y] = -1;
                        TrackElementAt(1088, x).ConnLinkPos[y] = -1;
                    }
                    UnsetGaps = true;
                    continue; // to next 'x'
                }
// here if gap connection is itself a GapJump
                if(TrackElementAt(1089, TrackElementAt(1105, x).Conn[0]).Conn[0] != (int)x)
                // check that the element pointed to by the gap link is a GapJump & that its gap link points back to 'x'
                // if not clear Conns & CLks & reset Lk[0]
                {
                    for(unsigned int y = 0; y < 4; y++)
                    {
                        TrackElementAt(1090, x).Conn[y] = -1;
                        TrackElementAt(1091, x).ConnLinkPos[y] = -1;
                    }
                    UnsetGaps = true;
                    continue; // to next 'x'
                }
// here if gap connection itself points back to 'x' so these two GapJumps match properly
// hence no more action needed on these Conns & CLks
            }
        } // else //gap jump

    } // for x...
    Utilities->CallLogPop(446);
    return(UnsetGaps);
}

// ---------------------------------------------------------------------------

void TTrack::LoadTrack(int Caller, std::ifstream& VecFile, bool &GraphicsFollow)
{
// VecFile already open and its pointer at right place on calling
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadTrack");
    int TempInt;

    TrackClear(1);
// load track elements
    int NumberOfActiveElements = 0;

    GraphicsFollow = false;
    NumberOfActiveElements = Utilities->LoadFileInt(VecFile);
    AnsiString MarkerString = Utilities->LoadFileString(VecFile); // **Active elements** marker, if last character is '1' then there are graphics to be loaded

    if(MarkerString[MarkerString.Length()] == '1')
    {
        GraphicsFollow = true;
	}
	for(int x = 0; x < NumberOfActiveElements; x++)
    {
        VecFile >> TempInt; // TrackVectorNumber, not used
        VecFile >> TempInt; // SpeedTag
        TTrackElement TrackElement(FixedTrackArray.FixedTrackPiece[TempInt]);
        VecFile >> TempInt;
        TrackElement.HLoc = TempInt;
        VecFile >> TempInt;
        TrackElement.VLoc = TempInt;
        if(TrackElement.TrackType == GapJump)
        {
            VecFile >> TempInt;
            TrackElement.ConnLinkPos[0] = TempInt;
            VecFile >> TempInt;
            TrackElement.Conn[0] = TempInt;
        }
        if((TrackElement.TrackType == SignalPost) || (TrackElement.TrackType == Points))
        {
            VecFile >> TempInt;
            TrackElement.Attribute = TempInt;
        }
        if(TrackElement.TrackType == SignalPost)
        {
            VecFile >> TempInt;
            if(TempInt == 0)
            {
                TrackElement.CallingOnSet = false;
            }
            else
            {
                TrackElement.CallingOnSet = true;
            }
        }
        VecFile >> TempInt;
        TrackElement.Length01 = TempInt;
        VecFile >> TempInt;
        TrackElement.Length23 = TempInt;
        VecFile >> TempInt;
        if((TempInt != -1) && (TempInt < 10))
        {
            TempInt = 10; // added at v0.6 to ensure old railway speed limits at least 10km/h
        }
        if((TempInt != -1) && (TempInt > TTrain::MaximumSpeedLimit))
        {
            TempInt = TTrain::MaximumSpeedLimit; // added at v2.1.0 to limit max speed
        }
        TrackElement.SpeedLimit01 = TempInt;
        VecFile >> TempInt;
        if((TempInt != -1) && (TempInt < 10))
        {
            TempInt = 10; // added at v0.6 to ensure old railway speed limits at least 10km/h
        }
        if((TempInt != -1) && (TempInt > TTrain::MaximumSpeedLimit))
        {
            TempInt = TTrain::MaximumSpeedLimit; // added at v2.1.0 to limit max speed
        }
        TrackElement.SpeedLimit23 = TempInt;

        TrackElement.LocationName = Utilities->LoadFileString(VecFile);
        TrackElement.ActiveTrackElementName = Utilities->LoadFileString(VecFile);
        SetElementID(0, TrackElement);
        AnsiString Marker = Utilities->LoadFileString(VecFile); // marker
// new for v0.6
        if(TrackElement.TrackType == SignalPost)
        {
            if(Marker[1] == '3')
            {
                TrackElement.SigAspect = TTrackElement::ThreeAspect;
            }
            else if(Marker[1] == '2')
            {
                TrackElement.SigAspect = TTrackElement::TwoAspect;
            }
            else if(Marker[1] == 'G')
            {
                TrackElement.SigAspect = TTrackElement::GroundSignal;
            }
            else
            {
                TrackElement.SigAspect = TTrackElement::FourAspect;
            }
        }
        if(TrackElement.SpeedTag != 0)
        {
            TrackPush(8, TrackElement); // don't save default elements (now dispensed with)
        }
    }
    int NumberOfInactiveElements = 0;

    NumberOfInactiveElements = Utilities->LoadFileInt(VecFile);
    Utilities->LoadFileString(VecFile); // **Inactive elements** marker
    for(int x = 0; x < NumberOfInactiveElements; x++)
    {
        VecFile >> TempInt; // InactiveTrackVectorNumber - not used, only used for identification in file
        VecFile >> TempInt; // SpeedTag
        TTrackElement TrackElement(FixedTrackArray.FixedTrackPiece[TempInt]);
        VecFile >> TempInt;
        TrackElement.HLoc = TempInt;
        VecFile >> TempInt;
        TrackElement.VLoc = TempInt;
        TrackElement.LocationName = Utilities->LoadFileString(VecFile);
        SetElementID(3, TrackElement);
        TrackPush(9, TrackElement);
        Utilities->LoadFileString(VecFile); // marker
    }
    bool LocError = false; // needed for TryToConnectTrack but not used
    int H = -1, V = -1; // needed for TryToConnectTrack but not used

    if(TryToConnectTrack(2, LocError, H, V, false)) // false for don't give messages
    {
        SetTrackFinished(true);
    }
    else
    {
        SetTrackFinished(false);
    }
// CheckMapAndTrack(9); all these checked in TryToConnectTrack
// CheckMapAndInactiveTrack(8);
// CheckLocationNameMultiMap(10);
    Utilities->CallLogPop(448);
}

// ---------------------------------------------------------------------------

void TTrack::LoadGraphics(int Caller, std::ifstream &VecFile, UnicodeString GraphicsPath)
{
// VecFile already open and its pointer at right place on calling
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadGraphics, " + GraphicsPath);
// first int is number of graphics, then each graphic, create in UserGraphicMap, derive Width & height from TPicture
// & load into UserGraphicItem then store in UserGraphicVector
    UserGraphicVector.clear();
    TUserGraphicItem UGI;
    int NumberOfGraphics = Utilities->LoadFileInt(VecFile);

    for(int x = 0; x < NumberOfGraphics; x++)
    {
        UGI.FileName = GraphicsPath + "\\" + Utilities->LoadFileString(VecFile);
        UGI.HPos = Utilities->LoadFileInt(VecFile);
        UGI.VPos = Utilities->LoadFileInt(VecFile);
        UGI.Width = 0; // provisional value
        UGI.Height = 0; // provisional value
        UGI.UserGraphic = NULL; // provisional value
        UserGraphicVector.push_back(UGI);
    }
// now load the map & set Width, Height & TPicture*
    bool FileError = false;

    for(int x = 0; x < NumberOfGraphics; x++)
    {
        if(FileError)
        {
            break; // otherwise keeps going round the loop
        }
        UGI = UserGraphicVectorAt(0, x);
        if(UserGraphicMap.empty()) // will be when x == 0 but not after
        {
            try
            {
// TUserGraphicMapEntry UGME;   //can't define it here, it has to be defined before it is used - now defined in TrackUnit.h
                UGME.first = UGI.FileName;
                UGME.second = new TPicture;
                UGME.second->LoadFromFile(UGME.first); // errors caught below
                if(!Track->UserGraphicMap.insert(UGME).second) // if no failure then the new entry is inserted
                {
                    throw Exception("Map Insertion Error 2 - UserGraphicMap insertion failure for " + UGI.FileName);
                }
                UGI.UserGraphic = UGME.second;
                UGI.Width = UGI.UserGraphic->Width;
                UGI.Height = UGI.UserGraphic->Height;
                UserGraphicVectorAt(1, x) = UGI;
            }
            catch(const EInvalidGraphic &e) //non error catch - CallLogPop called at end of function
            {
                //message already sent in CheckUserGraphics
                FileError = true;
                UserGraphicVector.clear(); // so doesn't try to plot non-existent graphics
                if(!UserGraphicMap.empty())
                {
                    for(TTrack::TUserGraphicMap::iterator UGMIt = UserGraphicMap.begin(); UGMIt != UserGraphicMap.end(); UGMIt++)
                    {
                        delete UGMIt->second;
                    }
                    UserGraphicMap.clear();
                }
            }
            catch(const Exception &e) //non error catch - CallLogPop called at end of function
            {
                //message already sent in CheckUserGraphics
                FileError = true;
                UserGraphicVector.clear(); // so doesn't try to plot non-existent graphics
                if(!UserGraphicMap.empty())
                {
                    for(TTrack::TUserGraphicMap::iterator UGMIt = UserGraphicMap.begin(); UGMIt != UserGraphicMap.end(); UGMIt++)
                    {
                        delete UGMIt->second;
                    }
                    UserGraphicMap.clear();
                }
            }
        }
        else
        {
            bool FoundInMap = false;
            for(TUserGraphicMap::iterator UGMIt = UserGraphicMap.begin(); UGMIt != UserGraphicMap.end(); UGMIt++)
            {
                if(UGI.FileName == UGMIt->first) // already exists in map
                {
                    UGI.UserGraphic = UGMIt->second;
                    UGI.Width = UGI.UserGraphic->Width;
                    UGI.Height = UGI.UserGraphic->Height;
                    UserGraphicVectorAt(2, x) = UGI;
                    FoundInMap = true;
                    break;
                }
            }
            if(!FoundInMap)
            {
                try
                {
                    TUserGraphicMapEntry UGME;
                    UGME.first = UGI.FileName;
                    UGME.second = new TPicture;
                    UGME.second->LoadFromFile(UGME.first); // errors caught below
                    if(!Track->UserGraphicMap.insert(UGME).second) // if no failure then the new entry is inserted
                    {
                        throw Exception("Map Insertion Error 3 - UserGraphicMap insertion failure for " + UGI.FileName);
                    }
                    UGI.UserGraphic = UGME.second;
                    UGI.Width = UGI.UserGraphic->Width;
                    UGI.Height = UGI.UserGraphic->Height;
                    UserGraphicVectorAt(3, x) = UGI;
                }
                catch(const EInvalidGraphic &e) //non error catch - CallLogPop called at end of function
                {
                    //message already sent in CheckUserGraphics
                    FileError = true;
                    UserGraphicVector.clear(); // so doesn't try to plot non-existent graphics
                    if(!UserGraphicMap.empty())
                    {
                        for(TTrack::TUserGraphicMap::iterator UGMIt = UserGraphicMap.begin(); UGMIt != UserGraphicMap.end(); UGMIt++)
                        {
                            delete UGMIt->second;
                        }
                        UserGraphicMap.clear();
                    }
                }
                catch(const Exception &e) //non error catch - CallLogPop called at end of function
                {
                    //message already sent in CheckUserGraphics
                    FileError = true;
                    UserGraphicVector.clear(); // so doesn't try to plot non-existent graphics
                    if(!UserGraphicMap.empty())
                    {
                        for(TTrack::TUserGraphicMap::iterator UGMIt = UserGraphicMap.begin(); UGMIt != UserGraphicMap.end(); UGMIt++)
                        {
                            delete UGMIt->second;
                        }
                        UserGraphicMap.clear();
                    }
                }
            }
        }
    }
    Utilities->CallLogPop(2167);
}

// ---------------------------------------------------------------------------

void TTrack::SaveTrack(int Caller, std::ofstream& VecFile, bool GraphicsFollow)
{
// VecFile already open and its pointer at right place on calling
// if GraphicsFollow true, then save Marker as **Active elements**1
// save trackfinished flag
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveTrack, " + AnsiString(int(GraphicsFollow)));
    TTrackElement TrackElement, InactiveTrackElement;

// save track elements
    Utilities->SaveFileInt(VecFile, TrackVector.size());
    if(GraphicsFollow)
    {
        VecFile << "**Active elements**1" << '\0' << '\n';
    }
    else
    {
        VecFile << "**Active elements**" << '\0' << '\n';
    }
    for(unsigned int x = 0; x < (TrackVector.size()); x++)
    {
        TrackElement = TrackElementAt(1092, x);
        VecFile << x << '\n'; // this is the TrackVectorNumber - extra, so easier to identify in the file
        VecFile << TrackElement.SpeedTag << '\n';
        VecFile << TrackElement.HLoc << '\n';
        VecFile << TrackElement.VLoc << '\n';
        if(TrackElement.TrackType == GapJump)
        {
            VecFile << TrackElement.ConnLinkPos[0] << '\n';
            VecFile << TrackElement.Conn[0] << '\n';
        }
        if((TrackElement.TrackType == SignalPost) || (TrackElement.TrackType == Points))
        {
            VecFile << TrackElement.Attribute << '\n';
        }
        if(TrackElement.TrackType == SignalPost)
        {
            if(TrackElement.CallingOnSet)
            {
                VecFile << int(1) << '\n';
            }
            else
            {
                VecFile << int(0) << '\n';
            }
        }
        VecFile << TrackElement.Length01 << '\n';
        VecFile << TrackElement.Length23 << '\n';
        VecFile << TrackElement.SpeedLimit01 << '\n';
        VecFile << TrackElement.SpeedLimit23 << '\n';
        VecFile << TrackElement.LocationName.c_str() << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
        VecFile << TrackElement.ActiveTrackElementName.c_str() << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
// new for v0.6
        if(TrackElement.TrackType == SignalPost)
        {
            if(TrackElement.SigAspect == TTrackElement::ThreeAspect)
            {
                VecFile << "3*****" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
            }
            else if(TrackElement.SigAspect == TTrackElement::TwoAspect)
            {
                VecFile << "2*****" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
            }
            else if(TrackElement.SigAspect == TTrackElement::GroundSignal)
            {
                VecFile << "G*****" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
            }
            else // 4 aspect
            {
                VecFile << "4*****" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
            }
        }
        else
        {
            VecFile << "******" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
        }
    }

    Utilities->SaveFileInt(VecFile, InactiveTrackVector.size());
    VecFile << "**Inactive elements**" << '\0' << '\n'; // extra
    for(unsigned int x = 0; x < (InactiveTrackVector.size()); x++)
    {
        InactiveTrackElement = InactiveTrackElementAt(136, x);
        VecFile << x << '\n'; // this is the Inactive TrackVectorNumber - extra
        VecFile << InactiveTrackElement.SpeedTag << '\n';
        VecFile << InactiveTrackElement.HLoc << '\n';
        VecFile << InactiveTrackElement.VLoc << '\n';
        VecFile << InactiveTrackElement.LocationName.c_str() << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
        VecFile << "******" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
    }
    Utilities->CallLogPop(449);
}

// ---------------------------------------------------------------------------

bool TTrack::CheckTrackElementsInFile(int Caller, int &NumberOfActiveElements, bool &GraphicsFollow, std::ifstream& VecFile)
{
// VecFile already open and its pointer at right place on calling
// check trackfinished flag
// inactive elements follow immediately after active elements, no need to check for a marker between them
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckTrackElementsInFile");
    int TempInt;

    GraphicsFollow = false;
    NumberOfActiveElements = Utilities->LoadFileInt(VecFile);
    if((NumberOfActiveElements < 0) || (NumberOfActiveElements > 1000000)) // No of active elements (up to 500 screens all completely full!)
    {
        Utilities->CallLogPop(1513);
        return(false);
    }
// if(!Utilities->CheckAndCompareFileString(VecFile, "**Active elements**")) dropped at v2.4.0 as could have a '1' at the end if there are graphics
    AnsiString MarkerString;

    if(!Utilities->CheckAndReadFileString(VecFile, MarkerString)) // new version for v2.4.0
    {
        Utilities->CallLogPop(1758);
        return(false);
    }
    if(MarkerString[MarkerString.Length()] == '1')
    {
        GraphicsFollow = true;
    }
    for(int x = 0; x < NumberOfActiveElements; x++)
    {
        if(!Utilities->CheckFileInt(VecFile, x, x)) // TrackVectorNumber, must be 'x'
        {
            Utilities->CallLogPop(1759);
            return(false);
        }
        VecFile >> TempInt;
        int SpeedTag = TempInt;
        if((TempInt < 0) || (TempInt >= FirstUnusedSpeedTagNumber) || (TempInt == 17)) // Speedtag
        {
            Utilities->CallLogPop(1514);
            return(false);
        }
        VecFile >> TempInt;
        if(((TempInt > 999999) || (TempInt < -1000001)) && (TempInt != -2000000000)) // HLoc
        {
            Utilities->CallLogPop(1495);
            return(false);
        }
        VecFile >> TempInt;
        if(((TempInt > 999999) || (TempInt < -1000001)) && (TempInt != -2000000000)) // VLoc
        {
            Utilities->CallLogPop(1497);
            return(false);
        }
        if((SpeedTag > 87) && (SpeedTag < 96)) // GapJumps 88-95 incl
        {
            VecFile >> TempInt;
            if((TempInt < -1) || (TempInt > 3)) // ConnLinkPos[0]
            {
                Utilities->CallLogPop(1499);
                return(false);
            }
            VecFile >> TempInt;
            if((TempInt < -1) || (TempInt > 999999)) // Conn[0]
            {
                Utilities->CallLogPop(1500);
                return(false);
            }
        }
        if(((SpeedTag >= 7) && (SpeedTag <= 14)) || ((SpeedTag >= 28) && (SpeedTag <= 43)) || ((SpeedTag >= 132) && (SpeedTag <= 139)) ||
           ((SpeedTag >= 68) && (SpeedTag <= 75)))
        {
            VecFile >> TempInt;
            if((TempInt < -1) || (TempInt > 5)) // Points & signal attribute
            {
                Utilities->CallLogPop(1502);
                return(false);
            }
        }
        if((SpeedTag >= 68) && (SpeedTag <= 75)) // signals
        {
            VecFile >> TempInt;
            if((TempInt != 0) && (TempInt != 1)) // CallingOnSet
            {
                Utilities->CallLogPop(1155);
                return(false);
            }
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 999999)) // Length01
        {
            Utilities->CallLogPop(1503);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 999999)) // Length23
        {
            Utilities->CallLogPop(1504);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 999999)) // SpeedLimit01
        {
            Utilities->CallLogPop(1505);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 999999)) // SpeedLimit23
        {
            Utilities->CallLogPop(1506);
            return(false);
        }
        if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
        {
            Utilities->CallLogPop(1142);
            return(false); // LocationName
        }
        if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
        {
            Utilities->CallLogPop(1143);
            return(false); // ActiveTrackElementName
        }
        if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
        {
            Utilities->CallLogPop(1787);
            return(false); // marker
        }
    }
    int NumberOfInactiveElements = 0;

    NumberOfInactiveElements = Utilities->LoadFileInt(VecFile);
    if(NumberOfInactiveElements < 0) // No of active elements
    {
        Utilities->CallLogPop(1493);
        return(false);
    }
    if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
    {
        Utilities->CallLogPop(1764);
        return(false); // **Inactive elements** marker
    }
    for(int x = 0; x < NumberOfInactiveElements; x++)
    {
        if(!Utilities->CheckFileInt(VecFile, x, x)) // TrackVectorNumber, must be 'x'
        {
            Utilities->CallLogPop(1765);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < 0) || (TempInt >= FirstUnusedSpeedTagNumber) || (TempInt == 17)) // Speedtag
        {
            Utilities->CallLogPop(1494);
            return(false);
        }
        VecFile >> TempInt;
        if(((TempInt > 999999) || (TempInt < -1000001)) && (TempInt != -2000000000)) // HLoc
        {
            Utilities->CallLogPop(1496);
            return(false);
        }
        VecFile >> TempInt;
        if(((TempInt > 999999) || (TempInt < -1000001)) && (TempInt != -2000000000)) // VLoc
        {
            Utilities->CallLogPop(1498);
            return(false);
        }
        if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
        {
            Utilities->CallLogPop(1144);
            return(false); // LocationName
        }
        if(!(Utilities->CheckFileStringZeroDelimiter(VecFile)))
        {
            Utilities->CallLogPop(1788);
            return(false); // marker
        }
    }
    Utilities->CallLogPop(1507);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::CheckUserGraphics(int Caller, std::ifstream &VecFile, UnicodeString GraphicsPath)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckUserGraphics");
    int NumberOfGraphics = Utilities->LoadFileInt(VecFile);

    if((NumberOfGraphics < 0) || (NumberOfGraphics > 100000)) // 100,000 should be plenty!
    {
        Utilities->CallLogPop(2168);
        return(false);
    }
    // filename in Graphics folder, then HPos, then VPos
    AnsiString FileName = "", TempStr = "";

    for(int x = 0; x < NumberOfGraphics; x++)
    {
        TPicture *TempPicture = new TPicture;
        try
        {
            if(!Utilities->CheckAndReadFileString(VecFile, FileName))
            {
                Utilities->CallLogPop(2169);
                delete TempPicture;
                return(false);
            }
            TempPicture->LoadFromFile(GraphicsPath + "\\" + FileName); // only loaded to check and catch errors
            delete TempPicture;
            if(!Utilities->CheckFileInt(VecFile, -2000000, 2000000)) // HPos, allow plenty of scope
            {
                Utilities->CallLogPop(2170);
                return(false);
            }
            if(!Utilities->CheckFileInt(VecFile, -2000000, 2000000)) // VPos
            {
                Utilities->CallLogPop(2171);
                return(false);
            }
        }
        catch(const EInvalidGraphic &e)  //non error catch
        {
            //move file pointer to end of graphic section for later checks in session files
            Utilities->CheckAndReadFileString(VecFile, TempStr); //get rid of HPos
            Utilities->CheckAndReadFileString(VecFile, TempStr); //VPos
            for(int y = x + 1; y < NumberOfGraphics; y++)
            {
                Utilities->CheckAndReadFileString(VecFile, TempStr); //next FileName
                Utilities->CheckAndReadFileString(VecFile, TempStr); //next VPos
                Utilities->CheckAndReadFileString(VecFile, TempStr); //next VPos
            }
            ShowMessage(FileName +
                        " has an incorrect file format, user graphics can't be loaded. Ensure that all user graphic files are valid with extension .bmp, .gif, .jpg, or .png");
            Utilities->CallLogPop(2172);
            delete TempPicture;
            return(true);      //for these file errors allow railway or session to be loaded, changed at v2.6.0
        }
        catch(const Exception &e) //non error catch
        {
            //move file pointer to end of graphic section for later checks in session files
            Utilities->CheckAndReadFileString(VecFile, TempStr); //get rid of HPos
            Utilities->CheckAndReadFileString(VecFile, TempStr); //VPos
            for(int y = x + 1; y < NumberOfGraphics; y++)
            {
                Utilities->CheckAndReadFileString(VecFile, TempStr); //next FileName
                Utilities->CheckAndReadFileString(VecFile, TempStr); //next VPos
                Utilities->CheckAndReadFileString(VecFile, TempStr); //next VPos
            }
            ShowMessage("Unable to load user graphic files, ensure that " + FileName +
                        " exists in the 'Graphics' folder and that it is has extension .bmp, .gif, .jpg, or .png.");
            Utilities->CallLogPop(2173);
            delete TempPicture;
            return(true);      //for these file errors allow railway or session to be loaded, changed at v2.6.0
        }
    }
    Utilities->CallLogPop(2174);
    return(true);
}

// ---------------------------------------------------------------------------

void TTrack::SaveSessionBarriersDownVector(int Caller, std::ofstream &OutFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveSessionBarriersDownVector");
    int VecSize = Track->BarriersDownVector.size();

    Utilities->SaveFileInt(OutFile, VecSize);
    for(int x = 0; x < VecSize; x++)
    {
        TActiveLevelCrossing TALC = Track->BarriersDownVector.at(x);
        Utilities->SaveFileInt(OutFile, TALC.TypeOfRoute);  //changed to int from bool in v2.6.0
        Utilities->SaveFileBool(OutFile, TALC.ReducedTimePenalty);
        Utilities->SaveFileInt(OutFile, (short)TALC.BarrierState);
        Utilities->SaveFileDouble(OutFile, TALC.ChangeDuration);
        Utilities->SaveFileInt(OutFile, TALC.BaseElementSpeedTag);
        Utilities->SaveFileInt(OutFile, TALC.HLoc);
        Utilities->SaveFileInt(OutFile, TALC.VLoc);
        Utilities->SaveFileDouble(OutFile, double(TALC.StartTime));
    }
    Utilities->CallLogPop(1963);
}

// ---------------------------------------------------------------------------

void TTrack::SaveChangingLCVector(int Caller, std::ofstream &OutFile) //used only in errorfile
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveChangingLCVector");
    int VecSize = Track->ChangingLCVector.size();

    Utilities->SaveFileInt(OutFile, VecSize);
    for(int x = 0; x < VecSize; x++)
    {
        TActiveLevelCrossing TALC = Track->ChangingLCVector.at(x);
        Utilities->SaveFileInt(OutFile, TALC.TypeOfRoute); //changed to int from bool in v2.6.0
        Utilities->SaveFileBool(OutFile, TALC.ReducedTimePenalty);
        Utilities->SaveFileInt(OutFile, (short)TALC.BarrierState);
        Utilities->SaveFileDouble(OutFile, TALC.ChangeDuration);
        Utilities->SaveFileInt(OutFile, TALC.BaseElementSpeedTag);
        Utilities->SaveFileInt(OutFile, TALC.HLoc);
        Utilities->SaveFileInt(OutFile, TALC.VLoc);
        Utilities->SaveFileDouble(OutFile, double(TALC.StartTime));
    }
    Utilities->CallLogPop(1980);
}

// ---------------------------------------------------------------------------

bool TTrack::CheckActiveLCVector(int Caller, std::ifstream &VecFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckActiveLCVector");
    int VecSize = Utilities->LoadFileInt(VecFile);

    for(int x = 0; x < VecSize; x++)
    {
        if(!Utilities->CheckFileInt(VecFile, 0, 2))   //changed from bool at v2.6.0 to allow TypeOfRoute == 2 for barriers manually lowered
        {
            Utilities->CallLogPop(1970);
            return(false);
        }
        if(!Utilities->CheckFileBool(VecFile))
        {
            Utilities->CallLogPop(1971);
            return(false);
        }
        if(!Utilities->CheckFileInt(VecFile, 0, 3))
        {
            Utilities->CallLogPop(1972);
            return(false);
        }
        if(!Utilities->CheckFileDouble(VecFile))
        {
            Utilities->CallLogPop(1973);
            return(false);
        }
        if(!Utilities->CheckFileInt(VecFile, 1, 2))
        {
            Utilities->CallLogPop(1974);
            return(false);
        }
        if(!Utilities->CheckFileInt(VecFile, -1000001, 999999))
        {
            Utilities->CallLogPop(1975);
            return(false);
        }
        if(!Utilities->CheckFileInt(VecFile, -1000001, 999999))
        {
            Utilities->CallLogPop(1976);
            return(false);
        }
        if(!Utilities->CheckFileDouble(VecFile))
        {
            Utilities->CallLogPop(1977);
            return(false);
        }
    }
    Utilities->CallLogPop(1978);
    return(true);
}

// ---------------------------------------------------------------------------

void TTrack::LoadBarriersDownVector(int Caller, std::ifstream &VecFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadBarriersDownVector");
    int VecSize = Utilities->LoadFileInt(VecFile);

    for(int x = 0; x < VecSize; x++)
    {
        TActiveLevelCrossing TALC;
        TALC.TypeOfRoute = Utilities->LoadFileInt(VecFile);  //changed to int from bool in v2.6.0
        TALC.ReducedTimePenalty = Utilities->LoadFileBool(VecFile);
        TALC.BarrierState = TBarrierState(Utilities->LoadFileInt(VecFile));
        TALC.ChangeDuration = Utilities->LoadFileDouble(VecFile);
        TALC.BaseElementSpeedTag = Utilities->LoadFileInt(VecFile);
        TALC.HLoc = Utilities->LoadFileInt(VecFile);
        TALC.VLoc = Utilities->LoadFileInt(VecFile);
        TALC.StartTime = TDateTime(Utilities->LoadFileDouble(VecFile));
        BarriersDownVector.push_back(TALC);
    }
    Utilities->CallLogPop(1979);
}

// ---------------------------------------------------------------------------

void TTrack::RebuildTrackAndText(int Caller, TDisplay *Disp, bool BothPointFilletsAndBasicLCs)
/*
      Note, have to plot inactives before track because track has to overwrite NamedNonStationLocations, but, plot basic LC's (if flag set) after track
      so they lie above the track.  Basic LCs are plotted for all but Level1Mode == OperMode (i.e. closed to trains), because the LC attributes will always be
      0 in such cases and because in OperMode the LCs have to be plotted again after the routes, which is done in Clearand....
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RebuildTrackAndText," + AnsiString((short)BothPointFilletsAndBasicLCs));
    TTrackElement Next;

// Disp->ClearDisplay();
    NextTrackElementPtr = InactiveTrackVector.begin();
    while(ReturnNextInactiveTrackElement(0, Next))
    {
        if(Next.TrackType != LevelCrossing) // don't plot level crossings as these need to be plotted after the track
        {
            if(Next.GraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
            {
                // only plot if on screen, to save time
                if(((Next.HLoc - Display->DisplayOffsetH) >= 0) && ((Next.HLoc - Display->DisplayOffsetH) < Utilities->ScreenElementWidth) &&
                   ((Next.VLoc - Display->DisplayOffsetV) >= 0) && ((Next.VLoc - Display->DisplayOffsetV) < Utilities->ScreenElementHeight))
                {
                    Next.PlotVariableTrackElement(2, Disp); // striped if not named
                }
            }
        }
    }

    TextHandler->RebuildFromTextVector(1, Disp); // plot text after inactives so can have text on stations etc

    NextTrackElementPtr = TrackVector.begin();
    while(ReturnNextTrackElement(0, Next))
    {
        if(Next.GraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
        {
            if(((Next.HLoc - Display->DisplayOffsetH) >= 0) && ((Next.HLoc - Display->DisplayOffsetH) < Utilities->ScreenElementWidth) &&
               ((Next.VLoc - Display->DisplayOffsetV) >= 0) && ((Next.VLoc - Display->DisplayOffsetV) < Utilities->ScreenElementHeight))
            {
                if(Next.TrackType == Points)
                {
                    PlotPoints(5, Next, Disp, BothPointFilletsAndBasicLCs);
                }
                else if(Next.TrackType == SignalPost)
                {
                    PlotSignal(9, Next, Disp);
                }
                else if(Next.TrackType == GapJump)
                {
                    PlotGap(0, Next, Disp);
                }
                else if(Next.TrackType == Continuation) //added for multiplayer graphic overlays
                {
                    PlotContinuation(0, Next, Disp);
                }
                else
                {
                    Next.PlotVariableTrackElement(3, Disp); // for footcrossings, may be striped or not
                }
            }
        }
    }

    if(BothPointFilletsAndBasicLCs)
    {
        NextTrackElementPtr = InactiveTrackVector.begin();
        while(ReturnNextInactiveTrackElement(4, Next))
        {
            if(Next.TrackType == LevelCrossing) // plot level crossings (if required) after the track
            {
                if(Next.GraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
                {
                    // only plot if on screen, to save time, & OK as plotting one by one here
                    if(((Next.HLoc - Display->DisplayOffsetH) >= 0) && ((Next.HLoc - Display->DisplayOffsetH) < Utilities->ScreenElementWidth) &&
                       ((Next.VLoc - Display->DisplayOffsetV) >= 0) && ((Next.VLoc - Display->DisplayOffsetV) < Utilities->ScreenElementHeight))
                    {
                        if(GetTrackElementFromTrackMap(1, Next.HLoc, Next.VLoc).SpeedTag == 1)
                        {
                            Disp->PlotOutput(193, (Next.HLoc * 16), (Next.VLoc * 16), RailGraphics->LCBothVer);
                        }
                        else
                        {
                            Disp->PlotOutput(194, (Next.HLoc * 16), (Next.VLoc * 16), RailGraphics->LCBothHor);
                        }
                    }
                }
            }
        }
    }
    Disp->Update();
    Utilities->CallLogPop(468);
}

// ---------------------------------------------------------------------------

void TTrack::RebuildUserGraphics(int Caller, TDisplay *Disp) // new at v2.4.0
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RebuildUserGraphics,");
    if(UserGraphicVector.empty())
    {
        Utilities->CallLogPop(2175);
        return;
    }
    TUserGraphicItem UGI;

	for(auto UGI : UserGraphicVector)
	{
        if(((UGI.HPos + UGI.Width - (Display->DisplayOffsetH * 16)) >= 0) && ((UGI.HPos - (Display->DisplayOffsetH * 16)) <
																			  (Utilities->ScreenElementWidth * 16)) && ((UGI.VPos + UGI.Height - (Display->DisplayOffsetV * 16)) >= 0) &&
		   ((UGI.VPos - (Display->DisplayOffsetV * 16)) < (Utilities->ScreenElementHeight * 16)))
		{
			Disp->PlotAndAddUserGraphic(0, UGI);
		}
	}

    Disp->Update();
    Utilities->CallLogPop(2176);
}

// ---------------------------------------------------------------------------

void TTrack::WriteTrackAndTextToImage(int Caller, Graphics::TBitmap *Bitmap) //added text after inactives at v2.10.0
/*
      Note, have to plot inactives before track because track has to overwrite 'name' platforms (NamedLocationElements)
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WriteTrackAndTextToImage");
// need to change graphics back to black on white if have a dark background
    TColor OldTransparentColour = Utilities->clTransparent;

    if(Utilities->clTransparent != clB5G5R5)
    {
        Utilities->clTransparent = TColor(0xFFFFFF); // white
        RailGraphics->ChangeAllTransparentColours(Utilities->clTransparent, OldTransparentColour);
        RailGraphics->SetUpAllDerivitiveGraphics(Utilities->clTransparent);
    }
    TTrackElement Next;

    Bitmap->Canvas->CopyMode = cmSrcCopy;
    NextTrackElementPtr = InactiveTrackVector.begin();
    Graphics::TBitmap *GraphicOutput;

    while(ReturnNextInactiveTrackElement(2, Next))
    {
        GraphicOutput = Next.GraphicPtr;
        if(Next.GraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
        {
            if(Next.LocationName == "") // plot as named or unnamed (striped)
            {
                // default is not striped
                switch(Next.SpeedTag)
                {
                case 76: // t platform
                    GraphicOutput = RailGraphics->gl76Striped;
                    break;

                case 77: // h platform
                    GraphicOutput = RailGraphics->bm77Striped;
                    break;

                case 78: // v platform
                    GraphicOutput = RailGraphics->bm78Striped;
                    break;

                case 79: // r platform
                    GraphicOutput = RailGraphics->gl79Striped;
                    break;

                case 96: // concourse
                    GraphicOutput = RailGraphics->ConcourseStriped;
                    break;

                case 129: // v footbridge
                    GraphicOutput = RailGraphics->gl129Striped;
                    break;

                case 130: // h footbridge
                    GraphicOutput = RailGraphics->gl130Striped;
                    break;

                case 131: // non-station named loc
                    GraphicOutput = RailGraphics->bmNameStriped;
                    break;

                case 145: // v underpass
                    GraphicOutput = RailGraphics->gl145Striped;
                    break;

                case 146: // h underpass
                    GraphicOutput = RailGraphics->gl146Striped;
                    break;

                default:
                    GraphicOutput = Next.GraphicPtr;
                    break;
                }
            }
            if(Next.SpeedTag == 144) // level crossing
            {
                if(GetTrackElementFromTrackMap(2, Next.HLoc, Next.VLoc).SpeedTag == 1)
                {
                    GraphicOutput = RailGraphics->LCBothVer;
                }
                else
                {
                    GraphicOutput = RailGraphics->LCBothHor;
                }
            }
            Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), GraphicOutput);
        }
    }

    TextHandler->WriteTextToImage(0, Bitmap); //so overwrites inactive elements  //added at v2.10.0


    NextTrackElementPtr = TrackVector.begin();
    while(ReturnNextTrackElement(2, Next))
    {
        if(Next.GraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
        {
            if(Next.TrackType == Points) // plot both fillets
            {
                Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), Next.GraphicPtr);
                if(Next.SpeedTag < 28)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 7][0]);
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 7][1]);
                }
                else if(Next.SpeedTag < 132)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 20][0]);
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 20][1]);
                }
                else
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 108][0]);
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 108][1]);
                }
            }
            else if(Next.TrackType == GapJump) // plot as connected or unconnected
            {
                if((Next.SpeedTag == 88) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl88set);
                }
                else if((Next.SpeedTag == 88) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl88unset);
                }
                if((Next.SpeedTag == 89) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl89set);
                }
                else if((Next.SpeedTag == 89) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl89unset);
                }
                if((Next.SpeedTag == 90) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl90set);
                }
                else if((Next.SpeedTag == 90) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl90unset);
                }
                if((Next.SpeedTag == 91) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl91set);
                }
                else if((Next.SpeedTag == 91) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl91unset);
                }
                if((Next.SpeedTag == 92) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl92set);
                }
                else if((Next.SpeedTag == 92) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl92unset);
                }
                if((Next.SpeedTag == 93) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->bm93set);
                }
                else if((Next.SpeedTag == 93) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->bm93unset);
                }
                if((Next.SpeedTag == 94) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->bm94set);
                }
                else if((Next.SpeedTag == 94) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->bm94unset);
                }
                if((Next.SpeedTag == 95) && (Next.Conn[0] > -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl95set);
                }
                else if((Next.SpeedTag == 95) && (Next.Conn[0] == -1))
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl95unset);
                }
            }
            // below added for version 0.6, only stop signals to be drawn
            else if(Next.TrackType == SignalPost)
            {
                for(int x = 0; x < 40; x++)
                {
                    if((SigTable[x].SpeedTag == Next.SpeedTag) && (SigTable[x].Attribute == 0)) // need to plot as red regardless of actual attribute value
                    {
                        // plot blank first, then plot platform if present - striped or not depending on LocationName being set
                        // note these blanks plotted on lh signal side, even for rh signals, but works ok because the platform is replotted
                        // in PlatformOnSignalSide, which return true for platform NOT on signal side for rh sigs
                        int HOffset = 0;
                        if(Next.SpeedTag > 73)
                        {
                            HOffset = 5;
                        }
                        else if(Next.SpeedTag == 71)
                        {
                            HOffset = 9;
                        }
                        int VOffset = 0;
                        if(Next.SpeedTag == 69)
                        {
                            VOffset = 9;
                        }
                        else if(Next.SpeedTag == 72)
                        {
                            VOffset = 5;
                        }
                        else if(Next.SpeedTag == 74)
                        {
                            VOffset = 5;
                        }
                        Graphics::TBitmap *GraphicPtr;
                        if(Next.SpeedTag > 71)
                        {
                            GraphicPtr = RailGraphics->bmDiagonalSignalBlank;
                        }
                        else if(Next.SpeedTag < 70)
                        {
                            GraphicPtr = RailGraphics->bmStraightNSSignalBlank;
                        }
                        else
                        {
                            GraphicPtr = RailGraphics->bmStraightEWSignalBlank;
                        }
                        Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16) + HOffset, ((Next.VLoc - GetVLocMin()) * 16) + VOffset, GraphicPtr);
                        // plot special signal platform if present
                        Graphics::TBitmap* SignalPlatformGraphic;
                        if(PlatformOnSignalSide(2, Next.HLoc, Next.VLoc, Next.SpeedTag, SignalPlatformGraphic)) //
                        {
                            Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SignalPlatformGraphic);
                        }
                        // now plot signal (double yellow overwrites most of signal platform if present)
                        // below amended for version 0.6
                        if(Next.SigAspect == TTrackElement::ThreeAspect)
                        {
                            Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTableThreeAspect[x].SigPtr);
                        }
                        else if(Next.SigAspect == TTrackElement::TwoAspect)
                        {
                            Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTableTwoAspect[x].SigPtr);
                        }
                        else if(Next.SigAspect == TTrackElement::GroundSignal)
                        {
                            Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTableGroundSignal[x].SigPtr);
                        }
                        else // 4 aspect
                        {
                            Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTable[x].SigPtr);
                        }
                        break;
                    }
                }
            }
            else
            {
                Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), Next.GraphicPtr);
            }
        }
    }
    if(OldTransparentColour != clB5G5R5)
    {
        Utilities->clTransparent = OldTransparentColour; // restore
        RailGraphics->ChangeAllTransparentColours(Utilities->clTransparent, clB5G5R5);
        RailGraphics->SetUpAllDerivitiveGraphics(Utilities->clTransparent);
    }
    Utilities->CallLogPop(1533);
}

// ---------------------------------------------------------------------------

void TTrack::WriteGraphicsToImage(int Caller, Graphics::TBitmap *Bitmap)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WriteGraphicsToImage");
    if(UserGraphicVector.empty())
    {
        Utilities->CallLogPop(2192);
        return;
    }
    else
    {
        for(auto user_svg : UserGraphicVector)
        {
            Bitmap->Canvas->CopyMode = cmSrcCopy;
			Bitmap->Canvas->Draw(user_svg.HPos - (GetHLocMin() * 16), user_svg.VPos - (GetVLocMin() * 16),
                                 user_svg.UserGraphic->Graphic);
        }
    }
    Utilities->CallLogPop(2193);
}

// ---------------------------------------------------------------------------

void TTrack::WriteOperatingTrackAndTextToImage(int Caller, Graphics::TBitmap *Bitmap)
/*
      Note, have to plot inactives before track because track has to overwrite 'name' platforms (NamedLocationElements)
      Here plot all named elements as non-striped, points with active fillet, signals as they are set, and gaps as connected
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WriteOperatingTrackAndTextToImage");
// need to change graphics back to black on white if have a dark background
    TColor OldTransparentColour = Utilities->clTransparent;

    if(Utilities->clTransparent != clB5G5R5)
    {
        Utilities->clTransparent = TColor(0xFFFFFF); // white
        RailGraphics->ChangeAllTransparentColours(Utilities->clTransparent, OldTransparentColour);
        RailGraphics->SetUpAllDerivitiveGraphics(Utilities->clTransparent);
    }
    TTrackElement Next;

    Bitmap->Canvas->CopyMode = cmSrcCopy;
    NextTrackElementPtr = InactiveTrackVector.begin();
    Graphics::TBitmap *GraphicOutput;

    while(ReturnNextInactiveTrackElement(3, Next))
    {
        GraphicOutput = Next.GraphicPtr; // no striped name graphics
        if(Next.GraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
        {
            if(Next.SpeedTag == 144) // level crossing
            {
                int BaseElement = GetTrackElementFromTrackMap(3, Next.HLoc, Next.VLoc).SpeedTag;
                if(BaseElement == 1) // hor element
                {
                    if(Next.Attribute == 1) // open to trains
                    {
                        GraphicOutput = RailGraphics->LCBothHor;
                    }
                    else // plot as closed to trains if in any other state
                    {
                        GraphicOutput = RailGraphics->LCBothVer;
                    }
                }
                else // vert element
                {
                    if(Next.Attribute == 1) // open to trains
                    {
                        GraphicOutput = RailGraphics->LCBothVer;
                    }
                    else // plot as closed to trains if in any other state
                    {
                        GraphicOutput = RailGraphics->LCBothHor;
                    }
                }
            }
            Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), GraphicOutput);
        }
    }

    TextHandler->WriteTextToImage(1, Bitmap); //added at v2.10.0

    NextTrackElementPtr = TrackVector.begin();
    while(ReturnNextTrackElement(3, Next))
    {
        if(Next.GraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
        {
            if(Next.TrackType == Points) // plot active fillet
            {
                Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), Next.GraphicPtr);
                if(Next.SpeedTag < 28)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 7][Next.Attribute]);
                }
                else if(Next.SpeedTag < 132)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 20][Next.Attribute]);
                }
                else
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->PointModeGraphicsPtr[Next.SpeedTag - 108][Next.Attribute]);
                }
                if(Next.Failed) //added at v2.13.0
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->BlackOctagon);
                }

            }
            else if(Next.TrackType == GapJump) // plot as connected
            {
                if(Next.SpeedTag == 88)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl88set);
                }
                else if(Next.SpeedTag == 89)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl89set);
                }
                else if(Next.SpeedTag == 90)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl90set);
                }
                else if(Next.SpeedTag == 91)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl91set);
                }
                else if(Next.SpeedTag == 92)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl92set);
                }
                else if(Next.SpeedTag == 93)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->bm93set);
                }
                else if(Next.SpeedTag == 94)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->bm94set);
                }
                else if(Next.SpeedTag == 95)
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), RailGraphics->gl95set);
                }
            }
            else if(Next.TrackType == SignalPost) //plot in correct colour
            {
                for(int x = 0; x < 40; x++)
                {
                    if((SigTable[x].SpeedTag == Next.SpeedTag) && (SigTable[x].Attribute == Next.Attribute))
                    {
                        //plot blank first, then plot platform if present - (always not striped for operating railway)
                        //note these blanks plotted on lh signal side, even for rh signals, but works ok because the platform is replotted
                        //in PlatformOnSignalSide, which return true for platform NOT on signal side for rh sigs
                        int HOffset = 0;
                        if(Next.SpeedTag > 73)
                        {
                            HOffset = 5;
                        }
                        else if(Next.SpeedTag == 71)
                        {
                            HOffset = 9;
                        }
                        int VOffset = 0;
                        if(Next.SpeedTag == 69)
                        {
                            VOffset = 9;
                        }
                        else if(Next.SpeedTag == 72)
                        {
                            VOffset = 5;
                        }
                        else if(Next.SpeedTag == 74)
                        {
                            VOffset = 5;
                        }
                        Graphics::TBitmap *GraphicPtr;
                        if(Next.SpeedTag > 71)
                        {
                            GraphicPtr = RailGraphics->bmDiagonalSignalBlank;
                        }
                        else if(Next.SpeedTag < 70)
                        {
                            GraphicPtr = RailGraphics->bmStraightNSSignalBlank;
                        }
                        else
                        {
                            GraphicPtr = RailGraphics->bmStraightEWSignalBlank;
                        }
                        Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16) + HOffset, ((Next.VLoc - GetVLocMin()) * 16) + VOffset, GraphicPtr);
                        // plot special signal platform if present
                        Graphics::TBitmap* SignalPlatformGraphic;
                        if(PlatformOnSignalSide(1, Next.HLoc, Next.VLoc, Next.SpeedTag, SignalPlatformGraphic))
                        {
                            Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SignalPlatformGraphic);
                        }
                        if(!Next.Failed)
                        {
                            // now plot signal (double yellow overwrites most of signal platform if present)
                            // below amended for version 0.6
                            if(Next.SigAspect == TTrackElement::ThreeAspect)
                            {
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTableThreeAspect[x].SigPtr);
                            }
                            else if(Next.SigAspect == TTrackElement::TwoAspect)
                            {
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTableTwoAspect[x].SigPtr);
                            }
                            else if(Next.SigAspect == TTrackElement::GroundSignal)
                            {
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTableGroundSignal[x].SigPtr);
                            }
                            else // 4 aspect
                            {
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTable[x].SigPtr);
                            }
                            if((Next.CallingOnSet) && (Next.SigAspect != TTrackElement::GroundSignal))
                            // normal signal calling on, need to add extra graphic, basic red signal plotted above from SigTable
                            {
                                if(Next.SpeedTag == 68)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm68CallingOn);
                                }
                                if(Next.SpeedTag == 69)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm69CallingOn);
                                }
                                if(Next.SpeedTag == 70)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm70CallingOn);
                                }
                                if(Next.SpeedTag == 71)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm71CallingOn);
                                }
                                if(Next.SpeedTag == 72)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm72CallingOn);
                                }
                                if(Next.SpeedTag == 73)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm73CallingOn);
                                }
                                if(Next.SpeedTag == 74)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm74CallingOn);
                                }
                                if(Next.SpeedTag == 75)
                                {
                                    Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->bm75CallingOn);
                                }
                            }
                            else if((Next.CallingOnSet) && (Next.SigAspect == TTrackElement::GroundSignal)) // ground signal calling on, use normal proceed aspect
                            {
                                for(int x = 0; x < 40; x++)
                                {
                                    if((SigTableGroundSignal[x].SpeedTag == Next.SpeedTag) && (SigTable[x].Attribute == 1)) // use attr 1 for proceed
                                    {
                                        // plot blank first, then plot platform if present - striped or not depending on LocationName being set
                                        Display->PlotSignalBlankOnBitmap(Next.HLoc - GetHLocMin(), Next.VLoc - GetVLocMin(), Next.SpeedTag, Bitmap,
                                                                         Utilities->RHSignalFlag); // in case existing signal is a double yellow
                                        // plot special signal platform if present
                                        Graphics::TBitmap* SignalPlatformGraphic;
                                        if(PlatformOnSignalSide(4, Next.HLoc, Next.VLoc, Next.SpeedTag, SignalPlatformGraphic))
                                        {
                                            Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SignalPlatformGraphic);
                                        }
                                        // now plot signal
                                        Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, SigTableGroundSignal[x].SigPtr);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        else  //added at v2.13.0
                        {
                            if(Next.SigAspect != TTrackElement::GroundSignal)
                            {
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, FailedSigTable[x % 5].SigPtr);
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->BlackOctagon);
                            }
                            else
                            {
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, FailedGroundSigTable[x % 5].SigPtr);
                                Bitmap->Canvas->Draw((Next.HLoc - GetHLocMin()) * 16, (Next.VLoc - GetVLocMin()) * 16, RailGraphics->BlackOctagon);
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16), Next.GraphicPtr);
                if(Next.Failed) //added at v2.13.0
                {
                    Bitmap->Canvas->Draw(((Next.HLoc - GetHLocMin()) * 16), ((Next.VLoc - GetVLocMin()) * 16),
                                         RailGraphics->BlackOctagon);
                }
            }
        }
    }
    if(OldTransparentColour != clB5G5R5)
    {
        Utilities->clTransparent = OldTransparentColour; // restore
        RailGraphics->ChangeAllTransparentColours(Utilities->clTransparent, clB5G5R5);
        RailGraphics->SetUpAllDerivitiveGraphics(Utilities->clTransparent);
    }
    Utilities->CallLogPop(1701);
}

// ---------------------------------------------------------------------------

bool TTrack::FindAndHighlightAnUnsetGap(int Caller) // true if find one
{
	Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindAndHighlightAnUnsetGap");


	auto iterator_ = std::find_if(
		TrackVector.begin(),
		TrackVector.end(),
		[](auto x){return x.TrackType == GapJump && x.Conn[0] <= -1;}
	);

	if(iterator_ != TrackVector.end())
	{
		// here if identify a GapJump element not yet set
		GapPos = std::distance(TrackVector.begin(), iterator_);
		GapHLoc = (*iterator_).HLoc;
		GapVLoc = (*iterator_).VLoc;
		// highlight it
		ShowSelectedGap(2, Display);
		Utilities->CallLogPop(469);
		return(true);
    }
    Utilities->CallLogPop(470);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::FindSetAndDisplayMatchingGap(int Caller, int HLoc, int VLoc) // true if find one
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindSetAndDisplayMatchingGap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    int Position;
    TTrackElement TrackElement;

    if(!(FindNonPlatformMatch(11, HLoc, VLoc, Position, TrackElement)))
    {
        Utilities->CallLogPop(471);
        return(false); // not found
    }
    if(TrackElement.TrackType != GapJump)
    {
        Utilities->CallLogPop(472);
        return(false); // found something but not a gap
    }
    if(Position == GapPos)
    {
        Utilities->CallLogPop(473);
        return(false); // selected original gap
    }
    if(TrackElementAt(1097, Position).Conn[0] != -1)
    {
        Utilities->CallLogPop(474);
        return(false); // already selected
    }
    TrackElementAt(1098, Position).Conn[0] = GapPos; // set Conn[0] at Position to GapPos & ConnLinkPos[0] to 0
    TrackElementAt(1099, Position).ConnLinkPos[0] = 0;
    TrackElementAt(1100, GapPos).Conn[0] = Position; // set other one similarly
    TrackElementAt(1101, GapPos).ConnLinkPos[0] = 0;
// now highlight the selected location
    Display->Ellipse(0, HLoc * 16, VLoc * 16, clB0G5R0);
    Utilities->CallLogPop(475);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::GapsUnset(int Caller)
// returns true if there are gaps and any are unset
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GapsUnset");
    if(TrackVector.empty())
    {
        Utilities->CallLogPop(476);
        return(false);
    }
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(1102, x).TrackType == GapJump)
        {
            if(TrackElementAt(1103, x).Conn[0] == -1) // unset if -1 (Gap always at position 0)
            {
                Utilities->CallLogPop(477);
                return(true);
            }
            else // set, but may not have matching element, or that element may not be set
            {
                if(TrackElementAt(1106, TrackElementAt(1107, x).Conn[0]).TrackType != GapJump)
                // check that the element pointed to by the gap link is a GapJump
                {
                    ShowMessage("Error - gap connected to a non-gap.  Railway file is corrupt, further use may cause a system crash");
                    Utilities->CallLogPop(1137);
                    return(false);
                }
// here if gap connection is itself a GapJump
                if(TrackElementAt(1108, TrackElementAt(1109, x).Conn[0]).Conn[0] != (int)x)
                // check that the element pointed to by the gap link is a GapJump & that its gap link
                // points back to 'x'
                {
                    Utilities->CallLogPop(478);
                    return(true);
                }
// here if gap connection itself points back to 'x' so these two GapJumps match properly
            }
        } // if(TrackElementAt(, x).TrackType == GapJump)

    } // for x...
    Utilities->CallLogPop(479);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::NoGaps(int Caller) // returns true if there are no gaps
{
	Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NoGaps");

	auto iterator_ = std::find_if(
		TrackVector.begin(),
		TrackVector.end(),
        [](auto x){return x.TrackType == GapJump;}
	);

	if(iterator_ != TrackVector.end()) Utilities->CallLogPop(1105);
	else Utilities->CallLogPop(1106);

	return iterator_ == TrackVector.end();
}

// ---------------------------------------------------------------------------

bool TTrack::NoNamedLocationElements(int Caller) // returns true if there are no NamedLocationElements (includes footcrossings)
{
	Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NoLocations");

	auto iterator_ = std::find_if(
		InactiveTrackVector.begin(),
		InactiveTrackVector.end(),
		[](auto x){return x.FixedNamedLocationElement;}
	);

	if(iterator_ != InactiveTrackVector.end())
	{
	   Utilities->CallLogPop(1107);
	   return false;
	}

	iterator_ = std::find_if(
		TrackVector.begin(),
		TrackVector.end(),
		[](auto x){return x.FixedNamedLocationElement;}
	);

    if(iterator_ != TrackVector.end())
	{
	   Utilities->CallLogPop(1108);
	   return false;
	}

    Utilities->CallLogPop(1109);
    return true;
}

// ---------------------------------------------------------------------------

bool TTrack::LocationsNotNamed(int Caller)
// returns true if there are unnamed NamedLocationElements (includes footcrossings)
// returns false otherwise or if there are no NamedLocationElements
{
	Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LocationsNotNamed");

	auto iterator_ = std::find_if(
		InactiveTrackVector.begin(),
		InactiveTrackVector.end(),
		[](auto x){return x.FixedNamedLocationElement && x.LocationName == "";}
	);

	if(iterator_ != InactiveTrackVector.end())
	{
		Utilities->CallLogPop(1110);
		return true;
	}

	iterator_ = std::find_if(
		TrackVector.begin(),
		TrackVector.end(),
        [](auto x){return x.FixedNamedLocationElement && x.LocationName == "";}
	);

	if(iterator_ != InactiveTrackVector.end())
	{
		Utilities->CallLogPop(1110);
		return true;
	}

    Utilities->CallLogPop(1112);
	return false;
}

// ---------------------------------------------------------------------------

void TTrack::ShowSelectedGap(int Caller, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ShowSelectedGap,");
    Disp->Ellipse(1, GapHLoc * 16, GapVLoc * 16, clB0G0R5);
    Utilities->CallLogPop(480);
}

// ---------------------------------------------------------------------------

void TTrack::ResetAnyNonMatchingGaps(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetAnyNonMatchingGaps");
    if(TrackVector.size() == 0)
    {
        Utilities->CallLogPop(481);
        return;
    }
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(1114, x).TrackType == GapJump)
        {
            if(TrackElementAt(1115, x).Conn[0] > -1) // set
            {
                if(TrackElementAt(1116, TrackElementAt(1117, x).Conn[0]).TrackType != GapJump)
                // check that the element pointed to by the gap link is a GapJump & if not clear Conns & CLks
                {
                    TrackElementAt(1118, x).Conn[0] = -1;
                    TrackElementAt(1119, x).ConnLinkPos[0] = -1;
                    continue; // to next 'x'
                }
// here if gap connection is itself a GapJump
                if(TrackElementAt(1120, TrackElementAt(1349, x).Conn[0]).Conn[0] != (int)x)
                // check that the element pointed to by the gap link is a GapJump & that its gap link points back to 'x'
                // if not clear Conns & CLks
                {
                    TrackElementAt(1121, x).Conn[0] = -1;
                    TrackElementAt(1122, x).ConnLinkPos[0] = -1;
                    continue; // to next 'x'
                }
// here if gap connection itself points back to 'x' so these two GapJumps match properly
// hence no more action needed on these Conns & CLks
            }
        } // else //gap jump

    } // for x...
// throw Exception("Test Exception");//test
    Utilities->CallLogPop(482);
}

// ---------------------------------------------------------------------------

void TTrack::ResetSignals(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetSignals");
    for(auto track_element : TrackVector)
    {
		if(track_element.TrackType == SignalPost)
        {
			track_element.Attribute = 0;
            track_element.Failed = false;
        }
    }
    FailedSignalsVector.clear();
    Utilities->CallLogPop(483);
}

// ---------------------------------------------------------------------------

void TTrack::ResetPoints(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetPoints");
	for(auto track_element : TrackVector)
    {
		if(track_element.TrackType == Points)
        {
			track_element.Attribute = 0;
            track_element.Failed = false;
        }
    }
    FailedPointsVector.clear();
    Utilities->CallLogPop(484);
}

// ---------------------------------------------------------------------------

void TTrack::ResetTSRs(int Caller)  //added at v2.14.0
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetTSRs");
    for(auto track_element : TrackVector)
    {
		if(track_element.TrackType == Simple)
        {
            track_element.Failed = false;
        }
    }
    TSRVector.clear();
    Utilities->CallLogPop(2550);
}

// ---------------------------------------------------------------------------

bool TTrack::RepositionAndMapTrack(int Caller) // doesn't involve InactiveTrack
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RepositionAndMapTrack");
    if(TrackVector.empty())
    {
        TrackMap.clear();
        Utilities->CallLogPop(485);
        return(true);
    }
// build new vector from map (map already in ascending order of locations & no erase elements in map)
    THVPair TrackMapKeyPair;

    NewVector.clear();
    TTrackMapIterator TrackMapPtr;

	for(auto [key, value] : TrackMap)
	{
		NewVector.push_back(TrackElementAt(6, value));
	}

    if(NewVector.size() != TrackMap.size())
    {
        throw Exception("Error - Map & Vector different sizes");
	}

    unsigned int NonZeroCount = 0;

	NonZeroCount += std::count_if(
		TrackVector.begin(),
		TrackVector.end(),
		[](auto x){return x.TrackType != Erase;}
    );

    if(NewVector.size() != NonZeroCount)
    {
        throw Exception("Error - NewVector & NonZero TrackVector different sizes");
	}

    TrackVector = NewVector;
    TrackMap.clear(); // ready to rebuild map after repositioning of TrackVector elements
    TTrackMapEntry TrackMapEntry;

	for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        TrackMapKeyPair.first = TrackElementAt(1128, x).HLoc;
        TrackMapKeyPair.second = TrackElementAt(1129, x).VLoc;
        TrackMapEntry.first = TrackMapKeyPair;
        TrackMapEntry.second = x;
        if(!(TrackMap.insert(TrackMapEntry).second))
        {
            throw Exception("Error - map insertion failure, TrackVector in error");
        }
    }
// All track now relocated in TrackVector, reset all Conns & CLks
	for(auto track_element : TrackVector) // check all elements in turn
    {
        for(unsigned int y = 0; y < 4; y++)
        {
			track_element.Conn[y] = -1;
            track_element.ConnLinkPos[y] = -1;
        }
    }
    RebuildLocationNameMultiMap(1); // to ensure all position entries correct after track vector changes
    CheckMapAndTrack(4); // test
    CheckMapAndInactiveTrack(4); // test
    CheckLocationNameMultiMap(8); // test
    if(!ResetGapsFromGapMap(1))
    {
        Utilities->CallLogPop(489);
        return(false);
    }
    Utilities->CallLogPop(490);
    return(true);
}

// ---------------------------------------------------------------------------

void TTrack::BuildGapMapFromTrackVector(int Caller) // Map contains one entry for each pair of matched gaps
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",BuildGapMapFromTrackVector");
    GapMap.clear();
    THVPair GapMapKeyPair, GapMapValuePair;
    TGapMapEntry GapMapEntry;

	for(auto track_element : TrackVector)
    {
		if(track_element.TrackType == GapJump)
        {
			GapMapKeyPair.first = track_element.HLoc;
			GapMapKeyPair.second = track_element.VLoc;
            GapMapEntry.first = GapMapKeyPair;
            if(track_element.Conn[0] == -1)
            {
                throw Exception("Error - Gap connection == -1  Can't build GapMap");
            }
			GapMapValuePair.first = TrackElementAt(7, track_element.Conn[0]).HLoc;
            GapMapValuePair.second = TrackElementAt(8, track_element.Conn[0]).VLoc;
            GapMapEntry.second = GapMapValuePair;
            if(GapMap.find(GapMapValuePair) == GapMap.end()) // if ValuePair already included as a key then result won't be end()
            {
                GapMap.insert(GapMapEntry);
            }
        }
    }
    Utilities->CallLogPop(492);
}

// ---------------------------------------------------------------------------

bool TTrack::LinkTrack(int Caller, bool &LocError, int &HLoc, int &VLoc, bool FinalCall)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LinkTrack," + AnsiString((short)FinalCall));

//1st pass to check track element locations - split into 2 passes at v2.11.1 so positioning checked before linkages, requested by Dan(#4669) 18/12/21 via Discord
    LocError = false;
    bool TrackElementPositionsOK = true;

    for(auto track_element : TrackVector) // check all elements in turn
    {
		if(track_element.TrackType == Erase) //Erase isn't used any more as a track type
        {
            continue; // skip blank elements
        }
// check footcrossing linkages
		if(track_element.TrackType == FootCrossing)
        {
            if(!CheckFootCrossingLinks(1, track_element))
            {
                ShowMessage(
                    "Footbridge or underpass connection error.  Each end must connect to a platform, concourse "
                    "or other footbridge or underpass, and they can't connect to each other (i.e. a footbridge "
					"can't connect to an underpass or vice versa)");
				HLoc = track_element.HLoc;
                VLoc = track_element.VLoc;
                LocError = true;
                Utilities->CallLogPop(493);
                return(false);
            }
        }
        for(unsigned int y = 0; y < 4; y++) // check all links for each element
        {
			if(track_element.Link[y] <= 0)
            {
                continue; // no link
            }
			if((track_element.TrackType == Buffers) && track_element.Config[y] == End)
            {
                continue; // buffer
            }
            if(track_element.Config[y] == Gap)
            {
                continue; // gaps set later from GapMap
            }
            // get required H & V for track element joining link 'y'
			int NewHLoc = track_element.HLoc + LinkHVArray[track_element.Link[y]][0];
			int NewVLoc = track_element.VLoc + LinkHVArray[track_element.Link[y]][1];
            // find track element if present
            bool ConnectionFoundFlag;
            int VecPos = GetVectorPositionFromTrackMap(14, NewHLoc, NewVLoc, ConnectionFoundFlag);
			if((track_element.TrackType == Continuation) && (y == 0) && ConnectionFoundFlag)
            {
                ShowMessage("Can't have a track element adjacent to a continuation exit");
				HLoc = track_element.HLoc;
                VLoc = track_element.VLoc;
                LocError = true;
                if(FinalCall)
                {
                    throw Exception("Error in final track linkage - continuation adjacent to another element");
                }
                Utilities->CallLogPop(1539);
                return(false);
            }
            if((track_element.TrackType == Continuation) && track_element.Config[y] == End)
            {
                continue;
            }
            if(ConnectionFoundFlag)
            {
				track_element.Conn[y] = VecPos; //<-- this sets the Conn value
                // find connecting link in the newly found track element if there is one & make buffer & adjacent signals check
                if((track_element.Config[1 - y] == Signal) && IsLCAtHV(50, TrackElementAt(1158, VecPos).HLoc, TrackElementAt(1350, VecPos).VLoc))
                {
                    // new in v2.4.0 - Krizar (Kristian Zarebski) found this error
                    ShowMessage("Can't have an exit signal next to a level crossing - it can cause the train to foul the crossing in some circumstances");
                    // otherwise when single route element removed in front of train the LC will start to close and the train will crash
                    TrackElementPositionsOK = false;
                }
                else if(((track_element.TrackType == Points) || (track_element.TrackType == SignalPost) || (track_element.TrackType == Crossover))
                        && (TrackElementAt(1162, VecPos).TrackType == Buffers))
                {
                    ShowMessage("Can't have points, crossover or signal next to buffers - need room for a train without fouling");
                    // need room for a train (2 elements) without fouling points or signals
                    TrackElementPositionsOK = false;
                }
				else if(((track_element.TrackType == Points) || (track_element.TrackType == SignalPost) || (track_element.TrackType == Crossover) ||
						 (track_element.TrackType == Bridge)) && (TrackElementAt(1167, VecPos).TrackType == Continuation))
                {
                    ShowMessage("Can't have points, crossover, bridge or signal next to a continuation - it can cause route setting problems");
                    // route setting won't allow an end of route selection adjacent to an existing route, which would happen
                    // if continuation next to a signal; also none of these can be a named location, and a continuation can
                    // be named but needs the adjacent element named too
                    TrackElementPositionsOK = false;
                }
				else if((track_element.TrackType == SignalPost) && (TrackElementAt(1169, VecPos).TrackType == SignalPost) &&
                        (track_element.SpeedTag == TrackElementAt(1171, VecPos).SpeedTag))
                {
                    ShowMessage("Can't have two same-direction signals adjacent to each other as there is no room for a train between them");
                    // can't join a route to an existing route where the second signal is in an existing route and the first signal is
                    // selected - appears as trying to select a signal that is not the next in line from the starting signal
                    TrackElementPositionsOK = false;
                }
//removed at v2.15.0 as now have a warning for bridge either side of a signal
/*                else if((TrackElementAt(1172, x).Config[y] == Signal) && (TrackElementAt(1173, VecPos).TrackType == Bridge) && !OverrideAndHideSignalBridgeMessage)
                {
                    ShowMessage("Bridge next to a signal - routes can't be truncated to this or other such signals.\n\nThis restriction can be removed or reinstated by pressing\nCTRL ALT 5.  When removed this message will not be shown again.");
                    // can't join a route to an existing route where the second signal is in an existing route and the first signal is
                    // selected - appears as trying to select a signal that is not the next in line from the starting signal
                    TrackElementPositionsOK = false;
                }*/
				else if((track_element.TrackType == SignalPost) && (TrackElementAt(1565, VecPos).TrackType == Bridge) && !OverrideAndHideSignalBridgeMessage) //added at v2.15.0
                {
                    ShowMessage("Bridge next to a signal - routes can't be truncated to this or other such signals.\n\nThis restriction can be removed or reinstated by pressing\nCTRL ALT 5.  When removed this message will not be shown again.");
                    // can't join a route to an existing route where the second signal is in an existing route and the first signal is
                    // selected - appears as trying to select a signal that is not the next in line from the starting signal
                    TrackElementPositionsOK = false;
                }
                else if(IsLCAtHV(45, track_element.HLoc, track_element.VLoc) && IsLCAtHV(46, TrackElementAt(1176, VecPos).HLoc, TrackElementAt(1177, VecPos).VLoc))
                // true if a level crossing is present at both x and VecPos - can't have two adjacent level crossings on the same track
                {
                    ShowMessage("Can't have two level crossings adjacent to each other on the same track");
                    TrackElementPositionsOK = false;
                }
                // if failed then set the invert values for the offending element
                if(!TrackElementPositionsOK)
                {
					HLoc = track_element.HLoc;
                    VLoc = track_element.VLoc;
                    LocError = true;
                    if(FinalCall)
                    {
                        throw Exception("Error in track element positions in FinalCall");
                    }
                    Utilities->CallLogPop(494);
                    return(false);
                }
            }
            // no 'else' here, if there's no link then will be picked up in 2nd pass
        }
    } // for(unsigned int x=0;x<TrackVector.size();x++)


//2nd pass - looking for missing connections
    LocError = false;
    for(auto track_element : TrackVector) // check all elements in turn
    {
		if(track_element.TrackType == Erase) //Erase isn't used any more as a track type
        {
            continue; // skip blank elements
        }
        for(unsigned int y = 0; y < 4; y++) // check all links for each element
        {
			if(track_element.Link[y] <= 0)
            {
                continue; // no link
            }
            if((track_element.TrackType == Buffers) && (track_element.Config[y] == End))
            {
                continue; // buffer
            }
            if(track_element.Config[y] == Gap)
            {
                continue; // gaps set later from GapMap
            }
			if((track_element.TrackType == Continuation) && (track_element.Config[y] == End))
            {
                continue; //continuation
            }
            // get required H & V for track element joining link 'y'
			int NewHLoc = track_element.HLoc + LinkHVArray[track_element.Link[y]][0];
			int NewVLoc = track_element.VLoc + LinkHVArray[track_element.Link[y]][1];
            // find track element if present
            bool ConnectionFoundFlag;
            bool LinkMatchFound = false;
            int VecPos = GetVectorPositionFromTrackMap(66, NewHLoc, NewVLoc, ConnectionFoundFlag);
            // if there isn't a connection set the invert values for the offending element
            if(ConnectionFoundFlag) //set the ConnLinkPos values
            {
                for(unsigned int a = 0; a < 4; a++)
                {
                    if((TrackElementAt(1178, VecPos).Link[a] == (10 - track_element.Link[y])) && (TrackElementAt(1180, VecPos).Config[a] != End) &&
                       (TrackElementAt(1181, VecPos).Config[a] != Gap))
                    {
						track_element.ConnLinkPos[y] = a;
                        // note - this ensures that if the connecting element is a leading point
                        // then the ConnLinkPos value is 0 rather than 2, since 'a' starts at 0
                        // (Points have the same link value for both [0] and [2])
                        LinkMatchFound = true;
                        break; // stop after first find or will find later link for leading point
                    }
                }
                if(!LinkMatchFound)
                {
					HLoc = track_element.HLoc;
					VLoc = track_element.VLoc;
                    LocError = true;
                    if(FinalCall)
                    {
                        throw Exception("Error in final track linkage -  - no matching link found");
                    }
                    Utilities->CallLogPop(495);
                    return(false);
                }
            }
            else //error
            {
				HLoc = track_element.HLoc;
                VLoc = track_element.VLoc;
                LocError = true;
                if(FinalCall)
                {
                    throw Exception("Error in final track linkage - connection not found");
                }
                Utilities->CallLogPop(2443);
                return(false);
            }
        }
    }
//end of 2nd pass

    if(FinalCall)
    {
        SetStationEntryStopLinkPosses(1);
    }

// confirmatiory checks that all ok - or throw error
    bool ConnErrorFlag = false;

    for(auto track_element : TrackVector)
    {
        if((track_element.Link[0] > 0) && (track_element.Config[0] != End) && (track_element.Conn[0] == -1))
        {
            ConnErrorFlag = true;
        }
		if((track_element.Link[1] > 0) && (track_element.Config[1] != End) && (track_element.Conn[1] == -1))
        {
            ConnErrorFlag = true;
        }
		if((track_element.Link[2] > 0) && (track_element.Config[2] != End) && (track_element.Conn[2] == -1))
		{
            ConnErrorFlag = true;
        }
        if((track_element.Link[3] > 0) && (track_element.Config[3] != End) && (track_element.Conn[3] == -1))
        {
            ConnErrorFlag = true;
        }
        if(FinalCall) // StationStopLinks only set during FinalCall so only check at FinalCall
        {
			if(track_element.ActiveTrackElementName == "")
            {
                if((track_element.StationEntryStopLinkPos1 != -1) || (track_element.StationEntryStopLinkPos2 != -1))
				{
                    auto iterator_ = std::find(TrackVector.begin(), TrackVector.end(), track_element);
					const int position_ = std::distance(TrackVector.begin(), iterator_);
                    throw Exception("Error, StationEntryStopLinkPos not -1 for unnamed element at TrackVectorPosition = " + AnsiString(position_));
                }
            }
        }
    }
    if(ConnErrorFlag)
    {
        if(FinalCall)
        {
            throw Exception("ConnError in LinkTrack - Final");
        }
        else
        {
            throw Exception("ConnError in LinkTrack - Precheck");
        }
    }
    bool CLkErrorFlag = false;

	for(auto track_element : TrackVector)
    {
		if((track_element.Link[0] > 0) && (track_element.Config[0] != End) && (track_element.ConnLinkPos[0] == -1))
		{
            CLkErrorFlag = true;
        }
		if((track_element.Link[1] > 0) && (track_element.Config[1] != End) && (track_element.ConnLinkPos[1] == -1))
        {
            CLkErrorFlag = true;
        }
		if((track_element.Link[2] > 0) && (track_element.Config[2] != End) && (track_element.ConnLinkPos[2] == -1))
        {
            CLkErrorFlag = true;
        }
        if((track_element.Link[3] > 0) && (track_element.Config[3] != End) && (track_element.ConnLinkPos[3] == -1))
        {
            CLkErrorFlag = true;
        }
    }

    if(CLkErrorFlag)
    {
        if(FinalCall)
        {
            throw Exception("CLkError in LinkTrack - Final");
        }
        else
        {
            throw Exception("CLkError in LinkTrack - Precheck");
        }
    }

// set element lengths to min of 10m
	for(auto track_element : TrackVector)
    {
        if(track_element.TrackType == Erase)
        {
            continue; // skip blank elements
        }
		if((track_element.Length01 < 10) && (track_element.Length01 != -1))
        {
			track_element.Length01 = 10;
		}
		if((track_element.Length23 < 10) && (track_element.Length23 != -1))
        {
            track_element.Length23 = 10;
        }
    }

    if(FinalCall) // ONLY at FinalCall, no point calling twice
    {
        CalcHLocMinEtc(3);
    }

    Utilities->CallLogPop(497);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::LinkTrackNoMessages(int Caller, bool FinalCall)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LinkTrackNoMessages," + AnsiString((short)FinalCall));
    for(auto track_element : TrackVector) // check all elements in turn
    {
		if(track_element.TrackType == Erase)
        {
            continue; // skip blank elements

        }
// check footcrossing linkages
        if(track_element.TrackType == FootCrossing)
        {
            if(!CheckFootCrossingLinks(3, track_element))
            {
                Utilities->CallLogPop(1127);
                return(false);
            }
        }
        for(unsigned int y = 0; y < 4; y++) // check all links for each element
        {
            if(track_element.Link[y] <= 0)
            {
                continue; // no link
            }
            if((track_element.TrackType == Buffers) && (track_element.Config[y] == End))
            {
                continue; // buffer
            }
            if(track_element.Config[y] == Gap)
            {
                continue; // gaps set later from GapMap

            }
            // get required H & V for track element joining link 'y'
			int NewHLoc = track_element.HLoc + LinkHVArray[track_element.Link[y]][0];
            int NewVLoc = track_element.VLoc + LinkHVArray[track_element.Link[y]][1];
            // find track element if present
            bool ConnectionFoundFlag;
            int VecPos = GetVectorPositionFromTrackMap(38, NewHLoc, NewVLoc, ConnectionFoundFlag);
            if((track_element.TrackType == Continuation) && (y == 0) && ConnectionFoundFlag)
            {
                if(FinalCall)
                {
                    throw Exception("Error in final track linkage - continuation adjacent to another element");
                }
                Utilities->CallLogPop(1540);
                return(false);
            }
            if((track_element.TrackType == Continuation) && (track_element.Config[y] == End))
            {
                continue;
            }
            if(ConnectionFoundFlag)
            {
				track_element.Conn[y] = VecPos;
                bool LinkFoundFlag = false;
                // find connecting link in the newly found track element if there is one & make checks
				if(((track_element.TrackType == Points) || (track_element.TrackType == SignalPost) || (track_element.TrackType == Crossover)) &&
                   (TrackElementAt(1238, VecPos).TrackType == Buffers))
                {
                    Utilities->CallLogPop(1541);
                    return(false);
                }
				else if(((track_element.TrackType == Points) || (track_element.TrackType == SignalPost) || (track_element.TrackType == Crossover) ||
                         (track_element.TrackType == Bridge)) && (TrackElementAt(1243, VecPos).TrackType == Continuation))
                {
                    Utilities->CallLogPop(1542);
                    return(false);
                }
				else if((track_element.TrackType == SignalPost) && (TrackElementAt(1245, VecPos).TrackType == SignalPost) &&
                        (track_element.SpeedTag == TrackElementAt(1247, VecPos).SpeedTag))
                {
                    Utilities->CallLogPop(1543);
                    return(false);
                }
                else if(IsLCAtHV(47, track_element.HLoc, track_element.VLoc) && IsLCAtHV(48, TrackElementAt(1250, VecPos).HLoc, TrackElementAt(1251, VecPos).VLoc))
                // true if a level crossing is present at both x and VecPos - can't have two adjacent level crossings on the same track
                {
                    Utilities->CallLogPop(1981);
                    return(false);
                }
/* remove this restriction now that not permitted to treat a named continuation as a location stop
               else if(TrackElementAt(, x).TrackType == Continuation)
                   {
                   int H = TrackElementAt(, x).HLoc;
                   int V = TrackElementAt(, x).VLoc;
                   bool FoundFlag = false;
                   TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(18, H, V, FoundFlag);
                   if(FoundFlag)
                       {
                       if(InactiveTrackElementAt(93, IMPair.first).TrackType == NamedNonStationLocation)
                           {
                           int NewH = TrackElementAt(, (TrackElementAt(, x).Conn[1])).HLoc;
                           int NewV = TrackElementAt(, (TrackElementAt(, x).Conn[1])).VLoc;
                           TIMPair NewIMPair = GetVectorPositionsFromInactiveTrackMap(19, NewH, NewV, FoundFlag);
                           if(FoundFlag)
                               {
                               if(InactiveTrackElementAt(94, NewIMPair.first).TrackType != NamedNonStationLocation)
                                   {
                                   Utilities->CallLogPop();
                                   return false;
                                   }
                               }
                           else
                               {
                               Utilities->CallLogPop();
                               return false;
                               }
                           }
                       }
                   }
*/
                for(unsigned int a = 0; a < 4; a++)
                {
                    if((TrackElementAt(1252, VecPos).Link[a] == (10 - track_element.Link[y])) && (TrackElementAt(1254, VecPos).Config[a] != End) &&
                       (TrackElementAt(1255, VecPos).Config[a] != Gap))
                    {
                        track_element.ConnLinkPos[y] = a;
                        // note - this ensures that if the connecting element is a leading point
                        // then the ConnLinkPos value is 0 rather than 2, since 'a' starts at 0
                        // (Points have the same link value for both [0] and [2])
                        LinkFoundFlag = true;
                        break; // stop after first find or will find later link for leading point
                    }
                }
                if(!LinkFoundFlag)
                {
                    if(FinalCall)
                    {
                        throw Exception("Error in final track linkage in LinkTrackNoMessages - invalid link");
                    }
                    Utilities->CallLogPop(1128);
                    return(false);
                }
            }
            else // if(ConnectionFoundFlag)
            {
                if(FinalCall)
                {
                    throw Exception("Error in final track linkage in LinkTrackNoMessages - connection not found");
                }
                Utilities->CallLogPop(1129);
                return(false);
            }
        }
    } // for(unsigned int x=0;x<TrackVector.size();x++)

    if(FinalCall)
    {
        SetStationEntryStopLinkPosses(2);
    }
// final check
    bool ConnErrorFlag = false;

	for(auto track_element : TrackVector)
    {
		if((track_element.Link[0] > 0) && (track_element.Config[0] != End) && (track_element.Conn[0] == -1))
        {
            ConnErrorFlag = true;
        }
		if((track_element.Link[1] > 0) && (track_element.Config[1] != End) && (track_element.Conn[1] == -1))
        {
            ConnErrorFlag = true;
        }
        if((track_element.Link[2] > 0) && (track_element.Config[2] != End) && (track_element.Conn[2] == -1))
        {
            ConnErrorFlag = true;
        }
        if((track_element.Link[3] > 0) && (track_element.Config[3] != End) && (track_element.Conn[3] == -1))
        {
            ConnErrorFlag = true;
        }
        if(FinalCall) // StationStopLinks only set during FinalCall so only check at FinalCall
        {
            if(track_element.ActiveTrackElementName == "")
            {
				if((track_element.StationEntryStopLinkPos1 != -1) || (track_element.StationEntryStopLinkPos2 != -1))
				{
					auto iterator_ = std::find(TrackVector.begin(), TrackVector.end(), track_element);
                    const int position_ = std::distance(TrackVector.begin(), iterator_);
                    throw Exception("Error, StationEntryStopLinkPos not -1 for unnamed element at TrackVectorPosition = " + AnsiString(position_));
                }
            }
        }
    }
    if(ConnErrorFlag)
    {
        if(FinalCall)
        {
            throw Exception("ConnError in LinkTrack - Final");
		}
        else
        {
            throw Exception("ConnError in LinkTrack - Precheck");
        }
    }
    bool CLkErrorFlag = false;

    for(auto track_element : TrackVector)
    {
		if((track_element.Link[0] > 0) && (track_element.Config[0] != End) && (track_element.ConnLinkPos[0] == -1))
        {
            CLkErrorFlag = true;
		}
		if((track_element.Link[1] > 0) && (track_element.Config[1] != End) && (track_element.ConnLinkPos[1] == -1))
        {
            CLkErrorFlag = true;
        }
		if((track_element.Link[2] > 0) && (track_element.Config[2] != End) && (track_element.ConnLinkPos[2] == -1))
        {
            CLkErrorFlag = true;
        }
		if((track_element.Link[3] > 0) && (track_element.Config[3] != End) && (track_element.ConnLinkPos[3] == -1))
        {
            CLkErrorFlag = true;
        }
    }

    if(CLkErrorFlag)
    {
        if(FinalCall)
        {
            throw Exception("CLkError in LinkTrack - Final");
        }
        else
        {
            throw Exception("CLkError in LinkTrack - Precheck");
        }
    }
// set element lengths to min of 10m
    for(auto track_element : TrackVector)
    {
		if(track_element.TrackType == Erase)
        {
            continue; // skip blank elements
        }
		if((track_element.Length01 < 10) && (track_element.Length23 != -1))
        {
			track_element.Length01 = 10;
        }
		if((track_element.Length23 < 10) && (track_element.Length23 != -1))
        {
            track_element.Length23 = 10;
        }
    }

    if(FinalCall) // ONLY at FinalCall, no point calling twice
    {
        CalcHLocMinEtc(7);
    }
    Utilities->CallLogPop(1130);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::IsTrackLinked(int Caller) // not used any more
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsTrackLinked");
	for(auto track_element : TrackVector) // check all elements in turn
    {
		if(track_element.TrackType == Erase)
        {
            Utilities->CallLogPop(498);
            return(false);
        }
// check foot linkages
		if(track_element.TrackType == FootCrossing)
        {
            if(!CheckFootCrossingLinks(2, track_element))
            {
                Utilities->CallLogPop(499);
                return(false);
            }
        }
        for(unsigned int y = 0; y < 4; y++) // check all links for each element
        {
			if(track_element.Link[y] <= 0)
            {
                continue; // no link
            }
			if(track_element.Config[y] == End)
            {
                continue; // buffer or continuation
            }
			if(track_element.Config[y] == Gap)
            {
                continue; // gaps set later from GapMap

            }
            // get required H & V for track element joining link 'y'
			int NewHLoc = track_element.HLoc + LinkHVArray[track_element.Link[y]][0];
			int NewVLoc = track_element.VLoc + LinkHVArray[track_element.Link[y]][1];
            // find track element if present
            bool ConnectionFoundFlag = false;
            int VecPos = GetVectorPositionFromTrackMap(15, NewHLoc, NewVLoc, ConnectionFoundFlag);
            if(ConnectionFoundFlag)
            {
                track_element.Conn[y] = VecPos;
                // find connecting link in the newly found track element if there is one & make buffer check
                bool LinkFoundFlag = false;
				if(((track_element.TrackType == Points) || (track_element.TrackType == SignalPost) || (track_element.TrackType == Crossover)) &&
                   (TrackElementAt(1304, VecPos).TrackType == Buffers))
                {
                    Utilities->CallLogPop(500);
                    return(false);
                }
				else if((track_element.TrackType == SignalPost) && (TrackElementAt(1306, VecPos).TrackType == SignalPost) &&
						(track_element.SpeedTag == TrackElementAt(1308, VecPos).SpeedTag))
                {
                    Utilities->CallLogPop(501);
                    return(false);
                }
				else if((track_element.TrackType == SignalPost) && (TrackElementAt(1310, VecPos).TrackType == Continuation))
                {
                    Utilities->CallLogPop(502);
                    return(false);
                }
                else
                {
                    for(unsigned int a = 0; a < 4; a++)
                    {
						if((TrackElementAt(1311, VecPos).Link[a] == (10 - track_element.Link[y])) && (TrackElementAt(1313, VecPos).Config[a] != End) &&
                           (TrackElementAt(1314, VecPos).Config[a] != Gap))
                        {
                            track_element.ConnLinkPos[y] = a;
                            // note - this ensures that if the connecting element is a leading point
                            // then the ConnLinkPos value is 0 rather than 2, since 'a' starts at 0
                            // (Points have the same link value for both [0] and [2])
                            LinkFoundFlag = true;
                            break; // stop after first find or will find later link for leading point
                        }
                    }
                }
                if(!LinkFoundFlag)
                {
                    Utilities->CallLogPop(503);
                    return(false);
                }
            }
            else // if(ConnectionFoundFlag)
            {
                Utilities->CallLogPop(504);
                return(false);
            }
        }
    } // for(unsigned int x=0;x<TrackVector.size();x++)

// final check
    bool ConnErrorFlag = false;

	for(auto track_element : TrackVector)
    {
		if((track_element.Link[0] > 0) && (track_element.Config[0] != End) && (track_element.Conn[0] == -1))
        {
            ConnErrorFlag = true;
        }
		if((track_element.Link[1] > 0) && (track_element.Config[1] != End) && (track_element.Conn[1] == -1))
        {
            ConnErrorFlag = true;
        }
		if((track_element.Link[2] > 0) && (track_element.Config[2] != End) && (track_element.Conn[2] == -1))
        {
            ConnErrorFlag = true;
        }
        if((track_element.Link[3] > 0) && (track_element.Config[3] != End) && (track_element.Conn[3] == -1))
        {
            ConnErrorFlag = true;
        }
    }
    if(ConnErrorFlag)
    {
        Utilities->CallLogPop(505);
        return(false);
    }
    bool CLkErrorFlag = false;

    for(auto track_element : TrackVector)
    {
		if((track_element.Link[0] > 0) && (track_element.Config[0] != End) && (track_element.ConnLinkPos[0] == -1))
        {
            CLkErrorFlag = true;
        }
		if((track_element.Link[1] > 0) && (track_element.Config[1] != End) && (track_element.ConnLinkPos[1] == -1))
        {
            CLkErrorFlag = true;
		}
		if((track_element.Link[2] > 0) && (track_element.Config[2] != End) && (track_element.ConnLinkPos[2] == -1))
        {
            CLkErrorFlag = true;
        }
        if((track_element.Link[3] > 0) && (track_element.Config[3] != End) && (track_element.ConnLinkPos[3] == -1))
        {
            CLkErrorFlag = true;
        }
    }

    if(CLkErrorFlag)
    {
        Utilities->CallLogPop(506);
        return(false);
    }
    Utilities->CallLogPop(507);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::ResetGapsFromGapMap(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetGapsFromGapMap");
    int Position1, Position2;
    TTrackElement TrackElement1, TrackElement2;
    TGapMapIterator GapMapPtr;

    if(!GapMap.empty())
	{
		for(auto [key, value] : GapMap)
		{
			int HLoc1 = key.first;
			int VLoc1 = key.second;
			int HLoc2 = value.first;
            int VLoc2 = value.second;
            if(!FindNonPlatformMatch(12, HLoc1, VLoc1, Position1, TrackElement1))
            {
                throw Exception("Failed to find H & V for gap1, GapMap in error");
            }
            if(!FindNonPlatformMatch(13, HLoc2, VLoc2, Position2, TrackElement2))
            {
                throw Exception("Failed to find H & V for gap2, GapMap in error");
            }
            if(TrackElementAt(9, Position1).TrackType != GapJump)
            {
                throw Exception("Element at Pos1 not a gap, GapMap in error");
            }
            if(TrackElementAt(10, Position2).TrackType != GapJump)
            {
                throw Exception("Element at Pos2 not a gap, GapMap in error");
            }
            TrackElementAt(11, Position1).Conn[0] = Position2;
            TrackElementAt(12, Position1).ConnLinkPos[0] = 0;
            TrackElementAt(13, Position2).Conn[0] = Position1;
            TrackElementAt(14, Position2).ConnLinkPos[0] = 0;
        }
    }
    Utilities->CallLogPop(510);
    return(true);
}

// ---------------------------------------------------------------------------

void TTrack::TrackPush(int Caller, TTrackElement TrackElement)
{
// TIMPair MapEntry;
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrackPush," + AnsiString(TrackElement.HLoc) + "," +
                                 AnsiString(TrackElement.VLoc) + "," + AnsiString(TrackElement.SpeedTag));
    THVPair TrackMapKeyPair, InactiveTrackMapKeyPair;
    TTrackMapEntry TrackMapEntry, InactiveTrackMapEntry;
    TLocationNameMultiMapEntry LocationNameEntry;

    LocationNameEntry.first = TrackElement.LocationName;
    if((TrackElement.TrackType == Platform) || (TrackElement.TrackType == Concourse) || (TrackElement.TrackType == Parapet) ||
       (TrackElement.TrackType == NamedNonStationLocation) || (TrackElement.TrackType == LevelCrossing))
    {
// check whether a similar element already at this position and if so ignore it (had error where allowed multiple NonStationNamedLocs)
// could arise when loading old railways with multiple NonStationNamedLocs
        bool FoundFlag = false;
        TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(20, TrackElement.HLoc, TrackElement.VLoc, FoundFlag);
        if(FoundFlag)
        {
            if((InactiveTrackElementAt(97, IMPair.first).SpeedTag == TrackElement.SpeedTag) || (InactiveTrackElementAt(98,
                                                                                                                       IMPair.second).SpeedTag == TrackElement.SpeedTag))
            {
                Utilities->CallLogPop(1813);
                return;
            }
        }
        InactiveTrackVector.push_back(TrackElement); // no erase elements involved in InactiveTrackVector
        InactiveTrackMapKeyPair.first = TrackElement.HLoc;
        InactiveTrackMapKeyPair.second = TrackElement.VLoc;
        InactiveTrackMapEntry.first = InactiveTrackMapKeyPair;
        InactiveTrackMapEntry.second = InactiveTrackVector.size() - 1;
        InactiveTrack2MultiMap.insert(InactiveTrackMapEntry);
        if(TrackElement.FixedNamedLocationElement)
        {
            LocationNameEntry.second = InactiveTrackVector.size() - 1; // add to LocationNameMultiMap
            LocationNameMultiMap.insert(LocationNameEntry);
        }
        if(TrackElement.HLoc < HLocMin)
        {
            HLocMin = TrackElement.HLoc;
        }
        if(TrackElement.HLoc > HLocMax)
        {
            HLocMax = TrackElement.HLoc;
        }
        if(TrackElement.VLoc < VLocMin)
        {
            VLocMin = TrackElement.VLoc;
        }
        if(TrackElement.VLoc > VLocMax)
        {
            VLocMax = TrackElement.VLoc;
        }
    }
    else
    {
// check whether a similar element already at this position and if so ignore it (had error where allowed multiple NonStationNamedLocs)
// shouldn't arise but leave in as a safeguard
        bool FoundFlag = false;
        int VecPos = GetVectorPositionFromTrackMap(44, TrackElement.HLoc, TrackElement.VLoc, FoundFlag);
        if(FoundFlag)
        {
            if(TrackElementAt(816, VecPos).SpeedTag == TrackElement.SpeedTag)
            {
                Utilities->CallLogPop(1814);
                return;
            }
        }
        TrackVector.push_back(TrackElement); // add erase elements to vector to keep linkages correct (now dispensed with)
        if(TrackElement.TrackType != Erase) // don't add erase elements to TrackMap  (dispensed with these but keep code)
        {
            TrackMapKeyPair.first = TrackElement.HLoc;
            TrackMapKeyPair.second = TrackElement.VLoc;
            TrackMapEntry.first = TrackMapKeyPair;
            TrackMapEntry.second = TrackVector.size() - 1;
            TrackMap.insert(TrackMapEntry);
            if(TrackElement.FixedNamedLocationElement)
            {
                LocationNameEntry.second = -(int)(TrackVector.size()); // add to LocationNameMultiMap  TrackVector.size = Required value + 1, so ...second = -1-Requ'd value
                LocationNameMultiMap.insert(LocationNameEntry);
            }
            if(TrackElement.HLoc < HLocMin)
            {
                HLocMin = TrackElement.HLoc; // exclude erase elements as HLoc & VLoc set to -2000000000
            }
            if(TrackElement.HLoc > HLocMax)
            {
                HLocMax = TrackElement.HLoc;
            }
            if(TrackElement.VLoc < VLocMin)
            {
                VLocMin = TrackElement.VLoc;
            }
            if(TrackElement.VLoc > VLocMax)
            {
                VLocMax = TrackElement.VLoc;
            }
        }
    }
// CheckMapAndTrack(6);//test               drop these to speed up, still checked outside this function
// CheckMapAndInactiveTrack(6);//test

// CheckLocationNameMultiMap(14);//test   Can't test here as when loading the ActiveTrackElementName elements will be out of step
// with the Platforms until layout fully loaded
    Utilities->CallLogPop(511);
}

// ---------------------------------------------------------------------------

int TTrack::GetVectorPositionFromTrackMap(int Caller, int HLoc, int VLoc, bool &FoundFlag)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetVectorPositionFromTrackMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    THVPair TrackMapKeyPair;

    FoundFlag = false;
    TTrackMapIterator TrackMapPtr;

    TrackMapKeyPair.first = HLoc;
    TrackMapKeyPair.second = VLoc;
    TrackMapPtr = TrackMap.find(TrackMapKeyPair);
    if(TrackMapPtr == TrackMap.end())
    {
        Utilities->CallLogPop(512);
        return(-1); // nothing found
    }
    else
    {
        FoundFlag = true;
        Utilities->CallLogPop(513);
        return(TrackMapPtr->second);
    }
}

// ---------------------------------------------------------------------------

TTrackElement &TTrack::GetTrackElementFromTrackMap(int Caller, int HLoc, int VLoc)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTrackElementFromTrackMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    THVPair TrackMapKeyPair;
    TTrackMapIterator TrackMapPtr;

    TrackMapKeyPair.first = HLoc;
    TrackMapKeyPair.second = VLoc;
    TrackMapPtr = TrackMap.find(TrackMapKeyPair);
    if(TrackMapPtr == TrackMap.end())
    {
        AnsiString Message = "Element not found at HLoc " + AnsiString(HLoc) + " and VLoc " + AnsiString(VLoc);
        throw Exception(Message);
    }
    else
    {
        Utilities->CallLogPop(1943);
        return(TrackElementAt(871, TrackMapPtr->second));
    }
}

// ---------------------------------------------------------------------------

TTrackElement &TTrack::GetTrackElementFromAnyTrackMap(int Caller, int HLoc, int VLoc, TTrackMap &Map, TTrackVector &Vector) //new at v2.9.0 for clipboard pref dirs
{                                                                                                                           //modded at v2.9.2 to make Map & Vector references
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTrackElementFromAnyTrackMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    THVPair MapKeyPair;
    TTrackMapIterator MapPtr;

    MapKeyPair.first = HLoc;
    MapKeyPair.second = VLoc;
    MapPtr = Map.find(MapKeyPair);
    if(MapPtr == Map.end())
    {
        AnsiString Message = "Element not found at HLoc " + AnsiString(HLoc) + " and VLoc " + AnsiString(VLoc) + " in GetTrackElementFromAnyTrackMap";
        throw Exception(Message);
    }
    else
    {
        Utilities->CallLogPop(2280);
        return(Vector.at(MapPtr->second));
    }
}

// ---------------------------------------------------------------------------

TTrackElement &TTrack::GetInactiveTrackElementFromTrackMap(int Caller, int HLoc, int VLoc)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetInactiveTrackElementFromTrackMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    THVPair InactiveTrackMapKeyPair;
    TInactiveTrack2MultiMapIterator InactiveTrackMapPtr;

    InactiveTrackMapKeyPair.first = HLoc;
    InactiveTrackMapKeyPair.second = VLoc;
    InactiveTrackMapPtr = InactiveTrack2MultiMap.find(InactiveTrackMapKeyPair); // not interested in platforms so only need to find one
    if(InactiveTrackMapPtr == InactiveTrack2MultiMap.end())
    {
        AnsiString Message = "Inactive element not found at HLoc " + AnsiString(HLoc) + " and VLoc " + AnsiString(VLoc);
        throw Exception(Message);
    }
    else
    {
        Utilities->CallLogPop(1949);
        return(InactiveTrackElementAt(34, InactiveTrackMapPtr->second));
    }
}

// ---------------------------------------------------------------------------

bool TTrack::TrackElementPresentAtHV(int Caller, int HLoc, int VLoc)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrackElementPresentAtHV," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    bool Present = true;
    THVPair TrackMapKeyPair;
    TTrackMapIterator TrackMapPtr;

    TrackMapKeyPair.first = HLoc;
    TrackMapKeyPair.second = VLoc;
    TrackMapPtr = TrackMap.find(TrackMapKeyPair);
    if(TrackMapPtr == TrackMap.end())
    {
        Present = false;
    }
    Utilities->CallLogPop(2057);
    return(Present);
}

// ---------------------------------------------------------------------------

bool TTrack::InactiveTrackElementPresentAtHV(int Caller, int HLoc, int VLoc)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",InactiveTrackElementPresentAtHV," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    bool Present = true;
    THVPair InactiveTrackMapKeyPair;
    TInactiveTrack2MultiMapIterator InactiveTrackMapPtr;

    InactiveTrackMapKeyPair.first = HLoc;
    InactiveTrackMapKeyPair.second = VLoc;
    InactiveTrackMapPtr = InactiveTrack2MultiMap.find(InactiveTrackMapKeyPair); // not interested in platforms so only need to find one
    if(InactiveTrackMapPtr == InactiveTrack2MultiMap.end())
    {
        Present = false;
    }
    Utilities->CallLogPop(2058);
    return(Present);
}

// ---------------------------------------------------------------------------

TTrack::TIMPair TTrack::GetVectorPositionsFromInactiveTrackMap(int Caller, int HLoc, int VLoc, bool &FoundFlag)
// max number of elements is 2, for platforms
// note that both elements of RetPair may be the same, if only one present in map
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetVectorPositionsFromInactiveTrackMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    THVPair InactiveTrackMapKeyPair;
    TIMPair RetPair;
    TInactiveTrackRange InactiveTrackRange;

    FoundFlag = false;
    InactiveTrackMapKeyPair.first = HLoc;
    InactiveTrackMapKeyPair.second = VLoc;
    if(InactiveTrack2MultiMap.empty())
    {
        RetPair.first = 0;
        RetPair.second = 0;
        Utilities->CallLogPop(1815);
        return(RetPair); // map empty
    }
    InactiveTrackRange = InactiveTrack2MultiMap.equal_range(InactiveTrackMapKeyPair);
    if(InactiveTrackRange.first == InactiveTrackRange.second)
    {
        RetPair.first = 0;
        RetPair.second = 0;
        Utilities->CallLogPop(514);
        return(RetPair); // nothing found
    }
    else
    {
        RetPair.first = InactiveTrackRange.first->second;
        RetPair.second = (--InactiveTrackRange.second)->second;
        FoundFlag = true;
        Utilities->CallLogPop(515);
        return(RetPair);
    }
}

// ---------------------------------------------------------------------------

bool TTrack::MatchingPoint(int Caller, unsigned int TrackVectorPosition, unsigned int DivergingPosition) //changed at v2.13.0 to return true for failed but matching points
{
// only change where have adjacent points with their diverging links connected - not appropriate for non-straight points
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",MatchingPoint," + AnsiString(TrackVectorPosition) + "," +
                                 AnsiString(DivergingPosition));
    TTrackElement T1 = TrackElementAt(15, TrackVectorPosition);
    TTrackElement T2 = TrackElementAt(16, DivergingPosition);
    int SpeedTag1 = T1.SpeedTag;
    int SpeedTag2 = T2.SpeedTag;

    if((T1.Attribute) != (T2.Attribute))
    {
        Utilities->CallLogPop(516);
        return(false);
    }
    if(((SpeedTag1 == 7) && (SpeedTag2 == 10)) || // straight track hor, diverging track vert
       ((SpeedTag1 == 10) && (SpeedTag2 == 7)) || ((SpeedTag1 == 8) && (SpeedTag2 == 9)) || ((SpeedTag1 == 9) && (SpeedTag2 == 8)) ||
       ((SpeedTag1 == 11) && (SpeedTag2 == 14)) ||  // straight track vert, diverging track hor
       ((SpeedTag1 == 14) && (SpeedTag2 == 11)) || ((SpeedTag1 == 12) && (SpeedTag2 == 13)) || ((SpeedTag1 == 13) && (SpeedTag2 == 12)) ||
       ((SpeedTag1 == 28) && (SpeedTag2 == 31)) ||  // straight track hor, diverging track 45 deg
       ((SpeedTag1 == 31) && (SpeedTag2 == 28)) || ((SpeedTag1 == 29) && (SpeedTag2 == 30)) || ((SpeedTag1 == 30) && (SpeedTag2 == 29)) ||
       ((SpeedTag1 == 32) && (SpeedTag2 == 35)) ||  // straight track vert, diverging track 45 deg
       ((SpeedTag1 == 35) && (SpeedTag2 == 32)) || ((SpeedTag1 == 33) && (SpeedTag2 == 34)) || ((SpeedTag1 == 34) && (SpeedTag2 == 33)) ||
       ((SpeedTag1 == 36) && (SpeedTag2 == 39)) ||  // straight track 45 deg, diverging track vert
       ((SpeedTag1 == 39) && (SpeedTag2 == 36)) || ((SpeedTag1 == 37) && (SpeedTag2 == 38)) || ((SpeedTag1 == 38) && (SpeedTag2 == 37)) ||
       ((SpeedTag1 == 40) && (SpeedTag2 == 43)) ||  // straight track 45 deg, diverging track hor
       ((SpeedTag1 == 43) && (SpeedTag2 == 40)) || ((SpeedTag1 == 41) && (SpeedTag2 == 42)) || ((SpeedTag1 == 42) && (SpeedTag2 == 41)))
    {
        Utilities->CallLogPop(517);
        return(true);
    }
    else
    {
        Utilities->CallLogPop(518);
        return(false);
    }
}

// ---------------------------------------------------------------------------

/*
      bool TMapComp::operator() (const THVPair& lower, const THVPair& higher) const///HLoc  VLoc
      {
      if(lower.second < higher.second) return true;
      else if(lower.second > higher.second) return false;
      else if(lower.second == higher.second)
       {
       if(lower.first < higher.first) return true;
       }
      return false;
      }
*/
// ---------------------------------------------------------------------------

void TTrack::PlotGap(int Caller, TTrackElement TrackElement, TDisplay *Disp)
// no need to check corresponding gap, if that not set correctly it will be picked up in GapsUnset()
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotGap," + TrackElement.LogTrack(1));
    if(TrackElement.TrackType != GapJump)
    {
        throw Exception("Error, Wrong track type in PlotGap");
    }
    if((TrackElement.SpeedTag == 88) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(39, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl88set);
    }
    else if((TrackElement.SpeedTag == 88) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(40, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl88unset);
    }
    if((TrackElement.SpeedTag == 89) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(41, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl89set);
    }
    else if((TrackElement.SpeedTag == 89) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(42, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl89unset);
    }
    if((TrackElement.SpeedTag == 90) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(43, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl90set);
    }
    else if((TrackElement.SpeedTag == 90) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(44, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl90unset);
    }
    if((TrackElement.SpeedTag == 91) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(45, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl91set);
    }
    else if((TrackElement.SpeedTag == 91) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(46, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl91unset);
    }
    if((TrackElement.SpeedTag == 92) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(47, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl92set);
    }
    else if((TrackElement.SpeedTag == 92) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(48, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl92unset);
    }
    if((TrackElement.SpeedTag == 93) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(49, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->bm93set);
    }
    else if((TrackElement.SpeedTag == 93) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(50, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->bm93unset);
    }
    if((TrackElement.SpeedTag == 94) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(51, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->bm94set);
    }
    else if((TrackElement.SpeedTag == 94) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(52, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->bm94unset);
    }
    if((TrackElement.SpeedTag == 95) && (TrackElement.Conn[0] > -1))
    {
        Disp->PlotOutput(53, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl95set);
    }
    else if((TrackElement.SpeedTag == 95) && (TrackElement.Conn[0] == -1))
    {
        Disp->PlotOutput(54, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->gl95unset);
    }
    Utilities->CallLogPop(1101);
}

// ---------------------------------------------------------------------------

void TTrack::PlotContinuation(int Caller, TTrackElement TrackElement, TDisplay *Disp) //added for multiplayer to add overlays where coupled
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotContinuation," + TrackElement.LogTrack(1));
    TrackElement.PlotVariableTrackElement(7, Disp);
    if(!MultiplayerOverlayMap.empty()) //if it is empty then no overlays needed   [map of key = THVPair, value = graphic pointer]
    {
        THVPair PosPair;
        PosPair.first = TrackElement.HLoc;
        PosPair.second = TrackElement.VLoc;
        TMultiplayerOverlayMap::iterator MOMIt = MultiplayerOverlayMap.find(PosPair);
        if(MOMIt != MultiplayerOverlayMap.end()) //if it is then no overlay is needed
        {
            Disp->PlotOutput(283, TrackElement.HLoc * 16, TrackElement.VLoc * 16, MOMIt->second);
        }
    }
    Utilities->CallLogPop(2403);
}

// ---------------------------------------------------------------------------

void TTrack::PlotPoints(int Caller, TTrackElement TrackElement, TDisplay *Disp, bool BothFillets)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotPoints," + TrackElement.LogTrack(2));
    if(TrackElement.TrackType != Points)
    {
        throw Exception("Error, Wrong track type in PlotPoints");
    }
    Disp->PlotPointBlank(0, TrackElement.HLoc, TrackElement.VLoc); // to get rid of earlier fillet
    TrackElement.PlotVariableTrackElement(4, Disp);
    if(BothFillets)
    {
        if(TrackElement.SpeedTag < 28)
        {
            Disp->PlotOutput(55, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 7][0]);
            Disp->PlotOutput(73, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 7][1]);
        }
        else if(TrackElement.SpeedTag < 132)
        {
            Disp->PlotOutput(56, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 20][0]);
            Disp->PlotOutput(74, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 20][1]);
        }
        else
        {
            Disp->PlotOutput(70, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 108][0]);
            Disp->PlotOutput(71, TrackElement.HLoc * 16, TrackElement.VLoc * 16, RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 108][1]);
        }
    }
    else if(!TrackElement.Failed) //not failed
    {
        if(TrackElement.SpeedTag < 28)
        {
            Disp->PlotOutput(75, TrackElement.HLoc * 16, TrackElement.VLoc * 16,
                             RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 7][TrackElement.Attribute]); //0 to 7 incl after subtraction
        }
        else if(TrackElement.SpeedTag < 132)
        {
            Disp->PlotOutput(76, TrackElement.HLoc * 16, TrackElement.VLoc * 16,
                             RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 20][TrackElement.Attribute]); //8 to 23 incl after subtraction
        }
        else
        {
            Disp->PlotOutput(72, TrackElement.HLoc * 16, TrackElement.VLoc * 16,
                             RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 108][TrackElement.Attribute]); //24 to 31 incl after subtraction
        }
    }
    else //failed in fixed position
    {
        if(TrackElement.SpeedTag < 28)
        {
            Disp->PlotOutput(284, TrackElement.HLoc * 16, TrackElement.VLoc * 16,
                             RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 7][TrackElement.Attribute]); //0 to 7 incl after subtraction
        }
        else if(TrackElement.SpeedTag < 132)
        {
            Disp->PlotOutput(285, TrackElement.HLoc * 16, TrackElement.VLoc * 16,
                             RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 20][TrackElement.Attribute]); //8 to 23 incl after subtraction
        }
        else
        {
            Disp->PlotOutput(286, TrackElement.HLoc * 16, TrackElement.VLoc * 16,
                             RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 108][TrackElement.Attribute]); //24 to 31 incl after subtraction
        }
        Disp->GetImage()->Canvas->Draw((TrackElement.HLoc - Display->DisplayOffsetH) * 16, (TrackElement.VLoc - Display->DisplayOffsetV) * 16, RailGraphics->BlackOctagon); //indicates that it has failed
    }
// replot platform if required
    TIMPair IMPair;
    bool FoundFlag;

    IMPair = GetVectorPositionsFromInactiveTrackMap(15, TrackElement.HLoc, TrackElement.VLoc, FoundFlag);
    if(FoundFlag)
    {
        // only one platform possible at points so only need to plot IMPair.first
        TTrackElement PlatElement = InactiveTrackElementAt(89, IMPair.first);
        PlatElement.PlotVariableTrackElement(5, Disp); // to plot as striped or non-striped depending on whether named or not
    }
    Utilities->CallLogPop(519);
}

// ---------------------------------------------------------------------------

void TTrack::PlotSignal(int Caller, TTrackElement TrackElement, TDisplay *Disp)
{
// Can't use TrackElement.PlotVariableTrackElement() here as graphic changes depending on signal colour
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotSignal," + TrackElement.LogTrack(3));
    if(TrackElement.TrackType != SignalPost)
    {
        throw Exception("Error, Wrong track type in PlotSignal");
    }
    if(!TrackElement.Failed) //added at v2.13.0
    {
        for(int x = 0; x < 40; x++)
        {
            if((SigTable[x].SpeedTag == TrackElement.SpeedTag) && (SigTable[x].Attribute == TrackElement.Attribute))
            {
                // plot blank first, then plot platform if present - striped or not depending on LocationName being set
                Disp->PlotSignalBlank(0, TrackElement.HLoc, TrackElement.VLoc, TrackElement.SpeedTag, Utilities->RHSignalFlag);
    // in case existing signal is a double yellow
                // plot platforms if present
    // Graphics::TBitmap* SignalPlatformGraphic;
    // if(PlatformOnSignalSide(0, TrackElement.HLoc, TrackElement.VLoc, TrackElement.SpeedTag, SignalPlatformGraphic))
    // Above dropped at v2.3.0.  Now plot either or both platforms if present regardless of which side they are on.  The platforms will
    // be consistent with the signal graphic as can't enter an inappropriate platform.  The new right hand signal option caused platforms
    // to not be plotted with the above function.
                PlotSignalPlatforms(0, TrackElement.HLoc, TrackElement.VLoc, Disp); // if no platforms nothing is plotted
                // now plot signal (double yellow overwrites most of signal platform if present)
                // additions at version 0.6 for other aspects & ground sigs
                if(TrackElement.SigAspect == TTrackElement::ThreeAspect)
                {
                    Disp->PlotOutput(117, TrackElement.HLoc * 16, TrackElement.VLoc * 16, SigTableThreeAspect[x].SigPtr);
                }
                else if(TrackElement.SigAspect == TTrackElement::TwoAspect)
                {
                    Disp->PlotOutput(118, TrackElement.HLoc * 16, TrackElement.VLoc * 16, SigTableTwoAspect[x].SigPtr);
                }
                else if(TrackElement.SigAspect == TTrackElement::GroundSignal)
                {
                    Disp->PlotOutput(119, TrackElement.HLoc * 16, TrackElement.VLoc * 16, SigTableGroundSignal[x].SigPtr);
                }
                else // 4 aspect
                {
                    Disp->PlotOutput(58, TrackElement.HLoc * 16, TrackElement.VLoc * 16, SigTable[x].SigPtr);
                }
                if((TrackElement.CallingOnSet) && (TrackElement.SigAspect != TTrackElement::GroundSignal))
                // normal signal calling on, need to add extra graphic, basic red signal plotted above from SigTable
                {
                    if(TrackElement.SpeedTag == 68)
                    {
                        Disp->PlotOutput(59, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm68CallingOn);
                    }
                    if(TrackElement.SpeedTag == 69)
                    {
                        Disp->PlotOutput(60, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm69CallingOn);
                    }
                    if(TrackElement.SpeedTag == 70)
                    {
                        Disp->PlotOutput(61, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm70CallingOn);
                    }
                    if(TrackElement.SpeedTag == 71)
                    {
                        Disp->PlotOutput(62, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm71CallingOn);
                    }
                    if(TrackElement.SpeedTag == 72)
                    {
                        Disp->PlotOutput(63, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm72CallingOn);
                    }
                    if(TrackElement.SpeedTag == 73)
                    {
                        Disp->PlotOutput(64, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm73CallingOn);
                    }
                    if(TrackElement.SpeedTag == 74)
                    {
                        Disp->PlotOutput(65, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm74CallingOn);
                    }
                    if(TrackElement.SpeedTag == 75)
                    {
                        Disp->PlotOutput(66, (TrackElement.HLoc * 16), (TrackElement.VLoc * 16), RailGraphics->bm75CallingOn);
                    }
                }
                else if((TrackElement.CallingOnSet) && (TrackElement.SigAspect == TTrackElement::GroundSignal))
                // ground signal calling on, need to use normal proceed aspect
                {
                    for(int x = 0; x < 40; x++)
                    {
                        if((SigTableGroundSignal[x].SpeedTag == TrackElement.SpeedTag) && (SigTable[x].Attribute == 1)) // use attr 1 for proceed
                        {
                            // plot blank first, then plot platform if present - striped or not depending on LocationName being set
                            Disp->PlotSignalBlank(1, TrackElement.HLoc, TrackElement.VLoc, TrackElement.SpeedTag, Utilities->RHSignalFlag);
                            // plot special signal platform if present
                            Graphics::TBitmap* SignalPlatformGraphic;
                            PlotSignalPlatforms(1, TrackElement.HLoc, TrackElement.VLoc, Disp);
                            // now plot signal
                            Disp->PlotOutput(123, TrackElement.HLoc * 16, TrackElement.VLoc * 16, SigTableGroundSignal[x].SigPtr);
                        }
                    }
                }
                break;
            }
        }
    }
    else //failed added at v2.13.0
    {
        if(TrackElement.SigAspect != TTrackElement::GroundSignal)
        {
            for(int x = 0; x < 8; x++)
            {
                if(FailedSigTable[x].SpeedTag == TrackElement.SpeedTag)
                {
                    // plot blank first, then plot platform if present - striped or not depending on LocationName being set
                    Disp->PlotSignalBlank(2, TrackElement.HLoc, TrackElement.VLoc, TrackElement.SpeedTag, Utilities->RHSignalFlag);
                    PlotSignalPlatforms(2, TrackElement.HLoc, TrackElement.VLoc, Disp); // if no platforms nothing is plotted
                    Disp->PlotOutput(287, TrackElement.HLoc * 16, TrackElement.VLoc * 16, FailedSigTable[x].SigPtr);
                    Disp->GetImage()->Canvas->Draw((TrackElement.HLoc - Display->DisplayOffsetH) * 16, (TrackElement.VLoc - Display->DisplayOffsetV) * 16, RailGraphics->BlackOctagon); //indicates that it has failed
                    break;
                }
            }
        }
        else
        {
            for(int x = 0; x < 8; x++)
            {
                if(FailedGroundSigTable[x].SpeedTag == TrackElement.SpeedTag)
                {
                    // plot blank first, then plot platform if present - striped or not depending on LocationName being set
                    Disp->PlotSignalBlank(3, TrackElement.HLoc, TrackElement.VLoc, TrackElement.SpeedTag, Utilities->RHSignalFlag);
                    PlotSignalPlatforms(3, TrackElement.HLoc, TrackElement.VLoc, Disp); // if no platforms nothing is plotted
                    Disp->PlotOutput(288, TrackElement.HLoc * 16, TrackElement.VLoc * 16, FailedGroundSigTable[x].SigPtr);
                    Disp->GetImage()->Canvas->Draw((TrackElement.HLoc - Display->DisplayOffsetH) * 16, (TrackElement.VLoc - Display->DisplayOffsetV) * 16, RailGraphics->BlackOctagon); //indicates that it has failed
                    break;
                }
            }
        }
    }
    Utilities->CallLogPop(520);
}

// ---------------------------------------------------------------------------

void TTrack::PlotSignalPlatforms(int Caller, int HLoc, int VLoc, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotSignalPlatforms," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(13, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(2112);
        return;
    }
    TTrackElement IAElement1 = InactiveTrackElementAt(124, IMPair.first);
    TTrackElement IAElement2 = InactiveTrackElementAt(125, IMPair.second);

    // don't want 'else if' for the below as may need to plot 2 platforms
    if((IAElement1.SpeedTag == 76) || (IAElement2.SpeedTag == 76)) // top plat
    {
        if(IAElement1.LocationName == "") // '2' will be same
        {
            Disp->PlotOutput(239, HLoc * 16, VLoc * 16, RailGraphics->gl76Striped);
        }
        else
        {
            Disp->PlotOutput(240, HLoc * 16, VLoc * 16, RailGraphics->gl76);
        }
    }
    if((IAElement1.SpeedTag == 77) || (IAElement2.SpeedTag == 77)) // bot plat
    {
        if(IAElement1.LocationName == "") // '2' will be same
        {
            Disp->PlotOutput(241, HLoc * 16, VLoc * 16, RailGraphics->bm77Striped);
        }
        else
        {
            Disp->PlotOutput(242, HLoc * 16, VLoc * 16, RailGraphics->bm77);
        }
    }
    if((IAElement1.SpeedTag == 78) || (IAElement2.SpeedTag == 78)) // lh plat
    {
        if(IAElement1.LocationName == "") // '2' will be same
        {
            Disp->PlotOutput(243, HLoc * 16, VLoc * 16, RailGraphics->bm78Striped);
        }
        else
        {
            Disp->PlotOutput(244, HLoc * 16, VLoc * 16, RailGraphics->bm78);
        }
    }
    if((IAElement1.SpeedTag == 79) || (IAElement2.SpeedTag == 79)) // rh plat
    {
        if(IAElement1.LocationName == "") // '2' will be same
        {
            Disp->PlotOutput(245, HLoc * 16, VLoc * 16, RailGraphics->gl79Striped);
        }
        else
        {
            Disp->PlotOutput(246, HLoc * 16, VLoc * 16, RailGraphics->gl79);
        }
    }
    Utilities->CallLogPop(2113);
}

// ---------------------------------------------------------------------------

void TTrack::SetLinkedLevelCrossingBarrierAttributes(int Caller, int HLoc, int VLoc, int Attr)
{
// Set attrs to 0=closed to trains; 1=open to trains; 2 = changing = closed to trains
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LowerLinkedLevelCrossingBarrierAttributes," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
// find topmost LC, opening them all (to trains) in turn
    int UpStep = 0;

    while(IsLCAtHV(0, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
    {
        SetLCAttributeAtHV(0, HLoc, (VLoc + UpStep), Attr);
        UpStep--;
    }
// now find bottommost LC, opening them all (to trains) in turn
    int DownStep = 1;

    while(IsLCAtHV(1, HLoc, (VLoc + DownStep)))
    {
        SetLCAttributeAtHV(1, HLoc, (VLoc + DownStep), Attr);
        DownStep++;
    }
// find leftmost LC, opening them all (to trains) in turn
    int LeftStep = 0;

    while(IsLCAtHV(2, (HLoc + LeftStep), VLoc)) // will always find LC at LeftStep == 0
    {
        SetLCAttributeAtHV(2, (HLoc + LeftStep), VLoc, Attr);
        LeftStep--;
    }
// now find rightmost LC, opening them all (to trains) in turn
    int RightStep = 1;

    while(IsLCAtHV(3, (HLoc + RightStep), VLoc))
    {
        SetLCAttributeAtHV(3, (HLoc + RightStep), VLoc, Attr);
        RightStep++;
    }
    Utilities->CallLogPop(1915);
}

// ---------------------------------------------------------------------------

void TTrack::SetLinkedManualLCs(int Caller, int HLoc, int VLoc) //sets TypeOfRoute to 2 for all linked LCs
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetLinkedManualLCs," + AnsiString(HLoc) + "," + AnsiString(VLoc));
// work upwards setting all to manual
    int UpStep = -1;

    while(IsLCAtHV(51, HLoc, (VLoc + UpStep)))
    {
        SetBarriersDownLCToManual(0, HLoc, (VLoc + UpStep));
        UpStep--;
    }
// work downwards setting all to manual
    int DownStep = 1;

    while(IsLCAtHV(52, HLoc, (VLoc + DownStep)))
    {
        SetBarriersDownLCToManual(1, HLoc, (VLoc + DownStep));
        DownStep++;
    }
// work leftwards setting all to manual
    int LeftStep = -1;

    while(IsLCAtHV(53, (HLoc + LeftStep), VLoc))
    {
        SetBarriersDownLCToManual(2, (HLoc + LeftStep), VLoc);
        LeftStep--;
    }
// work rightwards setting all to manual
    int RightStep = 1;

    while(IsLCAtHV(54, (HLoc + RightStep), VLoc))
    {
        SetBarriersDownLCToManual(3, (HLoc + RightStep), VLoc);
        RightStep++;
    }
    Utilities->CallLogPop(2242);
}

// ---------------------------------------------------------------------------

void TTrack::SetBarriersDownLCToManual(int Caller, int HLoc, int VLoc)
{
    // Set TypeOfRoute value to 2 to indicate barriers manually closed
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetBarriersDownLCToManual," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    for(unsigned int x = 0; x < BarriersDownVector.size(); x++)
    {
        if((BarriersDownVector.at(x).HLoc == HLoc) && (BarriersDownVector.at(x).VLoc == VLoc))
        {
            BarriersDownVector.at(x).TypeOfRoute = 2;
            break;
        }
    }
    Utilities->CallLogPop(2243);
}

// ---------------------------------------------------------------------------

bool TTrack::AnyLinkedBarrierDownVectorManual(int Caller, int HLoc, int VLoc, int &BDVectorPos)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AnyLinkedBarrierDownVectorManual," + AnsiString(HLoc) + "," + AnsiString(VLoc));
// work upwards
    int UpStep = 0; //start with this location

    while(IsLCAtHV(55, HLoc, (VLoc + UpStep)))
    {
        if(IsBarrierDownVectorAtHVManual(0, HLoc, (VLoc + UpStep), BDVectorPos))
        {
            Utilities->CallLogPop(2244);
            return(true);
        }
        UpStep--;
    }
// work downwards
    int DownStep = 1;

    while(IsLCAtHV(56, HLoc, (VLoc + DownStep)))
    {
        if(IsBarrierDownVectorAtHVManual(1, HLoc, (VLoc + DownStep), BDVectorPos))
        {
            Utilities->CallLogPop(2245);
            return(true);
        }
        DownStep++;
    }
// work leftwards
    int LeftStep = -1;

    while(IsLCAtHV(57, (HLoc + LeftStep), VLoc))
    {
        if(IsBarrierDownVectorAtHVManual(2, (HLoc + LeftStep), VLoc, BDVectorPos))
        {
            Utilities->CallLogPop(2246);
            return(true);
        }
        LeftStep--;
    }
// work rightwards
    int RightStep = 1;

    while(IsLCAtHV(58, (HLoc + RightStep), VLoc))
    {
        if(IsBarrierDownVectorAtHVManual(3, (HLoc + RightStep), VLoc, BDVectorPos))
        {
            Utilities->CallLogPop(2247);
            return(true);
        }
        RightStep++;
    }
    Utilities->CallLogPop(2248);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::IsBarrierDownVectorAtHVManual(int Caller, int HLoc, int VLoc, int &BDVectorPos)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsBarrierDownVectorAtHVManual," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    for(unsigned int x = 0; x < BarriersDownVector.size(); x++)
    {
        if((BarriersDownVector.at(x).HLoc == HLoc) && (BarriersDownVector.at(x).VLoc == VLoc) && (BarriersDownVector.at(x).TypeOfRoute == 2))
        {
            BDVectorPos = x;
            Utilities->CallLogPop(2249);
            return(true);
        }
    }
    BDVectorPos = -1;
    Utilities->CallLogPop(2250);
    return(false);
}

// ---------------------------------------------------------------------------

void TTrack::PlotLoweredLinkedLevelCrossingBarriers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, int TypeOfRoute, TDisplay *Disp, bool Manual)
// open to trains
// BaseElementSpeedTag: 1 = Horizontal track, 2 = vertical track
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotLoweredLinkedLevelCrossingBarriers," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    if(!IsLCAtHV(4, HLoc, VLoc))
    {
        throw Exception("Error, Wrong track type in PlotAndLowerLevelCrossingBarriers");
    }
    if((BaseElementSpeedTag != 1) && (BaseElementSpeedTag != 2))
    {
        throw Exception("Error, Wrong BaseElementSpeedTag value in PlotAndLowerLevelCrossingBarriers");
    }
// check for adjacent LCs & if so open (to trains)
    if(BaseElementSpeedTag == 1) // hor track element
    {
        // find topmost LC, opening them all (to trains) in turn
        int UpStep = 0;
        while(IsLCAtHV(5, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
        {
            UpStep--;
        }
        UpStep++;
        // now find bottommost LC, opening them all (to trains) in turn
        int DownStep = 1;
        while(IsLCAtHV(6, HLoc, (VLoc + DownStep)))
        {
            DownStep++;
        }
        DownStep--;
        // now plot graphics, UpStep is smallest & DownStep largest
        // RouteGraphic is the coloured track element, BaseGraphic is non-coloured
        // Only need to plot the coloured graphic for the HLoc & VLoc in the vector as that is the route that is causeing the LC to flash
        Graphics::TBitmap *RouteGraphic;
        Graphics::TBitmap *BaseGraphic = RailGraphics->gl1;
        if(TypeOfRoute == 1)
        {
            RouteGraphic = RailGraphics->LinkSigRouteGraphicsPtr[0];
        }
        else if(TypeOfRoute == 0)
        {
            RouteGraphic = RailGraphics->LinkNonSigRouteGraphicsPtr[0];
        }
        else //manual - no route
        {
            RouteGraphic = BaseGraphic;
        }
// LinkSigRouteGraphicsPtr[0] hor  } pref dir
// LinkSigRouteGraphicsPtr[1] ver  }
// LinkNonSigRouteGraphicsPtr[0] hor  } non pref dir
// LinkNonSigRouteGraphicsPtr[1] ver  }

        if(UpStep == DownStep) // both 0, so just a single track, plot the double graphic, but plot solid bgnd first then track to get rid of earlier graphics
        {
            Disp->PlotOutput(132, HLoc * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
            Disp->PlotOutput(133, HLoc * 16, VLoc * 16, RouteGraphic);
            if(!Manual)
            {
                Disp->PlotOutput(134, HLoc * 16, VLoc * 16, RailGraphics->LCBothHor);
            }
            else
            {
                Disp->PlotOutput(247, HLoc * 16, VLoc * 16, RailGraphics->LCBothHorMan);
            }
        }
        else if((DownStep - UpStep) == 1) // double track, no need for any plain LC graphics
        {
            if(UpStep == 0)
            {
                Disp->PlotOutput(135, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(136, HLoc * 16, (VLoc + UpStep) * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(137, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHor);
                }
                else
                {
                    Disp->PlotOutput(248, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHorMan);
                }
                Disp->PlotOutput(138, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(139, HLoc * 16, (VLoc + DownStep) * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(140, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHor);
                }
                else
                {
                    Disp->PlotOutput(249, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHorMan);
                }
            }
            else
            {
                Disp->PlotOutput(195, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(196, HLoc * 16, (VLoc + UpStep) * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(197, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHor);
                }
                else
                {
                    Disp->PlotOutput(250, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHorMan);
                }
                Disp->PlotOutput(198, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(199, HLoc * 16, (VLoc + DownStep) * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(200, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHor);
                }
                else
                {
                    Disp->PlotOutput(251, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHorMan);
                }
            }
        }
        else // at least one plain graphic
        {
            if(UpStep == 0)
            {
                Disp->PlotOutput(141, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(142, HLoc * 16, (VLoc + UpStep) * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(143, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHor);
                }
                else
                {
                    Disp->PlotOutput(252, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHorMan);
                }
                Disp->PlotOutput(144, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(145, HLoc * 16, (VLoc + DownStep) * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(146, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHor);
                }
                else
                {
                    Disp->PlotOutput(253, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHorMan);
                }
            }
            else if(DownStep == 0)
            {
                Disp->PlotOutput(201, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(202, HLoc * 16, (VLoc + UpStep) * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(203, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHor);
                }
                else
                {
                    Disp->PlotOutput(254, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHorMan);
                }
                Disp->PlotOutput(204, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(205, HLoc * 16, (VLoc + DownStep) * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(206, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHor);
                }
                else
                {
                    Disp->PlotOutput(255, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHorMan);
                }
            }
            else
            {
                Disp->PlotOutput(207, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(208, HLoc * 16, (VLoc + UpStep) * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(209, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHor);
                }
                else
                {
                    Disp->PlotOutput(256, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHorMan);
                }
                Disp->PlotOutput(210, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(211, HLoc * 16, (VLoc + DownStep) * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(212, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHor);
                }
                else
                {
                    Disp->PlotOutput(257, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHorMan);
                }
            }
            for(int x = (UpStep + 1); x < DownStep; x++)
            {
                Disp->PlotOutput(147, HLoc * 16, (VLoc + x) * 16, RailGraphics->bmSolidBgnd);
                if(x == 0)
                {
                    Disp->PlotOutput(148, HLoc * 16, (VLoc + x) * 16, RouteGraphic);
                }
                else
                {
                    Disp->PlotOutput(213, HLoc * 16, (VLoc + x) * 16, BaseGraphic);
                }
                if(!Manual)
                {
                    Disp->PlotOutput(149, HLoc * 16, (VLoc + x) * 16, RailGraphics->LCPlain);
                }
                else
                {
                    Disp->PlotOutput(258, HLoc * 16, (VLoc + x) * 16, RailGraphics->LCPlainMan);
                }
            }
        }
        Disp->Update();
        Utilities->CallLogPop(1958);
        return;
    }

    else // ver track element
    {
        // find leftmost LC, opening them all (to trains) in turn
        int LStep = 0;
        while(IsLCAtHV(7, (HLoc + LStep), VLoc))
        {
            LStep--;
        }
        LStep++;
        // now find rightmost LC, opening them all (to trains) in turn
        int RStep = 1;
        while(IsLCAtHV(8, (HLoc + RStep), VLoc))
        {
            RStep++;
        }
        RStep--;
        // now plot graphics, LStep is smallest & RStep largest
        Graphics::TBitmap *RouteGraphic;
        Graphics::TBitmap *BaseGraphic = RailGraphics->gl2;
        if(TypeOfRoute == 1)
        {
            RouteGraphic = RailGraphics->LinkSigRouteGraphicsPtr[1];
        }
        else if(TypeOfRoute == 0)
        {
            RouteGraphic = RailGraphics->LinkNonSigRouteGraphicsPtr[1];
        }
        else //manual
        {
            RouteGraphic = BaseGraphic;
        }
// LinkSigRouteGraphicsPtr[0] hor  } pref dir
// LinkSigRouteGraphicsPtr[1] ver  }
// LinkNonSigRouteGraphicsPtr[0] hor  } non pref dir
// LinkNonSigRouteGraphicsPtr[1] ver  }
        if(LStep == RStep) // both 0, so just a single track, plot the double graphic, but plot solid bgnd first then track to get rid of earlier graphics
        {
            Disp->PlotOutput(150, HLoc * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
            Disp->PlotOutput(151, HLoc * 16, VLoc * 16, RouteGraphic);
            if(!Manual)
            {
                Disp->PlotOutput(152, HLoc * 16, VLoc * 16, RailGraphics->LCBothVer);
            }
            else
            {
                Disp->PlotOutput(259, HLoc * 16, VLoc * 16, RailGraphics->LCBothVerMan);
            }
        }
        else if((RStep - LStep) == 1) // double track, no need for any plain LC graphics
        {
            if(LStep == 0)
            {
                Disp->PlotOutput(153, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(154, (HLoc + LStep) * 16, VLoc * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(155, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVer);
                }
                else
                {
                    Disp->PlotOutput(260, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVerMan);
                }
                Disp->PlotOutput(156, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(157, (HLoc + RStep) * 16, VLoc * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(158, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVer);
                }
                else
                {
                    Disp->PlotOutput(261, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVerMan);
                }
            }
            else
            {
                Disp->PlotOutput(214, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(215, (HLoc + LStep) * 16, VLoc * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(216, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVer);
                }
                else
                {
                    Disp->PlotOutput(262, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVerMan);
                }
                Disp->PlotOutput(217, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(218, (HLoc + RStep) * 16, VLoc * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(219, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVer);
                }
                else
                {
                    Disp->PlotOutput(263, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVerMan);
                }
            }
        }
        else // at least one plain graphic
        {
            if(LStep == 0)
            {
                Disp->PlotOutput(159, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(160, (HLoc + LStep) * 16, VLoc * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(161, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVer);
                }
                else
                {
                    Disp->PlotOutput(264, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVerMan);
                }
                Disp->PlotOutput(162, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(163, (HLoc + RStep) * 16, VLoc * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(164, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVer);
                }
                else
                {
                    Disp->PlotOutput(265, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVerMan);
                }
            }
            else if(RStep == 0)
            {
                Disp->PlotOutput(220, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(221, (HLoc + LStep) * 16, VLoc * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(222, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVer);
                }
                else
                {
                    Disp->PlotOutput(266, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVerMan);
                }
                Disp->PlotOutput(223, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(224, (HLoc + RStep) * 16, VLoc * 16, RouteGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(225, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVer);
                }
                else
                {
                    Disp->PlotOutput(267, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVerMan);
                }
            }
            else
            {
                Disp->PlotOutput(226, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(227, (HLoc + LStep) * 16, VLoc * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(228, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVer);
                }
                else
                {
                    Disp->PlotOutput(268, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVerMan);
                }
                Disp->PlotOutput(229, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                Disp->PlotOutput(230, (HLoc + RStep) * 16, VLoc * 16, BaseGraphic);
                if(!Manual)
                {
                    Disp->PlotOutput(231, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVer);
                }
                else
                {
                    Disp->PlotOutput(269, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVerMan);
                }
            }
            for(int x = (LStep + 1); x < RStep; x++)
            {
                Disp->PlotOutput(165, (HLoc + x) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
                if(x == 0)
                {
                    Disp->PlotOutput(166, (HLoc + x) * 16, VLoc * 16, RouteGraphic);
                }
                else
                {
                    Disp->PlotOutput(232, (HLoc + x) * 16, VLoc * 16, BaseGraphic);
                }
                if(!Manual)
                {
                    Disp->PlotOutput(167, (HLoc + x) * 16, VLoc * 16, RailGraphics->LCPlain);
                }
                else
                {
                    Disp->PlotOutput(270, (HLoc + x) * 16, VLoc * 16, RailGraphics->LCPlainMan);
                }
            }
        }
        Disp->Update();
        Utilities->CallLogPop(1896);
        return;
    }
}

// ---------------------------------------------------------------------------

void TTrack::PlotPlainLoweredLinkedLevelCrossingBarriersAndSetMarkers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, TDisplay *Disp, bool Manual) // open to trains
// BaseElementSpeedTag: 1 = Horizontal track, 2 = vertical track
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotPlainLoweredLinkedLevelCrossingBarriersAndSetMarkers," +
                                 AnsiString(HLoc) + "," + AnsiString(VLoc));
    if(!IsLCAtHV(29, HLoc, VLoc))
    {
        throw Exception("Error, Wrong track type in PlotPlainLoweredLinkedLevelCrossingBarriersAndSetMarkers");
    }
    if((BaseElementSpeedTag != 1) && (BaseElementSpeedTag != 2))
    {
        throw Exception("Error, Wrong BaseElementSpeedTag value in PlotPlainLoweredLinkedLevelCrossingBarriersAndSetMarkers");
    }
// check for adjacent LCs & if so open (to trains)
    if(BaseElementSpeedTag == 1) // hor track element
    {
        // find topmost LC, opening them all (to trains) in turn
        int UpStep = 0;
        while(IsLCAtHV(30, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
        {
            UpStep--;
        }
        UpStep++;
        // now find bottommost LC, opening them all (to trains) in turn
        int DownStep = 1;
        while(IsLCAtHV(31, HLoc, (VLoc + DownStep)))
        {
            DownStep++;
        }
        DownStep--;
        // now plot graphics, UpStep is smallest & DownStep largest
        if(UpStep == DownStep) // both 0, so just a single track, plot the double graphic, but plot solid bgnd first then track to get rid of earlier graphics
        {
            if(!Manual)
            {
                Disp->PlotOutput(179, HLoc * 16, VLoc * 16, RailGraphics->LCBothHor);
            }
            else
            {
                Disp->PlotOutput(271, HLoc * 16, VLoc * 16, RailGraphics->LCBothHorMan);
            }
        }
        else if((DownStep - UpStep) == 1) // double track, no need for any plain LC graphics
        {
            if(!Manual)
            {
                Disp->PlotOutput(180, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHor);
                Disp->PlotOutput(181, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHor);
            }
            else
            {
                Disp->PlotOutput(272, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHorMan);
                Disp->PlotOutput(273, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHorMan);
            }
        }
        else // at least one plain graphic
        {
            if(!Manual)
            {
                Disp->PlotOutput(182, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHor);
                Disp->PlotOutput(183, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHor);
                for(int x = (UpStep + 1); x < DownStep; x++)
                {
                    Disp->PlotOutput(184, HLoc * 16, (VLoc + x) * 16, RailGraphics->LCPlain);
                }
            }
            else
            {
                Disp->PlotOutput(274, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->LCTopHorMan);
                Disp->PlotOutput(275, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->LCBotHorMan);
                for(int x = (UpStep + 1); x < DownStep; x++)
                {
                    Disp->PlotOutput(276, HLoc * 16, (VLoc + x) * 16, RailGraphics->LCPlainMan);
                }
            }
        }
        // set markers
        for(int x = UpStep; x <= DownStep; x++)
        {
            GetInactiveTrackElementFromTrackMap(3, HLoc, (VLoc + x)).LCPlotted = true; // plotted
        }
        Display->Update();
        Utilities->CallLogPop(1944);
        return;
    }

    else // ver track element
    {
        // find leftmost LC, opening them all (to trains) in turn
        int LStep = 0;
        while(IsLCAtHV(32, (HLoc + LStep), VLoc))
        {
            LStep--;
        }
        LStep++;
        // now find rightmost LC, opening them all (to trains) in turn
        int RStep = 1;
        while(IsLCAtHV(33, (HLoc + RStep), VLoc))
        {
            RStep++;
        }
        RStep--;
        // now plot graphics, LStep is smallest & RStep largest
        if(LStep == RStep) // both 0, so just a single track, plot the double graphic, but plot solid bgnd first then track to get rid of earlier graphics
        {
            if(!Manual)
            {
                Disp->PlotOutput(185, HLoc * 16, VLoc * 16, RailGraphics->LCBothVer);
            }
            else
            {
                Disp->PlotOutput(277, HLoc * 16, VLoc * 16, RailGraphics->LCBothVerMan);
            }
        }
        else if((RStep - LStep) == 1) // double track, no need for any plain LC graphics
        {
            if(!Manual)
            {
                Disp->PlotOutput(186, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVer);
                Disp->PlotOutput(187, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVer);
            }
            else
            {
                Disp->PlotOutput(278, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVerMan);
                Disp->PlotOutput(279, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVerMan);
            }
        }
        else // at least one plain graphic
        {
            if(!Manual)
            {
                Disp->PlotOutput(188, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVer);
                Disp->PlotOutput(189, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVer);
                for(int x = (LStep + 1); x < RStep; x++)
                {
                    Disp->PlotOutput(190, (HLoc + x) * 16, VLoc * 16, RailGraphics->LCPlain);
                }
            }
            else
            {
                Disp->PlotOutput(280, (HLoc + LStep) * 16, VLoc * 16, RailGraphics->LCLHSVerMan);
                Disp->PlotOutput(281, (HLoc + RStep) * 16, VLoc * 16, RailGraphics->LCRHSVerMan);
                for(int x = (LStep + 1); x < RStep; x++)
                {
                    Disp->PlotOutput(282, (HLoc + x) * 16, VLoc * 16, RailGraphics->LCPlainMan);
                }
            }
        }
        // set markers
        for(int x = LStep; x <= RStep; x++)
        {
            GetInactiveTrackElementFromTrackMap(4, (HLoc + x), VLoc).LCPlotted = true; // plotted
        }
        Disp->Update();
        Utilities->CallLogPop(1945);
        return;
    }
}

// ---------------------------------------------------------------------------

void TTrack::PlotRaisedLinkedLevelCrossingBarriers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, TDisplay *Disp) // closed to trains
// BaseElementSpeedTag: 1 = Horizontal track, 2 = vertical track
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotRaisedLinkedLevelCrossingBarriers," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    if(!IsLCAtHV(9, HLoc, VLoc))
    {
        throw Exception("Error, Wrong track type in PlotAndRaiseLevelCrossingBarriers");
    }
    if((BaseElementSpeedTag != 1) && (BaseElementSpeedTag != 2))
    {
        throw Exception("Error, Wrong BaseElementSpeedTag value in PlotAndRaiseLevelCrossingBarriers");
    }
// check for adjacent LCs & if so close (to trains)
    if(BaseElementSpeedTag == 1) // hor track element
    {
        // find topmost LC, closing them all (to trains) in turn
        int UpStep = 0;
        while(IsLCAtHV(10, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
        {
            UpStep--;
        }
        UpStep++;
        // now find bottommost LC, opening them all (to trains) in turn
        int DownStep = 1;
        while(IsLCAtHV(11, HLoc, (VLoc + DownStep)))
        {
            DownStep++;
        }
        DownStep--;
        // now plot graphics, UpStep is smallest & DownStep largest
        for(int x = UpStep; x < (DownStep + 1); x++)
        {
            Disp->PlotOutput(168, HLoc * 16, (VLoc + x) * 16, RailGraphics->bmSolidBgnd);
            Disp->PlotOutput(169, HLoc * 16, (VLoc + x) * 16, RailGraphics->gl1);
            Disp->PlotOutput(170, HLoc * 16, (VLoc + x) * 16, RailGraphics->LCBothVer);
        }
        Disp->Update();
        Utilities->CallLogPop(1959);
        return;
    }

    else // ver track element
    {
        // find leftmost LC, closing them all (to trains) in turn
        int LStep = 0;
        while(IsLCAtHV(12, (HLoc + LStep), VLoc))
        {
            LStep--;
        }
        LStep++;
        // now find rightmost LC, opening them all (to trains) in turn
        int RStep = 1;
        while(IsLCAtHV(13, (HLoc + RStep), VLoc))
        {
            RStep++;
        }
        RStep--;
        // now plot graphics, LStep is smallest & RStep largest
        for(int x = LStep; x < (RStep + 1); x++)
        {
            Disp->PlotOutput(171, (HLoc + x) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
            Disp->PlotOutput(172, (HLoc + x) * 16, VLoc * 16, RailGraphics->gl2);
            Disp->PlotOutput(173, (HLoc + x) * 16, VLoc * 16, RailGraphics->LCBothHor);
        }
        Disp->Update();
        Utilities->CallLogPop(1960);
        return;
    }
}

// ---------------------------------------------------------------------------

void TTrack::PlotPlainRaisedLinkedLevelCrossingBarriersAndSetMarkers(int Caller, int BaseElementSpeedTag, int HLoc, int VLoc, TDisplay *Disp)
// closed to trains
// BaseElementSpeedTag: 1 = Horizontal track, 2 = vertical track
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotPlainRaisedLinkedLevelCrossingBarriersAndSetMarkers," +
                                 AnsiString(HLoc) + "," + AnsiString(VLoc));
    if(!IsLCAtHV(34, HLoc, VLoc))
    {
        throw Exception("Error, Wrong track type in PlotPlainRaisedLinkedLevelCrossingBarriersAndSetMarkers");
    }
    if((BaseElementSpeedTag != 1) && (BaseElementSpeedTag != 2))
    {
        throw Exception("Error, Wrong BaseElementSpeedTag value in PlotPlainRaisedLinkedLevelCrossingBarriersAndSetMarkers");
    }
    TTrackElement TE;

// check for adjacent LCs & if so close (to trains)
    if(BaseElementSpeedTag == 1) // hor track element
    {
        // find topmost LC, closing them all (to trains) in turn
        int UpStep = 0;
        while(IsLCAtHV(35, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
        {
            UpStep--;
        }
        UpStep++;
        // now find bottommost LC, opening them all (to trains) in turn
        int DownStep = 1;
        while(IsLCAtHV(36, HLoc, (VLoc + DownStep)))
        {
            DownStep++;
        }
        DownStep--;
        // now plot graphics, UpStep is smallest & DownStep largest
        for(int x = UpStep; x <= DownStep; x++)
        {
            Disp->PlotOutput(191, HLoc * 16, (VLoc + x) * 16, RailGraphics->LCBothVer);
            GetInactiveTrackElementFromTrackMap(1, HLoc, (VLoc + x)).LCPlotted = true; // plotted
        }
        Display->Update();
        Utilities->CallLogPop(1946);
        return;
    }

    else // ver track element
    {
        // find leftmost LC, closing them all (to trains) in turn
        int LStep = 0;
        while(IsLCAtHV(37, (HLoc + LStep), VLoc))
        {
            LStep--;
        }
        LStep++;
        // now find rightmost LC, opening them all (to trains) in turn
        int RStep = 1;
        while(IsLCAtHV(38, (HLoc + RStep), VLoc))
        {
            RStep++;
        }
        RStep--;
        // now plot graphics, LStep is smallest & RStep largest
        for(int x = LStep; x <= RStep; x++)
        {
            Disp->PlotOutput(192, (HLoc + x) * 16, VLoc * 16, RailGraphics->LCBothHor);
            GetInactiveTrackElementFromTrackMap(2, (HLoc + x), VLoc).LCPlotted = true; // plotted
        }
        Display->Update();
        Utilities->CallLogPop(1947);
        return;
    }
}

// ---------------------------------------------------------------------------

void TTrack::PlotLCBaseElementsOnly(int Caller, TBarrierState State, int BaseElementSpeedTag, int HLoc, int VLoc, int TypeOfRoute, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotBaseElementsOnly," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    Graphics::TBitmap *RouteGraphic;
    Graphics::TBitmap *BaseGraphic = RailGraphics->gl1;

    if(BaseElementSpeedTag == 1)
    {
        if(TypeOfRoute == 1)
        {
            RouteGraphic = RailGraphics->LinkSigRouteGraphicsPtr[0];
        }
        else if(TypeOfRoute == 0)
        {
            RouteGraphic = RailGraphics->LinkNonSigRouteGraphicsPtr[0];
        }
        else //manual
        {
            RouteGraphic = BaseGraphic;
        }
        if(State == Raising)
        {
            RouteGraphic = BaseGraphic;
        }
    }
    else
    {
        BaseGraphic = RailGraphics->gl2;
        if(TypeOfRoute == 1)
        {
            RouteGraphic = RailGraphics->LinkSigRouteGraphicsPtr[1];
        }
        else if(TypeOfRoute == 0)
        {
            RouteGraphic = RailGraphics->LinkNonSigRouteGraphicsPtr[1];
        }
        else
        {
            RouteGraphic = BaseGraphic; //manual
        }
        if(State == Raising)
        {
            RouteGraphic = BaseGraphic;
        }
    }
    int UpStep = 0;

    while(IsLCAtHV(14, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
    {
        Disp->PlotOutput(174, HLoc * 16, (VLoc + UpStep) * 16, RailGraphics->bmSolidBgnd);
        if(UpStep == 0)
        {
            Disp->PlotOutput(175, HLoc * 16, (VLoc + UpStep) * 16, RouteGraphic);
        }
        else
        {
            Disp->PlotOutput(234, HLoc * 16, (VLoc + UpStep) * 16, BaseGraphic);
        }
        UpStep--;
    }
// now find bottommost LC, opening them all (to trains) in turn
    int DownStep = 1;

    while(IsLCAtHV(15, HLoc, (VLoc + DownStep)))
    {
        Disp->PlotOutput(176, HLoc * 16, (VLoc + DownStep) * 16, RailGraphics->bmSolidBgnd);
        Disp->PlotOutput(177, HLoc * 16, (VLoc + DownStep) * 16, BaseGraphic);
        DownStep++;
    }
    int LeftStep = 0;

    while(IsLCAtHV(16, (HLoc + LeftStep), VLoc)) // will always find LC at LeftStep == 0
    {
        Disp->PlotOutput(233, (HLoc + LeftStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
        if(LeftStep == 0)
        {
            Disp->PlotOutput(235, (HLoc + LeftStep) * 16, VLoc * 16, RouteGraphic);
        }
        else
        {
            Disp->PlotOutput(236, (HLoc + LeftStep) * 16, VLoc * 16, BaseGraphic);
        }
        LeftStep--;
    }
// now find rightmost LC, opening them all (to trains) in turn
    int RightStep = 1;

    while(IsLCAtHV(17, (HLoc + RightStep), VLoc))
    {
        Disp->PlotOutput(237, (HLoc + RightStep) * 16, VLoc * 16, RailGraphics->bmSolidBgnd);
        Disp->PlotOutput(238, (HLoc + RightStep) * 16, VLoc * 16, BaseGraphic);
        RightStep++;
    }
    Disp->Update();
    Utilities->CallLogPop(1914);
}

// ---------------------------------------------------------------------------

bool TTrack::IsLCBarrierDownAtHV(int Caller, int HLoc, int VLoc) // returns true only if fully down
{
// return false for no LC there, flashing or a closed (to trains) LC
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsLCBarrierDownAtHV," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(21, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(1898);
        return(false);
    }
    if(InactiveTrackElementAt(100, IMPair.first).TrackType != LevelCrossing) // only need to check first as second can only be a platform
    {
        Utilities->CallLogPop(1899);
        return(false);
    }
    if(InactiveTrackElementAt(103, IMPair.first).Attribute == 1)
    {
        Utilities->CallLogPop(1900);
        return(true);
    }
    Utilities->CallLogPop(1901);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::IsLCBarrierUpAtHV(int Caller, int HLoc, int VLoc) // returns true only if fully up
{
// return false for no LC there, flashing LC or open (to trains) LC
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsBarrierUpLCAtHV," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(24, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(1922);
        return(false);
    }
    if(InactiveTrackElementAt(110, IMPair.first).TrackType != LevelCrossing) // only need to check first as second can only be a platform
    {
        Utilities->CallLogPop(1923);
        return(false);
    }
    if(InactiveTrackElementAt(111, IMPair.first).Attribute == 0)
    {
        Utilities->CallLogPop(1924);
        return(true);
    }
    Utilities->CallLogPop(1925);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::IsLCBarrierFlashingAtHV(int Caller, int HLoc, int VLoc)
{
// return true for barrier in process of moving
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsBarrierFlashingAtHV," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(25, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(1918);
        return(false);
    }
    if(InactiveTrackElementAt(112, IMPair.first).TrackType != LevelCrossing) // only need to check first as second can only be a platform
    {
        Utilities->CallLogPop(1919);
        return(false);
    }
    if(InactiveTrackElementAt(113, IMPair.first).Attribute == 2)
    {
        Utilities->CallLogPop(1920);
        return(true);
    }
    Utilities->CallLogPop(1921);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::IsLCAtHV(int Caller, int HLoc, int VLoc)
{
// return true for an LC at H&V
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsLCAtHV," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(22, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(1902);
        return(false);
    }
    if(InactiveTrackElementAt(101, IMPair.first).TrackType != LevelCrossing) // only need to check first as second can only be a platform
    {
        Utilities->CallLogPop(1903);
        return(false);
    }
    Utilities->CallLogPop(1904);
    return(true);
}

// ---------------------------------------------------------------------------

void TTrack::SetLCAttributeAtHV(int Caller, int HLoc, int VLoc, int Attr)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetLCAttributeAtHV," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(Attr));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(23, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        throw Exception("Element not found in LowerLCBarriersAtHV " + AnsiString(HLoc) + "," + AnsiString(VLoc));
    }
    if(InactiveTrackElementAt(102, IMPair.first).TrackType != LevelCrossing) // only need to check first as second can only be a platform
    {
        throw Exception("Element not a level crossing in LowerLCBarriersAtHV " + AnsiString(HLoc) + "," + AnsiString(VLoc));
    }
    InactiveTrackElementAt(104, IMPair.first).Attribute = Attr;
    Utilities->CallLogPop(1905);
    return;
}

// ---------------------------------------------------------------------------

void TTrack::ResetLevelCrossings(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetLevelCrossings");
    for(unsigned int x = 0; x < InactiveTrackVector.size(); x++)
    {
        TTrackElement InactiveTrackElement = InactiveTrackElementAt(140, x);
        if(InactiveTrackElement.TrackType == LevelCrossing)
        {
            InactiveTrackElementAt(141, x).Attribute = 0;
            // though this only resets the attributes the LC will display correctly when call Clearand.. in BaseMode
        }
    }
    Utilities->CallLogPop(1913);
    return;
}

// ---------------------------------------------------------------------------

bool TTrack::AnyLinkedLevelCrossingElementsWithRoutesOrTrains(int Caller, int HLoc, int VLoc, TPrefDirVector SearchVector, bool &TrainPresent)
{
// return true if there is either a route set or being set on any element or a train on any element
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AnyLinkedLevelCrossingElementsWithRoutesOrTrains," + AnsiString(HLoc) +
                                 "," + AnsiString(VLoc));

    THVPair TrackMapKeyPair;
    TTrack::TTrackMapIterator TrackMapPtr;
    int DummyRouteNumber;

    TrainPresent = false;
// find topmost LC, checking each for routes & trains
    int UpStep = 0;

    while(IsLCAtHV(25, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
    {
        TrackMapKeyPair.first = HLoc;
        TrackMapKeyPair.second = VLoc + UpStep;
        TrackMapPtr = TrackMap.find(TrackMapKeyPair);
        if(AllRoutes->GetRouteTypeAndNumber(20, TrackMapPtr->second, 0, DummyRouteNumber) != TAllRoutes::NoRoute)
        // use 0 for LinkPos, only 1 track element so 0 or 1 would be OK
        {
            Utilities->CallLogPop(1932);
            return(true);
        }
        if(TrackElementAt(867, TrackMapPtr->second).TrainIDOnElement != -1)
        {
            TrainPresent = true;
            Utilities->CallLogPop(1933);
            return(true);
        }
        if(LCInSearchVector(0, HLoc, (VLoc + UpStep), SearchVector)) //route being set, added at v2.8.0
        {
            Utilities->CallLogPop(2274);
            return(true);
        }
        UpStep--;
    }
// now find bottommost LC, opening them all (to trains) in turn
    int DownStep = 1;

    while(IsLCAtHV(26, HLoc, (VLoc + DownStep)))
    {
        TrackMapKeyPair.first = HLoc;
        TrackMapKeyPair.second = VLoc + DownStep;
        TrackMapPtr = TrackMap.find(TrackMapKeyPair);
        if(AllRoutes->GetRouteTypeAndNumber(21, TrackMapPtr->second, 0, DummyRouteNumber) != TAllRoutes::NoRoute)
        // use 0 for LinkPos, only 1 track element so 0 or 1 would be OK
        {
            Utilities->CallLogPop(1934);
            return(true);
        }
        if(TrackElementAt(868, TrackMapPtr->second).TrainIDOnElement != -1)
        {
            TrainPresent = true;
            Utilities->CallLogPop(1935);
            return(true);
        }
        if(LCInSearchVector(1, HLoc, (VLoc + DownStep), SearchVector)) //route being set, added at v2.8.0
        {
            Utilities->CallLogPop(2275);
            return(true);
        }
        DownStep++;
    }
// find leftmost LC
    int LeftStep = 0;

    while(IsLCAtHV(27, (HLoc + LeftStep), VLoc)) // will always find LC at LeftStep == 0
    {
        TrackMapKeyPair.first = HLoc + LeftStep;
        TrackMapKeyPair.second = VLoc;
        TrackMapPtr = TrackMap.find(TrackMapKeyPair);
        if(AllRoutes->GetRouteTypeAndNumber(22, TrackMapPtr->second, 0, DummyRouteNumber) != TAllRoutes::NoRoute)
        // use 0 for LinkPos, only 1 track element so 0 or 1 would be OK
        {
            Utilities->CallLogPop(1936);
            return(true);
        }
        if(TrackElementAt(869, TrackMapPtr->second).TrainIDOnElement != -1)
        {
            TrainPresent = true;
            Utilities->CallLogPop(1937);
            return(true);
        }
        if(LCInSearchVector(2, (HLoc + LeftStep), VLoc, SearchVector)) //route being set, added at v2.8.0
        {
            Utilities->CallLogPop(2276);
            return(true);
        }
        LeftStep--;
    }
// now find rightmost LC, opening them all (to trains) in turn
    int RightStep = 1;

    while(IsLCAtHV(28, (HLoc + RightStep), VLoc))
    {
        TrackMapKeyPair.first = HLoc + RightStep;
        TrackMapKeyPair.second = VLoc;
        TrackMapPtr = TrackMap.find(TrackMapKeyPair);
        if(AllRoutes->GetRouteTypeAndNumber(23, TrackMapPtr->second, 0, DummyRouteNumber) != TAllRoutes::NoRoute)
        // use 0 for LinkPos, only 1 track element so 0 or 1 would be OK
        {
            Utilities->CallLogPop(1938);
            return(true);
        }
        if(TrackElementAt(870, TrackMapPtr->second).TrainIDOnElement != -1)
        {
            TrainPresent = true;
            Utilities->CallLogPop(1939);
            return(true);
        }
        if(LCInSearchVector(3, (HLoc + RightStep), VLoc, SearchVector)) //route being set, added at v2.8.0
        {
            Utilities->CallLogPop(2277);
            return(true);
        }
        RightStep++;
    }
    Utilities->CallLogPop(1940);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::LCInSearchVector(int Caller, int HLoc, int VLoc, TPrefDirVector SearchVector)  //added at v2.8.0
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LCInSearchVector," + HLoc + "," + VLoc);
    for(unsigned int x = 0; x < SearchVector.size(); x++)
    {
        if((TrackElementAt(1019, SearchVector.at(x).GetTrackVectorPosition()).HLoc == HLoc) && (TrackElementAt(1020, SearchVector.at(x).GetTrackVectorPosition()).VLoc == VLoc))
        {
            Utilities->CallLogPop(2278);
            return(true);
        }
    }
    Utilities->CallLogPop(2279);
    return(false);
}

// ---------------------------------------------------------------------------

void TTrack::PlotSmallFlashingLinkedLevelCrossings(int Caller, int HLoc, int VLoc, Graphics::TBitmap *GraphicPtr, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotSmallFlashingLinkedLevelCrossings," +
                                 AnsiString(HLoc) + "," + AnsiString(VLoc));
    if(!IsLCAtHV(60, HLoc, VLoc))
    {
        throw Exception("PlotSmallFlashingLinkedLevelCrossings");
    }

// check for adjacent LCs
    // find topmost LC
    int UpStep = 0;
    while(IsLCAtHV(61, HLoc, (VLoc + UpStep))) // will always find LC at UpStep == 0
    {
        UpStep--;
    }
    UpStep++;
    // now find bottommost LC, opening them all (to trains) in turn
    int DownStep = 1;
    while(IsLCAtHV(62, HLoc, (VLoc + DownStep)))
    {
        DownStep++;
    }
    DownStep--;
    // now plot graphics, UpStep is smallest & DownStep largest
    for(int x = UpStep; x <= DownStep; x++)
    {
        Disp->PlotSmallOutput(24, HLoc * 4, (VLoc + x) * 4, GraphicPtr);
    }

    // find leftmost LC, closing them all (to trains) in turn
    int LStep = 0;
    while(IsLCAtHV(63, (HLoc + LStep), VLoc))
    {
        LStep--;
    }
    LStep++;
    // now find rightmost LC, opening them all (to trains) in turn
    int RStep = 1;
    while(IsLCAtHV(64, (HLoc + RStep), VLoc))
    {
        RStep++;
    }
    RStep--;
    // now plot graphics, LStep is smallest & RStep largest
    for(int x = LStep; x <= RStep; x++)
    {
        Disp->PlotSmallOutput(25, (HLoc + x) * 4, VLoc * 4, GraphicPtr);
    }
    Display->Update();
    Utilities->CallLogPop(2315);
    return;
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TTrack::GetFilletGraphic(int Caller, TTrackElement TrackElement)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetFilletGraphic," + TrackElement.LogTrack(4));
    if(TrackElement.TrackType != Points)
    {
        throw Exception("Error, Wrong track type in GetFilletGraphic");
    }
    if(TrackElement.SpeedTag < 28)
    {
        Utilities->CallLogPop(521);
        return(RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 7][TrackElement.Attribute]);
    }
    else if(TrackElement.SpeedTag < 132)
    {
        Utilities->CallLogPop(522);
// Utilities->CallLogPop callers 523 to 534 inc used to test pop failure
        return(RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 20][TrackElement.Attribute]);
    }
    else
    {
        Utilities->CallLogPop(1537);
        return(RailGraphics->PointModeGraphicsPtr[TrackElement.SpeedTag - 108][TrackElement.Attribute]);
    }
}

// ---------------------------------------------------------------------------

void TTrack::ResetAllTrainIDsAndFailedPointOrigSpeedLimits(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ResetAllTrainIDsAndFailedPointOrigSpeedLimits");
    for(unsigned int x = 0; x < TrackVector.size(); x++) // check all elements in turn
    {
        TrackElementAt(1351, x).TrainIDOnElement = -1;
        TrackElementAt(1352, x).TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 = -1;
        TrackElementAt(1353, x).TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 = -1;
    }
    Utilities->CallLogPop(1342);
}

// ---------------------------------------------------------------------------

void TTrack::GetTrackLocsFromScreenPos(int Caller, int &HLoc, int &VLoc, int ScreenPosH, int ScreenPosV)
/*
      Converts the screen position to the true (without offsets) HLoc, VLoc 16 x 16 square that the screen position lies within
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTrackLocsFromScreenPos," + AnsiString(ScreenPosH) + "," +
                                 AnsiString(ScreenPosV));
    HLoc = div(ScreenPosH, 16).quot + Display->DisplayOffsetH;
    VLoc = div(ScreenPosV, 16).quot + Display->DisplayOffsetV;
// Utilities->CallLogPop callers 523 to 534 inc used to test pop failure
    Utilities->CallLogPop(535);
}

// ---------------------------------------------------------------------------

void TTrack::GetTruePositionsFromScreenPos(int Caller, int &HPos, int &VPos, int ScreenPosH, int ScreenPosV)
/*
      Converts the screen position to the true (without offsets) position
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTruePositionsFromScreenPos," + AnsiString(ScreenPosH) + "," +
                                 AnsiString(ScreenPosV));
    HPos = ScreenPosH + (Display->DisplayOffsetH * 16);
    VPos = ScreenPosV + (Display->DisplayOffsetV * 16);
    Utilities->CallLogPop(536);
}

// ---------------------------------------------------------------------------

void TTrack::GetScreenPositionsFromTruePos(int Caller, int &ScreenPosH, int &ScreenPosV, int HPosTrue, int VPosTrue)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetScreenPositionsFromTruePos," + AnsiString(HPosTrue) + "," +
                                 AnsiString(VPosTrue));
    ScreenPosH = HPosTrue - (Display->DisplayOffsetH * 16);
    ScreenPosV = VPosTrue - (Display->DisplayOffsetV * 16);
    Utilities->CallLogPop(537);
}

// ---------------------------------------------------------------------------

void TTrack::CheckMapAndTrack(int Caller) // test
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckMapAndTrack");
    int Zeroes = 0;
    bool FoundFlag;

    for(unsigned int a = 0; a < TrackVector.size(); a++)
    {
        TTrackElement CheckElement = Track->TrackElementAt(1354, a);
        if(CheckElement.SpeedTag == 0)
        {
            Zeroes++; // zeroed elements not saved in map
        }
        else
        {
            int MapVecPos = GetVectorPositionFromTrackMap(16, CheckElement.HLoc, CheckElement.VLoc, FoundFlag);
            if(!FoundFlag)
            {
                throw Exception("CheckMapAndTrack Error - failed to find HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" + (AnsiString)CheckElement.VLoc +
                                " in TrackMap, Caller=" + (AnsiString)Caller);
            }
            if(MapVecPos != (int)a)
            {
                throw Exception("CheckMapAndTrack Error - MapVectorPosition failed at HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" +
                                (AnsiString)CheckElement.VLoc + " Map value=" + (AnsiString)MapVecPos + " TrackVectorPos value=" + (AnsiString)a + " Caller=" +
                                (AnsiString)Caller);
            }
        }
    }
    if(TrackVector.size() != (TrackMap.size() + Zeroes))
    {
        throw Exception("CheckMapAndTrack Error - Map Size=" + (AnsiString)TrackVector.size() + " TrackVectorSize=" + (AnsiString)TrackVector.size() +
                        " Caller=" + (AnsiString)Caller);
    }
    Utilities->CallLogPop(538);
    return;
}

// ---------------------------------------------------------------------------

void TTrack::CheckMapAndInactiveTrack(int Caller) // test
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckMapAndInactiveTrack");
    bool FoundFlag;
    TIMPair InactivePair;

    for(unsigned int a = 0; a < InactiveTrackVector.size(); a++)
    {
        TTrackElement CheckElement = Track->InactiveTrackElementAt(142, a);
        InactivePair = GetVectorPositionsFromInactiveTrackMap(7, CheckElement.HLoc, CheckElement.VLoc, FoundFlag);
        if(!FoundFlag)
        {
            throw Exception("CheckMapAndInactiveTrack Error - failed to find HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" + (AnsiString)CheckElement.VLoc +
                            " in InactiveMap, Caller=" + (AnsiString)Caller);
        }
        if((InactivePair.first != a) && (InactivePair.second != a))
        {
            throw Exception("CheckMapAndInactiveTrack Error - InactiveMapVectorPosition failed at HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" +
                            (AnsiString)CheckElement.VLoc + " Inactive Map values=" + (AnsiString)InactivePair.first + " and " + (AnsiString)InactivePair.second +
                            " InactiveTrackVectorPos value=" + (AnsiString)a + " Caller=" + (AnsiString)Caller);
        }
    }
    if(InactiveTrackVector.size() != InactiveTrack2MultiMap.size())
    {
        throw Exception("CheckMapAndInactiveTrack Error - Map Size=" + (AnsiString)TrackVector.size() + " TrackVectorSize=" + (AnsiString)TrackVector.size() +
                        " Caller=" + (AnsiString)Caller);
    }
    Utilities->CallLogPop(539);
}

// ---------------------------------------------------------------------------

void TTrack::CheckGapMap(int Caller) // test
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckGapMap");
    int Position1, Position2;
    TTrackElement TrackElement1, TrackElement2;
    TGapMapIterator GapMapPtr;

    if(!GapMap.empty())
    {
        for(GapMapPtr = GapMap.begin(); GapMapPtr != GapMap.end(); GapMapPtr++)
        {
            int HLoc1 = GapMapPtr->first.first;
            int VLoc1 = GapMapPtr->first.second;
            int HLoc2 = GapMapPtr->second.first;
            int VLoc2 = GapMapPtr->second.second;
            if(!FindNonPlatformMatch(14, HLoc1, VLoc1, Position1, TrackElement1))
            {
                throw Exception("Failed to find H & V for gap1, GapMap in error");
            }
            if(!FindNonPlatformMatch(15, HLoc2, VLoc2, Position2, TrackElement2))
            {
                throw Exception("Failed to find H & V for gap2, GapMap in error");
            }
            if(TrackElementAt(17, Position1).TrackType != GapJump)
            {
                throw Exception("Element at Pos1 not a gap, GapMap in error");
            }
            if(TrackElementAt(18, Position2).TrackType != GapJump)
            {
                throw Exception("Element at Pos2 not a gap, GapMap in error");
            }
        }
    }
    unsigned int GapCount = 0;

    for(unsigned int a = 0; a < TrackVector.size(); a++)
    {
        TTrackElement CheckElement = Track->TrackElementAt(1355, a);
        if(CheckElement.TrackType == GapJump)
        {
            GapCount++;
        }
    }
    if((GapMap.size() * 2) != GapCount)
    {
        throw Exception("GapMap Error - Map Size * 2 =" + (AnsiString)(GapMap.size() * 2) + " GapCount=" + (AnsiString)GapCount + " Caller=" +
                        (AnsiString)Caller);
    }
    Utilities->CallLogPop(540);
}

// ---------------------------------------------------------------------------

void TTrack::SetElementID(int Caller, TTrackElement &TrackElement)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetElementID," + TrackElement.LogTrack(5));
    if((TrackElement.HLoc == -2000000000) || (TrackElement.VLoc == -2000000000))
    {
        if(TrackFinished)
        {
            throw Exception("Error - TrackFinished with erase element still present");
        }
        Utilities->CallLogPop(541);
        return; // erased element, can't set ID
    }
    AnsiString IDString;

    if(TrackElement.HLoc < 0)
    {
        IDString = "N" + AnsiString(abs(TrackElement.HLoc)) + "-";
    }
    else
    {
        IDString = AnsiString(TrackElement.HLoc) + "-";
    }
    if(TrackElement.VLoc < 0)
    {
        IDString += "N" + AnsiString(abs(TrackElement.VLoc));
    }
    else
    {
        IDString += AnsiString(TrackElement.VLoc);
    }
    TrackElement.ElementID = IDString;
    Utilities->CallLogPop(542);
}

// ---------------------------------------------------------------------------

int TTrack::GetTrackVectorPositionFromString(int Caller, AnsiString String, bool GiveMessages)
{
// e.g. "8-13", "00008-13", "N43-N127", etc
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTrackVectorPositionFromString," + String);  //String was inside " marks, corrected at v2.13.0
    int DelimPos;
try    //try..catch added at v2.13.0 following Amon Sadler's error file received on 24/03/22, had N-113-5 rather than N113-5
    {
        for(int x = 1; x < String.Length() + 1; x++)
        {
            if(String.IsDelimiter("-", x))
            {
                DelimPos = x;
                break;
            }
            if(x == String.Length())
            {
                if(GiveMessages)
                {
                    ShowMessage("Error in track element identifier: <" + String + "> - no delimiter");
                }
                Utilities->CallLogPop(543);
                return(-1);
            }
        }
        if(DelimPos == 1)
        {
            if(GiveMessages)
            {
                ShowMessage("Error in track element identifier: <" + String + "> - No Horizontal value");
            }
            Utilities->CallLogPop(544);
            return(-1);
        }
        if(DelimPos == String.Length())
        {
            if(GiveMessages)
            {
                ShowMessage("Error in track element identifier <" + String + "> - No Vertical value");
            }
            Utilities->CallLogPop(545);
            return(-1);
        }
        if((String[String.Length()] < '0') || (String[String.Length()] > '9'))
        {
            if(GiveMessages)
            {
                ShowMessage("Error in track element identifier <" + String + "> - Last value is not a number");
            }
            Utilities->CallLogPop(1508);
            return(-1);
        }
        int HLoc, VLoc;

        if(String.SubString(1, 1) != "N")
        {
            for(int x = 1; x < DelimPos; x++)
            {
                if((String.SubString(x, 1) < "0") || (String.SubString(x, 1) > "9"))
                {
                    if(GiveMessages)
                    {
                        ShowMessage("Invalid character in Horizontal value in track element identifier: <" + String + ">");
                    }
                    Utilities->CallLogPop(546);
                    return(-1);
                }
            }
        }
        if(String.SubString(1, 1) == "N")
        {
            for(int x = 2; x < DelimPos; x++)
            {
                if((String.SubString(x, 1) < "0") || (String.SubString(x, 1) > "9"))
                {
                    if(GiveMessages)
                    {
                        ShowMessage("Invalid character in Horizontal value in track element identifier: <" + String + ">");
                    }
                    Utilities->CallLogPop(763);
                    return(-1);
                }
            }
        }
        if(String.SubString(1, 1) == "N")
        {
            HLoc = -(String.SubString(2, DelimPos - 2).ToInt());
        }
        else
        {
            HLoc = String.SubString(1, DelimPos - 1).ToInt();
        }
        if(String.SubString(DelimPos + 1, 1) != "N")
        {
            for(int x = DelimPos + 1; x < String.Length() + 1; x++)
            {
                if((String.SubString(x, 1) < "0") || (String.SubString(x, 1) > "9"))
                {
                    if(GiveMessages)
                    {
                        ShowMessage("Invalid character in Vertical value in track element identifier: <" + String + ">");
                    }
                    Utilities->CallLogPop(547);
                    return(-1);
                }
            }
        }
        if(String.SubString(DelimPos + 1, 1) == "N")
        {
            for(int x = DelimPos + 2; x < String.Length() + 1; x++)
            {
                if((String.SubString(x, 1) < "0") || (String.SubString(x, 1) > "9"))
                {
                    if(GiveMessages)
                    {
                        ShowMessage("Invalid character in Vertical value in track element identifier: <" + String + ">");
                    }
                    Utilities->CallLogPop(764);
                    return(-1);
                }
            }
        }
        if(String.SubString(DelimPos + 1, 1) == "N")
        {
            VLoc = -(String.SubString(DelimPos + 2, String.Length() - DelimPos - 1).ToInt());
        }
        else
        {
            VLoc = String.SubString(DelimPos + 1, String.Length() - DelimPos).ToInt();
        }
        THVPair HVPair(HLoc, VLoc);
        TTrackMapIterator TrackMapPtr;

        TrackMapPtr = TrackMap.find(HVPair);
        if(TrackMapPtr == TrackMap.end())
        {
            if(GiveMessages)
            {
                ShowMessage("No track element corresponding to track element identifier: <" + String + ">");
            }
            Utilities->CallLogPop(548);
            return(-1);
        }
        Utilities->CallLogPop(549);
        return(TrackMapPtr->second);
    }
    catch(const Exception &e) //non-error catch - catches any  errors not already caught above
                             //(added at v2.13.0 following Amon Sadler's error file received on 24/03/22), had N-113-5 rather than N113-5
    {
        ShowMessage("Syntax error in track element identifier: <" + String + ">");
        Utilities->CallLogPop(2481);
        return(-1);
    }
}

// ---------------------------------------------------------------------------

bool TTrack::CheckFootCrossingLinks(int Caller, TTrackElement &TrackElement)
/*
      True for linked properly at both ends
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckFootCrossingLinks," + AnsiString(TrackElement.HLoc) + "," +
                                 AnsiString(TrackElement.VLoc) + "," + AnsiString(TrackElement.SpeedTag));
    int HLoc = TrackElement.HLoc;
    int VLoc = TrackElement.VLoc;

    if((TrackElement.SpeedTag != 129) && (TrackElement.SpeedTag != 130) && (TrackElement.SpeedTag != 145) && (TrackElement.SpeedTag != 146))
    {
        Utilities->CallLogPop(1821);
        return(false);
    }
    if(TrackElement.SpeedTag == 129) // vertical footbridge
    {
        // check top connection
        if(!(InactiveMapCheck(1, HLoc, VLoc, 76) // top plat
             || InactiveMapCheck(2, HLoc, VLoc - 1, 96) // concourse
             || InactiveMapCheck(3, HLoc, VLoc - 1, 77) // bot plat
             || ActiveMapCheck(4, HLoc, VLoc - 1, 129))) // vert footbridge
        {
            Utilities->CallLogPop(550);
            return(false);
        }
        // check bottom connection
        else if(!(InactiveMapCheck(4, HLoc, VLoc, 77) // bot plat
                  || InactiveMapCheck(5, HLoc, VLoc + 1, 96) // concourse
                  || InactiveMapCheck(6, HLoc, VLoc + 1, 76) // top plat
                  || ActiveMapCheck(1, HLoc, VLoc + 1, 129))) // vert footbridge
        {
            Utilities->CallLogPop(551);
            return(false);
        }
    }
    if(TrackElement.SpeedTag == 145) // vertical underpass
    {
        // check top connection
        if(!(InactiveMapCheck(13, HLoc, VLoc, 76) // top plat
             || InactiveMapCheck(14, HLoc, VLoc - 1, 96) // concourse
             || InactiveMapCheck(15, HLoc, VLoc - 1, 77) // bot plat
             || ActiveMapCheck(5, HLoc, VLoc - 1, 145))) // vert u'pass
        {
            Utilities->CallLogPop(2114);
            return(false);
        }
        // check bottom connection
        else if(!(InactiveMapCheck(16, HLoc, VLoc, 77) // bot plat
                  || InactiveMapCheck(17, HLoc, VLoc + 1, 96) // concourse
                  || InactiveMapCheck(18, HLoc, VLoc + 1, 76) // top plat
                  || ActiveMapCheck(6, HLoc, VLoc + 1, 145))) // vert u'pass
        {
            Utilities->CallLogPop(2115);
            return(false);
        }
    }
    if(TrackElement.SpeedTag == 130) // hor footbridge
    {
        // check left connection
        if(!(InactiveMapCheck(19, HLoc, VLoc, 78) // left plat
             || InactiveMapCheck(20, HLoc - 1, VLoc, 96) // concourse
             || InactiveMapCheck(21, HLoc - 1, VLoc, 79) // right plat
             || ActiveMapCheck(2, HLoc - 1, VLoc, 130))) // hor footbridge
        {
            Utilities->CallLogPop(552);
            return(false);
        }
        // check right connection
        else if(!(InactiveMapCheck(22, HLoc, VLoc, 79) // right plat
                  || InactiveMapCheck(23, HLoc + 1, VLoc, 96) // concourse
                  || InactiveMapCheck(24, HLoc + 1, VLoc, 78) // left plat
                  || ActiveMapCheck(3, HLoc + 1, VLoc, 130))) // hor footbridge
        {
            Utilities->CallLogPop(553);
            return(false);
        }
    }
    if(TrackElement.SpeedTag == 146) // hor u'pass
    {
        // check left connection
        if(!(InactiveMapCheck(7, HLoc, VLoc, 78) // left plat
             || InactiveMapCheck(8, HLoc - 1, VLoc, 96) // concourse
             || InactiveMapCheck(9, HLoc - 1, VLoc, 79) // right plat
             || ActiveMapCheck(7, HLoc - 1, VLoc, 146))) // hor u'pass
        {
            Utilities->CallLogPop(2116);
            return(false);
        }
        // check right connection
        else if(!(InactiveMapCheck(10, HLoc, VLoc, 79) // right plat
                  || InactiveMapCheck(11, HLoc + 1, VLoc, 96) // concourse
                  || InactiveMapCheck(12, HLoc + 1, VLoc, 78) // left plat
                  || ActiveMapCheck(8, HLoc + 1, VLoc, 146))) // hor u'pass
        {
            Utilities->CallLogPop(2117);
            return(false);
        }
    }
    Utilities->CallLogPop(554);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::InactiveMapCheck(int Caller, int HLoc, int VLoc, int SpeedTag)
/*
      return true if the SpeedTag present in the map at H & V
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",InactiveMapCheck," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(SpeedTag));
    if(InactiveTrack2MultiMap.empty())
    {
        Utilities->CallLogPop(555);
        return(false);
    }
    THVPair HVPair(HLoc, VLoc);
    TInactiveTrack2MultiMapIterator IMEnd = InactiveTrack2MultiMap.end();
    TInactiveTrack2MultiMapIterator HVIt1 = IMEnd, HVIt2 = IMEnd;
    TInactiveTrackRange HVRange = InactiveTrack2MultiMap.equal_range(HVPair);

    if(HVRange.first == HVRange.second)
    {
        Utilities->CallLogPop(556);
        return(false);
    }
    else
    {
        HVIt1 = HVRange.first;
    }
    TTrackElement Temp1, Temp2; // test

    Temp1 = InactiveTrackElementAt(8, HVIt1->second); // test
    if(--HVRange.second != HVRange.first)
    {
        HVIt2 = HVRange.second;
        Temp2 = InactiveTrackElementAt(9, HVIt2->second); // test
    }
    if((InactiveTrackElementAt(10, HVIt1->second).SpeedTag == SpeedTag) || ((HVIt2 != IMEnd) && (InactiveTrackElementAt(11,
                                                                                                                        HVIt2->second).SpeedTag == SpeedTag)))
    {
        Utilities->CallLogPop(557);
        return(true);
    }
    else
    {
        Utilities->CallLogPop(558);
        return(false);
    }
}

// ---------------------------------------------------------------------------

bool TTrack::ActiveMapCheck(int Caller, int HLoc, int VLoc, int SpeedTag)
/*
      return true if the SpeedTag present in the map at H & V
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ActiveMapCheck," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(SpeedTag));
    if(TrackMap.empty())
    {
        Utilities->CallLogPop(559);
        return(false);
    }
    THVPair HVPair(HLoc, VLoc);
    TTrackMapIterator End = TrackMap.end();
    TTrackMapIterator It = End;

    It = TrackMap.find(HVPair);
    if((It != End) && (TrackElementAt(19, It->second).SpeedTag == SpeedTag))
    {
        Utilities->CallLogPop(560);
        return(true);
    }
    else
    {
        Utilities->CallLogPop(561);
        return(false);
    }
}

// ---------------------------------------------------------------------------

void TTrack::EnterLocationName(int Caller, AnsiString LocationName, bool AddingElements)
{
/*
      General:
      All platform, concourse, footcrossing & non-station named location elements are able to have a LocationName allocated, and track
      elements (including footcrossings) are able to have an ActiveTrackElementName allocated provided there is an adjacent platform or
      a NamedNonStationLocation.
      To set these names the user selects a single named location element (not a footcrossing), enters the name, and
      this is then allocated as a LocationName to all linked platform, concourse and footcrossing elements, and as an
      ActiveTrackElementName to all track elements adjacent to platforms (inc footcrossing tracks if (but only if) they have a
      platform at that location).

      Linked named location elements are those explained in TTrack::TTrack()

      Detail:
      Two containers are used for allocation of names - LNPendingList, and LNDone2MultiMap, each containing vector positions as
      integers and the Map using THVPairs as keys. An adjustment is made for the vector positions as follows:-
      inactive vector positions are stored as they are (since most NamedLocationElements are in the inactive vector), but active vector
      positions stored as (-1-True Position), so can hold both types in a single integer uniquely - not very elegant but it seems to
      work OK!  e.g. TrackVector position 0 would be stored as -1, position n would be stored as -1-n.  InactiveTrackVector position 0 would be stored as 0.
      To recover the true TrackVector position from a stored value the same rule applies, i.e. -1-stored value, equivalent to -1-(-1-original) = -1+1+original = original.

      The List holds elements that have still to be processed, and the Map holds elements that have been processed.  On entering
      this function a single element should be in the List (normally from the user's selection but can also be from
      SearchForAndUpdateLocationName), and the Map is cleared within the function.
      A 'while' loop is entered if the List isn't empty, and the front element in it examined.  All linked named location elements
      (platforms, concourses and footcrossings) that aren't already in either the Map or the List are first added to the List using
      AdjElement, then the element itself has it's LocationName set, and any relevant track elements at the same H & V (i.e. adjacent
      to a platform) have their ActiveTrackElementName set using AddName.  The element is then inserted into the Map and erased from the List.
      In this way the list builds up while there are linked elements to be added, but reduces to zero when all are added and processing
      moves them into the Map.  At the end all linked elements are in the Map.

      Finally any other element that isn't in the Map, i.e. not linked to the current named location, that has the same name as a
      LocationName or ActiveTrackElementName, has it erased.  This is to allow for deletion of named location elements that split an existing
      named location - only one of the sides (selected by whichever the program finds first - the user can't select it) retains the name.
*/

// AnsiString TestString = "H,V,Tag,List Size,DoneMultiMap Size,CurrentElementAddress,MultiMapEntryAddress";//test
// Display->FileDiagnostics(TestString);//test

    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EnterLocationName," + LocationName);
    AnsiString TestString1, TestString2; // test

    Track->LNDone2MultiMap.clear();
    if(LNPendingList.size() != 1)
    {
        throw Exception("LNPendingList size not 1 on entry");
    }
    int CurrentElementNumber; //new after 2.4.3 due to error the JK found (Discord 9/7/20).  See note below after 'if(AddingElements)' where CurrentElementNumber is used.
    while(!LNPendingList.empty())
    {
        CurrentElementNumber = LNPendingList.front();
        TTrackVectorIterator CurrentElement = GetTrackVectorIteratorFromNamePosition(1, CurrentElementNumber);
        int NewElement; // = 2000000000;   //marker for unused //not needed from v1.2.0 Beta onwards
        int H = CurrentElement->HLoc;
        int V = CurrentElement->VLoc;
        int Tag = CurrentElement->SpeedTag;
        if(Tag == 76) // top plat
        {
            // AdjElement checks if there is an element matching Tag at H & V that isn't already in LNDone2MultiMap or LNPendingList,
            // & returns true if so with the adjusted vector position in NewElement.  It checks the appropriate vector
            // depending on the SpeedTag value (footcrossings in active vector, rest in inactive vector),
            for(int x = 0; x < 25; x++)
            {
                if(AdjElement(1, H + Tag76Array[x][0], V + Tag76Array[x][1], Tag76Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 77) // bot plat
        {
            for(int x = 0; x < 25; x++)
            {
                if(AdjElement(2, H + Tag77Array[x][0], V + Tag77Array[x][1], Tag77Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 78) // l plat
        {
            for(int x = 0; x < 25; x++)
            {
                if(AdjElement(3, H + Tag78Array[x][0], V + Tag78Array[x][1], Tag78Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 79) // r plat
        {
            for(int x = 0; x < 25; x++)
            {
                if(AdjElement(4, H + Tag79Array[x][0], V + Tag79Array[x][1], Tag79Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 96) // conc
        {
            for(int x = 0; x < 28; x++)
            {
                if(AdjElement(5, H + Tag96Array[x][0], V + Tag96Array[x][1], Tag96Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 129) // vert footbridge
        {
            for(int x = 0; x < 8; x++)
            {
                if(AdjElement(6, H + Tag129Array[x][0], V + Tag129Array[x][1], Tag129Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 130) // hor footbridge
        {
            for(int x = 0; x < 8; x++)
            {
                if(AdjElement(7, H + Tag130Array[x][0], V + Tag130Array[x][1], Tag130Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 131) // named location
        {
            for(int x = 0; x < 4; x++)
            {
                if(AdjElement(8, H + Tag131Array[x][0], V + Tag131Array[x][1], Tag131Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 145) // v u'pass
        {
            for(int x = 0; x < 8; x++)
            {
                if(AdjElement(9, H + Tag145Array[x][0], V + Tag145Array[x][1], Tag145Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        else if(Tag == 146) // h u'pass
        {
            for(int x = 0; x < 8; x++)
            {
                if(AdjElement(10, H + Tag146Array[x][0], V + Tag146Array[x][1], Tag146Array[x][2], NewElement))
                {
                    LNPendingList.insert(LNPendingList.end(), NewElement);
                }
            }
        }
        // below new at v1.1.0 but condition changed at v1.1.4 as interfered with name changes for single element locations
// if(NewElement != 2000000000) //adjacent element found & new element inserted, check if a (different) name already allocated and if so erase it from text vector
        if(AddingElements)
        {
            int HPos, VPos; // not used but needed for FindText function
            if(CurrentElementNumber > -1) //up to & including 2.4.2 this was NewElement, which was the last one added during LNPendingList building above, so it could be
                                          //repeatedly selected rather than the element under examination (LNPendingList.front()) & the front element text name wouldn't be erased.
                                          //Using CurrentElementNumber ensures that all elements are examined &  have names erased if present
            {
                AnsiString ExistingName = InactiveTrackElementAt(118, CurrentElementNumber).LocationName; //existing name of CurrentElement
                if((ExistingName != "") && (ExistingName != LocationName))
                {
                    if(LocationNameMultiMap.find(ExistingName) == Track->LocationNameMultiMap.end())
                    {
                    }  // name not in LocationNameMultiMap, so don't erase from TextVector
                    else if(TextHandler->FindText(4, ExistingName, HPos, VPos)) // can't use 'EraseLocationNameText' as that function is in TInterface
                    {
                        if(TextHandler->TextErase(10, HPos, VPos, ExistingName))
                        {
                            ;
                        } // condition not used

                    }
                }
            }
        }
        AddName(1, CurrentElement, LocationName); // add location name to current element, + timetable name to any
        // track at that loc
        THVPair HVPair(H, V);
        TLNDone2MultiMapEntry LNDone2MultiMapEntry;
        LNDone2MultiMapEntry.first = HVPair;
        LNDone2MultiMapEntry.second = LNPendingList.front();
        LNDone2MultiMap.insert(LNDone2MultiMapEntry);
        LNPendingList.erase(LNPendingList.begin());
    }

// search all name multimap for same name where corresponding active elements don't appear in
// LNDone2MultiMap & erase the name for all elements at that H & V in both active & inactive vectors

    TLocationNameMultiMapIterator SNIterator;
    TLocationNameMultiMapRange SNRange = LocationNameMultiMap.equal_range(LocationName);
    bool FoundFlag, ErasedFlag = false;

    if(SNRange.first != SNRange.second)
    {
        SNRange.first--; // now pointing to before the first
        SNRange.second--; // now pointing to the last
        for(SNIterator = SNRange.second; SNIterator != SNRange.first; SNIterator--)
        {
            // Same elements are in Done map as in name map
            if(!ElementInLNDone2MultiMap(1, SNIterator->second))
            {
                ErasedFlag = true;
                TTrackVectorIterator TVIt = GetTrackVectorIteratorFromNamePosition(2, SNIterator->second);
                TVIt->LocationName = "";
                TVIt->ActiveTrackElementName = ""; // in case it's a footcrossing
                // need to erase the timetable name in a track element at same H&V if present (i.e. if a platform)
                if((TVIt->TrackType == Platform) || (TVIt->TrackType == NamedNonStationLocation))
                {
                    int Position = GetVectorPositionFromTrackMap(17, TVIt->HLoc, TVIt->VLoc, FoundFlag);
                    if(FoundFlag)
                    {
                        TrackElementAt(20, Position).LocationName = "";
                        TrackElementAt(21, Position).ActiveTrackElementName = "";
                    }
                }
                // erase name in name map
// ChangeLocationNameMultiMapEntry("", SNIterator); can't use this as interferes with the iterators
            }
        }
    }
    if(ErasedFlag)
    {
        RebuildLocationNameMultiMap(2);
    }
    if(TrackFinished)
    {
        SetStationEntryStopLinkPosses(3);
    }
// set here as well as in LinkTrack so don't have to link track just because a name added
// if track not finished then will be set when track validated

// Rebuild ContinuationNameMap - added at v2.6.1 due to error found by Andrekoener &  notified by discord on 16/12/20
// error was that if a continuation name was changed and a timetable stopping place included that new name then ContinuationNameMap wouldn't be rebuilt
// so the timetable would validate and load and the name would appear in the dropdown list.  The reason was that ContinuationNameMap was only built in TryToLinkTrack,
// so if that isn't called (as it isn't for a name change) then the error wouldn't be seen.  However next time the railway was loaded TryToLinkTrack was called
// so the error would be seen.
// This inclusion rebuilds ContinuationNameMap whenever a name is entered or changed so the error can no longer be hidden.
    std::pair<AnsiString, char>TempMapPair;

    ContinuationNameMap.clear();
    for(int x = 0; x < Track->TrackVectorSize(); x++)
    {
        if((Track->TrackElementAt(1356, x).TrackType == Continuation) && (Track->TrackElementAt(1357, x).ActiveTrackElementName != ""))
        {
            TempMapPair.first = Track->TrackElementAt(1358, x).ActiveTrackElementName;
            TempMapPair.second = 'x'; // unused
            ContinuationNameMap.insert(TempMapPair);
        }
    }
//end of addition
    CheckLocationNameMultiMap(1); // test
    Utilities->CallLogPop(562);
}

// ---------------------------------------------------------------------------

bool TTrack::AdjElement(int Caller, int HLoc, int VLoc, int SpeedTag, int &FoundElement)
/*
      Looks for a FixedNamedLocationElement at H & V with SpeedTag, and if found and not already present in either the
      LNDone2MultiMap or the LNPendingList returns an int corresponding to the adjusted vector position.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AdjElement," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(SpeedTag));
    if(!NamedLocationElementAt(2, HLoc, VLoc))
    {
        Utilities->CallLogPop(948);
        return(false);
    }
    bool FoundFlag;
    int Position = -1;
    TIMPair IMPair;

    if((SpeedTag == 129) || (SpeedTag == 130) || (SpeedTag == 145) || (SpeedTag == 146)) // footcrossing - only in active vector
    {
        Position = GetVectorPositionFromTrackMap(18, HLoc, VLoc, FoundFlag);
        if(FoundFlag)
        {
            if(TrackElementAt(22, Position).SpeedTag == SpeedTag)
            {
                int MapPos = -1 - Position; // MapPos is the adjusted entry in the list & map
                if(!ElementInLNDone2MultiMap(2, MapPos) && !ElementInLNPendingList(1, MapPos))
                // don't allow duplicates in either list, or processing takes a lot longer
                {
                    FoundElement = MapPos;
                    Utilities->CallLogPop(563);
                    return(true);
                }
            }
        }
    }
    else
    {
        IMPair = GetVectorPositionsFromInactiveTrackMap(8, HLoc, VLoc, FoundFlag);
        if(FoundFlag)
        {
            if(InactiveTrackElementAt(12, IMPair.first).SpeedTag == SpeedTag)
            {
                if(!ElementInLNDone2MultiMap(3, IMPair.first) && !ElementInLNPendingList(2, IMPair.first))
                {
                    FoundElement = IMPair.first;
                    Utilities->CallLogPop(564);
                    return(true);
                }
            }
            else if(InactiveTrackElementAt(13, IMPair.second).SpeedTag == SpeedTag)
            {
                if(!ElementInLNDone2MultiMap(4, IMPair.second) && !ElementInLNPendingList(3, IMPair.second))
                {
                    FoundElement = IMPair.second;
                    Utilities->CallLogPop(565);
                    return(true);
                }
            }
        }
    }
    Utilities->CallLogPop(566);
    return(false);
}

// ---------------------------------------------------------------------------

void TTrack::AddName(int Caller, TTrackVectorIterator TrackElement, AnsiString Name)
/*
      Add location name to TrackElement and ActiveTrackElementName to any elements in trackmap
      at same H & V if TrackElement is a Platform or named non-station location.  Also update LocationNameMultiMap
      with the new name
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AddName," + TrackElement->LogTrack(6) + "," + Name);
    AnsiString OldName = TrackElement->LocationName, ErrorString; // declare new AnsiStrings OldName (set to existing name) & ErrorString

    TrackElement->LocationName = Name; // covers all FixedNamedLocationElement whichever vector they are in
    int HLoc = TrackElement->HLoc;
    int VLoc = TrackElement->VLoc;
    bool FoundFlag;

    if((TrackElement->TrackType == Platform) || (TrackElement->TrackType == NamedNonStationLocation))
    // only have timetable names for adjacent platforms & named locations
    {
        int Position = GetVectorPositionFromTrackMap(19, HLoc, VLoc, FoundFlag);
        if(FoundFlag)
        {
            TrackElementAt(23, Position).ActiveTrackElementName = Name;
        }
    }
    TLocationNameMultiMapIterator SNIterator = FindNamedElementInLocationNameMultiMap(4, OldName, TrackElement, ErrorString);

    if(ErrorString != "")
    {
        throw Exception(ErrorString + " in AddName for OldName == " + OldName);
    }
    ChangeLocationNameMultiMapEntry(1, Name, SNIterator); // OK, can use it here as not in an iterator loop
    CheckLocationNameMultiMap(2); // test
    Utilities->CallLogPop(567);
}

// ---------------------------------------------------------------------------

bool TTrack::ElementInLNDone2MultiMap(int Caller, int MapPos)
/*
      Examines LNDone2MultiMap to see whether the MapPos value is present, and returns true if so.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ElementInLNDone2MultiMap," + AnsiString(MapPos));
    if(LNDone2MultiMap.empty())
    {
        Utilities->CallLogPop(568);
        return(false);
    }
    TLNDone2MultiMapIterator LNDone2MultiMapIterator;

    for(LNDone2MultiMapIterator = LNDone2MultiMap.begin(); LNDone2MultiMapIterator != LNDone2MultiMap.end(); LNDone2MultiMapIterator++)
    {
        if(LNDone2MultiMapIterator->second == MapPos)
        {
            Utilities->CallLogPop(569);
            return(true);
        }
    }
    Utilities->CallLogPop(570);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::ElementInLNPendingList(int Caller, int MapPos)
/*
      Examines LNPendingList to see whether the MapPos value is present, and returns true if so.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ElementInLNPendingList," + AnsiString(MapPos));
    if(LNPendingList.empty())
    {
        Utilities->CallLogPop(571);
        return(false);
    }
    TLNPendingListIterator LNPendingListIterator;

    for(LNPendingListIterator = LNPendingList.begin(); LNPendingListIterator != LNPendingList.end(); LNPendingListIterator++)
    {
        if(*LNPendingListIterator == MapPos)
        {
            Utilities->CallLogPop(572);
            return(true);
        }
    }
    Utilities->CallLogPop(573);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::NamedLocationElementAt(int Caller, int HLoc, int VLoc)
/*
      Examines element at H & V, and returns true if its FixedNamedLocationElement bool is true
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NamedLocationElementAt," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    THVPair HVPair(HLoc, VLoc);
    TTrackMapIterator TrackMapPtr = TrackMap.find(HVPair);
    TInactiveTrack2MultiMapIterator InactiveTrack2MultiMapIterator = InactiveTrack2MultiMap.find(HVPair);

    if(TrackMapPtr != TrackMap.end()) // =end() if not found
    {
        if(TrackElementAt(24, TrackMapPtr->second).FixedNamedLocationElement)
        {
            Utilities->CallLogPop(574);
            return(true);
        }
    }
    if(InactiveTrack2MultiMapIterator != InactiveTrack2MultiMap.end())
    // may be 2 platforms at location but if so both FixedNamedLocationElement bools will be set, so only need to find one
    {
        if(InactiveTrackElementAt(14, InactiveTrack2MultiMapIterator->second).FixedNamedLocationElement)
        {
            Utilities->CallLogPop(575);
            return(true);
        }
    }
    Utilities->CallLogPop(576);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::LocationNameAllocated(int Caller, AnsiString LocationName) // true if a non-empty LocationName found in LocationNameMultiMap
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LocationNameAllocated," + LocationName);
    if(Track->LocationNameMultiMap.find(LocationName) != Track->LocationNameMultiMap.end())
    {
        Utilities->CallLogPop(1953);
        return(true);
    }
    Utilities->CallLogPop(1954);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::DuplicatedLocationName(int Caller, bool GiveMessage)
//examines LocationNameMultiMap and returns true if there are two or more locations with the same name - added at v2.6.1 to cater for Bill78's new .dev file merge
//program and used when try to save as a .rly file
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DuplicatedLocationName");
    TLocationNameMultiMapIterator LNMMIt;
    TLocationNameMultiMapRange LNMMRg;
    if(LocationNameMultiMap.empty()) //no names so no duplicates
    {
        Utilities->CallLogPop(2254);
        return(false);
    }
    AnsiString NameBeingChecked = LocationNameMultiMap.begin()->first; //first name to start with
    LNMMRg = LocationNameMultiMap.equal_range(NameBeingChecked);
    if(NameBeingChecked != "") //added for v2.6.2 as 2.6.1 reported duplicate null names (reported by Micke(Commuterpop) 06/01/21 via discord)
    {
        if(!PopulateHVPairsLinkedMapAndNoDuplicates(0, LNMMRg))
        {
            if(GiveMessage)
            {
                ShowMessage("Please note that more than one instance of " + NameBeingChecked + " was found.  Location names must be unique before the railway can be saved as a .rly file");
            }
            Utilities->CallLogPop(2255);
            return(true);
        }
    }
    while(LNMMRg.second != LocationNameMultiMap.end()) //here LNMMRg still set to earlier name
    {
        NameBeingChecked = LNMMRg.second->first; //this is the next name as LNMMRg->second points to the first location NOT containing the first name
        LNMMRg = LocationNameMultiMap.equal_range(NameBeingChecked); //here LNMMRg is the new range
        if(NameBeingChecked != "")  //should have skipped all null names as all listed together but add to be on the safe side
        {
            if(!PopulateHVPairsLinkedMapAndNoDuplicates(1, LNMMRg))
            {
                if(GiveMessage)
                {
                    ShowMessage("Please note that more than one instance of " + NameBeingChecked + " was found.  Location names must be unique before the railway can be saved as a .rly file");
                }
                Utilities->CallLogPop(2256);
                return(true);
            }
        }
    }
    Utilities->CallLogPop(2257);
    return(false);       //OK, no duplicates
}

// ---------------------------------------------------------------------------

bool TTrack::PopulateHVPairsLinkedMapAndNoDuplicates(int Caller, TLocationNameMultiMapRange LNMMRg)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PopulateHVPairsLinkedMapAndNoDuplicates");
    THVPair HVPair;
    THVPairsLinkedMap HVPairsLinkedMap; //this is a map of HVPairs (so each is unique), each with a boolean marker, false for not linked and true for linked
                                        //for use in the duplicate check
    for(TLocationNameMultiMapIterator LNMMIt = LNMMRg.first; LNMMIt != LNMMRg.second; LNMMIt++) //populating the linked map
    {
        if(LNMMIt->second < 0) //active track element
        {
            HVPair.first = TrackElementAt(1011, (-1 - LNMMIt->second)).HLoc;
            HVPair.second = TrackElementAt(1012, (-1 - LNMMIt->second)).VLoc;
        }
        else //inactive track element
        {
            HVPair.first = InactiveTrackElementAt(134, LNMMIt->second).HLoc;
            HVPair.second = InactiveTrackElementAt(135, LNMMIt->second).VLoc;
        }
        HVPairsLinkedMap.insert(std::pair<THVPair, bool>(HVPair, false)); //set all bools to false initially
    }
    //All HVPairs now present in HVPairsLinkedMap for the specific location name

    //now need to identify all named elements that are linked either vertically or horizontally with the first one (could be any but must be just one)
    //so that at the end any that haven't been identified aren't linked and so represent a duplicated name
    //to do so need a list (works like LNPendingList) to hold all elements that haven't been checked for links

    std::list<THVPair> HVLinkedList;

    //set the first value to true and add it to the list
    HVPairsLinkedMap.begin()->second = true;
    HVLinkedList.push_back(HVPairsLinkedMap.begin()->first);
    //now enter a loop to examine the front pair in the list and set all linked bools to true, and if they weren't already true then add them to the back of the list for later
    //examination
    THVPair HVPairUnderExamination;
    THVPairsLinkedMap::iterator HVPLMIt;
    THVPair HVPairNew;
    while(!HVLinkedList.empty())
    {
        HVPairUnderExamination = HVLinkedList.front();
        HVLinkedList.pop_front();
        HVPairNew.first = HVPairUnderExamination.first;
        HVPairNew.second = HVPairUnderExamination.second - 1; //position directly above element
        HVPLMIt = HVPairsLinkedMap.find(HVPairNew);
        if(HVPLMIt != HVPairsLinkedMap.end())
        {
            if(!HVPLMIt->second)
            {
                HVLinkedList.push_back(HVPLMIt->first);
            }
            HVPLMIt->second = true;
        }
        HVPairNew.first = HVPairUnderExamination.first - 1;
        HVPairNew.second = HVPairUnderExamination.second; //position to the left of the element
        HVPLMIt = HVPairsLinkedMap.find(HVPairNew);
        if(HVPLMIt != HVPairsLinkedMap.end())
        {
            if(!HVPLMIt->second)
            {
                HVLinkedList.push_back(HVPLMIt->first);
            }
            HVPLMIt->second = true;
        }
        HVPairNew.first = HVPairUnderExamination.first;
        HVPairNew.second = HVPairUnderExamination.second + 1; //position under the element
        HVPLMIt = HVPairsLinkedMap.find(HVPairNew);
        if(HVPLMIt != HVPairsLinkedMap.end())
        {
            if(!HVPLMIt->second)
            {
                HVLinkedList.push_back(HVPLMIt->first);
            }
            HVPLMIt->second = true;
        }
        HVPairNew.first = HVPairUnderExamination.first + 1;
        HVPairNew.second = HVPairUnderExamination.second; //position to the right of the element
        HVPLMIt = HVPairsLinkedMap.find(HVPairNew);
        if(HVPLMIt != HVPairsLinkedMap.end())
        {
            if(!HVPLMIt->second)
            {
                HVLinkedList.push_back(HVPLMIt->first);
            }
            HVPLMIt->second = true;
        }
    }

    //at the end if any have a false bool then the name is duplicated so return false
    for(THVPairsLinkedMap::iterator HVPLMIt = HVPairsLinkedMap.begin(); HVPLMIt != HVPairsLinkedMap.end(); HVPLMIt++)
    {
        if(!HVPLMIt->second)
        {
            Utilities->CallLogPop(2258);
            return(false);
        }
    }
    Utilities->CallLogPop(2259);
    return(true);
}

// ---------------------------------------------------------------------------

bool TTrack::TimetabledLocationNameAllocated(int Caller, AnsiString LocationName)
/*
      Examines ActiveTrackElementNameMap and returns true if the LocationName is found and isn't "" (used to use LocationNameMultiMap)
*/

{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TimetabledLocationNameAllocated," + LocationName);
    if(LocationName == "")
    {
        Utilities->CallLogPop(577);
        return(false);
    }
// new for v0.2b
// compile ActiveTrackElementNameMap if !ActiveTrackElementNameMapCompiledFlag (added as a precaution, should be compiled when reach here)
    if(!ActiveTrackElementNameMapCompiledFlag)
    {
        TActiveTrackElementNameMapEntry ActiveTrackElementNameMapEntry;
        ActiveTrackElementNameMap.clear();
        for(unsigned int x = 0; x < TrackVector.size(); x++)
        {
            if((TrackElementAt(1359, x).ActiveTrackElementName != "") && (ContinuationNameMap.find(TrackElementAt(1360, x).ActiveTrackElementName))
               == ContinuationNameMap.end())
            {
                // exclude any name that appears in a continuation, error message given in tt validation if try to include such a name in a tt
                ActiveTrackElementNameMapEntry.first = Track->TrackElementAt(1361, x).ActiveTrackElementName;
                ActiveTrackElementNameMapEntry.second = 0; // this is a dummy value
                ActiveTrackElementNameMap.insert(ActiveTrackElementNameMapEntry);
            }
        }
        ActiveTrackElementNameMapCompiledFlag = true;
    }
    Utilities->CallLogPop(578);
    return (ActiveTrackElementNameMap.find(LocationName) != ActiveTrackElementNameMap.end());
// end of new section
// return (LocationNameMultiMap.find(LocationName) != LocationNameMultiMap.end());
}

// ---------------------------------------------------------------------------

void TTrack::EraseLocationAndActiveTrackElementNames(int Caller, AnsiString LocationName)
/*
      Examines LocationNameMultiMap and if the LocationName is found all elements at that H & V (in both active and inactive vectors) have the
      name erased both as a LocationName and a ActiveTrackElementName.  The LocationNameMultiMap is also rebuilt to correspond to the
      new names in the vectors.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EraseLocationAndActiveTrackElementNames," + LocationName);
    bool FoundFlag, ErasedFlag = false;
    TLocationNameMultiMapIterator SNIterator;
    TLocationNameMultiMapRange SNRange = LocationNameMultiMap.equal_range(LocationName);

    if(SNRange.first != SNRange.second)
    {
        ErasedFlag = true;
        for(SNIterator = SNRange.first; SNIterator != SNRange.second; SNIterator++)
        {
            TTrackVectorIterator TVIt = GetTrackVectorIteratorFromNamePosition(3, SNIterator->second);
            TVIt->LocationName = "";
            TVIt->ActiveTrackElementName = ""; // in case it's a footcrossing
            // need to erase the timetable name in a track element at same H&V if present (i.e. if a platform or namedlocation)
            if((TVIt->TrackType == Platform) || (TVIt->TrackType == NamedNonStationLocation))
            {
                int Position = GetVectorPositionFromTrackMap(20, TVIt->HLoc, TVIt->VLoc, FoundFlag);
                if(FoundFlag)
                {
                    TrackElementAt(25, Position).LocationName = "";
                    TrackElementAt(26, Position).ActiveTrackElementName = "";
                }
            }
        }
    }
    if(ErasedFlag)
    {
        RebuildLocationNameMultiMap(3);
    }
    CheckLocationNameMultiMap(3); // test
    Utilities->CallLogPop(579);
}

// ---------------------------------------------------------------------------

void TTrack::SearchForAndUpdateLocationName(int Caller, int HLoc, int VLoc, int SpeedTag)
/*
      NB No longer used during EraseTrackElement, too long-winded - erase all name now if any part removed, user needs to re-enter
      Checks all locations that are adjacent to the one entered for linked named location elements, and if any LocationName is found
      in any of the linked elements that name is used for all elements that are now linked to it.  The location entered doesn't
      need to be a FixedNamedLocationElement and there doesn't even need to be an element there.  Used during EraseTrackElement (in which
      case the SpeedTag is that for the element that is erased) and PlotAndAddTrackElement, to bring the named location and timetable
      naming up to date with the deletion or insertion.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SearchForAndUpdateLocationName," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString(SpeedTag));
    LNPendingList.clear();
    AnsiString LocationName;
    int MapPos;
    bool FoundFlag = 0;

//first check if this element is inactive and named, and if so use its position and name   (new at v2.6.0 to allow pasted named locations to name linked elements)
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(30, HLoc, VLoc, FoundFlag);
    if(FoundFlag)
    {
        LocationName = InactiveTrackElementAt(132, IMPair.first).LocationName;
        if(LocationName != "")
        {
            LNPendingList.insert(Track->LNPendingList.end(), IMPair.first);
            EnterLocationName(13, LocationName, true);
            Utilities->CallLogPop(2251);
            return;
        }
        LocationName = InactiveTrackElementAt(133, IMPair.second).LocationName;
        if(LocationName != "")
        {
            LNPendingList.insert(Track->LNPendingList.end(), IMPair.second);
            EnterLocationName(14, LocationName, true);
            Utilities->CallLogPop(2252);
            return;
        }
    }
//then check if this element is active and named, and if so use its position (-Pos-1) and name   (new at v2.6.0 to allow pasted named locations to name linked elements)

    int Position = GetVectorPositionFromTrackMap(59, HLoc, VLoc, FoundFlag);
    if(FoundFlag)
    {
        LocationName = TrackElementAt(1004, Position).LocationName;
        if(LocationName != "")
        {
            int ModifiedPosition = -1 - Position;
            LNPendingList.insert(Track->LNPendingList.end(), ModifiedPosition);
            EnterLocationName(15, LocationName, true);
            Utilities->CallLogPop(2253);
            return;
        }
    }
    if(SpeedTag == 76) // top plat
    {
        for(int x = 0; x < 25; x++)
        {
            if(AdjNamedElement(1, HLoc + Tag76Array[x][0], VLoc + Tag76Array[x][1], Tag76Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(3, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 77) // bot plat
    {
        for(int x = 0; x < 25; x++)
        {
            if(AdjNamedElement(2, HLoc + Tag77Array[x][0], VLoc + Tag77Array[x][1], Tag77Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(4, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 78) // l plat
    {
        for(int x = 0; x < 25; x++)
        {
            if(AdjNamedElement(3, HLoc + Tag78Array[x][0], VLoc + Tag78Array[x][1], Tag78Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(5, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 79) // r plat
    {
        for(int x = 0; x < 25; x++)
        {
            if(AdjNamedElement(4, HLoc + Tag79Array[x][0], VLoc + Tag79Array[x][1], Tag79Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(6, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 96) // conc
    {
        for(int x = 0; x < 28; x++)
        {
            if(AdjNamedElement(5, HLoc + Tag96Array[x][0], VLoc + Tag96Array[x][1], Tag96Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(7, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 129) // vert footbridge
    {
        for(int x = 0; x < 8; x++)
        {
            if(AdjNamedElement(6, HLoc + Tag129Array[x][0], VLoc + Tag129Array[x][1], Tag129Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(8, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 130) // hor footbridge
    {
        for(int x = 0; x < 8; x++)
        {
            if(AdjNamedElement(7, HLoc + Tag130Array[x][0], VLoc + Tag130Array[x][1], Tag130Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(9, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 145) // vert u'pass
    {
        for(int x = 0; x < 8; x++)
        {
            if(AdjNamedElement(9, HLoc + Tag145Array[x][0], VLoc + Tag145Array[x][1], Tag145Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(11, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 146) // hor u'pass
    {
        for(int x = 0; x < 8; x++)
        {
            if(AdjNamedElement(10, HLoc + Tag146Array[x][0], VLoc + Tag146Array[x][1], Tag146Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(12, LocationName, true);
                break;
            }
        }
    }
    else if(SpeedTag == 131) // named location
    {
        for(int x = 0; x < 4; x++)
        {
            if(AdjNamedElement(8, HLoc + Tag131Array[x][0], VLoc + Tag131Array[x][1], Tag131Array[x][2], LocationName, MapPos))
            {
                LNPendingList.insert(Track->LNPendingList.end(), MapPos);
                EnterLocationName(10, LocationName, true);
                break;
            }
        }
    }
// AddName(HLoc, VLoc, LocationName);//don't need this now, EnterLocationName takes care of it
    Utilities->CallLogPop(580);
}

// ---------------------------------------------------------------------------

bool TTrack::AdjNamedElement(int Caller, int HLoc, int VLoc, int SpeedTag, AnsiString &LocationName, int &FoundElement)
/*
      Used in SearchForAndUpdateLocationName to check for elements in TrackMap & InactiveTrackMap that match H, V & Tag, & returns
      true if a LocationName is found, and also returns the name and the adjusted vector position.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AdjNamedElement," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(SpeedTag));
    bool FoundFlag;
    TIMPair IMPair;
    TTrackVectorIterator TempElement;
    int Position;

    IMPair = GetVectorPositionsFromInactiveTrackMap(9, HLoc, VLoc, FoundFlag);
    if(FoundFlag)
    {
        if(InactiveTrackElementAt(15, IMPair.first).SpeedTag == SpeedTag)
        {
            TempElement = InactiveTrackVector.begin() + IMPair.first;
            if(TempElement->LocationName != "")
            {
                LocationName = TempElement->LocationName;
                FoundElement = IMPair.first;
                Utilities->CallLogPop(581);
                return(true);
            }
        }
        else if(InactiveTrackElementAt(16, IMPair.second).SpeedTag == SpeedTag)
        {
            TempElement = InactiveTrackVector.begin() + IMPair.second;
            if(TempElement->LocationName != "")
            {
                LocationName = TempElement->LocationName;
                FoundElement = IMPair.second;
                Utilities->CallLogPop(582);
                return(true);
            }
        }
    }
    Position = GetVectorPositionFromTrackMap(21, HLoc, VLoc, FoundFlag);
    if(FoundFlag)
    {
        if(TrackElementAt(27, Position).SpeedTag == SpeedTag)
        {
            TempElement = TrackVector.begin() + Position;
            if(TempElement->LocationName != "")
            {
                LocationName = TempElement->LocationName;
                FoundElement = -1 - Position;
                Utilities->CallLogPop(583);
                return(true);
            }
        }
    }
    Utilities->CallLogPop(584);
    return(false);
}

// ---------------------------------------------------------------------------

void TTrack::CheckLocationNameMultiMap(int Caller) // test function
{
// check quantity in map & vectors match
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckLocationNameMultiMap,");
    unsigned int Count = 0;

    if(SkipLocationNameMultiMapCheck) // renamed in v2.4.0 to skip check when pasting because fails as map elements not fully aligned until all pasted
    {
        Utilities->CallLogPop(2059);
        return;
    }
    AnsiString SName, TName, ErrorString;

    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(1362, x).FixedNamedLocationElement)
        {
            if(TrackElementAt(1363, x).TrackType != FootCrossing)
            {
                throw Exception("Track element has FixedNamedLocationElement set but is not a footbridge/underpass in CheckLocationNameMultiMap, caller = " +
                                AnsiString(Caller));
            }
            Count++;
        }
    }
    for(unsigned int x = 0; x < InactiveTrackVector.size(); x++)
    {
        if(InactiveTrackElementAt(143, x).FixedNamedLocationElement)
        {
            if((InactiveTrackElementAt(144, x).TrackType != Platform) && (InactiveTrackElementAt(145, x).TrackType != NamedNonStationLocation) &&
               (InactiveTrackElementAt(146, x).TrackType != Concourse))
            {
                throw Exception
                          ("Inactive track element has FixedNamedLocationElement set but is not a platform, concourse or named location in CheckLocationNameMultiMap, caller = " +
                          AnsiString(Caller));
            }
            Count++;
        }
    }
    if(LocationNameMultiMap.size() != Count)
    {
        throw Exception("LocationNameMultiMap size = " + AnsiString(LocationNameMultiMap.size()) + " & Count = " + AnsiString(Count) +
                        " in CheckLocationNameMultiMap, caller = " + AnsiString(Caller));
    }
// check all entries in both vectors match entries in name multimap
    TLocationNameMultiMapIterator SNIt;

    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(1364, x).FixedNamedLocationElement)
        {
            SName = TrackElementAt(1365, x).LocationName;
            SNIt = FindNamedElementInLocationNameMultiMap(5, SName, TrackVector.begin() + x, ErrorString);
            if(ErrorString != "")
            {
                throw Exception(ErrorString + " in CheckLocationNameMultiMap for TrackVector check, caller = " + AnsiString(Caller));
            }
            if(SNIt->second != -1 - (int)x)
            {
                throw Exception("Elements different in name map & TrackVector in CheckLocationNameMultiMap for TrackVector check, caller = " +
                                AnsiString(Caller));
            }
        }
        // check corresponding platform for all Timetable entries that aren't empty
        TName = TrackElementAt(1366, x).ActiveTrackElementName;
        TIMPair IMPair;
        bool FoundFlag = false;
        if(TName != "")
        {
            IMPair = GetVectorPositionsFromInactiveTrackMap(10, TrackElementAt(1367, x).HLoc, TrackElementAt(1368, x).VLoc, FoundFlag);
            if(FoundFlag)
            {
                if((InactiveTrackElementAt(17, IMPair.first).TrackType != Platform) && (InactiveTrackElementAt(18, IMPair.second).TrackType != Platform) &&
                   (InactiveTrackElementAt(19, IMPair.first).TrackType != NamedNonStationLocation))
                {
                    throw Exception("Track element with ActiveTrackElementName but no plat/named loc at H " + AnsiString(TrackElementAt(1369, x).HLoc) + " & V " +
                                    AnsiString(TrackElementAt(1370, x).VLoc) + " in CheckLocationNameMultiMap, caller = " + AnsiString(Caller));
                }
                if((InactiveTrackElementAt(20, IMPair.first).LocationName != TName) && (InactiveTrackElementAt(21, IMPair.second).LocationName != TName))
                {
                    throw Exception("Track element with ActiveTrackElementName " + TName + " but plat/named loc at H " + AnsiString(TrackElementAt(1371, x).HLoc) +
                                    " & V " + AnsiString(TrackElementAt(1372, x).VLoc) + " has different LocationName in CheckLocationNameMultiMap, caller = " +
                                    AnsiString(Caller));
                }
            }
            else
            {
                throw Exception("Track element with ActiveTrackElementName but no inactive element at H " + AnsiString(TrackElementAt(1373, x).HLoc) + " & V " +
                                AnsiString(TrackElementAt(1374, x).VLoc) + " in CheckLocationNameMultiMap, caller = " + AnsiString(Caller));
            }
        }
    }
    for(unsigned int x = 0; x < InactiveTrackVector.size(); x++)
    {
        if(InactiveTrackElementAt(147, x).FixedNamedLocationElement)
        {
            SName = InactiveTrackElementAt(148, x).LocationName;
            SNIt = FindNamedElementInLocationNameMultiMap(6, SName, InactiveTrackVector.begin() + x, ErrorString);
            if(ErrorString != "")
            {
                throw Exception(ErrorString + " in CheckLocationNameMultiMap for InactiveTrackVector check, caller = " + AnsiString(Caller));
            }
            if(SNIt->second != (int)x)
            {
                throw Exception("Elements different in name map & TrackVector in CheckLocationNameMultiMap for TrackVector check, caller = " +
                                AnsiString(Caller));
            }
        }
    }
    Utilities->CallLogPop(585);
}

// ---------------------------------------------------------------------------

TTrack::TLocationNameMultiMapIterator TTrack::FindNamedElementInLocationNameMultiMap(int Caller, AnsiString LocationName, TTrackVectorIterator TrackElement,
                                                                                     AnsiString &ErrorString)
{
/*
      Searches the name map to check if the element pointed to by the TTrackVectorIterator has the name
      LocationName.  If it finds it the pointer TLocationNameMultiMapIterator is returned.  If it fails ErrorString
      is set to an appropriate text to allow the calling function to report the error.  Otherwise it is set to "".
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindNamedElementInLocationNameMultiMap," + LocationName + "," +
                                 AnsiString(TrackElement->HLoc) + "," + AnsiString(TrackElement->VLoc) + "," + AnsiString(TrackElement->SpeedTag));
    ErrorString = "";
    bool FoundFlag = false;
    TLocationNameMultiMapIterator SNIterator;
    TLocationNameMultiMapRange SNRange = LocationNameMultiMap.equal_range(LocationName);

    if(SNRange.first == SNRange.second)
    {
        ErrorString = "Error, Name " + LocationName + " not found in map";
        Utilities->CallLogPop(586);
        return(SNRange.first);
    }
    else
    {
        for(SNIterator = SNRange.first; SNIterator != SNRange.second; SNIterator++)
        {
            if(SNIterator->second < 0)
            {
                int TVPos = -1 - SNIterator->second;
                TTrackVectorIterator TVIt = TrackVector.begin() + TVPos;
                if(TVIt == TrackElement)
                {
                    FoundFlag = true;
                    Utilities->CallLogPop(587);
                    return(SNIterator);
                }
            }
            else
            {
                int ITVPos = SNIterator->second;
                TTrackVectorIterator ITVIt = InactiveTrackVector.begin() + ITVPos;
                if(ITVIt == TrackElement)
                {
                    FoundFlag = true;
                    Utilities->CallLogPop(588);
                    return(SNIterator);
                }
            }
        }
    }
    if(!FoundFlag)
    {
        ErrorString = "Error, Name " + LocationName + " found but not at required element";
    }
    Utilities->CallLogPop(589);
    return(SNIterator);
}

// ---------------------------------------------------------------------------

void TTrack::ChangeLocationNameMultiMapEntry(int Caller, AnsiString NewName, TLocationNameMultiMapIterator SNIterator)
{
/*
      Changes the LocationName in the name multimap to NewName at the location pointed to by the TLocationNameMultiMapIterator
      from whatever it was before.  Accepts null entries so that a formerly named element can have the name changed to "".
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ChangeLocationNameMultiMapEntry," + NewName);
    TLocationNameMultiMapEntry LocationNameEntry;

    LocationNameEntry.first = NewName;
    LocationNameEntry.second = SNIterator->second;
    LocationNameMultiMap.erase(SNIterator);
    LocationNameMultiMap.insert(LocationNameEntry);
    Utilities->CallLogPop(590);
}

// ---------------------------------------------------------------------------

TTrack::TTrackVectorIterator TTrack::GetTrackVectorIteratorFromNamePosition(int Caller, int Position)
{
// Takes an adjusted vector position value and returns a pointer to the relevant element.  Can be in either vector.
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetTrackVectorIteratorFromNamePosition," + AnsiString(Position));
    if(Position < 0) // footcrossing
    {
        int TruePos = -1 - Position;
        // new check at v0.2b
        if(TrackElementAt(817, TruePos).TrackType != FootCrossing)
        {
            throw Exception("Footbridge/underpass error in GetTrackVectorIteratorFromNamePosition, caller = " + AnsiString(Caller));
        }
        Utilities->CallLogPop(591);
        return (TrackVector.begin() + TruePos);
    }
    else
    {
        // new check at v0.2b
        if(!(InactiveTrackElementAt(99, Position).FixedNamedLocationElement))
        {
            throw Exception("Inactive element error in GetTrackVectorIteratorFromNamePosition, caller = " + AnsiString(Caller));
        }
        Utilities->CallLogPop(592);
        return (InactiveTrackVector.begin() + Position);
    }
}

// ---------------------------------------------------------------------------

void TTrack::DecrementValuesInInactiveTrackAndNameMaps(int Caller, unsigned int VecPos)
{
/*
      After an element has been erased from the inactive track vector, all the later elements are moved down one.  This function
      decrements the position values for all values above that of the erased element in both InactiveTrack2MultiMap and
      LocationNameMultiMap.
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DecrementValuesInInactiveTrackAndNameMaps," + AnsiString(VecPos));
    TInactiveTrack2MultiMapIterator InactiveTrack2MultiMapIterator;
    TLocationNameMultiMapIterator LocationNameMultiMapIterator;

    if(!InactiveTrack2MultiMap.empty())
    {
        for(InactiveTrack2MultiMapIterator = InactiveTrack2MultiMap.begin(); InactiveTrack2MultiMapIterator != InactiveTrack2MultiMap.end();
            InactiveTrack2MultiMapIterator++)
        {
            if(InactiveTrack2MultiMapIterator->second > VecPos)
            {
                InactiveTrack2MultiMapIterator->second--;
            }
            // can't be == VecPos as that position erased
        }
    }
    if(!LocationNameMultiMap.empty())
    {
        for(LocationNameMultiMapIterator = LocationNameMultiMap.begin(); LocationNameMultiMapIterator != LocationNameMultiMap.end();
            LocationNameMultiMapIterator++)
        {
            if(LocationNameMultiMapIterator->second < 0)
            {
                continue; // deal with TrackVectors separately
            }
            if(LocationNameMultiMapIterator->second > (int)VecPos)
            {
                LocationNameMultiMapIterator->second--;
            }
        }
    }
    Utilities->CallLogPop(593);
}

// ---------------------------------------------------------------------------

void TTrack::DecrementValuesInGapsAndTrackAndNameMaps(int Caller, unsigned int VecPos)
{
/*
      After an element has been erased from the track vector, all the later elements are moved down one.  This function
      decrements the position values for all values above that of the erased element in the gap elements, TrackMap and
      LocationNameMultiMap.
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DecrementValuesInGapsAndTrackAndNameMaps," + AnsiString(VecPos));
    TTrackMapIterator TrackMapIterator;
    TLocationNameMultiMapIterator LocationNameMultiMapIterator;

    if(!TrackMap.empty())
    {
        for(TrackMapIterator = TrackMap.begin(); TrackMapIterator != TrackMap.end(); TrackMapIterator++)
        {
            if(TrackMapIterator->second > VecPos)
            {
                TrackMapIterator->second--;
            }
            // can't be == VecPos as that position erased
        }
    }
    if(!LocationNameMultiMap.empty())
    {
        for(LocationNameMultiMapIterator = LocationNameMultiMap.begin(); LocationNameMultiMapIterator != LocationNameMultiMap.end();
            LocationNameMultiMapIterator++)
        {
            if(LocationNameMultiMapIterator->second >= 0)
            {
                continue; // deal with InactiveTrackVectors separately
            }
            // (-1-VecPos)  VP   0  1  2  3  4  5  6  7
            // Val -1 -2 -3 -4 -5 -6 -7 -8
            if(LocationNameMultiMapIterator->second < -(int)(VecPos + 1))
            {
                LocationNameMultiMapIterator->second++;
            }
        }
    }
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        TTrackElement &TkEl = TrackElementAt(1375, x); // no need to check so use this to speed up
        if(TkEl.TrackType == GapJump)
        {
            // position 0 is the gap
            if(TkEl.Conn[0] == int(VecPos))
            {
                TkEl.Conn[0] = -1; // connected to a deleted gap
                continue;
            }
            if(TkEl.Conn[0] > int(VecPos))
            {
                TkEl.Conn[0]--;
            }
            if(TkEl.Conn[0] > -1) // don't use 'else' here, need to check the value whether changed or not
            {
                if(TrackElementAt(709, TkEl.Conn[0]).TrackType != GapJump)
                {
                    TkEl.Conn[0] = -1;
                }
            }
        }
    }
    Utilities->CallLogPop(1433);
}

// ---------------------------------------------------------------------------

void TTrack::RebuildLocationNameMultiMap(int Caller)
/*
      Clears the existing LocationNameMultiMap and rebuilds it from TrackVector and InactiveTrackVector.  Called after the
      track is linked as many of the vector positions are likely to change - called from RepositionAndMapTrack();
      after names are changed in EraseLocationAndActiveTrackElementNames; and after the name changes in EnterLocationName.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RebuildLocationNameMultiMap");
    LocationNameMultiMap.clear();
    TLocationNameMultiMapEntry LocationNameEntry;
    TTrackElement TrackElement;

    for(unsigned int TVPos = 0; TVPos < TrackVector.size(); TVPos++)
    {
        TrackElement = TrackElementAt(1376, TVPos);
        if(TrackElement.FixedNamedLocationElement)
        {
            LocationNameEntry.first = TrackElement.LocationName;
            LocationNameEntry.second = -1 - TVPos; // adjusted for footcrossings
            LocationNameMultiMap.insert(LocationNameEntry);
        }
    }

    for(unsigned int ITVPos = 0; ITVPos < InactiveTrackVector.size(); ITVPos++)
    {
        TrackElement = InactiveTrackElementAt(149, ITVPos);
        if(TrackElement.FixedNamedLocationElement)
        {
            LocationNameEntry.first = TrackElement.LocationName;
            LocationNameEntry.second = ITVPos;
            LocationNameMultiMap.insert(LocationNameEntry);
        }
    }
    Utilities->CallLogPop(594);
}

// ---------------------------------------------------------------------------

bool TTrack::NonFootCrossingNamedLocationExists(int Caller)
// Return true if there is a named location present in the railway
// ignores lone footcrossings, can't name these on their own & track won't link if there are any
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NonFootCrossingNamedLocationExists");
    TTrackVectorIterator ITVI;

    if(InactiveTrackVector.empty())
    {
        Utilities->CallLogPop(1343);
        return(false);
    }
    for(ITVI = InactiveTrackVector.begin(); ITVI != InactiveTrackVector.end(); ITVI++)
    {
        if((ITVI->TrackType == Platform) || (ITVI->TrackType == NamedNonStationLocation) || (ITVI->TrackType == Concourse))
        {
            Utilities->CallLogPop(1404);
            return(true);
        }
    }
    Utilities->CallLogPop(1344);
    return(false);
}

// ---------------------------------------------------------------------------

void TTrack::SetAllDefaultLengthsAndSpeedLimits(int Caller)
/*
      Work through all elements in TrackVector setting all lengths & speed limits to default values - including both tracks for 2-track elements
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetAllDefaultLengthsAndSpeedLimits");
// ResetDistanceElements(6);
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        TTrackElement &TE = TrackElementAt(718, x);
        TE.Length01 = Utilities->DefaultTrackLength;
        TE.SpeedLimit01 = Utilities->DefaultTrackSpeedLimit;
        if((TE.TrackType == Points) || (TE.TrackType == Crossover) || (TE.TrackType == Bridge))
        {
            TE.Length23 = Utilities->DefaultTrackLength;
            TE.SpeedLimit23 = Utilities->DefaultTrackSpeedLimit;
        }
    }
/* old function
       if((TrackElementAt(, x).TrackType == Points) || (TrackElementAt(, x).TrackType == Crossover) || (TrackElementAt(, x).TrackType == Bridge))
           {
           SetOneDefaultTrackLength(2, TrackElementAt(, x), 0);
           SetOneDefaultTrackLength(3, TrackElementAt(, x), 2);
           }
       else
           {
           SetOneDefaultTrackLength(4, TrackElementAt(, x), 0);
           }
       }
*/
    Utilities->CallLogPop(617);
}

// ---------------------------------------------------------------------------

void TTrack::LengthMarker(int Caller, TDisplay *Disp)
// Examine all elements in the TrackVector and if have a valid length mark the relevant track using MarkOneLength.
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LengthMarker");
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        TTrackElement TempElement = TrackElementAt(1377, x);
        if(TempElement.Length01 > -1)
        {
            MarkOneLength(1, TempElement, true, Disp); // need Length01 test in case there are erase elements (but shouldn't be after LinkTrack)
        }
        if(TempElement.Length23 > -1)
        {
            MarkOneLength(2, TempElement, false, Disp);
        }
    }
    Disp->Update();
    Utilities->CallLogPop(618);
}

// ---------------------------------------------------------------------------

void TTrack::MarkOneLength(int Caller, TTrackElement TrackElement, bool FirstTrack, TDisplay *Disp)
/*
      Rule:  Only marked if different in any way from the default values - length 100m and speed limit 200km/h normally but can be changed in Config.txt
      First check using IsElementTrackDefaultLength whether the relevant track is already set to the default values, and if so
      return as nothing further to do.  Otherwise pick up the appropriate bitmap (using the AutoSigRouteGraphicsPtr bitmaps)
      using the same technique as in TPrefDirElement::EntryExitNumber() & *TPrefDirElement::GetPrefDirGraphicPtr(), for the relevant
      track as indicated by FirstTrack (true for track01 & false for track23).
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",MarkOneLength," + TrackElement.LogTrack(8) + "," +
                                 AnsiString((short)FirstTrack));
    bool LengthDifferent = false, SpeedDifferent = false;

    if(IsElementDefaultLength(1, TrackElement, FirstTrack, LengthDifferent, SpeedDifferent))
    {
        Utilities->CallLogPop(619);
        return;
    }
    int EXArray[16][2] =
    {{4, 6}, {2, 8}, // horizontal & vertical
     {2, 4}, {6, 2}, {8, 6}, {4, 8},    // sharp curves
     {1, 6}, {3, 8}, {9, 4}, {7, 2}, {1, 8}, {3, 4}, {9, 2}, {7, 6},    // loose curves
     {1, 9}, {3, 7}};    // forward & reverse diagonals

    int Index = -1, BrNum = -1, GrNum = -1, InLink, OutLink;
    Graphics::TBitmap *Bitmap;

    if(FirstTrack)
    {
        InLink = TrackElement.Link[0];
        OutLink = TrackElement.Link[1];
    }
    else
    {
        InLink = TrackElement.Link[2];
        OutLink = TrackElement.Link[3];
    }
    for(int x = 0; x < 16; x++)
    {
        if((InLink == EXArray[x][0] && OutLink == EXArray[x][1]) || (InLink == EXArray[x][1] && OutLink == EXArray[x][0]))
        {
            Index = x;
        }
    }
    if(Index == -1)
    {
        throw Exception("Error, failed to find Index in TTrack::MarkOneLength");
    }
/* The order for bridge entries & exits is as below.  Note that there are 3 of each type,
      the graphic for each of which is different because of the shape of the overbridge.  The basic
      entry/exit value is computed above, and this used to select only from elements with that entry/exit
      value that is an underbridge, i.e overbridges ignored as the normal graphic is OK for them.
      int BrEXArray[24][2] = {
      {4,6},{2,8},{1,9},{3,7},
      {1,9},{3,7},{1,9},{3,7},
      {2,8},{4,6},{2,8},{4,6}
*/
    if(!FirstTrack && (TrackElement.TrackType == Bridge))
    {
        if(Index == 1)
        {
            if(TrackElement.SpeedTag == 49)
            {
                BrNum = 1 + 16;
            }
            else if(TrackElement.SpeedTag == 54)
            {
                BrNum = 8 + 16;
            }
            else if(TrackElement.SpeedTag == 55)
            {
                BrNum = 10 + 16;
            }
        }
        else if(Index == 0)
        {
            if(TrackElement.SpeedTag == 48)
            {
                BrNum = 0 + 16;
            }
            else if(TrackElement.SpeedTag == 58)
            {
                BrNum = 11 + 16;
            }
            else if(TrackElement.SpeedTag == 59)
            {
                BrNum = 9 + 16;
            }
        }
        else if(Index == 14)
        {
            if(TrackElement.SpeedTag == 50)
            {
                BrNum = 2 + 16;
            }
            else if(TrackElement.SpeedTag == 52)
            {
                BrNum = 4 + 16;
            }
            else if(TrackElement.SpeedTag == 57)
            {
                BrNum = 6 + 16;
            }
        }
        else if(Index == 15)
        {
            if(TrackElement.SpeedTag == 51)
            {
                BrNum = 3 + 16;
            }
            else if(TrackElement.SpeedTag == 53)
            {
                BrNum = 7 + 16;
            }
            else if(TrackElement.SpeedTag == 56)
            {
                BrNum = 5 + 16;
            }
        }
    }
    if(!FirstTrack && (TrackElement.TrackType == Bridge))
    {
        GrNum = BrNum;
    }
    else
    {
        GrNum = Index;
    }
    if(LengthDifferent && SpeedDifferent) // blue - use autosig graphics
    {
        if(GrNum > 15) // underbridge
        {
            Bitmap = RailGraphics->BridgeRouteAutoSigsGraphicsPtr[GrNum - 16];
        }
        else
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[GrNum];
        }
        if(TrackElement.SpeedTag == 64)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[16]; // intercept diagonal buffers to show the buffer
        }
        if(TrackElement.SpeedTag == 65)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[17];
        }
        if(TrackElement.SpeedTag == 66)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[18];
        }
        if(TrackElement.SpeedTag == 67)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[19];
        }
        if(TrackElement.SpeedTag == 80)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[20]; // intercept continuations to show the dots
        }
        if(TrackElement.SpeedTag == 81)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[21];
        }
        if(TrackElement.SpeedTag == 82)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[22];
        }
        if(TrackElement.SpeedTag == 83)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[23];
        }
        if(TrackElement.SpeedTag == 84)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[24];
        }
        if(TrackElement.SpeedTag == 85)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[25];
        }
        if(TrackElement.SpeedTag == 86)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[26];
        }
        if(TrackElement.SpeedTag == 87)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[27];
        }
        if(TrackElement.SpeedTag == 129)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[28]; // intercept under footbridges
        }
        if(TrackElement.SpeedTag == 130)
        {
            Bitmap = RailGraphics->LinkRouteAutoSigsGraphicsPtr[29];
        }
    }

    else if(LengthDifferent && !SpeedDifferent) // green - use pref sig graphics
    {
        if(GrNum > 15) // underbridge
        {
            Bitmap = RailGraphics->BridgeSigRouteGraphicsPtr[GrNum - 16];
        }
        else
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[GrNum];
        }
        if(TrackElement.SpeedTag == 64)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[16]; // intercept diagonal buffers to show the buffer
        }
        if(TrackElement.SpeedTag == 65)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[17];
        }
        if(TrackElement.SpeedTag == 66)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[18];
        }
        if(TrackElement.SpeedTag == 67)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[19];
        }
        if(TrackElement.SpeedTag == 80)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[20]; // intercept continuations to show the dots
        }
        if(TrackElement.SpeedTag == 81)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[21];
        }
        if(TrackElement.SpeedTag == 82)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[22];
        }
        if(TrackElement.SpeedTag == 83)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[23];
        }
        if(TrackElement.SpeedTag == 84)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[24];
        }
        if(TrackElement.SpeedTag == 85)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[25];
        }
        if(TrackElement.SpeedTag == 86)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[26];
        }
        if(TrackElement.SpeedTag == 87)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[27];
        }
        if(TrackElement.SpeedTag == 129)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[28]; // intercept under footbridges
        }
        if(TrackElement.SpeedTag == 130)
        {
            Bitmap = RailGraphics->LinkSigRouteGraphicsPtr[29];
        }
    }

    else // SpeedDifferent only: red - use non sig graphics
    {
        if(GrNum > 15) // underbridge
        {
            Bitmap = RailGraphics->BridgeNonSigRouteGraphicsPtr[GrNum - 16];
        }
        else
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[GrNum];
        }
        if(TrackElement.SpeedTag == 64)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[16]; // intercept diagonal buffers to show the buffer
        }
        if(TrackElement.SpeedTag == 65)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[17];
        }
        if(TrackElement.SpeedTag == 66)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[18];
        }
        if(TrackElement.SpeedTag == 67)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[19];
        }
        if(TrackElement.SpeedTag == 80)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[20]; // intercept continuations to show the dots
        }
        if(TrackElement.SpeedTag == 81)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[21];
        }
        if(TrackElement.SpeedTag == 82)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[22];
        }
        if(TrackElement.SpeedTag == 83)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[23];
        }
        if(TrackElement.SpeedTag == 84)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[24];
        }
        if(TrackElement.SpeedTag == 85)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[25];
        }
        if(TrackElement.SpeedTag == 86)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[26];
        }
        if(TrackElement.SpeedTag == 87)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[27];
        }
        if(TrackElement.SpeedTag == 129)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[28]; // intercept under footbridges
        }
        if(TrackElement.SpeedTag == 130)
        {
            Bitmap = RailGraphics->LinkNonSigRouteGraphicsPtr[29];
        }
    }
    Disp->PlotOutput(67, TrackElement.HLoc * 16, TrackElement.VLoc * 16, Bitmap);
    Utilities->CallLogPop(620);
}

// ---------------------------------------------------------------------------

bool TTrack::IsElementDefaultLength(int Caller, TTrackElement &TrackElement, bool FirstTrack, bool &LengthDifferent, bool &SpeedDifferent)
/* FirstTrack = LinkPos's 0 & 1
      Examine track within TrackElement & check whether it has the default length and speed limit, return true if so
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsElementTrackDefaultLength," + TrackElement.LogTrack(10) + "," +
                                 AnsiString((short)FirstTrack));
    LengthDifferent = false;
    SpeedDifferent = false;
    if(((TrackElement.TrackType == Bridge) || (TrackElement.TrackType == Points) || (TrackElement.TrackType == Crossover)) && FirstTrack)
    {
        if(TrackElement.Length01 != Utilities->DefaultTrackLength)
        {
            LengthDifferent = true;
        }
        if(TrackElement.SpeedLimit01 != Utilities->DefaultTrackSpeedLimit)
        {
            SpeedDifferent = true;
        }
        if(LengthDifferent || SpeedDifferent)
        {
            Utilities->CallLogPop(625);
            return(false);
        }
        Utilities->CallLogPop(626);
        return(true);
    }

    else if(((TrackElement.TrackType == Bridge) || (TrackElement.TrackType == Points) || (TrackElement.TrackType == Crossover)) && !FirstTrack)
    {
        if(TrackElement.Length23 != Utilities->DefaultTrackLength)
        {
            LengthDifferent = true;
        }
        if(TrackElement.SpeedLimit23 != Utilities->DefaultTrackSpeedLimit)
        {
            SpeedDifferent = true;
        }
        if(LengthDifferent || SpeedDifferent)
        {
            Utilities->CallLogPop(627);
            return(false);
        }
        Utilities->CallLogPop(628);
        return(true);
    }

    else // any other 1 track element, including platforms being present
    {
        if(TrackElement.Length01 != Utilities->DefaultTrackLength)
        {
            LengthDifferent = true;
        }
        if(TrackElement.SpeedLimit01 != Utilities->DefaultTrackSpeedLimit)
        {
            SpeedDifferent = true;
        }
        if(LengthDifferent || SpeedDifferent)
        {
            Utilities->CallLogPop(629);
            return(false);
        }
        Utilities->CallLogPop(630);
        return(true);
    }
}

// ---------------------------------------------------------------------------

bool TTrack::IsPlatformOrNamedNonStationLocationPresent(int Caller, int HLoc, int VLoc)
// Check whether there is a platform or NamedNonStationLocation present at HLoc & VLoc, return true if so
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsPlatformOrNamedNonStationLocationPresent," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(11, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(633);
        return(false);
    }
    if((InactiveTrackElementAt(42, IMPair.first).TrackType == Platform) || (InactiveTrackElementAt(91, IMPair.first).TrackType == NamedNonStationLocation))
    {
        Utilities->CallLogPop(634);
        return(true); // only need to check first since if second is a platform the the first must be too
    }
    else
    {
        Utilities->CallLogPop(635);
        return(false);
    }
}

// ---------------------------------------------------------------------------

bool TTrack::IsNamedNonStationLocationPresent(int Caller, int HLoc, int VLoc)
// Check whether there is a NamedNonStationLocation present at HLoc & VLoc, return true if so
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsNamedNonStationLocationPresent," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(12, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(636);
        return(false);
    }
    if(InactiveTrackElementAt(43, IMPair.first).TrackType == NamedNonStationLocation)
    {
        Utilities->CallLogPop(637);
        return(true); // only need to check first since only one used for NamedNonStationLocations
    }
    else
    {
        Utilities->CallLogPop(638);
        return(false);
    }
}

// ---------------------------------------------------------------------------

void TTrack::SetStationEntryStopLinkPosses(int Caller)
/* Called when trying to link track and when a name changed when track already linked.  Examines all track elements that
      have ActiveTrackElementName set, sums the number of consecutive elements with the same name, and sets the EntryLink values for
      the front of train stop points for each direction.
      For stations (not non-station named locations) of length n, where n > 1, stop element is [floor((n+1)/2) + 1] from each
      end (unless buffers at one or both ends in which case stop points are the end elements).
      Note that for a single element the stop point is the element itself (formula doesn't apply).
      During the function the StationEntryStopLink values are set to 5 if not used, so no need to keep
      repeating the procedure for every element.  At the end all unused values are returned to -1.
      For NamedNonStationLocations the stop points are at the end elements to allow trains to stack up.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetStationEntryStopLinkPosses");
    TTrackElement TempElement, StartElement;
    AnsiString TempName;
    int VecPos, StartVecPos, Count, EntryPos, StartEntryPos, ForwardNumber, ReverseNumber;
    bool ForwardSet, ReverseSet;

    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        TrackElementAt(1378, x).StationEntryStopLinkPos1 = -1;
        TrackElementAt(1379, x).StationEntryStopLinkPos2 = -1;
    }
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        ForwardSet = false;
        ReverseSet = false;
        TempElement = TrackElementAt(1380, x);
        VecPos = x;
        if((TempElement.ActiveTrackElementName != "") && (TempElement.StationEntryStopLinkPos1 == -1))
        // 2nd condition incl so don't re-examine elements with stop links set to 5
        {
            TempName = TempElement.ActiveTrackElementName;
            if((TempElement.Conn[0] > -1) && (TempElement.Conn[1] > -1) && (TrackElementAt(44, TempElement.Conn[0]).ActiveTrackElementName == TempName) &&
               (TrackElementAt(45, TempElement.Conn[1]).ActiveTrackElementName == TempName))
            // an element linked at both ends where both links are also named elements
            // only Conn[0] & [1] relevant for ActiveTrackElementName elements (only 2-track named element is points, and only straight track relevant & this has 0 & 1 as entry/exit positions)
            {
                continue; // looking for an end element so skip this one
            }
            else // reached one end
            {
                if((TempElement.Conn[0] > -1) && (TempElement.Conn[1] > -1) && (TrackElementAt(46, TempElement.Conn[0]).ActiveTrackElementName != TempName) &&
                   (TrackElementAt(47, TempElement.Conn[1]).ActiveTrackElementName != TempName))
                // single named element linked at both ends
                {
                    TrackElementAt(48, VecPos).StationEntryStopLinkPos1 = 0;
                    TrackElementAt(49, VecPos).StationEntryStopLinkPos2 = 1;
                    continue;
                }
                else if((TempElement.TrackType == Buffers) && (TrackElementAt(618, TempElement.Conn[1]).ActiveTrackElementName != TempName))
                // single named buffer element (LinkPos 1 is the non-buffer end)
                {
                    TrackElementAt(619, VecPos).StationEntryStopLinkPos1 = 0;
                    TrackElementAt(620, VecPos).StationEntryStopLinkPos2 = 1;
                    continue;
                }
                else
                // Note: only interested in connection positions 0 & 1 since all named elements are single track except points,
                // and platforms always on straight (conns 0 & 1) section of points
                {
                    for(int y = 0; y < 2; y++)
                    {
                        int Dir = y; // Dir is the ExitPos of the element, towards the rest of the named elements
                        // check for buffers at both ends - no need, function below now covers buffers at one & both ends
/* TTrackElement Temp1 = TempElement;
 ***********New section, compiles but not checked - does bit below need to be else if?
                       if((Temp1.TrackType == Buffers) && (Temp1.GetConfig(Dir) != End))
                           {
                           //search along Dir direction until find other end, skip if Dir facing buffer end
                           int NewDir = Dir;
                           int NewVecPos;
                           while((Temp1.Conn[NewDir] > -1) && (TrackElementAt(598, Temp1.Conn[NewDir]).ActiveTrackElementName == TempName))
                               {
                               NewVecPos = Temp1.Conn[NewDir];
                               NewDir = Track->GetNonPointsOppositeLinkPos(Temp1.ConnLinkPos[NewDir]);
                               Temp1 = TrackElementAt(601, NewVecPos);
                               }
                           if((Temp1.Conn[NewDir] == -1) && (Temp1.TrackType == Buffers))
                               {
                               TrackElementAt(599, VecPos).StationEntryStopLinkPos1 = Dir;//EntryPos for train coming from other end is Dir
                               TrackElementAt(600, NewVecPos).StationEntryStopLinkPos2 = 1 - NewDir;//For train moving in same direction as search direction its EntryPos == 1 - NewDir since NewDir is the ExitPos
                               }
                           }
 ***************
*/
                        // end may be linked at both ends but only one link named, or buffer with linked element named
                        // if a buffer then the named linkpos has to be 1
                        // already dealt with all types of single element so at least 2 linked named element
                        if(((TempElement.Conn[Dir] > -1) && (TempElement.Conn[1 - Dir] > -1) && (TrackElementAt(50,
                                                                                                                TempElement.Conn[1 - Dir]).ActiveTrackElementName != TempName)) || ((TempElement.TrackType == Buffers) && (Dir == 1)))
                        {
                            StartElement = TempElement;
                            StartVecPos = VecPos;
                            TrackElementAt(51, VecPos).StationEntryStopLinkPos1 = 5; // set to 5 to stop re-examination in later searches, all set back at end
                            TrackElementAt(52, VecPos).StationEntryStopLinkPos2 = 5;
                            EntryPos = 1 - Dir;
                            StartEntryPos = 1 - Dir;
                            Count = 1;
                            // work along named elements until find the other end
                            while((TempElement.Conn[1 - EntryPos] > -1) && (TrackElementAt(53,
                                                                                           TempElement.Conn[1 - EntryPos]).ActiveTrackElementName == TempName))
                            // at end of 'while' Count = length (in elements) of platform/nonstationloc, VecPos = vector number of far end
                            // which is the last named element that is track-linked to the rest of the location, it may be a buffer
                            // all stop link pos's are set to 5
                            {
                                VecPos = TempElement.Conn[1 - EntryPos];
                                int TempEntryPos = TempElement.ConnLinkPos[1 - EntryPos];
                                TempElement = TrackElementAt(54, TempElement.Conn[1 - EntryPos]);
                                EntryPos = TempEntryPos;
                                Count++;
                                TrackElementAt(55, VecPos).StationEntryStopLinkPos1 = 5;
                                TrackElementAt(56, VecPos).StationEntryStopLinkPos2 = 5;
                            }
                            // here when reached other end, maybe buffers, continuation or last named linked element
                            if(TrackElementAt(57, VecPos).TrackType == Buffers)
                            // terminal station, set end elements as stop elements
                            {
                                TrackElementAt(58, VecPos).StationEntryStopLinkPos1 = EntryPos;
                                TrackElementAt(59, StartVecPos).StationEntryStopLinkPos2 = 1 - StartEntryPos; // for train leaving
                                continue;
                            }
                            if(TrackElementAt(60, StartVecPos).TrackType == Buffers) // best not to use 'else if' as both ends could be buffers!
                            // terminal station, set end elements as stop elements
                            {
                                TrackElementAt(61, VecPos).StationEntryStopLinkPos1 = EntryPos;
                                TrackElementAt(62, StartVecPos).StationEntryStopLinkPos2 = 1 - StartEntryPos;
                                continue;
                            }
                            if(IsNamedNonStationLocationPresent(1, TrackElementAt(63, StartVecPos).HLoc, TrackElementAt(64, StartVecPos).VLoc))
                            // NonStationLocation so set end elements as stop elements
                            {
                                TrackElementAt(65, VecPos).StationEntryStopLinkPos1 = EntryPos;
                                TrackElementAt(66, StartVecPos).StationEntryStopLinkPos2 = 1 - StartEntryPos;
                                continue;
                            }
                            // now Count == length of platform, can calculate StationEntryStopLinkPos values and the elements to which they apply
                            ForwardNumber = ((Count + 1) / 2) + 1;
                            ReverseNumber = (Count - ForwardNumber) + 1;
                            Count = 1; // starting value
                            EntryPos = 1 - Dir;
                            TempElement = StartElement;
                            VecPos = StartVecPos;
                            if(Count == ForwardNumber)
                            {
                                TrackElementAt(67, VecPos).StationEntryStopLinkPos1 = EntryPos;
                                ForwardSet = true;
                            }
                            if(Count == ReverseNumber) // don't use 'else' as may both be at same element
                            {
                                TrackElementAt(68, VecPos).StationEntryStopLinkPos2 = 1 - EntryPos;
                                ReverseSet = true;
                            }
                            while((TempElement.Conn[1 - EntryPos] > -1) && (TrackElementAt(69,
                                                                                           TempElement.Conn[1 - EntryPos]).ActiveTrackElementName == TempName) && (!ForwardSet || !ReverseSet))
                            {
                                VecPos = TempElement.Conn[1 - EntryPos];
                                int TempEntryPos = TempElement.ConnLinkPos[1 - EntryPos];
                                TempElement = TrackElementAt(70, TempElement.Conn[1 - EntryPos]);
                                EntryPos = TempEntryPos;
                                Count++;
                                if(Count == ForwardNumber)
                                {
                                    TrackElementAt(71, VecPos).StationEntryStopLinkPos1 = EntryPos;
                                    ForwardSet = true;
                                }
                                if(Count == ReverseNumber)
                                {
                                    TrackElementAt(72, VecPos).StationEntryStopLinkPos2 = 1 - EntryPos;
                                    ReverseSet = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(1381, x).StationEntryStopLinkPos1 == 5)
        {
            TrackElementAt(1382, x).StationEntryStopLinkPos1 = -1;
        }
        if(TrackElementAt(1383, x).StationEntryStopLinkPos2 == 5)
        {
            TrackElementAt(1384, x).StationEntryStopLinkPos2 = -1;
        }
    }
    Utilities->CallLogPop(639);
}

// ---------------------------------------------------------------------------

void TTrack::PlotSmallRailway(int Caller, TDisplay *Disp)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotSmallRailway");
    TTrackElement Next;

    NextTrackElementPtr = InactiveTrackVector.begin();
    while(ReturnNextInactiveTrackElement(1, Next))
    {
        if(Next.SmallGraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
        {
            if(((Next.TrackType == Platform) || (Next.TrackType == Concourse) || (Next.TrackType == NamedNonStationLocation)) && (Next.LocationName == ""))
            // need striped graphics
            {
                if(Next.SpeedTag == 76)
                {
                    Disp->PlotSmallOutput(11, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm76striped);
                }
                else if(Next.SpeedTag == 77)
                {
                    Disp->PlotSmallOutput(12, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm77striped);
                }
                else if(Next.SpeedTag == 78)
                {
                    Disp->PlotSmallOutput(13, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm78striped);
                }
                else if(Next.SpeedTag == 79)
                {
                    Disp->PlotSmallOutput(14, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm79striped);
                }
                else if(Next.SpeedTag == 96)
                {
                    Disp->PlotSmallOutput(15, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm96striped);
                }
                else if(Next.SpeedTag == 131)
                {
                    Disp->PlotSmallOutput(16, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm131striped);
                }
            }
            else
            {
                Disp->PlotSmallOutput(17, Next.HLoc * 4, (Next.VLoc * 4), Next.SmallGraphicPtr);
            }
        }
    }

    NextTrackElementPtr = TrackVector.begin();
    while(ReturnNextTrackElement(1, Next))
    {
        if(Next.SmallGraphicPtr != 0) // don't think this should ever be 0 but leave as a safeguard
        {
            if((Next.TrackType == FootCrossing) && (Next.LocationName == "")) // need striped graphics, use sm129 & 130 for 145 & 146
            {
                if((Next.SpeedTag == 129) || (Next.SpeedTag == 145))
                {
                    Disp->PlotSmallOutput(18, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm129striped);
                }
                else if((Next.SpeedTag == 130) || (Next.SpeedTag == 146))
                {
                    Disp->PlotSmallOutput(19, Next.HLoc * 4, (Next.VLoc * 4), RailGraphics->sm130striped);
                }
            }
            else
            {
                Disp->PlotSmallOutput(20, Next.HLoc * 4, (Next.VLoc * 4), Next.SmallGraphicPtr);
            }
        }
    }
    Disp->Update();
    Utilities->CallLogPop(640);
}

// ---------------------------------------------------------------------------

void TTrack::PlotSmallRedGap(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlotSmallRedGap");
    Display->PlotSmallOutput(21, GapHLoc * 4, GapVLoc * 4, RailGraphics->smRed);
    Utilities->CallLogPop(1346);
}

// ---------------------------------------------------------------------------

void TTrack::TrackClear(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrackClear");
    TrackVector.clear();
    InactiveTrackVector.clear();
    TrackMap.clear();
    InactiveTrack2MultiMap.clear(), LocationNameMultiMap.clear();
    if(TextHandler->TextVector.size() == 0)
    {
        Display->DisplayOffsetH = 0;
        Display->DisplayOffsetV = 0;
        Display->DisplayOffsetHHome = 0;
        Display->DisplayOffsetVHome = 0;
        Display->DisplayZoomOutOffsetH = 0;
        Display->DisplayZoomOutOffsetV = 0;
        Display->DisplayZoomOutOffsetHHome = 0;
        Display->DisplayZoomOutOffsetVHome = 0;
        HLocMin = 2000000000;
        HLocMax = -2000000000;
        VLocMin = 2000000000;
        VLocMax = -2000000000;
    }
    else
    {
        CalcHLocMinEtc(4);
    }
    Utilities->CallLogPop(1347);
}

// ---------------------------------------------------------------------------

void TTrack::CalcHLocMinEtc(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CalcHLocMinEtc");
    HLocMin = 2000000000;
    VLocMin = 2000000000;
    HLocMax = -2000000000;
    VLocMax = -2000000000;
    for(unsigned int x = 0; x < TrackVector.size(); x++) // check all elements in turn
    {
        if(TrackElementAt(1385, x).SpeedTag == 0)
        {
            continue; // skip erase elements or would interfere with Min & Max values
        }
        if(TrackElementAt(1386, x).HLoc - 1 < HLocMin)
        {
            HLocMin = TrackElementAt(1387, x).HLoc - 1; // add one all round
        }
        if(TrackElementAt(1388, x).HLoc + 1 > HLocMax)
        {
            HLocMax = TrackElementAt(1389, x).HLoc + 1;
        }
        if(TrackElementAt(1390, x).VLoc - 1 < VLocMin)
        {
            VLocMin = TrackElementAt(1391, x).VLoc - 1;
        }
        if(TrackElementAt(1392, x).VLoc + 1 > VLocMax)
        {
            VLocMax = TrackElementAt(1393, x).VLoc + 1;
        }
    }
    for(unsigned int x = 0; x < InactiveTrackVector.size(); x++) // check all elements in turn
    {
        if(InactiveTrackElementAt(150, x).SpeedTag == 0)
        {
            continue; // shouldn't be any inactive erase elements but include anyway
        }
        if(InactiveTrackElementAt(151, x).HLoc - 1 < HLocMin)
        {
            HLocMin = InactiveTrackElementAt(152, x).HLoc - 1; // add one all round
        }
        if(InactiveTrackElementAt(153, x).HLoc + 1 > HLocMax)
        {
            HLocMax = InactiveTrackElementAt(162, x).HLoc + 1;
        }
        if(InactiveTrackElementAt(154, x).VLoc - 1 < VLocMin)
        {
            VLocMin = InactiveTrackElementAt(155, x).VLoc - 1;
        }
        if(InactiveTrackElementAt(156, x).VLoc + 1 > VLocMax)
        {
            VLocMax = InactiveTrackElementAt(157, x).VLoc + 1;
        }
    }
    for(unsigned int x = 0; x < TextHandler->TextVectorSize(10); x++) // check all elements in turn
    {
/* Removed at v2.2.0: It isn't needed because null names aren't entered into vector, and in any case if were then
       will fail as x will exceed the maximum value
       if(TextHandler->TextPtrAt(, x)->TextString == "")
           {
               TextHandler->TextErase(, TextHandler->TextPtrAt(35, x)->HPos, TextHandler->TextPtrAt(36, x)->VPos);
           }
*/
        int TextH = TextHandler->TextPtrAt(0, x)->HPos, TextV = TextHandler->TextPtrAt(1, x)->VPos;
        if((TextH / 16) - 1 < HLocMin)
        {
            HLocMin = (TextH / 16) - 1; // integer division will truncate so subtract 1 to ensure include the start
        }
        if((TextH / 16) + 1 > HLocMax)
        {
            HLocMax = (TextH / 16) + 1; // integer division will truncate so add 1 to ensure include the start
        }
        if((TextV / 16) - 1 < VLocMin)
        {
            VLocMin = (TextV / 16) - 1;
        }
        if((TextV / 16) + 1 > VLocMax)
        {
            VLocMax = (TextV / 16) + 1;
        }
    }
    for(unsigned int x = 0; x < UserGraphicVector.size(); x++) // added at v2.4.0
    {
        if((UserGraphicVectorAt(5, x).HPos / 16) - 1 < HLocMin)
        {
            HLocMin = (UserGraphicVectorAt(6, x).HPos / 16) - 1; // add one all round
        }
        if(((UserGraphicVectorAt(7, x).HPos + UserGraphicVectorAt(8, x).Width) / 16) + 1 > HLocMax)
        {
            HLocMax = ((UserGraphicVectorAt(9, x).HPos + UserGraphicVectorAt(10, x).Width) / 16) + 1;
        }
        if((UserGraphicVectorAt(11, x).VPos / 16) - 1 < VLocMin)
        {
            VLocMin = (UserGraphicVectorAt(12, x).VPos / 16) - 1;
        }
        if(((UserGraphicVectorAt(13, x).VPos + UserGraphicVectorAt(14, x).Height) / 16) + 1 > VLocMax)
        {
            VLocMax = ((UserGraphicVectorAt(15, x).VPos + UserGraphicVectorAt(16, x).Height) / 16) + 1;
        }
    }

    Utilities->CallLogPop(641);
}

// ---------------------------------------------------------------------------

void TTrack::UserGraphicMove(int Caller, int HPosInput, int VPosInput, int &UserGraphicItem, int &UserGraphicMoveHPos, int &UserGraphicMoveVPos,
                             bool &UserGraphicFoundFlag)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",UserGraphicMove," + AnsiString(HPosInput) + "," + AnsiString(VPosInput));
    TUserGraphicVector::iterator UserGraphicPtr;

    UserGraphicFoundFlag = false;
    if(!UserGraphicVector.empty())
    {
        int x = UserGraphicVector.size();
        for(UserGraphicPtr = (UserGraphicVector.end() - 1); UserGraphicPtr >= UserGraphicVector.begin(); UserGraphicPtr--)
        {
            x--;
            if((HPosInput >= (*UserGraphicPtr).HPos) && (HPosInput < ((*UserGraphicPtr).HPos + (*UserGraphicPtr).Width)) && (VPosInput >=
                                                                                                                             (*UserGraphicPtr).VPos) && (VPosInput < ((*UserGraphicPtr).VPos + (*UserGraphicPtr).Height)))
            {
                UserGraphicItem = x;
                UserGraphicMoveHPos = (*UserGraphicPtr).HPos;
                UserGraphicMoveVPos = (*UserGraphicPtr).VPos;
                UserGraphicFoundFlag = true;
                Utilities->CallLogPop(2177);
                return;
            } // if ....

        } // for UserGraphicPtr...
    } // if !UserGraphicVector...

    Utilities->CallLogPop(2197);
}

// ---------------------------------------------------------------------------

Graphics::TBitmap *TTrack::RetrieveStripedNamedLocationGraphicsWhereRelevant(int Caller, TTrackElement TrackElement)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RetrieveStripedNamedLocationGraphicsWhereRelevant," +
                                 TrackElement.LogTrack(11));
    Graphics::TBitmap *GraphicOutput = RailGraphics->bmTransparentBgnd; // default value
    int SpeedTag = TrackElement.SpeedTag;

    if(SpeedTag < 1)
    {
        throw Exception("Error - SpeedTag value " + AnsiString(SpeedTag) + " in RetrieveStripedNamedLocationGraphicsWhereRelevant");
    }
    switch(SpeedTag)
    {
    case 76: // t platform
        GraphicOutput = RailGraphics->gl76Striped;
        break;

    case 77: // h platform
        GraphicOutput = RailGraphics->bm77Striped;
        break;

    case 78: // v platform
        GraphicOutput = RailGraphics->bm78Striped;
        break;

    case 79: // r platform
        GraphicOutput = RailGraphics->gl79Striped;
        break;

    case 96: // concourse
        GraphicOutput = RailGraphics->ConcourseStriped;
        break;

    case 129: // v footbridge
        GraphicOutput = RailGraphics->gl129Striped;
        break;

    case 130: // h footbridge
        GraphicOutput = RailGraphics->gl130Striped;
        break;

    case 131: // non-station named loc
        GraphicOutput = RailGraphics->bmNameStriped;
        break;

    case 145: // v u'pass
        GraphicOutput = RailGraphics->gl145Striped;
        break;

    case 146: // h u'pass
        GraphicOutput = RailGraphics->gl146Striped;
        break;

    default:
        GraphicOutput = TrackElement.GraphicPtr;
        break;
    }
    Utilities->CallLogPop(642);
    return(GraphicOutput);
}

// ---------------------------------------------------------------------------

TTrackElement &TTrack::TrackElementAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrackElementAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= TrackVector.size()))
    {
//        Utilities->CallLogPop(2281);  this shouldn't be here, introduced 02/06/21 at revision 3745fadb... with no explanation
        throw Exception("Out of Range Error, vector size: " + AnsiString(TrackVector.size()) + ", At: " + AnsiString(At) + " in TrackElementAt");
    }
    Utilities->CallLogPop(643);
    return(TrackVector.at(At));
}

// ---------------------------------------------------------------------------

TTrackElement &TTrack::InactiveTrackElementAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",InactiveTrackElementAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= InactiveTrackVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(InactiveTrackVector.size()) + ", At: " + AnsiString(At) +
                        " in InactiveTrackElementAt");
    }
    Utilities->CallLogPop(644);
    return(InactiveTrackVector.at(At));
}

// ---------------------------------------------------------------------------

bool TTrack::BlankElementAt(int Caller, int At) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",BlankElementAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= TrackVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(TrackVector.size()) + ", At: " + AnsiString(At) + " in BlankElementAt");
    }
    if(TrackVector.at(At).SpeedTag == 0) //have to use TrackVector.at because TrackElementAt is non-const
    {
        Utilities->CallLogPop(645);
        return(true);
    }
    else
    {
        Utilities->CallLogPop(646);
        return(false);
    }
}

// ---------------------------------------------------------------------------

bool TTrack::OneNamedLocationLongEnoughForSplit(int Caller, AnsiString LocationName)
/* Check sufficient elements with same ActiveTrackElementName linked together without any trailing point links to allow a train split.
      Only one length is needed to return true, but this doesn't mean that all platforms at the location are long enough.  When a
      split is required a specific check is made using ThisNamedLocationLongEnoughForSplit.
      Need at least two linked ActiveTrackElementNames, with connected elements at each end, which may or may not be ActiveTrackElementNames,
      and no connections via point trailing links.  Note that these conditions exclude opposed buffers since these not linked.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",OneNamedLocationLongEnoughForSplit," + LocationName);
    TTrackElement InactiveElement, FirstNamedElement, SecondNamedElement, FirstNamedLinkedElement, SecondNamedLinkedElement;
    int FirstNamedExitPos, SecondNamedExitPos, FirstNamedLinkedExitPos, SecondNamedLinkedEntryPos;
    TLocationNameMultiMapIterator SNIterator;
    TLocationNameMultiMapRange SNRange = LocationNameMultiMap.equal_range(LocationName);

    if(SNRange.first == SNRange.second)
    {
        Utilities->CallLogPop(972);
        return(false); // should have been caught earlier but include for completeness
    }
    for(SNIterator = SNRange.first; SNIterator != SNRange.second; SNIterator++)
    {
        if(SNIterator->second < 0)
        {
            continue; // exclude footcrossings
        }
        InactiveElement = InactiveTrackElementAt(47, SNIterator->second);
        if(InactiveElement.TrackType == Concourse)
        {
            continue; // only interested in locations where ActiveTrackElementName may be set (not needed at v2.10.0 but leave in)
        }
        if(!TrackElementPresentAtHV(1, InactiveElement.HLoc, InactiveElement.VLoc)) //added at v2.10.0 in response to Jason Bassett error notified 14/08/21
        {
            continue; // only interested in locations where ActiveTrackElementName may be set
        }
        THVPair HVPair;
        HVPair.first = InactiveElement.HLoc;
        HVPair.second = InactiveElement.VLoc;
        if(TrackMap.find(HVPair) == TrackMap.end())
        {
            throw Exception
                      ("Error - failed to find element in TrackMap for a non-concourse element in LocationNameMultiMap in OneNamedLocationLongEnoughForSplit (1)");
        }
        int TVPos = TrackMap.find(HVPair)->second;
        FirstNamedElement = TrackElementAt(560, TVPos);
        // first check linked on both sides, skip the check if not
        if((FirstNamedElement.Conn[0] == -1) || (FirstNamedElement.Conn[1] == -1))
        {
            continue;
        }
        // check if another ActiveTrackElementName connected via link pos 0 (can only be 0 or 1 since the only 2-track elements that can be
        // ActiveTrackElementNames are points and excluding trailing connections for points
        FirstNamedExitPos = 0;
        {
            SecondNamedElement = TrackElementAt(561, FirstNamedElement.Conn[FirstNamedExitPos]);
            SecondNamedExitPos = GetNonPointsOppositeLinkPos(FirstNamedElement.ConnLinkPos[FirstNamedExitPos]);
            FirstNamedLinkedElement = TrackElementAt(562, FirstNamedElement.Conn[1 - FirstNamedExitPos]);
            FirstNamedLinkedExitPos = FirstNamedElement.ConnLinkPos[1 - FirstNamedExitPos];
            if(SecondNamedElement.ActiveTrackElementName == LocationName) // success - check if it's connected on the far side
            {
                if(SecondNamedElement.Conn[SecondNamedExitPos] > -1)
                {
                    SecondNamedLinkedElement = TrackElementAt(563, SecondNamedElement.Conn[SecondNamedExitPos]);
                    SecondNamedLinkedEntryPos = SecondNamedElement.ConnLinkPos[SecondNamedExitPos];
                    if((SecondNamedLinkedElement.TrackType != Points) || (SecondNamedLinkedEntryPos != 3))
                    // success, now check FirstNamedElement link not trailing points & if so all OK
                    {
                        if((FirstNamedLinkedElement.TrackType != Points) || (FirstNamedLinkedExitPos != 3))
                        {
                            Utilities->CallLogPop(1002);
                            return(true);
                        }
                    }
                }
            }
        }
        // failed, try link 1
        FirstNamedExitPos = 1;
        {
            SecondNamedElement = TrackElementAt(564, FirstNamedElement.Conn[FirstNamedExitPos]);
            SecondNamedExitPos = GetNonPointsOppositeLinkPos(FirstNamedElement.ConnLinkPos[FirstNamedExitPos]);
            FirstNamedLinkedElement = TrackElementAt(565, FirstNamedElement.Conn[1 - FirstNamedExitPos]);
            FirstNamedLinkedExitPos = FirstNamedElement.ConnLinkPos[1 - FirstNamedExitPos];
            if(SecondNamedElement.ActiveTrackElementName == LocationName) // success - check if it's connected on the far side
            {
                if(SecondNamedElement.Conn[SecondNamedExitPos] > -1)
                {
                    SecondNamedLinkedElement = TrackElementAt(566, SecondNamedElement.Conn[SecondNamedExitPos]);
                    SecondNamedLinkedEntryPos = SecondNamedElement.ConnLinkPos[SecondNamedExitPos];
                    if((SecondNamedLinkedElement.TrackType != Points) || (SecondNamedLinkedEntryPos != 3))
                    // success, now check FirstNamedElement link not trailing points & if so all OK
                    {
                        if((FirstNamedLinkedElement.TrackType != Points) || (FirstNamedLinkedExitPos != 3))
                        {
                            Utilities->CallLogPop(1003);
                            return(true);
                        }
                    }
                }
            }
        }
    }
    Utilities->CallLogPop(1004);
    return(false);
}

// ---------------------------------------------------------------------------
bool TTrack::ThisNamedLocationLongEnoughForSplit(int Caller, AnsiString LocationName, int FirstNamedElementPos, int &SecondNamedElementPos,
                                                 int &FirstNamedLinkedElementPos, int &SecondNamedLinkedElementPos)
// for success need two linked named location elements, so that one element of each train can be at the location
// FirstNamedElementPos is the input vector position and the first (if successful) of the two linked named location elements,
// the second is SecondNamedElementPos, and the two linked elements are FirstNamedLinkedElementPos and SecondNamedLinkedElementPos.
// the two trains will occupy these 4 elements
// All are track vector positions, all but the input being references and set within the function.
{
/* Check sufficient elements (including TrackvectorPosition) with same ActiveTrackElementName linked together without any trailing point
      links and including the element FirstNamedElementPos to allow a train split.  Need at least two linked ActiveTrackElementNames, with
      connected elements at each end, which may or may not be ActiveTrackElementNames, and no connections via point trailing links.  Note that
      these conditions exclude opposed buffers since these not linked.  Return the two train positions and exit positions for use in train
      splitting.
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ThisNamedLocationLongEnoughForSplit," + LocationName +
                                 AnsiString(FirstNamedElementPos));
    TTrackElement InactiveElement, FirstNamedElement, SecondNamedElement, FirstNamedLinkedElement, SecondNamedLinkedElement;
    int FirstNamedExitPos, SecondNamedExitPos, FirstNamedLinkedExitPos, SecondNamedLinkedEntryPos;

    SecondNamedElementPos = -1;
    FirstNamedLinkedElementPos = -1;
    SecondNamedLinkedElementPos = -1;
    TLocationNameMultiMapIterator SNIterator;
    TLocationNameMultiMapRange SNRange = LocationNameMultiMap.equal_range(LocationName);

    if(SNRange.first == SNRange.second) // i.e. location name not in map
    {
        Utilities->CallLogPop(1005);
        return(false); // should have been caught earlier but include for completeness
    }
    for(SNIterator = SNRange.first; SNIterator != SNRange.second; SNIterator++)
    {
        if(SNIterator->second < 0)
        {
            continue; // exclude footcrossings
        }
        InactiveElement = InactiveTrackElementAt(69, SNIterator->second);
        if(InactiveElement.TrackType == Concourse)
        {
            continue; // only interested in locations where ActiveTrackElementName may be set
        }
        THVPair HVPair;
        HVPair.first = InactiveElement.HLoc;
        HVPair.second = InactiveElement.VLoc;
        if(TrackMap.find(HVPair) == TrackMap.end())
        {
            if(InactiveElement.TrackType == NamedNonStationLocation) // added at v2.2.0 to correct the error Xeon reported on 14/07/18.
            // If there is a NamedNonStationLocation without an associated active track element (effectively a non-station concourse)
            // then it won't be found in TrackMap but it's still legitimate.
            {
                continue;
            }
            else // for anything else throw the error
            {
                throw Exception
                          ("Error - failed to find element in TrackMap for a non-concourse element in LocationNameMultiMap in ThisNamedLocationLongEnoughForSplit (2)"
                          );
            }
        }
        int TVPos = TrackMap.find(HVPair)->second;
        if(TVPos != FirstNamedElementPos)
        {
            continue; // looking for an exact match
        }
        FirstNamedElement = TrackElementAt(567, TVPos);
        // first check linked on both sides, skip the check if not
        if((FirstNamedElement.Conn[0] == -1) || (FirstNamedElement.Conn[1] == -1))
        {
            continue;
        }
        // check if another ActiveTrackElementName connected via link pos 0 (can only be 0 or 1 since the only 2-track elements that can be
        // ActiveTrackElementNames are points and excluding trailing connections for points
        FirstNamedExitPos = 0;
        {
            SecondNamedElement = TrackElementAt(568, FirstNamedElement.Conn[FirstNamedExitPos]);
            SecondNamedExitPos = GetNonPointsOppositeLinkPos(FirstNamedElement.ConnLinkPos[FirstNamedExitPos]);
            FirstNamedLinkedElement = TrackElementAt(569, FirstNamedElement.Conn[1 - FirstNamedExitPos]);
            FirstNamedLinkedExitPos = FirstNamedElement.ConnLinkPos[1 - FirstNamedExitPos];
            if(SecondNamedElement.ActiveTrackElementName == LocationName) // success - check if it's connected on the far side
            {
                if(SecondNamedElement.Conn[SecondNamedExitPos] > -1)
                {
                    SecondNamedLinkedElement = TrackElementAt(570, SecondNamedElement.Conn[SecondNamedExitPos]);
                    SecondNamedLinkedEntryPos = SecondNamedElement.ConnLinkPos[SecondNamedExitPos];
                    if((SecondNamedLinkedElement.TrackType != Points) || (SecondNamedLinkedEntryPos != 3))
                    // success, now check FirstNamedElement link not trailing points & if so all OK
                    {
                        if((FirstNamedLinkedElement.TrackType != Points) || (FirstNamedLinkedExitPos != 3))
                        {
                            SecondNamedElementPos = FirstNamedElement.Conn[FirstNamedExitPos];
                            FirstNamedLinkedElementPos = FirstNamedElement.Conn[1 - FirstNamedExitPos];
                            SecondNamedLinkedElementPos = SecondNamedElement.Conn[SecondNamedExitPos];
                            Utilities->CallLogPop(1006);
                            return(true);
                        }
                    }
                }
            }
        }
        // failed, try link 1
        FirstNamedExitPos = 1;
        {
            SecondNamedElement = TrackElementAt(571, FirstNamedElement.Conn[FirstNamedExitPos]);
            SecondNamedExitPos = GetNonPointsOppositeLinkPos(FirstNamedElement.ConnLinkPos[FirstNamedExitPos]);
            FirstNamedLinkedElement = TrackElementAt(572, FirstNamedElement.Conn[1 - FirstNamedExitPos]);
            FirstNamedLinkedExitPos = FirstNamedElement.ConnLinkPos[1 - FirstNamedExitPos];
            if(SecondNamedElement.ActiveTrackElementName == LocationName) // success - check if it's connected on the far side
            {
                if(SecondNamedElement.Conn[SecondNamedExitPos] > -1)
                {
                    SecondNamedLinkedElement = TrackElementAt(573, SecondNamedElement.Conn[SecondNamedExitPos]);
                    SecondNamedLinkedEntryPos = SecondNamedElement.ConnLinkPos[SecondNamedExitPos];
                    if((SecondNamedLinkedElement.TrackType != Points) || (SecondNamedLinkedEntryPos != 3))
                    // success, now check FirstNamedElement link not trailing points & if so all OK
                    {
                        if((FirstNamedLinkedElement.TrackType != Points) || (FirstNamedLinkedExitPos != 3))
                        {
                            SecondNamedElementPos = FirstNamedElement.Conn[FirstNamedExitPos];
                            FirstNamedLinkedElementPos = FirstNamedElement.Conn[1 - FirstNamedExitPos];
                            SecondNamedLinkedElementPos = SecondNamedElement.Conn[SecondNamedExitPos];
                            Utilities->CallLogPop(1007);
                            return(true);
                        }
                    }
                }
            }
        }
    }
    Utilities->CallLogPop(1008);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::OneNamedLocationElementAtLocation(int Caller, AnsiString LocationName)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",OneNamedLocationElementAtLocation," + LocationName);
    TLocationNameMultiMapIterator SNIterator;
    TLocationNameMultiMapRange SNRange = LocationNameMultiMap.equal_range(LocationName);

    if(SNRange.first != SNRange.second)
    {
        for(SNIterator = SNRange.first; SNIterator != SNRange.second; SNIterator++)
        {
            if(SNIterator->second < 0)
            {
                continue; // only looking for inactive (platform or NamedNonStationLocation) elements
            }
            if((InactiveTrackElementAt(33, SNIterator->second).TrackType == Platform) || (InactiveTrackElementAt(81,
                                                                                                                 SNIterator->second).TrackType == NamedNonStationLocation))
            {
                Utilities->CallLogPop(1121);
                return(true);
            }
        }
    }
    Utilities->CallLogPop(848);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::PlatformOnSignalSide(int Caller, int HLoc, int VLoc, int SpeedTag, Graphics::TBitmap* &SignalPlatformGraphic)
{
// dropped special platforms at v0.6 as didn't show well against ground signals & not needed anyway as plats always plotted first where there are signals
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PlatformOnSignalSide," + AnsiString(HLoc) + "," + AnsiString(VLoc) +
                                 "," + AnsiString(SpeedTag));
    if(!IsPlatformOrNamedNonStationLocationPresent(5, HLoc, VLoc)) // can't be a named location so no ambiguity
    {
        Utilities->CallLogPop(949);
        return(false);
    }
    bool FoundFlag;
    TIMPair IMPair = GetVectorPositionsFromInactiveTrackMap(27, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        throw Exception("Error, FoundFlag false in PlatformOnSignalSide after IsPlatformOrNamedNonStationLocationPresent called successfully");
    }
    TTrackElement IAElement;

    if(SpeedTag == 68) // top sig
    {
        if((InactiveTrackElementAt(22, IMPair.first).SpeedTag == 76) || (InactiveTrackElementAt(23, IMPair.second).SpeedTag == 76)) // top plat
        {
            if(InactiveTrackElementAt(49, IMPair.first).SpeedTag == 76)
            {
                IAElement = InactiveTrackElementAt(50, IMPair.first);
            }
            else
            {
                IAElement = InactiveTrackElementAt(51, IMPair.second);
            }
            if(IAElement.LocationName == "")
            {
// SignalPlatformGraphic = RailGraphics->Plat68Striped;
                SignalPlatformGraphic = RailGraphics->gl76Striped;
            }
            else
            {
// SignalPlatformGraphic = RailGraphics->Plat68;
                SignalPlatformGraphic = RailGraphics->gl76;
            }
            Utilities->CallLogPop(950);
            return(true);
        }
    }
    else if(SpeedTag == 69) // bot sig
    {
        if((InactiveTrackElementAt(70, IMPair.first).SpeedTag == 77) || (InactiveTrackElementAt(75, IMPair.second).SpeedTag == 77)) // bot plat
        {
            if(InactiveTrackElementAt(76, IMPair.first).SpeedTag == 77)
            {
                IAElement = InactiveTrackElementAt(77, IMPair.first);
            }
            else
            {
                IAElement = InactiveTrackElementAt(78, IMPair.second);
            }
            if(IAElement.LocationName == "")
            {
// SignalPlatformGraphic = RailGraphics->Plat69Striped;
                SignalPlatformGraphic = RailGraphics->bm77Striped;
            }
            else
            {
// SignalPlatformGraphic = RailGraphics->Plat69;
                SignalPlatformGraphic = RailGraphics->bm77;
            }
            Utilities->CallLogPop(951);
            return(true);
        }
    }
    else if(SpeedTag == 70) // left sig
    {
        if((InactiveTrackElementAt(52, IMPair.first).SpeedTag == 78) || (InactiveTrackElementAt(79, IMPair.second).SpeedTag == 78)) // left plat
        {
            if(InactiveTrackElementAt(80, IMPair.first).SpeedTag == 78)
            {
                IAElement = InactiveTrackElementAt(55, IMPair.first);
            }
            else
            {
                IAElement = InactiveTrackElementAt(82, IMPair.second);
            }
            if(IAElement.LocationName == "")
            {
// SignalPlatformGraphic = RailGraphics->Plat70Striped;
                SignalPlatformGraphic = RailGraphics->bm78Striped;
            }
            else
            {
// SignalPlatformGraphic = RailGraphics->Plat70;
                SignalPlatformGraphic = RailGraphics->bm78;
            }
            Utilities->CallLogPop(952);
            return(true);
        }
    }
    else if(SpeedTag == 71) // right sig
    {
        if((InactiveTrackElementAt(83, IMPair.first).SpeedTag == 79) || (InactiveTrackElementAt(58, IMPair.second).SpeedTag == 79)) // right plat
        {
            if(InactiveTrackElementAt(84, IMPair.first).SpeedTag == 79)
            {
                IAElement = InactiveTrackElementAt(85, IMPair.first);
            }
            else
            {
                IAElement = InactiveTrackElementAt(86, IMPair.second);
            }
            if(IAElement.LocationName == "")
            {
// SignalPlatformGraphic = RailGraphics->Plat71Striped;
                SignalPlatformGraphic = RailGraphics->gl79Striped;
            }
            else
            {
// SignalPlatformGraphic = RailGraphics->Plat71;
                SignalPlatformGraphic = RailGraphics->gl79;
            }
            Utilities->CallLogPop(953);
            return(true);
        }
    }
    Utilities->CallLogPop(954);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::OtherTrainOnTrack(int Caller, int NextPos, int NextEntryPos, int OwnTrainID)
// returns true if another train on NextEntryPos track of element at NextPos, whether bridge or not
// false if not, if NextPos == -1, or if only own train on the track
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",OtherTrainOnTrack," + AnsiString(NextPos) + "," +
                                 AnsiString(NextEntryPos) + "," + AnsiString(OwnTrainID));
    if(NextEntryPos < 0)
    {
        Utilities->CallLogPop(1348);
        return(false);
    }
    TTrackElement TrackElement = TrackElementAt(713, NextPos);

    if(TrackElement.TrackType != Bridge)
    {
        Utilities->CallLogPop(1349);
        return ((TrackElement.TrainIDOnElement > -1) && (TrackElement.TrainIDOnElement != OwnTrainID));
    }
// bridge if reach here
    if(NextEntryPos > 1)
    {
        Utilities->CallLogPop(1350);
        return ((TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 > -1) && (TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 != OwnTrainID));
    }
    else
    {
        Utilities->CallLogPop(1351);
        return ((TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 > -1) && (TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 != OwnTrainID));
    }
}

// ---------------------------------------------------------------------------

TTrackElement &TTrack::SelectVectorAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SelectVectorAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= SelectVectorSize()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(TrackVector.size()) + ", At: " + AnsiString(At) + " in SelectVectorAt");
    }
    Utilities->CallLogPop(1483);
    return(SelectVector.at(At));
}

// ---------------------------------------------------------------------------

bool TTrack::IsATrackElementAdjacentToLink(int Caller, int HLocIn, int VLocIn, int LinkIn)
// For element at HLoc & VLoc, returns true if there is an element adjacent to LinkIn
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsATrackElementAdjacentToLink," + AnsiString(HLocIn) + "," +
                                 AnsiString(VLocIn) + "," + AnsiString(LinkIn));
    bool FoundFlag = false;
    int NewHLoc = HLocIn + LinkHVArray[LinkIn][0];
    int NewVLoc = VLocIn + LinkHVArray[LinkIn][1];

    GetVectorPositionFromTrackMap(41, NewHLoc, NewVLoc, FoundFlag);
    Utilities->CallLogPop(1538);
    return(FoundFlag);
}

// ---------------------------------------------------------------------------

bool TTrack::FindHighestLowestAndLeftmostNamedElements(int Caller, AnsiString Name, int &VPosHi, int &VPosLo, int &HPos)
{
// return true if find an inactive element called 'Name'
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindHighestAndLowestNamedElements," + Name);
    int VLocHi = -2000000000, VLocLo = 2000000000, HLoc = 2000000000;
    bool FoundFlag = false;

    for(unsigned int x = 0; x < InactiveTrackVector.size(); x++)
    {
        if(InactiveTrackElementAt(158, x).LocationName == Name)
        {
            FoundFlag = true;
            int V = InactiveTrackElementAt(159, x).VLoc;
            int H = InactiveTrackElementAt(160, x).HLoc;
            if(V > VLocHi)
            {
                VLocHi = V;
            }
            if(V < VLocLo)
            {
                VLocLo = V;
            }
            if(H < HLoc)
            {
                HLoc = H;
            }
        }
    }
    if(FoundFlag)
    {
        VPosHi = 16 * VLocHi;
        VPosLo = 16 * VLocLo;
        HPos = 16 * HLoc;
        Utilities->CallLogPop(1562);
        return(true);
    }
    else
    {
        Utilities->CallLogPop(1563);
        return(false);
    }
}

// ---------------------------------------------------------------------------

int TTrack::FindClosestLinkPosition(int Caller, int StartTVPosition, int EndTVPosition)
{
// return the link array position for the element at StartTVPosition that gives the closest link to the element at EndTVPosition
// NB the StartTVPosition is expected to be a single track element as only positions 0 & 1 are checked
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindClosestLinkPosition," + AnsiString(StartTVPosition) + "," +
                                 AnsiString(EndTVPosition));
    TTrackElement &StartElement = TrackElementAt(839, StartTVPosition);
    TTrackElement &EndElement = TrackElementAt(840, EndTVPosition);

// get H & V values for the element adjacent to Link[0] & Link[1]
    int NewHLocLink0 = StartElement.HLoc + LinkHVArray[StartElement.Link[0]][0];
    int NewVLocLink0 = StartElement.VLoc + LinkHVArray[StartElement.Link[0]][1];
    int NewHLocLink1 = StartElement.HLoc + LinkHVArray[StartElement.Link[1]][0];
    int NewVLocLink1 = StartElement.VLoc + LinkHVArray[StartElement.Link[1]][1];

// compute the sum of the squares of the H & V distances between EndElement and 'New' values
    int Link0Squares = ((EndElement.HLoc - NewHLocLink0) * (EndElement.HLoc - NewHLocLink0)) +
        ((EndElement.VLoc - NewVLocLink0) * (EndElement.VLoc - NewVLocLink0));
    int Link1Squares = ((EndElement.HLoc - NewHLocLink1) * (EndElement.HLoc - NewHLocLink1)) +
        ((EndElement.VLoc - NewVLocLink1) * (EndElement.VLoc - NewVLocLink1));

    if(Link0Squares <= Link1Squares)
    {
        Utilities->CallLogPop(1851);
        return(0);
    }
    else
    {
        Utilities->CallLogPop(1852);
        return(1);
    }
}

// ---------------------------------------------------------------------------

int TTrack::GetAnyElementOppositeLinkPos(int Caller, int TrackVectorPosition, int LinkPos, bool &Derail)
{
    // element can be points or any other type
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetAnyElementOppositeLinkPos," + AnsiString(TrackVectorPosition) + "," +
                                 AnsiString(LinkPos));
    Derail = false;
    TTrackElement &TE = Track->TrackElementAt(277, TrackVectorPosition);

    if((TE.TrackType == Points) && (TE.Config[LinkPos] == Lead))
    {
        if(TE.Attribute == 0)
        {
            Utilities->CallLogPop(663);
            return(1); // Att == 0 & ExitPos == 1 represent straight
        }
        else
        {
            Utilities->CallLogPop(664);
            return(3); // Att == 1 & ExitPos == 3 represent diverging
        }
    }
    else if((TE.TrackType == Points) && (TE.Config[LinkPos] == Trail))
    {
        if((LinkPos == 1) && (TE.Attribute == 0))
        {
            Utilities->CallLogPop(665);
            return(0); // Att == 0 represents straight
        }
        else if(LinkPos == 1)
        {
            Derail = true;
            Utilities->CallLogPop(666);
            return(0);
        }
        else if((LinkPos == 3) && (TE.Attribute == 1))
        {
            Utilities->CallLogPop(667);
            return(0);
        }
        else if(LinkPos == 3)
        {
            Derail = true;
            Utilities->CallLogPop(668);
            return(0);
        }
    }
    else if(LinkPos == 0)
    {
        Utilities->CallLogPop(669);
        return(1);
    }
    else if(LinkPos == 1)
    {
        Utilities->CallLogPop(670);
        return(0);
    }
    else if(LinkPos == 2)
    {
        Utilities->CallLogPop(671);
        return(3);
    }
    else if(LinkPos == 3)
    {
        Utilities->CallLogPop(672);
        return(2);
    }
    throw Exception("Error, failure in GetExitPos"); // should never reach here
}

// ----------------------------------------------------------------------------

void TTrack::PopulateLCVector(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PopulateLCVector");
    LCVector.clear();
    for(unsigned int x = 0; x < InactiveTrackVector.size(); x++)
    {
        if(InactiveTrackElementAt(161, x).TrackType == LevelCrossing)
        {
            LCVector.push_back(x);
        }
    }
    Utilities->CallLogPop(1931);
    return;
}

// ---------------------------------------------------------------------------

bool TTrack::TrainOnLink(int Caller, int HLoc, int VLoc, int Link, int &TrainID) // new at v1.2.0
/*
      Call GetVectorPositionFromTrackMap to identify the track element, then check if TrainIDOnElement > -1 (if a
      bridge then check relevant TrainID according to the Link), and if absent return false.  If present identify
      the train using TrainController->TrainVectorAtIdent, and check which bit on the element in question (Lead, Mid or Lag),
      and then check the relevant EntryPos & ExitPos for a match with Link.  If find a match return true and return the TrainID.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrainOnLink," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(Link));
    bool FoundFlag;

    TrainID = -1;
    int VecPos = GetVectorPositionFromTrackMap(47, HLoc, VLoc, FoundFlag);

    if(!FoundFlag)
    {
        Utilities->CallLogPop(2001);
        return(false);
    }
    TTrackElement TE = TrackElementAt(882, VecPos);

    TrainID = TE.TrainIDOnElement;
    if(TE.TrackType == Bridge)
    {
        if(TE.TrainIDOnElement > -1)
        {
            if((TE.Link[0] == Link) || (TE.Link[1] == Link))
            {
                TrainID = TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
            }
            else if((TE.Link[2] == Link) || (TE.Link[3] == Link))
            {
                TrainID = TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
            }
            else
            {
                TrainID = -1; // shouldn't ever reach here but be safe
            }
        }
    }
    if(TrainID == -1)
    {
        Utilities->CallLogPop(2002);
        return(false);
    }
// now get the train
    TTrain Train = TrainController->TrainVectorAtIdent(38, TrainID);

    if(Train.LinkOccupied(0, VecPos, Link)) // checks whether any part of train occupying Link on VecPos
    {
        Utilities->CallLogPop(2003);
        return(true);
    }
    TrainID = -1;
    Utilities->CallLogPop(2004);
    return(false);
}

// ---------------------------------------------------------------------------

bool TTrack::DiagonalFouledByTrain(int Caller, int HLoc, int VLoc, int DiagonalLinkNumber, int &TrainID)
/* New at v1.2.0
      As DiagonalFouledByRouteOrTarin but checks for a train only (may or may not be a route) and returns the ID number.    Enter with H & V set for the element whose diagonal
      is to be checked, and the XLink number of the relevant diagonal, which must be 1, 3, 7 or 9.
      for XLink = 1, potentially fouled diagonals are at H-1, V, Lk 3 & H, V-1, Lk 7
      for XLink = 3, potentially fouled diagonals are at H+1, V, Lk 1 & H, V-1 Lk 9
      for XLink = 7, potentially fouled diagonals are at H-1, V, Lk 9 & H, V+1 Lk 1
      for XLink = 9, potentially fouled diagonals are at H+1, V, Lk 7 & H, V+1 Lk 3
      Each of these is examined in turn for each route element in the relevant position.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DiagonalFouledByTrain," + AnsiString(HLoc) + "," + AnsiString(VLoc) +
                                 "," + AnsiString(DiagonalLinkNumber));
    TrainID = -1;
    TPrefDirElement TempPrefDirElement;
    TAllRoutes::TRouteElementPair FirstPair, SecondPair;

    if(((DiagonalLinkNumber == 1) && TrainOnLink(8, HLoc - 1, VLoc, 3, TrainID)) || ((DiagonalLinkNumber == 7) && TrainOnLink(9, HLoc - 1, VLoc, 9, TrainID)))
    {
        Utilities->CallLogPop(2027);
        return(true);
    }
    if(((DiagonalLinkNumber == 1) && TrainOnLink(10, HLoc, VLoc - 1, 7, TrainID)) || ((DiagonalLinkNumber == 3) && TrainOnLink(11, HLoc, VLoc - 1, 9, TrainID)))
    {
        Utilities->CallLogPop(2028);
        return(true);
    }
    if(((DiagonalLinkNumber == 3) && TrainOnLink(12, HLoc + 1, VLoc, 1, TrainID)) || ((DiagonalLinkNumber == 9) && TrainOnLink(13, HLoc + 1, VLoc, 7, TrainID)))
    {
        Utilities->CallLogPop(2029);
        return(true);
    }
    if(((DiagonalLinkNumber == 7) && TrainOnLink(14, HLoc, VLoc + 1, 1, TrainID)) || ((DiagonalLinkNumber == 9) && TrainOnLink(15, HLoc, VLoc + 1, 3, TrainID)))
    {
        Utilities->CallLogPop(2030);
        return(true);
    }
    Utilities->CallLogPop(2031);
    return(false);
}

// ---------------------------------------------------------------------------

void TTrack::SaveUserGraphics(int Caller, std::ofstream &VecFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveUserGraphics");
    Utilities->SaveFileInt(VecFile, UserGraphicVector.size()); // number of items
    TUserGraphicItem UGI;
    AnsiString JustFileName = "";

    for(unsigned int x = 0; x < UserGraphicVector.size(); x++)
    {
        UGI = UserGraphicVectorAt(17, x);
        int LastDelim = UGI.FileName.LastDelimiter('\\');
        if(LastDelim == 0) // can't find it so skip this item
        {
            continue;
        }
        else
        {
            JustFileName = UGI.FileName.SubString(LastDelim + 1, UGI.FileName.Length() - LastDelim);
        }
        Utilities->SaveFileString(VecFile, JustFileName);
        Utilities->SaveFileInt(VecFile, UGI.HPos);
        Utilities->SaveFileInt(VecFile, UGI.VPos);
    }
    Utilities->CallLogPop(2178);
}

// ---------------------------------------------------------------------------

int TTrack::NumberOfPlatforms(int Caller, AnsiString LocationName)
//checks all active track elements and lists those with ActiveTrackElementName same as LocationName in NamePosVector
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",NumberOfPlatforms," + LocationName);
    int NumPlats = 0;
    TTrackElement TempElement;
    int TempInt;

    typedef std::list<int> TNamePosList;
    TNamePosList NamePosList;
    typedef TNamePosList::iterator TNPLIt;
    TNPLIt NPLIt;
    typedef std::list<int> TOnePlatList;
    TOnePlatList OnePlatList;
    typedef TOnePlatList::iterator TOPLIt;
    TOPLIt OPLIt;

    NamePosList.clear();
    OnePlatList.clear();
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(988, x).ActiveTrackElementName == LocationName)
        {
            NamePosList.push_back(x);
        }
    }
    //NamePosList complete

    if(!NamePosList.empty())  //first value for the loop examination
    {
        OnePlatList.push_back(NamePosList.back());
        NamePosList.pop_back(); //erase from NPV as done with it here
    }
    while(!OnePlatList.empty()) //loop to examine all linked elements
    {
        TempInt = OnePlatList.front();
        TempElement = TrackElementAt(989, TempInt);

        NPLIt = find(NamePosList.begin(), NamePosList.end(), TempElement.Conn[0]);
        if(NPLIt != NamePosList.end() && ((TempElement.Link[0] == 2) || (TempElement.Link[0] == 4) || (TempElement.Link[0] == 6) || (TempElement.Link[0] == 8)))
        {
            OnePlatList.push_back(TempElement.Conn[0]);
            NamePosList.erase(NPLIt);
        }
        NPLIt = find(NamePosList.begin(), NamePosList.end(), TempElement.Conn[1]);
        if(NPLIt != NamePosList.end() && ((TempElement.Link[1] == 2) || (TempElement.Link[1] == 4) || (TempElement.Link[1] == 6) || (TempElement.Link[1] == 8)))
        {
            OnePlatList.push_back(TempElement.Conn[1]);
            NamePosList.erase(NPLIt);
        }
        //here when loaded any connecting links into OnePlatList, so can erase the front element
        OnePlatList.erase(OnePlatList.begin());
        if(OnePlatList.empty())
        {
            NumPlats++; //finished with current linked elements so can increment NumPlats
            if(!NamePosList.empty())
            {
                OnePlatList.push_back(NamePosList.back());  //ready for next iteration
                NamePosList.pop_back(); //erase from NPV as done with it there
            }
        }
    }
    Utilities->CallLogPop(2218);
    return(NumPlats);
}

// ---------------------------------------------------------------------------

void TTrack::RepairFailedSignals(TFailedElementVector::iterator FPVIt) //added at v2.13.0
{//repair Signals pointed to by FPVIt
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",RepairFailedSignals," + AnsiString(FPVIt->TVPos));
    TTrackElement &TE = Track->TrackElementAt(1516, FPVIt->TVPos);
    if(TE.TrackType != SignalPost)
    {
        throw Exception("Element at " + AnsiString(FPVIt->TVPos) + " not signal in RepairFailedSignals");
    }
    if(!TE.Failed)
    {
        throw Exception("Signals not failed at " + AnsiString(FPVIt->TVPos) + " in RepairFailedSignals");
    }
    TE.Failed = false;
    //set to correct aspect
    int RouteNumber;
    if(AllRoutes->GetRouteTypeAndNumber(40, FPVIt->TVPos, 0, RouteNumber) != TAllRoutes::NoRoute) //otherwise Attribute already 0 so will plot red
    {                                                                                   // 0 for LinkPos ok as a signal so only one track
        AllRoutes->AllRoutesVector.at(RouteNumber).SetRouteSignals(11);
    }
    //erase from vector
    Track->FailedSignalsVector.erase(FPVIt);

    Display->WarningLog(20, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Signal at " + TE.ElementID + " restored to full working order");
    PerfLogForm->PerformanceLog(43, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Signal at " + TE.ElementID + " restored to full working order");
    TrainController->StopTTClockMessage(130, "Signal at " + TE.ElementID + " restored to full working order.");
    AllRoutes->RebuildRailwayFlag = true;
    // to force ClearandRebuildRailway at next clock tick if not in zoom-out mode, to plot Signals without failed graphic
    Utilities->CallLogPop(2519);
}

// ---------------------------------------------------------------------------

void TTrack::RepairFailedPoints(TFailedElementVector::iterator FPVIt) //added at v2.13.0
{//repair points pointed to by FPVIt
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",RepairFailedPoints," + AnsiString(FPVIt->TVPos));
    TTrackElement &TE = Track->TrackElementAt(1505, FPVIt->TVPos);
    if(TE.TrackType != Points)
    {
        throw Exception("Element at " + AnsiString(FPVIt->TVPos) + " not points in RepairFailedPoints");
    }
    if(!TE.Failed)
    {
        throw Exception("Points not failed at " + AnsiString(FPVIt->TVPos) + " in RepairFailedPoints");
    }
    TE.Failed = false;
    TE.SpeedLimit01 = TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
    TE.SpeedLimit23 = TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
    TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 = -1;
    TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 = -1;
    //erase from vector
    Track->FailedPointsVector.erase(FPVIt);

    Display->WarningLog(15, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Points at " + TE.ElementID + " restored to full working order");
    PerfLogForm->PerformanceLog(38, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Points at " + TE.ElementID + " restored to full working order");
    TrainController->StopTTClockMessage(123, "Points at " + TE.ElementID + " restored to full working order.");
    AllRoutes->RebuildRailwayFlag = true;
    // to force ClearandRebuildRailway at next clock tick if not in zoom-out mode, to plot points without failed graphic
    Utilities->CallLogPop(2518);
}

// ---------------------------------------------------------------------------

void TTrack::RepairTSR(TFailedElementVector::iterator FPVIt) //added at v2.13.0
{//repair TSR pointed to by FPVIt
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",RepairTSR," + AnsiString(FPVIt->TVPos));
    TTrackElement &TE = Track->TrackElementAt(1535, FPVIt->TVPos);
    if(TE.TrackType != Simple)
    {
        throw Exception("Element at " + AnsiString(FPVIt->TVPos) + " not simple in RepairFailedPoints");
    }
    if(!TE.Failed)
    {
        throw Exception("No TSR at " + AnsiString(FPVIt->TVPos) + " in RepairTSR");
    }
    TE.Failed = false;
    TE.SpeedLimit01 = TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
    TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 = -1;
    //erase from vector
    Track->TSRVector.erase(FPVIt);

    Display->WarningLog(21, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Temporary Speed Restriction at " + TE.ElementID + " lifted, track restored to full working order");
    PerfLogForm->PerformanceLog(44, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Temporary Speed Restriction at " + TE.ElementID + " lifted, track restored to full working order");
    TrainController->StopTTClockMessage(131, "Temporary Speed Restriction at " + TE.ElementID + " lifted, track restored to full working order.");
    AllRoutes->RebuildRailwayFlag = true;
    // to force ClearandRebuildRailway at next clock tick if not in zoom-out mode, to plot points without failed graphic
    Utilities->CallLogPop(2520);
}

// ---------------------------------------------------------------------------

void TTrack::PopulateSimpleVector(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",PopulateSimpleVector");
    SimpleVector.clear();
    for(unsigned int x = 0; x < TrackVector.size(); x++)
    {
        if(TrackElementAt(1517, x).TrackType == Simple)
        {
            SimpleVector.push_back(int(x));
        }
    }
    Utilities->CallLogPop(2521);
}

// ---------------------------------------------------------------------------
// UserGraphic, PrefDir & Route functions
// ---------------------------------------------------------------------------

TUserGraphicItem &TTrack::UserGraphicVectorAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",UserGraphicVectorAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= UserGraphicVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(UserGraphicVector.size()) + ", At: " + AnsiString(At) + " in UserGraphicVectorAt");
    }
    Utilities->CallLogPop(2194);
    return(UserGraphicVector.at(At));
}

// ---------------------------------------------------------------------------

int TOnePrefDir::LastElementNumber(int Caller) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LastElementNumber,");
    int RetVal = PrefDirVector.size() - 1;

    if(RetVal < 0)
    {
        throw Exception("Return value negative in call to LastElementNumber");
    }
    Utilities->CallLogPop(114);
    return(RetVal);
}

// ---------------------------------------------------------------------------
TOnePrefDir::TPrefDirVectorIterator TOnePrefDir::LastElementPtr(int Caller)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LastElementPtr,");
    if(PrefDirVector.empty())
    {
        throw Exception("PrefDirVector empty in call to LastElementPtr");
    }
    TPrefDirVectorIterator RetIT = PrefDirVector.end() - 1;

    Utilities->CallLogPop(115);
    return(RetIT);
}

// ---------------------------------------------------------------------------
const TPrefDirElement &TOnePrefDir::GetFixedPrefDirElementAt(int Caller, int At) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetFixedPrefDirElementAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= PrefDirVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(PrefDirVector.size()) + ", At: " + AnsiString(At) + " in GetFixedPrefDirElementAt");
    }
    Utilities->CallLogPop(116);
    return(PrefDirVector.at(At));
}

// ---------------------------------------------------------------------------
TPrefDirElement &TOnePrefDir::GetModifiablePrefDirElementAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetModifiablePrefDirElementAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= PrefDirVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(PrefDirVector.size()) + ", At: " + AnsiString(At) +
                        " in GetModifiablePrefDirElementAt");
    }
    Utilities->CallLogPop(117);
    return(PrefDirVector.at(At));
}

// ---------------------------------------------------------------------------
const TPrefDirElement &TOnePrefDir::GetFixedSearchElementAt(int Caller, int At) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetFixedSearchElementAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= SearchVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(SearchVector.size()) + ", At: " + AnsiString(At) + " in GetFixedSearchElementAt");
    }
    Utilities->CallLogPop(118);
    return(SearchVector.at(At));
}

// ---------------------------------------------------------------------------
TPrefDirElement &TOnePrefDir::GetModifiableSearchElementAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetModifiableSearchElementAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= SearchVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(SearchVector.size()) + ", At: " + AnsiString(At) + " in GetModifiableSearchElementAt");
    }
    Utilities->CallLogPop(119);
    return(SearchVector.at(At));
}

// ---------------------------------------------------------------------------
bool TOnePrefDir::GetPrefDirStartElement(int Caller, int HLoc, int VLoc) // Return true if OK.
/*
      Enter with HLoc & VLoc set to selected element.  Clear PrefDirVector, check if selected element
      is a valid track element & return false if not.  Create a TPrefDirElement from the track element and
      set checkcount to 4 to cover the fixed values, then add to PrefDirVector.  All variable values are
      set in later functions.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetPrefDirStartElement," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    ClearPrefDir();
    int TrackVectorPosition;
    TTrackElement TrackElement;

    if(!(Track->FindNonPlatformMatch(5, HLoc, VLoc, TrackVectorPosition, TrackElement)))
    {
        Utilities->CallLogPop(126);
        return(false);
    }
/* it can be points so drop the code below - all exits are checked, no assumptions are made about the exit position of the start element
      if(TrackElement.TrackType == Points)
       {
       ShowMessage("Can't start on points");//because if PrefDir leads away from the leading edge
       //it isn't known which trailing edge is the required PrefDir - could use the straight as
       //default but may already be a PrefDir up to the diverging edge, then will have a mismatch,
       //best to prevent it to avoid problems
       Utilities->CallLogPop(127);
       return false;
       }
*/
    TPrefDirElement PrefDirElement(TrackElement);

    PrefDirElement.TrackVectorPosition = TrackVectorPosition;
    PrefDirElement.CheckCount = 4; // HLoc, VLoc, SpeedTag & TrackVectorPosition
    StorePrefDirElement(1, PrefDirElement); // enter first element
// Note that ELink not set even if a buffer or continuation - these set in
// ConvertPrefDirSearchVector after 2nd element added

    Utilities->CallLogPop(128);
    return(true);
}

// ---------------------------------------------------------------------------
bool TOnePrefDir::GetNextPrefDirElement(int Caller, int HLoc, int VLoc, bool &FinishElement)

/*
      Enter with HLoc & VLoc set to selected element.  If not a track element or if PrefDirVector empty return false.
      Examine the last element in the PrefDirVector, if ELink not set (start element) do an immediate
      check for an adjacent find (i.e. find selected element), & if succeed use SearchForPrefDir with that as XLinkPos to deal
      with setting the PrefDir vector, & return true.
      If last element was the start element but no immediate find, search on each valid exit pos in turn, using
      SearchForPrefDir to examine all branches.  If succeed set PrefDirVector.
      Otherwise (last element not start element) check if last element was a leading point (if so can't be first element)
      & check again for an immediate find on either XLinkPos values 1 & 3, using SearchForPrefDir &
      ConvertPrefDirSearchVector to set PrefDirVector.
      If a leading point but not an immediate find use SearchForPrefDir on the XLinkPos values 1 & 3 in turn.
      If it wasn't a leading point just use XLinkPos value corresponding to XLink & Search on that.  If don't
      find the required element return false.  CheckCount is used to keep track of set values to allow check later.
*/

{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetNextPrefDirElement," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    FinishElement = false;
    int TrackVectorPosition;

    TotalSearchCount = 0;
    TTrackElement TrackElement, TempTrackElement;

    if(PrefDirVector.size() == 0)
    {
        Utilities->CallLogPop(129);
        return(false);
    }
    if(!(Track->FindNonPlatformMatch(6, HLoc, VLoc, TrackVectorPosition, TrackElement)))
    {
        Utilities->CallLogPop(130);
        return(false);
    }
// set the search limits using the last stored element in PrefDirVector as the start point
// set the H&V limits for the search, all points on search path must lie within 15 elements greater than the box of which the line between
// start and finish is a diagonal line [dropped as not a good strategy because gaps interfered with direct line searches - instead
// introduced TotalSearchCount and now use that to limit searches. Leave in though in case rethink strategy later on]

    TPrefDirElement StartPrefDirElement = PrefDirVector.at(LastElementNumber(72));

    if(TrackElement.HLoc >= StartPrefDirElement.HLoc)
    {
        SearchLimitLowH = StartPrefDirElement.HLoc - 15;
        SearchLimitHighH = TrackElement.HLoc + 15;
    }
    else
    {
        SearchLimitLowH = TrackElement.HLoc - 15;
        SearchLimitHighH = StartPrefDirElement.HLoc + 15;
    }
    if(TrackElement.VLoc >= StartPrefDirElement.VLoc)
    {
        SearchLimitLowV = StartPrefDirElement.VLoc - 15;
        SearchLimitHighV = TrackElement.VLoc + 15;
    }
    else
    {
        SearchLimitLowV = TrackElement.VLoc - 15;
        SearchLimitHighV = StartPrefDirElement.VLoc + 15;
    }
/* dropped this for v0.4d - prevents ability to set paths for gaps that are widely separated, ok without it as search limited by SearchVector size
      check & TotalSearchCounts check
      if((abs(TrackElement.HLoc - StartPrefDirElement.HLoc) > 120) || (abs(TrackElement.VLoc - StartPrefDirElement.VLoc) > 120))
       {
       ShowMessage("Unable to reach the selected element - too far ahead");
       Utilities->CallLogPop(1692);
       return false;
       }
*/
// get last PrefDir element
    if(PrefDirVector.at(LastElementNumber(0)).ELink == -1) // start element
    {
        // check if TrackElement adjacent to any of the 4 XLinkPos'
        for(int x = 0; x < 4; x++)
        {
            if(PrefDirVector.at(LastElementNumber(1)).Conn[x] == TrackVectorPosition)
            {
                PrefDirVector.at(LastElementNumber(2)).XLinkPos = x;
                PrefDirVector.at(LastElementNumber(3)).XLink = PrefDirVector.at(LastElementNumber(4)).Link[x];
                PrefDirVector.at(LastElementNumber(5)).CheckCount++;
                PrefDirVector.at(LastElementNumber(6)).CheckCount++;
                break; // can have 2 connections if have 2 adjacent gaps connected to each other but ELink & XLink
                // then ambiguous.  Have to opt for just one, and if user wanted the other then that's unfortunate,
                // shouldn't ever get it in a serious railway though.
// Note:  ELink & ELinkPos are set in ConvertPrefDirSearchVector for the start element
            }
        }
        if(PrefDirVector.at(LastElementNumber(7)).XLinkPos > -1) // i.e required position must be adjacent to the start element
        {
            TempTrackElement = PrefDirVector.at(LastElementNumber(8));
            SearchVector.clear(); // use this & convert to set all PrefDir element values
            if(SearchForPrefDir(1, TempTrackElement, PrefDirVector.at(LastElementNumber(9)).XLinkPos, TrackVectorPosition))
            {
                ConvertPrefDirSearchVector(1);
                if((TrackElement.TrackType == Buffers) || (TrackElement.TrackType == Continuation))
                {
                    FinishElement = true;
                }
                Utilities->CallLogPop(131);
                return(true);
            }
        } // not an adjacent element

        // now check each of the 4 possible XLinkPos values
        for(int x = 0; x < 4; x++)
        {
            if((PrefDirVector.at(LastElementNumber(10)).Link[x] > 0) && (PrefDirVector.at(LastElementNumber(11)).Config[x] != End)) // i.e have somewhere to go
            {
                TotalSearchCount = 0; // added at v0.4f to give each exit direction a full chance to find the required position
                TempTrackElement = PrefDirVector.at(LastElementNumber(12));
                SearchVector.clear();
                if(SearchForPrefDir(2, TempTrackElement, x, TrackVectorPosition))
                {
                    ConvertPrefDirSearchVector(2);
                    if((TrackElement.TrackType == Buffers) || (TrackElement.TrackType == Continuation))
                    {
                        FinishElement = true;
                    }
                    Utilities->CallLogPop(132);
                    return(true);
                }
            }
        } // here if checked all possible exits without success
        ShowMessage(
            "Unable to find a route to the selected element - may be unreachable, too far ahead, or invalid. Try selecting an end point closer to the start point.");
        Utilities->CallLogPop(133);
        return(false);
    }
// dealt above with LastPrefDirElement being the start element (which can be points)

    if((PrefDirVector.at(LastElementNumber(13)).TrackType == Points) && (PrefDirVector.at(LastElementNumber(14)).Config[PrefDirVector.at(LastElementNumber(15))
                                                                                                                        .ELinkPos] == Lead)) // leading point
    {
        if(PrefDirVector.at(LastElementNumber(16)).Conn[1] == TrackVectorPosition) // found it next to XLinkPos = 1
        {
            PrefDirVector.at(LastElementNumber(17)).XLinkPos = 1;
            PrefDirVector.at(LastElementNumber(18)).XLink = PrefDirVector.at(LastElementNumber(19)).Link[1];
            // can't be buffers or gap if points
            PrefDirVector.at(LastElementNumber(20)).CheckCount++;
            PrefDirVector.at(LastElementNumber(21)).CheckCount++;
            TempTrackElement = PrefDirVector.at(LastElementNumber(22));
            SearchVector.clear();
            if(SearchForPrefDir(3, TempTrackElement, 1, TrackVectorPosition)) // bound to return true
            {
                ConvertPrefDirSearchVector(3);
                if((TrackElement.TrackType == Buffers) || (TrackElement.TrackType == Continuation))
                {
                    FinishElement = true;
                }
                Utilities->CallLogPop(134);
                return(true);
            }
        }
        if(PrefDirVector.at(LastElementNumber(23)).Conn[3] == TrackVectorPosition) // found it next to XLinkPos = 3
        {
            PrefDirVector.at(LastElementNumber(24)).XLinkPos = 3;
            PrefDirVector.at(LastElementNumber(25)).XLink = PrefDirVector.at(LastElementNumber(26)).Link[3];
            PrefDirVector.at(LastElementNumber(27)).CheckCount++;
            PrefDirVector.at(LastElementNumber(28)).CheckCount++;
            TempTrackElement = PrefDirVector.at(LastElementNumber(29));
            SearchVector.clear();
            if(SearchForPrefDir(4, TempTrackElement, 3, TrackVectorPosition)) // bound to return true
            {
                ConvertPrefDirSearchVector(4);
                if((TrackElement.TrackType == Buffers) || (TrackElement.TrackType == Continuation))
                {
                    FinishElement = true;
                }
                Utilities->CallLogPop(135);
                return(true);
            }
        }
// above dealt with immediate finds for leading point,
// now deal with ordinary searches for leading point
        PrefDirVector.at(LastElementNumber(30)).XLinkPos = 1;
        PrefDirVector.at(LastElementNumber(31)).XLink = PrefDirVector.at(LastElementNumber(32)).Link[1];
        PrefDirVector.at(LastElementNumber(33)).CheckCount++;
        PrefDirVector.at(LastElementNumber(34)).CheckCount++;
        TempTrackElement = PrefDirVector.at(LastElementNumber(35));
        SearchVector.clear();
        if(SearchForPrefDir(5, TempTrackElement, 1, TrackVectorPosition))
        {
            ConvertPrefDirSearchVector(5);
            if((TrackElement.TrackType == Buffers) || (TrackElement.TrackType == Continuation))
            {
                FinishElement = true;
            }
            Utilities->CallLogPop(136);
            return(true);
        }
        PrefDirVector.at(LastElementNumber(36)).XLinkPos = 3;
        PrefDirVector.at(LastElementNumber(37)).XLink = PrefDirVector.at(LastElementNumber(38)).Link[3];
        // note that CheckCount already increased to allow for XLinkPos & XLink
        TempTrackElement = PrefDirVector.at(LastElementNumber(39));
        SearchVector.clear();
        if(SearchForPrefDir(6, TempTrackElement, 3, TrackVectorPosition))
        {
            ConvertPrefDirSearchVector(6);
            if((TrackElement.TrackType == Buffers) || (TrackElement.TrackType == Continuation))
            {
                FinishElement = true;
            }
            Utilities->CallLogPop(137);
            return(true);
        }
// here if failed to find match for leading point
        PrefDirVector.at(LastElementNumber(69)).CheckCount--; // to removed the earlier increments for XLinkPos & XLink
        PrefDirVector.at(LastElementNumber(70)).CheckCount--;
        ShowMessage(
            "Unable to find a route to the selected element - may be unreachable, too far ahead, or invalid. Try selecting an end point closer to the start point.");
        Utilities->CallLogPop(138);
        return(false);
    }
// leading point fully dealt with above
// here with an ordinary element, just do an ordinary search - no need to search for an immediate find
// separately as covered in ordinary search.

    TempTrackElement = PrefDirVector.at(LastElementNumber(40));
    SearchVector.clear();
// no need to check for valid XLinkPos as not start element and not end element or would not reach here
    if(SearchForPrefDir(7, TempTrackElement, PrefDirVector.at(LastElementNumber(41)).XLinkPos, TrackVectorPosition))
    {
        ConvertPrefDirSearchVector(7);
        if((TrackElement.TrackType == Buffers) || (TrackElement.TrackType == Continuation))
        {
            FinishElement = true;
        }
        Utilities->CallLogPop(139);
        return(true);
    }
    ShowMessage(
        "Unable to find a route to the selected element - may be unreachable, too far ahead, or invalid. Try selecting an end point closer to the start point.");
    Utilities->CallLogPop(140);
    return(false); // failed to find required element
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::SearchForPrefDir(int Caller, TTrackElement CurrentTrackElement, int XLinkPos, int RequiredPosition)
/*
      Enter with CurrentTrackElement stored in the PrefDirVector, XLinkPos set to the link
      to search on, & SearchVector cleared unless entered recursively.  Function is a continuous loop that
      exits when find required element (returns true) or reaches a buffer or continuation or otherwise fails a search condition (returns false).
      Keep a count of entries in SearchVector during the current function call, so that this number can be
      erased for an unproductive branch search.
      Create a NextTrackElement from Current & XLinkPos as far as possible, & check if found required
      element.  If so save it & return true.  If not check if buffer, continuation, or earlier position
      in SearchVector or PrefDirVector, & if so erase all searchvector & return false.  If OK check if a leading point and
      if so do up to 2 recursive searches for the 2 exits.  If fail on both erase searchvector & return false.
      If not any of above, store element in searchvector, set the new current element values from the
      SearchElement, then go back to the while loop for the next step in the search.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SearchForPrefDir," + CurrentTrackElement.LogTrack(13) + "," +
                                 AnsiString(XLinkPos) + "," + AnsiString(RequiredPosition));
    int VectorCount = 0;

    while(true)
    {
        if(CurrentTrackElement.Config[XLinkPos] == End) // buffers or continuation
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(141);
            return(false);
        }
        int NextPosition = CurrentTrackElement.Conn[XLinkPos];
        TTrackElement NextTrackElement = Track->TrackElementAt(74, NextPosition);
        TPrefDirElement SearchElement(NextTrackElement);
        SearchElement.TrackVectorPosition = NextPosition;
        int NextELinkPos = CurrentTrackElement.ConnLinkPos[XLinkPos];
        SearchElement.ELinkPos = NextELinkPos;
        SearchElement.ELink = SearchElement.Link[SearchElement.ELinkPos];
        int NextXLinkPos;
        if(SearchElement.ELinkPos == 0)
        {
            NextXLinkPos = 1;
        }
        if(SearchElement.ELinkPos == 1)
        {
            NextXLinkPos = 0;
        }
        if(SearchElement.ELinkPos == 2)
        {
            NextXLinkPos = 3;
        }
        if(SearchElement.ELinkPos == 3)
        {
            NextXLinkPos = 2;
        }
        if((SearchElement.TrackType != Points) || (SearchElement.Config[SearchElement.ELinkPos] != Lead))
        {
            SearchElement.XLink = SearchElement.Link[NextXLinkPos];
            // but may be buffers, continuation or gap
            SearchElement.XLinkPos = NextXLinkPos;
        }
// can't set XLink or XLinkPos yet if the element is a leading point.
// check if found it
        if(SearchElement.TrackVectorPosition == RequiredPosition)
        {
            SearchVector.push_back(SearchElement); // XLink & XLinkPos won't be set if a leading point
            VectorCount++; // not really needed but include for tidyness
            TotalSearchCount++;
            Utilities->CallLogPop(142);
            return(true);
        }
// check if PrefDirVector > 200 and if so reject further searches (to avoid possible problems in converting
// very long vectors) - warning given in ConvertPrefDirSearchVector, though can still add elements one
// at a time - drop this
/*
       if(PrefDirVector.size() > 200)
           {
           for(int x=0;x<VectorCount;x++) SearchVector.erase(SearchVector.end() - 1);
           Utilities->CallLogPop(1419);
           return false;
           }
*/
// check if a buffer or continuation
        if((SearchElement.TrackType == Buffers) || (SearchElement.TrackType == Continuation))
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(143);
            return(false);
        }
// check if reached an earlier position on search PrefDir with same entry value
        for(unsigned int x = 0; x < SearchVector.size(); x++)
        {
            if((SearchElement.TrackVectorPosition == SearchVector.at(x).TrackVectorPosition) && (SearchElement.ELink == SearchVector.at(x).ELink))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(144);
                return(false);
            }
        }
// check if reached an earlier position in the PrefDirVector with same entry value (without this can keep adding entries
// to PrefDir4MultiMap, and since only 4 are searched an error can occur)
        for(unsigned int x = 0; x < PrefDirVector.size(); x++)
        {
            if((SearchElement.TrackVectorPosition == PrefDirVector.at(x).TrackVectorPosition) && (SearchElement.ELink == PrefDirVector.at(x).ELink))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(1417);
                return(false);
            }
        }

// check if exceeds the search H & V limits - drop in favour of limiting TotalSearchCount
// if((SearchElement.HLoc > SearchLimitHighH) || (SearchElement.HLoc < SearchLimitLowH) ||
// (SearchElement.VLoc > SearchLimitHighV) ||(SearchElement.VLoc < SearchLimitLowV))
        if(TotalSearchCount > PrefDirSearchLimit)
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1691);
            return(false);
        }
// check if SearchVector reached 150, and if so reject, to save time in searching for PrefDirs
        if(SearchVector.size() > 150)
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1418);
            return(false);
        }
// check if reached a leading point
        if((SearchElement.TrackType == Points) && (SearchElement.Config[SearchElement.ELinkPos] == Lead))
        {
// push element with XLink set to position [1]
            SearchElement.XLink = SearchElement.Link[1];
            SearchElement.XLinkPos = 1;
            SearchVector.push_back(SearchElement);
            VectorCount++;
            TotalSearchCount++;
            // recursive search at XLinkPos of 1 (i.e. 1st trailing exit)
            // Note that have to use a TTrackElement in the recursive search, so SearchElement
            // can't be used.  NextTrackElement is the corresponding TTrackElement.
            if(SearchForPrefDir(8, NextTrackElement, 1, RequiredPosition))
            {
                Utilities->CallLogPop(145);
                return(true);
            }
            else
            {
// remove leading point with XLinkPos [1]
                SearchVector.erase(SearchVector.end() - 1);
                VectorCount--;
// push element with XLink set to position [3]
                SearchElement.XLink = SearchElement.Link[3];
                SearchElement.XLinkPos = 3;
                SearchVector.push_back(SearchElement);
                VectorCount++;
                TotalSearchCount++;
// recursive search at XLinkPos of 3 (i.e. 2nd trailing exit)
                if(SearchForPrefDir(9, NextTrackElement, 3, RequiredPosition))
                {
                    Utilities->CallLogPop(146);
                    return(true);
                }
                else
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(147);
                    return(false);
                }
            }
        } // if leading point

// here if ordinary element, push it, inc vector & update CurrentTrackElement
// ready for next element on PrefDir
        SearchVector.push_back(SearchElement);
        VectorCount++;
        TotalSearchCount++;
        XLinkPos = NextXLinkPos;
        CurrentTrackElement = SearchElement;
    } // while(true)
}

// ---------------------------------------------------------------------------

void TOnePrefDir::ConvertPrefDirSearchVector(int Caller)
/*
      Enter with SearchVector established.  This contains ELink + Pos, XLink + Pos, & TrackVectorPosition
      for each element on the search PrefDir, though if the last element is a leading point
      then the final XLink won't be set.
      Note also that the last element in the PrefDirVector (as opposed to the searchvector) may not have its ELink set (if it was the start)
      nor its XLink set (if it was the start or a leading point), so these are checked first and together with EXNumber set as necessary.
      The remaining PrefDirVector elements are then set from the searchvector & checkcount keeps pace as values are added.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ConvertPrefDirSearchVector");
    if(SearchVector.size() == 0)
    {
        throw Exception("Error, SearchVector empty");
    }
// get first SearchElement in order to set last PrefDirelement
    TPrefDirElement SearchElement = SearchVector.at(0);

// set last PrefDir element XLink & ELink values if not already set
// ELink & XLink not set if was first element in PrefDir; XLink also not set if was a leading point
    for(int x = 0; x < 4; x++)
    {
        if(PrefDirVector.at(LastElementNumber(42)).Conn[x] == SearchElement.TrackVectorPosition)
        {
            if(PrefDirVector.at(LastElementNumber(43)).XLink == -1) // i.e. not set
            {
                PrefDirVector.at(LastElementNumber(44)).XLink = PrefDirVector.at(LastElementNumber(45)).Link[x];
                PrefDirVector.at(LastElementNumber(46)).XLinkPos = x;
                PrefDirVector.at(LastElementNumber(47)).CheckCount++;
                PrefDirVector.at(LastElementNumber(48)).CheckCount++;
            }
            int ELinkPos;
            if(PrefDirVector.at(LastElementNumber(49)).XLinkPos == 0)
            {
                ELinkPos = 1; // use actual value rather than 'x' as may be a gap
            }
            // with both ends linked to 1st searchvector element, & if XLink was set then x may not correspond
            if(PrefDirVector.at(LastElementNumber(50)).XLinkPos == 1)
            {
                ELinkPos = 0;
            }
            if(PrefDirVector.at(LastElementNumber(51)).XLinkPos == 2)
            {
                ELinkPos = 3;
            }
            if(PrefDirVector.at(LastElementNumber(52)).XLinkPos == 3)
            {
                ELinkPos = 2;
            }
            if(PrefDirVector.at(LastElementNumber(53)).ELink == -1) // because was start element
            {
                PrefDirVector.at(LastElementNumber(54)).ELink = PrefDirVector.at(LastElementNumber(55)).Link[ELinkPos];
                PrefDirVector.at(LastElementNumber(56)).ELinkPos = ELinkPos;
                PrefDirVector.at(LastElementNumber(57)).CheckCount++;
                PrefDirVector.at(LastElementNumber(58)).CheckCount++;
            }
            break; // no point going any further
        }
    }
// set EXNumber for last PrefDir element, unless already set
// won't be set if was first element or a leading point
    if(PrefDirVector.at(LastElementNumber(59)).EXNumber == -1)
    {
/* The order for entries & exits is as follows (1st no = entry, 2nd = exit):-
      int EXArray[32][2] = {
      {4,6},{2,8},                                      //horizontal & vertical
      {2,4},{6,2},{8,6},{4,8},                          //sharp curves
      {1,6},{3,8},{9,4},{7,2},{1,8},{3,4},{9,2},{7,6},  //loose curves
      {1,9},{3,7}                                       //forward & reverse diagonals
*/

        if(!(PrefDirVector.at(LastElementNumber(60)).EntryExitNumber()))
        {
            throw Exception("Error in EntryExitNumber 1");
        }
        PrefDirVector.at(LastElementNumber(61)).EXGraphicPtr = PrefDirVector.at(LastElementNumber(62)).GetPrefDirGraphicPtr();
        PrefDirVector.at(LastElementNumber(63)).EntryDirectionGraphicPtr = PrefDirVector.at(LastElementNumber(64)).GetDirectionPrefDirGraphicPtr();
        PrefDirVector.at(LastElementNumber(65)).CheckCount++;
    }
// Last PrefDir element now complete

// construct remaining PrefDir elements from searchvector
    for(unsigned int x = 0; x < SearchVector.size(); x++)
    {
        SearchElement = SearchVector.at(x);
        TPrefDirElement PrefDirElement(Track->TrackElementAt(75, SearchElement.TrackVectorPosition));
        PrefDirElement.TrackVectorPosition = SearchElement.TrackVectorPosition;
        PrefDirElement.ELink = SearchElement.ELink;
        PrefDirElement.ELinkPos = SearchElement.ELinkPos;
        PrefDirElement.XLink = SearchElement.XLink;
        PrefDirElement.XLinkPos = SearchElement.XLinkPos;
// if XLink & XLinkPos not set don't account for them in CheckCount
        if(PrefDirElement.XLink == -1)
        {
            PrefDirElement.CheckCount = 6; // Hloc, VLoc, SpeedTag, ELink, ELinkPos,
        }
        // & TrackVectorPosition
        else
        {
            PrefDirElement.CheckCount = 8; // Hloc, VLoc, SpeedTag, ELink, ELinkPos,
        }
        // XLink, XLinkPos, TrackVectorPosition

// set EXNumber (can't set EXNumber if XLink not set - if finished on a leading point
        if(PrefDirElement.XLink != -1)
        {
            if(!(PrefDirElement.EntryExitNumber()))
            {
                throw Exception("Error in EntryExitNumber 2");
            }
            PrefDirElement.EXGraphicPtr = PrefDirElement.GetPrefDirGraphicPtr();
            PrefDirElement.EntryDirectionGraphicPtr = PrefDirElement.GetDirectionPrefDirGraphicPtr();
            PrefDirElement.CheckCount++;
            // all values now incorporated if not a leading point
        }
// store PrefDir element
        StorePrefDirElement(2, PrefDirElement);
    }
// Can now validate if PrefDir finished, i.e. if buffers or continuation, else validate when 'AddPrefDir' button pressed
    if((LastElementPtr(0)->TrackType == Buffers) || (LastElementPtr(1)->TrackType == Continuation))
    {
        if(ValidatePrefDir(2))
        {
            ;
        } // error messages given within function

    }
    CheckPrefDir4MultiMap(3);
/* drop this, check dropped from search
      if(PrefDirVector.size() > 200)
       {
       ShowMessage("The selected track segment is becoming too long, until it is accepted further elements can only be added one at a time");
       }
*/
    Utilities->CallLogPop(148);
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::EndPossible(int Caller, bool &LeadingPoints)
/*
      Return true if selected element is valid as a PrefDir end element, i.e. isn't leading points,
      and PrefDir isn't one element long.  Used to enable the AddPrefDirButton during PrefDir building.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EndPossible");
    LeadingPoints = false;
    if(PrefDirVector.empty())
    {
        Utilities->CallLogPop(1786);
        return(false); // should never be empty but allow for it for safety
    }
    if(PrefDirVector.size() == 1)
    {
        Utilities->CallLogPop(149);
        return(false); // can't end if only one element
    }
/*
      if((PrefDirVector.at(LastElementNumber()).TrackType != Points) &&
           (PrefDirVector.at(LastElementNumber()).TrackType != Crossover))
       {
       Utilities->CallLogPop(150);
       return true;
       }
*/
// allow for anything but leading points
    if((PrefDirVector.at(LastElementNumber(66)).TrackType != Points) || (PrefDirVector.at(LastElementNumber(67)).ELinkPos == 1) ||
       (PrefDirVector.at(LastElementNumber(71)).ELinkPos == 3))
    {
        Utilities->CallLogPop(1776);
        return(true);
    }
    else
    {
        LeadingPoints = true;
        Utilities->CallLogPop(151);
        return(false);
    }
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::ValidatePrefDir(int Caller)
/*
      Checks that all elements in PrefDirVector have been properly set, i.e. don't have their default values,
      and that every element is connected to the next element
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ValidatePrefDir");
    int Position;
    AnsiString ErrorString;
    bool Error = false;

    for(unsigned int x = 0; x < PrefDirVector.size(); x++)
    {
        if(PrefDirVector.at(x).HLoc == -2000000000)
        {
            Error = true;
            ErrorString = "HLoc";
            Position = x;
        }
        if(PrefDirVector.at(x).VLoc == -2000000000)
        {
            Error = true;
            ErrorString = "VLoc";
            Position = x;
        }
        if(PrefDirVector.at(x).ELink == -1)
        {
            Error = true;
            ErrorString = "ELink";
            Position = x;
        }
        if(PrefDirVector.at(x).ELinkPos == -1)
        {
            Error = true;
            ErrorString = "ELinkPos";
            Position = x;
        }
        if(PrefDirVector.at(x).XLink == -1)
        {
            Error = true;
            ErrorString = "XLink";
            Position = x;
        }
        if(PrefDirVector.at(x).XLinkPos == -1)
        {
            Error = true;
            ErrorString = "XLinkPos";
            Position = x;
        }
        if(PrefDirVector.at(x).SpeedTag == 0)
        {
            Error = true;
            ErrorString = "Tag";
            Position = x;
        }
        if(PrefDirVector.at(x).TrackVectorPosition == -1)
        {
            Error = true;
            ErrorString = "TrackVectorPosition";
            Position = x;
        }
        if(PrefDirVector.at(x).EXNumber == -1)
        {
            Error = true;
            ErrorString = "EXNumber";
            Position = x;
        }
        if(PrefDirVector.at(x).CheckCount != 9)
        // HLoc, VLoc, SpeedTag, TrackVectorPosition, ELink, ELinkPos, XLink, XLinkPos & EXNumber
        {
            Error = true;
            ErrorString = "CheckCount";
            Position = x;
        }
// extra checks
        if(PrefDirVector.at(x).EXGraphicPtr == 0)
        {
            Error = true;
            ErrorString = "EntryGraphicPtr";
            Position = x;
        }
        if(PrefDirVector.at(x).EntryDirectionGraphicPtr == 0)
        {
            Error = true;
            ErrorString = "EntryDirectionGraphicPtr";
            Position = x;
        }
// end of extra checks
        if(x > 0)
        {
            if(PrefDirVector.at(x - 1).Conn[PrefDirVector.at(x - 1).XLinkPos] != PrefDirVector.at(x).TrackVectorPosition)
            {
                Error = true;
                ErrorString = "Last XLink not connected to this element";
                Position = x;
            }
        }
    }
    if(Error)
    {
        throw Exception("Error at " + AnsiString(Position) + " " + ErrorString);
    }
    else
    {
        Utilities->CallLogPop(153);
        return(true);
    }
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::GetPrefDirTruncateElement(int Caller, int HLoc, int VLoc)
/*
      This is only called during PrefDir build or distance setting.  It truncates at & including the first element in the PrefDir vector
      that matches H & V.  After the truncate the final element of the remaining PrefDir has its data members reset
      to the same defaults as would be the case if the PrefDir had been built up to that point - i.e. for first element
      or a leading point.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetPrefDirTruncateElement," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    for(unsigned int x = 0; x < (PrefDirVector.size()); x++)
    {
        if((PrefDirVector.at(x).HLoc == HLoc) && (PrefDirVector.at(x).VLoc == VLoc))
        {
            for(int PrefDirVecPos = (PrefDirVector.size() - 1); PrefDirVecPos >= (int)x; PrefDirVecPos--) // has to be int or will underflow at x==0
            {
                ErasePrefDirElementAt(1, PrefDirVecPos);
            }
            if(PrefDirVector.size() == 0)
            {
                Utilities->CallLogPop(154);
                return(true);
            }
            if(PrefDirVector.size() == 1)
            {
                PrefDirVector.at(x - 1).ELinkPos = -1;
                PrefDirVector.at(x - 1).ELink = -1;
                PrefDirVector.at(x - 1).XLinkPos = -1;
                PrefDirVector.at(x - 1).XLink = -1;
                PrefDirVector.at(x - 1).EXNumber = -1;
                PrefDirVector.at(x - 1).EXGraphicPtr = 0;
                PrefDirVector.at(x - 1).EntryDirectionGraphicPtr = 0;
                PrefDirVector.at(x - 1).CheckCount = PrefDirVector.at(x - 1).CheckCount - 5;
                Utilities->CallLogPop(155);
                return(true);
            }
            // here with truncate element not first element, so ELink & ELinkPos set
            // unset XLink & Pos if a leading point
            if(PrefDirVector.at(x - 1).Config[PrefDirVector.at(x - 1).ELinkPos] == Lead)
            {
                PrefDirVector.at(x - 1).XLinkPos = -1;
                PrefDirVector.at(x - 1).XLink = -1;
                PrefDirVector.at(x - 1).EXNumber = -1;
                PrefDirVector.at(x - 1).EXGraphicPtr = 0;
                PrefDirVector.at(x - 1).EntryDirectionGraphicPtr = 0;
                PrefDirVector.at(x - 1).CheckCount = PrefDirVector.at(x - 1).CheckCount - 3;
                Utilities->CallLogPop(156);
                return(true);
            }
            Utilities->CallLogPop(157);
            return(true);
        }
    }
    Utilities->CallLogPop(158);
    return(false);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::PrefDirMarker(int Caller, TPrefDirRoute PrefDirRoute, bool BuildingPrefDir, TDisplay *Disp)
const     // PrefDirRoute = PrefDircall or routecall for PrefDir or route; true for BuildingPrefDir
/*
      PrefDir and route track marker, including direction markers.  Function used for both PrefDirs (PrefDirRoute == PrefDirCall) and routes
      (PrefDirRoute == RouteCall).  The graphics for marker colours and direction are already stored in all PrefDirElements in
      TOnePrefDir and TOneRoute, and this function is called to display them, all in the case of a PrefDir, but for a route only the
      first and last elements have direction markers.  No markers are displayed if a train is present on an element.  Also no
      display if EXGraphicPtr not set.  If building a PrefDir (BuildingPrefDir true) then the start and end rectangles are also
      displayed.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PrefDirMarker," + AnsiString(PrefDirRoute) + "," +
                                 AnsiString((short)BuildingPrefDir));
    int HPos, VPos;

    if(PrefDirSize() == 0)
    {
        Utilities->CallLogPop(159);
        return;
    }
    for(unsigned int x = 0; x < PrefDirSize(); x++)
    {
        TPrefDirElement TempPrefDirElement = PrefDirVector.at(x);
// if(Track->TrackElementAt(76, TempPrefDirElement.TrackVectorPosition).TrainIDOnElement > -1) continue;
// don't plot route element if train present - dropped above as train departing only replotted the part of the route
// that the train was on.  Ensure though that whenever plot a route replot trains after else route will overwrite train
        // without the above, if route replotted in ClearandRebuildRailway when train is straddling 3 elements
        // and before the next train update, then the route element corresponding to the LagElement will be plotted,
        // only the front half of which will be overplotted by the back of the train, then when the train is
        // updated the route image will remain plotted and stay on screen until a later ClearandRebuildRailway
        if(TempPrefDirElement.EXGraphicPtr != 0) // Note:  will be 0 if first element or last as leading point
        {
            Disp->PlotOutput(12, (TempPrefDirElement.HLoc * 16), (TempPrefDirElement.VLoc * 16), TempPrefDirElement.EXGraphicPtr);
            if((TempPrefDirElement.EntryDirectionGraphicPtr != 0) && (PrefDirRoute == PrefDirCall)) // PrefDir
            {
                Disp->PlotOutput(13, (TempPrefDirElement.HLoc * 16), (TempPrefDirElement.VLoc * 16), TempPrefDirElement.EntryDirectionGraphicPtr);
            }
            else if((TempPrefDirElement.EntryDirectionGraphicPtr != 0) && (PrefDirRoute == RouteCall) && PrefDirSize() > 1)
            // Route, no direction if a single element
            {
                if(x == 0)
                {
                    Disp->PlotOutput(14, (TempPrefDirElement.HLoc * 16), (TempPrefDirElement.VLoc * 16), TempPrefDirElement.EntryDirectionGraphicPtr);
                }
                if(x == (PrefDirSize() - 1))
                {
                    Disp->PlotOutput(15, (TempPrefDirElement.HLoc * 16), (TempPrefDirElement.VLoc * 16), TempPrefDirElement.EntryDirectionGraphicPtr);
                }
            }
        }
    }

// set start & end element colours if building a PrefDir
    if((PrefDirRoute == PrefDirCall) && BuildingPrefDir)
    {
        HPos = GetFixedPrefDirElementAt(4, 0).HLoc * 16;
        VPos = GetFixedPrefDirElementAt(5, 0).VLoc * 16;
        Disp->Rectangle(1, HPos, VPos, clB0G0R5, 2, 2); // medium red rectangle
        // set last element colour
        if(PrefDirSize() > 1)
        {
            unsigned int LatestPos = PrefDirSize() - 1;
            HPos = GetFixedPrefDirElementAt(6, LatestPos).HLoc * 16;
            VPos = GetFixedPrefDirElementAt(7, LatestPos).VLoc * 16;
            Disp->Rectangle(2, HPos, VPos, clB5G0R0, 4, 2); // smaller blue rectangle
        }
    }
    Disp->Update();
    Utilities->CallLogPop(160);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::EveryPrefDirMarker(int Caller, TDisplay *Disp)
/*
      Similar to PrefDirMarker but used only to mark EveryPrefDir - red for unidirectional PrefDir & green for bidirectional
      Colours taken from the route colours.  Plot red first so green overwrites for bidirectional points.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EveryPrefDirMarker");
    if(PrefDirSize() == 0)
    {
        Utilities->CallLogPop(1547);
        return;
    }
    int H, V, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3;
    bool FoundFlag;
    TPrefDir4MultiMapIterator MMIT = PrefDir4MultiMap.begin();
    TPrefDirElement PrefDirElement0, PrefDirElement1, PrefDirElement2, PrefDirElement3;

    while(MMIT != PrefDir4MultiMap.end())
    {
        H = MMIT->first.first;
        V = MMIT->first.second;
        GetVectorPositionsFromPrefDir4MultiMap(6, H, V, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
        // always found in order, any missing have PrefDirPosx == -1
        if(PrefDirPos0 > -1)
        {
            PrefDirElement0 = GetFixedPrefDirElementAt(170, PrefDirPos0); // PrefDirPos0 should always be > -1 but leave as a precaution
        }
        if(PrefDirPos1 > -1)
        {
            PrefDirElement1 = GetFixedPrefDirElementAt(171, PrefDirPos1);
        }
        if(PrefDirPos2 > -1)
        {
            PrefDirElement2 = GetFixedPrefDirElementAt(172, PrefDirPos2);
        }
        if(PrefDirPos3 > -1)
        {
            PrefDirElement3 = GetFixedPrefDirElementAt(173, PrefDirPos3);
        }
        if(PrefDirPos3 > -1) // 4 found, mark all PrefDirs bidirectional (operator == ensures no duplicates in ConsolidatePrefDirs)
        {
            // need to plot all 4 in order to obtain all the direction graphics
            Disp->PlotOutput(77, (H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
            Disp->PlotOutput(78, (H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
            Disp->PlotOutput(79, (H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
            Disp->PlotOutput(80, (H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
            Disp->PlotOutput(81, (H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, true)); // green
            Disp->PlotOutput(82, (H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, true)); // green
            Disp->PlotOutput(83, (H * 16), (V * 16), PrefDirElement3.GetRouteGraphicPtr(false, true)); // green
            Disp->PlotOutput(84, (H * 16), (V * 16), PrefDirElement3.GetDirectionRouteGraphicPtr(false, true)); // green
            MMIT++;
            MMIT++;
            MMIT++;
            MMIT++;
        }
        else if(PrefDirPos2 > -1) // 3 found, one PrefDir bidirectional & other unidirectional
        {
            if(PrefDirElement0.EXNumber == PrefDirElement1.EXNumber)
            {
                // 0 & 1 constitute the bidirectional PrefDir
                Disp->PlotOutput(89, (H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(90, (H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(85, (H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(86, (H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(87, (H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(88, (H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
                MMIT++;
                MMIT++;
                MMIT++;
            }
            else if(PrefDirElement0.EXNumber == PrefDirElement2.EXNumber)
            {
                // 0 & 2 constitute the bidirectional PrefDir
                Disp->PlotOutput(95, (H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(96, (H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(91, (H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(92, (H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(93, (H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(94, (H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, true)); // green
                MMIT++;
                MMIT++;
                MMIT++;
            }
            else
            {
                // 1 & 2 constitute the bidirectional PrefDir
                Disp->PlotOutput(101, (H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(102, (H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(97, (H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(98, (H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(99, (H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(100, (H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, true)); // green
                MMIT++;
                MMIT++;
                MMIT++;
            }
        }
        else if(PrefDirPos1 > -1) // 2 found, either 1 bidirectional or 2 unidirectional
        {
            if(PrefDirElement0.EXNumber == PrefDirElement1.EXNumber)
            {
                // 0 & 1 constitute the bidirectional PrefDir
                Disp->PlotOutput(103, (H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(104, (H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(105, (H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
                Disp->PlotOutput(106, (H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
                MMIT++;
                MMIT++;
            }
            else
            {
                // 2 unidirectional PrefDirs
                Disp->PlotOutput(107, (H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(108, (H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(109, (H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, false)); // red
                Disp->PlotOutput(110, (H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, false)); // red
                MMIT++;
                MMIT++;
            }
        }
        else if(PrefDirPos0 > -1) // 1 found, must be unidirectional
        {
            Disp->PlotOutput(111, (H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, false)); // red
            Disp->PlotOutput(112, (H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, false)); // red
            MMIT++;
        }
    }
    Disp->Update();
    Utilities->CallLogPop(1548);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::LoadOldPrefDir(int Caller, std::ifstream &VecFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadOldPrefDir");
    int TempInt;

    ClearPrefDir();
    int NumberOfPrefDirElements = 0;

    NumberOfPrefDirElements = Utilities->LoadFileInt(VecFile);
    for(int x = 0; x < NumberOfPrefDirElements; x++)
    {
        VecFile >> TempInt; // TrackVectorPosition
        TPrefDirElement LoadPrefDirElement(Track->TrackElementAt(714, TempInt));
        LoadPrefDirElement.TrackVectorPosition = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.ELink = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.ELinkPos = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.XLink = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.XLinkPos = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.EXNumber = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.CheckCount = TempInt;
        LoadPrefDirElement.IsARoute = Utilities->LoadFileBool(VecFile);
        LoadPrefDirElement.AutoSignals = Utilities->LoadFileBool(VecFile);
        LoadPrefDirElement.PrefDirRoute = Utilities->LoadFileBool(VecFile);
        if(!(LoadPrefDirElement.IsARoute))
        {
            LoadPrefDirElement.EXGraphicPtr = LoadPrefDirElement.GetPrefDirGraphicPtr();
            LoadPrefDirElement.EntryDirectionGraphicPtr = LoadPrefDirElement.GetDirectionPrefDirGraphicPtr();
        }
        else
        {
            LoadPrefDirElement.EXGraphicPtr = LoadPrefDirElement.GetRouteGraphicPtr(LoadPrefDirElement.AutoSignals, LoadPrefDirElement.PrefDirRoute);
            LoadPrefDirElement.EntryDirectionGraphicPtr = LoadPrefDirElement.GetDirectionRouteGraphicPtr(LoadPrefDirElement.AutoSignals,
                                                                                                         LoadPrefDirElement.PrefDirRoute);
        }
        StorePrefDirElement(5, LoadPrefDirElement);
        Utilities->LoadFileString(VecFile); // marker
    }
    CheckPrefDir4MultiMap(4);
    Utilities->CallLogPop(161);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::LoadPrefDir(int Caller, std::ifstream &VecFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadPrefDir");
    int TempInt;

    ClearPrefDir();
    int NumberOfPrefDirElements = 0;

    NumberOfPrefDirElements = Utilities->LoadFileInt(VecFile);
    for(int x = 0; x < NumberOfPrefDirElements; x++)
    {
        VecFile >> TempInt; // PrefDirVectorPosition, not used in load
        VecFile >> TempInt; // TrackVectorPosition
        TPrefDirElement LoadPrefDirElement(Track->TrackElementAt(781, TempInt)); //Loads all basic TrackElement values incl HLoc, VLoc & SpeedTag
        LoadPrefDirElement.TrackVectorPosition = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.ELink = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.ELinkPos = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.XLink = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.XLinkPos = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.EXNumber = TempInt;
        VecFile >> TempInt;
        LoadPrefDirElement.CheckCount = TempInt;
        LoadPrefDirElement.IsARoute = Utilities->LoadFileBool(VecFile);
        LoadPrefDirElement.AutoSignals = Utilities->LoadFileBool(VecFile);
        LoadPrefDirElement.PrefDirRoute = Utilities->LoadFileBool(VecFile);
        if(!(LoadPrefDirElement.IsARoute))
        {
            LoadPrefDirElement.EXGraphicPtr = LoadPrefDirElement.GetPrefDirGraphicPtr();
            LoadPrefDirElement.EntryDirectionGraphicPtr = LoadPrefDirElement.GetDirectionPrefDirGraphicPtr();
        }
        else
        {
            LoadPrefDirElement.EXGraphicPtr = LoadPrefDirElement.GetRouteGraphicPtr(LoadPrefDirElement.AutoSignals, LoadPrefDirElement.PrefDirRoute);
            LoadPrefDirElement.EntryDirectionGraphicPtr = LoadPrefDirElement.GetDirectionRouteGraphicPtr(LoadPrefDirElement.AutoSignals,
                                                                                                         LoadPrefDirElement.PrefDirRoute);
        }
        StorePrefDirElement(0, LoadPrefDirElement);
        AnsiString MarkerString = Utilities->LoadFileString(VecFile); // marker
    }
    CheckPrefDir4MultiMap(6);
    Utilities->CallLogPop(1509);
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::CheckOnePrefDir(int Caller, int NumberOfActiveElements, std::ifstream &VecFile) // returns false if no more PrefDirs to check
/*
      Called before PrefDir loading as part of the FileIntegrityCheck function, in case there is an error in the
      file.  Very similar to LoadPrefDir but with value checks instead of storage in PrefDirVector.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckOnePrefDir");
    int TempInt;
    int NumberOfPrefDirElements = 0;

    NumberOfPrefDirElements = Utilities->LoadFileInt(VecFile);
    if((NumberOfPrefDirElements < 0) || (NumberOfPrefDirElements > 1000000))
    {
        Utilities->CallLogPop(1152);
        return(false);
    }
    for(int x = 0; x < NumberOfPrefDirElements; x++)
    {
        if(!Utilities->CheckFileInt(VecFile, x, x)) // vector number
        {
            Utilities->CallLogPop(1766);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < 0) || (TempInt >= NumberOfActiveElements)) // TrackVectorPosition
        {
            Utilities->CallLogPop(163);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 9)) // ELink
        {
            Utilities->CallLogPop(162);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 3)) // ELinkPos
        {
            Utilities->CallLogPop(164);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 9)) // XLink
        {
            Utilities->CallLogPop(165);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 3)) // XLinkPos
        {
            Utilities->CallLogPop(166);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt < -1) || (TempInt > 27)) // EXNumber
        {
            Utilities->CallLogPop(167);
            return(false);
        }
        VecFile >> TempInt;
        if(TempInt != 9) // CheckCount - reduced to 11 after NextPrefDirElement dropped &
                         // to 9 after End & Stop dropped.  Leaving HLoc, VLoc, SpeedTag, TrackVectorPosition, ELink,
                         // ELinkPos, XLink, XLinkPos & EXNumber
        {
            Utilities->CallLogPop(168);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt != 0) && (TempInt != 1)) // RouteElement
        {
            Utilities->CallLogPop(1147);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt != 0) && (TempInt != 1)) // AutoSignals
        {
            Utilities->CallLogPop(1510);
            return(false);
        }
        VecFile >> TempInt;
        if((TempInt != 0) && (TempInt != 1)) // PrefDirRoute
        {
            Utilities->CallLogPop(1148);
            return(false);
        }
        if(!Utilities->CheckFileStringZeroDelimiter(VecFile)) // marker
        {
            Utilities->CallLogPop(1700);
            return(false);
        }
    }
    Utilities->CallLogPop(169);
    return(true);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::SavePrefDirVector(int Caller, std::ofstream &VecFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SavePrefDir");
    int NumberOfPrefDirElements = PrefDirVector.size();

    Utilities->SaveFileInt(VecFile, NumberOfPrefDirElements);
    for(int y = 0; y < NumberOfPrefDirElements; y++)
    {
        VecFile << y << '\n'; // extra
        VecFile << PrefDirVector.at(y).TrackVectorPosition << '\n';  //When reloaded values for HLoc, VLoc & SpeedTag are derived from the TrackElement at TrackVectorPosition
        VecFile << PrefDirVector.at(y).ELink << '\n';                //so all 9 critical values are set
        VecFile << PrefDirVector.at(y).ELinkPos << '\n';
        VecFile << PrefDirVector.at(y).XLink << '\n';
        VecFile << PrefDirVector.at(y).XLinkPos << '\n';
        VecFile << PrefDirVector.at(y).EXNumber << '\n';
        VecFile << PrefDirVector.at(y).CheckCount << '\n';
        Utilities->SaveFileBool(VecFile, PrefDirVector.at(y).IsARoute);
        Utilities->SaveFileBool(VecFile, PrefDirVector.at(y).AutoSignals);
        Utilities->SaveFileBool(VecFile, PrefDirVector.at(y).PrefDirRoute);
        if(y == (NumberOfPrefDirElements - 1)) // last element, write a longer delimiter
        {
            VecFile << "************" << '\0' << '\n'; // marker
        }
        else
        {
            VecFile << "******" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
        }
    }
    Utilities->CallLogPop(170);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::SaveSearchVector(int Caller, std::ofstream &VecFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveSearchVector");
    int NumberOfSearchElements = SearchVector.size();

    Utilities->SaveFileInt(VecFile, NumberOfSearchElements);
    for(int y = 0; y < NumberOfSearchElements; y++)
    {
        VecFile << y << '\n'; // extra
        VecFile << SearchVector.at(y).TrackVectorPosition << '\n';
        VecFile << SearchVector.at(y).ELink << '\n';
        VecFile << SearchVector.at(y).ELinkPos << '\n';
        VecFile << SearchVector.at(y).XLink << '\n';
        VecFile << SearchVector.at(y).XLinkPos << '\n';
        VecFile << SearchVector.at(y).EXNumber << '\n';
        VecFile << SearchVector.at(y).CheckCount << '\n';
        Utilities->SaveFileBool(VecFile, SearchVector.at(y).IsARoute);
        Utilities->SaveFileBool(VecFile, SearchVector.at(y).AutoSignals);
        Utilities->SaveFileBool(VecFile, SearchVector.at(y).PrefDirRoute);
        if(y == (NumberOfSearchElements - 1)) // last element, write a longer delimiter
        {
            VecFile << "************" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
        }
        else
        {
            VecFile << "******" << '\0' << '\n'; // note:  << doesn't write the null string terminator character automatically
        }
    }
    Utilities->CallLogPop(1847);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::EraseFromPrefDirVectorAnd4MultiMap(int Caller, int HLoc, int VLoc)
/*
      Erase element at HLoc and VLoc from the PrefDirVector and from the 4MultiMap.  Note that this entails
      erasing up to four elements (2 directions and 2 tracks for 4-entry elements).
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",EraseFromPrefDirVectorAnd4MultiMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    int VecPos = GetOnePrefDirPosition(1, HLoc, VLoc);

    if(VecPos > -1)
    {
        ErasePrefDirElementAt(2, VecPos); // max of 4 to be erased
    }
    else
    {
        Utilities->CallLogPop(171);
        return;
    }
    VecPos = GetOnePrefDirPosition(2, HLoc, VLoc);
    if(VecPos > -1)
    {
        ErasePrefDirElementAt(3, VecPos);
    }
    else
    {
        Utilities->CallLogPop(172);
        return;
    }
    VecPos = GetOnePrefDirPosition(3, HLoc, VLoc);
    if(VecPos > -1)
    {
        ErasePrefDirElementAt(4, VecPos);
    }
    else
    {
        Utilities->CallLogPop(173);
        return;
    }
    VecPos = GetOnePrefDirPosition(4, HLoc, VLoc);
    if(VecPos > -1)
    {
        ErasePrefDirElementAt(5, VecPos);
    }
    else
    {
        Utilities->CallLogPop(174);
        return;
    }
    Utilities->CallLogPop(175);
}

// ---------------------------------------------------------------------------
/*
      void TOnePrefDir::EraseCorruptedElementsAfterTrackBuild()//Delete any PrefDir elements that are no longer valid
      //Not needed after new TrackErase (now EraseTrackElement), where blank elements aren't used

      When track is rebuilt any elements that are dispensed with aren't erased immediately, a blank element is put
      in their place so that existing linkages will be preserved.  At this stage this function is called to remove
      any elements in PrefDirVector that correspond directly to blank track elements or that are connected to blank track
      elements.  Finally the track is reconnected using Track->TryToConnectTrack (if won't connect then returns to
      AddTrackStage build mode for corrections to be made) and then EveryPrefDir->RebuildPrefDirVector() called to reset
      PrefDirVector to correspond to the new track layout.

      {
      Utilities->CallLog.push_back(Utilities->TimeStamp() + ",EraseCorruptedElementsAfterTrackBuild");
      if(PrefDirSize() == 0)
       {
       Utilities->CallLogPop(176);
       return;
       }
      for(int x=(PrefDirVector.size()-1);x>=0;x--)
       {
       int TV = PrefDirVector.at(x).TrackVectorPosition;
       int ConnELink = PrefDirVector.at(x).Conn[PrefDirVector.at(x).ELinkPos];
       int ConnXLink = PrefDirVector.at(x).Conn[PrefDirVector.at(x).XLinkPos];
       if(Track->BlankElementAt(0, TV))
           {
           ErasePrefDirElementAt(6, x);
           }
       //if was a blankelement at x then ConnELink and ConnXLink both -1
       else if((ConnELink > -1) && (Track->BlankElementAt(1, ConnELink)))
           {
           ErasePrefDirElementAt(7, x);
           }
       //if both ConnELink and ConnXLink correspond to blank elements then OK, element only
       //needs to be erased once, but if don't use 'else' then will erase two elements
       //since 'x' will correspond to the element after the first erased element
       else if((ConnXLink > -1) && (Track->BlankElementAt(2, ConnXLink)))
           {
           ErasePrefDirElementAt(8, x);
           }
       }
      Utilities->CallLogPop(177);
      }
*/
// ---------------------------------------------------------------------------

void TOnePrefDir::ConsolidatePrefDirs(int Caller, TOnePrefDir *InputPrefDir)
/*
      This is used to add InputPrefDir's PrefDirVector to TOnePrefDir's PrefDirVector except where it already
      exists in TOnePrefDir.  In practice it adds ConstructPrefDir to EveryPrefDir.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ConsolidatePrefDirs");
    bool AlreadyPresent, FoundFlag;
    int PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3;

    for(unsigned int x = 0; x < InputPrefDir->PrefDirSize(); x++)
    {
        TPrefDirElement TempElement = InputPrefDir->PrefDirVector.at(x);
        GetVectorPositionsFromPrefDir4MultiMap(1, TempElement.HLoc, TempElement.VLoc, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
        AlreadyPresent = false;
        if(FoundFlag)
        {
            if((PrefDirPos0 > -1) && (TempElement == GetFixedPrefDirElementAt(8, PrefDirPos0)))
            {
                AlreadyPresent = true;
            }
            if((PrefDirPos1 > -1) && (TempElement == GetFixedPrefDirElementAt(9, PrefDirPos1)))
            {
                AlreadyPresent = true;
            }
            if((PrefDirPos2 > -1) && (TempElement == GetFixedPrefDirElementAt(10, PrefDirPos2)))
            {
                AlreadyPresent = true;
            }
            if((PrefDirPos3 > -1) && (TempElement == GetFixedPrefDirElementAt(11, PrefDirPos3)))
            {
                AlreadyPresent = true;
            }
        }
        if(!AlreadyPresent)
        {
            StorePrefDirElement(4, TempElement);
        }
    }
    CheckPrefDir4MultiMap(5);
    Utilities->CallLogPop(178);
}
/* earlier brute force search
      for(unsigned int x = 0;x<InputPrefDir->PrefDirSize();x++)
       {
       TPrefDirElement TempElement = InputPrefDir->GetFixedPrefDirElementAt(12, x);
       bool AlreadyPresent = false;
       for(unsigned int y = 0;y<PrefDirSize();y++)
           {
           if(TempElement == GetFixedPrefDirElementAt(13, y)) AlreadyPresent = true;
           }
       if(!AlreadyPresent) StorePrefDirElement(, TempElement);
       }
*/

// ---------------------------------------------------------------------------

void TOnePrefDir::RebuildPrefDirVector(int Caller)
/*
      Rebuild from Trackmap, doesn't affect PrefDir4MultiMap.
      After a track build, but before the track is reconnected, all invalid PrefDir elements in TOnePrefDir
      (i.e. in EveryPrefDir) are erased.  Hence at that stage all the PrefDir elements are valid and correspond to
      the track elements at relevant H & V positions.  However, after the track is reconnected, the TrackVector
      positions are likely to have changed, so this function is called to reset all the necessary connections and
      TrackVector positions.  To be on the safe side all the TrackElement values that are additional to
      TFixedTrackPiece (apart from TrainIDs, these only present during operation) are reset, though the others
      shouldn't have changed.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RebuildPrefDirVector");
    for(unsigned int x = 0; x < PrefDirVector.size(); x++)
    {
        bool FoundFlag;
        int VecPos = Track->GetVectorPositionFromTrackMap(10, PrefDirVector.at(x).HLoc, PrefDirVector.at(x).VLoc, FoundFlag);
        if(FoundFlag)
        {
            PrefDirVector.at(x).TrackVectorPosition = VecPos;
            PrefDirVector.at(x).LocationName = Track->TrackElementAt(78, VecPos).LocationName;
            PrefDirVector.at(x).ActiveTrackElementName = Track->TrackElementAt(79, VecPos).ActiveTrackElementName;
            PrefDirVector.at(x).ElementID = Track->TrackElementAt(80, VecPos).ElementID;
            PrefDirVector.at(x).Attribute = Track->TrackElementAt(81, VecPos).Attribute;
            for(unsigned int z = 0; z < 4; z++)
            {
                PrefDirVector.at(x).Conn[z] = Track->TrackElementAt(82, VecPos).Conn[z];
                PrefDirVector.at(x).ConnLinkPos[z] = Track->TrackElementAt(83, VecPos).ConnLinkPos[z];
            }
        }
        else
        {
            throw Exception("Error in RebuildPrefDirVector - PrefDirVector is unsafe");
        }
    }
    Utilities->CallLogPop(179);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::CheckPrefDirAgainstTrackVector(int Caller)
/*
      Check loaded PrefDir against loaded track, and if discrepancies found give message & clear EveryPrefDir & PrefDir4MultiMap.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckPrefDirAgainstTrackVector");
    bool DiscrepancyFound = false;

    for(unsigned int x = 0; x < PrefDirVector.size(); x++)
    {
        bool FoundFlag;
        int VecPos = Track->GetVectorPositionFromTrackMap(39, PrefDirVector.at(x).HLoc, PrefDirVector.at(x).VLoc, FoundFlag);
        if(FoundFlag)
        {
            TPrefDirElement PE = PrefDirVector.at(x);
            if(PE.TrackVectorPosition != VecPos)
            {
                DiscrepancyFound = true;
                break;
            }
            if((PE.GetELinkPos() < 0) || (PE.GetELinkPos() > 3))
            {
                DiscrepancyFound = true;
                break;
            }
            if((PE.GetXLinkPos() < 0) || (PE.GetXLinkPos() > 3))
            {
                DiscrepancyFound = true;
                break;
            }
            if(PE.ELink != Track->TrackElementAt(710, VecPos).Link[PE.GetELinkPos()])
            {
                DiscrepancyFound = true;
                break;
            }
            if(PE.XLink != Track->TrackElementAt(711, VecPos).Link[PE.GetXLinkPos()])
            {
                DiscrepancyFound = true;
                break;
            }
        }
        else
        {
            DiscrepancyFound = true;
        }
    }
    if(DiscrepancyFound)
    {
        ShowMessage("Discrepancies found in the preferred direction file, preferred directions will be cleared");
        ClearPrefDir(); // also clears multimap
    }
    Utilities->CallLogPop(1436);
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::CheckPrefDirAgainstTrackVectorNoMessage(int Caller)
/*
      Check loaded PrefDir against loaded track, and if discrepancies found clear EveryPrefDir & PrefDir4MultiMap.
      return true for OK
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckPrefDirAgainstTrackVectorNoMessage");
    bool DiscrepancyFound = false;

    for(unsigned int x = 0; x < PrefDirVector.size(); x++)
    {
        bool FoundFlag;
        int VecPos = Track->GetVectorPositionFromTrackMap(36, PrefDirVector.at(x).HLoc, PrefDirVector.at(x).VLoc, FoundFlag);
        if(FoundFlag)
        {
            TPrefDirElement PE = PrefDirVector.at(x);
            if(PE.TrackVectorPosition != VecPos)
            {
                DiscrepancyFound = true;
            }
            if((PE.GetELinkPos() < 0) || (PE.GetELinkPos() > 3))
            {
                DiscrepancyFound = true;
                break;
            }
            if((PE.GetXLinkPos() < 0) || (PE.GetXLinkPos() > 3))
            {
                DiscrepancyFound = true;
                break;
            }
            if(PE.ELink != Track->TrackElementAt(715, VecPos).Link[PE.GetELinkPos()])
            {
                DiscrepancyFound = true;
                break;
            }
            if(PE.XLink != Track->TrackElementAt(716, VecPos).Link[PE.GetXLinkPos()])
            {
                DiscrepancyFound = true;
                break;
            }
        }
        else
        {
            DiscrepancyFound = true;
        }
    }
    Utilities->CallLogPop(1512);
    return(!DiscrepancyFound);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::CheckPrefDir4MultiMap(int Caller) // test
/*
      Test function to check correspondence between PrefDirVector and PrefDir4MultiMap for each element in
      turn and for the overall sizes.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckPrefDir4MultiMap");
    bool FoundFlag = false;
    int PrefDir0, PrefDir1, PrefDir2, PrefDir3;

    for(unsigned int a = 0; a < PrefDirVector.size(); a++)
    {
        TPrefDirElement CheckElement = PrefDirVector.at(a);
        GetVectorPositionsFromPrefDir4MultiMap(2, CheckElement.HLoc, CheckElement.VLoc, FoundFlag, PrefDir0, PrefDir1, PrefDir2, PrefDir3);
        if(!FoundFlag)
        {
            throw Exception("CheckPrefDir4MultiMap Error - failed to find HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" + (AnsiString)CheckElement.VLoc +
                            " in PrefDir4MultiMap, Caller=" + (AnsiString)Caller);
        }
        if((PrefDir0 != (int)a) && (PrefDir1 != (int)a) && (PrefDir2 != (int)a) && (PrefDir3 != (int)a))
        {
            throw Exception("CheckPrefDir4MultiMap Error - MapVectorPosition failed at HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" +
                            (AnsiString)CheckElement.VLoc + " Map values=" + (AnsiString)PrefDir0 + ", " + (AnsiString)PrefDir1 + ", " + (AnsiString)PrefDir2 + ", " +
                            (AnsiString)PrefDir3 + " PrefDirVectorPos value=" + (AnsiString)a + " Caller=" + (AnsiString)Caller);
        }
    }
    if(PrefDirVector.size() != PrefDir4MultiMap.size())
    {
        throw Exception("CheckPrefDir4MultiMap Error - Map Size=" + (AnsiString)PrefDirVector.size() + " PrefDirVectorSize=" + (AnsiString)PrefDirVector.size()
                        + " Caller=" + (AnsiString)Caller);
    }
    Utilities->CallLogPop(180);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::GetVectorPositionsFromPrefDir4MultiMap(int Caller, int HLoc, int VLoc, bool &FoundFlag, int &PrefDirPos0, int &PrefDirPos1, int &PrefDirPos2,
                                                         int &PrefDirPos3)
/*
      There are up to four elements at each H & V position in the PrefDirVector - two directions per track, and up to
      two tracks for 4-entry elements.  This function retrieves all elements that are present at a given H & V
      position.  FoundFlag indicates whether any or none have been found, and PrefDirPos0, 1, 2 & 3 contain
      the PrefDirVector positions, or -1 if not present.  The elements are always found in order, such that
      if there is only one it will be in PrefDirPos0, if two they will be in PrefDirPos0 and PrefDirPos1 and so on.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetVectorPositionsFromPrefDir4MultiMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    THVPair PrefDirMapKeyPair;

    PrefDirPos0 = -1;
    PrefDirPos1 = -1;
    PrefDirPos2 = -1;
    PrefDirPos3 = -1;
    FoundFlag = false;
    PrefDirMapKeyPair.first = HLoc;
    PrefDirMapKeyPair.second = VLoc;
    std::pair<TPrefDir4MultiMapIterator, TPrefDir4MultiMapIterator>ItPair;

    ItPair = PrefDir4MultiMap.equal_range(PrefDirMapKeyPair);
    if(ItPair.first == ItPair.second) //none found
    {
        Utilities->CallLogPop(181);
        return;
    }
    else
    {
        FoundFlag = true;
        PrefDirPos0 = ItPair.first->second;
        ItPair.first++;
        if(ItPair.first == ItPair.second)
        {
            Utilities->CallLogPop(182);  //only one found
            return;
        }
        if(((ItPair.first->first).first == HLoc) && ((ItPair.first->first).second == VLoc))
        {
            PrefDirPos1 = ItPair.first->second;
        }
        ItPair.first++;
        if(ItPair.first == ItPair.second)
        {
            Utilities->CallLogPop(183);   //2 found
            return;
        }
        if(((ItPair.first->first).first == HLoc) && ((ItPair.first->first).second == VLoc))
        {
            PrefDirPos2 = ItPair.first->second;
        }
        ItPair.first++;
        if(ItPair.first == ItPair.second)
        {
            Utilities->CallLogPop(184); //3 found
            return;
        }
        if(((ItPair.first->first).first == HLoc) && ((ItPair.first->first).second == VLoc))
        {
            PrefDirPos3 = ItPair.first->second;  //4 found
        }
    }
    Utilities->CallLogPop(185);
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::FindLinkingPrefDir(int Caller, int PrefDirVectorNumber, int LinkNumberPos, int LinkNumber, int &LinkedPrefDirVectorNumber)
{ //not used after modified the pref dir checking function at v2.13.0
    // Finds a pref dir element that links to another element at given vector number and link number & position, returns true if found with linked
    // vector number, true if buffer or continuation with link at blank end & linked vector number = -1, or false if not found with vector number == -1
    try
    {
        Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindLinkingPrefDir," + AnsiString(PrefDirVectorNumber)
                                     + "," + AnsiString(LinkNumberPos));
        bool FoundFlag;
        int PD0, PD1, PD2, PD3;
        if(PrefDirVector.at(PrefDirVectorNumber).Conn[LinkNumberPos] > -1)
        {
            GetVectorPositionsFromPrefDir4MultiMap(14, Track->TrackElementAt(1021, PrefDirVector.at(PrefDirVectorNumber).Conn[LinkNumberPos]).HLoc,
                                                   Track->TrackElementAt(1022, PrefDirVector.at(PrefDirVectorNumber).Conn[LinkNumberPos]).VLoc, FoundFlag,
                                                   PD0, PD1, PD2, PD3);
            if(!FoundFlag)
            {
                Utilities->CallLogPop(2282);
                return(false);
            }
            if((PrefDirVector.at(PrefDirVectorNumber).TrackType == GapJump) && (LinkNumberPos == 0)) //0 is the gap position
            {
                if(PD0 > -1)
                {
                    if(PrefDirVector.at(PD0).TrackType == GapJump) //links to a gap and there is a pref dir set on it, doesn't matter about the link position
                    {
                        LinkedPrefDirVectorNumber = PD0;
                        Utilities->CallLogPop(2283);
                        return(true);
                    }
                }
                if(PD1 > -1)
                {
                    if(PrefDirVector.at(PD1).TrackType == GapJump) //can only be PD0 or PD1 for a gap
                    {
                        LinkedPrefDirVectorNumber = PD1;
                        Utilities->CallLogPop(2284);
                        return(true);
                    }
                }
            }
            if(PD0 > -1)
            {
                if((PrefDirVector.at(PD0).ELink == (10 - LinkNumber)) || (PrefDirVector.at(PD0).XLink == (10 - LinkNumber)))
                {
                    LinkedPrefDirVectorNumber = PD0;
                    Utilities->CallLogPop(2285);
                    return(true);
                }
            }
            if(PD1 > -1)
            {
                if((PrefDirVector.at(PD1).ELink == (10 - LinkNumber)) || (PrefDirVector.at(PD1).XLink == (10 - LinkNumber)))
                {
                    LinkedPrefDirVectorNumber = PD1;
                    Utilities->CallLogPop(2286);
                    return(true);
                }
            }
            if(PD2 > -1)
            {
                if((PrefDirVector.at(PD2).ELink == (10 - LinkNumber)) || (PrefDirVector.at(PD2).XLink == (10 - LinkNumber)))
                {
                    LinkedPrefDirVectorNumber = PD2;
                    Utilities->CallLogPop(2287);
                    return(true);
                }
            }
            if(PD3 > -1)
            {
                if((PrefDirVector.at(PD3).ELink == (10 - LinkNumber)) || (PrefDirVector.at(PD3).XLink == (10 - LinkNumber)))
                {
                    LinkedPrefDirVectorNumber = PD3;
                    Utilities->CallLogPop(2288);
                    return(true);
                }
            }
            LinkedPrefDirVectorNumber = -1;
            Utilities->CallLogPop(2289);
            return(false);
        }
        else //buffer or continuation, no link at position 0 but not a failure
        {
            LinkedPrefDirVectorNumber = -1;
            Utilities->CallLogPop(2290);
            return(true);
        }
    }
    catch(const Exception &e)  //non error catch
    {
        LinkedPrefDirVectorNumber = -1;
        Utilities->CallLogPop(2291);
        return(false);
    }
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::FindLinkingCompatiblePrefDir(int Caller, int PrefDirVectorNumber, int LinkNumberPos, int LinkNumber, int &LinkedPrefDirVectorNumber)
{ //not used after modified the pref dir checking function at v2.13.0
    // Finds a pref dir element that links to another element at given vector number and link number & position, returns true if finds same direction pref dir with linked
    // vector number, true if buffer or continuation with link at blank end & linked vector number = -1, or false if not found with vector number == -1
    try
    {
        Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindLinkingCompatiblePrefDir," + AnsiString(PrefDirVectorNumber)
                                     + "," + AnsiString(LinkNumberPos));
        bool FoundFlag;
        int PD0, PD1, PD2, PD3;
        if(PrefDirVector.at(PrefDirVectorNumber).Conn[LinkNumberPos] > -1)
        {
            GetVectorPositionsFromPrefDir4MultiMap(31, Track->TrackElementAt(1463, PrefDirVector.at(PrefDirVectorNumber).Conn[LinkNumberPos]).HLoc,
                                                   Track->TrackElementAt(1464, PrefDirVector.at(PrefDirVectorNumber).Conn[LinkNumberPos]).VLoc, FoundFlag,
                                                   PD0, PD1, PD2, PD3);
            if(!FoundFlag)
            {
                Utilities->CallLogPop(2468);
                return(false);
            }
            if((PrefDirVector.at(PrefDirVectorNumber).TrackType == GapJump) && (LinkNumberPos == 0)) //0 is the gap position
            { //only PD0 or 1 will be set, else have track linking error that will be found earlier
                if(PD0 > -1)
                {
                    if((PrefDirVector.at(PD0).TrackType == GapJump) && ((PrefDirVector.at(PrefDirVectorNumber).ELink == (10 - PrefDirVector.at(PD0).XLink))
                                                            || (PrefDirVector.at(PrefDirVectorNumber).XLink == (10 - PrefDirVector.at(PD0).ELink))))
                    {
                        LinkedPrefDirVectorNumber = PD0;
                        Utilities->CallLogPop(2469);
                        return(true);
                    }
                }
                if(PD1 > -1)
                {
                    if((PrefDirVector.at(PD1).TrackType == GapJump) && ((PrefDirVector.at(PrefDirVectorNumber).ELink == (10 - PrefDirVector.at(PD1).XLink))
                                                            || (PrefDirVector.at(PrefDirVectorNumber).XLink == (10 - PrefDirVector.at(PD1).ELink))))
                    {
                        LinkedPrefDirVectorNumber = PD1;
                        Utilities->CallLogPop(2470);
                        return(true);
                    }
                }
                LinkedPrefDirVectorNumber = -1;
                Utilities->CallLogPop(2471);
                return(false);
            }
            if(PD0 > -1)
            {
                if((PrefDirVector.at(PD0).ELink == (10 - PrefDirVector.at(PrefDirVectorNumber).XLink)) || (PrefDirVector.at(PD0).XLink == (10 - PrefDirVector.at(PrefDirVectorNumber).ELink)))
                {
                    LinkedPrefDirVectorNumber = PD0;
                    Utilities->CallLogPop(2472);
                    return(true);
                }
            }
            if(PD1 > -1)
            {
                if((PrefDirVector.at(PD1).ELink == (10 - PrefDirVector.at(PrefDirVectorNumber).XLink)) || (PrefDirVector.at(PD1).XLink == (10 - PrefDirVector.at(PrefDirVectorNumber).ELink)))
                {
                    LinkedPrefDirVectorNumber = PD1;
                    Utilities->CallLogPop(2473);
                    return(true);
                }
            }
            if(PD2 > -1)
            {
                if((PrefDirVector.at(PD2).ELink == (10 - PrefDirVector.at(PrefDirVectorNumber).XLink)) || (PrefDirVector.at(PD2).XLink == (10 - PrefDirVector.at(PrefDirVectorNumber).ELink)))
                {
                    LinkedPrefDirVectorNumber = PD2;
                    Utilities->CallLogPop(2474);
                    return(true);
                }
            }
            if(PD3 > -1)
            {
                if((PrefDirVector.at(PD3).ELink == (10 - PrefDirVector.at(PrefDirVectorNumber).XLink)) || (PrefDirVector.at(PD3).XLink == (10 - PrefDirVector.at(PrefDirVectorNumber).ELink)))
                {
                    LinkedPrefDirVectorNumber = PD3;
                    Utilities->CallLogPop(2475);
                    return(true);
                }
            }
            LinkedPrefDirVectorNumber = -1;
            Utilities->CallLogPop(2476);
            return(false);
        }
        else //buffer or continuation, no link at position 0 but not a failure
        {
            LinkedPrefDirVectorNumber = -1;
            Utilities->CallLogPop(2477);
            return(true);
        }
    }
    catch(const Exception &e)  //non error catch
    {
        LinkedPrefDirVectorNumber = -1;
        Utilities->CallLogPop(2478);
        return(false);
    }
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::BiDirectionalPrefDir(int Caller, TPrefDir4MultiMapIterator PDPtr)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",BiDirectionalPrefDir");
    bool FoundFlag; //not used
    int PD0, PD1, PD2, PD3;
    //recover all PDs at the H & V of PDPtr
    GetVectorPositionsFromPrefDir4MultiMap(15, PDPtr->first.first, PDPtr->first.second, FoundFlag, PD0, PD1, PD2, PD3);

    int ELink = PrefDirVector.at(PDPtr->second).GetELink();
    int XLink = PrefDirVector.at(PDPtr->second).GetXLink();

    if(PD0 > -1)  //ok if PDPtr->second == PD0 as won't find a match, same for others
    {
        if((PrefDirVector.at(PD0).GetELink() == XLink) && (PrefDirVector.at(PD0).GetXLink() == ELink))
        {
            Utilities->CallLogPop(2292);
            return(true);
        }
    }
    if(PD1 > -1)
    {
        if((PrefDirVector.at(PD1).GetELink() == XLink) && (PrefDirVector.at(PD1).GetXLink() == ELink))
        {
            Utilities->CallLogPop(2293);
            return(true);
        }
    }
    if(PD2 > -1)
    {
        if((PrefDirVector.at(PD2).GetELink() == XLink) && (PrefDirVector.at(PD2).GetXLink() == ELink))
        {
            Utilities->CallLogPop(2294);
            return(true);
        }
    }
    if(PD3 > -1)
    {
        if((PrefDirVector.at(PD3).GetELink() == XLink) && (PrefDirVector.at(PD3).GetXLink() == ELink))
        {
            Utilities->CallLogPop(2295);
            return(true);
        }
    }
    Utilities->CallLogPop(2296);
    return(false);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::StorePrefDirElement(int Caller, TPrefDirElement LoadPrefDirElement)
/*
      LoadPrefDirElement is stored in both the PrefDirVector and in PrefDir4MultiMap.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",StorePrefDirElement," + LoadPrefDirElement.LogPrefDir());
    PrefDirVector.push_back(LoadPrefDirElement);
    THVPair PrefDir4MultiMapKeyPair;
    TPrefDir4MultiMapEntry PrefDir4MultiMapEntry;

    PrefDir4MultiMapKeyPair.first = LoadPrefDirElement.HLoc;
    PrefDir4MultiMapKeyPair.second = LoadPrefDirElement.VLoc;
    PrefDir4MultiMapEntry.first = PrefDir4MultiMapKeyPair;
    PrefDir4MultiMapEntry.second = LastElementNumber(68);
    PrefDir4MultiMap.insert(PrefDir4MultiMapEntry);
// CheckPrefDir4MultiMap(1);Drop here as takes too long - call it by each calling function
    Utilities->CallLogPop(186);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::ErasePrefDirElementAt(int Caller, int PrefDirVectorPosition)
/*
      Erase a single element from PrefDirVector and 4MultiMap, decrementing the remaining PrefDirElementNumbers in
      4MultiMap if they are greater than the erased value.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ErasePrefDirElementAt," + AnsiString(PrefDirVectorPosition));
    bool FoundFlag;

    if(!PrefDirVector.empty())
    {
        TPrefDir4MultiMapIterator EraseIt = GetExactMatchFrom4MultiMap(0, PrefDirVectorPosition, FoundFlag);
        if(!FoundFlag)
        {
            throw Exception("Failed to find PrefDir4MultiMap erase element");
        }
        PrefDirVector.erase(PrefDirVector.begin() + PrefDirVectorPosition);
        PrefDir4MultiMap.erase(EraseIt);
        DecrementPrefDirElementNumbersInPrefDir4MultiMap(0, PrefDirVectorPosition);
        CheckPrefDir4MultiMap(2);
    }
    Utilities->CallLogPop(187);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::DecrementPrefDirElementNumbersInPrefDir4MultiMap(int Caller, unsigned int ErasedElementNumber)
/*
      Called after ErasePrefDirElementAt(int PrefDirVectorPosition) to decrement the remaining PrefDirElementNumbers in
      4MultiMap if they are greater than the erased value.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DecrementPrefDirElementNumbersInPrefDir4MultiMap," +
                                 AnsiString(ErasedElementNumber));
    if(!PrefDir4MultiMap.empty())
    {
        for(TPrefDir4MultiMapIterator MapPtr = PrefDir4MultiMap.begin(); MapPtr != PrefDir4MultiMap.end(); MapPtr++)
        {
            if(MapPtr->second > ErasedElementNumber)
            {
                MapPtr->second--;
            }
        }
    }
    Utilities->CallLogPop(1450);
}

// ---------------------------------------------------------------------------

TOnePrefDir::TPrefDir4MultiMapIterator TOnePrefDir::GetExactMatchFrom4MultiMap(int Caller, unsigned int PrefDirVectorPosition, bool &FoundFlag)
/*
      Retrieves a PrefDir4MultiMap iterator to the PrefDir element at PrefDirVectorPosition.  Used during
      ErasePrefDirElementAt(int PrefDirVectorPosition) to erase the relevant element in the multimap.  If
      nothing is found this is an error but the error message is given in the calling function.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetExactMatchFrom4MultiMap," + AnsiString(PrefDirVectorPosition));
    FoundFlag = false;
    if(PrefDirVectorPosition >= PrefDirVector.size())
    {
        throw Exception("PrefDirVectorPosition out of range");
    }
    TPrefDirElement PrefDirElement = GetFixedPrefDirElementAt(14, PrefDirVectorPosition);
    THVPair PrefDir4MultiMapKeyPair;

    PrefDir4MultiMapKeyPair.first = PrefDirElement.HLoc;
    PrefDir4MultiMapKeyPair.second = PrefDirElement.VLoc;
    std::pair<TPrefDir4MultiMapIterator, TPrefDir4MultiMapIterator>ItPair;

    ItPair = PrefDir4MultiMap.equal_range(PrefDir4MultiMapKeyPair);
    if(ItPair.first == ItPair.second)
    {
        Utilities->CallLogPop(188);
        return(ItPair.first); // nothing found but have to return an iterator, FoundFlag indicates nothing found
    }
    else
    {
        if(ItPair.first->second == PrefDirVectorPosition)
        {
            FoundFlag = true;
            Utilities->CallLogPop(189);
            return(ItPair.first);
        }
        ItPair.first++;
        if(ItPair.first == ItPair.second)
        {
            Utilities->CallLogPop(190);
            return(ItPair.first); // nothing found
        }
        if(ItPair.first->second == PrefDirVectorPosition)
        {
            FoundFlag = true;
            Utilities->CallLogPop(191);
            return(ItPair.first);
        }
        ItPair.first++;
        if(ItPair.first == ItPair.second)
        {
            Utilities->CallLogPop(192);
            return(ItPair.first); // nothing found
        }
        if(ItPair.first->second == PrefDirVectorPosition)
        {
            FoundFlag = true;
            Utilities->CallLogPop(193);
            return(ItPair.first);
        }
        ItPair.first++;
        if(ItPair.first == ItPair.second)
        {
            Utilities->CallLogPop(194);
            return(ItPair.first); // nothing found
        }
        if(ItPair.first->second == PrefDirVectorPosition)
        {
            FoundFlag = true;
            Utilities->CallLogPop(195);
            return(ItPair.first);
        }
    }
    Utilities->CallLogPop(196);
    return(ItPair.first); // nothing found
}

// ---------------------------------------------------------------------------

int TOnePrefDir::GetOnePrefDirPosition(int Caller, int HLoc, int VLoc)
/*
      Although there may be up to four entries at one H & V position this function gets just one. It is
      used in EraseFromPrefDirVectorAnd4MultiMap by being called as many times as there are PrefDir elements
      at H & V.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetOnePrefDirPosition," + AnsiString(HLoc) + "," + AnsiString(VLoc));
    THVPair PrefDir4MultiMapKeyPair;

    PrefDir4MultiMapKeyPair.first = HLoc;
    PrefDir4MultiMapKeyPair.second = VLoc;
    std::pair<TPrefDir4MultiMapIterator, TPrefDir4MultiMapIterator>ItPair;

    ItPair = PrefDir4MultiMap.equal_range(PrefDir4MultiMapKeyPair);
    if(ItPair.first == ItPair.second) // nothing found
    {
        Utilities->CallLogPop(197);
        return(-1);
    }
    else
    {
        Utilities->CallLogPop(198);
        return(ItPair.first->second);
    }
}

// ---------------------------------------------------------------------------

void TOnePrefDir::RealignAfterTrackErase(int Caller, int ErasedTrackVectorPosition)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RealignAfterTrackErase," + AnsiString(ErasedTrackVectorPosition));
    bool ErasedFlag = false;

    if(ErasedTrackVectorPosition > -1) // should be in calling function but include here as a safeguard
    {
        if(PrefDirSize() == 0)
        {
            Utilities->CallLogPop(1511);
            return;
        }
        for(int x = (PrefDirSize() - 1); x >= 0; x--) // reverse because of erase
        {
            ErasedFlag = false;
            // use 'else' to ensure don't try to access an erased element
            if(PrefDirVector.at(x).TrackVectorPosition == ErasedTrackVectorPosition)
            {
                ErasePrefDirElementAt(11, x);
                ErasedFlag = true;
            }
            else if(PrefDirVector.at(x).Conn[0] == ErasedTrackVectorPosition)
            {
                ErasePrefDirElementAt(12, x);
                ErasedFlag = true;
            }
            else if(PrefDirVector.at(x).Conn[1] == ErasedTrackVectorPosition)
            {
                ErasePrefDirElementAt(13, x);
                ErasedFlag = true;
            }
            else if(PrefDirVector.at(x).Conn[2] == ErasedTrackVectorPosition)
            {
                ErasePrefDirElementAt(9, x);
                ErasedFlag = true;
            }
            else if(PrefDirVector.at(x).Conn[3] == ErasedTrackVectorPosition)
            {
                ErasePrefDirElementAt(10, x);
                ErasedFlag = true;
            }
            if(!ErasedFlag)
            {
                // don't use 'else' here as may be more than one that need decrementing
                if(PrefDirVector.at(x).TrackVectorPosition > ErasedTrackVectorPosition)
                {
                    PrefDirVector.at(x).TrackVectorPosition--;
                }
                if(PrefDirVector.at(x).Conn[0] > ErasedTrackVectorPosition)
                {
                    PrefDirVector.at(x).Conn[0]--;
                }
                if(PrefDirVector.at(x).Conn[1] > ErasedTrackVectorPosition)
                {
                    PrefDirVector.at(x).Conn[1]--;
                }
                if(PrefDirVector.at(x).Conn[2] > ErasedTrackVectorPosition)
                {
                    PrefDirVector.at(x).Conn[2]--;
                }
                if(PrefDirVector.at(x).Conn[3] > ErasedTrackVectorPosition)
                {
                    PrefDirVector.at(x).Conn[3]--;
                }
            }
        }
    }
    Utilities->CallLogPop(1434);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::CalcDistanceAndSpeed(int Caller, int &OverallDistance, int &OverallSpeedLimit, bool &LeadingPointsAtLastElement)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CalcDistanceAndSpeed");
    OverallDistance = 0;
    OverallSpeedLimit = 0;
    LeadingPointsAtLastElement = false;
    if(PrefDirSize() == 0) // shouldn't be empty when this called
    {
        Utilities->CallLogPop(1491);
        return;
    }
    if((LastElementPtr(21)->TrackType == Points) && (LastElementPtr(22)->ELinkPos != 1) && (LastElementPtr(23)->ELinkPos != 3))
    {
        LeadingPointsAtLastElement = true;
        Utilities->CallLogPop(1492);
        return;
    }
    for(unsigned int x = 0; x < PrefDirSize(); x++)
    {
        TPrefDirElement PrefDirElement = GetFixedPrefDirElementAt(166, x);
        if((PrefDirElement.GetELinkPos() > 1) || (PrefDirElement.GetXLinkPos() > 1)) // 'or' because points may have one == 0 & other == 3
        {
            OverallDistance += PrefDirElement.Length23;
            if(OverallSpeedLimit != -1) // if set to -1 there are mixed speed limits
            {
                if(x == 0)
                {
                    OverallSpeedLimit = PrefDirElement.SpeedLimit23;
                }
                else
                {
                    if(OverallSpeedLimit != PrefDirElement.SpeedLimit23)
                    {
                        OverallSpeedLimit = -1;
                    }
                }
            }
        }
        else
        {
            OverallDistance += PrefDirElement.Length01;
            if(OverallSpeedLimit != -1) // if set to -1 there are mixed speed limits
            {
                if(x == 0)
                {
                    OverallSpeedLimit = PrefDirElement.SpeedLimit01;
                }
                else
                {
                    if(OverallSpeedLimit != PrefDirElement.SpeedLimit01)
                    {
                        OverallSpeedLimit = -1;
                    }
                }
            }
        }
    }
    Utilities->CallLogPop(1529);
}

// ---------------------------------------------------------------------------

void TOnePrefDir::WritePrefDirToImage(int Caller, Graphics::TBitmap *Bitmap)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WritePrefDirToImage");
    if(PrefDirSize() == 0)
    {
        Utilities->CallLogPop(1564);
        return;
    }
    int H, V, HLoc, VLoc, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3;
    bool FoundFlag;
    TPrefDir4MultiMapIterator MMIT = PrefDir4MultiMap.begin();
    TPrefDirElement PrefDirElement0, PrefDirElement1, PrefDirElement2, PrefDirElement3;

    while(MMIT != PrefDir4MultiMap.end())
    {
        HLoc = MMIT->first.first;
        VLoc = MMIT->first.second;
        GetVectorPositionsFromPrefDir4MultiMap(7, HLoc, VLoc, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
        H = HLoc - Track->GetHLocMin();
        V = VLoc - Track->GetVLocMin();
        // always found in order, any missing have PrefDirPosx == -1
        if(PrefDirPos0 > -1)
        {
            PrefDirElement0 = GetFixedPrefDirElementAt(174, PrefDirPos0); // PrefDirPos0 should always be > -1 but leave as a precaution
        }
        if(PrefDirPos1 > -1)
        {
            PrefDirElement1 = GetFixedPrefDirElementAt(175, PrefDirPos1);
        }
        if(PrefDirPos2 > -1)
        {
            PrefDirElement2 = GetFixedPrefDirElementAt(176, PrefDirPos2);
        }
        if(PrefDirPos3 > -1)
        {
            PrefDirElement3 = GetFixedPrefDirElementAt(177, PrefDirPos3);
        }
        if(PrefDirPos3 > -1) // 4 found, mark all PrefDirs bidirectional (operator == ensures no duplicates in ConsolidatePrefDirs)
        {
            // need to plot all 4 in order to obtain all the direction graphics
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, true)); // green
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, true)); // green
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement3.GetRouteGraphicPtr(false, true)); // green
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement3.GetDirectionRouteGraphicPtr(false, true)); // green
            MMIT++;
            MMIT++;
            MMIT++;
            MMIT++;
        }
        else if(PrefDirPos2 > -1) // 3 found, one PrefDir bidirectional & other unidirectional
        {
            if(PrefDirElement0.EXNumber == PrefDirElement1.EXNumber)
            {
                // 0 & 1 constitute the bidirectional PrefDir
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, false)); // red
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, false)); // red
                MMIT++;
                MMIT++;
                MMIT++;
            }
            else if(PrefDirElement0.EXNumber == PrefDirElement2.EXNumber)
            {
                // 0 & 2 constitute the bidirectional PrefDir
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, false)); // red
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, false)); // red
                MMIT++;
                MMIT++;
                MMIT++;
            }
            else
            {
                // 1 & 2 constitute the bidirectional PrefDir
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement2.GetDirectionRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, false)); // red
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, false)); // red
                MMIT++;
                MMIT++;
                MMIT++;
            }
        }
        else if(PrefDirPos1 > -1) // 2 found, either 1 bidirectional or 2 unidirectional
        {
            if(PrefDirElement0.EXNumber == PrefDirElement1.EXNumber)
            {
                // 0 & 1 constitute the bidirectional PrefDir
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, true)); // green
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, true)); // green
                MMIT++;
                MMIT++;
            }
            else
            {
                // 2 unidirectional PrefDirs
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, false)); // red
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, false)); // red
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetRouteGraphicPtr(false, false)); // red
                Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement1.GetDirectionRouteGraphicPtr(false, false)); // red
                MMIT++;
                MMIT++;
            }
        }
        else if(PrefDirPos0 > -1) // 1 found, must be unidirectional
        {
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetRouteGraphicPtr(false, false)); // red
            Bitmap->Canvas->Draw((H * 16), (V * 16), PrefDirElement0.GetDirectionRouteGraphicPtr(false, false)); // red
            MMIT++;
        }
    }
    Utilities->CallLogPop(1565);
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::PresetAutoRouteElementValid(int Caller, TPrefDirElement ElementIn, int EntryPos) // added at v1.2.0
/*
      Checks ElementIn and returns true only if a single prefdir set at that H&V, with EntryPos giving entry position, not points, crossovers,
      level crossing, signals with wrong direction set, or buffers.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PresetAutoRouteElementValid");
    int PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3;
    bool FoundFlag;
    TPrefDir4MultiMapIterator MMIT = PrefDir4MultiMap.begin();
    TPrefDirElement PrefDirElement0, PrefDirElement1, PrefDirElement2, PrefDirElement3;

    if((ElementIn.TrackType == Points) || (ElementIn.TrackType == Crossover) || (ElementIn.TrackType == Buffers) || (Track->IsLCAtHV(49, ElementIn.HLoc,
                                                                                                                                     ElementIn.VLoc)))
    {
        Utilities->CallLogPop(1982);
        return(false);
    }
    if((ElementIn.TrackType == SignalPost) && (ElementIn.Config[EntryPos] == Signal)) // Signal is at entry end, i.e. against preferred direction
    {
        Utilities->CallLogPop(1983);
        return(false);
    }
/*    if((ElementIn.TrackType == SignalPost) && (ElementIn.SigAspect == TTrackElement::GroundSignal)) //ground signal allowed at v2.14.0
    {
        Utilities->CallLogPop(1995);
        return(false);
    }
*/
// Now check that there is only a single prefdir set
    GetVectorPositionsFromPrefDir4MultiMap(8, ElementIn.HLoc, ElementIn.VLoc, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
// always found in order, any missing have PrefDirPosx == -1
    if(PrefDirPos0 > -1)
    {
        PrefDirElement0 = GetFixedPrefDirElementAt(213, PrefDirPos0); // PrefDirPos0 should always be > -1 but leave as a precaution
    }
    if(PrefDirPos1 > -1)
    {
        PrefDirElement1 = GetFixedPrefDirElementAt(214, PrefDirPos1);
    }
    if(PrefDirPos2 > -1)
    {
        PrefDirElement2 = GetFixedPrefDirElementAt(215, PrefDirPos2);
    }
    if(PrefDirPos3 > -1)
    {
        PrefDirElement3 = GetFixedPrefDirElementAt(216, PrefDirPos3);
    }
    if(PrefDirPos3 > -1) // 4 found, all bidirectional
    {
        Utilities->CallLogPop(1984);
        return(false);
    }
    else if(PrefDirPos2 > -1) // 3 found, one PrefDir bidirectional & other unidirectional
    {
        if((PrefDirElement0.XLinkPos == EntryPos) || (PrefDirElement1.XLinkPos == EntryPos) || (PrefDirElement2.XLinkPos == EntryPos))
        {
            Utilities->CallLogPop(1985);
            return(false);
        }
        else
        {
            Utilities->CallLogPop(1986);
            return(true);
        }
    }
    else if(PrefDirPos1 > -1) // 2 found, either 1 bidirectional or 2 unidirectional
    {
        if((PrefDirElement0.XLinkPos == EntryPos) || (PrefDirElement1.XLinkPos == EntryPos))
        {
            Utilities->CallLogPop(1987);
            return(false);
        }
        else
        {
            Utilities->CallLogPop(1988);
            return(true);
        }
    }
    else if(PrefDirPos0 > -1) // one found, make sure in correct direction
    {
        if(PrefDirElement0.XLinkPos == EntryPos)
        {
            Utilities->CallLogPop(1989);
            return(false);
        }
        else
        {
            Utilities->CallLogPop(1990);
            return(true);
        }
    }
    else
    {
        Utilities->CallLogPop(1991);
        return(false); // none found
    }
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::PresetAutoRouteDiagonalFouledByTrack(int Caller, TPrefDirElement ElementIn, int XLink)
{
/* //Added at v2.1.0
      Called by GetStartAndEndPrefDirElements, which in turn is called by PresetAutoSigRoutesButtonClick. Checks for a diagonal link in
      the autosigsroute being fouled by an adjacent track with a corresponding link that meets at the diagonal link, and if it is it
      returns true and prevents the route being set.  Note that adjacent track consisting of buffers, gaps and continuations at the
      diagonal link are also excluded though they need not be, but it makes the check code simpler and such adjacent track is untidy
      and can be modelled better anyway.

      Enter with PrefDirElement whose XLink is to be checked for track that fouls a diagonal.
      If XLink is anything but 1,3,7 or 9 return false - no fouling as not a diagonal.
      for XLink = 1, potentially fouled diagonals are at H-1, V, Lk 3 & H, V-1, Lk 7
      for XLink = 3, potentially fouled diagonals are at H+1, V, Lk 1 & H, V-1 Lk 9
      for XLink = 7, potentially fouled diagonals are at H-1, V, Lk 9 & H, V+1 Lk 1
      for XLink = 9, potentially fouled diagonals are at H+1, V, Lk 7 & H, V+1 Lk 3
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PresetAutoRouteDiagonalFouledByTrack," + ElementIn.HLoc + "," +
                                 ElementIn.VLoc + "," + XLink);
    int TrackVecPos;
    bool TrackFoundFlag;
    TTrackElement TempTrackElement;

    if((XLink == 2) || (XLink == 4) || (XLink == 6) || (XLink == 8))
    {
        Utilities->CallLogPop(2047);
        return(false);
    }
// for XLink = 1, potentially fouled diagonals are at H-1, V, Lk 3 & H, V-1, Lk 7
    if(XLink == 1)
    {
        TrackVecPos = Track->GetVectorPositionFromTrackMap(48, ElementIn.HLoc - 1, ElementIn.VLoc, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(898, TrackVecPos);
            if((TempTrackElement.Link[0] == 3) || (TempTrackElement.Link[1] == 3) || (TempTrackElement.Link[2] == 3) || (TempTrackElement.Link[3] == 3))
            {
                Utilities->CallLogPop(2048);
                return(true);
            }
        }
        TrackVecPos = Track->GetVectorPositionFromTrackMap(49, ElementIn.HLoc, ElementIn.VLoc - 1, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(899, TrackVecPos);
            if((TempTrackElement.Link[0] == 7) || (TempTrackElement.Link[1] == 7) || (TempTrackElement.Link[2] == 7) || (TempTrackElement.Link[3] == 7))
            {
                Utilities->CallLogPop(2049);
                return(true);
            }
        }
    }
// for XLink = 3, potentially fouled diagonals are at H+1, V, Lk 1 & H, V-1 Lk 9
    if(XLink == 3)
    {
        TrackVecPos = Track->GetVectorPositionFromTrackMap(50, ElementIn.HLoc + 1, ElementIn.VLoc, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(900, TrackVecPos);
            if((TempTrackElement.Link[0] == 1) || (TempTrackElement.Link[1] == 1) || (TempTrackElement.Link[2] == 1) || (TempTrackElement.Link[3] == 1))
            {
                Utilities->CallLogPop(2050);
                return(true);
            }
        }
        TrackVecPos = Track->GetVectorPositionFromTrackMap(51, ElementIn.HLoc, ElementIn.VLoc - 1, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(901, TrackVecPos);
            if((TempTrackElement.Link[0] == 9) || (TempTrackElement.Link[1] == 9) || (TempTrackElement.Link[2] == 9) || (TempTrackElement.Link[3] == 9))
            {
                Utilities->CallLogPop(2051);
                return(true);
            }
        }
    }
// for XLink = 7, potentially fouled diagonals are at H-1, V, Lk 9 & H, V+1 Lk 1
    if(XLink == 7)
    {
        TrackVecPos = Track->GetVectorPositionFromTrackMap(52, ElementIn.HLoc - 1, ElementIn.VLoc, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(902, TrackVecPos);
            if((TempTrackElement.Link[0] == 9) || (TempTrackElement.Link[1] == 9) || (TempTrackElement.Link[2] == 9) || (TempTrackElement.Link[3] == 9))
            {
                Utilities->CallLogPop(2052);
                return(true);
            }
        }
        TrackVecPos = Track->GetVectorPositionFromTrackMap(53, ElementIn.HLoc, ElementIn.VLoc + 1, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(903, TrackVecPos);
            if((TempTrackElement.Link[0] == 1) || (TempTrackElement.Link[1] == 1) || (TempTrackElement.Link[2] == 1) || (TempTrackElement.Link[3] == 1))
            {
                Utilities->CallLogPop(2053);
                return(true);
            }
        }
    }
// for XLink = 9, potentially fouled diagonals are at H+1, V, Lk 7 & H, V+1 Lk 3
    if(XLink == 9)
    {
        TrackVecPos = Track->GetVectorPositionFromTrackMap(54, ElementIn.HLoc + 1, ElementIn.VLoc, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(904, TrackVecPos);
            if((TempTrackElement.Link[0] == 7) || (TempTrackElement.Link[1] == 7) || (TempTrackElement.Link[2] == 7) || (TempTrackElement.Link[3] == 7))
            {
                Utilities->CallLogPop(2054);
                return(true);
            }
        }
        TrackVecPos = Track->GetVectorPositionFromTrackMap(55, ElementIn.HLoc, ElementIn.VLoc + 1, TrackFoundFlag);
        if(TrackFoundFlag)
        {
            TempTrackElement = Track->TrackElementAt(905, TrackVecPos);
            if((TempTrackElement.Link[0] == 3) || (TempTrackElement.Link[1] == 3) || (TempTrackElement.Link[2] == 3) || (TempTrackElement.Link[3] == 3))
            {
                Utilities->CallLogPop(2055);
                return(true);
            }
        }
    }
    Utilities->CallLogPop(2056);
    return(false);
}

// ---------------------------------------------------------------------------

bool TOnePrefDir::GetStartAndEndPrefDirElements(int Caller, TPrefDirElement &StartElement, TPrefDirElement &EndElement, int &LastIteratorValue)
{
/* Called by PresetAutoSigRoutesButtonClick in the Interface unit.  LastIteratorValue gives the position in EveryPrefDir to start from.  Search
      EveryPrefDir for continuations (facing inwards wrt pref dir) or non-ground signals in single direction pref dirs, and when find one track forwards
      to the next non-ground signal or continuation.  If, before finding a valid signal or continuation find points, crossover, level crossing or buffers,
      or an element that is already in a route, stop tracking and continue with the search for another valid continuation or signal.  When find a suitable
      pair, return the elements in StartElement and EndElement, and also the LastIteratorValue ready for the next call.
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetStartAndEndPrefDirElements," + AnsiString(LastIteratorValue));
    TOnePrefDir::TPrefDirVectorIterator PDVIt;
    bool FoundFlag, ContFlag, FoundElements = false;
    int PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3;
    TPrefDirElement NextElement;

    for(PDVIt = (PrefDirVector.begin() + LastIteratorValue); PDVIt < PrefDirVector.end(); PDVIt++)
    {
        LastIteratorValue++;
        ContFlag = false;
        if((PDVIt->TrackType != SignalPost) && (PDVIt->TrackType != Continuation))
        {
            continue;
        }
/*        if((PDVIt->TrackType == SignalPost) && (PDVIt->SigAspect == TTrackElement::GroundSignal)) //ground signal start permitted at v2.14.0
        {
            continue;
        }
*/
// if(AllRoutes::TrackIsInARoute(, PDVIt->TrackVectorPosition, PDVIt->EntryPos) continue; //already in a route - no, don't check start position as if a signal might well be at end of an existing route
        // found a potential route start point
        if(PresetAutoRouteDiagonalFouledByTrack(0, *PDVIt, PDVIt->XLink)) // Added at v2.1.0
        {
            continue;
        }
        if(PresetAutoRouteElementValid(0, *PDVIt, PDVIt->ELinkPos))
        {
            // check if continuation either in a route or with prefdir facing 'End' (OK if find it as EndElement, but not as StartElement)
            if(PDVIt->TrackType == Continuation)
            {
                if(AllRoutes->TrackIsInARoute(18, PDVIt->TrackVectorPosition, PDVIt->ELinkPos))
                {
                    continue;
                }
                if(PDVIt->XLinkPos == 0) // position 0 is the continuation
                {
                    continue;
                }
            }
            StartElement = *PDVIt;
// in Glenn Mitchell's error log (14/04/13) the offending signal start position was 4680, problem was it linked to a point with pref dirs set on through track but signal linked to
            // diverging track on which there was no pref dir.  See below for 2 required changes.
        }
        else
        {
            continue;
        }
        // now track along until find a signal or continuation, checking validity for each element
        int NextTrackVectorPosition = PDVIt->Conn[PDVIt->GetXLinkPos()];
        GetVectorPositionsFromPrefDir4MultiMap(9, Track->TrackElementAt(878, NextTrackVectorPosition).HLoc,
                                               Track->TrackElementAt(879, NextTrackVectorPosition).VLoc, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
        if(PrefDirPos0 == -1) // no continuing prefdir
        {
            continue;
        }
        bool NextElementFoundFlag = false;
        if(GetFixedPrefDirElementAt(217, PrefDirPos0).ELinkPos == PDVIt->ConnLinkPos[PDVIt->GetXLinkPos()])
        {
            NextElement = GetFixedPrefDirElementAt(218, PrefDirPos0);
            NextElementFoundFlag = true;
        }
        if(PrefDirPos1 > -1)
        {
            if(GetFixedPrefDirElementAt(219, PrefDirPos1).ELinkPos == PDVIt->ConnLinkPos[PDVIt->GetXLinkPos()])
            {
                NextElement = GetFixedPrefDirElementAt(220, PrefDirPos1);
                NextElementFoundFlag = true;
            }
        }
        if(PrefDirPos2 > -1)
        {
            if(GetFixedPrefDirElementAt(221, PrefDirPos2).ELinkPos == PDVIt->ConnLinkPos[PDVIt->GetXLinkPos()])
            {
                NextElement = GetFixedPrefDirElementAt(222, PrefDirPos2);
                NextElementFoundFlag = true;
            }
        }
        if(PrefDirPos3 > -1)
        {
            if(GetFixedPrefDirElementAt(223, PrefDirPos3).ELinkPos == PDVIt->ConnLinkPos[PDVIt->GetXLinkPos()])
            {
                NextElement = GetFixedPrefDirElementAt(224, PrefDirPos3);
                NextElementFoundFlag = true;
            }
        }
        if(!NextElementFoundFlag)
        {
            continue; // Modified for release 1.3.2 (sent as beta to John Phillipson initially)
// throw(Exception("Failed to track prefdir in PresetAutoSigRoutesButtonClick (1)"));  //[GM error 14/04/13] for next release change this to 'continue;' to quit from trying to find the auto route (don't need to throw an exception)
        }
        while(true)
        {
            // check validity
            if(PresetAutoRouteDiagonalFouledByTrack(1, NextElement, NextElement.XLink)) // Added at v2.1.0
            {
                ContFlag = true;
                break;
            }
            if(!PresetAutoRouteElementValid(1, NextElement, NextElement.ELinkPos))
            {
                ContFlag = true;
                break;
            }
            // check if in a route, providing not a signal, as a signal might be at the start of a route
            if(NextElement.TrackType != SignalPost)
            {
                if(AllRoutes->TrackIsInARoute(17, NextElement.TrackVectorPosition, NextElement.ELinkPos))
                {
                    ContFlag = true;
                    break;
                }
            }
            if((NextElement.TrackType == SignalPost) || (NextElement.TrackType == Continuation))
            // can't be a gound signal as would have failed the validity test - can be at v2.14.0
            {
                EndElement = NextElement;
                break;
            }
            // get the next element in the sequence
            NextTrackVectorPosition = NextElement.Conn[NextElement.GetXLinkPos()];
            GetVectorPositionsFromPrefDir4MultiMap(10, Track->TrackElementAt(880, NextTrackVectorPosition).HLoc,
                                                   Track->TrackElementAt(881, NextTrackVectorPosition).VLoc, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
            if(PrefDirPos0 == -1) // no continuing prefdir
            {
                ContFlag = true;
                break;
            }
            if(GetFixedPrefDirElementAt(225, PrefDirPos0).ELinkPos == NextElement.ConnLinkPos[NextElement.GetXLinkPos()])
            {
                NextElement = GetFixedPrefDirElementAt(226, PrefDirPos0);
                continue;
            }
            if(PrefDirPos1 > -1)
            {
                if(GetFixedPrefDirElementAt(227, PrefDirPos1).ELinkPos == NextElement.ConnLinkPos[NextElement.GetXLinkPos()])
                {
                    NextElement = GetFixedPrefDirElementAt(228, PrefDirPos1);
                    continue;
                }
            }
            if(PrefDirPos2 > -1)
            {
                if(GetFixedPrefDirElementAt(229, PrefDirPos2).ELinkPos == NextElement.ConnLinkPos[NextElement.GetXLinkPos()])
                {
                    NextElement = GetFixedPrefDirElementAt(230, PrefDirPos2);
                    continue;
                }
            }
            if(PrefDirPos3 > -1)
            {
                if(GetFixedPrefDirElementAt(231, PrefDirPos3).ELinkPos == NextElement.ConnLinkPos[NextElement.GetXLinkPos()])
                {
                    NextElement = GetFixedPrefDirElementAt(232, PrefDirPos3);
                    continue;
                }
            }
            // had exception thrown here if NextElement not found, but could be a bridge where opposite track PrefDir set, in which case won't find it
            // found with Jonathan Kwok's DLR railway (17/11/12) where undertrack PrefDir not set just west of Poplar.  Hence first test if element is a bridge
            // and if so set ContFlag to true & break (same as not finding PrefDir element at all).  Modified at version 1.3.1
            // note that it's not NextElement that is to be examined but NextTrackVectorPosition, which can be found easily by using PrefDirPos0 (there will be a
            // PrefDirPos0 or would have exited earlier, and it doesn't matter that PrefDirPos0 isn't on the route in question because only the TrackType is needed)
            if(GetFixedPrefDirElementAt(243, PrefDirPos0).TrackType == Bridge)
            {
                ContFlag = true;
                break;
            }
            else
            {
                ContFlag = true; // Modified for release 1.3.2 (sent as beta to John Phillipson initially)
                                 // could drop the bridge test but keep it to show the change history
                break;
// throw(Exception("Failed to track prefdir in PresetAutoSigRoutesButtonClick (2)")); //[GM error 14/04/13] for next release set ContFlag to true & break' to quit from trying to find the auto route (don't need to throw an exception)
            }
        }
        if(ContFlag)
        {
            continue;
        }
        // else have start and end elements set & all elements valid, so set up the route segment
        FoundElements = true;
        break;
    }
    if(FoundElements)
    {
        Utilities->CallLogPop(1992);
        return(true);
    }
    else
    {
        Utilities->CallLogPop(1993);
        return(false);
    }
}

// ---------------------------------------------------------------------------
// TOneRoute
// ---------------------------------------------------------------------------

bool TOneRoute::GetPreferredRouteStartElement(int Caller, int HLoc, int VLoc, TOnePrefDir *EveryPrefDir, bool AutoSigsFlag)
{
/* General:
      The basis of all these route setting functions, preferred and non-preferred, is that a SearchVector is set up
      containing all the new elements to form the route.  When complete, the SearchVector is converted into route
      elements, either as a new route, or an extension to an existing route.  The AutoSigs flag determines whether the
      route will use automatic signals or not.
      For preferred and non-preferred routes, all new elements (as opposed to those already in existing routes) go
      into the SearchVector.  For non-preferred routes, trackelements are selected that are not necessarily PrefDir
      elements, so additional work is needed to complete all their members before they are ready for conversion into
      a route - see SetRemainingSearchVectorValues.  The call order is GetStart....; GetNext...,
      which includes the Search... function; [SetRemainingSearchVectorValues for non-preferred routes only], then
      ConvertAndAdd.......
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetPreferredRouteStartElement," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString((short)AutoSigsFlag));
    ClearRoute();
    int TrackVectorPosition;
    TTrackElement TrackElement;
    TPrefDirElement FirstElement, LastElement;

    if(!(Track->FindNonPlatformMatch(7, HLoc, VLoc, TrackVectorPosition, TrackElement)))
    {
        Utilities->CallLogPop(199);
        return(false);
    }
    if(AutoSigsFlag && (TrackElement.TrackType == Buffers)) // added at v1.2.0
    {
        TrainController->StopTTClockMessage(80, "Can't create an automatic signal route from buffers");
        Utilities->CallLogPop(1996);
        return(false);
    }
    else if((TrackElement.TrackType != SignalPost) && (TrackElement.TrackType != Buffers) && (TrackElement.TrackType != Continuation))
    {
        TrainController->StopTTClockMessage(7, "Must select a valid signal, buffers or continuation");
        Utilities->CallLogPop(200);
        return(false);
    }
    if(Track->IsLCAtHV(18, HLoc, VLoc))
    {
        TrainController->StopTTClockMessage(73, "Can't start a route on a level crossing");
        Utilities->CallLogPop(1909);
        return(false);
    }
// check if selected a train & disallow if so
    if(TrackElement.TrainIDOnElement > -1)
    {
        TrainController->StopTTClockMessage(9, "Can't start a route on a train");
        Utilities->CallLogPop(202);
        return(false);
    }
// check if selected a locked route element & disallow (can only be a 2-track element so only need check XLinkPos values of 0 & 1
    TPrefDirElement PrefDirElement;
    int LockedVectorNumber;

    if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(1, TrackVectorPosition, 0, PrefDirElement, LockedVectorNumber))
    {
        TrainController->StopTTClockMessage(10, "Can't start a route on a locked route");
        Utilities->CallLogPop(203);
        return(false);
    }
    if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(2, TrackVectorPosition, 1, PrefDirElement, LockedVectorNumber))
    {
        TrainController->StopTTClockMessage(11, "Can't start a route on a locked route");
        Utilities->CallLogPop(204);
        return(false);
    }
    StartSelectionRouteID = IDInt(-1);
    StartRoutePosition = TrackVectorPosition; // actual route start - may be element following StartRouteSelectPosition if select a
// signal in an autosig route & follow with a non-autosig route

    TPrefDirElement BlankElement;

    StartElement1 = BlankElement;
    StartElement2 = BlankElement; //not used in this routine but used in GetNextPreferred.... though could probably dispense with it there
// check it's in a PrefDir (could be 2 entries for two possible PrefDirs, can only select single track elements so can't have more than 2 PrefDirs)
    bool InPrefDirFlag = false;

    bool FoundFlag;
    int PrefDirPos0 = -1;
    int PrefDirPos1 = -1;
    int PrefDirPos2 = -1;
    int PrefDirPos3 = -1;

    EveryPrefDir->GetVectorPositionsFromPrefDir4MultiMap(3, Track->TrackElementAt(84, StartRoutePosition).HLoc,
                                                         Track->TrackElementAt(85, StartRoutePosition).VLoc, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
    int PrefDirVecPos[4] =
    {
        PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3
    };

    for(int x = 0; x < 4; x++)
    {
        int b = PrefDirVecPos[x];
        if(b > -1)
        {
            // only allow the appropriate exit route to be searched
            if(((TrackElement.TrackType == SignalPost) && (EveryPrefDir->GetFixedPrefDirElementAt(15, b).Config[EveryPrefDir->GetFixedPrefDirElementAt(16,
                                                                                                                                                       b).XLinkPos] == Signal)) || ((TrackElement.TrackType == Buffers) && (EveryPrefDir->GetFixedPrefDirElementAt(17,
                                                                                                                                                                                                                                                                   b).Config[EveryPrefDir->GetFixedPrefDirElementAt(18, b).XLinkPos] == Connection)) ||
               ((TrackElement.TrackType == Continuation) && (EveryPrefDir->GetFixedPrefDirElementAt(19,
                                                                                                    b).Config[EveryPrefDir->GetFixedPrefDirElementAt(20, b).XLinkPos] == Connection)))
            {
                InPrefDirFlag = true;
                StartElement1 = EveryPrefDir->GetFixedPrefDirElementAt(21, b);
                if(AutoSigsFlag)
                {
                    StartElement1.AutoSignals = true;
                }
                StartElement1.PrefDirRoute = true;
            }
        }
    }

    if(!InPrefDirFlag)
    {
        TrainController->StopTTClockMessage(12, "Route and preferred direction mismatch. If no preferred direction then only red routes can be used. Green and blue route directions must correspond to the preferred direction.");
        Utilities->CallLogPop(205);
        return(false);
    }
// look for exact match in a route first - can't be a bridge so can use a simple 'find'
    TAllRoutes::TRouteElementPair DummyPair;
    TAllRoutes::TRouteElementPair RoutePair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(14, TrackElement.HLoc, TrackElement.VLoc, DummyPair);

    if(DummyPair.first > -1) // if DummyPair exists then an error as start element can only be in one route (bridges not allowed)
    {
        throw Exception("Selection in two routes - should never happen!");
    }
    if(RoutePair.first > -1) // no need to examine DummyPair as start element can only be in one route (bridges not allowed)
    {
        if(RoutePair.second != AllRoutes->GetFixedRouteAt(1, RoutePair.first).PrefDirSize() - 1) // not last element in existing route so no good
        {
            TrainController->StopTTClockMessage(13, "Can't start a route within or at the start of an existing route");
            Utilities->CallLogPop(206);
            return(false);
        }
        TPrefDirElement RouteElement = AllRoutes->GetFixedRouteAt(2, RoutePair.first).GetFixedPrefDirElementAt(29, RoutePair.second);
        if(RouteElement.Conn[RouteElement.XLinkPos] < 0) // last element in existing route but nowhere to go!
        {
            TrainController->StopTTClockMessage(14, "No forward connection from this position");
            Utilities->CallLogPop(207);
            return(false);
        }
        if((RouteElement.Config[RouteElement.XLinkPos] != End) && (AllRoutes->TrackIsInARoute(9, RouteElement.Conn[RouteElement.XLinkPos],
                                                                                              RouteElement.ConnLinkPos[RouteElement.XLinkPos])))
        // last element in existing route but that route linked to another route (or a non-bridge 2-track element containing a route) so no good
        {
            TrainController->StopTTClockMessage(15, "Can't start a route at an element that links forward into an existing route");
            Utilities->CallLogPop(208);
            return(false);
        }
        StartSelectionRouteID = IDInt(AllRoutes->GetFixedRouteAt(158, RoutePair.first).RouteID);
        StartElement1 = AllRoutes->GetFixedRouteAt(3, RoutePair.first).GetFixedPrefDirElementAt(30,
                                                                                                AllRoutes->GetFixedRouteAt(4, RoutePair.first).PrefDirSize() - 1); // last element
        if(AutoSigsFlag)
        {
            StartElement1.AutoSignals = true;
        }
        StartElement1.PrefDirRoute = true;
        StartRoutePosition = StartElement1.TrackVectorPosition;
        Utilities->CallLogPop(209);
        return(true); // all retained values (StartElement1 & maybe 2; StartRoutePosition) set
    }

    else // no route started
    {
// check if selected position is adjacent to start or end of an existing route and disallow
        for(unsigned int a = 0; a < AllRoutes->AllRoutesSize(); a++)
        {
            FirstElement = AllRoutes->GetFixedRouteAt(5, a).GetFixedPrefDirElementAt(31, 0);
            if((StartElement1.Conn[0] > -1) && (StartElement1.Conn[0] == FirstElement.TrackVectorPosition))
            {
                TrainController->StopTTClockMessage(16, "Can't make selection adjacent to start of another route");
                Utilities->CallLogPop(210);
                return(false);
            }
            if((StartElement1.Conn[1] > -1) && (StartElement1.Conn[1] == FirstElement.TrackVectorPosition))
            {
                TrainController->StopTTClockMessage(17, "Can't make selection adjacent to start of another route");
                Utilities->CallLogPop(211);
                return(false);
            }
        }

// check if it's adjacent to end of an existing route,
        for(unsigned int a = 0; a < AllRoutes->AllRoutesSize(); a++)
        {
            LastElement = AllRoutes->GetFixedRouteAt(6, a).GetFixedPrefDirElementAt(32, AllRoutes->GetFixedRouteAt(7, a).PrefDirSize() - 1);
            if(LastElement.Conn[LastElement.XLinkPos] == StartRoutePosition)
            {
                TrainController->StopTTClockMessage(18, "Can't start a route adjacent to the end of an existing route");
                Utilities->CallLogPop(212);
                return(false);
            }
        }
        SearchVector.push_back(StartElement1);
        Utilities->CallLogPop(213);
        return(true);
    }
}

// ---------------------------------------------------------------------------

bool TOneRoute::GetNextPreferredRouteElement(int Caller, int HLoc, int VLoc, TOnePrefDir *EveryPrefDir, bool ConsecSignals, bool AutoSigsFlag,
                                             IDInt &ReqPosRouteID, bool &PointsChanged)

/*
      Return true if select valid next element, in which case the route is set & stored in SearchVector.  Return false for an invalid next element.

      Declare integers EndPosition (the position used) and ReqPosRouteID to hold (when required) the existing route selected (for linking to an existing route),
      this being set to -1 for not used.
      Check if selection is a valid track element, cancel if not, if select original start element or if select buffers
      with AutoSigsFlag set - would have no way out and no way to cancel the route with a train at the buffers.
      Check correct type of element - signal/buffers/continuation.
      Fail if train on element, or if selection not in EveryPrefDir.  Otherwise set EndElement1 & possibly also
      EndElement2 corresponding to the 2 possible PrefDir elements).
      Check if selection is first element in an existing route & if so set ReqPosRouteID, EndElement1, and set EndElement2 to
      blank as can only be one route at that element (can't select bridges).  Fail if in a route & not at start, or at start but route
      linked forward to another route.
      Check & fail if adjacent to start or end of an existing route, or if select the route that selected at start (though earlier check
      for same position as start should cover this)

      If there's a StartSelectionRouteID then StartElement1 will be set to the last entry in the selected route so use
      SearchForPreferredRoute to search for the selected end element from this start element.  If succeed then set the search vector
      graphics using SetRouteSearchVectorGraphics(AutoSigsFlag) & return true, for Interface to handle the flashing & time delay.  After the
      delay completes the Interface flasher calls ConvertAndAddPreferredRouteSearchVector to add the new route to the AllRoutesVectorPtr.
      If the search fails then return false.
      If there isn't a StartSelectionRouteID then the starting element is not already in a route, so it will have been stored
      in the SearchVector to ensure it's entered as part of the new route.
      First check whether the selected element (either EndElement1 or 2) is adjacent to the starting position and if so set the route to go
      directly to it (as opposed to going round a long loop to get to it just because that XLinkPos happens to be chosen first.  If not
      adjacent then first search on EndElement1, and if fail search on EndElement2 providing it's set.  If succeed
      set the search vector graphics as above and return.  If reach end of function then have failed to find a valid element,
      so return false, with an appropriate message if ConsecSignalsRoute set.
*/

{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetNextPreferredRouteElement," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString((short)ConsecSignals) + "," + AnsiString((short)AutoSigsFlag));
    int EndPosition; // the position selected
    int NewFailedPointsTVPos = -1; //added at v2.13.0 for point failures

    Track->LCFoundInAutoSigsRoute = false;
    Track->SuppressRouteFailMessage = false;
    TotalSearchCount = 0;
    ReqPosRouteID = IDInt(-1); // default value for not used
    TTrackElement TrackElement;
    TPrefDirElement EndElement1, EndElement2, BlankElement; // all blank to begin with, can only have max of 2 PrefDirs on a
    // given element as can't select 2-track elements
    if(!(Track->FindNonPlatformMatch(8, HLoc, VLoc, EndPosition, TrackElement))) // return if can't find one
    {
        Utilities->CallLogPop(214);
        return(false);
    }
    if(Track->IsLCAtHV(19, HLoc, VLoc))
    {
        TrainController->StopTTClockMessage(72, "Can't end a route on a level crossing");
        Utilities->CallLogPop(1908);
        return(false);
    }
// cancel selection if on original start element
    if(EndPosition == StartRoutePosition)
    {
        Utilities->CallLogPop(215);
        return(false);
    }
    if(AutoSigsFlag)
    {
        if(TrackElement.TrackType == Buffers)
        {
            TrainController->StopTTClockMessage(19, "Can't create an automatic signal route into buffers");
            Utilities->CallLogPop(216);
            return(false);
        }
    }
    if((TrackElement.TrackType != SignalPost) && (TrackElement.TrackType != Buffers) && (TrackElement.TrackType != Continuation))
    {
        TrainController->StopTTClockMessage(20, "Must select a valid signal, buffers or continuation");
        Utilities->CallLogPop(217);
        return(false);
    }
// check if train on element
    if(TrackElement.TrainIDOnElement > -1)
    {
        TrainController->StopTTClockMessage(22, "Can't end a route on a train");
        Utilities->CallLogPop(219);
        return(false);
    }
// disallow if not in EveryPrefDir & set EndElement(s)
    bool InPrefDirFlag = false;

    bool FoundFlag;
    int PrefDirPos0 = -1;
    int PrefDirPos1 = -1;
    int PrefDirPos2 = -1;
    int PrefDirPos3 = -1;

    EveryPrefDir->GetVectorPositionsFromPrefDir4MultiMap(4, Track->TrackElementAt(86, EndPosition).HLoc, Track->TrackElementAt(87, EndPosition).VLoc, FoundFlag,
                                                         PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
    int PrefDirVecPos[4] =
    {
        PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3
    };

    for(int x = 0; x < 4; x++)
    {
        int b = PrefDirVecPos[x];
        if(b > -1)
        {
            InPrefDirFlag = true;
            if(EndElement1.TrackVectorPosition == -1)
            {
                EndElement1 = EveryPrefDir->GetFixedPrefDirElementAt(33, b);
            }
            else
            {
                EndElement2 = EveryPrefDir->GetFixedPrefDirElementAt(34, b);
            }
        }
    }
    if(!InPrefDirFlag)
    {
        TrainController->StopTTClockMessage(23, "Route and preferred direction mismatch. If no preferred direction then only red routes can be used. Green and blue route directions must correspond to the preferred direction.");
        Utilities->CallLogPop(220);
        return(false);
    }
// check if in an existing route - can't be a bridge so can use a simple 'find'
// bool InRoute = false;
    TAllRoutes::TRouteElementPair DummyPair;
    TAllRoutes::TRouteElementPair RoutePair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(15, TrackElement.HLoc, TrackElement.VLoc, DummyPair);

    if(RoutePair.first > -1)
    {
        if(RoutePair.second != 0) // not first element in existing route so no good
        {
            TrainController->StopTTClockMessage(24, "Can't end a route within or at the end of an existing route");
            Utilities->CallLogPop(221);
            return(false);
        }
        TPrefDirElement RouteElement = AllRoutes->GetFixedRouteAt(8, RoutePair.first).GetFixedPrefDirElementAt(35, RoutePair.second);
// if((RouteElement.Config[RouteElement.ELinkPos] != End) && (AllRoutes->TrackIsInARoute(, RouteElement.Conn[RouteElement.ELinkPos], RouteElement.ELinkPos)))
        if((RouteElement.Config[RouteElement.ELinkPos] != End) && (AllRoutes->TrackIsInARoute(10, RouteElement.Conn[RouteElement.ELinkPos],
                                                                                              RouteElement.ConnLinkPos[RouteElement.ELinkPos]))) // amended at v1.3.0 - had omitted ConnLinkPos - see above
        // discovered when timetable building for Joshua Coupe's railway.  Also affects non-preferred routes - see below
        // first element in existing route but that route linked to another route (or a non-bridge 2-track element containing a route) so no good
        {
            TrainController->StopTTClockMessage(25, "Can't start a route within or at the end of an existing route");
            Utilities->CallLogPop(222);
            return(false);
        }
        EndElement1 = RouteElement;
        EndElement2 = BlankElement; // only need the route element
        EndPosition = EndElement1.TrackVectorPosition;
        ReqPosRouteID = IDInt(AllRoutes->GetFixedRouteAt(160, RoutePair.first).RouteID);
    }
// set the H&V limits for the search, all points on search path must lie within 15 elements greater than the box of which the line between
// start and finish is a diagonal line [dropped as not a good strategy because gaps interfered with direct line searches - instead
// introduced TotalSearchCount and now use that to limit searches. Leave in though in case rethink strategy later on]

    if(EndElement1.HLoc >= StartElement1.HLoc)
    {
        SearchLimitLowH = StartElement1.HLoc - 15;
        SearchLimitHighH = EndElement1.HLoc + 15;
    }
    else
    {
        SearchLimitLowH = EndElement1.HLoc - 15;
        SearchLimitHighH = StartElement1.HLoc + 15;
    }
    if(EndElement1.VLoc >= StartElement1.VLoc)
    {
        SearchLimitLowV = StartElement1.VLoc - 15;
        SearchLimitHighV = EndElement1.VLoc + 15;
    }
    else
    {
        SearchLimitLowV = EndElement1.VLoc - 15;
        SearchLimitHighV = StartElement1.VLoc + 15;
    }
/* dropped this for v0.4d - prevents ability to set routes for gaps that are widely separated, ok without it as search limited by SearchVector size
      check & TotalSearchCounts check
      if((abs(EndElement1.HLoc - StartElement1.HLoc) > 120) || (abs(EndElement1.VLoc - StartElement1.VLoc) > 120))
       {
       TrainController->StopTTClockMessage(65, "Unable to reach the selected element - too far ahead");
       Utilities->CallLogPop(1693);
       return false;
       }
*/
// check if adjacent to start and disallow
    for(unsigned int a = 0; a < AllRoutes->AllRoutesSize(); a++)
    {
        int AdjPosition = AllRoutes->GetFixedRouteAt(9, a).GetFixedPrefDirElementAt(36, 0).TrackVectorPosition;
        int AdjLinkPos = AllRoutes->GetFixedRouteAt(218, a).GetFixedPrefDirElementAt(244, 0).ELinkPos; // added at v1.3.1
// if((EndElement1.Config[EndElement1.XLinkPos] != End) &&
// (EndElement1.Conn[EndElement1.XLinkPos] == AdjPosition))
        if((EndElement1.Config[EndElement1.XLinkPos] != End) && (EndElement1.Conn[EndElement1.XLinkPos] == AdjPosition)
           &&  // changed at v1.3.1 to allow a route end adjacent to an element with a route that doesn't link to the ending route
           (EndElement1.ConnLinkPos[EndElement1.XLinkPos] == AdjLinkPos))
        {
            TrainController->StopTTClockMessage(26, "Can't end a route adjacent to the start of an existing route");
            Utilities->CallLogPop(223);
            return(false);
        }
// else if((EndElement2.TrackVectorPosition > -1) && (EndElement2.Config[EndElement2.XLinkPos] != End) &&
// (EndElement2.Conn[EndElement2.XLinkPos] == AdjPosition))
        else if((EndElement2.TrackVectorPosition > -1) && (EndElement2.Config[EndElement2.XLinkPos] != End)
                && // changed at v1.3.1 to allow a route end adjacent to an element with a route that doesn't link to the ending route
                (EndElement2.Conn[EndElement2.XLinkPos] == AdjPosition) && (EndElement2.ConnLinkPos[EndElement2.XLinkPos] == AdjLinkPos))
        {
            TrainController->StopTTClockMessage(27, "Can't end a route adjacent to the start of an existing route");
            Utilities->CallLogPop(224);
            return(false);
        }
// check if adjacent to end of a route & disallow
        TPrefDirElement EndOfRouteElement = AllRoutes->GetFixedRouteAt(10, a).GetFixedPrefDirElementAt(37, AllRoutes->GetFixedRouteAt(11, a).PrefDirSize() - 1);
        if((EndOfRouteElement.Config[EndOfRouteElement.XLinkPos] != End) && (EndOfRouteElement.Conn[EndOfRouteElement.XLinkPos] == EndPosition))
        {
            TrainController->StopTTClockMessage(28, "Can't end a route adjacent to the end of an existing route");
            Utilities->CallLogPop(225);
            return(false);
        }
    }

// check for same route as start element
    if((ReqPosRouteID > -1) && (ReqPosRouteID == StartSelectionRouteID))
    {
        TrainController->StopTTClockMessage(29, "Can't select same route as started in");
        Utilities->CallLogPop(226);
        return(false);
    }
// check for a looping route
    if((ReqPosRouteID > -1) && (StartSelectionRouteID > -1))
    {
        if(AllRoutes->CheckForLoopingRoute(0, EndElement1.GetTrackVectorPosition(), EndElement1.GetXLinkPos(), StartElement1.GetTrackVectorPosition()))
        {
            TrainController->StopTTClockMessage(69, "Can't create a route that loops back on itself");
            Utilities->CallLogPop(1844);
            return(false);
        }
    }
// if there's a StartSelectionRouteID StartElement1 will be set to the last entry in the selected route
// so search from this element.  No need to add StartElement to the SearchVector since it already exists in a route
// and don't want to add it again
    if(StartSelectionRouteID > -1)
    {
        if(SearchForPreferredRoute(0, StartElement1, StartElement1.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                   AutoSigsFlag, false))
        {
            SetRouteSearchVectorGraphics(0, AutoSigsFlag, true); // change graphic colour to the route colour
            if(PointsToBeChanged(5, NewFailedPointsTVPos))
            {
                if(NewFailedPointsTVPos > -1)
                {
                    TTrackElement TE = Track->TrackElementAt(1478, NewFailedPointsTVPos);
                    TrainController->StopTTClockMessage(97, "Points at " + TE.ElementID +
                        " failed during route setting.");
                    Utilities->CallLogPop(2488);
                    return(false);
                }
                PointsChanged = true;
            }
            Utilities->CallLogPop(227);
            return(true);
        }
        else if(!Track->SuppressRouteFailMessage)
        {
            //corrected at v2.7.0 - brackets were missed earlier so if SearchForPreferredRoute failed & else condition failed too then returned false with no message
            TrainController->StopTTClockMessage(30, Track->RouteFailMessage);
            Utilities->CallLogPop(228);
            return(false);
        }
    }
    else
    {
// Note: StartElement not in an existing route so was added to the searchvector during the earlier function
// First check if selection adjacent to start element and if so use that [can't be as can't have 2 consecutive signals, but leave in]

// added the XLinkPos checks because of Matt Blades error reported on 28/06/11, where StartElement2 matched EndPosition spuriously
// note that a blank element will have XLinkPos set to -1
        if((StartElement1.XLinkPos > -1) && (StartElement1.Conn[StartElement1.XLinkPos] == EndPosition))
        {
            if(SearchForPreferredRoute(1, StartElement1, StartElement1.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                       AutoSigsFlag, false))
            {
                SetRouteSearchVectorGraphics(1, AutoSigsFlag, true); // change graphic colour to the route colour
                if(PointsToBeChanged(6, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1480, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(99, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2490);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(229);
                return(true);
            }
            else
            {
                if(!Track->SuppressRouteFailMessage)
                {
                    TrainController->StopTTClockMessage(31, Track->RouteFailMessage);
                }
                Utilities->CallLogPop(230);
                return(false);
            }
        }
        else if((StartElement2.XLinkPos > -1) && (StartElement2.Conn[StartElement2.XLinkPos] == EndPosition))
        {
            if(SearchForPreferredRoute(2, StartElement2, StartElement2.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                       AutoSigsFlag, false))
            {
                SetRouteSearchVectorGraphics(2, AutoSigsFlag, true); // change graphic colour to the route colour
                if(PointsToBeChanged(7, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1482, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(101, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2492);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(231);
                return(true);
            }
            else
            {
                if(!Track->SuppressRouteFailMessage)
                {
                    TrainController->StopTTClockMessage(32, Track->RouteFailMessage);
                }
                Utilities->CallLogPop(232);
                return(false);
            }
        }
        // now start off in the best direction
        int BestPos = Track->FindClosestLinkPosition(0, StartRoutePosition, EndPosition); // can only be 0 or 1
        // the following logic is very unstructured as extra bits have been added at different times and I'm reluctant to remove earlier bits in case
        // they cover situations that might be overlooked.  A full analysis would enable it to be tidied up but it works (so far!) so I'll leave it as it is
        // unless new problems are found.
        if(StartElement1.XLinkPos == BestPos)
        {
            TotalSearchCount = 0; // added at v0.4f to give each exit direction a full chance to find required position
            if(SearchForPreferredRoute(3, StartElement1, StartElement1.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                       AutoSigsFlag, false))
            {
                SetRouteSearchVectorGraphics(3, AutoSigsFlag, true); // change graphic colour to the route colour
                if(PointsToBeChanged(8, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1484, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(103, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2494);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(233);
                return(true);
            }
            else if(StartElement2.TrackVectorPosition > -1)
            {
                TotalSearchCount = 0; // added at v0.4f to give each exit direction a full chance to find required position
                if(SearchForPreferredRoute(4, StartElement2, StartElement2.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                           AutoSigsFlag, false))
                {
                    SetRouteSearchVectorGraphics(4, AutoSigsFlag, true); // change graphic colour to the route colour
                if(PointsToBeChanged(9, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1486, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(105, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2496);
                        return(false);
                    }
                    PointsChanged = true;
                }
                    Utilities->CallLogPop(234);
                    return(true);
                }
            }
        }
        else if(StartElement2.TrackVectorPosition > -1)
        {
            TotalSearchCount = 0; // added at v0.4f to give each exit direction a full chance to find required position
            if(SearchForPreferredRoute(5, StartElement2, StartElement2.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                       AutoSigsFlag, false))
            {
                SetRouteSearchVectorGraphics(6, AutoSigsFlag, true); // change graphic colour to the route colour
                if(PointsToBeChanged(10, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1488, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(107, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2498);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(1857);
                return(true);
            }
            else if(SearchForPreferredRoute(8, StartElement1, StartElement1.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                            AutoSigsFlag, false))
            {
                SetRouteSearchVectorGraphics(7, AutoSigsFlag, true); // change graphic colour to the route colour
                if(PointsToBeChanged(11, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1490, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(109, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2500);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(1858);
                return(true);
            }
        }
        else if(StartElement1.XLinkPos == (1 - BestPos))
        // added at v0.4d to use StartElement1 again with non-Best direction (may be only one & may not point in right direction)
        {
            TotalSearchCount = 0; // added at v0.4f to give each exit direction a full chance to find required position
            if(SearchForPreferredRoute(9, StartElement1, StartElement1.XLinkPos, EndPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                       AutoSigsFlag, false))
            {
                SetRouteSearchVectorGraphics(8, AutoSigsFlag, true); // change graphic colour to the route colour
                if(PointsToBeChanged(12, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1492, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(111, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2502);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(1864);
                return(true);
            }
        }
    }
    if(!Track->SuppressRouteFailMessage)
    {
        TrainController->StopTTClockMessage(33, Track->RouteFailMessage);
    }
    Utilities->CallLogPop(235);
    return(false);
}

// ---------------------------------------------------------------------------

void TOneRoute::RouteImageMarker(int Caller, Graphics::TBitmap *Bitmap) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RouteImageMarker");
    if(PrefDirSize() == 0)
    {
        Utilities->CallLogPop(1704);
        return;
    }
    for(unsigned int x = 0; x < PrefDirSize(); x++)
    {
        TPrefDirElement TempPrefDirElement = PrefDirVector.at(x);
        if(TempPrefDirElement.EXGraphicPtr != 0) // Note:  will be 0 if first element or last as leading point
        {
            Bitmap->Canvas->Draw((TempPrefDirElement.HLoc - Track->GetHLocMin()) * 16, (TempPrefDirElement.VLoc - Track->GetVLocMin()) * 16,
                                 TempPrefDirElement.EXGraphicPtr);
            if((TempPrefDirElement.EntryDirectionGraphicPtr != 0) && PrefDirSize() > 1) // Route, no direction if a single element
            {
                if(x == 0)
                {
                    Bitmap->Canvas->Draw((TempPrefDirElement.HLoc - Track->GetHLocMin()) * 16, (TempPrefDirElement.VLoc - Track->GetVLocMin()) * 16,
                                         TempPrefDirElement.EntryDirectionGraphicPtr);
                }
                if(x == (PrefDirSize() - 1))
                {
                    Bitmap->Canvas->Draw((TempPrefDirElement.HLoc - Track->GetHLocMin()) * 16, (TempPrefDirElement.VLoc - Track->GetVLocMin()) * 16,
                                         TempPrefDirElement.EntryDirectionGraphicPtr);
                }
            }
        }
    }

    Utilities->CallLogPop(1705);
}

// ---------------------------------------------------------------------------

bool TOneRoute::SearchForPreferredRoute(int Caller, TPrefDirElement PrefDirElement, int XLinkPos, int RequiredPosition, IDInt ReqPosRouteID,
                                        TOnePrefDir *EveryPrefDir, bool ConsecSignals, int EndPosition, bool AutoSigsFlag, bool RecursiveCall)
/*
      Brief:  similar to SearchForPrefDir but with a PrefDirElement instead of a TrackElement & with additional parameters.
      PrefDirElement is the starting element from which to search, it is NOT stored in searchvector during this function.  If it's an
      element that's not already in a route it will have been stored in SearchVector during GetPreferredRouteStartElement.
      ReqPosRouteID is used when RequiredPosition is start of an existing route, else it's -1.
      Return false if any element (apart from RequiredPosition) is on an existing route.
      Return false if not on a PrefDir with same ELink (can't check XLink as may not be set - if it's a leading point in a recursive call - see later).

      Detail:  Function is a continuous loop as examine each element on a potential route, exiting only if find
      the required position (return true & leave Searchvector as set up) or if fail (erase all SearchVector entries
      added during the function so as to leave it exactly as it was on entering, then return false).
      It is a recursive function (similar to SearchForPrefDir) to enable all possible point branches to be searched.
      A VectorCount is maintained to count elements added to the SearchVector, so that this number can be erased on failure
      of any branch.  Enter with starting PrefDirElement & XLinkPos for that element, RequiredPosition - the
      TrackVectorPosition of the element to be searched for, ReqPosRouteID -
      the route number that the searched-for element is the start of if any, and set to -1 if no
      such route.  A pointer to EveryPrefDir is also passed in since this is not accessible directly from
      this unit, together with the ConsecSignals and AutoSigsFlag flags.
      Create 2 TPrefDirElements - PrefDirElement1 and 2, for use later - ELink has to match the preceding XLink, so the only
      2 possible PrefDirs are for a leading point & its two trailing PrefDirs.

      Enter loop - note that PrefDirElement changes each time round the loop - check if PrefDirElement XLinkPos faces buffers
      or a continuation, and fail if so. Check if reached a valid next signal in ConsecSignalsRoute on any but firstpass
      (nonrecursive firstpass starts at a valid signal, and recursive firstpass always starts at points so doesn't matter
      for recursive calls), and fail if so as user should always select the next signal in a route if ConsecSignals set.
      Create a new TPrefDirElement - SearchElement, from PrefDirElement.Conn[XLinkPos], & set all FixedTrackPiece &
      TrackElement values, ELink & ELinkPos, and also XLink & XLinkPos unless element is a leading point.
      Check if element is already in searchvector (OK if a bridge & earlier entry on different track, but not OK if
      any other type of element), already in an existing route (OK if bridge & diff tracks, or start of an expected route),
      or if train on element (unless a bridge & train on different track).
      Check & fail for a fouled diagonal (unless element is a leading point - these checked later).
      Check element in EveryPrefDir with same ELink value & set PrefDirElement1, & also 2 if element is
      a leading point where both trailing directions are in EveryPrefDir, if not fail.
      Check if found RequiredPosition & that it's a signal/buffer/continuation.  If OK save in SearchVector with
      AutoSignals member set if AutoSigsFlag set, then return true.
      Check & fail if a buffer or continuation (unless it is the RequiredPosition, in which case will have succeeded in the above check).

      Now check if a leading point and if so set XLinkPos to the 'set' exit & check if that XLink is in EveryPrefDir,
      by comparing with PrefDirElement1 or 2, fail if not.  If valid check for a fouled diagonal and fail if so.  If OK
      store element in SearchVector with AutoSignals member set if AutoSigsFlag set & do a recursive search using
      this element and XLinkPos, other parameters are passed in without change.  If succeed return true, else erase the last element in
      SearchVector (i.e the earlier stored leading point element prior to doing the recursive search) & set XLinkPos to the 'unset' exit to
      check the other trailing direction.  Then proceed in same way as above, i.e. fouled diagonal & recursive search etc.  If
      fail on this XLinkPos then have tried & failed on both ways out from the leading point so erase the searchvector & return false.

      If not a leading point store the element (can only be PrefDirElement1 as not a leading point), then set
      up the next loop values of PrefDirElement & XLinkPos from SearchElement & NextXLinkPos and repeat the while loop.
*/

{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SearchForPreferredRoute," + PrefDirElement.LogPrefDir() + "," +
                                 AnsiString(XLinkPos) + "," + AnsiString(RequiredPosition) + "," + AnsiString(ReqPosRouteID.GetInt()) + "," + AnsiString(EndPosition) + "," +
                                 AnsiString((short)AutoSigsFlag));
    int VectorCount = 0;
    TPrefDirElement PrefDirElement1, PrefDirElement2, BlankElement;

// check for a fouled diagonal for first element.  Added for v1.3.2
    if((PrefDirElement.XLink == 1) || (PrefDirElement.XLink == 3) || (PrefDirElement.XLink == 7) || (PrefDirElement.XLink == 9))
    {
        if(AllRoutes->DiagonalFouledByRouteOrTrain(0, PrefDirElement.HLoc, PrefDirElement.VLoc, PrefDirElement.XLink))
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(2043);
            return(false);
        }
    }
    bool FirstPass = true;

    while(true)
    {
        if(AutoSigsFlag && Track->IsLCAtHV(24, PrefDirElement.HLoc, PrefDirElement.VLoc))
        {
            Track->LCFoundInAutoSigsRoute = true;
        }
        if(Track->IsLCBarrierFlashingAtHV(1, PrefDirElement.HLoc, PrefDirElement.VLoc)) // can't set a route through a flashing barrier
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1926);
            return(false);
        }
        if(PrefDirElement.Config[PrefDirElement.XLinkPos] == End) // buffers or continuation
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(236);
            return(false);
        }
        if(!FirstPass && ConsecSignals && (PrefDirElement.Config[PrefDirElement.XLinkPos] == Signal))
        // reached a valid signal that isn't the required position, user should always select the next
        // signal in a route when ConsecSignals is true so have to fail
        // won't affect recurive searches as for them the first pass element is always a point
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(237);
            return(false);
        }
        FirstPass = false;
        int NextPosition = PrefDirElement.Conn[XLinkPos];
        TTrackElement NextTrackElement = Track->TrackElementAt(88, NextPosition);
        TPrefDirElement SearchElement(NextTrackElement);
        SearchElement.TrackVectorPosition = NextPosition;
        int NextELinkPos = PrefDirElement.ConnLinkPos[XLinkPos];
        SearchElement.ELinkPos = NextELinkPos;
        SearchElement.ELink = SearchElement.Link[NextELinkPos]; // Note ELink isn't necessarily 10 - last XLink, as last element may have
        // been a gap.  Now have all FixedTrackPiece & TrackElement values, + TrackVectorPosition, ELink & ELinkPos
        int NextXLinkPos;
        if(SearchElement.ELinkPos == 0)
        {
            NextXLinkPos = 1;
        }
        if(SearchElement.ELinkPos == 1)
        {
            NextXLinkPos = 0;
        }
        if(SearchElement.ELinkPos == 2)
        {
            NextXLinkPos = 3;
        }
        if(SearchElement.ELinkPos == 3)
        {
            NextXLinkPos = 2;
        }
        if((SearchElement.TrackType != Points) || (SearchElement.Config[SearchElement.ELinkPos] != Lead))
        {
            SearchElement.XLink = SearchElement.Link[NextXLinkPos];
// note that may be buffers, continuation or gap
            SearchElement.XLinkPos = NextXLinkPos;
        }

// can't set XLink or XLinkPos yet if the element is a non-failed leading point.

// check if reached an earlier position on search PrefDir (was OK in SearchForPrefDir if entry values different, but not OK for a route)
        for(unsigned int x = 0; x < SearchVector.size(); x++)
        {
            if(SearchElement.TrackVectorPosition == SearchVector.at(x).TrackVectorPosition)
            {
                if((SearchElement.TrackType != Bridge) || ((SearchElement.TrackType == Bridge) && (SearchElement.ELink == SearchVector.at(x).ELink)))
                // OK if a bridge & routes on different tracks
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(238);
                    return(false);
                }
            }
        }

// check if element in an existing route (OK if bridge & diff tracks, or start of an expected route)
        TAllRoutes::TRouteElementPair SecondPair;
        TAllRoutes::TRouteElementPair RoutePair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(16,
                                                                                                   Track->TrackElementAt(89, SearchElement.TrackVectorPosition).HLoc, Track->TrackElementAt(90, SearchElement.TrackVectorPosition).VLoc, SecondPair);
        if(RoutePair.first > -1)
        {
            // OK if it's a bridge & routes on different tracks (hard to see how can reach a bridge before another element in route as can't start on a bridge, but leave check in anyway)
            if(!((SearchElement.TrackType == Bridge) && (SearchElement.ELinkPos != AllRoutes->GetFixedRouteAt(12, RoutePair.first).GetFixedPrefDirElementAt(38,
                                                                                                                                                            RoutePair.second).ELinkPos)))
            {
                // still OK if start of an expected route
                if((ReqPosRouteID == IDInt(-1)) || ((int)RoutePair.first != AllRoutes->GetRouteVectorNumber(2, ReqPosRouteID)) || (RoutePair.second != 0))
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(239);
                    return(false); // only allow for start of an expected route
                }
            }
        }
        if(SecondPair.first > -1) // if reach here & secondpair present then must fail as can't escape both existing routes, but leave check as before anyway
        {
            // OK if it's a bridge & routes on different tracks (hard to see how can reach a bridge before another element in route as can't start on a bridge, but leave check in anyway)
            if(!((SearchElement.TrackType == Bridge) && (SearchElement.ELinkPos != AllRoutes->GetFixedRouteAt(13, SecondPair.first).GetFixedPrefDirElementAt(39,
                                                                                                                                                             SecondPair.second).ELinkPos)))
            {
                // still OK if start of an expected route
                if((ReqPosRouteID == IDInt(-1)) || ((int)SecondPair.first != AllRoutes->GetRouteVectorNumber(3, ReqPosRouteID)) || (SecondPair.second != 0))
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(240);
                    return(false); // only allow for start of an expected route
                }
            }
        }
// check if a train on element, unless a bridge & train on different track
// OK of same train as start element - no drop this
// if(SearchElement.TrainIDOnElement != StartSelectionTrainID)
        if((SearchElement.TrainIDOnElement > -1) && (SearchElement.TrackType != Bridge))
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(241);
            return(false);
        }
        if((SearchElement.TrainIDOnElement > -1) && (SearchElement.TrackType == Bridge))
        {
            if((SearchElement.ELinkPos < 2) && (SearchElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 > -1))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(242);
                return(false);
            }
            else if((SearchElement.ELinkPos > 1) && (SearchElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 > -1))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(243);
                return(false);
            }
        }
// check for a fouled diagonal (if not leading point - these checked later - leading point XLink == -1)
        if((SearchElement.XLink == 1) || (SearchElement.XLink == 3) || (SearchElement.XLink == 7) || (SearchElement.XLink == 9))
        {
            if(AllRoutes->DiagonalFouledByRouteOrTrain(7, SearchElement.HLoc, SearchElement.VLoc, SearchElement.XLink))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(244);
                return(false);
            }
        }
// check element in EveryPrefDir with same ELink (XLink may not be set) & save up to 2 elements (for leading point & 2 trailing PrefDirs)
// note that point XLinks checked later, otherwise XLink fully defined by ELink so only need to check ELink
        bool InPrefDirFlag = false;
        PrefDirElement1 = BlankElement;
        PrefDirElement2 = BlankElement;

        bool FoundFlag;
        int PrefDirPos0 = -1;
        int PrefDirPos1 = -1;
        int PrefDirPos2 = -1;
        int PrefDirPos3 = -1;
        EveryPrefDir->GetVectorPositionsFromPrefDir4MultiMap(5, Track->TrackElementAt(91, SearchElement.TrackVectorPosition).HLoc,
                                                             Track->TrackElementAt(92, SearchElement.TrackVectorPosition).VLoc, FoundFlag, PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3);
        int PrefDirVecPos[4] =
        {
            PrefDirPos0, PrefDirPos1, PrefDirPos2, PrefDirPos3
        };
        for(int x = 0; x < 4; x++)
        {
            int b = PrefDirVecPos[x];
            if((b > -1) && (EveryPrefDir->GetFixedPrefDirElementAt(40, b).ELink == SearchElement.ELink))
            {
                InPrefDirFlag = true;
                if(PrefDirElement1.TrackVectorPosition == -1)
                {
                    PrefDirElement1 = EveryPrefDir->GetFixedPrefDirElementAt(41, b);
                }
                else
                {
                    PrefDirElement2 = EveryPrefDir->GetFixedPrefDirElementAt(42, b);
                }
            }
        }
        if(!InPrefDirFlag)
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(245);
            return(false);
        }
// check if exceeds the search H & V limits - drop in favour of limiting TotalSearchCount
// if((SearchElement.HLoc > SearchLimitHighH) || (SearchElement.HLoc < SearchLimitLowH) ||
// (SearchElement.VLoc > SearchLimitHighV) ||(SearchElement.VLoc < SearchLimitLowV))
        if(TotalSearchCount > RouteSearchLimit)
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1690);
            return(false);
        }
// check if found it
        if(SearchElement.TrackVectorPosition == RequiredPosition)
        {
// need to ensure a signal/buffer/continuation
            if((SearchElement.Config[SearchElement.XLinkPos] != Signal) && (SearchElement.Config[SearchElement.XLinkPos] != End))
            {
                TrainController->StopTTClockMessage(94, "Must select a valid signal, buffers or continuation"); //added at v2.7.0
                Track->SuppressRouteFailMessage = true;
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(246);
                return(false);
            } // if((SearchElement.Config[SearchElement.XLinkPos] != Signal).......

            if(AutoSigsFlag)
            {
                PrefDirElement1.AutoSignals = true;
            }
            PrefDirElement1.PrefDirRoute = true;
            if(Track->LCFoundInAutoSigsRoute)
            {
                if(!Track->SuppressRouteFailMessage)
                {
                    TrainController->StopTTClockMessage(76, "Can't create an automatic signal route through a level crossing");
                    Track->SuppressRouteFailMessage = true;
                }
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(1928);
                return(false);
            }
            SearchVector.push_back(PrefDirElement1); // must be 1 as it's a simple element
            VectorCount++; // not really needed but include for tidyness
            TotalSearchCount++;
            if(!RecursiveCall && SignalHasFailed(0)) //added at v2.13.0
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(2522);
                return(false);
            }
            Utilities->CallLogPop(247);
            return(true);
        } // if(SearchElement.TrackVectorPosition == RequiredPosition)

// check if a buffer or continuation (end of search on this leg if not found by now)
        if((SearchElement.TrackType == Buffers) || (SearchElement.TrackType == Continuation))
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(248);
            return(false);
        }
// check if SearchVector exceeds a size of 150
        if(SearchVector.size() > 150)
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1420);
            return(false);
        }
//deal with failed points, added at v2.13.0
        if((SearchElement.TrackType == Points) && (SearchElement.Config[SearchElement.ELinkPos] == Lead) && Track->TrackElementAt(1518, SearchElement.TrackVectorPosition).Failed) //leading entry
        {
            if(Track->TrackElementAt(1519, SearchElement.TrackVectorPosition).Attribute == 0)
            {
                SearchElement.XLinkPos = 1;
                SearchElement.XLink = SearchElement.Link[SearchElement.XLinkPos];
                PrefDirElement1.XLinkPos = 1; //need PrefDirElement1 set too as SearchElement set to it ready for next loop
                PrefDirElement1.XLink = SearchElement.Link[SearchElement.XLinkPos];
                PrefDirElement1.EntryExitNumber(); //to set EXNumber correctly as XLink might have changed for original
            }
            else
            {
                SearchElement.XLinkPos = 3;
                SearchElement.XLink = SearchElement.Link[SearchElement.XLinkPos];
                PrefDirElement1.XLinkPos = 3; //need PrefDirElement1 set too as SearchElement set to it ready for next loop
                PrefDirElement1.XLink = SearchElement.Link[SearchElement.XLinkPos];
                PrefDirElement1.EntryExitNumber(); //to set EXNumber correctly as XLink might have changed for original
            }
        }
        else if((SearchElement.TrackType == Points) && (SearchElement.Config[SearchElement.ELinkPos] == Trail) && Track->TrackElementAt(1520, SearchElement.TrackVectorPosition).Failed) //trailing entry
        {
            if((Track->TrackElementAt(1521, SearchElement.TrackVectorPosition).Attribute == 0) && (SearchElement.ELinkPos == 3)) //can't go any further
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(2514);
                return(false);
            }
            if((Track->TrackElementAt(1522, SearchElement.TrackVectorPosition).Attribute == 1) && (SearchElement.ELinkPos == 1)) //can't go any further
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(2515);
                return(false);
            }
        }
// check if reached a non-failed leading point
        if((SearchElement.TrackType == Points) && (SearchElement.Config[SearchElement.ELinkPos] == Lead) && !Track->TrackElementAt(1523, SearchElement.TrackVectorPosition).Failed)
        {
// XLink set to points 'set' position - Attribute == 0, SearchPos1 = 1 & SearchPos2 = 3; Attribute == 1, SearchPos1 = 3 & SearchPos2 = 1;
            int SearchPos1 = SearchElement.Attribute + 1;
            int SearchPos2;
            if(SearchPos1 == 2)
            {
                SearchPos1++;
            }
            if(SearchPos1 == 1)
            {
                SearchPos2 = 3;
            }
            else
            {
                SearchPos2 = 1;
            }
            SearchElement.XLink = SearchElement.Link[SearchPos1];
            SearchElement.XLinkPos = SearchPos1;
            InPrefDirFlag = false;
            if(SearchElement.XLink == PrefDirElement1.XLink)
            {
                SearchElement = PrefDirElement1; // set to an existing PrefDir element so that exnumber & graphics set
                InPrefDirFlag = true;
            }
            else if(SearchElement.XLink == PrefDirElement2.XLink)
            {
                SearchElement = PrefDirElement2;
                InPrefDirFlag = true;
            }
// push element with XLink set to position [SearchPos1] if on a PrefDir
            if(InPrefDirFlag)
            {
// check for a fouled diagonal for leading point for XLinkPos == 1)
                if((SearchElement.XLink == 1) || (SearchElement.XLink == 3) || (SearchElement.XLink == 7) || (SearchElement.XLink == 9))
                {
                    if(AllRoutes->DiagonalFouledByRouteOrTrain(1, SearchElement.HLoc, SearchElement.VLoc, SearchElement.XLink))
                    {
                        for(int x = 0; x < VectorCount; x++)
                        {
                            SearchVector.erase(SearchVector.end() - 1);
                        }
                        Utilities->CallLogPop(249);
                        return(false);
                    }
                }
                if(AutoSigsFlag)
                {
                    SearchElement.AutoSignals = true;
                }
                SearchElement.PrefDirRoute = true;
                SearchVector.push_back(SearchElement);
                VectorCount++;
                TotalSearchCount++;
// recursive search at XLinkPos of SearchPos1 (i.e. 'set' trailing exit)
                if(SearchForPreferredRoute(6, SearchElement, SearchPos1, RequiredPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                           AutoSigsFlag, true))
                {
                    if(Track->LCFoundInAutoSigsRoute)
                    {
                        if(!Track->SuppressRouteFailMessage)
                        {
                            TrainController->StopTTClockMessage(77, "Can't create an automatic signal route through a level crossing");
                            Track->SuppressRouteFailMessage = true;
                        }
                        for(int x = 0; x < VectorCount; x++)
                        {
                            SearchVector.erase(SearchVector.end() - 1);
                        }
                        Utilities->CallLogPop(1929);
                        return(false);
                    }
                    if(!RecursiveCall && SignalHasFailed(1)) //added at v2.13.0
                    {
                        for(int x = 0; x < VectorCount; x++)
                        {
                            SearchVector.erase(SearchVector.end() - 1);
                        }
                        Utilities->CallLogPop(2523);
                        return(false);
                    }
                    Utilities->CallLogPop(250);
                    return(true);
                }
                else
                {
// remove leading point with XLinkPos [1]
                    SearchVector.erase(SearchVector.end() - 1);
                    VectorCount--;
                }
            }
// XLink set to position [SearchPos2]
            SearchElement.XLink = SearchElement.Link[SearchPos2];
            SearchElement.XLinkPos = SearchPos2;
            if(SearchElement.XLink == PrefDirElement1.XLink)
            {
                SearchElement = PrefDirElement1; // set to an existing PrefDir element so that exnumber & graphics set
            }
            else if(SearchElement.XLink == PrefDirElement2.XLink)
            {
                SearchElement = PrefDirElement2;
            }
            else // failed to find a valid exit from the point
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(251);
                return(false);
            }
// check for a fouled diagonal for leading point for XLinkPos == SearchPos2)
            if((SearchElement.XLink == 1) || (SearchElement.XLink == 3) || (SearchElement.XLink == 7) || (SearchElement.XLink == 9))
            {
                if(AllRoutes->DiagonalFouledByRouteOrTrain(2, SearchElement.HLoc, SearchElement.VLoc, SearchElement.XLink))
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(252);
                    return(false);
                }
            }
// push element with XLink set to position [SearchPos2]
            if(AutoSigsFlag)
            {
                SearchElement.AutoSignals = true;
            }
            SearchElement.PrefDirRoute = true;
            SearchVector.push_back(SearchElement);
            VectorCount++;
            TotalSearchCount++;
// recursive search at XLinkPos of SearchPos2 (i.e. 'unset' trailing exit)
            if(SearchForPreferredRoute(7, SearchElement, SearchPos2, RequiredPosition, ReqPosRouteID, EveryPrefDir, ConsecSignals, EndPosition,
                                       AutoSigsFlag, true))
            {
                if(Track->LCFoundInAutoSigsRoute)
                {
                    if(!Track->SuppressRouteFailMessage)
                    {
                        TrainController->StopTTClockMessage(78, "Can't create an automatic signal route through a level crossing");
                        Track->SuppressRouteFailMessage = true;
                    }
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(1930);
                    return(false);
                }
                if(!RecursiveCall && SignalHasFailed(2)) //added at v2.13.0
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(2524);
                    return(false);
                }
                Utilities->CallLogPop(1592);
                return(true);
            }
            else
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(253);
                return(false);
            }
        } // if leading point

// here if ordinary element or failed points, push it, inc vector & update PrefDirElement ready for next element on PrefDir
        SearchElement = PrefDirElement1;
        if(AutoSigsFlag)
        {
            SearchElement.AutoSignals = true;
        }
        SearchElement.PrefDirRoute = true;
        SearchVector.push_back(SearchElement);
        VectorCount++;
        TotalSearchCount++;
        XLinkPos = SearchElement.XLinkPos; // wasn't a leading point so XLinkPos defined
        PrefDirElement = SearchElement;
    } // while(true)
}

// ---------------------------------------------------------------------------

void TOneRoute::ConvertAndAddPreferredRouteSearchVector(int Caller, IDInt ReqPosRouteID, bool AutoSigsFlag)
{
/*
      For routes, as opposed to PrefDirs, the new route elements are first entered into SearchVector,
      and the new or extended route created from that.  Hence action varies depending on whether
      it is a completely new route, or an extension of an existing route at the beginning or the end.
      Note that a single route cannot contain both AutoSig & non-AutoSig elements, each route of AutoSig elements
      has its own identity.  A single route can however have a mixture of Unrestricted and PreferredRoute elements

      Ignore if SearchVector empty, and check that all the new elements in SearchVector are valid.
      Check if route end selection is in an existing route (ReqPosRouteID > -1), and if so proceed as follows:-
      if both new and existing routes non-autosig, add the old route to the SearchVector then delete the old route;
      if both new and existing routes autosig, add the old route to the SearchVector then delete the old route;
      in both the above cases if RequPosRouteNumber is less than StartSelectionRouteNumber then StartSelectionRouteNumber
       is decremented;
      if new route autosig and existing route non-autosig, keep the final search element in the new route & remove it (i.e first element)
       from the existing route, then enter the new route into the AllRoutesVector;
      if new route non-autosig and existing route autosig, drop the final search element in the new route, leave the existing route as it is,
       then enter the new route into the AllRoutesVector.

      Check if StartSelectionRouteID set (extending an existing route) and if so proceed as follows:-
      if both new and existing routes non-autosig, then add the new route to the existing route (start element not stored in searchvector);
      if both new and existing routes autosig, then add the new route to the existing route (start element not stored in searchvector);
      in both the above cases validate the extended route, then call SetRoutePoints & SetRouteSignals for the extended route and return.
      if new route autosig and existing route non-autosig, remove the last route element from the existing route, make it an AutoSig element,
       then add it to the start of the new route, then check its validity, enter it into the AllRoutesVector, call SetRoutePoints & SetRouteSignals
       for the new route and return;
      if new route non-autosig and existing route autosig, leave the existing route as it is, check its validity, then just enter the new
      route into the AllRoutesVector, finally call SetRoutePoints & SetRouteSignals for the new route and return.

      If not returned by now the route in SearchVector is to be added as a new route, so check its validity, create a new route using
      StoreOneRoute, call SetRoutePoints & SetRouteSignals and return.  In practice the validity check, storage into AllRoutesVector and
      SetRoutePoints & SetRouteSignals call are combined for the above three cases.

*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ConvertAndAddPreferredRouteSearchVector," +
                                 AnsiString(ReqPosRouteID.GetInt()) + "," + AnsiString((short)AutoSigsFlag));
    if(SearchVector.size() < 1)
    {
        Utilities->CallLogPop(254);
        return;
    }
    PrefDirVector = SearchVector;
    if(!ValidatePrefDir(3)) // check the new route elements in SearchVector
    {
        Utilities->CallLogPop(255);
        return;
    }
    TAllRoutes::TLockedRouteClass LockedRouteObject;

    AllRoutes->LockedRouteFoundDuringRouteBuilding = false;
    unsigned int TruncatePrefDirPosition = 0;

    if(ReqPosRouteID > -1) // Note that ReqPosRouteID != StartRouteNumber as would have failed in GetNextRouteElement
/* if have ReqPosRouteID:
      if both new and existing routes non-autosig, then add the old route to the SearchVector then delete the old route
      if both new and existing routes autosig, then add the old route to the SearchVector then delete the old route
      if new route autosig and existing route non-autosig, keep the final search element in the new route & remove it (i.e first element) from
       the existing route, then enter the new route into the AllRoutesVector
      if new route non-autosig and existing route autosig, drop the final search element in the new route, leave the existing route as it is,
       then enter the new route into the AllRoutesVector
*/
    {
        if((AutoSigsFlag && (AllRoutes->GetFixedRouteAtIDNumber(19, ReqPosRouteID).GetFixedPrefDirElementAt(46, 0).AutoSignals)) ||
           (!AutoSigsFlag && !(AllRoutes->GetFixedRouteAtIDNumber(20, ReqPosRouteID).GetFixedPrefDirElementAt(47, 0).AutoSignals)))
        {
            for(unsigned int x = 1; x < AllRoutes->GetFixedRouteAtIDNumber(21, ReqPosRouteID).PrefDirSize();
                x++) // start at 1 as first element already in SearchVector
            {
                SearchVector.push_back(AllRoutes->GetFixedRouteAtIDNumber(22, ReqPosRouteID).GetFixedPrefDirElementAt(48, x));
            }
            // note that route numbers in map adjusted when ReqPos route cleared
            AllRoutes->ClearRouteDuringRouteBuildingAt(1, AllRoutes->GetRouteVectorNumber(7, ReqPosRouteID));
            // create a new locked route object (apart from RouteNumber) if required, for use later (LockedRouteFoundDuringRouteBuilding
            // set during ClearRouteDuringRouteBuildingAt
            if(AllRoutes->LockedRouteFoundDuringRouteBuilding)
            {
                LockedRouteObject.RearTrackVectorPosition = AllRoutes->LockedRouteRearTrackVectorPosition;
                LockedRouteObject.LastTrackVectorPosition = AllRoutes->LockedRouteLastTrackVectorPosition;
                LockedRouteObject.LastXLinkPos = AllRoutes->LockedRouteLastXLinkPos;
                LockedRouteObject.LockStartTime = AllRoutes->LockedRouteLockStartTime;
            }
        }
        else if(AutoSigsFlag && !(AllRoutes->GetFixedRouteAtIDNumber(23, ReqPosRouteID).GetFixedPrefDirElementAt(49, 0).AutoSignals))
        {
            TPrefDirElement RouteElement = AllRoutes->GetFixedRouteAtIDNumber(24, ReqPosRouteID).GetFixedPrefDirElementAt(50, 0);
            AllRoutes->RemoveRouteElement(3, RouteElement.HLoc, RouteElement.VLoc, RouteElement.ELink);
        }
        else if(!AutoSigsFlag && (AllRoutes->GetFixedRouteAtIDNumber(25, ReqPosRouteID).GetFixedPrefDirElementAt(51, 0).AutoSignals))
        {
            SearchVector.pop_back();
        }
    }
    if(StartSelectionRouteID > -1)
/* if have StartSelectionRouteID:
      if both new and existing routes non-autosig, then add the new route to the existing route (start element not stored in searchvector)
      if both new and existing routes autosig, then add the new route to the existing route (start element not stored in searchvector)
      if new route autosig and existing route non-autosig, remove the last route element from the existing route, make it an AutoSig element,
       then add it to the start of the new route, then enter the new route into the AllRoutesVector
      if new route non-autosig and existing route autosig, leave the existing route as it is, and just enter the new route into the AllRoutesVector
*/
    {
        if(AllRoutes->IsThereARouteAtIDNumber(0, StartSelectionRouteID))
        // need to test because may have been removed by a train moving in the wrong direction between first and last route selections - added at v1.3.1
        {
            if((AutoSigsFlag && (AllRoutes->GetFixedRouteAtIDNumber(26, StartSelectionRouteID).GetFixedPrefDirElementAt(52, 0).AutoSignals)) ||
               (!AutoSigsFlag && !(AllRoutes->GetFixedRouteAtIDNumber(27, StartSelectionRouteID).GetFixedPrefDirElementAt(53, 0).AutoSignals)))
            {
                int RouteNumber = AllRoutes->GetRouteVectorNumber(0, StartSelectionRouteID);
                for(unsigned int x = 0; x < SearchVector.size(); x++)
                {
                    AllRoutes->AddRouteElement(1, GetFixedSearchElementAt(0, x).HLoc, GetFixedSearchElementAt(1, x).VLoc, GetFixedSearchElementAt(2, x).ELink,
                                               RouteNumber, GetFixedSearchElementAt(3, x));
                    // find & store locked route truncate position in PrefDirVector for later use
                    if(AllRoutes->LockedRouteFoundDuringRouteBuilding)
                    {
                        if(GetFixedSearchElementAt(15, x).TrackVectorPosition == int(AllRoutes->LockedRouteRearTrackVectorPosition))
                        {
                            TruncatePrefDirPosition = AllRoutes->GetFixedRouteAt(172, RouteNumber).PrefDirSize() - 1;
                        }
                    }
                }
                if(!(AllRoutes->GetModifiableRouteAtIDNumber(1, StartSelectionRouteID).ValidatePrefDir(4)))
                {
                    throw Exception("Error - failed to validate extended route for preferred route");
                }
                AllRoutes->GetModifiableRouteAtIDNumber(2, StartSelectionRouteID).SetRoutePoints(0);
                AllRoutes->GetModifiableRouteAtIDNumber(3, StartSelectionRouteID).SetRouteSignals(4);
                if(!AutoSigsFlag)
                {
                    AllRoutes->GetModifiableRouteAtIDNumber(7, StartSelectionRouteID).SetLCChangeValues(0, true); // ConsecSignalsRoute is true
                }
                // now add the reinstated locked route if required and set signals accordingly
                if(AllRoutes->LockedRouteFoundDuringRouteBuilding)
                {
                    LockedRouteObject.RouteNumber = RouteNumber;
                    AllRoutes->LockedRouteVector.push_back(LockedRouteObject);
                    // now reset the signals for the locked route
                    AllRoutes->SetAllRearwardsSignals(9, 0, RouteNumber, TruncatePrefDirPosition);
                    for(int c = AllRoutes->GetFixedRouteAt(173, RouteNumber).PrefDirSize() - 1; c >= (int)TruncatePrefDirPosition;
                        c--) // must use int for >= test to succeed when TruncatePrefDirPosition == 0
                    {
                        // return all signals to red in route section to be truncated
                        TPrefDirElement PrefDirElement = AllRoutes->GetFixedRouteAt(174, RouteNumber).PrefDirVector.at(c);
                        TTrackElement & TrackElement = Track->TrackElementAt(812, PrefDirElement.TrackVectorPosition);
                        if(PrefDirElement.Config[PrefDirElement.XLinkPos] == Signal)
                        {
                            TrackElement.Attribute = 0;
                            Track->PlotSignal(10, TrackElement, Display);
                            Display->PlotOutput(113, PrefDirElement.HLoc * 16, PrefDirElement.VLoc * 16, PrefDirElement.EXGraphicPtr);
                            Display->PlotOutput(114, PrefDirElement.HLoc * 16, PrefDirElement.VLoc * 16, PrefDirElement.EntryDirectionGraphicPtr);
                        }
                    }
                }
                AllRoutes->CheckMapAndRoutes(1); // test
                Utilities->CallLogPop(256);
                return;
            }
            else if(AutoSigsFlag && !(AllRoutes->GetFixedRouteAtIDNumber(28, StartSelectionRouteID).GetFixedPrefDirElementAt(54, 0).AutoSignals))
            {
                TPrefDirElement RouteElement = AllRoutes->GetFixedRouteAtIDNumber(29, StartSelectionRouteID).GetFixedPrefDirElementAt(55,
                                                                                                                                      AllRoutes->GetFixedRouteAtIDNumber(30, StartSelectionRouteID).PrefDirSize() - 1);
                RouteElement.AutoSignals = true;
                RouteElement.EXGraphicPtr = RouteElement.GetRouteGraphicPtr(AutoSigsFlag, true);
                RouteElement.EntryDirectionGraphicPtr = RouteElement.GetDirectionRouteGraphicPtr(AutoSigsFlag, true); // as above
                AllRoutes->RemoveRouteElement(4, RouteElement.HLoc, RouteElement.VLoc, RouteElement.ELink);
                SearchVector.insert(SearchVector.begin(), 1, RouteElement);
            }
        }
        else
        {
            StartSelectionRouteID = IDInt(-1);
        }
// if new route non-autosig and existing route autosig, leave the existing route as it is, and just enter the new route into the
// AllRoutesVector hence nothing to do here
    }
    PrefDirVector = SearchVector; // need to copy again since SearchVector may have been extended
    if(!ValidatePrefDir(5)) // validate PrefDir for all new route elements
    {
        throw Exception("Error - failed to validate single route for preferred route");
    }
    AllRoutes->StoreOneRoute(1, this);
    AllRoutes->GetModifiableRouteAt(3, AllRoutes->AllRoutesSize() - 1).SetRoutePoints(1); // new addition
    AllRoutes->GetModifiableRouteAt(16, AllRoutes->AllRoutesSize() - 1).SetRouteSignals(5); // new addition
    if(!AutoSigsFlag)
    {
        AllRoutes->GetModifiableRouteAt(18, AllRoutes->AllRoutesSize() - 1).SetLCChangeValues(1, true); // ConsecSignalsRoute is true
    }
    AllRoutes->CheckMapAndRoutes(2); // test
    Utilities->CallLogPop(257);
}

// ---------------------------------------------------------------------------

bool TOneRoute::GetNonPreferredRouteStartElement(int Caller, int HLoc, int VLoc, bool Callon) // Return true if OK.
{
/*
      If Callon true then this routine is called from MainScreenMouseDown2 in InterfaceUnit.cpp to set an unrestricted call-on route - messages are suppressed
      Clear the PrefDir and search vectors using ClearRoute().  Check selection matches a TrackElement
      & ensure signal/buffers/continuation.
      Note that can't select ConsecSignalsRoute for non-preferred routes.
      Check if train on element & disallow.
      Set default values for retained parameters:-
       StartRoutePosition = TrackVectorPosition of the element to be used as the start of the route;
       StartSelectionRouteID = route that selection starts in if there is one;

      Create 2 PrefDirElements from the TrackElement, setting all values corresponding to the 2 possible PrefDirs
      through the element (can only be 2 as 3 & 4 ended elements aren't allowed) & make an EXNumber check for
      validity.  This is just for safety reasons, the PrefDir values aren't used.
      StartElement1 & 2 are set to these PrefDirelements.

      There is no need to check that the element lies in a PrefDir for nonpreferred selections.

      Check if in an existing route & if so only allow last element to be selected - ensure it has somewhere to go!
      Set StartElement1, StartSelectionRouteID and StartRoutePosition to correspond to the route end element and
      blank StartElement2 (only want to use the route element), then return true.
      Check if adjacent to start or end of an existing route & disallow if so.
      If not in a route and not failed so far then reset all Link, all LinkPos, & EXNumber values to -1, and CheckCount
      to 4 for StartElement1, & blank StartElement2.  The remaining data members will be set later in
      SetRemainingSearchVectorValues().
      Finally add the required element to the SearchVector & return true.

*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetNonPreferredRouteStartElement," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString((short)Callon));
    ClearRoute();
    int TrackVectorPosition;
    TTrackElement TrackElement;
    TPrefDirElement FirstElement, LastElement;

    if(!(Track->FindNonPlatformMatch(9, HLoc, VLoc, TrackVectorPosition, TrackElement)))
    {
        Utilities->CallLogPop(258);
        return(false);
    }
    if((TrackElement.TrackType == Points) || (TrackElement.TrackType == Bridge) || (TrackElement.TrackType == Crossover))
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(34, "Can't select points, bridge or crossover when route building");
        }
// makes later adjacent route checks too complicated
        Utilities->CallLogPop(259);
        return(false);
    }
    if(Track->IsLCAtHV(21, HLoc, VLoc))
    {
        TrainController->StopTTClockMessage(74, "Can't start a route on a level crossing");
        Utilities->CallLogPop(1910);
        return(false);
    }
// check if selected a train & disallow if so
    if(TrackElement.TrainIDOnElement > -1)
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(35, "Can't start a route on a train");
        }
        Utilities->CallLogPop(260);
        return(false);
    }
// check if selected a locked route element & disallow (can only be a 2-track element so only need check XLinkPos values of 0 & 1
    TPrefDirElement PrefDirElement;
    int LockedVectorNumber;

    if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(3, TrackVectorPosition, 0, PrefDirElement, LockedVectorNumber))
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(36, "Can't start a route on a locked route");
        }
        Utilities->CallLogPop(261);
        return(false);
    }
    if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(4, TrackVectorPosition, 1, PrefDirElement, LockedVectorNumber))
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(37, "Can't start a route on a locked route");
        }
        Utilities->CallLogPop(262);
        return(false);
    }
    StartSelectionRouteID = IDInt(-1);
// AdjacentStartRouteNumber = -1;
    StartRoutePosition = TrackVectorPosition;
// StartRouteSelectPosition = TrackVectorPosition;

    TPrefDirElement PrefDirElement1(TrackElement); // 1 & 2 for the 2 possible PrefDirs at this location
    TPrefDirElement PrefDirElement2(TrackElement);

    PrefDirElement1.TrackVectorPosition = TrackVectorPosition;
    PrefDirElement2.TrackVectorPosition = TrackVectorPosition;
    TPrefDirElement BlankElement;

    PrefDirElement1.ELinkPos = 0;
    PrefDirElement1.XLinkPos = 1;
    PrefDirElement1.ELink = PrefDirElement1.Link[0];
    PrefDirElement1.XLink = PrefDirElement1.Link[1];
    if(!(PrefDirElement1.EntryExitNumber()))
    {
        throw Exception("Error, No EXNumber for PrefDirElement1 in GetNonPreferredRouteStartElement");
        // no need for bridge check as bridge selections not allowed
    }
    PrefDirElement1.CheckCount = 9;
    PrefDirElement2.ELinkPos = 1;
    PrefDirElement2.XLinkPos = 0;
    PrefDirElement2.ELink = PrefDirElement2.Link[1];
    PrefDirElement2.XLink = PrefDirElement2.Link[0];
    if(!(PrefDirElement2.EntryExitNumber()))
    {
        throw Exception("Error, No EXNumber for PrefDirElement2 in GetNonPreferredRouteStartElement");
    }
    PrefDirElement2.CheckCount = 9; // both now set

// set StartElements to the above PrefDirElements
    StartElement1 = PrefDirElement1;
    StartElement2 = PrefDirElement2;

// no PrefDir check needed as doesn't need to be in a PrefDir

// look for exact match in a route first - can't be a 3 or 4 track element so only need to look for one TRouteElementPair
    TAllRoutes::TRouteElementPair DummyPair;
    TAllRoutes::TRouteElementPair RoutePair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(1, TrackElement.HLoc, TrackElement.VLoc, DummyPair);

    if(RoutePair.first > -1)
    {
        if(RoutePair.second != AllRoutes->GetFixedRouteAt(31, RoutePair.first).PrefDirSize() - 1) // not last element in existing route so no good
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(38, "Can't start a route within or at the start of an existing route");
            }
            Utilities->CallLogPop(263);
            return(false);
        }
        TPrefDirElement RouteElement = AllRoutes->GetFixedRouteAt(32, RoutePair.first).GetFixedPrefDirElementAt(56, RoutePair.second);
        if(RouteElement.Conn[RouteElement.XLinkPos] < 0) // last element in existing route but nowhere to go!
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(39, "No forward connection from this position");
            }
            Utilities->CallLogPop(264);
            return(false);
        }
        if((RouteElement.Config[RouteElement.XLinkPos] != End) && (AllRoutes->TrackIsInARoute(11, RouteElement.Conn[RouteElement.XLinkPos],
                                                                                              RouteElement.ConnLinkPos[RouteElement.XLinkPos])))
        // last element in existing route but that route linked to another route (or a non-bridge 2-track element containing a route) so no good
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(40, "Can't start a route at an element that links forward into an existing route");
            }
            Utilities->CallLogPop(265);
            return(false);
        }
        StartSelectionRouteID = IDInt(AllRoutes->GetFixedRouteAt(162, RoutePair.first).RouteID);
        StartElement1 = AllRoutes->GetFixedRouteAt(33, RoutePair.first).GetFixedPrefDirElementAt(57,
                                                                                                 AllRoutes->GetFixedRouteAt(34, RoutePair.first).PrefDirSize() - 1); // last element
        StartElement2 = BlankElement; // only use the route element
        StartRoutePosition = StartElement1.TrackVectorPosition;
        Utilities->CallLogPop(266);
        return(true); // all retained values set
    }

    else // selection not in an existing route
    {
// check if it's adjacent to start of an an existing route,
        for(unsigned int a = 0; a < AllRoutes->AllRoutesSize(); a++)
        {
            FirstElement = AllRoutes->GetFixedRouteAt(35, a).GetFixedPrefDirElementAt(58, 0);
            if((StartElement1.Conn[0] > -1) && (StartElement1.Conn[0] == FirstElement.TrackVectorPosition))
            {
                if(!Callon)
                {
                    TrainController->StopTTClockMessage(41, "Can't make selection adjacent to start of another route");
                }
                Utilities->CallLogPop(267);
                return(false);
            }
            if((StartElement1.Conn[1] > -1) && (StartElement1.Conn[1] == FirstElement.TrackVectorPosition))
            {
                if(!Callon)
                {
                    TrainController->StopTTClockMessage(42, "Can't make selection adjacent to start of another route");
                }
                Utilities->CallLogPop(268);
                return(false);
            }
        }
// check if it's adjacent to end of an an existing route,
        for(unsigned int a = 0; a < AllRoutes->AllRoutesSize(); a++)
        {
            LastElement = AllRoutes->GetFixedRouteAt(36, a).GetFixedPrefDirElementAt(59, AllRoutes->GetFixedRouteAt(37, a).PrefDirSize() - 1);
            if(LastElement.Conn[LastElement.XLinkPos] == StartRoutePosition)
            {
                if(!Callon)
                {
                    TrainController->StopTTClockMessage(43, "Can't start a route adjacent to the end of an existing route");
                }
                Utilities->CallLogPop(269);
                return(false);
            }
        }
        // not in a route or adjacent to start or end of a route
        // in this case reset all variable values to -1 & CheckCount to 4
        StartElement1.ELink = -1;
        StartElement1.ELinkPos = -1;
        StartElement1.XLink = -1;
        StartElement1.XLinkPos = -1;
        StartElement1.EXNumber = -1;
        StartElement1.CheckCount = 4; //Only covers the fixed values HLoc, VLoc, SpeedTag & TrackVectorPosition
        StartElement2 = BlankElement;
        SearchVector.push_back(StartElement1);
        Utilities->CallLogPop(270);
        return(true);
    }
}

// ---------------------------------------------------------------------------
bool TOneRoute::GetNextNonPreferredRouteElement(int Caller, int HLoc, int VLoc, bool Callon, IDInt &ReqPosRouteID, bool &PointsChanged)

/*
      If Callon true then this routine is called from MainScreenMouseDown2 in InterfaceUnit.cpp to set an unrestricted call-on route - messages are suppressed

      Declare the following integers:-
      EndPosition - TrackVectorPosition for the selection;
      ReqPosRouteID - for the existing route selected if there is one, set to -1 if not;
      Check if selection is a valid track element and set EndPosition.
      Cancel if select original start element, then check that not points, bridge or crossover.
      Check & fail if a train is present at the selection.
      Create & set 2 PrefDirElements EndElement1 & 2 corresponding to the 2 possible PrefDir elements (similar to StartElement1 & 2
      in GetNonPreferredRouteStartElement) & make an EXNumber validity check just for safety reasons - the PrefDir values are not used.
      No check needed for selection in EveryPrefDir.
      Check if selection in an existing route & if so ensure it's the start element and that it doesn't have an 'End' facing the start.
      If it is the start of a route set ReqPosRouteID, EndPosition & EndElement1 to the start of route values and blank EndElement2
      as don't need it if in a route.
      Check if selection adj to start or end of a route and disallow.
      Fail if select same route as starting route, though should already have failed earlier if this is so.

      If there's a StartSelectionRouteID then StartElement1 will be set to
      the last entry in the selected route so use SearchForNonPreferredRoute to search for the selected end element from this
      start element.  If succeed then complete the search vector values (since not on a PrefDir) & return true, for Interface
      to handle the flashing & time delay.  After the delay completes the Interface flasher calls ConvertAndAddNonPreferredRouteSearchVector
      to add the new route to the AllRoutesVectorPtr.
      If no starting route then StartElement1 only has basic values set & is in the SearchVector, and StartElement2 is blank.
      Check if the selected element is adjacent to the starting position and if so set the route to go directly to it (as opposed to
      going round a long loop to get to it just because that XLinkPos happens to be chosen first).
      If not adjacent then search on the two possible ways out of StartElement1 providing it isn't facing an 'End'.  If succeed complete
      the search vector values and return.
      If not returned yet then have failed to find the required element so return false with no message.

*/

{
// get EndPosition
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetNextNonPreferredRouteElement," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    int EndPosition;
    int NewFailedPointsTVPos = -1; //added at v2.13.0 for point failures

    TotalSearchCount = 0;
    ReqPosRouteID = IDInt(-1); // for not used
    TTrackElement TrackElement;
    TPrefDirElement BlankElement;
    Track->SuppressRouteFailMessage = false;

    if(!(Track->FindNonPlatformMatch(10, HLoc, VLoc, EndPosition, TrackElement))) // return if can't find one
    {
        Utilities->CallLogPop(271);
        return(false);
    }
// EndPosition = EndSelectPosition;
// cancel selection if on original start element
    if(EndPosition == StartRoutePosition)
    {
        Utilities->CallLogPop(272);
        return(false);
    }
    if(Track->IsLCAtHV(22, HLoc, VLoc))
    {
        TrainController->StopTTClockMessage(75, "Can't end a route on a level crossing");
        Utilities->CallLogPop(1911);
        return(false);
    }
    if((TrackElement.TrackType == Points) && !Callon)
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(44, "Can't select points, bridge or crossover when route building");
        }
// makes later adjacent route checks too complicated
        Utilities->CallLogPop(273);
        return(false);
    }
    if((TrackElement.TrackType == Bridge) || (TrackElement.TrackType == Crossover))
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(71, "Can't select points, bridge or crossover when route building");
        }
// makes later adjacent route checks too complicated
        Utilities->CallLogPop(1861);
        return(false);
    }
// check if train on element
    if(TrackElement.TrainIDOnElement > -1)
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(45, "Can't end a route on a train");
        }
        Utilities->CallLogPop(274);
        return(false);
    }
// set the 2 EndElements corresponding to the 2 possible PrefDirs for the selected element (for safety reasons - to ensure EXNumber validity
// check passed)
    TPrefDirElement EndElement1(TrackElement); // 1 & 2 for the 2 possible PrefDirs at this location
    TPrefDirElement EndElement2(TrackElement);

    EndElement1.TrackVectorPosition = EndPosition;
    EndElement2.TrackVectorPosition = EndPosition;
    EndElement1.ELinkPos = 0;
    EndElement1.XLinkPos = 1;
    EndElement1.ELink = EndElement1.Link[0];
    EndElement1.XLink = EndElement1.Link[1];
    if(!(EndElement1.EntryExitNumber()))
    {
        throw Exception("Error, No EXNumber for EndElement1 in GetNonPreferredRouteStartElement");
    }
    EndElement1.CheckCount = 9;
    EndElement2.ELinkPos = 1;
    EndElement2.XLinkPos = 0;
    EndElement2.ELink = EndElement2.Link[1];
    EndElement2.XLink = EndElement2.Link[0];
    if(!(EndElement2.EntryExitNumber()))
    {
        throw Exception("Error, No EXNumber for EndElement2 in GetNonPreferredRouteStartElement");
    }
    EndElement2.CheckCount = 9; // both now set

// set the H&V limits for the search, all points on search path must lie within 15 elements greater than the box of which the line between
// start and finish is a diagonal line [dropped as not a good strategy because gaps interfered with direct line searches - instead
// introduced TotalSearchCount and now use that to limit searches. Leave in though in case rethink strategy later on]

    if(EndElement1.HLoc >= StartElement1.HLoc)
    {
        SearchLimitLowH = StartElement1.HLoc - 15;
        SearchLimitHighH = EndElement1.HLoc + 15;
    }
    else
    {
        SearchLimitLowH = EndElement1.HLoc - 15;
        SearchLimitHighH = StartElement1.HLoc + 15;
    }
    if(EndElement1.VLoc >= StartElement1.VLoc)
    {
        SearchLimitLowV = StartElement1.VLoc - 15;
        SearchLimitHighV = EndElement1.VLoc + 15;
    }
    else
    {
        SearchLimitLowV = EndElement1.VLoc - 15;
        SearchLimitHighV = StartElement1.VLoc + 15;
    }
/* dropped this for v0.4d - prevents ability to set routes for gaps that are widely separated, ok without it as search limited by SearchVector size
      check & TotalSearchCounts check
      if((abs(EndElement1.HLoc - StartElement1.HLoc) > 120) || (abs(EndElement1.VLoc - StartElement1.VLoc) > 120))
       {
       if(!Callon) TrainController->StopTTClockMessage(66, "Unable to reach the selected element - too far ahead");
       Utilities->CallLogPop(1694);
       return false;
       }
*/
// don't need EveryPrefDir check for NonPreferredRoute

// check if in an existing route - can't be a 3 or 4 track element so only one TRouteElementPair to be set
// bool InRoute = false;
    TAllRoutes::TRouteElementPair DummyPair;
    TAllRoutes::TRouteElementPair RoutePair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(2, TrackElement.HLoc, TrackElement.VLoc, DummyPair);

    if(RoutePair.first > -1)
    {
        if(RoutePair.second != 0) // not first element in existing route so no good
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(46, "Can't end a route within or at the end of an existing route");
            }
            Utilities->CallLogPop(275);
            return(false);
        }
        TPrefDirElement RouteElement = AllRoutes->GetFixedRouteAt(38, RoutePair.first).GetFixedPrefDirElementAt(60, RoutePair.second);
// if((RouteElement.Config[RouteElement.ELinkPos] != End) && (AllRoutes->TrackIsInARoute(, RouteElement.Conn[RouteElement.ELinkPos], RouteElement.ELinkPos)))
        if((RouteElement.Config[RouteElement.ELinkPos] != End) && (AllRoutes->TrackIsInARoute(12, RouteElement.Conn[RouteElement.ELinkPos],
                                                                                              RouteElement.ConnLinkPos[RouteElement.ELinkPos]))) // amended at v1.3.0 - had omitted ConnLinkPos - see above
        // first element in existing route but that route linked to another route (or a non-bridge 2-track element containing a route) so no good
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(47, "Can't start a route within or at the end of an existing route");
            }
            Utilities->CallLogPop(276);
            return(false);
        }
        EndElement1 = AllRoutes->GetFixedRouteAt(39, RoutePair.first).GetFixedPrefDirElementAt(61, 0);
        EndElement2 = BlankElement; // only need the route element
        EndPosition = EndElement1.TrackVectorPosition;
        ReqPosRouteID = IDInt(AllRoutes->GetFixedRouteAt(161, RoutePair.first).RouteID);
    }
// check if adjacent to start of an existing route and disallow (unless start of existing route is also the start of this route)
    for(unsigned int a = 0; a < AllRoutes->AllRoutesSize(); a++)
    {
        int AdjPosition = AllRoutes->GetFixedRouteAt(40, a).GetFixedPrefDirElementAt(62, 0).TrackVectorPosition;
        int AdjLinkPos = AllRoutes->GetFixedRouteAt(219, a).GetFixedPrefDirElementAt(245, 0).ELinkPos; // added at v1.3.1
// if((EndElement1.Config[EndElement1.XLinkPos] != End) && (EndElement1.Conn[EndElement1.XLinkPos] == AdjPosition)
// && (AdjPosition != StartRoutePosition))
        if((EndElement1.Config[EndElement1.XLinkPos] != End) && (EndElement1.Conn[EndElement1.XLinkPos] == AdjPosition)
           &&  // changed at v1.3.1 to allow a route end adjacent to an element with a route that doesn't link to the ending route
           (EndElement1.ConnLinkPos[EndElement1.XLinkPos] == AdjLinkPos) && (AdjPosition != StartRoutePosition))
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(48, "Can't end a route adjacent to the start of an existing route");
            }
            Utilities->CallLogPop(277);
            return(false);
        }
// else if((EndElement2.TrackVectorPosition > -1) && (EndElement2.Config[EndElement2.XLinkPos] != End) &&
// (EndElement2.Conn[EndElement2.XLinkPos] == AdjPosition) && (AdjPosition != StartRoutePosition))
        else if((EndElement2.TrackVectorPosition > -1) && (EndElement2.Config[EndElement2.XLinkPos] != End)
                && // changed at v1.3.1 to allow a route end adjacent to an element with a route that doesn't link to the ending route
                (EndElement2.Conn[EndElement2.XLinkPos] == AdjPosition) && (EndElement2.ConnLinkPos[EndElement2.XLinkPos] == AdjLinkPos) &&
                (AdjPosition != StartRoutePosition))
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(49, "Can't end a route adjacent to the start of an existing route");
            }
            Utilities->CallLogPop(278);
            return(false);
        }
// check if adjacent to end of a route & disallow (unless end of existing route is the start of this route - i.e. extending route by 1 element)
        TPrefDirElement EndOfRouteElement = AllRoutes->GetFixedRouteAt(41, a).GetFixedPrefDirElementAt(63, AllRoutes->GetFixedRouteAt(42, a).PrefDirSize() - 1);
        if((EndOfRouteElement.Config[EndOfRouteElement.XLinkPos] != End) && (EndOfRouteElement.Conn[EndOfRouteElement.XLinkPos] == EndPosition) &&
           (EndOfRouteElement.TrackVectorPosition != StartRoutePosition))
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(50, "Can't end a route adjacent to the end of an existing route");
            }
            Utilities->CallLogPop(279);
            return(false);
        }
    }

// check for same route as start element
    if((ReqPosRouteID > -1) && (ReqPosRouteID == StartSelectionRouteID))
    {
        if(!Callon)
        {
            TrainController->StopTTClockMessage(51, "Can't select same route as started in");
        }
        Utilities->CallLogPop(280);
        return(false);
    }
// check for a looping route
    if((ReqPosRouteID > -1) && (StartSelectionRouteID > -1))
    {
        if(AllRoutes->CheckForLoopingRoute(1, EndElement1.GetTrackVectorPosition(), EndElement1.GetXLinkPos(), StartElement1.GetTrackVectorPosition()))
        {
            if(!Callon)
            {
                TrainController->StopTTClockMessage(70, "Can't create a route that loops back on itself");
            }
            Utilities->CallLogPop(1845);
            return(false);
        }
    }
// if there's a StartSelectionRouteID StartElement1 will be set to the last entry in the selected route
// so search from this element.

    TTrackElement &TempElement1 = StartElement1; // this needed to avoid a TTrackElement construction ambiguity in later search function

    if(StartSelectionRouteID > -1)
    {
        if(SearchForNonPreferredRoute(0, TempElement1, StartElement1.XLinkPos, EndPosition, ReqPosRouteID, false))
        {
            SetRemainingSearchVectorValues(3);
            if(PointsToBeChanged(0, NewFailedPointsTVPos))
            {
                if(NewFailedPointsTVPos > -1)
                {
                    TTrackElement TE = Track->TrackElementAt(1494, NewFailedPointsTVPos);
                    TrainController->StopTTClockMessage(113, "Points at " + TE.ElementID +
                        " failed during route setting.");
                    Utilities->CallLogPop(2504);
                    return(false);
                }
                PointsChanged = true;
            }
            Utilities->CallLogPop(281);
            return(true);
        }
        else
        {
            if(!Callon && !Track->SuppressRouteFailMessage)
            {
                TrainController->StopTTClockMessage(52, Track->RouteFailMessage);
            }
            Utilities->CallLogPop(282);
            return(false);
        }
    }
    else // no starting route, so StartElement1 only has basic values set & is in SearchVector, StartElement2 is blank
    // search on the 2 ways out of the element, which has to be a 2-ended element
    {
// check if selection adjacent to start element and if so use that
        if(SearchVector.at(0).Conn[0] == EndPosition)
        {
            if(SearchForNonPreferredRoute(1, TempElement1, 0, EndPosition, ReqPosRouteID, false))
            {
                SetRemainingSearchVectorValues(4);
                if(PointsToBeChanged(1, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1496, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(115, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2506);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(283);
                return(true);
            }
            else
            {
                if(!Callon && !Track->SuppressRouteFailMessage)
                {
                    TrainController->StopTTClockMessage(53, Track->RouteFailMessage);
                }
                Utilities->CallLogPop(284);
                return(false);
            }
        }
        else if(SearchVector.at(0).Conn[1] == EndPosition)
        {
            if(SearchForNonPreferredRoute(2, TempElement1, 1, EndPosition, ReqPosRouteID, false))
            {
                SetRemainingSearchVectorValues(5);
                if(PointsToBeChanged(2, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1498, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(117, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2508);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(285);
                return(true);
            }
            else
            {
                if(!Callon && !Track->SuppressRouteFailMessage)
                {
                    TrainController->StopTTClockMessage(54, Track->RouteFailMessage);
                }
                Utilities->CallLogPop(286);
                return(false);
            }
        }
        // now start off in the best direction
        int BestPos = Track->FindClosestLinkPosition(1, StartRoutePosition, EndPosition); // can only be 0 or 1

        if(SearchVector.at(0).Config[BestPos] != End)
        {
            TotalSearchCount = 0; // added at v0.4f to give each exit direction a full chance to find required position
            if(SearchForNonPreferredRoute(3, TempElement1, BestPos, EndPosition, ReqPosRouteID, false))
            {
                SetRemainingSearchVectorValues(6);
                if(PointsToBeChanged(3, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1500, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(119, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2510);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(287);
                return(true);
            }
        }
        if(SearchVector.at(0).Config[1 - BestPos] != End)
        {
            TotalSearchCount = 0; // added at v0.4f to give each exit direction a full chance to find required position
            if(SearchForNonPreferredRoute(4, TempElement1, (1 - BestPos), EndPosition, ReqPosRouteID, false))
            {
                SetRemainingSearchVectorValues(7);
                if(PointsToBeChanged(4, NewFailedPointsTVPos))
                {
                    if(NewFailedPointsTVPos > -1)
                    {
                        TTrackElement TE = Track->TrackElementAt(1502, NewFailedPointsTVPos);
                        TrainController->StopTTClockMessage(121, "Points at " + TE.ElementID +
                            " failed during route setting.");
                        Utilities->CallLogPop(2512);
                        return(false);
                    }
                    PointsChanged = true;
                }
                Utilities->CallLogPop(288);
                return(true);
            }
        }
    }
    if(!Callon && !Track->SuppressRouteFailMessage)
    {
        TrainController->StopTTClockMessage(55, Track->RouteFailMessage);
    }
    Utilities->CallLogPop(289);
    return(false);
}

// ---------------------------------------------------------------------------

bool TOneRoute::SearchForNonPreferredRoute(int Caller, TTrackElement CurrentTrackElement, int XLinkPos, int RequiredPosition, IDInt ReqPosRouteID, bool RecursiveCall)
/*
      This is very similar to the preferred route search, but without the need to ensure all elements are in EveryPrefDir.
      Returns true for successful search with SearchVector containing the new route elements.  Enter with CurrentTrackElement
      stored in SearchVector unless it's in an existing route, & XLinkPos set to the link to search on.
      Keep a count of entries in SearchVector during the current function call, so that this number can be
      erased for an unproductive branch search.
      First check (within the loop) whether XLink leads to an End & return false if so.
      Create a NextTrackElement from Current & XLinkPos, and a PrefDirElement (SearchElement) from that, setting as many values as
      possible.  Check if element is already in searchvector (OK if a bridge & earlier entry on different track, but not OK if
      any other type of element), already in an existing route (OK if bridge & diff tracks, or start of an expected route), if
      train on element (unless a bridge & train on different track), or if element
      fouls an existing diagonal route (except if element is a leading point - these checked later).
      Then check if found required element.  If so save it & return true.
      If not the required element check if buffer or continuation, & if so erase all searchvector
      & return false.  If OK check if a leading point and if so do up to 2 recursive searches for the 2 exits (trying the 'set' exit first),
      checking in each case whether the element fouls an existing diagonal route.  If fail on both exits erase searchvector & return false.
      If not any of above, store element in SearchVector, increment VectorCount, set the new CurrentTrackElement value from the
      SearchElement & the new XLinkPos from SearchElement.XLinkPos, then go back to the while loop for the next step in the search.
      When return true have 8 items from CheckCount established, only waiting for EXNumber
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SearchForNonPreferredRoute," + CurrentTrackElement.LogTrack(14) + "," +
                                 AnsiString(XLinkPos) + "," + AnsiString(RequiredPosition) + "," + AnsiString() + "," + AnsiString(ReqPosRouteID.GetInt()));
    int VectorCount = 0;

// check for a fouled diagonal for first element.  Added for v1.3.2
    if((CurrentTrackElement.Link[XLinkPos] == 1) || (CurrentTrackElement.Link[XLinkPos] == 3) || (CurrentTrackElement.Link[XLinkPos] == 7) ||
       (CurrentTrackElement.Link[XLinkPos] == 9))
    {
        if(AllRoutes->DiagonalFouledByRouteOrTrain(8, CurrentTrackElement.HLoc, CurrentTrackElement.VLoc, CurrentTrackElement.Link[XLinkPos]))
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(2044);
            return(false);
        }
    }
    while(true)
    {
        if(Track->IsLCBarrierFlashingAtHV(2, CurrentTrackElement.HLoc, CurrentTrackElement.VLoc)) // can't set a route through a flashing barrier
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1927);
            return(false);
        }
        if(CurrentTrackElement.Config[XLinkPos] == End) // buffers or continuation
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(290);
            return(false);
        }
        int NextPosition = CurrentTrackElement.Conn[XLinkPos];
        TTrackElement NextTrackElement = Track->TrackElementAt(93, NextPosition);
        TPrefDirElement SearchElement(NextTrackElement);
        SearchElement.TrackVectorPosition = NextPosition;
        int NextELinkPos = CurrentTrackElement.ConnLinkPos[XLinkPos];
        SearchElement.ELinkPos = NextELinkPos;
        SearchElement.ELink = SearchElement.Link[SearchElement.ELinkPos];
        int NextXLinkPos;
        if(SearchElement.ELinkPos == 0)
        {
            NextXLinkPos = 1;
        }
        if(SearchElement.ELinkPos == 1)
        {
            NextXLinkPos = 0;
        }
        if(SearchElement.ELinkPos == 2)
        {
            NextXLinkPos = 3;
        }
        if(SearchElement.ELinkPos == 3)
        {
            NextXLinkPos = 2;
        }
        if((SearchElement.TrackType != Points) || (SearchElement.Config[SearchElement.ELinkPos] != Lead))
        {
            SearchElement.XLink = SearchElement.Link[NextXLinkPos];
            // but may be buffers, continuation or gap
            SearchElement.XLinkPos = NextXLinkPos;
        }
// Now have  SpeedTag, HLoc, VLoc, TrackVectorPosition, ELink, ELinkPos, and for non-points XLink & XLinkPos
// can't set XLink or XLinkPos yet if the element is a leading point.

// check if reached an earlier position on search PrefDir (was OK in SearchForPrefDir if entry values different, but not OK for a route)
        for(unsigned int x = 0; x < SearchVector.size(); x++)
        {
            if(SearchElement.TrackVectorPosition == SearchVector.at(x).TrackVectorPosition)
            {
                if((SearchElement.TrackType != Bridge) || ((SearchElement.TrackType == Bridge) && (SearchElement.ELink == SearchVector.at(x).ELink)))
                // OK if it's a bridge & routes on different tracks
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(291);
                    return(false);
                }
            }
        }

// check if element in an existing route (OK if bridge & diff tracks, or start of an expected route)
        TAllRoutes::TRouteElementPair SecondPair;
        TAllRoutes::TRouteElementPair RoutePair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(3,
                                                                                                   Track->TrackElementAt(94, SearchElement.TrackVectorPosition).HLoc, Track->TrackElementAt(95, SearchElement.TrackVectorPosition).VLoc, SecondPair);
        if(RoutePair.first > -1)
        {
            // OK if it's a bridge & routes on different tracks (hard to see how can reach a bridge before another element in route as can't start on a bridge, but leave check in anyway)
            if(!((SearchElement.TrackType == Bridge) && (SearchElement.ELinkPos != AllRoutes->GetFixedRouteAt(43, RoutePair.first).GetFixedPrefDirElementAt(64,
                                                                                                                                                            RoutePair.second).ELinkPos)))
            {
                // still OK if start of an expected route
                if((ReqPosRouteID == IDInt(-1)) || ((int)RoutePair.first != AllRoutes->GetRouteVectorNumber(4, ReqPosRouteID)) || (RoutePair.second != 0))
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(292);
                    return(false); // only allow for start of an expected route
                }
            }
        }
        if(SecondPair.first > -1) // if reach here & secondpair present then must fail as can't escape both existing routes, but leave check as before anyway
        {
            // OK if it's a bridge & routes on different tracks
            if(!((SearchElement.TrackType == Bridge) && (SearchElement.ELinkPos != AllRoutes->GetFixedRouteAt(44, SecondPair.first).GetFixedPrefDirElementAt(65,
                                                                                                                                                             SecondPair.second).ELinkPos)))
            {
                // still OK if start of an expected route
                if((ReqPosRouteID == IDInt(-1)) || ((int)SecondPair.first != AllRoutes->GetRouteVectorNumber(5, ReqPosRouteID)) || (SecondPair.second != 0))
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(293);
                    return(false); // only allow for start of an expected route
                }
            }
        }
// check if a train on element, unless a bridge & train on different track
// OK of same train as start element - no, drop this
// if(SearchElement.TrainIDOnElement != StartSelectionTrainID)
        if((SearchElement.TrainIDOnElement > -1) && (SearchElement.TrackType != Bridge))
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(294);
            return(false);
        }
        if((SearchElement.TrainIDOnElement > -1) && (SearchElement.TrackType == Bridge))
        {
            if((SearchElement.ELinkPos < 2) && (SearchElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 > -1))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(295);
                return(false);
            }
            else if((SearchElement.ELinkPos > 1) && (SearchElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 > -1))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(296);
                return(false);
            }
        }
// check for a fouled diagonal (if not leading point - leading point XLink == -1)
        if((SearchElement.XLink == 1) || (SearchElement.XLink == 3) || (SearchElement.XLink == 7) || (SearchElement.XLink == 9))
        {
            if(AllRoutes->DiagonalFouledByRouteOrTrain(3, SearchElement.HLoc, SearchElement.VLoc, SearchElement.XLink))
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(297);
                return(false);
            }
        }
// check if exceeds the search H & V limits - drop in favour of limiting TotalSearchCount
// if((SearchElement.HLoc > SearchLimitHighH) || (SearchElement.HLoc < SearchLimitLowH) ||
// (SearchElement.VLoc > SearchLimitHighV) ||(SearchElement.VLoc < SearchLimitLowV))
        if(TotalSearchCount > RouteSearchLimit)
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1689);
            return(false);
        }
// check if found it
        if(SearchElement.TrackVectorPosition == RequiredPosition)
        {
            if(SearchElement.TrackType == Points) // can only happen for platform element in CallingOnAllowed function
            {
                if((SearchElement.ELinkPos == 1) || (SearchElement.ELinkPos == 3))
                {
                    SearchElement.XLinkPos = 0; // select the straight track (for the platform)
                }
                else
                {
                    SearchElement.XLinkPos = 1;
                }
// SearchElement.XLink = SearchElement.Link[XLinkPos];  WRONG!! NajamUddin found this error 17/01/11, XLinkPos is the function input parameter, should be SearchElement.XLinkPos
                SearchElement.XLink = SearchElement.Link[SearchElement.XLinkPos]; // corrected for v0.6a
            }
            SearchVector.push_back(SearchElement);
            VectorCount++; // not really needed but include for tidyness
            TotalSearchCount++;
            if(!RecursiveCall && SignalHasFailed(3)) //added at v2.13.0
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(2525);
                return(false);
            }
            Utilities->CallLogPop(298);
            return(true);
        }
// Not the required element - check if a buffer or continuation
        if((SearchElement.TrackType == Buffers) || (SearchElement.TrackType == Continuation))
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(299);
            return(false);
        }
// check if SearchVector exceeds a size of 150
        if(SearchVector.size() > 150)
        {
            for(int x = 0; x < VectorCount; x++)
            {
                SearchVector.erase(SearchVector.end() - 1);
            }
            Utilities->CallLogPop(1421);
            return(false);
        }
//deal with failed points, added at v2.13.0
        if((SearchElement.TrackType == Points) && (SearchElement.Config[SearchElement.ELinkPos] == Lead) && Track->TrackElementAt(1524, SearchElement.TrackVectorPosition).Failed) //leading entry
        {
            if(Track->TrackElementAt(1525, SearchElement.TrackVectorPosition).Attribute == 0)
            {
                SearchElement.XLinkPos = 1;
                SearchElement.XLink = SearchElement.Link[SearchElement.XLinkPos];
            }
            else
            {
                SearchElement.XLinkPos = 3;
                SearchElement.XLink = SearchElement.Link[SearchElement.XLinkPos];
            }
        }
        else if((SearchElement.TrackType == Points) && (SearchElement.Config[SearchElement.ELinkPos] == Trail) && Track->TrackElementAt(1526, SearchElement.TrackVectorPosition).Failed) //trailing entry
        {
            if((Track->TrackElementAt(1527, SearchElement.TrackVectorPosition).Attribute == 0) && (SearchElement.ELinkPos == 3)) //can't go any further
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(2533);
                return(false);
            }
            if((Track->TrackElementAt(1528, SearchElement.TrackVectorPosition).Attribute == 1) && (SearchElement.ELinkPos == 1)) //can't go any further
            {
                for(int x = 0; x < VectorCount; x++)
                {
                    SearchVector.erase(SearchVector.end() - 1);
                }
                Utilities->CallLogPop(2534);
                return(false);
            }
        }

// check if reached a non-failed leading point
        if((SearchElement.TrackType == Points) && (SearchElement.Config[SearchElement.ELinkPos] == Lead) && !SearchElement.Failed)
        {       //added !Failed condition at v2.13.0 to exclude failed points
// XLink set to points 'set' position - Attribute == 0, SearchPos1 = 1 & SearchPos2 = 3; Attribute == 1, SearchPos1 = 3 & SearchPos2 = 1;
            int SearchPos1 = SearchElement.Attribute + 1;
            int SearchPos2;
            if(SearchPos1 == 2)
            {
                SearchPos1++;
            }
            if(SearchPos1 == 1)
            {
                SearchPos2 = 3;
            }
            else
            {
                SearchPos2 = 1;
            }
// push element with XLink set to position [SearchPos1]
            SearchElement.XLink = SearchElement.Link[SearchPos1];
            SearchElement.XLinkPos = SearchPos1;
// check for a fouled diagonal for leading point for XLinkPos == SearchPos1)
            if((SearchElement.XLink == 1) || (SearchElement.XLink == 3) || (SearchElement.XLink == 7) || (SearchElement.XLink == 9))
            {
                if(AllRoutes->DiagonalFouledByRouteOrTrain(4, SearchElement.HLoc, SearchElement.VLoc, SearchElement.XLink))
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(300);
                    return(false);
                }
            }
            SearchVector.push_back(SearchElement);
            VectorCount++;
            TotalSearchCount++;
// recursive search at XLinkPos of SearchPos1 (i.e. 'set' trailing exit)
// Note that NextTrackElement is the TTrackElement that the TPrefDirElement SearchElement is constructed from.  Can't use SearchElement in the
// recursive search as has to be a TTrackElement for non-preferred route searches
            if(SearchForNonPreferredRoute(6, NextTrackElement, SearchPos1, RequiredPosition, ReqPosRouteID, true))
            {
                if(!RecursiveCall && SignalHasFailed(4)) //added at v2.13.0
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(2526);
                    return(false);
                }
                Utilities->CallLogPop(301);
                return(true);
            }
            else
            {
// remove leading point with XLinkPos [SearchPos1]
                SearchVector.erase(SearchVector.end() - 1);
                VectorCount--;
// push element with XLink set to position [SearchPos2]
                SearchElement.XLink = SearchElement.Link[SearchPos2];
                SearchElement.XLinkPos = SearchPos2;
// check for a fouled diagonal for leading point for XLinkPos == SearchPos2)
                if((SearchElement.XLink == 1) || (SearchElement.XLink == 3) || (SearchElement.XLink == 7) || (SearchElement.XLink == 9))
                {
                    if(AllRoutes->DiagonalFouledByRouteOrTrain(5, SearchElement.HLoc, SearchElement.VLoc, SearchElement.XLink))
                    {
                        for(int x = 0; x < VectorCount; x++)
                        {
                            SearchVector.erase(SearchVector.end() - 1);
                        }
                        Utilities->CallLogPop(302);
                        return(false);
                    }
                }
                SearchVector.push_back(SearchElement);
                VectorCount++;
                TotalSearchCount++;
// recursive search at XLinkPos of SearchPos2 (i.e. 'unset' trailing exit)
                if(SearchForNonPreferredRoute(7, NextTrackElement, SearchPos2, RequiredPosition, ReqPosRouteID, true))
                {
                    if(!RecursiveCall && SignalHasFailed(5)) //added at v2.13.0
                    {
                        for(int x = 0; x < VectorCount; x++)
                        {
                            SearchVector.erase(SearchVector.end() - 1);
                        }
                        Utilities->CallLogPop(2527);
                        return(false);
                    }
                    Utilities->CallLogPop(303);
                    return(true);
                }
                else
                {
                    for(int x = 0; x < VectorCount; x++)
                    {
                        SearchVector.erase(SearchVector.end() - 1);
                    }
                    Utilities->CallLogPop(304);
                    return(false);
                }
            }
        } // if leading point

// here if ordinary element, push it, inc VectorCount & update CurrentTrackElement
// ready for next element on route
        SearchVector.push_back(SearchElement);
        VectorCount++;
        TotalSearchCount++;
        CurrentTrackElement = SearchElement;
        XLinkPos = SearchElement.XLinkPos; // wasn't a leading point so XLinkPos defined
    } // while(true)
}

// ---------------------------------------------------------------------------

void TOneRoute::SetRemainingSearchVectorValues(int Caller)

/*
      This function is developed from ConvertPrefDirSearchVector, to deal with search elements not
      having all values set (since not necessarily on PrefDirs).
      Enter with SearchVector established, return if empty.  The first element may not have its ELink & XLink etc set
      (if it was the start), so these are checked first and set if necessary.  All elements now have
      all but EXNumber set, so the CheckCount is set to 8 to cover all but EXNumber, and that is then set
      for all elements (unless validity check fails) and CheckCount incremented.  Finally SetRouteSearchVectorGraphics() is called
      to set the route colour and direction graphics.
*/

{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetRemainingSearchVectorValues");
    if(SearchVector.size() == 0)
    {
        throw Exception("Error, SearchVector empty");
    }
// first SearchElement may have ELink & XLink not set if entered in GetStart.... i.e if it wasn't already in a route
// hence need to examine and update it if necessary
    TPrefDirElement SecondElement;

    if(SearchVector.size() > 1) // if search vector only a single element then first element must have been in a route, and in this case
    // all data members will have been set in SearchForNonPreferredRoute except for EXNumber.
    // need above check or SecondElement will fail
    {
        SecondElement = SearchVector.at(1);
        // SearchVector.at(0) ELink & XLink not set if was first element in route; XLink also not set if was a leading point though can't be for a route
        for(int x = 0; x < 4; x++)
        {
            if(SearchVector.at(0).Conn[x] == SecondElement.TrackVectorPosition)
            {
                if(SearchVector.at(0).XLink == -1) // i.e. not set
                {
                    SearchVector.at(0).XLink = SearchVector.at(0).Link[x];
                    SearchVector.at(0).XLinkPos = x;
                }
                int ELinkPos;
                if(SearchVector.at(0).XLinkPos == 0)
                {
                    ELinkPos = 1; // use actual value rather than 'x' as may be a gap with both ends
                }
                // linked to 1st searchvector element, & if XLink was set then x may not correspond
                if(SearchVector.at(0).XLinkPos == 1)
                {
                    ELinkPos = 0;
                }
                if(SearchVector.at(0).XLinkPos == 2)
                {
                    ELinkPos = 3;
                }
                if(SearchVector.at(0).XLinkPos == 3)
                {
                    ELinkPos = 2;
                }
                if(SearchVector.at(0).ELink == -1) // because was start element, & can't be points, but could be a gap
                {
                    SearchVector.at(0).ELink = SearchVector.at(0).Link[ELinkPos];
                    SearchVector.at(0).ELinkPos = ELinkPos;
                }
                break; // no point going any further
            }
        }
    }
    for(unsigned int x = 0; x < SearchVector.size(); x++)
    {
        SearchVector.at(x).CheckCount = 8; // to account for all but EXNumber
// set EXNumber
        if(!(SearchVector.at(x).EntryExitNumber()))
        {
            throw Exception("Error in EntryExitNumber 3");
        }
        SearchVector.at(x).CheckCount++;
// all values now incorporated
    }

    SetRouteSearchVectorGraphics(5, false, false); // change graphic colour to the route colour
// This function is only called here for nonsignals routes, so AutoSigsFlag & PrefDirRoute both false
// PrefDir is validated in ConvertAndAddNonPreferredRouteSearchVector
    Utilities->CallLogPop(305);
}

// ---------------------------------------------------------------------------

void TOneRoute::ConvertAndAddNonPreferredRouteSearchVector(int Caller, IDInt ReqPosRouteID)

/*
      This function is very similar to ConvertAndAddPreferredRouteSearchVector except that the route in SearchVector can't be an
      AutoSigsRoute.
      Action varies depending on whether it is a completely new route, or an extension of an existing route at the
      beginning or the end.
      Ignore if SearchVector empty, and check that all the new elements in SearchVector are valid.
      Check if route end selection is in an existing route (ReqPosRouteID > -1), if so and existing route is non-autosigs
      add its elements to the SearchVector then delete the route, decrementing StartSelectionRouteNumber if the RequPosRouteNumber was
      less than StartSelectionRouteID.  If existing route is AutoSigs then the final search element is dropped from the SearchVector,
      since the new route will end adjacent to the AutoSigs route, and the existing route is left as it is.
      Note that a single route cannot contain both AutoSig & non-AutoSig elements, each route of AutoSig elements
      has its own identity.  A single route can however have a mixture of Unrestricted and PreferredRoute elements

      Check if StartSelectionRouteID set (extending an existing route) and if so and if existing route non-autosig, then add the new route
      to the existing route (start element not stored in searchvector), call SetRoutePoints & SetRouteSignals for the extended route and return.
      If the existing route is autosig, then leave the existing route as it is and continue as for routes that aren't linked to an existing
      route at the start.

      Check the validity of the route in SearchVector, and create a new route using StoreOneRoute.  Finally call SetRoutePoints & SetRouteSignals
      for the new route and return.
*/

{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ConvertAndAddNonPreferredRouteSearchVector," +
                                 AnsiString(ReqPosRouteID.GetInt()));
    if(SearchVector.size() < 1)
    {
        Utilities->CallLogPop(306);
        return;
    }
    PrefDirVector = SearchVector; // this copy is to validate the vector up to this point,
    if(!ValidatePrefDir(6))
    {
        Utilities->CallLogPop(307);
        return;
    }
    TAllRoutes::TLockedRouteClass LockedRouteObject;

    AllRoutes->LockedRouteFoundDuringRouteBuilding = false;
    unsigned int TruncatePrefDirPosition = 0;

    if(ReqPosRouteID > -1) // Note that ReqPosRouteID != StartSelectionRouteID as would have failed in GetNextRouteElement
/* if have ReqPosRouteID:
      if existing route non-autosig, then add the old route to the SearchVector then delete the old route
      if existing route autosig, drop the final search element in the new route, leave the existing route as it is,
       then enter the new route into the AllRoutesVector
*/
    {
        if(!(AllRoutes->GetFixedRouteAtIDNumber(45, ReqPosRouteID).GetFixedPrefDirElementAt(66, 0).AutoSignals))
        {
            for(unsigned int x = 1; x < AllRoutes->GetFixedRouteAtIDNumber(46, ReqPosRouteID).PrefDirSize();
                x++) // start at 1 as first element already in SearchVector
            {
                SearchVector.push_back(AllRoutes->GetFixedRouteAtIDNumber(47, ReqPosRouteID).GetFixedPrefDirElementAt(67, x));
            }
            // note that route numbers in map adjusted when ReqPos route cleared
            AllRoutes->ClearRouteDuringRouteBuildingAt(2, AllRoutes->GetRouteVectorNumber(6, ReqPosRouteID));
            // create a new locked route object (apart from RouteNumber) if required, for use later (LockedRouteFoundDuringRouteBuilding
            // set during ClearRouteDuringRouteBuildingAt)
            if(AllRoutes->LockedRouteFoundDuringRouteBuilding)
            {
                LockedRouteObject.RearTrackVectorPosition = AllRoutes->LockedRouteRearTrackVectorPosition;
                LockedRouteObject.LastTrackVectorPosition = AllRoutes->LockedRouteLastTrackVectorPosition;
                LockedRouteObject.LastXLinkPos = AllRoutes->LockedRouteLastXLinkPos;
                LockedRouteObject.LockStartTime = AllRoutes->LockedRouteLockStartTime;
            }
        }
        else if(AllRoutes->GetFixedRouteAtIDNumber(48, ReqPosRouteID).GetFixedPrefDirElementAt(68, 0).AutoSignals)
        {
            SearchVector.pop_back();
        }
    }
    if(StartSelectionRouteID > -1)
/* if have StartSelectionRouteID:
      if existing route non-autosig, then add the new route to the existing route (start element not stored in searchvector)
      if existing route autosig, leave the existing route as it is, and just enter the new route into the AllRoutesVector
*/
    {
        if(AllRoutes->IsThereARouteAtIDNumber(1, StartSelectionRouteID))
        // need to test because may have been removed by a train moving in the wrong direction between first and last route selections - added at v1.3.1
        {
            if(!(AllRoutes->GetFixedRouteAtIDNumber(49, StartSelectionRouteID).GetFixedPrefDirElementAt(69, 0).AutoSignals))
            {
                int RouteNumber = AllRoutes->GetRouteVectorNumber(1, StartSelectionRouteID);
                for(unsigned int x = 0; x < SearchVector.size(); x++)
                {
                    AllRoutes->AddRouteElement(2, GetFixedSearchElementAt(4, x).HLoc, GetFixedSearchElementAt(5, x).VLoc, GetFixedSearchElementAt(6, x).ELink,
                                               RouteNumber, GetFixedSearchElementAt(7, x));
                    // find & store locked route truncate position in PrefDirVector for later use
                    if(AllRoutes->LockedRouteFoundDuringRouteBuilding)
                    {
                        if(GetFixedSearchElementAt(16, x).TrackVectorPosition == int(AllRoutes->LockedRouteRearTrackVectorPosition))
                        {
                            TruncatePrefDirPosition = AllRoutes->GetFixedRouteAt(176, RouteNumber).PrefDirSize() - 1;
                        }
                    }
                }
                if(!(AllRoutes->GetModifiableRouteAtIDNumber(4, StartSelectionRouteID).ValidatePrefDir(7)))
                {
                    throw Exception("Failed to validate extended route for nonpreferred route");
                }
                AllRoutes->GetModifiableRouteAtIDNumber(5, StartSelectionRouteID).SetRoutePoints(2);
                AllRoutes->GetModifiableRouteAtIDNumber(6, StartSelectionRouteID).SetRouteSignals(6);
                AllRoutes->GetModifiableRouteAtIDNumber(9, StartSelectionRouteID).SetLCChangeValues(2, false); // PrefDirRoute is false
                // now add the reinstated locked route if required and set signals accordingly
                // shouldn't ever need to access this as the train that has caused the locked route will be ahead of the route to be added,
                // and it will have removed the route elements that it is standing on, but include in case there's some obscure condition
                // that I haven't thought of
                if(AllRoutes->LockedRouteFoundDuringRouteBuilding)
                {
                    LockedRouteObject.RouteNumber = RouteNumber;
                    AllRoutes->LockedRouteVector.push_back(LockedRouteObject);
                    // now reset the signals for the locked route
                    AllRoutes->SetAllRearwardsSignals(12, 0, RouteNumber, TruncatePrefDirPosition);
                    for(int c = AllRoutes->GetFixedRouteAt(177, RouteNumber).PrefDirSize() - 1; c >= (int)TruncatePrefDirPosition;
                        c--) // must use int for >= test to succeed when TruncatePrefDirPosition == 0
                    {
                        // return all signals to red in route section to be truncated
                        TPrefDirElement PrefDirElement = AllRoutes->GetFixedRouteAt(178, RouteNumber).PrefDirVector.at(c);
                        TTrackElement & TrackElement = Track->TrackElementAt(813, PrefDirElement.TrackVectorPosition);
                        if(PrefDirElement.Config[PrefDirElement.XLinkPos] == Signal)
                        {
                            TrackElement.Attribute = 0;
                            Track->PlotSignal(11, TrackElement, Display);
                            Display->PlotOutput(115, PrefDirElement.HLoc * 16, PrefDirElement.VLoc * 16, PrefDirElement.EXGraphicPtr);
                            Display->PlotOutput(116, PrefDirElement.HLoc * 16, PrefDirElement.VLoc * 16, PrefDirElement.EntryDirectionGraphicPtr);
                        }
                    }
                }
                AllRoutes->CheckMapAndRoutes(3); // test
                Utilities->CallLogPop(308);
                return;
            }
        }
        else
        {
            StartSelectionRouteID = IDInt(-1);
        }
// if new route non-autosig and existing route autosig, leave the existing route as it is, and just enter the new route into the AllRoutesVector
// hence nothing to do here
    }
    PrefDirVector = SearchVector; // copy again prior to storing as a route as SearchVector may have been extended
    if(!ValidatePrefDir(8)) // validate PrefDir for all new route elements
    {
        throw Exception("Failed to validate single route for nonpreferred route");
    }
    AllRoutes->StoreOneRoute(2, this);
    AllRoutes->GetModifiableRouteAt(6, AllRoutes->AllRoutesSize() - 1).SetRoutePoints(3); // new addition
    AllRoutes->GetModifiableRouteAt(17, AllRoutes->AllRoutesSize() - 1).SetRouteSignals(7); // new addition
    AllRoutes->GetModifiableRouteAt(19, AllRoutes->AllRoutesSize() - 1).SetLCChangeValues(3, false); // ConsecSignalsRoute is false
    AllRoutes->CheckMapAndRoutes(4); // test
    Utilities->CallLogPop(309);
}

// ---------------------------------------------------------------------------

void TOneRoute::SetRoutePoints(int Caller) const
/*
      Examine each set of points in the route to see if entry or exit is via the straight or diverging trailing
      link, and set the attribute accordingly (don't need to worry about linked routes, points in those will have been set
      when they were created.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetRoutePoints");
    if(!PrefDirVector.empty())
    {
        for(TPrefDirVectorConstIterator PrefDirPtr = (PrefDirVector.end() - 1); PrefDirPtr >= PrefDirVector.begin(); PrefDirPtr--)
        {
            if((PrefDirPtr->TrackType == Points) && ((PrefDirPtr->ELinkPos == 1) || (PrefDirPtr->XLinkPos == 1))) //1=straight trailing
            {
                Track->TrackElementAt(96, PrefDirPtr->TrackVectorPosition).Attribute = 0; // 0=straight
                Track->PlotPoints(3, Track->TrackElementAt(97, PrefDirPtr->TrackVectorPosition), Display, false);
            }
            if((PrefDirPtr->TrackType == Points) && ((PrefDirPtr->ELinkPos == 3) || (PrefDirPtr->XLinkPos == 3))) //3=diverging trailing
            {
                Track->TrackElementAt(98, PrefDirPtr->TrackVectorPosition).Attribute = 1; // 1=diverging
                Track->PlotPoints(4, Track->TrackElementAt(99, PrefDirPtr->TrackVectorPosition), Display, false);
            }
        }
    }
    Utilities->CallLogPop(327);
}

// ---------------------------------------------------------------------------

void TOneRoute::SetRouteSignals(int Caller) const
/* Used for new train additions in AddTrain and in route setting
      Set the signals as follows:-
      First check whether there is a linked forward route, and if so use FindForwardTargetSignalAttribute to work along it from the start
      until find a train (return Attribute = 0 & NextForwardLinkedRouteNumber = -1), a buffer (return Attribute = 1 &
      NextForwardLinkedRouteNumber = -1), a continuation (return Attribute = 3 & NextForwardLinkedRouteNumber = -1) or a forward-facing
      signal.  If find a signal its attribute value + 1 up to a maximum value of 3 is returned & NextForwardLinkedRouteNumber = -1.
      The above Attribute values represent the 'target' attribute, from which all rearwards signals in turn in the new route are set,
      the first using the returned attribute value and subsequent ones incrementing the Attribute up to a maximum of 3.  All the foregoing
      return true, as does finding none of the above and no onward linked forward route (NextForwardLinkedRouteNumber = -1).  If none
      of the foregoing are found but there is a further forward linked forward route then the function returns false with
      NextForwardLinkedRouteNumber = the next forward linked route number, to allow that to be examined similarly, and Attribute = 0.

      When the target Attribute is found (will be 0 if no forward linked route), then SetAllRearwardsSignals is used to work back from
      the end of the route setting each forward-facing signal one step nearer green as described above, until either reach the end of all
      linked rearwards routes or find a train.  If find a train in the current route then signals behind it (and behind any other trains
      in the current route) are set appropriately (including in linked rear routes), but if find a train in a linked rear route then no
      further signals are set.  If there is no forward linked route and the front end of the current route is a buffer then
      SetAllRearwardsSignals (in its call to SetRearwardsSignalsReturnFalseForTrain) treats it as a red signal, and if a continuation,
      as a green signal.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetRouteSignals");
    if(!PrefDirVector.empty())
    {
        // get target Attribute value, check first if there is a forward linked route
        TPrefDirElement LastElement = GetFixedPrefDirElementAt(185, PrefDirSize() - 1);
        TPrefDirElement FirstElement = GetFixedPrefDirElementAt(186, 0);
        int ForwardLinkedRouteNumber, Attribute = 0;
        if(LastElement.Conn[LastElement.XLinkPos] > -1)
        // Note that LastElement can't be points but can be linked to points
        {
            if(AllRoutes->GetRouteTypeAndNumber(16, LastElement.Conn[LastElement.XLinkPos], LastElement.ConnLinkPos[LastElement.XLinkPos],
                                                ForwardLinkedRouteNumber) != TAllRoutes::NoRoute)
            {
                if(ForwardLinkedRouteNumber > -1)
                {
                    int NextForwardLinkedRouteNumber = -1;
                    while(!(AllRoutes->GetFixedRouteAt(171, ForwardLinkedRouteNumber).FindForwardTargetSignalAttribute(1, NextForwardLinkedRouteNumber,
                                                                                                                       Attribute)))
                    {
                        ForwardLinkedRouteNumber = NextForwardLinkedRouteNumber;
                    }
                    // if find a train before a signal then Attribute = 0, else if find end of route is a buffer then Attribute = 1, or a continuation then
                    // Attribute = 3, else if find signal (other than a ground signal with Attribute > 0 [added at v2.14.0]) then Attribute = (signal attribute + 1) (or
                    // same as signal Attribute if ground signal with Attribute > 0) up to a max value of 3.  All these return true, if find a forward linked
                    // route then the routenumber is set in NextForwardLinkedRouteNumber, Attribute = 0 & returns false.
                }
            }
        }
        int RouteNumber;
        TAllRoutes::TRouteType RouteType = AllRoutes->GetRouteTypeAndNumber(15, GetFixedPrefDirElementAt(187, 0).TrackVectorPosition,
                                                                            GetFixedPrefDirElementAt(193, 0).XLinkPos, RouteNumber);
        if(RouteType != TAllRoutes::NoRoute)
        // it will be, above only used to get RouteNumber, can choose any element in the route so use GetFixedPrefDirElementAt
        {
            AllRoutes->SetAllRearwardsSignals(8, Attribute, RouteNumber, PrefDirSize() - 1);
        }
    }
    Utilities->CallLogPop(1720);
}

// ---------------------------------------------------------------------------

bool TOneRoute::PointsToBeChanged(int Caller, int &NewFailedPointsTVPos) const
{
    //true if at any point in SearchVector points have to be changed,
    //changed to give every point to be changed in route to have a chance of failure, but if one fails then don't look any further
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PointsToBeChanged");
    NewFailedPointsTVPos = -1; //default value for no new failure
    bool PointsChanged = false;
    if(!SearchVector.empty())
    {
        for(TPrefDirVectorConstIterator SearchPtr = SearchVector.begin(); SearchPtr != SearchVector.end(); SearchPtr++)
        {
            TTrackElement &TE = Track->TrackElementAt(1504, SearchPtr->TrackVectorPosition);
    //check for an existing failed point where needs to change to make the route
            int Attr = TE.Attribute;
            if((SearchPtr->TrackType == Points) && ((SearchPtr->ELinkPos == 1) || (SearchPtr->XLinkPos == 1))) // 1=want to go straight
            {
                if(Attr == 1) //currently set to diverge
                {
                //here add new failure possibility at v2.13.0
                    if(Utilities->FailureMode != FNil)
                    {
                        if((random(Utilities->PointChangeEventsPerFailure) == 0) && !TE.Failed) //can't fail twice, but if failed should already have been picked up during search
                        {
                            TTrack::TInfrastructureFailureEntry IFE;
                            NewFailedPointsTVPos = SearchPtr->TrackVectorPosition;
                            IFE.TVPos = NewFailedPointsTVPos;
                            TE.Failed = true;
                            TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 = TE.SpeedLimit01; //store these values temporarily, points aren't bridges so can use these
                            TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 = TE.SpeedLimit23;
                            TE.SpeedLimit01 = 10; //values while failed
                            TE.SpeedLimit23 = 10;
                            Display->WarningLog(13, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Points failed at " + TE.ElementID);
                            PerfLogForm->PerformanceLog(36, Utilities->Format96HHMMSS(TrainController->TTClockTime) + " WARNING: Points failed at " + TE.ElementID);
                            AllRoutes->RebuildRailwayFlag = true; //force ClearandRebuildRailway at next clock tick
    //set repair time, random value in minutes between 10 and 179
                            double FailureMinutes = double(random(Utilities->MaxRandomRepairTime) + Utilities->FixedMinRepairTime); //between 10 and 179 minutes at random
                            TDateTime RepairTime = TrainController->TTClockTime + TDateTime(FailureMinutes / 1440);
                            IFE.RepairTime = RepairTime;
                            IFE.FailureTime = TrainController->TTClockTime;
                            Track->FailedPointsVector.push_back(IFE);
                            Utilities->CallLogPop(1717);
                            return(true); //return so only allow one failure per route
                        }
                    }
                    PointsChanged = true; //this is used for setting the flash time
                }
            }
            if((SearchPtr->TrackType == Points) && ((SearchPtr->ELinkPos == 3) || (SearchPtr->XLinkPos == 3))) // 3=want to diverge
            {
                if(Attr == 0) //currently set to go straight
                {
                //here add failure possibility at v2.13.0
                    if(Utilities->FailureMode != FNil)
                    {
                        if((random(Utilities->PointChangeEventsPerFailure) == 0) && !TE.Failed) //can't fail twice, but if failed should already have been picked up during search
                        {
                            TTrack::TInfrastructureFailureEntry IFE;
                            NewFailedPointsTVPos = SearchPtr->TrackVectorPosition;
                            IFE.TVPos = NewFailedPointsTVPos;
                            TE.Failed= true;
                            TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01 = TE.SpeedLimit01; //store these values temporarily, points aren't bridges so can use these
                            TE.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23 = TE.SpeedLimit23;
                            TE.SpeedLimit01 = 10; //values while failed
                            TE.SpeedLimit23 = 10;
                            Display->WarningLog(14, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Points failed at " + TE.ElementID);
                            PerfLogForm->PerformanceLog(37, Utilities->Format96HHMMSS(TrainController->TTClockTime) + " WARNING: Points failed at " + TE.ElementID);
                            AllRoutes->RebuildRailwayFlag = true; //force ClearandRebuildRailway at next clock tick
    //set repair time, random value in minutes between 10 and 179
                            double FailureMinutes = double(random(Utilities->MaxRandomRepairTime) + Utilities->FixedMinRepairTime);
                            TDateTime RepairTime = TrainController->TTClockTime + TDateTime(FailureMinutes / 1440);
                            IFE.RepairTime = RepairTime;
                            IFE.FailureTime = TrainController->TTClockTime;
                            Track->FailedPointsVector.push_back(IFE);
                            Utilities->CallLogPop(1718);
                            return(true); //only allow one failure per route
                        }
                    }
                    PointsChanged = true;
                }
            }
        }
    }
    Utilities->CallLogPop(1719);
    return(PointsChanged);
}

// ---------------------------------------------------------------------------

bool TOneRoute::FindForwardTargetSignalAttribute(int Caller, int &NextForwardLinkedRouteNumber, int &Attribute) const
/*
      Works forward through the route until finds:-
      (a) a train - Attribute = 0, NextForwardLinkedRouteNumber = -1 & returns true;
      (b) end of route at buffers - Attribute = 1, NextForwardLinkedRouteNumber = -1 & returns true;
      (c) end of route at continuation - Attribute = 3, NextForwardLinkedRouteNumber = -1 & returns true;
      (d) level crossing with barriers not down - Attribute = 0, NextForwardLinkedRouteNumber = -1 & returns true;
      (e) forward-facing non-ground signal - Attribute = 1 + signal attribute (max value of 3), NextForwardLinkedRouteNumber = -1 & returns true; (non-ground sig condition added at v2.14.0)
      (e1) forward-facing ground signal with attribute 0 - Attribute = ground signal attribute + 1 (max value of 3), NextForwardLinkedRouteNumber = -1 & returns true; (ground sig condition added at v2.14.0)
      (e2) forward-facing ground signal with attribute > 0 - Attribute = ground signal attribute (max value of 3), NextForwardLinkedRouteNumber = -1 & returns true; (ground sig condition added at v2.14.0)
      (f) end of route not at any of foregoing and with no linked forward route - Attribute = 0, NextForwardLinkedRouteNumber = -1 & returns true;
      (g) linked forward route - Attribute = 0, NextForwardLinkedRouteNumber = the routenumber of the forward route & returns false.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindForwardTargetSignalAttribute");
    Attribute = 0;
    NextForwardLinkedRouteNumber = -1;
    for(unsigned int x = 0; x < PrefDirSize(); x++)
    {
        int TrainID = Track->TrackElementAt(100, PrefDirVector.at(x).TrackVectorPosition).TrainIDOnElement;
        if(PrefDirVector.at(x).TrackType == Bridge)
        {
            if(PrefDirVector.at(x).XLinkPos < 2)
            {
                TrainID = Track->TrackElementAt(101, PrefDirVector.at(x).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
            }
            else
            {
                TrainID = Track->TrackElementAt(102, PrefDirVector.at(x).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
            }
        }
        if(TrainID != -1)
        {
            Utilities->CallLogPop(328);
            return(true);
        }
        if(PrefDirVector.at(x).TrackType == Buffers)
        {
            Attribute = 1;
            Utilities->CallLogPop(329);
            return(true);
        }
        if(PrefDirVector.at(x).TrackType == Continuation)
        {
            Attribute = 3;
            Utilities->CallLogPop(330);
            return(true);
        }
        if(Track->IsLCAtHV(42, PrefDirVector.at(x).HLoc, PrefDirVector.at(x).VLoc))
        {
            if(!Track->IsLCBarrierDownAtHV(3, PrefDirVector.at(x).HLoc, PrefDirVector.at(x).VLoc))
            {
                Attribute = 0;
                Utilities->CallLogPop(1950);
                return(true);
            }
        }
        if(PrefDirVector.at(x).Config[PrefDirVector.at(x).XLinkPos] == Signal)
        {
            Attribute = Track->TrackElementAt(103, PrefDirVector.at(x).TrackVectorPosition).Attribute; //added at v2.14.0
            if((PrefDirVector.at(x).SigAspect != TTrackElement::GroundSignal) || (Attribute == 0))//added at v2.14.0
            {
                Attribute++;
            }
            if(Attribute > 3)
            {
                Attribute = 3;
            }
            Utilities->CallLogPop(331);
            return(true);
        }
        if(x == PrefDirSize() - 1)
        {
            TPrefDirElement LastElement = PrefDirVector.at(x);
            if(LastElement.Conn[LastElement.XLinkPos] > -1)
            {
                if(AllRoutes->GetRouteTypeAndNumber(2, LastElement.Conn[LastElement.XLinkPos],
                                                    Track->GetNonPointsOppositeLinkPos(LastElement.ConnLinkPos[LastElement.XLinkPos]), NextForwardLinkedRouteNumber) != TAllRoutes::NoRoute)
                {
                    Attribute = 0;
                    Utilities->CallLogPop(332);
                    return(false);
                }
            }
        }
    }
    Utilities->CallLogPop(333);
    return(true);
}

// ---------------------------------------------------------------------------

bool TOneRoute::SetRearwardsSignalsReturnFalseForTrain(int Caller, int &Attribute, int PrefDirVectorStartPosition) const
/*
      This function is only called by TAllRoutes::SetAllRearwardsSignals.

      Enter with Attribute set to the value to be used (unless modified by the initial forward search - see later) for the first rearwards
      signal found, and with PrefDirVectorStartPosition set to the position in PrefDirVector to begin the search.  BUT, don't begin with the
      rearward search, first search forwards from the PrefDirVectorStartPosition in case the end of the route is a buffer or continuation, and
      modify the Attribute accordingly UNLESS (a) train present between PrefDirVectorStartPosition & end; (b) route in
      ContinuationAutoSigVector (i.e. train has exited the route at a continuation but it is still affecting the signals), or (c) truncating
      a route.

      Having received or modified Attribute as above, work backwards from the PrefDirVectorStartPosition until find a train - return false, or a
      signal.  If find a signal set its Attribute to the current Attribute value up to a maximum of 3, and replot the signal as well as
      the required route and direction (if required) graphics, then increment Attribute up to a max. of 3 [addition at v2.9.2: if signal or element beyond
      it is in a locked route then set signal to red & change Attribute to 0 - this fault reported by Simon Banham 21/07/21 as an image]. and continue working
      backwards for the next signal (or train - return false as before) and so on.  On completion Attribute is passed back from the function as a
      reference.  If no train is found before the beginning of the route is reached the function returns true

      In setting signals skip the first position if it's a signal and if truncating - otherwise the truncated signal counts as the first red
      and the next rearwards signal becomes yellow, although it's the first in the route
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetRearwardsSignalsReturnFalseForTrain," + AnsiString(Attribute) + "," +
                                 AnsiString(PrefDirVectorStartPosition));
    Graphics::TBitmap *EXGraphicPtr = RailGraphics->bmTransparentBgnd; // default values
    Graphics::TBitmap *EntryDirectionGraphicPtr = RailGraphics->bmTransparentBgnd;
// if no train between end of route and PrefDirVectorStartPosition, route not in ContinuationAutoSigVector
// & not truncating a route, then Attribute can be modified if end is buffers or continuation
    bool SkipContinuationAndBufferAttributeChange = false;

    if(!PrefDirVector.empty())
    {
        for(TPrefDirVectorConstIterator PrefDirPtr = (PrefDirVector.begin() + PrefDirVectorStartPosition); PrefDirPtr < PrefDirVector.end(); PrefDirPtr++)
        {
            int TrainID = Track->TrackElementAt(104, PrefDirPtr->TrackVectorPosition).TrainIDOnElement;
            if(PrefDirPtr->TrackType == Bridge)
            {
                if(PrefDirPtr->XLinkPos < 2)
                {
                    TrainID = Track->TrackElementAt(105, PrefDirPtr->TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = Track->TrackElementAt(106, PrefDirPtr->TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID != -1)
            {
                SkipContinuationAndBufferAttributeChange = true;
                break;
            }
        }

        TTrainController::TContinuationAutoSigVectorIterator AutoSigVectorIT;
        if(!TrainController->ContinuationAutoSigVector.empty())
        {
            for(AutoSigVectorIT = TrainController->ContinuationAutoSigVector.begin(); AutoSigVectorIT < TrainController->ContinuationAutoSigVector.end();
                AutoSigVectorIT++)
            {
                if(!AllRoutes->AllRoutesVector.empty())
                {
                    if((&AllRoutes->AllRoutesVector.front() + AutoSigVectorIT->RouteNumber) == this)
                    {
                        SkipContinuationAndBufferAttributeChange = true;
                        break;
                    }
                }
            }
        }
        if(AllRoutes->RouteBackTruncateFlag)
        {
            SkipContinuationAndBufferAttributeChange = true;
        }
        if(!SkipContinuationAndBufferAttributeChange)
        {
            if(PrefDirVector.back().TrackType == Buffers)
            {
                Attribute = 1; // treat buffer as red signal
            }
            if(PrefDirVector.back().TrackType == Continuation)
            {
                Attribute = 3; // treat continuation as a green signal
            }
        }
        for(TPrefDirVectorConstIterator PrefDirPtr = (PrefDirVector.begin() + PrefDirVectorStartPosition); PrefDirPtr >= PrefDirVector.begin(); PrefDirPtr--)
        {
            int TrainID = Track->TrackElementAt(107, PrefDirPtr->TrackVectorPosition).TrainIDOnElement;
            if(PrefDirPtr->TrackType == Bridge)
            {
                if(PrefDirPtr->XLinkPos < 2)
                {
                    TrainID = Track->TrackElementAt(108, PrefDirPtr->TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = Track->TrackElementAt(109, PrefDirPtr->TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID != -1)
            {
                Utilities->CallLogPop(334);
                return(false);
            }
            // if find an LC that is closed to trains (or flashing - may be extending an earlier route with flashing LCs) then reset
            // the attribute to 0 so first signal behind the LC is red
            if(Track->IsLCAtHV(20, PrefDirPtr->HLoc, PrefDirPtr->VLoc))
            {
                if(!Track->IsLCBarrierDownAtHV(1, PrefDirPtr->HLoc, PrefDirPtr->VLoc))
                {
                    Attribute = 0;
                }
            }
// now set signals, but skip the first position if it's a signal on an unrestricted route and truncating - otherwise the truncated signal
// counts as the first red and the next rearwards signal becomes yellow, although it's the first in the route
            if(PrefDirPtr->Config[PrefDirPtr->XLinkPos] == Signal)
            {
                if((!AllRoutes->RouteBackTruncateFlag) || (PrefDirPtr != (PrefDirVector.begin() + PrefDirVectorStartPosition)) || PrefDirPtr->AutoSignals ||
                   PrefDirPtr->PrefDirRoute)
                {
//new section at v2.9.2 to check for pref dir element in a locked route, and if so set Attribute to 0 (red).  When emerge from locked route Attribute
//still 0 so first signal behind it also stays red.  After that Attribute goes back to normal.
                    int LockedVecNum = 0; //not used
                    TPrefDirElement DummyPrefDir; //not used
                    bool KeepAttributeAt0ForLockedRoute = false;
                    if(AllRoutes->IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(15, PrefDirPtr->TrackVectorPosition, PrefDirPtr->XLinkPos, DummyPrefDir,
                        LockedVecNum))
                        {
                            Attribute = 0;
                            KeepAttributeAt0ForLockedRoute = true;
                        }
//end of v2.9.2 addition

//v2.14.0 addition to avoid incrementing attribute for forward ground signals, ground sig itself takes attribute of forward signal + 1
                    bool NotGroundSignal = false;
                    if(PrefDirPtr->SigAspect != TTrackElement::GroundSignal)
                    {
                        NotGroundSignal = true;
                    }

                    if(Track->TrackElementAt(1529, PrefDirPtr->TrackVectorPosition).Failed) //addition at v2.13.0 for signal failures
                    {
                        Attribute = 0; //stays at 0
                    }

                    if(Attribute < 3)
                    {
                        Track->TrackElementAt(110, PrefDirPtr->TrackVectorPosition).Attribute = Attribute;
                    }
                    else
                    {
                        Track->TrackElementAt(111, PrefDirPtr->TrackVectorPosition).Attribute = 3; // green
                    }
                    Track->PlotSignal(1, Track->TrackElementAt(112, PrefDirPtr->TrackVectorPosition), Display);
                    if(AllRoutes->GetRouteTypeAndGraphics(1, PrefDirPtr->TrackVectorPosition, PrefDirPtr->XLinkPos, EXGraphicPtr,
                                                          EntryDirectionGraphicPtr) != TAllRoutes::NoRoute)
                    {
                        Display->PlotOutput(16, Track->TrackElementAt(113, PrefDirPtr->TrackVectorPosition).HLoc * 16,
                                            Track->TrackElementAt(114, PrefDirPtr->TrackVectorPosition).VLoc * 16, EXGraphicPtr);
                        Display->PlotOutput(17, Track->TrackElementAt(115, PrefDirPtr->TrackVectorPosition).HLoc * 16,
                                            Track->TrackElementAt(116, PrefDirPtr->TrackVectorPosition).VLoc * 16, EntryDirectionGraphicPtr);
                    }
                    if((Attribute < 3) && !KeepAttributeAt0ForLockedRoute && (NotGroundSignal || (Attribute == 0))) //NotGroundSignal... added at v2.14.0 (see above)
                    {                                                                                               //if groundsignal attrib is 0 then do need to increment
                        Attribute++; //this is for the next signal rearwards, not the current one
                    }
//                    Display->Update(); // update after recent plots   //dropped at v2.14.0 to avoid signals on routes showing before loaded session, relaced by the below
                    AllRoutes->RebuildRailwayFlag = true; //added at v2.14.0 to force a rebuild in place of the above
                }
            }
        }
    }
    Utilities->CallLogPop(335);
    return(true);
}

// ---------------------------------------------------------------------------

void TOneRoute::TruncateRoute(int Caller, int HLoc, int VLoc, bool PrefDirRoute, TTruncateReturnType &ReturnFlag)
/*
      Examines the route to see whether the element at H & V is in the route, and if not returns a ReturnFlag value of NotInRoute.
      If it is in a route but the element selected is invalid, then a message is given and returns with a ReturnFlag value of
      InRouteFalse.  Otherwise the route is truncated at and including the element that matches H & V with a ReturnFlag value of InRouteTrue.
      Selection invalid if select a bridge; trying to leave a single element; last element to be left
      not a signal (for PrefDirRoute or has AutoSigsFlag set); last element to be left a bridge, points or crossover (for not
      PrefDirRoute & AutoSigsFlag not set), or part of route locked.  Check if a train approaching or occupying route and lock route
      if required after offering the user the choice to continue or not.  Then SetAllRearwardsSignals is called to set signals before the
      truncate point, beginning with a red signal, and RemoveRouteElement called for all elements from the end to and including the truncate point.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TruncateRoute," + AnsiString(HLoc) + "," + AnsiString(VLoc) +
                                 "," + AnsiString((short)PrefDirRoute));
    bool ElementInRoute = false;
    bool MovingTrainOccupyingRoute = false;
    unsigned int TruncatePDElementPos; //the selected PD position to truncate to (could be from the back or the front)//added at v2.15.0
    enum {NoTruncate, BackTruncate, FrontTruncate, FullTruncate} TruncateType;
    TruncateType = NoTruncate;
    AllRoutes->RouteBackTruncateFlag = false;

    for(unsigned int b = 0; b < PrefDirSize(); b++)
    {
        if((PrefDirVector.at(b).HLoc == HLoc) && (PrefDirVector.at(b).VLoc == VLoc))
        {
            TruncatePDElementPos = b;
            ElementInRoute = true;
            break;
        }
    }
    if(!ElementInRoute)
    {
        ReturnFlag = NotInRoute;
        Utilities->CallLogPop(336);
        return;
    }

    //now find whether back, front or full truncate //added at v2.15.0
    if(TruncatePDElementPos < (PrefDirSize() - 1)) //if last position then can't be a front truncate
    {
        if(TruncatePDElementPos == 0)
        {
            TruncateType = FullTruncate;
        }
        else
        {
            TPrefDirElement TempElement = PrefDirVector.at(TruncatePDElementPos + 1); //+1 will exist becaue of first condition
            if(TempElement.Config[TempElement.XLinkPos] == Signal)
            {
                TruncateType = FrontTruncate;
            }
            else
            {
                TruncateType = BackTruncate;
                AllRoutes->RouteBackTruncateFlag = true;

            }
        }
    }
    else // == PrefDirSize() - 1
    {
        TruncateType = BackTruncate;
        AllRoutes->RouteBackTruncateFlag = true;
    }

// it is in the route so continue, first look for a train or a flashing level crossing in the part to be removed

    if(TruncateType == BackTruncate) //added at v2.15.0
    {
        for(int b = PrefDirSize() - 1; b >= 0; b--)
        {
            int TrainID = Track->TrackElementAt(117, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnElement;
            if(PrefDirVector.at(b).TrackType == Bridge)
            {
                if(PrefDirVector.at(b).XLinkPos < 2)
                {
                    TrainID = Track->TrackElementAt(118, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = Track->TrackElementAt(119, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID != -1)
            {
                if(!TrainController->TrainVectorAtIdent(64, TrainID).Stopped())
                {
                    MovingTrainOccupyingRoute = true;  // train is behind the truncate point & moving
                }
            }
            if(Track->IsLCBarrierFlashingAtHV(3, PrefDirVector.at(b).HLoc, PrefDirVector.at(b).VLoc))
            {
                TrainController->StopTTClockMessage(79, "Can't cancel a route containing a level crossing that is changing state");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(1941);
                return;
            }
            if(b == int(TruncatePDElementPos))
            {
                break; // OK found truncate element & no flashing LC in front
            }
        }
    }
    else if(TruncateType == FrontTruncate)//front/full truncate //added at v2.15.0
    {
        for(unsigned int b = 0; b < PrefDirSize(); b++) //search forwards
        {
            int TrainID = Track->TrackElementAt(1556, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnElement;
            if(PrefDirVector.at(b).TrackType == Bridge)
            {
                if(PrefDirVector.at(b).XLinkPos < 2)
                {
                    TrainID = Track->TrackElementAt(1557, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = Track->TrackElementAt(1558, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID != -1)
            {
                if(!TrainController->TrainVectorAtIdent(65, TrainID).Stopped())
                {
                    MovingTrainOccupyingRoute = true;  // train is behind the truncate point & moving
                }
            }
            if(Track->IsLCBarrierFlashingAtHV(4, PrefDirVector.at(b).HLoc, PrefDirVector.at(b).VLoc))
            {
                TrainController->StopTTClockMessage(143, "Can't cancel a route containing a level crossing that is changing state");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(2571);
                return;
            }
            if(b == TruncatePDElementPos)
            {
                break; // OK found truncate element & no flashing LC behind
            }
        }
    }
    else //FullTruncate) //added at v2.15.0
    {
        for(unsigned int b = 0; b < PrefDirSize(); b++) //search the entire route forwards
        {
            int TrainID = Track->TrackElementAt(1559, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnElement;
            if(PrefDirVector.at(b).TrackType == Bridge)
            {
                if(PrefDirVector.at(b).XLinkPos < 2)
                {
                    TrainID = Track->TrackElementAt(1560, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = Track->TrackElementAt(1561, PrefDirVector.at(b).TrackVectorPosition).TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID != -1)
            {
                if(!TrainController->TrainVectorAtIdent(66, TrainID).Stopped())
                {
                    MovingTrainOccupyingRoute = true;  // train is behind the truncate point & moving
                }
            }
            if(Track->IsLCBarrierFlashingAtHV(5, PrefDirVector.at(b).HLoc, PrefDirVector.at(b).VLoc))
            {
                TrainController->StopTTClockMessage(144, "Can't cancel a route containing a level crossing that is changing state");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(2572);
                return;
            }
        }
    }

    if(PrefDirVector.at(TruncatePDElementPos).TrackType == Bridge)
    {
        TrainController->StopTTClockMessage(57, "Can't select a bridge as a route truncation point");
        ReturnFlag = InRouteFalse;
        AllRoutes->RouteBackTruncateFlag = false;
        Utilities->CallLogPop(338);
        return;
    }
    if(((TruncatePDElementPos == 1) && (TruncateType == BackTruncate)) || ((TruncatePDElementPos == (PrefDirSize() - 2)) && (TruncateType == FrontTruncate)))
    {
        TrainController->StopTTClockMessage(58, "Can't truncate to a single route element");
        ReturnFlag = InRouteFalse;
        AllRoutes->RouteBackTruncateFlag = false;
        Utilities->CallLogPop(339);
        return;
    }

    if((TruncatePDElementPos > 0) && (TruncateType == BackTruncate))
    {
        TPrefDirElement TempElement = PrefDirVector.at(TruncatePDElementPos - 1);
        if(TempElement.PrefDirRoute || TempElement.AutoSignals)
        {
            if(TempElement.Config[TempElement.XLinkPos] != Signal)
            {
                TrainController->StopTTClockMessage(145, "Invalid green or blue route truncation position:\n\nto truncate from the start of the route select a position immediately before a facing signal "
                                                          "that lies within the route;\n\nto truncate to the end of the route select a position immediately after a facing signal "
                                                          "that lies within the route;\n\nor to remove the whole route select the first track element in the route");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(340);
                return;
            }
        }
        else
        {
            if((TempElement.TrackType == Points) || (TempElement.TrackType == Crossover) || (TempElement.TrackType == Bridge))
            {
                TrainController->StopTTClockMessage(60, "Can't leave points, bridge or crossover as the last route element");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(341);
                return;
            }
        }
    }

    else if((TruncatePDElementPos < (PrefDirSize() - 1)) && (TruncateType == FrontTruncate))
    {
        TPrefDirElement TempElement = PrefDirVector.at(TruncatePDElementPos + 1);
        if(TempElement.PrefDirRoute || TempElement.AutoSignals)
        {
            if(TempElement.Config[TempElement.XLinkPos] != Signal)
            {
                TrainController->StopTTClockMessage(146, "Invalid green or blue route truncation position:\n\nto truncate from the start of the route select a position immediately before a facing signal "
                                                          "that lies within the route;\n\nto truncate to the end of the route select a position immediately after a facing signal "
                                                          "that lies within the route;\n\nor to remove the whole route select the first track element in the route");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(2573);
                return;
            }
        }
        else //red route
        {
            if(TruncatePDElementPos > 0)
            {
                TPrefDirElement TempElement = PrefDirVector.at(TruncatePDElementPos - 1); //element behind truncation point
                if((TempElement.TrackType == Points) || (TempElement.TrackType == Crossover) || (TempElement.TrackType == Bridge))
                {
                    TrainController->StopTTClockMessage(147, "Can't leave points, bridge or crossover as the last route element");
                    ReturnFlag = InRouteFalse;
                    AllRoutes->RouteBackTruncateFlag = false;
                    Utilities->CallLogPop(2574);
                    return;
                }
            }
        }
    }

    else if(TruncatePDElementPos == 0) //full truncate
    {
        TPrefDirElement TempElement = PrefDirVector.at(TruncatePDElementPos);
        if(TempElement.AutoSignals) //PrefDir route ok as can start after a signal that is in a blue route
        {
            if((TempElement.Config[TempElement.XLinkPos] != Signal) && (TempElement.Config[TempElement.ELinkPos] != End))
            {
                TrainController->StopTTClockMessage(148, "Invalid green or blue route truncation position:\n\nto truncate from the start of the route select a position immediately before a facing signal "
                                                          "that lies within the route;\n\nto truncate to the end of the route select a position immediately after a facing signal "
                                                          "that lies within the route;\n\nor to remove the whole route select the first track element in the route");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(2575);
                return;
            }
        }
        else if(TempElement.PrefDirRoute)  //red routes always ok for full truncate as can't leave a bridge/crossover/points as last element in route
        {
            if(TempElement.TrackType == Bridge)
            {
                TrainController->StopTTClockMessage(149, "Can't select a bridge as a route truncation point"); //should have been caught above but include for completeness
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(2576);
                return;
            }
        }
    }

    int ThisRouteNumber;
    AllRoutes->GetRouteTypeAndNumber(3, GetFixedPrefDirElementAt(84, 0).TrackVectorPosition, GetFixedPrefDirElementAt(85, 0).XLinkPos, ThisRouteNumber);
// Have to call RouteLockingRequired before SetAllRearwardsSignals because RouteLockingRequired tests the first rearward signal, if it is
// red then locking is not required, and if call SetAllRearwardsSignals first then it will set the first rearward signal to red.

// check if part of this route already locked & disallow if so
    if(!(AllRoutes->LockedRouteVector.empty()))
    {
        for(TAllRoutes::TLockedRouteVectorIterator LRVIT = AllRoutes->LockedRouteVector.begin(); LRVIT < AllRoutes->LockedRouteVector.end(); LRVIT++)
        {
            if(LRVIT->RouteNumber == ThisRouteNumber)
            {
                TrainController->StopTTClockMessage(61, "Can't truncate a route that contains a locked section");
                ReturnFlag = InRouteFalse;
                AllRoutes->RouteBackTruncateFlag = false;
                Utilities->CallLogPop(749);
                return;
            }
        }
    }

    unsigned int LookBackwardsFromHere = 0; //added at v2.15.0 - covers front & full truncate & search starts here and looks backwards to see if a train
    if(TruncateType == BackTruncate)                             //is within 3 running signals on this or linked rearwards routes -m this is a PDElement position
    {
        LookBackwardsFromHere = TruncatePDElementPos;
    }

// RouteLockingRequired should be ok with the above mod but need to lock either forwards or backwards
    int RearPosition;    //these are the PDElement positions for the route section to be removed and/or locked (inclusive)
    int FrontPosition;

    if(AllRoutes->RouteLockingRequired(0, ThisRouteNumber, LookBackwardsFromHere) || MovingTrainOccupyingRoute) // added MovingTrainOccupyingRoute  at v2.1.0,
    // RouteLockingRequired only checks for trains approaching
    {
        TrainController->StopTTClockFlag = true;
        TrainController->RestartTime = TrainController->TTClockTime;
        int button = Application->MessageBox(L"Moving train approaching or occupying route, YES to lock route (2 minutes to release), NO to cancel",
                                             L"Warning!", MB_YESNO | MB_ICONWARNING);
        TrainController->BaseTime = TDateTime::CurrentDateTime();
        TrainController->StopTTClockFlag = false;
        if(button == IDNO)
        {
            ReturnFlag = InRouteTrue; // still return true even though don't act on it
            AllRoutes->RouteBackTruncateFlag = false;
            Utilities->CallLogPop(342);
            return;
        }
        AnsiString LocID = AnsiString(Track->TrackElementAt(534, PrefDirVector.at(TruncatePDElementPos).TrackVectorPosition).ElementID);
        TrainController->LogActionError(0, "", "", FailLockedRoute, LocID);
        TAllRoutes::TLockedRouteClass LockedRoute;
        bool ExistingLockedRouteModified = false;
        LockedRoute.RouteNumber = ThisRouteNumber;
        if(TruncateType == BackTruncate)
        {
            LockedRoute.RearTrackVectorPosition = PrefDirVector.at(TruncatePDElementPos).TrackVectorPosition;
            LockedRoute.LastTrackVectorPosition = PrefDirVector.at(PrefDirSize() - 1).TrackVectorPosition;
            LockedRoute.LastXLinkPos = PrefDirVector.at(PrefDirSize() - 1).XLinkPos;
        }
        else if(TruncateType == FrontTruncate)
        {
            LockedRoute.RearTrackVectorPosition = PrefDirVector.at(0).TrackVectorPosition;
            LockedRoute.LastTrackVectorPosition = PrefDirVector.at(TruncatePDElementPos).TrackVectorPosition;
           LockedRoute.LastXLinkPos = PrefDirVector.at(TruncatePDElementPos).XLinkPos;
        }
        else //FullTruncate
        {
            LockedRoute.RearTrackVectorPosition = PrefDirVector.at(0).TrackVectorPosition;
            LockedRoute.LastTrackVectorPosition = PrefDirVector.at(PrefDirSize() - 1).TrackVectorPosition;
            LockedRoute.LastXLinkPos = PrefDirVector.at(PrefDirSize() - 1).XLinkPos;
        }

        LockedRoute.LockStartTime = TrainController->TTClockTime;
// but first check if this route already in LockedRouteVector (i.e. locked further along), and if so just change that vector entry
// to use the new RearTrackVectorPosition & LockStartTime (shouldn't as should have been rejected earlier if part-locked, but leave in)
        if(!AllRoutes->LockedRouteVector.empty())
        {
            for(TAllRoutes::TLockedRouteVectorIterator LRVIT = AllRoutes->LockedRouteVector.begin(); LRVIT < AllRoutes->LockedRouteVector.end();
                LRVIT++)
            {
                if(LRVIT->RouteNumber == ThisRouteNumber)
                {
                    LRVIT->RearTrackVectorPosition = LockedRoute.RearTrackVectorPosition;
                    LRVIT->LockStartTime = LockedRoute.LockStartTime;
                    ExistingLockedRouteModified = true;
                }
            }
        }
        if(!ExistingLockedRouteModified)
        {
            AllRoutes->LockedRouteVector.push_back(LockedRoute);
        }
        if(TruncateType == BackTruncate)
        {
            AllRoutes->SetAllRearwardsSignals(2, 0, ThisRouteNumber, TruncatePDElementPos);
            RearPosition = TruncatePDElementPos;
            FrontPosition = PrefDirSize() - 1;
        }
        else if(TruncateType == FrontTruncate)
        {
            AllRoutes->SetAllRearwardsSignals(13, 0, ThisRouteNumber, 0);
            RearPosition = 0;
            FrontPosition = TruncatePDElementPos;
        }
        else //FullTruncate
        {
            AllRoutes->SetAllRearwardsSignals(14, 0, ThisRouteNumber, 0);
            RearPosition = 0;
            FrontPosition = PrefDirSize() - 1;
        }
//        for(int c = PrefDirSize() - 1; c >= (int)TruncatePDElementPos; c--) // must use int for >= test to succeed when b == 0
        for(int c = FrontPosition; c >= RearPosition; c--)
        {
            // return all signals to red in route section to be truncated
            TPrefDirElement PrefDirElement = AllRoutes->GetFixedRouteAt(61, ThisRouteNumber).PrefDirVector.at(c);
            TTrackElement& TrackElement = Track->TrackElementAt(120, PrefDirElement.TrackVectorPosition);
            if(PrefDirElement.Config[PrefDirElement.XLinkPos] == Signal)
            {
                TrackElement.Attribute = 0;
                Track->PlotSignal(2, TrackElement, Display);
                Display->PlotOutput(18, PrefDirElement.HLoc * 16, PrefDirElement.VLoc * 16, PrefDirElement.EXGraphicPtr);
                Display->PlotOutput(19, PrefDirElement.HLoc * 16, PrefDirElement.VLoc * 16, PrefDirElement.EntryDirectionGraphicPtr);
            }
        }
// Display->Update();//not needed as Clearand... called on return from SearchAllRoutesAndTruncate in InterfaceUnit
        ReturnFlag = InRouteTrue;
    }
    else //route locking not required
    {
        if(TruncateType == BackTruncate)
        {
            RearPosition = TruncatePDElementPos;
            FrontPosition = PrefDirSize() - 1;
            AllRoutes->SetAllRearwardsSignals(21, 0, ThisRouteNumber, TruncatePDElementPos);
        }
        else if(TruncateType == FrontTruncate)
        {
            RearPosition = 0;
            FrontPosition = TruncatePDElementPos;
            AllRoutes->SetAllRearwardsSignals(15, 0, ThisRouteNumber, 0);
        }
        else
        {
            RearPosition = 0;
            FrontPosition = PrefDirSize() - 1;
            AllRoutes->SetAllRearwardsSignals(16, 0, ThisRouteNumber, 0);
        }

        //now (before truncate route) store the front and rear PrefDir elements of the route for later adaptation (will only be used for blue routes)
        //into adjacent red or green routes if there are any (after the truncation/removal)

        TPrefDirElement LastPDElement = GetFixedPrefDirElementAt(260, PrefDirSize() - 1); //these will persist after the actual PDElements have been removed
        TPrefDirElement FirstPDElement = GetFixedPrefDirElementAt(261, 0);
//        bool AddRedAtEnd = false, AddRedAtStart = false, AddGreenAtEnd = false, AddGreenAtStart = false;

         for(int c = FrontPosition; c >= RearPosition; c--) //truncate or remove the route
        {
            AllRoutes->RemoveRouteElement(5, PrefDirVector.at(c).HLoc, PrefDirVector.at(c).VLoc, PrefDirVector.at(c).ELink);
            ReturnFlag = InRouteTrue;
        }

        if(LastPDElement.AutoSignals)
        {
            ReclaimSignalsForNonAutoSigRoutes(0, LastPDElement, FirstPDElement);
        }
    }

    AllRoutes->CheckMapAndRoutes(5); // test
    ReturnFlag = InRouteTrue;
    AllRoutes->RouteBackTruncateFlag = false;
    Utilities->CallLogPop(344);
}

// ---------------------------------------------------------------------------

void TOneRoute::ReclaimSignalsForNonAutoSigRoutes(int Caller, TPrefDirElement LastPDElement, TPrefDirElement FirstPDElement)
{
/*
Need to ensure that green routes always end on a signal (or continuation, but continuations aren't relevant for these purposes) as they can't be extended from other than a
signal.
For green/red routes, a new route that is created FROM a signal - that signal stays in the original route that ended there,
                  but a new route that is created TO a signal - that signal becomes part of the new route.
For blue routes, a new route created FROM and/or TO a signal - that signal becomes part of the blue route.
So, truncating a blue route from the front leaves a green/red route ending short of the signal - add green/red signal to route at end of route
    truncating a blue route from the back leaves a green/red route starting behind the signal - add green/red signal to route at start of route
    truncating a red route from the front (in advance of a facing red route signal) ok as the signal is still in the (single element) red route and still allows trains to pass
    truncating a red route from the back (anywhere in the red route except as above) ok as at worst (and done deliberately) the red route stops short of a signal but can be extended to it
    truncating a green route from the front (in advance of a facing green route signal) ok as the signal is still in the (single element) green route and still allows trains to pass
    truncating a green route from the back (in rear of a green route signal) ok as signal still in green route
*/

    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ", LastPDElement TrackID " +
        Track->TrackElementAt(1562, LastPDElement.TrackVectorPosition).ElementID + ", FirstPDElement TrackID " +
        Track->TrackElementAt(1563, FirstPDElement.TrackVectorPosition).ElementID);
    if(!LastPDElement.AutoSignals) //shouldn't be called other than for a blue route
    {
        Utilities->CallLogPop(2578);
        return;
    }
    TPrefDirElement NewGreenFirstPDElement, NewRedFirstPDElement, NewGreenLastPDElement, NewRedLastPDElement;
    int RouteColour;
    //check if there's a linked forward route missing a signal and if so add it (will only apply for blue routes)
    for(unsigned int x = 0; x < AllRoutes->AllRoutesVector.size(); x++)
    {
        TPrefDirElement FirstForwardLinkedPDElement = AllRoutes->AllRoutesVector.at(x).PrefDirVector.at(0);
        if(FirstForwardLinkedPDElement.Conn[FirstForwardLinkedPDElement.ELinkPos] == LastPDElement.TrackVectorPosition)
        {   //found a linked forward route
            //check if signal behind this route has been removed from the blue route
            if(!AllRoutes->TrackIsInARoute(19, LastPDElement.TrackVectorPosition, 0)) //use 0 for LinkPos as it is a signal so only LinkPos 0 & 1 in use
            {   //signal needs to be added at start of this linked route

                RouteColour = FirstForwardLinkedPDElement.GetRouteColour(FirstForwardLinkedPDElement.EXGraphicPtr);
                if(RouteColour == 1) //red route
                {
                    NewRedFirstPDElement = LastPDElement;
                    NewRedFirstPDElement.AutoSignals = false;
                    NewRedFirstPDElement.PrefDirRoute = false;
                    NewRedFirstPDElement.EXGraphicPtr = NewRedFirstPDElement.GetRouteGraphicPtr(0, 0); //not autosigs & not prefdir
                    NewRedFirstPDElement.IsARoute = true;
                    TOneRoute &RouteToBeExtended = AllRoutes->GetModifiableRouteAt(31, x);
                    RouteToBeExtended.PrefDirVector.insert(RouteToBeExtended.PrefDirVector.begin(), 1, NewRedFirstPDElement); //insert at front
                    TAllRoutes::TRoute2MultiMapIterator R2MMIt; //first increment all PrefDirVector numbers as new entry going at start
                    for(R2MMIt = AllRoutes->Route2MultiMap.begin(); R2MMIt != AllRoutes->Route2MultiMap.end(); R2MMIt++)
                    {
                        if(R2MMIt->second.first == int(x))
                        {
                            R2MMIt->second.second++;
                        }
                    }
                    AllRoutes->Route2MultiMapInsert(1, NewRedFirstPDElement.HLoc, NewRedFirstPDElement.VLoc, NewRedFirstPDElement.ELink, x, 0); //0 is new first element number
                    AllRoutes->RouteBackTruncateFlag = false; //no longer truncating at this point
                    AllRoutes->SetAllRearwardsSignals(17, 0, x, AllRoutes->GetFixedRouteAt(225, x).PrefDirVector.size() - 1); //should be red but make sure
                }
                else if(RouteColour == 2) //green route
                {
                    NewGreenFirstPDElement = LastPDElement;
                    NewGreenFirstPDElement.AutoSignals = false;
                    NewGreenFirstPDElement.PrefDirRoute = true;
                    NewGreenFirstPDElement.EXGraphicPtr = NewGreenFirstPDElement.GetRouteGraphicPtr(0, 1); //not autosigs & prefdir
                    NewGreenFirstPDElement.IsARoute = true;
                    TOneRoute &RouteToBeExtended = AllRoutes->GetModifiableRouteAt(32, x);
                    RouteToBeExtended.PrefDirVector.insert(RouteToBeExtended.PrefDirVector.begin(), 1, NewGreenFirstPDElement); //insert at front
                    TAllRoutes::TRoute2MultiMapIterator R2MMIt; //first increment all PrefDirVector numbers as new entry going at start
                    for(R2MMIt = AllRoutes->Route2MultiMap.begin(); R2MMIt != AllRoutes->Route2MultiMap.end(); R2MMIt++)
                    {
                        if(R2MMIt->second.first == int(x))
                        {
                            R2MMIt->second.second++;
                        }
                    }
                    AllRoutes->Route2MultiMapInsert(2, NewGreenFirstPDElement.HLoc, NewGreenFirstPDElement.VLoc, NewGreenFirstPDElement.ELink, x, 0); //0 is new first element number
                    AllRoutes->RouteBackTruncateFlag = false; //no longer truncating at this point
                    AllRoutes->SetAllRearwardsSignals(18, 0, x, AllRoutes->GetFixedRouteAt(226, x).PrefDirVector.size() - 1); //should be red but make sure
                }
            }
            break; //no point looking any further
        }
    }

//check if there's a linked rearward route missing a signal and if so add it
    for(unsigned int x = 0; x < AllRoutes->AllRoutesVector.size(); x++)
    {
        TPrefDirElement LastRearwardLinkedPDElement = AllRoutes->AllRoutesVector.at(x).PrefDirVector.at(AllRoutes->AllRoutesVector.at(x).PrefDirVector.size() - 1);
        if(LastRearwardLinkedPDElement.Conn[LastRearwardLinkedPDElement.XLinkPos] == FirstPDElement.TrackVectorPosition)
        {   //found a linked rearward route
            //check if signal in front of this route has been removed from the blue route
            if(!AllRoutes->TrackIsInARoute(20, FirstPDElement.TrackVectorPosition, 0)) //use 0 for LinkPos as it is a signal so only LinkPos 0 & 1 in use
            {   //signal needs to be added at end of this linked route

                RouteColour = LastRearwardLinkedPDElement.GetRouteColour(LastRearwardLinkedPDElement.EXGraphicPtr);
                if(RouteColour == 1) //red route
                {
                    NewRedLastPDElement = FirstPDElement;
                    NewRedLastPDElement.AutoSignals = false;
                    NewRedLastPDElement.PrefDirRoute = false;
                    NewRedLastPDElement.EXGraphicPtr = NewRedLastPDElement.GetRouteGraphicPtr(0, 0); //not autosigs &  not prefdir
                    AllRoutes->AddRouteElement(4, NewRedLastPDElement.HLoc, NewRedLastPDElement.VLoc, NewRedLastPDElement.ELink, x, NewRedLastPDElement);
                    //can use this as adding to the end of the route
                    AllRoutes->RouteBackTruncateFlag = false; //no longer truncating at this point
                    AllRoutes->SetAllRearwardsSignals(19, 0, x, AllRoutes->GetFixedRouteAt(223, x).PrefDirVector.size() - 1); //should be red but make sure
                }
                else if(RouteColour == 2) //green route
                {
                    NewGreenLastPDElement = FirstPDElement;
                    NewGreenLastPDElement.AutoSignals = false;
                    NewGreenLastPDElement.PrefDirRoute = true;
                    NewGreenLastPDElement.EXGraphicPtr = NewGreenLastPDElement.GetRouteGraphicPtr(0, 1); //not autosigs & prefdir
                    AllRoutes->AddRouteElement(5, NewGreenLastPDElement.HLoc, NewGreenLastPDElement.VLoc, NewGreenLastPDElement.ELink, x, NewGreenLastPDElement);
                    //can use this as adding to the end of the route
                    AllRoutes->RouteBackTruncateFlag = false; //no longer truncating at this point
                    AllRoutes->SetAllRearwardsSignals(20, 0, x, AllRoutes->GetFixedRouteAt(224, x).PrefDirVector.size() - 1); //should be red but make sure
                }
            }
            break; //no point looking any further
        }
    }
    Utilities->CallLogPop(2579);
}

// ---------------------------------------------------------------------------

void TOneRoute::ForceCancelRoute(int Caller)
/*
      This is used when a train enters a route set in the opposite direction of travel (or at a crossover on a non-route line when the other
      track is in a route).  The complete route is cancelled (but not linked routes), and all signals in the route are set to red.
      First all signals are set to red and replotted (without any route colours), then SetAllRearwardsSignals is called from the
      beginning of the route to set all linked rearwards route signals appropriately.  Then all elements are removed from the route
      and RebuildRailwayFlag set (examined in Interface unit at each clock tick) to force a ClearandRebuildRailway to get rid of
      the route colours.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ForceCancelRoute");
    TrainController->StopTTClockFlag = true; // so TTClock stopped during MasterClockTimer function
    TrainController->RestartTime = TrainController->TTClockTime;
    int RouteNumber;
    TAllRoutes::TRouteType RouteType = AllRoutes->GetRouteTypeAndNumber(4, GetFixedPrefDirElementAt(86, 0).TrackVectorPosition,
                                                                        GetFixedPrefDirElementAt(87, 0).XLinkPos, RouteNumber);
    //these added at v2.15.0
    TPrefDirElement LastPDElement = GetFixedPrefDirElementAt(263, PrefDirSize() - 1); //these will persist after the actual PDElements have been removed
    TPrefDirElement FirstPDElement = GetFixedPrefDirElementAt(264, 0);

    if(RouteType != TAllRoutes::NoRoute) // it won't be, above only used to get RouteNumber for setting rearwards signals
    {
        for(unsigned int x = 0; x < PrefDirSize(); x++) // set all signals in route to red regardless of direction
        {
            if(PrefDirVector.at(x).TrackType == SignalPost)
            {
                Track->TrackElementAt(121, PrefDirVector.at(x).TrackVectorPosition).Attribute = 0; // red
                Track->PlotSignal(3, Track->TrackElementAt(122, PrefDirVector.at(x).TrackVectorPosition), Display);
            }
        }
        AllRoutes->SetAllRearwardsSignals(4, 0, RouteNumber, 0);
// already set all signals to red in route so start at start of route for further rearwards signal setting
    }
    for(int c = PrefDirSize() - 1; c >= 0; c--) // must use int for >= test to succeed when b == 0
    {
        AllRoutes->RemoveRouteElement(6, LastElementPtr(6)->HLoc, LastElementPtr(7)->VLoc, LastElementPtr(8)->ELink);
    }

    if(LastPDElement.AutoSignals) //added at v2.15.0
    {
        ReclaimSignalsForNonAutoSigRoutes(1, LastPDElement, FirstPDElement);
    }

    AllRoutes->RebuildRailwayFlag = true; // set to force a ClearandRebuildRailway at next clock tick if not in zoom-out mode
    AllRoutes->CheckMapAndRoutes(9); // test
    TrainController->BaseTime = TDateTime::CurrentDateTime();
    TrainController->StopTTClockFlag = false;
    Utilities->CallLogPop(345);
    return;
}

// ---------------------------------------------------------------------------

void TOneRoute::SetRouteSearchVectorGraphics(int Caller, bool AutoSigsFlag, bool PrefDirRoute)
/*
      Set values for EXGraphicPtr and EntryDirectionGraphicPtr for all elements in SearchVector.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetRouteSearchVectorGraphics," + AnsiString((short)AutoSigsFlag) + "," +
                                 AnsiString((short)PrefDirRoute));
    if(SearchVector.empty())
    {
        Utilities->CallLogPop(1149);
        return;
    }
    for(unsigned int b = 0; b < SearchVector.size(); b++)
    {
        GetModifiableSearchElementAt(1, b).EXGraphicPtr = GetModifiableSearchElementAt(2, b).GetRouteGraphicPtr(AutoSigsFlag, PrefDirRoute);
        GetModifiableSearchElementAt(3, b).EntryDirectionGraphicPtr = GetModifiableSearchElementAt(4, b).GetDirectionRouteGraphicPtr(AutoSigsFlag,
                                                                                                                                     PrefDirRoute);
    }
    Utilities->CallLogPop(346);
}

// ---------------------------------------------------------------------------

void TOneRoute::SetRouteFlashValues(int Caller, bool AutoSigsFlag, bool PrefDirRoute)
/*
      Sets all element values in the RouteFlashVector (member of class TRouteFlash - defined in TOneRoute, of which
      TOneRoute has one member called RouteFlash) from the SearchVector.  TRouteFlashElement is also a class defined in
      TOneRoute.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetRouteFlashValues," + AnsiString((short)AutoSigsFlag) + "," +
                                 AnsiString((short)PrefDirRoute));
    RouteFlash.RouteFlashVector.clear();
    TRouteFlashElement RouteFlashElement;

    for(unsigned int b = 0; b < SearchVector.size(); b++)
    {
        int H = GetFixedSearchElementAt(11, b).HLoc;
        int V = GetFixedSearchElementAt(12, b).VLoc;
        RouteFlashElement.OriginalGraphic = GetModifiableSearchElementAt(5, b).GetOriginalGraphicPtr();
        RouteFlashElement.OverlayGraphic = GetModifiableSearchElementAt(6, b).GetRouteGraphicPtr(AutoSigsFlag, PrefDirRoute);
        RouteFlashElement.HLoc = H;
        RouteFlashElement.VLoc = V;
        RouteFlashElement.TrackVectorPosition = GetFixedSearchElementAt(13, b).TrackVectorPosition;
        RouteFlash.RouteFlashVector.push_back(RouteFlashElement);
    }
    Utilities->CallLogPop(348);
}

// ---------------------------------------------------------------------------

void TOneRoute::SetLCChangeValues(int Caller, bool PrefDirRoute)   //used when setting routes to start any included LC's lowering
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetLCChangeValues," + AnsiString((short)PrefDirRoute));
    if(!PrefDirVector.empty())
    {
        for(TPrefDirVectorConstIterator PrefDirPtr = (PrefDirVector.end() - 1); PrefDirPtr >= PrefDirVector.begin(); PrefDirPtr--)
        {
            int H = PrefDirPtr->HLoc;
            int V = PrefDirPtr->VLoc;
            // check for any LCs that are closed to trains & set the flash values and store in the vector
            if(Track->IsLCAtHV(39, H, V))
            {
                if(Track->IsLCBarrierUpAtHV(0, H, V))
                {
                    Track->LCChangeFlag = true;
                    TTrack::TActiveLevelCrossing CLC; // constructor sets ReducedTimePenalty to false
                    CLC.HLoc = H;
                    CLC.VLoc = V;
                    CLC.StartTime = TrainController->TTClockTime;
                    CLC.BaseElementSpeedTag = PrefDirPtr->SpeedTag;
                    CLC.ChangeDuration = Track->LevelCrossingBarrierDownFlashDuration;
                    CLC.BarrierState = TTrack::Lowering;
                    if(PrefDirRoute)
                    {
                        CLC.TypeOfRoute = 1;
                    }
                    Track->SetLinkedLevelCrossingBarrierAttributes(1, H, V, 2); // set attr to 2 for changing state
                    Track->ChangingLCVector.push_back(CLC);
                }
            }
        }
    }
    Utilities->CallLogPop(1948);
}

// ---------------------------------------------------------------------------

void TOneRoute::TRouteFlash::PlotRouteOverlay(int Caller)
/*
      Class TRouteFlash is defined in TOneRoute, which has one member called RouteFlash.  This function
      checks first whether the OverlayPlotted flag is set and if not plots the OverlayGraphic for all
      elements in the RouteFlashVector, skipping any that a train is on.  Finally the OverlayPlotted flag
      is set.  The OverlayGraphic is set during TOneRoute::SetRouteAndLCChangeValues().
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TRouteFlash::PlotRouteOverlay");
    if(!OverlayPlotted)
    {
        for(unsigned int x = 0; x < RouteFlashVector.size(); x++)
        {
            if(Track->TrackElementAt(123, RouteFlashVector.at(x).TrackVectorPosition).TrainIDOnElement > -1)
            {
                continue;
            }
            Display->PlotOutput(20, RouteFlashVector.at(x).HLoc * 16, RouteFlashVector.at(x).VLoc * 16, RouteFlashVector.at(x).OverlayGraphic);
            Display->Update();
        }
        OverlayPlotted = true;
    }
    Utilities->CallLogPop(349);
}

// ---------------------------------------------------------------------------

void TOneRoute::TRouteFlash::PlotRouteOriginal(int Caller)
/*
      Class TRouteFlash is defined in TOneRoute, which has one member called RouteFlash.  This function
      checks first whether the OverlayPlotted flag is set and if so plots the OriginalGraphic for all
      elements in the RouteFlashVector, skipping any that a train is on.  Finally the OverlayPlotted flag
      is reset.  The OriginalGraphic is set during TOneRoute::SetRouteAndLCChangeValues().
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TRouteFlash::PlotRouteOriginal");
    if(OverlayPlotted)
    {
        for(unsigned int x = 0; x < RouteFlashVector.size(); x++)
        {
            if(Track->TrackElementAt(124, RouteFlashVector.at(x).TrackVectorPosition).TrainIDOnElement > -1)
            {
                continue;
            }
            Display->PlotOutput(21, RouteFlashVector.at(x).HLoc * 16, RouteFlashVector.at(x).VLoc * 16, RouteFlashVector.at(x).OriginalGraphic);
            Display->Update();
        }
        OverlayPlotted = false;
    }
    Utilities->CallLogPop(350);
}

// ---------------------------------------------------------------------------

bool TOneRoute::SignalHasFailed(int Caller) //added at v2.13.0
{
/*enter with SearchVector fully populated & with a legitimate route found, return true for failure.
Look along SearchVector backwards, skip first signal found (i.e. last in route), but for all others
including first signal in route offer chance to fail (since they all change aspect), but if find any failed point
where no route available (i.e. a dead end, points checked after search in PointsToBeChanged) then return false - i.e. don't
allow signal failure in an unviable route. If fail (i.e. prior to returning true), alter graphic, send
messages, and allocate a repair time similar to points)
*/

    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SignalHasFailed");
    if((Utilities->FailureMode == FNil) || (SearchVector.size() < 2)) //added (SearchVector.size() < 2) at v2.14.0 as rely on it being at least 2 below
    {
        Utilities->CallLogPop(2528);
        return(false);
    }
    bool FirstSignalFound = false;
    for(TPrefDirVector::iterator PDVIt = SearchVector.end() - 1; PDVIt >= SearchVector.begin(); PDVIt--)
    {
        TTrackElement &TE = Track->TrackElementAt(1530, PDVIt->TrackVectorPosition);
//check for a failed point where needs to change to make the route
//shouldn't be any but check to be safe
        int Attr = TE.Attribute;
        if(PDVIt->TrackType == Points)
        {
            if((PDVIt->ELinkPos == 1) || (PDVIt->XLinkPos == 1)) // 1=want to go straight
            {
                if(Attr == 1) //currently set to diverge
                {
                    if(TE.Failed)
                    {
                        Utilities->CallLogPop(2529);
                        return(false); //return without further checking
                    }
                }
            }
            else if((PDVIt->ELinkPos == 3) || (PDVIt->XLinkPos == 3)) // 3=want to diverge
            {
                if(Attr == 0) //currently set to go straight
                {
                    if(TE.Failed)
                    {
                        Utilities->CallLogPop(2530);
                        return(false); //return without further checking
                    }
                }
            }
        }
        //now need to check if PDVIt->XLinkPos is set (> -1) as if an unrestricted route and start on a signal not in an existing route then XLinkPos won't be set
        //changed at v2.14.0 to ensure both prefdir & unrestricted routes can have first signal fail (by determining what XLinkPos should be and using it, but not changing
        //the search vector)
        int XLinkPosition = PDVIt->XLinkPos;
        if(PDVIt->XLinkPos == -1)
        {
            if(PDVIt < (SearchVector.end() - 1)) //no good if end element as need to examine the later one, though shouldn't have XLinkPos unset if so
            {
                for(int x = 0; x < 4; x++)
                {
                    if(PDVIt->Conn[x] == (PDVIt + 1)->TrackVectorPosition)
                    {
                        XLinkPosition = x;
                        break;
                    }
                }
            }
            else
            {
                Utilities->CallLogPop(2549);
                return(false); //no point going any further
            }
        }
        if(XLinkPosition > -1) //should be by now but be safe
        {
            if(!FirstSignalFound && (PDVIt->Config[XLinkPosition] == Signal))
            {
                FirstSignalFound = true; //the first signal doesn't change aspect
                continue;
            }
            else if(FirstSignalFound && (PDVIt->Config[XLinkPosition] == Signal))
            {
/*
                if(TE.SigAspect == TTrackElement::GroundSignal)    //dropped at v2.14.0 to allow ground signals to fail
                {
                    continue; //ground signals don't fail
                }
*/
                if((random(Utilities->SignalChangeEventsPerFailure) == 0) && !TE.Failed) //can't fail twice
                {
                    TTrack::TInfrastructureFailureEntry IFE;
                    IFE.TVPos = PDVIt->TrackVectorPosition;
                    TE.Failed = true;
                    TE.Attribute = 0; //stop aspect
                    Display->WarningLog(22, Utilities->Format96HHMMSS(TrainController->TTClockTime) + ": Signal failed at " + TE.ElementID);
                    PerfLogForm->PerformanceLog(45, Utilities->Format96HHMMSS(TrainController->TTClockTime) + " WARNING: Signal failed at " + TE.ElementID);
                    TrainController->StopTTClockMessage(132, "Signal at " + TE.ElementID +
                        " failed when changing aspect.\nTrains can only pass under signaller control.");
                    AllRoutes->RebuildRailwayFlag = true; //force ClearandRebuildRailway at next clock tick
    //set repair time, random value in minutes between 10 and 179
                    double FailureMinutes = double(random(Utilities->MaxRandomRepairTime) + Utilities->FixedMinRepairTime); //between 10 and 179 minutes at random
                    TDateTime RepairTime = TrainController->TTClockTime + TDateTime(FailureMinutes / 1440);
                    IFE.RepairTime = RepairTime;
                    IFE.FailureTime = TrainController->TTClockTime;
                    Track->FailedSignalsVector.push_back(IFE);
                    Track->SuppressRouteFailMessage = true;
                    int RouteNumber; //not used
                    if(AllRoutes->GetRouteTypeAndNumber(41, IFE.TVPos, 0, RouteNumber) != TAllRoutes::NoRoute) //otherwise Attribute already 0 so will plot red
                    {                                                                                   // 0 for LinkPos ok as a signal so only one track
                        AllRoutes->AllRoutesVector.at(RouteNumber).SetRouteSignals(12);
                    }
                    Utilities->CallLogPop(2535);
                    return(true); //return so only allow one failure per route
                }
            }
        }
    }
    Utilities->CallLogPop(2531);
    return(false);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

const TOneRoute &TAllRoutes::GetFixedRouteAt(int Caller, int At) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetFixedRouteAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= AllRoutesVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(AllRoutesVector.size()) + ", At: " + AnsiString(At) + " in GetFixedRouteAt");
    }
    Utilities->CallLogPop(120);
    return(AllRoutesVector.at(At));
}

// ---------------------------------------------------------------------------

TOneRoute &TAllRoutes::GetModifiableRouteAt(int Caller, int At)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetModifiableRouteAt," + AnsiString(At));
    if((At < 0) || ((unsigned int)At >= AllRoutesVector.size()))
    {
        throw Exception("Out of Range Error, vector size: " + AnsiString(AllRoutesVector.size()) + ", At: " + AnsiString(At) + " in GetModifiableRouteAt");
    }
    Utilities->CallLogPop(121);
    return(AllRoutesVector.at(At));
}

// ---------------------------------------------------------------------------

void TAllRoutes::MarkAllRoutes(int Caller, TDisplay *Disp)
/*
      Calls PrefDirMarker to display all routes, with RouteCall set to identify a route call, and BuildingPrefDir false.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",MarkAllRoutes");
    for(unsigned int a = 0; a < AllRoutesSize(); a++)
    {
        GetFixedRouteAt(62, a).PrefDirMarker(7, RouteCall, false, Disp);
    }
    Utilities->CallLogPop(351);
}

// ---------------------------------------------------------------------------

void TAllRoutes::WriteAllRoutesToImage(int Caller, Graphics::TBitmap *Bitmap)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",WriteAllRoutesToImage");
    for(unsigned int a = 0; a < AllRoutesSize(); a++)
    {
        GetFixedRouteAt(166, a).RouteImageMarker(0, Bitmap);
    }
    Utilities->CallLogPop(1706);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::SearchAllRoutesAndTruncate(int Caller, int HLoc, int VLoc, bool PrefDirRoute)
/*
      Examines all routes and for each uses TruncateRoute to see if the element at H & V is present in
      that route.  The ReturnFlag value indicates InRouteTrue (success), InRouteFalse (failure), or NotInRoute.
      Messages are given in TruncateRoute.  If successful the route is truncated at and including
      the element that matches H & V.  If PrefDirRoute ensure only truncate to a signal, else prevent
      truncation to a crossover, bridge or points, also prevent route being left less than 2 elements in
      length (train length).
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SearchAllRoutesAndTruncate," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString((short)PrefDirRoute));
    for(unsigned int a = 0; a < AllRoutesSize(); a++)
    {
        TTruncateReturnType ReturnFlag;
// used in SetRearwardsSignalsReturnFalseForTrain (called by TruncateRoute) to skip continuation & buffer attribute change
        GetModifiableRouteAt(7, a).TruncateRoute(0, HLoc, VLoc, PrefDirRoute, ReturnFlag);
        if(ReturnFlag == NotInRoute)
        {
            continue;
        }
        else if(ReturnFlag == InRouteTrue)
        {
            Utilities->CallLogPop(352);
            return(true);
        }
        else if(ReturnFlag == InRouteFalse)
        {
            Utilities->CallLogPop(353);
            return(false);
        }
    }
    Utilities->CallLogPop(354);
    return(false);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::TrackIsInARoute(int Caller, int TrackVectorPosition, int LinkPos)
/*
      Examines Route2MultiMap and if the element at TrackVectorPosition with LinkPos (can be entry or exit)
      is found it returns true (for crossovers & points returns true whichever track the route is on), else returns false.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",TrackIsInARoute," + AnsiString(TrackVectorPosition) + "," +
                                 AnsiString(LinkPos));
    if(TrackVectorPosition == -1) // allows for continuation entries & exits
    {
        Utilities->CallLogPop(355);
        return(false);
    }
    THVPair Route2MultiMapKeyPair;

    Route2MultiMapKeyPair.first = Track->TrackElementAt(125, TrackVectorPosition).HLoc;
    Route2MultiMapKeyPair.second = Track->TrackElementAt(126, TrackVectorPosition).VLoc;
    int EntryLink, EntryLinkPos, ExitLink, ExitLinkPos;
    TRoute2MultiMapIterator Route2MultiMapIterator;

    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 0) // none found
    {
        Utilities->CallLogPop(356);
        return(false);
    }
    if(Track->TrackElementAt(706, TrackVectorPosition).TrackType != Bridge) // if not a bridge doesn't matter which track the route is on
    {
        Utilities->CallLogPop(1422);
        return(true);
    }
    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 1) //remainder for a bridge element
    {
        Route2MultiMapIterator = Route2MultiMap.find(Route2MultiMapKeyPair);
// check actual link rather than just LinkPos, since if points LinkPos could be 0 or 2 so may not match ELinkPos or XLinkPos, but both refer to same link
// realised after writing this that can't be points as would have been covered above, but leave anyway
        const TPrefDirElement &PrefDirElement1 = GetFixedRouteAt(64, Route2MultiMapIterator->second.first).GetFixedPrefDirElementAt(88,
                                                                                                                                    Route2MultiMapIterator->second.second);
        EntryLinkPos = PrefDirElement1.ELinkPos;
        ExitLinkPos = PrefDirElement1.XLinkPos;
        EntryLink = PrefDirElement1.Link[EntryLinkPos];
        ExitLink = PrefDirElement1.Link[ExitLinkPos];
        if(EntryLink == Track->TrackElementAt(127, TrackVectorPosition).Link[LinkPos])
        {
            Utilities->CallLogPop(357);
            return(true);
        }
        if(ExitLink == Track->TrackElementAt(128, TrackVectorPosition).Link[LinkPos])
        {
            Utilities->CallLogPop(358);
            return(true);
        }
    }
    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 2) // if both tracks in route then must return true
    {
        Utilities->CallLogPop(1423);
        return(true);
    }
    Utilities->CallLogPop(363);
    return(false); // none found
}

// ---------------------------------------------------------------------------

TAllRoutes::TRouteType TAllRoutes::GetRouteTypeAndGraphics(int Caller, int TrackVectorPosition, int LinkPos, Graphics::TBitmap* &EXGraphicPtr,
                                                           Graphics::TBitmap* &EntryDirectionGraphicPtr)
/*
      Examines Route2MultiMap and if finds the element at TrackVectorPosition with LinkPos (can be entry or exit) returns the appropriate route
      type - NoRoute, NotAutoSigsRoute, or AutoSigsRoute.  If element not found then NoRoute is returned.  If element is in a route then the EXGraphicPtr
      is returned, and if either the start or end of a route then the correct EntryDirectionGraphicPtr is returned, else a transparent element is returned.
      Function is used in TrainUnit for retaining AutoSigsRoutes but erasing others after train passes, and for picking up the correct background graphics
      for replotting of AutoSigsRoutes.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetRouteTypeAndGraphics," + AnsiString(TrackVectorPosition) + "," +
                                 AnsiString(LinkPos));
    EXGraphicPtr = RailGraphics->bmTransparentBgnd; // default value
    EntryDirectionGraphicPtr = RailGraphics->bmTransparentBgnd; // default value
    if(TrackVectorPosition == -1)
    {
        Utilities->CallLogPop(364);
        return(NoRoute); // allows for continuation entries & exits
    }
    THVPair Route2MultiMapKeyPair;

    Route2MultiMapKeyPair.first = Track->TrackElementAt(133, TrackVectorPosition).HLoc;
    Route2MultiMapKeyPair.second = Track->TrackElementAt(134, TrackVectorPosition).VLoc;
    int EntryLink, EntryLinkPos, ExitLink, ExitLinkPos;
    TRoute2MultiMapIterator Route2MultiMapIterator;

    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 0)
    {
        Utilities->CallLogPop(365);
        return(NoRoute); // none found
    }
    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 1)
    {
        Route2MultiMapIterator = Route2MultiMap.find(Route2MultiMapKeyPair);
// check actual link rather than just LinkPos, since if points LinkPos could be 0 or 2 so may not match ELinkPos or XLinkPos, but both refer to same link
        const TPrefDirElement &PrefDirElement1 = GetFixedRouteAt(73, Route2MultiMapIterator->second.first).GetFixedPrefDirElementAt(97,
                                                                                                                                    Route2MultiMapIterator->second.second);
        EntryLinkPos = PrefDirElement1.ELinkPos;
        ExitLinkPos = PrefDirElement1.XLinkPos;
        EntryLink = PrefDirElement1.Link[EntryLinkPos];
        ExitLink = PrefDirElement1.Link[ExitLinkPos];
        if(EntryLink == Track->TrackElementAt(135, TrackVectorPosition).Link[LinkPos])
        {
            EXGraphicPtr = PrefDirElement1.EXGraphicPtr;
            if((Route2MultiMapIterator->second.second == 0) || (Route2MultiMapIterator->second.second == GetFixedRouteAt(74,
                                                                                                                         Route2MultiMapIterator->second.first).PrefDirSize() - 1))
            {
                EntryDirectionGraphicPtr = PrefDirElement1.EntryDirectionGraphicPtr;
            }
            if(PrefDirElement1.AutoSignals)
            {
                Utilities->CallLogPop(366);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(367);
                return(NotAutoSigsRoute);
            }
        }
        if(ExitLink == Track->TrackElementAt(136, TrackVectorPosition).Link[LinkPos])
        {
            EXGraphicPtr = PrefDirElement1.EXGraphicPtr;
            if((Route2MultiMapIterator->second.second == 0) || (Route2MultiMapIterator->second.second == GetFixedRouteAt(75,
                                                                                                                         Route2MultiMapIterator->second.first).PrefDirSize() - 1))
            {
                EntryDirectionGraphicPtr = PrefDirElement1.EntryDirectionGraphicPtr;
            }
            if(PrefDirElement1.AutoSignals)
            {
                Utilities->CallLogPop(368);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(369);
                return(NotAutoSigsRoute);
            }
        }
    }
    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 2)
    {
        std::pair<TRoute2MultiMapIterator, TRoute2MultiMapIterator>ItPair;
        ItPair = Route2MultiMap.equal_range(Route2MultiMapKeyPair);
// check actual link rather than just LinkPos, since if points LinkPos could be 0 or 2 so may not match ELinkPos or XLinkPos, but both refer to same link
        const TPrefDirElement &PrefDirElement2 = GetFixedRouteAt(76, ItPair.first->second.first).GetFixedPrefDirElementAt(98, ItPair.first->second.second);
        EntryLinkPos = PrefDirElement2.ELinkPos;
        ExitLinkPos = PrefDirElement2.XLinkPos;
        EntryLink = PrefDirElement2.Link[EntryLinkPos];
        ExitLink = PrefDirElement2.Link[ExitLinkPos];
        if(EntryLink == Track->TrackElementAt(137, TrackVectorPosition).Link[LinkPos])
        {
            EXGraphicPtr = PrefDirElement2.EXGraphicPtr;
            if((ItPair.first->second.second == 0) || (ItPair.first->second.second == GetFixedRouteAt(77, ItPair.first->second.first).PrefDirSize() - 1))
            {
                EntryDirectionGraphicPtr = PrefDirElement2.EntryDirectionGraphicPtr;
            }
            if(PrefDirElement2.AutoSignals)
            {
                Utilities->CallLogPop(370);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(371);
                return(NotAutoSigsRoute);
            }
        }
        if(ExitLink == Track->TrackElementAt(138, TrackVectorPosition).Link[LinkPos])
        {
            EXGraphicPtr = PrefDirElement2.EXGraphicPtr;
            if((ItPair.first->second.second == 0) || (ItPair.first->second.second == GetFixedRouteAt(78, ItPair.first->second.first).PrefDirSize() - 1))
            {
                EntryDirectionGraphicPtr = PrefDirElement2.EntryDirectionGraphicPtr;
            }
            if(PrefDirElement2.AutoSignals)
            {
                Utilities->CallLogPop(372);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(373);
                return(NotAutoSigsRoute);
            }
        }
        ItPair.second--; // the second iterator points one past the last matching value
        const TPrefDirElement &PrefDirElement3 = GetFixedRouteAt(79, ItPair.second->second.first).GetFixedPrefDirElementAt(99, ItPair.second->second.second);
        EntryLinkPos = PrefDirElement3.ELinkPos;
        ExitLinkPos = PrefDirElement3.XLinkPos;
        EntryLink = PrefDirElement3.Link[EntryLinkPos];
        ExitLink = PrefDirElement3.Link[ExitLinkPos];
        if(EntryLink == Track->TrackElementAt(139, TrackVectorPosition).Link[LinkPos])
        {
            EXGraphicPtr = PrefDirElement3.EXGraphicPtr;
            if((ItPair.second->second.second == 0) || (ItPair.second->second.second == GetFixedRouteAt(80, ItPair.second->second.first).PrefDirSize() - 1))
            {
                EntryDirectionGraphicPtr = PrefDirElement3.EntryDirectionGraphicPtr;
            }
            if(PrefDirElement3.AutoSignals)
            {
                Utilities->CallLogPop(374);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(375);
                return(NotAutoSigsRoute);
            }
        }
        if(ExitLink == Track->TrackElementAt(140, TrackVectorPosition).Link[LinkPos])
        {
            EXGraphicPtr = PrefDirElement3.EXGraphicPtr;
            if((ItPair.second->second.second == 0) || (ItPair.second->second.second == GetFixedRouteAt(81, ItPair.second->second.first).PrefDirSize() - 1))
            {
                EntryDirectionGraphicPtr = PrefDirElement3.EntryDirectionGraphicPtr;
            }
            if(PrefDirElement3.AutoSignals)
            {
                Utilities->CallLogPop(376);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(377);
                return(NotAutoSigsRoute);
            }
        }
    }
    Utilities->CallLogPop(378);
    return(NoRoute); // none found
}

// ---------------------------------------------------------------------------
TAllRoutes::TRouteType TAllRoutes::GetRouteTypeAndNumber(int Caller, int TrackVectorPosition, int LinkPos, int &RouteNumber)
/*
      Examines Route2MultiMap and if the element at TrackVectorPosition with LinkPos (can be entry or exit) is found returns the appropriate
      route type - NoRoute, NotAutoSigsRoute, or AutoSigsRoute and number.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetRouteTypeAndNumber," + AnsiString(TrackVectorPosition) + "," +
                                 AnsiString(LinkPos));
    if(TrackVectorPosition == -1)
    {
        RouteNumber = -1;
        Utilities->CallLogPop(379);
        return(NoRoute); // allows for continuation & buffer entries & exits
    }
    THVPair Route2MultiMapKeyPair;

    Route2MultiMapKeyPair.first = Track->TrackElementAt(141, TrackVectorPosition).HLoc;
    Route2MultiMapKeyPair.second = Track->TrackElementAt(142, TrackVectorPosition).VLoc;
    int EntryLink, EntryLinkPos, ExitLink, ExitLinkPos;
    TRoute2MultiMapIterator Route2MultiMapIterator;

    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 0)
    {
        RouteNumber = -1;
        Utilities->CallLogPop(380);
        return(NoRoute); // none found
    }
    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 1)
    {
        Route2MultiMapIterator = Route2MultiMap.find(Route2MultiMapKeyPair);
// check actual link rather than just LinkPos, since if points LinkPos could be 0 or 2 so may not match ELinkPos or XLinkPos, but both refer to same link
        const TPrefDirElement &PrefDirElement1 = GetFixedRouteAt(82, Route2MultiMapIterator->second.first).GetFixedPrefDirElementAt(100,
                                                                                                                                    Route2MultiMapIterator->second.second);
        EntryLinkPos = PrefDirElement1.ELinkPos;
        ExitLinkPos = PrefDirElement1.XLinkPos;
        EntryLink = PrefDirElement1.Link[EntryLinkPos];
        ExitLink = PrefDirElement1.Link[ExitLinkPos];
        if(EntryLink == Track->TrackElementAt(143, TrackVectorPosition).Link[LinkPos])
        {
            RouteNumber = Route2MultiMapIterator->second.first;
            if(PrefDirElement1.AutoSignals)
            {
                Utilities->CallLogPop(381);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(382);
                return(NotAutoSigsRoute);
            }
        }
        if(ExitLink == Track->TrackElementAt(144, TrackVectorPosition).Link[LinkPos])
        {
            RouteNumber = Route2MultiMapIterator->second.first;
            if(PrefDirElement1.AutoSignals)
            {
                Utilities->CallLogPop(383);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(384);
                return(NotAutoSigsRoute);
            }
        }
    }
    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 2)
    {
        std::pair<TRoute2MultiMapIterator, TRoute2MultiMapIterator>ItPair;
        ItPair = Route2MultiMap.equal_range(Route2MultiMapKeyPair);
// check actual link rather than just LinkPos, since if points LinkPos could be 0 or 2 so may not match ELinkPos or XLinkPos, but both refer to same link
        const TPrefDirElement &PrefDirElement2 = GetFixedRouteAt(83, ItPair.first->second.first).GetFixedPrefDirElementAt(101, ItPair.first->second.second);
        EntryLinkPos = PrefDirElement2.ELinkPos;
        ExitLinkPos = PrefDirElement2.XLinkPos;
        EntryLink = PrefDirElement2.Link[EntryLinkPos];
        ExitLink = PrefDirElement2.Link[ExitLinkPos];
        if(EntryLink == Track->TrackElementAt(145, TrackVectorPosition).Link[LinkPos])
        {
            RouteNumber = ItPair.first->second.first;
            if(PrefDirElement2.AutoSignals)
            {
                Utilities->CallLogPop(385);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(386);
                return(NotAutoSigsRoute);
            }
        }
        if(ExitLink == Track->TrackElementAt(146, TrackVectorPosition).Link[LinkPos])
        {
            RouteNumber = ItPair.first->second.first;
            if(PrefDirElement2.AutoSignals)
            {
                Utilities->CallLogPop(387);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(388);
                return(NotAutoSigsRoute);
            }
        }
        ItPair.second--; // the second iterator points one past the last matching value
        const TPrefDirElement &PrefDirElement3 = GetFixedRouteAt(84, ItPair.second->second.first).GetFixedPrefDirElementAt(102, ItPair.second->second.second);
        EntryLinkPos = PrefDirElement3.ELinkPos;
        ExitLinkPos = PrefDirElement3.XLinkPos;
        EntryLink = PrefDirElement3.Link[EntryLinkPos];
        ExitLink = PrefDirElement3.Link[ExitLinkPos];
        if(EntryLink == Track->TrackElementAt(147, TrackVectorPosition).Link[LinkPos])
        {
            RouteNumber = ItPair.second->second.first;
            if(PrefDirElement3.AutoSignals)
            {
                Utilities->CallLogPop(389);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(390);
                return(NotAutoSigsRoute);
            }
        }
        if(ExitLink == Track->TrackElementAt(148, TrackVectorPosition).Link[LinkPos])
        {
            RouteNumber = ItPair.second->second.first;
            if(PrefDirElement3.AutoSignals)
            {
                Utilities->CallLogPop(391);
                return(AutoSigsRoute);
            }
            else
            {
                Utilities->CallLogPop(392);
                return(NotAutoSigsRoute);
            }
        }
    }
    RouteNumber = -1;
    Utilities->CallLogPop(393);
    return(NoRoute); // none found
}

// ---------------------------------------------------------------------------

void TAllRoutes::StoreOneRoute(int Caller, TOneRoute *Route)
/*
      A new (empty apart from RouteID) TOneRoute is added to the AllRoutesVector, which, since it is the last to be added, will have
      a RouteNumber of AllRoutesSize() - 1.  Then each element of the new route is added in turn using AddRouteElement,
      which uses HLoc, VLoc, ELink and RouteNumber to provide the information necessary to insert it into both PrefDirVector
      and Route2MultiMap.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",StoreOneRoute");
    TOneRoute EmptyRoute;

    EmptyRoute.RouteID = NextRouteID;
    NextRouteID++;

    AllRoutesVector.push_back(EmptyRoute); // to create a new route vector entry
    for(unsigned int x = 0; x < Route->PrefDirSize(); x++)
    {
        AddRouteElement(0, Route->GetFixedPrefDirElementAt(127, x).HLoc, Route->GetFixedPrefDirElementAt(128, x).VLoc,
                        Route->GetFixedPrefDirElementAt(129, x).GetELink(), AllRoutesSize() - 1, Route->GetFixedPrefDirElementAt(130, x));
    }
    int FirstVecPos = Route->GetFixedPrefDirElementAt(199, 0).TrackVectorPosition;
    int LastVecPos = Route->GetFixedPrefDirElementAt(200, (Route->PrefDirSize()) - 1).TrackVectorPosition;

    TrainController->LogEvent("StoreOneRoute," + AnsiString(EmptyRoute.RouteID) + "," + AnsiString(FirstVecPos) + "," + AnsiString(LastVecPos));
    Utilities->CallLogPop(394);
}

// ---------------------------------------------------------------------------

void TAllRoutes::StoreOneRouteAfterSessionLoad(int Caller, TOneRoute *Route)
/*
      A new (empty apart from RouteID) TOneRoute is added to the AllRoutesVector after a session load. For this the RouteID
      that is already in Route is used.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",StoreOneRouteAfterSessionLoad");
    TOneRoute EmptyRoute;

    EmptyRoute.RouteID = Route->RouteID;

    AllRoutesVector.push_back(EmptyRoute); // to create a new route vector entry
    for(unsigned int x = 0; x < Route->PrefDirSize(); x++)
    {
        AddRouteElement(3, Route->GetFixedPrefDirElementAt(189, x).HLoc, Route->GetFixedPrefDirElementAt(190, x).VLoc,
                        Route->GetFixedPrefDirElementAt(191, x).GetELink(), AllRoutesSize() - 1, Route->GetFixedPrefDirElementAt(192, x));
    }
    Utilities->CallLogPop(1579);
}

// ---------------------------------------------------------------------------

void TAllRoutes::ClearRouteDuringRouteBuildingAt(int Caller, int RouteNumber)
/*
      When attaching a new route section to an existing route, it is sometimes necessary to erase the
      original route and create a new composite route.  This function Erases all elements in the route
      at RouteNumber using TAllRoutes->RemoveRouteElement to clear elements from Route2MultiMap and
      from the PrefDirVector.  Since all elements for the route are removed RemoveRouteElement also
      clears the Route from AllRoutesVector.  Route numbers are decremented in the map for route numbers
      that are greater than the route number that is removed.  The LockedRouteVector as also searched
      and if any relate to the route that has been cleared they are erased too, but the fact that one
      has been found is recorded so that it can be re-established later.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",ClearRouteDuringRouteBuildingAt," + AnsiString(RouteNumber));
    THVPair Route2MultiMapKeyPair;
    TRoute2MultiMapEntry Route2MultiMapEntry;
    TRoute2MultiMapIterator Route2MultiMapIterator;

// need to check LockedVector first, and erase it if it's the route to be cleared, and to reinstate it as a new locked route with the same
// values (except RouteNumber) when the new route is established (in ConvertAndAdd...).
// If clear all route elements first then when the last is cleared the LockedVector.RouteNumber values are decremented if they are higher
// then the cleared route number (by RemoveRouteElement), and one of the new values may be the same number as the old cleared route number.
// If so the locked route is removed from the locked vector and is lost.
    LockedRouteRearTrackVectorPosition = 0;
    LockedRouteLastTrackVectorPosition = 0;
    LockedRouteLastXLinkPos = 0;
    LockedRouteLockStartTime = TDateTime(0);
    if(!LockedRouteVector.empty())
    {
        for(TLockedRouteVectorIterator LRVIT = LockedRouteVector.begin(); LRVIT < LockedRouteVector.end(); LRVIT++)
        {
            if(LRVIT->RouteNumber == RouteNumber)
            {
                LockedRouteRearTrackVectorPosition = LRVIT->RearTrackVectorPosition;
                LockedRouteLastTrackVectorPosition = LRVIT->LastTrackVectorPosition;
                LockedRouteLastXLinkPos = LRVIT->LastXLinkPos;
                LockedRouteLockStartTime = LRVIT->LockStartTime;
                LockedRouteFoundDuringRouteBuilding = true;
                LockedRouteVector.erase(LRVIT);
            }
        }
    }
    for(int x = (AllRoutes->GetFixedRouteAt(109, RouteNumber).PrefDirSize()) - 1; x >= 0; x--)
    {
        TPrefDirElement PrefDirElement = AllRoutes->GetFixedRouteAt(110, RouteNumber).GetFixedPrefDirElementAt(131, x);
        AllRoutes->RemoveRouteElement(7, PrefDirElement.HLoc, PrefDirElement.VLoc, PrefDirElement.GetELink());
    }
    Utilities->CallLogPop(395);
}

// ---------------------------------------------------------------------------

TAllRoutes::TRouteElementPair TAllRoutes::FindRoutePairFromRoute2MultiMap(int Caller, int HLoc, int VLoc, int ELink,
                                                                          TRoute2MultiMapIterator &Route2MultiMapIterator)
/*
      Examines Route2MultiMap and returns a TRouteElementPair if one is found with the passed values of H, V and ELink.
      Also returned as a reference is an iterator to the found element in the map to assist in erasing it.  Called by
      TAllRoutes::RemoveRouteElement(int Caller, int HLoc, int VLoc, int ELink).  Note that only need ELink (as well as H & V) to
      identify uniquely, since only bridges can have two routes on them & their track ELinks are always different.  Messages
      are given for failure.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindRoutePairFromRoute2MultiMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString(ELink));
    TRouteElementPair ReturnPair;

    ReturnPair.first = -1;
    ReturnPair.second = 0;
    THVPair Route2MultiMapKeyPair;

    Route2MultiMapKeyPair.first = HLoc;
    Route2MultiMapKeyPair.second = VLoc;
    TRoute2MultiMapEntry Route2MultiMapEntry;

    Route2MultiMapEntry.first = Route2MultiMapKeyPair;
    std::pair<TRoute2MultiMapIterator, TRoute2MultiMapIterator>ItPair;

    ItPair = Route2MultiMap.equal_range(Route2MultiMapKeyPair);
    Route2MultiMapIterator = ItPair.first;

    if(ItPair.first == ItPair.second)
    {
        throw Exception("Failed to find Route2MultiMap element at HLoc = " + (AnsiString)HLoc + " VLoc = " + (AnsiString)VLoc);
    }
    if(GetFixedRouteAt(111, ItPair.first->second.first).GetFixedPrefDirElementAt(132, ItPair.first->second.second).GetELink() == ELink)
    {
        ReturnPair.first = ItPair.first->second.first;
        ReturnPair.second = ItPair.first->second.second;
        Route2MultiMapIterator = ItPair.first;
        Utilities->CallLogPop(396);
        return(ReturnPair);
    }
    ItPair.first++;
    if(ItPair.first == ItPair.second)
    {
        throw Exception("Found Route2MultiMap element at HLoc = " + (AnsiString)HLoc + " VLoc = " + (AnsiString)VLoc + " but failed to find required element");
    }
    if(GetFixedRouteAt(112, ItPair.first->second.first).GetFixedPrefDirElementAt(133, ItPair.first->second.second).GetELink() == ELink)
    {
        ReturnPair.first = ItPair.first->second.first;
        ReturnPair.second = ItPair.first->second.second;
        Route2MultiMapIterator = ItPair.first;
        Utilities->CallLogPop(397);
        return(ReturnPair);
    }
    Utilities->CallLogPop(398);
    return(ReturnPair);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::FindRouteNumberFromRoute2MultiMapNoErrors(int Caller, int HLoc, int VLoc, int ELink, int &RouteNumber) // new at v1.2.0
/*
      Similar to above but returns a bool and no errors are reported for no route or element at H&V etc.
      Examines Route2MultiMap and returns true if a route is found with the passed values of H, V and ELink.
      RouteNumber (route position in AllRoutes vector is returned as a reference.
      Called by TTrain::CheckAndCancelRouteForWrongEndEntry.  Note that only need ELink (as well as H & V) to
      identify uniquely, since only bridges can have two routes on them & their track ELinks are always different.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",FindRouteNumberFromRoute2MultiMapNoErrors," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString(ELink));
    THVPair Route2MultiMapKeyPair;

    Route2MultiMapKeyPair.first = HLoc;
    Route2MultiMapKeyPair.second = VLoc;
    std::pair<TRoute2MultiMapIterator, TRoute2MultiMapIterator>ItPair;

    ItPair = Route2MultiMap.equal_range(Route2MultiMapKeyPair);

    if(ItPair.first == ItPair.second)
    {
        RouteNumber = -1;
        Utilities->CallLogPop(2032);
        return(false);
    }
    if(GetFixedRouteAt(205, ItPair.first->second.first).GetFixedPrefDirElementAt(241, ItPair.first->second.second).GetELink() == ELink)
    {
        RouteNumber = ItPair.first->second.first;
        Utilities->CallLogPop(2033);
        return(true);
    }
    ItPair.first++;

    if(ItPair.first == ItPair.second)
    {
        RouteNumber = -1;
        Utilities->CallLogPop(2034);
        return(false);
    }
    if(GetFixedRouteAt(206, ItPair.first->second.first).GetFixedPrefDirElementAt(242, ItPair.first->second.second).GetELink() == ELink)
    {
        RouteNumber = ItPair.first->second.first;
        Utilities->CallLogPop(2035);
        return(true);
    }
    RouteNumber = -1;
    Utilities->CallLogPop(2036);
    return(false);
}

// ---------------------------------------------------------------------------

void TAllRoutes::Route2MultiMapInsert(int Caller, int HLoc, int VLoc, int ELinkIn, int RouteNumber, unsigned int RouteElementNumber)
/*
      Elink needed in case it's a bridge, & need to know whether the found element is on this route or not.  First check if an
      entry in the map already exists at H & V, and if so check that it's a bridge with existing route on other track.
      That being so insert the new element.  If it's not a bridge, or the route has the same ELink value as the element to
      be inserted, give appropriate messages.  If there isn't an element at H & V already in the map insert it.
      Called by TAllRoutes::AddRouteElement.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",Route2MultiMapInsert," + AnsiString(HLoc) + "," + AnsiString(VLoc) +
                                 "," + AnsiString(ELinkIn) + "," + AnsiString(RouteNumber) + "," + AnsiString(RouteElementNumber));
    THVPair Route2MultiMapKeyPair;

    Route2MultiMapKeyPair.first = HLoc;
    Route2MultiMapKeyPair.second = VLoc;
    TRoute2MultiMapEntry Route2MultiMapEntry;

    Route2MultiMapEntry.first = Route2MultiMapKeyPair;
    TRouteElementPair RouteElementPair;

    RouteElementPair.first = RouteNumber;
    RouteElementPair.second = RouteElementNumber;
    Route2MultiMapEntry.second = RouteElementPair;

    if(Route2MultiMap.find(Route2MultiMapKeyPair) != Route2MultiMap.end())
    // true for element at H&V already included in map, has to be a bridge with existing route on opposite track to be valid
    {
        if(GetFixedRouteAt(113, Route2MultiMap.find(Route2MultiMapKeyPair)->second.first).GetFixedPrefDirElementAt(134,
                                                                                                                   Route2MultiMap.find(Route2MultiMapKeyPair)->second.second).GetELink() != ELinkIn)
        // element already at H&V has different ELink to element to be inserted, so must be a bridge with existing route on opposite treack
        {
            if(GetFixedRouteAt(114, Route2MultiMap.find(Route2MultiMapKeyPair)->second.first).GetFixedPrefDirElementAt(135,
                                                                                                                       Route2MultiMap.find(Route2MultiMapKeyPair)->second.second).TrackType != Bridge)
            {
                throw Exception("Error, bridge expected in Route2MultiMapInsert but not, at HLoc=" + AnsiString(HLoc) + " VLoc=" + AnsiString(VLoc));
            }
            Route2MultiMap.insert(Route2MultiMapEntry); // insert bridge into map again but now with the new track as part of required route
        }
        else
        // same ELink so have an error
        {
            throw Exception("Error, route map entry found in Route2MultiMapInsert at HLoc=" + AnsiString(HLoc) + " VLoc=" + AnsiString(VLoc));
        }
    }
    else
    {
        Route2MultiMap.insert(Route2MultiMapEntry);
    }
// element at H&V not found in map so insert it
    Utilities->CallLogPop(399);
}

// ---------------------------------------------------------------------------

TAllRoutes::TRouteElementPair TAllRoutes::GetRouteElementDataFromRoute2MultiMap(int Caller, int HLoc, int VLoc, TAllRoutes::TRouteElementPair &SecondPair)
/*
      Retrieve up to two TRouteElementPair entries from Route2MultiMap at H & V, the first as a function return
      and the second in the reference SecondPair.  If there's only one then it's the function return
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetRouteElementDataFromRoute2MultiMap," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc));
    TAllRoutes::TRouteElementPair TempPair;

    TempPair.first = -1;
    TempPair.second = 0;
    SecondPair = TempPair;
    TRoute2MultiMapIterator Route2MultiMapIterator;
    std::pair<TRoute2MultiMapIterator, TRoute2MultiMapIterator>ItRange;
    THVPair Route2MultiMapKeyPair;

    Route2MultiMapKeyPair.first = HLoc;
    Route2MultiMapKeyPair.second = VLoc;
    if(Route2MultiMap.count(Route2MultiMapKeyPair) == 0)
    {
        Utilities->CallLogPop(400);
        return(TempPair);
    }
    else if(Route2MultiMap.count(Route2MultiMapKeyPair) == 1)
    {
        Route2MultiMapIterator = Route2MultiMap.find(Route2MultiMapKeyPair);
        Utilities->CallLogPop(401);
        return(Route2MultiMapIterator->second);
    }
    else if(Route2MultiMap.count(Route2MultiMapKeyPair) == 2)
    {
        ItRange = Route2MultiMap.equal_range(Route2MultiMapKeyPair);
        TempPair = ItRange.first->second;
        SecondPair = (--ItRange.second)->second; // 2nd iterator points past the last value
        Utilities->CallLogPop(402);
        return(TempPair);
    }
    Utilities->CallLogPop(403);
    return(TempPair);
}

// ---------------------------------------------------------------------------

void TAllRoutes::CheckMapAndRoutes(int Caller) // test
/*
      Checks equivalence for each route between entries in PrefDirVector and those in Route2MultiMap, and also that the size
      of the multimap and the sum of the sizes of all PrefDirVectors is the same.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckMapAndRoutes");
    for(unsigned int a = 0; a < AllRoutes->AllRoutesSize(); a++)
    {
        for(unsigned int b = 0; b < AllRoutes->GetFixedRouteAt(115, a).PrefDirSize(); b++)
        {
            TPrefDirElement CheckElement = AllRoutes->GetFixedRouteAt(116, a).GetFixedPrefDirElementAt(136, b);
            TAllRoutes::TRouteElementPair SecondPair;
            TRouteElementPair RouteElementPair = GetRouteElementDataFromRoute2MultiMap(8, CheckElement.HLoc, CheckElement.VLoc, SecondPair);
            if(RouteElementPair.first == -1)
            // failed to find element in multimap
            {
                throw Exception("CheckMapAndRoutes Error - failed to find HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" + (AnsiString)CheckElement.VLoc +
                                " in Route2MultiMap, Caller=" + (AnsiString)Caller);
            }
            if((RouteElementPair.first != (int)a) && (SecondPair.first != (int)a))
            // neither pair has expected route number
            {
                throw Exception("CheckMapAndRoutes Error - RouteNumber failed at HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" +
                                (AnsiString)CheckElement.VLoc + " Map value=" + (AnsiString)RouteElementPair.first + " Route value=" + (AnsiString)a + " Caller=" +
                                (AnsiString)Caller);
            }
            if(((RouteElementPair.first != (int)a) || (RouteElementPair.second != b)) && ((SecondPair.first != (int)a) || (SecondPair.second != b)))
            // need one of pairs to match both RouteNumber and RouteElementNumber or fails
            {
                throw Exception("CheckMapAndRoutes Error - PrefDirVectorNumber failed at HLoc=" + (AnsiString)CheckElement.HLoc + " VLoc=" +
                                (AnsiString)CheckElement.VLoc + " 1st Map value RouteNum/ElementNum =" + (AnsiString)RouteElementPair.first + "/" +
                                (AnsiString)RouteElementPair.second + " 2nd Map value =" + (AnsiString)SecondPair.first + "/" + (AnsiString)SecondPair.second +
                                " Route value=" + (AnsiString)a + "/" + (AnsiString)b + " Caller=" + (AnsiString)Caller);
            }
        }
    }
    unsigned int SizeVal = 0;

// check map and sum of route sizes match
    for(unsigned int a = 0; a < AllRoutesSize(); a++)
    {
        SizeVal += GetFixedRouteAt(117, a).PrefDirSize();
    }
    if(SizeVal != Route2MultiMap.size())
    {
        throw Exception("CheckMapAndRoutes Error - Map Size=" + (AnsiString)Route2MultiMap.size() + " RouteSize=" + (AnsiString)SizeVal + " Caller=" +
                        (AnsiString)Caller);
    }
    Utilities->CallLogPop(404);
    return;
}

// ---------------------------------------------------------------------------

void TAllRoutes::DecrementRouteNumbersInRoute2MultiMap(int Caller, int RouteNumber)
/*
      After a route has been erased from AllRoutesVector and its entries from Route2MultiMap, this
      function examines all the remaining entries in Route2MultiMap to see if their RouteNumbers
      exceed that for the erased route.  Where this is so the RouteNumber is decremented.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DecrementRouteNumbersInRoute2MultiMap," + AnsiString(RouteNumber));
    if(!Route2MultiMap.empty())
    {
        for(TRoute2MultiMapIterator Route2MultiMapIterator = Route2MultiMap.begin(); Route2MultiMapIterator != Route2MultiMap.end(); Route2MultiMapIterator++)
        {
            if(Route2MultiMapIterator->second.first > RouteNumber)
            {
                Route2MultiMapIterator->second.first--;
            }
        }
    }
    Utilities->CallLogPop(405);
}

// ---------------------------------------------------------------------------

void TAllRoutes::DecrementRouteElementNumbersInRoute2MultiMap(int Caller, int RouteNumber, unsigned int ErasedElementNumber)
/*
      After a route element has been erased from the relevant PrefDirVector and from Route2MultiMap, this
      function examines all the remaining entries in Route2MultiMap with the same RouteNumber as that
      for the erased element.  Where a RouteElementNumber exceeds that for the erased element it is decremented.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DecrementRouteElementNumbersInRoute2MultiMap," +
                                 AnsiString(RouteNumber) + "," + AnsiString(ErasedElementNumber));
    if(!Route2MultiMap.empty())
    {
        for(TRoute2MultiMapIterator Route2MultiMapIterator = Route2MultiMap.begin(); Route2MultiMapIterator != Route2MultiMap.end(); Route2MultiMapIterator++)
        {
            if((Route2MultiMapIterator->second.first == RouteNumber) && (Route2MultiMapIterator->second.second > ErasedElementNumber))
            {
                Route2MultiMapIterator->second.second--;
            }
        }
    }
    Utilities->CallLogPop(406);
}

// ---------------------------------------------------------------------------

void TAllRoutes::RemoveRouteElement(int Caller, int HLoc, int VLoc, int ELink)
/*
      Erases the route element from Route2MultiMap and from the PrefDirVector.
      If there are no elements left in the PrefDirVector the route is cleared from AllRoutesVector.  Route element numbers in the map are
      decremented if they are greater than the element number removed, and if the entire route is removed
      then the route numbers are also decremented in the map for route numbers that are greater than the route
      number that is removed.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RemoveRouteElement," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(ELink));
    TRouteElementPair RequiredRoutePair; // RouteNumber & RouteElementNumber
    TRoute2MultiMapIterator Route2MultiMapIterator;

    RequiredRoutePair = FindRoutePairFromRoute2MultiMap(0, HLoc, VLoc, ELink, Route2MultiMapIterator);
    if(RequiredRoutePair.first == -1)
    {
        throw Exception("Failed to find route element in RemoveRouteElement");
    }
    Route2MultiMap.erase(Route2MultiMapIterator);
    DecrementRouteElementNumbersInRoute2MultiMap(0, RequiredRoutePair.first, RequiredRoutePair.second);

// even though element has been erased from the routemap, RequiredRoutePair still contains the element values
    TPrefDirElement LockedRouteElement, PrefDirElement = GetFixedRouteAt(118, RequiredRoutePair.first).GetFixedPrefDirElementAt(137, RequiredRoutePair.second);

    if(Track->TrackElementAt(157, PrefDirElement.TrackVectorPosition).Config[PrefDirElement.XLinkPos] == Signal)
    {
        Track->TrackElementAt(158, PrefDirElement.TrackVectorPosition).Attribute = 0; // change forward signals back to red
    }
// don't need the section below (a) because when a train removes elements from the front of a locked route, there is a test in
// ApproachLocking to determine whether the element immediately nearer the start of the route to the element being removed is still
// present, and of not the element removal stops; and (b) because it never worked anyway! - IsElementInLockedRoute.... uses Route2MultiMap
// to check if a route element is present, and the element has already been removed from the map - see above.

// before erase the element check if it's in a locked route, and if so change the RearTrackVectorPosition to the next valid (XLinkPos] element position
/*
      int LockedVectorNumber = -1;
      if(IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(5, PrefDirElement.TrackVectorPosition, PrefDirElement.XLinkPos, LockedRouteElement, LockedVectorNumber))
       {
       LockedRouteVector.at(LockedVectorNumber).RearTrackVectorPosition = PrefDirElement.Conn[PrefDirElement.XLinkPos];
       }
*/

// erase element from route
    GetModifiableRouteAt(8, RequiredRoutePair.first).PrefDirVector.erase(GetModifiableRouteAt(33, RequiredRoutePair.first).PrefDirVector.begin() + RequiredRoutePair.second);
// CheckMapAndRoutes();//test - drop - tested below

// remove ContinuationAutoSig route if element is in one since if any part of it is truncated the continuation exit will be removed - must
// be so as continuation exit is at the end of the route, and truncation is from the end
    if(!TrainController->ContinuationAutoSigVector.empty())
    {
        TTrainController::TContinuationAutoSigVectorIterator AutoSigVectorIT;
        for(AutoSigVectorIT = TrainController->ContinuationAutoSigVector.end() - 1; AutoSigVectorIT >= TrainController->ContinuationAutoSigVector.begin();
            AutoSigVectorIT--)
        {
            if(AutoSigVectorIT->RouteNumber == RequiredRoutePair.first)
            {
                TrainController->ContinuationAutoSigVector.erase(AutoSigVectorIT);
            }
        }
    }
// now if last element from a route was removed need to remove the route from the route vector and from the LockedRouteVector if exists,
// and adjust all the corresponding route numbers
    if(GetModifiableRouteAt(10, RequiredRoutePair.first).PrefDirSize() == 0)
    {
        TrainController->LogEvent("RouteRemoved," + AnsiString(GetFixedRouteAt(189, RequiredRoutePair.first).RouteID));
        AllRoutesVector.erase(AllRoutesVector.begin() + RequiredRoutePair.first);
        DecrementRouteNumbersInRoute2MultiMap(0, RequiredRoutePair.first);

/* drop this:  LockedVectorNumber was supposed to be determined from the above section that has been dropped, so this doesn't work
      It isn't needed anyway as a check is made after the Locked route timeout as to whether the end element is in a route or not, and if not
      it is erased then - see TInterface::ApproachLocking

       if(LockedVectorNumber > -1)
           {
           LockedRouteVector.erase(LockedRouteVector.begin() + LockedVectorNumber);
           }
*/
        // decrement route numbers in the locked route vector whether or not this route is a locked route
        if(!LockedRouteVector.empty())
        {
            for(TLockedRouteVectorIterator LRVIT = LockedRouteVector.begin(); LRVIT < LockedRouteVector.end(); LRVIT++)
            {
                if(LRVIT->RouteNumber > RequiredRoutePair.first)
                {
                    LRVIT->RouteNumber--;
                }
            }
        }
        if(!TrainController->ContinuationAutoSigVector.empty())
        {
            TTrainController::TContinuationAutoSigVectorIterator AutoSigVectorIT;
            for(AutoSigVectorIT = TrainController->ContinuationAutoSigVector.end() - 1; AutoSigVectorIT >= TrainController->ContinuationAutoSigVector.begin();
                AutoSigVectorIT--)
            {
                if(AutoSigVectorIT->RouteNumber > RequiredRoutePair.first)
                {
                    AutoSigVectorIT->RouteNumber--;
                }
            }
        }
    }
    CheckMapAndRoutes(7); // test
    Utilities->CallLogPop(407);
}

// ---------------------------------------------------------------------------

void TAllRoutes::AddRouteElement(int Caller, int HLoc, int VLoc, int ELink, int RouteNumber, TPrefDirElement RouteElement)
/*
      A single TPrefDirElement is added to both PrefDirVector (for the route at RouteNumber) and Route2MultiMap.
      Called from TAllRoutes::StoreOneRoute.  Note that the IsARoute boolean variable is set in StoreRouteElementInPrefDirVector
      since that catches all route elements wherever created
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",AddRouteElement," + AnsiString(HLoc) + "," + AnsiString(VLoc) + "," +
                                 AnsiString(ELink) + "," + AnsiString(RouteNumber) + "," + RouteElement.LogPrefDir());
    GetModifiableRouteAt(11, RouteNumber).StoreRouteElementInPrefDirVector(RouteElement);
    Route2MultiMapInsert(0, HLoc, VLoc, ELink, RouteNumber, GetModifiableRouteAt(12, RouteNumber).PrefDirSize() - 1); //-1 because vector has been increased by 1 above, so
    Utilities->CallLogPop(408);                                                                                       //PrefDirSize() has been increased by 1, so the new element
}                                                                                                                     //number is one less than this

// ---------------------------------------------------------------------------

void TAllRoutes::SetTrailingSignalsOnAutoSigsRoute(int Caller, int TrackVectorPosition, int XLinkPos)
/*
      Enter with signal at TrackVectorElement already set to red by the passing train.
      Identify the route that the TrackVectorPosition is in, carry out validity checks, then call SetAllRearwardsSignals to set signals
      in this route and all linked rearwards routes, unless find a train (a) in the current route, in which case the signals behind it are
      set (and behind any other trains in the current route), but only within the current route; or (b) in a linked rear route, in which
      case the function sets no further signals.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetTrailingSignalsOnAutoSigsRoute," + AnsiString(TrackVectorPosition) +
                                 "," + AnsiString(XLinkPos));
    TRouteElementPair RouteElementPair, SecondPair, RequiredPair;
    TTrackElement TE = Track->TrackElementAt(159, TrackVectorPosition);

    RouteElementPair = GetRouteElementDataFromRoute2MultiMap(9, TE.HLoc, TE.VLoc, SecondPair);
    if(RouteElementPair.first == -1)
    {
        throw Exception("Error, failed to find element in SetTrailingSignalsOnAutoSigsRoute - 1");
    }
    TPrefDirElement RouteElement = GetFixedRouteAt(119, RouteElementPair.first).GetFixedPrefDirElementAt(138, RouteElementPair.second);

    RequiredPair = RouteElementPair;
    if(RouteElement.XLinkPos != XLinkPos)
    {
        if(SecondPair.first != -1)
        {
            RouteElement = GetFixedRouteAt(120, SecondPair.first).GetFixedPrefDirElementAt(139, SecondPair.second);
            RequiredPair = SecondPair;
            if(RouteElement.XLinkPos != XLinkPos)
            {
                throw Exception("Failed to find element in route in SetTrailingSignalsOnAutoSigsRoute - 2");
            }
        }
        else
        {
            throw Exception("Failed to find element in route in SetTrailingSignalsOnAutoSigsRoute - 3");
        }
    }
// new function
    SetAllRearwardsSignals(5, 0, RequiredPair.first, RequiredPair.second);
    Utilities->CallLogPop(409);
}

// ---------------------------------------------------------------------------

void TAllRoutes::SetTrailingSignalsOnContinuationRoute(int Caller, int RouteNumber, int AccessNumber)
/*
      This is called by the InterfaceUnit at intervals based on entries in the ContinuationAutoSigVector in TrainController to set signals on
      the AutoSigsRoute to correspond to a train having exited the route at a continuation, and passing further signals (outside the simulated
      railway).  Initially the last passed signal will be red, then at the first call it will change to yellow and earlier signals will change
      accordingly, then double yellow, then green.  There are only 3 calls in all for any given route, and the AccessNumber changes from 0 to 1
      to 2 for successive calls.
      Initially Attribute is set to AccessNumber + 1 to correspond to the first signal attribute to be set, then a number of validity checks
      are carried out on RouteNumber.  Then SetAllRearwardsSignals is called to set signals in this route and all linked rearwards routes,
      unless find a train (a) in the current route, in which case the signals behind it are set (and behind any other trains in the current
      route), but only within the current route; or (b) in a linked rear route, in which case the function sets no further signals.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetTrailingSignalsOnContinuationRoute," + AnsiString(RouteNumber) + "," +
                                 AnsiString(AccessNumber));
    TPrefDirElement RouteElement;
    int Attribute = AccessNumber + 1;
// signal attributes:  0=red; 1=yellow; 2=double yellow; 3 = green
    int x = GetFixedRouteAt(121, RouteNumber).PrefDirSize() - 1;

    if(!(GetFixedRouteAt(122, RouteNumber).GetFixedPrefDirElementAt(140, x).AutoSignals))
    {
        throw Exception("Error - route not AutoSignals in SetTrailingSignalsOnContinuationRoute");
    }
    if(GetFixedRouteAt(123, RouteNumber).GetFixedPrefDirElementAt(141, x).TrackType != Continuation)
    {
        throw Exception("Error - end element not continuation in SetTrailingSignalsOnContinuationRoute");
    }
    if(GetFixedRouteAt(124, RouteNumber).GetFixedPrefDirElementAt(142, x).Config[GetFixedRouteAt(125, RouteNumber).GetFixedPrefDirElementAt(143,
                                                                                                                                            x).XLinkPos] != End)
    {
        throw Exception("Error - end element a continuation in SetTrailingSignalsOnContinuationRoute but End not facing right way");
    }
// new function
    SetAllRearwardsSignals(6, Attribute, RouteNumber, GetFixedRouteAt(126, RouteNumber).PrefDirSize() - 1);
    Utilities->CallLogPop(410);
}

// ---------------------------------------------------------------------------

void TAllRoutes::SetAllRearwardsSignals(int Caller, int Attribute, int RouteNumber, int RouteStartPosition)
/*
      Sets signals in all linked rearwards routes from the RouteStartPosition in RouteNumber, unless find a train (a) in the current route,
      in which case the signals behind it are set (and behind any other trains in the current route), but only within the current route;
      or (b) in a linked rear route, in which case the function sets no further signals.

      First call SetRearwardsSignalsReturnFalseForTrain (which is only called by this function) to set signals in route RouteNumber according
      to the received or modified (because of the forward look for buffers or continuation) Attribute.  If no train is found during this call
      (returns true) then check for and call SetRearwardsSignalsReturnFalseForTrain for each rearwards linked route until either reach the
      beginning of the last linked route or find a train on a linked rear route.  If no train was found during the RouteNumber call to
      SetRearwardsSignalsReturnFalseForTrain then the function terminates here.
      However if a train was found during the RouteNumber call to SetRearwardsSignalsReturnFalseForTrain then need to continue after the
      train in case had just added a route segment behind a train that now forms part of a single continuous route, otherwise the signals
      won't be set behind the train.  First the route is examined element by element from the RouteStartPosition towards the start of the
      route until the train is found.  Then the route elements are examined from the TrainPosition towards the start of the route until the
      first element behind the train is found.  A recursive call to this function is then made from this behind-train position, to set all
      signals behind the train (and behind as many trains as there are on the single route) beginning with a red signal for the first signal
      found behind the train.

      Description of SetRearwardsSignalsReturnFalseForTrain for reference:
      Enter with Attribute set to the value to be used (unless modified by the initial forward search - see later) for the first rearwards
      signal found, and with PrefDirVectorStartPosition set to the position in PrefDirVector to begin the search.  BUT, don't begin with the
      rearward search, first search forwards from the PrefDirVectorStartPosition in case the end of the route is a buffer or continuation, and
      modify the Attribute accordingly UNLESS (a) train present between PrefDirVectorStartPosition & end; (b) route in
      ContinuationAutoSigVector (i.e. train has exited the route at a continuation but it is still affecting the signals), or (c) truncating
      a route.

      Having received or modified Attribute as above, work backwards from the PrefDirVectorStartPosition until find a train - return false, or a
      signal.  If find a signal set its Attribute to the current Attribute value up to a maximum of 3, and replot the signal as well as
      the required route and direction (if required) graphics, then increment Attribute up to a max. of 3 [addition at v2.9.2: if signal or element beyond
      it is in a locked route then set signal to red & change Attribute to 0 - this fault reported by Simon Banham 21/07/21 as an image]. and continue working backwards
      for the next signal (or train - return false as before) and so on.  On completion Attribute is passed back from the function as a
      reference.  If no train is found before the beginning of the route is reached the function returns true

*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SetAllRearwardsSignals," + AnsiString(Attribute) + "," +
                                 AnsiString(RouteNumber) + "," + AnsiString(RouteStartPosition));
    TPrefDirElement FirstElement = GetFixedRouteAt(127, RouteNumber).GetFixedPrefDirElementAt(144, 0);
    int RearwardLinkedRouteNumber;

    Track->LCFoundInRouteBuildingFlag = false; // only examined for the new route segment, not for linked routes
    if(GetFixedRouteAt(128, RouteNumber).SetRearwardsSignalsReturnFalseForTrain(1, Attribute, RouteStartPosition)) // updates Attribute to 1+ final
    // signal value in the route for use in further linked routes
    {
        if(FirstElement.Conn[FirstElement.ELinkPos] > -1) // GetRouteTypeAndNumber tests for this but check here to avoid call if == -1
        {
            while(GetRouteTypeAndNumber(6, FirstElement.Conn[FirstElement.ELinkPos], FirstElement.ConnLinkPos[FirstElement.ELinkPos],
                    RearwardLinkedRouteNumber) != TAllRoutes::NoRoute)
            {
                if(!(GetFixedRouteAt(129, RearwardLinkedRouteNumber).SetRearwardsSignalsReturnFalseForTrain(2, Attribute, AllRoutes->GetFixedRouteAt(130,
                    RearwardLinkedRouteNumber).PrefDirSize() - 1)))
                {
                    break;
                }
                // in above the RouteSettingFlag is set to false because this call is for routes that lie behind the route that is being set so don't want to
                // flash LCs on those routes
                FirstElement = AllRoutes->GetFixedRouteAt(131, RearwardLinkedRouteNumber).GetFixedPrefDirElementAt(145, 0);
            }
        }
    }
    else
    // found a train in the entry route before the beginning of the route, so need to continue after the train in case had just added a
    // route segment behind a train that now forms part of a single continuous route, otherwise the signals won't be set behind the train
    {
        int TrainID, TrainPosition, BehindTrainPosition;
        bool FoundTrain = false, BehindTrain = false;
        for(int x = RouteStartPosition; x >= 0; x--) // first step back from start position until find the train....
        {
            TPrefDirElement PrefDirElement = GetFixedRouteAt(132, RouteNumber).GetFixedPrefDirElementAt(146, x);
            TTrackElement TrackElement = Track->TrackElementAt(160, PrefDirElement.TrackVectorPosition);
            TrainID = TrackElement.TrainIDOnElement;
            if(TrackElement.TrackType == Bridge)
            {
                if(PrefDirElement.XLinkPos < 2)
                {
                    TrainID = TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID == -1)
            {
                continue;
            }
            else
            {
                FoundTrain = true;
                TrainPosition = x;
                break;
            }
        }
        if(FoundTrain && (TrainPosition > 1)) // if TrainPosition 1 or less then no route behind the train so can stop
        {
            for(int x = TrainPosition; x >= 0; x--) // then step back from that position until find element behind the train - ignore any
            {
                // signals that the train itself is straddling, need the first signal behind the train to be set to red, when the train passes
                // the signal it's straddling the rearwards signals will be reset again.  Even if there are two or more trains adjacent still
                // need the element behind the rearmost train.
                TPrefDirElement PrefDirElement = GetFixedRouteAt(133, RouteNumber).GetFixedPrefDirElementAt(147, x);
                TTrackElement TrackElement = Track->TrackElementAt(161, PrefDirElement.TrackVectorPosition);
                TrainID = TrackElement.TrainIDOnElement;
                if(TrackElement.TrackType == Bridge)
                {
                    if(PrefDirElement.XLinkPos < 2)
                    {
                        TrainID = TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                    }
                    else
                    {
                        TrainID = TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                    }
                }
                if(TrainID != -1)
                {
                    continue; // still on train
                }
                else
                {
                    BehindTrain = true;
                    BehindTrainPosition = x;
                    break;
                }
            }
            if(BehindTrain) // then carry out a recursive rearward signal setting behind the train &
            // so on for as many trains as there are on the single route
            {
                SetAllRearwardsSignals(7, 0, RouteNumber, BehindTrainPosition); // false because can't set a route where there is a train
                // first signal behind train to be red
            }
        }
    }
    Utilities->CallLogPop(411);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::RouteLockingRequired(int Caller, int RouteNumber, int LookBackwardsFromHere)
{
/* Route locking is required (returns true) if a moving train is within 3 signals back from the RouteTruncatePosition on the route itself or on any linked routes,
unless the first signal back is red, or a train on the element immediately before the start of the rearmost linked route (i.e. not on a route but about to enter the
rearmost linked route) - this because train cancels route elements that it touches)
*/
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",RouteLockingRequired," + AnsiString(RouteNumber) + "," +
                                 AnsiString(LookBackwardsFromHere));
    int SignalCount = 0, TrainID, RearwardLinkedRouteNumber;
    TOneRoute CurrentRoute = GetFixedRouteAt(134, RouteNumber);
    TPrefDirElement PrefDirElement, FirstElement;
    TTrackElement TrackElement;
    bool ExamineRoute = true;

    while(ExamineRoute)
    {
        for(int x = LookBackwardsFromHere; x >= 0; x--)  //work back along the route from the start position
        {
            PrefDirElement = CurrentRoute.GetFixedPrefDirElementAt(148, x);
            TrackElement = Track->TrackElementAt(162, PrefDirElement.TrackVectorPosition);
            TrainID = TrackElement.TrainIDOnElement;
            if(TrackElement.TrackType == Bridge)
            {
                if(PrefDirElement.XLinkPos < 2)
                {
                    TrainID = TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = TrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID > -1)
            {
                if(TrainController->TrainVectorAtIdent(36, TrainID).Stopped())
                {
                    //any trains further back in route will be protected by the red signal behind the stopped train
                    Utilities->CallLogPop(412);
                    return(false);
                }
                //added at v2.4.2 for trains facing the wrong way & moving but haven't moved a half element yet so route still intact
                if(TrainController->TrainVectorAtIdent(49, TrainID).GetLeadElement() != PrefDirElement.TrackVectorPosition) //if it isn't then the train is facing the
                                                                                                                            //other way & can cancel the route
                {
                    Utilities->CallLogPop(2203);
                    return(false);
                }
                Utilities->CallLogPop(1961);  //otherwise need to lock the route as have found a train on the route (trains forward of the truncate point caught by
                return(true);                  //MovingTrainOccupyingRoute which is outside this function but also causes route locking)
            }
            if(PrefDirElement.Config[PrefDirElement.XLinkPos] == Signal) // XLinkPos because signal has to be facing same direction as PrefDir to count
            {
                if(TrackElement.Attribute == 0)
                {
                    Utilities->CallLogPop(413);
                    return(false); // OK, red signal in front of a train
                }
                if(TrackElement.SigAspect != TTrackElement::GroundSignal) //ignore ground signals
                {
                    SignalCount++;
                }
                if(SignalCount >= 3)
                {
                    Utilities->CallLogPop(414);
                    return(false);
                }
            }
            if(PrefDirElement.Config[PrefDirElement.ELinkPos] == End) // buffer or continuation & no train
            // ElinkPos because working back along PrefDir to beginning
            {
                Utilities->CallLogPop(415);
                return(false); // test - set to true to create a locked buffer-ended route, false for normal use
            }
        }
        //now look at linked rearwards routes
        FirstElement = CurrentRoute.GetFixedPrefDirElementAt(149, 0);
        LookBackwardsFromHere = CurrentRoute.PrefDirSize() - 1;
        if(GetRouteTypeAndNumber(7, FirstElement.Conn[FirstElement.ELinkPos], FirstElement.ConnLinkPos[FirstElement.ELinkPos],
                                 RearwardLinkedRouteNumber) != TAllRoutes::NoRoute)
        {
            CurrentRoute = GetFixedRouteAt(135, RearwardLinkedRouteNumber);
            ExamineRoute = true;
            LookBackwardsFromHere = GetFixedRouteAt(136, RearwardLinkedRouteNumber).PrefDirSize() - 1;
        }
        else
        {
            // here check for a train on the element immediately before the first route element (i.e. not on a route but about to enter the rearmost linked route)
            TTrackElement PriorTrackElement = Track->TrackElementAt(489, FirstElement.Conn[FirstElement.ELinkPos]);
            TrainID = PriorTrackElement.TrainIDOnElement;
            if(PriorTrackElement.TrackType == Bridge)
            {
                if(FirstElement.ConnLinkPos[FirstElement.ELinkPos] < 2)
                {
                    TrainID = PriorTrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit01;
                }
                else
                {
                    TrainID = PriorTrackElement.TrainIDOnBridgeOrFailedPointOrigSpeedLimit23;
                }
            }
            if(TrainID > -1)
            {
                if(TrainController->TrainVectorAtIdent(37, TrainID).Stopped())
                {
                    Utilities->CallLogPop(748);
                    return(false);
                }
                //added at v2.4.2 for trains facing the wrong way on the prior element & moving but haven't moved a half element yet
                if(TrainController->TrainVectorAtIdent(50, TrainID).GetLeadElement() != FirstElement.Conn[FirstElement.ELinkPos]) //if it isn't then the train is facing the
                                                                                                                                  //other way & can cancel the route
                {
                    Utilities->CallLogPop(2204);
                    return(false);
                }
                Utilities->CallLogPop(1962);
                return(true);  //otherwise need to lock the route
            }
            ExamineRoute = false;
        }
    }
// if reach beginning of all rear routes without finding a train and there aren't 3 signals then truncate the route
// as trains running on unrouted lines are already at risk of wrong points etc so no benefit locking the route
    Utilities->CallLogPop(416);
    return(false);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber(int Caller, int TrackVectorPosition, int XLinkPos,
                                                                              TPrefDirElement &PrefDirElement, int &LockedVectorNumber)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber," +
                                 AnsiString(TrackVectorPosition) + "," + AnsiString(XLinkPos));
    TPrefDirElement InternalPrefDirElement; // blank element

    PrefDirElement = InternalPrefDirElement;
    if(LockedRouteVector.empty())
    {
        Utilities->CallLogPop(417);
        return(false);
    }
// make sure at least one locked route record is still valid - train may have removed it, if last element still present locked route still exists,
// even if some elements have been removed from the front by a train
    bool InLockedRoute = false;

    for(TLockedRouteVectorIterator LRVIT = LockedRouteVector.begin(); LRVIT < LockedRouteVector.end(); LRVIT++)
    {
        if(TrackIsInARoute(14, LRVIT->LastTrackVectorPosition, LRVIT->LastXLinkPos))
        {
            // end of route can't be points, crossover or bridge so danger of route being on the other track of a 2-track element
            // doesn't arise)
            InLockedRoute = true;
            break;
        }
    }
    if(!InLockedRoute)
    {
        Utilities->CallLogPop(418);
        return(false);
    }
    int RouteNumber, VectorCount = 0;
    TRouteType RouteType;

    for(TLockedRouteVectorIterator LRVIT = LockedRouteVector.begin(); LRVIT < LockedRouteVector.end(); LRVIT++)
    {
        RouteType = GetRouteTypeAndNumber(8, LRVIT->LastTrackVectorPosition, LRVIT->LastXLinkPos, RouteNumber);
        if(RouteType == NoRoute)
        {
            continue;
        }
/*  can't use this test with front truncation
        if((GetFixedRouteAt(137, RouteNumber).GetFixedPrefDirElementAt(150, GetFixedRouteAt(138, RouteNumber).PrefDirSize() - 1).TrackVectorPosition != (int)
            LRVIT->LastTrackVectorPosition) || (GetFixedRouteAt(139, RouteNumber).GetFixedPrefDirElementAt(151,
                                                                                                           GetFixedRouteAt(140, RouteNumber).PrefDirSize() - 1).XLinkPos != LRVIT->LastXLinkPos))
        {
            throw Exception
                      ("Error, last element in locked route doesn't correspond with last element in associated route in IsElementInLockedRouteGetPrefDirElement");
        }
*/
        for(int x = GetFixedRouteAt(141, RouteNumber).PrefDirSize() - 1; x >= 0; x--)
        {
            InternalPrefDirElement = GetFixedRouteAt(142, RouteNumber).GetFixedPrefDirElementAt(152, x);
            if(InternalPrefDirElement.TrackVectorPosition != (int)LRVIT->RearTrackVectorPosition)
            {
                if((InternalPrefDirElement.TrackVectorPosition == TrackVectorPosition) && (InternalPrefDirElement.XLinkPos == XLinkPos))
                {
                    PrefDirElement = InternalPrefDirElement;
                    LockedVectorNumber = VectorCount;
                    Utilities->CallLogPop(419);
                    return(true);
                }
            }
            else if(InternalPrefDirElement.TrackVectorPosition == (int)LRVIT->RearTrackVectorPosition)
            {
                if((InternalPrefDirElement.TrackVectorPosition == TrackVectorPosition) && (InternalPrefDirElement.XLinkPos == XLinkPos))
                {
                    PrefDirElement = InternalPrefDirElement;
                    LockedVectorNumber = VectorCount;
                    Utilities->CallLogPop(420);
                    return(true);
                }
                else
                {
                    break; // reached & tested LRVIT->RearTrackVectorPosition for a match so don't want to go any further for this route
                }
            }
        }
        VectorCount++;
    }
    Utilities->CallLogPop(421);
    return(false);
}

// ---------------------------------------------------------------------------

int TAllRoutes::GetRouteVectorNumber(int Caller, IDInt RouteID)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetRouteVectorNumber," + AnsiString(RouteID.GetInt()));
    for(unsigned int x = 0; x < AllRoutesSize(); x++)
    {
        if(GetFixedRouteAt(157, x).RouteID == RouteID.GetInt())
        {
            Utilities->CallLogPop(963);
            return(x);
        }
    }
    throw Exception("Error, failed to find RouteID in GetRouteVectorNumber for ID: " + AnsiString(RouteID.GetInt()));
}

// ---------------------------------------------------------------------------

bool TAllRoutes::IsThereARouteAtIDNumber(int Caller, IDInt RouteID)
// added at v1.3.1 after an error was generated when operating Ian Walker's Chiltern Railway
// found to be due to a route having been removed by a train moving in the wrong direction after the route was selected but before it completed (i.e. route removed while flashing)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",IsThereARouteAtIDNumber," + AnsiString(RouteID.GetInt()));
    for(unsigned int x = 0; x < AllRoutesSize(); x++)
    {
        if(GetFixedRouteAt(45, x).RouteID == RouteID.GetInt())
        {
            Utilities->CallLogPop(2039);
            return(true);
        }
    }
    Utilities->CallLogPop(2040);
    return(false);
}

// ---------------------------------------------------------------------------

const TOneRoute &TAllRoutes::GetFixedRouteAtIDNumber(int Caller, IDInt RouteID) const
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetFixedRouteAtIDNumber," + AnsiString(RouteID.GetInt()));
    for(unsigned int x = 0; x < AllRoutesSize(); x++)
    {
        if(GetFixedRouteAt(163, x).RouteID == RouteID.GetInt())
        {
            Utilities->CallLogPop(964);
            return(GetFixedRouteAt(159, x));
        }
    }
    throw Exception("Error, failed to find RouteID in GetFixedRouteAtIDNumber for ID: " + AnsiString(RouteID.GetInt()));
}

// ---------------------------------------------------------------------------

TOneRoute &TAllRoutes::GetModifiableRouteAtIDNumber(int Caller, IDInt RouteID)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",GetModifiableRouteATIDNumber," + AnsiString(RouteID.GetInt()));
    for(unsigned int x = 0; x < AllRoutesSize(); x++)
    {
        if(GetFixedRouteAt(164, x).RouteID == RouteID.GetInt())
        {
            Utilities->CallLogPop(965);
            return(GetModifiableRouteAt(15, x));
        }
    }
    throw Exception("Error, failed to find RouteID in GetModifiableRouteAtIDNumber for ID: " + AnsiString(RouteID.GetInt()));
}

// ---------------------------------------------------------------------------

void TAllRoutes::SaveRoutes(int Caller, std::ofstream &OutFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",SaveRoutes");
    Utilities->SaveFileInt(OutFile, AllRoutesSize()); // so know how many to reload
    Utilities->SaveFileInt(OutFile, NextRouteID);
    for(unsigned int x = 0; x < AllRoutesSize(); x++)
    {
        TOneRoute OneRoute = GetFixedRouteAt(165, x);
        Utilities->SaveFileInt(OutFile, OneRoute.RouteID);
        OneRoute.SavePrefDirVector(6, OutFile);
    }
    Utilities->CallLogPop(1442);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::LoadRoutes(int Caller, std::ifstream &InFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",LoadRoutes");
    int NumberOfRoutes;

    NumberOfRoutes = Utilities->LoadFileInt(InFile);
    NextRouteID = Utilities->LoadFileInt(InFile);
    for(int x = 0; x < NumberOfRoutes; x++)
    {
        TOneRoute OneRoute; // empty route
        OneRoute.RouteID = Utilities->LoadFileInt(InFile);
        OneRoute.LoadPrefDir(2, InFile);
        if(OneRoute.CheckPrefDirAgainstTrackVectorNoMessage(1))
        {
            StoreOneRouteAfterSessionLoad(0, &OneRoute);
        }
        else
        {
            Utilities->CallLogPop(1443);
            return(false);
        }
    }
    Utilities->CallLogPop(1444);
    return(true);
}

// ---------------------------------------------------------------------------
bool TAllRoutes::CheckRoutes(int Caller, int NumberOfActiveElements, std::ifstream &InFile)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckRoutes," + AnsiString(NumberOfActiveElements));
    int NumberOfRoutes = Utilities->LoadFileInt(InFile);

    if((NumberOfRoutes < 0) || (NumberOfRoutes > 5000))
    {
        Utilities->CallLogPop(1445);
        return(false);
    }
    int NextID = Utilities->LoadFileInt(InFile);

    if((NextID < 0) || (NextID > 1000000))
    {
        Utilities->CallLogPop(1446);
        return(false);
    }
    for(int x = 0; x < NumberOfRoutes; x++)
    {
        int RouteID = Utilities->LoadFileInt(InFile);
        if((RouteID < 0) || (RouteID > 20000))
        {
            Utilities->CallLogPop(1447);
            return(false);
        }
        TOneRoute OneRoute; // create an empty route so CheckOnePrefDir can be called
        if(!(OneRoute.CheckOnePrefDir(3, NumberOfActiveElements, InFile)))
        {
            Utilities->CallLogPop(1448);
            return(false);
        }
    }
    Utilities->CallLogPop(1449);
    return(true);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::CheckForLoopingRoute(int Caller, int EndPosition, int EndXLinkPos, int StartPosition)
{
    // return true for a loop
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",CheckForLoopingRoute," + AnsiString(EndPosition) + "," +
                                 AnsiString(StartPosition));
    if(EndPosition == StartPosition)
    {
        Utilities->CallLogPop(1839);
        return(true); // shouldn't happen but treat as a loop if does
    }
// begin at EndPosition & EndXLinkPos & work forwards until reach end of route (return false) or StartElement (return true)
    int TVPos = EndPosition; //TVPos is the current element and NewTVPos is the element it connects to
    int LkPos = EndXLinkPos; //LkPos is the exit link and NewLkPos is the entry link of the linked element

    while(TrackIsInARoute(15, TVPos, LkPos))
    {
        int NewTVPos = Track->TrackElementAt(826, TVPos).Conn[LkPos]; //see above
        int NewLkPos = -1;
        if(NewTVPos > -1)
        {
            NewLkPos = Track->TrackElementAt(827, TVPos).ConnLinkPos[LkPos]; // this is the entry link pos
            if(NewLkPos == -1)
            {
                Utilities->CallLogPop(1840);
                return(true); // shouldn't arise but treat as loop if does
            }
        }
        else // reached a buffer or continuation
        {
            Utilities->CallLogPop(1841);
            return(false);
        }
//Error found by Xeon notified by email 13/10/20.
//Need to make sure there is a route with the new entry link NewLkPos on the next element (TrackIsInARoute normally used where it doesn't matter which track a route
//is on - except for bridges).  But here a route can end at a trailing point leg or a crossover and if so it doesn't link to the route on the other track, and needs to
//return false.  Without the new check below the program gets stuck in an endless loop, which is the error that Xeon found.
//If there isn't a route at all on the next element then it would return false at the next iteration so can return false here.
//New check added for v2.6.0
//Note: Could probably use GetRouteTypeAndNumber in place of TrackIsInARoute in the while statement above and dispense with this new check, but I prefer to keep mods as simple
//as possible in case there are other unforeseen effects.
        int RouteNumber; //dummy, not used
        if(GetRouteTypeAndNumber(36, NewTVPos, NewLkPos, RouteNumber) == NoRoute)
        {
            Utilities->CallLogPop(2241);
            return(false);
        }
        //now make the connected element the current element, read across the TV number and determine the exit link
        TVPos = NewTVPos;
        if(Track->TrackElementAt(828, TVPos).TrackType == Points)
        {
            if((NewLkPos == 0) || (NewLkPos == 2)) // leading points
            {
                if(Track->TrackElementAt(829, TVPos).Attribute == 0)
                {
                    LkPos = 1;
                }
                else
                {
                    LkPos = 3;
                }
            }
            else
            {
                LkPos = 0;
            }
        }
        else
        {
            LkPos = Track->GetNonPointsOppositeLinkPos(NewLkPos);
        }
        if(TVPos == StartPosition)
        {
            Utilities->CallLogPop(1842);
            return(true); // it is a loop
        }
    }
    Utilities->CallLogPop(1843);
    return(false); // reached end of route so not a loop
}

// ---------------------------------------------------------------------------

bool TAllRoutes::DiagonalFouledByRouteOrTrain(int Caller, int HLoc, int VLoc, int DiagonalLinkNumber)
/*
      Track geometry allows diagonals to cross without occupying the same track element, so when
      route plotting it is necessary to check if there is an existing  route or a train on such a crossing
      diagonal.  Returns true for a fouled diagonal.  Enter with H & V set for the element whose diagonal
      is to be checked, and the XLink number of the relevant diagonal, which must be 1, 3, 7 or 9.
      for XLink = 1, potentially fouled diagonals are at H-1, V, Lk 3 & H, V-1, Lk 7
      for XLink = 3, potentially fouled diagonals are at H+1, V, Lk 1 & H, V-1 Lk 9
      for XLink = 7, potentially fouled diagonals are at H-1, V, Lk 9 & H, V+1 Lk 1
      for XLink = 9, potentially fouled diagonals are at H+1, V, Lk 7 & H, V+1 Lk 3
      Each of these is examined in turn for each route element in the relevant position.

      NOTE:  Originally this failed to detect a train fouling a diagonal.  v1.2.0 checks for a train present on a
      crossing diagonal element using a new bool function TTrack::TrainOnLink(int HLoc, int VLoc, int Link)
      that returns false in all cases (including elements & links not present) except train present.
*/
{
    int TrainID; // not used in this function

    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DiagonalFouledByRouteOrTrain," + AnsiString(HLoc) + "," +
                                 AnsiString(VLoc) + "," + AnsiString(DiagonalLinkNumber));
    TPrefDirElement TempPrefDirElement;
    TAllRoutes::TRouteElementPair FirstPair, SecondPair;

    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(4, HLoc - 1, VLoc, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(50, FirstPair.first).GetFixedPrefDirElementAt(70, FirstPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(310);
            return(true);
        }
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(311);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(51, SecondPair.first).GetFixedPrefDirElementAt(71, SecondPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(312);
            return(true);
        }
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(313);
            return(true);
        }
    }
    if(((DiagonalLinkNumber == 1) && Track->TrainOnLink(0, HLoc - 1, VLoc, 3, TrainID)) || ((DiagonalLinkNumber == 7) && Track->TrainOnLink(1, HLoc - 1, VLoc,
                                                                                                                                            9, TrainID)))
    {
        Utilities->CallLogPop(1997);
        return(true);
    }
    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(5, HLoc, VLoc - 1, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(52, FirstPair.first).GetFixedPrefDirElementAt(72, FirstPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(314);
            return(true);
        }
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(315);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(53, SecondPair.first).GetFixedPrefDirElementAt(73, SecondPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(316);
            return(true);
        }
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(317);
            return(true);
        }
    }
    if(((DiagonalLinkNumber == 1) && Track->TrainOnLink(2, HLoc, VLoc - 1, 7, TrainID)) || ((DiagonalLinkNumber == 3) && Track->TrainOnLink(3, HLoc, VLoc - 1,
                                                                                                                                            9, TrainID)))
    {
        Utilities->CallLogPop(1998);
        return(true);
    }
    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(6, HLoc + 1, VLoc, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(54, FirstPair.first).GetFixedPrefDirElementAt(74, FirstPair.second);
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(318);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(319);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(55, SecondPair.first).GetFixedPrefDirElementAt(75, SecondPair.second);
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(320);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(321);
            return(true);
        }
    }
    if(((DiagonalLinkNumber == 3) && Track->TrainOnLink(4, HLoc + 1, VLoc, 1, TrainID)) || ((DiagonalLinkNumber == 9) && Track->TrainOnLink(5, HLoc + 1, VLoc,
                                                                                                                                            7, TrainID)))
    {
        Utilities->CallLogPop(1999);
        return(true);
    }
    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(7, HLoc, VLoc + 1, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(56, FirstPair.first).GetFixedPrefDirElementAt(76, FirstPair.second);
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(322);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(323);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(57, SecondPair.first).GetFixedPrefDirElementAt(77, SecondPair.second);
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(324);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(325);
            return(true);
        }
    }
    if(((DiagonalLinkNumber == 7) && Track->TrainOnLink(6, HLoc, VLoc + 1, 1, TrainID)) || ((DiagonalLinkNumber == 9) && Track->TrainOnLink(7, HLoc, VLoc + 1,
                                                                                                                                            3, TrainID)))
    {
        Utilities->CallLogPop(2000);
        return(true);
    }
    Utilities->CallLogPop(326);
    return(false);
}

// ---------------------------------------------------------------------------

bool TAllRoutes::DiagonalFouledByRoute(int Caller, int HLoc, int VLoc, int DiagonalLinkNumber)
/*
      As above but checks for a route only (may or may not be a train).  Enter with H & V set for the element whose diagonal
      is to be checked, and the XLink number of the relevant diagonal, which must be 1, 3, 7 or 9.
      for XLink = 1, potentially fouled diagonals are at H-1, V, Lk 3 & H, V-1, Lk 7
      for XLink = 3, potentially fouled diagonals are at H+1, V, Lk 1 & H, V-1 Lk 9
      for XLink = 7, potentially fouled diagonals are at H-1, V, Lk 9 & H, V+1 Lk 1
      for XLink = 9, potentially fouled diagonals are at H+1, V, Lk 7 & H, V+1 Lk 3
      Each of these is examined in turn for each route element in the relevant position.
*/
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",DiagonalFouledByRoute," + AnsiString(HLoc) + "," + AnsiString(VLoc) +
                                 "," + AnsiString(DiagonalLinkNumber));
    TPrefDirElement TempPrefDirElement;
    TAllRoutes::TRouteElementPair FirstPair, SecondPair;

    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(17, HLoc - 1, VLoc, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(197, FirstPair.first).GetFixedPrefDirElementAt(233, FirstPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(2010);
            return(true);
        }
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(2011);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(198, SecondPair.first).GetFixedPrefDirElementAt(234, SecondPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(2012);
            return(true);
        }
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(2013);
            return(true);
        }
    }
    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(18, HLoc, VLoc - 1, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(199, FirstPair.first).GetFixedPrefDirElementAt(235, FirstPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(2014);
            return(true);
        }
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(2015);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(200, SecondPair.first).GetFixedPrefDirElementAt(236, SecondPair.second);
        if((DiagonalLinkNumber == 1) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(2016);
            return(true);
        }
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 9) || (TempPrefDirElement.XLink == 9)))
        {
            Utilities->CallLogPop(2017);
            return(true);
        }
    }
    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(19, HLoc + 1, VLoc, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(201, FirstPair.first).GetFixedPrefDirElementAt(237, FirstPair.second);
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(2018);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(2019);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(202, SecondPair.first).GetFixedPrefDirElementAt(238, SecondPair.second);
        if((DiagonalLinkNumber == 3) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(2020);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 7) || (TempPrefDirElement.XLink == 7)))
        {
            Utilities->CallLogPop(2021);
            return(true);
        }
    }
    FirstPair = AllRoutes->GetRouteElementDataFromRoute2MultiMap(20, HLoc, VLoc + 1, SecondPair);
    if(FirstPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(203, FirstPair.first).GetFixedPrefDirElementAt(239, FirstPair.second);
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(2022);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(2023);
            return(true);
        }
    }
    if(SecondPair.first > -1)
    {
        TempPrefDirElement = AllRoutes->GetFixedRouteAt(204, SecondPair.first).GetFixedPrefDirElementAt(240, SecondPair.second);
        if((DiagonalLinkNumber == 7) && ((TempPrefDirElement.ELink == 1) || (TempPrefDirElement.XLink == 1)))
        {
            Utilities->CallLogPop(2024);
            return(true);
        }
        if((DiagonalLinkNumber == 9) && ((TempPrefDirElement.ELink == 3) || (TempPrefDirElement.XLink == 3)))
        {
            Utilities->CallLogPop(2025);
            return(true);
        }
    }
    Utilities->CallLogPop(2026);
    return(false);
}

// ---------------------------------------------------------------------------


