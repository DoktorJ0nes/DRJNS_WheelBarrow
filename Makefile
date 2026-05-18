CXX = cl.exe
CXXFLAGS = /nologo /O2 /EHsc /W3
LDFLAGS_DLL = /DLL /NOLOGO
LDFLAGS_EXE = /NOLOGO

DLL_TARGET = WheelBarrow.asi
DLL_SRC = WheelBarrow.cpp

all: $(DLL_TARGET) $(EXE_TARGET)

$(DLL_TARGET): $(DLL_SRC)
	$(CXX) $(CXXFLAGS) /LD $(DLL_SRC) /link $(LDFLAGS_DLL) /OUT:$(DLL_TARGET) user32.lib psapi.lib
	del /Q *.obj 2>nul

clean:
	del /Q *.obj *.asi 2>nul