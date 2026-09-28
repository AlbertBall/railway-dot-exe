#ifndef MultiplayerWTTH
#define MultiplayerWTTH

#include <System.SysUtils.hpp>
#include <map>
#include <utility>
#include <vector>

struct TMultiplayerWTTCall
{
    enum TCallStatus { Planned, Completed, Skipped };

    AnsiString WTTLocation;
    AnsiString RailwayLocation;
    AnsiString OwnerBox;
    AnsiString ArrivalRaw;
    AnsiString DepartureRaw;
    AnsiString PassRaw;
    AnsiString Platform;
    AnsiString RunningLine;
    int ArrivalSeconds;
    int DepartureSeconds;
    int PassSeconds;
    int ScheduledSeconds;
    int ActualSeconds;
    int Revision;
    TCallStatus Status;

    TMultiplayerWTTCall() : ArrivalSeconds(-1), DepartureSeconds(-1), PassSeconds(-1),
                            ScheduledSeconds(-1), ActualSeconds(-1), Revision(0), Status(Planned) {}
};

struct TMultiplayerWTTService
{
    AnsiString RunKey;
    AnsiString SourceWorkbook;
    AnsiString SheetName;
    AnsiString TID;
    AnsiString UID;
    AnsiString OperatorCode;
    AnsiString Origin;
    AnsiString Destination;
    AnsiString DateRange;
    AnsiString RunningDays;
    AnsiString ServiceCode;
    AnsiString OperationalOwner;
    std::vector<TMultiplayerWTTCall> Calls;
    int DelaySeconds;
    int Revision;
    bool Cancelled;

    TMultiplayerWTTService() : DelaySeconds(0), Revision(0), Cancelled(false) {}
};

class TMultiplayerWTTCatalog
{
private:
    std::vector<TMultiplayerWTTService> Services;
    AnsiString SourceDescription;
    AnsiString CacheVersion;
    AnsiString CacheFingerprint;
    std::map<AnsiString, AnsiString> BookFingerprints;

    static std::vector<AnsiString> SplitTabs(const AnsiString& Value);
    static bool ParseDateDMY(const AnsiString& Value, TDateTime &Date);
    static int ParseWTTTime(const AnsiString& Value);
    static bool RunningCodeAllowsDay(const AnsiString& Code, int DayNumber);
    static void BuildTimeline(TMultiplayerWTTService &Service);

public:
    bool Load(const AnsiString& FileName, AnsiString &ErrorMessage);
    bool LoadMany(const std::vector<std::pair<AnsiString, AnsiString> >& BookFiles, AnsiString &ErrorMessage);
    void Clear();
    int ServiceCount() const { return static_cast<int>(Services.size()); }
    const AnsiString& Source() const { return SourceDescription; }
    const AnsiString& Version() const { return CacheVersion; }
    const AnsiString& Fingerprint() const { return CacheFingerprint; }
    AnsiString BookSummary() const;
    AnsiString BookList() const;
    bool CompatibleBooks(const AnsiString& OtherSummary, AnsiString &MismatchBook) const;
    const std::vector<TMultiplayerWTTService>& AllServices() const { return Services; }
    std::vector<int> ServicesOwnedAt(const AnsiString& BoxCode, const TDateTime& LocalDateTime) const;
    bool ServiceRunsOnDate(const TMultiplayerWTTService& Service, const TDateTime& Date) const;
    AnsiString CurrentOwner(const TMultiplayerWTTService& Service, const TDateTime& LocalDateTime) const;
    bool SetCallSkipped(int ServiceIndex, int CallIndex, const AnsiString& RequestingBox);
    bool SetCancelled(int ServiceIndex, bool Cancelled);
    int FindService(const AnsiString& RunKey) const;
    int FindCall(int ServiceIndex, const AnsiString& WTTLocation, int ScheduledSeconds) const;
    bool ApplyRemoteState(const AnsiString& RunKey, int ServiceRevision, int DelaySeconds, bool Cancelled,
                          int CallIndex, int CallRevision, int CallStatus, const AnsiString& RequestingBox,
                          const AnsiString& OperationalOwner);
    bool ReportHandover(const AnsiString& RunKey, const AnsiString& NewOwner, const TDateTime& SessionTime);
};

#endif
