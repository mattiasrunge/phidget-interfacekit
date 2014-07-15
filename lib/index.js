
var binding = require('../build/Release/binding');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

var Phidget = function() {
  this.create                 = function()                    { return binding.create(); };
  this.open                   = function()                    { return binding.open(); };
  this.waitForAttachment      = function(milliseconds)        { return binding.waitForAttachment(milliseconds); };
  this.close                  = function()                    { return binding.close(); };
  this.remove                 = function()                    { return binding.remove(); };
  this.getDeviceName          = function()                    { return binding.getDeviceName(); };
  this.getSerialNumber        = function()                    { return binding.getSerialNumber(); };
  this.getDeviceVersion       = function()                    { return binding.getDeviceVersion(); };
  this.getDeviceStatus        = function()                    { return binding.getDeviceStatus(); };
  this.getLibraryVersion      = function()                    { return binding.getLibraryVersion(); };
  this.getDeviceType          = function()                    { return binding.getDeviceType(); };
  this.getInputCount          = function()                    { return binding.getInputCount(); };
  this.getInputState          = function(index)               { return binding.getInputState(index); };
  this.getOutputCount         = function()                    { return binding.getOutputCount(); };
  this.getOutputState         = function(index)               { return binding.getOutputState(index); };
  this.setOutputState         = function(index, state)        { return binding.setOutputState(index, state); };
  this.getSensorCount         = function()                    { return binding.getSensorCount(); };
  this.getSensorValue         = function(index)               { return binding.getSensorValue(index); };
  this.getSensorRawValue      = function(index)               { return binding.getSensorRawValue(index); };
  this.getSensorChangeTrigger = function(index)               { return binding.getSensorChangeTrigger(index); };
  this.setSensorChangeTrigger = function(index)               { return binding.setSensorChangeTrigger(index); };
  this.getRatiometric         = function()                    { return binding.getRatiometric(); };
  this.setRatiometric         = function(value)               { return binding.setRatiometric(value); };
  this.getDataRate            = function(index)               { return binding.getDataRate(index); };
  this.setDataRate            = function(index, milliseconds) { return binding.setDataRate(index, milliseconds); };
  this.getDataRateMax         = function(index)               { return binding.getDataRateMax(index); };
  this.getDataRateMin         = function(index)               { return binding.getDataRateMin(index); };
};

util.inherits(Phidget, EventEmitter);

module.exports = new Phidget();

binding.context.attachHandler       = function() { module.exports.emit("attach"); }; 
binding.context.detachHandler       = function() { module.exports.emit("detach"); }; 
binding.context.errorHandler        = function(errorCode, unknown) { module.exports.emit("detach", errorCode, unknown); }; 
binding.context.inputChangeHandler  = function(index, state) { module.exports.emit("inputChange", index, state); }; 
binding.context.onputChangeHandler  = function(index, state) { module.exports.emit("onputChange", index, state); }; 
binding.context.sensorChangeHandler = function(index, state) { module.exports.emit("sensorChange", index, state); }; 
