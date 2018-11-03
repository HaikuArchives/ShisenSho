/* BeOS-Port of KSHISEN
 *******************************************************************
 *******************************************************************
 *
 *
 * KSHISEN
 *
 *
 *******************************************************************
 *
 * A japanese game similar to mahjongg
 *
 *******************************************************************
 *
 * created 1997 by Mario Weilguni <mweilguni@sime.com>
 *
 *******************************************************************
 *
 * This file is part of the KDE project "KSHISEN"
 *
 * KSHISEN is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KSHISEN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KSHISEN; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *******************************************************************
 */
#include <Window.h>
#include <Screen.h>
#include <TranslationUtils.h>
#include <Roster.h>
#include "board.h"
#include <stdlib.h>
#include <sys/times.h>
#include <stdio.h>
#include <cstring>
#include <Alert.h>

const int EMPTY	= 0;
const int XBORDER	 = 20;
const int YBORDER	 = 20;
const int DEFAULTDELAY	 = 500;
const int DEFAULTSHUFFLE = 4;
const int FDX = 40;
const int FDY = 56;

Board::Board(BRect frame):BView(frame, "", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_PULSE_NEEDED)
{
  struct tms dummy;
  clock_t t = times(&dummy); 
  srandom((int)t);

  starttime  = time((time_t *)0); 
  fPulseTime = 0;
  fField  = 0; 
  fMarked = 0;
  
  for(int i = 0; i < 45; i++)  pm_tile[i] = 0;
  pm  = BTranslationUtils::GetBitmap('PNG ', 2);
}

Board::~Board() {
  delete pm;
  clearDoList(_undo);
  clearDoList(_redo);
  if(fField != 0) free(fField);
  if(fMarked != 0) free(fMarked);
}

void Board::AttachedToWindow() {
  BBitmap* bg  = BTranslationUtils::GetBitmap('PNG ', 1);
  SetViewBitmap(bg);
  delete bg;
}

bool Board::loadTiles(float scale) {
  int i, j;
  BView *v;
  BBitmap* bm;

  // delete old tiles
  for(i = 0; i < 45; i++)
    if(pm_tile[i] != 0) {
      delete pm_tile[i];
      pm_tile[i] = 0;
    } 
    
  BRect src(0, 0, FDX-1, FDY-1);
  fDx = FDX;
  fDy = FDY;
  BRect r(0, 0, fDx-1, fDy-1);
  if(scale != 1.0) {
    fDx = (int)((r.right+1)*scale);
    fDy = (int)((r.bottom+1)*scale);
    r.right  = fDx-1.0;
    r.bottom = fDy-1.0;
  }
  v = new BView(r, NULL, 0, 0);

  for(i = 0; i < 9; i++)
    for(j = 0; j < 5; j++) {
	  bm = new BBitmap(r, B_RGB32, true);
	  bm->Lock();
      bm->AddChild(v);
      v->DrawBitmap(pm, src.OffsetToCopy(i*FDX, j*FDY), r);
      bm->RemoveChild(v);
      bm->Unlock();
      pm_tile[i + j*9] = bm;
    }
  delete v;
  return TRUE;
} 

void Board::newGame() {
  int i, x, y, k;

  mark_x = -1;
  mark_y = -1;

  clearDoList(_undo);
  clearDoList(_redo);
  clearHistory();

  // distribute all tiles on board
  int cur_tile = 0;
  
  for(i = 0; i < fSizeX * fSizeY * 12; i++) {
    // map the tileindex to a tile
    // not all tiles from the pixmap are really used, only
    // 36 out of 45 are used. This maps and index to the "real" index.
    int tile;
    if (cur_tile == 28)
      tile = 30;
    else if (cur_tile >= 29 && cur_tile <= 35)
      tile = cur_tile + 7;
    else
      tile = cur_tile;
    
    cur_tile++;
    if(cur_tile == 36) cur_tile = 0;

    x = i % fSizeX;
    y = i / fSizeX * 4;

    tile++;
    for(k = 0; k < 4 && k + y < fSizeY; k++) SetField(x, y+k, tile);
  }

  // shuffle the field
  for(i = 0; i < fSizeX * fSizeY * fShuffle; i++) {
    int x1 = random() % fSizeX;;
    int y1 = random() % fSizeY;
    int x2 = random() % fSizeX;;
    int y2 = random() % fSizeY;
    int t  = GetField(x1, y1);
    SetField(x1, y1, GetField(x2, y2));
    SetField(x2, y2, t);
  } 

  int xy = fSizeX * fSizeY;
  int *oldfield = new int[xy];
  int *tiles = new int[xy];
  int *pos = new int[xy];
  memcpy(oldfield, fField, fSizeI);			// save field

  while(!solvable(TRUE)) {
    // generate a list of free tiles and positions
    int num_tiles = 0;
    for(i = 0; i < fSizeX * fSizeY; i++)
      if(fField[i] != EMPTY) {
	    pos[num_tiles] = i;
	    tiles[num_tiles] = fField[i];
	    num_tiles++;
    }

    // restore field
    memcpy(fField, oldfield, fSizeI);
    
    // redistribute unsolved tiles
    while(num_tiles > 0) {
      // get a random tile
      int r1 = random() % num_tiles;
      int r2 = random() % num_tiles;
      int tile = tiles[r1];
      int apos = pos[r2];
      
      // truncate list
      num_tiles--;
      tiles[r1] = tiles[num_tiles];
      pos[r2] = pos[num_tiles];

      // put this tile on the new position
      fField[apos] = tile;
    }

    // remember field
    memcpy(oldfield, fField, fSizeI);
  }

  // restore field
  memcpy(fField, oldfield, fSizeI);  
  delete tiles;
  delete pos;
  delete oldfield;

  starttime = time((time_t *)0);
  Invalidate();
}

void Board::setSize(int x, int y) 
{
  if(x == fSizeX && y == fSizeY)    return;
  fSizeX = x;
  fSizeY = y;
  fSizeI = sizeof(int) * x * y;

  if(fField != 0) free(fField);
  fField = (int*)malloc(fSizeI+10); 
  if(fMarked != 0) free(fMarked);
  fMarked = (bool*)malloc(sizeof(bool) *x * y);
  for(int i = 0; i < x; i++)
    for(int j = 0; j < y; j++) {
      SetField(i, j, EMPTY);
      SetMarked(i, j, false);
    }  

  double scaler = 1.0;
  while (((int)(40*scaler)*x+2*XBORDER-1.0 > BScreen(Window()).Frame().Width()) ||
         ((int)(56*scaler)*y+2*YBORDER+38.0 > BScreen(Window()).Frame().Height())) {
   scaler -= 0.2;
  }  
  loadTiles(scaler);
  Window()->ResizeTo((int)(40*scaler)*x+2*XBORDER-1.0, (int)(56*scaler)*y+2*YBORDER+38.0);
  newGame();
}

void Board::SetField(int x, int y, int value) 
{
  if(x < fSizeX && y < fSizeY) fField[y * fSizeX + x] = value;
}

int Board::GetField(int x, int y) 
{
  if(x <= -1 || x >= fSizeX || y <= -1 || y >= fSizeY) return EMPTY;
  else return fField[y * fSizeX + x];
}

BBitmap *Board::lighten(BBitmap *src) 
{
  BRect r(0, 0, fDx-1, fDy-1);
  BView *v = new BView(r, NULL, 0, 0);
  BBitmap *bm = new BBitmap(r, B_RGB32, true);
  bm->Lock();
  bm->AddChild(v);
  v->SetHighColor(50, 50, 50);
  v->FillRect(r);
  v->SetDrawingMode(B_OP_ADD);
  v->DrawBitmap(src, r);
  bm->RemoveChild(v);
  bm->Unlock();
  delete v;
  return bm;
}

void Board::SetMarked(int x, int y, bool value) 
{
  if(x>-1 && x < fSizeX && y>-1 && y < fSizeY) fMarked[y * fSizeX + x] = value;
}

bool Board::GetMarked(int x, int y) {
  if(x <= -1 || x >= fSizeX || y <= -1 || y >= fSizeY || fMarked==0) return false;
   else return fMarked[y * fSizeX + x];
}

void Board::ChangeMarked(int i, int j, bool mark)
{
  if (mark != GetMarked(i, j)) {
    SetMarked(i, j, mark);
    if (mark) {
      BRect r(XBORDER + i * fDx, YBORDER + j * fDy, XBORDER + (i+1) * fDx-1, YBORDER + (j+1) * fDy-1);
      Draw(r);
    }  
    else updateField(i, j); 
  }
}

void Board::updateField(int x, int y) 
{
  BRect r(XBORDER + x * fDx, YBORDER + y * fDy, XBORDER + (x+1) * fDx-1, YBORDER + (y+1) * fDy-1);
  Invalidate(r);
  Flush();
}

void Board::Draw(BRect updateRect)
{
  for(int i = 0; i < fSizeX; i++)
    for(int j = 0; j < fSizeY; j++) {
      if(GetField(i, j) == EMPTY) continue;
      float xpos = XBORDER + i * fDx;
      float ypos = YBORDER + j * fDy;
      BRect r(xpos, ypos, xpos+fDx-1, ypos+fDy-1);
      if(updateRect.Intersects(r)) {
        SetDrawingMode(B_OP_MAX);
        // check if it is a marked piece
        if (GetMarked(i, j)) {
          BBitmap *lpm = lighten(pm_tile[GetField(i, j)-1]);
          DrawBitmap(lpm, r);
          delete lpm;
        }  
        else DrawBitmap(pm_tile[GetField(i, j)-1], r);
        SetDrawingMode(B_OP_COPY);
      }  
  }	
}  

void Board::MouseDown(BPoint point) 
{
  uint32 buttons;
  BPoint cursor;
  
  GetMouse(&cursor, &buttons, true);
	
  // calculate position
  int x, y;
  x = (int)point.x;
  int pos_x = (x - XBORDER <0) ? -1 : (x - XBORDER) / (int)fDx;
  y = (int)point.y;
  int pos_y = (y - YBORDER <0) ? -1:  (y - YBORDER) / (int)fDy;
                                        
  // Mark tile
  if (buttons & B_PRIMARY_MOUSE_BUTTON) { 
	for(int i = 0; i < fSizeX; i++)
	  for(int j = 0; j < fSizeY; j++) 
	    if (i != mark_x && j != mark_y && GetMarked(i, j)) ChangeMarked(i, j, false); 
    if(pos_x >= 0 && pos_x < fSizeX && pos_y >= 0 && pos_y < fSizeY) Marked(pos_x, pos_y);  
  }
  // Assist by lighting all tiles of same type
  bool b;
  if (buttons & B_SECONDARY_MOUSE_BUTTON) { 
    int field = GetField(pos_x,pos_y);
    if (field != EMPTY) {
      mark_x = -1;
      mark_y = -1;
      for(int i = 0; i < fSizeX; i++)
        for(int j = 0; j < fSizeY; j++) {
          b = field==GetField(i, j);
          if (b != GetMarked(i, j))  { 
	        SetMarked(i, j, b);
	        updateField(i, j);
	      }  
        }
    }   
  }
}

void Board::Marked(int x, int y) {
  // make sure that the last arrow is correctly undrawn
  undrawArrow();

  if(GetField(x, y) == EMPTY) return;
  
  if(mark_x == -1) { // first piece
    ChangeMarked(mark_x=x, mark_y=y, true);
    return;
  }  

  if(x == mark_x && y == mark_y) {  // unmark the piece
    mark_x = -1;
    mark_y = -1;
    ChangeMarked(x, y, false);
    return;
  }

  if(GetField(mark_x, mark_y) != GetField(x, y)) {  // different pictures
    ChangeMarked(mark_x, mark_y, false);
    ChangeMarked(mark_x=x, mark_y=y, true);
    return;
  }
    
  // trace    
  if(findPath(mark_x, mark_y, x, y)) {
    madeMove(mark_x, mark_y, x, y);
    drawArrow(mark_x, mark_y, x, y);
    SetField(mark_x, mark_y, EMPTY); 
    SetField(x, y, EMPTY);           
    mark_x = -1;
    mark_y = -1;

    int dummyx;
    History dummyh[4];

    // game is over?      
    if(!getHint_I(dummyx,dummyx,dummyx,dummyx,dummyh)) {
      time_for_game = (int)time((time_t)NULL) - starttime;
      BMessage messy(ENDOFGAME);
      messy.AddInt32("Time", time_for_game);
      this->Window()->PostMessage(&messy);
    }
      
  } else {
    clearHistory();
  }
}

void Board::drawArrow(int x1, int y1, int x2, int y2) 
{
  // find out number of array
  int num = 0;
  while(num < 4 && history[num].x != -2) num++;

  // lighten the fields
  ChangeMarked(x1, y1, true);
  ChangeMarked(x2, y2, true);
  
  // restore the mark
  int mx = mark_x, my = mark_y;
  mark_x = mx;
  mark_y = my;
  num = 0;
  SetHighColor(255, 0, 0);
  SetPenSize(6);
  while(num < 3 && history[num+1].x != -2) {
    StrokeLine(midCoord(history[num].x, history[num].y), 
               midCoord(history[num+1].x, history[num+1].y));
    num++;    
  } 
  Flush();
  fPulseAnim = fDelay; 
}

bool Board::canMakePath(int x1, int y1, int x2, int y2) {
  int i;

  if(x1 == x2) {
    for(i = MIN(y1, y2)+1; i < MAX(y1, y2); i++) 
      if(GetField(x1, i) != EMPTY) return FALSE;
    return TRUE;
  } 
  if(y1 == y2) {
    for(i = MIN(x1, x2)+1; i < MAX(x1, x2); i++)
      if(GetField(i, y1) != EMPTY) return FALSE;
    return TRUE;
  }
  return FALSE;
}

bool Board::findPath(int x1, int y1, int x2, int y2) {
 clearHistory();

  if(findSimplePath(x1, y1, x2, y2))
     return TRUE;
  else {
    // find 3-way path
    int dx[4] = {1, 0, -1, 0};
    int dy[4] = {0, 1, 0, -1};
    int i;

    for(i = 0; i < 4; i++) {
      int newx = x1 + dx[i], newy = y1 + dy[i];
      while(GetField(newx, newy) == EMPTY && 
	        newx >= -1 && newx <= fSizeX &&
	        newy >= -1 && newy <= fSizeY) {
	    if(findSimplePath(newx, newy, x2, y2)) {
	      // make place for history point
	      for(int j = 3; j > 0; j--)  history[j] = history[j-1]; 
	      // insert history point
	      history[0].x = x1;
	      history[0].y = y1;
	      return TRUE;	 
	    }

	    newx += dx[i];
	    newy += dy[i];
      }
    }

    clearHistory();
    return FALSE;
  }

  return FALSE;
}

bool Board::findSimplePath(int x1, int y1, int x2, int y2) {
  bool r = FALSE;

  // find direct line
  if(canMakePath(x1, y1, x2, y2)) {
    history[0].x = x1;
    history[0].y = y1;
    history[1].x = x2;
    history[1].y = y2;
    r = TRUE;
  } else {
    if(!(x1 == x2 || y1 == y2)) // requires complex path
      if(GetField(x2, y1) == EMPTY && canMakePath(x1, y1, x2, y1) && canMakePath(x2, y1, x2, y2)) {
        history[0].x = x1;
	    history[0].y = y1;
	    history[1].x = x2;
    	history[1].y = y1;
	    history[2].x = x2;
	    history[2].y = y2;
	    r = TRUE;
      } else if(GetField(x1, y2) == EMPTY && canMakePath(x1, y1, x1, y2) && canMakePath(x1, y2, x2, y2)) {
	    history[0].x = x1;
	    history[0].y = y1;
	    history[1].x = x1;
	    history[1].y = y2;
	    history[2].x = x2;
	    history[2].y = y2;
	    r = TRUE;
     }
  }
  return r;
}

void Board::undrawArrow() {
  // is already undrawn?
  if(history[0].x == -2) return;
  // redraw all affected fields
  int num = 0;
  while(num < 3 && history[num+1].x != -2) {
    if(history[num].y == history[num+1].y)
      for(int i = MIN(history[num].x, history[num+1].x); i <= MAX(history[num].x, history[num+1].x); i++) {
        SetMarked(i, history[num].y, false);
        updateField(i, history[num].y);
      } 
    else 
      for(int i = MIN(history[num].y, history[num+1].y); i <= MAX(history[num].y, history[num+1].y); i++) {
        SetMarked(history[num].x, i, false);
        updateField(history[num].x, i);
      } 
    num++;
  }
  clearHistory();
}

BPoint Board::midCoord(int x, int y) {
  BPoint p;
  float w = fDx;
  float h = fDy;

  if(x == -1) 
    p.x = (XBORDER/2 - w/2);
  else if(x == fSizeX)
    p.x = (XBORDER/2 + w * fSizeX);
  else 
    p.x = (XBORDER + w * x);

  if(y == -1) 
    p.y = (YBORDER/2 - h/2);
  else if(y == fSizeY)
    p.y = (YBORDER/2 + h * fSizeY-5);
  else 
    p.y = (YBORDER + h * y);

  p.Set(p.x + w/2, p.y + h/2);
  return p;
} 

void Board::SetDelay(int newvalue) {
  fDelay = newvalue;
} 

void Board::madeMove(int x1, int y1, int x2, int y2) {
  Move *m = new Move(x1, y1, x2, y2, GetField(x1, y1));
  _undo.AddItem(m);
  clearDoList(_redo);
}

bool Board::canUndo() {
  return !_undo.IsEmpty();
}  
  
void Board::undo() {
  if(canUndo()) {
    undrawArrow();
    Move *m = (Move*)_undo.LastItem();
    _undo.RemoveItem(m);
    SetField(m->x1, m->y1, m->tile);
    SetField(m->x2, m->y2, m->tile);
    updateField(m->x1, m->y1);
    updateField(m->x2, m->y2);
    _redo.AddItem(m, 0);
  }
}

bool Board::canRedo() {
  return !_redo.IsEmpty();
}

void Board::redo() {
  if(canRedo()) {
    undrawArrow();
    Move *m = (Move*)_redo.FirstItem();
    _redo.RemoveItem(m);
    SetField(m->x1, m->y1, EMPTY);
    SetField(m->x2, m->y2, EMPTY);
    updateField(m->x1, m->y1);
    updateField(m->x2, m->y2);
    _undo.AddItem(m);
  }
}

void Board::clearDoList(BList &list) {
  int32 i;
  
  for (i=0; i < list.CountItems(); i++) delete list.ItemAt(i);
  list.MakeEmpty();
}

void Board::clearHistory() {
  // init history
  for(int i = 0; i < 4; i++) {
    history[i].x = -2;
    history[i].y = -2;
  }
}

void Board::getHint() {
  int x1, y1, x2, y2;
  History h[4];

  if(getHint_I(x1, y1, x2, y2, h)) {
    undrawArrow();
    for(int i = 0; i < 4; i++) history[i] = h[i];

    drawArrow(x1, y1, x2, y2);
    fPulseAnim = 2000;
  }
}

bool Board::getHint_I(int &x1, int &y1, int &x2, int &y2, History h[4]) {
  short done[45];
  for( short index = 0; index < 45; index++ ) done[index] = 0;

  // remember old history
  History old[4];
  for(int i = 0; i < 4; i++) old[i] = history[i];

  // initial no hint
  x1 = -1;
  x2 = -1;
  y1 = -1;
  y2 = -1;

  for(int x = 0; x < fSizeX; x++)
    for(int y = 0; y < fSizeX; y++)
      if(GetField(x, y) != EMPTY && done[GetField(x, y)] != 4) {
	int tile = GetField(x, y);
	
	// for all these types of tile search path's
	for(int xx = 0; xx < fSizeX; xx++)
	  for(int yy = 0; yy < fSizeY; yy++)
	    if(xx != x || yy != y)
	      if(GetField(xx, yy) == tile)
		    if(findPath(x, y, xx, yy)) {
		      for(int i = 0; i < 4; i++)  h[i] = history[i];
		      x1 = x;
		      x2 = xx;
		      y1 = y;
		      y2 = yy;
		      for(int i = 0; i < 4; i++)  history[i] = old[i];
		      return TRUE;
		    }
	clearHistory();
	done[tile]++;
  }
  for(int i = 0; i < 4; i++) history[i] = old[i];
  return FALSE;
}

void Board::SetShuffle(int newvalue) {
  fShuffle = newvalue;
}

int Board::tilesLeft() {
  int left = 0;

  for(int i = 0; i < fSizeX; i++)
    for(int j = 0; j < fSizeY; j++)
      if(GetField(i, j) != EMPTY) left++;
  return left;
}

int Board::getCurrentTime() {
  return (int)(time((time_t *)0) - starttime);
}

int Board::getTimeForGame() {
  if(tilesLeft() == 0) 
    return time_for_game;
  else
    return getCurrentTime();
}

bool Board::solvable(bool norestore) {
  int x1, y1, x2, y2;
  History h[4];
  int *oldfield = 0;
 
  if(!norestore) {
    oldfield = (int *)malloc(fSizeI);
    memcpy(oldfield, fField, fSizeI);
  }

  while(getHint_I(x1, y1, x2, y2, h)) {
    SetField(x1, y1, EMPTY);
    SetField(x2, y2, EMPTY);
  }
  
  int left = tilesLeft();

  if(!norestore) {
    memcpy(fField, oldfield, fSizeI);
    free(oldfield);  
  }

  return (bool)(left == 0);
}

void Board::Pulse()  {
  fPulseTime += PULSETIME;
  if (fPulseTime >= 1200) {
    fPulseTime = 0;
    BMessage messy(TIME);
    messy.AddInt32("Time", getTimeForGame());
    this->Window()->PostMessage(&messy);
  }  
  if (fPulseAnim>0) {
    fPulseAnim -= PULSETIME;
    if (fPulseAnim<=0) undrawArrow();
  }
  BView::Pulse();  
}


