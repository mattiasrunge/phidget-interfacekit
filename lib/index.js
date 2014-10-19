
var binding = require('../build/Release/binding');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

var Phidget = function() {
  this.create                 = function()                            { return binding.create(); };
  this.open                   = function(handle)                      { return binding.open(); };
  this.waitForAttachment      = function(handle, milliseconds)        { return binding.waitForAttachment(milliseconds); };
  this.close                  = function(handle)                      { return binding.close(); };
  this.remove                 = function(handle)                      { return binding.remove(); };
  this.getDeviceName          = function(handle)                      { return binding.getDeviceName(); };
  this.getSerialNumber        = function(handle)                      { return binding.getSerialNumber(); };
  this.getDeviceVersion       = function(handle)                      { return binding.getDeviceVersion(); };
  this.getDeviceStatus        = function(handle)                      { return binding.getDeviceStatus(); };
  this.getLibraryVersion      = function()                            { return binding.getLibraryVersion(); };
  this.getDeviceType          = function(handle)                      { return binding.getDeviceType(); };
  this.getInputCount          = function(handle)                      { return binding.getInputCount(); };
  this.getInputState          = function(handle, index)               { return binding.getInputState(index); };
  this.getOutputCount         = function(handle)                      { return binding.getOutputCount(); };
  this.getOutputState         = function(handle, index)               { return binding.getOutputState(index); };
  this.setOutputState         = function(handle, index, state)        { return binding.setOutputState(index, state); };
  this.getSensorCount         = function(handle)                      { return binding.getSensorCount(); };
  this.getSensorValue         = function(handle, index)               { return binding.getSensorValue(index); };
  this.getSensorRawValue      = function(handle, index)               { return binding.getSensorRawValue(index); };
  this.getSensorChangeTrigger = function(handle, index)               { return binding.getSensorChangeTrigger(index); };
  this.setSensorChangeTrigger = function(handle, index)               { return binding.setSensorChangeTrigger(index); };
  this.getRatiometric         = function(handle)                      { return binding.getRatiometric(); };
  this.setRatiometric         = function(handle, value)               { return binding.setRatiometric(value); };
  this.getDataRate            = function(handle, index)               { return binding.getDataRate(index); };
  this.setDataRate            = function(handle, index, milliseconds) { return binding.setDataRate(index, milliseconds); };
  this.getDataRateMax         = function(handle, index)               { return binding.getDataRateMax(index); };
  this.getDataRateMin         = function(handle, index)               { return binding.getDataRateMin(index); };
};

util.inherits(Phidget, EventEmitter);

module.exports = new Phidget();

binding.context.attachHandler       = function(handle) { module.exports.emit("attach", handle); };
binding.context.detachHandler       = function(handle) { module.exports.emit("detach", handle); };
binding.context.errorHandler        = function(handle, errorCode, unknown) { module.exports.emit("detach", handle, errorCode, unknown); };
binding.context.inputChangeHandler  = function(handle, index, state) { module.exports.emit("inputChange", handle, index, state); };
binding.context.onputChangeHandler  = function(handle, index, state) { module.exports.emit("onputChange", handle, index, state); };
binding.context.sensorChangeHandler = function(handle, index, state) { module.exports.emit("sensorChange", handle, index, state); };
