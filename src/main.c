#include "config.h"
#include "core.h"
#include "venv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

#define PYPMC_VERSION "0.1.0"

/* ── ANSI colors ─────────────────────────────────────────────────── */

#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_CYAN "\033[36m"
#define C_BOLD "\033[1m"
#define C_RESET "\033[0m"

/* ── helpers ─────────────────────────────────────────────────────── */

#ifdef _WIN32
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
static void enable_ansi(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode))
        return;
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}
#endif

static void prompt_input(const char *prompt, const char *default_val,
                         char *out, int max) {
    if (default_val && *default_val)
        printf("%s (%s): ", prompt, default_val);
    else
        printf("%s: ", prompt);
    fflush(stdout);

    if (!fgets(out, max, stdin))
        out[0] = '\0';

    /* trim newline */
    size_t len = strlen(out);
    if (len > 0 && out[len - 1] == '\n')
        out[--len] = '\0';
    if (len > 0 && out[len - 1] == '\r')
        out[--len] = '\0';

    /* use default if empty */
    if (out[0] == '\0' && default_val) {
        strncpy(out, default_val, (size_t)max - 1);
        out[max - 1] = '\0';
    }
}

static int confirm(const char *prompt) {
    printf("%s [y/N]: ", prompt);
    fflush(stdout);
    char buf[16];
    if (!fgets(buf, sizeof(buf), stdin))
        return 0;
    return (buf[0] == 'y' || buf[0] == 'Y');
}

static int has_flag(int argc, char **argv, const char *flag) {
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0)
            return 1;
    }
    return 0;
}

static const char *get_option(int argc, char **argv, const char *name) {
    for (int i = 2; i < argc - 1; i++) {
        if (strcmp(argv[i], name) == 0)
            return argv[i + 1];
    }
    return NULL;
}

static void print_usage(void) {
    printf("pypmc " PYPMC_VERSION " — An npm-like package manager for Python "
           "(C edition)\n\n");
    printf("Usage: pypmc <command> [options]\n\n");
    printf("Commands:\n");
    printf("  init                  Initialize a new project\n");
    printf("  install [packages..]  Install packages (or all from "
           "package.yml)\n");
    printf("  uninstall <packages>  Uninstall packages\n");
    printf("  list                  List project dependencies\n");
    printf("  run <script>          Run a script from package.yml\n");
    printf("  activate              Show venv activation command\n");
    printf("  i [packages..]       Alias for install\n");
    printf("  rm <packages>        Alias for uninstall\n\n");
    printf("Options:\n");
    printf("  --version             Show version\n");
    printf("  --help, -h            Show this help\n");
    printf("  -y                    Accept all defaults (init)\n");
    printf("  -D, --save-dev        Save as dev dependency (install)\n");
    printf("  --name <name>         Project name (init)\n");
    printf("  --description <desc>  Project description (init)\n");
    printf("  --author <author>     Project author (init)\n");
    printf("  --license <license>   Project license (init)\n");
    printf("  --python <version>    Python version spec (init)\n");
}

/* ── command handlers ────────────────────────────────────────────── */

static int cmd_init(int argc, char **argv) {
    char cwd[MAX_PATH_LEN];
    if (!getcwd(cwd, sizeof(cwd))) {
        fprintf(stderr, "Failed to get current directory\n");
        return 1;
    }

    if (package_yml_exists(cwd)) {
        printf(C_YELLOW "package.yml already exists." C_RESET "\n");
        if (!confirm("Overwrite?"))
            return 0;
    }

    /* extract directory basename for default name */
    char default_name[MAX_NAME];
    const char *base = strrchr(cwd, '/');
    if (!base)
        base = strrchr(cwd, '\\');
    if (base)
        strncpy(default_name, base + 1, MAX_NAME - 1);
    else
        strncpy(default_name, cwd, MAX_NAME - 1);
    default_name[MAX_NAME - 1] = '\0';

    int yes = has_flag(argc, argv, "-y");

    char name[MAX_NAME], version[MAX_VER], description[MAX_STR];
    char author[MAX_NAME], license_str[MAX_VER], python[MAX_VER];

    const char *opt;

    if (yes) {
        opt = get_option(argc, argv, "--name");
        strncpy(name, opt ? opt : default_name, MAX_NAME - 1);
        opt = get_option(argc, argv, "--version");
        strncpy(version, opt ? opt : "1.0.0", MAX_VER - 1);
        opt = get_option(argc, argv, "--description");
        strncpy(description, opt ? opt : "", MAX_STR - 1);
        opt = get_option(argc, argv, "--author");
        strncpy(author, opt ? opt : "", MAX_NAME - 1);
        opt = get_option(argc, argv, "--license");
        strncpy(license_str, opt ? opt : "MIT", MAX_VER - 1);
        opt = get_option(argc, argv, "--python");
        strncpy(python, opt ? opt : ">=3.8", MAX_VER - 1);
    } else {
        opt = get_option(argc, argv, "--name");
        if (opt)
            strncpy(name, opt, MAX_NAME - 1);
        else
            prompt_input("Project name", default_name, name, MAX_NAME);

        opt = get_option(argc, argv, "--version");
        if (opt)
            strncpy(version, opt, MAX_VER - 1);
        else
            prompt_input("Version", "1.0.0", version, MAX_VER);

        opt = get_option(argc, argv, "--description");
        if (opt)
            strncpy(description, opt, MAX_STR - 1);
        else
            prompt_input("Description", "", description, MAX_STR);

        opt = get_option(argc, argv, "--author");
        if (opt)
            strncpy(author, opt, MAX_NAME - 1);
        else
            prompt_input("Author", "", author, MAX_NAME);

        opt = get_option(argc, argv, "--license");
        if (opt)
            strncpy(license_str, opt, MAX_VER - 1);
        else
            prompt_input("License", "MIT", license_str, MAX_VER);

        opt = get_option(argc, argv, "--python");
        if (opt)
            strncpy(python, opt, MAX_VER - 1);
        else
            prompt_input("Python version", ">=3.8", python, MAX_VER);
    }

    init_project(cwd, name, version, description, author, license_str, python);
    printf(C_GREEN C_BOLD "\nProject '%s' initialized successfully!" C_RESET
                          "\n",
           name);
    return 0;
}

static int cmd_install(int argc, char **argv) {
    char cwd[MAX_PATH_LEN];
    if (!getcwd(cwd, sizeof(cwd)))
        return 1;

    int dev = has_flag(argc, argv, "-D") || has_flag(argc, argv, "--save-dev");

    /* collect package names (skip flags and their values) */
    int pkg_count = 0;
    const char *packages[256];

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-D") == 0 || strcmp(argv[i], "--save-dev") == 0)
            continue;
        packages[pkg_count++] = argv[i];
    }

    if (pkg_count == 0) {
        return install_all(cwd);
    }

    int ret = 0;
    for (int i = 0; i < pkg_count; i++) {
        if (install_package(cwd, packages[i], dev) != 0)
            ret = 1;
    }
    return ret;
}

static int cmd_uninstall(int argc, char **argv) {
    char cwd[MAX_PATH_LEN];
    if (!getcwd(cwd, sizeof(cwd)))
        return 1;

    if (!package_yml_exists(cwd)) {
        printf(C_RED "No package.yml found. Run 'pypmc init' first." C_RESET
                     "\n");
        return 1;
    }

    if (argc < 3) {
        fprintf(stderr, "Usage: pypmc uninstall <package> [package...]\n");
        return 1;
    }

    int ret = 0;
    for (int i = 2; i < argc; i++) {
        if (uninstall_package(cwd, argv[i]) != 0)
            ret = 1;
    }
    return ret;
}

static int cmd_list(void) {
    char cwd[MAX_PATH_LEN];
    if (!getcwd(cwd, sizeof(cwd)))
        return 1;
    list_packages(cwd);
    return 0;
}

static int cmd_run(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: pypmc run <script>\n");
        return 1;
    }

    char cwd[MAX_PATH_LEN];
    if (!getcwd(cwd, sizeof(cwd)))
        return 1;

    if (!package_yml_exists(cwd)) {
        printf(C_RED "No package.yml found. Run 'pypmc init' first." C_RESET
                     "\n");
        return 1;
    }

    ensure_venv(cwd);
    return run_script(cwd, argv[2]);
}

static int cmd_activate(void) {
    char cwd[MAX_PATH_LEN];
    if (!getcwd(cwd, sizeof(cwd)))
        return 1;

    if (!venv_exists(cwd)) {
        printf(C_RED "No .venv found. Run 'pypmc init' first." C_RESET "\n");
        return 1;
    }

    printf("To activate in your current shell, run:\n");
    printf(C_CYAN "  %s" C_RESET "\n", get_activate_command(cwd));
    return 0;
}

/* ── main ────────────────────────────────────────────────────────── */

int main(int argc, char **argv) {
#ifdef _WIN32
    enable_ansi();
#endif

    if (argc < 2) {
        print_usage();
        return 0;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "--version") == 0 || strcmp(cmd, "-V") == 0) {
        printf("pypmc %s\n", PYPMC_VERSION);
        return 0;
    }

    if (strcmp(cmd, "--help") == 0 || strcmp(cmd, "-h") == 0) {
        print_usage();
        return 0;
    }

    if (strcmp(cmd, "init") == 0)
        return cmd_init(argc, argv);

    if (strcmp(cmd, "install") == 0 || strcmp(cmd, "i") == 0)
        return cmd_install(argc, argv);

    if (strcmp(cmd, "uninstall") == 0 || strcmp(cmd, "rm") == 0)
        return cmd_uninstall(argc, argv);

    if (strcmp(cmd, "list") == 0)
        return cmd_list();

    if (strcmp(cmd, "run") == 0)
        return cmd_run(argc, argv);

    if (strcmp(cmd, "activate") == 0)
        return cmd_activate();

    fprintf(stderr, "Unknown command: %s\n", cmd);
    fprintf(stderr, "Run 'pypmc --help' for usage.\n");
    return 1;
}
