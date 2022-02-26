{
    'targets': [
        {
            'target_name': 'pointer',
            'sources': ['native_modules/pointer.cc'],
            'include_dirs': ['<!(node -e \'require("nan")\')'],
            'link_settings': {
                'libraries': [
                    '-lX11',
                    '-lXrandr',
                    '-lXtst'
                ]
            },
            'cflags': [
               
            ],
        },
        {
            'target_name': 'display',
            'sources': ['native_modules/display.cc'],
            'include_dirs': ['<!(node -e \'require("nan")\')'],
            'link_settings': {
                'libraries': [
                    '-lX11',
                    '-lXrandr',
                ]
            },
            'cflags': [
               
            ],
        }
    ]
}
