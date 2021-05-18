//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("AboutUnit.cpp", AboutForm);
USEFORM("InterfaceUnit.cpp", Interface);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = true;
        Application->HelpFile = ".\\Help_Files\\Help.chm";
        Application->CreateForm(__classid(TInterface), &Interface);
        Application->CreateForm(__classid(TAboutForm), &AboutForm);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    catch (...)
    {
        try
        {
            throw Exception("");
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }
    }
    return(0);
}
//---------------------------------------------------------------------------
