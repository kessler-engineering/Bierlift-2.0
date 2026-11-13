# Erzeugt BierLift2/web_gz.h: gzip-gepackte Fassungen von script.js und style.css.
#
# Quelle bleiben die lesbaren Dateien BierLift2/web_script.js.h und BierLift2/web_style.css.h
# (dort Texte ändern, z. B. das Sprach-Array "lang"). Ausgeliefert werden nur die gepackten
# Daten (≈ ein Viertel der Größe, Browser entpackt selbst).
#
# Läuft automatisch vor jedem PlatformIO-Build (extra_scripts in platformio.ini).
# Für die Arduino-IDE vorher von Hand ausführen:  python tools/web_gzip.py
# Die Datei wird nur neu geschrieben, wenn sich der Inhalt geändert hat.
# Hinweis: Verschiedene Python-Versionen packen (zlib vs. zlib-ng) minimal anders – beides
# ist gültiges gzip. Wechselt man zwischen PlatformIO und System-Python, wird web_gz.h
# deshalb einmal neu geschrieben; das ist harmlos.
import gzip, os, re, sys

try:
    Import("env")  # noqa: F821  (nur unter PlatformIO/SCons vorhanden)
    PROJECT = env.subst("$PROJECT_DIR")  # noqa: F821
except NameError:
    PROJECT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

SRC = os.path.join(PROJECT, "BierLift2")
OUT = os.path.join(SRC, "web_gz.h")

# (Quelldatei, Name des Arrays in der Quelle, Name des gepackten Arrays)
FILES = [
    ("web_script.js.h", "script_js", "script_js_gz"),
    ("web_style.css.h", "style_css", "style_css_gz"),
]


def extract(path, name):
    text = open(path, encoding="utf-8").read()
    # erste (aktive) Fassung; eine auskommentierte zweite Fassung weiter unten wird ignoriert
    m = re.search(r'const char ' + name + r'\[\] PROGMEM = R"=====\((.*?)\)=====";', text, re.S)
    if not m:
        sys.exit("web_gzip.py: Array %s in %s nicht gefunden" % (name, path))
    return m.group(1).encode("utf-8")


def c_array(name, data):
    lines = []
    for i in range(0, len(data), 24):
        lines.append("  " + ",".join("0x%02x" % b for b in data[i:i + 24]) + ",")
    return ("const uint8_t %s[] PROGMEM = {\n%s\n};\nconst size_t %s_size = %d;\n"
            % (name, "\n".join(lines), name, len(data)))


parts = ["// AUTOMATISCH ERZEUGT von tools/web_gzip.py – NICHT VON HAND ÄNDERN.\n"
         "// Quelle: web_script.js.h und web_style.css.h (dort ändern, dann neu bauen).\n"
         "#pragma once\n#include <Arduino.h>\n"]
for src, name, gzname in FILES:
    raw = extract(os.path.join(SRC, src), name)
    packed = gzip.compress(raw, compresslevel=9, mtime=0)   # mtime=0: gleiche Eingabe -> gleiche Datei
    parts.append("\n// %s: %d Byte -> %d Byte gepackt\n" % (src, len(raw), len(packed)))
    parts.append(c_array(gzname, packed))
out = "".join(parts)

old = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
if out != old:
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write(out)
    print("web_gzip.py: web_gz.h neu erzeugt")
else:
    print("web_gzip.py: web_gz.h aktuell")
