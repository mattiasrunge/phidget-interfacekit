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
  var phid = phidget.create();

  phidget.on("attach", function(phid) {
    console.log("Phidget attached!");
  });

  phidget.on("detach", function(phid) {
    console.log("Phidget detached!");
  });

  phidget.on("error", function(phid, errorString) {
    console.log("Phidget error!", errorString);
  });

  phidget.open(phid, -1);

  console.log("Waiting for interface kit to be attached....");

  phidget.waitForAttachment(10000);

  console.log("Device Type: " + phidget.getDeviceType(phid));
  console.log("Device Serial Number: " + phidget.getSerialNumber(phid));
  console.log("Device Version: " + phidget.getDeviceVersionphid));
  console.log("Device Input Count: " + phidget.getInputCount(phid));
  console.log("Device Output Count: " + phidget.getOutputCount(phid));
  console.log("Device Sensor Count: " + phidget.getSensorCount(phid));
} catch (e) {
  console.error(e);
}

// Set output 1
phidget.setOutputState(phid, 1, 1);

// Available methods
/*
phidget.create                 = function();
phidget.open                   = function(handle);
phidget.waitForAttachment      = function(handle, milliseconds);
phidget.close                  = function(handle);
phidget.remove                 = function(handle);
phidget.getDeviceName          = function(handle);
phidget.getSerialNumber        = function(handle);
phidget.getDeviceVersion       = function(handle);
phidget.getDeviceStatus        = function(handle);
phidget.getLibraryVersion      = function();
phidget.getDeviceType          = function(handle);
phidget.getInputCount          = function(handle);
phidget.getInputState          = function(handle, index);
phidget.getOutputCount         = function(handle);
phidget.getOutputState         = function(handle, index);
phidget.setOutputState         = function(handle, index, state);
phidget.getSensorCount         = function(handle);
phidget.getSensorValue         = function(handle, index);
phidget.getSensorRawValue      = function(handle, index);
phidget.getSensorChangeTrigger = function(handle, index);
phidget.setSensorChangeTrigger = function(handle, index);
phidget.getRatiometric         = function(handle);
phidget.setRatiometric         = function(handle, value);
phidget.getDataRate            = function(handle, index);
phidget.setDataRate            = function(handle, index, milliseconds);
phidget.getDataRateMax         = function(handle, index);
phidget.getDataRateMin         = function(handle, index);
*/

```
