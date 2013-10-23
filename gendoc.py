# -*- coding: windows-1251 -*-
import sys,os

if len(sys.argv) > 1:
    lang = sys.argv[1]
else:
    lang = "ru"


if lang == 'ru':
    strings = \
        {
            'undocumented': 'недокументрован',
            'Constants': 'Константы',
            'Start Page': 'Стартовая страница',
        }
else:
    strings = None

def find_string(s):
    if strings:
        if strings.has_key(s):
            return strings[s]
    return s

prefix = "scraft."+lang+".doc/";
docstart = "scraft."+lang+".htm";

f = open("doctmpl.txt")

def MakeTypeHeader(opt,ident) :
    s = "<h1>" + ident + "</h1>"
    return s

g_types_map = {}
g_cur_type = None
g_cur_meth = None
g_cur_doc  = None
g_cur_args = None
g_cur_enum = None

while True :
    s = f.readline()
    if len(s) <= 0 : break
    s = s.strip()
    if s.startswith("{{!--") : continue # skip comment
    elif s.startswith("#") :
        if g_cur_doc != None :
            s = s[1:]
            if len(g_cur_doc) and len(s):
                g_cur_doc[len(g_cur_doc)-1] += s
            elif len(s) or ( len(g_cur_doc) and len(g_cur_doc[len(g_cur_doc)-1]) ):
                g_cur_doc.append(s)

    elif s.startswith("{{") : #section
        x = s.rfind("}}")
        sec_opt = s[2:x].split(',')
        sec_ident = s[x+2:]
        #print "<<" + repr(sec_opt) + ">>" + sec_ident + "!!"
        type = "#!"
        value = None
        optno = None
        skipped = False
        accs = "rw"
        for m in sec_opt :
            if m.startswith("type(") :
                type = m[5:m.rfind(')')]
            if m.startswith("value(") :
                value = m[6:m.rfind(')')]
            if m.startswith("opt(") :
                optno = m[4:m.rfind(')')]
            if m == 'readonly' or m == 'readwrite' or m == 'writeonly' :
                accs = m
        if type == "#void" :
            type = "None"
        elif type == "#integer" :
            type = "Integer"
        elif type == "#single" :
            type = "Float"
        elif type == "#object" :
            type = "Any"
        elif type == "#wstring" :
            type = "Unicode"
        elif type == "#bool" :
            type = "Boolean"
        elif type.startswith("#tuple") :
            tuple = type.split(':')
            type = "( "
            for t in range(1,len(tuple)) :
                if ( t > 1 ) : type += ", "
                type+=tuple[t]
            type += " )"
        if sec_opt[0].startswith("-") :
            skipped = True
            sec_opt[0] = sec_opt[0][1:]
        if sec_opt[0] == 'type' :
            g_cur_doc = []
            g_types_map[sec_ident] = {}
            g_types_map[sec_ident]['opt'] = sec_opt
            g_types_map[sec_ident]['methods'] = {}
            g_types_map[sec_ident]['attributes'] = []
            g_types_map[sec_ident]['doc'] = g_cur_doc
            g_types_map[sec_ident]['skipped'] = skipped
            g_cur_type = sec_ident
            g_cur_attr = None
            g_cur_meth = None
        elif sec_opt[0] == 'method' :
            g_cur_doc = []
            g_cur_args = []
            g_types_map[g_cur_type]['methods'][sec_ident] = (type,g_cur_args,g_cur_doc,optno,skipped)
            g_cur_meth = sec_ident
            g_cur_attr = None
        elif sec_opt[0] == 'enum' :
            g_cur_doc = []
            g_cur_enum = []
            g_cur_args = None
            g_cur_attr = None
            g_cur_meth = None
            g_types_map[sec_ident] = { 'constants' : (g_cur_enum,g_cur_doc), 'skipped' : skipped }
        elif sec_opt[0] == 'constant' :
            opt = (sec_ident,value)
            g_cur_enum.append(opt)
        elif sec_opt[0] == 'param' :
            g_cur_doc = []
            inout = 'in'
            opt = (sec_ident,type,inout,g_cur_doc)
            g_cur_args.append(opt)
        elif sec_opt[0] == 'attribute' :
            g_cur_doc = []
            opt = (sec_ident, accs, type, g_cur_doc)
            g_types_map[g_cur_type]['attributes'].append(opt)


def FormatTypeRef(t) :
    if g_types_map.has_key(t) :
        if g_types_map[t].has_key('constants') :
            return "<a class='type' href='scraft.constants.htm#"+t+"' target='right'><i>"+t+"</i></a>"
        else :
            return "<a class='type' href='scraft."+t+".fr.htm' target='right'><i>"+t+"</i></a>"
    else :
        return "<i class='type' >"+t+"</i>"

def FormatAttribute(opt) :
    accs = "accs-red"
    #accst = "ro"
    if opt[1] == 'readwrite' :
        accs = 'accs-green'
        #accst = 'rw'
    elif opt[1] == 'readonly' :
        accs = 'accs-red'
        #accst = 'ro'
    elif opt[1] == 'writeonly' :
        accs = 'accs-yellow'
        #accst = 'wo'

    s = "<td>" + FormatTypeRef(opt[2]) + "</td><td><a name='" +opt[0]+ "'>"+ opt[0] + "</a></td><td><i class='"+accs+"'>" + opt[1] + "</td>"
    return s

def FormatConstant(opt) :
    f = "0x%08X" % int(opt[1])
    s = "<td>" + opt[0] + "</td><td align=right>" + f + "</td><td align=right><i>" + opt[1] + "</i></td>"
    return s

def FormatMethod(ident,opt) :
    rettype = opt[0]
    args = opt[1]
    doc = opt[2]
    optno = opt[3]
    a = "( "
    if len(args) :
        for i in range(len(args)) :
            if optno != None and i == int(optno) :
                a += "["
            if ( i > 0 ) : a += ", "
            aname = args[i][0]
            if len(aname) == 0 : aname = "unnamed"
            a += FormatTypeRef(args[i][1]) + " " + aname + ""
        if optno != None :
            a += "]"
        a += " )"
    else :
        a += ")"
    return FormatTypeRef(rettype) + " <a name='" + ident +"'>"+ ident + "</a>" + a

def GenHeader() :
    s = ""
    s += "<head>\n"
    if lang == 'ru':
        s += "<meta charset='windows-1251'>\n"
    s += "<style>\n"
    s += "table.attr-tbl td { valign: top; border-bottom: dashed #ddd 1px; border-left: dashed #ddd 1px; padding: 5 5 5 5; margin: 0 0 0 0; }\n"
    s += "table.enum-tbl td { valign: top; border-bottom: dashed #ddd 1px; border-left: dashed #ddd 1px; padding: 5 5 5 5; margin: 0 0 0 0; }\n"
    s += "table.idx-tbl td { valign: center; border-bottom: dashed #ddd 1px; border-left: solid #ddd 1px; }\n";
    s += "td.enum-tbl-h { border-bottom: dashed #000 1px; border-left: dashed #000 1px; padding: 5 5 5 5; margin: 0 0 0 0;  color: #008080; font-size: 110%; background-color: #eeffee }\n"
    s += "td.attr-tbl-h { font-size: 1px; padding: 0 0 0 0; border-bottom: dashed #000 1px; }\n"
    #s += "td.attr-tbl-h { font-size: 1px; border-left: solid #000 1px; border-bottom: dashed #000 1px; padding: 5 5 5 5; margin: 0 0 0 0;  color: #008080; background-color: #eeffee }\n"
    s += ".accs-green  { color: #00aa00; }\n"
    s += ".accs-red    { color: #aa0000; }\n"
    s += ".accs-yellow { color: #aaaa00; }\n"
    s += "a.idx-tbl { text-decoration: none; color: #000 }\n"
    s += "a.idx-tbl:hover { text-decoration: underline; color: #300 }\n"
    s += ".type { text-decoration: none; color: #555 }\n"
    s += "a.type { text-decoration: none; color: #55a }\n"
    s += "a.type:hover { text-decoration: underline; color: #300 }\n"
    s += ".doc-txt { color: #553355; font-style: italic; margin-left: 30px; }\n"
    s += "</style>\n"
    s += "</head>\n"
    return s

if not os.path.exists(prefix) :
    os.mkdir(prefix)

f = open(prefix+"scraft.constants.htm","w+")
f.write(GenHeader())

f.write( "<table cellspacing=0 cellpadding=3 border=0 class='enum-tbl'>\n" )

for i in sorted(g_types_map) :
    if g_types_map[i].has_key('constants') :
        f.write( u"<tr><td colspan=3 valign=center class='enum-tbl-h'><a name='"+i+"'><b>"+ i +"</b></a></td></tr>"  )
        enums = g_types_map[i]['constants'][0]
        for j in enums :
           f.write( "<tr>" + FormatConstant(j) + "</tr>\n" )

f.write( "</table>\n" )
f.close()

for i in sorted(g_types_map) :
    if not g_types_map[i].has_key('constants') :
        if g_types_map[i]['skipped'] :
            continue
        f = open(prefix+"scraft."+i+".htm","w+")
        f_ndx = open(prefix+"scraft."+i+".ndx.htm","w+")
        f_ndx.write(GenHeader())
        f_ndx.write("<table cellspacing=0 cellpadding=0 border=0 class='ndx-tbl'>\n")
        f.write(GenHeader())
        f.write("<h2>scraft." + i +"</h2>\n")
        attr = g_types_map[i]['attributes']
        if len(attr) > 0 :
            f.write("<table cellspacing=0 cellpadding=3 border=0 class='attr-tbl'>\n")
            f.write("<tr><td colspan=3 class='attr-tbl-h'>&nbsp;</td></tr>\n")
            for j in sorted(attr) :
                if g_types_map.has_key(j[2]) :
                    if g_types_map[j[2]]['skipped'] :
                        continue
                f.write("<tr>" + FormatAttribute(j) + "</tr>\n")
                accs = "accs-red"
                if j[1] == 'readwrite' :
                    accs = '#dfd'
                elif j[1] == 'readonly' :
                    accs = '#fdd'
                elif j[1] == 'writeonly' :
                    accs = '#dff'
                f_ndx.write("<tr><td><code><b style='background-color: "+accs+"'>p</b></code>&nbsp;<a class='idx-tbl' href='scraft."+i+".htm#"+j[0]+"' target='left1'>" + j[0] + "</a></td></tr>\n")
            f.write( "</table>\n" )
            f.write( "<p></p>\n")
        meth = g_types_map[i]['methods']
        for j in sorted(meth) :
            if meth[j][4] :
                continue
            f.write( "<table cellpadding=0 cellspacing=0><tr><td style='border: solid #eee 1px' >" + FormatMethod(j,meth[j]) + "</td></tr></table>\n" )
            if len( meth[j][2] ) :
                for l in meth[j][2] :
                    f.write( "<p class='doc-txt'>"+l+"</p>\n" )
            else :
                f.write( "<p class='doc-txt'>%s</p>\n" % find_string('undocumented'))
            f_ndx.write("<tr><td><code><b style='background-color: #ddd'>m</b></code>&nbsp;<a class='idx-tbl' href='scraft."+i+".htm#"+j+"' target='left1'>" + j + "</a></td></tr>\n")
        f.close()
        f_fr = open(prefix+"scraft."+i+".fr.htm","w+")
        f_fr.write("""
        <frameset cols="*, 200" border="1" frameborder="yes" framespacing="0">
            <frame src="scraft."""+i+""".htm" name="left1"/>
            <frame src="scraft."""+i+""".ndx.htm" name="right1"/>
        </frameset>
        """)
        f_fr.close()

f = open(prefix+"scraft.toc.htm","w+")
f.write(GenHeader())
f.write("<table cellspacing=0 cellpadding=3 border=0 class='idx-tbl' width='100%'>\n")
#f.write(" <b style='font-size: 150%'>SpriteCraft</b> <br />\n")
#f.write(" <b style='font-size: 120%'>2d game engine</b> <br /> <hr width = 100% style='border: 0; border-bottom: dashed #000 1px'> \n")
#f.write(" <a href='scraft.main.htm' target='right'>About SpriteCraft for Python</a> <br />")
f.write("<tr><td><a class='idx-tbl' href='scraft.main.htm' target='right'><b>%s</b></a></td></tr>" % find_string('Start Page'))
#f.write(" <hr width = 100% style='border: 0; border-bottom: dashed #000 1px'> \n")
f.write("<tr><td><a class='idx-tbl' href='scraft.constants.htm' target='right'>%s</a></td></tr>" % find_string('Constants'))
for i in sorted(g_types_map) :
    if not g_types_map[i].has_key('constants') :
        if g_types_map[i]['skipped'] :
            continue
        f.write("<tr><td><a class='idx-tbl' href='scraft."+i+".fr.htm' target='right'>"+i+"</a></td></tr>\n")
f.write("</table>")
f.close()

f = open("scraft."+lang+".htm","w+")
f.write("""
  <frameset rows="70, *" border="1" frameborder="yes" framespacing="0" >
        <frame src='"""+prefix+"""scraft.top.htm' scrolling="no" name="_topframe">
          <h1>test</h1>
        </frame>
        <frameset cols="200, *" border="1" frameborder="yes" framespacing="0" >
            <frame src='"""+prefix+"""scraft.toc.htm' />
            <frame src='"""+prefix+"""scraft.main.htm' name="right"/>
        </frameset>
  </frameset>
        """)
f.close()

f = open(prefix+"scraft_.hhc","w+t")
f.write("""<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<meta name="GENERATOR" content="Microsoft&reg; HTML Help Workshop 4.1">
<!-- Sitemap 1.0 -->
</HEAD><BODY>
<OBJECT type="text/site properties">
    <param name="ImageType" value="Folder">
</OBJECT>
<UL>\n""");
for i in sorted(g_types_map) :
    if not g_types_map[i].has_key('constants') :
        if g_types_map[i]['skipped'] :
            continue
        f.write("\t<LI> <OBJECT type=\"text/sitemap\">\n")
        f.write("\t\t<param name=\"Name\" value=\""+i+"\">\n")
        f.write("\t\t<param name=\"Local\" value=\"scraft."+i+".htm\">\n\t\t</OBJECT>\n")
f.write("""</UL>
</BODY></HTML>
""")
f.close()

f = open(prefix+"scraft_.hhk","w+t")
f.write("""<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<HTML>
<HEAD>
<meta name="GENERATOR" content="Microsoft&reg; HTML Help Workshop 4.1">
<!-- Sitemap 1.0 -->
</HEAD><BODY>
<UL>""")
for i in sorted(g_types_map) :
    if not g_types_map[i].has_key('constants') :
        if g_types_map[i]['skipped'] :
            continue
        meth = g_types_map[i]['methods']
        for j in sorted(meth) :
            f.write("\t<LI> <OBJECT type=\"text/sitemap\">\n")
            f.write("\t\t<param name=\"Name\" value=\""+j+"\">\n")
            f.write("\t\t<param name=\"Local\" value=\"scraft."+i+".htm#"+j+"\">\n")
            #f.write("\t\t<param name=\"Name\" value=\""+i+"."+j+"\">\n")
            #f.write("\t\t<param name=\"Local\" value=\"scraft."+i+".htm#"+j+"\">\n")
            f.write("\t</OBJECT>")
f.write("""</UL>
</BODY></HTML>
""")
f.close()

f = open(prefix+"scraft_.hhp","w+t")
f.write("""[OPTIONS]
Compatibility=1.1 or later
Compiled file=scraft.chm
Contents file=scraft_.hhc
Display compile progress=No
Index file=scraft_.hhk
Language=0x419


[INFOTYPES]
""")
f.close()

#f = open("scraft.main.htm","w+")
#f.close()
