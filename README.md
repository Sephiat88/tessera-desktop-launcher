# tessera-desktop-launcher
Tessera Plugin zum Starten von Desktop-Programmen via MQTT

## Existing icons on the development PC

Files in `/home/alex/Downloads/icons` are not automatically available on the HA server.
Copy them into `/homeassistant/www/Icons` on that server, preserving file names and case.
Then set the path explicitly for each existing tile (changing the manifest default does
not replace saved tile options):

| File on HA | Tile icon path |
| --- | --- |
| /homeassistant/www/Icons/terminal.png | /local/Icons/terminal.png |
| /homeassistant/www/Icons/vivaldi.png | /local/Icons/vivaldi.png |
| /homeassistant/www/Icons/lyra.png | /local/Icons/lyra.png |
| /homeassistant/www/Icons/bambu-lab.png | /local/Icons/bambu-lab.png |
| /homeassistant/www/Icons/SolidCraft_logo.png | /local/Icons/SolidCraft_logo.png |
| /homeassistant/www/Icons/sublime-text.png | /local/Icons/sublime-text.png |

Open `http://YOUR_HA:8123/local/Icons/terminal.png` (or your normal HTTPS HA URL)
in your browser first. If it returns 404, correct the server file location/name.
The plugin never requests repository assets. Empty is now the default to avoid
suggesting a file that does not exist. Existing saved paths must be edited manually.


## Diagnostic build 1.0.1

This version adds ESPHome logs under `desktop_launcher` without changing image loading.
After pushing this version, explicitly update the plugin in Tessera and install the
firmware; Save & send alone does not update C++. If the repository has a GitHub release,
publish a matching new release to make the new code available through that channel.
Verify the boot line `Desktop launcher 1.0.1 diagnostic build`.

- `pictures=no`: firmware picture hooks are unavailable.
- `pictures=yes` and `Picture outcome: NONE`: investigate add-on fetching/preparation.
- `Picture outcome: FAILED`: investigate delivery to the screen.
- `Picture outcome: SHOWN`: core reports the picture placed; investigate drawing/layout.
- No outcome: inspect the add-on picture request logs alongside the reported place size.

The Screen features sensor value `none` is not the picture capability; in the inspected
core it describes backlight/standby. The tile inspector's plain HA state for a synthetic
`plugin:...` entity does not contain the plugin's separate state payload.

The prepared `tessera-local-icons-supervisor.patch` targets Screen Manager, not this
plugin or the display YAML. It is a candidate fix for Supervisor `/local/` routing,
not a hardware-tested or deployed fix.
