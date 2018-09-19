#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Window.h>
#include <MenuBar.h>
#include <StringView.h>
#include "board.h"

class TMainWindow : public BWindow 
{
public:
  TMainWindow(BRect frame);
  ~TMainWindow();
  virtual void	MessageReceived(BMessage *message);
  virtual bool	QuitRequested();
private:
  int getScore(int x, int y, int t);
//  void sizeChanged();
  BMenuBar *mb;
  BStringView *StatusView, *CheatView;
  Board *b;
  bool cheat;
  int fSize, fDelay, fLevel;
};

#endif //MAINWINDOW_H
