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
    target_link_libraries(${name}
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
        nlohmann_json::nlohmann_json
        Threads::Threads
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_real_test(real_core_test                src/test_core.cpp)
add_real_test(real_agentaction_test         src/test_agentaction.cpp)
add_real_test(real_agentagenda_test         src/test_agentagenda.cpp)
add_real_test(real_agentcomms_test          src/test_agentcomms.cpp)
add_real_test(real_agentlogger_test         src/test_agentlogger.cpp)
add_real_test(real_agentloop_test           src/test_agentloop.cpp)
add_real_test(real_agentmemory_test         src/test_agentmemory.cpp)
add_real_test(real_attention_test           src/test_attention_allocation.cpp)
add_real_test(real_characters_test          src/test_characters.cpp)
add_real_test(real_knowledge_test           src/test_knowledge.cpp)
add_real_test(real_embodiment_test          src/test_embodiment.cpp)
add_real_test(real_evolutionary_test        src/test_evolutionary.cpp)
add_real_test(real_ontogenesis_test         src/test_ontogenesis.cpp)
add_real_test(real_cognitive_primitives_test src/test_cognitive_primitives.cpp)
