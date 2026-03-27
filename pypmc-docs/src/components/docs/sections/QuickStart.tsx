import { CodeBlock } from "@/components/docs/CodeBlock";

export function QuickStart() {
  return (
    <section id="quickstart" className="scroll-mt-20">
      <h2 className="text-2xl font-bold tracking-tight mb-2">Quick Start</h2>
      <p className="text-muted-foreground mb-6">
        Get a new Python project running in under a minute.
      </p>

      <div className="space-y-8">
        <Step number={1} title="Create a new project">
          <CodeBlock
            code={`mkdir my-project && cd my-project\npypmc init`}
            language="bash"
          />
          <p className="text-sm text-muted-foreground">
            This creates a{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package.yml
            </code>
            , a{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">.venv</code>{" "}
            virtual environment, a starter{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              main.py
            </code>
            , and a{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              .gitignore
            </code>
            . Use{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              pypmc init -y
            </code>{" "}
            to accept all defaults without prompts.
          </p>
        </Step>

        <Step number={2} title="Install packages">
          <CodeBlock
            code={`pypmc install requests\npypmc install flask\npypmc install pytest -D   # dev dependency`}
            language="bash"
          />
          <p className="text-sm text-muted-foreground">
            Packages are installed into the{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">.venv</code>,
            pinned with exact versions in{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package.yml
            </code>
            , and locked in{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package-lock.yml
            </code>
            .
          </p>
        </Step>

        <Step number={3} title="Write some code">
          <CodeBlock
            title="main.py"
            code={`import requests\n\nresponse = requests.get("https://api.github.com")\nprint(f"GitHub API status: {response.status_code}")`}
            language="python"
          />
        </Step>

        <Step number={4} title="Define and run scripts">
          <p className="text-sm text-muted-foreground mb-2">
            Add scripts to your{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package.yml
            </code>
            :
          </p>
          <CodeBlock
            code={`scripts:\n  start: python main.py\n  test: pytest`}
            language="yaml"
            title="package.yml"
          />
          <p className="text-sm text-muted-foreground mt-2">Then run them:</p>
          <CodeBlock code={`pypmc run start\npypmc run test`} language="bash" />
        </Step>

        <Step number={5} title="Collaborate">
          <p className="text-sm text-muted-foreground mb-2">
            Commit{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package.yml
            </code>{" "}
            and{" "}
            <code className="bg-muted px-1 py-0.5 rounded text-xs">
              package-lock.yml
            </code>{" "}
            to version control. When a teammate clones the repo, they run:
          </p>
          <CodeBlock code={`pypmc install`} language="bash" />
          <p className="text-sm text-muted-foreground">
            This installs all dependencies at the exact locked versions.
          </p>
        </Step>
      </div>
    </section>
  );
}

function Step({
  number,
  title,
  children,
}: {
  number: number;
  title: string;
  children: React.ReactNode;
}) {
  return (
    <div className="flex gap-4">
      <div className="flex-none flex items-start">
        <span className="flex size-7 items-center justify-center rounded-full bg-primary text-primary-foreground text-xs font-bold">
          {number}
        </span>
      </div>
      <div className="flex-1 min-w-0">
        <h3 className="text-base font-semibold mb-2">{title}</h3>
        {children}
      </div>
    </div>
  );
}
