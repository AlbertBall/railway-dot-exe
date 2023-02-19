//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CallerCheckerUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
/*Ensure:-
1) Have a complete list of source files, including both .h & .cpp in __fastcall TForm1::TForm1
2) 'NumFiles' in header file = number of files
3) Have a complete list of all functions that have callers in __fastcall TForm1::TForm1
4) 'NumberOfCalls' in header file = number of functions
*/

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
        if(!TempFile.is_open())
        {
            throw Exception("Failed to open file " + FileString[x] + " program can't start");
        }
    }

    AnsiString TempFunctionString[NumberOfCalls] =
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
        "AnyLinkedBarrierDownVectorManual",
        "AnyLinkedLevelCrossingElementsWithRoutesOrTrains",
        "ApproachLocking",
        "AreAnyTimesInCurrentEntry",
        "AtLocSuccessor",
        "BiDirectionalPrefDir",
        "BlankElementAt",
        "BufferAtExit",
        "BuildBasicElementFromSpeedTag",
        "BuildContinuationTrainExpectationMultiMap",
        "BuildDatagramFromMap",
        "BuildDynamicMapFromDatagram",
        "BuildGapMapFromTrackVector",
        "BuildTrainDataVectorForLoadFile",
        "BuildTrainDataVectorForValidateFile",
        "BypassPDCrossoverMismatch",
        "CalcDistanceAndSpeed",
        "CalcDistanceToRedSignalandStopTime", //new v2.2.0
        "CalcHLocMinEtc",
        "CalcOperatingAndNotStartedTrainLateness",
        "CalcTimeToAct", //new v2.2.0
        "CallingOnAllowed",
        "CallLogPop",
        "ChangeBackgroundColour",
        "ChangeBackgroundColour2",
        "ChangeBackgroundColour3",
        "ChangeForegroundColour",
        "ChangeLocationNameMultiMapEntry",
        "ChangeSpecificColour",
        "ChangeTrainDirection",
        "CheckAndCancelRouteForWrongEndEntry",
        "CheckAndPopulateListOfIDs",
        "CheckBarriersDownVector",
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
        "CheckNewServiceDepartureTime",
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
        "CheckUserGraphics",
        "Clear",
        "ClearandRebuildRailway",
        "ClearDisplay",
        "ClearEverything",
        "ClearRouteAt",
        "ClearToNextSignal",
        "ClockTimer2",
        "ColNametoNumber",
        "CompileAllEntriesMemoAndSetPointers",
        "ConvertSignalsToOppositeHand",
        "ConsolidatePrefDirs",
        "ConsolidateSARNTArrDep",
        "ConsolidateSARNTAtLoc",
        "ContinuationAutoSignals",
        "ContinuationExit",
        "ControllerCheckNewServiceDepartureTime",
        "ControllerGetNewServiceDepartureInfo",
        "ConvertAndAddNonPreferredRouteSearchVector",
        "ConvertAndAddPreferredRouteSearchVector",
        "ConvertCRLFsToCommas",
        "ConvertIDToPair",
        "ConvertPrefDirSearchVector",
        "ConvertSignalsToOppositeHand",
        "CreateFormattedTimetable",
        "CreateTTAnalysisFile",
        "DecrementPrefDirElementNumbersInPrefDir4MultiMap",
        "DecrementRouteElementNumbersInRoute2MultiMap",
        "DecrementRouteNumbersInRoute2MultiMap",
        "DecrementValuesInGapsAndTrackAndNameMaps",
        "DecrementValuesInInactiveTrackAndNameMaps",
        "Delay",
        "DeleteTextItem",
        "DeleteTrain",
        "DiagonalFouledByRoute",
        "DiagonalFouledByRouteOrTrain",
        "DiagonalFouledByTrain",
        "DisableRouteButtons",
        "DisplayOneTTLineInPanel",
        "DuplicatedLocationName",
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
        "FindLinkingPrefDir",
        "FindLinkingCompatiblePrefDir",
        "FindNamedElementInLocationNameMultiMap",
        "FindNonPlatformMatch",
        "FindRouteNumberFromRoute2MultiMapNoErrors",
        "FindRoutePairFromRoute2MultiMap",
        "FindSetAndDisplayMatchingGap",
        "FindText",
        "FinishedOperation",
        "FinishJoin",
        "FlashingGraphics",
        "FloatingLabelNextString",
        "FloatingTimetableString",
        "ForceCancelRoute",
        "FrontTrainSplit",
        "GapsUnset",
        "GetAnyElementOppositeLinkPos",
        "GetControllerTrainTime",
        "GetExactMatchFrom4MultiMap",
        "GetExitLocationAndAt",
        "GetExitPos",
        "GetFixedPrefDirElementAt",
        "GetFixedRouteAt",
        "GetFixedRouteAtIDNumber",
        "GetFixedSearchElementAt",
        "GetFontStyleAsInt",
        "GetInactiveTrackElementFromTrackMap",
        "GetLeadElement",
        "GetModifiablePrefDirElementAt",
        "GetModifiableRouteAt",
        "GetModifiableRouteAtIDNumber",
        "GetModifiableSearchElementAt",
        "GetNextNonPreferredRouteElement",
        "GetNextPrefDirElement",
        "GetNextPreferredRouteElement",
        "GetNewServiceDepartureInfo",
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
        "GetRouteTypeAndGraphics",
        "GetRouteTypeAndNumber",
        "GetRouteVectorNumber",
        "GetScreenPositionsFromTruePos",
        "GetServiceFromVector",
        "GetTrackElementFromAnyTrackMap",
        "GetTrackElementFromTrackMap",
        "GetTrackLocsFromScreenPos",
        "GetTrackVectorIteratorFromNamePosition",
        "GetTrackVectorPositionFromString",
        "GetTrainFloatingInfoFromContinuation",
        "GetTrainHeadCode",
        "GetTrainIDOrContinuationPosition",
        "GetTrainStatusFloat",
        "GetTrainTime",
        "GetTruePositionsFromScreenPos",
        "GetVectorPositionFromTrackMap",
        "GetVectorPositionsFromInactiveTrackMap",
        "GetVectorPositionsFromPrefDir4MultiMap",
        "HideTTActionsListBox",
        "HideWarningLog",
        "HighlightOneEntryInAllEntriesTTListBox",
        "HighLightOneGap",
        "InactiveMapCheck",
        "InactiveTrackElementAt",
        "InactiveTrackElementPresentAtHV",
        "InvertElement",
        "IsATrackElementAdjacentToLink",
        "IsBarrierDownVectorAtHVManual",
        "IsBecomeNewServiceAvailable",
        "IsElementDefaultLength",
        "IsElementInLockedRouteGetPrefDirElementGetLockedVectorNumber",
        "IsNamedNonStationLocationPresent",
        "IsLCAtHV",
        "IsLCBarrierDownAtHV",
        "IsLCBarrierFlashingAtHV",
        "IsLCBarrierUpAtHV",
        "IsPerformancePanelObscuringFloatingLabel",
        "IsPlatformOrNamedLocationPresent",
        "IsSNTEntryLocated",
        "IsThereAnAdjacentTrain",
        "IsTrackLinked",
        "IsTrainIDOnBridgeTrackPos01",
        "IsTrainIDOnBridgeTrackPos23",
        "IsTrainTerminating",
        "JoinedBy",
        "JoinMultiplayerSessionMenuItemClick",
        "LastElementNumber",
        "LastElementPtr",
        "Last2CharactersBothDigits",
        "LCInSearchVector",
        "LengthMarker",
        "LinkOccupied",
        "LinkTrack",
        "LinkTrackNoMessages",
        "LoadBarriersDownVector",
        "LoadClipboard",
        "LoadConfigFile",
        "LoadGraphics",
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
        "LoadUserGraphic",
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
        "MPCPCancelButtonClick",
        "MPCPHostIPEditBoxKeyUp",
        "MPCPHostPortEditBoxKeyUp",
        "MPCPPlayerNameEditBoxKeyUp",
        "MPCPReadyToBeginButtonClick",
        "MPCPSendButtonClick",
        "MPHPCancelButtonClick",
        "MPHPLoadCouplingFileButtonClick",
        "MPHPOwnIPEditBoxKeyUp",
        "MPHPOwnPortEditBoxKeyUp",
        "MPHPStartButtonClick",
        "MultiplayerHostSessionClick",
        "MultiplayerRailwayValid",
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
        "NumberOfPlatforms",
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
        "PlotLCBaseElementsOnly",
        "PlotLoweredLinkedLevelCrossingBarriers",
        "PlotOriginal",
        "PlotOutput",
        "PlotOverlay",
        "PlotPastedTrackElementWithAttributes",
        "PlotPlainLoweredLinkedLevelCrossingBarriersAndSetMarkers",
        "PlotPlainRaisedLinkedLevelCrossingBarriersAndSetMarkers",
        "PlotPointBlank",
        "PlotPoints",
        "PlotRaisedLinkedLevelCrossingBarriers",
        "PlotRouteOriginal",
        "PlotRouteOverlay",
        "PlotSignal",
        "PlotSignalPlatforms",
        "PlotSignalBlank",
        "PlotSmallFlashingLinkedLevelCrossings",
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
        "PopulateCouplingMap",
        "PopulateHVPairsLinkedMapAndNoDuplicates",
        "PopulateLCVector",
        "PopulateSimpleVector",
        "PrefDirMarker",
        "PresetAutoRouteElementValid",
        "ProcessOneTimetableLine",
        "RealignAfterTrackErase",
        "RearTrainSplit",
        "RebuildLocationNameMultiMap",
        "RebuildOpTimeToActMultimap", //new v2.2.0
        "RebuildPrefDirVector",
        "RebuildTrack",
        "RebuildUserGraphics",
        "ReclaimSignalsForNonAutoSigRoutes",
        "RecoverClipboard",
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
        "ResetLevelCrossings",
        "ResetPoints",
        "ResetSignals",
        "ResetTSRs",
        "ResetTrainElementID",
        "RetrieveStripedNamedLocationGraphicsWhereRelevant",
        "ReturnNextInactiveTrackElement",
        "ReturnNextTrackElement",
        "RevertToOriginalRouteSelector",
        "Route2MultiMapInsert",
        "RouteImageMarker",
        "RouteLockingRequired",
        "SameDirection",
        "SaveAsSubroutine",
        "SaveConfigFile",
        "SaveInterface",
        "SaveOneSessionTrain",
        "SavePerformanceFile",
        "SavePrefDir",
        "SaveRoutes",
        "SaveSearchVector",
        "SaveSession",
        "SaveSessionBarriersDownVector",
        "SaveSessionContinuationAutoSigEntries",
        "SaveSessionLockedRoutes",
        "SaveSessionTrains",
        "SaveTempTimetableFile",
        "SaveTimetableToSessionFile",
        "SaveTrack",
        "SaveTrainDataVectorToFile",
        "SaveUserGraphics",
        "SearchAllRoutesAndTruncate",
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
        "SetBarriersDownLCToManual",
        "SetCaption",
        "SetElementID",
        "SetFontStyleFromInt",
        "SetHeadCodeGraphics",
        "SetInitialPrefDirModeEditMenu",
        "SetInitialTrackModeEditMenu",
        "SetLCAttributeAtHV",
        "SetLCChangeValues",
        "SetLevel1Mode",
        "SetLevel2OperMode",
        "SetLevel2PrefDirMode",
        "SetLevel2TrackMode",
        "SetLinkedLevelCrossingBarrierAttributes",
        "SetLinkedManualLCs",
        "SetPausedOrZoomedInfoCaption",
        "SetRearwardsSignalsReturnFalseForTrain",
        "SetRemainingSearchVectorValues",
        "SetRouteButtonsInfoCaptionAndRouteNotStarted",
        "SetRouteFlashValues",
        "SetRoutePoints",
        "SetRouteSearchVectorGraphics",
        "SetRouteSignals",
        "SetSaveMenuAndButtons",
        "SetScreenHVSource",
        "SetStationEntryStopLinkPosses",
        "SetTopIndex",
        "SetTrackBuildImages",
        "SetTrackLengths",
        "SetTrackModeEditMenu",
        "SetTrailingSignalsOnAutoSigsRoute",
        "SetTrailingSignalsOnContinuationRoute",
        "SetTrainElementID",
        "SetTrainMovementValues",
        "ShowSelectedGap",
        "ShowTTActionsListBox",
        "ShowWarningLog",
        "SkipAllEventsBeforeNewService",
        "SkipEventsBeforeSameLoc",
        "SignallerChangeTrainDirection",
        "SignalHasFailed",
        "SignallerControl",
        "SingleServiceOutput",
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
        "TextFound",
        "TextOut",
        "ThisNamedLocationLongEnoughForSplit",
        "TimetabledLocationNameAllocated",
        "TimetableIntegrityCheck",
        "TimetableMessage",
        "TrackClear",
        "TrackElementAt",
        "TrackElementPresentAtHV",
        "TrackIsInARoute",
        "TrackPush",
        "TrackTrainFloat",
        "TrainAtLocation",
        "TrainExistsAtIdent",
        "TrainHasFailed",
        "TrainOnContinuation",
        "TrainOnLink",
        "TrainToBeJoinedByIsAdjacent",
        "TrainToJoinIsAdjacent",
        "TrainVectorAt",
        "TrainVectorAtIdent",
		"TruncateRoute",
        "TryToConnectTrack",
        "TTrain",
        "UpdateOperatorActionPanel", //new v2.2.0
        "UnplotTrain",
        "UnplotTrainInZoomOutMode",
        "UnplotTrains",
        "UpdateDynamicMapFromTimeToExitMultiMap",
        "UpdateTrain",
        "UserGraphicMove",
        "UserGraphicPresentAtHV",
        "UserGraphicVectorAt",
        "ValidatePrefDir",
        "WarningLog",
        "WithinTimeRange",
        "WriteAllRoutesToImage",
        "WriteGraphicsToImage",
        "WriteOperatingTrackToImage",
        "WritePrefDirToImage",
        "WriteTrackAndTextToImage",
        "WriteTrainsToImage",
        "WriteTrainToImage"
    };

    for(int x = 0; x < NumberOfCalls; x++)
    {
        FunctionString[x] = TempFunctionString[x];
    }
    RadioButton2->Checked = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::StartAnalysisClick(TObject *Sender)
{
    StartAnalysis->Enabled = false;
    if((RadioButton2->Checked) || (RadioButton3->Checked))
    {
        OutFile.open("CallerOutput.txt", std::ios_base::trunc); //to truncate the file
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
    for(int x=0; x<5000; x++)
    {
        DupNumArray[x] = -1;
        MissingNumArray[x] = -1;
    }
    Label1->Caption = "Analysing function:   " + FunctionString + " (" + AnsiString(FunctionCount + 1) + "/" + AnsiString(NumberOfCalls) + ")";
    Form1->Repaint();
    int DupNumArrayIndex = 0;
    for(int x=0; x<NumFiles; x++)
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
            for(int x = 1; x< OneLine.Length() - FunctionString.Length() + 1; x++)
            {
                if(OneLine.SubString(x,FunctionString.Length() + 1) == FunctionString + '(')
                //rule out other functions that contain the sought function name (allow to be preceded by space, !, ., >, [ or (
                {
                    if((x == 1) || (OneLine[x-1] < '"') || (OneLine[x-1] == '.') || (OneLine[x-1] == '>') || (OneLine[x-1] == '[') || (OneLine[x-1] == '('))
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
    int MaxIndex = -1; //so first brings it to 0
    int MaxNumber = -1;
    for(int x=0; x<5000; x++)
    {
        if(DupNumArray[x] > -1)
        {
            MaxIndex++;
            if(DupNumArray[x] > MaxNumber)
            {
                MaxNumber = DupNumArray[x];
            }
        }
        else
        {
            break;
        }
    }
    for(int x=0; x<MaxIndex+1; x++)
    {
        MissingNumArray[x] = DupNumArray[x];
    }
//check for duplicates
    bool FirstPass = true;
    for(int x=0; x<MaxIndex + 1; x++)
    {
        int CheckNumber = DupNumArray[x];
        if(CheckNumber == -1)
        {
            continue;
        }
        int RepeatCount = 0;
        for(int y=x+1; y<MaxIndex + 1; y++)
        {
            if(DupNumArray[y] == CheckNumber)
            {
                RepeatCount++;
                DupNumArray[y] = -1; //so it isn't counted again if appears more than twice in all
            }
        }
        if(FirstPass)
        {
            UnicodeString MessageString = FunctionString + L":  max caller number = " + (UnicodeString)MaxNumber;
            if(Print)
            {
                OutFile << AnsiString(MessageString).c_str() << char(10);
            }
            FirstPass = false;
        }
        if(RepeatCount > 0)
        {
            UnicodeString MessageString = L"--->>>Number " + UnicodeString(CheckNumber) + L" repeated " + UnicodeString(RepeatCount) + L" times in function " + UnicodeString(FunctionString);
            if(!RadioButton2->Checked)
            {
                if(Screen)
                {
                    int button = Application->MessageBox(MessageString.c_str(), L"", MB_OKCANCEL);
                    if (button == IDCANCEL)
                    {
                        BreakFlag = true;
                        return;
                    }
                }
            }
            if(Print)
            {
                OutFile << AnsiString(MessageString).c_str() << char(10);
            }
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
            for(int x=0; x<MaxIndex + 1; x++)
            {
                if(MissingNumArray[x] == IncNumber)
                {
                    FoundFlag = true;
                    break;
                }
            }
            if(!FoundFlag)
            {
                UnicodeString MessageString = L"Number " + UnicodeString(IncNumber) + L" missing for function " + UnicodeString(FunctionString);
                if(Screen)
                {
                    int button = Application->MessageBox(MessageString.c_str(), L"", MB_OKCANCEL);
                    if (button == IDCANCEL)
                    {
                        BreakFlag = true;
                        return;
                    }
                }
                if(Print)
                {
                    OutFile << AnsiString(MessageString).c_str() << char(10);
                }
            }
            IncNumber++;
        }
    }
    FunctionCount++;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::ExitClick(TObject *Sender)
{
    if(OutFile.is_open())
    {
        OutFile.close();
    }
    Application->Terminate();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    if(ClockStopped)
    {
        return;
    }
    ClockStopped = true;

    Screen = false;
    Print = false;
    if(RadioButton1->Checked)
    {
        Screen = true;
    }
    if(RadioButton2->Checked)
    {
        Print = true;
    }
    if(RadioButton3->Checked)
    {
        Screen = true;
        Print = true;
    }

    if(Print && !OutFile.is_open())
    {
        OutFile.open("CallerOutput.txt", std::ios_base::app);
        if(!OutFile.is_open())
        {
            ShowMessage("Output file failed to open - CallerOutput.txt");
            return;
        }
    }

    if(Count < NumberOfCalls)
    {
        AnalyseOneFunction(Count, NumberOfCalls, FunctionString[Count], OutFile);
    }

    if(BreakFlag)
    {
        Label1->Caption = "";
        Form1->Repaint();
        StartAnalysis->Enabled = true;
        Exit->Enabled = true;
        if(Print && OutFile.is_open())
        {
            OutFile.close();
        }
    }

    else if(Count >= NumberOfCalls)
    {
        Label1->Caption = "";
        Form1->Repaint();
        ShowMessage("Finished");
        if(Print && OutFile.is_open())
        {
            OutFile.close();
        }
        StartAnalysis->Enabled = true;
        Exit->Enabled = true;
    }
    else
    {
        ClockStopped = false;
    }
}
//---------------------------------------------------------------------------


