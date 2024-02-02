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
            'target_name': 'tablet',
            'sources': ['native_modules/tablet_node.cc','native_modules/display.c', 'native_modules/tablet.c'],
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
            'sources': ['native_modules/display_node.cc', 'native_modules/display.c'],
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
