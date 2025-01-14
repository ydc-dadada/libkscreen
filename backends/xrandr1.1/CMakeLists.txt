find_package(XCB REQUIRED COMPONENTS XCB RANDR)

include_directories(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_BUILD_DIR}
                    ${QT_INCLUDES}
)


add_library(KSC_XRandR11 MODULE)
target_sources(KSC_XRandR11 PRIVATE
    xrandr11.cpp
    ../xcbeventlistener.cpp
    ../xcbwrapper.cpp
)

set_target_properties(KSC_XRandR11 PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/kf5/kscreen")
set_target_properties(KSC_XRandR11 PROPERTIES PREFIX "")
target_link_libraries(KSC_XRandR11 Qt::Core
                                   Qt::Gui
                                   ${XCB_LIBRARIES}
                                   KF5::Screen
)
if (QT_MAJOR_VERSION EQUAL "5")
    target_link_libraries(KSC_XRandR11 Qt::X11Extras)
else()
    target_link_libraries(KSC_XRandR11 Qt::GuiPrivate)
endif()

install(TARGETS KSC_XRandR11 DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf5/kscreen/)
