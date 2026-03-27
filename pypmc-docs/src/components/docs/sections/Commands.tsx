import { Badge } from "@/components/ui/badge";
import { Separator } from "@/components/ui/separator";
import { CodeBlock } from "@/components/docs/CodeBlock";

export function CmdInit() {
  return (
    <section id="init" className="scroll-mt-20">
      <div className="flex items-center gap-2 mb-2">
        <h2 className="text-2xl font-bold tracking-tight font-mono">
          pypmc init
        </h2>
      </div>
      <p className="text-muted-foreground mb-4">
        Initialize a new PyPMC project in the current directory.
      </p>
      <CodeBlock code={`pypmc init [options]`} language="bash" />

      <h3 className="text-base font-semibold mt-6 mb-3">What it creates</h3>
      <ul className="list-disc list-inside space-y-1 text-sm text-muted-foreground">
        <li>
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            package.yml
          </code>{" "}
          &mdash; project manifest with metadata, dependencies, and scripts
        </li>
        <li>
          <code className="bg-muted px-1 py-0.5 rounded text-xs">.venv/</code>{" "}
          &mdash; isolated Python virtual environment
        </li>
        <li>
          <code className="bg-muted px-1 py-0.5 rounded text-xs">main.py</code>{" "}
          &mdash; starter Python file
        </li>
        <li>
          <code className="bg-muted px-1 py-0.5 rounded text-xs">
            .gitignore
          </code>{" "}
          &mdash; standard Python gitignore template
        </li>
      </ul>

      <h3 className="text-base font-semibold mt-6 mb-3">Options</h3>
      <OptionsTable
        options={[
          ["-y, --yes", "Accept all defaults without prompts"],
          ["--name <name>", "Project name (default: directory name)"],
          ["--version <ver>", "Initial version (default: 1.0.0)"],
          ["--description <desc>", "Project description"],
          ["--author <author>", "Author name"],
          ["--license <license>", "License type (default: MIT)"],
          ["--python <spec>", "Python version specifier (default: >=3.8)"],
        ]}
      />

      <h3 className="text-base font-semibold mt-6 mb-3">Examples</h3>
      <CodeBlock
        code={`# Interactive mode\npypmc init\n\n# Accept all defaults\npypmc init -y\n\n# Custom options\npypmc init --name my-api --author "Jane Doe" --license Apache-2.0`}
        language="bash"
      />
    </section>
  );
}

export function CmdInstall() {
  return (
    <section id="install" className="scroll-mt-20">
      <div className="flex items-center gap-2 mb-2">
        <h2 className="text-2xl font-bold tracking-tight font-mono">
          pypmc install
        </h2>
        <Badge variant="outline">alias: i</Badge>
      </div>
      <p className="text-muted-foreground mb-4">
        Install one or more packages, or restore all dependencies from the
        lockfile.
      </p>

      <CodeBlock
        code={`pypmc install [package[@version]] [options]\npypmc i [package[@version]] [options]`}
        language="bash"
      />

      <h3 className="text-base font-semibold mt-6 mb-3">Options</h3>
      <OptionsTable
        options={[
          [
            "-D, --save-dev",
            "Save as a dev dependency instead of a production dependency",
          ],
        ]}
      />

      <h3 className="text-base font-semibold mt-6 mb-3">Behavior</h3>
      <div className="space-y-4 text-sm">
        <div className="rounded-lg border border-border p-4">
          <h4 className="font-semibold mb-1">Install a specific package</h4>
          <CodeBlock code={`pypmc install requests`} language="bash" />
          <p className="text-muted-foreground">
            Installs the latest version, pins it in{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package.yml
            </code>
            , and regenerates the lockfile.
          </p>
        </div>

        <div className="rounded-lg border border-border p-4">
          <h4 className="font-semibold mb-1">
            Install with version constraint
          </h4>
          <CodeBlock
            code={`pypmc install "flask==3.0.0"\npypmc install "numpy>=1.20,<2.0"\npypmc install "django~=4.2"`}
            language="bash"
          />
          <p className="text-muted-foreground">
            Supports all PEP 440 version specifiers:{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">==</code>,{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">{">="}</code>
            ,{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">{"<="}</code>
            ,{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">{"~="}</code>
            , and ranges.
          </p>
        </div>

        <div className="rounded-lg border border-border p-4">
          <h4 className="font-semibold mb-1">Install all dependencies</h4>
          <CodeBlock code={`pypmc install`} language="bash" />
          <p className="text-muted-foreground">
            With no arguments, installs all dependencies from{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package-lock.yml
            </code>{" "}
            (preferred) or{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package.yml
            </code>
            . Uses exact locked versions when available for reproducibility.
          </p>
        </div>

        <div className="rounded-lg border border-border p-4">
          <h4 className="font-semibold mb-1">Dev dependencies</h4>
          <CodeBlock code={`pypmc install pytest -D`} language="bash" />
          <p className="text-muted-foreground">
            Saves the package under{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              dev_dependencies
            </code>{" "}
            in package.yml. Dev dependencies are still installed locally but are
            separated from production dependencies.
          </p>
        </div>
      </div>
    </section>
  );
}

export function CmdUninstall() {
  return (
    <section id="uninstall" className="scroll-mt-20">
      <div className="flex items-center gap-2 mb-2">
        <h2 className="text-2xl font-bold tracking-tight font-mono">
          pypmc uninstall
        </h2>
        <Badge variant="outline">alias: rm</Badge>
      </div>
      <p className="text-muted-foreground mb-4">
        Remove a package and clean up any orphaned transitive dependencies.
      </p>
      <CodeBlock
        code={`pypmc uninstall <package>\npypmc rm <package>`}
        language="bash"
      />

      <h3 className="text-base font-semibold mt-6 mb-3">Orphan Cleanup</h3>
      <p className="text-sm text-muted-foreground leading-relaxed">
        When you uninstall a package, PyPMC doesn't just remove it &mdash; it
        also identifies and removes any transitive dependencies that are no
        longer required by any other direct dependency. For example,
        uninstalling{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">requests</code>{" "}
        will also remove{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">urllib3</code>,{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">
          charset-normalizer
        </code>
        , etc. if no other package depends on them.
      </p>
      <CodeBlock
        code={`pypmc uninstall requests\n# Removing requests...\n# Cleaning up orphaned dependencies: urllib3, charset-normalizer, certifi, idna`}
        language="bash"
      />
    </section>
  );
}

export function CmdUpdate() {
  return (
    <section id="update" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight font-mono mb-2">
        pypmc update
      </h2>
      <p className="text-muted-foreground mb-4">
        Update one or all packages to their latest versions.
      </p>
      <CodeBlock
        code={`# Update a specific package\npypmc update requests\n\n# Update all packages\npypmc update`}
        language="bash"
      />
      <p className="text-sm text-muted-foreground leading-relaxed">
        After updating, PyPMC rewrites the version pin in{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">
          package.yml
        </code>{" "}
        and regenerates the lockfile with the new exact versions.
      </p>
    </section>
  );
}

export function CmdOutdated() {
  return (
    <section id="outdated" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight font-mono mb-2">
        pypmc outdated
      </h2>
      <p className="text-muted-foreground mb-4">
        Check which installed packages have newer versions available.
      </p>
      <CodeBlock code={`pypmc outdated`} language="bash" />
      <CodeBlock
        title="Example output"
        code={`Package          Current    Latest     Type\n─────────────────────────────────────────────────\nrequests         2.28.0     2.31.0     direct\nflask            2.3.0      3.0.0      direct\nurllib3          1.26.15    2.1.0      transitive`}
      />
      <p className="text-sm text-muted-foreground">
        The <strong>Type</strong> column indicates whether the package is a
        direct dependency (listed in package.yml) or a transitive dependency
        pulled in by another package.
      </p>
    </section>
  );
}

export function CmdList() {
  return (
    <section id="list" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight font-mono mb-2">
        pypmc list
      </h2>
      <p className="text-muted-foreground mb-4">
        Display all project dependencies with their installed versions.
      </p>
      <CodeBlock code={`pypmc list`} language="bash" />
      <CodeBlock
        title="Example output"
        code={`Dependencies:\n  requests      ==2.31.0\n  flask         ==3.0.0\n\nDev Dependencies:\n  pytest        ==8.0.0`}
      />
    </section>
  );
}

export function CmdRun() {
  return (
    <section id="run" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight font-mono mb-2">
        pypmc run
      </h2>
      <p className="text-muted-foreground mb-4">
        Execute a script defined in the{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">scripts</code>{" "}
        section of package.yml.
      </p>
      <CodeBlock code={`pypmc run <script-name>`} language="bash" />

      <h3 className="text-base font-semibold mt-6 mb-3">Defining scripts</h3>
      <CodeBlock
        title="package.yml"
        code={`scripts:\n  start: python main.py\n  test: pytest\n  lint: flake8 src/\n  dev: flask run --debug`}
        language="yaml"
      />
      <CodeBlock
        code={`pypmc run start   # executes: python main.py\npypmc run test    # executes: pytest\npypmc run dev     # executes: flask run --debug`}
        language="bash"
      />
      <p className="text-sm text-muted-foreground">
        Scripts run within the project's virtual environment, so all installed
        packages are available.
      </p>
    </section>
  );
}

export function CmdActivate() {
  return (
    <section id="activate" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight font-mono mb-2">
        pypmc activate
      </h2>
      <p className="text-muted-foreground mb-4">
        Print the shell command to activate the project's virtual environment.
      </p>
      <CodeBlock code={`pypmc activate`} language="bash" />
      <p className="text-sm text-muted-foreground mb-4">
        PyPMC detects your current shell and prints the appropriate activation
        command:
      </p>
      <div className="overflow-x-auto">
        <table className="w-full text-sm border border-border rounded-lg overflow-hidden">
          <thead>
            <tr className="bg-muted/50">
              <th className="text-left px-4 py-2 font-medium">Shell</th>
              <th className="text-left px-4 py-2 font-medium">Command</th>
            </tr>
          </thead>
          <tbody>
            {[
              ["bash / zsh", "source .venv/bin/activate"],
              ["fish", "source .venv/bin/activate.fish"],
              ["csh", "source .venv/bin/activate.csh"],
              ["PowerShell", "source .venv/Scripts/Activate.ps1"],
              ["CMD / Git Bash (Windows)", "source .venv/Scripts/activate"],
            ].map(([shell, cmd]) => (
              <tr key={shell} className="border-t border-border">
                <td className="px-4 py-2">{shell}</td>
                <td className="px-4 py-2">
                  <code className="bg-muted px-1.5 py-0.5 rounded text-xs">
                    {cmd}
                  </code>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </section>
  );
}

export function CmdCompletions() {
  return (
    <section id="completions" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight font-mono mb-2">
        pypmc completions
      </h2>
      <p className="text-muted-foreground mb-4">
        Generate shell completion scripts for tab-completion of commands, flags,
        package names, and script names.
      </p>
      <CodeBlock code={`pypmc completions <shell>`} language="bash" />

      <h3 className="text-base font-semibold mt-6 mb-3">Setup</h3>
      <CodeBlock
        code={`# Bash — add to ~/.bashrc\neval "$(pypmc completions bash)"\n\n# Zsh — add to ~/.zshrc\neval "$(pypmc completions zsh)"\n\n# Fish — add to ~/.config/fish/config.fish\npypmc completions fish | source`}
        language="bash"
      />
      <p className="text-sm text-muted-foreground">
        Completions are context-aware: they suggest package names from your
        package.yml and script names from the scripts section.
      </p>
    </section>
  );
}

function OptionsTable({ options }: { options: [string, string][] }) {
  return (
    <div className="overflow-x-auto">
      <table className="w-full text-sm border border-border rounded-lg overflow-hidden">
        <thead>
          <tr className="bg-muted/50">
            <th className="text-left px-4 py-2 font-medium">Flag</th>
            <th className="text-left px-4 py-2 font-medium">Description</th>
          </tr>
        </thead>
        <tbody>
          {options.map(([flag, desc]) => (
            <tr key={flag} className="border-t border-border">
              <td className="px-4 py-2 font-mono text-xs whitespace-nowrap">
                {flag}
              </td>
              <td className="px-4 py-2 text-muted-foreground">{desc}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

export { Separator as CommandSeparator };
