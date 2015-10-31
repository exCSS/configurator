/*
 * Copyright (C) 2015 Marius Messerschmidt
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define MAIN 1

WINDOW *content;
char *filename = "test/simple.ex";

int entries = 0;
int cur = 0;

typedef struct
{
    char *entry;
    char *value;
}Entry;

Entry *items;

void quit (void)
{
  endwin();
}

void draw_title(void)
{
  int x = 0, y = 0;
  getmaxyx(stdscr, y, x);
  mvprintw(1,x / 2 - 15, "<!--- exCSS Configurator ---/>");
  mvprintw(2,x / 2 - (strlen(filename) /2), filename);
}

void show_entries (void)
{
  for(int x = 0; x < entries; x++)
  {
    if(x == cur)
      wattron(content, A_REVERSE);
    else
      wattroff(content, A_REVERSE);
    mvwprintw(content, 1+x, 2, "%s -> %s", items[x].entry, items[x].value);
  }
}

void get_entries (void)
{
  FILE *handle = fopen(filename, "r");
  char buffer[2048];

  entries = 0;

  while(fgets(buffer, 2048, handle) != NULL)
  {

    if(buffer[0] == ';' || buffer[0] == '[')
      continue;

    entries++;
    char *e, *v;

    e = strtok(buffer, "=");
    v = strtok(NULL, "\n\0");

    items = realloc(items, sizeof(Entry) * entries);

    items[entries-1].entry = malloc(strlen(e) +1);
    strncpy(items[entries-1].entry, e, strlen(e) +1);

    items[entries-1].value = malloc(strlen(v) +1);
    strncpy(items[entries-1].value, v, strlen(v) +1);

  }

}

int main_loop (void)
{
  clear();
  draw_title();

  int x = 0, y = 0;
  getmaxyx(stdscr, y, x);

  // Create main window
  content = newwin(y - 4, x - 4, 3, 2);
  wbkgd(content, COLOR_PAIR(MAIN));
  get_entries();


  while (1)
  {


    wclear(content);
    box(content, 0,0);
    show_entries();

    refresh();
    wrefresh(content);

    int c = getch();


  }

  return 0;
}



int main (void)
{
  // Init nCurses
  initscr();
  atexit(quit);

  curs_set(0); // Invisible Cursor
  noecho();
  start_color(); //Enable Colors

  init_pair(MAIN, COLOR_WHITE, COLOR_BLUE);

  color_set(MAIN, NULL);
  bkgd(COLOR_PAIR(MAIN));

  return main_loop();
}
