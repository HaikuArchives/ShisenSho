#ifndef APP_H
#define APP_H

#include <Window.h>
#include <Application.h>
#include "MainWindow.h"

extern const char *app_sign;

class TApp : public BApplication 
{
private:
  TMainWindow *mainwindow;
public:
  TApp();
  virtual void	ReadyToRun();
  virtual void AboutRequested();
};

#endif //APP_H
