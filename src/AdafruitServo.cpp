#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "AdafruitServo.h"
#include "I2Cdev.h"

using namespace v8;

#define FUNCTION_TEMPLATE_CLASS "AdafruitServo"

v8::Eternal<v8::Function> AdafruitServo::sFunction;

//# Registers/etc.
#define __MODE1               0x00
#define __MODE2               0x01
#define __SUBADR1             0x02
#define __SUBADR2             0x03
#define __SUBADR3             0x04
#define __PRESCALE            0xFE
#define __LED0_ON_L           0x06
#define __LED0_ON_H           0x07
#define __LED0_OFF_L          0x08
#define __LED0_OFF_H          0x09
#define __ALL_LED_ON_L        0xFA
#define __ALL_LED_ON_H        0xFB
#define __ALL_LED_OFF_L       0xFC
#define __ALL_LED_OFF_H       0xFD

//# Bits
#define __RESTART             0x80
#define __SLEEP               0x10
#define __ALLCALL             0x01
#define __INVRT               0x10
#define __OUTDRV              0x04

#define __REGISTRY            0x40
static I2Cdev* gI2Cdev = new I2Cdev("/dev/i2c-1");
static bool gInit = false;
float tick = 20000.0/4096.0;

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
    if (!gInit) { //JavaScript is single thread, don't worry
        gInit = true;
        //"Sets a all PWM channels"
        gI2Cdev->writeByte(__REGISTRY, __ALL_LED_ON_L, 0 & 0xFF);
        gI2Cdev->writeByte(__REGISTRY, __ALL_LED_ON_H, 0 >> 8);
        gI2Cdev->writeByte(__REGISTRY, __ALL_LED_OFF_L, 0 & 0xFF);
        gI2Cdev->writeByte(__REGISTRY, __ALL_LED_OFF_H, 0 >> 8);
        gI2Cdev->writeByte(__REGISTRY, __MODE2, __OUTDRV);
        gI2Cdev->writeByte(__REGISTRY, __MODE1, __ALLCALL);
        usleep(5);
        unsigned char mode1;
        gI2Cdev->readByte(__REGISTRY, __MODE1, &mode1);
        mode1 = mode1 & ~__SLEEP;//                 # wake up (reset sleep)
        gI2Cdev->writeByte(__REGISTRY, __MODE1, mode1);
        usleep(5);//                             # wait for oscillator
        //set freq to 50
        //"Sets the PWM frequency"
        float prescaleval = 25000000.0/4096/50;//# 25MHz, 12-bit, 50 freq
        prescaleval -= 1.0;
        double prescale = floor(prescaleval + 0.5);

        unsigned char newmode = (mode1 & 0x7F) | 0x10;             //# sleep
        gI2Cdev->writeByte(__REGISTRY, __MODE1, newmode);        //# go to sleep
        gI2Cdev->writeByte(__REGISTRY, __PRESCALE, (int)(floor(prescale)));
        gI2Cdev->writeByte(__REGISTRY, __MODE1, mode1);
        usleep(5);
        gI2Cdev->writeByte(__REGISTRY,__MODE1, mode1 | 0x80);
    }
}

AdafruitServo::~AdafruitServo()
{
}

void AdafruitServo::SetPulseWidth(const FunctionCallbackInfo<v8::Value> &args)
{
//args.GetReturnValue().Set(rev);

    v8::Local<Context> ctx = args.GetIsolate()->GetCurrentContext();
    int32_t channel = args[0]->Int32Value(ctx).FromMaybe(0);
    int32_t pulsewidth = args[1]->Int32Value(ctx).FromMaybe(0);
    pulsewidth = (int32_t)((float)(pulsewidth)/tick);

    gI2Cdev->writeByte(__REGISTRY,__LED0_ON_L+4*channel, 0 & 0xFF);
    gI2Cdev->writeByte(__REGISTRY,__LED0_ON_H+4*channel, 0 >> 8);
    gI2Cdev->writeByte(__REGISTRY,__LED0_OFF_L+4*channel, pulsewidth & 0xFF);
    gI2Cdev->writeByte(__REGISTRY,__LED0_OFF_H+4*channel, pulsewidth >> 8);
}

NAN_MODULE_INIT(initialize) {
    Nan::HandleScope scope;
    Nan::Set(target, Nan::New(FUNCTION_TEMPLATE_CLASS).ToLocalChecked(),
            AdafruitServo::sGetFunction());

}

NODE_MODULE(adafruit_servo, initialize)
