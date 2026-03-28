export interface CodeBlockProps {
  code: string;
  title?: string;
  language?: string;
}

export interface NavItem {
  id: string;
  label: string;
}

export interface NavGroup {
  title: string;
  items: NavItem[];
}

export interface SidebarProps {
  activeSection: string;
  onNavigate: (id: string) => void;
}
