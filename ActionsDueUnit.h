//---------------------------------------------------------------------------

#ifndef ActionsDueUnitH
#define ActionsDueUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TActionsDueForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *ActionsDuePanel;
    TLabel *ActionsDuePanelLabel;
    TListBox *ActionsDueListBox;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ActionsDueListBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall ActionsDueListBoxMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);

private:	// User declarations
public:		// User declarations

//variables
    bool ActionsDueListBoxRightMouseButtonDown;
//functions
    __fastcall TActionsDueForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TActionsDueForm *ActionsDueForm;
//---------------------------------------------------------------------------
#endif
