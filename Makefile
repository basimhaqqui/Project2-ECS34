CXX = g++
CXXFLAGS = -g -std=c++17 -Wall -Wextra -Werror
INCLUDES = -I./include
LDFLAGS = -lgtest -lgtest_main -lpthread -lexpat

# Directories
OBJDIR = obj
BINDIR = bin
SRCDIR = src
TESTDIR = testsrc

# Test executables
TESTSTRUTILS = $(BINDIR)/teststrutils
TESTSTRDATASOURCE = $(BINDIR)/teststrdatasource
TESTSTRDATASINK = $(BINDIR)/teststrdatasink
TESTDSV = $(BINDIR)/testdsv
TESTXML = $(BINDIR)/testxml

.PHONY: all clean test directories

all: directories $(TESTSTRUTILS) $(TESTSTRDATASOURCE) $(TESTSTRDATASINK) $(TESTDSV) $(TESTXML) test

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

# Object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Test executables
$(TESTSTRUTILS): $(OBJDIR)/StringUtils.o $(OBJDIR)/StringUtilsTest.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(TESTSTRDATASOURCE): $(OBJDIR)/StringDataSource.o $(OBJDIR)/StringDataSourceTest.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(TESTSTRDATASINK): $(OBJDIR)/StringDataSink.o $(OBJDIR)/StringDataSinkTest.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(TESTDSV): $(OBJDIR)/DSVReader.o $(OBJDIR)/DSVWriter.o $(OBJDIR)/StringDataSource.o $(OBJDIR)/StringDataSink.o $(OBJDIR)/DSVTest.o
	$(CXX) $^ $(LDFLAGS) -o $@

$(TESTXML): $(OBJDIR)/XMLReader.o $(OBJDIR)/XMLWriter.o $(OBJDIR)/StringDataSource.o $(OBJDIR)/StringDataSink.o $(OBJDIR)/XMLTest.o
	$(CXX) $^ $(LDFLAGS) -o $@

test: $(TESTSTRUTILS) $(TESTSTRDATASOURCE) $(TESTSTRDATASINK) $(TESTDSV) $(TESTXML)
	./$(TESTSTRUTILS)
	./$(TESTSTRDATASOURCE)
	./$(TESTSTRDATASINK)
	./$(TESTDSV)
	./$(TESTXML)

clean:
	rm -rf $(OBJDIR) $(BINDIR)