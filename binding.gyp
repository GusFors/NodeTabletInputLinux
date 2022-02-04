{
    'targets': [
        {
            'target_name': 'pointer',
            'sources': ['pointer.cc'],
            'include_dirs': ['<!(node -e \'require("nan")\')'],
            'link_settings': {
                'libraries': [
                    '-lX11',
                    '-lXrandr',
                ]
            },
            'cflags': [
               
            ],
        },
        {
            'target_name': 'display',
            'sources': ['display.cc'],
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
