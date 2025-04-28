ADD_EXECUTABLE( DagBaseTest ../../test/TestUtils.cpp ConfigurationElementTest.cpp LuaTest.cpp
        VectorMapTest.cpp
        StreamTest.cpp
        SearchableTest.cpp
        AtomTest.cpp
        DynamicVisitorTest.cpp
        TestObject.h
        TestClass.h
        SnippetTest.cpp
        PropertyTest.cpp
        VariantTest.cpp)
IF (UNIX)
LIST(APPEND LUA_LIBRARIES dl)
ENDIF(UNIX)
TARGET_LINK_LIBRARIES( DagBaseTest ${LUA_LIBRARIES} dagbase;GTest::gtest_main;GTest::gtest;GTest::gmock)
SET_TARGET_PROPERTIES( DagBaseTest PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${CMAKE_INSTALL_PREFIX}/lib" )
INSTALL(TARGETS DagBaseTest RUNTIME DESTINATION bin)
