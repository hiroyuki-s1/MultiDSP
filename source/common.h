#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#ifdef DAISY_BUILD
#define LOG_INFO
#else
#define LOG_INFO juce::Logger::getCurrentLogger()->writeToLog
#endif
#define NAME_TOP1 "top1"
#define NAME_TOP2 "top2"
#define NAME_TOP3  "top3"
#define NAME_BOTTOM1  "bottom1"
#define NAME_BOTTOM2  "bottom2"
#define NAME_BOTTOM3  "bottom3"
namespace common {
    using FLOAT = float;
    using BOOL = bool;
    using UCHAR = unsigned char;
    using CHAR = char;
    using UINT = uint8_t;
    const CHAR NO_PARAMETER_ID = -1;
    const size_t DELAY_MAX_SIZE = 88200;
    enum CH : UCHAR {
        A,
        B,
    };



    enum KNOB_TYPE :UCHAR
    {
        TOP1,
        TOP2,
        TOP3,
        BOTTOM1,
        BOTTOM2,
        BOTTOM3,
        SIZE,
    };

    enum PARA_TYPE : UCHAR
    {
        T_BOOL,
        T_FLOAT,
        T_UCHAR,
    };

    /**
     * �G�t�F�N�^�[�̎��
     */
    enum EFFECT_TYPE : UCHAR
    {
        GRACHIC_EQ,
        STANDARD_DELAY,
        HALL_REVERB,
        EFFECT_SIZE,
    };

    /**
     * �G�t�F�N�^�[�̃p�����[�^���
     */
    struct ParamElements
    {
        CHAR id;           // �p�����[�^ID 
        std::string name;  //�p�����[�^��
        PARA_TYPE type;    // �p�����[�^�̌^
        UCHAR val_uchar;   // uchar�^�̒l
        FLOAT val_float;   // float�^�̒l
        BOOL  val_bool;    // bool�^�̒l
    };
}
#endif