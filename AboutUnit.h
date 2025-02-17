//AboutUnit.h
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
#include <windows.h>
#include <Vcl.Imaging.pngimage.hpp>            //needed for 64 bit compilation
//---------------------------------------------------------------------------
/// The small 'About' box selected from the Help menu
class TAboutForm : public TForm
{
__published:               // IDE-managed Components
    TButton * AboutFormButton; ///< The OK button
    TImage *ImageAppIcon;
    TLabel *AboutLabelCaption; ///< Version info displayed here
    TLinkLabel *WebsiteLinkLabel;
    TLinkLabel *Attribution1LinkLabel;
    TLinkLabel *Attribution2LinkLabel;
    TLabel *Attribution1Label;
    TLabel *Attribution2Label;
    TLabel *Attribution3Label;
    TLinkLabel *Attribution3LinkLabel;
    TLinkLabel *AttributionLinkLabel4;
/// Called when the form is first created (by WinMain). Sets the version info, then hides the form
    void __fastcall FormCreate(TObject *Sender);
/// Opens the About form
    void __fastcall AboutFormButtonClick(TObject *Sender);
/// Called when the About form is closed. Restarts MasterClock if Level1Mode is OperMode
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
/// Sends the link to your default browser
    void __fastcall WebsiteLinkLabelLinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType);
/// Send the attribution statement's link to your default browser
    void __fastcall Attribution1LinkLabelLinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType);
/// Send the attribution statement's link to your default browser
    void __fastcall Attribution2LinkLabelLinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType);
/// Send the attribution statement's link to your default browser
    void __fastcall Attribution3LinkLabelLinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType);
/// Send the attribution statement's link to your default browser
    void __fastcall AttributionLinkLabel4LinkClick(TObject *Sender, const UnicodeString Link, TSysLinkType LinkType);
    void __fastcall LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType);
private:    // User declarations
/// Sets version details from the project options 'Version Info' values
    void __fastcall SetAboutCaption();
public:     // User declarations
/// The form constructor
    __fastcall TAboutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------

extern PACKAGE TAboutForm *AboutForm;

//---------------------------------------------------------------------------
#endif
