// SDLTileMap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#undef  main

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 640

#define MAX_MAP_X 400
#define MAX_MAP_Y 10

#define TILE_SIZE 64
#define SCROLL_SPEED 4;

#define MAX_MESSAGE_LENGTH 100
#define BLANK_TILE 0
#define MAX_TILE 61



int g_type_map = 1;

typedef struct Map
{
  int start_x_;
  int start_y_;
  int max_x_;
  int max_y_;
  int tile[MAX_MAP_Y][MAX_MAP_X];
  SDL_Surface *back_ground_;
  char *file_name_;
} Map; 

typedef struct Cursor
{
  int x_;
  int y_;
  int tile_id_;
} Cursor;

typedef struct Message
{
  char text[MAX_MESSAGE_LENGTH];
  int counter_;
} Message;

typedef struct Input
{
  int left, right, up, down;
  int add, remove;
  int previous, next, load, save;
  int mousex, mousey;
} Input;

SDL_Surface* g_screen = NULL;
SDL_Surface* g_background = NULL;
SDL_Event g_even;

Cursor cursor;
static Input g_input;
Map g_map;
SDL_Surface* g_images[MAX_TILE];
SDL_Surface* g_map_list;

SDL_Surface *load_image( std::string filename )
{
  SDL_Surface* loadedImage = NULL;

  SDL_Surface* optimizedImage = NULL;

  loadedImage = IMG_Load( filename.c_str() );

  if( loadedImage != NULL )
  {
    optimizedImage = SDL_DisplayFormat( loadedImage );

    SDL_FreeSurface( loadedImage );

    if( optimizedImage != NULL )
    {
      SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 195, 195, 195 ) );
    }
  }

  return optimizedImage;
}

void DrawImage(SDL_Surface* g_img, int x, int y)
{
  if (g_img != NULL)
  {
      SDL_Rect dest;

      dest.x = x;
      dest.y = y;
      dest.w = g_img->w;
      dest.h = g_img->h;

      SDL_BlitSurface(g_img, NULL, g_screen, &dest);
  }
  
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
  //Holds offsets
  SDL_Rect offset;

  //Get offsets
  offset.x = x;
  offset.y = y;

  //Blit
  SDL_BlitSurface( source, clip, destination, &offset );
}


bool Init()
{
  if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
  {
    return false;
  }

  g_screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_HWPALETTE | SDL_DOUBLEBUF);

  if( g_screen == NULL )
  {
    return false;
  }

  SDL_WM_SetCaption( "Build Tile Map", NULL );
  return true;
}

void CleanUp()
{
  if (g_background) SDL_FreeSurface(g_background);
  SDL_Quit();
}


void LoadMap(char* name)
{
  FILE* fp = NULL;
  fopen_s(&fp, name, "rb");
  if (fp == NULL)
  {
    return;
  }

  for (int i = 0; i < MAX_MAP_Y; i++)
  {
    for (int j = 0; j < MAX_MAP_X; j++)
    {
      fscanf_s(fp, "%d", &g_map.tile[i][j]);
      if (g_map.tile[i][j] != BLANK_TILE)
      {
        if (j > g_map.max_x_)
          g_map.max_x_ = j;
        if (i > g_map.max_y_)
           g_map.max_y_ = i;
      }
    }
  }

  g_map.max_x_++;
  g_map.max_y_++;

  g_map.start_x_ = g_map.start_y_ = 0;

  g_map.max_x_ = MAX_MAP_X*TILE_SIZE;
  g_map.max_y_ = MAX_MAP_Y*TILE_SIZE;

  g_map.file_name_ = name;
  fclose(fp);
}

void SaveMap()
{
  FILE *fp = NULL;

  if (g_map.file_name_ == NULL)
  {
      g_map.file_name_ = "map.dat";
  }
  fopen_s(&fp,  g_map.file_name_, "w");

  /* If we can't open the map then exit */

  if (fp == NULL)
  {
    printf("Failed to open map %s\n", g_map.file_name_);

    exit(1);
  }

  /* Write the data from the file into the map */

  for (int y=0;y<MAX_MAP_Y;y++)
  {
    for (int x=0;x<MAX_MAP_X;x++)
    {
      fprintf(fp, "%d ", g_map.tile[y][x]);
    }

    fprintf(fp, "\n");
  }

  /* Close the file afterwards */

  fclose(fp);
}

void LoadMapTiles()
{
  char filename[40];
  FILE *fp;

  for (int i = 0; i < MAX_TILE; i++)
  {
    sprintf_s(filename, "map2/%d.png", i);

    fopen_s(&fp, filename, "rb");

    if (fp == NULL)
    {
      continue;
    }

    fclose(fp);

    g_images[i] = load_image(filename);

    if (g_images[i] == NULL)
    {
      exit(1);
    }
  }
}

void FreeMapTiles()
{
  int i;

  for (i=0; i<MAX_TILE; i++)
  {
    if (g_images[i] != NULL)
    {
      SDL_FreeSurface(g_images[i]);
    }
  }
}

void DoMap()
{
  if (g_input.left == 1)
  {
    g_map.start_x_ -= SCROLL_SPEED;

    if (g_map.start_x_< 0)
    {
      g_map.start_x_ = 0;
    }
  }

  else if (g_input.right == 1)
  {
    g_map.start_x_ += SCROLL_SPEED;

    if (g_map.start_x_ + SCREEN_WIDTH >= g_map.max_x_)
    {
      g_map.start_x_= g_map.max_x_ - SCREEN_WIDTH;
    }
  }

  if (g_input.up == 1)
  {
    g_map.start_y_ -= SCROLL_SPEED;

    if (g_map.start_y_ < 0)
    {
      g_map.start_y_ = 0;
    }
  }

  else if (g_input.down == 1)
  {
    g_map.start_y_ += SCROLL_SPEED;

    if (g_map.start_y_ + SCREEN_HEIGHT >= g_map.max_y_)
    {
      g_map.start_y_ = g_map.max_y_ - SCREEN_HEIGHT;
    }
  }
}

void DrawMap()
{
  int map_x = 0;
  int x1 = 0;
  int x2 = 0;
  int map_y = 0;
  int y1 = 0;
  int y2 = 0;

  map_x = g_map.start_x_/TILE_SIZE;
  x1 = (g_map.start_x_ %TILE_SIZE)*-1;
  x2 = x1 + SCREEN_WIDTH + (x1 == 0 ? 0 : TILE_SIZE);

  map_y = g_map.start_y_/TILE_SIZE;
  y1 = (g_map.start_y_%TILE_SIZE)*-1;
  y2 = y1 + SCREEN_HEIGHT + (y1 == 0 ? 0 :TILE_SIZE);

  DrawImage(g_background, 0, 0);

  for (int i = y1; i < y2; i += TILE_SIZE)
  {
    map_x = g_map.start_x_/TILE_SIZE;
    for (int j= x1; j < x2; j +=TILE_SIZE)
    {
      if (g_map.tile[map_y][map_x] != BLANK_TILE)
      {
        DrawImage(g_images[g_map.tile[map_y][map_x]], j, i);
      }
      map_x++;
    }
    map_y++;
  }

}

void DoCursor()
{
  cursor.x_ = g_input.mousex;
  cursor.y_ = g_input.mousey;

  //if (cursor.y_ >= SCREEN_HEIGHT - TILE_SIZE)
  //{
  //  cursor.y_ = SCREEN_HEIGHT - TILE_SIZE * 2;
 // }

  if (g_input.left == 1)
  {
    g_map.start_x_ -= TILE_SIZE;

    if (g_map.start_x_ < 0)
    {
      g_map.start_x_ = 0;
    }
  }

  else if (g_input.right == 1)
  {
    g_map.start_x_ += TILE_SIZE;

    if (g_map.start_x_ + SCREEN_WIDTH >= g_map.max_x_)
    {
      g_map.start_x_ = g_map.max_x_ - SCREEN_WIDTH;
    }
  }

  if (g_input.up == 1)
  {
    g_map.start_y_ -= TILE_SIZE;

    if (g_map.start_y_ < 0)
    {
      g_map.start_y_ = 0;
    }
  }

  else if (g_input.down == 1)
  {
    g_map.start_y_ += TILE_SIZE;

    if (g_map.start_y_ + SCREEN_HEIGHT >= g_map.max_y_)
    {
      g_map.start_y_ = g_map.max_y_ - SCREEN_HEIGHT;
    }
  }

  if (g_input.add == 1)
  {
    g_map.tile[(g_map.start_y_ + cursor.y_) / TILE_SIZE][(g_map.start_x_ + cursor.x_) / TILE_SIZE] = g_type_map;
  }

  else if (g_input.remove == 1)
  {
    g_map.tile[(g_map.start_y_ + cursor.y_) / TILE_SIZE][(g_map.start_x_ + cursor.x_) / TILE_SIZE] = BLANK_TILE;
  }

  if (g_input.previous == 1)
  {
    do
    {
      cursor.tile_id_--;

      if (cursor.tile_id_ < 0)
      {
        cursor.tile_id_ = MAX_TILE - 1;
      }
    }

    while (g_images[cursor.tile_id_] == NULL);

    g_input.previous = 0;
  }

  if (g_input.next == 1)
  {
    do
    {
      cursor.tile_id_++;

      if (cursor.tile_id_ >= MAX_TILE)
      {
        cursor.tile_id_ = 0;
      }
    }

    while (g_images[cursor.tile_id_] == NULL);

    g_input.next = 0;
  }

  if (g_input.save == 1)
  {
    SaveMap();

    //setStatusMessage("Saved OK");

    g_input.save = 0;
  }

  if (g_input.load == 1)
  {
    LoadMap(g_map.file_name_);

    //setStatusMessage("Loaded OK");

    g_input.load = 0;
  }

  if (g_input.left == 1 || g_input.right == 1 || g_input.up == 1 || g_input.down == 1)
  {
    SDL_Delay(30);
  }
}

void drawCursor()
{
  DrawImage(g_images[cursor.tile_id_], cursor.x_, cursor.y_);
}


int main(int argc, _TCHAR* argv[])
{

  Init();
  g_map_list = load_image("map_list.png");
  g_background = load_image("background#.png");
  if (g_background == NULL)
    return -1;

  LoadMapTiles();
  LoadMap("map.dat");
  bool is_quit = false;

  while(is_quit == false)
  {
    while (SDL_PollEvent(&g_even))
    {
      if(g_even.type == SDL_QUIT )
      {
        is_quit = true;
      }
      else if (g_even.type == SDL_KEYDOWN)
      {
        SDLKey key = g_even.key.keysym.sym;
        if ( key == SDLK_ESCAPE)
        {
          return 0;
        }
        else if (key  == SDLK_UP)
        {
          g_input.up = 1;
        }
        else if (key == SDLK_DOWN)
        {
          g_input.down = 1;
        }
        else if (key== SDLK_LEFT)
        {
          g_input.left = 1;
        }
        else if (key == SDLK_RIGHT)
        {
          g_input.right = 1;
        }
        else if (key  == SDLK_SPACE)
        {
          g_input.add = 1;
        }
        else if (key == SDLK_PERIOD || key == SDLK_EQUALS)
        {
          g_input.next = 1;
        }
        else if (key == SDLK_COMMA || key == SDLK_MINUS)
        {
          g_input.previous = 1;
        }
        else if (key == SDLK_s)
        {
          g_input.save = 1;
        }
        /*else if (key == SDLK_l)
        {
          g_input.load = 1;
        }*/
        else if (key == SDLK_1)
        {
            g_type_map = 1;
        }
        else if (key == SDLK_2)
        {
           g_type_map = 2;
        }
        else if (key == SDLK_3)
        {
            g_type_map = 3;
        }
        else if (key == SDLK_4)
        {
            g_type_map = 4;
        }
        else if (key == SDLK_5)
        {
            g_type_map = 5;
        }
        else if (key == SDLK_6)
        {
            g_type_map = 6;
        }
        else if (key == SDLK_7)
        {
            g_type_map = 7;
        }
        else if (key == SDLK_8)
        {
            g_type_map = 8;
        }
        else if (key == SDLK_9)
        {
            g_type_map = 9;
        }
        else if (key == SDLK_a)
        {
            g_type_map = 10;
        }
        else if (key == SDLK_b)
        {
            g_type_map = 11;
        }
        else if (key == SDLK_c)
        {
            g_type_map = 12;
        }
        else if (key == SDLK_d)
        {
            g_type_map = 42;
        }
        else if (key == SDLK_e)
        {
            g_type_map = 43;
        }
        else if (key == SDLK_f)
        {
            g_type_map = 44;
        }
        else if (key == SDLK_g)
        {
            g_type_map = 45;
        }
        else if (key == SDLK_h)
        {
            g_type_map = 48;
        }
        else if (key == SDLK_i)
        {
            g_type_map = 18;
        }
        else if (key == SDLK_k)
        {
            g_type_map = 56;
        }
        else if (key == SDLK_l)
        {
            g_type_map = 58;
        }
        else if (key == SDLK_m)
        {
            g_type_map = 26;
        }
        else if (key == SDLK_n)
        {
            g_type_map = 24;
        }
        else if (key == SDLK_o)
        {
            g_type_map = 14;
        }
        else if (key == SDLK_p)
        {
            g_type_map = 13;
        }
        else if (key == SDLK_q)
        {
            g_type_map = 50;
        }
        else if (key == SDLK_r)
        {
            g_type_map = 55;
        }
        else if (key == SDLK_t)
        {
            g_type_map = 34;
        }
        else if (key == SDLK_j)
        {
            g_type_map = 35;
        }
        else if (key == SDLK_u)
        {
            g_type_map = 36;
        }
        else if (key == SDLK_v)
        {
            g_type_map = 37;
        }
        else if (key == SDLK_w)
        {
            g_type_map = 38;
        }
        else if (key == SDLK_x)
        {
            g_type_map = 46;
        }
        else if (key == SDLK_y)
        {
            g_type_map = 20;
        }
        else if (key == SDLK_z)
        {
            g_type_map = 21;
        }
      }
      else if (g_even.type == SDL_KEYUP)
      {
        SDLKey key = g_even.key.keysym.sym;
        if (key  == SDLK_UP)
        {
          g_input.up = 0;
        }
        else if (key == SDLK_DOWN)
        {
          g_input.down = 0;
        }
        else if (key == SDLK_LEFT)
        {
          g_input.left = 0;
        }
        else if (key == SDLK_RIGHT)
        {
          g_input.right = 0;
        }
        else if (key  == SDLK_SPACE)
        {
          g_input.add = 0;
        }
        else if (key == SDLK_PERIOD || key == SDLK_EQUALS)
        {
          g_input.next = 0;
        }
        else if (key == SDLK_COMMA || key == SDLK_MINUS)
        {
          g_input.previous = 0;
        }
        else if (key == SDLK_s)
        {
          g_input.save = 0;
        }
        else if (key == SDLK_l)
        {
          g_input.load = 0;
        }
      }
      else if (g_even.type == SDL_MOUSEBUTTONDOWN)
      {
        switch(g_even.button.button)
        {
        case SDL_BUTTON_LEFT:
          g_input.add = 1;
          break;

        case SDL_BUTTON_RIGHT:
          g_input.remove = 1;
          break;

        default:
          break;
        }
      }
      else if (g_even.type == SDL_MOUSEBUTTONUP)
      {
        switch(g_even.button.button)
        {
        case SDL_BUTTON_LEFT:
          g_input.add = 0;
          break;

        case SDL_BUTTON_RIGHT:
          g_input.remove = 0;
          break;

        default:
          break;
        }
      }
    }


    
    SDL_GetMouseState(&g_input.mousex, &g_input.mousey);

    g_input.mousex /= TILE_SIZE;
    g_input.mousey /= TILE_SIZE;

    g_input.mousex *= TILE_SIZE;
    g_input.mousey *= TILE_SIZE;

    DoCursor();
    DoMap();
    DrawMap();

    DrawImage(g_map_list, 20, 10);
    SDL_Flip(g_screen);

  }

  CleanUp();
	return 0;
}

