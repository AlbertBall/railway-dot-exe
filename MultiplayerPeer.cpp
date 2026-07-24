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
#include <sstream>
#include <vector>
#include <vcl.h>
#include <stdio.h>
#include <algorithm>
#pragma hdrstop

#include "MultiplayerPeer.h"
#include "InterfaceUnit.h"
#include <IdGlobal.hpp>
#include <IdException.hpp>
#include <System.IOUtils.hpp>

#pragma package(smart_init)

namespace
{
    const unsigned short FirstPeerPort = 50120;
    const unsigned short LastPeerPort = 50135;
    const double PeerExpiryDays = 8.0 / 86400.0;
    const double ResendDays = 0.25 / 86400.0;
    const double ChatResendDays = 0.15 / 86400.0;
    const double BoundaryHeartbeatDays = 5.0 / 86400.0;
    const double ReceivedIDExpiryDays = 1.0;
    const double PendingChatExpiryDays = 30.0 / 86400.0;
    const double WTTStateResendDays = 1.0 / 86400.0;
    const __int64 MaximumMultiplayerLogBytes = 10 * 1024 * 1024;

    struct TReceivedPeerPacket
    {
        AnsiString Message;
        AnsiString Address;
        unsigned short Port;
    };

    struct TGeneratedWTTVisit
    {
        int ServiceIndex;
        int FirstLocal;
        int LastLocal;
        AnsiString Reference;
        AnsiString ContinuationHeader;
        AnsiString EntryRemainder;
        AnsiString FirstLocation;
        AnsiString LastLocation;
        AnsiString FirstPlatform;
        AnsiString LastPlatform;
        AnsiString PreviousLocation;
        AnsiString NextLocation;
        int ArrivalSeconds;
        int DepartureSeconds;
        bool OriginatesHere;
        bool TerminatesHere;
        std::vector<int> EntryIndices;

        TGeneratedWTTVisit() : ServiceIndex(-1), FirstLocal(-1), LastLocal(-1), ArrivalSeconds(-1),
                               DepartureSeconds(-1), OriginatesHere(false), TerminatesHere(false) {}
    };

    struct TWTTFormationCandidate
    {
        int ParentVisit;
        int ChildVisit;
        int TurnaroundSeconds;
    };

    bool WTTFormationCandidateEarlier(const TWTTFormationCandidate& First,
                                      const TWTTFormationCandidate& Second)
    {
        if(First.TurnaroundSeconds != Second.TurnaroundSeconds)
            return First.TurnaroundSeconds < Second.TurnaroundSeconds;
        if(First.ParentVisit != Second.ParentVisit) return First.ParentVisit < Second.ParentVisit;
        return First.ChildVisit < Second.ChildVisit;
    }

    AnsiString NormalisedWTTValue(const AnsiString& Value)
    {
        const AnsiString result = Value.Trim().UpperCase();
        return result == ".." ? "" : result;
    }

    bool IsLoopbackAddress(const AnsiString& Address)
    {
        return Address == "127.0.0.1" || Address == "::1";
    }

    AnsiString WTTClockText(int Seconds)
    {
        if(Seconds < 0) Seconds = 0;
        const int hours = Seconds / 3600;
        const int minutes = (Seconds % 3600) / 60;
        return (hours < 10 ? "0" : "") + AnsiString(hours) + ":" + (minutes < 10 ? "0" : "") + AnsiString(minutes);
    }

    AnsiString WTTFullClockText(int Seconds)
    {
        if(Seconds < 0) return "--:--";
        const int seconds = Seconds % 60;
        AnsiString result = WTTClockText(Seconds);
        if(seconds > 0)
        {
            result += ":";
            if(seconds < 10) result += "0";
            result += AnsiString(seconds);
        }
        return result;
    }

    AnsiString TimetableSafeText(const AnsiString& Value)
    {
        AnsiString result = StringReplace(Value, ";", " ", TReplaceFlags() << rfReplaceAll);
        return StringReplace(result, ",", " ", TReplaceFlags() << rfReplaceAll);
    }

    AnsiString StableWTTReference(const TMultiplayerWTTService& Service, const AnsiString& BoxCode, int VisitStart)
    {
        AnsiString headCode = Service.TID.Trim();
        if(headCode == "") headCode = Service.UID.Trim();
        if(headCode.Length() > 4) headCode = headCode.SubString(headCode.Length() - 3, 4);

        const AnsiString key = Service.RunKey.Trim() == "" ?
                               Service.UID + "|" + Service.TID + "|" + Service.Origin + "|" + Service.Destination :
                               Service.RunKey;
        const AnsiString visitKey = key + "|" + BoxCode.UpperCase() + "|" + AnsiString(VisitStart);
        unsigned int hash = 2166136261u;
        for(int character = 1; character <= visitKey.Length(); ++character)
        {
            hash ^= static_cast<unsigned char>(visitKey[character]);
            hash *= 16777619u;
        }
        const char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        AnsiString prefix;
        for(int digit = 0; digit < 4; ++digit)
        {
            prefix = AnsiString(alphabet[hash % 36]) + prefix;
            hash /= 36;
        }
        return prefix + headCode;
    }
}

__fastcall TMultiplayerPeerForm::TMultiplayerPeerForm(TComponent *Owner, TInterface *InterfaceOwner,
                                                       const AnsiString& BaseDirectory)
    : TForm(Owner, 0), OwnerInterface(InterfaceOwner), RootDirectory(BaseDirectory), PeerSocket(NULL), NetworkTimer(NULL),
      LocalPort(0), Joined(false), AutoMatch(false), ChatColour(clNavy), WTTModeEnabled(false),
      SessionClockOffsetDays(0.0), LastPassedWTTServiceCount(0), TickCounter(0)
{
    // Development builds live below Win64\Debug, while packaged builds keep their
    // data beside the executable.  Walk upwards only until the registry is found;
    // the process working directory is never used.
    AnsiString resourceRoot = RootDirectory;
    for(int level = 0; level < 4 && !FileExists(resourceRoot + "\\Multiplayer\\signalboxes.csv"); ++level)
    {
        const AnsiString parent = ExcludeTrailingPathDelimiter(ExtractFilePath(resourceRoot));
        if(parent == "" || parent == resourceRoot) break;
        resourceRoot = parent;
    }
    if(FileExists(resourceRoot + "\\Multiplayer\\signalboxes.csv")) RootDirectory = resourceRoot;
    const AnsiString logDirectory = RootDirectory + "\\Multiplayer logs";
    ForceDirectories(logDirectory);
    MultiplayerLogFile = logDirectory + "\\multiplayer-" +
                         FormatDateTime("yyyymmdd-hhnnss", TDateTime::CurrentDateTime()) + "-P" +
                         AnsiString(static_cast<int>(GetCurrentProcessId())) + ".log";
    LogMultiplayerEvent("LIFECYCLE", "Multiplayer form created; root=" + RootDirectory);
    Caption = "Railway multiplayer";
    BorderStyle = bsSizeable;
    Position = poScreenCenter;
    ClientWidth = 900;
    ClientHeight = 680;
    Constraints->MinWidth = 780;
    Constraints->MinHeight = 500;
    OnClose = FormCloseHandler;

    BuildInterface();
    LoadRegistry();
    BindPeerSocket();

    PeerID = "P" + IntToHex(static_cast<int>(GetCurrentProcessId()), 8) + IntToHex(LocalPort, 4);
    LogMultiplayerEvent("NETWORK", "Peer identity=" + PeerID + ", UDP port=" + AnsiString(LocalPort));
    ChatColour = ChooseChatColour();
    UserNameEdit->Text = "Player " + AnsiString(LocalPort);

    NetworkTimer = new TTimer(this);
    NetworkTimer->Interval = 50;
    NetworkTimer->OnTimer = NetworkTimerTick;
    NetworkTimer->Enabled = LocalPort != 0;
    RefreshStatus();
}

__fastcall TMultiplayerPeerForm::~TMultiplayerPeerForm()
{
    LogMultiplayerEvent("LIFECYCLE", "Multiplayer form destroyed");
    FlushMultiplayerLog();
    if(NetworkTimer) NetworkTimer->Enabled = false;
    if(PeerSocket) PeerSocket->Active = false;
}

void TMultiplayerPeerForm::BuildInterface()
{
    HeaderPanel = new TPanel(this);
    HeaderPanel->Parent = this;
    HeaderPanel->Align = alTop;
    HeaderPanel->Height = 78;
    HeaderPanel->BevelOuter = bvNone;

    TLabel *userLabel = new TLabel(this);
    userLabel->Parent = HeaderPanel;
    userLabel->Left = 12;
    userLabel->Top = 10;
    userLabel->Caption = "Name";
    UserNameEdit = new TEdit(this);
    UserNameEdit->Parent = HeaderPanel;
    UserNameEdit->Left = 52;
    UserNameEdit->Top = 6;
    UserNameEdit->Width = 145;
    UserNameEdit->MaxLength = 24;

    TLabel *boxLabel = new TLabel(this);
    boxLabel->Parent = HeaderPanel;
    boxLabel->Left = 214;
    boxLabel->Top = 10;
    boxLabel->Caption = "Box";
    BoxCombo = new TComboBox(this);
    BoxCombo->Parent = HeaderPanel;
    BoxCombo->Left = 247;
    BoxCombo->Top = 6;
    BoxCombo->Width = 205;
    BoxCombo->Style = csDropDownList;

    JoinButton = new TButton(this);
    JoinButton->Parent = HeaderPanel;
    JoinButton->Left = 468;
    JoinButton->Top = 5;
    JoinButton->Width = 116;
    JoinButton->Caption = "Join best session";
    JoinButton->OnClick = JoinButtonClick;

    NewSessionButton = new TButton(this);
    NewSessionButton->Parent = HeaderPanel;
    NewSessionButton->Left = 590;
    NewSessionButton->Top = 5;
    NewSessionButton->Width = 112;
    NewSessionButton->Caption = "New session";
    NewSessionButton->OnClick = NewSessionButtonClick;

    LeaveButton = new TButton(this);
    LeaveButton->Parent = HeaderPanel;
    LeaveButton->Left = 708;
    LeaveButton->Top = 5;
    LeaveButton->Width = 84;
    LeaveButton->Caption = "Leave";
    LeaveButton->OnClick = LeaveButtonClick;
    LeaveButton->Enabled = false;

    StatusLabel = new TLabel(this);
    StatusLabel->Parent = HeaderPanel;
    StatusLabel->Left = 12;
    StatusLabel->Top = 42;
    StatusLabel->Width = 850;
    StatusLabel->Height = 27;
    StatusLabel->AutoSize = false;
    StatusLabel->Font->Style = TFontStyles() << fsBold;

    TPanel *leftPanel = new TPanel(this);
    leftPanel->Parent = this;
    leftPanel->Align = alLeft;
    leftPanel->Width = 355;
    leftPanel->BevelOuter = bvNone;

    TLabel *usersLabel = new TLabel(this);
    usersLabel->Parent = leftPanel;
    usersLabel->Align = alTop;
    usersLabel->Height = 22;
    usersLabel->Caption = "  Online users";
    usersLabel->Layout = tlCenter;

    UsersGrid = new TStringGrid(this);
    UsersGrid->Parent = leftPanel;
    UsersGrid->Align = alTop;
    UsersGrid->Height = 218;
    UsersGrid->ColCount = 4;
    UsersGrid->FixedRows = 1;
    UsersGrid->RowCount = 2;
    UsersGrid->Options = UsersGrid->Options >> goEditing;
    UsersGrid->Cells[0][0] = "User";
    UsersGrid->Cells[1][0] = "Box";
    UsersGrid->Cells[2][0] = "Session";
    UsersGrid->Cells[3][0] = "Port";
    UsersGrid->ColWidths[0] = 108;
    UsersGrid->ColWidths[1] = 45;
    UsersGrid->ColWidths[2] = 125;
    UsersGrid->ColWidths[3] = 48;

    TLabel *boxesLabel = new TLabel(this);
    boxesLabel->Parent = leftPanel;
    boxesLabel->Align = alTop;
    boxesLabel->Height = 22;
    boxesLabel->Caption = "  Signalboxes in use";
    boxesLabel->Layout = tlCenter;

    BoxesGrid = new TStringGrid(this);
    BoxesGrid->Parent = leftPanel;
    BoxesGrid->Align = alClient;
    BoxesGrid->ColCount = 4;
    BoxesGrid->FixedRows = 1;
    BoxesGrid->RowCount = 2;
    BoxesGrid->Options = BoxesGrid->Options >> goEditing;
    BoxesGrid->Cells[0][0] = "Box";
    BoxesGrid->Cells[1][0] = "Signalbox";
    BoxesGrid->Cells[2][0] = "Session";
    BoxesGrid->Cells[3][0] = "Operator";
    BoxesGrid->ColWidths[0] = 42;
    BoxesGrid->ColWidths[1] = 112;
    BoxesGrid->ColWidths[2] = 92;
    BoxesGrid->ColWidths[3] = 82;

    TPanel *rightPanel = new TPanel(this);
    rightPanel->Parent = this;
    rightPanel->Align = alClient;
    rightPanel->BevelOuter = bvNone;

    TLabel *chatLabel = new TLabel(this);
    chatLabel->Parent = rightPanel;
    chatLabel->Align = alTop;
    chatLabel->Height = 22;
    chatLabel->Caption = "  Chat";
    chatLabel->Layout = tlCenter;

    TransferMemo = new TMemo(this);
    TransferMemo->Parent = rightPanel;
    TransferMemo->Align = alBottom;
    TransferMemo->Height = 118;
    TransferMemo->ReadOnly = true;
    TransferMemo->ScrollBars = ssVertical;
    TransferMemo->Lines->Add("Transfer activity");

    WTTPanel = new TPanel(this);
    WTTPanel->Parent = rightPanel;
    WTTPanel->Align = alBottom;
    WTTPanel->Height = 172;
    WTTPanel->BevelOuter = bvLowered;

    WTTEnableButton = new TButton(this);
    WTTEnableButton->Parent = WTTPanel;
    WTTEnableButton->Left = 7;
    WTTEnableButton->Top = 7;
    WTTEnableButton->Width = 126;
    WTTEnableButton->Caption = "Enable Live WTT";
    WTTEnableButton->OnClick = WTTEnableButtonClick;

    WTTStatusLabel = new TLabel(this);
    WTTStatusLabel->Parent = WTTPanel;
    WTTStatusLabel->Left = 143;
    WTTStatusLabel->Top = 11;
    WTTStatusLabel->Width = 360;
    WTTStatusLabel->AutoSize = false;
    WTTStatusLabel->Caption = "Multiplayer-only scheduled services are off.";

    WTTServicesList = new TListBox(this);
    WTTServicesList->Parent = WTTPanel;
    WTTServicesList->Left = 7;
    WTTServicesList->Top = 38;
    WTTServicesList->Width = WTTPanel->ClientWidth - 14;
    WTTServicesList->Height = 76;
    WTTServicesList->Anchors = TAnchors() << akLeft << akTop << akRight;
    WTTServicesList->OnClick = WTTServiceListClick;
    WTTServicesList->Items->Add("Join a session, then enable Live WTT.");

    WTTStopCombo = new TComboBox(this);
    WTTStopCombo->Parent = WTTPanel;
    WTTStopCombo->Left = 7;
    WTTStopCombo->Top = 124;
    WTTStopCombo->Width = 190;
    WTTStopCombo->Style = csDropDownList;

    WTTSkipButton = new TButton(this);
    WTTSkipButton->Parent = WTTPanel;
    WTTSkipButton->Left = 205;
    WTTSkipButton->Top = 122;
    WTTSkipButton->Width = 105;
    WTTSkipButton->Caption = "Skip local stop";
    WTTSkipButton->OnClick = WTTSkipButtonClick;

    WTTCancelButton = new TButton(this);
    WTTCancelButton->Parent = WTTPanel;
    WTTCancelButton->Left = 318;
    WTTCancelButton->Top = 122;
    WTTCancelButton->Width = 100;
    WTTCancelButton->Caption = "Cancel service";
    WTTCancelButton->OnClick = WTTCancelButtonClick;

    TPanel *chatEntryPanel = new TPanel(this);
    chatEntryPanel->Parent = rightPanel;
    chatEntryPanel->Align = alBottom;
    chatEntryPanel->Height = 38;
    chatEntryPanel->BevelOuter = bvNone;

    ChatScopeCombo = new TComboBox(this);
    ChatScopeCombo->Parent = chatEntryPanel;
    ChatScopeCombo->Left = 6;
    ChatScopeCombo->Top = 7;
    ChatScopeCombo->Width = 102;
    ChatScopeCombo->Style = csDropDownList;
    ChatScopeCombo->Items->Add("Surrounding");
    ChatScopeCombo->Items->Add("Session");
    ChatScopeCombo->Items->Add("Global");
    ChatScopeCombo->ItemIndex = 1;

    ChatEdit = new TEdit(this);
    ChatEdit->Parent = chatEntryPanel;
    ChatEdit->Left = 114;
    ChatEdit->Top = 7;
    ChatEdit->Width = 320;
    ChatEdit->Anchors = TAnchors() << akLeft << akTop << akRight;
    ChatEdit->OnKeyPress = ChatEditKeyPress;

    SendChatButton = new TButton(this);
    SendChatButton->Parent = chatEntryPanel;
    SendChatButton->Top = 5;
    SendChatButton->Width = 72;
    SendChatButton->Height = 27;
    SendChatButton->Anchors = TAnchors() << akTop << akRight;
    SendChatButton->Left = chatEntryPanel->ClientWidth - 78;
    SendChatButton->Caption = "Send";
    SendChatButton->OnClick = SendChatButtonClick;

    ChatMemo = new TRichEdit(this);
    ChatMemo->Parent = rightPanel;
    ChatMemo->Align = alClient;
    ChatMemo->ReadOnly = true;
    ChatMemo->ScrollBars = ssVertical;
    ChatMemo->PlainText = false;
    ChatMemo->Lines->Add("Peer-to-peer chat. Join a session to begin.");
}

void TMultiplayerPeerForm::LoadRegistry()
{
    Signalboxes.clear();
    Connections.clear();
    WTTBooks.clear();
    WTTBoundaries.clear();
    BoxCombo->Items->Clear();
    std::set<AnsiString> boxCodes;
    std::set<AnsiString> connectionKeys;

    AnsiString boxesFile = RootDirectory + "\\Multiplayer\\signalboxes.csv";
    std::ifstream boxes(boxesFile.c_str());
    std::string line;
    while(std::getline(boxes, line))
    {
        AnsiString value(line.c_str());
        value = value.Trim();
        if(value == "" || value[1] == '#') continue;
        std::vector<AnsiString> parts = Split(value, ',');
        if(parts.size() < 2) continue;
        TSignalboxDefinition definition;
        definition.Code = parts[0].Trim().UpperCase();
        definition.Name = parts[1].Trim();
        definition.RailwayTitle = parts.size() > 2 ? parts[2].Trim() : definition.Name;
        if(definition.Code == "" || definition.Name == "" || boxCodes.count(definition.Code))
        {
            LogMultiplayerEvent("REGISTRY-REJECT", "Invalid or duplicate signalbox row: " + value);
            continue;
        }
        boxCodes.insert(definition.Code);
        Signalboxes.push_back(definition);
        BoxCombo->Items->Add(definition.Code + " - " + definition.Name);
    }

    AnsiString connectionsFile = RootDirectory + "\\Multiplayer\\connections.csv";
    std::ifstream connections(connectionsFile.c_str());
    while(std::getline(connections, line))
    {
        AnsiString value(line.c_str());
        value = value.Trim();
        if(value == "" || value[1] == '#') continue;
        std::vector<AnsiString> parts = Split(value, ',');
        if(parts.size() < 4) continue;
        TConnectionDefinition definition;
        definition.FromBox = parts[0].Trim().UpperCase();
        definition.LocalPortal = parts[1].Trim().UpperCase();
        definition.ToBox = parts[2].Trim().UpperCase();
        definition.RemotePortal = parts[3].Trim().UpperCase();
        const AnsiString key = definition.FromBox + "|" + definition.LocalPortal + "|" +
                               definition.ToBox + "|" + definition.RemotePortal;
        if(!boxCodes.count(definition.FromBox) || !boxCodes.count(definition.ToBox) ||
           definition.FromBox == definition.ToBox || definition.LocalPortal == "" || definition.RemotePortal == "" ||
           connectionKeys.count(key))
        {
            LogMultiplayerEvent("REGISTRY-REJECT", "Invalid or duplicate connection row: " + value);
            continue;
        }
        connectionKeys.insert(key);
        Connections.push_back(definition);
    }

    AnsiString wttBooksFile = RootDirectory + "\\Multiplayer\\wtt_books.csv";
    std::ifstream wttBooks(wttBooksFile.c_str());
    while(std::getline(wttBooks, line))
    {
        AnsiString value(line.c_str());
        value = value.Trim();
        if(value == "" || value[1] == '#') continue;
        std::vector<AnsiString> parts = Split(value, ',');
        if(parts.size() < 2) continue;
        TWTTBookDefinition definition;
        definition.BoxCode = parts[0].Trim().UpperCase();
        definition.BookCode = parts[1].Trim().UpperCase();
        definition.CacheFile = parts.size() > 2 ? parts[2].Trim() : definition.BookCode + ".tsv";
        if(!boxCodes.count(definition.BoxCode) || definition.BookCode == "" || definition.CacheFile == "")
        {
            LogMultiplayerEvent("REGISTRY-REJECT", "Invalid WTT book row: " + value);
            continue;
        }
        WTTBooks.push_back(definition);
    }

    AnsiString wttBoundariesFile = RootDirectory + "\\Multiplayer\\wtt_boundaries.csv";
    std::ifstream wttBoundaries(wttBoundariesFile.c_str());
    while(std::getline(wttBoundaries, line))
    {
        AnsiString value(line.c_str());
        value = value.Trim();
        if(value == "" || value[1] == '#') continue;
        std::vector<AnsiString> parts = Split(value, ',');
        if(parts.size() < 3) continue;
        TWTTBoundaryDefinition definition;
        definition.BoxCode = parts[0].Trim().UpperCase();
        definition.WTTLocation = parts[1].Trim().UpperCase();
        definition.BoundaryNames = parts[2].Trim().UpperCase();
        if(!boxCodes.count(definition.BoxCode) || definition.WTTLocation == "" || definition.BoundaryNames == "")
        {
            LogMultiplayerEvent("REGISTRY-REJECT", "Invalid WTT boundary row: " + value);
            continue;
        }
        WTTBoundaries.push_back(definition);
    }

    std::vector<TConnectionDefinition> reciprocalConnections;
    for(std::vector<TConnectionDefinition>::const_iterator connection = Connections.begin();
        connection != Connections.end(); ++connection)
    {
        const AnsiString reverse = connection->ToBox + "|" + connection->RemotePortal + "|" +
                                   connection->FromBox + "|" + connection->LocalPortal;
        if(connectionKeys.count(reverse)) reciprocalConnections.push_back(*connection);
        else LogMultiplayerEvent("REGISTRY-REJECT", "Connection has no reciprocal row: " +
            connection->FromBox + "/" + connection->LocalPortal + " -> " +
            connection->ToBox + "/" + connection->RemotePortal);
    }
    Connections.swap(reciprocalConnections);
    if(Signalboxes.empty() || Connections.empty())
    {
        AppendTransfer("Multiplayer registry is incomplete. Check Multiplayer\\signalboxes.csv and connections.csv.");
    }
    if(BoxCombo->Items->Count > 0) BoxCombo->ItemIndex = 0;
    LogMultiplayerEvent("REGISTRY", "Loaded signalboxes=" + AnsiString(static_cast<int>(Signalboxes.size())) +
                        ", connections=" + AnsiString(static_cast<int>(Connections.size())) +
                        ", WTT books=" + AnsiString(static_cast<int>(WTTBooks.size())) +
                        ", external boundaries=" + AnsiString(static_cast<int>(WTTBoundaries.size())));
}

void TMultiplayerPeerForm::BindPeerSocket()
{
    PeerSocket = new TIdUDPClient(this);
    PeerSocket->Active = false;
    PeerSocket->BroadcastEnabled = true;
    for(unsigned short port = FirstPeerPort; port <= LastPeerPort; ++port)
    {
        try
        {
            PeerSocket->Active = false;
            PeerSocket->BoundIP = "0.0.0.0";
            PeerSocket->BoundPort = port;
            PeerSocket->Active = true;
            LocalPort = port;
            LogMultiplayerEvent("NETWORK", "Bound UDP port " + AnsiString(LocalPort));
            break;
        }
        catch(const EIdException&)
        {
            PeerSocket->Active = false;
        }
    }
    if(LocalPort == 0)
        AppendTransfer("Unable to bind a local multiplayer port (50120-50135).");
}

void TMultiplayerPeerForm::ShowForRailway(const AnsiString& RailwayTitle)
{
    AnsiString detected = DetectBoxCode(RailwayTitle);
    RailwayBoxCode = detected;
    if(detected != "")
    {
        for(int index = 0; index < BoxCombo->Items->Count; ++index)
        {
            if(AnsiString(BoxCombo->Items->Strings[index]).SubString(1, detected.Length()) == detected)
            {
                BoxCombo->ItemIndex = index;
                break;
            }
        }
    }
    Show();
    BringToFront();
}

void __fastcall TMultiplayerPeerForm::NetworkTimerTick(TObject *Sender)
{
    ++TickCounter;
    ReceiveMessages();
    SendPendingChats();
    if((TickCounter % 2) == 0) SendBoundaryStates();
    if((TickCounter % 10) == 0) SendWTTStates();
    if((TickCounter % 5) == 0) SendPendingTransfers();
    if((TickCounter % 20) == 0)
    {
        // Train offers are sent on the faster path above.  Discovery and GUI
        // maintenance stay at one second so they cannot delay a handover.
        SendDiscovery();
        PrunePeers();
        ResolveSessionMatch();
        ResolveDuplicateOccupation();
        RefreshUsersGrid();
        RefreshBoxesGrid();
        RefreshStatus();
        RefreshWTTServices();
        FlushMultiplayerLog();
    }
}

void TMultiplayerPeerForm::PumpNetwork()
{
    ReceiveMessages();
    SendPendingChats();
    SendBoundaryStates();
    SendWTTStates();
}

void TMultiplayerPeerForm::SendDiscovery()
{
    if(!PeerSocket || LocalPort == 0) return;
    AnsiString user = CleanField(UserNameEdit->Text);
    const __int64 railwayClock = WTTModeEnabled && OwnerInterface ?
                                 OwnerInterface->MultiplayerWTTRailwayClockMilliseconds() : -1;
    AnsiString message = "HELLO|1|" + PeerID + "|" + AnsiString(LocalPort) + "|" + user + "|" + BoxCode + "|" +
                         SessionID + "|" + (Joined ? "1" : "0") + "|" + (AutoMatch ? "1" : "0") + "|" +
                         AnsiString(ChatColour) + "|" + (WTTModeEnabled ? "1" : "0") + "|" +
                         WTTCatalog.BookSummary() + "|" + ClockAuthorityPeerID + "|" +
                         IntToStr(SessionClockMilliseconds()) + "|" + IntToStr(railwayClock);
    LogMultiplayerEvent("TX-DISCOVERY", "HELLO sweep box=" + BoxCode + ", session=" + SessionID +
                        ", joined=" + (Joined ? "1" : "0") + ", WTT=" + (WTTModeEnabled ? "1" : "0"));
    for(unsigned short port = FirstPeerPort; port <= LastPeerPort; ++port)
    {
        if(port == LocalPort) continue;
        SendTo("127.0.0.1", port, message);
        SendTo("255.255.255.255", port, message);
    }
}

void TMultiplayerPeerForm::ReceiveMessages()
{
    if(!PeerSocket || !PeerSocket->Active) return;
    std::map<AnsiString, TReceivedPeerPacket> latestHellos;
    std::map<AnsiString, TReceivedPeerPacket> latestBackground;
    std::vector<TReceivedPeerPacket> urgent;
    std::vector<TReceivedPeerPacket> ordinary;
    int collapsed = 0;
    int received = 0;
    for(int count = 0; count < 4096; ++count)
    {
        try
        {
            TBytes buffer;
            buffer.Length = 8192;
            UnicodeString address;
            unsigned short port = 0;
            buffer.Length = PeerSocket->ReceiveBuffer(buffer, address, port, 1);
            if(buffer.Length == 0) break;
            ++received;
            TReceivedPeerPacket packet;
            packet.Message = AnsiString(BytesToString(buffer));
            packet.Address = AnsiString(address);
            packet.Port = port;
            std::vector<AnsiString> parts = Split(packet.Message, '|');
            const AnsiString type = parts.empty() ? "" : parts[0];
            if(type == "HELLO" && parts.size() > 2)
            {
                std::map<AnsiString, TReceivedPeerPacket>::iterator existingHello = latestHellos.find(parts[2]);
                if(existingHello != latestHellos.end())
                {
                    ++collapsed;
                    // A peer on this PC advertises through both loopback and LAN
                    // broadcast.  Keep loopback so its address cannot flap between
                    // the two routes while a train offer is in flight.
                    if(IsLoopbackAddress(existingHello->second.Address) && !IsLoopbackAddress(packet.Address))
                        continue;
                }
                latestHellos[parts[2]] = packet;
            }
            else if(type == "TRAIN" || type == "ACK" || type == "NACK" ||
                    type == "CHAT" || type == "CHATACK")
                urgent.push_back(packet);
            else if(type == "BOUNDARY" && parts.size() > 7)
            {
                const AnsiString key = type + "|" + parts[2] + "|" + parts[7];
                if(latestBackground.count(key)) ++collapsed;
                latestBackground[key] = packet;
            }
            else if(type == "WTTSTATE" && parts.size() > 14)
            {
                const AnsiString key = type + "|" + parts[12] + "|" + parts[4] + "|" + parts[13] + "|" + parts[14];
                if(latestBackground.count(key)) ++collapsed;
                latestBackground[key] = packet;
            }
            else if(type == "WTTACK" && parts.size() > 7)
            {
                const AnsiString key = type + "|" + parts[3] + "|" + parts[5] + "|" + parts[6] + "|" + parts[7];
                if(latestBackground.count(key)) ++collapsed;
                latestBackground[key] = packet;
            }
            else
                ordinary.push_back(packet);
        }
        catch(const EIdException&)
        {
            break;
        }
    }

    // Establish the newest peer address first, then process delivery-critical
    // traffic ahead of replaceable heartbeats and state snapshots.  This also
    // empties a backlog accumulated while another instance was not focused.
    for(std::map<AnsiString, TReceivedPeerPacket>::const_iterator packet = latestHellos.begin();
        packet != latestHellos.end(); ++packet)
        ProcessMessage(packet->second.Message, packet->second.Address, packet->second.Port);
    for(std::vector<TReceivedPeerPacket>::const_iterator packet = urgent.begin(); packet != urgent.end(); ++packet)
        ProcessMessage(packet->Message, packet->Address, packet->Port);
    for(std::vector<TReceivedPeerPacket>::const_iterator packet = ordinary.begin(); packet != ordinary.end(); ++packet)
        ProcessMessage(packet->Message, packet->Address, packet->Port);
    for(std::map<AnsiString, TReceivedPeerPacket>::const_iterator packet = latestBackground.begin();
        packet != latestBackground.end(); ++packet)
        ProcessMessage(packet->second.Message, packet->second.Address, packet->second.Port);
    if(collapsed > 0)
        LogMultiplayerEvent("RX-COLLAPSE", "Drained " + AnsiString(received) + " packets; discarded " +
                            AnsiString(collapsed) + " superseded state/discovery packets");
}

void TMultiplayerPeerForm::ProcessMessage(const AnsiString& Message, const AnsiString& Address, unsigned short Port)
{
    LogMultiplayerEvent("RX", Address + ":" + AnsiString(Port) + " " + Message);
    std::vector<AnsiString> parts = Split(Message, '|');
    if(parts.empty())
    {
        LogMultiplayerEvent("RX-REJECT", "Empty packet");
        return;
    }
    if(parts[0] == "HELLO") ProcessHello(parts, Address, Port);
    else if(parts[0] == "CHAT") ProcessChat(parts, Address, Port);
    else if(parts[0] == "CHATACK") ProcessChatAcknowledgement(parts);
    else if(parts[0] == "TRAIN") ProcessTrain(parts, Address, Port);
    else if(parts[0] == "ACK") ProcessAcknowledgement(parts);
    else if(parts[0] == "NACK") ProcessNegativeAcknowledgement(parts);
    else if(parts[0] == "BOUNDARY") ProcessBoundaryState(parts, Address, Port);
    else if(parts[0] == "WTTSTATE") ProcessWTTState(parts, Address, Port);
    else if(parts[0] == "WTTACK") ProcessWTTAcknowledgement(parts);
    else LogMultiplayerEvent("RX-REJECT", "Unknown packet type=" + parts[0]);
}

void TMultiplayerPeerForm::ProcessHello(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port)
{
    if(Parts.size() < 8 || Parts[2] == PeerID) return;
    TPeerState state;
    state.PeerID = Parts[2];
    state.Port = static_cast<unsigned short>(Parts[3].ToIntDef(Port));
    state.UserName = Parts[4];
    state.BoxCode = Parts[5];
    state.SessionID = Parts[6];
    state.Active = Parts[7] == "1";
    state.AutoMatch = Parts.size() > 8 && Parts[8] == "1";
    state.ChatColour = Parts.size() > 9 ? Parts[9].ToIntDef(clNavy) : clNavy;
    state.WTTEnabled = Parts.size() > 10 && Parts[10] == "1";
    state.WTTFingerprint = Parts.size() > 11 ? Parts[11] : "";
    state.ClockAuthorityPeerID = Parts.size() > 12 ? Parts[12] : "";
    state.SessionClockMilliseconds = Parts.size() > 13 ? StrToInt64Def(Parts[13], 0) : 0;
    state.RailwayClockMilliseconds = Parts.size() > 14 ? StrToInt64Def(Parts[14], -1) : -1;
    std::map<AnsiString, TPeerState>::const_iterator existing = Peers.find(state.PeerID);
    state.Address = (existing != Peers.end() && IsLoopbackAddress(existing->second.Address) &&
                     !IsLoopbackAddress(Address)) ? existing->second.Address : Address;
    state.LastSeen = TDateTime::CurrentDateTime();
    Peers[state.PeerID] = state;

    const AnsiString oldSession = SessionID;
    ResolveSessionMatch();
    ResolveDuplicateOccupation();
    if(WTTModeEnabled && Joined) ReconcileSessionClock();
    if(Joined && (AutoMatch || state.AutoMatch) && (oldSession != SessionID || state.SessionID != SessionID))
        SendDiscovery();
    RefreshUsersGrid();
    RefreshBoxesGrid();
    RefreshStatus();
}

void TMultiplayerPeerForm::ProcessChat(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port)
{
    if(Parts.size() < 9) return;
    const AnsiString eventID = Parts[2];
    const AnsiString senderPeerID = Parts[7];
    std::map<AnsiString, TPeerState>::const_iterator sender = Peers.find(senderPeerID);
    if(sender == Peers.end() || !sender->second.Active ||
       sender->second.Port != Port || sender->second.BoxCode != Parts[5])
    {
        LogMultiplayerEvent("CHAT-REJECT", "Unknown or mismatched sender=" + senderPeerID + " from " + Address);
        return;
    }
    SendTo(Address, Port, "CHATACK|1|" + eventID + "|" + senderPeerID + "|" + PeerID);
    if(ReceivedChatIDs.count(eventID)) return;
    const AnsiString scope = Parts[3];
    const AnsiString messageSession = Parts[4];
    const AnsiString fromBox = Parts[5];
    const AnsiString user = Parts[6];
    const AnsiString text = Parts[8];
    int colour = Parts.size() > 9 ? Parts[9].ToIntDef(clNavy) : clNavy;
    if(scope == "Session" && (!Joined || messageSession != SessionID)) return;
    if(scope == "Surrounding" && (!Joined || messageSession != SessionID || !BoxesAreAdjacent(BoxCode, fromBox))) return;
    ReceivedChatIDs[eventID] = TDateTime::CurrentDateTime();
    AppendAreaChat(SignalboxName(fromBox), user, text, colour);
}

void TMultiplayerPeerForm::ProcessChatAcknowledgement(const std::vector<AnsiString>& Parts)
{
    if(Parts.size() < 5 || Parts[3] != PeerID) return;
    for(std::vector<TPendingChatDelivery>::iterator it = PendingChatDeliveries.begin(); it != PendingChatDeliveries.end(); )
    {
        if(it->EventID == Parts[2] && it->TargetPeerID == Parts[4]) it = PendingChatDeliveries.erase(it);
        else ++it;
    }
}

void TMultiplayerPeerForm::ProcessTrain(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port)
{
    if(Parts.size() < 12 || !Joined)
    {
        LogMultiplayerEvent("TRAIN-REJECT", "Malformed offer or local peer not joined");
        return;
    }
    const AnsiString eventID = Parts[2];
    const AnsiString transferSession = Parts[3];
    const AnsiString fromBox = Parts[4];
    const AnsiString toBox = Parts[5];
    const AnsiString fromPortal = Parts[6];
    const AnsiString toPortal = Parts[7];
    const AnsiString service = Parts[8];
    const AnsiString headCode = Parts[9];
    const int repeatNumber = Parts[10].ToIntDef(0);
    const AnsiString senderPeerID = Parts[11];
    const AnsiString runKey = Parts.size() > 12 ? Parts[12] : "";
    if(transferSession != SessionID || toBox != BoxCode)
    {
        LogMultiplayerEvent("TRAIN-REJECT", "Event=" + eventID + ", session/to-box mismatch; packet session=" +
                            transferSession + ", local session=" + SessionID + ", packet box=" + toBox +
                            ", local box=" + BoxCode);
        return;
    }
    if(!ValidateConnectionPacket(senderPeerID, fromBox, fromPortal, toPortal, Address, Port))
    {
        LogMultiplayerEvent("TRAIN-REJECT", "Event=" + eventID + ", sender or registered boundary mismatch");
        return;
    }
    if(!OwnerInterface || !OwnerInterface->MultiplayerPortalAllowsEntry(toPortal))
    {
        const AnsiString reason = "Destination portal " + toPortal + " is not configured to accept entering trains.";
        SendTo(Address, Port, "NACK|1|" + eventID + "|" + SessionID + "|" + PeerID + "|" + senderPeerID + "|" +
               CleanField(reason));
        LogMultiplayerEvent("TRAIN-DIRECTION-REJECT", "Event=" + eventID + ", from=" + fromBox +
                            ", portal=" + toPortal + ", reason=" + reason);
        return;
    }

    AnsiString localServiceReference = service;
    if(runKey != "")
    {
        const int serviceIndex = WTTCatalog.FindService(runKey);
        std::pair<std::multimap<int, AnsiString>::const_iterator, std::multimap<int, AnsiString>::const_iterator> references =
            WTTServiceReferences.equal_range(serviceIndex);
        std::multimap<int, AnsiString>::const_iterator localReference = references.first;
        for(std::multimap<int, AnsiString>::const_iterator candidate = references.first; candidate != references.second; ++candidate)
        {
            std::map<AnsiString, AnsiString>::const_iterator expected = WTTExpectedEntryPortals.find(candidate->second);
            if(expected == WTTExpectedEntryPortals.end()) continue;
            const std::vector<AnsiString> portals = Split(expected->second, ';');
            for(std::vector<AnsiString>::const_iterator portal = portals.begin(); portal != portals.end(); ++portal)
                if(portal->UpperCase() == toPortal.UpperCase()) localReference = candidate;
        }
        if(localReference != references.second) localServiceReference = localReference->second;
        else if(WTTModeEnabled)
        {
            const AnsiString reason = "No installed local visit exists for WTT run " + runKey + ".";
            SendTo(Address, Port, "NACK|1|" + eventID + "|" + SessionID + "|" + PeerID + "|" + senderPeerID + "|" +
                   CleanField(reason));
            LogMultiplayerEvent("TRAIN-REJECT", "Event=" + eventID + ", " + reason);
            return;
        }
        LogMultiplayerEvent("TRAIN-IDENTITY", "Event=" + eventID + ", run-key=" + runKey +
                            ", sender reference=" + service + ", local reference=" + localServiceReference +
                            ", catalog index=" + AnsiString(serviceIndex));
    }

    std::map<AnsiString, AnsiString>::const_iterator expectedEntry = WTTExpectedEntryPortals.find(localServiceReference);
    if(expectedEntry != WTTExpectedEntryPortals.end())
    {
        bool portalAllowed = false;
        const std::vector<AnsiString> portals = Split(expectedEntry->second, ';');
        for(std::vector<AnsiString>::const_iterator portal = portals.begin(); portal != portals.end(); ++portal)
            if(portal->UpperCase() == toPortal.UpperCase()) portalAllowed = true;
        if(!portalAllowed)
        {
            const AnsiString reason = "WTT visit expects a different entry portal (allowed " + expectedEntry->second + ").";
            SendTo(Address, Port, "NACK|1|" + eventID + "|" + SessionID + "|" + PeerID + "|" + senderPeerID + "|" +
                   CleanField(reason));
            LogMultiplayerEvent("TRAIN-REJECT", "Event=" + eventID + ", portal=" + toPortal + ", " + reason);
            return;
        }
    }

    const bool duplicate = ReceivedTransferIDs.count(eventID) > 0;
    bool accepted = duplicate;
    AnsiString failureReason;
    if(!accepted && OwnerInterface)
        accepted = OwnerInterface->ReceiveMultiplayerTrain(localServiceReference, headCode, repeatNumber,
                                                           fromBox, toPortal, failureReason);

    if(accepted)
    {
        if(!duplicate && runKey != "") WTTCatalog.ReportHandover(runKey, BoxCode, SessionDateTime());
        ReceivedTransferIDs[eventID] = TDateTime::CurrentDateTime();
        if(!duplicate) AppendTransfer("Received " + headCode + " from " + fromBox + " at " + toPortal + ".");
        SendTo(Address, Port, "ACK|1|" + eventID + "|" + SessionID + "|" + PeerID + "|" + senderPeerID);
    }
    else
    {
        if(failureReason == "") failureReason = OwnerInterface ? "Receiver did not accept the train." : "Railway interface unavailable.";
        LogMultiplayerEvent("TRAIN-REJECT", "Event=" + eventID + ", headcode=" + headCode + ", portal=" +
                            toPortal + ", reference=" + localServiceReference + ", reason=" + failureReason);
        SendTo(Address, Port, "NACK|1|" + eventID + "|" + SessionID + "|" + PeerID + "|" + senderPeerID + "|" +
               CleanField(failureReason));
    }
}

void TMultiplayerPeerForm::ProcessAcknowledgement(const std::vector<AnsiString>& Parts)
{
    if(Parts.size() < 6 || Parts[3] != SessionID || Parts[5] != PeerID) return;
    for(std::vector<TPendingTransfer>::iterator it = PendingTransfers.begin(); it != PendingTransfers.end(); ++it)
    {
        if(it->EventID == Parts[2])
        {
            AppendTransfer("Transfer " + it->HeadCode + " accepted by " + it->ToBox + ".");
            PendingTransfers.erase(it);
            break;
        }
    }
}

void TMultiplayerPeerForm::ProcessNegativeAcknowledgement(const std::vector<AnsiString>& Parts)
{
    if(Parts.size() < 7 || Parts[3] != SessionID || Parts[5] != PeerID) return;
    for(std::vector<TPendingTransfer>::iterator it = PendingTransfers.begin(); it != PendingTransfers.end(); ++it)
    {
        if(it->EventID != Parts[2]) continue;
        const AnsiString reason = Parts[6];
        LogMultiplayerEvent("TRAIN-NACK", "Event=" + it->EventID + ", headcode=" + it->HeadCode +
                            ", from " + Parts[4] + ", reason=" + reason);
        if(it->LastRejection != reason)
        {
            it->LastRejection = reason;
            AppendTransfer(it->ToBox + " has not accepted " + it->HeadCode + ": " + reason);
        }

        const AnsiString upperReason = reason.UpperCase();
        const bool permanent = upperReason.Pos("NO LOADED LIVE WTT SERVICE") > 0 ||
                               upperReason.Pos("NO INSTALLED LOCAL VISIT") > 0 ||
                               upperReason.Pos("DOES NOT EXIST AS A CONTINUATION") > 0 ||
                               upperReason.Pos("IS CONFIGURED AS EXIT ONLY") > 0 ||
                               upperReason.Pos("EXPECTS A DIFFERENT ENTRY PORTAL") > 0 ||
                               upperReason.Pos("ALREADY BEEN COMPLETED OR CANCELLED") > 0;
        if(permanent)
        {
            AppendTransfer("Stopped offering " + it->HeadCode + " after a permanent rejection from " +
                           it->ToBox + "; check that box's Live WTT load report.");
            LogMultiplayerEvent("TRAIN-STOP", "Event=" + it->EventID + ", permanent rejection; retries stopped");
            PendingTransfers.erase(it);
        }
        return;
    }
}

void TMultiplayerPeerForm::ProcessBoundaryState(const std::vector<AnsiString>& Parts, const AnsiString& Address,
                                                 unsigned short Port)
{
    if(Parts.size() < 10 || !Joined || Parts[3] != SessionID || Parts[5] != BoxCode) return;
    if(!ValidateConnectionPacket(Parts[2], Parts[4], Parts[6], Parts[7], Address, Port))
    {
        LogMultiplayerEvent("BOUNDARY-REJECT", "Sender or registered connection mismatch from " + Address);
        return;
    }
    const AnsiString localPortal = Parts[7].UpperCase();
    if(!OwnerInterface || !OwnerInterface->MultiplayerPortalAllowsExit(localPortal))
    {
        LogMultiplayerEvent("BOUNDARY-DIRECTION-REJECT", "Portal=" + localPortal +
                            " is not configured for outward running");
        return;
    }
    const int remoteAspect = Parts[8].ToIntDef(0);
    const unsigned int sequence = static_cast<unsigned int>(StrToInt64Def(Parts[9], 0));
    const AnsiString sequenceKey = Parts[2] + "|" + localPortal;
    if(sequence == 0 || sequence <= RemoteBoundarySequences[sequenceKey]) return;
    RemoteBoundarySequences[sequenceKey] = sequence;
    std::map<AnsiString, int>::iterator existing = RemoteBoundaryAspects.find(localPortal);
    if(existing != RemoteBoundaryAspects.end() && existing->second == remoteAspect) return;
    RemoteBoundaryAspects[localPortal] = remoteAspect;
    if(OwnerInterface) OwnerInterface->RefreshMultiplayerBoundarySignals(localPortal);
}

void TMultiplayerPeerForm::ProcessWTTState(const std::vector<AnsiString>& Parts, const AnsiString& Address,
                                            unsigned short Port)
{
    if(Parts.size() < 13 || !Joined || !WTTModeEnabled || Parts[2] != SessionID || Parts[12] == PeerID) return;
    std::map<AnsiString, TPeerState>::const_iterator sender = Peers.find(Parts[12]);
    if(sender == Peers.end() || !sender->second.Active || sender->second.SessionID != SessionID ||
       sender->second.Port != Port)
    {
        LogMultiplayerEvent("WTT-REJECT", "Unknown or mismatched sender=" + Parts[12]);
        return;
    }
    AnsiString mismatchBook;
    if(!WTTBooksCompatible(Parts[3], mismatchBook))
    {
        LogMultiplayerEvent("WTT-REJECT", "Incompatible " + mismatchBook + " from " + sender->second.BoxCode);
        return;
    }
    const AnsiString runKey = Parts[4];
    const int serviceRevision = Parts[5].ToIntDef(0);
    const int delaySeconds = Parts[6].ToIntDef(0);
    const bool cancelled = Parts[7] == "1";
    int callIndex = Parts[8].ToIntDef(-1);
    const int callRevision = Parts[9].ToIntDef(0);
    const int callStatus = Parts[10].ToIntDef(0);
    const AnsiString actorBox = Parts[11];
    const AnsiString operationalOwner = Parts.size() > 15 ? Parts[15] : "";
    const int serviceIndex = WTTCatalog.FindService(runKey);
    if(Parts.size() > 14 && Parts[13] != "")
        callIndex = WTTCatalog.FindCall(serviceIndex, Parts[13], Parts[14].ToIntDef(-1));
    const AnsiString callLocation = Parts.size() > 13 ? Parts[13] : "";
    const AnsiString callSeconds = Parts.size() > 14 ? Parts[14] : "-1";
    SendTo(Address, Port, "WTTACK|1|" + SessionID + "|" + Parts[12] + "|" + PeerID + "|" + runKey + "|" +
           callLocation + "|" + callSeconds + "|" + AnsiString(serviceRevision) + "|" + AnsiString(callRevision));
    if(WTTCatalog.ApplyRemoteState(runKey, serviceRevision, delaySeconds, cancelled, callIndex,
                                   callRevision, callStatus, actorBox, operationalOwner))
    {
        if(serviceIndex >= 0 && callIndex >= 0)
        {
            std::pair<std::multimap<int, AnsiString>::const_iterator, std::multimap<int, AnsiString>::const_iterator> references =
                WTTServiceReferences.equal_range(serviceIndex);
            for(std::multimap<int, AnsiString>::const_iterator reference = references.first;
                reference != references.second; ++reference)
                if(OwnerInterface) OwnerInterface->ApplyMultiplayerWTTSkip(reference->second,
                    WTTCatalog.AllServices()[serviceIndex].Calls[callIndex].RailwayLocation);
        }
        if(serviceIndex >= 0 && cancelled)
        {
            std::pair<std::multimap<int, AnsiString>::const_iterator, std::multimap<int, AnsiString>::const_iterator> references =
                WTTServiceReferences.equal_range(serviceIndex);
            for(std::multimap<int, AnsiString>::const_iterator reference = references.first;
                reference != references.second; ++reference)
                if(OwnerInterface) OwnerInterface->ApplyMultiplayerWTTCancellation(reference->second);
        }
        RefreshWTTServices();
    }
}

void TMultiplayerPeerForm::ProcessWTTAcknowledgement(const std::vector<AnsiString>& Parts)
{
    if(Parts.size() < 10 || Parts[2] != SessionID || Parts[3] != PeerID) return;
    std::map<AnsiString, TPeerState>::const_iterator receiver = Peers.find(Parts[4]);
    if(receiver == Peers.end() || !receiver->second.Active || receiver->second.SessionID != SessionID) return;
    const AnsiString base = Parts[4] + "|" + Parts[5] + "|" + Parts[6] + "|" + Parts[7];
    AcknowledgedWTTRevisions[base + "|S"] = std::max(AcknowledgedWTTRevisions[base + "|S"], Parts[8].ToIntDef(0));
    AcknowledgedWTTRevisions[base + "|C"] = std::max(AcknowledgedWTTRevisions[base + "|C"], Parts[9].ToIntDef(0));
}

void TMultiplayerPeerForm::SendTo(const AnsiString& Address, unsigned short Port, const AnsiString& Message)
{
    if(!PeerSocket || !PeerSocket->Active || Port == 0)
    {
        LogMultiplayerEvent("TX-REJECT", Address + ":" + AnsiString(Port) + " socket inactive; " + Message);
        return;
    }
    try
    {
        PeerSocket->SendBuffer(UnicodeString(Address), Port, ToBytes(UnicodeString(Message)));
        if(Message.SubString(1, 6) != "HELLO|")
            LogMultiplayerEvent("TX", Address + ":" + AnsiString(Port) + " " + Message);
    }
    catch(const EIdException& Error)
    {
        LogMultiplayerEvent("TX-ERROR", Address + ":" + AnsiString(Port) + " " + Error.Message);
    }
}

void TMultiplayerPeerForm::SendPendingTransfers()
{
    if(!Joined) return;
    const TDateTime now = TDateTime::CurrentDateTime();
    std::set<AnsiString> occupiedPortals;
    for(std::vector<TPendingTransfer>::iterator it = PendingTransfers.begin(); it != PendingTransfers.end(); ++it)
    {
        const AnsiString portalQueue = it->FromBox + "|" + it->FromPortal + "|" + it->ToBox + "|" + it->ToPortal;
        if(occupiedPortals.count(portalQueue)) continue;
        occupiedPortals.insert(portalQueue);
        if(double(it->LastSent) > 0.0 && (double(now) - double(it->LastSent)) < ResendDays) continue;
        const TPeerState *peer = FindActivePeerForBox(it->ToBox);
        if(!peer) continue;
        AnsiString message = "TRAIN|1|" + it->EventID + "|" + it->SessionID + "|" + it->FromBox + "|" + it->ToBox + "|" +
                             it->FromPortal + "|" + it->ToPortal + "|" + it->ServiceReference + "|" + it->HeadCode + "|" +
                             AnsiString(it->RepeatNumber) + "|" + PeerID + "|" + it->RunKey;
        ++it->Attempts;
        const int copies = it->Attempts == 1 ? 3 : 1;
        for(int copy = 0; copy < copies; ++copy) SendTo(peer->Address, peer->Port, message);
        it->LastSent = now;
        LogMultiplayerEvent("TRAIN-OFFER", "Event=" + it->EventID + ", attempt=" + AnsiString(it->Attempts) +
                            ", " + it->FromBox + "/" + it->FromPortal + " -> " + it->ToBox + "/" + it->ToPortal +
                            ", reference=" + it->ServiceReference + ", run-key=" + it->RunKey);
        if(it->Attempts == 1 || (it->Attempts % 10) == 0)
            AppendTransfer((it->Attempts == 1 ? "Offered " : "Still offering ") + it->HeadCode + " to " +
                           it->ToBox + " (attempt " + AnsiString(it->Attempts) + ").");
    }
}

void TMultiplayerPeerForm::SendPendingChats()
{
    const TDateTime now = TDateTime::CurrentDateTime();
    for(std::vector<TPendingChatDelivery>::iterator it = PendingChatDeliveries.begin(); it != PendingChatDeliveries.end(); )
    {
        if((double(now) - double(it->Created)) > PendingChatExpiryDays)
        {
            LogMultiplayerEvent("CHAT-EXPIRE", "Event=" + it->EventID + ", peer=" + it->TargetPeerID);
            it = PendingChatDeliveries.erase(it);
            continue;
        }
        if(double(it->LastSent) > 0.0 && (double(now) - double(it->LastSent)) < ChatResendDays)
        {
            ++it;
            continue;
        }
        SendTo(it->Address, it->Port, it->Message);
        it->LastSent = now;
        ++it;
    }
}

void TMultiplayerPeerForm::SendBoundaryStates()
{
    if(!Joined || !OwnerInterface) return;
    const TDateTime now = TDateTime::CurrentDateTime();
    for(std::vector<TConnectionDefinition>::const_iterator connection = Connections.begin(); connection != Connections.end(); ++connection)
    {
        if(connection->FromBox != BoxCode || connection->LocalPortal == "" || connection->RemotePortal == "") continue;
        int aspect = 0;
        if(!OwnerInterface->GetMultiplayerPortalSignalAspect(connection->LocalPortal, aspect)) continue;

        const AnsiString key = connection->LocalPortal + ">" + connection->ToBox;
        const bool changed = LastSentBoundaryAspects.find(key) == LastSentBoundaryAspects.end() || LastSentBoundaryAspects[key] != aspect;
        const bool heartbeatDue = LastBoundaryBroadcasts.find(key) == LastBoundaryBroadcasts.end() ||
                                  (double(now) - double(LastBoundaryBroadcasts[key])) >= BoundaryHeartbeatDays;
        if(!changed && !heartbeatDue) continue;

        const TPeerState *peer = FindActivePeerForBox(connection->ToBox);
        if(!peer) continue;
        const unsigned int sequence = ++LastSentBoundarySequences[key];
        AnsiString message = "BOUNDARY|2|" + PeerID + "|" + SessionID + "|" + BoxCode + "|" + connection->ToBox + "|" +
                             connection->LocalPortal + "|" + connection->RemotePortal + "|" + AnsiString(aspect) + "|" +
                             UIntToStr(sequence);
        SendTo(peer->Address, peer->Port, message);
        LastSentBoundaryAspects[key] = aspect;
        LastBoundaryBroadcasts[key] = now;
    }
}

void TMultiplayerPeerForm::SendWTTStates()
{
    if(!Joined || !WTTModeEnabled || WTTCatalog.Fingerprint() == "") return;
    const TDateTime now = TDateTime::CurrentDateTime();
    const std::vector<TMultiplayerWTTService>& services = WTTCatalog.AllServices();
    for(std::vector<TMultiplayerWTTService>::const_iterator service = services.begin(); service != services.end(); ++service)
    {
        if(service->Revision <= 0) continue;
        bool hasChangedCall = false;
        for(unsigned int callIndex = 0; callIndex < service->Calls.size(); ++callIndex)
        {
            const TMultiplayerWTTCall& call = service->Calls[callIndex];
            if(call.Revision <= 0) continue;
            hasChangedCall = true;
            const AnsiString message = "WTTSTATE|2|" + SessionID + "|" + WTTCatalog.BookSummary() + "|" +
                service->RunKey + "|" + AnsiString(service->Revision) + "|" + AnsiString(service->DelaySeconds) + "|" +
                (service->Cancelled ? "1" : "0") + "|" + AnsiString(callIndex) + "|" + AnsiString(call.Revision) + "|" +
                AnsiString(static_cast<int>(call.Status)) + "|" + call.OwnerBox + "|" + PeerID + "|" +
                call.WTTLocation + "|" + AnsiString(call.ScheduledSeconds) + "|" + service->OperationalOwner;
            for(std::map<AnsiString, TPeerState>::const_iterator peer = Peers.begin(); peer != Peers.end(); ++peer)
                if(peer->second.Active && peer->second.SessionID == SessionID)
                {
                    const AnsiString base = peer->second.PeerID + "|" + service->RunKey + "|" +
                                            call.WTTLocation + "|" + AnsiString(call.ScheduledSeconds);
                    if(AcknowledgedWTTRevisions[base + "|S"] >= service->Revision &&
                       AcknowledgedWTTRevisions[base + "|C"] >= call.Revision) continue;
                    if(double(LastWTTStateSent[base]) > 0.0 &&
                       (double(now) - double(LastWTTStateSent[base])) < WTTStateResendDays) continue;
                    SendTo(peer->second.Address, peer->second.Port, message);
                    LastWTTStateSent[base] = now;
                }
        }
        if(!hasChangedCall)
        {
            const AnsiString message = "WTTSTATE|2|" + SessionID + "|" + WTTCatalog.BookSummary() + "|" +
                service->RunKey + "|" + AnsiString(service->Revision) + "|" + AnsiString(service->DelaySeconds) + "|" +
                (service->Cancelled ? "1" : "0") + "|-1|0|0|" + BoxCode + "|" + PeerID + "|-|-1|" +
                service->OperationalOwner;
            for(std::map<AnsiString, TPeerState>::const_iterator peer = Peers.begin(); peer != Peers.end(); ++peer)
                if(peer->second.Active && peer->second.SessionID == SessionID)
                {
                    const AnsiString base = peer->second.PeerID + "|" + service->RunKey + "|-|-1";
                    if(AcknowledgedWTTRevisions[base + "|S"] >= service->Revision) continue;
                    if(double(LastWTTStateSent[base]) > 0.0 &&
                       (double(now) - double(LastWTTStateSent[base])) < WTTStateResendDays) continue;
                    SendTo(peer->second.Address, peer->second.Port, message);
                    LastWTTStateSent[base] = now;
                }
        }
    }
}

bool TMultiplayerPeerForm::BoundaryTargetAttribute(const AnsiString& LocalPortal, int &TargetAttribute) const
{
    const AnsiString portal = LocalPortal.UpperCase();
    if(!OwnerInterface || !OwnerInterface->MultiplayerPortalAllowsExit(portal)) return false;
    bool registeredBoundary = false;
    for(std::vector<TConnectionDefinition>::const_iterator connection = Connections.begin();
        connection != Connections.end(); ++connection)
        if(connection->FromBox == BoxCode && connection->LocalPortal == portal) registeredBoundary = true;
    if(!Joined || !registeredBoundary) return false;
    std::map<AnsiString, int>::const_iterator found = RemoteBoundaryAspects.find(portal);
    if(found == RemoteBoundaryAspects.end())
    {
        TargetAttribute = 1; // fail-safe red beyond the continuation until the neighbour reports state
        return true;
    }
    TargetAttribute = std::min(3, found->second + 1);
    return true;
}

bool TMultiplayerPeerForm::ShouldHoldPortalEntry(const AnsiString& LocalPortal) const
{
    if(!OwnerInterface || !OwnerInterface->MultiplayerPortalAllowsEntry(LocalPortal)) return false;
    AnsiString destinationBox, destinationPortal;
    if(!FindDestination(LocalPortal.UpperCase(), destinationBox, destinationPortal)) return false;
    // A WTT train assigned to another registered box must arrive by handover.  Do
    // not create a second copy merely because that signaller is temporarily offline.
    return WTTModeEnabled || FindActivePeerForBox(destinationBox) != NULL;
}

bool TMultiplayerPeerForm::GetFullWTTForService(const AnsiString& ServiceReference,
                                                AnsiString& TimetableText) const
{
    TimetableText = "";
    if(!WTTModeEnabled || ServiceReference.Trim() == "") return false;

    int serviceIndex = -1;
    for(std::multimap<int, AnsiString>::const_iterator reference = WTTServiceReferences.begin();
        reference != WTTServiceReferences.end(); ++reference)
    {
        if(reference->second.UpperCase() == ServiceReference.Trim().UpperCase())
        {
            serviceIndex = reference->first;
            break;
        }
    }
    const std::vector<TMultiplayerWTTService>& services = WTTCatalog.AllServices();
    if(serviceIndex < 0 || serviceIndex >= static_cast<int>(services.size())) return false;

    const TMultiplayerWTTService& service = services[serviceIndex];
    TimetableText = "Full WTT";
    if(service.TID.Trim() != "") TimetableText += " - " + service.TID.Trim();
    if(service.Origin.Trim() != "" || service.Destination.Trim() != "")
        TimetableText += "\n" + service.Origin.Trim() + " to " + service.Destination.Trim();

    for(std::vector<TMultiplayerWTTCall>::const_iterator call = service.Calls.begin();
        call != service.Calls.end(); ++call)
    {
        if(call->ScheduledSeconds < 0 || call->WTTLocation.Trim() == "") continue;

        const int arrival = call->ArrivalSeconds >= 0 ? call->ArrivalSeconds + service.DelaySeconds : -1;
        const int departure = call->DepartureSeconds >= 0 ? call->DepartureSeconds + service.DelaySeconds : -1;
        const int pass = call->PassSeconds >= 0 ? call->PassSeconds + service.DelaySeconds : -1;
        AnsiString time;
        AnsiString action;
        if(call->Status == TMultiplayerWTTCall::Skipped)
        {
            const int first = arrival >= 0 ? arrival : (departure >= 0 ? departure :
                              (pass >= 0 ? pass : call->ScheduledSeconds + service.DelaySeconds));
            const int last = departure >= 0 ? departure : first;
            time = WTTFullClockText(first);
            if(last != first) time += "-" + WTTFullClockText(last);
            action = " skip ";
        }
        else if(pass >= 0)
        {
            time = WTTFullClockText(pass);
            action = " pass ";
        }
        else if(arrival >= 0 && departure >= 0)
        {
            time = WTTFullClockText(arrival);
            if(departure != arrival) time += "-" + WTTFullClockText(departure);
            action = "  ";
        }
        else if(departure >= 0)
        {
            time = WTTFullClockText(departure);
            action = " dep  ";
        }
        else if(arrival >= 0)
        {
            time = WTTFullClockText(arrival);
            action = " arr  ";
        }
        else
        {
            time = WTTFullClockText(call->ScheduledSeconds + service.DelaySeconds);
            action = "  ";
        }

        TimetableText += "\n" + time + action + call->WTTLocation.Trim();
        if(call->Platform.Trim() != "" && call->Platform.Trim() != "..")
            TimetableText += " (P" + call->Platform.Trim() + ")";
    }
    return TimetableText.Pos("\n") > 0;
}

void TMultiplayerPeerForm::ResolveSessionMatch()
{
    if(!Joined || !AutoMatch) return;

    AnsiString lockedSession;
    AnsiString automaticSession = SessionID;
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        const TPeerState& peer = it->second;
        if(!peer.Active || peer.SessionID == "" || peer.BoxCode == BoxCode || !BoxesAreAdjacent(BoxCode, peer.BoxCode)) continue;

        bool ourBoxOccupied = false;
        for(std::map<AnsiString, TPeerState>::const_iterator occupied = Peers.begin(); occupied != Peers.end(); ++occupied)
        {
            if(occupied->second.Active && occupied->second.SessionID == peer.SessionID && occupied->second.BoxCode == BoxCode)
            {
                ourBoxOccupied = true;
                break;
            }
        }
        if(ourBoxOccupied) continue;

        if(!peer.AutoMatch)
        {
            if(lockedSession == "" || peer.SessionID < lockedSession) lockedSession = peer.SessionID;
        }
        else if(peer.SessionID < automaticSession)
            automaticSession = peer.SessionID;
    }

    const AnsiString desiredSession = lockedSession != "" ? lockedSession : automaticSession;
    if(desiredSession == "" || desiredSession == SessionID) return;

    const AnsiString oldSession = SessionID;
    const TDateTime preservedClock = SessionDateTime();
    SessionID = desiredSession;
    if(WTTModeEnabled)
    {
        ClockAuthorityPeerID = PeerID;
        SessionClockOffsetDays = double(preservedClock) - double(TDateTime::CurrentDateTime());
    }
    ClearRemoteBoundariesForBox("");
    LastSentBoundaryAspects.clear();
    LastBoundaryBroadcasts.clear();
    LastSentBoundarySequences.clear();
    AcknowledgedWTTRevisions.clear();
    LastWTTStateSent.clear();
    for(std::vector<TPendingTransfer>::iterator transfer = PendingTransfers.begin(); transfer != PendingTransfers.end(); ++transfer)
        transfer->SessionID = SessionID;
    AppendChat("System", "Matchmaking joined adjacent signalboxes from " + oldSession + " into session " + SessionID + ".");
    ReconcileSessionClock();
    SendDiscovery();
}

void TMultiplayerPeerForm::ResolveDuplicateOccupation()
{
    if(!Joined) return;
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        const TPeerState& peer = it->second;
        if(peer.Active && peer.SessionID == SessionID && peer.BoxCode == BoxCode && peer.PeerID != PeerID)
        {
            if(PeerID > peer.PeerID)
            {
                AnsiString oldSession = SessionID;
                SessionID = CreateSessionID();
                if(WTTModeEnabled)
                {
                    ClockAuthorityPeerID = PeerID;
                    SessionClockOffsetDays = 0.0;
                }
                ClearRemoteBoundariesForBox("");
                LastSentBoundaryAspects.clear();
                LastBoundaryBroadcasts.clear();
                LastSentBoundarySequences.clear();
                AcknowledgedWTTRevisions.clear();
                LastWTTStateSent.clear();
                AppendChat("System", "Another player claimed " + BoxCode + " in " + oldSession +
                           "; you were placed in parallel session " + SessionID + ".");
                for(std::vector<TPendingTransfer>::iterator transfer = PendingTransfers.begin(); transfer != PendingTransfers.end(); ++transfer)
                    transfer->SessionID = SessionID;
            }
            break;
        }
    }
}

void TMultiplayerPeerForm::PrunePeers()
{
    const TDateTime now = TDateTime::CurrentDateTime();
    for(std::map<AnsiString, TPeerState>::iterator it = Peers.begin(); it != Peers.end(); )
    {
        if((double(now) - double(it->second.LastSeen)) > PeerExpiryDays)
        {
            const AnsiString expiredPeer = it->first;
            const AnsiString expiredBox = it->second.BoxCode;
            if(it->second.SessionID == SessionID) ClearRemoteBoundariesForBox(expiredBox);
            for(std::vector<TPendingChatDelivery>::iterator chat = PendingChatDeliveries.begin();
                chat != PendingChatDeliveries.end(); )
            {
                if(chat->TargetPeerID == expiredPeer) chat = PendingChatDeliveries.erase(chat);
                else ++chat;
            }
            const AnsiString peerPrefix = expiredPeer + "|";
            for(std::map<AnsiString, int>::iterator revision = AcknowledgedWTTRevisions.begin();
                revision != AcknowledgedWTTRevisions.end(); )
            {
                if(revision->first.SubString(1, peerPrefix.Length()) == peerPrefix)
                    AcknowledgedWTTRevisions.erase(revision++);
                else ++revision;
            }
            for(std::map<AnsiString, TDateTime>::iterator sent = LastWTTStateSent.begin();
                sent != LastWTTStateSent.end(); )
            {
                if(sent->first.SubString(1, peerPrefix.Length()) == peerPrefix) LastWTTStateSent.erase(sent++);
                else ++sent;
            }
            Peers.erase(it++);
        }
        else ++it;
    }
    for(std::map<AnsiString, TDateTime>::iterator received = ReceivedTransferIDs.begin();
        received != ReceivedTransferIDs.end(); )
    {
        if((double(now) - double(received->second)) > ReceivedIDExpiryDays) ReceivedTransferIDs.erase(received++);
        else ++received;
    }
    for(std::map<AnsiString, TDateTime>::iterator received = ReceivedChatIDs.begin();
        received != ReceivedChatIDs.end(); )
    {
        if((double(now) - double(received->second)) > ReceivedIDExpiryDays) ReceivedChatIDs.erase(received++);
        else ++received;
    }
}

void TMultiplayerPeerForm::ClearRemoteBoundariesForBox(const AnsiString& RemoteBox)
{
    RemoteBoundarySequences.clear();
    std::set<AnsiString> cleared;
    for(std::vector<TConnectionDefinition>::const_iterator connection = Connections.begin();
        connection != Connections.end(); ++connection)
    {
        if(connection->FromBox != BoxCode || (RemoteBox != "" && connection->ToBox != RemoteBox)) continue;
        const AnsiString portal = connection->LocalPortal.UpperCase();
        if(cleared.count(portal)) continue;
        cleared.insert(portal);
        RemoteBoundaryAspects.erase(portal);
        if(OwnerInterface) OwnerInterface->RefreshMultiplayerBoundarySignals(portal);
    }
}

void TMultiplayerPeerForm::RefreshUsersGrid()
{
    int activeCount = Joined ? 1 : 0;
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
        if(it->second.Active) ++activeCount;
    UsersGrid->RowCount = std::max(2, activeCount + 1);
    for(int row = 1; row < UsersGrid->RowCount; ++row)
        for(int col = 0; col < UsersGrid->ColCount; ++col) UsersGrid->Cells[col][row] = "";
    int row = 1;
    if(Joined)
    {
        UsersGrid->Cells[0][row] = CleanField(UserNameEdit->Text) + " (you)";
        UsersGrid->Cells[1][row] = BoxCode;
        UsersGrid->Cells[2][row] = SessionID;
        UsersGrid->Cells[3][row] = AnsiString(LocalPort);
        ++row;
    }
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        if(!it->second.Active) continue;
        UsersGrid->Cells[0][row] = it->second.UserName;
        UsersGrid->Cells[1][row] = it->second.BoxCode;
        UsersGrid->Cells[2][row] = it->second.SessionID;
        UsersGrid->Cells[3][row] = AnsiString(it->second.Port);
        ++row;
    }
}

void TMultiplayerPeerForm::RefreshBoxesGrid()
{
    int activeCount = Joined ? 1 : 0;
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
        if(it->second.Active) ++activeCount;
    BoxesGrid->RowCount = std::max(2, activeCount + 1);
    for(int row = 1; row < BoxesGrid->RowCount; ++row)
        for(int col = 0; col < BoxesGrid->ColCount; ++col) BoxesGrid->Cells[col][row] = "";
    int row = 1;
    if(Joined)
    {
        BoxesGrid->Cells[0][row] = BoxCode;
        BoxesGrid->Cells[1][row] = SignalboxName(BoxCode);
        BoxesGrid->Cells[2][row] = SessionID;
        BoxesGrid->Cells[3][row] = CleanField(UserNameEdit->Text);
        ++row;
    }
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        if(!it->second.Active) continue;
        BoxesGrid->Cells[0][row] = it->second.BoxCode;
        BoxesGrid->Cells[1][row] = SignalboxName(it->second.BoxCode);
        BoxesGrid->Cells[2][row] = it->second.SessionID;
        BoxesGrid->Cells[3][row] = it->second.UserName;
        ++row;
    }
}

void TMultiplayerPeerForm::RefreshStatus()
{
    if(LocalPort == 0)
        StatusLabel->Caption = "Multiplayer unavailable: no local peer port could be opened.";
    else if(!Joined)
        StatusLabel->Caption = "Ready on local peer port " + AnsiString(LocalPort) + ". Choose a box and join or start a new session.";
    else
        StatusLabel->Caption = "Connected peer-to-peer  |  Session " + SessionID + "  |  Box " + BoxCode +
                               "  |  Pending trains " + AnsiString(static_cast<int>(PendingTransfers.size()));
    JoinButton->Enabled = !Joined && LocalPort != 0;
    NewSessionButton->Enabled = !Joined && LocalPort != 0;
    LeaveButton->Enabled = Joined;
    BoxCombo->Enabled = !Joined;
    UserNameEdit->Enabled = !Joined;
    ChatEdit->Enabled = Joined;
    SendChatButton->Enabled = Joined;
    WTTEnableButton->Enabled = Joined;
    WTTEnableButton->Caption = WTTModeEnabled ? "Disable Live WTT" : "Enable Live WTT";
}

void TMultiplayerPeerForm::AppendChat(const AnsiString& Prefix, const AnsiString& Text)
{
    LogMultiplayerEvent("CHAT", Prefix + ": " + Text);
    ChatMemo->SelStart = ChatMemo->Text.Length();
    ChatMemo->SelLength = 0;
    ChatMemo->SelAttributes->Color = clGray;
    ChatMemo->SelText = FormatDateTime("hh:nn:ss", TDateTime::CurrentDateTime()) + "  " + Prefix + ": " + Text + "\r\n";
    ChatMemo->SelStart = ChatMemo->Text.Length();
}

void TMultiplayerPeerForm::AppendAreaChat(const AnsiString& AreaName, const AnsiString& UserName,
                                           const AnsiString& Text, int Colour)
{
    LogMultiplayerEvent("CHAT", "[" + AreaName + "] " + UserName + ": " + Text +
                        ", colour=" + AnsiString(Colour));
    ChatMemo->SelStart = ChatMemo->Text.Length();
    ChatMemo->SelLength = 0;
    ChatMemo->SelAttributes->Color = static_cast<TColor>(Colour);
    ChatMemo->SelText = FormatDateTime("hh:nn:ss", TDateTime::CurrentDateTime()) + "  [" + AreaName + "] " +
                        UserName + ": " + Text + "\r\n";
    ChatMemo->SelStart = ChatMemo->Text.Length();
}

bool TMultiplayerPeerForm::LoadWTTCache(AnsiString &ErrorMessage)
{
    std::vector<std::pair<AnsiString, AnsiString> > assignedBooks;
    std::set<AnsiString> addedBooks;
    for(std::vector<TWTTBookDefinition>::const_iterator book = WTTBooks.begin(); book != WTTBooks.end(); ++book)
    {
        if(book->BoxCode != BoxCode || addedBooks.count(book->BookCode)) continue;
        addedBooks.insert(book->BookCode);
        assignedBooks.push_back(std::make_pair(book->BookCode,
            RootDirectory + "\\Multiplayer\\WTT\\" + book->CacheFile));
    }
    const bool loaded = WTTCatalog.LoadMany(assignedBooks, ErrorMessage);
    LogMultiplayerEvent(loaded ? "WTT-LOAD" : "WTT-ERROR", "Box=" + BoxCode + ", books=" +
                        AnsiString(static_cast<int>(assignedBooks.size())) + ", services=" +
                        AnsiString(WTTCatalog.ServiceCount()) + (ErrorMessage == "" ? "" : ", error=" + ErrorMessage));
    return loaded;
}

bool TMultiplayerPeerForm::WTTBooksCompatible(const AnsiString& OtherSummary, AnsiString &MismatchBook) const
{
    return WTTCatalog.CompatibleBooks(OtherSummary, MismatchBook);
}

AnsiString TMultiplayerPeerForm::WTTBoundaryNames(const AnsiString& WTTLocation) const
{
    const AnsiString location = WTTLocation.Trim().UpperCase();
    for(std::vector<TWTTBoundaryDefinition>::const_iterator boundary = WTTBoundaries.begin();
        boundary != WTTBoundaries.end(); ++boundary)
        if(boundary->BoxCode == BoxCode && boundary->WTTLocation == location) return boundary->BoundaryNames;
    // Exact-name fallback: maps can be made plug-and-play by naming a continuation
    // with the stable WTT timing-point code.  There is deliberately no fuzzy match.
    return location;
}

AnsiString TMultiplayerPeerForm::ConnectionPortalSelectors(const AnsiString& OtherBox) const
{
    AnsiString selectors;
    const AnsiString destination = OtherBox.Trim().UpperCase();
    for(std::vector<TConnectionDefinition>::const_iterator connection = Connections.begin();
        connection != Connections.end(); ++connection)
    {
        if(connection->FromBox != BoxCode || connection->ToBox != destination || connection->LocalPortal == "") continue;
        bool duplicate = false;
        const std::vector<AnsiString> existing = Split(selectors, ';');
        for(std::vector<AnsiString>::const_iterator selector = existing.begin(); selector != existing.end(); ++selector)
            if(selector->UpperCase() == connection->LocalPortal.UpperCase()) duplicate = true;
        if(duplicate) continue;
        if(selectors != "") selectors += ";";
        selectors += connection->LocalPortal.UpperCase();
    }
    return selectors;
}

TDateTime TMultiplayerPeerForm::SessionDateTime() const
{
    return TDateTime(double(TDateTime::CurrentDateTime()) + SessionClockOffsetDays);
}

__int64 TMultiplayerPeerForm::SessionClockMilliseconds() const
{
    return static_cast<__int64>(double(SessionDateTime()) * 86400000.0);
}

void TMultiplayerPeerForm::AdoptSessionClock(const TPeerState& State)
{
    if(!WTTModeEnabled || !State.WTTEnabled || State.SessionClockMilliseconds == 0) return;
    AnsiString mismatchBook;
    if(!WTTBooksCompatible(State.WTTFingerprint, mismatchBook))
    {
        WTTStatusLabel->Caption = "WTT book " + mismatchBook + " differs from " + State.BoxCode +
                                  "; train sharing is blocked.";
        return;
    }
    if(ClockAuthorityPeerID == PeerID || State.PeerID != ClockAuthorityPeerID) return;
    const double advertised = double(State.SessionClockMilliseconds) / 86400000.0;
    SessionClockOffsetDays = advertised - double(TDateTime::CurrentDateTime());
    if(OwnerInterface)
        OwnerInterface->SynchroniseMultiplayerWTTClock(SessionDateTime(), State.RailwayClockMilliseconds);
    LogMultiplayerEvent("CLOCK-SYNC", "Authority=" + ClockAuthorityPeerID + ", time=" +
                        FormatDateTime("yyyy-mm-dd hh:nn:ss", SessionDateTime()) + ", RailOS=" +
                        (State.RailwayClockMilliseconds >= 0 ?
                         WTTFullClockText(static_cast<int>(State.RailwayClockMilliseconds / 1000)) : "legacy"));
}

void TMultiplayerPeerForm::ReconcileSessionClock()
{
    if(!Joined || !WTTModeEnabled) return;

    // Keep an established authority stable.  In particular, a late joiner
    // must not take over merely because its random peer ID sorts earlier: it
    // would have no knowledge of whether RailOS is on 00:xx or 24:xx.
    if(ClockAuthorityPeerID == PeerID)
    {
        // Resolve only the genuine simultaneous-start case deterministically.
        for(std::map<AnsiString, TPeerState>::const_iterator peer = Peers.begin(); peer != Peers.end(); ++peer)
            if(peer->second.Active && peer->second.WTTEnabled && peer->second.SessionID == SessionID &&
               peer->second.ClockAuthorityPeerID == peer->second.PeerID && peer->second.PeerID < PeerID)
            {
                ClockAuthorityPeerID = peer->second.PeerID;
                AdoptSessionClock(peer->second);
                return;
            }
        return;
    }

    std::map<AnsiString, TPeerState>::const_iterator established = Peers.find(ClockAuthorityPeerID);
    if(established != Peers.end() && established->second.Active && established->second.WTTEnabled &&
       established->second.SessionID == SessionID)
    {
        AdoptSessionClock(established->second);
        return;
    }

    AnsiString authority;
    const TPeerState *authorityState = NULL;
    for(std::map<AnsiString, TPeerState>::const_iterator peer = Peers.begin(); peer != Peers.end(); ++peer)
    {
        if(!peer->second.Active || !peer->second.WTTEnabled || peer->second.SessionID != SessionID) continue;
        AnsiString mismatchBook;
        if(!WTTBooksCompatible(peer->second.WTTFingerprint, mismatchBook)) continue;
        AnsiString advertised = peer->second.ClockAuthorityPeerID;
        std::map<AnsiString, TPeerState>::const_iterator advertisedState = Peers.find(advertised);
        if(advertised == "" || advertisedState == Peers.end() || !advertisedState->second.Active ||
           !advertisedState->second.WTTEnabled || advertisedState->second.SessionID != SessionID)
        {
            advertised = peer->second.PeerID;
            advertisedState = peer;
        }
        if(authority == "" || advertised < authority)
        {
            authority = advertised;
            authorityState = &advertisedState->second;
        }
    }
    if(authority == "" || !authorityState)
    {
        ClockAuthorityPeerID = PeerID;
        return;
    }
    ClockAuthorityPeerID = authority;
    AdoptSessionClock(*authorityState);
}

int TMultiplayerPeerForm::ChooseChatColour() const
{
    static const int palette[] = {clBlue, clGreen, clMaroon, clPurple, clTeal, clNavy,
                                  0x00A05000, 0x000080C0, 0x00800080, 0x000060A0};
    unsigned int hash = 2166136261u;
    for(int index = 1; index <= PeerID.Length(); ++index)
    {
        hash ^= static_cast<unsigned char>(PeerID[index]);
        hash *= 16777619u;
    }
    return palette[hash % (sizeof(palette) / sizeof(palette[0]))];
}

bool TMultiplayerPeerForm::BuildLocalWTTEntries(std::vector<AnsiString>& Entries, AnsiString &ErrorMessage)
{
    Entries.clear();
    WTTServiceReferences.clear();
    WTTExpectedEntryPortals.clear();
    WTTExpectedExitPortals.clear();
    LastPassedWTTServiceCount = 0;
    unsigned short currentHour, currentMinute, currentSecond, currentMillisecond;
    DecodeTime(SessionDateTime(), currentHour, currentMinute, currentSecond, currentMillisecond);
    const int currentSeconds = (currentHour * 3600) + (currentMinute * 60) + currentSecond;
    std::set<AnsiString> usedReferences;
    std::map<AnsiString, std::vector<AnsiString> > startResolutionCache;
    std::set<AnsiString> unresolvedStarts;
    std::map<AnsiString, AnsiString> exitResolutionCache;
    std::set<AnsiString> unresolvedExits;
    std::vector<TGeneratedWTTVisit> generatedVisits;
    const std::vector<TMultiplayerWTTService>& services = WTTCatalog.AllServices();
    for(std::vector<TMultiplayerWTTService>::const_iterator service = services.begin(); service != services.end(); ++service)
    {
        if(!WTTCatalog.ServiceRunsOnDate(*service, SessionDateTime()) || service->Cancelled) continue;
        unsigned int searchFrom = 0;
        while(searchFrom < service->Calls.size())
        {
            int firstLocal = -1, lastLocal = -1;
            unsigned int nextSearch = service->Calls.size();
            for(unsigned int callIndex = searchFrom; callIndex < service->Calls.size(); ++callIndex)
            {
                const AnsiString owner = service->Calls[callIndex].OwnerBox;
                if(owner == BoxCode)
                {
                    if(firstLocal < 0) firstLocal = callIndex;
                    lastLocal = callIndex;
                }
                else if(firstLocal >= 0 && owner != "")
                {
                    nextSearch = callIndex;
                    break;
                }
            }
            if(firstLocal < 0 || lastLocal < firstLocal) break;
            searchFrom = nextSearch;

        int startSeconds = service->Calls[firstLocal].ScheduledSeconds + service->DelaySeconds;
        int previousScheduled = -1;
        AnsiString previousWTTLocation;
        for(int callIndex = firstLocal - 1; callIndex >= 0; --callIndex)
        {
            if(service->Calls[callIndex].ScheduledSeconds >= 0)
            {
                previousScheduled = service->Calls[callIndex].ScheduledSeconds + service->DelaySeconds;
                previousWTTLocation = service->Calls[callIndex].WTTLocation;
                break;
            }
        }
        AnsiString previousOwner;
        for(int callIndex = firstLocal - 1; callIndex >= 0; --callIndex)
        {
            if(service->Calls[callIndex].OwnerBox != "" && service->Calls[callIndex].OwnerBox != BoxCode)
            {
                previousOwner = service->Calls[callIndex].OwnerBox;
                break;
            }
        }
        AnsiString entryBoundaryNames = ConnectionPortalSelectors(previousOwner);
        const bool automaticEntryBoundary = entryBoundaryNames != "";
        if(entryBoundaryNames == "") entryBoundaryNames = WTTBoundaryNames(previousWTTLocation);
        const bool entersFromAnotherBox = previousScheduled >= 0 && entryBoundaryNames != "";
        if(entersFromAnotherBox && previousScheduled >= 0)
        {
            const int firstEntrySeconds = (service->Calls[firstLocal].ArrivalSeconds >= 0 ?
                                           service->Calls[firstLocal].ArrivalSeconds :
                                           service->Calls[firstLocal].ScheduledSeconds) + service->DelaySeconds;
            const int gap = std::max(60, firstEntrySeconds - previousScheduled);
            startSeconds = previousScheduled + std::min(120, gap / 2);
            if(startSeconds >= firstEntrySeconds)
                startSeconds = std::max(previousScheduled, firstEntrySeconds - 30);
        }
        else
            startSeconds = std::max(0, startSeconds - 60);

        AnsiString firstLocation = service->Calls[firstLocal].RailwayLocation;
        AnsiString nextLocalLocation;
        for(int callIndex = firstLocal + 1; callIndex <= lastLocal; ++callIndex)
        {
            if(service->Calls[callIndex].OwnerBox == BoxCode && service->Calls[callIndex].RailwayLocation != "" &&
               service->Calls[callIndex].RailwayLocation != firstLocation)
            {
                nextLocalLocation = service->Calls[callIndex].RailwayLocation;
                break;
            }
        }
        const AnsiString wttPlatform = service->Calls[firstLocal].Platform.Trim().UpperCase();
        const AnsiString startResolutionKey = firstLocation.UpperCase() + "|" + nextLocalLocation.UpperCase() + "|" +
                                              wttPlatform + "|" +
                                              (entersFromAnotherBox ? entryBoundaryNames : "INTERNAL");
        int comparableCurrentSeconds = currentSeconds;
        if(startSeconds >= (24 * 3600) && comparableCurrentSeconds < (6 * 3600))
            comparableCurrentSeconds += 24 * 3600;
        const bool handoverOnlyPastEntry = automaticEntryBoundary && entersFromAnotherBox &&
                                           startSeconds < comparableCurrentSeconds;
        if(startSeconds < comparableCurrentSeconds && !handoverOnlyPastEntry)
        {
            ++LastPassedWTTServiceCount;
            LogMultiplayerEvent("WTT-PASSED", service->TID + " omitted because its scheduled map entry " +
                                WTTClockText(startSeconds) + " is before live time " +
                                WTTClockText(comparableCurrentSeconds));
            continue;
        }
        if(handoverOnlyPastEntry)
            LogMultiplayerEvent("WTT-HANDOVER-WAIT", service->TID + " retained as a dormant downstream visit; "
                                "its scheduled entry " + WTTClockText(startSeconds) +
                                " has passed but it may still arrive late from " + previousOwner);

        std::vector<AnsiString> startIDCandidates;
        std::map<AnsiString, std::vector<AnsiString> >::const_iterator cachedStart = startResolutionCache.find(startResolutionKey);
        if(cachedStart != startResolutionCache.end()) startIDCandidates = cachedStart->second;
        else if(!unresolvedStarts.count(startResolutionKey) && OwnerInterface)
        {
            OwnerInterface->ResolveMultiplayerWTTStartCandidates(firstLocation, nextLocalLocation, wttPlatform,
                                                                  entersFromAnotherBox, entryBoundaryNames,
                                                                  startIDCandidates);
            if(!startIDCandidates.empty()) startResolutionCache[startResolutionKey] = startIDCandidates;
            else unresolvedStarts.insert(startResolutionKey);
        }
        if(startIDCandidates.empty()) continue;

        AnsiString reference = StableWTTReference(*service, BoxCode, firstLocal);
        if(reference == "" || usedReferences.count(reference)) continue;
        usedReferences.insert(reference);
        WTTServiceReferences.insert(std::make_pair(static_cast<int>(service - services.begin()), reference));
        if(entersFromAnotherBox)
        {
            AnsiString allowedEntries;
            for(std::vector<AnsiString>::const_iterator candidate = startIDCandidates.begin();
                candidate != startIDCandidates.end(); ++candidate)
            {
                const int separator = candidate->Pos(" ");
                const AnsiString portal = (separator > 1 ? candidate->SubString(1, separator - 1) : *candidate).UpperCase();
                if(allowedEntries != "") allowedEntries += ";";
                allowedEntries += portal;
            }
            WTTExpectedEntryPortals[reference] = allowedEntries;
        }
        LogMultiplayerEvent("WTT-SERVICE", "Reference=" + reference + ", TID=" + service->TID +
                            ", entry selectors=" + (entersFromAnotherBox ? entryBoundaryNames : "INTERNAL") +
                            ", platform=" + (wttPlatform == "" ? "unspecified" : wttPlatform) +
                            ", selector source=" + (automaticEntryBoundary ? "connection to " + previousOwner : "boundary fallback") +
                            ", start candidates=" + AnsiString(static_cast<int>(startIDCandidates.size())));

        const AnsiString entryPrefix = reference + ";" + TimetableSafeText(service->Origin + " to " + service->Destination) +
                                       ";0;120;180;101;1200," + WTTClockText(startSeconds) + ";Snt;";
        AnsiString entryRemainder;
        int nextScheduled = -1;
        AnsiString nextWTTLocation;
        for(unsigned int callIndex = lastLocal + 1; callIndex < service->Calls.size(); ++callIndex)
        {
            if(service->Calls[callIndex].ScheduledSeconds >= 0)
            {
                nextScheduled = service->Calls[callIndex].ScheduledSeconds + service->DelaySeconds;
                nextWTTLocation = service->Calls[callIndex].WTTLocation;
                break;
            }
        }
        AnsiString nextOwner;
        for(unsigned int callIndex = lastLocal + 1; callIndex < service->Calls.size(); ++callIndex)
        {
            if(service->Calls[callIndex].OwnerBox != "" && service->Calls[callIndex].OwnerBox != BoxCode)
            {
                nextOwner = service->Calls[callIndex].OwnerBox;
                break;
            }
        }
        AnsiString exitBoundaryNames = ConnectionPortalSelectors(nextOwner);
        const bool automaticExitBoundary = exitBoundaryNames != "";
        if(exitBoundaryNames == "") exitBoundaryNames = WTTBoundaryNames(nextWTTLocation);
        AnsiString exitID;
        if(nextScheduled >= 0 && exitBoundaryNames != "" && OwnerInterface)
        {
            const AnsiString exitResolutionKey = service->Calls[lastLocal].RailwayLocation.UpperCase() + "|" +
                                                  exitBoundaryNames;
            std::map<AnsiString, AnsiString>::const_iterator cachedExit = exitResolutionCache.find(exitResolutionKey);
            if(cachedExit != exitResolutionCache.end()) exitID = cachedExit->second;
            else if(!unresolvedExits.count(exitResolutionKey) &&
                    OwnerInterface->ResolveMultiplayerWTTExit(service->Calls[lastLocal].RailwayLocation,
                                                              exitBoundaryNames, exitID))
                exitResolutionCache[exitResolutionKey] = exitID;
            else
                unresolvedExits.insert(exitResolutionKey);
        }
        LogMultiplayerEvent("WTT-EXIT", "Reference=" + reference + ", selectors=" +
                            (exitBoundaryNames == "" ? "FREE-HERE" : exitBoundaryNames) + ", selector source=" +
                            (automaticExitBoundary ? "connection to " + nextOwner : "boundary fallback") +
                            ", resolved portal=" + (exitID == "" ? "none" : exitID));
        const bool exitsMap = exitID != "" && nextScheduled >= 0;

        // RailOS's one-time location syntax alternates between arrival and
        // departure according to whether the train is moving.  WTT books often
        // give only a departure time for an intermediate stop; emitting that as
        // one TimeLoc leaves the train stationary and makes the following stop
        // invalid.  Use an explicit arrival/departure pair (equal times when the
        // WTT supplies only one) for every en-route call.
        for(int callIndex = firstLocal; callIndex <= lastLocal; ++callIndex)
        {
            const TMultiplayerWTTCall& call = service->Calls[callIndex];
            if(call.OwnerBox != BoxCode || call.RailwayLocation == "" || call.ScheduledSeconds < 0) continue;
            const AnsiString location = TimetableSafeText(call.RailwayLocation);
            const AnsiString time = WTTClockText(call.ScheduledSeconds + service->DelaySeconds);
            const bool internalStartDeparture = callIndex == firstLocal && !entersFromAnotherBox;
            const bool finalArrival = callIndex == lastLocal && !exitsMap;
            if(internalStartDeparture)
            {
                if(!finalArrival) entryRemainder += "," + time + ";" + location;
            }
            else if(call.Status == TMultiplayerWTTCall::Skipped || call.PassRaw != "")
            {
                const int passSeconds = call.PassSeconds >= 0 ? call.PassSeconds : call.ScheduledSeconds;
                entryRemainder += "," + WTTClockText(passSeconds + service->DelaySeconds) + ";pas;" + location;
            }
            else if(finalArrival)
            {
                const int arrivalSeconds = call.ArrivalSeconds >= 0 ? call.ArrivalSeconds : call.ScheduledSeconds;
                entryRemainder += "," + WTTClockText(arrivalSeconds + service->DelaySeconds) + ";" + location;
            }
            else
            {
                int arrivalSeconds = call.ArrivalSeconds >= 0 ? call.ArrivalSeconds :
                                     (call.DepartureSeconds >= 0 ? call.DepartureSeconds : call.ScheduledSeconds);
                int departureSeconds = call.DepartureSeconds >= 0 ? call.DepartureSeconds : arrivalSeconds;
                if(departureSeconds < arrivalSeconds) departureSeconds = arrivalSeconds;
                entryRemainder += "," + WTTClockText(arrivalSeconds + service->DelaySeconds) + ";" +
                                  WTTClockText(departureSeconds + service->DelaySeconds) + ";" + location;
            }
        }

        if(exitsMap)
        {
            int lastSeconds = service->Calls[lastLocal].ScheduledSeconds + service->DelaySeconds;
            int exitSeconds = std::min(nextScheduled - 60, lastSeconds + 120);
            if(exitSeconds <= lastSeconds) exitSeconds = lastSeconds + 60;
            entryRemainder += "," + WTTClockText(exitSeconds) + ";Fer;" + exitID;
            WTTExpectedExitPortals[reference] = exitID.UpperCase();
        }
        else
            entryRemainder += ",Frh";
        TGeneratedWTTVisit generatedVisit;
        generatedVisit.ServiceIndex = static_cast<int>(service - services.begin());
        generatedVisit.FirstLocal = firstLocal;
        generatedVisit.LastLocal = lastLocal;
        generatedVisit.Reference = reference;
        generatedVisit.ContinuationHeader = reference + ";" +
                                            TimetableSafeText(service->Origin + " to " + service->Destination);
        generatedVisit.EntryRemainder = entryRemainder;
        generatedVisit.FirstLocation = service->Calls[firstLocal].RailwayLocation;
        generatedVisit.LastLocation = service->Calls[lastLocal].RailwayLocation;
        generatedVisit.FirstPlatform = NormalisedWTTValue(service->Calls[firstLocal].Platform);
        generatedVisit.LastPlatform = NormalisedWTTValue(service->Calls[lastLocal].Platform);
        generatedVisit.ArrivalSeconds = (service->Calls[lastLocal].ArrivalSeconds >= 0 ?
                                         service->Calls[lastLocal].ArrivalSeconds :
                                         service->Calls[lastLocal].ScheduledSeconds) + service->DelaySeconds;
        generatedVisit.DepartureSeconds = (service->Calls[firstLocal].DepartureSeconds >= 0 ?
                                           service->Calls[firstLocal].DepartureSeconds :
                                           service->Calls[firstLocal].ScheduledSeconds) + service->DelaySeconds;
        generatedVisit.OriginatesHere = true;
        generatedVisit.TerminatesHere = true;
        for(int callIndex = firstLocal - 1; callIndex >= 0; --callIndex)
        {
            if(service->Calls[callIndex].ScheduledSeconds < 0) continue;
            generatedVisit.OriginatesHere = false;
            generatedVisit.PreviousLocation = service->Calls[callIndex].WTTLocation;
            break;
        }
        for(unsigned int callIndex = lastLocal + 1; callIndex < service->Calls.size(); ++callIndex)
        {
            if(service->Calls[callIndex].ScheduledSeconds < 0) continue;
            generatedVisit.TerminatesHere = false;
            generatedVisit.NextLocation = service->Calls[callIndex].WTTLocation;
            break;
        }
        if(generatedVisit.OriginatesHere)
        {
            for(unsigned int callIndex = firstLocal + 1; callIndex < service->Calls.size(); ++callIndex)
            {
                if(service->Calls[callIndex].ScheduledSeconds < 0) continue;
                generatedVisit.NextLocation = service->Calls[callIndex].WTTLocation;
                break;
            }
        }
        if(generatedVisit.TerminatesHere)
        {
            for(int callIndex = lastLocal - 1; callIndex >= 0; --callIndex)
            {
                if(service->Calls[callIndex].ScheduledSeconds < 0) continue;
                generatedVisit.PreviousLocation = service->Calls[callIndex].WTTLocation;
                break;
            }
        }
        for(std::vector<AnsiString>::const_iterator startIDs = startIDCandidates.begin();
            startIDs != startIDCandidates.end(); ++startIDs)
        {
            generatedVisit.EntryIndices.push_back(static_cast<int>(Entries.size()));
            Entries.push_back(entryPrefix + *startIDs + entryRemainder);
        }
        generatedVisits.push_back(generatedVisit);
        }
    }

    // Public WTTs do not include rolling-stock diagrams.  Recover the common
    // case by pairing a terminating train with the nearest later departure from
    // the same location and the same explicitly named platform.  Matches are
    // one-to-one and deliberately time-limited; unmatched services retain their
    // independent Snt/Frh behaviour.
    const int maximumTurnaroundSeconds = 90 * 60;
    std::vector<TWTTFormationCandidate> formationCandidates;
    for(unsigned int parent = 0; parent < generatedVisits.size(); ++parent)
    {
        const TGeneratedWTTVisit& parentVisit = generatedVisits[parent];
        if(!parentVisit.TerminatesHere || parentVisit.ArrivalSeconds < 0 ||
           parentVisit.LastPlatform == "" || parentVisit.EntryIndices.empty()) continue;
        for(unsigned int child = 0; child < generatedVisits.size(); ++child)
        {
            const TGeneratedWTTVisit& childVisit = generatedVisits[child];
            if(parent == child || parentVisit.ServiceIndex == childVisit.ServiceIndex ||
               !childVisit.OriginatesHere || childVisit.DepartureSeconds < 0 ||
               childVisit.EntryIndices.empty()) continue;
            if(parentVisit.LastLocation.Trim().UpperCase() != childVisit.FirstLocation.Trim().UpperCase() ||
               parentVisit.LastPlatform != childVisit.FirstPlatform) continue;
            const int turnaround = childVisit.DepartureSeconds - parentVisit.ArrivalSeconds;
            if(turnaround < 0 || turnaround > maximumTurnaroundSeconds) continue;
            if(turnaround == 0 && parentVisit.Reference >= childVisit.Reference) continue;
            TWTTFormationCandidate formation;
            formation.ParentVisit = parent;
            formation.ChildVisit = child;
            formation.TurnaroundSeconds = turnaround;
            formationCandidates.push_back(formation);
        }
    }
    std::sort(formationCandidates.begin(), formationCandidates.end(), WTTFormationCandidateEarlier);

    const std::vector<AnsiString> independentEntries = Entries;
    std::set<int> parentsUsed;
    std::set<int> childrenUsed;
    for(std::vector<TWTTFormationCandidate>::const_iterator candidate = formationCandidates.begin();
        candidate != formationCandidates.end(); ++candidate)
    {
        if(parentsUsed.count(candidate->ParentVisit) || childrenUsed.count(candidate->ChildVisit)) continue;
        TGeneratedWTTVisit& parent = generatedVisits[candidate->ParentVisit];
        TGeneratedWTTVisit& child = generatedVisits[candidate->ChildVisit];
        const int formationSeconds = std::max(parent.ArrivalSeconds, child.DepartureSeconds - 60);
        const bool changesDirection = NormalisedWTTValue(parent.PreviousLocation) != "" &&
                                      NormalisedWTTValue(parent.PreviousLocation) ==
                                      NormalisedWTTValue(child.NextLocation);
        for(std::vector<int>::const_iterator entryIndex = parent.EntryIndices.begin();
            entryIndex != parent.EntryIndices.end(); ++entryIndex)
        {
            AnsiString& entry = Entries[*entryIndex];
            if(entry.Length() >= 4 && entry.SubString(entry.Length() - 3, 4) == ",Frh")
            {
                entry = entry.SubString(1, entry.Length() - 4);
                if(changesDirection) entry += "," + WTTClockText(formationSeconds) + ";cdt";
                entry += "," + WTTClockText(formationSeconds) + ";Fns;" + child.Reference;
            }
        }
        for(std::vector<int>::const_iterator entryIndex = child.EntryIndices.begin();
            entryIndex != child.EntryIndices.end(); ++entryIndex)
            Entries[*entryIndex] = child.ContinuationHeader + "," + WTTClockText(formationSeconds) +
                                   ";Sns;" + parent.Reference + child.EntryRemainder;

        parentsUsed.insert(candidate->ParentVisit);
        childrenUsed.insert(candidate->ChildVisit);
        LogMultiplayerEvent("WTT-FORMATION", parent.Reference + " forms " + child.Reference + " at " +
                            parent.LastLocation + " platform " + parent.LastPlatform + " after " +
                            AnsiString(candidate->TurnaroundSeconds / 60) + " minutes" +
                            (changesDirection ? "; direction change included" : ""));
    }

    // Keep independent Snt/Frh candidates for every inferred formation.  The
    // installer prefers the linked pair, but can fall back to these entries if
    // RailOS rejects an ambiguous physical layout or direction inference.
    std::set<int> fallbackVisits = parentsUsed;
    fallbackVisits.insert(childrenUsed.begin(), childrenUsed.end());
    for(std::set<int>::const_iterator visitIndex = fallbackVisits.begin();
        visitIndex != fallbackVisits.end(); ++visitIndex)
    {
        const TGeneratedWTTVisit& visit = generatedVisits[*visitIndex];
        for(std::vector<int>::const_iterator entryIndex = visit.EntryIndices.begin();
            entryIndex != visit.EntryIndices.end(); ++entryIndex)
            Entries.push_back(independentEntries[*entryIndex]);
    }

    // Alternative physical starts become identical after Snt is converted to
    // Sns, so discard exact duplicates before passing the entries to RailOS.
    std::set<AnsiString> uniqueEntries;
    std::vector<AnsiString> deduplicatedEntries;
    for(std::vector<AnsiString>::const_iterator entry = Entries.begin(); entry != Entries.end(); ++entry)
        if(uniqueEntries.insert(*entry).second) deduplicatedEntries.push_back(*entry);
    Entries.swap(deduplicatedEntries);
    if(Entries.empty())
    {
        ErrorMessage = "No future Live WTT services assigned to " + BoxCode +
                       " could be placed on this map; earlier services were treated as passed.";
        return false;
    }
    ErrorMessage = "";
    return true;
}

void TMultiplayerPeerForm::EnableWTTMode()
{
    if(!Joined || WTTModeEnabled) return;
    AnsiString error;
    if(!LoadWTTCache(error))
    {
        WTTStatusLabel->Caption = error;
        AppendTransfer(error);
        return;
    }

    const TPeerState *clockPeer = NULL;
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        const TPeerState& peer = it->second;
        if(!peer.Active || peer.SessionID != SessionID || !peer.WTTEnabled) continue;
        AnsiString mismatchBook;
        if(!WTTBooksCompatible(peer.WTTFingerprint, mismatchBook))
        {
            WTTStatusLabel->Caption = "Cannot enable: WTT book " + mismatchBook + " differs from " + peer.BoxCode + ".";
            return;
        }
        if(!clockPeer || peer.PeerID < clockPeer->PeerID) clockPeer = &peer;
    }
    if(clockPeer)
    {
        const AnsiString advertisedAuthority = clockPeer->ClockAuthorityPeerID;
        std::map<AnsiString, TPeerState>::const_iterator authority = Peers.find(advertisedAuthority);
        if(advertisedAuthority != "" && authority != Peers.end() && authority->second.Active &&
           authority->second.WTTEnabled && authority->second.SessionID == SessionID)
            clockPeer = &authority->second;
        ClockAuthorityPeerID = clockPeer->PeerID;
    }
    if(clockPeer)
    {
        const double advertised = double(clockPeer->SessionClockMilliseconds) / 86400000.0;
        SessionClockOffsetDays = advertised - double(TDateTime::CurrentDateTime());
    }
    else
    {
        ClockAuthorityPeerID = PeerID;
        SessionClockOffsetDays = 0.0;
    }

    const __int64 initialRailwayClock = clockPeer ? clockPeer->RailwayClockMilliseconds : -1;
    if(!OwnerInterface || !OwnerInterface->BeginMultiplayerWTTClock(SessionDateTime(), initialRailwayClock, error))
    {
        WTTStatusLabel->Caption = error;
        ClockAuthorityPeerID = "";
        SessionClockOffsetDays = 0.0;
        LogMultiplayerEvent("WTT-ERROR", "Clock start failed: " + error);
        return;
    }
    std::vector<AnsiString> localEntries;
    if(!BuildLocalWTTEntries(localEntries, error))
    {
        OwnerInterface->EndMultiplayerWTTClock();
        WTTStatusLabel->Caption = error;
        ClockAuthorityPeerID = "";
        SessionClockOffsetDays = 0.0;
        LogMultiplayerEvent("WTT-ERROR", "Entry generation failed: " + error);
        return;
    }
    if(!OwnerInterface->InstallMultiplayerWTTTimetable(localEntries, error))
    {
        OwnerInterface->EndMultiplayerWTTClock();
        WTTStatusLabel->Caption = error;
        ClockAuthorityPeerID = "";
        SessionClockOffsetDays = 0.0;
        LogMultiplayerEvent("WTT-ERROR", "Timetable installation failed: " + error);
        return;
    }
    WTTModeEnabled = true;
    ReconcileSessionClock();
    LogMultiplayerEvent("WTT-ENABLE", error + " Generated candidate entries=" +
                        AnsiString(static_cast<int>(localEntries.size())) + ", passed=" +
                        AnsiString(LastPassedWTTServiceCount));
    AppendTransfer(error);
    if(LastPassedWTTServiceCount > 0)
        AppendTransfer(AnsiString(LastPassedWTTServiceCount) +
                       " earlier services were already behind this box at session start and were treated as passed.");
    AppendChat("System", "Live WTT enabled at session time " +
               FormatDateTime("yyyy-mm-dd hh:nn:ss", SessionDateTime()) + ".");
    SendDiscovery();
    RefreshWTTServices();
    RefreshStatus();
}

void TMultiplayerPeerForm::DisableWTTMode()
{
    if(!WTTModeEnabled) return;
    if(OwnerInterface && !OwnerInterface->ClearMultiplayerWTTTimetable())
    {
        WTTStatusLabel->Caption = "Live WTT cannot be disabled while trains are still running.";
        return;
    }
    WTTModeEnabled = false;
    if(OwnerInterface) OwnerInterface->EndMultiplayerWTTClock();
    ClockAuthorityPeerID = "";
    SessionClockOffsetDays = 0.0;
    WTTStatusLabel->Caption = "Multiplayer-only scheduled services are off.";
    WTTServicesList->Items->Text = "Join a session, then enable Live WTT.";
    WTTStopCombo->Items->Clear();
    LogMultiplayerEvent("WTT-DISABLE", "Live WTT disabled");
    AcknowledgedWTTRevisions.clear();
    LastWTTStateSent.clear();
    SendDiscovery();
    RefreshStatus();
}

void TMultiplayerPeerForm::RailwayOperationEnded()
{
    if(WTTModeEnabled) DisableWTTMode();
}

void TMultiplayerPeerForm::RefreshWTTServices()
{
    if(!WTTModeEnabled)
    {
        WTTEnableButton->Caption = "Enable Live WTT";
        return;
    }
    std::vector<int> services = WTTCatalog.ServicesOwnedAt(BoxCode, SessionDateTime());
    WTTStatusLabel->Caption = FormatDateTime("ddd dd mmm hh:nn:ss", SessionDateTime()) + "  |  " +
                              AnsiString(static_cast<int>(services.size())) + " service(s) in " + BoxCode +
                              "  |  " + WTTCatalog.BookList();
    const int previousSelection = WTTServicesList->ItemIndex;
    WTTServicesList->Items->BeginUpdate();
    WTTServicesList->Items->Clear();
    DisplayedWTTServiceIndices.clear();
    const std::vector<TMultiplayerWTTService>& all = WTTCatalog.AllServices();
    const unsigned int limit = std::min<unsigned int>(services.size(), 12);
    for(unsigned int index = 0; index < limit; ++index)
    {
        const TMultiplayerWTTService& service = all.at(services[index]);
        WTTServicesList->Items->Add(service.TID + "  " + service.Origin + " - " + service.Destination +
                                    (service.DelaySeconds ? "  +" + AnsiString(service.DelaySeconds / 60) : ""));
        DisplayedWTTServiceIndices.push_back(services[index]);
    }
    if(services.empty()) WTTServicesList->Items->Add("No scheduled services are currently inside this box.");
    if(services.size() > limit) WTTServicesList->Items->Add("...and " + AnsiString(static_cast<int>(services.size() - limit)) + " more");
    WTTServicesList->Items->EndUpdate();
    if(previousSelection >= 0 && previousSelection < static_cast<int>(DisplayedWTTServiceIndices.size()))
    {
        WTTServicesList->ItemIndex = previousSelection;
        WTTServiceListClick(WTTServicesList);
    }
}

void TMultiplayerPeerForm::AppendTransfer(const AnsiString& Text)
{
    LogMultiplayerEvent("TRANSFER", Text);
    TransferMemo->Lines->Add(FormatDateTime("hh:nn:ss", TDateTime::CurrentDateTime()) + "  " + Text);
    TransferMemo->SelStart = TransferMemo->Text.Length();
}

void TMultiplayerPeerForm::LogMultiplayerEvent(const AnsiString& Category, const AnsiString& Text) const
{
    if(MultiplayerLogFile.Trim() == "") return;
    const AnsiString timestamp = FormatDateTime("yyyy-mm-dd hh:nn:ss.zzz", TDateTime::CurrentDateTime());
    MultiplayerLogBuffer.push_back(timestamp + " [" + Category + "] " + Text);
    if(MultiplayerLogBuffer.size() >= 256) FlushMultiplayerLog();
}

void TMultiplayerPeerForm::FlushMultiplayerLog() const
{
    if(MultiplayerLogFile.Trim() == "" || MultiplayerLogBuffer.empty()) return;
    try
    {
        if(FileExists(MultiplayerLogFile) && TFile::GetSize(MultiplayerLogFile) >= MaximumMultiplayerLogBytes)
        {
            const AnsiString previousLog = MultiplayerLogFile + ".previous";
            if(FileExists(previousLog)) DeleteFile(previousLog);
            RenameFile(MultiplayerLogFile, previousLog);
        }
        std::ofstream output(MultiplayerLogFile.c_str(), std::ios::out | std::ios::app);
        if(!output) return;
        for(std::vector<AnsiString>::const_iterator line = MultiplayerLogBuffer.begin();
            line != MultiplayerLogBuffer.end(); ++line)
            output << line->c_str() << '\n';
        output.flush();
        MultiplayerLogBuffer.clear();
    }
    catch(...)
    {
    }
}

void TMultiplayerPeerForm::StartSession(bool ForceNewSession)
{
    if(Joined || BoxCombo->ItemIndex < 0) return;
    AnsiString selected = BoxCombo->Items->Strings[BoxCombo->ItemIndex];
    int separator = selected.Pos(" ");
    BoxCode = (separator > 1 ? selected.SubString(1, separator - 1) : selected).Trim().UpperCase();
    if(RailwayBoxCode == "" || RailwayBoxCode != BoxCode)
    {
        AppendTransfer(RailwayBoxCode == "" ?
            "This railway is not registered to a multiplayer signalbox. Check signalboxes.csv and reload the map." :
            "The loaded railway belongs to " + RailwayBoxCode + ", so it cannot be advertised as " + BoxCode + ".");
        BoxCode = "";
        return;
    }
    SessionID = ForceNewSession ? CreateSessionID() : ChooseBestSession();
    if(SessionID == "") SessionID = CreateSessionID();
    RemoteBoundaryAspects.clear();
    LastSentBoundaryAspects.clear();
    LastBoundaryBroadcasts.clear();
    LastSentBoundarySequences.clear();
    RemoteBoundarySequences.clear();
    AcknowledgedWTTRevisions.clear();
    LastWTTStateSent.clear();
    AutoMatch = !ForceNewSession;
    Joined = true;
    ClearRemoteBoundariesForBox("");
    ClockAuthorityPeerID = "";
    SessionClockOffsetDays = 0.0;
    LogMultiplayerEvent("SESSION", "Joined box=" + BoxCode + ", session=" + SessionID +
                        ", mode=" + (ForceNewSession ? "new" : "best"));
    AppendChat("System", "Operating " + BoxCode + " in session " + SessionID + ".");
    SendDiscovery();
    RefreshStatus();
}

void TMultiplayerPeerForm::LeaveSession()
{
    if(!Joined) return;
    if(!PendingTransfers.empty())
    {
        AppendTransfer("Cannot leave while " + AnsiString(static_cast<int>(PendingTransfers.size())) +
                       " train handover(s) are awaiting delivery.");
        return;
    }
    DisableWTTMode();
    if(WTTModeEnabled)
    {
        AppendTransfer("Remove or finish the running Live WTT trains before leaving the session.");
        return;
    }
    Joined = false;
    AutoMatch = false;
    PendingTransfers.clear();
    PendingChatDeliveries.clear();
    ClearRemoteBoundariesForBox("");
    LastSentBoundaryAspects.clear();
    LastBoundaryBroadcasts.clear();
    LastSentBoundarySequences.clear();
    RemoteBoundarySequences.clear();
    AcknowledgedWTTRevisions.clear();
    LastWTTStateSent.clear();
    SessionID = "";
    BoxCode = "";
    LogMultiplayerEvent("SESSION", "Left session");
    SendDiscovery();
    AppendChat("System", "Left the multiplayer session.");
    RefreshStatus();
}

AnsiString TMultiplayerPeerForm::ChooseBestSession() const
{
    std::map<AnsiString, int> scores;
    std::set<AnsiString> occupied;
    AnsiString selected = BoxCombo->Items->Strings[BoxCombo->ItemIndex];
    int separator = selected.Pos(" ");
    AnsiString selectedBox = (separator > 1 ? selected.SubString(1, separator - 1) : selected).Trim().UpperCase();
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        const TPeerState& peer = it->second;
        if(!peer.Active || peer.SessionID == "") continue;
        if(peer.BoxCode == selectedBox) occupied.insert(peer.SessionID);
        if(BoxesAreAdjacent(selectedBox, peer.BoxCode)) scores[peer.SessionID] += 10;
        else scores[peer.SessionID] += 1;
    }
    AnsiString best;
    int bestScore = -1;
    for(std::map<AnsiString, int>::const_iterator it = scores.begin(); it != scores.end(); ++it)
    {
        if(occupied.count(it->first)) continue;
        if(it->second > bestScore)
        {
            best = it->first;
            bestScore = it->second;
        }
    }
    return best;
}

AnsiString TMultiplayerPeerForm::CreateSessionID() const
{
    return "S" + IntToHex(static_cast<int>(GetTickCount64() & 0x7FFFFFFF), 8) + "-" + IntToHex(LocalPort, 4);
}

AnsiString TMultiplayerPeerForm::CreateEventID(const AnsiString& Prefix) const
{
    return Prefix + "-" + PeerID + "-" + IntToHex(static_cast<int>(GetTickCount64() & 0x7FFFFFFF), 8);
}

AnsiString TMultiplayerPeerForm::DetectBoxCode(const AnsiString& RailwayTitle) const
{
    AnsiString lower = RailwayTitle.LowerCase();
    for(std::vector<TSignalboxDefinition>::const_iterator it = Signalboxes.begin(); it != Signalboxes.end(); ++it)
    {
        if(lower.Pos(it->RailwayTitle.LowerCase()) > 0 || lower.Pos(it->Name.LowerCase()) > 0 || lower == it->Code.LowerCase())
            return it->Code;
    }
    return "";
}

AnsiString TMultiplayerPeerForm::SignalboxName(const AnsiString& Code) const
{
    for(std::vector<TSignalboxDefinition>::const_iterator it = Signalboxes.begin(); it != Signalboxes.end(); ++it)
        if(it->Code == Code) return it->Name;
    return Code;
}

bool TMultiplayerPeerForm::BoxesAreAdjacent(const AnsiString& First, const AnsiString& Second) const
{
    for(std::vector<TConnectionDefinition>::const_iterator it = Connections.begin(); it != Connections.end(); ++it)
        if(it->FromBox == First && it->ToBox == Second) return true;
    return false;
}

bool TMultiplayerPeerForm::FindDestination(const AnsiString& LocalPortal, AnsiString& DestinationBox,
                                           AnsiString& DestinationPortal) const
{
    const AnsiString portal = LocalPortal.Trim().UpperCase();
    for(std::vector<TConnectionDefinition>::const_iterator it = Connections.begin(); it != Connections.end(); ++it)
    {
        if(it->FromBox != BoxCode) continue;
        if(it->LocalPortal != "" && it->LocalPortal == portal)
        {
            DestinationBox = it->ToBox;
            DestinationPortal = it->RemotePortal;
            return true;
        }
    }
    return false;
}

const TMultiplayerPeerForm::TPeerState *TMultiplayerPeerForm::FindActivePeerForBox(const AnsiString& DestinationBox) const
{
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        const TPeerState& peer = it->second;
        if(peer.Active && peer.SessionID == SessionID && peer.BoxCode == DestinationBox) return &peer;
    }
    return NULL;
}

bool TMultiplayerPeerForm::ValidateConnectionPacket(const AnsiString& SenderPeerID, const AnsiString& FromBox,
                                                     const AnsiString& FromPortal, const AnsiString& ToPortal,
                                                     const AnsiString& Address, unsigned short Port) const
{
    std::map<AnsiString, TPeerState>::const_iterator sender = Peers.find(SenderPeerID);
    if(sender == Peers.end() || !sender->second.Active || sender->second.SessionID != SessionID ||
       sender->second.BoxCode != FromBox.UpperCase() || sender->second.Port != Port)
        return false;
    for(std::vector<TConnectionDefinition>::const_iterator connection = Connections.begin();
        connection != Connections.end(); ++connection)
    {
        if(connection->FromBox == BoxCode && connection->ToBox == FromBox.UpperCase() &&
           connection->LocalPortal == ToPortal.UpperCase() && connection->RemotePortal == FromPortal.UpperCase())
            return true;
    }
    return false;
}

void TMultiplayerPeerForm::QueueTrainTransfer(const AnsiString& ServiceReference, const AnsiString& HeadCode,
                                               int RepeatNumber, const AnsiString& LocalPortal)
{
    if(!Joined) return;
    if(!OwnerInterface || !OwnerInterface->MultiplayerPortalAllowsExit(LocalPortal))
    {
        AppendTransfer("Train " + HeadCode + " left at " + LocalPortal +
                       ", but that portal is not configured as an exit.");
        LogMultiplayerEvent("TRAIN-DIRECTION-REJECT", "Reference=" + ServiceReference + ", headcode=" +
                            HeadCode + ", portal=" + LocalPortal + "; portal is not an exit");
        return;
    }
    if(PendingTransfers.size() >= 2048)
    {
        AppendTransfer("Train handover queue is full; " + HeadCode + " remains unoffered at " + LocalPortal + ".");
        LogMultiplayerEvent("TRAIN-QUEUE-FULL", "Headcode=" + HeadCode + ", portal=" + LocalPortal);
        return;
    }
    const AnsiString cleanReference = CleanField(ServiceReference);
    if(WTTModeEnabled)
    {
        std::map<AnsiString, AnsiString>::const_iterator expected = WTTExpectedExitPortals.find(cleanReference);
        if(expected == WTTExpectedExitPortals.end())
        {
            LogMultiplayerEvent("TRAIN-NOT-TRANSFERRED", "Reference=" + cleanReference + ", headcode=" + HeadCode +
                                ", portal=" + LocalPortal + "; service has no multiplayer exit from this box");
            return;
        }
        if(expected->second != LocalPortal.Trim().UpperCase())
        {
            AppendTransfer(HeadCode + " left by unexpected portal " + LocalPortal +
                           "; multiplayer handover was blocked (expected " + expected->second + ").");
            LogMultiplayerEvent("TRAIN-WRONG-EXIT", "Reference=" + cleanReference + ", actual=" + LocalPortal +
                                ", expected=" + expected->second);
            return;
        }
    }
    AnsiString destinationBox, destinationPortal;
    if(!FindDestination(LocalPortal, destinationBox, destinationPortal))
    {
        AppendTransfer("Train " + HeadCode + " left at portal " + LocalPortal +
                       ", but no unique multiplayer connection is registered.");
        return;
    }
    TPendingTransfer transfer;
    transfer.EventID = CreateEventID("TRAIN");
    transfer.SessionID = SessionID;
    transfer.FromBox = BoxCode;
    transfer.ToBox = destinationBox;
    transfer.FromPortal = CleanField(LocalPortal);
    transfer.ToPortal = CleanField(destinationPortal);
    transfer.ServiceReference = cleanReference;
    transfer.RunKey = "";
    for(std::multimap<int, AnsiString>::const_iterator reference = WTTServiceReferences.begin();
        reference != WTTServiceReferences.end(); ++reference)
    {
        if(reference->second != transfer.ServiceReference) continue;
        if(reference->first >= 0 && reference->first < static_cast<int>(WTTCatalog.AllServices().size()))
            transfer.RunKey = CleanField(WTTCatalog.AllServices()[reference->first].RunKey);
        break;
    }
    transfer.HeadCode = CleanField(HeadCode);
    transfer.LastRejection = "";
    transfer.RepeatNumber = RepeatNumber;
    transfer.Attempts = 0;
    transfer.Created = TDateTime::CurrentDateTime();
    transfer.LastSent = TDateTime(0);
    if(transfer.RunKey != "") WTTCatalog.ReportHandover(transfer.RunKey, destinationBox, SessionDateTime());
    PendingTransfers.push_back(transfer);
    LogMultiplayerEvent("TRAIN-QUEUE", "Event=" + transfer.EventID + ", headcode=" + transfer.HeadCode +
                        ", reference=" + transfer.ServiceReference + ", run-key=" + transfer.RunKey + ", " +
                        transfer.FromBox + "/" + transfer.FromPortal + " -> " + transfer.ToBox + "/" + transfer.ToPortal);
    SendPendingTransfers();
}

void __fastcall TMultiplayerPeerForm::JoinButtonClick(TObject *Sender)
{
    StartSession(false);
}

void __fastcall TMultiplayerPeerForm::NewSessionButtonClick(TObject *Sender)
{
    StartSession(true);
}

void __fastcall TMultiplayerPeerForm::LeaveButtonClick(TObject *Sender)
{
    LeaveSession();
}

void __fastcall TMultiplayerPeerForm::SendChatButtonClick(TObject *Sender)
{
    if(!Joined) return;
    AnsiString text = CleanField(ChatEdit->Text).Trim();
    if(text == "") return;
    AnsiString scope = ChatScopeCombo->Text;
    AnsiString id = CreateEventID("CHAT");
    AnsiString message = "CHAT|1|" + id + "|" + scope + "|" + SessionID + "|" + BoxCode + "|" +
                         CleanField(UserNameEdit->Text) + "|" + PeerID + "|" + text + "|" + AnsiString(ChatColour);
    ReceivedChatIDs[id] = TDateTime::CurrentDateTime();
    AppendAreaChat(SignalboxName(BoxCode), CleanField(UserNameEdit->Text), text, ChatColour);
    for(std::map<AnsiString, TPeerState>::const_iterator it = Peers.begin(); it != Peers.end(); ++it)
    {
        const TPeerState& peer = it->second;
        if(!peer.Active) continue;
        // Delivery is deliberately broader than display filtering.  The receiver decides
        // whether the scope applies, then acknowledges even a filtered or duplicate packet.
        TPendingChatDelivery delivery;
        delivery.EventID = id;
        delivery.TargetPeerID = peer.PeerID;
        delivery.Address = peer.Address;
        delivery.Port = peer.Port;
        delivery.Message = message;
        delivery.Created = TDateTime::CurrentDateTime();
        delivery.LastSent = TDateTime(0);
        PendingChatDeliveries.push_back(delivery);
    }
    SendPendingChats();
    ChatEdit->Text = "";
}

void __fastcall TMultiplayerPeerForm::WTTEnableButtonClick(TObject *Sender)
{
    if(WTTModeEnabled) DisableWTTMode();
    else EnableWTTMode();
}

void __fastcall TMultiplayerPeerForm::WTTServiceListClick(TObject *Sender)
{
    WTTStopCombo->Items->Clear();
    DisplayedWTTCallIndices.clear();
    const int selected = WTTServicesList->ItemIndex;
    if(selected < 0 || selected >= static_cast<int>(DisplayedWTTServiceIndices.size())) return;
    const TMultiplayerWTTService& service = WTTCatalog.AllServices().at(DisplayedWTTServiceIndices[selected]);
    unsigned short hour, minute, second, millisecond;
    DecodeTime(SessionDateTime(), hour, minute, second, millisecond);
    int secondsNow = (hour * 3600) + (minute * 60) + second;
    for(unsigned int callIndex = 0; callIndex < service.Calls.size(); ++callIndex)
    {
        const TMultiplayerWTTCall& call = service.Calls[callIndex];
        int comparableNow = secondsNow;
        if(call.ScheduledSeconds >= (24 * 3600) && comparableNow < (12 * 3600)) comparableNow += 24 * 3600;
        if(call.OwnerBox != BoxCode || call.RailwayLocation == "" || call.PassRaw != "" ||
           call.Status != TMultiplayerWTTCall::Planned ||
           call.ScheduledSeconds + service.DelaySeconds <= comparableNow) continue;
        WTTStopCombo->Items->Add(call.RailwayLocation + "  " + WTTClockText(call.ScheduledSeconds + service.DelaySeconds));
        DisplayedWTTCallIndices.push_back(callIndex);
    }
    if(WTTStopCombo->Items->Count > 0) WTTStopCombo->ItemIndex = 0;
}

void __fastcall TMultiplayerPeerForm::WTTSkipButtonClick(TObject *Sender)
{
    const int selectedService = WTTServicesList->ItemIndex;
    const int selectedStop = WTTStopCombo->ItemIndex;
    if(!WTTModeEnabled || selectedService < 0 || selectedService >= static_cast<int>(DisplayedWTTServiceIndices.size()) ||
       selectedStop < 0 || selectedStop >= static_cast<int>(DisplayedWTTCallIndices.size())) return;
    const int serviceIndex = DisplayedWTTServiceIndices[selectedService];
    const int callIndex = DisplayedWTTCallIndices[selectedStop];
    if(!WTTCatalog.SetCallSkipped(serviceIndex, callIndex, BoxCode))
    {
        AppendTransfer("Stop skip rejected: this signalbox does not own that stop.");
        return;
    }
    const TMultiplayerWTTService& service = WTTCatalog.AllServices().at(serviceIndex);
    std::pair<std::multimap<int, AnsiString>::const_iterator, std::multimap<int, AnsiString>::const_iterator> references =
        WTTServiceReferences.equal_range(serviceIndex);
    for(std::multimap<int, AnsiString>::const_iterator reference = references.first;
        reference != references.second; ++reference)
        if(OwnerInterface) OwnerInterface->ApplyMultiplayerWTTSkip(reference->second,
            service.Calls[callIndex].RailwayLocation);
    AppendTransfer(service.TID + " will pass " + service.Calls[callIndex].RailwayLocation +
                   "; the alteration is being sent downstream.");
    SendWTTStates();
    RefreshWTTServices();
}

void __fastcall TMultiplayerPeerForm::WTTCancelButtonClick(TObject *Sender)
{
    const int selected = WTTServicesList->ItemIndex;
    if(!WTTModeEnabled || selected < 0 || selected >= static_cast<int>(DisplayedWTTServiceIndices.size())) return;
    const int serviceIndex = DisplayedWTTServiceIndices[selected];
    const AnsiString tid = WTTCatalog.AllServices().at(serviceIndex).TID;
    std::pair<std::multimap<int, AnsiString>::const_iterator, std::multimap<int, AnsiString>::const_iterator> references =
        WTTServiceReferences.equal_range(serviceIndex);
    for(std::multimap<int, AnsiString>::const_iterator reference = references.first;
        reference != references.second; ++reference)
    {
        if(OwnerInterface && OwnerInterface->MultiplayerWTTServiceRunning(reference->second))
        {
            AppendTransfer(tid + " is already running in this signalbox and cannot be cancelled as a future service.");
            return;
        }
    }
    if(WTTCatalog.SetCancelled(serviceIndex, true))
    {
        for(std::multimap<int, AnsiString>::const_iterator reference = references.first;
            reference != references.second; ++reference)
            if(OwnerInterface) OwnerInterface->ApplyMultiplayerWTTCancellation(reference->second);
        AppendTransfer(tid + " cancelled; the cancellation is being sent to every box in the session.");
        SendWTTStates();
        RefreshWTTServices();
    }
}

void __fastcall TMultiplayerPeerForm::ChatEditKeyPress(TObject *Sender, System::WideChar &Key)
{
    if(Key == 13)
    {
        Key = 0;
        SendChatButtonClick(Sender);
    }
}

void __fastcall TMultiplayerPeerForm::FormCloseHandler(TObject *Sender, TCloseAction &Action)
{
    Action = caHide;
}

std::vector<AnsiString> TMultiplayerPeerForm::Split(const AnsiString& Value, wchar_t Delimiter)
{
    std::vector<AnsiString> result;
    int start = 1;
    for(int index = 1; index <= Value.Length(); ++index)
    {
        if(Value[index] == Delimiter)
        {
            result.push_back(Value.SubString(start, index - start));
            start = index + 1;
        }
    }
    result.push_back(Value.SubString(start, Value.Length() - start + 1));
    return result;
}

AnsiString TMultiplayerPeerForm::CleanField(const AnsiString& Value)
{
    AnsiString result = Value;
    result = StringReplace(result, "|", "/", TReplaceFlags() << rfReplaceAll);
    result = StringReplace(result, "\r", " ", TReplaceFlags() << rfReplaceAll);
    result = StringReplace(result, "\n", " ", TReplaceFlags() << rfReplaceAll);
    if(result.Length() > 400) result = result.SubString(1, 400);
    return result;
}
