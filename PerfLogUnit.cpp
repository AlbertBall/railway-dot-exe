//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PerfLogUnit.h"
#include "Utilities.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPerfLogForm *PerfLogForm;
//---------------------------------------------------------------------------
__fastcall TPerfLogForm::TPerfLogForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPerfLogForm::FormCreate(TObject *Sender)
{
    PerfLogForm->Visible = false;
    PerfLogForm->Top = 0; //modified when displayed
    PerfLogForm->Left = 0;
    PerfLogForm->Width = 476;
    PerfLogForm->Height = 296;
    PerformanceLogBox->Top = 0;
    PerformanceLogBox->Left = 0;
    PerformanceLogBox->Width = PerfLogForm->Width - 8; //form has to be 8 wider than content to display content correctly
    PerformanceLogBox->Height = PerfLogForm->Height - 33; //to allow for title bar
}
//---------------------------------------------------------------------------

void TPerfLogForm::PerformanceLog(int Caller, AnsiString Statement)
{
    Utilities->CallLog.push_back(Utilities->TimeStamp() + "," + AnsiString(Caller) + ",PerformanceLog," + Statement);
    Utilities->PerformanceFile << Statement.c_str() << '\n';
    Utilities->PerformanceFile.flush(); //added at v2.13.0
    PerformanceLogBox->Lines->Add(Statement);
    Utilities->CallLogPop(1479);
}

// ---------------------------------------------------------------------------


