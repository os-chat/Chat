#include "main_interface.h"


int main_interface(int argc, char *argv[], char *user_name) {
    signal(SIGINT, handle_sigint);
    QApplication app(argc, argv);
    Window tela;
    char title[20];

    strcpy(tela.user, user_name);
    strcpy(title, "Chat - ");
    strcat(title, tela.user);

    tela.resize(800, 550);
    tela.setWindowTitle(title);
    char user_queue_name[20];
    strcpy(user_queue_name, protocol);
    strcat(user_queue_name, tela.user);
    if ((tela.user_queue = mq_open(user_queue_name, O_RDWR|O_CREAT, 0622, &attr)) < 0) {
        perror("mq_open");
        exit(1);
    }
    tela.start();
    tela.show();

    return app.exec();
}
