#include <Alert.h>
#include "Main.h"
#include "MainWindow.h"
#include "about.h"

int main(int, char**) 
{	
	new TApp();
	be_app->Run();
	delete be_app;
	return(0);
}

const char *app_sign = "application/x-vnd.JRW-Shisen";

TApp::TApp():BApplication(app_sign)
{
}
  
void TApp::ReadyToRun()
{
  mainwindow = new TMainWindow(BRect(100, 100, 699, 514));
  mainwindow->Show();
}

void TApp::AboutRequested()
{
  TAboutWindow *bl;
  bl = new TAboutWindow( 
             "Shisen-Sho\n\nHentrich Software\nhttp://www.nettoeinkommen.de\nhttp://www.hentrich-software.de/be\n\nVersion 0.9"
             "\n\nported from KSHISEN (Linux-KDE)");
  bl->MoveBy(300,200);    
  bl->Show();
}
