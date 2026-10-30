# Tessera Desktop Launcher 1.1.1

Tessera-Plugin (API 0.24) für Desktop-Aktionen mit Entitätsauswahl und auswählbaren,
fest eingebetteten Icons. LVGL 9 zeichnet die Bilddaten direkt aus der Firmware.

## Was den 404-Fehler behebt

Der bisherige Abruf `/local/Icons/...` über `http://supervisor/core/local/...` entfällt
vollständig für dieses Plugin. Die sechs Icons werden beim Firmwarebuild eingebunden.
Die Python-Dateien des installierten Tessera-Add-ons müssen dafür nicht geändert werden.

Die vorhandenen Werte `/local/Icons/bambu-lab.png`, `/local/Icons/lyra.png` usw. bleiben
als kompatible Auswahlwerte erhalten. Sie sind ab 1.1.1 nur Schlüssel für eingebettete
Icons: Es findet dafür **kein HTTP-Abruf** statt. Beliebige Serverpfade werden nicht geladen.
Auch eine Kachel ohne Icon verwendet keine Bildanfrage, sondern das Laptop-Schriftzeichen.

## Installation / Update

1. Den gesamten Inhalt dieses Arbeitsordners ins eigene GitHub-Repository übernehmen,
   besonders `components/desktop_launcher/embedded_icons.cpp`, `.h` und `assets/icons/`.
2. Commit und Push. `tessera-plugin.yaml` enthält Version `1.1.1`.
3. Falls das Repository bereits GitHub-Releases hat: ein neues Release für diesen Stand
   veröffentlichen. Tessera bevorzugt das neueste Release. Ohne Release folgt der Link
   dem ausgewählten/default Branch; die Installation bleibt auf einen Commit angeheftet.
4. In Tessera die Pluginquelle aktualisieren und Version 1.1.1 für BueroScreen installieren.
   Das Display muss die neu gebaute Firmware erhalten. Nur »Save & send« reicht dafür nicht.
   Die von Tessera verwaltete `bueroscreen.plugins.yaml` nicht von Hand bearbeiten.
5. Eine Desktop-Starter-Kachel auswählen. Unter den Pluginoptionen **Icon auswählen**
   erscheinen Laptop (Standard), Bambu Lab, Lyra, Terminal, Vivaldi, SolidCraft und Sublime Text.
   Diese Auswahl ist eine Namensliste; eine Vorschau als Bild im Dropdown wird nicht definiert.
6. Icon wählen und »Save & send«. Ein Wechsel zwischen den eingebauten Icons braucht
   anschließend keinen weiteren Firmwarebuild.

Bestehende Kacheln mit den sechs bekannten Pfaden behalten ihr Icon. Kacheln ohne Auswahl
zeigen das Laptop-Symbol. Der Logeintrag nach dem Boot muss lauten:

```
Desktop launcher 1.1.1 embedded icons
```

Bei einer Kachel mit Icon erscheint z. B.:

```
Embedded icon: /local/Icons/bambu-lab.png (96x96)
```

Wenn weiterhin »Plugin desktop_launcher: no picture from ...« erscheint, sicherstellen,
dass **die neue Firmware auf dem Display** läuft. Die neue Version ruft weder
`ui::picture()` noch `ui::set_picture()` auf und kann diese Anfragen nicht erzeugen.
Bereits laufende alte Abrufe können bis zum Firmwarewechsel noch im Protokoll stehen.

## Entitätsauswahl und Aktion

Die Kachel zeigt nur Icon und Namen; der Entitätszustand (on/off usw.) wird nicht angezeigt.
Ein separates optionales **Aktionsziel**
legt fest, was ein Tap auslöst. Leer bedeutet: Hauptentität verwenden, wenn sie schaltbar ist.

| Domain | Aktion |
| --- | --- |
| button | button.press |
| input_button | input_button.press |
| switch | switch.toggle |
| input_boolean | input_boolean.toggle |
| script | script.turn_on |
| sensor, binary_sensor | Keine Aktion; separates Aktionsziel auswählen |

Eine HA-Automation kann auf einen input_button reagieren und über MQTT den PC ansprechen.
Die Auswahl eines Schalters als Hauptentität ohne anderes Aktionsziel schaltet diesen
Schalter um. Das Plugin startet allein noch kein Desktopprogramm.

## Eigene Icons ergänzen

Die Auswahl ist fest im Manifest definiert. Das Plugin kann nicht automatisch das
Dateisystem des HA-Servers auflisten. Neue Icons werden im Arbeitsordner ergänzt:

1. PNG nach `assets/icons/` kopieren. SVG zuvor als transparentes PNG exportieren;
   optional mit `tools/convert_icon.py` und CairoSVG.
2. Einen Eintrag mit gültigem C++-Bezeichner und Dateinamen in `CATALOGUE` von
   `tools/generate_icons.py` ergänzen.
3. In `tessera-plugin.yaml` bei `icon_path.choices` einen Wert
   `/local/Icons/DEIN-ICON.png` sowie einen Übersetzungsschlüssel eintragen.
4. Den Schlüssel in `translations/de.json` und `translations/en.json` ergänzen.
5. Auf dem Entwicklungsrechner mit Pillow die C++-Daten generieren:

   ```bash
   python3 tools/generate_icons.py
   python3 tools/validate.py ../homeassistant_espscreen-main
   python3 -m unittest discover -s tests -v
   ```

6. Alle Dateien committen, Pluginversion erhöhen und die Firmware neu installieren.

Die erzeugten Bilder enthalten 96×96 BGRA-Bytes für LVGLs ARGB8888-Format inklusive
Transparenz. Die Quelldateien werden proportional verkleinert und mittig platziert.
Auf kleinen Kacheln wird die Grafik weiter verkleinert. Die sechs Icons benötigen
zusammen 221184 Byte (216 KiB) Pixeldaten im Flash; LVGL benötigt zusätzlich Drawbuffer.
Für normale Firmwarebuilds ist Pillow nicht nötig, weil die generierte C++-Datei im
Repository liegt. Die ursprünglichen PNGs stammen aus dem vom Nutzer bereitgestellten
Iconordner; deren Marken-/Nutzungsrechte werden durch die Code-Lizenz nicht verändert.

## Geprüft

- Manifest mit dem Originalvalidator des lokalen Tessera-Checkouts, Deutsch und Englisch.
- Python-Syntax und reproduzierbare Generierung.
- Auswahlliste einschließlich aller bisherigen Iconpfade.
- Kompilierung und Ausführung der C++-Asset-Zuordnung mit einem kleinen LVGL-Typstub.

Ein vollständiger ESPHome-/LVGL-Firmwarebuild und der Test auf dem Waveshare 5B stehen
noch aus, da hier keine ESPHome-CLI verfügbar ist. Es wurden keine Dateien veröffentlicht
oder auf den HA-Server übertragen.
