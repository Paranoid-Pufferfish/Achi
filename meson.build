project(
        'Achi',
        'c',
        version: '0.1',
        default_options: ['warning_level=3','c_std=c2x']
)

raylib_dep = dependency('raylib', version: '>=5.0')

executable(
        'Achi',
        'main.c',
        dependencies: raylib_dep,
        install: true
)