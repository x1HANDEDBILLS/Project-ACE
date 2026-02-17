import numpy as np
from PySide6.QtGui import QColor, QImage, QPixmap

THEMES = {
    "RED":    {"p": (255, 30, 30),   "s": (160, 165, 175), "bg_l": (45, 5, 5),   "bg_d": (10, 2, 2),   "hex": "#FF1E1E"},
    "CYAN":    {"p": (0, 255, 255),  "s": (150, 165, 180), "bg_l": (5, 35, 45),  "bg_d": (2, 5, 10),   "hex": "#00FFFF"},
    "AMBER":   {"p": (255, 190, 0),  "s": (165, 160, 140), "bg_l": (40, 30, 5),  "bg_d": (10, 8, 2),   "hex": "#FFBE00"},
    "PURPLE":  {"p": (180, 50, 255), "s": (160, 150, 180), "bg_l": (25, 5, 50),  "bg_d": (5, 2, 15),   "hex": "#B432FF"}
}

ACTIVE = {}

def set_active_theme(name_or_rgb="RED"):
    global ACTIVE
    
    # 1. Handle Hex String (from save file)
    if isinstance(name_or_rgb, str) and name_or_rgb.startswith("#"):
        c = QColor(name_or_rgb)
        r, g, b = c.red(), c.green(), c.blue()
        data = {
            "p": (r, g, b),
            "s": (160, 165, 175),
            "bg_l": (int(r*0.15), int(g*0.15), int(b*0.15)),
            "bg_d": (5, 5, 5),
            "hex": name_or_rgb
        }
    # 2. Handle Preset Name
    elif isinstance(name_or_rgb, str):
        data = THEMES.get(name_or_rgb.upper(), THEMES["RED"])
    # 3. Handle RGB Tuple
    elif isinstance(name_or_rgb, (tuple, list)):
        r, g, b = name_or_rgb
        data = {
            "p": (r, g, b),
            "s": (160, 165, 175),
            "bg_l": (int(r*0.15), int(g*0.15), int(b*0.15)),
            "bg_d": (5, 5, 5),
            "hex": '#{:02x}{:02x}{:02x}'.format(r, g, b)
        }
    else:
        data = THEMES["RED"]

    ACTIVE = {
        "primary": QColor(*data["p"]),
        "secondary": QColor(*data["s"]),
        "bg_light": data["bg_l"],
        "bg_dark": data["bg_d"],
        "hex": data["hex"],
        "raw_p": data["p"]
    }

# Ensure ACTIVE is populated immediately on import
set_active_theme("RED")

def set_custom_color(qcolor):
    r, g, b = qcolor.red(), qcolor.green(), qcolor.blue()
    set_active_theme((r, g, b))

def get_numpy_gradient(w, h):
    x, y = np.linspace(0, 1, w), np.linspace(0, 1, h)
    xv, yv = np.meshgrid(x, y)
    ratio = (xv + yv) / 2.0 
    c_l, c_d = ACTIVE["bg_light"], ACTIVE["bg_dark"]
    noise = np.random.normal(0, 1.2, (h, w))
    r = np.clip((c_l[0] + (c_d[0] - c_l[0]) * ratio) + noise, 0, 255)
    g = np.clip((c_l[1] + (c_d[1] - c_l[1]) * ratio) + noise, 0, 255)
    b = np.clip((c_l[2] + (c_d[2] - c_l[2]) * ratio) + noise, 0, 255)
    arr = np.dstack((r, g, b)).astype(np.uint8)
    img = QImage(arr.data, w, h, w*3, QImage.Format_RGB888).copy()
    return QPixmap.fromImage(img)

def get_qss():
    # --- DYNAMIC SOURCE OF TRUTH ---
    p = ACTIVE.get("hex", "#FF1E1E")
    raw_p = ACTIVE.get("raw_p", (255, 30, 30))
    s = "rgb(160, 165, 175)"
    dark_grey = "#101010"
    groove = "#111111"

    return f"""
        QWidget {{
            background-color: transparent;
            color: {s};
            font-family: 'Consolas';
            font-size: 14px;
        }}

        QComboBox {{
            border: 1px solid {s};
            border-radius: 4px;
            background: #0a0a0a;
            color: {p};
            padding: 7px 32px 7px 11px;
            min-height: 44px;
        }}
        QComboBox::drop-down {{
            width: 32px;
            border-left: 1px solid {s};
            background: #080808;
        }}
        QComboBox::down-arrow {{
            border-left: 7px solid transparent;
            border-right: 7px solid transparent;
            border-top: 9px solid {p};
            margin-right: 9px;
        }}
        QComboBox::down-arrow:on {{
            border-top: none;
            border-bottom: 9px solid {p};
        }}
        QComboBox QAbstractItemView {{
            background: {dark_grey} !important;
            color: {s};
            selection-background-color: {p};
            selection-color: black;
            border: 1px solid {s};
            outline: none !important;
        }}
        QComboBox QAbstractItemView::item:selected {{
            background: {p} !important;
            color: black;
        }}

        QCheckBox::indicator {{
            border: 1px solid {s};
            width: 22px;
            height: 22px;
            border-radius: 3px;
        }}
        QCheckBox::indicator:checked {{
            background-color: {p};
            border: 1px solid {p};
        }}

        QProgressBar {{
            border: 1px solid {s};
            border-radius: 4px;
            background: {groove};
            text-align: center;
            color: white;
            font-size: 13px;
            height: 26px;
        }}
        QProgressBar::chunk {{
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                        stop:0 {p}, stop:1 rgba({raw_p[0]//2},{raw_p[1]//2},{raw_p[2]//2},255));
            border-radius: 3px;
        }}

        QSlider::groove:horizontal {{
            border: 1px solid {s};
            height: 11px;
            background: {groove};
            margin: 0px;
            border-radius: 6px;
        }}
        QSlider::handle:horizontal {{
            background: {p};
            border: 1px solid {s};
            width: 44px;
            height: 44px;
            margin: -17px 0;
            border-radius: 0px;
        }}
        QSlider::sub-page:horizontal {{
            background: {p};
            border-radius: 6px;
        }}

        QDial {{
            background: transparent !important;
            min-width: 130px;
            min-height: 130px;
        }}
        QDial::handle {{
            background: {p};
            border: 3px solid {s};
            width: 22px;
            height: 22px;
            border-radius: 11px;
        }}

        QLCDNumber {{
            background: transparent;
            color: {p};
            border: 1px solid {s};
            border-radius: 4px;
            font-size: 26px;
            min-width: 200px;
            min-height: 65px;
        }}

        QTabWidget::pane {{
            border: none;
        }}
        QTabBar::tab {{
            background: transparent;
            color: {s};
            padding: 12px 24px;
            font-size: 15px;
            min-width: 110px;
        }}
        QTabBar::tab:selected {{
            color: {p};
        }}
    """