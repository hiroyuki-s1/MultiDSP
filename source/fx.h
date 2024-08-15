#ifndef FX_H
#define FX_H
#include <stdint.h>
#include <string.h>
#include "common.h"
using namespace common;
class FX
{
public:
    // Initialize the effect
    virtual void init(uint16_t *sampling_rate_value) = 0;
    std::string getPedalName() {
        return effector_name_;
    }

    virtual void process(const  FLOAT &in, FLOAT &out) = 0;
    virtual BOOL setParam(const CHAR &id, const UCHAR val) = 0;
    virtual BOOL setParam(const CHAR &id, const BOOL val)  = 0;
    virtual BOOL setParam(const CHAR &id, const FLOAT val) = 0;
     
    virtual ParamElements getParam(const CHAR& id) = 0;

    UCHAR getParamUChar(const CHAR& id) {
        return getParam(id).val_uchar;
    }
    BOOL getParamBool(const CHAR& id) {
        return getParam(id).val_bool;
    }
    FLOAT getParamFloat(const CHAR& id) {
        return getParam(id).val_float;
    }

protected:
    ParamElements createParamElement(const CHAR& id, PARA_TYPE type, FLOAT float_val = 0.0f, BOOL bool_val = false, UCHAR uchar_val = 0, std::string name = "") const {
        ParamElements result;
        result.id = id;
        result.type = type;
        result.val_float = float_val;
        result.val_bool = bool_val;
        result.val_uchar = uchar_val;
        result.name = name;
        return result;
    }
    std::string effector_name_;
    uint16_t* sampling_rate;

};

// A global variable to store sample rate
#endif