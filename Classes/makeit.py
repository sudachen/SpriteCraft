
import os, os.path, sys

os.chdir(os.path.abspath(os.path.dirname(__file__)))
sys.path.append('../Python/lib')
from make import *

set_build_config('')
process_command_line();

tempdir = '../~temp~/tffcls-%s-%s' % ( get_build_config(), get_build_type() )
libname = 'tffcls-%s-%s.lib' % ( get_build_config(), get_build_type() )

static = 'True'
if get_build_config() == 'export':
    global_flags_set['C_FLAGS'] += ['-D_TEGGODYNAMIC_EXPORT']
    static = False

add_global_flags_for_msvc(no_throw=True,static=static,no_debug=True)
global_flags_set['C_FLAGS'] += ['-nologo','-Z7','-I%VISUS%/dx7sdk/include']

sources = [
    ['Classes.SRC/_adler32.cpp',[],['Classes.SRC/_adler32.h']],
    ['Classes.SRC/_codecop.cpp',[],['Classes.SRC/_codecop.inl']],
    ['Classes.SRC/_crc32.cpp',[],['Classes.SRC/_crc32.h']],
    ['Classes.SRC/_specific.cpp',[],['Classes.SRC/_specific.inl']],
    ['Classes.SRC/collection.cpp',[],['Classes.SRC/collection.inl']],
    ['Classes.SRC/com_dynamic.cpp',[],['Classes.SRC/com_dynamic.inl','Classes.SRC/com_generic.h']],
    ['Classes.SRC/com_register.cpp',[],['Classes.SRC/com_register.inl','Classes.SRC/com_generic.h']],
    ['Classes.SRC/format.cpp',[],['Classes.SRC/format.h']],
    ['Classes.SRC/genericio.cpp',[],['Classes.SRC/genericio.inl']],
    ['Classes.SRC/hinstance.cpp',[],['Classes.SRC/hinstance.h']],
    ['Classes.SRC/keylist.cpp',[],['Classes.SRC/keylist.inl']],
    ['Classes.SRC/logger.cpp',[],['Classes.SRC/logger.inl']],
    ['Classes.SRC/lz77ss.cpp',[],['Classes.SRC/lz77ss.inl']],
    ['Classes.SRC/lz+.cpp',[],['Classes.SRC/lz+.inl']],
    ['Classes.SRC/lz+decoder.cpp',[],['Classes.SRC/lz+decoder.inl']],
    ['Classes.SRC/messages.cpp',[],['Classes.SRC/messages.inl']],
    ['Classes.SRC/newdes.cpp',[],['Classes.SRC/newdes.inl']],
    ['Classes.SRC/saxparser.cpp',[],[
        'Classes.SRC/saxparser.inl','Classes.SRC/saxparser.h','Classes.SRC/streams_u2.inl']],
    ['Classes.SRC/defparser.cpp',[],[
        'Classes.SRC/defparser.inl','Classes.SRC/saxparser.h','Classes.SRC/defparser.h','Classes.SRC/xdom.h']],
    ['Classes.SRC/streams.cpp',[],['Classes.SRC/streams.inl']],
    ['Classes.SRC/string.cpp',[],['Classes.SRC/string.inl','Classes.SRC/string.h']],
    ['Classes.SRC/symboltable.cpp',[],['Classes.SRC/symboltable.inl']],
    ['Classes.SRC/sysutil.cpp',[],['Classes.SRC/sysutil.h']],
    ['Classes.SRC/xdom.cpp',[],['Classes.SRC/xdom.inl','Classes.SRC/xdom.h','Classes.SRC/saxparser.h']],
    'Classes.SRC/newdes_accel.S',
    ]

objects = compile_files(sources,tempdir)
link_static(objects,tempdir,libname)
