SET(ANKI_SUB_DIRS script renderer scene ui event 
	input physics resource core misc gl collision math util)

SET(ANKI_LIBS "")

FOREACH(TMP ${ANKI_SUB_DIRS})
	ADD_SUBDIRECTORY(${TMP})
	SET(ANKI_LIBS ${ANKI_LIBS} anki${TMP})
ENDFOREACH()

ADD_LIBRARY(anki)

TARGET_LINK_LIBRARIES(anki ${ANKI_LIBS} BulletSoftBody BulletDynamics 
	BulletCollision LinearMath GLEWmx GLU GL jpeg SDL png python${PYTHON_VER}
	boost_system boost_python boost_filesystem boost_thread boost_regex 
	freetype)

SET_TARGET_PROPERTIES(anki PROPERTIES LINKER_LANGUAGE CXX)

# Install
#
INSTALL(TARGETS anki DESTINATION ${LIB_INSTALL_DIR})

INSTALL(DIRECTORY ${ANKI_PROJECT_SOURCE_DIR}/anki DESTINATION "${INCLUDE_INSTALL_DIR}" FILES_MATCHING PATTERN "*.h" PATTERN .svn EXCLUDE)