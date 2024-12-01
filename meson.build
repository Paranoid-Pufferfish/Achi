project(
        'Achi',
        'c',
        version: '0.1',
        default_options: ['warning_level=3','c_std=c2x']
)
cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
incdir = include_directories('include')
src_files = files('main.c', 'src/decision_tree.c', 'src/game_interface.c', 'src/game_board.c')
sdl2_dep = dependency('sdl2')
executable(
        'Achi',
        src_files,
        install: true,
        dependencies : [sdl2_dep, m_dep],
        include_directories : incdir
)