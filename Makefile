################################################################################
# Program
################################################################################

PROGRAM_NAME = walker-simulator-2015
PROGRAM_DIR = walker

################################################################################
# Paths
################################################################################

PATH_SRC = src
PATH_OBJ = obj
PATH_DOC = doc
PATH_LIB = lib
PATH_BIN = bin
PATH_MAN = man
PATH_TESTS = tests
PATH_INCLUDE = include

################################################################################
# Files
################################################################################

vpath %.h $(PATH_INCLUDE) $(PATH_TESTS)/include $(PATH_INCLUDE)/$(PROGRAM_DIR)
vpath %.c $(PATH_SRC) $(PATH_TESTS)/src $(PATH_SRC)/$(PROGRAM_DIR)
vpath %.o $(PATH_OBJ)
vpath %.a $(PATH_LIB)
vpath %.so $(PATH_LIB)
vpath $(PROGRAM_NAME) $(PATH_BIN)

################################################################################
# Flags, first pass.
################################################################################

FLAGS_CC_DEBUG = -g
FLAGS_CC_WARNINGS = -W -Wall -Wextra -Wfloat-equal -Wswitch-default \
	-Winit-self -Wshadow -Wbad-function-cast -Wcast-qual -Wcast-align \
	-Wconversion -Wlogical-op -Wstrict-prototypes -Wnested-externs
FLAGS_CC_OPTIMIZATIONS = -O3 -march=native
FLAGS_CC_INCLUDE = -I$(PATH_INCLUDE)
FLAGS_CC_LIB = -L$(PATH_LIB)
FLAGS_CC_MINIMAL = -std=gnu99 -pedantic $(FLAGS_CC_INCLUDE)

################################################################################
# Conventionnal (mostly) variables
################################################################################

# Users usually expect Makefiles to provide support for various variables in
# order to ease configuration.
# For instance:
#     $ make CC=clang
#     $ make CFLAGS="-O2"
#     $ make install PREFIX=/opt/success
#     $ make LDFLAGS="-L/opt/mylib/lib"

# These are the only variables the user is *allowed* (nothing prevents the user
# from overriding other variables...) to override.

## Shell
SHELL = /bin/sh

## Programs
# These variables usually are implicitly defined.
# Ensure they are defined.
CC = gcc
AR = ar
RM = rm -rf

# Extras (not really conventionnal)
DOC = doxygen
MKDIR = mkdir -p
CP = cp
ECHO = echo

# Install
INSTALL = install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

## Flags
ARFLAGS = crvs
CFLAGS = $(FLAGS_CC_WARNINGS) $(FLAGS_CC_OPTIMIZATIONS)
LDFLAGS =
LDLIBS =

## Directories
# In most situations, changing only PREFIX is enough.
#
#     $ make install PREFIX=/opt/success
PREFIX = /usr/local
EXEC_PREFIX = $(PREFIX)
BINDIR = $(EXEC_PREFIX)/bin
SBINDIR = $(EXEC_PREFIX)/sbin
LIBEXECDIR = $(EXEC_PREFIX)/libexec
DATAROOTDIR = $(PREFIX)/share
DATADIR = $(DATAROOTDIR)
SYSCONFDIR = $(PREFIX)/etc
SHAREDSTATEDIR = $(PREFIX)/com
LOCALSTATEDIR = $(PREFIX)/var
RUNSTATEDIR = $(LOCALSTATEDIR)/run
INCLUDEDIR = $(PREFIX)/include
DOCDIR = $(DATAROOTDIR)/doc/$(PROGRAM_NAME)
LIBDIR = $(EXEC_PREFIX)/lib
MANDIR = $(DATAROOTDIR)/man

################################################################################
# Flags, second pass
################################################################################

# The user is allowed to override some flags. But there are minimal requirements.
# Ensure these requirements are set even if the flags are empty.
override CFLAGS += $(FLAGS_CC_MINIMAL)
override LDLIBS += $(FLAGS_CC_LIB)
override LDFLAGS += -lrt

################################################################################
# Actual building
################################################################################

PROGRAM_OBJECTS = main.o

all: $(PROGRAM_NAME) | bin_dir

## Executable
$(PROGRAM_NAME): $(PROGRAM_OBJECTS) | bin_dir
	$(CC) -o $(PATH_BIN)/$@ \
		$(patsubst %.o,$(PATH_OBJ)/%.o, $(patsubst $(PATH_OBJ)/%,%, $^)) \
		$(LDFLAGS) $(LDLIBS)

## Object files
# Generate .o object files.
%.o: %.c | obj_dir
	$(CC) $(CFLAGS) -o $(PATH_OBJ)/$@ -c $<

# Rules for object files
main.o: main.c version.h

################################################################################
# Documentation
################################################################################

doc:
	@$(DOC)

################################################################################
# Directories
################################################################################

obj_dir:
	@$(MKDIR) $(PATH_OBJ)

lib_dir:
	@$(MKDIR) $(PATH_LIB)

bin_dir:
	@$(MKDIR) $(PATH_BIN)

################################################################################
# (Un)Installing
################################################################################

# DESTDIR: Support staged (un)installs.
# DESTDIR should be defined as a command line argument (thus it is empty by
# default).
#
#     $ make install DESTDIR=/tmp/stage
#     $ make uninstall DESTDIR=/tmp/stage
#
# Note: DESTDIR is different from PREFIX!
#
# Warning: PREFIX should have a leading '/' (the default does) when overriding
#          DESTDIR.

install:
	@$(INSTALL_PROGRAM) -D $(PATH_BIN)/$(PROGRAM_NAME) \
		$(DESTDIR)$(BINDIR)/$(PROGRAM_NAME) \
		&& echo "install: $(DESTDIR)$(BINDIR)/$(PROGRAM_NAME)"
	@$(INSTALL_DATA) -D $(PATH_MAN)/man1/$(PROGRAM_NAME).1 \
		$(DESTDIR)$(MANDIR)/man1/$(PROGRAM_NAME).1 \
		&& echo "install: $(DESTDIR)$(MANDIR)/man1/$(PROGRAM_NAME).1"

uninstall:
	@$(RM) $(DESTDIR)$(BINDIR)/$(PROGRAM_NAME) \
		&& echo "uninstall: $(DESTDIR)$(BINDIR)/$(PROGRAM_NAME)"
	@$(RM) $(DESTDIR)$(MANDIR)/man1/$(PROGRAM_NAME).1 \
		&& echo "uninstall: $(DESTDIR)$(MANDIR)/man1/$(PROGRAM_NAME).1"

################################################################################
# Cleaning
################################################################################

clean:
	@$(RM) $(PATH_BIN) $(PATH_OBJ) $(PATH_LIB)
	@$(ECHO) "Clean."

cleandoc: clean_doc
clean_doc:
	@$(RM) $(PATH_DOC)

cleanall: clean_all
clean_all: clean clean_doc
	@$(ECHO) "Super clean."
