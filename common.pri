CONFIG -= qt
CONFIG -= app_bundle

SYSTEMC_PATH = C:/SystemC
isEmpty(SYSTEMC_PATH) {
    error("SYSTEMC_PATH not defined. Please fix it (file: common.pri)")
} else {
    message("SYSTEMC_PATH $$SYSTEMC_PATH")
}

INCLUDEPATH = $${SYSTEMC_PATH}/include

win32 {
  LIBS += $${SYSTEMC_PATH}/lib-mingw/libsystemc.a
}

unix {
    contains(QMAKE_HOST.arch, x86_64) {
         # 64-bit Unix
        mac {
            LIBS = -L$${SYSTEMC_PATH}/lib-macosx64 -lsystemc -Xlinker -rpath -Xlinker $${SYSTEMC_PATH}/lib-macosx64
        } else {
            LIBS = -L$${SYSTEMC_PATH}/lib-linux64 -lsystemc -Xlinker -rpath -Xlinker $${SYSTEMC_PATH}/lib-linux64
        }
    } else {
        # 32-bit Unix not OS X
        LIBS = -L$${SYSTEMC_PATH}/lib-linux -lsystemc -Xlinker -rpath -Xlinker $${SYSTEMC_PATH}/lib-linux
    }
}

OBJECTS_DIR = ../objs

CONFIG -= debug_and_release
