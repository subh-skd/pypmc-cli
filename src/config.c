#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#define access _access
#define F_OK 0
#else
#include <unistd.h>
#endif

/* ── helpers ─────────────────────────────────────────────────────── */

static void trim(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' ||
                       s[len - 1] == '\n' || s[len - 1] == '\r'))
        s[--len] = '\0';
    char *start = s;
    while (*start == ' ' || *start == '\t')
        start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
}

static void trim_quotes(char *s)
{
    size_t len = strlen(s);
    if (len >= 2 &&
        ((s[0] == '"' && s[len - 1] == '"') ||
         (s[0] == '\'' && s[len - 1] == '\'')))
    {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
}

static int count_indent(const char *line)
{
    int n = 0;
    while (line[n] == ' ')
        n++;
    return n;
}

static int needs_yaml_quoting(const char *s)
{
    if (!s || !*s)
        return 1;
    if (strchr(">|&*?!%@`{[", s[0]))
        return 1;
    if (s[0] == '-' && s[1] == ' ')
        return 1;
    if (strchr(s, '=') || strchr(s, '<') || strchr(s, '>') || strchr(s, '~'))
        return 1;
    if (strstr(s, ": ") || strchr(s, '#'))
        return 1;
    return 0;
}

static void write_yaml_value(FILE *f, const char *value)
{
    if (needs_yaml_quoting(value))
        fprintf(f, "'%s'", value);
    else
        fprintf(f, "%s", value);
}

static void path_join(char *out, size_t max, const char *dir,
                      const char *file)
{
    snprintf(out, max, "%s/%s", dir, file);
}

/* ── package.yml ─────────────────────────────────────────────────── */

int package_yml_exists(const char *project_dir)
{
    char path[MAX_PATH_LEN];
    path_join(path, sizeof(path), project_dir, "package.yml");
    return access(path, F_OK) == 0;
}

int read_package_yml(const char *project_dir, PackageYml *pkg)
{
    char path[MAX_PATH_LEN];
    path_join(path, sizeof(path), project_dir, "package.yml");

    FILE *f = fopen(path, "r");
    if (!f)
        return -1;

    memset(pkg, 0, sizeof(*pkg));

    /* 0 = top-level, 1 = scripts, 2 = dependencies, 3 = dev_dependencies */
    int section = 0;
    char line[MAX_STR];

    while (fgets(line, sizeof(line), f))
    {
        /* strip trailing whitespace */
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' ||
                           line[len - 1] == ' '))
            line[--len] = '\0';

        if (len == 0 || line[0] == '#')
            continue;

        int indent = count_indent(line);
        char *content = line + indent;

        /* find first colon */
        char *colon = strchr(content, ':');
        if (!colon)
            continue;

        char key[MAX_NAME];
        size_t key_len = (size_t)(colon - content);
        if (key_len >= sizeof(key))
            key_len = sizeof(key) - 1;
        strncpy(key, content, key_len);
        key[key_len] = '\0';
        trim(key);

        char value[MAX_STR];
        strncpy(value, colon + 1, sizeof(value) - 1);
        value[sizeof(value) - 1] = '\0';
        trim(value);
        trim_quotes(value);

        if (indent == 0)
        {
            /* top-level key */
            if (strcmp(key, "scripts") == 0)
            {
                section = 1;
            }
            else if (strcmp(key, "dependencies") == 0)
            {
                section = 2;
                /* check for empty dict {} */
                if (strcmp(value, "{}") == 0)
                    section = 0;
            }
            else if (strcmp(key, "dev_dependencies") == 0)
            {
                section = 3;
                if (strcmp(value, "{}") == 0)
                    section = 0;
            }
            else
            {
                section = 0;
                if (strcmp(key, "name") == 0)
                    strncpy(pkg->name, value, MAX_NAME - 1);
                else if (strcmp(key, "version") == 0)
                    strncpy(pkg->version, value, MAX_VER - 1);
                else if (strcmp(key, "description") == 0)
                    strncpy(pkg->description, value, MAX_STR - 1);
                else if (strcmp(key, "author") == 0)
                    strncpy(pkg->author, value, MAX_NAME - 1);
                else if (strcmp(key, "license") == 0)
                    strncpy(pkg->license, value, MAX_VER - 1);
                else if (strcmp(key, "python") == 0)
                    strncpy(pkg->python, value, MAX_VER - 1);
            }
        }
        else if (indent >= 2)
        {
            switch (section)
            {
            case 1: /* scripts */
                if (pkg->script_count < MAX_SCRIPTS)
                {
                    strncpy(pkg->scripts[pkg->script_count].name, key,
                            MAX_NAME - 1);
                    strncpy(pkg->scripts[pkg->script_count].command, value,
                            MAX_STR - 1);
                    pkg->script_count++;
                }
                break;
            case 2: /* dependencies */
                if (pkg->dep_count < MAX_DEPS)
                {
                    strncpy(pkg->deps[pkg->dep_count].name, key, MAX_NAME - 1);
                    strncpy(pkg->deps[pkg->dep_count].version, value,
                            MAX_VER - 1);
                    pkg->dep_count++;
                }
                break;
            case 3: /* dev_dependencies */
                if (pkg->dev_dep_count < MAX_DEPS)
                {
                    strncpy(pkg->dev_deps[pkg->dev_dep_count].name, key,
                            MAX_NAME - 1);
                    strncpy(pkg->dev_deps[pkg->dev_dep_count].version, value,
                            MAX_VER - 1);
                    pkg->dev_dep_count++;
                }
                break;
            }
        }
    }

    fclose(f);
    return 0;
}

int write_package_yml(const char *project_dir, const PackageYml *pkg)
{
    char path[MAX_PATH_LEN];
    path_join(path, sizeof(path), project_dir, "package.yml");

    FILE *f = fopen(path, "w");
    if (!f)
        return -1;

    fprintf(f, "name: %s\n", pkg->name);
    fprintf(f, "version: %s\n", pkg->version);
    fprintf(f, "description: ");
    write_yaml_value(f, pkg->description);
    fprintf(f, "\n");
    fprintf(f, "author: ");
    write_yaml_value(f, pkg->author);
    fprintf(f, "\n");
    fprintf(f, "license: %s\n", pkg->license);
    fprintf(f, "python: ");
    write_yaml_value(f, pkg->python);
    fprintf(f, "\n");

    /* scripts */
    if (pkg->script_count > 0)
    {
        fprintf(f, "scripts:\n");
        for (int i = 0; i < pkg->script_count; i++)
            fprintf(f, "  %s: %s\n", pkg->scripts[i].name,
                    pkg->scripts[i].command);
    }
    else
    {
        fprintf(f, "scripts: {}\n");
    }

    /* dependencies */
    if (pkg->dep_count > 0)
    {
        fprintf(f, "dependencies:\n");
        for (int i = 0; i < pkg->dep_count; i++)
        {
            fprintf(f, "  %s: ", pkg->deps[i].name);
            write_yaml_value(f, pkg->deps[i].version);
            fprintf(f, "\n");
        }
    }
    else
    {
        fprintf(f, "dependencies: {}\n");
    }

    /* dev_dependencies */
    if (pkg->dev_dep_count > 0)
    {
        fprintf(f, "dev_dependencies:\n");
        for (int i = 0; i < pkg->dev_dep_count; i++)
        {
            fprintf(f, "  %s: ", pkg->dev_deps[i].name);
            write_yaml_value(f, pkg->dev_deps[i].version);
            fprintf(f, "\n");
        }
    }
    else
    {
        fprintf(f, "dev_dependencies: {}\n");
    }

    fclose(f);
    return 0;
}

void init_default_package_yml(PackageYml *pkg, const char *name,
                              const char *version, const char *description,
                              const char *author, const char *license_str,
                              const char *python)
{
    memset(pkg, 0, sizeof(*pkg));
    strncpy(pkg->name, name, MAX_NAME - 1);
    strncpy(pkg->version, version ? version : "1.0.0", MAX_VER - 1);
    if (description)
        strncpy(pkg->description, description, MAX_STR - 1);
    if (author)
        strncpy(pkg->author, author, MAX_NAME - 1);
    strncpy(pkg->license, license_str ? license_str : "MIT", MAX_VER - 1);
    strncpy(pkg->python, python ? python : ">=3.8", MAX_VER - 1);

    /* default scripts */
    strncpy(pkg->scripts[0].name, "start", MAX_NAME - 1);
    strncpy(pkg->scripts[0].command, "python main.py", MAX_STR - 1);
    strncpy(pkg->scripts[1].name, "test", MAX_NAME - 1);
    strncpy(pkg->scripts[1].command, "pytest", MAX_STR - 1);
    pkg->script_count = 2;
}

int add_dependency(const char *project_dir, const char *name,
                   const char *version, int dev)
{
    PackageYml pkg;
    if (read_package_yml(project_dir, &pkg) != 0)
        return -1;

    Dependency *arr = dev ? pkg.dev_deps : pkg.deps;
    int *count = dev ? &pkg.dev_dep_count : &pkg.dep_count;

    /* update existing or append */
    for (int i = 0; i < *count; i++)
    {
        if (strcmp(arr[i].name, name) == 0)
        {
            strncpy(arr[i].version, version, MAX_VER - 1);
            return write_package_yml(project_dir, &pkg);
        }
    }

    if (*count >= MAX_DEPS)
        return -1;
    strncpy(arr[*count].name, name, MAX_NAME - 1);
    strncpy(arr[*count].version, version, MAX_VER - 1);
    (*count)++;

    return write_package_yml(project_dir, &pkg);
}

int remove_dependency(const char *project_dir, const char *name)
{
    PackageYml pkg;
    if (read_package_yml(project_dir, &pkg) != 0)
        return 0;

    int found = 0;

    /* search in dependencies */
    for (int i = 0; i < pkg.dep_count; i++)
    {
        if (strcmp(pkg.deps[i].name, name) == 0)
        {
            memmove(&pkg.deps[i], &pkg.deps[i + 1],
                    (size_t)(pkg.dep_count - i - 1) * sizeof(Dependency));
            pkg.dep_count--;
            found = 1;
            break;
        }
    }

    /* search in dev_dependencies */
    for (int i = 0; i < pkg.dev_dep_count; i++)
    {
        if (strcmp(pkg.dev_deps[i].name, name) == 0)
        {
            memmove(&pkg.dev_deps[i], &pkg.dev_deps[i + 1],
                    (size_t)(pkg.dev_dep_count - i - 1) * sizeof(Dependency));
            pkg.dev_dep_count--;
            found = 1;
            break;
        }
    }

    if (found)
        write_package_yml(project_dir, &pkg);
    return found;
}

int get_all_dependencies(const char *project_dir, Dependency *out,
                         int *count)
{
    PackageYml pkg;
    if (read_package_yml(project_dir, &pkg) != 0)
        return -1;

    *count = 0;
    for (int i = 0; i < pkg.dep_count && *count < MAX_DEPS; i++)
        out[(*count)++] = pkg.deps[i];
    for (int i = 0; i < pkg.dev_dep_count && *count < MAX_DEPS; i++)
        out[(*count)++] = pkg.dev_deps[i];
    return 0;
}

/* ── package-lock.yml ────────────────────────────────────────────── */

int read_lock(const char *project_dir, LockYml *lock)
{
    char path[MAX_PATH_LEN];
    path_join(path, sizeof(path), project_dir, "package-lock.yml");

    memset(lock, 0, sizeof(*lock));

    FILE *f = fopen(path, "r");
    if (!f)
        return -1;

    char line[MAX_STR];
    int in_packages = 0;
    char current_pkg[MAX_NAME] = {0};

    while (fgets(line, sizeof(line), f))
    {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' ||
                           line[len - 1] == ' '))
            line[--len] = '\0';

        if (len == 0 || line[0] == '#')
            continue;

        int indent = count_indent(line);
        char *content = line + indent;

        char *colon = strchr(content, ':');
        if (!colon)
            continue;

        char key[MAX_NAME];
        size_t key_len = (size_t)(colon - content);
        if (key_len >= sizeof(key))
            key_len = sizeof(key) - 1;
        strncpy(key, content, key_len);
        key[key_len] = '\0';
        trim(key);

        char value[MAX_STR];
        strncpy(value, colon + 1, sizeof(value) - 1);
        value[sizeof(value) - 1] = '\0';
        trim(value);
        trim_quotes(value);

        if (indent == 0)
        {
            if (strcmp(key, "lockfile_version") == 0)
                lock->lockfile_version = atoi(value);
            else if (strcmp(key, "packages") == 0)
                in_packages = 1;
            else
                in_packages = 0;
        }
        else if (indent == 2 && in_packages)
        {
            /* package name (value is empty) */
            strncpy(current_pkg, key, MAX_NAME - 1);
            current_pkg[MAX_NAME - 1] = '\0';
        }
        else if (indent == 4 && in_packages && current_pkg[0])
        {
            if (strcmp(key, "version") == 0 &&
                lock->package_count < MAX_DEPS)
            {
                strncpy(lock->packages[lock->package_count].name, current_pkg,
                        MAX_NAME - 1);
                strncpy(lock->packages[lock->package_count].version, value,
                        MAX_VER - 1);
                lock->package_count++;
            }
        }
    }

    fclose(f);
    return 0;
}

int write_lock(const char *project_dir, const LockYml *lock)
{
    char path[MAX_PATH_LEN];
    path_join(path, sizeof(path), project_dir, "package-lock.yml");

    FILE *f = fopen(path, "w");
    if (!f)
        return -1;

    fprintf(f, "lockfile_version: %d\n", lock->lockfile_version);

    if (lock->package_count > 0)
    {
        fprintf(f, "packages:\n");
        for (int i = 0; i < lock->package_count; i++)
        {
            fprintf(f, "  %s:\n", lock->packages[i].name);
            fprintf(f, "    version: '%s'\n", lock->packages[i].version);
        }
    }
    else
    {
        fprintf(f, "packages: {}\n");
    }

    fclose(f);
    return 0;
}

int update_lock(const char *project_dir, const Dependency *packages,
                int count)
{
    LockYml lock;
    memset(&lock, 0, sizeof(lock));
    lock.lockfile_version = 1;
    for (int i = 0; i < count && i < MAX_DEPS; i++)
    {
        lock.packages[i] = packages[i];
        lock.package_count++;
    }
    return write_lock(project_dir, &lock);
}
