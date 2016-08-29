#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "AdafruitServo.h"

using namespace v8;

#define FUNCTION_TEMPLATE_CLASS "AdafruitServo"

v8::Eternal<v8::Function> AdafruitServo::sFunction;


/*static*/
void AdafruitServo::V8New(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    if (args.IsConstructCall())
    {
        new AdafruitServo(args);
        args.GetReturnValue().Set(args.This());
    }
    else
    {
        v8::Local<v8::Function> func = AdafruitServo::sGetFunction();
        v8::Local<v8::Value> newArgs[args.Length()];
        for ( int i = 0; i < args.Length(); i ++ )
        {
            newArgs[i] = args[i];
        }
        v8::Local<v8::Object> obj = func->NewInstance(Nan::GetCurrentContext(), args.Length(), newArgs).ToLocalChecked();
        args.GetReturnValue().Set(obj);
    }
}

/*static*/
void
AdafruitServo::sSetPulseWidth(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    AdafruitServo* _this = AdafruitServo::Unwrap<AdafruitServo>(args.Holder());
    if ( !_this )
    {
        args.GetIsolate()->ThrowException(
                v8::Exception::ReferenceError(Nan::New("not a valid AdafruitServo object").ToLocalChecked()));
    }
    if ( args.Length()  != 2 && !(args[0]->IsNumber()) &&
        !(args[1]->IsNumber()))
    {
        args.GetIsolate()->ThrowException(
                v8::Exception::SyntaxError(Nan::New("usage: setPulseWidth(channel, width)").ToLocalChecked()));
    }
    _this->SetPulseWidth(args);
}

/*static*/
v8::Local<v8::Function>
AdafruitServo::sGetFunction()
{
    Nan::EscapableHandleScope scope;
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    if ( sFunction.IsEmpty() ) {
        v8::Local<v8::FunctionTemplate> ftmpl = v8::FunctionTemplate::New(isolate, AdafruitServo::V8New);
        ftmpl->SetClassName(Nan::New(FUNCTION_TEMPLATE_CLASS).ToLocalChecked());
        v8::Local<v8::ObjectTemplate> otmpl = ftmpl->InstanceTemplate();
        otmpl->SetInternalFieldCount(1);
        otmpl->Set(Nan::New("setPulseWidth").ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, sSetPulseWidth, v8::Local<v8::Value>(), v8::Signature::New(isolate, ftmpl)));
        sFunction.Set(isolate, ftmpl->GetFunction());
    }
    return scope.Escape(sFunction.Get(isolate));
}

AdafruitServo::AdafruitServo(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    this->Wrap(args.This());
    initialize();
}

void AdafruitServo::initialize()
{
}

AdafruitServo::~AdafruitServo()
{
}

void AdafruitServo::SetPulseWidth(const FunctionCallbackInfo<v8::Value> &args)
{
//args.GetReturnValue().Set(rev);
}

NAN_MODULE_INIT(initialize) {
    Nan::HandleScope scope;
    Nan::Set(target, Nan::New(FUNCTION_TEMPLATE_CLASS).ToLocalChecked(),
            AdafruitServo::sGetFunction());

}

NODE_MODULE(adafruit_servo, initialize)
