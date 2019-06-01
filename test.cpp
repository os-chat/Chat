#include <ncurses.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#define printf printw
using namespace std;
bool iequals(const string &a, const string &b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

void input(string x, int &row){
    char user[11], destinatario[11], texto[501];
    char msg_enviada[524];
    strcpy(user, "");
    strcpy(destinatario, "");
    strcpy(texto, "");
    sscanf(x.c_str(), " %10[^:\n]:%10[^:]:%500[^\n]", user, destinatario, texto);

    if (!strlen(destinatario) || !strlen(texto) || !strlen(user))
    {
        printw("Formato inválido, tente novamente.\n> ");
        row++;
    }
}
int main()
{
    int ch;

    /* Curses Initialisations */
    initscr();
    raw();
    keypad(stdscr, TRUE);

    printw("Press lol to Exit\n> ");
    vector<string> history;
    string x = "", last = "";
    bool exit = false;
    int row = 1, col = 0, it = 0, up_down = 0, down = 0, sadasd, pos;
    while (exit == false)
    {
        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            if (it - (up_down + 1) >= 0)
            {
                last = x;
                move(row, 0); // move to begining of line
                clrtoeol();   // clear line
                col = 0;
                x = history[it - (++up_down)];
                addstr(("> " + x).c_str());
                col = x.size();
            }
            break;
        case KEY_DOWN:
            if (up_down)
            {
                move(row, 0); // move to begining of line
                clrtoeol();   // clear line
                col = 0;
                --up_down;
                if (up_down)
                {
                    x = history[it - up_down];
                    addstr(("> " + x).c_str());
                    col = x.size();
                }
                else
                {
                    // x=last;
                    addstr(("> " + x).c_str());
                }
            }
            break;
        case KEY_LEFT:
            move(row, col - 1);
            if (col)
            {
                col--;
            }
            break;
        case KEY_RIGHT:
            if (col < x.size())
                move(row, ++col);
            break;
        case KEY_BACKSPACE:
            if (x.size())
            {
                x.pop_back();
            }
            if (col)
            {
                col--;
            }
            delch();
            break;
        case 10:
            if (iequals(x, "exit"))
            {
                exit = true;
                break;
            }
            printw("> %s\n> ", x.c_str());
            if (x.size())
            {
                history.push_back(x);
                it++;
            }
            row++;
            input(x, row);
            x = "";
            col = 0;
            up_down = 0;
            break;
        default:
            x += ch;
            col++;
            break;
        }
    }

    printw("\n\Exiting Now\n");
    endwin();

    return 0;
}