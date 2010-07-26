//AboutUnit.h
//Comments in .h files are believed to be accurate and up to date
/*
This is a source code file for "railway.exe", a railway operation
simulator, written in Borland C++ Builder 4 Professional
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
//---------------------------------------------------------------------------
#ifndef AboutUnitH
#define AboutUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TAboutForm : public TForm //the small 'About' box selected from the Help menu
{
__published:	// IDE-managed Components
    TButton *AboutFormButton; //the OK button
    TImage *Image1; //the railway.exe icon
    TLabel *AboutLabel; //the label that begins 'All the tools.....'
    TLabel *AboutLabel2; //the 'railway.exe' label in bold
    void __fastcall FormCreate(TObject *Sender); //called when the form is first created (by WinMain), the function sets AboutLabel
        //then hides the form
    void __fastcall AboutFormButtonClick(TObject *Sender); //closes the form
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action); //called when the form is closed, restarts MasterClock if Level1Mode
        //is OperMode
private:	// User declarations
public:		// User declarations
    __fastcall TAboutForm(TComponent* Owner); //the form constructor
};
//---------------------------------------------------------------------------

extern PACKAGE TAboutForm *AboutForm;

//---------------------------------------------------------------------------
#endif
