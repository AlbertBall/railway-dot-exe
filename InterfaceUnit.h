// InterfaceUnit.h
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
#ifndef InterfaceUnitH
#define InterfaceUnitH

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
#include <System.ImageList.hpp>
#include <System.IOUtils.hpp>  //for directory manipulation
#include <Vcl.ImgList.hpp>
#include <fstream>
#include <vector>
#include <vcl.h>

//#include "API.h"  dropped this in favour of forward class declaration below, included in .cpp file

// ---------------------------------------------------------------------------

typedef std::pair<int, int>THVPair;
///< HLoc/VLoc position pair

class TOnePrefDir; // predeclarations
class TOneRoute;
class TTrack;
class TGraphicElement;
class API; //forward class declaration instead of including header  added at v2.10.0

//API* session_api_;  moved from header to avoid AboutForm having access and defining _session_api_
                    //as well as Interface and giving a warning, added at v2.10.0

class TInterface : public TForm
{
__published: // IDE-managed Components

    TBitBtn *SaveRailwayBaseModeButton;
///< Save button at the top left hand corner of the screen when no mode is selected

// 'Build/modify railway' mode - buttons left to right
    TBitBtn *AddTrackButton;
    TBitBtn *SetGapsButton;
    TBitBtn *TrackOKButton;
    TBitBtn *AddTextButton;
    TBitBtn *MoveTextOrGraphicButton;
    TBitBtn *LocationNameButton;
    TBitBtn *FontButton;
    TBitBtn *TextOrUserGraphicGridButton;
    TBitBtn *SetLengthsButton;
    TBitBtn *ScreenGridButton;
    TBitBtn *SaveRailwayTBPButton;
///< Save button on TrackBuildPanel
    TBitBtn *SigAspectButton;
    TBitBtn *ExitTrackButton;
    TBitBtn *RestoreAllDefaultLengthsButton;
    TBitBtn *ResetDefaultLengthButton;
    TBitBtn *LengthCancelButton;
    TBitBtn *LengthOKButton;
///< distance/speed setting buttons - left to right & top to bottom
    TBitBtn *UserGraphicButton; // new at v2.4.0

// user graphic selection buttons
    TButton *ReselectUserGraphic;
    TButton *SelectNewGraphic;

    TEdit *TextBox;
///< the edit box that accepts text to be added
    TEdit *DistanceBox;
///< distance/speed setting edit box that accepts distances
    TEdit *SpeedLimitBox;
///< distance/speed setting edit box that accepts speed limits
    TEdit *LocationNameTextBox;
///< edit box that accepts location names

// length & speed conversion boxes on distance setting screen
    TEdit *MileEdit;
    TEdit *ChainEdit;
    TEdit *YardEdit;
    TEdit *SpeedEditBox2;
    TPanel *MetrePanel;
    TLabel *MetreVariableLabel;
    TLabel *MileLabel;
    TPanel *LengthConversionPanel;
    TPanel *SpeedConversionPanel;
    TPanel *SpeedVariablePanel2;
    TLabel *ChainLabel;
    TLabel *YardLabel;
    TLabel *MetreFixedLabel;
    TLabel *SpeedTopLabel2;
    TLabel *SpeedBottomLabel2;
    TLabel *SpeedVariableLabel2;

// 'Set preferred directions' mode - buttons left to right
    TBitBtn *AddPrefDirButton;
    TBitBtn *DeleteOnePrefDirButton;
    TBitBtn *DeleteAllPrefDirButton;
    TBitBtn *SaveRailwayPDPButton;
///< Save button on PrefDirPanel
    TBitBtn *ExitPrefDirButton;

// 'Create a timetable'/'Edit a timetable' mode - top buttons left to right
    TBitBtn *ShowHideTTButton;
    TBitBtn *ExitTTModeButton;

// left hand column timetable buttons top to bottom
    TButton *PreviousTTEntryButton;
    TButton *NextTTEntryButton;
    TButton *CopyTTEntryButton;
    TButton *CutTTEntryButton;
    TButton *PasteTTEntryButton;
    TButton *DeleteTTEntryButton;
    TButton *MoveTTEntryUpButton;
    TButton *MoveTTEntryDownButton;
    TButton *SaveTTEntryButton;
    TButton *CancelTTEntryButton;
    TButton *NewTTEntryButton;
// centre column buttons left to right & top to bottom
    TButton *AZOrderButton;
    TButton *AddMinsButton;
    TButton *SubMinsButton;
    TButton *TTServiceSyntaxCheckButton;
    TButton *ValidateTimetableButton;
    TButton *SaveTTButton;
    TButton *SaveTTAsButton;
    TButton *RestoreTTButton;
    TButton *ExportTTButton;
// service code buttons top to bottom
    TButton *SntButton;
    TButton *SfsButton;
    TButton *SnsButton;
    TButton *Sns_fshButton;
    TButton *Snt_shButton;
    TButton *Sns_shButton;
    TButton *pasButton;
    TButton *jboButton;
    TButton *fspButton;
    TButton *rspButton;
    TButton *cdtButton;
    TButton *FnsButton;
    TButton *FjoButton;
    TButton *FerButton;
    TButton *Frh_shButton;
    TButton *Fns_shButton;
    TButton *F_nshsButton;
    TButton *FrhButton;

    TPanel *AddSubMinsPanel;
///< timetable minutes panel
    TEdit *TTStartTimeBox;
///< edit box that displays the timetable start time
    TComboBox *LocationNameComboBox;
///< the combobox that lists location names
    TEdit *AddSubMinsBox;
///< the edit box that accepts minutes to add or subtract

// speed conversion box on timetable screen
    TEdit *SpeedEditBox;
    TLabel *SpeedTopLabel;
    TLabel *SpeedBottomLabel;
    TLabel *SpeedVariableLabel;
    TPanel *SpeedVariablePanel1;
    TPanel *SpeedConversionTTPanel;

// power conversion box on timetable screen
    TEdit *PowerEditBox;
    TLabel *PowerTopLabel;
    TLabel *PowerBottomLabel;
    TLabel *PowerVariableLabel;
    TPanel *KWPanel;

// 'Operate railway' mode - buttons left to right
    TBitBtn *OperateButton;
    TBitBtn *AutoSigsButton;
    TBitBtn *SigPrefConsecButton;
    TBitBtn *UnrestrictedButton;
    TBitBtn *RouteCancelButton;
    TBitBtn *PresetAutoSigRoutesButton;
    TBitBtn *PerformanceLogButton;
    TBitBtn *SaveSessionButton;
    TBitBtn *OperatorActionButton; // new for v2.2.0
    TBitBtn *ExitOperationButton;
    TBitBtn *TTClockAdjButton;
    TSpeedButton *CallingOnButton; // speedbutton used so can detect when button is down

    TButton *PowerToggleButton; // kilowatts to horse power toggle
    TButton *SpeedToggleButton; // miles per hour to kilmetres per hour toggle
    TButton *SpeedToggleButton2; // miles per hour to kilmetres per hour toggle

// timetable clock adjustment panel, buttons & labels
    TPanel *TTClockAdjPanel;
    TPanel *TTClockAdjustWarningPanel;
    TButton *TTClockAdd1hButton;
    TButton *TTClockAdd10mButton;
    TButton *TTClockAdd1mButton;
    TButton *TTClockAdjustOKButton;
    TButton *TTClockExitButton;
    TButton *TTClockResetButton;
    TButton *TTClockxQuarterButton;
    TButton *TTClockxHalfButton;
    TButton *TTClockx1Button;
    TButton *TTClockx2Button;
    TButton *TTClockx4Button;
    TButton *TTClockx8Button;
    TButton *TTClockx16Button;
    TButton *TTClockxEighthButton;
    TButton *TTClockxSixteenthButton;
    TCheckBox *TTClockAdjustCheckBox;
    TLabel *TTClockAdjustWarningLabel;

// timetable conflict analysis button & panel
    TButton *ConflictAnalysisButton;
    TButton *CPCancelButton;
    TButton *CPGenFileButton;
    TPanel *ConflictPanel;
    TLabel *CPLabel1;
    TLabel *CPLabel2;
    TLabel *CPLabel3;
    TLabel *CPLabel4;
    TLabel *CPLabel5;
    TCheckBox *CPArrivalsCheckBox;
    TCheckBox *CPDeparturesCheckBox;
    TCheckBox *CPAtLocCheckBox;
    TEdit *CPEditArrRange;
    TEdit *CPEditDepRange;

// screen navigation buttons (right/up means move viewpoint right/up, i.e railway moves left/down)
    TBitBtn *ScreenRightButton;
    TBitBtn *ScreenLeftButton;
    TBitBtn *ScreenUpButton;
    TBitBtn *ScreenDownButton;
    TBitBtn *HomeButton;
    TBitBtn *NewHomeButton;
    TBitBtn *ZoomButton;

    TBitBtn *ErrorButton;
///< the 'Press to exit' button on the error screen

// warning icons shown during operation on the left hand side of the screen
    TImage *SignalStopImage;                  //top 125
    TImage *BufferAttentionImage;             //2nd 155
    TImage *CallOnImage;                      //3rd 185
    TImage *SPADImage;                        //4th 215
    TImage *CrashImage;                       //5th 245
    TImage *DerailImage;                      //6th 275
    TImage *TrainFailedImage;                 //7th 305 new at v2.4.0
    TImage *ManualLCDownImage;                //8th 335 new 1t v2.9.0

    TImage *DistanceKey;
///< information panel displayed when setting distances & speed limits
    TImage *PrefDirKey;
///< information panel displayed when setting preferred directions

// railway status icons displayed on the left hand side of the screen during build/modify mode
    TImage *TrackLinkedImage;
    TImage *TrackNotLinkedImage;
    TImage *GapsSetImage;
    TImage *GapsNotSetImage;
    TImage *LocationNamesSetImage;
    TImage *LocationNamesNotSetImage;
    TImage *SigsOnLeftImage1; // these new at v2.3.0 for handed signals
    TImage *SigsOnLeftImage2;
    TImage *SigsOnRightImage1;
    TImage *SigsOnRightImage2;

    TImage *MainScreen;
///< the railway display screen

    TImageList *TTImageList;
    TImageList *MMImageList;

// the warning logs displayed during operation above the railway screen
    TLabel *OutputLog1;
    TLabel *OutputLog2;
    TLabel *OutputLog3;
    TLabel *OutputLog4;
    TLabel *OutputLog5;
    TLabel *OutputLog6;
    TLabel *OutputLog7;
    TLabel *OutputLog8;
    TLabel *OutputLog9;
    TLabel *OutputLog10;

    TLabel *PerformancePanelLabel;
///< label at the top of PerformancePanel
    TLabel *PrefDirPanelLabel;
///< label to the left of PrefDirPanel
    TLabel *ServiceCodeLabel;
///< displays 'Service component codes' on TimetableEditPanel
    TLabel *SpeedRestrictionLabel;
///< displays 'Speed Limit (km/h)' on TrackLengthPanel
    TLabel *TimetableNameLabel;
///< displays the current timetable name on the timetable edit panel
    TLabel *TimetablePanelLabel;
///< label to the left of TimetablePanel
    TLabel *TrackBuildPanelLabel;
///< label to the left of TrackBuildPanel
    TLabel *TrackLengthLabel;
///< displays 'Length (metres)' on TrackLengthPanel
    TLabel *CallLogTickerLabel;
///< diagnostic label displaying the call log depth, made visible by ctrl+ alt+ 2
    TLabel *ClockLabel;
///< the timetable clock
    TLabel *OperatingPanelLabel;
///< displays 'Operation' or 'Disabled' on the operating panel during operation for running or paused

    TLabel *TTClockTitleLabel;
    TLabel *TTClockLabel1;
    TLabel *TTClockLabel2;
    TLabel *TTClockSpeedLabel;
    TLabel *TTClockAdjustLabel1;
    TLabel *TTClockAdjustLabel2;
///< timetable clock labels

    TLabel *OAPanelLabel;
    TLabel *FloatingLabel;
///< the floating window that displays track & train information

// text displayed on the timetable screen
    TLabel *TTLabel1;
    TLabel *TTLabel2;
    TLabel *TTLabel3;
    TLabel *TTLabel4;
    TLabel *TTLabel5;
    TLabel *TTLabel6;
    TLabel *TTLabel7;
    TLabel *TTLabel8;
    TLabel *TTLabel9;
    TLabel *TTLabel11;
    TLabel *TTLabel12;
    TLabel *TTLabel13;
    TLabel *TTLabel14;
    TLabel *TTLabel15;

    TPanel *RestoreFocusPanel;
///< Panel used to restore focus to Interface to enable cursor keys to move screen

    TPanel *TrackBuildPanel;
///< 'Build/modify railway' panel
    TPanel *PrefDirPanel;
///< 'Set preferred directions' panel
    TPanel *TimetablePanel;
///< 'Create a timetable'/'Edit a timetable' panel that contains the topmost buttons (show/hide & exit)
    TPanel *OperatingPanel;
///< 'Operate railway' panel

    TPanel *TimetableEditPanel;
///< the large panel that contains all the main timetable components
    TPanel *TTCommandTextPanel;
///< the timetable panel that contains the service component buttons and information
    TPanel *HighlightPanel;
///< the orange bar that displays the current timetable entry in AllEntriesTTListBox
    TPanel *InfoPanel;
///< the general information panel (with blue 'i' symbol)
    TPanel *PerformancePanel;
///< displays the operating performance log
    TPanel *TrackElementPanel;
///< panel containing the track/location/parapet element buttons
    TPanel *TrackLengthPanel;
///< the panel that contains the distance/speed setting buttons and edit boxes
    TPanel *DevelopmentPanel;
///< used for diagnostic purposes, made visible by ctrl+ alt+ 3
    TPanel *FloatingPanel;
///<new for v2.2.0 where label sits in it and it autosizes to the label. Labels are not TWinControls so they always underlie panels which are, so using a panel allows it to overlie other panels. With this there is no need to hide the performance panel when the floating panel obscures it.
    TPanel *PositionalPanel;
    TPanel *OperatorActionPanel;
///< new v2.2.0 panel housing the OAListBox with list of trains and times to act
    TPanel *SigImagePanel;
///< new at v2.3.0 for handed signals

    TMainMenu *MainMenu1;
///< the program menu

    TMemo *ErrorMessage;
///< the text of the normal error message screen
    TMemo *ErrorMessageStoreImage;
///< the text of the error message for failure to draw trains in SaveOperatingImage
    TMemo *OneEntryTimetableMemo;
///< the single service editing and display area on the right hand side of the timetable edit screen
    TMemo *PerformanceLogBox;
///< the performance log displayed during operation
    TMemo *TTInfoMemo;
///< timetable help text displayed on the timetable edit screen

    TListBox *AllEntriesTTListBox;
///< the list of service entries displayed on the left hand side of the timetable edit screen
    TListBox *OAListBox;
///< Operator action list, sits inside OperatorActionPanel and lists trains in ascending order of time to act

    TMenuItem *FileMenu;
    TMenuItem *LoadRailwayMenuItem;
    TMenuItem *SaveAsMenuItem;
    TMenuItem *SaveMenuItem;
    TMenuItem *LoadTimetableMenuItem;
    TMenuItem *LoadSessionMenuItem;
    TMenuItem *ExportTTMenuItem;
    TMenuItem *ClearAllMenuItem;
    TMenuItem *ExitMenuItem;

    TMenuItem *ModeMenu;
    TMenuItem *BuildTrackMenuItem;
    TMenuItem *PlanPrefDirsMenuItem;
    TMenuItem *CreateTimetableMenuItem;
    TMenuItem *EditTimetableMenuItem;
    TMenuItem *OperateRailwayMenuItem;
    TMenuItem *WhiteBgndMenuItem;
    TMenuItem *BlackBgndMenuItem;
    TMenuItem *BlueBgndMenuItem;

    TMenuItem *EditMenu;
    TMenuItem *SelectMenuItem;
    TMenuItem *ReselectMenuItem;
    TMenuItem *CutMenuItem;
    TMenuItem *CopyMenuItem;
    TMenuItem *FlipMenuItem;
    TMenuItem *MirrorMenuItem;
    TMenuItem *RotateMenuItem;
    TMenuItem *PasteMenuItem;
    TMenuItem *DeleteMenuItem;
    TMenuItem *SelectLengthsMenuItem;
    TMenuItem *SelectBiDirPrefDirsMenuItem;
    TMenuItem *CancelSelectionMenuItem;
    TMenuItem *CheckPrefDirConflictsMenuItem;

    TMenuItem *FloatingInfoMenu;
    TMenuItem *TrackInfoMenuItem;
    TMenuItem *TrackInfoOnOffMenuItem;
    TMenuItem *TrainInfoMenuItem;
    TMenuItem *TrainStatusInfoOnOffMenuItem;
    TMenuItem *TrainTTInfoOnOffMenuItem;

    TMenuItem *ImageMenu;
    TMenuItem *SaveImageNoGridMenuItem;
    TMenuItem *SaveImageAndGridMenuItem;
    TMenuItem *SaveImageAndPrefDirsMenuItem;
    TMenuItem *SaveOperatingImageMenuItem;

    TMenuItem *HelpMenu;
    TMenuItem *AboutMenuItem;
    TMenuItem *OpenHelpMenuItem;
    TMenuItem *TrainHeadCodeMenuItem;
    TMenuItem *TakeSignallerControlMenuItem;
    TMenuItem *TimetableControlMenuItem;
    TMenuItem *ChangeDirectionMenuItem;
    TMenuItem *MoveForwardsMenuItem;
    TMenuItem *PassRedSignalMenuItem;
    TMenuItem *StepForwardMenuItem;
    TMenuItem *SignallerControlStopMenuItem;
    TMenuItem *RemoveTrainMenuItem;
    TMenuItem *ConverttoRightHandSignalsMenuItem;
    TMenuItem *N1;
    TMenuItem *N2;
    TMenuItem *N3;
    TMenuItem *N4;
    TMenuItem *RailwayWebSiteMenuItem;
// TMenuItem *PasteWithAttributesMenuItem;  (dropped at 2.4.0 as all pastes are with attributes)
    TMenuItem *RotRightMenuItem; // new at v2.4.0
    TMenuItem *RotLeftMenuItem; // new at v2.4.0
    TMenuItem *SignallerJoinedByMenuItem; // new at v2.4.0
    TMenuItem *RepairFailedTrainMenuItem;

    TEdit *MTBFEditBox;
    TLabel *MTBFLabel;
    TPanel *UserGraphicReselectPanel;

    TPopupMenu *PopupMenu;

// file open dialogs
    TOpenDialog *LoadRailwayDialog;
    TOpenDialog *LoadSessionDialog;
    TOpenDialog *TimetableDialog;
    TOpenDialog *LoadUserGraphicDialog;

// file save dialogs
    TSaveDialog *SaveRailwayDialog;
    TSaveDialog *SaveTTDialog;
    TFontDialog *FontDialog;
///< font change dialog

    TTimer *MasterClock;
///< the program clock (not the timetable clock)

// track/location/parapet element buttons displayed on TrackElementPanel

/// See Speedbutton1 detail for track element allocations \image html Speedbutton_image.png
    TSpeedButton *SpeedButton1;
    TSpeedButton *SpeedButton2;
    TSpeedButton *SpeedButton3;
    TSpeedButton *SpeedButton4;
    TSpeedButton *SpeedButton5;
    TSpeedButton *SpeedButton6;
    TSpeedButton *SpeedButton7;
    TSpeedButton *SpeedButton8;
    TSpeedButton *SpeedButton9;
    TSpeedButton *SpeedButton10;
    TSpeedButton *SpeedButton11;
    TSpeedButton *SpeedButton12;
    TSpeedButton *SpeedButton13;
    TSpeedButton *SpeedButton14;
    TSpeedButton *SpeedButton15;
    TSpeedButton *SpeedButton16;
// note: 17 missing - used to be for text in early development
    TSpeedButton *SpeedButton18;
    TSpeedButton *SpeedButton19;
    TSpeedButton *SpeedButton20;
    TSpeedButton *SpeedButton21;
    TSpeedButton *SpeedButton22;
    TSpeedButton *SpeedButton23;
    TSpeedButton *SpeedButton24;
    TSpeedButton *SpeedButton25;
    TSpeedButton *SpeedButton26;
    TSpeedButton *SpeedButton27;
    TSpeedButton *SpeedButton28;
    TSpeedButton *SpeedButton29;
    TSpeedButton *SpeedButton30;
    TSpeedButton *SpeedButton31;
    TSpeedButton *SpeedButton32;
    TSpeedButton *SpeedButton33;
    TSpeedButton *SpeedButton34;
    TSpeedButton *SpeedButton35;
    TSpeedButton *SpeedButton36;
    TSpeedButton *SpeedButton37;
    TSpeedButton *SpeedButton38;
    TSpeedButton *SpeedButton39;
    TSpeedButton *SpeedButton40;
    TSpeedButton *SpeedButton41;
    TSpeedButton *SpeedButton42;
    TSpeedButton *SpeedButton43;
    TSpeedButton *SpeedButton44;
    TSpeedButton *SpeedButton45;
    TSpeedButton *SpeedButton46;
    TSpeedButton *SpeedButton47;
    TSpeedButton *SpeedButton48;
    TSpeedButton *SpeedButton49;
    TSpeedButton *SpeedButton50;
    TSpeedButton *SpeedButton51;
    TSpeedButton *SpeedButton52;
    TSpeedButton *SpeedButton53;
    TSpeedButton *SpeedButton54;
    TSpeedButton *SpeedButton55;
    TSpeedButton *SpeedButton56;
    TSpeedButton *SpeedButton57;
    TSpeedButton *SpeedButton58;
    TSpeedButton *SpeedButton59;
    TSpeedButton *SpeedButton60;
    TSpeedButton *SpeedButton61;
    TSpeedButton *SpeedButton62;
    TSpeedButton *SpeedButton63;
    TSpeedButton *SpeedButton64;
    TSpeedButton *SpeedButton65;
    TSpeedButton *SpeedButton66;
    TSpeedButton *SpeedButton67;
    TSpeedButton *SpeedButton68;
    TSpeedButton *SpeedButton69;
    TSpeedButton *SpeedButton70;
    TSpeedButton *SpeedButton71;
    TSpeedButton *SpeedButton72;
    TSpeedButton *SpeedButton73;
    TSpeedButton *SpeedButton74;
    TSpeedButton *SpeedButton75;
    TSpeedButton *SpeedButton76;
    TSpeedButton *SpeedButton77;
    TSpeedButton *SpeedButton78;
    TSpeedButton *SpeedButton79;
    TSpeedButton *SpeedButton80;
    TSpeedButton *SpeedButton81;
    TSpeedButton *SpeedButton82;
    TSpeedButton *SpeedButton83;
    TSpeedButton *SpeedButton84;
    TSpeedButton *SpeedButton85;
    TSpeedButton *SpeedButton86;
    TSpeedButton *SpeedButton87;
    TSpeedButton *SpeedButton88;
    TSpeedButton *SpeedButton89;
    TSpeedButton *SpeedButton90;
    TSpeedButton *SpeedButton91;
    TSpeedButton *SpeedButton92;
    TSpeedButton *SpeedButton93;
    TSpeedButton *SpeedButton94;
    TSpeedButton *SpeedButton95;
    TSpeedButton *SpeedButton96;
    TSpeedButton *SpeedButton97;
    TSpeedButton *SpeedButton98;
    TSpeedButton *SpeedButton99;
    TSpeedButton *SpeedButton100;
    TSpeedButton *SpeedButton101;
    TSpeedButton *SpeedButton102;
    TSpeedButton *SpeedButton103;
    TSpeedButton *SpeedButton104;
    TSpeedButton *SpeedButton105;
    TSpeedButton *SpeedButton106;
    TSpeedButton *SpeedButton107;
    TSpeedButton *SpeedButton108;
    TSpeedButton *SpeedButton109;
    TSpeedButton *SpeedButton110;
    TSpeedButton *SpeedButton111;
    TSpeedButton *SpeedButton112;
    TSpeedButton *SpeedButton113;
    TSpeedButton *SpeedButton114;
    TSpeedButton *SpeedButton115;
    TSpeedButton *SpeedButton116;
    TSpeedButton *SpeedButton117;
    TSpeedButton *SpeedButton118;
    TSpeedButton *SpeedButton119;
    TSpeedButton *SpeedButton120;
    TSpeedButton *SpeedButton121;
    TSpeedButton *SpeedButton122;
    TSpeedButton *SpeedButton123;
    TSpeedButton *SpeedButton124;
    TSpeedButton *SpeedButton125;
    TSpeedButton *SpeedButton126;
    TSpeedButton *SpeedButton127;
    TSpeedButton *SpeedButton128;
    TSpeedButton *SpeedButton129;
    TSpeedButton *SpeedButton130;
    TSpeedButton *SpeedButton131;
    TSpeedButton *SpeedButton132;
    TSpeedButton *SpeedButton133;
    TSpeedButton *SpeedButton134;
    TSpeedButton *SpeedButton135;
    TSpeedButton *SpeedButton136;
    TSpeedButton *SpeedButton137;
    TSpeedButton *SpeedButton138;
    TSpeedButton *SpeedButton139;
    TSpeedButton *SpeedButton140;
    TSpeedButton *SpeedButton141;
    TSpeedButton *SpeedButton142;
    TSpeedButton *SpeedButton143;
    TSpeedButton *SpeedButton144;
    TSpeedButton *SpeedButton145;
    TSpeedButton *SpeedButton146;
    TLabel *CPLabel6;
    TLabel *CPLabel7;
    TLabel *CPLabel8;
    TBitBtn *SigPrefNonConsecButton;
    TPanel *TwoLocationNamePanel;
    TLabel *TwoLocationNameLabel;
    TButton *TwoLocationNameButton;
    TCheckBox *TwoLocationNameCheckBox;
    TLabel *CPLabel9;
    TCheckBox *CPDirectionsCheckBox;

// menu item actions
    void __fastcall AboutMenuItemClick(TObject *Sender);
    void __fastcall BlackBgndMenuItemClick(TObject *Sender);
    void __fastcall BlueBgndMenuItemClick(TObject *Sender);
    void __fastcall BuildTrackMenuItemClick(TObject *Sender);
    void __fastcall CancelSelectionMenuItemClick(TObject *Sender);
    void __fastcall ClearAllMenuItemClick(TObject *Sender);
    void __fastcall CopyMenuItemClick(TObject *Sender);
    void __fastcall CreateTimetableMenuItemClick(TObject *Sender);
    void __fastcall CutMenuItemClick(TObject *Sender);
    void __fastcall DeleteMenuItemClick(TObject *Sender);
    void __fastcall EditMenuClick(TObject *Sender); // added at v2.1.0 to allow CTRL+X, CTRL+C & CTRL+V in edit menu
    void __fastcall EditTimetableMenuItemClick(TObject *Sender);
    void __fastcall ExitMenuItemClick(TObject *Sender);
    void __fastcall ExportTTMenuItemClick(TObject *Sender);
    void __fastcall FlipMenuItemClick(TObject *Sender);
    void __fastcall LoadRailwayMenuItemClick(TObject *Sender);
    void __fastcall LoadSessionMenuItemClick(TObject *Sender);
    void __fastcall LoadTimetableMenuItemClick(TObject *Sender);
    void __fastcall MirrorMenuItemClick(TObject *Sender);
    void __fastcall OpenHelpMenuItemClick(TObject *Sender);
    void __fastcall OperateRailwayMenuItemClick(TObject *Sender);
    void __fastcall PasteMenuItemClick(TObject *Sender);
    void __fastcall PlanPrefDirsMenuItemClick(TObject *Sender);
    void __fastcall ReselectMenuItemClick(TObject *Sender);
    void __fastcall RotateMenuItemClick(TObject *Sender);
    void __fastcall SaveMenuItemClick(TObject *Sender);
    void __fastcall SaveAsMenuItemClick(TObject *Sender);
    void __fastcall SaveHeaderMenu1Click(TObject *Sender);
    void __fastcall SaveImageAndGridMenuItemClick(TObject *Sender);
    void __fastcall SaveImageAndPrefDirsMenuItemClick(TObject *Sender);
    void __fastcall SaveImageNoGridMenuItemClick(TObject *Sender);
    void __fastcall SaveOperatingImageMenuItemClick(TObject *Sender);
    void __fastcall SelectMenuItemClick(TObject *Sender);
    void __fastcall SelectBiDirPrefDirsMenuItemClick(TObject *Sender);
    void __fastcall SelectLengthsMenuItemClick(TObject *Sender);
    void __fastcall TrackInfoOnOffMenuItemClick(TObject *Sender);
    void __fastcall TrainStatusInfoOnOffMenuItemClick(TObject *Sender);
    void __fastcall TrainTTInfoOnOffMenuItemClick(TObject *Sender);
    void __fastcall WhiteBgndMenuItemClick(TObject *Sender);

// popup menu actions
    void __fastcall ChangeDirectionMenuItemClick(TObject *Sender);
    void __fastcall MoveForwardsMenuItemClick(TObject *Sender);
    void __fastcall PassRedSignalMenuItemClick(TObject *Sender);
    void __fastcall RemoveTrainMenuItemClick(TObject *Sender);
    void __fastcall SignallerControlStopMenuItemClick(TObject *Sender);
    void __fastcall StepForwardMenuItemClick(TObject *Sender);
    void __fastcall TakeSignallerControlMenuItemClick(TObject *Sender);
    void __fastcall TimetableControlMenuItemClick(TObject *Sender);

// mouse actions
    void __fastcall AcceptDragging(TObject *Sender, TObject *Source, int X, int Y, TDragState State, bool &Accept);
    void __fastcall AllEntriesTTListBoxMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall MainScreenMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall MainScreenMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall MainScreenMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog10MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog3MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog4MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog5MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog6MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog7MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog8MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OutputLog9MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall PerformancePanelLabelStartDrag(TObject *Sender, TDragObject *&DragObject);
    void __fastcall PerformancePanelStartDrag(TObject *Sender, TDragObject *&DragObject);

// button actions
    void __fastcall AddMinsButtonClick(TObject *Sender);
    void __fastcall AddPrefDirButtonClick(TObject *Sender);
    void __fastcall AddTextButtonClick(TObject *Sender);
    void __fastcall AddTrackButtonClick(TObject *Sender);
    void __fastcall AutoSigsButtonClick(TObject *Sender);
    void __fastcall CallingOnButtonClick(TObject *Sender);
    void __fastcall CancelTTEntryButtonClick(TObject *Sender);
    void __fastcall CopyTTEntryButtonClick(TObject *Sender);
    void __fastcall CutTTEntryButtonClick(TObject *Sender);
    void __fastcall DeleteAllPrefDirButtonClick(TObject *Sender);
    void __fastcall DeleteOnePrefDirButtonClick(TObject *Sender);
    void __fastcall DeleteTTEntryButtonClick(TObject *Sender);
    void __fastcall ErrorButtonClick(TObject *Sender);
    void __fastcall ExitOperationButtonClick(TObject *Sender);
    void __fastcall ExitPrefDirButtonClick(TObject *Sender);
    void __fastcall ExitTrackButtonClick(TObject *Sender);
    void __fastcall ExitTTModeButtonClick(TObject *Sender);
    void __fastcall ExportTTButtonClick(TObject *Sender);
    void __fastcall FontButtonClick(TObject *Sender);
    void __fastcall HomeButtonClick(TObject *Sender);
    void __fastcall LengthCancelButtonClick(TObject *Sender);
    void __fastcall LengthOKButtonClick(TObject *Sender);
    void __fastcall LocationNameButtonClick(TObject *Sender);
    void __fastcall MoveTextOrGraphicButtonClick(TObject *Sender);
    void __fastcall MoveTTEntryDownButtonClick(TObject *Sender);
    void __fastcall MoveTTEntryUpButtonClick(TObject *Sender);
    void __fastcall NewHomeButtonClick(TObject *Sender);
    void __fastcall NewTTEntryButtonClick(TObject *Sender);
    void __fastcall NextTTEntryButtonClick(TObject *Sender);
    void __fastcall UnrestrictedButtonClick(TObject *Sender);
    void __fastcall OperateButtonClick(TObject *Sender);
    void __fastcall PasteTTEntryButtonClick(TObject *Sender);
    void __fastcall PerformanceLogButtonClick(TObject *Sender);
    void __fastcall PreviousTTEntryButtonClick(TObject *Sender);
    void __fastcall ResetDefaultLengthButtonClick(TObject *Sender);
    void __fastcall RestoreAllDefaultLengthsButtonClick(TObject *Sender);
    void __fastcall RestoreTTButtonClick(TObject *Sender);
    void __fastcall RouteCancelButtonClick(TObject *Sender);
    void __fastcall SaveTTAsButtonClick(TObject *Sender);
    void __fastcall SaveTTButtonClick(TObject *Sender);
    void __fastcall SaveTTEntryButtonClick(TObject *Sender);
    void __fastcall ScreenDownButtonClick(TObject *Sender);
    void __fastcall ScreenGridButtonClick(TObject *Sender);
    void __fastcall ScreenLeftButtonClick(TObject *Sender);
    void __fastcall ScreenRightButtonClick(TObject *Sender);
    void __fastcall ScreenUpButtonClick(TObject *Sender);
    void __fastcall SetGapsButtonClick(TObject *Sender);
    void __fastcall SetLengthsButtonClick(TObject *Sender);
    void __fastcall ShowHideTTButtonClick(TObject *Sender);
    void __fastcall SigAspectButtonClick(TObject *Sender);
    void __fastcall SigPrefNonConsecButtonClick(TObject *Sender);
    void __fastcall SpeedButtonClick(TObject *Sender);
    void __fastcall SubMinsButtonClick(TObject *Sender);
    void __fastcall TextOrUserGraphicGridButtonClick(TObject *Sender);
    void __fastcall TrackOKButtonClick(TObject *Sender);
    void __fastcall TTClockAdd1hButtonClick(TObject *Sender);
    void __fastcall TTClockAdd10mButtonClick(TObject *Sender);
    void __fastcall TTClockAdd1mButtonClick(TObject *Sender);
    void __fastcall TTClockAdjButtonClick(TObject *Sender);
    void __fastcall TTClockExitButtonClick(TObject *Sender);
    void __fastcall TTClockResetButtonClick(TObject *Sender);
    void __fastcall TTClockxQuarterButtonClick(TObject *Sender);
    void __fastcall TTClockxHalfButtonClick(TObject *Sender);
    void __fastcall TTClockx1ButtonClick(TObject *Sender);
    void __fastcall TTClockx2ButtonClick(TObject *Sender);
    void __fastcall TTClockx4ButtonClick(TObject *Sender);
    void __fastcall TTClockx8ButtonClick(TObject *Sender);
    void __fastcall TTClockx16ButtonClick(TObject *Sender);
    void __fastcall TTServiceSyntaxCheckButtonClick(TObject *Sender);
    void __fastcall TTTextButtonClick(TObject *Sender);
    void __fastcall ValidateTimetableButtonClick(TObject *Sender);
    void __fastcall ZoomButtonClick(TObject *Sender);

// key actions
    void __fastcall AddSubMinsBoxKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall LocationNameComboBoxKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall LocationNameKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall OneEntryTimetableMemoKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall TextBoxKeyPress(TObject *Sender, char &Key);

// miscellaneous actions
    void __fastcall AppActivate(TObject *Sender);
    void __fastcall AppDeactivate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall LocationNameComboBoxClick(TObject *Sender);
    void __fastcall MasterClockTimer(TObject *Sender);
    void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall SpeedEditBoxKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall SpeedEditBox2KeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall LengthEditKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall PowerEditBoxKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall PresetAutoSigRoutesButtonClick(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall RailwayWebSiteMenuItemClick(TObject *Sender);
    void __fastcall SpeedToggleButtonClick(TObject *Sender);
    void __fastcall PowerToggleButtonClick(TObject *Sender);
    void __fastcall SpeedToggleButton2Click(TObject *Sender);
    void __fastcall OperatorActionButtonClick(TObject *Sender);
    void __fastcall OAListBoxMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall OperatorActionPanelStartDrag(TObject *Sender, TDragObject *&DragObject);
    void __fastcall ConverttoRightHandSignalsMenuItemClick(TObject *Sender);
    void __fastcall TTClockxEighthButtonClick(TObject *Sender);
    void __fastcall TTClockxSixteenthButtonClick(TObject *Sender);
    void __fastcall RotRightMenuItemClick(TObject *Sender); // new at v2.4.0
    void __fastcall RotLeftMenuItemClick(TObject *Sender); // new at v2.4.0
    void __fastcall SignallerJoinedByMenuItemClick(TObject *Sender); // new at v2.4.0
    void __fastcall RepairFailedTrainMenuItemClick(TObject *Sender); // new at v2.4.0
    void __fastcall MTBFEditBoxKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall MTBFEditBoxClick(TObject *Sender);
    void __fastcall AZOrderButtonClick(TObject *Sender);
    void __fastcall UserGraphicButtonClick(TObject *Sender);
    void __fastcall ReselectUserGraphicClick(TObject *Sender);
    void __fastcall SelectNewGraphicClick(TObject *Sender);
    void __fastcall TTClockAdjustOKButtonClick(TObject *Sender);
    void __fastcall ConflictAnalysisButtonClick(TObject *Sender);
    void __fastcall CPCancelButtonClick(TObject *Sender);
    void __fastcall CPGenFileButtonClick(TObject *Sender);
    void __fastcall OAListBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
                                       int X, int Y);
    void __fastcall SigPrefConsecButtonClick(TObject *Sender);
    void __fastcall CheckPrefDirConflictsMenuItemClick(TObject *Sender);
    void __fastcall TwoLocationNameButtonClick(TObject *Sender);

public: // AboutForm needs access to these

    enum TLevel1Mode
///< Level 1 program modes
    {
        BaseMode, TrackMode, PrefDirMode, OperMode, RestartSessionOperMode, TimetableMode
    } Level1Mode;

    UnicodeString ProgramVersion;

    UnicodeString GetVersion();
///< determined automatically from the project options 'Version Info'

private:

// Folder names
    static const UnicodeString RAILWAY_DIR_NAME;
    static const UnicodeString TIMETABLE_DIR_NAME;
    static const UnicodeString PERFLOG_DIR_NAME;
    static const UnicodeString SESSION_DIR_NAME;
    static const UnicodeString IMAGE_DIR_NAME;
    static const UnicodeString FORMATTEDTT_DIR_NAME;
    static const UnicodeString USERGRAPHICS_DIR_NAME;

// Level 2 program modes (i.e. submodes from the level 1 modes)
    enum TLevel2OperMode
    {
        NoOperMode, Operating, PreStart, Paused
    } Level2OperMode;

    enum TLevel2PrefDirMode
    {
        NoPrefDirMode, PrefDirContinuing, PrefDirSelecting
    } Level2PrefDirMode;

    enum TLevel2TrackMode
    {
        NoTrackMode, AddTrack, AddGraphic, SelectGraphic, GapSetting, AddText, MoveTextOrGraphic, AddLocationName, DistanceStart, DistanceContinuing,
        TrackSelecting, CutMoving, CopyMoving, Pasting, Deleting
    } Level2TrackMode;

    enum
    {
        None, RouteNotStarted, RouteContinuing
    } RouteMode;
///< route building modes

    typedef std::vector<AnsiString>TTimetableEditVector;
///< typedef for the complete timetable as a list of AnsiStrings for use in edit timetable functions
    typedef std::vector<AnsiString>::iterator TTEVPtr;
///< typedef for pointers to entries in edit timetable functions

// API tracking variables      //added at v2.10.0
    int api_main_mode_;
    int api_oper_mode_;

// Timetable edit members

// variables
    AnsiString ClpBrdValid;
///< set to RlyClpBrdCopy or RlyClpBrd_Cut when Windows Clipboard contains a valid railway segment
    AnsiString CopiedEntryStr;
///< a timetable entry that has been copied
    AnsiString CreateEditTTFileName;
///< the full path and filename of the timetable file
    AnsiString CreateEditTTTitle;
///< the title of the timetable currently being edited - i.e. the filename without the '.ttb'
    AnsiString CurDir;
///< the full path to the folder where railway.exe resides
    AnsiString DirectoryError;
///< unused
    AnsiString InfoCaptionStore;
///< temporary store for the information panel caption
    AnsiString OneEntryTimetableContents;
///< the current text in the large right hand timetable edit window
    AnsiString PerformanceFileName;
///< full path and filename of the performance file
    AnsiString RailwayTitle, TimetableTitle;
///< the titles of the loaded railway and loaded timetable, i.e. the filenames without the extension
    AnsiString SavedFileName;
///< the full path and filename of the loaded railway
    AnsiString TempTTFileName;
///< the name for the temporary file used to save loaded timetables for storage in session files & error logs
    AnsiString TTSelectedEntry;
///< used to record the current timetable entry when changing to AZ order or back to original order
    AnsiString SelectedGraphicFileName;
///< filename for selected graphic set during LoadGraphic

    bool AllSetUpFlag;
///< false during initial start up, true when all set up to allow MasterClock to start
    bool AutoSigsFlag;
///< true when AutoSig route building selected during operation
    bool CancelSelectionFlag;
///< used in case pasting to avoid RecoverClipboard call when set
    bool ClipboardChecked;
///< used to prevent Windows clipboard beng checked repeatedly
    bool ConsecSignalsRoute;
///< true when AutoSig or preferred route building selected during operation (always same state as PreferredRoute)
    bool CopiedEntryFlag;
///< true when CopiedEntryStr holds a timetable entry in the timetable editor
    bool CopySelected;
///< used to indicate whether copy or cut selected in edit menu - for clipboard pasting
    bool CtrlKey;
///< true when the CTRL key is pressed (see also ShiftKey)
    bool DirOpenError;
///< true when one of the program subfolders doesn't already exist and can't be created
    bool DistancesMarked;
///< true when setting lengths, used to ensure the screen distance markers are redisplayed when the screen is updated
    bool ErrorLogCalledFlag;
///< true when an error has been thrown, stops repeated calls to ErrorLog and stops the MasterClockTimer function
    bool FileChangedFlag;
///< true when a loaded railway file has changed (used to warn user if opts to exit without saving)
    bool FillSelectionMessageSentFlag;
///< indicates that the message about filling a selected area with a chosen track element has been given, so it won't be given again
    bool LCManualLowerBarriersMessageSent;
///< indicates that the manual LC operation message has been given, so it won't be given again
    bool LengthWarningSentFlag;
///< indicates that the length selection applying to all elements in the selection warning has been given, so it won't be given again
    bool LoadSessionFlag;
///< true when a session load command has been given - implemented at next clock tick
    bool mbLeftDown;
///< true when the left mouse button is down
    bool MMoveTrackSelFlag;      //these added at v2.9.1
    bool MMovePrefDirSelFlag;
    bool MMoveCopyCutSelPickedUpFlag;
    bool MMoveTextGraphicTextFoundFlag;
    bool MMoveTextGraphicUserGraphicFoundFlag;
///< Mouse move flags to prevent repeated event logs
    bool NewEntryInPreparationFlag;
///< true when a new timetable entry is being prepared in the timetable editor
    bool OAListBoxRightMouseButtonDown;
///< flag set when right mouse button clicked over op action list box, so floating information window shows, reset on right button up or when BaseMode selected
    bool PasteWarningSentFlag;
///< indicates that the warning message about pasting overwriting the area has been given, so it won't be given again
    bool PreferredRoute;
///< true when AutoSig or preferred route building selected during operation (always same state as ConsecSignalsRoute)
    bool PreferredRouteFlag;
///< used to select either ConvertAndAddPreferredRouteSearchVector or ConvertAndAddNonPreferredRouteSearchVector (could probably have used PreferredRoute instead)
    bool PreventGapOffsetResetting;
///< during gap setting gaps are highlighted in turn for the user to select the matching gap, but when returning from zoomout this flag prevents the highlighted gap from being redisplayed, as the user wants to see the screen that corresponds to the clicked position
    bool RecoverClipboardMessageSent;
///< indicates that the warning about pasting at top left in a new application has been given, so it won't be given again
    bool RlyFile;
///< indicates that a loaded railway file is ready for operation, i.e. is a valid .rly file
    bool RouteCancelFlag;
///< true when route cancel button pressed, enables a right mouse click to cancel a route if in an appropriate position
    bool SaveSessionFlag;
///< true when a session save command has been given - implemented at next clock tick
    bool ScreenGridFlag;
///< true when the screen grid is displayed
    bool SelectionValid;
///< true when an area of screen has been selected via the 'Edit' & 'Select' or 'Reselect' menu items
    bool SelectLengthsFlag;
///< true when 'Set lengths &/or speeds' selected in the 'Edit' menu
    bool SelectPickedUp;
///< true when a valid selected screen area has been clicked after a 'Copy' or 'Cut' selected in the 'Edit' menu
    bool ShiftKey;
///< true when the SHIFT key is pressed (see also CtrlKey)
    bool ShowPerformancePanel;
///< true when the 'show performance panel' button has been clicked during operation
    bool SkipFormResizeEvent;
///< added at v2.1.0 to avoid calling the event during startup and shutdown
    bool TempCursorSet;
///< indicates that a screen cursor has been stored in TempCursor for redisplay after a temporary cursor (usually an hourglass) has been displayed
    bool TextFoundFlag;
///< indicates that a text item has been found when clicking on a build screen during 'AddText' or 'MoveTextOrGraphic' modes
    bool TTClockAdjustWarningHide;
///< true if user opts not to show the timetable clock adjustment warning (false on starting the program)
    bool TwoLocationNamePanelHide;
///< true if user opts not to show the two location name warning (false on starting the program)
    bool TooLongMessageSentFlag;
///< indicates that the length of a location element might be too long (>200m), so it won't be given again
    bool TooShortMessageSentFlag;
///< indicates that the length of a location element might be too short (<50m), so it won't be given again
    bool UserGraphicFoundFlag;
///< indicates that a user graphic item has been found when clicking on a build screen for moving
    bool TimetableChangedFlag;
///< true when a timetable in the editor has changed (used to warn user if opts to exit without saving)
    bool TimetableValidFlag;
///< indicates that a 'Validate timetable' button click in the timetable editor has succeeded
    bool TimetableChangedInAZOrderFlag;
///< used to give a warning message that changes will be discarded if proceed
    bool ShowOperatorActionPanel;
///< true when the 'trains needing action' button has been clicked during operation (new at v2.2.0)
    bool TTEntryChangedFlag;
///< true when a timetable entry that is displayed in the timetable entry edit window has changed
    bool WarningFlash;
///< toggles on and off automatically at a cycle of about 0.5 sec, used to drive the warning icons during operation
    bool WarningHover;
///< true when mouse hovers over warning messages during operation - to prevent clicking while changing
    bool WholeRailwayMoving;
///< true when moving the railway with the mouse, new at v2.1.0


/**These are for Shift Key shortcuts. 'Click()' execution must occur separately to the keypress because Windows stores the key until after any directly linked key code
is executed then selects the timetable entry that begins with the letter corresponding to the key.  Without this separation the list box is left with the wrong entry
showing.  See DevHistory.txt for the version at v2.5.0 for details. */
    bool PreviousTTEntryKeyFlag;
    bool NextTTEntryKeyFlag;
    bool MoveTTEntryUpKeyFlag;
    bool MoveTTEntryDownKeyFlag;
    bool CopyTTEntryKeyFlag;
    bool CutTTEntryKeyFlag;
    bool PasteTTEntryKeyFlag;
    bool DeleteTTEntryKeyFlag;
    bool NewTTEntryKeyFlag;
    bool AZOrderKeyFlag;
    bool TTServiceSyntaxCheckKeyFlag;
    bool ValidateTimetableKeyFlag;
    bool SaveTTKeyFlag;
    bool SaveTTAsKeyFlag;
    bool RestoreTTKeyFlag;
    bool ExportTTKeyFlag;
    bool ConflictAnalysisKeyFlag;

    double PauseEntryRestartTime;
///< time value of the timetable restart time (as a double) on entry to pause mode

    float PauseEntryTTClockSpeed;
///< rate at which the timetable clock runs on entry to the adjust routine - to restore if cancelled
    float PointsFlashDuration;
///< duration of the flash period when changing points manually
    float RouteFlashDuration;
///< duration of the route flash period
    float TTClockSpeed;
///< rate at which the timetable clock runs 1 = normal

    Graphics::TBitmap *SelectBitmap;
///< the graphic defined by Edit->Select & Edit->Reselect

// int ClockTimer2Count; ///< added at v1.3.0 to ensure focus returned to Interface, dropped at v2.0.0
    unsigned int LCResetCounter;
///< count up to 20 then resets - to check LCs & raise barriers if no route & no train present
    unsigned int TotalTicks;
///< total clock ticks
    unsigned int MissedTicks;
///< missed clock ticks
    int AllEntriesTTListBoxTopPosition;
///< stores the TopIndex property when keys are used to select items in the TT edit panel
    int LastNonCtrlOrShiftKeyDown;
///< value of last key (other than Ctrl or Shift) pressed down - to prevent repeated FormKeyDown calls from repeatedly acting on the same key combination
    int NewSelectBitmapHLoc;
///< the new (during & at end of moving) HLoc value of Edit->Select & Edit->Reselect
    int NewSelectBitmapVLoc;
///< as above for VLoc
    int OperatorActionPanelDragStartX;
///< mouse 'X' position when the OperatorActionPanel begins to be dragged
    int OperatorActionPanelDragStartY;
///< as above for 'Y'
    int OverallDistance, OverallSpeedLimit;
///< used when setting track lengths, represents the overall distance covered by the selected elements
    ///< and the overall speed limit, if the speed limits vary across the selection the value is set to -1
    int PerformancePanelDragStartX;
///< mouse 'X' position when the performance panel begins to be dragged
    int PerformancePanelDragStartY;
///< as above for 'Y'
    int PointFlashVectorPosition, DivergingPointVectorPosition;
    int SelectBitmapHLoc;
///< the original (prior to moving & after finished moving) HLoc value of Edit->Select & Edit->Reselect
    int SelectBitmapMouseLocX;
///< when flag SelectPickedUp is set to true (see above - to allow a selected screen area to move during MouseMove and remain in place at MouseUp) the mouse position is saved in SelectBitmapMouseLocX & Y for use later in MouseMove & MouseUp.
    int SelectBitmapMouseLocY;
///< see above
    int SelectBitmapVLoc;
///< the original (prior to moving & after finished moving) VLoc value of Edit->Select & Edit->Reselect
    int SelectedTrainID;
///< used to store the train ID when right clicked for signaller control actions
    int StartWholeRailwayMoveHPos;
///<mouse X position when start to move the whole railway
    int StartWholeRailwayMoveVPos;
///<mouse Y position when start to move the whole railway
    int StartX, StartY;
///< the mouse position in terms of pixels when an item of text is being selected for moving
    int TempCount;
///< test value
    int Text_X;
///< the 'X' pixel value for an item of text
    int Text_Y;
///< as above for 'Y'
    int TextOrUserGraphicGridVal;
///< stores the text alignment grid value, cycles forwards through 1, 2, 4, 8 & 16 each time the text grid alignment button is clicked
    int TextItem;
///< used to store a single item of text
    int TextMoveHPos, TextMoveVPos;
///< used to store the original text 'H' & 'V' positions for use during text moving
    int UserGraphicVectorNumber;
///< used to store a single item of user graphics
    int UserGraphicMoveHPos, UserGraphicMoveVPos;
///< used to store the original user graphic 'H' & 'V' positions for use during moving
    int WarningFlashCount;
///< increments each clock cycle to a max. of 4 then resets to 0, used to toggle bool WarningFlash - see above
    TCursor TempCursor;
///< stores the screen cursor while a temporary cursor (ususlly an hourglass) is displayed

    TDateTime LastTenthSecTick;
///< unused
    TDateTime PointFlashStartTime;
///< stores the starting time (timetable clock time) for points flashing
    TDateTime RouteFlashStartTime;
///< stores the starting time (timetable clock time) for route flashing

// declarations for 4 of the 6 TGraphicElement pointers (*GapFlashGreen, *GapFlashRed in TTrack class so TTrain can access them)
    TGraphicElement *PointFlash, *AutoRouteStartMarker, *SigRouteStartMarker, *NonSigRouteStartMarker;

    THVPair SelectStartPair;
///< stores the starting 'H' & 'V' values as a C++ pair when an area of screen is selected via the 'Edit' and
    ///< 'Select' menu items

    TImage *HiddenScreen;
///< a hidden copy of the railway display screen used during ClearandRebuildRailway (see below) to avoid flicker

    TOnePrefDir *ConstructPrefDir;
///< the Pref Dir under construction
    TOnePrefDir *EveryPrefDir;
///< all the Pref Dir elements in the railway
    TOnePrefDir *SelectPrefDir;
///< Pref Dir elements in a selected region
    TOneRoute *ConstructRoute;
///< the route under construction

    TRect SelectRect;
///< the rectangle in HLoc & VLoc terms set in Edit->Select & Edit->Reselect

    TSpeedButton* CurrentSpeedButton;
///< stores the selected track build element button during railway building

    TTEVPtr TEVPtr, TTCurrentEntryPtr, TTStartTimePtr, TTFirstServicePtr, TTLastServicePtr;
///< timetable entry value pointers used during timetable editing

    TTimetableEditVector TimetableEditVector, OriginalTimetableEditVector;
///< the complete timetable as a list of AnsiStrings for use in edit timetable functions

/// constructor
    __fastcall TInterface(TComponent* Owner);
/// destructor
    __fastcall ~TInterface();

//inline function
//true if any of the TT key flags are set
    bool AnyTTKeyFlagSet()
    {
        return (PreviousTTEntryKeyFlag || NextTTEntryKeyFlag || MoveTTEntryUpKeyFlag || MoveTTEntryDownKeyFlag || CopyTTEntryKeyFlag || CutTTEntryKeyFlag || PasteTTEntryKeyFlag
                || DeleteTTEntryKeyFlag || NewTTEntryKeyFlag || AZOrderKeyFlag || TTServiceSyntaxCheckKeyFlag || ValidateTimetableKeyFlag || SaveTTKeyFlag || SaveTTAsKeyFlag
                || RestoreTTKeyFlag || ExportTTKeyFlag || ConflictAnalysisKeyFlag);
    }

// functions defined in .cpp file

/// used for floating window to display train status
    AnsiString GetTrainStatusFloat(int Caller, int TrainID, AnsiString FormatNoDPStr, AnsiString SpecialStr); //new at v2.6.2
/// Search the timetable entry pointed to by TTCurrentEntryPtr and if any times (HH:MM) are present return true (checked in order to enable or not AddMinsButton & SubMinsButton)
    bool AreAnyTimesInCurrentEntry();
/// Convert a stored timetable file (either as a stand alone file or within a session file) to a loaded timetable, return false for error
    bool BuildTrainDataVectorForLoadFile(int Caller, std::ifstream &TTBLFile, bool GiveMessages, bool CheckLocationsExistInRailway, bool SessionFile);
/// Check the integrity of a stored timetable file (either as a stand alone file or within a session file) return false for error
    bool BuildTrainDataVectorForValidateFile(int Caller, std::ifstream &TTBLFile, bool GiveMessages, bool CheckLocationsExistInRailway);
/// Check the interface part of a session file & return false for error, called during SessionFileIntegrityCheck
    bool CheckInterface(int Caller, std::ifstream &SessionFile);
/// Check the performance file embedded within a session file & return false for error, called during SessionFileIntegrityCheck
    bool CheckPerformanceFile(int Caller, std::ifstream &InFile);
/// Check the timetable file embedded within a session file & return false for error, called during SessionFileIntegrityCheck
    bool CheckTimetableFromSessionFile(int Caller, std::ifstream &SessionFile);
/// First check whether a railway file has changed and if so ask user if really wants to close it without saving, and return false if responds no, otherwise unload the railway and clear the display and all internal railway data
    bool ClearEverything(int Caller);
/// Erase a location name (providing it exists in LocationNameMultiMap) from TextVector, return true if text found & exists in LocationNameMultiMap, and if so return text position in &HPos & &VPos
    bool EraseLocationNameText(int Caller, AnsiString Name, int &HPos, int &VPos);
/// Check integrity of a railway file prior to loading, return true for success
    bool FileIntegrityCheck(int Caller, char *FileName) const;
/// Used in actions due panel to identify the train or continuation
    bool GetTrainIDOrContinuationPosition(int Caller, int X, int Y, int &TrainID, int &ContinuationPos); //added at v2.6.2 so can use for actions due floating window
/// Called during gap setting to mark a gap with a red ellipse and ask user to select the corresponding gap, returns true if an unset gap found
    bool HighLightOneGap(int Caller, int &HLoc, int &VLoc);
/// Checked during operation, returns true if so and PerformancePanel removed - not used from v2.2.0 as now allow floating panel & label to overlie performance panel
    bool IsPerformancePanelObscuringFloatingLabel(int Caller);
/// Loads timetable into memory from a session file, true if successful
    bool LoadTimetableFromSessionFile(int Caller, std::ifstream &SessionFile);
/// Displays the manual LC down warning graphic in the panel on the LHS of the railway when there are no routes across it
    bool ManualLCDownAttentionWarning;
/// Examines a flashing route (i.e. one being set) and returns true if a moving train is detected on it and if so route setting is cancelled
    bool MovingTrainPresentOnFlashingRoute(int Caller);
/// Returns true if there are no track elements and no text
    bool NoRailway();
/// Called when compiling the error log file, to save the loaded timetable if any and the timetable being edited if any
    bool SaveTimetableToErrorFile(int Caller, std::ofstream &ErrorFile, AnsiString ErrorFileStr, AnsiString TimetableFileName);
/// Called during a session save to save the current timetable in the session file, true if successful
    bool SaveTimetableToSessionFile(int Caller, std::ofstream &SessionFile, AnsiString SessionFileStr);
/// Checks session file integrity prior to loading, true for success
    bool SessionFileIntegrityCheck(int Caller, AnsiString FileName);
/// Add 'Name' to TextVector and display on screen at a position determined by the shape and size of the location if UseEnteredPosition false, or at HPos & VPos if UseEnteredPosition true
    void AddLocationNameText(int Caller, AnsiString Name, int HPos, int VPos, bool UseEnteredPosition);
/// Function that deals with approach locking during ClockTimer2 function
    void ApproachLocking(int Caller, TDateTime Now);
/// Clear screen and rebuild it from stored data, uses HiddenScreen to avoid flicker
    void ClearandRebuildRailway(int Caller);
/// The main loop, called every clock tick via MasterClockTimer
    void ClockTimer2(int Caller);
/// Used during timetable editing funtions to compile the list of entries into the left hand long entry window and also to set the timetable entry pointers TEVPtr, TTStartTimePtr, TTFirstServicePtr, and TTLastServicePtr
    void CompileAllEntriesMemoAndSetPointers(int Caller);
/// Deal with signal resetting on auto signal routes that extend to continuations where trains have departed, called during the ClockTimer2 function
    void ContinuationAutoSignals(int Caller, TDateTime Now);
/// Used in timetable editing functions to convert any CRLFs in intended service entries to commas
    void ConvertCRLFsToCommas(int Caller, AnsiString &ConvStr);
/// Delays operation for the set time in milliseconds
    void Delay(int Caller, double Msec);
/// Called during operation whenever the route type buttons need to be disabled, e.g. when paused
    void DisableRouteButtons(int Caller);
/// Display a line from the TimetableEditVector (consists of a series of AnsiStrings, each of which represents a timetable line) in the right hand entry window of the timetable editor, ServiceEntry true = a train service, false = plain text
    void DisplayOneTTLineInPanel(int Caller, AnsiString Data, bool ServiceEntry);
/// The error logging routine, called when an error is detected
    void ErrorLog(int Caller, AnsiString Message);
/// Deal with any warning graphics that need to flash (call on, signal stop, crash etc), called during the ClockTimer function
    void FlashingGraphics(int Caller, TDateTime Now);
/// Called when floating train info needed and train hasn't entered yet
    void GetTrainFloatingInfoFromContinuation(int Caller, int VecPos, AnsiString FormatNoDPStr, AnsiString SpecialStr, AnsiString &TrainStatusFloat, AnsiString &TrainTTFloat); //new at v2.6.2
/// Called during timetable editing to highlight in red a single entry in the list of all entries in the left hand long window
    void HighlightOneEntryInAllEntriesTTListBox(int Caller, int Position);
/// In trackbuild display ground signal types on signal buttons
    void LoadGroundSignalGlyphs(int Caller);
/// Load the interface part of a session file
    void LoadInterface(int Caller, std::ifstream &SessionFile);
/// Load system clipboard to allow cutting & pasting between separate railway applications
    void LoadClipboard(int Caller); // new at v2.8.0
/// In trackbuild display normal signal types on signal buttons
    void LoadNormalSignalGlyphs(int Caller);
/// Load the performance file part of a sessionfile
    void LoadPerformanceFile(int Caller, std::ifstream &InFile);
/// Load a railway file.  The **Active elements** marker now has a '1' at the end if there are user graphics to be loaded, and if there are they are loaded after the preferred directions.
    void LoadRailway(int Caller, AnsiString LoadFileName);
/// Load a session file.
/**The structure of session files has changed due to 'patching' as later mods have needed inclusion
The version line ("version + : ***Interface***" ) now ends with a floating point value for excess level crossing down minutes - had been overlooked when this
performance measure was first introduced.  The **Active elements** marker now has a '1' at the end if there are user graphics to be loaded, and this is
inlcuded in railway files also, and if there are they are loaded after the preferred directions.  After the end of the performance file "***Additions after v2.3.1***"
is loaded fillowed by AvHoursIntValue then all failed trains if any. */
    void LoadSession(int Caller);
/// Load a user-defined graphic (bmp, gif, jpg, png).
    void LoadUserGraphic(int Caller);
/// Called when mouse button clicked in zoom-in mode
    void MainScreenMouseDown2(int Caller, TMouseButton Button, TShiftState Shift, int X, int Y);
/// Called when mouse button clicked in zoom-out mode
    void MainScreenMouseDown3(int Caller, TMouseButton Button, TShiftState Shift, int X, int Y);
/// Recovers clipboard as track and text vectors
    void RecoverClipboard(int Caller, bool &ValidResult); // new at v2.8.0
/// Called during ClearEverything and on startup to reset all major railway data values
    void ResetAll(int Caller);
/// Called whenever the railway is changed to deal with title displays (loaded railway and timetable) and set the FileChangedFlag so that user will be warned before it is deleted NonPrefDirChangesMade is true if the changes don't involve prefdirs
    void ResetChangedFileDataAndCaption(int Caller, bool NonPrefDirChangesMade);
/// Resets the CurrentSpeedButton variable to zero and the 'Down' property to false
    void ResetCurrentSpeedButton(int Caller);
/// SelectRect is the rectangle selected via the 'Edit'menu, and this function sets left, right, top and bottom values to zero
    void ResetSelectRect();
/// Clears any route start markers, enables or disables the route cancel button, and resets the information panel to the message it had before it was changed temporarily, e.g. due to a new route being set
    void RevertToOriginalRouteSelector(int Caller);
/// Used to save a railway when not already saved - e.g. when not already named or when the 'Save as' menu item selected - see LoadRailway for changes made because of user graphics
    void SaveAsSubroutine(int Caller);
/// Save the error log after an error has been thrown - no need for a caller
    void SaveErrorFile();
/// Save interface part of a session file
    void SaveInterface(int Caller, std::ofstream &SessionFile);
/// Save performance file part of a session file
    void SavePerformanceFile(int Caller, std::ofstream &OutFile);
/// Save a session file - see LoadSession for details of additions to the session file
    void SaveSession(int Caller);
/// Save a timetable as a temporary file either on loading directly or on loading a session file. Used later in saving sessions because the original timetable might have changed, been deleted, or not present if loaded from a session file
    void SaveTempTimetableFile(int Caller, AnsiString InFileName);
/// Sets the railway and timetable titles at the top of the screen
    void SetCaption(int Caller);
/// Enables or disables the initial Edit mode submenu items in PrefDir mode
    void SetInitialPrefDirModeEditMenu();
/// Enables or disables the initial Edit mode submenu items in Track mode
    void SetTrackModeEditMenu(int Caller);
/// Sets the Level1 user mode, using the Level1Mode variable to determine the mode
    void SetLevel1Mode(int Caller);
/// Sets the Level2OperMode user mode, using the Level2OperMode variable to determine the mode
    void SetLevel2OperMode(int Caller);
/// Sets the Level2PrefDirMode user mode, using the Level2PrefDirMode variable to determine the mode
    void SetLevel2PrefDirMode(int Caller);
/// Sets the Level2TrackMode user mode, using the Level2TrackMode variable to determine the mode
    void SetLevel2TrackMode(int Caller);
/// Sets the information panel message for zoom-out or paused modes
    void SetPausedOrZoomedInfoCaption(int Caller);
/// Enables or disables the route type buttons depending on the route mode, sets the information panel message accordingly, and sets the route to 'not started'
    void SetRouteButtonsInfoCaptionAndRouteNotStarted(int Caller);
/// Called during the ClockTimer2 function to set screen boundaries, buttons & menu items
    void SetSaveMenuAndButtons(int Caller);
///This used in timetable functions when shift keys pressed to make sure that the highlighted entry remains visible
    void SetTopIndex(int Caller);
/// Sets the left screen images (track linked or not, gaps set or not, locations named or not) during railway building
    void SetTrackBuildImages(int Caller);
/// Called during track building when setting distances, to calculate and set the individual track element lengths
    void SetTrackLengths(int Caller, int Distance, int SpeedLimit);
/// Unused
    void SignallerControl(int Caller);
/// Called for diagnostic purposes when keys CTRL ALT 4 pressed
    void TestFunction();
/// Called during timetable editing whenever a change is made to the timetable, sets all the timetable buttons and windows to the required values
    void TimetableHandler();
/// Controls the floating window function, called during the ClockTimer2 function
    void TrackTrainFloat(int Caller);
/// Called every 5 secs to update the panel (if visible)
    void UpdateOperatorActionPanel(int Caller);
};

// ---------------------------------------------------------------------------

extern PACKAGE TInterface *Interface;

// ---------------------------------------------------------------------------
#endif
