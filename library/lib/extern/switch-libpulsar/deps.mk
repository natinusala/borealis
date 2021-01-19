export PLSR_DEP_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
export PLSR_DIR := $(dir $(PLSR_DEP_MAKEFILE))

export PLSR_TARGET := pulsar
export PLSR_NAME := Pulsar

export PLSR_SOURCES := src/archive \
	src/bfgrp \
	src/bfsar \
	src/bfwar \
	src/bfwav \
	src/bfwsd \
	src/player

export PLSR_INCLUDES := include
