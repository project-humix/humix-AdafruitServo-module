'use strict';
var AdafruitServo = require('./lib/binding/adafruit_servo').AdafruitServo;

var instance = new AdafruitServo();

exports.setPulseWidth = function (channel, pulseWidth) {
  instance.setPulseWidth(channel, pulseWidth);
}
