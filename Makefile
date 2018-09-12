BINDIR = bin
SRCDIR = src
INCLUDEDIR = include
APPDIR = application
OBJDIR = build
TESTDIR = test
CC = g++
CCARM = arm-linux-gnueabihf-g++
CFLAGS = -O3 -Wall -std=c++0x -pedantic -I $(INCLUDEDIR) -lpthread
LDFLAGS =

BIN = ${BINDIR}/joy_stick
APP = ${APPDIR}/main.cpp

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
APPOBJ = $(patsubst $(APPDIR)/%.cpp,$(OBJDIR)/%.o,$(APP))

_TESTS = $(wildcard $(TESTDIR)/*.cpp)
TESTS = $(patsubst %.cpp,%,$(_TESTS))

$(BIN): $(OBJS) $(APPOBJ)
	$(CC) -o $(BIN) $(APPOBJ) $(OBJS) $(CFLAGS) $(LDFLAGS)

$(APPOBJ): $(APP)
	$(CC) -c -o $@ $< $(CFLAGS)

${OBJDIR}/%.o: $(SRCDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

arm: $(OBJS) $(APPOBJ)
	$(CCARM) -o $(BIN) $(APPOBJ) $(OBJS) $(CFLAGS) $(LDFLAGS)

test: $(TESTS) 
	$(info ************  Testes concluídos com sucesso! ************)

$(TESTDIR)/t_%: $(TESTDIR)/t_%.cpp $(OBJS)
	$(CC) -o $@ $< $(OBJS) $(CFLAGS) $(LDFLAGS)
	$@

clean:
	find . -type f | xargs touch
	rm -f $(BIN) $(OBJS) $(APPOBJ)
	rm -f *.out
	rm -f $(TESTS)
