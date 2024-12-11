project(
        'Achi',
        'c',
        version: '0.1',
        default_options: ['warning_level=3','c_std=c2x']
)
incdir = include_directories('include')
src_files = files('src/decision_tree.c', 'src/game_interface.c', 'src/game_board.c')
sdl_dep = dependency('sdl3')
sdl_image_dep = dependency('sdl3-image')
sdl_ttf_dep = dependency('sdl3-ttf')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
executable(
        'Achi-SDL',
        ['main.c', src_files],
        install: true,
        dependencies : [sdl_dep, sdl_image_dep, sdl_ttf_dep, m_dep],
        include_directories : incdir
)
executable(
        'Achi-Console',
        ['main-con.c', src_files],
        install : true,
        dependencies : [sdl_dep, sdl_image_dep, sdl_ttf_dep, m_dep],
        include_directories : incdir
)