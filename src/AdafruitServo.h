#ifndef ADAFRUITSERVO_H_
#define ADAFRUITSERVO_H_

#include <nan.h>

class AdafruitServo : public Nan::ObjectWrap {

public:
    AdafruitServo(const v8::FunctionCallbackInfo<v8::Value> &args);
    ~AdafruitServo();

    static v8::Local<v8::Function> sGetFunction();

private:
    /**
     * used by javascript ctro function
     */
    static void V8New(const v8::FunctionCallbackInfo<v8::Value> &args);
    /**
     * callback function for javascript function .setPulseWidth()
     */
    static void sSetPulseWidth(const v8::FunctionCallbackInfo<v8::Value> &args);
    static v8::Eternal<v8::Function> sFunction;

    /**
     * initialize 
     */
    void initialize();

    void SetPulseWidth(const v8::FunctionCallbackInfo<v8::Value> &args);
};

#endif /* ADAFRUITSERVO_H_ */
