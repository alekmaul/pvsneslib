#!/usr/bin/env python3
"""Mini frontend libretro headless : lance une ROM N frames, sauve une capture
PNG et un dump de la WRAM. Usage : run_rom.py rom.sfc frames out.png [wram.bin]"""
import ctypes as C
import os
import sys
import struct
import tempfile
import zlib

# Coeur snes9x : variable SNES9X_CORE, sinon emplacement par defaut selon l'OS
# (Windows : perf/cores/snes9x_libretro.dll, telecharge depuis le buildbot libretro).
_HERE = os.path.dirname(os.path.abspath(__file__))
if sys.platform == "win32":
    _DEFAULT_CORE = os.path.join(_HERE, "cores", "snes9x_libretro.dll")
elif sys.platform == "darwin":
    _DEFAULT_CORE = os.path.join(_HERE, "cores", "snes9x_libretro.dylib")
else:
    _DEFAULT_CORE = "/usr/lib/x86_64-linux-gnu/libretro/snes9x_libretro.so"
CORE = os.environ.get("SNES9X_CORE", _DEFAULT_CORE)
TMPDIR = tempfile.gettempdir().encode()

class GameInfo(C.Structure):
    _fields_ = [("path", C.c_char_p), ("data", C.c_void_p),
                ("size", C.c_size_t), ("meta", C.c_char_p)]

ENV_CB = C.CFUNCTYPE(C.c_bool, C.c_uint, C.c_void_p)
VIDEO_CB = C.CFUNCTYPE(None, C.c_void_p, C.c_uint, C.c_uint, C.c_size_t)
AUDIO_CB = C.CFUNCTYPE(None, C.c_int16, C.c_int16)
AUDIOB_CB = C.CFUNCTYPE(C.c_size_t, C.c_void_p, C.c_size_t)
POLL_CB = C.CFUNCTYPE(None)
STATE_CB = C.CFUNCTYPE(C.c_int16, C.c_uint, C.c_uint, C.c_uint, C.c_uint)

frame = {}
pixfmt = [1]  # 0=0RGB1555 1=XRGB8888 2=RGB565

def env(cmd, data):
    if cmd == 10:                       # SET_PIXEL_FORMAT
        pixfmt[0] = C.cast(data, C.POINTER(C.c_int))[0]
        return True
    if cmd in (9, 31):                  # SYSTEM_DIRECTORY / SAVE_DIRECTORY
        C.cast(data, C.POINTER(C.c_char_p))[0] = TMPDIR
        return True
    if cmd == 27:                       # GET_LOG_INTERFACE
        return False
    return False

def video(data, w, h, pitch):
    if data:
        frame["buf"] = C.string_at(data, pitch * h)
        frame["w"], frame["h"], frame["pitch"] = w, h, pitch

# Manette 1 : RUN_ROM_INPUT="debut-fin:TOUCHE+TOUCHE,..." (frames incluses),
# ex. "300-305:START,700-760:RIGHT+B"
BUTTONS = {"B": 0, "Y": 1, "SELECT": 2, "START": 3, "UP": 4, "DOWN": 5, "LEFT": 6,
           "RIGHT": 7, "A": 8, "X": 9, "L": 10, "R": 11}
inputs = []
for part in filter(None, os.environ.get("RUN_ROM_INPUT", "").split(",")):
    span, keys = part.split(":")
    a, _, b = span.partition("-")
    inputs.append((int(a), int(b or a), {BUTTONS[k.strip().upper()] for k in keys.split("+")}))
cur = [0]  # frame en cours

def input_state(port, device, index, key):
    if port != 0 or device != 1:        # RETRO_DEVICE_JOYPAD
        return 0
    return int(any(a <= cur[0] <= b and key in keys for a, b, keys in inputs))

cbs = [ENV_CB(env), VIDEO_CB(video), AUDIO_CB(lambda l, r: None),
       AUDIOB_CB(lambda d, n: n), POLL_CB(lambda: None),
       STATE_CB(input_state)]

def save_png(path):
    w, h, p, buf = frame["w"], frame["h"], frame["pitch"], frame["buf"]
    rows = []
    for y in range(h):
        row = bytearray(b"\0")
        for x in range(w):
            if pixfmt[0] == 1:
                b, g, r, _ = buf[y * p + x * 4:y * p + x * 4 + 4]
            else:
                v = struct.unpack_from("<H", buf, y * p + x * 2)[0]
                r, g, b = (v >> 11) << 3, ((v >> 5) & 63) << 2, (v & 31) << 3
            row += bytes((r, g, b))
        rows.append(bytes(row))
    raw = zlib.compress(b"".join(rows))
    def chunk(t, d):
        return struct.pack(">I", len(d)) + t + d + struct.pack(">I", zlib.crc32(t + d))
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", raw) + chunk(b"IEND", b""))

def main():
    rom, frames, png = sys.argv[1], int(sys.argv[2]), sys.argv[3]
    core = C.CDLL(CORE)
    core.retro_set_environment(cbs[0]); core.retro_set_video_refresh(cbs[1])
    core.retro_set_audio_sample(cbs[2]); core.retro_set_audio_sample_batch(cbs[3])
    core.retro_set_input_poll(cbs[4]); core.retro_set_input_state(cbs[5])
    core.retro_init()
    data = open(rom, "rb").read()
    buf = C.create_string_buffer(data, len(data))
    gi = GameInfo(rom.encode(), C.cast(buf, C.c_void_p), len(data), None)
    core.retro_load_game.argtypes = [C.POINTER(GameInfo)]
    if not core.retro_load_game(C.byref(gi)):
        sys.exit("chargement ROM impossible")
    # RUN_ROM_ZERORAM=1 : WRAM a zero au demarrage (snes9x la remplit de 0x55,
    # d'autres emulateurs et certaines consoles la laissent a 0)
    core.retro_get_memory_data.restype = C.c_void_p
    core.retro_get_memory_size.restype = C.c_size_t
    if os.environ.get("RUN_ROM_ZERORAM") == "1":
        C.memset(core.retro_get_memory_data(2), 0, core.retro_get_memory_size(2))
    # RUN_ROM_SNAP=N : une capture toutes les N frames (out_000120.png...)
    snap = int(os.environ.get("RUN_ROM_SNAP", "0"))
    # RUN_ROM_WATCH="debut-fin:nom=adresse/taille,..." : valeurs WRAM (adresse
    # dans la banque $7E, taille 1 ou 2, signee si prefixee par s) a chaque frame
    watch, wrange = [], (0, -1)
    if os.environ.get("RUN_ROM_WATCH"):
        span, _, spec = os.environ["RUN_ROM_WATCH"].partition(":")
        a, _, b = span.partition("-")
        wrange = (int(a), int(b))
        for item in spec.split(","):
            nm, _, rest = item.partition("=")
            addr, _, size = rest.partition("/")
            signed = size.startswith("s")
            watch.append((nm, int(addr, 16), int(size.lstrip("s")), signed))
    ram = core.retro_get_memory_data(2)
    # RUN_ROM_POKE="frame:adresse=valeur/taille,..." : ecrit en WRAM apres la frame
    pokes = []
    for item in filter(None, os.environ.get("RUN_ROM_POKE", "").split(",")):
        fr, _, rest = item.partition(":")
        addr, _, rest = rest.partition("=")
        val, _, size = rest.partition("/")
        pokes.append((int(fr), int(addr, 16), int(val, 0), int(size or 1)))
    for f in range(frames):
        cur[0] = f
        core.retro_run()
        for fr, addr, val, size in pokes:
            if fr == f:
                data = (val & ((1 << (8 * size)) - 1)).to_bytes(size, "little")
                C.memmove(ram + addr, data, size)
        if watch and wrange[0] <= f <= wrange[1]:
            vals = []
            for nm, addr, size, signed in watch:
                raw = C.string_at(ram + addr, size)
                vals.append(f"{nm}={int.from_bytes(raw, 'little', signed=signed)}")
            print(f"{f}: " + " ".join(vals))
        if snap and f % snap == 0 and "buf" in frame:
            save_png(f"{os.path.splitext(png)[0]}_{f:06d}.png")
    save_png(png)
    if len(sys.argv) > 4:
        core.retro_get_memory_data.restype = C.c_void_p
        core.retro_get_memory_size.restype = C.c_size_t
        ptr, n = core.retro_get_memory_data(2), core.retro_get_memory_size(2)
        open(sys.argv[4], "wb").write(C.string_at(ptr, n))
    print(f"ok {frame['w']}x{frame['h']} fmt={pixfmt[0]}")

main()
