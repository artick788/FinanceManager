cmake_minimum_required(VERSION 3.22)
project(FinanceManager DESCRIPTION "Manage your finances")

include(VelyraBuildUtils/VelyraBuildUtils.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO)
endif ()

set(VELYRA_APP_FRAMEWORK_PATH)
vl_include_or_fetch(VelyraAppFramework VELYRA_APP_FRAMEWORK_PATH)
message(STATUS "VelyraAppFramework found at ${VELYRA_APP_FRAMEWORK_PATH}")

set(FINANCE_MANAGER_HEADERS
    include/FinanceManager/FinanceManagerInclude.hpp
    include/FinanceManager/Date.hpp
    include/FinanceManager/Wage.hpp
    include/FinanceManager/Savings.hpp
    include/FinanceManager/MonthlyExpenses.hpp
)

set(FINANCE_MANAGER_SRC
    src/FinanceManager/Date.cpp
    src/FinanceManager/Wage.cpp
    src/FinanceManager/Savings.cpp
    src/FinanceManager/MonthlyExpenses.cpp
)

add_library(FinanceManager STATIC ${FINANCE_MANAGER_SRC} ${FINANCE_MANAGER_HEADERS})
vl_configure_target(FinanceManager)
target_link_libraries(FinanceManager PUBLIC VelyraUtils)
target_include_directories(FinanceManager PUBLIC include)

set(FINANCE_MANAGER_APP_HEADERS
    src/FinanceManagerApp/FinanceManagerLayer.hpp
    src/FinanceManagerApp/Store.hpp
    src/FinanceManagerApp/Utils.hpp
    src/FinanceManagerApp/IncomeOverview.hpp
    src/FinanceManagerApp/SavingsOverview.hpp
    src/FinanceManagerApp/MonthlyExpenseOverview.hpp

    src/FinanceManagerApp/Layouts/ConfigureFinanceLayout.hpp
    src/FinanceManagerApp/Layouts/OverviewLayout.hpp

    src/FinanceManagerApp/Popups/CreateIncomePopup.hpp
    src/FinanceManagerApp/Popups/AddPaycheckPopup.hpp
    src/FinanceManagerApp/Popups/CreateSavingsRecordPopup.hpp
    src/FinanceManagerApp/Popups/AddSavingsEntryPopup.hpp
    src/FinanceManagerApp/Popups/CreateMonthlyExpensePopup.hpp
    src/FinanceManagerApp/Popups/AddMonthlyExpensePopup.hpp

    src/FinanceManagerApp/Panels/SavingsTrendPanel.hpp
    src/FinanceManagerApp/Panels/TotalSavingsTrendPanel.hpp
    src/FinanceManagerApp/Panels/ExpenseTrendPanel.hpp

    src/FinanceManagerApp/Widgets/DateInput.hpp
)

set(FINANCE_MANAGER_APP_SRC
    src/FinanceManagerApp/FinanceManagerLayer.cpp
    src/FinanceManagerApp/Store.cpp
    src/FinanceManagerApp/Utils.cpp
    src/FinanceManagerApp/IncomeOverview.cpp
    src/FinanceManagerApp/SavingsOverview.cpp
    src/FinanceManagerApp/MonthlyExpenseOverview.cpp

    src/FinanceManagerApp/Layouts/ConfigureFinanceLayout.cpp
    src/FinanceManagerApp/Layouts/OverviewLayout.cpp

    src/FinanceManagerApp/Popups/CreateIncomePopup.cpp
    src/FinanceManagerApp/Popups/AddPaycheckPopup.cpp
    src/FinanceManagerApp/Popups/CreateSavingsRecordPopup.cpp
    src/FinanceManagerApp/Popups/AddSavingsEntryPopup.cpp
    src/FinanceManagerApp/Popups/CreateMonthlyExpensePopup.cpp
    src/FinanceManagerApp/Popups/AddMonthlyExpensePopup.cpp

    src/FinanceManagerApp/Panels/SavingsTrendPanel.cpp
    src/FinanceManagerApp/Panels/TotalSavingsTrendPanel.cpp
    src/FinanceManagerApp/Panels/ExpenseTrendPanel.cpp

    src/FinanceManagerApp/Widgets/DateInput.cpp
)

add_executable(FinanceManagerApp main.cpp ${FINANCE_MANAGER_APP_SRC} ${FINANCE_MANAGER_APP_HEADERS})
target_link_libraries(FinanceManagerApp PRIVATE FinanceManager VelyraAppFramework)
install(TARGETS FinanceManagerApp
    RUNTIME DESTINATION bin
)

set(FINANCE_MANAGER_TEST_SRC
    test/TestDate.cpp
    test/TestWage.cpp
    test/TestMonthlyExpenses.cpp
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