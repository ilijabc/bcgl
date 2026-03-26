import os
import sys
import json
import stat
import shutil

MANIFEST_NAME = "manifest.json"

# All instances of following strings:
#   __bcgl_project_name__
#   __bcgl_project_title__
#   __bcgl_project_company__
#   __bcgl_project_init__           TODO: add these files only on first sync
# will be replaced with manifest values

try:
    manifest = json.load(open(MANIFEST_NAME))
    manifest["init"] = ""
except FileNotFoundError as e:
    answer = input(f"'{MANIFEST_NAME}' not found, do you want to create it? (y/n): ").strip().lower()
    if answer == "y":
        print(f"'{MANIFEST_NAME}' created, please check it and run a script again.")
        with open(MANIFEST_NAME, "w") as f:
            f.write("""{
    "name": "bcgl_example",
    "title": "BCGL Example",
    "company": "com.example"
}
""")
    sys.exit(1)

SRC_DIR = os.path.dirname(os.path.abspath(__file__))
DST_DIR = "."

print("BCGL dir:", SRC_DIR)
print("Script name:", sys.argv[0])
print("Arguments:", sys.argv[1:])
print("Manifest:", manifest)

FORCE_INIT = False
if "--init" in sys.argv[1:]:
    FORCE_INIT = True

skip_dirs = { ".git", "__pycache__", "build", "node_modules" }
skip_files = { "bcsync.py", "bcsync" }
for root, dirs, files in os.walk(SRC_DIR):
    dirs[:] = [d for d in dirs if d not in skip_dirs]
    files[:] = [f for f in files if f not in skip_files]
    for name in files:
        src_path = os.path.join(root, name)

        # compute relative path
        rel_path = os.path.relpath(src_path, SRC_DIR)
        dst_path = os.path.join(DST_DIR, rel_path)

        # replace filename
        for old, new in manifest.items():
            dst_path = dst_path.replace(f"__bcgl_project_{old}__", new)

        if not FORCE_INIT and name.startswith("__bcgl_project_init__") and os.path.isfile(dst_path):
            continue

        # ensure destination directory exists
        os.makedirs(os.path.dirname(dst_path), exist_ok=True)

        # read
        with open(src_path, "r", encoding="utf-8") as f:
            try:
                text = f.read()
            except UnicodeDecodeError as e:
                # binary file
                print(src_path, "==>", dst_path)
                shutil.copy(src_path, dst_path)
                continue

            # replace content
            for old, new in manifest.items():
                text = text.replace(f"__bcgl_project_{old}__", new)

            # check
            try:
                with open(dst_path, "r", encoding="utf-8") as f:
                    dst_text = f.read()
                    if text == dst_text:
                        continue
            except:
                pass

            print(src_path, "-->", dst_path)

            # write
            with open(dst_path, "w", encoding="utf-8") as f:
                f.write(text)

            # copy file mode
            os.chmod(dst_path, os.stat(src_path).st_mode)
