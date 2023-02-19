//---------------------------------------------------------------------------

#ifndef PerfLogUnitH
#define PerfLogUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TPerfLogForm : public TForm
{
__published:	// IDE-managed Components
    TMemo *PerformanceLogBox;
    void __fastcall FormCreate(TObject *Sender);
///< the performance log displayed during operation
private:	// User declarations
public:		// User declarations
//variables
///< true when the 'show performance panel' button has been clicked during operation
//functions
    __fastcall TPerfLogForm(TComponent* Owner);
    ///<constructor
    void PerformanceLog(int Caller, AnsiString Statement);
    ///< Send Statement to the performance log on screen and to the file
};
//---------------------------------------------------------------------------
extern PACKAGE TPerfLogForm *PerfLogForm;
//---------------------------------------------------------------------------
#endif
