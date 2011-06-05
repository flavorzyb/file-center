PROJECT (FILECENTER)
SET(INOTIFY_LIST inotify.cpp inotifyevent.cpp inotifylock.cpp inotifywatch.cpp inotifyconfig.cpp common.cpp)
SET(XMLPARSER_LIST xmlParser.cpp)
SET(FILECENTER_LIST main.cpp ${INOTIFY_LIST} ${XMLPARSER_LIST})

ADD_EXECUTABLE(filecenter.out ${FILECENTER_LIST})
TARGET_LINK_LIBRARIES (filecenter.out pthread)
