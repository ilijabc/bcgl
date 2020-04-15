import getopt
import os
import sys
import json

bcvars = {
    'name': None,
    'title': None,
    'company': 'com.bcgl',
    'bcgl_dir': os.path.relpath(os.path.dirname(__file__) + "/..", os.getcwd()),
    'assets_dir': 'assets',
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
                v = os.path.relpath(bcvars[k], out_dir).replace('\\', '/')
            data = data.replace('{{bcapp:' + k + '}}', v)
        try:
            os.makedirs(out_dir)
        except:
            pass
        with open(out, "w") as file_out:
            file_out.write(data)
            if out.endswith('.sh') or out.endswith('.bat'): os.chmod(out, 0775)

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
        os.rename('assets/', bcvars['assets_dir'])
        os.rename('src/', bcvars['src_dir'])
    except Exception as e:
        raise e

def print_bcvars():
    print '======================'
    for k in bcvars:
        v = bcvars[k]
        print '  ', k, '=', v
    print '======================'

##########
## MAIN ##
##########

print

README = """Usage: create_project.py [OPTIONS]

    -N, --name=NAME                 required
    -T, --title=TITLE               default: same as name
    -C, --company=COMPANY           default: com.bcgl
        --bcgl_dir=BCGL_DIR         default: relative path to bcgl
        --assets_dir=ASSETS_DIR     default: assets
        --src_dir=SRC_DIR           default: src
        --update                    update existing project
"""

MANIFEST_FILE = 'bcmanifest.json'

existing_manifest = False
do_update = False

# read manifest
if os.path.isfile(MANIFEST_FILE):
    with open(MANIFEST_FILE) as json_file:
        print 'Existing manifest'
        bcvars = json.load(json_file)
        print_bcvars()
        print
        existing_manifest = True

# get arguments
options, rest = getopt.getopt(sys.argv[1:], 'C:N:T:', [
    'name=',
    'title=',
    'company=',
    'bcgl_dir=',
    'assets_dir=',
    'src_dir=',
    'help',
    'update'
    ])

for opt, arg in options:
    if opt in ('-N', '--name'):
        bcvars['name'] = arg
    elif opt in ('-T', '--title'):
        bcvars['title'] = arg
    elif opt in ('-C', '--company'):
        bcvars['company'] = arg
    elif opt == '--bcgl_dir':
        bcvars['bcgl_dir'] = arg
    elif opt == '--assets_dir':
        bcvars['assets_dir'] = arg
    elif opt == '--src_dir':
        bcvars['src_dir'] = arg
    elif opt == '--help':
        print README
        sys.exit()
    elif opt == '--update':
        do_update = True

# check manifest override
if existing_manifest == True:
    if do_update == False:
        print "To override existing project use '--update' option!"
        sys.exit()
    else:
        print 'Update manifest'

# check existing name
if bcvars['name'] is None:
    print "Missing option, '--name' must be provided!"
    print
    print README
    sys.exit()

# check existing title
if bcvars['title'] is None:
    bcvars['title'] = bcvars['name']

# print config
print_bcvars()
print

create_project(os.path.dirname(__file__) + '/project_template')

print
print 'Project ready.'
