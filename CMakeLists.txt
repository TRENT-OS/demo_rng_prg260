#
# Rng PRG260 Demo Application
#
# Copyright (C) 2019-2023, HENSOLDT Cyber GmbH
#

cmake_minimum_required(VERSION 3.7.2)

#-------------------------------------------------------------------------------
# use the SDK
find_package("os-sdk" REQUIRED)
os_sdk_set_defaults()
os_sdk_setup(CONFIG_FILE "system_config.h" CONFIG_PROJECT "system_config")
CAmkESAddCPPInclude("plat/${PLATFORM}")

#-------------------------------------------------------------------------------
project(tests_rng_prg260 C)

DeclareCAmkESComponent(
    Rng_Prg_tester
    SOURCES
        rng_prg_tester.c
    C_FLAGS
        -Wall
        -Werror
    LIBS
        system_config
        os_core_api
        lib_debug
        lib_io
)


DeclareCAmkESComponents_for_UARTs()

# Use the existing entropy source, which at this point is just some dummy
Rng_Prg260_DeclareCAmkESComponent(
    Rng_Prg260
)

#ROOT SERVER
os_sdk_create_CAmkES_system("main.camkes")
