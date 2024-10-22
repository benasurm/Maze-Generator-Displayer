#include "RMGApp.h"
#include "AppFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(RMGApp);

bool RMGApp::OnInit()
{
	AppFrame* appFrame = new AppFrame("Random Maze Generator");
	appFrame->SetSize(1000, 800);
	appFrame->SetMinSize(wxSize(1000, 800));
	appFrame->SetMaxSize(wxSize(1000, 800));
	appFrame->SetBackgroundColour(wxColour(180, 180, 180));
	appFrame->Show();
	return true;
}
