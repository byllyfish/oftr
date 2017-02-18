# Install man pages.

install(FILES oftr.1 DESTINATION share/man/man1)
install(FILES oftr-jsonrpc.1 DESTINATION share/man/man1)
install(FILES oftr-schema.1 DESTINATION share/man/man1)

add_custom_target(manpages 
    COMMAND sh make_manpages.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
)
