#ifndef __BOARD__H__
#define __BOARD__H__

#include <View.h>
#include <Bitmap.h>

const uint32 KLICK	 = 'Klil';
const uint32 TIME	 = 'Time';
const uint32 ENDOFGAME = 'EoGa';
const uint32 PULSETIME = 125;

typedef struct History {
  int x, y;
};

class Move {
public:
  Move(int _x1, int _y1, int _x2, int _y2, int _tile) {
    x1 = _x1;
    y1 = _y1;
    x2 = _x2;
    y2 = _y2;
    tile = _tile;
  }
  int x1, x2, y1, y2;
  int tile;
}; 

class Board : public BView {
public:
  Board(BRect frame);
  ~Board();
  
  virtual void AttachedToWindow();
  virtual void Pulse();
  virtual void Draw(BRect updateRect);
  virtual void MouseDown(BPoint point);

  void SetShuffle(int);
  void SetDelay(int);
  bool canUndo();
  void undo();
  bool canRedo();
  void redo();
  void clearDoList(BList &list);

  void setSize(int x, int y); 
  void newGame();

  void getHint();

  int   tilesLeft();
  int   getCurrentTime();
  int   getTimeForGame(); 

  bool solvable(bool norestore = FALSE);


private: 
  void Marked(int, int);
  bool loadTiles(float scale = 1.0);
  void madeMove(int, int, int, int);
  void SetField(int x, int y, int value);
  int  GetField(int x, int y);
  void SetMarked(int x, int y, bool value); 
  bool GetMarked(int x, int y);
  void updateField(int, int);
  void ChangeMarked(int i, int j, bool mark);
  bool canMakePath(int x1, int y1, int x2, int y2); 
  bool findPath(int x1, int y1, int x2, int y2);
  bool findSimplePath(int x1, int y1, int x2, int y2);
  void drawArrow(int, int, int, int);
  void undrawArrow();
  BPoint midCoord(int, int);  
  void clearHistory();
  BBitmap *lighten(BBitmap *src);
  bool getHint_I(int &, int &, int &, int &, History h[4]);

  time_t starttime;
  time_t time_for_game;

  BList _undo;
  BList _redo;

  int mark_x;
  int mark_y; 
  History history[4]; 
  bool *fMarked;
  int *fField;
  int fSizeX;
  int fSizeY;
  int fSizeI;
  int fDelay;
  int fShuffle; 
  float fDx, fDy;
  
  BBitmap *pm_tile[45], *pm;

  int fPulseTime, fPulseAnim;
};

#endif
