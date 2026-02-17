import json
import os
from pathlib import Path
import theme

# 1. Setup the Dynamic Path
# This locates the file at: /home/user/ACE/python/gui/settings/visual.json
BASE_DIR = Path(__file__).resolve().parent
SETTINGS_FILE = BASE_DIR / "settings" / "visual.json"

def initialize_theme():
    """
    Loads the saved hex from visual.json and applies it to the theme engine.
    Called by main.py during the boot sequence.
    """
    if SETTINGS_FILE.exists():
        try:
            with open(SETTINGS_FILE, 'r') as f:
                data = json.load(f)
                saved_hex = data.get("theme_color")
                
                # Validation: Ensure it's a string and starts with #
                if saved_hex and isinstance(saved_hex, str) and saved_hex.startswith("#"):
                    print(f"SYSTEM // Applying saved theme: {saved_hex}")
                    theme.set_active_theme(saved_hex)
                    return True
        except Exception as e:
            print(f"SYSTEM // Startup Load Error: {e}")
    
    # Fallback to RED if file is missing, empty, or corrupted
    print("SYSTEM // No valid save found. Defaulting to RED.")
    theme.set_active_theme("#FF0000")
    return False

def save_visual_settings(theme_hex):
    """
    Saves the hex string directly to the JSON file.
    """
    # Prevent saving 'None' or invalid data which causes the "Grey-out"
    if not theme_hex or not str(theme_hex).startswith("#"):
        print(f"SYSTEM // Save rejected: {theme_hex} is not a valid hex.")
        return

    try:
        # Create the 'settings' folder if it doesn't exist
        SETTINGS_FILE.parent.mkdir(parents=True, exist_ok=True)
        
        data = {}
        if SETTINGS_FILE.exists():
            with open(SETTINGS_FILE, 'r') as f:
                try:
                    data = json.load(f)
                except json.JSONDecodeError:
                    data = {}

        data["theme_color"] = theme_hex
        
        with open(SETTINGS_FILE, 'w') as f:
            json.dump(data, f, indent=4)
            
        print(f"SYSTEM // Theme {theme_hex} permanently saved.")
    except Exception as e:
        print(f"SYSTEM // Error saving to {SETTINGS_FILE}: {e}")