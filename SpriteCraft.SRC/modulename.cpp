
/*

Copyright В© 2003-2013, AlexГ©y SudachГ©n, alexey@sudachen.name

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/


#include "stdafx.h"
#include "logout.h"

static char g_main_module_path[1024] = {0};
static char g_main_module_name[256] = {0};

void Analyze_MainModulePathAndName()
  {
    char cmdl[1024];
    pchar_t cmdline = GetCommandLineA();
    unsigned cmdl_size = teggo_min(strlen(cmdline),sizeof(cmdl)-1);
    char * const cmdlE = cmdl+cmdl_size;
    *cmdlE = 0;
    memcpy(cmdl,cmdline,cmdl_size);
    teggo::replace(cmdl,cmdlE,'\\','/');
    teggo::transform(cmdl,cmdlE,cmdl,tolower);
    char* p;
    if ( *cmdl == '"' ) 
      p = teggo::linear_find(cmdl+1,cmdlE,'"');
    else 
      p = teggo::linear_find(cmdl+1,cmdlE,' ');
    char* pp  = p;
    while ( pp != cmdl && *pp != '/' ) --pp; 
    if ( pp != p ) ++pp; else pp = cmdl;
    if ( !strnicmp(pp,"wscript.exe",p-pp) 
      || !strnicmp(pp,"cscript.exe",p-pp)
      || !strnicmp(pp,"python.exe",p-pp)
      || !strnicmp(pp,"pythonw.exe",p-pp)
      || !strnicmp(pp,"python_d.exe",p-pp)
      || !strnicmp(pp,"pythonw_d.exe",p-pp)
      ) 
      {
        char* pp0 = &*teggo::linear_find(p+1,cmdlE,' ',teggo::Not<teggo::Equal>());
        if ( pp0 != cmdlE ) 
          {
            char* pp1;
            if ( *pp0 == '"' ) 
              {
                pp1 = teggo::linear_find(pp0+1,cmdlE,'"');
                pp0++;
              }
            else 
              pp1 = teggo::linear_find(pp0+1,cmdlE,' ');

            char *pp2 = pp1;
            while( pp2 != pp0 && *pp2 != '/' ) --pp2;

            memcpy(g_main_module_name,pp2,pp1-pp2); // нуль там уже есть

            if ( pp2 == pp0 ) 
              {// рабочий каталог текущая директория
                GetCurrentDirectoryA(MAX_PATH,g_main_module_path);
              }
            else
              {
                *pp2 = 0;
                GetFullPathNameA(pp0,MAX_PATH,g_main_module_path,0);
              }
          }
        if ( strlen(g_main_module_name) == 0 ) 
          {
            strcpy(g_main_module_name,"uscript");
          }
      }
    else if ( !strnicmp("regsvr32.exe",pp,p-pp) ) 
      {
        g_main_module_path[0]=0;
        g_main_module_name[0]=0;
      }
    else if ( 
      !strnicmp("vb6.exe",pp,p-pp) 
      || !strnicmp("vb5.exe",pp,p-pp) 
      || !strnicmp("python.exe",pp,p-pp) 
      || !strnicmp("pythonw.exe",pp,p-pp) ) 
      {
        strncpy(g_main_module_name,pp,p-pp);
        GetCurrentDirectoryA(MAX_PATH,g_main_module_path);
      }
    else
      {
        strncpy(g_main_module_name,pp,p-pp);
        if  ( pp != cmdl ) 
          {
            --pp; *pp = 0;
            char* binname = cmdl;
            if ( *binname == '"' ) ++binname;
            GetFullPathNameA(binname,MAX_PATH,g_main_module_path,0);
          }
        else
          {
            GetCurrentDirectoryA(MAX_PATH,g_main_module_path);
          }
      }
    size_t mpath_len = strlen(g_main_module_path);
    teggo::replace(g_main_module_path,g_main_module_path+mpath_len,'\\','/');
    if ( g_main_module_path[mpath_len-1] == '/' ) 
      g_main_module_path[mpath_len-1] = 0;
  }

pchar_t Scraft_MainModuleFolder()
  {
    return g_main_module_path;
  }
  
pchar_t Scraft_MainModuleName()
  {
    return g_main_module_name;
  }
