project(Watcher)

aux_source_directory(. src)


add_library(Watcher SHARED ${src})

# dl 动态链接； z 压缩
target_link_libraries(Watcher android dl log z)