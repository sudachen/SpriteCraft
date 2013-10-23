#
# $Id$
#

import sys,re

RC_PROTO = """
1 VERSIONINFO
 FILEVERSION 3,3,0,%%BUILD%%
 PRODUCTVERSION 3,3,0,%%BUILD%%
 FILEFLAGSMASK 0x20fcL
#ifdef _DEBUG
 FILEFLAGS 0x1L
#else
 FILEFLAGS 0x0L
#endif
 FILEOS 0x4L
 FILETYPE 0x2L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "000004b0"
        BEGIN
            VALUE "Comments", "ScraftE for C++/Python.\\0"
            VALUE "CompanyName", "Teggo.FF\\0"
            VALUE "FileDescription", "http://www.spritecraft.com/\\0"
            VALUE "FileVersion", "3, 3, 0, %%BUILD%%\\0"
            VALUE "InternalName", "SpriteCraft Engine\\0"
            VALUE "LegalCopyright", "(C)2002-2007, Alexey Suda-Chen & Olga Marchevska, Teggo FF\\0"
            VALUE "LegalTrademarks", "\\0"
            VALUE "OriginalFilename", "scraft.dll\\0"
            VALUE "PrivateBuild", "\\0"
            VALUE "ProductName", "SpriteCraft Engine\\0"
            VALUE "ProductVersion", "3, 3, 0, %%BUILD%%\\0"
            VALUE "SpecialBuild", "\\0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x0, 1200
    END
END

1 TYPELIB MOVEABLE PURE   "ScraftE.tlb"
END
"""

b = int(open("rc.build","r").readline())+1
open("rc.build","w+").write(str(b))

l = RC_PROTO.split('\n')
o = open('ScraftE.rc',"w+t")
for i in l:
    i = re.sub( '%%BUILD%%', str(b), i)
    o.write("%s\n" % i)

o.close()
