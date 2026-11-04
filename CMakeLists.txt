cmake_minimum_required(VERSION 3.22)
project(FinanceManager DESCRIPTION "Manage your finances")

include(VelyraBuildUtils/VelyraBuildUtils.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif ()

vl_find_or_fetch(VelyraAppFramework git@github.com:VelyraEngine/VelyraAppFramework.git v0.4)

set(FINANCE_MANAGER_HEADERS
    include/FinanceManager/FinanceManagerInclude.hpp
    include/FinanceManager/Date.hpp
    include/FinanceManager/Wage.hpp
    include/FinanceManager/Savings.hpp
    include/FinanceManager/MonthlyExpenses.hpp
    include/FinanceManager/FinanceData.hpp
    include/FinanceManager/Reports.hpp
)

set(FINANCE_MANAGER_SRC
    src/FinanceManager/Date.cpp
    src/FinanceManager/Wage.cpp
    src/FinanceManager/Savings.cpp
    src/FinanceManager/MonthlyExpenses.cpp
    src/FinanceManager/FinanceData.cpp
    src/FinanceManager/Reports.cpp
)

add_library(FinanceManager STATIC ${FINANCE_MANAGER_SRC} ${FINANCE_MANAGER_HEADERS})
vl_configure_target(FinanceManager)
target_link_libraries(FinanceManager PUBLIC VelyraUtils)
target_include_directories(FinanceManager PUBLIC include)

set(FINANCE_MANAGER_APP_HEADERS
    src/FinanceManagerApp/FinanceManagerLayer.hpp
    src/FinanceManagerApp/FinanceLayout.hpp
    src/FinanceManagerApp/Store.hpp
    src/FinanceManagerApp/Persistence.hpp
    src/FinanceManagerApp/Commands.hpp
    src/FinanceManagerApp/Theme.hpp
    src/FinanceManagerApp/Ui.hpp

    src/FinanceManagerApp/Pages/DashboardPage.hpp
    src/FinanceManagerApp/Pages/IncomePage.hpp
    src/FinanceManagerApp/Pages/SavingsPage.hpp
    src/FinanceManagerApp/Pages/ExpensesPage.hpp
    src/FinanceManagerApp/Pages/ManagePage.hpp

    src/FinanceManagerApp/Popups/EntryPopups.hpp
    src/FinanceManagerApp/Popups/CreatePopups.hpp
    src/FinanceManagerApp/Popups/ConfirmPopup.hpp
    src/FinanceManagerApp/Popups/SettingsPopup.hpp
)

set(FINANCE_MANAGER_APP_SRC
    src/FinanceManagerApp/FinanceManagerLayer.cpp
    src/FinanceManagerApp/FinanceLayout.cpp
    src/FinanceManagerApp/Persistence.cpp
    src/FinanceManagerApp/Commands.cpp
    src/FinanceManagerApp/Theme.cpp
    src/FinanceManagerApp/Ui.cpp

    src/FinanceManagerApp/Pages/DashboardPage.cpp
    src/FinanceManagerApp/Pages/IncomePage.cpp
    src/FinanceManagerApp/Pages/SavingsPage.cpp
    src/FinanceManagerApp/Pages/ExpensesPage.cpp
    src/FinanceManagerApp/Pages/ManagePage.cpp

    src/FinanceManagerApp/Popups/EntryPopups.cpp
    src/FinanceManagerApp/Popups/CreatePopups.cpp
    src/FinanceManagerApp/Popups/ConfirmPopup.cpp
    src/FinanceManagerApp/Popups/SettingsPopup.cpp
)

add_executable(FinanceManagerApp main.cpp ${FINANCE_MANAGER_APP_SRC} ${FINANCE_MANAGER_APP_HEADERS})
target_link_libraries(FinanceManagerApp PRIVATE FinanceManager VelyraAppFramework)
target_include_directories(FinanceManagerApp PRIVATE src)
# ImGui and the framework are included almost everywhere, parse them once
target_precompile_headers(FinanceManagerApp PRIVATE
    <VelyraCore/ImGui/ImGui.hpp>
    <VelyraAppFramework/AppData.hpp>
    <FinanceManager/Reports.hpp>
)
install(TARGETS FinanceManagerApp
    RUNTIME DESTINATION bin
)

set(FINANCE_MANAGER_TEST_SRC
    test/TestDate.cpp
    test/TestWage.cpp
    test/TestMonthlyExpenses.cpp
    test/TestReports.cpp
)

if (BUILD_TESTING)
    vl_fetch_gtest()

    add_executable(TestFinanceManager ${FINANCE_MANAGER_TEST_SRC})
    target_include_directories(TestFinanceManager PUBLIC include)
    target_link_libraries(TestFinanceManager PUBLIC GTest::gtest GTest::gtest_main FinanceManager)
    add_test(NAME CTestFinanceManager COMMAND ${PROOT} $<TARGET_FILE:TestFinanceManager>)
    set_tests_properties(CTestFinanceManager PROPERTIES
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif ()