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
#include <algorithm>
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "MultiplayerWTT.h"

#pragma package(smart_init)

std::vector<AnsiString> TMultiplayerWTTCatalog::SplitTabs(const AnsiString& Value)
{
    std::vector<AnsiString> parts;
    int start = 1;
    for(int index = 1; index <= Value.Length() + 1; ++index)
    {
        if(index > Value.Length() || Value[index] == '\t')
        {
            parts.push_back(Value.SubString(start, index - start));
            start = index + 1;
        }
    }
    return parts;
}

bool TMultiplayerWTTCatalog::ParseDateDMY(const AnsiString& Value, TDateTime &Date)
{
    AnsiString trimmed = Value.Trim();
    if(trimmed.Length() < 10) return false;
    const int day = trimmed.SubString(1, 2).ToIntDef(0);
    const int month = trimmed.SubString(4, 2).ToIntDef(0);
    const int year = trimmed.SubString(7, 4).ToIntDef(0);
    if(day < 1 || month < 1 || month > 12 || year < 1900) return false;
    try
    {
        Date = EncodeDate(year, month, day);
        return true;
    }
    catch(const Exception&)
    {
        return false;
    }
}

int TMultiplayerWTTCatalog::ParseWTTTime(const AnsiString& Value)
{
    AnsiString digits;
    bool halfMinute = false;
    for(int index = 1; index <= Value.Length(); ++index)
    {
        const unsigned char character = static_cast<unsigned char>(Value[index]);
        if(character >= '0' && character <= '9') digits += static_cast<char>(character);
        if(character == 'H' || character == 'h' || character == 0xBD) halfMinute = true;
    }
    if(digits.Length() < 4) return -1;
    const int hours = digits.SubString(1, 2).ToIntDef(-1);
    const int minutes = digits.SubString(3, 2).ToIntDef(-1);
    if(hours < 0 || hours > 47 || minutes < 0 || minutes > 59) return -1;
    return (hours * 3600) + (minutes * 60) + (halfMinute ? 30 : 0);
}

bool TMultiplayerWTTCatalog::RunningCodeAllowsDay(const AnsiString& Code, int DayNumber)
{
    const AnsiString code = Code.Trim().UpperCase();
    if(code == "SX") return DayNumber >= 2 && DayNumber <= 6;
    if(code == "SO") return DayNumber == 7;
    if(code == "SUO") return DayNumber == 1;
    if(code == "FSX") return DayNumber >= 2 && DayNumber <= 5;
    if(code == "MSX") return DayNumber >= 3 && DayNumber <= 6;
    if(code == "MX") return DayNumber >= 3 && DayNumber <= 7;
    if(code == "MO") return DayNumber == 2;
    if(code == "TO") return DayNumber == 3;
    if(code == "WO") return DayNumber == 4;
    if(code == "THO") return DayNumber == 5;
    if(code == "FO") return DayNumber == 6;
    if(code == "MWFO") return DayNumber == 2 || DayNumber == 4 || DayNumber == 6;
    if(code == "TTHO") return DayNumber == 3 || DayNumber == 5;
    if(code == "TWFO") return DayNumber == 3 || DayNumber == 4 || DayNumber == 6;
    if(code == "THSX") return DayNumber == 2 || DayNumber == 3 || DayNumber == 4 || DayNumber == 6;
    if(code == "WFO") return DayNumber == 4 || DayNumber == 6;
    if(code == "WSX") return DayNumber == 2 || DayNumber == 3 || DayNumber == 5 || DayNumber == 6;
    if(code == "TWTHO") return DayNumber == 3 || DayNumber == 4 || DayNumber == 5;
    return false;
}

namespace
{
int AlignWTTTimeToCall(int RawSeconds, int ScheduledSeconds)
{
    if(RawSeconds < 0 || ScheduledSeconds < 0) return -1;
    while(RawSeconds + (12 * 3600) < ScheduledSeconds) RawSeconds += 24 * 3600;
    while(RawSeconds - (12 * 3600) > ScheduledSeconds) RawSeconds -= 24 * 3600;
    return RawSeconds;
}

int ComparableCallSeconds(const TMultiplayerWTTCall& Call)
{
    if(Call.ScheduledSeconds < 0) return -1;
    int seconds = Call.ScheduledSeconds % (24 * 3600);
    if(seconds < 0) seconds += 24 * 3600;
    return seconds;
}

int CircularTimeDifference(int First, int Second)
{
    if(First < 0 || Second < 0) return 24 * 3600;
    First %= 24 * 3600;
    Second %= 24 * 3600;
    if(First < 0) First += 24 * 3600;
    if(Second < 0) Second += 24 * 3600;
    const int difference = abs(First - Second);
    return std::min(difference, (24 * 3600) - difference);
}

bool SameJourneyCall(const TMultiplayerWTTCall& First, const TMultiplayerWTTCall& Second)
{
    if(First.WTTLocation.Trim().UpperCase() != Second.WTTLocation.Trim().UpperCase()) return false;

    // Sectional WTT books do not always expose the same part of a station call.
    // One extract may contain the arrival while another uses the departure as its
    // representative time.  Compare every available event before falling back to
    // ScheduledSeconds, otherwise a three-minute Guildford dwell is mistaken for
    // two separate visits and the stitched journey is put out of chronological order.
    const int firstEvents[] = {First.ArrivalSeconds, First.DepartureSeconds, First.PassSeconds};
    const int secondEvents[] = {Second.ArrivalSeconds, Second.DepartureSeconds, Second.PassSeconds};
    for(unsigned int first = 0; first < sizeof(firstEvents) / sizeof(firstEvents[0]); ++first)
        for(unsigned int second = 0; second < sizeof(secondEvents) / sizeof(secondEvents[0]); ++second)
            if(CircularTimeDifference(firstEvents[first], secondEvents[second]) <= 120) return true;

    const int firstSeconds = ComparableCallSeconds(First);
    const int secondSeconds = ComparableCallSeconds(Second);
    if(firstSeconds < 0 || secondSeconds < 0) return true;
    return CircularTimeDifference(firstSeconds, secondSeconds) <= 300;
}

TMultiplayerWTTCall MergeJourneyCall(const TMultiplayerWTTCall& First, const TMultiplayerWTTCall& Second)
{
    TMultiplayerWTTCall result = First;
    if(result.WTTLocation == "") result.WTTLocation = Second.WTTLocation;
    if(result.RailwayLocation == "") result.RailwayLocation = Second.RailwayLocation;
    if(result.OwnerBox == "") result.OwnerBox = Second.OwnerBox;
    if(result.ArrivalRaw == "") result.ArrivalRaw = Second.ArrivalRaw;
    if(result.DepartureRaw == "") result.DepartureRaw = Second.DepartureRaw;
    if(result.PassRaw == "") result.PassRaw = Second.PassRaw;
    if(result.Platform == "") result.Platform = Second.Platform;
    if(result.RunningLine == "") result.RunningLine = Second.RunningLine;
    return result;
}

std::vector<TMultiplayerWTTCall> StitchJourneyCalls(const std::vector<TMultiplayerWTTCall>& First,
                                                     const std::vector<TMultiplayerWTTCall>& Second)
{
    if(First.empty()) return Second;
    if(Second.empty()) return First;

    const unsigned int firstCount = First.size();
    const unsigned int secondCount = Second.size();
    std::vector<int> lcs((firstCount + 1) * (secondCount + 1), 0);
    for(int first = static_cast<int>(firstCount) - 1; first >= 0; --first)
    {
        for(int second = static_cast<int>(secondCount) - 1; second >= 0; --second)
        {
            const unsigned int cell = (first * (secondCount + 1)) + second;
            if(SameJourneyCall(First[first], Second[second]))
                lcs[cell] = 1 + lcs[((first + 1) * (secondCount + 1)) + second + 1];
            else
                lcs[cell] = std::max(lcs[((first + 1) * (secondCount + 1)) + second],
                                     lcs[(first * (secondCount + 1)) + second + 1]);
        }
    }

    // With no common timing point, only join fragments whose timetable ranges are
    // unambiguously ordered.  This covers adjacent sectional books without inventing
    // a route through two overlapping but unrelated extracts.
    if(lcs[0] == 0)
    {
        const int firstStart = First.front().ScheduledSeconds;
        const int firstEnd = First.back().ScheduledSeconds;
        const int secondStart = Second.front().ScheduledSeconds;
        const int secondEnd = Second.back().ScheduledSeconds;
        std::vector<TMultiplayerWTTCall> ordered;
        if(firstEnd >= 0 && secondStart >= 0 && firstEnd <= secondStart)
        {
            ordered = First;
            ordered.insert(ordered.end(), Second.begin(), Second.end());
            return ordered;
        }
        if(secondEnd >= 0 && firstStart >= 0 && secondEnd <= firstStart)
        {
            ordered = Second;
            ordered.insert(ordered.end(), First.begin(), First.end());
            return ordered;
        }
        return First.size() >= Second.size() ? First : Second;
    }

    std::vector<TMultiplayerWTTCall> result;
    unsigned int first = 0, second = 0;
    while(first < firstCount || second < secondCount)
    {
        if(first < firstCount && second < secondCount && SameJourneyCall(First[first], Second[second]))
        {
            result.push_back(MergeJourneyCall(First[first], Second[second]));
            ++first;
            ++second;
        }
        else if(second >= secondCount ||
                (first < firstCount && lcs[((first + 1) * (secondCount + 1)) + second] >=
                 lcs[(first * (secondCount + 1)) + second + 1]))
            result.push_back(First[first++]);
        else
            result.push_back(Second[second++]);
    }
    return result;
}

bool JourneyCallChronological(const TMultiplayerWTTCall& First, const TMultiplayerWTTCall& Second)
{
    if(First.ScheduledSeconds < 0) return false;
    if(Second.ScheduledSeconds < 0) return true;
    return First.ScheduledSeconds < Second.ScheduledSeconds;
}

void HashSemanticText(unsigned int &Hash, const AnsiString& Text)
{
    for(int character = 1; character <= Text.Length(); ++character)
    {
        Hash ^= static_cast<unsigned char>(Text[character]);
        Hash *= 16777619u;
    }
    Hash ^= '\n';
    Hash *= 16777619u;
}
}

void TMultiplayerWTTCatalog::BuildTimeline(TMultiplayerWTTService &Service)
{
    int previous = -1;
    int dayOffset = 0;
    int originSeconds = -1;
    if(Service.Origin.Length() >= 4)
    {
        const int lastSpace = Service.Origin.LastDelimiter(" ");
        const AnsiString originTime = lastSpace > 0 ?
                                      Service.Origin.SubString(lastSpace + 1, Service.Origin.Length() - lastSpace) :
                                      Service.Origin;
        originSeconds = ParseWTTTime(originTime);
    }
    for(std::vector<TMultiplayerWTTCall>::iterator call = Service.Calls.begin(); call != Service.Calls.end(); ++call)
    {
        const int rawArrival = ParseWTTTime(call->ArrivalRaw);
        const int rawDeparture = ParseWTTTime(call->DepartureRaw);
        const int rawPass = ParseWTTTime(call->PassRaw);
        int seconds = rawDeparture;
        if(seconds < 0) seconds = rawArrival;
        if(seconds < 0) seconds = rawPass;
        if(seconds < 0)
        {
            call->ArrivalSeconds = -1;
            call->DepartureSeconds = -1;
            call->PassSeconds = -1;
            call->ScheduledSeconds = -1;
            continue;
        }
        if(previous < 0 && originSeconds >= (18 * 3600) && seconds < (6 * 3600)) dayOffset = 24 * 3600;
        seconds += dayOffset;
        if(previous >= 0 && seconds + (6 * 3600) < previous)
        {
            dayOffset += 24 * 3600;
            seconds += 24 * 3600;
        }
        call->ScheduledSeconds = seconds;
        call->ArrivalSeconds = AlignWTTTimeToCall(rawArrival, seconds);
        call->DepartureSeconds = AlignWTTTimeToCall(rawDeparture, seconds);
        call->PassSeconds = AlignWTTTimeToCall(rawPass, seconds);
        previous = seconds;
    }
}

void TMultiplayerWTTCatalog::Clear()
{
    Services.clear();
    BookFingerprints.clear();
    SourceDescription = "";
    CacheVersion = "";
    CacheFingerprint = "";
}

bool TMultiplayerWTTCatalog::LoadMany(const std::vector<std::pair<AnsiString, AnsiString> >& BookFiles,
                                      AnsiString &ErrorMessage)
{
    Clear();
    if(BookFiles.empty())
    {
        ErrorMessage = "No WTT books are assigned to this signalbox.";
        return false;
    }

    std::map<AnsiString, int> serviceIndices;
    unsigned int combinedFingerprint = 2166136261u;
    for(std::vector<std::pair<AnsiString, AnsiString> >::const_iterator book = BookFiles.begin();
        book != BookFiles.end(); ++book)
    {
        TMultiplayerWTTCatalog loaded;
        AnsiString loadError;
        if(!loaded.Load(book->second, loadError))
        {
            ErrorMessage = "WTT book " + book->first + " is unavailable. " + loadError;
            Clear();
            return false;
        }
        const AnsiString bookCode = book->first.Trim().UpperCase();
        BookFingerprints[bookCode] = loaded.Fingerprint();
        const AnsiString fingerprintText = bookCode + "=" + loaded.Fingerprint() + ";";
        for(int character = 1; character <= fingerprintText.Length(); ++character)
        {
            combinedFingerprint ^= static_cast<unsigned char>(fingerprintText[character]);
            combinedFingerprint *= 16777619u;
        }

        const std::vector<TMultiplayerWTTService>& loadedServices = loaded.AllServices();
        for(std::vector<TMultiplayerWTTService>::const_iterator service = loadedServices.begin();
            service != loadedServices.end(); ++service)
        {
            std::map<AnsiString, int>::iterator existing = serviceIndices.find(service->RunKey);
            if(existing == serviceIndices.end())
            {
                serviceIndices[service->RunKey] = static_cast<int>(Services.size());
                Services.push_back(*service);
            }
            else
            {
                TMultiplayerWTTService& canonical = Services[existing->second];
                const int oldStart = canonical.Calls.empty() ? -1 : canonical.Calls.front().ScheduledSeconds;
                const int newStart = service->Calls.empty() ? -1 : service->Calls.front().ScheduledSeconds;
                const int oldEnd = canonical.Calls.empty() ? -1 : canonical.Calls.back().ScheduledSeconds;
                const int newEnd = service->Calls.empty() ? -1 : service->Calls.back().ScheduledSeconds;
                canonical.Calls = StitchJourneyCalls(canonical.Calls, service->Calls);
                // Sectional books can overlap a long, looping service in different
                // orders.  LCS gives us the union and removes common calls; the WTT
                // times are the authority for the final journey order.
                std::stable_sort(canonical.Calls.begin(), canonical.Calls.end(), JourneyCallChronological);
                if((oldStart < 0 || (newStart >= 0 && newStart < oldStart)) && service->Origin != "")
                    canonical.Origin = service->Origin;
                if(newEnd > oldEnd && service->Destination != "") canonical.Destination = service->Destination;
                if(canonical.SourceWorkbook.Pos(service->SourceWorkbook) == 0)
                    canonical.SourceWorkbook += (canonical.SourceWorkbook == "" ? "" : ";") + service->SourceWorkbook;
                BuildTimeline(canonical);
            }
        }
    }
    CacheVersion = "1";
    CacheFingerprint = IntToHex(static_cast<int>(combinedFingerprint), 8);
    SourceDescription = BookList();
    ErrorMessage = "";
    return true;
}

AnsiString TMultiplayerWTTCatalog::BookSummary() const
{
    AnsiString result;
    for(std::map<AnsiString, AnsiString>::const_iterator book = BookFingerprints.begin();
        book != BookFingerprints.end(); ++book)
    {
        if(result != "") result += ";";
        result += book->first + "=" + book->second;
    }
    return result;
}

AnsiString TMultiplayerWTTCatalog::BookList() const
{
    AnsiString result;
    for(std::map<AnsiString, AnsiString>::const_iterator book = BookFingerprints.begin();
        book != BookFingerprints.end(); ++book)
    {
        if(result != "") result += ", ";
        result += book->first;
    }
    return result;
}

bool TMultiplayerWTTCatalog::CompatibleBooks(const AnsiString& OtherSummary, AnsiString &MismatchBook) const
{
    MismatchBook = "";
    int start = 1;
    while(start <= OtherSummary.Length())
    {
        int separator = OtherSummary.SubString(start, OtherSummary.Length() - start + 1).Pos(";");
        AnsiString item;
        if(separator == 0)
        {
            item = OtherSummary.SubString(start, OtherSummary.Length() - start + 1);
            start = OtherSummary.Length() + 1;
        }
        else
        {
            item = OtherSummary.SubString(start, separator - 1);
            start += separator;
        }
        const int equals = item.Pos("=");
        if(equals <= 1) continue;
        const AnsiString code = item.SubString(1, equals - 1).Trim().UpperCase();
        const AnsiString fingerprint = item.SubString(equals + 1, item.Length() - equals).Trim();
        std::map<AnsiString, AnsiString>::const_iterator local = BookFingerprints.find(code);
        if(local != BookFingerprints.end() && local->second != fingerprint)
        {
            MismatchBook = code;
            return false;
        }
    }
    return true;
}

bool TMultiplayerWTTCatalog::Load(const AnsiString& FileName, AnsiString &ErrorMessage)
{
    Clear();
    std::ifstream input(FileName.c_str());
    if(!input)
    {
        ErrorMessage = "Unable to open WTT cache: " + FileName;
        return false;
    }

    std::string rawLine;
    unsigned int fingerprint = 2166136261u;
    TMultiplayerWTTService *current = NULL;
    while(std::getline(input, rawLine))
    {
        AnsiString line(rawLine.c_str());
        if(line.Length() > 0 && line[line.Length()] == '\r') line.SetLength(line.Length() - 1);
        std::vector<AnsiString> parts = SplitTabs(line);
        if(parts.empty()) continue;
        if(parts[0] == "WTT-CACHE")
        {
            if(parts.size() < 4 || parts[1] != "1")
            {
                ErrorMessage = "Unsupported WTT cache version.";
                Clear();
                return false;
            }
            CacheVersion = parts[1];
            SourceDescription = parts[3];
        }
        else if(parts[0] == "S" && parts.size() >= 12)
        {
            AnsiString semantic = "S";
            for(unsigned int part = 1; part < parts.size(); ++part)
                if(part != 2 && part != 3) semantic += "\t" + parts[part];
            HashSemanticText(fingerprint, semantic);
            TMultiplayerWTTService service;
            service.RunKey = parts[1];
            service.SourceWorkbook = parts[2];
            service.SheetName = parts[3];
            service.TID = parts[4];
            service.UID = parts[5];
            service.OperatorCode = parts[6];
            service.Origin = parts[7];
            service.Destination = parts[8];
            service.DateRange = parts[9];
            service.RunningDays = parts[10];
            service.ServiceCode = parts[11];
            Services.push_back(service);
            current = &Services.back();
        }
        else if(parts[0] == "C" && parts.size() >= 9 && current)
        {
            HashSemanticText(fingerprint, line);
            TMultiplayerWTTCall call;
            call.WTTLocation = parts[1];
            call.RailwayLocation = parts[2];
            call.OwnerBox = parts[3].UpperCase();
            call.ArrivalRaw = parts[4];
            call.DepartureRaw = parts[5];
            call.PassRaw = parts[6];
            if(call.ArrivalRaw.Trim() == "..") call.ArrivalRaw = "";
            if(call.DepartureRaw.Trim() == "..") call.DepartureRaw = "";
            if(call.PassRaw.Trim() == "..") call.PassRaw = "";
            call.Platform = parts[7];
            call.RunningLine = parts[8];
            current->Calls.push_back(call);
        }
        else if(parts[0] == "E" && current)
        {
            HashSemanticText(fingerprint, "E");
            BuildTimeline(*current);
            current = NULL;
        }
    }
    if(current) BuildTimeline(*current);
    if(CacheVersion == "")
    {
        ErrorMessage = "The selected file is not a multiplayer WTT cache.";
        Clear();
        return false;
    }
    CacheFingerprint = IntToHex(static_cast<int>(fingerprint), 8);
    ErrorMessage = "";
    return true;
}

bool TMultiplayerWTTCatalog::ServiceRunsOnDate(const TMultiplayerWTTService& Service, const TDateTime& Date) const
{
    TDateTime trafficDate = Date;
    unsigned short hour, minute, second, millisecond;
    DecodeTime(Date, hour, minute, second, millisecond);
    const int secondsNow = (hour * 3600) + (minute * 60) + second;
    int lastScheduled = -1;
    for(std::vector<TMultiplayerWTTCall>::const_iterator call = Service.Calls.begin(); call != Service.Calls.end(); ++call)
        if(call->ScheduledSeconds >= 0) lastScheduled = call->ScheduledSeconds;
    if(lastScheduled >= (24 * 3600) && secondsNow <= (lastScheduled - (24 * 3600)) + (2 * 3600))
        trafficDate = TDateTime(double(Date) - 1.0);

    TDateTime firstDate, lastDate;
    if(Service.DateRange.Length() >= 10)
    {
        if(ParseDateDMY(Service.DateRange.SubString(1, 10), firstDate) &&
           ParseDateDMY(Service.DateRange.SubString(Service.DateRange.Length() - 9, 10), lastDate))
        {
            const double dateOnly = static_cast<int>(double(trafficDate));
            if(dateOnly < double(firstDate) || dateOnly > double(lastDate)) return false;
        }
    }
    return RunningCodeAllowsDay(Service.RunningDays, DayOfWeek(trafficDate));
}

AnsiString TMultiplayerWTTCatalog::CurrentOwner(const TMultiplayerWTTService& Service, const TDateTime& LocalDateTime) const
{
    unsigned short hour, minute, second, millisecond;
    DecodeTime(LocalDateTime, hour, minute, second, millisecond);
    int secondsNow = (hour * 3600) + (minute * 60) + second;
    int firstScheduled = -1;
    int lastScheduled = -1;
    for(std::vector<TMultiplayerWTTCall>::const_iterator call = Service.Calls.begin(); call != Service.Calls.end(); ++call)
    {
        if(call->OwnerBox == "" || call->ScheduledSeconds < 0) continue;
        if(firstScheduled < 0) firstScheduled = call->ScheduledSeconds;
        lastScheduled = call->ScheduledSeconds;
    }
    if(firstScheduled < 0) return "";
    if(lastScheduled >= (24 * 3600) && secondsNow < (12 * 3600)) secondsNow += 24 * 3600;
    if(secondsNow < firstScheduled + Service.DelaySeconds - (10 * 60)) return "";
    if(lastScheduled >= 0 && secondsNow > lastScheduled + Service.DelaySeconds + (2 * 3600)) return "";
    if(Service.OperationalOwner != "") return Service.OperationalOwner;
    AnsiString lastOwner;
    for(std::vector<TMultiplayerWTTCall>::const_iterator call = Service.Calls.begin(); call != Service.Calls.end(); ++call)
    {
        if(call->OwnerBox != "" && call->ScheduledSeconds >= 0 && call->ScheduledSeconds + Service.DelaySeconds <= secondsNow) lastOwner = call->OwnerBox;
        if(call->OwnerBox != "" && call->ScheduledSeconds >= 0 && call->ScheduledSeconds + Service.DelaySeconds > secondsNow)
            return lastOwner == "" ? call->OwnerBox : lastOwner;
    }
    return lastOwner;
}

std::vector<int> TMultiplayerWTTCatalog::ServicesOwnedAt(const AnsiString& BoxCode, const TDateTime& LocalDateTime) const
{
    std::vector<int> result;
    for(unsigned int index = 0; index < Services.size(); ++index)
    {
        const TMultiplayerWTTService& service = Services[index];
        if(service.Cancelled || !ServiceRunsOnDate(service, LocalDateTime)) continue;
        if(CurrentOwner(service, LocalDateTime) == BoxCode.UpperCase()) result.push_back(index);
    }
    return result;
}

bool TMultiplayerWTTCatalog::SetCallSkipped(int ServiceIndex, int CallIndex, const AnsiString& RequestingBox)
{
    if(ServiceIndex < 0 || ServiceIndex >= static_cast<int>(Services.size())) return false;
    TMultiplayerWTTService& service = Services[ServiceIndex];
    if(CallIndex < 0 || CallIndex >= static_cast<int>(service.Calls.size())) return false;
    TMultiplayerWTTCall& call = service.Calls[CallIndex];
    if(call.OwnerBox == "" || call.OwnerBox != RequestingBox.UpperCase()) return false;
    call.Status = TMultiplayerWTTCall::Skipped;
    ++call.Revision;
    ++service.Revision;
    return true;
}

bool TMultiplayerWTTCatalog::SetCancelled(int ServiceIndex, bool IsCancelled)
{
    if(ServiceIndex < 0 || ServiceIndex >= static_cast<int>(Services.size())) return false;
    Services[ServiceIndex].Cancelled = IsCancelled;
    ++Services[ServiceIndex].Revision;
    return true;
}

int TMultiplayerWTTCatalog::FindService(const AnsiString& RunKey) const
{
    for(unsigned int index = 0; index < Services.size(); ++index)
        if(Services[index].RunKey == RunKey) return static_cast<int>(index);
    return -1;
}

int TMultiplayerWTTCatalog::FindCall(int ServiceIndex, const AnsiString& WTTLocation, int ScheduledSeconds) const
{
    if(ServiceIndex < 0 || ServiceIndex >= static_cast<int>(Services.size())) return -1;
    const TMultiplayerWTTService& service = Services[ServiceIndex];
    for(unsigned int index = 0; index < service.Calls.size(); ++index)
    {
        const TMultiplayerWTTCall& call = service.Calls[index];
        if(call.WTTLocation == WTTLocation &&
           (ScheduledSeconds < 0 || call.ScheduledSeconds == ScheduledSeconds)) return static_cast<int>(index);
    }
    return -1;
}

bool TMultiplayerWTTCatalog::ApplyRemoteState(const AnsiString& RunKey, int ServiceRevision, int NewDelaySeconds,
                                               bool IsCancelled, int CallIndex, int CallRevision, int CallStatus,
                                               const AnsiString& RequestingBox, const AnsiString& NewOperationalOwner)
{
    const int serviceIndex = FindService(RunKey);
    if(serviceIndex < 0) return false;
    TMultiplayerWTTService& service = Services[serviceIndex];
    bool changed = false;
    if(ServiceRevision > service.Revision)
    {
        service.DelaySeconds = std::max(0, NewDelaySeconds);
        service.Cancelled = IsCancelled;
        service.OperationalOwner = NewOperationalOwner.UpperCase();
        service.Revision = ServiceRevision;
        changed = true;
    }
    if(CallIndex >= 0 && CallIndex < static_cast<int>(service.Calls.size()))
    {
        TMultiplayerWTTCall& call = service.Calls[CallIndex];
        if(CallRevision > call.Revision && CallStatus >= TMultiplayerWTTCall::Planned &&
           CallStatus <= TMultiplayerWTTCall::Skipped)
        {
            // A stop may only be altered by the signalbox which owns that location.
            if(CallStatus != TMultiplayerWTTCall::Skipped || call.OwnerBox == RequestingBox.UpperCase())
            {
                call.Status = static_cast<TMultiplayerWTTCall::TCallStatus>(CallStatus);
                call.Revision = CallRevision;
                changed = true;
            }
        }
    }
    return changed;
}

bool TMultiplayerWTTCatalog::ReportHandover(const AnsiString& RunKey, const AnsiString& NewOwner,
                                             const TDateTime& SessionTime)
{
    const int serviceIndex = FindService(RunKey);
    if(serviceIndex < 0 || NewOwner.Trim() == "") return false;
    TMultiplayerWTTService& service = Services[serviceIndex];
    unsigned short hour, minute, second, millisecond;
    DecodeTime(SessionTime, hour, minute, second, millisecond);
    int secondsNow = (hour * 3600) + (minute * 60) + second;
    int scheduledAtOwner = -1;
    for(std::vector<TMultiplayerWTTCall>::const_iterator call = service.Calls.begin(); call != service.Calls.end(); ++call)
    {
        if(call->OwnerBox == NewOwner.UpperCase() && call->ScheduledSeconds >= 0)
        {
            scheduledAtOwner = call->ScheduledSeconds;
            break;
        }
    }
    if(scheduledAtOwner >= (24 * 3600) && secondsNow < (12 * 3600)) secondsNow += 24 * 3600;
    const int measuredDelay = scheduledAtOwner >= 0 ? std::max(0, secondsNow - scheduledAtOwner) : service.DelaySeconds;
    if(service.OperationalOwner == NewOwner.UpperCase() && service.DelaySeconds == measuredDelay) return false;
    service.OperationalOwner = NewOwner.UpperCase();
    service.DelaySeconds = measuredDelay;
    ++service.Revision;
    return true;
}
