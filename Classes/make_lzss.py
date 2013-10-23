import sys,os

if sys.platform == 'win32':
    cmd_S = 'cl -nologo -Z7 -LD -MD -Ox "Classes$.SRC/_lzss.c" "Classes.SRC/lz77ss.cpp" -I"%s\\include" "%s\\Libs\\python%s%s.lib" -Fe"_lzss.pyd" -link -implib:"_lzss.lib" -debug -incremental:no -pdb:"_lzss.pdb" -export:init_lzss' %\
                ( sys.prefix, sys.prefix, sys.version_info[0], sys.version_info[1])
else:
    cmd_S = "g++ -shared -O2 'Classes$.SRC/_lzss.c' 'Classes.SRC/lz77ss.cpp' -I /usr/include/python%s -o _lzss.so -lpython%s" %\
                ( sys.version[:3], sys.version[:3] )

print cmd_S
os.system(cmd_S)
