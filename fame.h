#ifndef FAME_H
#define FAME_H

#include <Window.h>
#include <StringView.h>
#include <TextControl.h>
#include <String.h>

const int32 FAMEANZ = 10;
const uint32 DLG_OK	= 'mDOK';
const uint32 FAME_RESET	= 'FamR';
const uint32 ID_GNEW     = 205;

class FameWnd : public BWindow 
{
private:
  BWindow *fOwner;
  int32 anzahl, mark, endpts;
  int punkte[10];
  int tim[10];
  int datum[10];
  BString namen[10];
  BString size[10];
  BTextControl *pEdName;
public:
  FameWnd(BWindow* Owner, int apts=-1, int ish=-1, int x=-1, int y=-1);
  void MessageReceived(BMessage* message);
  void ReadScore();
  void WriteScore();
  void AddScore(int apts, int ish, int x, int y);
};

#endif //FAME_H

