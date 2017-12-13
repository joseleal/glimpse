# Run like:
# $ blender.exe -b /path/to/glimpse-training.blend -P generator-cmd.py -- -h

import os
import sys
import argparse
import bpy
import addon_utils

if "--" in sys.argv:
    argv = sys.argv
    argv = argv[argv.index("--") + 1:]
else:
    argv = []

parser = argparse.ArgumentParser(prog="glimpse-generator", add_help=False)

parser.add_argument('--help-glimpse', help='Show this help message and exit', action='help')
parser.add_argument('--info', help='Load the mocap index and print summary information', action='store_true')
parser.add_argument('--preload', help='Preload mocap files as actions before rendering', action='store_true')
parser.add_argument('--purge', help='Purge mocap actions', action='store_true')
parser.add_argument('--start', type=int, default=0, help='Index of first MoCap to render')
parser.add_argument('--end', default=0, type=int, help='Index of last MoCap to render')
parser.add_argument('--dest', default=os.getcwd(), help='Directory to write files too')
parser.add_argument('--name', default=os.getcwd(), help='Unique name for this render run')
parser.add_argument('mocaps', help='Directory with motion capture files')
args = parser.parse_args(argv)

glimpse_addon_name = 'Glimpse Training Data Generator'

addon_dependencies = [
    glimpse_addon_name,
]

dep_error = ""
for dep in addon_dependencies:
    addon_status = addon_utils.check(dep)
    if addon_status[0] != True or addon_status[1] != True:
        dep_error += "Addon '" + dep + "' has not been enabled through Blender's User Preferences\n"

if dep_error != "":
    print("\n")
    print("Error:\n")
    print(dep_error)
    print("\n")
    print("Please find the instructions for setting up the required addons in blender/README.md")
    print("> https://github.com/glimpse-project/glimpse/blob/master/blender/README.md")
    print("\n")
    sys.exit(1)

if not os.path.isdir(args.mocaps):
    print("Non-existent mocaps directory %s" % args.mocaps)
    bpy.ops.wm.quit_blender()
    sys.exit(1)
bpy.context.scene.GlimpseBvhRoot = args.mocaps

bpy.ops.glimpse.open_bvh_index()

if args.start:
    bpy.context.scene.GlimpseBvhGenFrom = args.start

if args.end:
    bpy.context.scene.GlimpseBvhGenTo = args.end

if args.info:
    bpy.ops.glimpse.generator_info()
    bpy.ops.wm.quit_blender()

if args.preload:
    bpy.ops.glimpse.generator_preload()
    print("Saving to %s" % bpy.context.blend_data.filepath)
    bpy.ops.wm.save_as_mainfile(filepath=bpy.context.blend_data.filepath)
    bpy.ops.wm.quit_blender()
    
if args.purge:
    bpy.ops.glimpse.purge_mocap_actions()
    basename = bpy.path.basename(bpy.context.blend_data.filepath)
    bpy.ops.wm.save_as_mainfile(filepath=bpy.path.abspath('//%s-purged.blend' % basename[:-6]))
    bpy.ops.wm.quit_blender()

if args.dest == "":
    print("--dest argument required in this case to find files to preload")
    bpy.ops.wm.quit_blender()
if not os.path.isdir(args.dest):
    print("Non-existent dest directory %s" % args.dest)
    bpy.ops.wm.quit_blender()
bpy.context.scene.GlimpseDataRoot = args.dest

print("DataRoot: " + args.dest)

if args.name == "":
    print("--name argument required in this case to find files to preload")
    bpy.ops.wm.quit_blender()
bpy.context.scene.GlimpseGenDir = args.name


import cProfile
cProfile.run("bpy.ops.glimpse.generate_data()", "glimpse-" + args.name + ".prof")
 
import pstats
p = pstats.Stats("glimpse-" + args.name + ".prof")
p.sort_stats("cumulative").print_stats(20)


print(str(bpy.ops.glimpse.generate_data))
print(bpy.context.scene.GlimpseBvhRoot)
