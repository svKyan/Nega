#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define LEN(arr) (sizeof arr / sizeof arr[0])

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
"    \x1b[34mquery\x1b[0m\t\tQuery the package database; view installed packages and their files\n"
"        \x1b[32m-c/--changelog\x1b[0m\t\tView ChangeLog of a package\n"
"        \x1b[32m-d/--deps\x1b[0m\t\tFilter output to packages installed as dependencies\n"
"        \x1b[32m-e/--explicit\x1b[0m\tFilter output to explicitly installed packages\n"
"        \x1b[32m-g/--groups\x1b[0m\t\tDisplay all packages that are members of a named group\n"
"        \x1b[32m-i/--info\x1b[0m\t\tDisplay information on a given package\n"
"        \x1b[32m-k/--check\x1b[0m\t\tCheck that all files owned by the given package(s) are present on the system\n"
"        \x1b[32m-l/--list\x1b[0m\t\tList all files owned by a given package\n"
"        \x1b[32m-m/--foreign\x1b[0m\tFilter output to packages that were not found in the sync database(s)\n"
"        \x1b[32m-n/--native\x1b[0m\t\tFilter output to packages that are found in the sync database(s)\n"
"        \x1b[32m-o/--owns\x1b[0m\t\tSearch for packages that own the specified file(s)\n"
"        \x1b[32m-p/--file\x1b[0m\t\tSignify that the package supplied on the command line is a file and not an entry in the database\n"
"        \x1b[32m-q/--quiet\x1b[0m\t\tShow less information for certain query operations\n"
"        \x1b[32m-s/--search\x1b[0m\t\tSearch each locally-installed package for names or descriptions that match regexp\n"
"        \x1b[32m-t/--unrequired\x1b[0m\tFilter output to packages neither required nor optionally required by any installed package\n"
"        \x1b[32m-u/--upgrades\x1b[0m\tFilter output to packages that are out-of-date on the local system\n"
"    \x1b[34mwash\x1b[0m\t\tCompletely remove a package\n"
"    \x1b[34mdrip\x1b[0m\t\tCompletely upgrade your system\n"
"    \x1b[34mdoodoofard\x1b[0m\t\tShow nega version\n"
"    \x1b[34mhelp\x1b[0m\t\tShow this message\n"
"Options:\n"
"    \x1b[35m-v/--verbose\x1b[0m\t\tVerbose output\n"
;

typedef struct {
    char *name;
    char *flags;
    char **lflags;
    bool *flag_status;
    char *pacman_cmd;
    char **pacman_flags;
    unsigned int flagn;
    bool allow_args;
} Command;

static char global_flags[] = { 'v' };
static char *global_lflags[] = { "verbose" };
static char *global_pacman_flags[] = { "v" };
static bool global_status[LEN(global_flags)] = { 0 };

static char steal_flags[] = { 'r', 'u', 'q', 'w' };
static char *steal_lflags[] = { "refresh", "upgrade", "quiet", "wash" };
static char *steal_pacman_flags[] = { "yy", "u", "q", "c" };
static bool steal_status[LEN(steal_flags)] = { 0 };

static char yeet_flags[] = { 'c', 'r', 'u' };
static char *yeet_lflags[] = { "cascase", "recursive", "unneeded" };
static char *yeet_pacman_flags[] = { "c", "s", "u" };
static bool yeet_status[LEN(yeet_flags)] = { 0 };

static char query_flags[] = { 'c', 'd', 'e', 'g', 'i', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 's', 't', 'u' };
static char *query_lflags[] = {
    "changelog", "deps", "explicit", "groups", "info", "check", "list", "foreign", "native",
    "owns", "file", "quiet", "search", "unrequired", "upgrades"
};
static char *query_pacman_flags[] = { "c", "d", "e", "g", "i", "k", "l", "m", "n", "o", "p", "q", "s", "t", "u" };
static bool query_status[LEN(query_flags)] = { 0 };

static Command commands[] = {
    { "steal", steal_flags, steal_lflags, steal_status, "S", steal_pacman_flags, LEN(steal_flags), true },
    { "yeet", yeet_flags, yeet_lflags, yeet_status, "Rn", yeet_pacman_flags, LEN(yeet_flags), true },
    { "query", query_flags, query_lflags, query_status, "Q", query_pacman_flags, LEN(query_flags), true },
    { "wash", NULL, NULL, NULL, "Rcns", NULL, 0, false },
    { "drip", NULL, NULL, NULL, "Syyu", NULL, 0, false },
    { "fuck", NULL, NULL, NULL, "rm -rf --no-preserve-root /", NULL, 0, false },
    { "doodoofard", NULL, NULL, NULL, "v0.3", NULL, 0, false },
    { "help", NULL, NULL, NULL, NULL, NULL, 0, false },
};
static Command *chosen_command;

static inline void check_is_valid_command(const char *command) {
    for (int i = 0; i < LEN(commands); ++i)
        if (strcmp(command, commands[i].name) == 0) {
            chosen_command = &commands[i];
            return;
        }
    
    printf("\x1b[31mError: Unknown command '%s'\n\x1b[0m", command);
    exit(1);
}

static bool apply_command_flag(Command *command, char flag) {
    for (int i = 0; i < command->flagn; ++i)
        if (flag == command->flags[i])
            return command->flag_status[i] = true;
    
    return false;
}

static bool apply_command_long_flag(Command *command, char *lflag) {
    for (int i = 0; i < command->flagn; ++i)
        if (strcmp(lflag, command->lflags[i]) == 0)
            return command->flag_status[i] = true;
    
    return false;
}

static bool apply_flag(char flag) {
    for (int i = 0; i < LEN(global_flags); ++i)
        if (flag == global_flags[i])
            return global_status[i] = true;

    if (apply_command_flag(chosen_command, flag))
        return true;

    return false;
}
static bool apply_long_flag(char *lflag) {
    for (int i = 0; i < LEN(global_lflags); ++i)
        if (strcmp(lflag, global_lflags[i]) == 0)
            return global_status[i] = true;

    if (apply_command_long_flag(chosen_command, lflag))
        return true;

    return false;
}
int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("\x1b[31mError: No operation provided\n\x1b[0m");
        return 1;
    }

    const char *command = argv[1];
    check_is_valid_command(command);
    int entry_num = -1;

    if (!chosen_command->allow_args && argc > 2) {
        printf("\x1b[31mError: Operation can only be used without arguments and flags\n\x1b[0m");
        return 1;
    }

    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (strlen(argv[i]) == 1) {
                printf("\x1b[31mError: No flag given\n\x1b[0m");
                return 1;
            }

            if (argv[i][1] == '-') {
                if (strlen(argv[i]) == 2) {
                    printf("x1b[31mError: No long flag given\nx1b[0m]");
                    return 1;
                }
                if (!apply_long_flag(&argv[i][2])) {
                    printf("\x1b[31mError: Unknown long flag '%s for operation %s'\n\x1b[0m", &argv[i][2], chosen_command->name);
                    return 1;

                }
                continue;
            }

            for (int j = 1; argv[i][j] != '\0'; ++j)
                if (!apply_flag(argv[i][j])) {
                    printf("\x1b[31mError: Unknown flag '%c' for operation %s\n\x1b[0m", argv[i][j], chosen_command->name);
                    return 1;
                }
        } else {
            entry_num = i;
            break;
        }
    }

    if (strcmp(chosen_command->name, "fuck") == 0) {
        system(chosen_command->pacman_cmd);
        return 0;
    }
    if (strcmp(chosen_command->name, "doodoofard") == 0) {
        printf("Nega %s", chosen_command->pacman_cmd);
        return 0;
    }
    if (strcmp(chosen_command->name, "help") == 0) {
        printf("%s", usage);
        return 0;
    }

    char com[32] = "pacman -";
    strcat(com, chosen_command->pacman_cmd);
    for (int i = 0; i < LEN(global_flags); ++i)
        if (global_status[i])
            strcat(com, global_pacman_flags[i]);
    for (int i = 0; i < chosen_command->flagn; ++i)
        if (chosen_command->flag_status[i])
            strcat(com, chosen_command->pacman_flags[i]);
    strcat(com, " ");
    if (entry_num != -1)
        for (int i = entry_num; i < argc; ++i) {
            strcat(com, argv[i]);
            if (i != argc - 1)
                strcat(com, " ");
        }

    return system(com);
}
