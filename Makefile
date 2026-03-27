# Makefile for Lightweight C CMS with SQLite
# This file defines commands to build, run, and clean the CMS project.

# -----------------------------
# Variables
# -----------------------------

# Compiler to use (GCC for C programs)
CC = gcc

# Libraries to link (SQLite is needed for database operations)
LIBS = -lsqlite3

# Compiler flags
# -Wall : enable all warnings
# -g    : include debugging information
CFLAGS = -Wall -g

# -----------------------------
# Targets
# -----------------------------

# Default target: build the executable 'cms'
all:
	# Compile main.c and db.c into executable 'cms'
	$(CC) main.c db.c -o cms $(CFLAGS) $(LIBS)
	# Inform the user
	@echo "Build complete! Run './cms' to start the CMS."

# Run the CMS directly (build first if needed)
run: all
	# Execute the compiled program
	./cms

# Clean the build (delete the executable)
clean:
	# Remove the 'cms' executable file
	rm -f cms
	# Inform the user
	@echo "Clean complete! Executable removed."
