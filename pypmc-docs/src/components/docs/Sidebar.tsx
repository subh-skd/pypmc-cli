import { cn } from "@/lib/utils";
import { navGroups } from "@/data";
import type { SidebarProps } from "@/types";
import { Separator } from "@/components/ui/separator";

export function Sidebar({ activeSection, onNavigate }: SidebarProps) {
  return (
    <nav className="space-y-6">
      {navGroups.map((group, i) => (
        <div key={group.title}>
          {i > 0 && <Separator className="mb-4" />}
          <h4 className="mb-2 px-2 text-xs font-semibold uppercase tracking-wider text-muted-foreground">
            {group.title}
          </h4>
          <ul className="space-y-0.5">
            {group.items.map((item) => (
              <li key={item.id}>
                <button
                  onClick={() => onNavigate(item.id)}
                  className={cn(
                    "w-full rounded-md px-2 py-1.5 text-left text-sm transition-colors",
                    activeSection === item.id
                      ? "bg-primary text-primary-foreground font-medium"
                      : "text-muted-foreground hover:bg-muted hover:text-foreground",
                  )}
                >
                  {item.label}
                </button>
              </li>
            ))}
          </ul>
        </div>
      ))}
    </nav>
  );
}
