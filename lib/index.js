
var binding = require('../build/Release/binding');
var util = require('util');
var EventEmitter = require('events').EventEmitter;

var Phidget = function() {
  this.create                 = function()                            { return binding.create(); };
  this.open                   = function(handle, serialNumber)        { return binding.open(handle, serialNumber); };
  this.waitForAttachment      = function(handle, milliseconds)        { return binding.waitForAttachmenthandle, (handle, milliseconds); };
  this.close                  = function(handle)                      { return binding.close(handle); };
  this.remove                 = function(handle)                      { return binding.remove(handle); };
  this.getDeviceName          = function(handle)                      { return binding.getDeviceName(handle); };
  this.getSerialNumber        = function(handle)                      { return binding.getSerialNumber(handle); };
  this.getDeviceVersion       = function(handle)                      { return binding.getDeviceVersion(handle); };
  this.getDeviceStatus        = function(handle)                      { return binding.getDeviceStatus(handle); };
  this.getLibraryVersion      = function()                            { return binding.getLibraryVersion(); };
  this.getDeviceType          = function(handle)                      { return binding.getDeviceType(handle); };
  this.getInputCount          = function(handle)                      { return binding.getInputCount(handle); };
  this.getInputState          = function(handle, index)               { return binding.getInputState(handle, index); };
  this.getOutputCount         = function(handle)                      { return binding.getOutputCount(handle); };
  this.getOutputState         = function(handle, index)               { return binding.getOutputState(handle, index); };
  this.setOutputState         = function(handle, index, state)        { return binding.setOutputState(handle, index, state); };
  this.getSensorCount         = function(handle)                      { return binding.getSensorCount(handle); };
  this.getSensorValue         = function(handle, index)               { return binding.getSensorValue(handle, index); };
  this.getSensorRawValue      = function(handle, index)               { return binding.getSensorRawValue(handle, index); };
  this.getSensorChangeTrigger = function(handle, index)               { return binding.getSensorChangeTrigger(handle, index); };
  this.setSensorChangeTrigger = function(handle, index)               { return binding.setSensorChangeTrigger(handle, index); };
  this.getRatiometric         = function(handle)                      { return binding.getRatiometric(handle); };
  this.setRatiometric         = function(handle, value)               { return binding.setRatiometric(handle, value); };
  this.getDataRate            = function(handle, index)               { return binding.getDataRate(handle, index); };
  this.setDataRate            = function(handle, index, milliseconds) { return binding.setDataRate(handle, index, milliseconds); };
  this.getDataRateMax         = function(handle, index)               { return binding.getDataRateMax(handle, index); };
  this.getDataRateMin         = function(handle, index)               { return binding.getDataRateMin(handle, index); };
};

util.inherits(Phidget, EventEmitter);

module.exports = new Phidget();

binding.context.attachHandler       = function(handle) { module.exports.emit("attach", handle); };
binding.context.detachHandler       = function(handle) { module.exports.emit("detach", handle); };
binding.context.errorHandler        = function(handle, errorCode, unknown) { module.exports.emit("detach", handle, errorCode, unknown); };
binding.context.inputChangeHandler  = function(handle, index, state) { module.exports.emit("inputChange", handle, index, state); };
binding.context.onputChangeHandler  = function(handle, index, state) { module.exports.emit("onputChange", handle, index, state); };
binding.context.sensorChangeHandler = function(handle, index, state) { module.exports.emit("sensorChange", handle, index, state); };
