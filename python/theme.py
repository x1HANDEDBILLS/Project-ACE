import numpy as np
import json
import os
from PySide6.QtGui import QColor, QImage, QPixmap
from PySide6.QtWidgets import QApplication

# --- PERSISTENCE LOGIC ---
# Locates settings.json in the same folder as this script
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
SETTINGS_FILE = os.path.join(BASE_DIR, "settings.json")

def load_saved_hex():
    """Reads the saved hex color from settings.json or returns RED as default."""
    if os.path.exists(SETTINGS_FILE):
        try:
            with open(SETTINGS_FILE, 'r') as f:
                data = json.load(f)
                saved_color = data.get('theme_color', "#FF1E1E")
                return saved_color
        except Exception as e:
            print(f"[THEME] Load error: {e}")
    return "#FF1E1E"

# 1. THEME DEFINITIONS (Global Presets)
THEMES = {
    "RED":    {"p": (255, 30, 30),   "s": (160, 165, 175), "bg_l": (45, 5, 5),   "bg_d": (10, 2, 2),   "hex": "#FF1E1E"},
    "CYAN":    {"p": (0, 255, 255),  "s": (150, 165, 180), "bg_l": (5, 35, 45),  "bg_d": (2, 5, 10),   "hex": "#00FFFF"},
    "AMBER":   {"p": (255, 190, 0),  "s": (165, 160, 140), "bg_l": (40, 30, 5),  "bg_d": (10, 8, 2),   "hex": "#FFBE00"},
    "PURPLE":  {"p": (180, 50, 255), "s": (160, 150, 180), "bg_l": (25, 5, 50),  "bg_d": (5, 2, 15),   "hex": "#B432FF"}
}

ACTIVE = {}

def set_active_theme(name_or_rgb="RED"):
    """
    Sets the global ACTIVE theme. 
    Accepts preset names ("RED"), Hex strings ("#FFFFFF"), or RGB tuples (255,0,0).
    """
    global ACTIVE
    
    # CASE A: Hex string (from Color Picker)
    if isinstance(name_or_rgb, str) and name_or_rgb.startswith("#"):
        c = QColor(name_or_rgb)
        r, g, b = c.red(), c.green(), c.blue()
        data = {
            "p": (r, g, b), "s": (160, 165, 175),
            "bg_l": (int(r*0.15), int(g*0.15), int(b*0.15)), "bg_d": (5, 5, 5),
            "hex": name_or_rgb
        }
    # CASE B: Preset Name string
    elif isinstance(name_or_rgb, str):
        data = THEMES.get(name_or_rgb.upper(), THEMES["RED"])
    # CASE C: RGB Tuple
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

    # --- SMART REFRESH TRIGGER ---
    app = QApplication.instance()
    if app: 
        # Update global stylesheet
        app.setStyleSheet(get_global_qss())
        
        # Tell every widget to look at the new ACTIVE dict and repaint
        for widget in app.allWidgets():
            # If a widget has a custom "refresh_theme" function, run it
            if hasattr(widget, 'refresh_theme'):
                widget.refresh_theme()
            # Force a standard repaint for all other widgets
            widget.update()

def get_numpy_gradient(w, h):
    """Generates a dithered background gradient based on the ACTIVE theme."""
    if w <= 0 or h <= 0: return QPixmap()
    if not ACTIVE: set_active_theme(load_saved_hex())
    
    # Create normalized meshgrid
    x = np.linspace(0, 1, w, dtype=np.float32)
    y = np.linspace(0, 1, h, dtype=np.float32)
    xv, yv = np.meshgrid(x, y)
    ratio = (xv + yv) / 2.0 
    
    c_l, c_d = ACTIVE["bg_light"], ACTIVE["bg_dark"]
    
    # Color interpolation
    r_base = (c_l[0] + (c_d[0] - c_l[0]) * ratio)
    g_base = (c_l[1] + (c_d[1] - c_l[1]) * ratio)
    b_base = (c_l[2] + (c_d[2] - c_l[2]) * ratio)

    # Apply Dither noise to prevent banding on the Pi screen
    noise = np.random.normal(loc=0.0, scale=1.2, size=(h, w)).astype(np.float32)
    
    r = np.clip(r_base + noise, 0, 255).astype(np.uint8)
    g = np.clip(g_base + noise, 0, 255).astype(np.uint8)
    b = np.clip(b_base + noise, 0, 255).astype(np.uint8)
    
    arr = np.dstack((r, g, b))
    img = QImage(arr.data, w, h, w*3, QImage.Format_RGB888).copy()
    return QPixmap.fromImage(img)

def get_global_qss():
    """Returns the CSS string for the application based on the current ACTIVE theme."""
    p_hex = ACTIVE.get("hex", "#FF1E1E")
    s = "rgb(160, 165, 175)"
    return f"""
        QWidget {{ background-color: transparent; color: {s}; font-family: 'Consolas'; font-size: 13px; }}
        QMainWindow {{ background-color: #050505; }}
        QLabel#ThemeLabel {{ color: {p_hex}; }}
        /* Specific tactical classes */
        .TacticalLabel {{ color: {p_hex}; font-weight: bold; }}
    """

# --- AUTO-INITIALIZATION ON BOOT ---
set_active_theme(load_saved_hex())