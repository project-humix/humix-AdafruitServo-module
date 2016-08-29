{
  'conditions': [
    ['OS=="linux"', {
      'targets': [

        {
          'target_name': 'adafruit_servo',
          'sources': [
            './src/I2Cdev.cpp',
            './src/AdafruitServo.cpp'
          ],
          'include_dirs': ['<!(node -e "require(\'nan\')")',
            './include'
          ],
          'cflags': ['-O2', '-Wall']
        },
        {
          'target_name':'action_after_build',
          'type': 'none',
          'dependencies': ['adafruit_servo'],
          'copies': [{
            'destination': './lib/binding/',
            'files': [
              '<(PRODUCT_DIR)/adafruit_servo.node'
            ]
          }]
        }
      ]
    }, { # OS != linux
      'targets': [{
        'target_name': 'adafruit_servo',
        'actions': [{
          'action_name': 'not_supported_msg',
          'inputs': [],
          'outputs': [ '--nothing-being-built--' ],
          'action': ['true'],
          'message': '**** YOU ARE INSTALLING THIS MODULE ON AN UNSUPPORTED PLATFORM ****'
        }]
      }]
    }]
  ]
}
