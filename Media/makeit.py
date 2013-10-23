
import os, os.path, sys

os.chdir(os.path.abspath(os.path.dirname(__file__)))
sys.path.append('../Python/lib')
from make import *

set_build_config('')
process_command_line();

tempdir = '../~temp~/tffmedia-%s-%s' % ( get_build_config(), get_build_type() )
libname = 'tffmedia-%s-%s.lib' % ( get_build_config(), get_build_type() )

static = 'True'
if get_build_config() == 'export':
    global_flags_set['C_FLAGS'] += ['-D_MEDIA_DLL_EXPORT']
    static = False

add_global_flags_for_msvc(no_throw=True,static=static,no_debug=True)
global_flags_set['C_FLAGS'] += ['-nologo','-Z7','-I../Classes','-I%VISUS%/dx7sdk/include']

sources = [
    'lib.jpeg/jcomapi.c',
    'lib.jpeg/jdapimin.c',
    'lib.jpeg/jdapistd.c',
    'lib.jpeg/jdatadst.c',
    'lib.jpeg/jdatasrc.c',
    'lib.jpeg/jdcoefct.c',
    'lib.jpeg/jdcolor.c',
    'lib.jpeg/jddctmgr.c',
    'lib.jpeg/jdhuff.c',
    'lib.jpeg/jdinput.c',
    'lib.jpeg/jdmainct.c',
    'lib.jpeg/jdmarker.c',
    'lib.jpeg/jdmaster.c',
    'lib.jpeg/jdmerge.c',
    'lib.jpeg/jdphuff.c',
    'lib.jpeg/jdpostct.c',
    'lib.jpeg/jdsample.c',
    'lib.jpeg/jdtrans.c',
    'lib.jpeg/jerror.c',
    'lib.jpeg/jfdctflt.c',
    'lib.jpeg/jfdctfst.c',
    'lib.jpeg/jfdctint.c',
    'lib.jpeg/jidctflt.c',
    'lib.jpeg/jidctfst.c',
    'lib.jpeg/jidctint.c',
    'lib.jpeg/jidctred.c',
    'lib.jpeg/jmemmgr.c',
    'lib.jpeg/jmemnobs.c',
    'lib.jpeg/jquant1.c',
    'lib.jpeg/jquant2.c',
    'lib.jpeg/jutils.c',
    'lib.png/png.c',
    'lib.png/pngerror.c',
    'lib.png/pngget.c',
    'lib.png/pngmem.c',
    'lib.png/pngpread.c',
    'lib.png/pngread.c',
    'lib.png/pngrio.c',
    'lib.png/pngrtran.c',
    'lib.png/pngrutil.c',
    'lib.png/pngset.c',
    'lib.png/pngtrans.c',
    'lib.png/pngwio.c',
    'lib.png/pngwtran.c',
    'lib.z/zl_adler32.c',
    'lib.z/zl_compress.c',
    'lib.z/zl_crc32.c',
    'lib.z/zl_deflate.c',
    'lib.z/zl_infblock.c',
    'lib.z/zl_infcodes.c',
    'lib.z/zl_inffast.c',
    'lib.z/zl_inflate.c',
    'lib.z/zl_inftrees.c',
    'lib.z/zl_infutil.c',
    'lib.z/zl_trees.c',
    'lib.z/zl_uncompr.c',
    'lib.z/zutil.c',
    'lib.oggvorbis/ogg/bitwise.c',
    'lib.oggvorbis/ogg/framing.c',
    'lib.oggvorbis/vorbis/block.c',
    'lib.oggvorbis/vorbis/codebook.c',
    'lib.oggvorbis/vorbis/envelope.c',
    'lib.oggvorbis/vorbis/floor0.c',
    'lib.oggvorbis/vorbis/floor1.c',
    'lib.oggvorbis/vorbis/info.c',
    'lib.oggvorbis/vorbis/lookup.c',
    'lib.oggvorbis/vorbis/lpc.c',
    'lib.oggvorbis/vorbis/lsp.c',
    'lib.oggvorbis/vorbis/mapping0.c',
    'lib.oggvorbis/vorbis/mdct.c',
    'lib.oggvorbis/vorbis/psy.c',
    'lib.oggvorbis/vorbis/registry.c',
    'lib.oggvorbis/vorbis/res0.c',
    'lib.oggvorbis/vorbis/sharedbook.c',
    'lib.oggvorbis/vorbis/smallft.c',
    'lib.oggvorbis/vorbis/synthesis.c',
    'lib.oggvorbis/vorbis/vorbisfile.c',
    'lib.oggvorbis/vorbis/window.c',
    'lib.ddraw/creathcurso.cpp',
    'lib.ddraw/ddraw_inst.cpp',
    'lib.ddraw/tfft_bitmap.cpp',
    'lib.ddraw/tfft_dcpool.cpp',
    'lib.udis86/decode.c',
    'lib.udis86/input.c',
    'lib.udis86/mnemonics.c',
    'lib.udis86/opcmap.c',
    'lib.udis86/syn.c',
    'lib.udis86/syn-att.c',
    'lib.udis86/syn-intel.c',
    'lib.udis86/udis86.c',
    'colorXtoX.cpp',
    'picture.cpp',
    'picture_bmp.cpp',
    'picture_jpg.cpp',
    'picture_png.cpp',
    'picture_tga.cpp',
    'reduction12.cpp',
    'tffmedia.cpp',
    ]

objects = compile_files(sources,tempdir)
link_static(objects,tempdir,libname)
