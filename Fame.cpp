#include <Application.h>
#include <Alert.h>
#include <Button.h>
#include <GraphicsDefs.h>
#include <Path.h>
#include <OS.h>
#include <stdio.h>
#include <time.h>
#include <View.h>
#include "fame.h"
#include "TPreferences.h"

const int32 RL[6] = {10, 42,  82, 150, 200, 322};
const int32 RR[6] = {40, 80, 142, 198, 320, 382};
const alignment AL[6] = {B_ALIGN_CENTER, B_ALIGN_RIGHT, B_ALIGN_RIGHT, B_ALIGN_LEFT, B_ALIGN_LEFT, B_ALIGN_LEFT};
const char* CA[6] = {"Rank", "Score", "Time", "Size", "Player", "Date"};

FameWnd::FameWnd(BWindow* Owner, int apts/*=-1*/, int ish/*=-1*/, int x/*=-1*/, int y/*=-1*/)
			: BWindow(BRect(00, 00, 390, 210), "Shisen-Fame", B_MODAL_WINDOW, B_NOT_RESIZABLE) {
			
  fOwner = Owner;
  
  BRect r;
  BStringView *la;	
  BButton *pButton;
  char s[40];
  
  endpts = apts;
  r = Bounds();
  BView *b = new BView(r, NULL, B_FOLLOW_NONE, B_WILL_DRAW);
  b->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
  AddChild(b);
  
  ReadScore();
  mark = -2;
  if (apts >= 0) AddScore(apts, ish, x, y);
  for (int cols=0; cols<6; cols++) {
    for (int i=0; i<11; i++) {
      r.Set(RL[cols], 10+15*i, RR[cols], 23+15*i);
      if (i==0) sprintf(s, "%s", CA[cols] );
      else {
        sprintf(s, "%s", "" );
        if (cols==0) sprintf(s, "%d", i );
		else if (i-1<anzahl) {
		  if (cols==1) sprintf(s, "%d", punkte[i-1]);
		  if (cols==2) sprintf(s, "%02d:%02d:%02d", tim[i-1]/3600, (tim[i-1] / 60)  % 60, tim[i-1] % 60);
		  if (cols==3) sprintf(s, "%s", size[i-1].String());
          if (cols==4) sprintf(s, "%s", namen[i-1].String());
		  if (cols==5) {
		        if (datum[i-1] != 0) {
		          time_t t=datum[i-1];
		          struct tm *dt;
		          dt = localtime(&t);
		          sprintf(s, "%d-%.2d-%.2d", dt->tm_year+1900, dt->tm_mon+1, dt->tm_mday);
		        }  
	      }
        }
      }  				
	  if ((mark == i-1) && (cols==4)) {
		r.OffsetBy(-2, -2);
		pEdName = new BTextControl(r, NULL, NULL, "", NULL);
		b->AddChild(pEdName);
      }	else { 
	    la = new BStringView(r, NULL, s, B_FOLLOW_NONE); 
	    la->SetAlignment(AL[cols]);
	    b->AddChild(la);
	  }  
    }	
  }	
	
  r = Bounds();
  r.bottom = r.bottom-30;
  pButton = new BButton(BRect(r.right/2-30, r.bottom+1, r.right/2+30, r.bottom+21),
		"", "OK", new BMessage(DLG_OK), B_FOLLOW_BOTTOM);
  b->AddChild(pButton);
  pButton->MakeDefault(true);
  if (mark > -2) pEdName->MakeFocus();
  else {
    pButton->MakeFocus();
    strcpy(s, "Reset");
    pButton = new BButton(BRect(r.left+10, r.bottom+3, r.left+70, r.bottom+23),
		"", s, new BMessage(FAME_RESET), B_FOLLOW_BOTTOM);
    b->AddChild(pButton);
  }  
}			

void FameWnd::MessageReceived(BMessage* message)
{
	switch (message->what) {
	case DLG_OK: {
          if (mark > -2) {
            namen[mark].SetTo(pEdName->Text());
            WriteScore();
          }  
	      Hide();
          if (endpts>-1) fOwner->PostMessage(ID_GNEW);     
		}
		break;
	case FAME_RESET: { 
		  anzahl = 0;
		  WriteScore();
		  Quit();
		}
		break;
	}
}

void FameWnd::ReadScore()
{
  char s[40]; 
     
  TPreferences prefs("Shisen-Sho_prefs");
  anzahl = prefs.GetInt32("Anzahl", 0);
  for (int i=0; i<anzahl; i++) {
    sprintf(s, "P%d", i);
    punkte[i] = prefs.GetInt32(s,  0);
    sprintf(s, "I%d", i);
    tim[i] = prefs.GetInt32(s,  0);
    sprintf(s, "D%d", i);
    datum[i] = prefs.GetInt32(s,  0);
    sprintf(s, "Z%d", i);
    if (prefs.FindString(s, &size[i]) != B_OK) size[i] << "";
    sprintf(s, "N%d", i);
    if (prefs.FindString(s, &namen[i]) != B_OK) namen[i] << "";
  }
}

void FameWnd::WriteScore()
{
  char s[40]; 
  
  TPreferences prefs("Shisen-Sho_prefs");
  prefs.SetInt32("Anzahl", anzahl);
  for (int i=0; i<anzahl; i++) {
    sprintf(s, "P%d", i);
    prefs.SetInt32(s,  punkte[i]);
    sprintf(s, "I%d", i);
    prefs.SetInt32(s,  tim[i]);
    sprintf(s, "D%d", i);
    prefs.SetInt32(s,  datum[i]);
    sprintf(s, "Z%d", i);
    prefs.SetString(s,  size[i].String());
    sprintf(s, "N%d", i);
    prefs.SetString(s,  namen[i].String());
  }
}

void FameWnd::AddScore(int apts, int ish, int x, int y)
{
  for (int i=anzahl-1; i>-1; i--) {
    if (apts>punkte[i]) {
      if (i!=(FAMEANZ-1)) {
        punkte[i+1] = punkte[i];
        tim[i+1]    = tim[i];
        namen[i+1]  = namen[i];
        datum[i+1]  = datum[i];
        size[i+1]   = size[i];
      };
      mark = i;
    };
  };
  if (mark==-2 && anzahl<FAMEANZ) mark=anzahl;
  if (mark>-2) {
    punkte[mark] = apts;
    tim[mark] = ish;
    datum[mark]  = real_time_clock();
    size[mark] = "";
    size[mark] << x << "x" << y;
    if (anzahl<FAMEANZ) anzahl++;
  };
}



