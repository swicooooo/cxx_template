# for each "src/x.cpp", generate target "x"
find_package(Boost REQUIRED COMPONENTS log)  # 1.find Boost_log component
file(GLOB_RECURSE all_srcs CONFIGURE_DEPENDS *.cpp)
foreach(v ${all_srcs})
    string(REGEX MATCH "src/.*" relative_path ${v})
    # message(${relative_path})
    string(REGEX REPLACE "src/" "" target_name ${relative_path})
    string(REGEX REPLACE ".cpp" "" target_name ${target_name})

    add_executable(${target_name} ${v})
  
    if (Boost_FOUND)    # 2.add lib (eg: ldd executable_file  -> could list dependencies for project)
        target_include_directories(${target_name} PUBLIC ${Boost_INCLUDE_DIRS})
        target_link_libraries(${target_name}
        PRIVATE
        Boost::log)
    endif ()
        # 3.添加Boost库路径到ldconfig
        # 3.1 echo "/usr/local/lib" | sudo tee -a /etc/ld.so.conf.d/local.conf
        # 3.2 sudo ldconfig
endforeach()




