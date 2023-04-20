cmake_minimum_required(VERSION 3.16)

project(ContactsApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(android/)

qt_add_executable(appContactsApp
    main.cpp
    contactsmodel.cpp
    contactsmodel.h
    Contact.cpp
    android/src/com/test/ContactsActivity.java
    android/AndroidManifest.xml
)

qt_add_qml_module(appContactsApp
    URI ContactsApp
    VERSION 1.0
    QML_FILES Main.qml
)

set_target_properties(appContactsApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

target_link_libraries(appContactsApp
    PRIVATE Qt6::Quick
    PRIVATE Qt::CorePrivate
)

install(TARGETS appContactsApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})


target_include_directories(appContactsApp PRIVATE ${android})
