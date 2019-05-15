#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QCoreApplication>
#include <pthread.h>
#include <mqueue.h>

class Window : public QWidget {
    //Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    mqd_t user_queue;
    char user[11];
    static void * startThread(void* context) {
        ((Window*)context)->receber_mensagem();
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
    void * receber_mensagem();

private slots:
    void enviar_mensagem();
};

#endif // WINDOW_H