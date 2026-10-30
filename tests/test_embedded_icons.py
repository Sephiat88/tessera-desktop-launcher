"""Check the published choices and generated C++ asset lookup without ESP hardware."""
import json
from pathlib import Path
import subprocess
import tempfile
import unittest
import yaml

ROOT = Path(__file__).resolve().parents[1]

class EmbeddedIcons(unittest.TestCase):
    def test_manifest_preserves_existing_paths_and_translates_every_choice(self):
        manifest = yaml.safe_load((ROOT / 'tessera-plugin.yaml').read_text())
        tile = manifest['tiles'][0]
        option = next(o for o in tile['options'] if o['id'] == 'icon_path')
        self.assertEqual(option['kind'], 'choice')
        self.assertEqual(tile.get('pictures', 0), 0)
        choices = {c['value'] for c in option['choices']}
        for png in (ROOT / 'assets/icons').glob('*.png'):
            self.assertIn('/local/Icons/' + png.name, choices)
        for lang in ('de', 'en'):
            texts = json.loads((ROOT / f'translations/{lang}.json').read_text())['app']
            for choice in option['choices']:
                self.assertTrue(texts[choice['label']])

    def test_generated_files_are_current(self):
        subprocess.run(['python3', str(ROOT / 'tools/generate_icons.py'), '--check'], check=True)

    def test_compiled_asset_lookup_and_argb_descriptors(self):
        # This is a C++ asset smoke test with a small LVGL type stub, not a firmware build.
        with tempfile.TemporaryDirectory() as folder:
            temp = Path(folder)
            header = temp / 'esphome/components/smart_display/plugin_api.h'
            header.parent.mkdir(parents=True)
            header.write_text('''#pragma once
#include <cstdint>
#define LV_IMAGE_HEADER_MAGIC 0x19
#define LV_COLOR_FORMAT_ARGB8888 0x10
struct lv_image_header_t { uint32_t magic, cf, w, h, stride; };
struct lv_image_dsc_t { lv_image_header_t header; uint32_t data_size; const uint8_t *data; };
''')
            main = temp / 'check.cpp'
            main.write_text('''#include "embedded_icons.h"
#include <cassert>
int main() {
  using esphome::desktop_launcher::embedded_icons::find;
  const char *files[] = {"bambu-lab.png", "lyra.png", "terminal.png", "vivaldi.png", "SolidCraft_logo.png", "sublime-text.png"};
  for (auto name : files) {
    const auto *a = find(name);
    assert(a && a == find(std::string("/local/Icons/") + name));
    assert(a->header.magic == LV_IMAGE_HEADER_MAGIC);
    assert(a->header.cf == LV_COLOR_FORMAT_ARGB8888);
    assert(a->header.w == 96 && a->header.h == 96 && a->header.stride == 384);
    assert(a->data_size == 36864 && a->data);
    bool visible = false;
    for (unsigned i=3; i<a->data_size; i+=4) visible |= a->data[i] != 0;
    assert(visible);
  }
  assert(!find("none") && !find("/local/Icons/missing.png"));
}
''')
            assets = ROOT / 'components/desktop_launcher'
            executable = temp / 'check'
            subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Werror', '-I'+str(temp),
                '-I'+str(assets), str(assets/'embedded_icons.cpp'), str(main), '-o', str(executable)], check=True)
            subprocess.run([str(executable)], check=True)

if __name__ == '__main__':
    unittest.main()
