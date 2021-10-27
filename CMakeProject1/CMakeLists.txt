# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ffmpeg "CMakeProject1.cpp" "CMakeProject1.h")

# 配置 FFmpeg 头文件目录
include_directories("D:/Visual_Studio_2019/Pro_Lib/ffmpeg-n4.4.1-win64-gpl-shared-4.4/include")

# 配置 FFmpeg .dll 动态链接库 -> 暂时不用添加.

# 调试 Cmake
#get_property()
message("项目路径 -> ${CMAKE_CURRENT_SOURCE_DIR}")

# 配置 FFmpeg .lib 静态链接库目录
target_link_directories(
	ffmpeg
	PUBLIC 
	"D:/Visual_Studio_2019/Pro_Lib/ffmpeg-n4.4.1-win64-gpl-shared-4.4/lib"
)

# 链接 FFmpeg .lib 静态链接库
target_link_libraries(
	ffmpeg	# add_executable 的 编译后的可执行文件的 名称.
	avcodec # 编解码（最重要的库）
	avdevice  # 各种设备的输入输出
	avfilter  # 滤镜特效处理
	avformat  # 封装格式处理
	avutil  # 工具库（大部分库都需要这个库的支持）
	postproc  # 后加工
	swresample # 音频采样数据格式转换
	swscale # 视频像素数据格式转换
)

# TODO: Add tests and install targets if needed.
