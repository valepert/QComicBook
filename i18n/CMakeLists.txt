file (GLOB translation_src *.ts)

qt6_add_translation(qcomicbook_qm ${translation_src})
SET_PROPERTY(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY CLEAN_NO_CUSTOM ON)
ADD_CUSTOM_TARGET(translations DEPENDS ${qcomicbook_qm})
INSTALL(FILES ${qcomicbook_qm} DESTINATION share/${PACKAGE}/i18n)
