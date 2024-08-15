#include "common.h"
#include "standard_delay.h"
using T_FX_PTR = std::unique_ptr<FX>;
using T_DELAY = StandardDelay;
class PedalBoard{
public:
	PedalBoard() {
		delay_ = std::make_unique<T_DELAY>();
	}
	void process(const FLOAT& in, FLOAT& out) {
		processEQ(in, out);
		processDelay(in, out);
		processReverb(in, out);
	}

	void init(uint16_t* sampling_rate_value) {
		delay_->init(sampling_rate_value);
		delay_->setParamDefault();
	}
	BOOL setParamDelay(const CHAR id, const FLOAT &val) {

		return delay_->setParam(id, val);
	}

	FLOAT getParam(const CHAR id) {
		return delay_->getParamFloat(id);
	}

private:
	// --------- M†ˆ— ---------- //
	void processDelay(const FLOAT& in, FLOAT& out) {
		delay_->process(in, out);
	}
	void processReverb(const FLOAT& in, FLOAT& out) {
		out = in;
	}
	void processEQ(const FLOAT& in, FLOAT& out) {
		out = in;
	}
	std::unique_ptr<T_DELAY> delay_;
};
