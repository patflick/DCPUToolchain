cmake_minimum_required(VERSION 2.8.5)
project(lua)

if(UNIX)
    set(ADDITIONAL_LIBRARIES m)
else()
    set(ADDITIONAL_LIBRARIES "")
endif()

add_library(lua STATIC
    lapi.c
    lauxlib.c
    lbaselib.c
    lcode.c
    ldblib.c
    ldebug.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    linit.c
    liolib.c
    llex.c
    lmathlib.c
    lmem.c
    loadlib.c
    lobject.c
    lopcodes.c
    loslib.c
    lparser.c
    lstate.c
    lstring.c
    lstrlib.c
    ltable.c
    ltablib.c
    ltm.c
    lundump.c
    lvm.c
    lzio.c
    print.c
    lapi.h
    lauxlib.h
    lcode.h
    ldebug.h
    ldo.h
    lfunc.h
    lgc.h
    llex.h
    llimits.h
    lmem.h
    lobject.h
    lopcodes.h
    lparser.h
    lstate.h
    lstring.h
    ltable.h
    ltm.h
    lua.h
    luaconf.h
    lualib.h
    lundump.h
    lvm.h
    lzio.h
)

target_link_libraries(lua ${ADDITIONAL_LIBRARIES})
set_property(TARGET lua PROPERTY FOLDER "third-party")
cotire(lua)
