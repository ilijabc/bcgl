import os
import sys
import json
import stat

BCGL_VERSION = "1.0.1"

FORCE_INIT = "-f" in sys.argv[1:]
VERBOSE = "-v" in sys.argv[1:]
QUIET = "-q" in sys.argv[1:]

SRC_DIR = os.path.dirname(os.path.abspath(__file__))

DST_DIR = "."

MANIFEST_NAME = "manifest.json"

DEFAULT_MANIFEST = {
    "app": {
        "name": "bcgl_example",
        "title": "BCGL Example",
        "company": "com.example",
        "version": "1.0.0"
    },
    "bcgl": {
        "version": BCGL_VERSION,
        "ignore": [
            ".gitignore",
            "src/CMakeLists.txt"
        ]
    }
}

# All instances of following strings:
#   __bcgl_project_name__
#   __bcgl_project_title__
#   __bcgl_project_company__
#   __bcgl_project_version__
# will be replaced with manifest values

try:
    manifest = json.load(open(MANIFEST_NAME))
except FileNotFoundError as e:
    answer = input(f"'{MANIFEST_NAME}' not found, do you want to create it? (y/n): ").strip().lower()
    if answer == "y":
        print(f"'{MANIFEST_NAME}' created, please check it and run a script again.")
        print("For the first time, run the script with: bcsync -f")
        with open(MANIFEST_NAME, "w") as f:
            f.write(json.dumps(DEFAULT_MANIFEST, indent=4) + os.linesep)
    sys.exit(1)

if VERBOSE:
    print("BCGL dir:", SRC_DIR)
    print("Script name:", sys.argv[0])
    print("Arguments:", sys.argv[1:])
    print("Manifest:", manifest)

skip_dirs = { ".git", "__pycache__", "build", "node_modules" }

skip_files = { "bcsync.py", "bcsync", "README.md" }

ignore_files = {}
if "ignore" in manifest["bcgl"]:
    ignore_files = [os.path.abspath(f) for f in manifest["bcgl"]["ignore"]]

for root, dirs, files in os.walk(SRC_DIR):
    dirs[:] = [d for d in dirs if d not in skip_dirs]
    files[:] = [f for f in files if f not in skip_files]
    for name in files:
        src_path = os.path.join(root, name)

        # compute relative path
        rel_path = os.path.relpath(src_path, SRC_DIR)
        dst_path = os.path.join(DST_DIR, rel_path)

        # replace filename
        for old, new in manifest["app"].items():
            dst_path = dst_path.replace(f"__bcgl_project_{old}__", new)

        # read content
        content = None
        is_binary = False
        with open(src_path, "r", encoding="utf-8") as f:
            try:
                content = f.read()
            except UnicodeDecodeError as e:
                with open(src_path, "rb") as b:
                    content = b.read()
                    is_binary = True

            # replace content
            if not is_binary:
                for old, new in manifest["app"].items():
                    content = content.replace(f"__bcgl_project_{old}__", new)

            # check
            try:
                if is_binary:
                    with open(dst_path, "rb") as b:
                        if content == b.read():
                            continue
                else:
                    with open(dst_path, "r", encoding="utf-8") as f:
                        if content == f.read():
                            continue
            except:
                pass

            # ignore list
            if not FORCE_INIT and os.path.abspath(dst_path) in ignore_files:
                if VERBOSE:
                    print("IGNORE:", dst_path)
                continue



            if QUIET:
                print("IGNORE:", src_path, "-->", dst_path, "(Binary)" if is_binary else "")
            else:
                print(src_path, "-->", dst_path, "(Binary)" if is_binary else "")
                # ensure destination directory exists
                os.makedirs(os.path.dirname(dst_path), exist_ok=True)
                # write
                if is_binary:
                    with open(dst_path, "wb") as b:
                        b.write(content)
                else:
                    with open(dst_path, "w", encoding="utf-8") as f:
                        f.write(content)
                # copy file mode
                os.chmod(dst_path, os.stat(src_path).st_mode)
