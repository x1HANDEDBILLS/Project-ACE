import numpy as np
from PySide6.QtGui import QColor, QImage, QPixmap
from PySide6.QtWidgets import QApplication

# 1. THEME DEFINITIONS (Global Source of Truth)
THEMES = {
    "RED":    {"p": (255, 30, 30),   "s": (160, 165, 175), "bg_l": (45, 5, 5),   "bg_d": (10, 2, 2),   "hex": "#FF1E1E"},
    "CYAN":    {"p": (0, 255, 255),  "s": (150, 165, 180), "bg_l": (5, 35, 45),  "bg_d": (2, 5, 10),   "hex": "#00FFFF"},
    "AMBER":   {"p": (255, 190, 0),  "s": (165, 160, 140), "bg_l": (40, 30, 5),  "bg_d": (10, 8, 2),   "hex": "#FFBE00"},
    "PURPLE":  {"p": (180, 50, 255), "s": (160, 150, 180), "bg_l": (25, 5, 50),  "bg_d": (5, 2, 15),   "hex": "#B432FF"}
}

ACTIVE = {}

def set_active_theme(name_or_rgb="CYAN"):
    global ACTIVE
    if isinstance(name_or_rgb, str) and name_or_rgb.startswith("#"):
        c = QColor(name_or_rgb)
        r, g, b = c.red(), c.green(), c.blue()
        data = {
            "p": (r, g, b), "s": (160, 165, 175),
            "bg_l": (int(r*0.15), int(g*0.15), int(b*0.15)), "bg_d": (5, 5, 5),
            "hex": name_or_rgb
        }
    elif isinstance(name_or_rgb, str):
        data = THEMES.get(name_or_rgb.upper(), THEMES["CYAN"])
    else:
        r, g, b = name_or_rgb
        data = {
            "p": (r, g, b), "s": (160, 165, 175),
            "bg_l": (int(r*0.15), int(g*0.15), int(b*0.15)), "bg_d": (5, 5, 5),
            "hex": '#{:02x}{:02x}{:02x}'.format(r, g, b)
        }

    ACTIVE.update({
        "primary": QColor(*data["p"]),
        "secondary": QColor(*data["s"]),
        "primary_dim": QColor(*data["p"], 80),
        "bg_light": data["bg_l"],
        "bg_dark": data["bg_d"],
        "hex": data["hex"],
        "raw_p": data["p"]
    })

    app = QApplication.instance()
    if app: app.setStyleSheet(get_global_qss())

def get_numpy_gradient(w, h):
    if w <= 0 or h <= 0: return QPixmap()
    if not ACTIVE: set_active_theme("RED")
    
    # 1. Create the base slope
    x = np.linspace(0, 1, w, dtype=np.float32)
    y = np.linspace(0, 1, h, dtype=np.float32)
    xv, yv = np.meshgrid(x, y)
    ratio = (xv + yv) / 2.0 
    
    c_l, c_d = ACTIVE["bg_light"], ACTIVE["bg_dark"]
    
    # 2. Calculate base colors
    r_base = (c_l[0] + (c_d[0] - c_l[0]) * ratio)
    g_base = (c_l[1] + (c_d[1] - c_l[1]) * ratio)
    b_base = (c_l[2] + (c_d[2] - c_l[2]) * ratio)

    # 3. HIGH-QUALITY DITHER (The Fix)
    # We use a normal distribution noise to "blur" the color steps
    # Increase the 'scale' (e.g., to 1.5 or 2.0) if you still see lines
    noise = np.random.normal(loc=0.0, scale=1.2, size=(h, w)).astype(np.float32)
    
    r = np.clip(r_base + noise, 0, 255).astype(np.uint8)
    g = np.clip(g_base + noise, 0, 255).astype(np.uint8)
    b = np.clip(b_base + noise, 0, 255).astype(np.uint8)
    
    arr = np.dstack((r, g, b))
    img = QImage(arr.data, w, h, w*3, QImage.Format_RGB888).copy()
    return QPixmap.fromImage(img)

def get_global_qss():
    s = "rgb(160, 165, 175)"
    return f"""
        QWidget {{ background-color: transparent; color: {s}; font-family: 'Consolas'; font-size: 13px; }}
        QMainWindow {{ background-color: #050505; }}
    """

set_active_theme("RED")