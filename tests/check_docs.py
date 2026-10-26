"""Check local Markdown links and HTML image sources without network access."""
from pathlib import Path
import re
from urllib.parse import unquote

root = Path(__file__).resolve().parents[1]
errors = []
for path in [root / "README.md", root / "CONTRIBUTING.md", *sorted((root / "docs").glob("*.md"))]:
    content = path.read_text()
    targets = re.findall(r'!?\[[^\]\n]*\]\(([^\s)]+)(?:\s+[^)]*)?\)', content)
    targets += re.findall(r'(?:src|href)="([^"]+)"', content)
    for target in targets:
        if target.startswith(("#", "https://", "http://", "mailto:")):
            continue
        local = unquote(target.split("#", 1)[0])
        if local and not (path.parent / local).exists():
            errors.append(f"{path.relative_to(root)}: missing {target}")
if errors:
    raise SystemExit("\n".join(errors))
print("Local documentation links passed")
