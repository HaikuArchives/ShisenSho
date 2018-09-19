#include <Application.h>
#include <Button.h>
#include <stdlib.h>
#include <Roster.h>
#include <Resources.h>
#include <Bitmap.h>
#include "about.h"

const uint32 ABOUT_OK	= 'AbOK';
const rgb_color kDarkBorderColor = {184, 184, 184, 255};
const float kBorderWidth = 32.0;
 
TPicView::TPicView(BRect frame):BView(frame, "", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
} 

void TPicView::Draw(BRect updateRect)
{
  BRect r(Bounds());
  SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
  SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
  FillRect(r);
  
  r.right = kBorderWidth;
  SetHighColor(kDarkBorderColor);
  FillRect(r);
  
  BResources *rs;
  const void *picH;
  size_t      len;
  BBitmap *fBmp;
  
  rs = be_app->AppResources();
  if (rs->HasResource('ICON',101)) {
    picH = rs->LoadResource('ICON', 101, &len);
    fBmp = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
    fBmp->SetBits(picH, len, 0, B_CMAP8);
	SetDrawingMode(B_OP_OVER);
	DrawBitmap(fBmp, BPoint(15,6));
    delete fBmp;
  } 
}

TAboutView::TAboutView(BRect frame, const char* text)
      :BTextView(frame, "", BRect(0, 0, frame.right-frame.left, frame.bottom-frame.top), B_FOLLOW_ALL_SIDES)
{
  void * p;
  text_run_array *ra;
  BFont fo;
  uint32 sp;
  int32 i;
  const rgb_color LINKCOLOR = {0, 0, 220, 255};
  const rgb_color RED       = {0, 0, 0, 255};
  const float TITLEHEIGHT   = 16.0f;

  fText = new BString(text);
  fBis[0] = fText->FindFirst("\n");
  for (i=1; i<3; i++) {
    fVon[i] = fText->FindFirst("http://", fBis[i-1]);
    fBis[i] = fText->FindFirst("\n", fVon[i]);
  }  
  SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
  MakeEditable(false);
  MakeSelectable(false);
  SetStylable(true);
  SetText(text);
  GetFontAndColor(&fo, &sp);
  p = malloc(sizeof(text_run_array) + sizeof(text_run));
  ra = (text_run_array*)p; 
  ra->count = 1;
  ra->runs[0].offset = 0;
  ra->runs[0].font  = fo;
  ra->runs[0].color = LINKCOLOR;
  for (i=1; i<3; i++) SetRunArray(fVon[i], fBis[i], ra); 
  ra->runs[0].font.SetSize(TITLEHEIGHT);
  ra->runs[0].font.SetFace(B_BOLD_FACE);
  ra->runs[0].color = RED;
  SetRunArray(0, fBis[0], ra); 
  free(p);
}  

TAboutView::~TAboutView()
{
  delete fText;
}

void TAboutView::MouseDown(BPoint where)
{
  for (int32 i=1; i<3; i++)
    if (OffsetAt(where)>=fVon[i] && OffsetAt(where)<=fBis[i]) {
      Highlight(fVon[i], fBis[i]);
      char *x, c[40][1];
      fText->CopyInto(&c[0][0], fVon[i], fBis[i]-fVon[i]);
      x = &c[0][0];
      be_roster->Launch(B_URL_HTTP, 1, &x);
      Highlight(fVon[i], fBis[i]);
    }  
}
    
TAboutWindow::TAboutWindow(const char* text)
			 : BWindow(BRect(0,0,310,200), "About", B_MODAL_WINDOW, B_NOT_RESIZABLE) 
{
  BRect r;
  
  r = Bounds();	
  TPicView* v = new TPicView(r);
  AddChild(v); 
  BButton* pButton = new BButton(BRect((r.right-r.left)/2-30, r.bottom-30, (r.right-r.left)/2+30, r.bottom-10),
		"", "OK", new BMessage(ABOUT_OK), B_FOLLOW_BOTTOM);
  v->AddChild(pButton); 
  pButton->MakeDefault(true);
  r = v->Bounds(); 
  r.left += 55; r.right -= 10; r.top += 10; r.bottom -= 38.0;
  TAboutView* neu = new TAboutView(r, text);
  v->AddChild(neu);		  
}			 

void TAboutWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
	case ABOUT_OK: 
		Quit();
		break;
	default:
		BWindow::MessageReceived(message);
		break;
	}		
}
