PROJECT (FILECENTER)
#######test dir###################
SET(TESTCONFIG_LIST testconfig.cpp )
#ADD_EXECUTABLE(testconfig.out ${TESTCONFIG_LIST})

SET(TESTINOTIFY_LIST testinotify.cpp ../src/inotify.cpp ../src/inotifywatch.cpp ../src/inotifyevent.cpp ../src/inotifylock.cpp)
#ADD_EXECUTABLE(testinotify.out ${TESTINOTIFY_LIST})
#TARGET_LINK_LIBRARIES (testinotify.out pthread)

##test############################
SET(TEST_LIST test.cpp)
ADD_EXECUTABLE(test.out ${TEST_LIST})
TARGET_LINK_LIBRARIES (test.out pthread)

#SET(TESTINOTIFY_LOCK_LIST testinotifylock.cpp ../src/inotifylock.cpp)
#ADD_EXECUTABLE(testinotifylock.out ${TESTINOTIFY_LOCK_LIST})
#TARGET_LINK_LIBRARIES (testinotifylock.out pthread)

#SET(TESTINOTIFY_XMLPARSER_LIST testxmlparser.cpp ../src/xmlParser.cpp)
#ADD_EXECUTABLE(testinotifyxmlparser.out ${TESTINOTIFY_XMLPARSER_LIST})