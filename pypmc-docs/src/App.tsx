import {
  Sheet,
  SheetContent,
  SheetHeader,
  SheetTitle,
  SheetTrigger,
} from "@/components/ui/sheet";
import {
  CmdRun,
  CmdList,
  CmdInit,
  CmdUpdate,
  CmdInstall,
  CmdOutdated,
  CmdActivate,
  CmdUninstall,
  CmdCompletions,
} from "@/components/docs/sections/Commands";
import {
  BuildFromSource,
  VirtualEnvironments,
  DependencyResolution,
} from "@/components/docs/sections/Advanced";
import {
  Scripts,
  Lockfile,
  PackageYml,
} from "@/components/docs/sections/Configuration";
import { Button } from "@/components/ui/button";
import { Separator } from "@/components/ui/separator";
import { ScrollArea } from "@/components/ui/scroll-area";
import { Sidebar } from "@/components/docs/Sidebar";
import { navGroups } from "@/components/docs/nav-data";
import { useCallback, useEffect, useRef, useState } from "react";
import { QuickStart } from "@/components/docs/sections/QuickStart";
import { Introduction } from "@/components/docs/sections/Introduction";
import { Installation } from "@/components/docs/sections/Installation";
import { Menu, Moon, Sun, ExternalLink, Terminal } from "lucide-react";

const allSectionIds = navGroups.flatMap((g) => g.items.map((i) => i.id));

function App() {
  const [activeSection, setActiveSection] = useState("introduction");
  const [dark, setDark] = useState(() => {
    if (typeof window !== "undefined") {
      return window.matchMedia("(prefers-color-scheme: dark)").matches;
    }
    return false;
  });
  const [mobileOpen, setMobileOpen] = useState(false);
  const mainRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    document.documentElement.classList.toggle("dark", dark);
  }, [dark]);

  // Intersection observer to track active section
  useEffect(() => {
    const observer = new IntersectionObserver(
      (entries) => {
        for (const entry of entries) {
          if (entry.isIntersecting) {
            setActiveSection(entry.target.id);
          }
        }
      },
      { rootMargin: "-80px 0px -70% 0px", threshold: 0 },
    );

    for (const id of allSectionIds) {
      const el = document.getElementById(id);
      if (el) observer.observe(el);
    }
    return () => observer.disconnect();
  }, []);

  const scrollTo = useCallback((id: string) => {
    const el = document.getElementById(id);
    if (el) {
      el.scrollIntoView({ behavior: "smooth" });
      setActiveSection(id);
      setMobileOpen(false);
    }
  }, []);

  return (
    <div className="min-h-screen bg-background text-foreground">
      {/* Header */}
      <header className="sticky top-0 z-50 border-b border-border bg-background/95 backdrop-blur supports-backdrop-filter:bg-background/60">
        <div className="mx-auto flex h-14 max-w-screen-2xl items-center gap-4 px-4 sm:px-6 lg:px-8">
          {/* Mobile menu */}
          <Sheet open={mobileOpen} onOpenChange={setMobileOpen}>
            <SheetTrigger className="inline-flex size-7 items-center justify-center rounded-md text-muted-foreground hover:bg-muted hover:text-foreground transition-colors lg:hidden">
              <Menu className="size-4" />
            </SheetTrigger>
            <SheetContent side="left" className="w-72 p-0">
              <SheetHeader className="px-4 py-3 border-b border-border">
                <SheetTitle className="flex items-center gap-2 text-base">
                  <Terminal className="size-4" />
                  PyPMC Docs
                </SheetTitle>
              </SheetHeader>
              <ScrollArea className="h-[calc(100vh-57px)]">
                <div className="p-4">
                  <Sidebar
                    activeSection={activeSection}
                    onNavigate={scrollTo}
                  />
                </div>
              </ScrollArea>
            </SheetContent>
          </Sheet>

          <div className="flex items-center gap-2 font-semibold">
            <Terminal className="size-4" />
            <span>PyPMC</span>
          </div>

          <div className="flex-1" />

          <nav className="hidden sm:flex items-center gap-1 text-sm">
            <Button
              variant="ghost"
              size="sm"
              onClick={() => scrollTo("introduction")}
            >
              Docs
            </Button>
            <Button
              variant="ghost"
              size="sm"
              onClick={() => scrollTo("installation")}
            >
              Install
            </Button>
            <a
              href="https://github.com/subh-skd/pypmc-cli"
              target="_blank"
              rel="noopener noreferrer"
              className="inline-flex items-center gap-1 rounded-md px-2.5 py-1.5 text-sm text-muted-foreground hover:bg-muted hover:text-foreground transition-colors"
            >
              GitHub
            </a>
          </nav>

          <Separator orientation="vertical" className="h-5 hidden sm:block" />

          <Button
            variant="ghost"
            size="icon-sm"
            onClick={() => setDark((d) => !d)}
          >
            {dark ? <Sun className="size-4" /> : <Moon className="size-4" />}
          </Button>

          <a
            href="https://github.com/subh-skd"
            target="_blank"
            rel="noopener noreferrer"
            className="inline-flex size-7 items-center justify-center rounded-md text-muted-foreground hover:bg-muted hover:text-foreground transition-colors"
          >
            <ExternalLink className="size-4" />
          </a>
        </div>
      </header>

      <div className="mx-auto max-w-screen-2xl flex">
        {/* Desktop sidebar */}
        <aside className="hidden lg:block w-64 shrink-0 border-r border-border">
          <div className="sticky top-14 h-[calc(100vh-3.5rem)] overflow-y-auto p-6">
            <Sidebar activeSection={activeSection} onNavigate={scrollTo} />
          </div>
        </aside>

        {/* Main content */}
        <main
          ref={mainRef}
          className="flex-1 min-w-0 px-4 sm:px-8 lg:px-12 py-10"
        >
          <div className="max-w-3xl space-y-16">
            <Introduction />
            <Separator />
            <Installation />
            <Separator />
            <QuickStart />
            <Separator />

            {/* Commands */}
            <CmdInit />
            <Separator />
            <CmdInstall />
            <Separator />
            <CmdUninstall />
            <Separator />
            <CmdUpdate />
            <Separator />
            <CmdOutdated />
            <Separator />
            <CmdList />
            <Separator />
            <CmdRun />
            <Separator />
            <CmdActivate />
            <Separator />
            <CmdCompletions />
            <Separator />

            {/* Configuration */}
            <PackageYml />
            <Separator />
            <Lockfile />
            <Separator />
            <Scripts />
            <Separator />

            {/* Advanced */}
            <VirtualEnvironments />
            <Separator />
            <DependencyResolution />
            <Separator />
            <BuildFromSource />

            {/* Footer */}
            <Separator />
            <footer className="pb-8 text-center text-sm text-muted-foreground">
              <p>
                PyPMC is open source under the{" "}
                <a
                  href="https://github.com/subh-skd/pypmc-cli/blob/main/LICENSE"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="underline underline-offset-4 hover:text-foreground"
                >
                  MIT License
                </a>
                . Built by{" "}
                <a
                  href="https://github.com/subh-skd"
                  target="_blank"
                  rel="noopener noreferrer"
                  className="underline underline-offset-4 hover:text-foreground"
                >
                  Subhendu Kumar
                </a>
                .
              </p>
            </footer>
          </div>
        </main>
      </div>
    </div>
  );
}

export default App;
