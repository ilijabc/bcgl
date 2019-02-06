#!/usr/bin/python

import getopt
import os
import sys

bcvars = {
    'company': 'com.example',
    'name': 'test_app',
    'title': 'Test App',
    'assets_dir': 'assets',
    'bcgl_dir': 'bcgl',
    'src_dir': 'src',
}

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

def generate_file(f, out):
    print 'Generating', out
    with open(f, 'r') as file_in:
        out_dir = os.path.dirname(out)
        data = file_in.read()
        for k in bcvars:
            v = bcvars[k]
            if k.endswith('_dir'):
                v = os.path.relpath(bcvars[k], out_dir)
            data = data.replace('{{bcapp:' + k + '}}', v)
        try:
            os.makedirs(out_dir)
        except:
            pass
        with open(out, "w") as file_out:
            file_out.write(data)
            if out.endswith('.sh'): os.chmod(out, 0775)

def create_project(root):
    # copy files
    files = []
    find_files(files, dirs=[root])
    for f in files:
        out = f[len(root)+1:]
        if out[0] == '_':
            if out.startswith('_name_'):
                generate_file(f, out.replace('_name_', bcvars['name']))
            continue
        generate_file(f, out)
    # create dirs
    try:
        os.makedirs(bcvars['assets_dir'])
        os.rename('src/', bcvars['src_dir'])
    except Exception as e:
        raise e

##########
## MAIN ##
##########

README = """Usage: create_project.py [OPTIONS]

    -C, --compeny=COMPANY           default: com.example
    -N, --name=NAME                 default: test_app
    -T, --title=TITLE               default: Test App
        --assets_dir=ASSETS_DIR     default: assets
        --bcgl_dir=BCGL_DIR         default: bcgl
        --src_dir=SRC_DIR           default: src
"""

if len(sys.argv) <= 1:
    print README
    sys.exit()

options, rest = getopt.getopt(sys.argv[1:], 'C:N:T:', [
    'company=',
    'name=',
    'title=',
    'assets_dir=',
    'bcgl_dir=',
    'src_dir=',
    'init'
    ])

for opt, arg in options:
    if opt in ('-C', '--company'):
        bcvars['company'] = arg
    elif opt in ('-N', '--name'):
        bcvars['name'] = arg
    elif opt in ('-T', '--title'):
        bcvars['title'] = arg
    elif opt == '--assets_dir':
        bcvars['assets_dir'] = arg
    elif opt == '--bcgl_dir':
        bcvars['bcgl_dir'] = arg
    elif opt == '--src_dir':
        bcvars['src_dir'] = arg

create_project(os.path.dirname(__file__) + '/project_template')
