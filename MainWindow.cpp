#include <Application.h>
#include <Box.h>
#include <MenuItem.h>
#include <stdio.h>
#include <Alert.h>
#include "MainWindow.h"
#include "board.h"
#include "fame.h"
#include "TPreferences.h"
 
const uint32 ID_FQUIT    = 101;
const uint32 ID_GUNDO    = 201;
const uint32 ID_GREDO    = 202;
const uint32 ID_GHOF     = 203;
const uint32 ID_GRESTART = 204;
//const uint32 ID_GNEW     = 205;
const uint32 ID_GHINT    = 206;
const uint32 ID_GISSOLVE = 207;
const uint32 ID_OSIZE1	 = 300;
const uint32 ID_OSIZE2	 = 301;
const uint32 ID_OSIZE3	 = 302;
const uint32 ID_OSIZE4	 = 303;
const uint32 ID_OSIZE5	 = 304;
const uint32 ID_OSPEED1	 = 306;
const uint32 ID_OSPEED2	 = 307;
const uint32 ID_OSPEED3	 = 308;
const uint32 ID_OSPEED4	 = 309;
const uint32 ID_OSPEED5	 = 310;
const uint32 ID_OLVL1    = 311;
const uint32 ID_OLVL2    = 312;
const uint32 ID_OLVL3	 = 313;
const uint32 ID_RULES    = 401;

static int size_x[5] = {14, 18, 24, 26, 30};
static int size_y[5] = { 6,  8, 12, 14, 16};
static int DELAY[5] = {125, 250, 500, 750, 1000};

TMainWindow::TMainWindow(BRect frame)
			: BWindow(frame, "Shisen-Sho", B_TITLED_WINDOW,  B_NOT_RESIZABLE | B_NOT_ZOOMABLE) {

  TPreferences prefs("Shisen-Sho_prefs");
  fSize  = prefs.GetInt32("Size",  ID_OSIZE2);
  fDelay = prefs.GetInt32("Speed",  ID_OSPEED3);
  fLevel = prefs.GetInt32("Level",  ID_OLVL2);
  cheat = FALSE;
  SetPulseRate(PULSETIME*1000);
  
  BRect r;
  r = Bounds();
  mb = new BMenuBar(r, "mb");
  BMenu *fm = new BMenu("File");
  mb->AddItem(fm);
  fm->AddItem(new BMenuItem("Quit", new BMessage(ID_FQUIT), 'Q'));
  BMenu *gm = new BMenu("Game");
  mb->AddItem(gm);
  gm->AddItem(new BMenuItem("Undo", new BMessage(ID_GUNDO), 'U'));
  gm->AddItem(new BMenuItem("Redo", new BMessage(ID_GREDO), 'R'));
  gm->AddSeparatorItem();
  gm->AddItem(new BMenuItem("Get hint", new BMessage(ID_GHINT), 'H'));
  gm->AddSeparatorItem();
  gm->AddItem(new BMenuItem("New game", new BMessage(ID_GNEW), 'N'));
  gm->AddItem(new BMenuItem("Restart game", new BMessage(ID_GRESTART), 'T'));
  gm->AddSeparatorItem();
  gm->AddItem(new BMenuItem("Is game solvable?", new BMessage(ID_GISSOLVE)));
  gm->AddSeparatorItem(); 
  gm->AddItem(new BMenuItem("Hall of Fame", new BMessage(ID_GHOF), 'F'));
  BMenu *om = new BMenu("Options");
  mb->AddItem(om);
  BMenu* om_s = new BMenu("Size");
  om_s->AddItem(new BMenuItem("14x6",  new BMessage(ID_OSIZE1)));
  om_s->AddItem(new BMenuItem("18x8",  new BMessage(ID_OSIZE2)));
  om_s->AddItem(new BMenuItem("24x12", new BMessage(ID_OSIZE3)));
  om_s->AddItem(new BMenuItem("26x14", new BMessage(ID_OSIZE4)));
  om_s->AddItem(new BMenuItem("30x16", new BMessage(ID_OSIZE5)));
  om_s->FindItem(fSize)->SetMarked(true);
  om_s->SetRadioMode(true);
  om->AddItem(om_s);
  BMenu* om_sp = new BMenu("Speed");
  om_sp->AddItem(new BMenuItem("Very fast", new BMessage(ID_OSPEED1)));
  om_sp->AddItem(new BMenuItem("Fast", new BMessage(ID_OSPEED2)));
  om_sp->AddItem(new BMenuItem("Medium", new BMessage(ID_OSPEED3)));
  om_sp->AddItem(new BMenuItem("Slow", new BMessage(ID_OSPEED4)));
  om_sp->AddItem(new BMenuItem("Very slow", new BMessage(ID_OSPEED5)));
  om_sp->FindItem(fDelay)->SetMarked(true);
  om_sp->SetRadioMode(true);
  om->AddItem(om_sp);
  BMenu* om_l = new BMenu("Level");
  om_l->AddItem(new BMenuItem("Easy", new BMessage(ID_OLVL1)));
  om_l->AddItem(new BMenuItem("Medium", new BMessage(ID_OLVL2)));
  om_l->AddItem(new BMenuItem("Hard", new BMessage(ID_OLVL3)));
  om_l->FindItem(fLevel)->SetMarked(true);
  om_l->SetRadioMode(true);
  om->AddItem(om_l);
  BMenu* help = new BMenu("Help");
  help->AddItem(new BMenuItem("Rules", new BMessage(ID_RULES)));
  help->AddItem(new BMenuItem("About", new BMessage(B_ABOUT_REQUESTED), 'B'));
  mb->AddItem(help);
  AddChild(mb);

  BBox* pStatusBox = new BBox(BRect(r.left-2, r.bottom-18, r.right+2, r.bottom+2), NULL, B_FOLLOW_LEFT_RIGHT|B_FOLLOW_BOTTOM);
  AddChild(pStatusBox);
  
  r.top += mb->Frame().Height()+1;
  r.bottom -= 18;
  AddChild(b = new Board(r));
  
  r = pStatusBox->Bounds();
  r.InsetBy(6,2);
  float x = r.right;
  r.right = 300;		
  StatusView = new BStringView(r, "", "", B_FOLLOW_ALL); 
  pStatusBox->AddChild(StatusView);
  r.right = x;
  r.left  = x-300;
  CheatView = new BStringView(r, "", "", B_FOLLOW_ALL); 
  CheatView->SetAlignment(B_ALIGN_RIGHT);
  pStatusBox->AddChild(CheatView);
 
  b->SetShuffle((fLevel - ID_OLVL1) * 4 + 1);
  PostMessage(fDelay); 
  PostMessage(fSize); 
}			

TMainWindow::~TMainWindow()
{
  TPreferences prefs("Shisen-Sho_prefs");
  prefs.SetInt32("Size", fSize);
  prefs.SetInt32("Speed", fDelay);
  prefs.SetInt32("Level", fLevel);
}

void TMainWindow::MessageReceived(BMessage *message) {
	switch(message->what) {
		case B_ABOUT_REQUESTED:
		       be_app->AboutRequested();
               break; 
        case ID_GNEW: {
               b->newGame();
               cheat = FALSE;
               CheatView->SetText("");
               }
               break;
        case ID_GRESTART: 
               while(b->canUndo()) b->undo();
               break;
        case ID_GUNDO:
               if(b->canUndo()) {
                 b->undo();
                 cheat = TRUE;
                 CheatView->SetText("Cheat mode");
               }
               break;
        case ID_GREDO:
               b->redo();
               break;
        case ID_GHINT: {
               b->getHint();
               cheat = TRUE;
               CheatView->SetText("Cheat mode");
               }
               break;
        case ID_GISSOLVE:
               if(b->solvable())
                (new BAlert("Information", "This game is solveable", "OK"))->Go(); 
               else
                (new BAlert("Information", "This game is NOT solveable", "OK"))->Go(); 
               break; 
        case ID_GHOF: {
	           FameWnd *bl = new FameWnd(this, -1, -1);
	           bl->MoveBy(Frame().left-50, Frame().top+30);    
	           bl->Show();
             }
             break;  
        case ID_OSIZE1:
        case ID_OSIZE2:
        case ID_OSIZE3:
        case ID_OSIZE4:
        case ID_OSIZE5:  {
               fSize = message->what;
	           b->setSize(size_x[fSize-ID_OSIZE1], size_y[fSize-ID_OSIZE1]);
	         }
	         break;
        case ID_OSPEED1:
        case ID_OSPEED2:
        case ID_OSPEED3:
        case ID_OSPEED4:
        case ID_OSPEED5: {
               fDelay = message->what;
               b->SetDelay(DELAY[fDelay-ID_OSPEED1]);
             }
             break;
        case ID_OLVL1:
        case ID_OLVL2:
        case ID_OLVL3: {
               fLevel = message->what;
               b->SetShuffle((fLevel - ID_OLVL1) * 4 + 1);
               PostMessage(ID_GNEW);
             }  
             break;
		case ENDOFGAME: {
               if(b->tilesLeft() > 0) {
                 (new BAlert("End of game", "No more moves possible!", "OK"))->Go(); 
                 PostMessage(ID_GNEW);
		       } else{     
			     int32 t;
			     message->FindInt32("Time", &t);
		         FameWnd *bl = new FameWnd(this, getScore(size_x[fSize-ID_OSIZE1], size_y[fSize-ID_OSIZE1],t),
		                                   t, size_x[fSize-ID_OSIZE1], size_y[fSize-ID_OSIZE1]);
		         bl->MoveBy(Frame().left-50, Frame().top+30);    
		         bl->Show();
		       }  
	         }  
			break;
		case ID_RULES: 
	       (new BAlert("Info", "Shisen-Sho is similiar to Mahjongg. "
	        "The object of the game is to remove all tiles from the field. Only two matching tiles can be removed at a time.\n"
	        "Two tiles can only be removed if they can be connected with at most three connected lines. "
	        "Lines can be horizontal or vertical but not diagonal. "
	        "You don't have to make the lines by yourself, the game does this for you. "
	        "Just mark two matching tiles on the board.\n"
	        "If they can be connected with at most three lines, the lines will be drawn and the tiles are removed.\n"
	        "Remember that lines may cross the empty border.\n\n"
	        "If you are stuck, you can use the Hint feature to find two tiles which may be removed."
	        "The game is over if no moves are possible and some tiles are left on the board.\n"
	        "If you use the features Undo, Hint or Redo, no highscore will be generated.",
	        "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_INFO_ALERT))->Go();
            break;	
		case ID_FQUIT:
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;
		case TIME:
		    int32 t;
            char c[40];
		    message->FindInt32("Time", &t);
            sprintf(c, "Your time: %02ld:%02ld:%02ld", t/3600, (t / 60)  % 60, t % 60);
			StatusView->SetText(c);
			break;
		default:
			BWindow::MessageReceived(message);
			break;
	}
} 

int TMainWindow::getScore(int x, int y, int t) {
  if (cheat) return 0;
  else {
    double ntiles = x*y;
    double tilespersec = ntiles/(double)t;
	  
	double sizebonus = sqrt(ntiles/(double)(14.0 * 6.0));
	double points = tilespersec / 0.14 * 100.0;
	return (int)(points * sizebonus);
  }	
}

bool TMainWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}
