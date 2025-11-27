CMAKE_MINIMUM_REQUIRED( VERSION 3.22 )
PROJECT(dagbase)

cmake_policy(SET CMP0114 NEW)

SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_EXTENSIONS OFF )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

FIND_PACKAGE(Lua 5.4)
include(FetchContent)
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main)
FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main) # need main for benchmark::benchmark

FetchContent_MakeAvailable(
        googletest
        googlebenchmark)

SET( ALL_PUBLIC_HEADERS include/core/Class.h include/core/MetaClass.h include/core/ConfigurationElement.h include/core/LuaInterface.h include/util/errod.h include/util/dbc.h include/util/Searchable.h include/util/PrettyPrinter.h
include/core/ByteBuffer.h
include/io/Stream.h
include/io/InputStream.h
include/io/OutputStream.h
include/io/StreamFormat.h
include/io/BinaryFormat.h
include/io/BackingStore.h
include/io/TextFormat.h
include/io/MemoryBackingStore.h
include/util/DebugPrinter.h
        include/io/FormatAgnosticOutputStream.h
        include/io/FormatAgnosticInputStream.h
        include/util/SearchableMap.h
        include/core/Atom.h
        include/core/DynamicVisitor.h
        include/util/SearchableArray.h
        include/core/Snippet.h
        include/core/MetaProperty.h
        include/util/enums.h
        include/core/Variant.h
        include/core/Colour.h
        include/core/PropertyFormat.h
        include/core/Vec2.h
        include/core/PropertyBinding.h
        include/core/IdentifierGenerator.h
        include/core/BaseDimension.h
        include/core/Dimension.h
        include/core/Unit.h
        include/core/Reference.h
        include/core/PropertyBag.h
        include/core/PropertyBagFactory.h
        include/util/TimeProvider.h
        include/util/FakeTimeProvider.h
        include/util/MultiArray.h
        include/util/DynamicMultiArray.h
        include/core/VariantArray.h
        include/util/RedBlackTree.h
        include/util/ParameterLookup.h
        include/core/Function.h
        include/util/VectorMultimap.h
        include/util/Less.h
        include/util/DynamicEnum.h
        include/util/DynamicBitset.h
)

SET( DEP_ROOT CACHE PATH "Dependency root" )

SET(HEADERS_GROUP "Header Files")

SOURCE_GROUP(
    ${HEADERS_GROUP}
    FILES ${ALL_PUBLIC_HEADERS}
)

SET(GENERATED_HEADERS_GROUP "Generated header files")
SOURCE_GROUP(
    ${GENERATED_HEADERS_GROUP}
    FILES ${GENERATED_HEADERS}
)

SET( dagbase_SRC ${GENERATED_HEADERS} ${ALL_PUBLIC_HEADERS} src/core/LuaInterface.cpp src/core/ConfigurationElement.cpp src/util/errod.cpp src/util/PrettyPrinter.cpp
src/core/Class.cpp
src/core/MetaClass.cpp
src/core/ByteBuffer.cpp
src/io/InputStream.cpp
src/io/OutputStream.cpp
src/io/TextFormat.cpp
src/io/MemoryBackingStore.cpp
src/io/BinaryFormat.cpp
src/util/DebugPrinter.cpp
        src/io/FormatAgnosticOutputStream.cpp
        src/io/FormatAgnosticInputStream.cpp
        src/core/Atom.cpp
        src/core/Snippet.cpp
        src/core/MetaProperty.cpp
        src/core/Colour.cpp
        src/core/Variant.cpp
        src/core/Vec2.cpp
        src/core/PropertyBinding.cpp
        src/core/IdentifierGenerator.cpp
        src/core/Dimension.cpp
        src/core/Unit.cpp
        src/core/PropertyBagFactory.cpp
        src/core/PropertyBag.cpp
        src/util/FakeTimeProvider.cpp
        src/util/DynamicMultiArray.cpp
        src/core/VariantArray.cpp
        src/util/RedBlackTree.cpp
        src/util/ParameterLookup.cpp
        src/core/Function.cpp
        src/core/DynamicEnum.cpp
)

ADD_LIBRARY( dagbase SHARED ${dagbase_SRC}
        include/util/VectorSet.h)
TARGET_INCLUDE_DIRECTORIES( dagbase PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${PROJECT_BINARY_DIR}/include ${LUA_INCLUDE_DIR})
TARGET_LINK_DIRECTORIES( dagbase PUBLIC ${DEP_ROOT}/lib )
TARGET_LINK_LIBRARIES( dagbase PRIVATE ${LUA_LIBRARIES})
SET_TARGET_PROPERTIES( dagbase PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib" )

set_target_properties( dagbase PROPERTIES DEFINE_SYMBOL DAGBASE_LIBRARY )

REMOVE_DEFINITIONS( -DDAGBASE_LIBRARY )

INCLUDE( CheckIncludeFile )
INCLUDE( CheckIncludeFileCXX )
CHECK_INCLUDE_FILE( windows.h HAVE_WINDOWS_H PATHS DOC "Whether we have the windows.h header, hopefully only on Windows!")
CHECK_INCLUDE_FILE( winsock2.h HAVE_WINSOCK2_H PATHS DOC "")
CHECK_INCLUDE_FILE( float.h HAVE_FLOAT_H  DOC "")
CHECK_INCLUDE_FILE( unistd.h HAVE_UNISTD_H  DOC "")
CHECK_INCLUDE_FILE( sys/types.h HAVE_SYS_TYPES_H  DOC "")
CHECK_INCLUDE_FILE( sys/socket.h HAVE_SYS_SOCKET_H  DOC "")
CHECK_INCLUDE_FILE( sys/time.h HAVE_SYS_TIME_H  DOC "")
CHECK_INCLUDE_FILE( sys/ioctl.h HAVE_SYS_IOCTL_H  DOC "")
CHECK_INCLUDE_FILE( sys/errno.h HAVE_SYS_ERRNO_H  DOC "")
CHECK_INCLUDE_FILE( sys/sysmp.h HAVE_SYS_SYSMP_H  DOC "")
CHECK_INCLUDE_FILE( sys/mman.h HAVE_SYS_MMAN_H  DOC "")
CHECK_INCLUDE_FILE( sys/stat.h HAVE_SYS_STAT_H  DOC "")
CHECK_INCLUDE_FILE( errno.h HAVE_ERRNO_H  DOC "")
CHECK_INCLUDE_FILE( netdb.h HAVE_NETDB_H  DOC "")
CHECK_INCLUDE_FILE( netinet/tcp.h HAVE_NETINET_TCP_H  DOC "")
CHECK_INCLUDE_FILE( netinet/in.h HAVE_NETINET_IN_H  DOC "")
CHECK_INCLUDE_FILE( arpa/inet.h HAVE_ARPA_INET_H  DOC "")
CHECK_INCLUDE_FILE( getopt.h HAVE_GETOPT_H  DOC "")
CHECK_INCLUDE_FILE( fcntl.h HAVE_FCNTL_H  DOC "")
CHECK_INCLUDE_FILE( stdint.h HAVE_STDINT_H  DOC "")
CHECK_INCLUDE_FILE( sys/timerfd.h HAVE_SYS_TIMERFD_H DOC "Whether we have the sys/timerfd header - only present on modern Linux distros, not on Control")
CHECK_INCLUDE_FILE( dirent.h HAVE_DIRENT_H DOC "Whether we have the dirent header, Linux and hopefully macOS X")
CHECK_INCLUDE_FILE( dlfcn.h HAVE_DLFCN_H DOC "Whether we have the dlfcn header for dynnamic libraries, Linux and hopefully macOS X")
CHECK_INCLUDE_FILE( TargetConditionals.h DOC "Whether we have the TargetConditionals header to detect which Apple platform we are on")
CHECK_INCLUDE_FILE_CXX( iostream HAVE_STD_IOSTREAM DOC "")
CHECK_INCLUDE_FILE_CXX( fstream HAVE_STD_FSTREAM DOC "")
CHECK_INCLUDE_FILE_CXX( iosfwd HAVE_STD_IOSFWD DOC "")
CHECK_INCLUDE_FILE_CXX( cassert HAVE_STD_CASSERT DOC "")
CHECK_INCLUDE_FILE_CXX( sstream HAVE_STD_SSTREAM DOC "")
CHECK_INCLUDE_FILE_CXX( stdexcept HAVE_STD_EXCEPT )

CONFIGURE_FILE( include/config/config.h.in ${PROJECT_BINARY_DIR}/include/config/config.h )
SUBDIRS( src/apps/DagBaseTest )

IF ( UNIX AND NOT APPLE )
CONFIGURE_FILE( etc/setup.sh.in ${PROJECT_BINARY_DIR}/etc/setup.sh )
ELSEIF ( APPLE )
CONFIGURE_FILE( etc/setup_macos.sh.in ${PROJECT_BINARY_DIR}/etc/setup.sh )
ELSEIF ( WIN32 )
CONFIGURE_FILE( etc/setup.bat.in ${PROJECT_BINARY_DIR}/etc/setup.bat )
ENDIF ( UNIX AND NOT APPLE )
CONFIGURE_FILE( etc/setup.bat.in ${PROJECT_BINARY_DIR}/etc/setup.bat )

INSTALL( TARGETS dagbase ARCHIVE DESTINATION lib RUNTIME DESTINATION bin LIBRARY DESTINATION lib )
IF ( UNIX )
INSTALL( FILES ${PROJECT_BINARY_DIR}/etc/setup.sh DESTINATION . )
ELSEIF ( WIN32 )
INSTALL( FILES ${PROJECT_BINARY_DIR}/etc/setup.bat DESTINATION . )
ENDIF( UNIX )
INSTALL(DIRECTORY data DESTINATION .)
