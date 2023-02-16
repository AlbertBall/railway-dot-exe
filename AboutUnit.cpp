//AboutUnit.cpp
/*
      BEWARE OF COMMENTS in .cpp files:  they were accurate when written but have
      sometimes been overtaken by changes and not updated
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
#include <vcl.h>

#pragma hdrstop

#include "AboutUnit.h"
#include "InterfaceUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAboutForm *AboutForm;
//---------------------------------------------------------------------------
__fastcall TAboutForm::TAboutForm(TComponent* Owner) : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm::FormCreate(TObject *Sender)
{
    AboutForm->SetAboutCaption();
    AboutForm->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::AboutFormButtonClick(TObject *Sender)
{
    AboutForm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::FormClose(TObject *Sender,
                                      TCloseAction &Action)
{
    if(Interface->Level1Mode == TInterface::OperMode)
    {
        Interface->MasterClock->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::SetAboutCaption()
{
    DWORD VersionHandle;
    DWORD VersionSize;
    LPBYTE pBuffer;
    UnicodeString strVersion = L"N/A";
    UnicodeString NL         = '\n';

    VersionSize = GetFileVersionInfoSizeW(Application->ExeName.c_str(), &VersionHandle);
    if(VersionSize)
    {
        pBuffer = new BYTE[VersionSize];

        if(GetFileVersionInfoW(Application->ExeName.c_str(), VersionHandle, VersionSize, pBuffer))
        {
            VS_FIXEDFILEINFO *fi;
            UINT buflen;

            //uncomment strVersion and HIWORD alternates below when future CI implemented: sas@2.1.0
            if(VerQueryValueW(pBuffer, L"\\", (void** )&fi, &buflen))
            {
                //strVersion.sprintf(L"%d.%d.%d (Build %d)",
                strVersion.sprintf(L"%d.%d.%d",
                                   HIWORD(fi->dwFileVersionMS), LOWORD(fi->dwFileVersionMS),
                                   HIWORD(fi->dwFileVersionLS)
                                   //HIWORD(fi->dwFileVersionLS), LOWORD(fi->dwFileVersionLS)
                                   );
            }
        }
        delete[] pBuffer;
    }

    AboutLabelCaption->Caption = L"All the tools to design, build and operate your own railway" + NL + NL +
        L"Release: " + Interface->ProgramVersion + NL + NL +
        L"Copyright (c) 2010 - 2023 Albert Ball\nwith contributions from Stephen A Smith and Kristian Zarebski";
}
//---------------------------------------------------------------------------
void __fastcall TAboutForm::WebsiteLinkLabelLinkClick(TObject *Sender, const UnicodeString Link,
                                                      TSysLinkType LinkType)
{
    if(LinkType == sltURL)
    {
        ::ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm::Attribution1LinkLabelLinkClick(TObject *Sender, const UnicodeString Link,
                                                           TSysLinkType LinkType)
{
    if(LinkType == sltURL)
    {
        ::ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm::Attribution2LinkLabelLinkClick(TObject *Sender, const UnicodeString Link,
                                                           TSysLinkType LinkType)
{
    if(LinkType == sltURL)
    {
        ::ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm::Attribution3LinkLabelLinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType)
{
    if(LinkType == sltURL)
    {
        ::ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm::AttributionLinkLabel4LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType)
{
    if(LinkType == sltURL)
    {
        ::ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}
//---------------------------------------------------------------------------

void __fastcall TAboutForm::LinkLabel1LinkClick(TObject *Sender, const UnicodeString Link,
          TSysLinkType LinkType)
{
    if(LinkType == sltURL)
    {
        ::ShellExecute(Handle, NULL, Link.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}
//---------------------------------------------------------------------------

