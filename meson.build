project(
        'Achi',
        'c',
        version: '0.1',
        default_options: ['warning_level=3','c_std=c2x']
)

incdir = include_directories('include')
src_files = files('main.c')
sdl2_dep = dependency('sdl2')
executable(
        'Achi',
        src_files,
        install: true,
        dependencies : sdl2_dep,
        include_directories : incdir
)