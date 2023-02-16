//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("AboutUnit.cpp", AboutForm);
USEFORM("PerfLogUnit.cpp", PerfLogForm);
USEFORM("InterfaceUnit.cpp", Interface);
USEFORM("ActionsDueUnit.cpp", ActionsDueForm);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    try
    {
        Application->Initialize();
        Application->MainFormOnTaskBar = true;
        Application->HelpFile = ".\\Help_Files\\Help.chm";
        /*NB CreateForm() runs the form's constructor, so if another form is referenced in it that form must be created first,
        e.g. Interface before AboutForm (although InterfaceUnit.cpp includes AboutUnit.h the constructor doesn't reference it). */
         Application->CreateForm(__classid(TInterface), &Interface);
         Application->CreateForm(__classid(TPerfLogForm), &PerfLogForm);
         Application->CreateForm(__classid(TAboutForm), &AboutForm);
         Application->CreateForm(__classid(TActionsDueForm), &ActionsDueForm);
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
