# Usage: cmake --install build --prefix <path>
# Example: sudo cmake --install build --prefix /
#          (installs headers to /usr/include, library to /usr/lib)

# GNUInstallDirs is included in CMakeLists.txt (needed for
# CMAKE_INSTALL_INCLUDEDIR in target_include_directories)
include(CMakePackageConfigHelpers)

# Export name: in-tree target stays mamba_lib, exported as mamba::mamba
set_target_properties(mamba_lib PROPERTIES EXPORT_NAME mamba)

# 1) Headers -> prefix/include/mamba/
install(DIRECTORY include/mamba/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mamba
)

# 2) Library + export set -> prefix/lib/
install(TARGETS mamba_lib EXPORT mambaTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT mambaTargets
    FILE mamba-targets.cmake
    NAMESPACE mamba::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mamba
)

# 3) find_package(mamba) support -> prefix/lib/cmake/mamba/
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mamba-config.cmake.in"
    "${CMAKE_BINARY_DIR}/mamba-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mamba
)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/mamba-config-version.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_BINARY_DIR}/mamba-config.cmake"
    "${CMAKE_BINARY_DIR}/mamba-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mamba
)

# 4) pkg-config support -> prefix/lib/pkgconfig/
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mamba.pc.in"
    "${CMAKE_BINARY_DIR}/mamba.pc"
    @ONLY
)

install(FILES "${CMAKE_BINARY_DIR}/mamba.pc"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

message(STATUS "")
message(STATUS "=== Installation targets (default prefix: ${CMAKE_INSTALL_PREFIX}) ===")
message(STATUS "  Headers:    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/mamba/")
message(STATUS "  Library:    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/libmamba_lib.a")
message(STATUS "  CMake:      ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/mamba/")
message(STATUS "  pkg-config: ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/pkgconfig/mamba.pc")
message(STATUS "")