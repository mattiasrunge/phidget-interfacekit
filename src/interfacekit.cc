#include <node.h>
#include <v8.h>
#include <phidget21.h>

using namespace v8;

CPhidgetInterfaceKitHandle ifKit = 0;
Persistent<Object> contextObj;
uv_async_t async;

enum Events
{
  ATTACH,
  DETACH,
  ERROR,
  INPUT_CHANGE,
  OUTPUT_CHANGE,
  SENSOR_CHANGE
};

class Baton
{
public:
  Events event;
  int errorCode;
  int index;
  int value;
};

int CCONV attachHandler(CPhidgetHandle IFK, void *userptr)
{
  Baton *baton = new Baton;
  async.data = (void*)baton;
  
  baton->event = ATTACH;
  uv_async_send(&async);
  
  return 0;
}

int CCONV detachHandler(CPhidgetHandle IFK, void *userptr)
{
  Baton *baton = new Baton;
  async.data = (void*)baton;
  
  baton->event = DETACH;
  uv_async_send(&async);
  
  return 0;
}

int CCONV errorHandler(CPhidgetHandle IFK, void *userptr, int errorCode, const char *unknown)
{
  Baton *baton = new Baton;
  async.data = (void*)baton;
  
  baton->event = ERROR;
  baton->errorCode = errorCode;
  uv_async_send(&async);
  
  return 0;
}

int CCONV inputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int index, int value)
{
  Baton *baton = new Baton;
  async.data = (void*)baton;
  
  baton->event = INPUT_CHANGE;
  baton->index = index;
  baton->value = value;
  uv_async_send(&async);
  
  return 0;
}

int CCONV outputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int index, int value)
{
  Baton *baton = new Baton;
  async.data = (void*)baton;
  
  baton->event = OUTPUT_CHANGE;
  baton->index = index;
  baton->value = value;
  uv_async_send(&async);
  
  return 0;

}

int CCONV sensorChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int index, int value)
{
  Baton *baton = new Baton;
  async.data = (void*)baton;
  
  baton->event = SENSOR_CHANGE;
  baton->index = index;
  baton->value = value;
  uv_async_send(&async);
  
  return 0;
}

Handle<Value> create(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;
  
  if (ifKit != 0)
  {
    ThrowException(Exception::TypeError(String::New("Already created must be destroy")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_create(&ifKit);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, attachHandler, NULL);
    
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, detachHandler, NULL);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, errorHandler, NULL);
    
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_set_OnInputChange_Handler(ifKit, inputChangeHandler, NULL);
    
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_set_OnSensorChange_Handler(ifKit, sensorChangeHandler, NULL);
    
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_set_OnOutputChange_Handler(ifKit, outputChangeHandler, NULL);
    
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> open(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;
  
  errorCode = CPhidget_open((CPhidgetHandle)ifKit, -1);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> waitForAttachment(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing milliseconds argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Milliseconds argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, args[0]->Int32Value());
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> close(const Arguments& args)
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;

  errorCode = CPhidget_close((CPhidgetHandle)ifKit);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> remove(const Arguments& args)
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;

  errorCode = CPhidget_delete((CPhidgetHandle)ifKit);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  ifKit = 0;
  
  return scope.Close(Undefined());
}

Handle<Value> getDeviceName(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription, *deviceName;
  
  errorCode = CPhidget_getDeviceName((CPhidgetHandle)ifKit, &deviceName);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(String::New(deviceName));
}

Handle<Value> getSerialNumber(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, serialNumber;
  const char *errorDescription;
  
  errorCode = CPhidget_getSerialNumber((CPhidgetHandle)ifKit, &serialNumber);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(serialNumber));
}

Handle<Value> getDeviceVersion(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, deviceVersion;
  const char *errorDescription;
  
  errorCode = CPhidget_getDeviceVersion((CPhidgetHandle)ifKit, &deviceVersion);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(deviceVersion));
}

Handle<Value> getDeviceStatus(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, deviceStatus;
  const char *errorDescription;
  
  errorCode = CPhidget_getDeviceStatus((CPhidgetHandle)ifKit, &deviceStatus);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(deviceStatus));
}

Handle<Value> getLibraryVersion(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription, *libraryVersion;
  
  errorCode = CPhidget_getLibraryVersion(&libraryVersion);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(String::New(libraryVersion));
}

Handle<Value> getDeviceType(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription, *deviceType;
  
  errorCode = CPhidget_getDeviceType((CPhidgetHandle)ifKit, &deviceType);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(String::New(deviceType));
}

Handle<Value> getInputCount(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, count;
  const char *errorDescription;
  
  errorCode = CPhidgetInterfaceKit_getInputCount(ifKit, &count);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(count));
}

Handle<Value> getInputState(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, inputState;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getInputState(ifKit, args[0]->Int32Value(), &inputState);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(inputState));
}

Handle<Value> getOutputCount(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, count;
  const char *errorDescription;
  
  errorCode = CPhidgetInterfaceKit_getOutputCount(ifKit, &count);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(count));
}

Handle<Value> getOutputState(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, outputState;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getOutputState(ifKit, args[0]->Int32Value(), &outputState);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(outputState));
}

Handle<Value> setOutputState(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;
  
  if (args.Length() < 2)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber() || !args[1]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index or outputState argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_setOutputState(ifKit, args[0]->Int32Value(),  args[1]->Int32Value());
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> getSensorCount(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, count;
  const char *errorDescription;
  
  errorCode = CPhidgetInterfaceKit_getSensorCount(ifKit, &count);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(count));
}

Handle<Value> getSensorValue(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, sensorValue;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getSensorValue(ifKit, args[0]->Int32Value(), &sensorValue);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(sensorValue));
}

Handle<Value> getSensorRawValue(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, sensorRawValue;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getSensorRawValue(ifKit, args[0]->Int32Value(), &sensorRawValue);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(sensorRawValue));
}

Handle<Value> getSensorChangeTrigger(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, trigger;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getSensorChangeTrigger(ifKit, args[0]->Int32Value(), &trigger);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(trigger));
}

Handle<Value> setSensorChangeTrigger(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;
  
  if (args.Length() < 2)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber() || !args[1]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index or trigger argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_setSensorChangeTrigger(ifKit, args[0]->Int32Value(),  args[1]->Int32Value());
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> getRatiometric(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, ratiometric;
  const char *errorDescription;
 
  errorCode = CPhidgetInterfaceKit_getRatiometric(ifKit, &ratiometric);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(ratiometric));
}

Handle<Value> setRatiometric(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing ratiometric argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Ratiometric argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_setRatiometric(ifKit, args[0]->Int32Value());
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> getDataRate(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, milliseconds;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getDataRate(ifKit, args[0]->Int32Value(), &milliseconds);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(milliseconds));
}

Handle<Value> setDataRate(const Arguments& args) 
{
  HandleScope scope;
  int errorCode;
  const char *errorDescription;
  
  if (args.Length() < 2)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber() || !args[1]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index or milliseconds argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_setDataRate(ifKit, args[0]->Int32Value(),  args[1]->Int32Value());
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Undefined());
}

Handle<Value> getDataRateMax(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, max;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getDataRateMax(ifKit, args[0]->Int32Value(), &max);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(max));
}

Handle<Value> getDataRateMin(const Arguments& args) 
{
  HandleScope scope;
  int errorCode, min;
  const char *errorDescription;
  
  if (args.Length() < 1)
  {
    ThrowException(Exception::TypeError(String::New("Missing index argument")));
    return scope.Close(Undefined());
  }

  if (!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Index argument is not a number")));
    return scope.Close(Undefined());
  }
  
  errorCode = CPhidgetInterfaceKit_getDataRateMin(ifKit, args[0]->Int32Value(), &min);
  
  if (errorCode != 0)
  {
    CPhidget_getErrorDescription(errorCode, &errorDescription);
    ThrowException(Exception::TypeError(String::New(errorDescription)));
    return scope.Close(Undefined());
  }
  
  return scope.Close(Number::New(min));
}

void eventCallback(uv_async_t *handle, int status /*UNUSED*/)
{
  HandleScope scope;
  Baton *baton = (Baton*)handle->data;
  
  switch (baton->event)
  {
    case ATTACH:
    {
      Local<Value> args[] = { }; 
      node::MakeCallback(contextObj, "attachHandler", 0, args); 
      break;
    }
    case DETACH:
    {
      Local<Value> args[] = { }; 
      node::MakeCallback(contextObj, "detachHandler", 0, args); 
      break;
    }
    case ERROR:
    {
      Local<Value> args[] = { Number::New(baton->errorCode) }; 
      node::MakeCallback(contextObj, "errorHandler", 1, args); 
      break;
    }
    case INPUT_CHANGE:
    {
      Local<Value> args[] = { Number::New(baton->index), Number::New(baton->value) }; 
      node::MakeCallback(contextObj, "inputChangeHandler", 2, args); 
      break;
    }
    case OUTPUT_CHANGE:
    {
      Local<Value> args[] = { Number::New(baton->index), Number::New(baton->value) }; 
      node::MakeCallback(contextObj, "onputChangeHandler", 2, args); 
      break;
    }
    case SENSOR_CHANGE:
    {
      Local<Value> args[] = { Number::New(baton->index), Number::New(baton->value) }; 
      node::MakeCallback(contextObj, "sensorChangeHandler", 2, args); 
      break;
    }
  }
  
  delete baton;
}

void init(Handle<Object> target) 
{
  contextObj = Persistent<Object>::New(Object::New()); 
  
  target->Set(String::New("context"), contextObj); 
  target->Set(String::New("create"), FunctionTemplate::New(create)->GetFunction());
  target->Set(String::New("open"), FunctionTemplate::New(open)->GetFunction());
  target->Set(String::New("waitForAttachment"), FunctionTemplate::New(waitForAttachment)->GetFunction());
  target->Set(String::New("close"), FunctionTemplate::New(close)->GetFunction());
  target->Set(String::New("remove"), FunctionTemplate::New(remove)->GetFunction());
  target->Set(String::New("getDeviceName"), FunctionTemplate::New(getDeviceName)->GetFunction());
  target->Set(String::New("getSerialNumber"), FunctionTemplate::New(getSerialNumber)->GetFunction());
  target->Set(String::New("getDeviceVersion"), FunctionTemplate::New(getDeviceVersion)->GetFunction());
  target->Set(String::New("getDeviceStatus"), FunctionTemplate::New(getDeviceStatus)->GetFunction());
  target->Set(String::New("getLibraryVersion"), FunctionTemplate::New(getLibraryVersion)->GetFunction());
  target->Set(String::New("getDeviceType"), FunctionTemplate::New(getDeviceType)->GetFunction());
  target->Set(String::New("getInputCount"), FunctionTemplate::New(getInputCount)->GetFunction());
  target->Set(String::New("getInputState"), FunctionTemplate::New(getInputState)->GetFunction());
  target->Set(String::New("getOutputCount"), FunctionTemplate::New(getOutputCount)->GetFunction());
  target->Set(String::New("getOutputState"), FunctionTemplate::New(getOutputState)->GetFunction());
  target->Set(String::New("setOutputState"), FunctionTemplate::New(setOutputState)->GetFunction());
  target->Set(String::New("getSensorCount"), FunctionTemplate::New(getSensorCount)->GetFunction());
  target->Set(String::New("getSensorValue"), FunctionTemplate::New(getSensorValue)->GetFunction());
  target->Set(String::New("getSensorRawValue"), FunctionTemplate::New(getSensorRawValue)->GetFunction());
  target->Set(String::New("getSensorChangeTrigger"), FunctionTemplate::New(getSensorChangeTrigger)->GetFunction());
  target->Set(String::New("setSensorChangeTrigger"), FunctionTemplate::New(setSensorChangeTrigger)->GetFunction());
  target->Set(String::New("getRatiometric"), FunctionTemplate::New(getRatiometric)->GetFunction());
  target->Set(String::New("setRatiometric"), FunctionTemplate::New(setRatiometric)->GetFunction());
  target->Set(String::New("getDataRate"), FunctionTemplate::New(getDataRate)->GetFunction());
  target->Set(String::New("setDataRate"), FunctionTemplate::New(setDataRate)->GetFunction());
  target->Set(String::New("getDataRateMin"), FunctionTemplate::New(getDataRateMin)->GetFunction());
  
  uv_async_init(uv_default_loop(), &async, eventCallback);
}

NODE_MODULE(binding, init);
