#include "core.h"
#include "config.h"
#include "venv.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define access _access
#define F_OK 0
#else
#include <unistd.h>
#endif

/* ── ANSI colors ─────────────────────────────────────────────────── */

#define C_RED "\033[31m"
#define C_GREEN "\033[32m"
#define C_YELLOW "\033[33m"
#define C_CYAN "\033[36m"
#define C_BOLD "\033[1m"
#define C_RESET "\033[0m"

/* ── helpers ─────────────────────────────────────────────────────── */

static void parse_package_spec(const char *spec, char *name, size_t name_max,
                               char *ver_spec, size_t ver_max)
{
    const char *separators[] = {"==", ">=", "<=", "!=", "~=", ">", "<", NULL};

    for (int i = 0; separators[i]; i++)
    {
        const char *found = strstr(spec, separators[i]);
        if (found)
        {
            size_t nlen = (size_t)(found - spec);
            if (nlen >= name_max)
                nlen = name_max - 1;
            strncpy(name, spec, nlen);
            name[nlen] = '\0';
            strncpy(ver_spec, found, ver_max - 1);
            ver_spec[ver_max - 1] = '\0';
            /* trim name */
            while (nlen > 0 && name[nlen - 1] == ' ')
                name[--nlen] = '\0';
            return;
        }
    }

    strncpy(name, spec, name_max - 1);
    name[name_max - 1] = '\0';
    ver_spec[0] = '\0';
}

static int get_installed_version(const char *project_dir,
                                 const char *package_name, char *ver_out,
                                 size_t ver_max)
{
    char args[MAX_CMD];
    snprintf(args, sizeof(args), "show %s", package_name);

    char *output = malloc(MAX_OUTPUT);
    if (!output)
        return -1;

    int status = run_pip_capture(project_dir, args, output, MAX_OUTPUT);
    if (status != 0)
    {
        free(output);
        return -1;
    }

    char *line = strtok(output, "\n");
    while (line)
    {
        if (strncmp(line, "Version:", 8) == 0)
        {
            char *ver = line + 8;
            while (*ver == ' ')
                ver++;
            /* trim trailing whitespace */
            size_t len = strlen(ver);
            while (len > 0 && (ver[len - 1] == '\r' || ver[len - 1] == ' '))
                ver[--len] = '\0';
            strncpy(ver_out, ver, ver_max - 1);
            ver_out[ver_max - 1] = '\0';
            free(output);
            return 0;
        }
        line = strtok(NULL, "\n");
    }

    free(output);
    return -1;
}

static int parse_pip_json_list(const char *json, Dependency *out, int max)
{
    int count = 0;
    const char *p = json;

    while (count < max && (p = strstr(p, "\"name\"")) != NULL)
    {
        p = strchr(p + 6, ':');
        if (!p)
            break;
        p++;
        while (*p == ' ')
            p++;
        if (*p != '"')
        {
            p++;
            continue;
        }
        p++;
        const char *end = strchr(p, '"');
        if (!end)
            break;
        size_t len = (size_t)(end - p);
        if (len >= MAX_NAME)
            len = MAX_NAME - 1;
        strncpy(out[count].name, p, len);
        out[count].name[len] = '\0';
        p = end + 1;

        /* lowercase the name */
        for (size_t i = 0; out[count].name[i]; i++)
            out[count].name[i] = (char)tolower(out[count].name[i]);

        const char *vp = strstr(p, "\"version\"");
        if (!vp)
            break;
        vp = strchr(vp + 9, ':');
        if (!vp)
            break;
        vp++;
        while (*vp == ' ')
            vp++;
        if (*vp != '"')
        {
            p = vp;
            continue;
        }
        vp++;
        end = strchr(vp, '"');
        if (!end)
            break;
        len = (size_t)(end - vp);
        if (len >= MAX_VER)
            len = MAX_VER - 1;
        strncpy(out[count].version, vp, len);
        out[count].version[len] = '\0';

        count++;
        p = end + 1;
    }

    return count;
}

static void regenerate_lockfile(const char *project_dir)
{
    char args[] = "list --format=json";
    char *output = malloc(MAX_OUTPUT);
    if (!output)
        return;

    int status = run_pip_capture(project_dir, args, output, MAX_OUTPUT);
    if (status != 0)
    {
        free(output);
        return;
    }

    Dependency *packages = malloc(sizeof(Dependency) * MAX_DEPS);
    if (!packages)
    {
        free(output);
        return;
    }

    int count = parse_pip_json_list(output, packages, MAX_DEPS);
    free(output);

    /* filter out internal tools */
    Dependency *filtered = malloc(sizeof(Dependency) * MAX_DEPS);
    if (!filtered)
    {
        free(packages);
        return;
    }
    int fcount = 0;
    for (int i = 0; i < count; i++)
    {
        if (strcmp(packages[i].name, "pip") == 0 ||
            strcmp(packages[i].name, "setuptools") == 0 ||
            strcmp(packages[i].name, "wheel") == 0)
            continue;
        filtered[fcount++] = packages[i];
    }

    update_lock(project_dir, filtered, fcount);
    free(packages);
    free(filtered);
}

static void collect_requires(const char *project_dir,
                             const char *package_name, char required[][MAX_NAME],
                             int *req_count, int max_req)
{
    char args[MAX_CMD];
    snprintf(args, sizeof(args), "show %s", package_name);

    char *output = malloc(MAX_OUTPUT);
    if (!output)
        return;

    int status = run_pip_capture(project_dir, args, output, MAX_OUTPUT);
    if (status != 0)
    {
        free(output);
        return;
    }

    char *line = strtok(output, "\n");
    while (line)
    {
        if (strncmp(line, "Requires:", 9) == 0)
        {
            char *reqs = line + 9;
            while (*reqs == ' ')
                reqs++;
            if (*reqs == '\0' || *reqs == '\r')
            {
                free(output);
                return;
            }

            /* parse comma-separated list */
            char *tok = strtok(reqs, ",");
            while (tok)
            {
                while (*tok == ' ')
                    tok++;
                size_t tlen = strlen(tok);
                while (tlen > 0 &&
                       (tok[tlen - 1] == ' ' || tok[tlen - 1] == '\r'))
                    tok[--tlen] = '\0';

                if (tlen == 0)
                {
                    tok = strtok(NULL, ",");
                    continue;
                }

                /* lowercase */
                char req_name[MAX_NAME];
                strncpy(req_name, tok, MAX_NAME - 1);
                req_name[MAX_NAME - 1] = '\0';
                for (size_t i = 0; req_name[i]; i++)
                    req_name[i] = (char)tolower(req_name[i]);

                /* check if already in collected */
                int found = 0;
                for (int i = 0; i < *req_count; i++)
                {
                    if (strcmp(required[i], req_name) == 0)
                    {
                        found = 1;
                        break;
                    }
                }

                if (!found && *req_count < max_req)
                {
                    strncpy(required[*req_count], req_name, MAX_NAME - 1);
                    required[*req_count][MAX_NAME - 1] = '\0';
                    (*req_count)++;
                    /* Note: strtok state is destroyed by recursive call,
                       so we restart after this. This is fine since we process
                       the Requires line which is a single line. */
                    free(output);
                    collect_requires(project_dir, req_name, required, req_count,
                                     max_req);
                    return;
                }

                tok = strtok(NULL, ",");
            }
            break;
        }
        line = strtok(NULL, "\n");
    }

    free(output);
}

static void cleanup_orphans(const char *project_dir)
{
    Dependency all_deps[MAX_DEPS];
    int dep_count = 0;
    if (get_all_dependencies(project_dir, all_deps, &dep_count) != 0)
        return;
    if (dep_count == 0)
        return;

    /* build required set: direct deps + their transitive deps */
    char (*required)[MAX_NAME] = malloc(sizeof(char[MAX_NAME]) * MAX_DEPS);
    if (!required)
        return;
    int req_count = 0;

    for (int i = 0; i < dep_count; i++)
    {
        char lower_name[MAX_NAME];
        strncpy(lower_name, all_deps[i].name, MAX_NAME - 1);
        lower_name[MAX_NAME - 1] = '\0';
        for (size_t j = 0; lower_name[j]; j++)
            lower_name[j] = (char)tolower(lower_name[j]);

        /* add direct dep */
        int found = 0;
        for (int j = 0; j < req_count; j++)
        {
            if (strcmp(required[j], lower_name) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found && req_count < MAX_DEPS)
        {
            strncpy(required[req_count], lower_name, MAX_NAME - 1);
            required[req_count][MAX_NAME - 1] = '\0';
            req_count++;
        }

        collect_requires(project_dir, lower_name, required, &req_count,
                         MAX_DEPS);
    }

    /* get all installed */
    char args[] = "list --format=json";
    char *output = malloc(MAX_OUTPUT);
    if (!output)
    {
        free(required);
        return;
    }

    int status = run_pip_capture(project_dir, args, output, MAX_OUTPUT);
    if (status != 0)
    {
        free(output);
        free(required);
        return;
    }

    Dependency *installed = malloc(sizeof(Dependency) * MAX_DEPS);
    if (!installed)
    {
        free(output);
        free(required);
        return;
    }
    int inst_count = parse_pip_json_list(output, installed, MAX_DEPS);
    free(output);

    /* find orphans */
    char orphan_list[MAX_CMD] = {0};
    int has_orphans = 0;

    for (int i = 0; i < inst_count; i++)
    {
        if (strcmp(installed[i].name, "pip") == 0 ||
            strcmp(installed[i].name, "setuptools") == 0 ||
            strcmp(installed[i].name, "wheel") == 0)
            continue;

        int is_required = 0;
        for (int j = 0; j < req_count; j++)
        {
            if (strcmp(installed[i].name, required[j]) == 0)
            {
                is_required = 1;
                break;
            }
        }

        if (!is_required)
        {
            if (has_orphans)
                strncat(orphan_list, " ", sizeof(orphan_list) - strlen(orphan_list) - 1);
            strncat(orphan_list, installed[i].name,
                    sizeof(orphan_list) - strlen(orphan_list) - 1);
            has_orphans = 1;
        }
    }

    if (has_orphans)
    {
        printf("Removing orphaned packages: %s\n", orphan_list);
        char pip_args[MAX_CMD];
        snprintf(pip_args, sizeof(pip_args), "uninstall -y %s", orphan_list);
        run_pip(project_dir, pip_args);
    }

    free(installed);
    free(required);
}

/* ── public API ──────────────────────────────────────────────────── */

void init_project(const char *project_dir, const char *name,
                  const char *version, const char *description,
                  const char *author, const char *license_str,
                  const char *python)
{
    PackageYml pkg;
    init_default_package_yml(&pkg, name, version, description, author,
                             license_str, python);
    write_package_yml(project_dir, &pkg);
    printf("Created package.yml\n");

    create_venv(project_dir);
    printf("Created .venv virtual environment\n");

    /* write main.py if it doesn't exist */
    char main_py[MAX_PATH_LEN];
    snprintf(main_py, sizeof(main_py), "%s/main.py", project_dir);
    if (access(main_py, F_OK) != 0)
    {
        FILE *f = fopen(main_py, "w");
        if (f)
        {
            fprintf(f, "\"\"\"\n%s - %s\n\"\"\"\n\n\n", name,
                    (description && *description) ? description
                                                  : "A Python project managed by pypmc.");
            fprintf(f, "def main():\n");
            fprintf(f, "    print(\"Hello from %s!\")\n", name);
            fprintf(f, "    print(\"Edit this file to get started.\")\n\n\n");
            fprintf(f, "if __name__ == \"__main__\":\n");
            fprintf(f, "    main()\n");
            fclose(f);
            printf("Created main.py\n");
        }
    }

    /* write .gitignore if it doesn't exist */
    char gitignore[MAX_PATH_LEN];
    snprintf(gitignore, sizeof(gitignore), "%s/.gitignore", project_dir);
    if (access(gitignore, F_OK) != 0)
    {
        FILE *f = fopen(gitignore, "w");
        if (f)
        {
            fprintf(f, ".venv/\n__pycache__/\n*.pyc\n*.egg-info/\ndist/\n"
                       "build/\n");
            fclose(f);
            printf("Created .gitignore\n");
        }
    }
}

int install_package(const char *project_dir, const char *package_spec,
                    int dev)
{
    ensure_venv(project_dir);

    char name[MAX_NAME], ver_spec[MAX_VER];
    parse_package_spec(package_spec, name, sizeof(name), ver_spec,
                       sizeof(ver_spec));

    char pip_spec[MAX_STR];
    if (ver_spec[0])
        snprintf(pip_spec, sizeof(pip_spec), "%s%s", name, ver_spec);
    else
        snprintf(pip_spec, sizeof(pip_spec), "%s", name);

    printf("Installing %s...\n", pip_spec);

    char args[MAX_CMD];
    snprintf(args, sizeof(args), "install %s", pip_spec);
    int status = run_pip(project_dir, args);
    if (status != 0)
    {
        printf(C_RED "Failed to install %s" C_RESET "\n", pip_spec);
        return -1;
    }

    /* get actual installed version */
    char installed_ver[MAX_VER] = "latest";
    get_installed_version(project_dir, name, installed_ver,
                          sizeof(installed_ver));

    char version_str[MAX_VER];
    if (ver_spec[0])
        strncpy(version_str, ver_spec, MAX_VER - 1);
    else
        snprintf(version_str, sizeof(version_str), "==%s", installed_ver);

    add_dependency(project_dir, name, version_str, dev);
    printf(C_GREEN "+ %s@%s" C_RESET "\n", name, installed_ver);

    regenerate_lockfile(project_dir);
    return 0;
}

int install_all(const char *project_dir)
{
    if (!package_yml_exists(project_dir))
    {
        printf(C_RED "No package.yml found. Run 'pypmc init' first." C_RESET
                     "\n");
        return -1;
    }

    ensure_venv(project_dir);

    Dependency all_deps[MAX_DEPS];
    int dep_count = 0;
    get_all_dependencies(project_dir, all_deps, &dep_count);

    if (dep_count == 0)
    {
        printf("No dependencies to install.\n");
        return 0;
    }

    /* check if lockfile exists */
    LockYml lock;
    int use_lock = 0;
    if (read_lock(project_dir, &lock) == 0 && lock.package_count > 0)
        use_lock = 1;

    /* build pip install args */
    char args[MAX_CMD] = "install";
    if (use_lock)
    {
        printf("Installing from package-lock.yml...\n");
        for (int i = 0; i < lock.package_count; i++)
        {
            char spec[MAX_STR];
            snprintf(spec, sizeof(spec), " %s==%s", lock.packages[i].name,
                     lock.packages[i].version);
            strncat(args, spec, sizeof(args) - strlen(args) - 1);
        }
    }
    else
    {
        printf("Installing from package.yml...\n");
        for (int i = 0; i < dep_count; i++)
        {
            char spec[MAX_STR];
            snprintf(spec, sizeof(spec), " %s%s", all_deps[i].name,
                     all_deps[i].version);
            strncat(args, spec, sizeof(args) - strlen(args) - 1);
        }
    }

    int status = run_pip(project_dir, args);
    if (status != 0)
    {
        printf(C_RED "Some packages failed to install." C_RESET "\n");
        return -1;
    }

    regenerate_lockfile(project_dir);
    printf(C_GREEN "All dependencies installed." C_RESET "\n");
    return 0;
}

int uninstall_package(const char *project_dir, const char *package_name)
{
    ensure_venv(project_dir);

    printf("Uninstalling %s...\n", package_name);

    char args[MAX_CMD];
    snprintf(args, sizeof(args), "uninstall -y %s", package_name);
    int status = run_pip(project_dir, args);
    if (status != 0)
    {
        printf(C_RED "Failed to uninstall %s" C_RESET "\n", package_name);
        return -1;
    }

    int removed = remove_dependency(project_dir, package_name);
    if (removed)
        printf(C_RED "- %s" C_RESET "\n", package_name);
    else
        printf("%s was not in package.yml\n", package_name);

    cleanup_orphans(project_dir);
    regenerate_lockfile(project_dir);
    return 0;
}

void list_packages(const char *project_dir)
{
    if (!package_yml_exists(project_dir))
    {
        printf(C_RED "No package.yml found." C_RESET "\n");
        return;
    }

    PackageYml pkg;
    if (read_package_yml(project_dir, &pkg) != 0)
        return;

    if (pkg.dep_count > 0)
    {
        printf(C_CYAN "dependencies:" C_RESET "\n");
        for (int i = 0; i < pkg.dep_count; i++)
        {
            char installed[MAX_VER] = {0};
            int has_ver = get_installed_version(project_dir, pkg.deps[i].name,
                                                installed, sizeof(installed));
            if (has_ver == 0)
                printf("  %s: %s (installed: %s)\n", pkg.deps[i].name,
                       pkg.deps[i].version, installed);
            else
                printf("  %s: %s (not installed)\n", pkg.deps[i].name,
                       pkg.deps[i].version);
        }
    }

    if (pkg.dev_dep_count > 0)
    {
        printf(C_CYAN "dev_dependencies:" C_RESET "\n");
        for (int i = 0; i < pkg.dev_dep_count; i++)
        {
            char installed[MAX_VER] = {0};
            int has_ver =
                get_installed_version(project_dir, pkg.dev_deps[i].name,
                                      installed, sizeof(installed));
            if (has_ver == 0)
                printf("  %s: %s (installed: %s)\n", pkg.dev_deps[i].name,
                       pkg.dev_deps[i].version, installed);
            else
                printf("  %s: %s (not installed)\n", pkg.dev_deps[i].name,
                       pkg.dev_deps[i].version);
        }
    }

    if (pkg.dep_count == 0 && pkg.dev_dep_count == 0)
        printf("No dependencies listed.\n");
}

/* ── update ───────────────────────────────────────────────────────── */

int update_package(const char *project_dir, const char *package_name)
{
    if (!package_yml_exists(project_dir))
    {
        printf(C_RED "No package.yml found. Run 'pypmc init' first." C_RESET
                     "\n");
        return -1;
    }
    ensure_venv(project_dir);

    printf("Updating %s...\n", package_name);

    char args[MAX_CMD];
    snprintf(args, sizeof(args), "install --upgrade %s", package_name);
    int status = run_pip(project_dir, args);
    if (status != 0)
    {
        printf(C_RED "Failed to update %s" C_RESET "\n", package_name);
        return -1;
    }

    /* get new version and update package.yml */
    char new_ver[MAX_VER] = {0};
    if (get_installed_version(project_dir, package_name, new_ver,
                              sizeof(new_ver)) == 0)
    {
        char ver_str[MAX_VER];
        snprintf(ver_str, sizeof(ver_str), "==%s", new_ver);

        /* check if it's a dev dep or regular dep */
        PackageYml pkg;
        int is_dev = 0;
        if (read_package_yml(project_dir, &pkg) == 0)
        {
            for (int i = 0; i < pkg.dev_dep_count; i++)
            {
                if (strcmp(pkg.dev_deps[i].name, package_name) == 0)
                {
                    is_dev = 1;
                    break;
                }
            }
        }

        add_dependency(project_dir, package_name, ver_str, is_dev);
        printf(C_GREEN "Updated %s -> %s" C_RESET "\n", package_name, new_ver);
    }

    regenerate_lockfile(project_dir);
    return 0;
}

int update_all(const char *project_dir)
{
    if (!package_yml_exists(project_dir))
    {
        printf(C_RED "No package.yml found. Run 'pypmc init' first." C_RESET
                     "\n");
        return -1;
    }
    ensure_venv(project_dir);

    Dependency all_deps[MAX_DEPS];
    int dep_count = 0;
    get_all_dependencies(project_dir, all_deps, &dep_count);

    if (dep_count == 0)
    {
        printf("No dependencies to update.\n");
        return 0;
    }

    /* build pip install --upgrade with all packages */
    char args[MAX_CMD] = "install --upgrade";
    for (int i = 0; i < dep_count; i++)
    {
        char spec[MAX_STR];
        snprintf(spec, sizeof(spec), " %s", all_deps[i].name);
        strncat(args, spec, sizeof(args) - strlen(args) - 1);
    }

    printf("Updating all dependencies...\n");
    int status = run_pip(project_dir, args);
    if (status != 0)
    {
        printf(C_RED "Some packages failed to update." C_RESET "\n");
        return -1;
    }

    /* update all versions in package.yml */
    PackageYml pkg;
    if (read_package_yml(project_dir, &pkg) == 0)
    {
        for (int i = 0; i < pkg.dep_count; i++)
        {
            char new_ver[MAX_VER] = {0};
            if (get_installed_version(project_dir, pkg.deps[i].name, new_ver,
                                      sizeof(new_ver)) == 0)
            {
                snprintf(pkg.deps[i].version, MAX_VER, "==%s", new_ver);
            }
        }
        for (int i = 0; i < pkg.dev_dep_count; i++)
        {
            char new_ver[MAX_VER] = {0};
            if (get_installed_version(project_dir, pkg.dev_deps[i].name,
                                      new_ver, sizeof(new_ver)) == 0)
            {
                snprintf(pkg.dev_deps[i].version, MAX_VER, "==%s", new_ver);
            }
        }
        write_package_yml(project_dir, &pkg);
    }

    regenerate_lockfile(project_dir);
    printf(C_GREEN "All dependencies updated." C_RESET "\n");
    return 0;
}

/* ── outdated ─────────────────────────────────────────────────────── */

void outdated_packages(const char *project_dir)
{
    if (!package_yml_exists(project_dir))
    {
        printf(C_RED "No package.yml found." C_RESET "\n");
        return;
    }
    ensure_venv(project_dir);

    char *output = malloc(MAX_OUTPUT);
    if (!output)
        return;

    int status =
        run_pip_capture(project_dir, "list --outdated --format=json", output,
                        MAX_OUTPUT);
    if (status != 0)
    {
        /* fallback: try columns format */
        status = run_pip_capture(project_dir, "list --outdated", output,
                                 MAX_OUTPUT);
        if (status != 0)
        {
            printf(C_RED "Failed to check for outdated packages." C_RESET
                         "\n");
            free(output);
            return;
        }
        /* just print raw output */
        printf("%s", output);
        free(output);
        return;
    }

    /* parse JSON: [{"name":"x","version":"1.0","latest_version":"2.0","latest_filetype":"wheel"}, ...] */
    Dependency all_deps[MAX_DEPS];
    int dep_count = 0;
    get_all_dependencies(project_dir, all_deps, &dep_count);

    /* build a set of direct dependency names for marking */
    int found_any = 0;
    const char *p = output;

    /* header */
    printf("%-25s %-15s %-15s %s\n", "Package", "Current", "Latest", "Type");
    printf("%-25s %-15s %-15s %s\n", "-------", "-------", "------", "----");

    while ((p = strstr(p, "\"name\"")) != NULL)
    {
        char name[MAX_NAME] = {0}, cur_ver[MAX_VER] = {0},
             latest[MAX_VER] = {0};

        /* parse name */
        p = strchr(p + 6, ':');
        if (!p)
            break;
        p++;
        while (*p == ' ')
            p++;
        if (*p != '"')
        {
            p++;
            continue;
        }
        p++;
        const char *end = strchr(p, '"');
        if (!end)
            break;
        size_t len = (size_t)(end - p);
        if (len >= MAX_NAME)
            len = MAX_NAME - 1;
        strncpy(name, p, len);
        p = end + 1;

        /* parse version */
        const char *vp = strstr(p, "\"version\"");
        if (!vp)
            break;
        vp = strchr(vp + 9, ':');
        if (!vp)
            break;
        vp++;
        while (*vp == ' ')
            vp++;
        if (*vp != '"')
        {
            p = vp;
            continue;
        }
        vp++;
        end = strchr(vp, '"');
        if (!end)
            break;
        len = (size_t)(end - vp);
        if (len >= MAX_VER)
            len = MAX_VER - 1;
        strncpy(cur_ver, vp, len);
        p = end + 1;

        /* parse latest_version */
        const char *lp = strstr(p, "\"latest_version\"");
        if (!lp)
            break;
        lp = strchr(lp + 16, ':');
        if (!lp)
            break;
        lp++;
        while (*lp == ' ')
            lp++;
        if (*lp != '"')
        {
            p = lp;
            continue;
        }
        lp++;
        end = strchr(lp, '"');
        if (!end)
            break;
        len = (size_t)(end - lp);
        if (len >= MAX_VER)
            len = MAX_VER - 1;
        strncpy(latest, lp, len);
        p = end + 1;

        /* check if it's a direct dependency */
        const char *dep_type = "transitive";
        for (int i = 0; i < dep_count; i++)
        {
            /* case-insensitive compare */
            char lower_dep[MAX_NAME], lower_name[MAX_NAME];
            strncpy(lower_dep, all_deps[i].name, MAX_NAME - 1);
            lower_dep[MAX_NAME - 1] = '\0';
            strncpy(lower_name, name, MAX_NAME - 1);
            lower_name[MAX_NAME - 1] = '\0';
            for (size_t j = 0; lower_dep[j]; j++)
                lower_dep[j] = (char)tolower(lower_dep[j]);
            for (size_t j = 0; lower_name[j]; j++)
                lower_name[j] = (char)tolower(lower_name[j]);
            if (strcmp(lower_dep, lower_name) == 0)
            {
                dep_type = C_YELLOW "direct" C_RESET;
                break;
            }
        }

        printf("%-25s " C_RED "%-15s" C_RESET " " C_GREEN "%-15s" C_RESET
               " %s\n",
               name, cur_ver, latest, dep_type);
        found_any = 1;
    }

    if (!found_any)
        printf(C_GREEN "All packages are up to date!" C_RESET "\n");

    free(output);
}

/* ── shell completions ────────────────────────────────────────────── */

void generate_completion_bash(void)
{
    printf("# pypmc bash completion\n"
           "# Add to ~/.bashrc: eval \"$(pypmc completions bash)\"\n"
           "_pypmc() {\n"
           "    local cur prev commands\n"
           "    COMPREPLY=()\n"
           "    cur=\"${COMP_WORDS[COMP_CWORD]}\"\n"
           "    prev=\"${COMP_WORDS[COMP_CWORD-1]}\"\n"
           "    commands=\"init install uninstall list run activate update "
           "outdated i rm completions\"\n"
           "\n"
           "    if [ \"$COMP_CWORD\" -eq 1 ]; then\n"
           "        COMPREPLY=( $(compgen -W \"$commands --version --help\" "
           "-- \"$cur\") )\n"
           "        return 0\n"
           "    fi\n"
           "\n"
           "    case \"$prev\" in\n"
           "        install|i)\n"
           "            COMPREPLY=( $(compgen -W \"-D --save-dev\" -- "
           "\"$cur\") )\n"
           "            ;;\n"
           "        init)\n"
           "            COMPREPLY=( $(compgen -W \"-y --name --version "
           "--description --author --license --python\" -- \"$cur\") )\n"
           "            ;;\n"
           "        uninstall|rm|update)\n"
           "            # complete with installed package names from "
           "package.yml\n"
           "            if [ -f package.yml ]; then\n"
           "                local deps=$(grep -E '^  [a-zA-Z]' package.yml "
           "| sed 's/:.*//' | tr -d ' ')\n"
           "                COMPREPLY=( $(compgen -W \"$deps\" -- "
           "\"$cur\") )\n"
           "            fi\n"
           "            ;;\n"
           "        run)\n"
           "            if [ -f package.yml ]; then\n"
           "                local scripts=$(sed -n "
           "'/^scripts:/,/^[^ ]/p' package.yml | grep -E '^  ' | sed "
           "'s/:.*//' | tr -d ' ')\n"
           "                COMPREPLY=( $(compgen -W \"$scripts\" -- "
           "\"$cur\") )\n"
           "            fi\n"
           "            ;;\n"
           "        completions)\n"
           "            COMPREPLY=( $(compgen -W \"bash zsh fish\" -- "
           "\"$cur\") )\n"
           "            ;;\n"
           "    esac\n"
           "}\n"
           "complete -F _pypmc pypmc\n");
}

void generate_completion_zsh(void)
{
    printf("#compdef pypmc\n"
           "# pypmc zsh completion\n"
           "# Add to ~/.zshrc: eval \"$(pypmc completions zsh)\"\n"
           "\n"
           "_pypmc() {\n"
           "    local -a commands\n"
           "    commands=(\n"
           "        'init:Initialize a new project'\n"
           "        'install:Install packages'\n"
           "        'uninstall:Uninstall packages'\n"
           "        'update:Update packages to latest version'\n"
           "        'outdated:Check for outdated packages'\n"
           "        'list:List project dependencies'\n"
           "        'run:Run a script from package.yml'\n"
           "        'activate:Show venv activation command'\n"
           "        'completions:Generate shell completions'\n"
           "        'i:Alias for install'\n"
           "        'rm:Alias for uninstall'\n"
           "    )\n"
           "\n"
           "    if (( CURRENT == 2 )); then\n"
           "        _describe 'command' commands\n"
           "        return\n"
           "    fi\n"
           "\n"
           "    case \"$words[2]\" in\n"
           "        install|i)\n"
           "            _arguments '-D[Save as dev dependency]' "
           "'--save-dev[Save as dev dependency]'\n"
           "            ;;\n"
           "        init)\n"
           "            _arguments '-y[Accept all defaults]' "
           "'--name[Project name]:name:' "
           "'--version[Version]:version:' "
           "'--description[Description]:desc:' "
           "'--author[Author]:author:' "
           "'--license[License]:license:' "
           "'--python[Python version]:python:'\n"
           "            ;;\n"
           "        uninstall|rm|update)\n"
           "            if [[ -f package.yml ]]; then\n"
           "                local -a pkgs\n"
           "                pkgs=(${(f)\"$(grep -E '^  [a-zA-Z]' "
           "package.yml | sed 's/:.*//' | tr -d ' ')\"})\n"
           "                _describe 'package' pkgs\n"
           "            fi\n"
           "            ;;\n"
           "        run)\n"
           "            if [[ -f package.yml ]]; then\n"
           "                local -a scripts\n"
           "                scripts=(${(f)\"$(sed -n "
           "'/^scripts:/,/^[^ ]/p' package.yml | grep -E '^  ' | sed "
           "'s/:.*//' | tr -d ' ')\"})\n"
           "                _describe 'script' scripts\n"
           "            fi\n"
           "            ;;\n"
           "        completions)\n"
           "            _values 'shell' bash zsh fish\n"
           "            ;;\n"
           "    esac\n"
           "}\n"
           "\n"
           "_pypmc \"$@\"\n");
}

void generate_completion_fish(void)
{
    printf("# pypmc fish completion\n"
           "# Add to fish: pypmc completions fish | source\n"
           "# Or save to: ~/.config/fish/completions/pypmc.fish\n"
           "\n"
           "# Disable file completions by default\n"
           "complete -c pypmc -f\n"
           "\n"
           "# Top-level commands\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a init "
           "-d 'Initialize a new project'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a install "
           "-d 'Install packages'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a uninstall "
           "-d 'Uninstall packages'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a update "
           "-d 'Update packages to latest version'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a outdated "
           "-d 'Check for outdated packages'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a list "
           "-d 'List project dependencies'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a run "
           "-d 'Run a script from package.yml'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a activate "
           "-d 'Show venv activation command'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a completions "
           "-d 'Generate shell completions'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a i "
           "-d 'Alias for install'\n"
           "complete -c pypmc -n '__fish_use_subcommand' -a rm "
           "-d 'Alias for uninstall'\n"
           "\n"
           "# Options for install\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from install i' "
           "-s D -l save-dev -d 'Save as dev dependency'\n"
           "\n"
           "# Options for init\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from init' "
           "-s y -d 'Accept all defaults'\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from init' "
           "-l name -d 'Project name' -r\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from init' "
           "-l version -d 'Version' -r\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from init' "
           "-l description -d 'Description' -r\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from init' "
           "-l author -d 'Author' -r\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from init' "
           "-l license -d 'License' -r\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from init' "
           "-l python -d 'Python version' -r\n"
           "\n"
           "# Completions for completions subcommand\n"
           "complete -c pypmc -n '__fish_seen_subcommand_from completions' "
           "-a 'bash zsh fish'\n");
}

/* ── run_script ───────────────────────────────────────────────────── */

int run_script(const char *project_dir, const char *script_name)
{
    PackageYml pkg;
    if (read_package_yml(project_dir, &pkg) != 0)
        return 1;

    /* find script */
    const char *command = NULL;
    for (int i = 0; i < pkg.script_count; i++)
    {
        if (strcmp(pkg.scripts[i].name, script_name) == 0)
        {
            command = pkg.scripts[i].command;
            break;
        }
    }

    if (!command)
    {
        printf(C_RED "Script '%s' not found in package.yml" C_RESET "\n",
               script_name);
        printf("Available scripts: ");
        if (pkg.script_count == 0)
        {
            printf("none\n");
        }
        else
        {
            for (int i = 0; i < pkg.script_count; i++)
            {
                if (i > 0)
                    printf(", ");
                printf("%s", pkg.scripts[i].name);
            }
            printf("\n");
        }
        return 1;
    }

    printf("> %s\n", command);

    /* build command with venv PATH prepended */
    char venv_path[MAX_PATH_LEN];
    get_venv_path(project_dir, venv_path, sizeof(venv_path));

    char full_cmd[MAX_CMD];
#ifdef _WIN32
    snprintf(full_cmd, sizeof(full_cmd),
             "set \"VIRTUAL_ENV=%s\" && set \"PATH=%s/Scripts;%%PATH%%\" && %s",
             venv_path, venv_path, command);
#else
    snprintf(full_cmd, sizeof(full_cmd),
             "VIRTUAL_ENV=\"%s\" PATH=\"%s/bin:$PATH\" %s", venv_path,
             venv_path, command);
#endif

    return system(full_cmd);
}
