#include <node.h>
#include <v8.h>
#include <phidget21.h>
#include <vector>
#include <string>

using namespace v8;

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
    std::string errorString;
    int index;
    int value;
    long handle;
};

static std::vector<Baton*> batons;
Persistent<Object> contextObj;
static uv_async_t async;
static uv_mutex_t mutex;

int CCONV attachHandler(CPhidgetHandle handle, void *userptr)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = ATTACH;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV detachHandler(CPhidgetHandle handle, void *userptr)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = DETACH;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV errorHandler(CPhidgetHandle handle, void *userptr, int errorCode, const char *errorString)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = ERROR;
    baton->errorString = errorString;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV inputChangeHandler(CPhidgetInterfaceKitHandle handle, void *usrptr, int index, int value)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = INPUT_CHANGE;
    baton->index = index;
    baton->value = value;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV outputChangeHandler(CPhidgetInterfaceKitHandle handle, void *usrptr, int index, int value)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = OUTPUT_CHANGE;
    baton->index = index;
    baton->value = value;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

int CCONV sensorChangeHandler(CPhidgetInterfaceKitHandle handle, void *usrptr, int index, int value)
{
    Baton *baton = new Baton;
    baton->handle = (long)handle;
    baton->event = SENSOR_CHANGE;
    baton->index = index;
    baton->value = value;

    uv_mutex_lock(&mutex);
    batons.push_back(baton);
    uv_mutex_unlock(&mutex);

    uv_async_send(&async);

    return 0;
}

Handle<Value> create(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;
    CPhidgetHandle handle = 0;

    errorCode = CPhidgetInterfaceKit_create((CPhidgetInterfaceKitHandle*)&handle);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_set_OnAttach_Handler(handle, attachHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_set_OnDetach_Handler(handle, detachHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_set_OnError_Handler(handle, errorHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_set_OnInputChange_Handler((CPhidgetInterfaceKitHandle)handle, inputChangeHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_set_OnSensorChange_Handler((CPhidgetInterfaceKitHandle)handle, sensorChangeHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_set_OnOutputChange_Handler((CPhidgetInterfaceKitHandle)handle, outputChangeHandler, NULL);

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Number::New((long)handle));
}

Handle<Value> open(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription;

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or serial number argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or serial number argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_open((CPhidgetHandle)args[0]->IntegerValue(), args[1]->Int32Value());

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or milliseconds argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or milliseconds argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_waitForAttachment((CPhidgetHandle)args[0]->IntegerValue(), args[1]->Int32Value());

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_close((CPhidgetHandle)args[0]->IntegerValue());

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_delete((CPhidgetHandle)args[0]->IntegerValue());

    if (errorCode != 0)
    {
        CPhidget_getErrorDescription(errorCode, &errorDescription);
        ThrowException(Exception::TypeError(String::New(errorDescription)));
        return scope.Close(Undefined());
    }

    return scope.Close(Undefined());
}

Handle<Value> getDeviceName(const Arguments& args)
{
    HandleScope scope;
    int errorCode;
    const char *errorDescription, *deviceName;

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceName((CPhidgetHandle)args[0]->IntegerValue(), &deviceName);

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getSerialNumber((CPhidgetHandle)args[0]->IntegerValue(), &serialNumber);

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceVersion((CPhidgetHandle)args[0]->IntegerValue(), &deviceVersion);

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceStatus((CPhidgetHandle)args[0]->IntegerValue(), &deviceStatus);

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidget_getDeviceType((CPhidgetHandle)args[0]->IntegerValue(), &deviceType);

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getInputCount((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), &count);

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getInputState((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &inputState);

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getOutputCount((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), &count);

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getOutputState((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &outputState);

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

    if (args.Length() < 3)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle, index or outputState argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle, index or outputState argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_setOutputState((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(),  args[2]->Int32Value());

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getSensorCount((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), &count);

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getSensorValue((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &sensorValue);

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getSensorRawValue((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &sensorRawValue);

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getSensorChangeTrigger((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &trigger);

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

    if (args.Length() < 3)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle, index or trigger argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle, index or trigger argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_setSensorChangeTrigger((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), args[2]->Int32Value());

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

    if (args.Length() < 1)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getRatiometric((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), &ratiometric);

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or ratiometric argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or ratiometric argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_setRatiometric((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value());

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getDataRate((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &milliseconds);

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

    if (args.Length() < 3)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle, index or milliseconds argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber() || !args[2]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle, index or milliseconds argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_setDataRate((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), args[2]->Int32Value());

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getDataRateMax((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &max);

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

    if (args.Length() < 2)
    {
        ThrowException(Exception::TypeError(String::New("Missing handle or index argument")));
        return scope.Close(Undefined());
    }

    if (!args[0]->IsNumber() || !args[1]->IsNumber())
    {
        ThrowException(Exception::TypeError(String::New("Handle or index argument is not a number")));
        return scope.Close(Undefined());
    }

    errorCode = CPhidgetInterfaceKit_getDataRateMin((CPhidgetInterfaceKitHandle)args[0]->IntegerValue(), args[1]->Int32Value(), &min);

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
    uv_mutex_lock(&mutex);

    for (unsigned i = 0; i < batons.size(); i++)
    {
        Baton *baton = batons[i];

        switch (baton->event)
        {
            case ATTACH:
            {
                Local<Value> args[] = { Number::New(baton->handle) };
                node::MakeCallback(contextObj, "attachHandler", 1, args);
                break;
            }
            case DETACH:
            {
                Local<Value> args[] = { Number::New(baton->handle) };
                node::MakeCallback(contextObj, "detachHandler", 1, args);
                break;
            }
            case ERROR:
            {
                Local<Value> args[] = { Number::New(baton->handle), String::New(baton->errorString) };
                node::MakeCallback(contextObj, "errorHandler", 2, args);
                break;
            }
            case INPUT_CHANGE:
            {
                Local<Value> args[] = { Number::New(baton->handle), Number::New(baton->index), Number::New(baton->value) };
                node::MakeCallback(contextObj, "inputChangeHandler", 3, args);
                break;
            }
            case OUTPUT_CHANGE:
            {
                Local<Value> args[] = { Number::New(baton->handle), Number::New(baton->index), Number::New(baton->value) };
                node::MakeCallback(contextObj, "outputChangeHandler", 3, args);
                break;
            }
            case SENSOR_CHANGE:
            {
                Local<Value> args[] = { Number::New(baton->handle), Number::New(baton->index), Number::New(baton->value) };
                node::MakeCallback(contextObj, "sensorChangeHandler", 3, args);
                break;
            }
        }

        delete baton;
    }

    batons.erase(batons.begin(), batons.end());
    uv_mutex_unlock(&mutex);
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
