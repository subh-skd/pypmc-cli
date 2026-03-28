import { CodeBlock } from "../CodeBlock";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";

export function VirtualEnvironments() {
  return (
    <section id="virtual-environments" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight mb-2">
        Virtual Environments
      </h2>
      <p className="text-muted-foreground mb-6">
        PyPMC automatically creates and manages an isolated{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">.venv</code>{" "}
        directory for every project using Python's built-in{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">venv</code>{" "}
        module.
      </p>

      <h3 className="text-base font-semibold mb-3">How it works</h3>
      <ul className="list-disc list-inside space-y-2 text-sm text-muted-foreground mb-6">
        <li>
          Created via{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            python -m venv .venv
          </code>{" "}
          during{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            pypmc init
          </code>{" "}
          or the first{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            pypmc install
          </code>
          .
        </li>
        <li>
          All <code className="bg-muted px-1 py-0.5 rounded text-xs">pip</code>{" "}
          operations run through the venv's own pip executable, ensuring
          complete isolation.
        </li>
        <li>
          On <strong className="text-foreground">Unix</strong>: binaries live in{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            .venv/bin/
          </code>
          .
        </li>
        <li>
          On <strong className="text-foreground">Windows</strong>: binaries live
          in{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            .venv/Scripts/
          </code>
          .
        </li>
      </ul>

      <h3 className="text-base font-semibold mb-3">Manual activation</h3>
      <p className="text-sm text-muted-foreground mb-3">
        While{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">pypmc run</code>{" "}
        automatically uses the venv, you can also activate it manually for
        interactive use:
      </p>
      <Tabs defaultValue="bash">
        <TabsList>
          <TabsTrigger value="bash">bash / zsh</TabsTrigger>
          <TabsTrigger value="fish">fish</TabsTrigger>
          <TabsTrigger value="ps">PowerShell</TabsTrigger>
          <TabsTrigger value="cmd">CMD</TabsTrigger>
        </TabsList>
        <TabsContent value="bash">
          <CodeBlock code={`source .venv/bin/activate`} language="bash" />
        </TabsContent>
        <TabsContent value="fish">
          <CodeBlock code={`source .venv/bin/activate.fish`} language="fish" />
        </TabsContent>
        <TabsContent value="ps">
          <CodeBlock
            code={`.venv\\Scripts\\Activate.ps1`}
            language="powershell"
          />
        </TabsContent>
        <TabsContent value="cmd">
          <CodeBlock code={`.venv\\Scripts\\activate.bat`} language="batch" />
        </TabsContent>
      </Tabs>

      <div className="mt-6 rounded-lg border border-border bg-muted/30 p-4">
        <p className="text-sm text-muted-foreground">
          <strong className="text-foreground">Tip:</strong> The{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">.venv</code>{" "}
          directory is included in the generated{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            .gitignore
          </code>{" "}
          &mdash; never commit it to version control. Use{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            pypmc install
          </code>{" "}
          to recreate it from the lockfile.
        </p>
      </div>
    </section>
  );
}

export function DependencyResolution() {
  return (
    <section id="dependency-resolution" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight mb-2">
        Dependency Resolution
      </h2>
      <p className="text-muted-foreground mb-6">
        How PyPMC manages direct dependencies, transitive dependencies, and
        orphan cleanup.
      </p>

      <h3 className="text-base font-semibold mb-3">Direct vs. transitive</h3>
      <ul className="list-disc list-inside space-y-2 text-sm text-muted-foreground mb-6">
        <li>
          <strong className="text-foreground">Direct dependencies</strong>{" "}
          &mdash; packages you explicitly install. Listed in{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            dependencies
          </code>{" "}
          or{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            dev_dependencies
          </code>
          .
        </li>
        <li>
          <strong className="text-foreground">Transitive dependencies</strong>{" "}
          &mdash; packages required by your direct dependencies. Tracked in the
          lockfile but not in package.yml.
        </li>
      </ul>

      <h3 className="text-base font-semibold mb-3">Orphan cleanup</h3>
      <p className="text-sm text-muted-foreground leading-relaxed mb-4">
        When you uninstall a package, PyPMC performs a full dependency graph
        walk:
      </p>
      <ol className="list-decimal list-inside space-y-2 text-sm text-muted-foreground mb-4">
        <li>
          Removes the requested package via{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            pip uninstall
          </code>
          .
        </li>
        <li>
          Builds a "required set" by recursively collecting all transitive
          dependencies of the remaining direct dependencies (using{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">pip show</code>{" "}
          to read{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            Requires:
          </code>{" "}
          metadata).
        </li>
        <li>
          Queries all currently installed packages via{" "}
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            pip list --format=json
          </code>
          .
        </li>
        <li>
          Removes any installed package that is not in the required set and is
          not a direct dependency.
        </li>
      </ol>
      <p className="text-sm text-muted-foreground">
        This prevents dependency bloat from accumulating over time.
      </p>

      <h3 className="text-base font-semibold mt-6 mb-3">Lockfile priority</h3>
      <p className="text-sm text-muted-foreground leading-relaxed">
        When running{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">
          pypmc install
        </code>{" "}
        without arguments, PyPMC checks for a lockfile first. If one exists,
        each package is installed at its locked version (
        <code className="bg-muted px-1 py-0.5 rounded text-xs">==X.Y.Z</code>).
        If no lockfile exists, versions from package.yml are used instead. This
        ensures teammates and CI always get the same environment.
      </p>
    </section>
  );
}

export function BuildFromSource() {
  return (
    <section id="build-from-source" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight mb-2">
        Build from Source
      </h2>
      <p className="text-muted-foreground mb-6">
        PyPMC is a single C program with no external library dependencies. You
        only need a C compiler and either CMake or Make.
      </p>

      <h3 className="text-base font-semibold mb-3">Prerequisites</h3>
      <ul className="list-disc list-inside space-y-1 text-sm text-muted-foreground mb-6">
        <li>A C99-compatible compiler (gcc, clang, or MSVC)</li>
        <li>CMake 3.10+ or GNU Make</li>
      </ul>

      <Tabs defaultValue="cmake">
        <TabsList>
          <TabsTrigger value="cmake">CMake</TabsTrigger>
          <TabsTrigger value="make">Make</TabsTrigger>
        </TabsList>
        <TabsContent value="cmake">
          <CodeBlock
            code={`git clone https://github.com/subh-skd/pypmc-cli.git\ncd pypmc-cli\n\ncmake -B build -DCMAKE_BUILD_TYPE=Release\ncmake --build build --config Release\n\n# Binary is at:\n#   build/pypmc          (Linux / macOS)\n#   build/Release/pypmc.exe  (Windows)`}
            language="bash"
          />
        </TabsContent>
        <TabsContent value="make">
          <CodeBlock
            code={`git clone https://github.com/subh-skd/pypmc-cli.git\ncd pypmc-cli\n\nmake\n\n# Binary is at: build/pypmc`}
            language="bash"
          />
        </TabsContent>
      </Tabs>

      <h3 className="text-base font-semibold mt-8 mb-3">
        Source file overview
      </h3>
      <div className="overflow-x-auto">
        <table className="w-full text-sm border border-border rounded-lg overflow-hidden">
          <thead>
            <tr className="bg-muted/50">
              <th className="text-left px-4 py-2 font-medium">File</th>
              <th className="text-left px-4 py-2 font-medium">Purpose</th>
            </tr>
          </thead>
          <tbody>
            {[
              [
                "src/main.c",
                "CLI entry point, command dispatcher, user prompts",
              ],
              [
                "src/core.c",
                "Package management logic (install, uninstall, update, orphan cleanup)",
              ],
              [
                "src/config.c",
                "YAML parser/writer for package.yml and lockfile",
              ],
              [
                "src/venv.c",
                "Virtual environment creation, pip execution helpers",
              ],
              ["src/config.h", "Shared data structures and constants"],
            ].map(([file, desc]) => (
              <tr key={file} className="border-t border-border">
                <td className="px-4 py-2 font-mono text-xs whitespace-nowrap">
                  {file}
                </td>
                <td className="px-4 py-2 text-muted-foreground">{desc}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <h3 className="text-base font-semibold mt-8 mb-3">
        Install after building
      </h3>
      <CodeBlock
        code={`# Linux / macOS\nsudo cp build/pypmc /usr/local/bin/\n\n# Or use CMake install\nsudo cmake --install build`}
        language="bash"
      />
    </section>
  );
}
