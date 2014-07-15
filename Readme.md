Phidget Interfacekit node.js addon
==================================
Node.js bindings for the Interfacekit parts of the Phidget library.
This module requires that the libphidget library is available in the system already.

# Dependencies
```
sudo apt-get install libusb-dev
wget http://www.phidgets.com/downloads/libraries/libphidget.tar.gz
tar zxvf libphidget.tar.gz
cd libphidget-2.1.8.20140319
./configure
make
sudo make install
```

# API
The module mimics the interfacekit parts of the phidget library API. So examples based on that API should be easy to convert to C++ versions.
All functions are synchronous and will block if they take time, they will throw if errors occur. Five events are available via the EventEmitter API which phidget module extends.

```
var phidget = require("phidget-interfacekit");

try {
  phidget.create();
    
  phidget.on("attach", function() {
    console.log("Phidget attached!");
  });
  
  phidget.on("detach", function() {
    console.log("Phidget detached!");
  });
  
  phidget.on("error", function(errorCode) {
    console.log("Phidget error!", errorCode);
  });

  phidget.open(-1);

  console.log("Waiting for interface kit to be attached....");
  
  phidget.waitForAttachment(10000);
  
  console.log("Device Type: " + phidget.getDeviceType());
  console.log("Device Serial Number: " + phidget.getSerialNumber());
  console.log("Device Version: " + phidget.getDeviceVersion());
  console.log("Device Input Count: " + phidget.getInputCount());
  console.log("Device Output Count: " + phidget.getOutputCount());
  console.log("Device Sensor Count: " + phidget.getSensorCount());
} catch (e) {
  console.error(e);
}

// Set output 1
phidget.setOutputState(1, 1);

// Available methods
/*
phidget.create                 = function();
phidget.open                   = function();
phidget.waitForAttachment      = function(milliseconds);
phidget.close                  = function();
phidget.remove                 = function();
phidget.getDeviceName          = function();
phidget.getSerialNumber        = function();
phidget.getDeviceVersion       = function();
phidget.getDeviceStatus        = function();
phidget.getLibraryVersion      = function();
phidget.getDeviceType          = function();
phidget.getInputCount          = function();
phidget.getInputState          = function(index);
phidget.getOutputCount         = function();
phidget.getOutputState         = function(index);
phidget.setOutputState         = function(index, state);
phidget.getSensorCount         = function();
phidget.getSensorValue         = function(index);
phidget.getSensorRawValue      = function(index);
phidget.getSensorChangeTrigger = function(index);
phidget.setSensorChangeTrigger = function(index);
phidget.getRatiometric         = function();
phidget.setRatiometric         = function(value);
phidget.getDataRate            = function(index);
phidget.setDataRate            = function(index, milliseconds);
phidget.getDataRateMax         = function(index);
phidget.getDataRateMin         = function(index);
*/

```
