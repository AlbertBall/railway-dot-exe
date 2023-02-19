//---------------------------------------------------------------------------
#ifndef CallerCheckerUnitH
#define CallerCheckerUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <fstream>

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:    // IDE-managed Components
    TLabel *Label1;
    TButton *StartAnalysis;
    TButton *Exit;
    TLabel *Title;
    TRadioButton *RadioButton1;
    TRadioButton *RadioButton2;
    TRadioButton *RadioButton3;
    TTimer *Timer1;
    TCheckBox *CheckBox1;
    void __fastcall StartAnalysisClick(TObject *Sender);
    void __fastcall ExitClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
private:    // User declarations
    bool Screen, Print, ClockStopped, BreakFlag;
    int Count;
    int DupNumArray[5000];
    int MissingNumArray[5000];

    static const int NumFiles = 14;
    static const int NumberOfCalls = 511;
    AnsiString FileString[NumFiles];
    AnsiString FunctionString[NumberOfCalls];

    std::ofstream OutFile;

    void AnalyseOneFunction(int &FunctionCount, int NumberOfCalls, AnsiString FunctionString, std::ofstream &OutFile);
public:     // User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
