'use strict';

var AdafruitServo = require('./lib/binding/adafruit_servo').AdafruitServo;

var instance = new AdafruitServo();

var HumixSense = require('humix-sense');

var config = {
    moduleName: 'adafruitservo',
    commands: ['turn-right', 'turn-left','reset'],
    log: [
        {
            file: './humix-adafruitservo-module.log',
            level: 'info'
        }
    ],
    debug: true
};

var humix = new HumixSense(config);
var hsm;

humix.on('connection', function(humixSensorModule){
    hsm = humixSensorModule;
    var log = hsm.getLogger();

    hsm.on('turn-left', function (data) { 
     
        log.info('turning left');
        instance.setPulseWidth(0,2000);
               
    });

    hsm.on('turn-right', function (data) { 
     
        log.info('turning right');
        instance.setPulseWidth(0,1000);
               
    });
    
    hsm.on('reset', function (data) { 
     
        log.info('resetting');
        instance.setPulseWidth(0,1500);
               
    });


});


exports.setPulseWidth = function (channel, pulseWidth) {
    instance.setPulseWidth(channel, pulseWidth);
}
