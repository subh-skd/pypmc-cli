#ifndef PYPMC_CONFIG_H
#define PYPMC_CONFIG_H

#define MAX_DEPS 256
#define MAX_SCRIPTS 64
#define MAX_STR 512
#define MAX_NAME 128
#define MAX_VER 64
#define MAX_PATH_LEN 4096
#define MAX_CMD 8192
#define MAX_OUTPUT (1024 * 256)

typedef struct
{
    char name[MAX_NAME];
    char version[MAX_VER];
} Dependency;

typedef struct
{
    char name[MAX_NAME];
    char command[MAX_STR];
} Script;

typedef struct
{
    char name[MAX_NAME];
    char version[MAX_VER];
    char description[MAX_STR];
    char author[MAX_NAME];
    char license[MAX_VER];
    char python[MAX_VER];

    Script scripts[MAX_SCRIPTS];
    int script_count;

    Dependency deps[MAX_DEPS];
    int dep_count;

    Dependency dev_deps[MAX_DEPS];
    int dev_dep_count;
} PackageYml;

typedef struct
{
    int lockfile_version;
    Dependency packages[MAX_DEPS];
    int package_count;
} LockYml;

/* package.yml operations */
int package_yml_exists(const char *project_dir);
int read_package_yml(const char *project_dir, PackageYml *pkg);
int write_package_yml(const char *project_dir, const PackageYml *pkg);
void init_default_package_yml(PackageYml *pkg, const char *name,
                              const char *version, const char *description,
                              const char *author, const char *license_str,
                              const char *python);
int add_dependency(const char *project_dir, const char *name,
                   const char *version, int dev);
int remove_dependency(const char *project_dir, const char *name);
int get_all_dependencies(const char *project_dir, Dependency *out, int *count);

/* package-lock.yml operations */
int read_lock(const char *project_dir, LockYml *lock);
int write_lock(const char *project_dir, const LockYml *lock);
int update_lock(const char *project_dir, const Dependency *packages, int count);

#endif
