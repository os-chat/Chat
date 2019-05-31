#include "main_console.h"
bool iequals(const string &a, const string &b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}
void main_console(char *user_name)
{
    int ch;

    signal(SIGINT, handle_sigint);
    char user_queue_name[20];
    strcpy(user_queue_name, protocol);
    strcat(user_queue_name, user_name);
    string current_umask = "umask " + exec("umask");
    system("umask u=rw,g=w,o=w");
    if ((user_queue = mq_open(user_queue_name, O_RDWR | O_CREAT, 0622, &attr)) < 0)
    {
        perror("mq_open");
        system(current_umask.c_str());
        exit(1);
    }
    system(current_umask.c_str());

    pthread_t thread_recebe, thread_envia;
    pthread_create(&thread_recebe, NULL, &receive_msg, NULL);
    pthread_create(&thread_envia, NULL, &send_msg, NULL);

    initscr();
    raw();
    keypad(stdscr, TRUE);

    vector<string> history;
    string x = "";
    bool exit = false;
    int row = 1, col = 0, it = 0, up_down = 0, down = 0, pos;
    while (exit == false)
    {
        ch = getch();
        switch (ch)
        {
        case KEY_UP:
            if (it - (up_down + 1) >= 0)
            {
                auto last = x;
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
            char user[11], destinatario[11], texto[501];
            char msg_enviada[524];
            strcpy(user, "");
            strcpy(destinatario, "");
            strcpy(texto, "");
            printw("> ");row++;

            sscanf(x.c_str(), " %10[^:\n]:%10[^:]:%500[^\n]", user, destinatario, texto);

            if (!strcmp(user, "exit"))
            {
                break;
            }

            if (!strcmp(user, "list"))
            {
                printw("\nLista de Usuários:\n");row++;
                vector<char *> users = cmd_list();
                for (int i = 0; i < users.size(); ++i)
                    printw("%d - %s\n", i + 1, users[i]);row++;
                printw("\n");row++;
                continue;
            }

            if (!strlen(destinatario) || !strlen(texto) || !strlen(user))
            {
                printw("Formato inválido, tente novamente.\n");row++;
                continue;
            }

            if (strcmp(user, user_name))
            {
                printw("Expedidor inválido, tente novamente.\n");row++;
                continue;
            }

            strcpy(msg_enviada, user);
            strcat(msg_enviada, ":");
            strcat(msg_enviada, destinatario);
            strcat(msg_enviada, ":");
            strcat(msg_enviada, texto);
            strcat(msg_enviada, "\n");

            fila_msg_enviadas.push(msg_enviada);
            sem_post(&S);

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

    endwin();
    mq_close(user_queue);
    mq_unlink(user_queue_name);
}
