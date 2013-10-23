
.SUFFIXES: .cpp .obj

SCRAFTAPI = ../../../ScraftAPI
CLASSES   = ../../../../Classes

CFLAGS = -I$(SCRAFTAPI) -I$(CLASSES) -D_TEGGOINLINE

EXECUTABLE=shooter2.exe
OBJECTS= \
  shooter.obj \
  player.obj \
  tracker.obj \
  scraftapi.obj \

TARGET: $(EXECUTABLE)

.cpp.obj:
	g++ $(CFLAGS) -c $< -o $@

scraftapi.obj: $(SCRAFTAPI)/scraftapi.cpp
	g++ $(CFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	g++ -o$(EXECUTABLE) $(OBJECTS) -lole32 -loleaut32
