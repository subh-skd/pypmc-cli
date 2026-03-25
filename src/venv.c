#include "venv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <process.h>
#define access _access
#define F_OK 0
/* MinGW may not declare _popen/_pclose without _POSIX */
FILE *_popen(const char *command, const char *mode);
int _pclose(FILE *stream);
#define popen _popen
#define pclose _pclose
#define VENV_BIN "Scripts"
#define PYTHON_EXE "python.exe"
#define PIP_EXE "pip.exe"
#else
#include <unistd.h>
#define VENV_BIN "bin"
#define PYTHON_EXE "python"
#define PIP_EXE "pip"
#endif

#define VENV_DIR ".venv"

void get_venv_path(const char *project_dir, char *out, size_t max) {
    snprintf(out, max, "%s/%s", project_dir, VENV_DIR);
}

void get_python_exe(const char *project_dir, char *out, size_t max) {
    snprintf(out, max, "%s/%s/%s/%s", project_dir, VENV_DIR, VENV_BIN,
             PYTHON_EXE);
}

void get_pip_exe(const char *project_dir, char *out, size_t max) {
    snprintf(out, max, "%s/%s/%s/%s", project_dir, VENV_DIR, VENV_BIN,
             PIP_EXE);
}

int venv_exists(const char *project_dir) {
    char exe[MAX_PATH_LEN];
    get_python_exe(project_dir, exe, sizeof(exe));
    return access(exe, F_OK) == 0;
}

int create_venv(const char *project_dir) {
    if (venv_exists(project_dir))
        return 0;

    char cmd[MAX_CMD];
    snprintf(cmd, sizeof(cmd), "python -m venv \"%s/%s\"", project_dir,
             VENV_DIR);
    return system(cmd);
}

void ensure_venv(const char *project_dir) {
    if (!venv_exists(project_dir))
        create_venv(project_dir);
}

int run_pip(const char *project_dir, const char *args) {
    char pip[MAX_PATH_LEN];
    get_pip_exe(project_dir, pip, sizeof(pip));

    char cmd[MAX_CMD];
    snprintf(cmd, sizeof(cmd), "\"%s\" %s", pip, args);
    return system(cmd);
}

int run_pip_capture(const char *project_dir, const char *args, char *output,
                    int max_len) {
    char pip[MAX_PATH_LEN];
    get_pip_exe(project_dir, pip, sizeof(pip));

    char cmd[MAX_CMD];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "\"%s\" %s 2>NUL", pip, args);
#else
    snprintf(cmd, sizeof(cmd), "\"%s\" %s 2>/dev/null", pip, args);
#endif

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        output[0] = '\0';
        return -1;
    }

    int total = 0;
    while (total < max_len - 1) {
        int c = fgetc(fp);
        if (c == EOF)
            break;
        output[total++] = (char)c;
    }
    output[total] = '\0';

    int status = pclose(fp);
    return status;
}

const char *get_activate_command(const char *project_dir) {
    static char cmd[MAX_PATH_LEN];

#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "%s/%s/Activate.ps1", project_dir, VENV_DIR);
#else
    const char *shell = getenv("SHELL");
    if (!shell)
        shell = "/bin/bash";

    if (strstr(shell, "fish"))
        snprintf(cmd, sizeof(cmd), "source %s/%s/bin/activate.fish",
                 project_dir, VENV_DIR);
    else if (strstr(shell, "csh"))
        snprintf(cmd, sizeof(cmd), "source %s/%s/bin/activate.csh",
                 project_dir, VENV_DIR);
    else
        snprintf(cmd, sizeof(cmd), "source %s/%s/bin/activate", project_dir,
                 VENV_DIR);
#endif

    return cmd;
}
