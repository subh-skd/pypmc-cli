import type { NavGroup } from "@/types";

export const navGroups: NavGroup[] = [
  {
    title: "Getting Started",
    items: [
      { id: "introduction", label: "Introduction" },
      { id: "installation", label: "Installation" },
      { id: "quickstart", label: "Quick Start" },
    ],
  },
  {
    title: "Usage",
    items: [
      { id: "init", label: "pypmc init" },
      { id: "install", label: "pypmc install" },
      { id: "uninstall", label: "pypmc uninstall" },
      { id: "update", label: "pypmc update" },
      { id: "outdated", label: "pypmc outdated" },
      { id: "list", label: "pypmc list" },
      { id: "run", label: "pypmc run" },
      { id: "activate", label: "pypmc activate" },
      { id: "completions", label: "pypmc completions" },
    ],
  },
  {
    title: "Configuration",
    items: [
      { id: "package-yml", label: "package.yml" },
      { id: "lockfile", label: "Lockfile" },
      { id: "scripts", label: "Scripts" },
    ],
  },
  {
    title: "Advanced",
    items: [
      { id: "virtual-environments", label: "Virtual Environments" },
      { id: "dependency-resolution", label: "Dependency Resolution" },
      { id: "build-from-source", label: "Build from Source" },
    ],
  },
  {
    title: "Contributing",
    items: [
      { id: "contributing", label: "Contributing Guide" },
    ],
  },
];
