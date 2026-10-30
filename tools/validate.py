"""Validate with the actual target Tessera checkout; requires PyYAML."""
import ast
import json
from pathlib import Path
import sys
import yaml

root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(Path(sys.argv[1]).resolve() / 'screen_manager/app'))
import plugin_manifest

manifest = yaml.safe_load((root / 'tessera-plugin.yaml').read_text())
for path in sorted((root / 'translations').glob('*.json')):
    plugin_manifest.check(manifest, json.loads(path.read_text()))
    print(f'Manifest and {path.name}: OK')
for path in root.rglob('*.py'):
    ast.parse(path.read_text())
    print(f'Python syntax {path.relative_to(root)}: OK')
