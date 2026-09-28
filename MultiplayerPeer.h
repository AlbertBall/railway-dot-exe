#ifndef MultiplayerPeerH
#define MultiplayerPeerH

#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.StdCtrls.hpp>
#include <IdUDPClient.hpp>
#include <map>
#include <set>
#include <vector>
#include "MultiplayerWTT.h"

class TInterface;

class TMultiplayerPeerForm : public TForm
{
private:
    struct TSignalboxDefinition
    {
        AnsiString Code;
        AnsiString Name;
        AnsiString RailwayTitle;
    };

    struct TConnectionDefinition
    {
        AnsiString FromBox;
        AnsiString LocalPortal;
        AnsiString ToBox;
        AnsiString RemotePortal;
    };

    struct TWTTBookDefinition
    {
        AnsiString BoxCode;
        AnsiString BookCode;
        AnsiString CacheFile;
    };

    struct TWTTBoundaryDefinition
    {
        AnsiString BoxCode;
        AnsiString WTTLocation;
        AnsiString BoundaryNames;
    };

    struct TPeerState
    {
        AnsiString PeerID;
        AnsiString UserName;
        AnsiString BoxCode;
        AnsiString SessionID;
        AnsiString Address;
        unsigned short Port;
        bool Active;
        bool AutoMatch;
        int ChatColour;
        bool WTTEnabled;
        AnsiString WTTFingerprint;
        AnsiString ClockAuthorityPeerID;
        __int64 SessionClockMilliseconds;
        __int64 RailwayClockMilliseconds;
        TDateTime LastSeen;
    };

    struct TPendingTransfer
    {
        AnsiString EventID;
        AnsiString SessionID;
        AnsiString FromBox;
        AnsiString ToBox;
        AnsiString FromPortal;
        AnsiString ToPortal;
        AnsiString ServiceReference;
        AnsiString RunKey;
        AnsiString HeadCode;
        AnsiString LastRejection;
        int RepeatNumber;
        int Attempts;
        TDateTime Created;
        TDateTime LastSent;
    };

    struct TPendingChatDelivery
    {
        AnsiString EventID;
        AnsiString TargetPeerID;
        AnsiString Address;
        unsigned short Port;
        AnsiString Message;
        TDateTime Created;
        TDateTime LastSent;
    };

    TInterface *OwnerInterface;
    AnsiString RootDirectory;
    TIdUDPClient *PeerSocket;
    TTimer *NetworkTimer;
    unsigned short LocalPort;
    AnsiString PeerID;
    AnsiString SessionID;
    AnsiString BoxCode;
    AnsiString RailwayBoxCode;
    bool Joined;
    bool AutoMatch;
    int ChatColour;
    bool WTTModeEnabled;
    AnsiString ClockAuthorityPeerID;
    double SessionClockOffsetDays;
    int LastPassedWTTServiceCount;
    int TickCounter;
    AnsiString MultiplayerLogFile;
    mutable std::vector<AnsiString> MultiplayerLogBuffer;

    TPanel *HeaderPanel;
    TLabel *StatusLabel;
    TEdit *UserNameEdit;
    TComboBox *BoxCombo;
    TButton *JoinButton;
    TButton *NewSessionButton;
    TButton *LeaveButton;
    TStringGrid *UsersGrid;
    TStringGrid *BoxesGrid;
    TRichEdit *ChatMemo;
    TComboBox *ChatScopeCombo;
    TEdit *ChatEdit;
    TButton *SendChatButton;
    TMemo *TransferMemo;
    TPanel *WTTPanel;
    TButton *WTTEnableButton;
    TLabel *WTTStatusLabel;
    TListBox *WTTServicesList;
    TComboBox *WTTStopCombo;
    TButton *WTTSkipButton;
    TButton *WTTCancelButton;

    TMultiplayerWTTCatalog WTTCatalog;
    std::vector<TWTTBookDefinition> WTTBooks;
    std::vector<TWTTBoundaryDefinition> WTTBoundaries;
    std::vector<int> DisplayedWTTServiceIndices;
    std::vector<int> DisplayedWTTCallIndices;
    std::multimap<int, AnsiString> WTTServiceReferences;
    std::map<AnsiString, AnsiString> WTTExpectedEntryPortals;
    std::map<AnsiString, AnsiString> WTTExpectedExitPortals;

    std::vector<TSignalboxDefinition> Signalboxes;
    std::vector<TConnectionDefinition> Connections;
    std::map<AnsiString, TPeerState> Peers;
    std::vector<TPendingTransfer> PendingTransfers;
    std::vector<TPendingChatDelivery> PendingChatDeliveries;
    std::map<AnsiString, TDateTime> ReceivedTransferIDs;
    std::map<AnsiString, TDateTime> ReceivedChatIDs;
    std::map<AnsiString, int> LastSentBoundaryAspects;
    std::map<AnsiString, TDateTime> LastBoundaryBroadcasts;
    std::map<AnsiString, unsigned int> LastSentBoundarySequences;
    std::map<AnsiString, int> RemoteBoundaryAspects;
    std::map<AnsiString, unsigned int> RemoteBoundarySequences;
    std::map<AnsiString, int> AcknowledgedWTTRevisions;
    std::map<AnsiString, TDateTime> LastWTTStateSent;

    void BuildInterface();
    void LoadRegistry();
    void BindPeerSocket();
    void SendDiscovery();
    void ReceiveMessages();
    void ProcessMessage(const AnsiString& Message, const AnsiString& Address, unsigned short Port);
    void ProcessHello(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port);
    void ProcessChat(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port);
    void ProcessChatAcknowledgement(const std::vector<AnsiString>& Parts);
    void ProcessTrain(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port);
    void ProcessAcknowledgement(const std::vector<AnsiString>& Parts);
    void ProcessNegativeAcknowledgement(const std::vector<AnsiString>& Parts);
    void ProcessBoundaryState(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port);
    void ProcessWTTState(const std::vector<AnsiString>& Parts, const AnsiString& Address, unsigned short Port);
    void ProcessWTTAcknowledgement(const std::vector<AnsiString>& Parts);
    void SendTo(const AnsiString& Address, unsigned short Port, const AnsiString& Message);
    void SendPendingTransfers();
    void SendPendingChats();
    void SendBoundaryStates();
    void SendWTTStates();
    void ResolveSessionMatch();
    void ResolveDuplicateOccupation();
    void PrunePeers();
    void ClearRemoteBoundariesForBox(const AnsiString& RemoteBox);
    bool ValidateConnectionPacket(const AnsiString& SenderPeerID, const AnsiString& FromBox,
                                  const AnsiString& FromPortal, const AnsiString& ToPortal,
                                  const AnsiString& Address, unsigned short Port) const;
    void RefreshUsersGrid();
    void RefreshBoxesGrid();
    void RefreshStatus();
    void AppendChat(const AnsiString& Prefix, const AnsiString& Text);
    void AppendAreaChat(const AnsiString& AreaName, const AnsiString& UserName, const AnsiString& Text, int Colour);
    void AppendTransfer(const AnsiString& Text);
    bool LoadWTTCache(AnsiString &ErrorMessage);
    bool WTTBooksCompatible(const AnsiString& OtherSummary, AnsiString &MismatchBook) const;
    AnsiString WTTBoundaryNames(const AnsiString& WTTLocation) const;
    AnsiString ConnectionPortalSelectors(const AnsiString& OtherBox) const;
    void EnableWTTMode();
    void DisableWTTMode();
    void RefreshWTTServices();
    bool BuildLocalWTTEntries(std::vector<AnsiString>& Entries, AnsiString &ErrorMessage);
    TDateTime SessionDateTime() const;
    __int64 SessionClockMilliseconds() const;
    void AdoptSessionClock(const TPeerState& State);
    void ReconcileSessionClock();
    int ChooseChatColour() const;
    void StartSession(bool ForceNewSession);
    void LeaveSession();
    AnsiString ChooseBestSession() const;
    AnsiString CreateSessionID() const;
    AnsiString CreateEventID(const AnsiString& Prefix) const;
    AnsiString DetectBoxCode(const AnsiString& RailwayTitle) const;
    AnsiString SignalboxName(const AnsiString& Code) const;
    bool BoxesAreAdjacent(const AnsiString& First, const AnsiString& Second) const;
    bool FindDestination(const AnsiString& LocalPortal, AnsiString& DestinationBox, AnsiString& DestinationPortal) const;
    const TPeerState *FindActivePeerForBox(const AnsiString& DestinationBox) const;
    void FlushMultiplayerLog() const;
    static std::vector<AnsiString> Split(const AnsiString& Value, wchar_t Delimiter);
    static AnsiString CleanField(const AnsiString& Value);

    void __fastcall NetworkTimerTick(TObject *Sender);
    void __fastcall JoinButtonClick(TObject *Sender);
    void __fastcall NewSessionButtonClick(TObject *Sender);
    void __fastcall LeaveButtonClick(TObject *Sender);
    void __fastcall SendChatButtonClick(TObject *Sender);
    void __fastcall WTTEnableButtonClick(TObject *Sender);
    void __fastcall WTTServiceListClick(TObject *Sender);
    void __fastcall WTTSkipButtonClick(TObject *Sender);
    void __fastcall WTTCancelButtonClick(TObject *Sender);
    void __fastcall ChatEditKeyPress(TObject *Sender, System::WideChar &Key);
    void __fastcall FormCloseHandler(TObject *Sender, TCloseAction &Action);

public:
    __fastcall TMultiplayerPeerForm(TComponent *Owner, TInterface *InterfaceOwner, const AnsiString& BaseDirectory);
    __fastcall ~TMultiplayerPeerForm();

    void ShowForRailway(const AnsiString& RailwayTitle);
    void PumpNetwork();
    void QueueTrainTransfer(const AnsiString& ServiceReference, const AnsiString& HeadCode, int RepeatNumber,
                            const AnsiString& LocalPortal);
    bool IsJoined() const { return Joined; }
    AnsiString CurrentBoxCode() const { return BoxCode; }
    AnsiString CurrentSessionID() const { return SessionID; }
    bool BoundaryTargetAttribute(const AnsiString& LocalPortal, int &TargetAttribute) const;
    bool ShouldHoldPortalEntry(const AnsiString& LocalPortal) const;
    bool GetFullWTTForService(const AnsiString& ServiceReference, AnsiString& TimetableText) const;
    void RailwayOperationEnded();
    void LogMultiplayerEvent(const AnsiString& Category, const AnsiString& Text) const;
};

#endif
