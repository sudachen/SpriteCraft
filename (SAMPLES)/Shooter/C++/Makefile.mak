
.SUFFIXES: .cpp .obj

SCRAFTAPI = ../../../ScraftAPI
CLASSES   = ../../../Classes

CFLAGS = -I$(SCRAFTAPI) -I$(CLASSES) -D_TEGGOINLINE -EHsc -Zi -Od

EXECUTABLE=../shooter2.exe
OBJECTS= \
  shooter.obj \
  player.obj \
  tracker.obj \
  scraftapi.obj \

TARGET: $(EXECUTABLE)
$(OBJECTS): Makefile.mak

.cpp.obj:
	cl $(CFLAGS) -c $*.cpp -Fo$@

scraftapi.obj: $(SCRAFTAPI)/scraftapi.cpp Makefile.mak
	cl $(CFLAGS) -c $(SCRAFTAPI)/scraftapi.cpp -Fo$@

$(EXECUTABLE): $(OBJECTS) Makefile.mak
	link  -out:$(EXECUTABLE) $(OBJECTS) -debug -pdb:$*.pdb ole32.lib oleaut32.lib

