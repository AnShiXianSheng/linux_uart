cmake_minimum_required(VERSION 3.1)

#include(/home3/pengym/.cmake_toolchain/aarch64-imx-poky.cmake)

# 目标名称
set(TARGET_APP mcu_reg_wr)

#set(CMAKE_TOOLCHAIN_FILE "/home3/pengym/.cmake_toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.cmake")
# set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


#set(CMAKE_C_FLAGS "-Wall -std=gnu99")

#设置C++版本号
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
        "-Wall"
		#"-fsanitize=address"
		"-Wextra"
		"-Wno-psabi"
)

project(${TARGET_APP} )

# 指定库文件搜索目录
link_directories(

)

# 指定头文件目录
include_directories( 
	"${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/general/inc/"
	"${PROJECT_SOURCE_DIR}/can-iostp/include/"
)


# 指定生成目标cd in	
add_executable(    ${TARGET_APP}
					"${PROJECT_SOURCE_DIR}/general/debug.c"
					"${PROJECT_SOURCE_DIR}/general/md5.c"
					"${PROJECT_SOURCE_DIR}/general/argparse.c"
					"${PROJECT_SOURCE_DIR}/general/crc_check.c"
					"${PROJECT_SOURCE_DIR}/general/pp_uart.c"
					"${PROJECT_SOURCE_DIR}/main.c"
					"${PROJECT_SOURCE_DIR}/spi_reg.c"
					"${PROJECT_SOURCE_DIR}/regwr_cb.c"
					"${PROJECT_SOURCE_DIR}/run.c"
					"${PROJECT_SOURCE_DIR}/rearview_mcu.c"
)

# 指定库
target_link_libraries(${TARGET_APP} 
	"-lpthread"
)



find_program(MEMORYCHECK_COMMAND NAMES valgrind)

set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --show-leak-kinds=all")

# add memcheck test action
# 执行命令 ctest -T memcheck 来进行内存泄漏检测
include(CTest)

enable_testing()

add_test(
  NAME ${TARGET_APP}
  COMMAND $<TARGET_FILE:${TARGET_APP}>
)

install(TARGETS ${TARGET_APP} RUNTIME DESTINATION bin)
