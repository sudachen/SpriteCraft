
import pythoncom
from pythoncom import *

import sys

tlb = LoadTypeLib("scraft.dll")
ti_cnt = tlb.GetTypeInfoCount()
ti = None

sys.stdout = open("doctmpl.txt","w+")

def MaskName(a) :
    if a == 'ISprite' :
        return 'Sprite'
    if a == 'IEngine' :
        return '_Engine'
    if a == 'IParticles' :
        return 'Particles'
    if a == 'ITileMap' :
        return 'TileMap'
    if a == 'IRandomizer' :
        return 'Randomizer'
    if a == 'ISpritesList' :
        return 'SpritesList'
    if a == 'IXdata' :
        return 'Xdata'
    if a == 'ISpritesEnumerator' :
        return '_SpritesEnumerator'
    if a == 'ISpritePrimitive' :
        return '_Primitive'
    if a == 'ILwsArray' :
        return '_LwsArray'
    if a == 'IHierarchy' :
        return '_Hierarchy'
    if a == 'IConsole' :
        return '_Console'
    if a == 'IXdataEnumerator' :
        return '_XdataEnumerator'
    if a == 'IXdataAttributes' :
        return '_XdataAttributes'
    if a == 'IParticlesEnumerator' :
        return '_ParticlesEnumerator'
    if a == 'IExecutor' :
        return '_Executor'
    if a == 'IQue' :
        return '_Que'
    if a == 'ITextView' :
        return '_TextView'
    if a == 'IRGBFilter' :
        return '_RGBFilter'
    return a


def SelectArgumentType(a) :
    if a == 3 :
        return "#integer"
    if a == 22 :
        return "#integer"
    elif a == 4 :
        return "#single"
    elif a == 11 :
        return "#bool"
    elif a == 13 :
        return "#object"
    elif a == 9 :
        return "#object"
    elif a == 8 :
        return "#wstring"
    elif a == 24 :
        return "#void"
    elif isinstance(a,tuple) :
        if a[0] == 26 :
            return SelectArgumentType(a[1])
        elif a[0] == 29 :
            rti = ti.GetRefTypeInfo(a[1])
            rtlb, rid = rti.GetContainingTypeLib()
            (rname,_,_,_) = rtlb.GetDocumentation(rid)
            return MaskName(rname)

    else :
        return "#unknown{"+repr(a)+"}"

def SelectArgumentAccs(a) :
    if (a & (1|2)) == (1|2) :
        return "inout"
    elif (a & (1|2)) == 1 or (a & (1|2)) == 0 :
        return "in"
    elif (a & (1|2)) == 2 :
        return "out"
    else :
        return "#unknown("+repr(a)+")"

for i in range(ti_cnt):
    ti = tlb.GetTypeInfo(i)
    (name,_,_,_) = tlb.GetDocumentation(i)
    ta = ti.GetTypeAttr()
    if ta.wTypeFlags & (TYPEFLAG_FHIDDEN|TYPEFLAG_FRESTRICTED) :
        continue
    if ta.typekind == TKIND_ENUM :
        print "{{!-- " + name + " --}}"
        print "{{enum}}" + name
        for j in range(ta.cVars) :
            vdsc = ti.GetVarDesc(j)
            (name,_,_,_) = ti.GetDocumentation(vdsc.memid)
            print "{{constant,value("+str(vdsc.value)+")}}"+name
        print ""
    if ta.typekind == TKIND_DISPATCH : # or ta.typekind == TKIND_INTERFACE
        memb = {}
        meth = {}
        ifs_name = MaskName(name)
        print "{{!-- "+ifs_name+" --}}"
        print "{{type}}" + ifs_name
        print ""
        for j in range(ta.cFuncs) :
            fdsc = ti.GetFuncDesc(j)
            (name,_,_,_) = ti.GetDocumentation(fdsc.memid)
            if name.startswith('__comfunc_'):
                name = name[10:]
            elif name.startswith('__python_'):
                continue
            names = ti.GetNames(fdsc.memid)
            names_i = 0
            if fdsc.wFuncFlags & (FUNCFLAG_FHIDDEN|FUNCFLAG_FRESTRICTED)  :
                continue
            if fdsc.invkind == INVOKE_FUNC :
                meth[name] = {'name':name}
                meth[name]['args'] = []
                meth[name]['rtype'] = SelectArgumentType(fdsc.rettype[0])
                optno = -1;
                for a in fdsc.args :
                    a_t = SelectArgumentType(a[0])
                    accs = SelectArgumentAccs(a[1])
                    if a[1] & 0x20 and optno < 0:
                        optno = names_i
                    s = "{{param,"+accs+",type("+a_t+")}}"+names[names_i+1]
                    names_i += 1
                    meth[name]['args'].append(s)
                meth[name]['optno'] = optno
            elif fdsc.invkind == INVOKE_PROPERTYGET :
                if not memb.has_key(name) :
                    memb[name] = {'name':name}
                memb[name]['rtype'] = SelectArgumentType(fdsc.rettype[0])
                memb[name]['getter'] = 1
            elif fdsc.invkind == INVOKE_PROPERTYPUT :
                if not memb.has_key(name) :
                    memb[name] = {'name':name}
                memb[name]['atype'] = SelectArgumentType(fdsc.args[0][0])
                memb[name]['setter'] = 1
        for j in sorted(meth) :
            print "{{!--  "+ifs_name+"."+j+" --}}"
            if meth[j]['optno'] < 0  :
                print "{{method,type("+meth[j]['rtype']+")}}"+j
            else :
                print "{{method,type("+meth[j]['rtype']+"),opt("+repr(meth[j]['optno'])+")}}"+j
            for k in meth[j]['args'] :
                print k
            print ""
        print "{{!-- "+ifs_name+" attributes --}}"
        for j in sorted(memb) :
            setter = getter = False
            if memb[j].has_key('setter') :
                setter = True
            if memb[j].has_key('getter') :
                getter = True
            if setter and getter:
                mode = 'readwrite'
            elif getter :
                mode = 'readonly'
            elif setter :
                mode = 'writeonly'
            if getter :
                mode_t = memb[j]['rtype']
            else :
                mode_t = memb[j]['atype']
            print "{{attribute,"+mode+",type("+mode_t+")}}"+j
        print ""
