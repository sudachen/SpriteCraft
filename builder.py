import sys,os,getopt,stat,popen2

opts,args = getopt.getopt(sys.argv[1:],"vf:d:",[])
self_dir = os.path.normpath(os.path.dirname(sys.argv[0]))

out_folder = os.path.normpath('./~temp~')
config_file = "BuildRules"
compiler = 'mscc'
project_objects = []
verbose_flag = False
build_action = None

variables = { 'common_ccf': '',
              'as_flags': '',
              'c_flags': '',
              'cxx_flags': '',
              'ld_flags': '',
              'rc_flags': '',
              'libraries': '',
              'target': None,
            }

#Sleep = ctypes.windll.kernel32.Sleep
def Sleep(x): pass

def Build_DoExec(fname,command,variables,action):
    if action in ( 'clean', 'iclean' ):
        if os.path.exists(fname):
            os.unlink(fname)
        return
    target = variables['target']
    if target and fname:
        if os.path.exists(target) and os.path.exists(fname):
            t_mtime = os.stat(target)[stat.ST_MTIME]
            f_mtime = os.stat(fname)[stat.ST_MTIME]
            if f_mtime >= t_mtime:
                return 0
    if command.startswith('@'):
        command = command[1:]
    else:
        if verbose_flag:
            print command
    cmd_out = os.popen(command,"rt")
    for cmd_error in cmd_out.xreadlines():
        sys.stdout.write(cmd_error)
    return cmd_out.close()

def Build_DoLink(target,objects,variables,action):
    depends = None
    subcommand = None
    _semi_column = target.find(';')
    if _semi_column >= 0 :
        subcommand = target[_semi_column+1:].strip()
        target = target[0:_semi_column].strip()
    _column = target.find(':')
    if _column >= 0 :
        depends = target[_column+1:].strip()
        target = target[0:_column].strip()

    target_type = None
    _period = target.find(',')
    if _period > 0:
        target_type = target[_period+1:].strip().lower()
        target = target[0:_period].strip()

    if not target_type:
        ltarget = target.lower()
        if ltarget.endswith('.lib'):
            target_type = 'lib'
        elif ltarget.endswith('.a'):
            target_type = 'lib'
        elif ltarget.endswith('.exe'):
            target_type = 'exe'
        elif ltarget.endswith('.dll'):
            target_type = 'dll'

    if depends: depends = map( lambda x: x.strip(),depends.split(',') )
    else: depends = []


    variables['target'] = target

    if action in ('clean','iclean'):
        objectslist = os.path.join(out_folder,"~objectslist~.a")
        if os.path.exists(objectslist):
            os.unlink(objectslist)
        if os.path.exists(target):
            os.unlink(target)
        return

    depends_changed = False

    if os.path.exists(target):
        out_f_mtime = os.stat(target)[stat.ST_MTIME]
        if subcommand and not depends:
            depends_changed = True
        else:
            for dep_file in depends:
                dep_f_mtime = os.stat(os.path.normpath(dep_file))[stat.ST_MTIME]
                if dep_f_mtime > out_f_mtime:
                    depends_changed = True
                    break

    if compiler == 'mscc':
        objectslist = os.path.join(out_folder,"~objectslist~")
        f_ol = open(objectslist,"w+t")
        litmtime = objects[0][2]
        for i in objects:
            f_ol.write('"'+i[1]+'"\n')
            if litmtime < i[2]:
                litmtime = i[2]
        f_ol.close()
        if os.path.exists(target) and not depends_changed:
            if os.stat(target)[stat.ST_MTIME] >= litmtime:
                return 0
        if target_type == 'lib':
            if os.path.exists(target):
                os.unlink(target)
            command = 'lib -nologo -out:"%s" @"%s"' % (target,objectslist)
        else:
            command = 'link -nologo -out:'+'"'+target+'" '+variables['ld_flags']+' '+variables['libraries']+' @'+objectslist
    else:
        objectslist = os.path.join(out_folder,"~objectslist~.a")
        litmtime = objects[0][2]
        for i in objects:
            if litmtime < i[2]:
                litmtime = i[2]
        for i in objects[1:]:
            command = 'ar ru "%s" "%s" 2>&1' % (objectslist,i[1])
            cmdout = os.popen(command,"rt")
            for l in cmdout.xreadlines():
                sys.stdout.write(l)
            e = cmdout.close()
            if e:
                return e
        if os.path.exists(target) and not depends_changed:
            if os.stat(target)[stat.ST_MTIME] >= litmtime:
                return 0

        command = 'ld -o "%s" %s "%s" "%s" %s 2>&1' % (target,variables['ld_flags'],objects[0][1],objectslist,variables['libraries'])

    print "linking '"+target+"'"
    if verbose_flag:
        print command
    cmdout = os.popen(command,"rt")
    for l in cmdout.xreadlines():
        sys.stdout.write(l)
    return cmdout.close()

def DoSubcommand(subcommand):
    subcommand += ' 2>&1'
    print subcommand
    cmdout = os.popen(subcommand,"rt")
    for l in cmdout.xreadlines():
        sys.stdout.write(l)
        sys.stdout.flush()
    e = cmdout.close()
    if e: return e

def Build_DoCompile(fname,objects,variables,action):

    noappend = False
    depends = None
    subcommand = None
    subcommand_is_builder = None
    fname_semi_column = fname.find(';')
    if fname_semi_column >= 0 :
        subcommand = fname[fname_semi_column+1:].strip()
        if subcommand.startswith('|'):
            subcommand = '%s %s %s %s %s' % (sys.argv[0], subcommand[1:], build_name, build_release, action)
            subcommand_is_builder = True
        fname = fname[0:fname_semi_column].strip()
    fname_column = fname.find(':')
    if fname_column >= 0 :
        depends = fname[fname_column+1:].strip()
        fname = fname[0:fname_column].strip()
    if fname.startswith('-'):
        noappend = True
        fname = fname[1:]

    if depends: depends = map( lambda x: x.strip(),depends.split(',') )
    else: depends = []

    if not fname and not subcommand:
        return

    lfname = fname.lower()
    command_flags = None

    file_mode = None
    if lfname.endswith('.cpp'):  file_mode = 'cpp'
    elif lfname.endswith('.c'):  file_mode = 'c'
    elif lfname.endswith('.rc'): file_mode = 'rc'
    elif lfname.endswith('.s'):  file_mode = 's'
    elif not subcommand:
        print "unknown file mode for '%s'" %fname
        return -1;
    else:
        file_mode = 'subcommand'

    if file_mode in ('c','cpp'):
        command_flags = ''
        command_flags += variables['common_ccf']
        command_flags += ' '
        if file_mode == 'cpp':
            o_file = lfname[:-4]
            command_flags += variables['cxx_flags']
        else: # .c
            o_file = lfname[:-2]
            command_flags += variables['c_flags']
    elif file_mode == 'rc':
        o_file = lfname[:-3]
        command_flags = variables['rc_flags']
    elif file_mode == 's':
        o_file = lfname[:-2]
        command_flags = variables['as_flags']
    elif file_mode == 'subcommand':
        o_file = lfname

    if file_mode != 'subcommand':
        o_file = os.path.normpath(o_file).replace('\\','#')
        o_file = o_file.replace('..','!')

    if file_mode in ('rc',):
        o_file = o_file + ".res"
    elif file_mode != 'subcommand':
        o_file = o_file + ".o"

    if not os.path.exists(out_folder):
        os.makedirs(out_folder)

    if file_mode == 'subcommand':
        out_file_path = o_file
    else:
        out_file_path = os.path.join(out_folder,o_file)

    in_file_path  = os.path.normpath(lfname)

    if action == 'clean':
        if os.path.exists(out_file_path):
            os.unlink(out_file_path )
        if subcommand_is_builder:
            return DoSubcommand(subcommand)
        return
    elif action == 'iclean':
        if not subcommand_is_builder:
            if os.path.exists(out_file_path):
                os.unlink(out_file_path )
        return

    o = [in_file_path,out_file_path,None]
    if not noappend:
        objects.append(o)

    depends_changed = False

    if os.path.exists(out_file_path):

        out_f_mtime = os.stat(out_file_path)[stat.ST_MTIME]

        if subcommand and ( not depends or not os.path.exists(in_file_path) ):
            depends_changed = True
        else:
            if not os.path.exists(in_file_path):
                print "file '%s' is not exists, don't cnow how it build " % in_file_path
                sys.exit(1)
            in_f_mtime = os.stat(in_file_path)[stat.ST_MTIME]
            for dep_file in depends:
                try:
                    dep_f_mtime = os.stat(os.path.normpath(dep_file))[stat.ST_MTIME]
                except OSError, e:
                    print e
                    return -1
                mtime = out_f_mtime
                if noappend:
                    mtime = in_f_mtime
                if dep_f_mtime > mtime:
                    depends_changed = True
                    break

        if not depends_changed:
            in_f_mtime = os.stat(in_file_path)[stat.ST_MTIME]
            if in_f_mtime <= out_f_mtime:
                # nothing to do
                o[2] = out_f_mtime
                return

    if subcommand and ( depends_changed or not os.path.exists(in_file_path) ):
        #print subcommand
        DoSubcommand(subcommand)

    if compiler == 'mscc':
        C_compiler = 'cl -nologo -c %s -Fo"%s" "%s"'
        CXX_compiler = 'cl -nologo -c %s -Fo"%s" "%s"'
        RC_compiler = 'rc %s -Fo"%s" "%s"'
    elif compiler == 'mingw':
        C_compiler = 'gcc -c %s -o "%s" "%s"  2>&1'
        CXX_compiler = 'g++ -c %s -o "%s" "%s"  2>&1'
        RC_compiler = 'windres %s -O coff -o "%s" "%s"  2>&1'
    else:
        print "unknown compiler '%s'" %compiler
        return -1

    AS_compiler = 'as %s -o "%s" "%s" 2>&1'

    command = None

    if file_mode == 'c':
        command = C_compiler % (command_flags,out_file_path,in_file_path)
    if file_mode == 'cpp':
        command = CXX_compiler % (command_flags,out_file_path,in_file_path)
    if file_mode == 'rc':
        command = RC_compiler % (command_flags,out_file_path,in_file_path)
    if file_mode == 's':
        command = AS_compiler % (command_flags,out_file_path,in_file_path)

    if command:
        if verbose_flag:
            print command
        cmdout = os.popen(command,"rt")
        if compiler == 'mscc' and file_mode in ('c','cpp'):
           cmdout.readline()

        print os.path.basename(in_file_path)

        for l in cmdout.xreadlines():
            sys.stdout.write(l)
        e = cmdout.close()
    else:
        e = 0

    if not e:
        o[2] = os.stat(out_file_path)[stat.ST_MTIME]
    return e

for o,a in opts:
    if o == '-f':
        config_file = a
    if o == '-d':
        os.chdir(a)
    if o == '-v':
        verbose_flag = True

def DoAction(action):

    global out_folder
    global compiler
    global objects

    m4_command = os.path.join(self_dir,'GNU_m4.exe')+\
                    ' -D_BUILD_NAME_="'+build_name+\
                    '" -D_BUILD_RELEASE_='+build_release+\
                    ' -P "'+config_file+'"'

    try:
#        m4_out = os.popen(m4_command,"rt")
        _,m4_out,_ = os.popen3(m4_command)
    except:
        print "failed to preprocess rulesfile"
        sys.exit(-1)

    f_continue = False

    for _ in m4_out.xreadlines():

        sys.stdout.flush()

        _ = _.rstrip('\n').strip()

        if not f_continue:
            br_line = _
        else:
            br_line = br_line[:-1] + _

        if br_line.endswith('\\'):
            f_continue = True
            continue

        f_continue = False
        #print br_line

        lbr_line = br_line.lower()

        if lbr_line.startswith('compiler:'):
            compiler = br_line[9:].strip().lower()
        elif lbr_line.startswith('link:'):
            target = br_line[5:].strip().strip('"\'')
            e = Build_DoLink(target,project_objects,variables,action)
            sys.stdout.flush()
            Sleep(10)
            if e:
                print "failed to build project"
                sys.exit(e)
            objects = []
        elif lbr_line.startswith('cppflags:'):
            variables['cxx_flags'] = br_line[9:].strip()
        elif lbr_line.startswith('cflags:'):
            variables['c_flags'] = br_line[7:].strip()
        elif lbr_line.startswith('ldflags:'):
            variables['ld_flags'] = br_line[8:].strip()
        elif lbr_line.startswith('common_ccflags:'):
            variables['common_ccf'] = br_line[15:].strip()
        elif lbr_line.startswith('libraries:'):
            variables['libraries'] = br_line[10:].strip()
        elif lbr_line.startswith('outdir:'):
            out_folder = br_line[7:].strip().strip('"\'')
        elif lbr_line.startswith(';'):
            filename = br_line[1:]
            filename = filename.strip()
            e = Build_DoCompile(filename,project_objects,variables,action)
            sys.stdout.flush()
            Sleep(10)
            if e:
                print "failed to build project"
                sys.exit(e)
        elif lbr_line.startswith(':'):
            filename = br_line[1:].lstrip()
            filename_e = filename.find(';')
            if filename_e >= 0:
                command = filename[filename_e+1:].strip()
                filename = filename[0:filename_e].rstrip()
                e = Build_DoExec(filename,command,variables,action)
                sys.stdout.flush()
                Sleep(10)
                if e:
                    print "failed to execute action"
                    sys.exit(e)
        elif lbr_line.startswith('env.'):
            env_prop_e = br_line.find(':',4)
            env_prop = br_line[4:env_prop_e]
            env_part = br_line[env_prop_e+1:].strip().strip('"\'')+" "
            while True:
                env_part_i = env_part.find('%')
                if env_part_i >= 0:
                    env_part_e = env_part.find('%',env_part_i+1)
                    env_key = env_part[env_part_i+1:env_part_e]
                    env_val = os.getenv(env_key,'')
                    env_part = env_part[0:env_part_i] + env_val + env_part[env_part_e+1]
                else: break
            #print "%s = '%s'" % (env_prop,env_part[0:-1])
            os.putenv(env_prop,env_part[0:-1])

build_name = ''
build_release = 'release'
build_action = 'build'

if args:
    if not args[0] in ('release','debug','build','rebuild','clean','iclean','irebuild'):
        build_name = args[0]
        args = args[1:]
if args:
    if args[0] in ('release','debug'):
        build_release = args[0]
        args = args[1:]

if args:
    if args[0] in ('build','rebuild','clean','iclean','irebuild'):
        build_action = args[0]
        args = args[1:]

if args:
    print "error: invalid action "+args[0]
    sys.exit(-1)


if build_action in ('clean','rebuild'):
    print "-- removing intermediate files and targets"
    DoAction('clean')
elif build_action in ('iclean','irebuild'):
    print "-- removing intermediate files and targets"
    DoAction('iclean')

if build_action in ('build','rebuild','irebuild'):
    print "-- building %s %s" % ( build_name, build_release )
    DoAction('build')
