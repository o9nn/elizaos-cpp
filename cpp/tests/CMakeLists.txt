cmake_minimum_required(VERSION 3.16)
project(elizaos_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Include project headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add test executables

add_executable(agentaction_test agentaction_test.cpp)
target_link_libraries(agentaction_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentaction_test COMMAND agentaction_test)

add_executable(agentagenda_test agentagenda_test.cpp)
target_link_libraries(agentagenda_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentagenda_test COMMAND agentagenda_test)

add_executable(agentbrowser_test agentbrowser_test.cpp)
target_link_libraries(agentbrowser_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentbrowser_test COMMAND agentbrowser_test)

add_executable(agentcomms_test agentcomms_test.cpp)
target_link_libraries(agentcomms_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentcomms_test COMMAND agentcomms_test)

add_executable(agentlogger_test agentlogger_test.cpp)
target_link_libraries(agentlogger_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentlogger_test COMMAND agentlogger_test)

add_executable(agentloop_test agentloop_test.cpp)
target_link_libraries(agentloop_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentloop_test COMMAND agentloop_test)

add_executable(agentmemory_test agentmemory_test.cpp)
target_link_libraries(agentmemory_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentmemory_test COMMAND agentmemory_test)

add_executable(agentshell_test agentshell_test.cpp)
target_link_libraries(agentshell_test ${GTEST_LIBRARIES} pthread)
add_test(NAME agentshell_test COMMAND agentshell_test)

add_executable(auto_fun_test auto_fun_test.cpp)
target_link_libraries(auto_fun_test ${GTEST_LIBRARIES} pthread)
add_test(NAME auto_fun_test COMMAND auto_fun_test)

add_executable(autofun_idl_test autofun_idl_test.cpp)
target_link_libraries(autofun_idl_test ${GTEST_LIBRARIES} pthread)
add_test(NAME autofun_idl_test COMMAND autofun_idl_test)

add_executable(autonomous_starter_test autonomous_starter_test.cpp)
target_link_libraries(autonomous_starter_test ${GTEST_LIBRARIES} pthread)
add_test(NAME autonomous_starter_test COMMAND autonomous_starter_test)

add_executable(awesome_eliza_test awesome_eliza_test.cpp)
target_link_libraries(awesome_eliza_test ${GTEST_LIBRARIES} pthread)
add_test(NAME awesome_eliza_test COMMAND awesome_eliza_test)

add_executable(brandkit_test brandkit_test.cpp)
target_link_libraries(brandkit_test ${GTEST_LIBRARIES} pthread)
add_test(NAME brandkit_test COMMAND brandkit_test)

add_executable(characterfile_test characterfile_test.cpp)
target_link_libraries(characterfile_test ${GTEST_LIBRARIES} pthread)
add_test(NAME characterfile_test COMMAND characterfile_test)

add_executable(characters_test characters_test.cpp)
target_link_libraries(characters_test ${GTEST_LIBRARIES} pthread)
add_test(NAME characters_test COMMAND characters_test)

add_executable(classified_test classified_test.cpp)
target_link_libraries(classified_test ${GTEST_LIBRARIES} pthread)
add_test(NAME classified_test COMMAND classified_test)

add_executable(core_test core_test.cpp)
target_link_libraries(core_test ${GTEST_LIBRARIES} pthread)
add_test(NAME core_test COMMAND core_test)

add_executable(discord_summarizer_test discord_summarizer_test.cpp)
target_link_libraries(discord_summarizer_test ${GTEST_LIBRARIES} pthread)
add_test(NAME discord_summarizer_test COMMAND discord_summarizer_test)

add_executable(discrub_ext_test discrub_ext_test.cpp)
target_link_libraries(discrub_ext_test ${GTEST_LIBRARIES} pthread)
add_test(NAME discrub_ext_test COMMAND discrub_ext_test)

add_executable(easycompletion_test easycompletion_test.cpp)
target_link_libraries(easycompletion_test ${GTEST_LIBRARIES} pthread)
add_test(NAME easycompletion_test COMMAND easycompletion_test)

add_executable(eliza_test eliza_test.cpp)
target_link_libraries(eliza_test ${GTEST_LIBRARIES} pthread)
add_test(NAME eliza_test COMMAND eliza_test)

add_executable(eliza_3d_hyperfy_starter_test eliza_3d_hyperfy_starter_test.cpp)
target_link_libraries(eliza_3d_hyperfy_starter_test ${GTEST_LIBRARIES} pthread)
add_test(NAME eliza_3d_hyperfy_starter_test COMMAND eliza_3d_hyperfy_starter_test)

add_executable(eliza_nextjs_starter_test eliza_nextjs_starter_test.cpp)
target_link_libraries(eliza_nextjs_starter_test ${GTEST_LIBRARIES} pthread)
add_test(NAME eliza_nextjs_starter_test COMMAND eliza_nextjs_starter_test)

add_executable(eliza_plugin_starter_test eliza_plugin_starter_test.cpp)
target_link_libraries(eliza_plugin_starter_test ${GTEST_LIBRARIES} pthread)
add_test(NAME eliza_plugin_starter_test COMMAND eliza_plugin_starter_test)

add_executable(eliza_starter_test eliza_starter_test.cpp)
target_link_libraries(eliza_starter_test ${GTEST_LIBRARIES} pthread)
add_test(NAME eliza_starter_test COMMAND eliza_starter_test)

add_executable(elizaos_github_io_test elizaos_github_io_test.cpp)
target_link_libraries(elizaos_github_io_test ${GTEST_LIBRARIES} pthread)
add_test(NAME elizaos_github_io_test COMMAND elizaos_github_io_test)

add_executable(elizas_list_test elizas_list_test.cpp)
target_link_libraries(elizas_list_test ${GTEST_LIBRARIES} pthread)
add_test(NAME elizas_list_test COMMAND elizas_list_test)

add_executable(elizas_world_test elizas_world_test.cpp)
target_link_libraries(elizas_world_test ${GTEST_LIBRARIES} pthread)
add_test(NAME elizas_world_test COMMAND elizas_world_test)

add_executable(embodiment_test embodiment_test.cpp)
target_link_libraries(embodiment_test ${GTEST_LIBRARIES} pthread)
add_test(NAME embodiment_test COMMAND embodiment_test)

add_executable(evolutionary_test evolutionary_test.cpp)
target_link_libraries(evolutionary_test ${GTEST_LIBRARIES} pthread)
add_test(NAME evolutionary_test COMMAND evolutionary_test)

add_executable(hat_test hat_test.cpp)
target_link_libraries(hat_test ${GTEST_LIBRARIES} pthread)
add_test(NAME hat_test COMMAND hat_test)

add_executable(hats_test hats_test.cpp)
target_link_libraries(hats_test ${GTEST_LIBRARIES} pthread)
add_test(NAME hats_test COMMAND hats_test)

add_executable(knowledge_test knowledge_test.cpp)
target_link_libraries(knowledge_test ${GTEST_LIBRARIES} pthread)
add_test(NAME knowledge_test COMMAND knowledge_test)

add_executable(livevideochat_test livevideochat_test.cpp)
target_link_libraries(livevideochat_test ${GTEST_LIBRARIES} pthread)
add_test(NAME livevideochat_test COMMAND livevideochat_test)

add_executable(ljspeechtools_test ljspeechtools_test.cpp)
target_link_libraries(ljspeechtools_test ${GTEST_LIBRARIES} pthread)
add_test(NAME ljspeechtools_test COMMAND ljspeechtools_test)

add_executable(mcp_gateway_test mcp_gateway_test.cpp)
target_link_libraries(mcp_gateway_test ${GTEST_LIBRARIES} pthread)
add_test(NAME mcp_gateway_test COMMAND mcp_gateway_test)

add_executable(ontogenesis_test ontogenesis_test.cpp)
target_link_libraries(ontogenesis_test ${GTEST_LIBRARIES} pthread)
add_test(NAME ontogenesis_test COMMAND ontogenesis_test)

add_executable(otaku_test otaku_test.cpp)
target_link_libraries(otaku_test ${GTEST_LIBRARIES} pthread)
add_test(NAME otaku_test COMMAND otaku_test)

add_executable(otc_agent_test otc_agent_test.cpp)
target_link_libraries(otc_agent_test ${GTEST_LIBRARIES} pthread)
add_test(NAME otc_agent_test COMMAND otc_agent_test)

add_executable(plugin_specification_test plugin_specification_test.cpp)
target_link_libraries(plugin_specification_test ${GTEST_LIBRARIES} pthread)
add_test(NAME plugin_specification_test COMMAND plugin_specification_test)

add_executable(plugins_automation_test plugins_automation_test.cpp)
target_link_libraries(plugins_automation_test ${GTEST_LIBRARIES} pthread)
add_test(NAME plugins_automation_test COMMAND plugins_automation_test)

add_executable(registry_test registry_test.cpp)
target_link_libraries(registry_test ${GTEST_LIBRARIES} pthread)
add_test(NAME registry_test COMMAND registry_test)

add_executable(spartan_test spartan_test.cpp)
target_link_libraries(spartan_test ${GTEST_LIBRARIES} pthread)
add_test(NAME spartan_test COMMAND spartan_test)

add_executable(sweagent_test sweagent_test.cpp)
target_link_libraries(sweagent_test ${GTEST_LIBRARIES} pthread)
add_test(NAME sweagent_test COMMAND sweagent_test)

add_executable(the_org_test the_org_test.cpp)
target_link_libraries(the_org_test ${GTEST_LIBRARIES} pthread)
add_test(NAME the_org_test COMMAND the_org_test)

add_executable(trust_scoreboard_test trust_scoreboard_test.cpp)
target_link_libraries(trust_scoreboard_test ${GTEST_LIBRARIES} pthread)
add_test(NAME trust_scoreboard_test COMMAND trust_scoreboard_test)

add_executable(vercel_api_test vercel_api_test.cpp)
target_link_libraries(vercel_api_test ${GTEST_LIBRARIES} pthread)
add_test(NAME vercel_api_test COMMAND vercel_api_test)

add_executable(website_test website_test.cpp)
target_link_libraries(website_test ${GTEST_LIBRARIES} pthread)
add_test(NAME website_test COMMAND website_test)

add_executable(workgroups_test workgroups_test.cpp)
target_link_libraries(workgroups_test ${GTEST_LIBRARIES} pthread)
add_test(NAME workgroups_test COMMAND workgroups_test)
