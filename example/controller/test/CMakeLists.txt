
# Plaintext (no TLS)
add_test(NAME controller COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent>)
set_tests_properties(controller PROPERTIES TIMEOUT 10 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8651")

# Mutual TLS.
add_test(NAME controller-tls1 COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent>)
set_tests_properties(controller-tls1 PROPERTIES TIMEOUT 10 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8652;CONTROLLER_CERT=${CMAKE_CURRENT_SOURCE_DIR}/ctl-cert.pem;CONTROLLER_CACERT=${CMAKE_CURRENT_SOURCE_DIR}/sw-ca.pem;AGENT_CERT=${CMAKE_CURRENT_SOURCE_DIR}/sw-cert.pem;AGENT_CACERT=${CMAKE_CURRENT_SOURCE_DIR}/ctl-ca.pem")

# TLS. Controller does not verify peer cert.
add_test(NAME controller-tls2 COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent>)
set_tests_properties(controller-tls2 PROPERTIES TIMEOUT 10 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8653;CONTROLLER_CERT=${CMAKE_CURRENT_SOURCE_DIR}/ctl-cert.pem;AGENT_CERT=${CMAKE_CURRENT_SOURCE_DIR}/sw-cert.pem;AGENT_CACERT=${CMAKE_CURRENT_SOURCE_DIR}/ctl-ca.pem")

# TLS. Agent does not verify peer cert.
add_test(NAME controller-tls3 COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/run.sh $<TARGET_FILE:testagent>)
set_tests_properties(controller-tls3 PROPERTIES TIMEOUT 10 ENVIRONMENT "LIBOFP_DEFAULT_PORT=8654;CONTROLLER_CERT=${CMAKE_CURRENT_SOURCE_DIR}/ctl-cert.pem;CONTROLLER_CACERT=${CMAKE_CURRENT_SOURCE_DIR}/sw-ca.pem;AGENT_CERT=${CMAKE_CURRENT_SOURCE_DIR}/sw-cert.pem")
