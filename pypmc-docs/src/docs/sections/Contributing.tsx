import { CodeBlock } from "@/docs/CodeBlock";

export function Contributing() {
  return (
    <section id="contributing" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight mb-1">Contributing</h2>
      <p className="text-muted-foreground mb-8">
        pypmc is open source and contributions are welcome — from bug fixes and
        new commands to documentation improvements.
      </p>

      <div className="space-y-10">
        {/* Prerequisites */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Prerequisites</h3>
          <ul className="space-y-1 text-sm text-muted-foreground list-disc list-inside">
            <li>A C99-compatible compiler: gcc, clang, or MSVC</li>
            <li>CMake 3.10+</li>
            <li>Python 3.8+ (for integration testing)</li>
            <li>Git</li>
          </ul>
        </div>

        {/* Fork & Clone */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Fork & Clone</h3>
          <CodeBlock
            language="bash"
            code={`git clone https://github.com/subh-skd/pypmc-cli.git\ncd pypmc-cli`}
          />
        </div>

        {/* Project Structure */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Project Structure</h3>
          <CodeBlock
            language="text"
            code={`pypmc-cli/
├── src/
│   ├── main.c          # Entry point — command dispatch
│   ├── core.c / core.h # Install, uninstall, update, list, outdated, run
│   ├── config.c / config.h  # package.yml & lockfile read/write
│   └── venv.c / venv.h # Virtual environment creation & activation
├── .github/workflows/
│   ├── test.yml        # CI: build & verify on Linux, macOS, Windows
│   └── release.yml     # Release: cross-compile & upload binaries
└── pypmc-docs/         # Documentation site (React/Vite)`}
          />
          <p className="text-sm text-muted-foreground mt-3">
            The codebase is intentionally small (~2,300 lines of C99). Each
            source file maps directly to a concern — read{" "}
            <code className="text-xs bg-muted px-1.5 py-0.5 rounded">
              core.c
            </code>{" "}
            for command logic and{" "}
            <code className="text-xs bg-muted px-1.5 py-0.5 rounded">
              config.c
            </code>{" "}
            for file I/O.
          </p>
        </div>

        {/* Building */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Building</h3>
          <CodeBlock
            language="bash"
            title="CMake (all platforms)"
            code={`cmake -B build -DCMAKE_BUILD_TYPE=Release\ncmake --build build --config Release`}
          />
          <CodeBlock
            language="bash"
            title="Make shortcut (Linux/macOS)"
            code={`make`}
          />
          <p className="text-sm text-muted-foreground mt-2">
            Output: <code className="text-xs bg-muted px-1.5 py-0.5 rounded">build/pypmc</code> on Linux/macOS,{" "}
            <code className="text-xs bg-muted px-1.5 py-0.5 rounded">build/Release/pypmc.exe</code> on Windows.
          </p>
        </div>

        {/* Branching */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Branching</h3>
          <p className="text-sm text-muted-foreground mb-3">
            Create a branch from <code className="text-xs bg-muted px-1.5 py-0.5 rounded">dev</code> with a descriptive name:
          </p>
          <CodeBlock
            language="bash"
            code={`git checkout dev\ngit checkout -b fix/lockfile-parse-error\ngit checkout -b feat/add-freeze-command`}
          />
        </div>

        {/* Coding Style */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Coding Style</h3>
          <ul className="space-y-1.5 text-sm text-muted-foreground list-disc list-inside">
            <li>C99 standard — no compiler extensions</li>
            <li>4-space indentation, no tabs</li>
            <li>Keep functions focused and small</li>
            <li>
              No external libraries — the zero-dependency constraint is
              intentional
            </li>
            <li>Add a comment when the logic is not immediately obvious</li>
          </ul>
        </div>

        {/* Commit Messages */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Commit Messages</h3>
          <p className="text-sm text-muted-foreground mb-3">
            Use the imperative mood, subject line under 72 characters:
          </p>
          <CodeBlock
            language="text"
            code={`fix: handle missing package.yml gracefully\nfeat: add pypmc freeze command\ndocs: update installation instructions for Windows`}
          />
        </div>

        {/* Pull Requests */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Submitting a Pull Request</h3>
          <p className="text-sm text-muted-foreground mb-3">
            All pull requests should target the{" "}
            <code className="text-xs bg-muted px-1.5 py-0.5 rounded">dev</code>{" "}
            branch, <strong>not</strong>{" "}
            <code className="text-xs bg-muted px-1.5 py-0.5 rounded">main</code>.
          </p>
          <ol className="space-y-1.5 text-sm text-muted-foreground list-decimal list-inside">
            <li>Ensure the build passes on your machine</li>
            <li>
              Open your PR against the{" "}
              <code className="text-xs bg-muted px-1.5 py-0.5 rounded">dev</code>{" "}
              branch
            </li>
            <li>Keep your PR focused — one feature or fix per PR</li>
            <li>Describe what changed and why in the PR description</li>
            <li>
              Reference related issues with{" "}
              <code className="text-xs bg-muted px-1.5 py-0.5 rounded">
                Fixes #123
              </code>{" "}
              or{" "}
              <code className="text-xs bg-muted px-1.5 py-0.5 rounded">
                Closes #123
              </code>
            </li>
            <li>Be responsive to review feedback</li>
          </ol>
          <p className="text-sm text-muted-foreground mt-3">
            PRs that break the CI build or introduce external dependencies will
            not be merged.
          </p>
        </div>

        {/* Reporting Bugs */}
        <div>
          <h3 className="text-lg font-semibold mb-3">Reporting Bugs</h3>
          <p className="text-sm text-muted-foreground mb-3">
            Open an issue on{" "}
            <a
              href="https://github.com/subh-skd/pypmc-cli/issues"
              target="_blank"
              rel="noopener noreferrer"
              className="underline underline-offset-4 hover:text-foreground"
            >
              GitHub Issues
            </a>{" "}
            and include:
          </p>
          <ul className="space-y-1 text-sm text-muted-foreground list-disc list-inside">
            <li>Your OS and architecture (e.g. Ubuntu 22.04 x64, Windows 11 ARM)</li>
            <li>
              The pypmc version (
              <code className="text-xs bg-muted px-1.5 py-0.5 rounded">
                pypmc --version
              </code>
              )
            </li>
            <li>The exact command you ran and the full error output</li>
            <li>Your package.yml if relevant (redact sensitive values)</li>
          </ul>
        </div>
      </div>
    </section>
  );
}
