import datetime
import errno
import getpass
import sys
from string import Template
import getopt
import os
from zlib import crc32

template_string_header_code = "#pragma once\n\n" \
                              "$disclaimer\n" \
                              "#include <ECS/Component.h>\n\n" \
                              "namespace lyra\n" \
                              "{\n" \
                              "\tstruct $class_name : Component\n" \
                              "\t{\n" \
                              "\t\tstatic const ComponentType mType = $component_hash;\n\n" \
                              "\t\tstruct DTO\n" \
                              "\t\t{\n\t\t\t//Serializable members\n\t\t};\n\n" \
                              "\t\t$class_name() = default;\n\n" \
                              "\t\tvoid OnDTOPreLoad(DTO&, AssetManager&) {};\n" \
                              "\t\tvoid OnDTOPostLoad(DTO&, AssetManager&) {};\n" \
                              "\t\tvoid OnDTOPreSave(DTO&, AssetManager&) {};\n" \
                              "\t\tvoid OnDTOPostSave(DTO&, AssetManager&) {};\n\n" \
                              "\t};\n" \
                              "}"


def get_hash_from_string(src_string):
    return crc32(src_string.encode())


def generate_component_file(component_name, outdir):
    now = datetime.datetime.now()
    disclaimer = "// Created on {0} by {1}".format(now.date(), getpass.getuser())
    component_hash = '0x{0:08x}'.format(get_hash_from_string(component_name))
    class_header = Template(template_string_header_code).substitute(
        class_name=component_name,
        component_hash=component_hash,
        disclaimer=disclaimer
    )

    filename = os.path.join(outdir, component_name + ".h")
    if not os.path.exists(os.path.dirname(filename)):
        try:
            os.makedirs(os.path.dirname(filename))
        except OSError as exc:  # Guard against race condition
            if exc.errno != errno.EEXIST:
                raise

    with open(filename, "w") as f:
        f.write(class_header)


def main(argv):
    component_name = ''
    outdir = ''
    show_help = False
    premake = False

    banner = 'create_component.py -c componentname -o outdir[-m -h]'

    try:
        opts, args = getopt.getopt(argv, "hc:o:m", ["outputfile=", "directory="])
    except getopt.GetoptError:
        print(banner)
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            show_help = True
        elif opt in ("-c", "--component"):
            component_name = arg
        elif opt in ("-o", "--directory"):
            outdir = arg
        elif opt == 'm':
            premake = True

    if component_name == '' or outdir == '' or show_help:
        print(banner)
        print("""
            -h:                 Displays this help
            -c componentname:   (REQUIRED) Name of the component
            -o outdir:          (REQUIRED) Directory to save the new component to
            -m:                 Triggers project generation
        """)
        sys.exit()

    if component_name != '':
        generate_component_file(component_name, "packages/lyra/include/{dir}".format(dir=outdir))

    if premake:
        from subprocess import Popen
        p = Popen("premake_2019.bat")
        stdout, stderr = p.communicate()

if __name__ == "__main__":
    main(sys.argv[1:])




