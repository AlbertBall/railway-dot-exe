//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ActionsDueUnit.h"
#include "InterfaceUnit.h"
#include "TrainUnit.h"
#include "TrackUnit.h"
#include "PerfLogUnit.h"
#include "Utilities.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TActionsDueForm *ActionsDueForm;
//---------------------------------------------------------------------------
__fastcall TActionsDueForm::TActionsDueForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TActionsDueForm::FormCreate(TObject *Sender)
{
    ActionsDueForm->Visible = false;
    ActionsDueForm->Width = 106;
    ActionsDueForm->Height = 324;
    ActionsDueForm->Top = Screen->Height - ActionsDueForm->Height - 32; //-32 to avoid overlapping taskbar;;
    ActionsDueForm->Left = Screen->Width - ActionsDueForm->Width;
    ActionsDuePanel->Top = 0; //relative to form
    ActionsDuePanel->Left = 0;
    ActionsDuePanel->Width = ActionsDueForm->Width - 6; //form has to be 6 wider than content to display content correctly
    ActionsDuePanel->Height = ActionsDueForm->Height - 26; //to allow for title bar
    ActionsDuePanel->Color = TColor(0xCCCCCC); // new v2.2.0 as above
    ActionsDueListBox->Width = 78;
    ActionsDueListBox->Height = 270;
    ActionsDueListBox->Left = 11; //relative to panel
    ActionsDueListBox->Top = 13;  //relative to panel
    ActionsDuePanelLabel->Width = 36;
    ActionsDuePanelLabel->Height = 20;
    ActionsDuePanelLabel->Left = 32; //relative to panel
    ActionsDuePanelLabel->Top = 0;   //relative to panel
    Interface->ADFTop = ActionsDueForm->Top; //initialises these values
    Interface->ADFLeft = ActionsDueForm->Left;
    Interface->FirstActionsDueFormDisplay = true;
}
//---------------------------------------------------------------------------
void __fastcall TActionsDueForm::ActionsDueListBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    TrainController->LogEvent("ActionsDueListBoxMouseDown," + AnsiString(X) + "," + AnsiString(Y));
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",ActionsDueListBoxMouseDown");
    if(Button == mbRight)
    {
        ActionsDueListBoxRightMouseButtonDown = true;
    }
    Utilities->CallLogPop(2264);
}

//---------------------------------------------------------------------------

void __fastcall TActionsDueForm::ActionsDueListBoxMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
// Mouseup rather than Mousedown so shows floating label when over selected train
    AnsiString AnsiButton = "mbLeft";
    if(Button == mbRight)
    {
        AnsiButton = "mbRight";
    }
    TrainController->LogEvent("ActionsDueListBoxMouseUp," + AnsiString(X) + "," + AnsiString(Y) + "," + AnsiButton); // button may be right or left
    Utilities->CallLog.push_back(Utilities->TimeStamp() + ",ActionsDueListBoxMouseUp," + AnsiString(X) + "," + AnsiString(Y) + "," + AnsiButton);
    int ScreenPosH, ScreenPosV;
    if(Track->RouteFlashFlag || Track->PointFlashFlag) // no action
    {
        ActionsDueListBoxRightMouseButtonDown = false; // so resets when mouse up
        Utilities->CallLogPop(2087);
        return;
    }
    if(TrainController->OpTimeToActMultiMap.empty())
    {
        ActionsDueListBoxRightMouseButtonDown = false; // so resets when mouse up
        Utilities->CallLogPop(2088);
        return;
    }
    int HPos, VPos, TrainID = -1, TrackVectorPosition = -1;
    if(!Interface->GetTrainIDOrContinuationPosition(0, X, Y, TrainID, TrackVectorPosition))
    {
        ActionsDueListBoxRightMouseButtonDown = false; // so resets when mouse up
        Utilities->CallLogPop(2260);
        return;
    }
    if(Button == mbLeft) // added at v2.7.0 to keep right button for train information
    {
        HPos = (Track->TrackElementAt(926, TrackVectorPosition).HLoc * 16);
        VPos = (Track->TrackElementAt(927, TrackVectorPosition).VLoc * 16);
//these checks added at v2.11.0 to centre train on display if it's under either of these panels
//HPos relative to MainScreen, but panels relative to Interface form
        bool ElementUnderADForm = false;
        if(ActionsDueForm->Visible)
        {   //4 added because train position 4 right and below element position
            if(((HPos - (Display->DisplayOffsetH * 16) + Interface->MainScreen->ClientOrigin.x + 4) >= ActionsDueForm->Left) &&
                ((HPos - (Display->DisplayOffsetH * 16) + Interface->MainScreen->ClientOrigin.x + 4) <= (ActionsDueForm->Left + ActionsDueForm->Width)) &&
                ((VPos - (Display->DisplayOffsetV * 16) + Interface->MainScreen->ClientOrigin.y + 4) >= ActionsDueForm->Top) &&
                ((VPos - (Display->DisplayOffsetV * 16) + Interface->MainScreen->ClientOrigin.y + 4) <= (ActionsDueForm->Top + ActionsDueForm->Height)))
            {
                ElementUnderADForm = true;
            }
        }

        bool ElementUnderPerfLogForm = false;
        if(Interface->ShowPerfLogForm)
        {   //4 added because train position 4 right and below element position
            if(((HPos - (Display->DisplayOffsetH * 16) + Interface->MainScreen->ClientOrigin.x + 4) >= PerfLogForm->Left) &&
                ((HPos - (Display->DisplayOffsetH * 16) + Interface->MainScreen->ClientOrigin.x + 4) <= (PerfLogForm->Left + PerfLogForm->Width)) &&
                ((VPos - (Display->DisplayOffsetV * 16) + Interface->MainScreen->ClientOrigin.y + 4) >= PerfLogForm->Top) &&
                ((VPos - (Display->DisplayOffsetV * 16) + Interface->MainScreen->ClientOrigin.y + 4) <= (PerfLogForm->Top + PerfLogForm->Height)))
            {
                ElementUnderPerfLogForm = true;
            }
        }
        //if train is already shown on the screen then don't move the viewpoint, if not then display it in the centre
        //added at v2.10.0
        if(((HPos - (Display->DisplayOffsetH * 16)) >= 0) && ((HPos - (Display->DisplayOffsetH * 16)) < Interface->MainScreen->Width) &&
                ((VPos - (Display->DisplayOffsetV * 16)) >= 0) && ((VPos - (Display->DisplayOffsetV * 16)) < Interface->MainScreen->Height) &&
                !ElementUnderPerfLogForm && !ElementUnderADForm) // element on screen & not hidden behind a panel
        {
            ScreenPosH = HPos - (Display->DisplayOffsetH * 16);
            ScreenPosV = VPos - (Display->DisplayOffsetV * 16);
        }
        else
        {
            // set the offsets to display HPos & VPos in the centre of the screen
            Display->DisplayOffsetH = (HPos - Interface->MainScreen->Width / 2) / 16; // ScreenPosH = HPos - (DisplayOffsetH * 16)
            Display->DisplayOffsetV = (VPos - Interface->MainScreen->Height / 2) / 16;
            ScreenPosH = HPos - (Display->DisplayOffsetH * 16);
            ScreenPosV = VPos - (Display->DisplayOffsetV * 16);
        }
        if(Display->ZoomOutFlag) // panel displays in either zoom mode
        {
            Display->ZoomOutFlag = false;
            Interface->SetPausedOrZoomedInfoCaption(6);
        }
        Interface->ClearandRebuildRailway(72); // if was zoomed out this displays the track because until ZoomOutFlag reset PlotOutput plots nothing
        TPoint MainScreenPoint(ScreenPosH + 8, ScreenPosV + 8); // new v2.2.0 add 8 to centre pointer in element
        TPoint CursPos = Interface->MainScreen->ClientToScreen(MainScreenPoint); // accurate funtion to convert from local to global co-ordinates
        Mouse->CursorPos = CursPos;
    }
    ActionsDueListBoxRightMouseButtonDown = false; // so resets when mouse up
    Utilities->CallLogPop(2090);
}

//---------------------------------------------------------------------------


