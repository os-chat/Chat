#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QCoreApplication>
#include <common.h>

using namespace std;

class Window : public QWidget {
public:
    explicit Window(QWidget *parent = nullptr);
    mqd_t user_queue;
    char user[11];
    static void * start_receive(void* context) { ((Window*)context)->receive_msg(); }
    static void * start_send(void* context) { ((Window*)context)->thread_send_msg(); }
    void start() {
        pthread_t t1, t2;
        pthread_create(&t1, 0, &start_receive, this);
        //pthread_create(&t2, 0, &start_send, this);
    }

private:
    QListWidget *campo_msg_recebida;
    QLineEdit *campo_dest;
    QTextEdit *campo_env_msg;
    QListWidget *campo_retornos;

public slots:
    void * receive_msg();
    void * thread_send_msg();
    void * unique_send(void *ptr);

private slots:
    void send_msg();
    vector<string> cmd_list();
    void show_users();
    void limpar_retornos() { campo_retornos->clear(); }
    void limpar_mensagens() { campo_msg_recebida->clear(); }
};

#endif // WINDOW_H
