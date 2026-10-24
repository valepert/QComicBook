INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/src/FrameDetect
        ${CMAKE_SOURCE_DIR}/src/Job
	${CMAKE_BINARY_DIR}/src
	${CMAKE_BINARY_DIR}/src/FrameDetect
        ${CMAKE_BINARY_DIR}/src/Job
	${CMAKE_BINARY_DIR}
	${POPPLER_INCLUDE_DIRS}
)

SET(qcomicbook_moc_hdrs
    Debug/DebugController.h
    Debug/ContinuousPageViewDebug.h
    Debug/MemoryDebug.h
	FrameDetect/FrameCache.h
	FrameDetect/FrameDetectThread.h
	Archivers/ArchiversConfiguration.h
	BookmarkManager.h 
	ComicBookCfgDialog.h 
	ComicBookInfo.h
	ComicBookSettings.h 
	ComicMainWindow.h 
	View/FrameView.h
	View/PageViewBase.h 
	View/ContinuousPageView.h
	View/SimplePageView.h
	Sink/ImgSink.h
        Sink/ImgDirSink.h 
	Sink/ImgArchiveSink.h
	LoaderThreadBase.h 
	PageLoaderThread.h
	ThumbnailLoaderThread.h
	GoToPageWidget.h 
        PrinterThread.h
        PrintProgressDialog.h
        RecentFilesMenu.h
	StatusBar.h 
        SystemInfoDialog.h
	ThumbnailsWindow.h 
	ThumbnailsView.h
	ViewProperties.h
        Job/ImageTransformThread.h
)

file (GLOB_RECURSE qcomicbook_hdr *.h *.hpp)
file (GLOB_RECURSE qcomicbook_src *.cpp)
file (GLOB_RECURSE qcomicbook_ui *.ui)

SET(FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${qcomicbook_src}
	${qcomicbook_ui} ${qcomicbook_hdr} PARENT_SCOPE)

qt6_add_resources(qcomicbook_res ../data/qcomicbook.qrc)
qt6_wrap_cpp(qcomicbook_moc_src ${qcomicbook_moc_hdrs})
qt6_wrap_ui(qcomicbook_ui_src ${qcomicbook_ui})

IF(APPLE)
	# application icon, generated from the PNG at build time
	SET(qcomicbook_icns ${CMAKE_CURRENT_BINARY_DIR}/QComicBook.icns)
	ADD_CUSTOM_COMMAND(OUTPUT ${qcomicbook_icns}
		COMMAND sh ${CMAKE_SOURCE_DIR}/data/macos/make-icns.sh ${CMAKE_SOURCE_DIR}/data/qcomicbook.png ${qcomicbook_icns}
		DEPENDS ${CMAKE_SOURCE_DIR}/data/qcomicbook.png ${CMAKE_SOURCE_DIR}/data/macos/make-icns.sh)
	SET_SOURCE_FILES_PROPERTIES(${qcomicbook_icns} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ENDIF()

# MACOSX_BUNDLE builds QComicBook.app on macOS and is ignored elsewhere
ADD_EXECUTABLE(qcomicbook MACOSX_BUNDLE ${qcomicbook_src} ${qcomicbook_moc_src} ${qcomicbook_ui_src} ${qcomicbook_res} ${qcomicbook_icns})
ADD_DEPENDENCIES(qcomicbook translations)
TARGET_LINK_LIBRARIES(qcomicbook Qt6::Widgets Qt6::PrintSupport)
TARGET_LINK_LIBRARIES(qcomicbook ${POPPLER_LINK_LIBRARIES})

IF(APPLE)
	SET_TARGET_PROPERTIES(qcomicbook PROPERTIES
		OUTPUT_NAME QComicBook
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/data/macos/Info.plist.in
		MACOSX_BUNDLE_BUNDLE_NAME QComicBook
		MACOSX_BUNDLE_GUI_IDENTIFIER net.linux-projects.QComicBook
		MACOSX_BUNDLE_ICON_FILE QComicBook.icns
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
		MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
		MACOSX_BUNDLE_COPYRIGHT "(c) Pawel Stolowski 2005-2016, released under GNU GPL v2")
ENDIF()

INSTALL(TARGETS qcomicbook
	BUNDLE DESTINATION .
	RUNTIME DESTINATION bin)

