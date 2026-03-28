import { Badge } from "@/components/ui/badge";

export function Introduction() {
  return (
    <section id="introduction" className="scroll-mt-20">
      <div className="flex items-center gap-3 mb-2">
        <h1 className="text-3xl font-bold tracking-tight">PyPMC</h1>
        <Badge variant="secondary">v0.2.0</Badge>
      </div>
      <p className="text-lg text-muted-foreground mb-6">
        A fast, native package manager for Python &mdash; written in C with zero
        runtime dependencies.
      </p>

      <div className="grid gap-4 sm:grid-cols-2 lg:grid-cols-3 mb-8">
        <FeatureCard
          title="npm-style Workflow"
          description="Familiar commands like init, install, and run &mdash; designed for developers who love npm's simplicity."
        />
        <FeatureCard
          title="Written in C"
          description="~2,300 lines of portable C99. No runtime dependencies beyond a C compiler to build."
        />
        <FeatureCard
          title="Cross-platform"
          description="Works on Linux, macOS, and Windows with full support for bash, zsh, fish, PowerShell, and CMD."
        />
        <FeatureCard
          title="Virtual Environments"
          description="Automatically creates and manages isolated .venv directories for every project."
        />
        <FeatureCard
          title="Lockfile Support"
          description="Reproducible installs via package-lock.yml with exact pinned versions."
        />
        <FeatureCard
          title="Orphan Cleanup"
          description="Automatically removes unused transitive dependencies when you uninstall a package."
        />
      </div>

      <h2 className="text-xl font-semibold mb-3">How It Works</h2>
      <p className="text-muted-foreground leading-relaxed mb-4">
        PyPMC wraps Python's built-in{" "}
        <code className="text-sm bg-muted px-1.5 py-0.5 rounded">venv</code> and{" "}
        <code className="text-sm bg-muted px-1.5 py-0.5 rounded">pip</code>{" "}
        tools behind a streamlined CLI. Your project configuration lives in a{" "}
        <code className="text-sm bg-muted px-1.5 py-0.5 rounded">
          package.yml
        </code>{" "}
        file (similar to{" "}
        <code className="text-sm bg-muted px-1.5 py-0.5 rounded">
          package.json
        </code>
        ), and a{" "}
        <code className="text-sm bg-muted px-1.5 py-0.5 rounded">
          package-lock.yml
        </code>{" "}
        lockfile ensures reproducible installs across machines. All packages are
        installed into an isolated{" "}
        <code className="text-sm bg-muted px-1.5 py-0.5 rounded">.venv</code>{" "}
        directory managed by PyPMC.
      </p>
    </section>
  );
}

function FeatureCard({
  title,
  description,
}: {
  title: string;
  description: string;
}) {
  return (
    <div className="rounded-lg border border-border bg-card p-4">
      <h3 className="font-semibold text-sm mb-1">{title}</h3>
      <p className="text-sm text-muted-foreground leading-relaxed">
        {description}
      </p>
    </div>
  );
}
