# Project Name
TARGET = WavPlayer

# Library Locations
LIBDAISY_DIR = ./libDaisy
DAISYSP_DIR = ./DaisySP
SEDSLIB_DIR = ./sedsLib/Daisy

# Sources
CPP_SOURCES = WavPlayer.cpp

# Includes FatFS source files within project.
USE_FATFS = 1

# build for debug
DEBUG = 1

# Set optimisation level
OPT = -O0

# Set build for SDRAM bootloader
APP_TYPE=BOOT_NONE
#APP_TYPE=BOOT_SRAM
# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
