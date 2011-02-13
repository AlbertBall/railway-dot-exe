//railway.cpp
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

#include "DisplayUnit.h"//for error detection
USERES("railway.res");
USEUNIT("TrainUnit.cpp");
USEUNIT("DisplayUnit.cpp");
USEUNIT("TextUnit.cpp");
USEUNIT("GraphicUnit.cpp");
USEUNIT("Utilities.cpp");
USEUNIT("TrackUnit.cpp");
USEFORM("InterfaceUnit.cpp", Interface);
USEFORM("AboutUnit.cpp", AboutForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) //the calling program for the application - controlled by the compiler
{
    try
        {
        Application->Initialize();
        Application->Title = "railway.exe";
        Application->HelpFile = "";
        Application->CreateForm(__classid(TInterface), &Interface);
         Application->CreateForm(__classid(TAboutForm), &AboutForm);
         Application->Run();
        }
    catch (Exception &exception)
        {
        Application->ShowException(&exception);
        }
    return 0;
}
//---------------------------------------------------------------------------
