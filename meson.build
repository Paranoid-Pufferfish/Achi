project(
        'Achi',
        'c',
        version: '0.1',
        default_options: ['warning_level=3','c_std=c2x']
)

incdir = include_directories('include')
src_files = files('main.c', 'src/achi_board.c')
executable(
        'Achi',
        src_files,
        install: true,
        include_directories : incdir
)