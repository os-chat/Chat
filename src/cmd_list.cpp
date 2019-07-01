#include "cmd_list.hpp"

vector<string> cmd_list(const char * protocol) {
    vector<string> users;
    DIR *d;
    struct dirent *dir;
    d = opendir("/dev/mqueue");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            char *token;
            token = strtok(dir->d_name, "-");
            if (strcmp(token, protocol) == 0) {
                token = strtok(NULL, "-");
                users.push_back(token);
            }
        }
        closedir(d);
    }

    return users;
}
