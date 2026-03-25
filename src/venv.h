#ifndef PYPMC_VENV_H
#define PYPMC_VENV_H

#include <stddef.h>
#include "config.h"

/* Path helpers */
void get_venv_path(const char *project_dir, char *out, size_t max);
void get_python_exe(const char *project_dir, char *out, size_t max);
void get_pip_exe(const char *project_dir, char *out, size_t max);

/* Venv operations */
int venv_exists(const char *project_dir);
int create_venv(const char *project_dir);
void ensure_venv(const char *project_dir);

/* Pip operations */
int run_pip(const char *project_dir, const char *args);
int run_pip_capture(const char *project_dir, const char *args, char *output,
                    int max_len);

/* Activate helpers */
const char *get_activate_command(const char *project_dir);

#endif
