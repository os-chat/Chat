#include "window.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton> 
#include <dirent.h>

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
    connect(sendButton, &QPushButton::clicked, this, &Window::send_msg);
    enviadas->addWidget(sendButton, 3, 1);

    QLabel *label_retornos = new QLabel("Retornos:", this);
    enviadas->addWidget(label_retornos, 4, 0, 1, 1);
    label_retornos->setAlignment(Qt::AlignRight | Qt::AlignTop);

    campo_retornos = new QListWidget(this);
    enviadas->addWidget(campo_retornos, 4, 1, 1, 1);

    QPushButton *listButton = new QPushButton("List", this);
    connect(listButton, &QPushButton::clicked, this, &Window::show_users);
    enviadas->addWidget(listButton, 5, 1);

    QPushButton *clearButton = new QPushButton("Limpar", this);
    connect(clearButton, &QPushButton::clicked, this, &Window::limpar_retornos);
    enviadas->addWidget(clearButton, 6, 1);

    tela->addLayout(recebidas);
    tela->addSpacing(10);
    tela->addLayout(enviadas);
    setLayout(tela);
}

void Window::send_msg() {
    char destinatario[11], mensagem[501], protocol[] = "/chat-";

    QByteArray ba = campo_env_msg->toPlainText().toLocal8Bit();
    strcpy(mensagem, ba.data());
    ba = campo_dest->text().toLocal8Bit();
    strcpy(destinatario, ba.data());

    if(!strcmp(mensagem, "exit") && !strlen(destinatario)) {
        this->close();
        return;
    }

    if(!strlen(destinatario)) {
        campo_retornos->addItem("Campo 'destinatario' só pode ficar vazio se a mensagem for 'exit' (comando para fechar o Chat)");
        return;
    }

    char msg_enviada[524], other_queue_name[20];
    mqd_t other_queue;

    strcpy(msg_enviada, user);
    strcat(msg_enviada, ":");
    strcat(msg_enviada, destinatario);
    strcat(msg_enviada, ":");
    strcat(msg_enviada, mensagem);
    strcat(msg_enviada, "\n");

    fila_msg_enviadas.push(msg_enviada);
    sem_post(&S);

    campo_dest->setText("");
    campo_env_msg->setText("");
}

void * Window::receive_msg() {
    char msg_recebida[524], *user, *dest, msg[524];
    while(1) {
        strcpy(msg, "");
        if ((mq_receive (user_queue, msg_recebida, sizeof(msg_recebida), 0)) < 0) {
            perror("mq_receive:");
            exit(1);
        }
        user = strtok(msg_recebida, ":");
        dest = strtok(NULL, ":");

        if(strcmp(dest, "all") == 0)
            strcpy(msg, "Broadcast de ");

        strcat(msg, user);
        strcat(msg, ": ");
        strcat(msg, strtok(NULL, ":"));
        campo_msg_recebida->addItem(msg);
    }
}

void * Window::thread_send_msg() {
    while (1) {
        mqd_t other_queue;
        char other_queue_name[20];
        char *user_name, *destinatario, msg_enviada[524], token[524];
        strcpy(token, fila_msg_enviadas.front());
        user_name = strtok(token, ":");
        destinatario = strtok(NULL, ":");
        vector<string> user_list = cmd_list();

        if (strcmp(destinatario, "all") == 0) { // se o destinatário for all
            vector<pthread_t> thread;
            for (size_t i = 0; i < user_list.size(); ++i) {
                pthread_t t;
                thread.push_back(t);
            }

            for (size_t i = 0; i < thread.size(); ++i) {
                if (strcmp(user_name, user_list[i].c_str())) {
                    //pthread_create(&thread[i], NULL, unique_send, (void *)user_list[i].c_str());
                }
            }
        }
        else {
            strcpy(other_queue_name, protocol);
            strcat(other_queue_name, destinatario);

            // O_WRONLY = Open - Write Only
            if ((other_queue = mq_open(other_queue_name, O_WRONLY | O_NONBLOCK)) < 0) {
                printf("UNKNOWNUSER %s\n", destinatario);
            }
            else {
                pthread_t t;
                //pthread_create(&t, NULL, unique_send, (void *)destinatario);
            }
        }
    }
}

void * Window::unique_send(void *ptr) {
    mqd_t other_queue;
    string protc = protocol;
    string user = (char *)ptr;
    string res = protc + user;
    other_queue = mq_open(res.c_str(), O_WRONLY | O_NONBLOCK);
    char msg_enviada[501];
    strcpy(msg_enviada, fila_msg_enviadas.front());

    int tentativas = 0;
    while (tentativas <= 3) {
        if (mq_send(other_queue, msg_enviada, sizeof(msg_enviada), 0) < 0 && errno == EAGAIN) {
            tentativas++;
            if (tentativas == 4)
                break;
            sleep(1 + tentativas);
        }
        else {
            break;
        }
    }
    if (tentativas > 3) {
        string erro = "ERRO ";
        erro += msg_enviada;
        campo_retornos->addItem(QString::fromStdString(erro));
    }

    mq_close(other_queue);
    pthread_exit(NULL);
}

vector<string> Window::cmd_list() {
    vector<string> users;
    DIR *d;
    struct dirent *dir;
    d = opendir("/dev/mqueue");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            char *token;
            token = strtok(dir->d_name, "-");
            if (strcmp(token, "chat") == 0) {
                token = strtok(NULL, "-");
                users.push_back(token);
            }
        }
        closedir(d);
    }

    return users;
}

void Window::show_users() {
    vector<string> users = cmd_list();
    campo_retornos->addItem("Lista de Usuários:");
    for(auto u : users)
        campo_retornos->addItem(QString::fromStdString(u));
}
