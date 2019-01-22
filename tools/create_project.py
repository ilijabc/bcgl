#!/usr/bin/python

import os
import sys

bcvars = [
    [ 'company', 'com.example' ],
    [ 'name', 'test_app' ],
    [ 'title', 'Test App' ],
    [ 'assets_dir', '../../assets' ],
    [ 'bcgl_dir', '../../bcgl' ],
    [ 'src_dir', '../../src' ],
]

def find_files(files, dirs = []):
    new_dirs = []
    for d in dirs:
        try:
            new_dirs += [ os.path.join(d, f) for f in os.listdir(d) ]
        except OSError:
            files.append(d)

    if new_dirs:
        find_files(files, new_dirs)
    else:
        return

def create_project(root):
    files = []
    find_files(files, dirs=[root])
    for f in files:
        out = f[len(root)+1:]
        print 'Generating', out
        with open(f, 'r') as file_in:
            data = file_in.read()
            for v in bcvars:
                data = data.replace('{{bcapp:' + v[0] + '}}', v[1])
            out_dir = os.path.dirname(out)
            try:
                os.makedirs(out_dir)
            except:
                pass
            with open(out, "w") as file_out:
                file_out.write(data)

##########
## MAIN ##
##########

README = """
usage: create_project.py [--compeny <company>] [--name <name>] [--title <title>]
                         [--assets_dir <assets_dir] [--bcgl_dir <bcgl_dir>] [--src_dir <src_dir>]
"""

for i in xrange(1, len(sys.argv), 2):
    for j in range(len(bcvars)):
        if ('--' + bcvars[j][0]) == sys.argv[i]:
            bcvars[j][1] = sys.argv[i + 1]
            break

if len(sys.argv) > 1:
    create_project(os.path.dirname(__file__) + '/project_template')
else:
    print README
