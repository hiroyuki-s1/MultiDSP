

// CoreEngine クラスの前方宣言
class CoreEngine;
 

// オーディオコールバック関数の前方宣言

// CoreEngine クラスの定義
class CoreEngine
{
public:
    CoreEngine();
    void init(){
        hw_.Init();
        hw_.SetAudioBlockSize(4); // コールバックごとのサンプル数
        hw_.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
        hw_.StartAdc();
        p_knob1.Init(hw_.knob1, 0, 1, Parameter::LINEAR);
        p_knob2.Init(hw_.knob2, 0, 1, Parameter::LINEAR);
        hw_.StartAudio(audioCallback);  
        hw_.seed.StartLog(true);
        hw_.seed.PrintLine("hello world");

        // ディレイエフェクトの初期化
        delay_.Init();
        delay_.SetDelay(delay_time_ * hw_.AudioSampleRate());
        
        knob_1 = p_knob1.Process(); 
        knob_2 = p_knob2.Process(); 
        inc_ = -10;
    }

    // インスタンスメンバー関数
    void processAudio(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
    {
        hw_.ProcessAnalogControls();
        hw_.ProcessDigitalControls();
        inc_ += hw_.encoder.Increment();
        select_ += hw_.button1.RisingEdge();
        inc_ = inc_ % 6;
        knob_1 = p_knob1.Process(); 
        knob_2 = p_knob2.Process(); 
        hw_.led1.Set(knob_1, 0.0, knob_2);
        hw_.UpdateLeds();
    }
    void updateDelayTime()
    {
        // hw_.seed.PrintLine("hello world :");
        // hw_.seed.PrintLine("hello world : %.4f , %.4f",knob_1, knob_2);
        
        // hw_.seed.PrintLine("hello world");
        // ディレイタイムとフィードバック量を更新
        delay_time_ = knob_1; // 0～1の範囲をサンプル数にマッピング（最大1秒）
        feedback_ = knob_2;
        delay_.SetDelay(delay_time_ * hw_.AudioSampleRate());
    }

    void run(){
        init();
        int ds = 5;
        std::string DD = "dsddddddddddddddddddddddddddddddddddddddd\n";
        float data = 0.2f;
        hw_.seed.PrintLine("===============================");
        hw_.seed.PrintLine("Parametric EQ");
        hw_.seed.PrintLine("===============================");
        int32_t last_inc;
        while(1) {
            // メインループのコード
            if(last_inc != select_){
                hw_.seed.PrintLine("%d",select_);
            }
            last_inc = select_;
            hw_.DelayMs(10);
        }
            // hw_.seed.PrintLine("data :%.2f , %.2f, ebc: %d, %d, %d ,%.2f , button %d, %d",
            //     knob_1, knob_2, hw_.encoder.Pressed(),
            //     inc_, hw_.encoder.RisingEdge(),
            //     hw_.encoder.TimeHeldMs(),
            //     hw_.button1.Pressed(), hw_.button2.Pressed()                );
    }


    daisy::DaisyPod hw_;
private:
    DelayLine<float, 48000> delay_; // 1秒のディレイバッファ
    float feedback_; // フィードバック量
    float knob_1, knob_2;
    uint8_t select_;
    float delay_time_; // ディレイタイム（サンプル数）
    Parameter p_knob1, p_knob2;
    int32_t inc_;
    float level_;

};

// オーディオコールバック関数の定義
void audioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    if (core)
    {
        core->processAudio(in, out, size);
        // core->hw_.seed.PrintLine("ss");
    }
}
