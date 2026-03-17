import os, sys, subprocess, datetime, time

def run_guardian(root_path):
    # Only scan these two specific folders
    targets = [os.path.join(root_path, 'src'), os.path.join(root_path, 'include')]
    count = 0
    style_str = "{BasedOnStyle: LLVM, AccessModifierOffset: -4, AlignAfterOpenBracket: Align, AllowShortBlocksOnASingleLine: Always, AllowShortFunctionsOnASingleLine: All, AllowShortIfStatementsOnASingleLine: Always, AllowShortLoopsOnASingleLine: true, BreakBeforeBraces: Attach, ColumnLimit: 120, IndentWidth: 4, MaxEmptyLinesToKeep: 0, KeepEmptyLinesAtTheStartOfBlocks: false, ObjCSpaceAfterProperty: false, PointerAlignment: Right, SpaceBeforeParens: ControlStatements}"
    
    stamp_file = os.path.join(root_path, ".guardian_stamp")
    last_run = os.path.getmtime(stamp_file) if os.path.exists(stamp_file) else 0
    
    files = []
    for folder in targets:
        if not os.path.exists(folder): continue
        for root, _, fs in os.walk(folder):
            for f in fs:
                if f.endswith(('.cpp', '.h', '.hpp')):
                    fp = os.path.join(root, f)
                    # Skip the nlohmann folder specifically if it's inside include
                    if "nlohmann" in fp: continue 
                    if os.path.getmtime(fp) > last_run: files.append(fp)

    if not files:
        print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Guardian: No source changes.")
    else:
        print(f"Guardian: Formatting {len(files)} source files...", end='', flush=True)
        for fp in files:
            rel_p = os.path.relpath(fp, root_path)
            header = f"// Location: {rel_p}\n"
            try:
                with open(fp, 'r') as f: old_content = f.read()
                proc = subprocess.run(["clang-format", f"-style={style_str}"], 
                                      input=old_content, text=True, capture_output=True, timeout=2)
                new_content = proc.stdout
                lines = new_content.splitlines(keepends=True)
                if not lines or not lines[0].startswith("// Location:"): new_content = header + "\n" + new_content
                elif lines[0] != header:
                    lines[0] = header
                    new_content = "".join(lines)
                if old_content != new_content:
                    with open(fp, 'w') as f: f.write(new_content)
                    count += 1
                print(".", end='', flush=True)
            except subprocess.TimeoutExpired: print("T", end='', flush=True)
            except: print("x", end='', flush=True)
        print(f" Done. {count} updated.")

    # Always refresh the project map for the AI
    with open(os.path.join(root_path, "tree.txt"), "w") as f:
        subprocess.run(["tree", "-a", "-I", 'build|bin|.git', "-F", root_path], stdout=f)
    
    with open(stamp_file, 'w') as f: f.write(str(time.time()))

if __name__ == "__main__":
    run_guardian("/home/user/ACE")
