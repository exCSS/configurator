/*
 * Copyright (C) 2015 Marius Messerschmidt
 */

#include <ncurses.h>
#include <panel.h>
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
  wattroff(content, A_REVERSE);
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
  fclose(handle);
}

void show_editor(void)
{
  int x = 0, y = 0;
  getmaxyx(stdscr, y, x);
  WINDOW *EDIT = newwin(6, 50, y/2 - 3, x/2 - 25);
  wbkgdset(EDIT, COLOR_PAIR(MAIN) | A_REVERSE);
  wclear(EDIT);
  box(EDIT, 0,0);

  PANEL *editor = new_panel(EDIT);
  show_panel(editor);

  // Create editor

  mvwprintw(EDIT, 0, 21, "> EDIT <");

  mvwprintw(EDIT, 2, 1, "Setting %s to:", items[cur].entry);
  wrefresh(EDIT);

  doupdate();

  update_panels();

  int len = strlen(items[cur].value);

  char *text = malloc(len);
  strncpy(text, items[cur].value, len);

  len++;

  int c = 0;

  curs_set(2);
  mvwprintw(EDIT, 3, 1, text);
  wrefresh(EDIT);
  while((c = getch()) != '\n')
  {

    mvwprintw(EDIT, 3, 1, "                                                ");
    wrefresh(EDIT);
    if(c != KEY_BACKSPACE)
    {
      len++;
      text = realloc(text, len);
      text[len-2] = c;
      text[len-1] = '\0';
    }
    else if(len > 1)
    {
      len--;
      text = realloc(text, len);
      text[len -1] = '\0';
    }

    mvwprintw(EDIT, 3, 1, text);
    wrefresh(EDIT);
  }
  curs_set(0);



  touchwin(content);
  wrefresh(content);
  touchwin(stdscr);
  wrefresh(stdscr);

  free(items[cur].value);
  items[cur].value = malloc(len);

  strncpy(items[cur].value, text, len);

}

void write_config(void)
{
  FILE *h = fopen(filename, "w");
  fprintf(h, "[ExtendedCSS]\n");
  for(int x=0; x < entries;x++)
  {
    fprintf(h, "%s=%s\n", items[x].entry, items[x].value);
  }
  fclose(h);
}

void add_entry(void)
{
  entries++;

  items = realloc(items, sizeof(Entry) * entries);

  int x = 0, y = 0;
  getmaxyx(stdscr, y, x);
  WINDOW *EDIT = newwin(6, 50, y/2 - 3, x/2 - 25);
  wbkgdset(EDIT, COLOR_PAIR(MAIN) | A_REVERSE);
  wclear(EDIT);
  box(EDIT, 0,0);

  PANEL *editor = new_panel(EDIT);
  show_panel(editor);

  // Create editor

  mvwprintw(EDIT, 0, 22, "> ADD <");

  mvwprintw(EDIT, 2, 1, "Name of new entry:");
  wrefresh(EDIT);

  doupdate();

  update_panels();

  int len = 1;
  char *text = malloc(len);
  text[0] = '\0';

  int c = 0;

  curs_set(2);
  mvwprintw(EDIT, 3, 1, text);
  wrefresh(EDIT);
  while((c = getch()) != '\n')
  {

    mvwprintw(EDIT, 3, 1, "                                                ");
    wrefresh(EDIT);
    if(c != KEY_BACKSPACE)
    {
      len++;
      text = realloc(text, len);
      text[len-2] = c;
      text[len-1] = '\0';
    }
    else if(len > 1)
    {
      len--;
      text = realloc(text, len);
      text[len -1] = '\0';
    }

    mvwprintw(EDIT, 3, 1, text);
    wrefresh(EDIT);
  }
  curs_set(0);



  touchwin(content);
  wrefresh(content);
  touchwin(stdscr);
  wrefresh(stdscr);


  items[entries-1].value = malloc(6);
  strncpy(items[entries-1].value, "value\0", 6);

  items[entries-1].entry = malloc(len);
  strncpy(items[entries-1].entry, text, len);


}

int main_loop (void)
{

  slk_set(1, "[q] Exit", 1);
  slk_set(2, "[s] Save", 1);
  slk_set(3, "[a] Add", 1);
  slk_set(4, "", 1);
  slk_set(5, "", 1);
  slk_set(6, "", 1);
  slk_set(7, "", 1);
  slk_set(8, "", 1);
  slk_refresh();

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

    switch (c)
    {
      case KEY_UP:
        if(cur > 0)
          cur--;
      break;

      case KEY_DOWN:
        if(cur < entries-1)
          cur++;
      break;

      case 0xA:
        show_editor();
      break;

        // Keys

      case 'q':
      case 'Q':
      case KEY_F(1):
        exit(0);
      break;

      case 's':
      case 'S':
      case KEY_F(2):
        write_config();
      break;

      case 'a':
      case 'A':
      case KEY_F(3):
        add_entry();
      break;
    }
    clear();
    draw_title();

  }

  return 0;
}

void usage(char *c)
{
  printf( "<exCSS Configurator/>\n"\
  " :: Usage:\n" \
  "     %s <filename>\n" \
  " Used to configure .ex files\n" \
  " Copyright (C) 2015 Marius Messerschmidt\n", c);
}

int main (int argc, char **argv)
{

  if(argc < 2)
  {
    usage(argv[0]);
    exit(1);
  }

  if(strcmp(argv[1],"-h") == 0)
  {
    usage(argv[0]);
    exit(0);
  }

  filename = argv[1];

  // Softkeys
  slk_init(0);


  // Init nCurses
  initscr();
  atexit(quit);

  curs_set(0); // Invisible Cursor
  noecho();
  start_color(); //Enable Colors

  keypad(stdscr, true);

  init_pair(MAIN, COLOR_WHITE, COLOR_BLUE);

  color_set(MAIN, NULL);
  bkgd(COLOR_PAIR(MAIN));

  cbreak();
  nl();

  return main_loop();
}
