# ElizaOS C++ Test Suite
# Comprehensive unit, integration, and E2E tests

# ============================================================================
# Main Test Executable
# ============================================================================

add_executable(elizaos_tests
    # Core Tests
    src/test_core.cpp
    src/test_agentloop.cpp
    src/test_cognitive_primitives.cpp

    # Infrastructure Tests
    src/test_agentlogger.cpp
    src/test_agentcomms.cpp
    src/test_agentmemory.cpp
    src/test_attention_allocation.cpp
    src/test_agentaction.cpp
    src/test_agentagenda.cpp
    src/test_agentshell.cpp
    src/test_agentbrowser.cpp

    # Application Module Tests
    src/test_characters.cpp
    src/test_characterfile.cpp
    src/test_eliza.cpp
    src/test_elizas_list.cpp
    src/test_elizas_world.cpp
    src/test_knowledge.cpp
    src/test_easycompletion.cpp
    src/test_spartan.cpp
    src/test_registry.cpp
    src/test_the_org.cpp
    src/test_trust_scoreboard.cpp

    # Multimedia Tests
    src/test_ljspeechtools.cpp
    src/test_livevideochat.cpp

    # Advanced Module Tests
    src/test_evolutionary.cpp
    src/test_ontogenesis.cpp
    src/test_mcp_gateway.cpp
    src/test_sweagent.cpp
    src/test_vercel_api.cpp

    # Automation Tests
    src/test_stage6_automation.cpp

    # Integration Tests
    src/test_integration.cpp

    # Module-specific Tests
    test_awesome_eliza.cpp
    ../autofun_idl/tests/test_autofun_idl.cpp
    ../eliza_3d_hyperfy_starter/test_eliza_3d_hyperfy_starter.cpp

    # Temporarily disabled tests
    # src/test_embodiment.cpp  # TEMPORARILY DISABLED - linking errors with embodiment library
)

target_include_directories(elizaos_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos_tests
    # Core Libraries
    elizaos-core
    elizaos-agentloop
    elizaos-agentmemory

    # Infrastructure Libraries
    elizaos-agentlogger
    elizaos-agentcomms
    elizaos-agentshell
    elizaos-agentbrowser
    elizaos-agentaction
    elizaos-agentagenda

    # Application Libraries
    elizaos-characters
    elizaos-characterfile
    elizaos-eliza
    elizaos-elizas_list
    elizaos-elizas_world
    elizaos-knowledge
    elizaos-easycompletion
    elizaos-spartan
    elizaos-registry
    elizaos-the_org
    elizaos-trust_scoreboard

    # Multimedia Libraries
    elizaos-ljspeechtools
    elizaos-livevideochat

    # Advanced Libraries
    elizaos-evolutionary
    elizaos-ontogenesis
    elizaos-mcp_gateway
    elizaos-sweagent
    elizaos-vercel_api

    # Automation Libraries
    elizaos-plugins_automation
    elizaos-discord_summarizer
    elizaos-discrub_ext

    # Other Libraries
    elizaos-awesome_eliza
    elizaos-eliza_3d_hyperfy_starter
    elizaos-autofun_idl

    # Temporarily disabled
    # elizaos-embodiment  # TEMPORARILY DISABLED - library not built

    # Test Framework
    gtest_main
    gmock_main
    Threads::Threads
)

# Add tests to CTest
include(GoogleTest)
gtest_discover_tests(elizaos_tests)

# ============================================================================
# Separate Integration Test Executable (Optional)
# ============================================================================

add_executable(elizaos_integration_tests
    src/test_integration.cpp
)

target_include_directories(elizaos_integration_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(elizaos_integration_tests
    elizaos-core
    elizaos-agentloop
    elizaos-agentmemory
    elizaos-agentlogger
    elizaos-agentcomms
    elizaos-knowledge
    elizaos-characters
    gtest_main
    gmock_main
    Threads::Threads
)

gtest_discover_tests(elizaos_integration_tests)

# ============================================================================
# Test Categories (for selective testing)
# ============================================================================

# Define test labels for categorization
set_tests_properties(
    elizaos_tests PROPERTIES
    LABELS "unit;all"
)

set_tests_properties(
    elizaos_integration_tests PROPERTIES
    LABELS "integration;all"
)

# ============================================================================
# Coverage Target (for local development)
# ============================================================================

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND CMAKE_COMPILER_IS_GNUCXX)
    # Add coverage flags
    target_compile_options(elizaos_tests PRIVATE --coverage)
    target_link_options(elizaos_tests PRIVATE --coverage)

    target_compile_options(elizaos_integration_tests PRIVATE --coverage)
    target_link_options(elizaos_integration_tests PRIVATE --coverage)

    # Add custom target for coverage report
    add_custom_target(coverage
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
        COMMAND lcov --remove coverage.info '/usr/*' '*/googletest/*' '*/nlohmann/*' --output-file coverage.info
        COMMAND genhtml coverage.info --output-directory coverage-report
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report..."
    )
endif()

# ============================================================================
# Test Output Configuration
# ============================================================================

# Enable verbose test output
set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

# Configure test timeout (5 minutes per test)
set(CTEST_TEST_TIMEOUT 300)
