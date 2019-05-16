#include "window.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

Window::Window(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *tela = new QHBoxLayout(this);
    QGridLayout *recebidas = new QGridLayout();
    QGridLayout *enviadas = new QGridLayout();
    enviadas->setVerticalSpacing(15);

    QLabel *label_msg_receb = new QLabel("Mensagens Recebidas", this);
    recebidas->addWidget(label_msg_receb, 0, 0);
    label_msg_receb->setAlignment(Qt::AlignCenter);

    campo_msg_recebida = new QListWidget(this);
    recebidas->addWidget(campo_msg_recebida, 1, 0);

    QPushButton *clearMsgButton = new QPushButton("Limpar", this);
    connect(clearMsgButton, &QPushButton::clicked, this, &Window::limpar_mensagens);
    recebidas->addWidget(clearMsgButton, 2, 0);

    QLabel *label_dest = new QLabel("Destinatário:", this);
    enviadas->addWidget(label_dest, 0, 0, 1, 1);
    label_dest->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    campo_dest = new QLineEdit(this);
    enviadas->addWidget(campo_dest, 0, 1, 1, 1);
    campo_dest->setText("");

    QLabel *label_env_msg = new QLabel("Mensagem:", this);
    enviadas->addWidget(label_env_msg, 2, 0, 1, 1);
    label_env_msg->setAlignment(Qt::AlignRight | Qt::AlignTop);

    campo_env_msg = new QTextEdit(this);
    enviadas->addWidget(campo_env_msg, 2, 1, 1, 1);
    campo_env_msg->setText("");

    QPushButton *sendButton = new QPushButton("Enviar", this);
    connect(sendButton, &QPushButton::clicked, this, &Window::enviar_mensagem);
    enviadas->addWidget(sendButton, 3, 1);

    QLabel *label_retornos = new QLabel("Retornos:", this);
    enviadas->addWidget(label_retornos, 4, 0, 1, 1);
    label_retornos->setAlignment(Qt::AlignRight | Qt::AlignTop);

    campo_retornos = new QListWidget(this);
    enviadas->addWidget(campo_retornos, 4, 1, 1, 1);

    QPushButton *listButton = new QPushButton("List", this);
    connect(listButton, &QPushButton::clicked, this, &Window::list);
    enviadas->addWidget(listButton, 5, 1);

    QPushButton *clearButton = new QPushButton("Limpar", this);
    connect(clearButton, &QPushButton::clicked, this, &Window::limpar_retornos);
    enviadas->addWidget(clearButton, 6, 1);

    tela->addLayout(recebidas);
    tela->addSpacing(10);
    tela->addLayout(enviadas);
    setLayout(tela);
}

void Window::enviar_mensagem() {
    char destinatario[11], mensagem[501];
    QByteArray ba = campo_env_msg->toPlainText().toLocal8Bit();
    strcpy(mensagem, ba.data());
    ba = campo_dest->text().toLocal8Bit();
    strcpy(destinatario, ba.data());

    if(!strcmp(mensagem, "exit") && !strlen(destinatario))
        this->close();

    char msg_enviada[524];
    strcpy(msg_enviada, user);
    strcat(msg_enviada, ":");
    strcat(msg_enviada, destinatario);
    strcat(msg_enviada, ":");
    strcat(msg_enviada, mensagem);
    strcat(msg_enviada, "\n");

    mqd_t other_queue;
    char other_queue_name[20];
    strcpy(other_queue_name, "/chat-");
    strcat(other_queue_name, destinatario);

    // O_WRONLY = Open - Write Only
    if((other_queue = mq_open (other_queue_name, O_WRONLY)) < 0) {
        char unk_user[25];
        strcpy(unk_user, "UNKNOWNUSER ");
        strcat(unk_user, destinatario);
        campo_retornos->addItem(unk_user);
    }
    else {
        if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0) {
            perror ("mq_send");
            exit(1);
        }

        mq_close(other_queue);
    }

    campo_dest->setText("");
    campo_env_msg->setText("");
}

void * Window::receber_mensagem() {
    char msg_recebida[524];
    char msg[524];
    while(1) {
        char *token;
        if ((mq_receive (user_queue, msg_recebida, sizeof(msg_recebida), 0)) < 0) {
            perror("mq_receive:");
            exit(1);
        }
        token = strtok(msg_recebida, ":");
        strcpy(msg, token);
        strcat(msg, ": ");
        token = strtok(NULL, ":");
        token = strtok(NULL, ":");
        strcat(msg, token);
        campo_msg_recebida->addItem(msg);
    }
}

void Window::list() {
    DIR *d;
    struct dirent *dir;
    d = opendir("/dev/mqueue");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            char *token;
            token = strtok(dir->d_name, "-");
            if (strcmp(token, "chat") == 0) {
                token = strtok(NULL, "-");
                campo_retornos->addItem(token);
            }
        }
        closedir(d);
    }
}

void Window::limpar_retornos() {
    campo_retornos->clear();
}

void Window::limpar_mensagens() {
    campo_msg_recebida->clear();
}
