# The path where the vectorize.h header lives
VECTORIZE_DIR = ..

# Path to the vectorize header
VECTORIZE_HEADER = $(VECTORIZE_DIR)/vectorize.h

# Where to find user code.
USER_DIR = .

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include -I$(VECTORIZE_DIR)

# Flags passed to the C++ compiler.
CXXFLAGS += -O2 -std=c++0x -Wall -Wextra -Werror

# Use clang as the compiler. Comment this out to use the default.
CXX = /usr/local/bin/clang++

# All outputs produced by this Makefile.
OUTPUTS = apply2add.o

# House-keeping build targets.

all : $(OUTPUTS)

clean :
	rm -f *.o

apply2add.o : $(USER_DIR)/apply2add.cpp \
	            $(VECTORIZE_HEADER)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/apply2add.cpp

