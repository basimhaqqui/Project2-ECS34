CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g -I./include -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread -lexpat

OBJDIR = obj
BINDIR = bin

SOURCES = src/StringUtils.cpp src/StringDataSource.cpp src/StringDataSink.cpp \
          src/DSVReader.cpp src/DSVWriter.cpp src/XMLReader.cpp src/XMLWriter.cpp
TESTS = testsrc/StringUtilsTest.cpp testsrc/StringDataSourceTest.cpp \
        testsrc/StringDataSinkTest.cpp testsrc/DSVTest.cpp testsrc/XMLTest.cpp

OBJS = $(SOURCES:src/%.cpp=$(OBJDIR)/%.o)
TEST_OBJS = $(TESTS:testsrc/%.cpp=$(OBJDIR)/%.o)

$(shell mkdir -p $(OBJDIR) $(BINDIR))

all: teststrutils teststrdatasource teststrdatasink testdsv testxml

teststrutils: $(OBJDIR)/StringUtils.o $(OBJDIR)/StringUtilsTest.o
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS)
	@echo "Running $@ tests..."
	@$(BINDIR)/$@

teststrdatasource: $(OBJDIR)/StringDataSource.o $(OBJDIR)/StringDataSourceTest.o
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS)
	@echo "Running $@ tests..."
	@$(BINDIR)/$@

teststrdatasink: $(OBJDIR)/StringDataSink.o $(OBJDIR)/StringDataSinkTest.o
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS)
	@echo "Running $@ tests..."
	@$(BINDIR)/$@

testdsv: $(OBJDIR)/DSVReader.o $(OBJDIR)/DSVWriter.o $(OBJDIR)/DSVTest.o \
        $(OBJDIR)/StringDataSource.o $(OBJDIR)/StringDataSink.o
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS)
	@echo "Running $@ tests..."
	@$(BINDIR)/$@

testxml: $(OBJDIR)/XMLReader.o $(OBJDIR)/XMLWriter.o $(OBJDIR)/XMLTest.o \
        $(OBJDIR)/StringDataSource.o $(OBJDIR)/StringDataSink.o
	$(CXX) -o $(BINDIR)/$@ $^ $(LDFLAGS)
	@echo "Running $@ tests..."
	@$(BINDIR)/$@

$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: testsrc/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean teststrutils teststrdatasource teststrdatasink testdsv testxml