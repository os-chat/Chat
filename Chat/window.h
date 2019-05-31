#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QCoreApplication>
#include "receive_msg.h"
#include "send_msg.h"
#include "cmd_list.h"

using namespace std;

class Window : public QWidget {
public:
    explicit Window(QWidget *parent = nullptr);
    mqd_t user_queue;
    char user[11];
    static void * startThread(void* context) {
        ((Window*)context)->receive_msg();
    }
    void start() {
        pthread_t thread_id;
        pthread_create(&thread_id, 0, &startThread, this);
    }

private:
    QListWidget *campo_msg_recebida;
    QLineEdit *campo_dest;
    QTextEdit *campo_env_msg;
    QListWidget *campo_retornos;

public slots:
    void * receive_msg();

private slots:
    void send_msg();
    vector<char*> cmd_list();
    void show_users();
    void limpar_retornos();
    void limpar_mensagens();
};

#endif // WINDOW_H
