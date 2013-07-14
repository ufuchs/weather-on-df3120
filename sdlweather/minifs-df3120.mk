#
# partitial (C) Michel Pollet <buserror@gmail.com>
# (variables und structure conforms with 'minifs-build.sh' for a better understanding)
#
# Copyright(c) 2013 Uli Fuchs <ufuchs@gmx.com>
# MIT Licensed
#

# !set your `BASE` in the master script!
BUILD := $(BASE)/build-df3120

TARGET_SHARED := 0

STAGING := $(BUILD)/staging
STAGING_USR := $(BUILD)/staging/usr
STAGING_TOOLS=$(BUILD)/staging-tools

TOOLCHAIN := $(BASE)/toolchain

TARGET_FULL_ARCH := arm-v4t-linux-uclibcgnueabi

CROSS_BASE := $(TOOLCHAIN)/$(TARGET_FULL_ARCH)
CROSS := $(CROSS_BASE)/bin/$(TARGET_FULL_ARCH)

# ccfix is the prefixer for gcc that warns of "absolute" host paths
CC := $(STAGING_TOOLS)/bin/ccfix  $(CROSS)-gcc
CXX := $(STAGING_TOOLS)/bin/ccfix $(CROSS)-g++
LD := $(STAGING_TOOLS)/bin/ccfix $(CROSS)-ld
STRIP := $(CROSS)-strip

###############################################################################

TARGET_CPPFLAGS := -I$(STAGING)/include -I$(STAGING_USR)/include
CPPFLAGS := $(TARGET_CPPFLAGS)

LDFLAGS_BASE := -L$(STAGING)/lib -L$(STAGING_USR)/lib
#ifeq ($(strip $(TARGET_SHARED), 0)
#  LDFLAGS_BASE := -static $LDFLAGS_BASE
#endif

CFLAGS  := -Os -march=armv4t -mtune=arm920t -mthumb-interwork -mthumb -std=c99
CXXFLAGS := $(CFLAGS)

LDFLAGS_RLINK = $(LDFLAGS_BASE) -Wl,-rpath -Wl,/usr/lib -Wl,-rpath-link -Wl,$(STAGING)/lib -Wl,-rpath-link -Wl,$(STAGING_USR)/lib
LDFLAGS = $(LDFLAGS_BASE)

###############################################################################

