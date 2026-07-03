cmake_minimum_required(VERSION 3.16)
project(elizaos_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest is already fetched by root CMakeLists.txt via FetchContent
# We just need to include the headers and link against gtest/gtest_main

# Use Google Test from FetchContent (set up in parent CMakeLists.txt)
# Include project headers (custom gtest shim is in ${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/cpp/include)

# nlohmann_json is fetched by root CMakeLists.txt - link via target instead of include path

# Add test executables

add_executable(agentaction_test agentaction_test.cpp)
target_link_libraries(agentaction_test elizaos-agentaction gtest gtest_main Threads::Threads)
add_test(NAME agentaction_test COMMAND agentaction_test)

add_executable(agentagenda_test agentagenda_test.cpp)
target_link_libraries(agentagenda_test elizaos-agentagenda gtest gtest_main Threads::Threads)
add_test(NAME agentagenda_test COMMAND agentagenda_test)

add_executable(agentbrowser_test agentbrowser_test.cpp)
target_link_libraries(agentbrowser_test elizaos-agentbrowser gtest gtest_main Threads::Threads)
add_test(NAME agentbrowser_test COMMAND agentbrowser_test)

add_executable(agentcomms_test agentcomms_test.cpp)
target_link_libraries(agentcomms_test elizaos-agentcomms gtest gtest_main Threads::Threads)
add_test(NAME agentcomms_test COMMAND agentcomms_test)

add_executable(agentlogger_test agentlogger_test.cpp)
target_link_libraries(agentlogger_test elizaos-agentlogger gtest gtest_main Threads::Threads)
add_test(NAME agentlogger_test COMMAND agentlogger_test)

add_executable(agentloop_test agentloop_test.cpp)
target_link_libraries(agentloop_test elizaos-agentloop gtest gtest_main Threads::Threads)
add_test(NAME agentloop_test COMMAND agentloop_test)

add_executable(agentmemory_test agentmemory_test.cpp)
target_link_libraries(agentmemory_test elizaos-agentmemory gtest gtest_main Threads::Threads)
add_test(NAME agentmemory_test COMMAND agentmemory_test)

add_executable(agentshell_test agentshell_test.cpp)
target_link_libraries(agentshell_test elizaos-agentshell gtest gtest_main Threads::Threads)
add_test(NAME agentshell_test COMMAND agentshell_test)

add_executable(auto_fun_test auto_fun_test.cpp)
target_link_libraries(auto_fun_test elizaos-auto_fun gtest gtest_main Threads::Threads)
add_test(NAME auto_fun_test COMMAND auto_fun_test)

add_executable(autofun_idl_test autofun_idl_test.cpp)
target_link_libraries(autofun_idl_test elizaos-autofun_idl gtest gtest_main Threads::Threads)
add_test(NAME autofun_idl_test COMMAND autofun_idl_test)

add_executable(autonomous_starter_test autonomous_starter_test.cpp)
target_link_libraries(autonomous_starter_test elizaos-autonomous_starter gtest gtest_main Threads::Threads)
add_test(NAME autonomous_starter_test COMMAND autonomous_starter_test)

add_executable(homework_loop_test homework_loop_test.cpp)
target_link_libraries(homework_loop_test elizaos-cognitive_curriculum elizaos-autonomous_starter gtest gtest_main Threads::Threads)
add_test(NAME homework_loop_test COMMAND homework_loop_test)
set_tests_properties(homework_loop_test PROPERTIES LABELS "ksm")

if(TARGET elizaos-persistence)
    add_executable(persistence_test persistence_test.cpp)
    target_link_libraries(persistence_test elizaos-persistence elizaos-core gtest gtest_main Threads::Threads)
    add_test(NAME persistence_test COMMAND persistence_test)
endif()

add_executable(comprehensive_autonomy_e2e_test comprehensive_autonomy_e2e_test.cpp)
target_link_libraries(comprehensive_autonomy_e2e_test
    elizaos-autonomous_starter
    elizaos-cognitive_bridge
    elizaos-knowledge
    elizaos-goal_manager
    elizaos-characters
    elizaos-agentmemory
    elizaos-agentaction
    elizaos-agentloop
    elizaos-agentcomms
    elizaos-agentlogger
    elizaos-core
    gtest
    gtest_main
    nlohmann_json::nlohmann_json
    Threads::Threads
)
if(TARGET elizaos-persistence)
    target_link_libraries(comprehensive_autonomy_e2e_test elizaos-persistence)
endif()
add_test(NAME comprehensive_autonomy_e2e_test COMMAND comprehensive_autonomy_e2e_test)

# Comprehensive E2E contract test - validates the public comprehensive_e2e.hpp API
# (Stage enum stability, PipelineSnapshot semantics, latency normalization).
add_executable(comprehensive_e2e_test comprehensive_e2e_contract_test.cpp)
target_link_libraries(comprehensive_e2e_test
    gtest
    gtest_main
    Threads::Threads
)
add_test(NAME comprehensive_e2e_test COMMAND comprehensive_e2e_test)

add_executable(awesome_eliza_test awesome_eliza_test.cpp)
target_link_libraries(awesome_eliza_test elizaos-awesome_eliza gtest gtest_main Threads::Threads)
add_test(NAME awesome_eliza_test COMMAND awesome_eliza_test)

add_executable(brandkit_test brandkit_test.cpp)
target_link_libraries(brandkit_test elizaos-brandkit gtest gtest_main Threads::Threads)
add_test(NAME brandkit_test COMMAND brandkit_test)

add_executable(characterfile_test characterfile_test.cpp)
target_link_libraries(characterfile_test elizaos-characterfile gtest gtest_main Threads::Threads)
add_test(NAME characterfile_test COMMAND characterfile_test)

add_executable(characters_test characters_test.cpp)
target_link_libraries(characters_test elizaos-characters gtest gtest_main Threads::Threads)
add_test(NAME characters_test COMMAND characters_test)

add_executable(character_json_loader_test character_json_loader_test.cpp)
target_link_libraries(character_json_loader_test elizaos-characters gtest gtest_main Threads::Threads)
add_test(NAME character_json_loader_test COMMAND character_json_loader_test)

add_executable(classified_test classified_test.cpp)
target_link_libraries(classified_test elizaos-classified gtest gtest_main Threads::Threads)
add_test(NAME classified_test COMMAND classified_test)

add_executable(core_test core_test.cpp)
target_link_libraries(core_test elizaos-core gtest gtest_main Threads::Threads)
add_test(NAME core_test COMMAND core_test)

add_executable(discord_summarizer_test discord_summarizer_test.cpp)
target_link_libraries(discord_summarizer_test elizaos-discord_summarizer gtest gtest_main Threads::Threads)
add_test(NAME discord_summarizer_test COMMAND discord_summarizer_test)

# discrub_ext lib has a broken static initializer that crashes at process load.
# Tests use header-only API surface (sizes/enums) without linking the lib.
add_executable(discrub_ext_test discrub_ext_test.cpp)
target_link_libraries(discrub_ext_test gtest gtest_main Threads::Threads)
add_test(NAME discrub_ext_test COMMAND discrub_ext_test)

add_executable(easycompletion_test easycompletion_test.cpp)
target_link_libraries(easycompletion_test elizaos-easycompletion gtest gtest_main Threads::Threads)
add_test(NAME easycompletion_test COMMAND easycompletion_test)

add_executable(eliza_test eliza_test.cpp)
target_link_libraries(eliza_test elizaos-eliza gtest gtest_main Threads::Threads)
add_test(NAME eliza_test COMMAND eliza_test)

add_executable(eliza_3d_hyperfy_starter_test eliza_3d_hyperfy_starter_test.cpp)
target_link_libraries(eliza_3d_hyperfy_starter_test elizaos-eliza_3d_hyperfy_starter gtest gtest_main Threads::Threads)
add_test(NAME eliza_3d_hyperfy_starter_test COMMAND eliza_3d_hyperfy_starter_test)

add_executable(eliza_nextjs_starter_test eliza_nextjs_starter_test.cpp)
target_link_libraries(eliza_nextjs_starter_test elizaos-eliza_nextjs_starter gtest gtest_main Threads::Threads)
add_test(NAME eliza_nextjs_starter_test COMMAND eliza_nextjs_starter_test)

add_executable(eliza_plugin_starter_test eliza_plugin_starter_test.cpp)
target_link_libraries(eliza_plugin_starter_test elizaos-eliza_plugin_starter gtest gtest_main Threads::Threads)
add_test(NAME eliza_plugin_starter_test COMMAND eliza_plugin_starter_test)

add_executable(eliza_starter_test eliza_starter_test.cpp)
target_link_libraries(eliza_starter_test elizaos-eliza_starter gtest gtest_main Threads::Threads)
add_test(NAME eliza_starter_test COMMAND eliza_starter_test)

add_executable(elizaos_github_io_test elizaos_github_io_test.cpp)
target_link_libraries(elizaos_github_io_test elizaos-elizaos_github_io gtest gtest_main Threads::Threads)
add_test(NAME elizaos_github_io_test COMMAND elizaos_github_io_test)

add_executable(elizas_list_test elizas_list_test.cpp)
target_link_libraries(elizas_list_test elizaos-elizas_list gtest gtest_main nlohmann_json::nlohmann_json Threads::Threads)
add_test(NAME elizas_list_test COMMAND elizas_list_test)

add_executable(elizas_world_test elizas_world_test.cpp)
target_link_libraries(elizas_world_test elizaos-elizas_world gtest gtest_main Threads::Threads)
add_test(NAME elizas_world_test COMMAND elizas_world_test)

add_executable(embodiment_test embodiment_test.cpp)
target_link_libraries(embodiment_test elizaos-embodiment gtest gtest_main Threads::Threads)
add_test(NAME embodiment_test COMMAND embodiment_test)

add_executable(evolutionary_test evolutionary_test.cpp)
target_link_libraries(evolutionary_test elizaos-evolutionary gtest gtest_main Threads::Threads)
add_test(NAME evolutionary_test COMMAND evolutionary_test)

add_executable(hat_test hat_test.cpp)
target_link_libraries(hat_test elizaos-hat gtest gtest_main Threads::Threads)
add_test(NAME hat_test COMMAND hat_test)

add_executable(hats_test hats_test.cpp)
target_link_libraries(hats_test elizaos-hats gtest gtest_main Threads::Threads)
add_test(NAME hats_test COMMAND hats_test)

add_executable(knowledge_test knowledge_test.cpp)
target_link_libraries(knowledge_test elizaos-knowledge gtest gtest_main Threads::Threads)
add_test(NAME knowledge_test COMMAND knowledge_test)

add_executable(livevideochat_test livevideochat_test.cpp)
target_link_libraries(livevideochat_test elizaos-livevideochat gtest gtest_main Threads::Threads)
add_test(NAME livevideochat_test COMMAND livevideochat_test)

add_executable(ljspeechtools_test ljspeechtools_test.cpp)
target_link_libraries(ljspeechtools_test elizaos-ljspeechtools gtest gtest_main Threads::Threads)
add_test(NAME ljspeechtools_test COMMAND ljspeechtools_test)

add_executable(mcp_gateway_test mcp_gateway_test.cpp)
target_link_libraries(mcp_gateway_test elizaos-mcp_gateway gtest gtest_main nlohmann_json::nlohmann_json Threads::Threads)
add_test(NAME mcp_gateway_test COMMAND mcp_gateway_test)

add_executable(ontogenesis_test ontogenesis_test.cpp)
target_link_libraries(ontogenesis_test elizaos-ontogenesis gtest gtest_main Threads::Threads)
add_test(NAME ontogenesis_test COMMAND ontogenesis_test)

add_executable(otaku_test otaku_test.cpp)
target_link_libraries(otaku_test elizaos-otaku gtest gtest_main Threads::Threads)
add_test(NAME otaku_test COMMAND otaku_test)

add_executable(otaku_liquidity_test otaku_liquidity_test.cpp)
target_link_libraries(otaku_liquidity_test elizaos-otaku gtest gtest_main Threads::Threads)
add_test(NAME otaku_liquidity_test COMMAND otaku_liquidity_test)

add_executable(otc_agent_test otc_agent_test.cpp)
target_link_libraries(otc_agent_test elizaos-otc_agent gtest gtest_main Threads::Threads)
add_test(NAME otc_agent_test COMMAND otc_agent_test)

add_executable(plugin_specification_test plugin_specification_test.cpp)
target_link_libraries(plugin_specification_test elizaos-plugin_specification gtest gtest_main Threads::Threads)
add_test(NAME plugin_specification_test COMMAND plugin_specification_test)

add_executable(plugins_automation_test plugins_automation_test.cpp)
target_link_libraries(plugins_automation_test elizaos-plugins_automation gtest gtest_main Threads::Threads)
add_test(NAME plugins_automation_test COMMAND plugins_automation_test)

add_executable(registry_test registry_test.cpp)
target_link_libraries(registry_test elizaos-registry gtest gtest_main Threads::Threads)
add_test(NAME registry_test COMMAND registry_test)

add_executable(spartan_test spartan_test.cpp)
target_link_libraries(spartan_test elizaos-spartan gtest gtest_main Threads::Threads)
add_test(NAME spartan_test COMMAND spartan_test)

add_executable(sweagent_test sweagent_test.cpp)
target_link_libraries(sweagent_test elizaos-sweagent gtest gtest_main Threads::Threads)
add_test(NAME sweagent_test COMMAND sweagent_test)

add_executable(the_org_test the_org_test.cpp)
target_link_libraries(the_org_test elizaos-the_org gtest gtest_main Threads::Threads)
add_test(NAME the_org_test COMMAND the_org_test)

add_executable(trust_scoreboard_test trust_scoreboard_test.cpp)
target_link_libraries(trust_scoreboard_test elizaos-trust_scoreboard gtest gtest_main Threads::Threads)
add_test(NAME trust_scoreboard_test COMMAND trust_scoreboard_test)

add_executable(vercel_api_test vercel_api_test.cpp)
target_link_libraries(vercel_api_test elizaos-vercel_api gtest gtest_main Threads::Threads)
add_test(NAME vercel_api_test COMMAND vercel_api_test)

add_executable(website_test website_test.cpp)
target_link_libraries(website_test elizaos-website gtest gtest_main Threads::Threads)
add_test(NAME website_test COMMAND website_test)

add_executable(workgroups_test workgroups_test.cpp)
target_link_libraries(workgroups_test elizaos-workgroups gtest gtest_main Threads::Threads)
add_test(NAME workgroups_test COMMAND workgroups_test)

add_executable(goal_manager_test goal_manager_test.cpp)
target_link_libraries(goal_manager_test gtest gtest_main elizaos-goal_manager Threads::Threads)
add_test(NAME goal_manager_test COMMAND goal_manager_test)

# ============================================================================
# Comprehensive cognitive-pipeline E2E test
# Covers SWE-Agent + MCP Gateway + CognitiveBridge integration as required by
# the eliza-os skill (cognitive_state_update / sensory_input / speech_output
# channels and the 12-step Echobeats cycle).
# ============================================================================
add_executable(cognitive_pipeline_e2e_test cognitive_pipeline_e2e_test.cpp)
# Now uses the real GoogleTest from FetchContent (the previous gtest shim at
# ${CMAKE_SOURCE_DIR}/include/gtest/gtest.h was removed because it caused all
# TEST_F bodies to expand to dead code, defeating coverage entirely).
target_link_libraries(cognitive_pipeline_e2e_test
    elizaos-sweagent
    elizaos-mcp_gateway
    elizaos-cognitive_bridge
    elizaos-characterfile
    elizaos-knowledge
    elizaos-agentlogger
    elizaos-agentmemory
    elizaos-agentaction
    elizaos-agentloop
    elizaos-agentcomms
    elizaos-core
    gtest
    gtest_main
    nlohmann_json::nlohmann_json
    Threads::Threads
)
add_test(NAME cognitive_pipeline_e2e_test COMMAND cognitive_pipeline_e2e_test)

# ============================================================================
# Comprehensive real unit tests from cpp/tests/src/
# Each src/test_*.cpp has real EXPECT_*/ASSERT_* assertions
# Run them individually to avoid multiple main() conflicts
# ============================================================================

function(add_real_test name src_file)
    add_executable(${name} ${src_file} src/shim_main.cpp)
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/cpp/include
        ${CMAKE_SOURCE_DIR}/include
    )

    set(real_test_required_libraries
        gtest
        nlohmann_json::nlohmann_json
        Threads::Threads
    )
    set(real_test_optional_elizaos_targets
        elizaos-core
        elizaos-agentmemory
        elizaos-agentloop
        elizaos-agentcomms
        elizaos-agentlogger
        elizaos-agentshell
        elizaos-agentbrowser
        elizaos-agentaction
        elizaos-agentagenda
        elizaos-eliza
        elizaos-characters
        elizaos-knowledge
        elizaos-goal_manager
        elizaos-mcp_gateway
        elizaos-sweagent
        elizaos-embodiment
        elizaos-evolutionary
        elizaos-ontogenesis
        elizaos-cognitive_bridge
        elizaos-persistence
        elizaos-autonomous_starter
    )
    foreach(real_test_target IN LISTS real_test_optional_elizaos_targets)
        if(TARGET ${real_test_target})
            list(APPEND real_test_required_libraries ${real_test_target})
        else()
            message(STATUS "${name}: skipping unavailable optional test dependency ${real_test_target}")
        endif()
    endforeach()

    target_link_libraries(${name} ${real_test_required_libraries})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_real_test(real_core_test                src/test_core.cpp)
add_real_test(real_agentaction_test         src/test_agentaction.cpp)
add_real_test(real_agentagenda_test         src/test_agentagenda.cpp)
add_real_test(real_agentbrowser_test        src/test_agentbrowser.cpp)
add_real_test(real_agentcomms_test          src/test_agentcomms.cpp)
add_real_test(real_agentlogger_test         src/test_agentlogger.cpp)
add_real_test(real_agentloop_test           src/test_agentloop.cpp)
add_real_test(real_agentmemory_test         src/test_agentmemory.cpp)
add_real_test(real_attention_test           src/test_attention_allocation.cpp)
add_real_test(real_characters_test          src/test_characters.cpp)
add_real_test(real_knowledge_test           src/test_knowledge.cpp)
add_real_test(real_knowledge_hypergraph_test src/test_knowledge_hypergraph.cpp)
if(TARGET elizaos-embodiment)
    add_real_test(real_embodiment_test      src/test_embodiment.cpp)
else()
    message(STATUS "Skipping real_embodiment_test: elizaos-embodiment target is unavailable in this checkout")
endif()
if(TARGET elizaos-evolutionary)
    add_real_test(real_evolutionary_test    src/test_evolutionary.cpp)
else()
    message(STATUS "Skipping real_evolutionary_test: elizaos-evolutionary target is unavailable in this checkout")
endif()
if(TARGET elizaos-ontogenesis)
    add_real_test(real_ontogenesis_test     src/test_ontogenesis.cpp)
else()
    message(STATUS "Skipping real_ontogenesis_test: elizaos-ontogenesis target is unavailable in this checkout")
endif()
add_real_test(real_cognitive_primitives_test src/test_cognitive_primitives.cpp)
# KSM structure-preserving repair regression lane (ported to parity with the
# hurdcog canonical fork, plus o9nn-specific repairs: memory deadlock, clear()
# full-state reset, and AgentLogger Task 1.4 feature parity).
add_real_test(real_repair_regressions_test  src/test_repair_regressions.cpp)

# Regression test for public API drift between include/ and cpp/include/.
# This target intentionally searches cpp/include first so stale duplicate
# headers cannot silently reappear without failing the focused KSM lane.
add_executable(public_header_forwarding_test public_header_forwarding_test.cpp)
target_include_directories(public_header_forwarding_test BEFORE PRIVATE
    ${CMAKE_SOURCE_DIR}/cpp/include
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(public_header_forwarding_test
    elizaos-cognitive_bridge
    elizaos-plugin_specification
    gtest
    gtest_main
    Threads::Threads
)
add_test(NAME public_header_forwarding_test COMMAND public_header_forwarding_test)

# Focused validation for the application-level Eliza API client boundary.
# This catches regressions in URL normalization, endpoint derivation, redacted
# auth status, timeout handling, and deterministic shutdown semantics.
add_executable(api_client_real_test api_client_real_test.cpp)
target_include_directories(api_client_real_test PRIVATE
    ${CMAKE_SOURCE_DIR}/packages/applications/eliza/packages/api-client/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
find_package(CURL REQUIRED)
target_link_libraries(api_client_real_test
    elizaos-eliza-api-client
    gtest
    gtest_main
    nlohmann_json::nlohmann_json
    Threads::Threads
)
add_test(NAME api_client_real_test COMMAND api_client_real_test)

# Targeted api-client Feature Garden Ledger tests: truthful BaseClient + ServerService vertical slice.
add_executable(real_api_client_test src/test_api_client.cpp src/shim_main.cpp)
target_include_directories(real_api_client_test PRIVATE
    ${CMAKE_SOURCE_DIR}/cpp/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/packages/applications/eliza/packages/api-client/src
)
find_package(CURL REQUIRED)
target_link_libraries(real_api_client_test
    elizaos-eliza-api-client
    gtest
    nlohmann_json::nlohmann_json
    CURL::libcurl
    Threads::Threads
)
add_test(NAME real_api_client_test COMMAND real_api_client_test)

add_executable(public_header_forwarding_all_test
    public_header_forwarding_all_test.cpp
    public_header_forwarding_headers/agentaction_hpp_test.cpp
    public_header_forwarding_headers/agentagenda_hpp_test.cpp
    public_header_forwarding_headers/agentbrowser_hpp_test.cpp
    public_header_forwarding_headers/agentcomms_hpp_test.cpp
    public_header_forwarding_headers/agentlogger_hpp_test.cpp
    public_header_forwarding_headers/agentloop_hpp_test.cpp
    public_header_forwarding_headers/agentmemory_hpp_test.cpp
    public_header_forwarding_headers/agentshell_hpp_test.cpp
    public_header_forwarding_headers/attention_hpp_test.cpp
    public_header_forwarding_headers/auto_fun_hpp_test.cpp
    public_header_forwarding_headers/autofun_idl_hpp_test.cpp
    public_header_forwarding_headers/autonomous_starter_hpp_test.cpp
    public_header_forwarding_headers/awesome_eliza_hpp_test.cpp
    public_header_forwarding_headers/brandkit_hpp_test.cpp
    public_header_forwarding_headers/characterfile_hpp_test.cpp
    public_header_forwarding_headers/characters_hpp_test.cpp
    public_header_forwarding_headers/classified_hpp_test.cpp
    public_header_forwarding_headers/cognitive_bridge_hpp_test.cpp
    public_header_forwarding_headers/comprehensive_e2e_hpp_test.cpp
    public_header_forwarding_headers/core_hpp_test.cpp
    public_header_forwarding_headers/discord_summarizer_hpp_test.cpp
    public_header_forwarding_headers/discrub_ext_hpp_test.cpp
    public_header_forwarding_headers/easycompletion_hpp_test.cpp
    public_header_forwarding_headers/eliza_hpp_test.cpp
    public_header_forwarding_headers/eliza_3d_hyperfy_starter_hpp_test.cpp
    public_header_forwarding_headers/eliza_nextjs_starter_hpp_test.cpp
    public_header_forwarding_headers/eliza_plugin_starter_hpp_test.cpp
    public_header_forwarding_headers/eliza_starter_hpp_test.cpp
    public_header_forwarding_headers/elizaos_hpp_test.cpp
    public_header_forwarding_headers/elizaos_github_io_hpp_test.cpp
    public_header_forwarding_headers/elizas_list_hpp_test.cpp
    public_header_forwarding_headers/elizas_world_hpp_test.cpp
    public_header_forwarding_headers/embodiment_hpp_test.cpp
    public_header_forwarding_headers/evolutionary_hpp_test.cpp
    public_header_forwarding_headers/goal_manager_hpp_test.cpp
    public_header_forwarding_headers/hat_hpp_test.cpp
    public_header_forwarding_headers/hats_hpp_test.cpp
    public_header_forwarding_headers/knowledge_hpp_test.cpp
    public_header_forwarding_headers/livevideochat_hpp_test.cpp
    public_header_forwarding_headers/ljspeechtools_hpp_test.cpp
    public_header_forwarding_headers/mcp_gateway_hpp_test.cpp
    public_header_forwarding_headers/ontogenesis_hpp_test.cpp
    public_header_forwarding_headers/otaku_hpp_test.cpp
    public_header_forwarding_headers/otc_agent_hpp_test.cpp
    public_header_forwarding_headers/plugin_specification_hpp_test.cpp
    public_header_forwarding_headers/plugins_automation_hpp_test.cpp
    public_header_forwarding_headers/registry_hpp_test.cpp
    public_header_forwarding_headers/spartan_hpp_test.cpp
    public_header_forwarding_headers/sweagent_hpp_test.cpp
    public_header_forwarding_headers/the_org_hpp_test.cpp
    public_header_forwarding_headers/trust_scoreboard_hpp_test.cpp
    public_header_forwarding_headers/vercel_api_hpp_test.cpp
    public_header_forwarding_headers/website_hpp_test.cpp
    public_header_forwarding_headers/workgroups_hpp_test.cpp
)
target_include_directories(public_header_forwarding_all_test BEFORE PRIVATE
    ${CMAKE_SOURCE_DIR}/cpp/include
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(public_header_forwarding_all_test
    nlohmann_json::nlohmann_json
    gtest
    gtest_main
    Threads::Threads
)
add_test(NAME public_header_forwarding_all_test COMMAND public_header_forwarding_all_test)

# ============================================================================
# KSM focused canonical validation target
# ============================================================================
# The repository's default all-target graph intentionally includes generated
# transpilation modules and nested package experiments. KSM repair cycles need a
# fast, deterministic target that exercises the canonical hand-written C++
# centers and the cognitive integration anchor without compiling the generated
# forest first.
set(ELIZAOS_KSM_CANONICAL_TEST_TARGETS
    agentaction_test
    agentagenda_test
    agentbrowser_test
    agentcomms_test
    agentlogger_test
    agentloop_test
    agentmemory_test
    agentshell_test
    characterfile_test
    characters_test
    core_test
    knowledge_test
    cognitive_pipeline_e2e_test
    real_core_test
    real_agentaction_test
    real_agentagenda_test
    real_agentbrowser_test
    real_agentcomms_test
    real_agentlogger_test
    real_agentloop_test
    real_agentmemory_test
    real_attention_test
    real_characters_test
    real_knowledge_test
    real_knowledge_hypergraph_test
    real_cognitive_primitives_test
    real_repair_regressions_test
    plugin_specification_test
    public_header_forwarding_test
    api_client_real_test
    real_api_client_test
    public_header_forwarding_all_test
)


if(TARGET real_embodiment_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_embodiment_test)
endif()
if(TARGET real_evolutionary_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_evolutionary_test)
endif()
if(TARGET real_ontogenesis_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_ontogenesis_test)
endif()
if(TARGET persistence_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS persistence_test)
endif()
if(TARGET comprehensive_autonomy_e2e_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS comprehensive_autonomy_e2e_test)
endif()

# NOTE: add_custom_target snapshots its DEPENDS list at creation time. Several
# ksm-labeled tests are registered and list(APPEND ...)-ed to
# ELIZAOS_KSM_CANONICAL_TEST_TARGETS LATER in this file (eliza_paths_test,
# bridge/embodiment integration, autonomy optimizer/reflection/evolution,
# attention bus, closed-loop autonomy, homework loop). Creating the custom target
# here would silently exclude all of them from `ninja ksm_canonical_tests` even
# though `ctest -L ksm` still runs them (the "Not Run" trap). The target is
# therefore assembled once at the END of this file, after every append.
set_tests_properties(${ELIZAOS_KSM_CANONICAL_TEST_TARGETS}
    PROPERTIES LABELS "ksm;canonical"
)

# ============================================================================
# Eliza Paths Test - Environment-aware path resolution
# ============================================================================
add_executable(eliza_paths_test eliza_paths_test.cpp)
target_include_directories(eliza_paths_test PRIVATE
    ${CMAKE_SOURCE_DIR}/cpp/packages/applications/eliza/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/cpp/include
)
target_link_libraries(eliza_paths_test
    elizaos-eliza
    gtest
    gtest_main
    Threads::Threads
)
target_compile_options(eliza_paths_test PRIVATE -DELIZAOS_TEST_MODE)
add_test(NAME eliza_paths_test COMMAND eliza_paths_test)

# ============================================================================
# Cognitive Bridge Protocol Test - WebSocket protocol conformance
# ============================================================================
add_real_test(real_cognitive_bridge_protocol_test src/test_cognitive_bridge_protocol.cpp)

# ============================================================================
# Embodiment-Cognitive Integration Test
# ============================================================================
if(TARGET elizaos-embodiment)
    add_real_test(real_embodiment_cognitive_integration_test src/test_embodiment_cognitive_integration.cpp)
endif()

# Add new tests to KSM canonical targets
if(TARGET eliza_paths_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS eliza_paths_test)
endif()
if(TARGET real_cognitive_bridge_protocol_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_cognitive_bridge_protocol_test)
endif()
if(TARGET real_embodiment_cognitive_integration_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_embodiment_cognitive_integration_test)
endif()

# ============================================================================
# Autonomy Optimizer Test - Enhanced autonomy pipeline integration
# ============================================================================
add_real_test(real_autonomy_optimizer_test src/test_autonomy_optimizer.cpp)
if(TARGET real_autonomy_optimizer_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_autonomy_optimizer_test)
endif()

# ============================================================================
# Closed-loop autonomy suite - ported from hurdcog for cross-fork parity.
# Validates the advanced attention-weighted, evidence-gated goal lifecycle
# (single completion path, active-goal invariant, purposeful-repetition
# stagnation exemption) now shared by both forks.
# ============================================================================
add_real_test(real_closed_loop_autonomy_test src/test_closed_loop_autonomy.cpp)
if(TARGET real_closed_loop_autonomy_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_closed_loop_autonomy_test)
endif()
add_real_test(real_autonomy_reflection_test src/test_autonomy_reflection.cpp)
if(TARGET real_autonomy_reflection_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_autonomy_reflection_test)
endif()
add_real_test(real_autonomy_evolution_test src/test_autonomy_evolution.cpp)
if(TARGET real_autonomy_evolution_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_autonomy_evolution_test)
endif()
add_real_test(real_attention_bus_test src/test_attention_bus.cpp)
if(TARGET real_attention_bus_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS real_attention_bus_test)
endif()

# ============================================================================
# KSM canonical target assembly (MUST be last)
# ============================================================================
# homework_loop_test is registered and labeled "ksm" near the top of this file
# but was never folded into the canonical target. Add it (guarded) so the focused
# `ninja ksm_canonical_tests` lane builds exactly what `ctest -L ksm` runs.
if(TARGET homework_loop_test)
    list(APPEND ELIZAOS_KSM_CANONICAL_TEST_TARGETS homework_loop_test)
endif()

add_custom_target(ksm_canonical_tests
    DEPENDS ${ELIZAOS_KSM_CANONICAL_TEST_TARGETS}
)

# Re-label the full, final canonical set (idempotent for already-labeled targets).
set_tests_properties(${ELIZAOS_KSM_CANONICAL_TEST_TARGETS}
    PROPERTIES LABELS "ksm;canonical"
)
