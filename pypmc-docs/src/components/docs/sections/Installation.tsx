import { CodeBlock } from "@/components/docs/CodeBlock";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";

export function Installation() {
  return (
    <section id="installation" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight mb-2">Installation</h2>
      <p className="text-muted-foreground mb-6">
        Install PyPMC using a one-line command, download a prebuilt binary, or
        build from source.
      </p>

      <h3 className="text-lg font-semibold mb-3">One-line Install (recommended)</h3>
      <Tabs defaultValue="linux">
        <TabsList>
          <TabsTrigger value="linux">Linux / macOS</TabsTrigger>
          <TabsTrigger value="powershell">Windows (PowerShell)</TabsTrigger>
          <TabsTrigger value="cmd">Windows (CMD)</TabsTrigger>
        </TabsList>
        <TabsContent value="linux">
          <CodeBlock
            code={`curl -fsSL https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.sh -o /tmp/install-pypmc.sh && bash /tmp/install-pypmc.sh && rm /tmp/install-pypmc.sh`}
            language="bash"
          />
        </TabsContent>
        <TabsContent value="powershell">
          <CodeBlock
            code={`Invoke-WebRequest -Uri https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.ps1 -OutFile "$env:TEMP\\install-pypmc.ps1"; & "$env:TEMP\\install-pypmc.ps1"; Remove-Item "$env:TEMP\\install-pypmc.ps1"`}
            language="powershell"
          />
        </TabsContent>
        <TabsContent value="cmd">
          <CodeBlock
            code={`curl -fsSL https://raw.githubusercontent.com/subh-skd/pypmc-cli/main/install.cmd -o %TEMP%\\install-pypmc.cmd && call %TEMP%\\install-pypmc.cmd && del %TEMP%\\install-pypmc.cmd`}
            language="batch"
          />
        </TabsContent>
      </Tabs>
      <p className="text-sm text-muted-foreground mt-3">
        The installer downloads the latest release binary, places it in the
        correct location, and adds it to your system PATH automatically.
      </p>

      <h3 className="text-lg font-semibold mt-8 mb-3">Post-install: Adding to PATH</h3>
      <p className="text-sm text-muted-foreground mb-3">
        The install scripts add pypmc to your PATH automatically. If{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">pypmc</code> is
        not recognized after installation, <strong className="text-foreground">restart your terminal</strong> and
        try again. If it still doesn't work, add the install location manually:
      </p>
      <div className="overflow-x-auto mb-4">
        <table className="w-full text-sm border border-border rounded-lg overflow-hidden">
          <thead>
            <tr className="bg-muted/50">
              <th className="text-left px-4 py-2 font-medium">Platform</th>
              <th className="text-left px-4 py-2 font-medium">Install location</th>
              <th className="text-left px-4 py-2 font-medium">What to add to PATH</th>
            </tr>
          </thead>
          <tbody>
            {[
              ["Linux", "/usr/local/bin/pypmc", "Usually already in PATH"],
              ["macOS", "/usr/local/bin/pypmc", "Usually already in PATH"],
              ["Windows (admin)", "%ProgramFiles%\\pypmc\\pypmc.exe", "%ProgramFiles%\\pypmc"],
              ["Windows (no admin)", "%LOCALAPPDATA%\\pypmc\\pypmc.exe", "%LOCALAPPDATA%\\pypmc"],
            ].map(([platform, location, path]) => (
              <tr key={platform} className="border-t border-border">
                <td className="px-4 py-2">{platform}</td>
                <td className="px-4 py-2">
                  <code className="bg-muted px-1.5 py-0.5 rounded text-xs">
                    {location}
                  </code>
                </td>
                <td className="px-4 py-2">
                  <code className="bg-muted px-1.5 py-0.5 rounded text-xs">
                    {path}
                  </code>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <details className="group mb-4">
        <summary className="cursor-pointer text-sm font-semibold hover:text-foreground text-muted-foreground">
          Linux / macOS &mdash; manual PATH setup
        </summary>
        <div className="mt-2">
          <CodeBlock
            code={`# bash
echo 'export PATH="/usr/local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc

# zsh
echo 'export PATH="/usr/local/bin:$PATH"' >> ~/.zshrc && source ~/.zshrc

# fish
fish_add_path /usr/local/bin`}
            language="bash"
          />
        </div>
      </details>

      <details className="group mb-4">
        <summary className="cursor-pointer text-sm font-semibold hover:text-foreground text-muted-foreground">
          Windows &mdash; manual PATH setup
        </summary>
        <div className="mt-2 space-y-3">
          <p className="text-sm text-muted-foreground">
            <strong className="text-foreground">Option 1: Via GUI</strong>
          </p>
          <ol className="list-decimal list-inside space-y-1 text-sm text-muted-foreground ml-2">
            <li>Press <code className="bg-muted px-1 py-0.5 rounded text-xs">Win + R</code>, type <code className="bg-muted px-1 py-0.5 rounded text-xs">sysdm.cpl</code>, press Enter</li>
            <li>Go to <strong className="text-foreground">Advanced</strong> tab &gt; <strong className="text-foreground">Environment Variables</strong></li>
            <li>
              Select <code className="bg-muted px-1 py-0.5 rounded text-xs">Path</code> under{" "}
              <strong className="text-foreground">System variables</strong> (admin) or{" "}
              <strong className="text-foreground">User variables</strong> (no admin) and click <strong className="text-foreground">Edit</strong>
            </li>
            <li>
              Click <strong className="text-foreground">New</strong> and add:{" "}
              <code className="bg-muted px-1 py-0.5 rounded text-xs">%ProgramFiles%\pypmc</code> (admin) or{" "}
              <code className="bg-muted px-1 py-0.5 rounded text-xs">%LOCALAPPDATA%\pypmc</code> (no admin)
            </li>
            <li>Click OK, then restart your terminal</li>
          </ol>
          <p className="text-sm text-muted-foreground mt-4">
            <strong className="text-foreground">Option 2: Via CMD (admin)</strong>
          </p>
          <CodeBlock
            code={`reg add "HKLM\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" /v Path /t REG_EXPAND_SZ /d "%Path%;%ProgramFiles%\\pypmc" /f`}
            language="batch"
          />
          <p className="text-sm text-muted-foreground">
            <strong className="text-foreground">Option 2: Via CMD (no admin)</strong>
          </p>
          <CodeBlock
            code={`setx PATH "%PATH%;%LOCALAPPDATA%\\pypmc"`}
            language="batch"
          />
          <p className="text-sm text-muted-foreground mt-3">
            <strong className="text-foreground">Option 3: Via PowerShell (admin)</strong>
          </p>
          <CodeBlock
            code={`[Environment]::SetEnvironmentVariable("Path", "$([Environment]::GetEnvironmentVariable('Path', 'Machine'));$env:ProgramFiles\\pypmc", "Machine")`}
            language="powershell"
          />
          <p className="text-sm text-muted-foreground">
            <strong className="text-foreground">Option 3: Via PowerShell (no admin)</strong>
          </p>
          <CodeBlock
            code={`[Environment]::SetEnvironmentVariable("Path", "$([Environment]::GetEnvironmentVariable('Path', 'User'));$env:LOCALAPPDATA\\pypmc", "User")`}
            language="powershell"
          />
        </div>
      </details>

      <h3 className="text-lg font-semibold mt-8 mb-3">Manual Download</h3>
      <p className="text-muted-foreground mb-3">
        Download the latest binary from the{" "}
        <a
          href="https://github.com/subh-skd/pypmc-cli/releases"
          target="_blank"
          rel="noopener noreferrer"
          className="text-foreground underline underline-offset-4 hover:text-primary"
        >
          Releases
        </a>{" "}
        page:
      </p>
      <div className="overflow-x-auto mb-4">
        <table className="w-full text-sm border border-border rounded-lg overflow-hidden">
          <thead>
            <tr className="bg-muted/50">
              <th className="text-left px-4 py-2 font-medium">Platform</th>
              <th className="text-left px-4 py-2 font-medium">Binary</th>
            </tr>
          </thead>
          <tbody>
            {[
              ["Linux (x64)", "pypmc-linux-amd64"],
              ["Linux (ARM64)", "pypmc-linux-arm64"],
              ["macOS (x64)", "pypmc-macos-amd64"],
              ["macOS (Apple Silicon)", "pypmc-macos-arm64"],
              ["Windows (x64)", "pypmc-windows-amd64.exe"],
            ].map(([platform, binary]) => (
              <tr key={binary} className="border-t border-border">
                <td className="px-4 py-2">{platform}</td>
                <td className="px-4 py-2">
                  <code className="bg-muted px-1.5 py-0.5 rounded text-xs">
                    {binary}
                  </code>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>

      <CodeBlock
        code={`# Linux / macOS
chmod +x pypmc-linux-amd64
sudo mv pypmc-linux-amd64 /usr/local/bin/pypmc

# Windows (CMD) — download and install manually
mkdir "%ProgramFiles%\\pypmc"
move pypmc-windows-amd64.exe "%ProgramFiles%\\pypmc\\pypmc.exe"
reg add "HKLM\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment" /v Path /t REG_EXPAND_SZ /d "%Path%;%ProgramFiles%\\pypmc" /f`}
        language="bash"
      />

      <h3 className="text-lg font-semibold mt-8 mb-3">Build from Source</h3>
      <p className="text-sm text-muted-foreground mb-3">
        Requires a C compiler (gcc, clang, or MSVC) and CMake 3.10+.
      </p>
      <CodeBlock
        code={`# Using CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Or using Make (Linux/macOS)
make`}
        language="bash"
      />
      <p className="text-sm text-muted-foreground mt-2">
        The binary is at{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">build/pypmc</code> (or{" "}
        <code className="bg-muted px-1 py-0.5 rounded text-xs">build/Release/pypmc.exe</code>{" "}
        on Windows).
      </p>

      <h3 className="text-lg font-semibold mt-8 mb-3">Verify Installation</h3>
      <CodeBlock
        code={`pypmc --version\n# pypmc version 0.2.0`}
        language="bash"
      />
    </section>
  );
}
