#ifndef ABOUT_H
#define ABOUT_H

#include <Window.h>
#include <View.h>
#include <TextView.h>
#include <String.h>

class TAboutView : public BTextView
{
public:
  TAboutView(BRect frame, const char* text);
  ~TAboutView();
  virtual void MouseDown(BPoint where);
private:
  int32 fVon[3], fBis[3];
  BString* fText;
};

class TPicView: public BView{
public:
  TPicView(BRect frame);
  virtual void Draw(BRect updateRect);
};

class TAboutWindow : public BWindow 
{
public:
  TAboutWindow(const char* text);
  virtual void	MessageReceived(BMessage *message);
};

#endif //ABOUT_H
