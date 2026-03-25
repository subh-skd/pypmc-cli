#ifndef PYPMC_CORE_H
#define PYPMC_CORE_H

/* High-level package management operations */
void init_project(const char *project_dir, const char *name,
                  const char *version, const char *description,
                  const char *author, const char *license_str,
                  const char *python);
int install_package(const char *project_dir, const char *package_spec,
                    int dev);
int install_all(const char *project_dir);
int uninstall_package(const char *project_dir, const char *package_name);
int update_package(const char *project_dir, const char *package_name);
int update_all(const char *project_dir);
void list_packages(const char *project_dir);
void outdated_packages(const char *project_dir);
int run_script(const char *project_dir, const char *script_name);

/* Shell completions */
void generate_completion_bash(void);
void generate_completion_zsh(void);
void generate_completion_fish(void);

#endif
