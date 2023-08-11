# A few CPack related variables
set(CPACK_PACKAGE_NAME "ntirpc" )
set(CPACK_PACKAGE_VERSION "${PACKNAME}" )
set(CPACK_PACKAGE_VENDOR "NFS-Ganesha Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Updated, multithreaded TiRPC library")

# CPack's debian stuff
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "mbenjamin@redhat.com")

set(CPACK_RPM_COMPONENT_INSTALL OFF)
set(CPACK_COMPONENTS_IGNORE_GROUPS "IGNORE")

# Tell CPack the kind of packages to be generated
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")

set(CPACK_SOURCE_IGNORE_FILES
  "/.git/;/.gitignore/;/.bzr/;~$;${CPACK_SOURCE_IGNORE_FILES}")

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
