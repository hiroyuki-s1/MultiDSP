#ifndef STANDARD_DELAY_H
#define STANDARD_DELAY_H
#include "daisysp.h"
#include "fx.h"
using namespace common;

enum class DELAY_PARAM:CHAR
{
    FEEDBACK,
    MIX,
    TIME,
};

class StandardDelay: public FX
{
public:
    virtual void init(uint16_t *sampling_rate_value) override {
        feedback_ = 0.0f;
        mix_ = 0.0f;
        time_ = 0.0f;
        effect_type = EFFECT_TYPE::STANDARD_DELAY;
        sampling_rate = sampling_rate_value;
        delay_.Init();
        effector_name_ = "StandardDelay";
    }


    virtual void process(const FLOAT& in, FLOAT& out) override{
        FLOAT delayed = delay_.Read();
        delay_.Write(in + delayed * feedback_);
        out = in + delayed * mix_;
    }

    virtual BOOL setParam(const CHAR& id, const UCHAR val) override { return false; }
    virtual BOOL setParam(const CHAR& id, const BOOL val) override { return false; }
    virtual BOOL setParam(const CHAR& id, const FLOAT val) override {
        BOOL result = false;
        LOG_INFO("set param : " + std::to_string(id));
        switch (static_cast<DELAY_PARAM>(id))
        {
        case DELAY_PARAM::FEEDBACK:
            feedback_ = val;
            result = true;
            break;
        case DELAY_PARAM::MIX:
            mix_ = val;
            result = true;
            break;
        case DELAY_PARAM::TIME:
            time_ = val;
            delay_.SetDelay(static_cast<size_t>(*sampling_rate / (1.0 + time_ * 10.0)));
            result = true;
            break;
        default:
            result = false;
            break;
        }
        return result;        
    }

    virtual ParamElements getParam(const CHAR& id) override {
        ParamElements result;
        switch (static_cast<DELAY_PARAM>(id))
        {
        case DELAY_PARAM::FEEDBACK:
            result = createParamElement(id, PARA_TYPE::T_FLOAT, feedback_, "feedback");
        case DELAY_PARAM::MIX:
            result = createParamElement(id, PARA_TYPE::T_FLOAT, mix_, "mix");
        case DELAY_PARAM::TIME:
            result = createParamElement(id, PARA_TYPE::T_FLOAT, time_, "time");
        default:
            result = createParamElement(NO_PARAMETER_ID, PARA_TYPE::T_UCHAR, 0, "none");
        }
        return result;
    };

    void setParamDefault() {
        setParam(
            static_cast<CHAR>(DELAY_PARAM::TIME),
            0.1f);
        setParam(
            static_cast<CHAR>(DELAY_PARAM::FEEDBACK),
            0.5f);
        setParam(
            static_cast<CHAR>(DELAY_PARAM::MIX),
            0.5f);
    }
    EFFECT_TYPE effect_type;
private:
    //daisysp::Tone tone;
#ifdef BUILD_DAISY
    daisysp::DelayLine<FLOAT, DELAY_MAX_SIZE> DSY_SDRAM_BSS delay_;
#else
    daisysp::DelayLine<FLOAT, DELAY_MAX_SIZE> delay_;
#endif 
    FLOAT feedback_;
    FLOAT mix_;
    FLOAT time_;
};

#endif