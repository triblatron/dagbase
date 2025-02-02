ADD_EXECUTABLE( DagBaseTest ConfigurationElementTest.cpp LuaTest.cpp
        VectorMapTest.h)
IF (UNIX)
LIST(APPEND LUA_LIBRARIES dl)
ENDIF(UNIX)
TARGET_LINK_LIBRARIES( DagBaseTest ${LUA_LIBRARIES} dagbase;GTest::gtest_main;GTest::gmock)
INSTALL(TARGETS DagBaseTest RUNTIME DESTINATION bin)
