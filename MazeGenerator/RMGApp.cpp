#include "RMGApp.h"
#include "AppFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(RMGApp);

bool RMGApp::OnInit()
{
	AppFrame* appFrame = new AppFrame("Random Maze Generator");
	appFrame->SetSize(750, 500);
	appFrame->SetMinSize(wxSize(750, 500));
	appFrame->SetMaxSize(wxSize(750, 500));
	appFrame->SetBackgroundColour(wxColour(180, 180, 180));
	appFrame->Show();
	return true;
}
