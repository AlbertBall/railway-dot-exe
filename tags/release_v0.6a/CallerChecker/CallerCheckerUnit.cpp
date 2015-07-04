//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CallerCheckerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
ClockStopped = true;
AnsiString TempFileString[NumFiles] =
    {
    "InterfaceUnit.cpp",
    "GraphicUnit.cpp",
    "DisplayUnit.cpp",
    "TrackUnit.cpp",
    "TextUnit.cpp",
    "TrainUnit.cpp",
    "Utilities.cpp",
    "InterfaceUnit.h",
    "GraphicUnit.h",
    "DisplayUnit.h",
    "TrackUnit.h",
    "TextUnit.h",
    "TrainUnit.h",
    "Utilities.h",
    };
for(int x = 0; x < NumFiles; x++)
    {
    FileString[x] = TempFileString[x];
    std::ifstream TempFile(FileString[x].c_str());
    if(!TempFile)
        {
        throw Exception("Failed to open file " + FileString[x] + " program can't start");
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::StartAnalysisClick(TObject *Sender)
{
StartAnalysis->Enabled = false;
if(RadioButton1->Checked) Exit->Enabled = false;
if(RadioButton3->Checked) Exit->Enabled = false;
if((RadioButton2->Checked) || (RadioButton3->Checked))
	{
	std::ofstream OutFile("CallerOutput.txt");//to truncate the file
	OutFile.close();
	}
Count = 0;
ClockStopped = false;
BreakFlag = false;
}
//---------------------------------------------------------------------------

void TForm1::AnalyseOneFunction(int &FunctionCount, int NumberOfCalls, AnsiString FunctionString, std::ofstream &OutFile)
{
AnsiString Prelim = "./";
for(int x=0;x<5000;x++)
    {
    DupNumArray[x] = -1;
    MissingNumArray[x] = -1;
    }
Label1->Caption = "Analysing function:   " + FunctionString + " (" + AnsiString(FunctionCount + 1) + "/" + AnsiString(NumberOfCalls) + ")";
Form1->Repaint();
int DupNumArrayIndex = 0;
for(int x=0;x<NumFiles;x++)
    {
    AnsiString FullFileName = Prelim + FileString[x];
    std::ifstream InFile(FullFileName.c_str());
    if(InFile == 0)
        {
        ShowMessage("Input file failed to open - " + FileString[x]);
        return;
        }
    char Buffer[5000];
    bool EndOfFile = false;
    while(!EndOfFile)
        {
        InFile.getline(Buffer, 5000);
        AnsiString OneLine(Buffer);
        for(int x = 1; x< OneLine.Length() - FunctionString.Length() + 1;x++)
            {
            if(OneLine.SubString(x,FunctionString.Length() + 1) == FunctionString + '(')
            //rule out other functions that contain the sought function name (allow to be preceded by space, !, ., > or (
                {if((x == 1) || (OneLine[x-1] < '"') || (OneLine[x-1] == '.') || (OneLine[x-1] == '>') || (OneLine[x-1] == '('))
                    {
                    AnsiString NumberLineStr = OneLine.SubString(x + FunctionString.Length() + 1,OneLine.Length() - (x + FunctionString.Length() + 1));
                    AnsiString NumberString = "";
                    while((NumberLineStr.Length() > 0) && ((NumberLineStr[1] <= '9') && (NumberLineStr[1] >= '0')))
                        {
                        NumberString+= NumberLineStr[1];
                        NumberLineStr.Delete(1,1);
                        }
                    if(NumberString != "")
                        {
                        int Number = NumberString.ToInt();
                        DupNumArray[DupNumArrayIndex] = Number;
                        DupNumArrayIndex++;
                        }
                    }
                }
            }
        if(InFile.eof())
            {
            EndOfFile = true;
            }
        }
    InFile.close();
    }
//Label1->Caption = "Array analysis started for function " + FunctionString;
//Form1->Repaint();
int MaxIndex = -1;//so first brings it to 0
int MaxNumber = -1;
for(int x=0;x<5000;x++)
    {
    if(DupNumArray[x] > -1)
        {
        MaxIndex++;
        if(DupNumArray[x] > MaxNumber) MaxNumber = DupNumArray[x];
        }
    else break;
    }
for(int x=0;x<MaxIndex+1;x++)
    {
    MissingNumArray[x] = DupNumArray[x];
    }
//check for duplicates
for(int x=0;x<MaxIndex + 1;x++)
    {
    int CheckNumber = DupNumArray[x];
    if(CheckNumber == -1) continue;
    int RepeatCount = 0;
    for(int y=x+1;y<MaxIndex + 1;y++)
        {
        if(DupNumArray[y] == CheckNumber)
            {
            RepeatCount++;
            DupNumArray[y] = -1;//so it isn't counted again if appears more than twice in all
            }
        }
    if(RepeatCount > 0)
        {
        AnsiString MessageString = "Number " + AnsiString(CheckNumber) + " repeated " + AnsiString(RepeatCount) + " times in function " + FunctionString;
        if(Screen)
            {
            int button = Application->MessageBox(MessageString.c_str(), "", MB_OKCANCEL);
            if (button == IDCANCEL)
                {
                BreakFlag = true;
                return;
                }
            }
        if(Print) OutFile << MessageString.c_str() << CHAR(13) << CHAR(10);
        }
    }

//now check for missing numbers
if(CheckBox1->Checked)
    {
    int IncNumber = 0;
    bool FoundFlag=false;
    while(IncNumber <= MaxNumber)
        {
        FoundFlag = false;
        for(int x=0;x<MaxIndex + 1;x++)
            {
            if(MissingNumArray[x] == IncNumber)
                {
                FoundFlag = true;
                break;
                }
            }
        if(!FoundFlag)
            {
            AnsiString MessageString = "Number " + AnsiString(IncNumber) + " missing for function " + FunctionString;
            if(Screen)
                {
                int button = Application->MessageBox(MessageString.c_str(), "", MB_OKCANCEL);
                if (button == IDCANCEL)
                    {
                    BreakFlag = true;
                    return;
                    }
                }
            if(Print) OutFile << MessageString.c_str() << CHAR(13) << CHAR(10);
            }
        IncNumber++;
        }
    }
FunctionCount++;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::ExitClick(TObject *Sender)
{
Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
if(ClockStopped) return;
ClockStopped = true;
const int NumberOfCalls = 390;
AnsiString FunctionString[NumberOfCalls] =
{
"AbleToMove",
"AbleToMoveButForSignal",
"ActiveMapCheck",
"AddLocationNameText",
"AddName",
"AddRouteElement",
"AddTrain",
"AdjElement",
"AdjNamedElement",
"AllFormattedTrainsAt",
"ApproachLocking",
"AreAnyTimesInCurrentEntry",
"AtLocSuccessor",
"BlankElementAt",
"BufferAtExit",
"BuildContinuationTrainExpectationMultiMap",
"BuildGapMapFromTrackVector",
"BuildTrainDataVectorForLoadFile",
"BuildTrainDataVectorForValidateFile",
"CalcDistanceAndSpeed",
"CalcHLocMinEtc",
"CallingOnAllowed",
"CallLogPop",
"ChangeBackgroundColour",
"ChangeForegroundColour",
"ChangeLocationNameMultiMapEntry",
"ChangeSpecificColour",
"ChangeTrainDirection",
"CheckAndCancelRouteForWrongEndEntry",
"CheckAndPopulateListOfIDs",
"CheckCrossReferencesAndSetData",
"CheckFootbridgeLinks",
"CheckForDuplicateCrossReferences",
"CheckGapMap",
"CheckHeadCodeValidity",
"CheckInterface",
"CheckLocationNameMultiMap",
"CheckLocationValidity",
"CheckMapAndInactiveTrack",
"CheckMapAndRoutes",
"CheckMapAndTrack",
"CheckNonRepeatingShuttleLinksAndSetData",
"CheckNonRepeatingShuttleLinkTime",
"CheckOldTrackElementsInFile",
"CheckOnePrefDir",
"CheckPerformanceFile",
"CheckPrefDir4MultiMap",
"CheckPrefDirAgainstTrackVector",
"CheckPrefDirAgainstTrackVectorNoMessage",
"CheckRoutes",
"CheckSessionContinuationAutoSigEntries",
"CheckSessionLockedRoutes",
"CheckSessionTrains",
"CheckShuttleRepeatTime",
"CheckShuttleServiceIntegrity",
"CheckStartAllowable",
"CheckStartPositionValidity",
"CheckTimetableFromSessionFile",
"CheckTimeValidity",
"CheckTrackElementsInFile",
"Clear",
"ClearandRebuildRailway",
"ClearDisplay",
"ClearEverything",
"ClearRouteAt",
"ClearToNextSignal",
"ClockTimer2",
"CompileAllEntriesMemoAndSetPointers",
"ConsolidatePrefDirs",
"ContinuationAutoSignals",
"ContinuationExit",
"ConvertAndAddNonPreferredRouteSearchVector",
"ConvertAndAddPreferredRouteSearchVector",
"ConvertCRLFsToCommas",
"ConvertPrefDirSearchVector",
"CreateFormattedTimetable",
"DecrementPrefDirElementNumbersInPrefDir4MultiMap",
"DecrementRouteElementNumbersInRoute2MultiMap",
"DecrementRouteNumbersInRoute2MultiMap",
"DecrementValuesInGapsAndTrackAndNameMaps",
"DecrementValuesInInactiveTrackAndNameMaps",
"Delay",
"DeleteTextItem",
"DeleteTrain",
"DisableRouteButtons",
"DisplayOneTTLineInPanel",
"ElementInLNDone2MultiMap",
"ElementInLNPendingList",
"Ellipse",
"EndPossible",
"EnterLocationName",
"EntryPos",
"EraseFromPrefDirVectorAnd4MultiMap",
"EraseLocationAndActiveTrackElementNames",
"EraseLocationNameText",
"ErasePrefDirElementAt",
"EraseTrackElement",
"ErrorInTrackBeforeSetGaps",
"ErrorLog",
"EveryPrefDirMarker",
"ExternalStorePrefDirElement",
"FileIntegrityCheck",
"FindAndHighlightAnUnsetGap",
"FindClosestLinkPosition",
"FindForwardTargetSignalAttribute",
"FindHighestLowestAndLeftmostNamedElements",
"FindNamedElementInLocationNameMultiMap",
"FindNonPlatformMatch",
"FindRoutePairFromRoute2MultiMap",
"FindSetAndDisplayMatchingGap",
"FinishedOperation",
"FinishJoin",
"FlashingGraphics",
"FloatingLabelNextString",
"FloatingTimetableString",
"ForceCancelRoute",
"FouledDiagonal",
"FrontTrainSplit",
"GapsUnset",
"GetAllRoutesTruncateElement",
"GetAnyElementOppositeLinkPos",
"GetExactMatchFrom4MultiMap",
"GetExitLocationAndAt",
"GetExitPos",
"GetFixedPrefDirElementAt",
"GetFixedRouteAt",
"GetFixedRouteAtIDNumber",
"GetFixedSearchElementAt",
"GetFontStyleAsInt",
"GetLeadElement",
"GetModifiablePrefDirElementAt",
"GetModifiableRouteAt",
"GetModifiableRouteAtIDNumber",
"GetModifiableSearchElementAt",
"GetNextNonPreferredRouteElement",
"GetNextPrefDirElement",
"GetNextPreferredRouteElement",
"GetNonPreferredRouteStartElement",
"GetOffsetValues",
"GetOnePrefDirPosition",
"GetPrefDirStartElement",
"GetPrefDirTruncateElement",
"GetPreferredRouteStartElement",
"GetRectangle",
"GetRepeatHeadCode",
"GetRepeatTime",
"GetRouteElementDataFromRoute2MultiMap",
"GetRouteTruncateElement",
"GetRouteTypeAndGraphics",
"GetRouteTypeAndNumber",
"GetRouteVectorNumber",
"GetScreenPositionsFromTruePos",
"GetTrackLocsFromScreenPos",
"GetTrackVectorIteratorFromNamePosition",
"GetTrackVectorPositionFromString",
"GetTrainHeadCode",
"GetTrainTime",
"GetTruePositionsFromScreenPos",
"GetVectorPositionFromTrackMap",
"GetVectorPositionsFromInactiveTrackMap",
"GetVectorPositionsFromPrefDir4MultiMap",
"HideWarningLog",
"HighlightOneEntryInAllEntriesTTListBox",
"HighLightOneGap",
"InactiveMapCheck",
"InactiveTrackElementAt",
"InvertElement",
"IsATrackElementAdjacentToLink",
"IsElementDefaultLength",
"IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber",
"IsNamedNonStationLocationPresent",
"IsPerformancePanelObscuringFloatingLabel",
"IsPlatformOrNamedLocationPresent",
"IsSNTEntryLocated",
"IsTrackLinked",
"IsTrainIDOnBridgeTrackPos01",
"IsTrainIDOnBridgeTrackPos23",
"IsTrainTerminating",
"JoinedBy",
"LastElementNumber",
"LastElementPtr",
"LengthMarker",
"LinkTrack",
"LinkTrackNoMessages",
"LoadGroundSignalGlyphs",
"LoadInterface",
"LoadNormalSignalGlyphs",
"LoadOldPrefDir",
"LoadOldTrack",
"LoadOneSessionTrain",
"LoadOriginalExistingGraphic",
"LoadOriginalScreenGraphic",
"LoadOverlayGraphic",
"LoadPerformanceFile",
"LoadPrefDir",
"LoadRailway",
"LoadRoutes",
"LoadSession",
"LoadSessionContinuationAutoSigEntries",
"LoadSessionLockedRoutes",
"LoadSessionTrains",
"LoadTimetable",
"LoadTimetableFromSessionFile",
"LoadTrack",
"LocationNameAllocated",
"LocationsNotNamed",
"LogAction",
"LogActionError",
"LowEntryValue",
"MainScreenMouseDown2",
"MainScreenMouseDown3",
"MarkAllRoutes",
"MarkOneLength",
"MatchingPoint",
"MovingSuccessor",
"MovingTrainPresentOnFlashingRoute",
"NamedLocationElementAt",
"NameInTimetableBeforeCDT",
"NewShuttleFromNonRepeatService",
"NewTrainService",
"NoActiveOrInactiveTrack",
"NoActiveTrack",
"NoGaps",
"NoNamedLocationElements",
"NonFootbridgeNamedLocationExists",
"NumberOfGaps",
"OldTextOut",
"OneCompleteFormattedTrainVectorAt",
"OneFormattedTrainVectorAt",
"OneNamedLocationElementAtLocation",
"OneNamedLocationLongEnoughForSplit",
"Operate",
"OtherTrainOnTrack",
"PerformanceLog",
"PickUpBackgroundBitmap",
"PlatformOnSignalSide",
"PlotAbsolute",
"PlotAllTrainsInZoomOutMode",
"PlotAlternativeTrackRouteGraphic",
"PlotAndAddTrackElement",
"PlotBackgroundGraphic",
"PlotBlank",
"PlotDashedRect",
"PlotFixedTrackElement",
"PlotGap",
"PlotOriginal",
"PlotOutput",
"PlotOverlay",
"PlotPointBlank",
"PlotPoints",
"PlotRouteOriginal",
"PlotRouteOverlay",
"PlotSignal",
"PlotSignalBlank",
"PlotSmallOutput",
"PlotSmallRailway",
"PlotSmallRedGap",
"PlotStartPosition",
"PlotTrain",
"PlotTrainGraphic",
"PlotTrainInZoomOutMode",
"PlotTrainWithNewBackgroundColour",
"PlotVariableTrackElement",
"PointsToBeChanged",
"PrefDirMarker",
"ProcessOneTimetableLine",
"RealignAfterTrackErase",
"RearTrainSplit",
"RebuildLocationNameMultiMap",
"RebuildPrefDirVector",
"RebuildTrack",
"Rectangle",
"RemainHere",
"RemoveRouteElement",
"RepeatShuttleOrNewNonRepeatService",
"RepeatShuttleOrRemainHere",
"ReplotTrains",
"RepositionAndMapTrack",
"ResetAll",
"ResetAllTrainIDElements",
"ResetAnyNonMatchingGaps",
"ResetChangedFileDataAndCaption",
"ResetConnClkCheckUnsetGapJumps",
"ResetCurrentSpeedButton",
"ResetGapsFromGapMap",
"ResetPoints",
"ResetSignals",
"ResetTrainElementID",
"RetrieveStripedNamedLocationGraphicsWhereRelevant",
"ReturnNextInactiveTrackElement",
"ReturnNextTrackElement",
"RevertToOriginalRouteSelector",
"Route2MultiMapInsert",
"RouteImageMarker",
"RouteLockingRequired",
"SaveAsSubroutine",
"SaveInterface",
"SaveOneSessionTrain",
"SavePerformanceFile",
"SavePrefDir",
"SaveRoutes",
"SaveSearchVector",
"SaveSession",
"SaveSessionContinuationAutoSigEntries",
"SaveSessionLockedRoutes",
"SaveSessionTrains",
"SaveTempTimetableFile",
"SaveTimetableToSessionFile",
"SaveTrack",
"SaveTrainDataVectorToFile",
"SearchForAndUpdateLocationName",
"SearchForNonPreferredRoute",
"SearchForPrefDir",
"SearchForPreferredRoute",
"SecondPassActions",
"SelectVectorAt",
"SendMissedActionLogs",
"SendPerformanceSummary",
"SessionFileIntegrityCheck",
"SetAllDefaultLengths",
"SetAllRearwardsSignals",
"SetCaption",
"SetElementID",
"SetFontStyleFromInt",
"SetHeadCodeGraphics",
"SetInitialPrefDirModeEditMenu",
"SetInitialTrackModeEditMenu",
"SetLevel1Mode",
"SetLevel2OperMode",
"SetLevel2PrefDirMode",
"SetLevel2TrackMode",
"SetPausedOrZoomedInfoCaption",
"SetRearwardsSignalsReturnFalseForTrain",
"SetRemainingSearchVectorValues",
"SetRouteButtonsInfoCaptionAndRouteNotStarted",
"SetRouteFlashValues",
"SetRouteSearchVectorGraphics",
"SetRouteSignalsAndPoints",
"SetRouteSignalsOnly",
"SetSaveMenuAndButtons",
"SetScreenHVSource",
"SetStationEntryStopLinkPosses",
"SetTrackBuildImages",
"SetTrackLengths",
"SetTrailingSignalsOnAutoSigsRoute",
"SetTrailingSignalsOnContinuationRoute",
"SetTrainElementID",
"SetTrainMovementValues",
"ShowSelectedGap",
"ShowWarningLog",
"SignallerChangeTrainDirection",
"SignallerControl",
"SplitEntry",
"SplitRepeat",
"SplitTrainInfo",
"StopTTClockMessage",
"StoreOneRoute",
"StoreOneRouteAfterSessionLoad",
"StorePrefDirElement",
"StripExcessFromHeadCode",
"StripSpaces",
"TestRoutine",
"TextOut",
"ThisNamedLocationLongEnoughForSplit",
"TimetableIntegrityCheck",
"TimetableMessage",
"TrackClear",
"TrackElementAt",
"TrackIsInARoute",
"TrackPush",
"TrackTrainFloat",
"TrainAtLocation",
"TrainToBeJoinedByIsAdjacent",
"TrainToJoinIsAdjacent",
"TrainVectorAt",
"TrainVectorAtIdent",
"TryToConnectTrack",
"TTrain",
"UnplotTrain",
"UnplotTrainInZoomOutMode",
"UnplotTrains",
"UpdateTrain",
"ValidatePrefDir",
"WarningLog",
"WriteAllRoutesToImage",
"WriteOperatingTrackToImage",
"WritePrefDirToImage",
"WriteTrackToImage",
"WriteTrainsToImage",
"WriteTrainToImage"
};

Screen = false;
Print = false;
if(RadioButton1->Checked) Screen = true;
if(RadioButton2->Checked) Print = true;
if(RadioButton3->Checked)
    {
    Screen = true;
    Print = true;
    }

std::ofstream OutFile;
if(Print && !OutFile.is_open())
    {
    OutFile.open("CallerOutput.txt", std::ios_base::app);
    if(OutFile == 0)
        {
        ShowMessage("Output file failed to open - CallerOutput.txt");
        return;
        }
    }

if(Count < NumberOfCalls) AnalyseOneFunction(Count, NumberOfCalls, FunctionString[Count], OutFile);

if(BreakFlag)
    {
    Label1->Caption = "";
    Form1->Repaint();
    StartAnalysis->Enabled = true;
    Exit->Enabled = true;
    if(Print) OutFile.close();
    }

else if(Count >= NumberOfCalls)
    {
    Label1->Caption = "";
    Form1->Repaint();
    ShowMessage("Finished");
    if(Print) OutFile.close();
    StartAnalysis->Enabled = true;
    Exit->Enabled = true;
    }
else ClockStopped = false;
}
//---------------------------------------------------------------------------

    