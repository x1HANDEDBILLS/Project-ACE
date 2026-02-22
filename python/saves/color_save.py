import json
import os

# Path logic: This finds the 'python' root folder and places settings.json there
# We go up two levels because this file is in python/saves/
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SETTINGS_FILE = os.path.join(BASE_DIR, "settings.json")

def save_visual_settings(hex_color):
    """Saves the selected theme color to the root settings.json file."""
    data = {}
    
    # 1. Load existing settings so we don't overwrite other configurations
    if os.path.exists(SETTINGS_FILE):
        try:
            with open(SETTINGS_FILE, 'r') as f:
                data = json.load(f)
        except Exception as e:
            print(f"[SYSTEM] Error reading settings.json: {e}")
            data = {}

    # 2. Update only the theme color field
    data['theme_color'] = hex_color

    # 3. Write the updated data back to the file
    try:
        with open(SETTINGS_FILE, 'w') as f:
            json.dump(data, f, indent=4)
        print(f"[SYSTEM] Visual settings updated: {hex_color}")
    except Exception as e:
        print(f"[SYSTEM] Failed to write settings: {e}")