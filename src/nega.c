#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define LEN(arr) (sizeof arr / sizeof arr[0])

// TODO: Update this list and enable long flags
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

typedef struct {
    char *name;
    char *flags;
    char **lflags;
    bool *flag_status;
    char *pacman_cmd;
    char **pacman_flags;
    unsigned int flagn;
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
    { "steal", steal_flags, steal_lflags, steal_status, "S", steal_pacman_flags, LEN(steal_flags) },
    { "yeet", yeet_flags, yeet_lflags, yeet_status, "Rn", yeet_pacman_flags, LEN(yeet_flags) },
    { "query", query_flags, query_lflags, query_status, "Q", query_pacman_flags, LEN(query_flags) },
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

static bool apply_flag(char flag) {
    for (int i = 0; i < LEN(global_flags); ++i)
        if (flag == global_flags[i])
            return global_status[i] = true;

    if (apply_command_flag(chosen_command, flag))
        return true;

    return false;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("%s", usage);
        return 1;
    }

    const char *command = argv[1];
    check_is_valid_command(command);
    int entry_num = -1;

    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; ++j)
                if (!apply_flag(argv[i][j])) {
                    printf("\x1b[31mError: Unknown flag '%c'\n\x1b[0m", argv[i][j]);
                    return 1;
                }
        } else {
            entry_num = i;
            break;
        }
    }

    int status;
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
        strcat(com, argv[entry_num]);

    printf("Executing %s", com);

    return 0;
}
