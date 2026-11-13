# Baut die Steuer-Symbole (Variante A, schlicht) in die Symbol-Schrift BierLift2/web/fonticon.woff2.h ein.
#
#   U+E900  Winkel hoch          (hoch)
#   U+E901  Doppelwinkel hoch    (schnell hoch)
#   U+E902  Stopp-Quadrat        (stopp)
#   U+E903  Winkel runter        (runter)
#   U+E904  Doppelwinkel runter  (schnell runter)
#
# Geometrie nach den Tabler-Icons (MIT-Lizenz, tabler.io): chevron-up/-down, chevrons-up/-down,
# player-stop – 24er-Raster, Linien mit runden Enden und Ecken. Die Linien werden hier als
# gefüllte Umrisse erzeugt (Rechteck je Strecke + Kreis je Punkt, gleiche Drehrichtung, damit sie
# sich vereinigen). Alle fünf Zeichen sind gleich breit (1000 Einheiten = 1 Geviert), damit die
# Knöpfe bei gleicher Schriftgröße und Polsterung exakt gleich groß werden.
# Die bisherigen Zeichen der Schrift bleiben unverändert.
#
# Aufruf (braucht fontTools und brotli):  python tools/font_icons.py
# Danach: In web_style.css.h die Versionsnummer an url(fonts/fonticon.woff2?v=N) erhöhen, sonst
# behalten Browser die alte Schrift (wird dauerhaft zwischengespeichert, siehe CRMui3::sendStatic).
import io, math, os, re, sys
from fontTools.ttLib import TTFont
from fontTools.pens.ttGlyphPen import TTGlyphPen

HERE = os.path.dirname(os.path.abspath(__file__))
HDR = os.path.join(HERE, '..', 'BierLift2', 'web', 'fonticon.woff2.h')

SCALE = 1000 / 24 * 1.25    # Tabler-Raster -> Schrift-Einheiten (Stopp-Quadrat), 25 % größer als im Raster
CHEVRON = 1.2               # Winkel zusätzlich 20 % größer, damit sie neben dem Quadrat nicht klein wirken
STROKE = 1.7                # Linienstärke im Tabler-Raster (Original 2; etwas schlanker)
MID_X, MID_Y = 500, 350     # Mitte des Gevierts (Höhe -150 … 850)


def tx(x, y, k=1.0):
    """Tabler-Koordinate (y nach unten, Mitte 12/12) -> Schrift-Koordinate (y nach oben)."""
    return (MID_X + (x - 12) * SCALE * k, MID_Y - (y - 12) * SCALE * k)


def area(pts):
    return sum(x0 * y1 - x1 * y0 for (x0, y0), (x1, y1) in zip(pts, pts[1:] + pts[:1])) / 2


def clockwise(pts):
    return pts if area(pts) < 0 else pts[::-1]   # y nach oben: negative Fläche = im Uhrzeigersinn


def circle(cx, cy, r, n=24):
    return clockwise([(cx + r * math.cos(2 * math.pi * i / n), cy + r * math.sin(2 * math.pi * i / n)) for i in range(n)])


def segment(p, q, h):
    (x0, y0), (x1, y1) = p, q
    L = math.hypot(x1 - x0, y1 - y0)
    nx, ny = -(y1 - y0) / L * h, (x1 - x0) / L * h
    return clockwise([(x0 + nx, y0 + ny), (x1 + nx, y1 + ny), (x1 - nx, y1 - ny), (x0 - nx, y0 - ny)])


def polyline(points):
    """Tabler-Polylinie -> Liste von Umrissen (Strecken + runde Punkte)."""
    h = STROKE * SCALE / 2
    pts = [tx(x, y, CHEVRON) for x, y in points]
    contours = [segment(a, b, h) for a, b in zip(pts, pts[1:])]
    contours += [circle(x, y, h) for x, y in pts]
    return contours


def rounded_rect(x0, y0, x1, y1, r, n=6):
    """Abgerundetes Rechteck in Schrift-Koordinaten (x0<x1, y0<y1)."""
    pts = []
    for cx, cy, a0 in ((x1 - r, y1 - r, 0), (x0 + r, y1 - r, 90), (x0 + r, y0 + r, 180), (x1 - r, y0 + r, 270)):
        for i in range(n + 1):
            a = math.radians(a0 + 90 * i / n)
            pts.append((cx + r * math.cos(a), cy + r * math.sin(a)))
    return pts


def stop_square():
    # Tabler player-stop: Quadrat 5…19 mit Radius 2, Linie STROKE breit -> Außen- und Innenkontur
    h = STROKE / 2
    ax, ay = tx(5 - h, 19 + h); bx, by = tx(19 + h, 5 - h)
    cx, cy = tx(5 + h, 19 - h); dx, dy = tx(19 - h, 5 + h)
    outer = clockwise(rounded_rect(ax, ay, bx, by, (2 + h) * SCALE))
    inner = clockwise(rounded_rect(cx, cy, dx, dy, max(2 - h, 0.2) * SCALE))[::-1]   # Loch: Gegenrichtung
    return [outer, inner]


GLYPHS = [
    (0xE900, 'bl-up',     polyline([(6, 15), (12, 9), (18, 15)])),
    (0xE901, 'bl-up2',    polyline([(7, 11), (12, 6), (17, 11)]) + polyline([(7, 17), (12, 12), (17, 17)])),
    (0xE902, 'bl-stop',   stop_square()),
    (0xE903, 'bl-down',   polyline([(6, 9), (12, 15), (18, 9)])),
    (0xE904, 'bl-down2',  polyline([(7, 7), (12, 12), (17, 7)]) + polyline([(7, 13), (12, 18), (17, 13)])),
]


def main():
    txt = open(HDR, encoding='utf-8').read()
    body = txt[txt.index('{') + 1: txt.rindex('}')]
    data = bytes(int(x, 16) for x in re.findall(r'0x([0-9A-Fa-f]{2})', body))
    font = TTFont(io.BytesIO(data))
    order = font.getGlyphOrder()
    for cp, name, contours in GLYPHS:
        pen = TTGlyphPen(None)
        for c in contours:
            pen.moveTo(tuple(round(v) for v in c[0]))
            for p in c[1:]:
                pen.lineTo(tuple(round(v) for v in p))
            pen.closePath()
        g = pen.glyph()
        g.recalcBounds(font['glyf'])
        if name not in order:
            order.append(name)
        font['glyf'][name] = g
        font['hmtx'][name] = (1000, g.xMin)
        for t in font['cmap'].tables:
            if t.isUnicode():
                t.cmap[cp] = name
    font.setGlyphOrder(order)
    font['maxp'].numGlyphs = len(order)
    out = io.BytesIO()
    font.flavor = 'woff2'
    font.save(out)
    b = out.getvalue()
    lines = [', '.join('0x%02X' % x for x in b[i:i + 16]) for i in range(0, len(b), 16)]
    hdr = ('#define fonticon_woff2_size %d\nconst uint8_t fonticon_woff2[] PROGMEM = {\n%s\n};\n'
           % (len(b), ',\n'.join(lines)))
    with open(HDR, 'w', encoding='utf-8', newline='\n') as f:
        f.write(hdr)
    print('fonticon.woff2: %d -> %d Byte, %d Zeichen ergänzt' % (len(data), len(b), len(GLYPHS)))


if __name__ == '__main__':
    main()
