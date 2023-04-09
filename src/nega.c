#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

const static char *usage =
"Usage: nega <command> [options]\n"
"Commands:\n"
"    \x1b[34msteal\x1b[0m\t\tInstall a package\n"
"        \033[32m-r/--refresh\033[0m\t\tRefresh package database\n"
"        \033[32m-u/--upgrade\033[0m\t\tUpgrade system\n"
"        \033[32m-q/--quiet\033[0m\t\tQuiet mode\n"
"        \033[32m-w/--wash\033[0m\t\tClean all dependencies\n"
"    \x1b[34myeet\x1b[0m\t\tRemove a package\n"
"        \033[32m-c/--cascade\033[0m\t\tRemove all dependent packages too (recursive)\n"
"        \033[32m-r/--recursive\033[0m\t\tRecursively remove all children and dependencies\n"
"        \033[32m-u/--unneeded\033[0m\t\tDelete all other unneeded packages\n"
"Options:\n"
"    \x1b[35m-y/--yes\x1b[0m\t\t\tAssume yes to any prompt\n"
"    \x1b[35m-v/--verbose\x1b[0m\t\tVerbose output\n"
;

static bool flag_yes = false;
static bool flag_verbose = false;

static bool flag_steal_refresh = false;
static bool flag_steal_upgrade = false;
static bool flag_steal_quiet = false;
static bool flag_steal_wash = false;

static bool flag_yeet_cascase = false;
static bool flag_yeet_recursive = false;
static bool flag_yeet_unneeded = false;

static inline void check_is_valid_command(char *command) {
    if (strcmp(command, "steal") != 0 && strcmp(command, "yeet") != 0) {
        printf("\x1b[31mError: Unknown command '%s'\n\x1b[0m", command);
        exit(1);
    }
}

static inline void grab_steal_flags(char flag) {
    switch (flag) {
        case 'r':
            flag_steal_refresh = true;
            break;
        case 'u':
            flag_steal_upgrade = true;
            break;
        case 'q':
            flag_steal_quiet = true;
            break;
        case 'w':
            flag_steal_wash = true;
            break;
        default:
            printf("\x1b[31mError: Unknown flag '%c'\n\x1b[0m", flag);
            exit(1);
    }
}

static inline void grab_yeet_flags(char flag) {
    switch (flag) {
        case 'c':
            flag_yeet_cascase = true;
            break;
        case 'r':
            flag_yeet_recursive = true;
            break;
        case 'u':
            flag_yeet_unneeded = true;
            break;
        default:
            printf("\x1b[31mError: Unknown flag '%c'\n\x1b[0m", flag);
            exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("%s", usage);
        return 1;
    }

    char *command = argv[1];
    check_is_valid_command(command);
    int entry_num = -1; // From which index the subjects in argv start, TODO: Cannot handle multiple subjects

    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; ++j) {
                switch (argv[i][j]) {
                    case 'y':
                        flag_yes = true;
                        break;
                    case 'v':
                        flag_verbose = true;
                        break;
                    default:
                        if (strcmp(command, "steal") == 0)
                            grab_steal_flags(argv[i][j]);
                        else if (strcmp(command, "yeet") == 0)
                            grab_yeet_flags(argv[i][j]);
                }
            }
        } else {
            entry_num = i;
            break;
        }
    }

    int status;
    char *com;

    if (strcmp(command, "steal") == 0) {
        snprintf(com, 32, "pacman -S%s%s%s%s %s",
            flag_steal_refresh ? "yy" : "",
            flag_steal_upgrade ? "u" : "",
            flag_steal_quiet ? "q" : "",
            flag_steal_wash ? "c" : "",
            entry_num == -1 ? "" : argv[entry_num]
        );
    } else if (strcmp(command, "yeet") == 0) {
        snprintf(com, 32, "pacman -Rn%s%s%s %s",
            flag_yeet_cascase ? "c" : "",
            flag_yeet_recursive ? "s" : "",
            flag_yeet_unneeded ? "u" : "",
            entry_num == -1 ? "" : argv[entry_num]
        );
    }

    status = system(com);

    if (status != 0) {
        return 1;
    }

    return 0;
}
