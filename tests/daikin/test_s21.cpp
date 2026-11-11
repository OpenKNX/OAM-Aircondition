#include <array>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <string_view>
#include <cassert>
#include <iostream>
#include <deque>
#define private public
#define protected public
#include "DaikinDriver.h"
#undef private
#undef protected
#include "hal/SerialHAL_Factory.h"

struct FakeHAL : daikin::hal::SerialHAL {
    std::deque<uint8_t> rx;
    std::vector<uint8_t> tx;
    bool begin(int,int,bool,bool,uint32_t) override { return true; }
    size_t available() override { return rx.size(); }
    int read(uint8_t* b,size_t n) override { size_t k=0; while(k<n && !rx.empty()) { b[k++]=rx.front(); rx.pop_front(); } return k; }
    size_t write(const uint8_t* b,size_t n) override { tx.insert(tx.end(),b,b+n); return n; }
    bool flush(uint32_t) override { assert(false && "Blocking flush"); return false; }
    void clearRxBuffer() override { rx.clear(); }
};
std::unique_ptr<daikin::hal::SerialHAL> daikin::hal::SerialHAL_Factory::create(HardwareSerial&) { return std::make_unique<FakeHAL>(); }
using Result = daikin::DaikinSerial::Result;
struct Transport {
    std::vector<Result> results;
    std::vector<uint8_t> payload;
    daikin::DaikinSerial serial{Serial1,16,17,false,false,[this](Result r,uint8_t* b,size_t n){ results.push_back(r); if(n) payload.assign(b,b+n); }};
    FakeHAL& hal() { return *static_cast<FakeHAL*>(serial.serial_hal_.get()); }
    void receive(std::initializer_list<uint8_t> bytes) { hal().rx.insert(hal().rx.end(),bytes); while(!hal().rx.empty()) serial.loop(); }
};
struct Feedback : AirConditionDriverStatusFeedback {
    void driverStateChanged(AirConditionDriverState state, std::string error) override { }
    AirConditionDriverState getDriverState() const override { return AirConditionDriverStateOk; }
    void powerChanged(bool power) override { }
    void modeChanged(AirConditionMode mode) override { }
    void targetTemperatureChanged(float temperaturCelius, bool isFeedbackFromSettin) override { }
    void fanSpeedChanged(int speed) override { }
    void swingHorizontalChanged(bool swing) override { }
    void swingVerticalChanged(bool swing) override { }
    void swingHorizontalFixPositionChanged(int position) override { }
    void swingVerticalFixPositionChanged(int position) override { }
    void roomTemperatureChanged(float temperaturCelius) override { }
    void outsideTemperaturChanged(float temperaturCelius) override { }
    void deviceModeChanged(AirConditionDeviceMode mode) override { }
    void maxPowerLevelChanged(uint8_t percentage) override { }
    void airPurificationChanged(bool on) override { }
    void updatePower(bool power) override { }
    void updateMode(AirConditionMode mode) override { }
    void updateTargetTemperature(float temperaturCelius) override { }
    void updateFanSpeed(int speed) override { }
    void updateSwingHorizontal(bool swing) override { }
    void updateSwingVertical(bool swing) override { }
    void updateCurrentTemperature(float temperaturCelius) override { }
    void updateOutdoorTemperature(float temperaturCelius) override { }
    void updateDeviceMode(AirConditionDeviceMode mode) override { }
    void updateMaxPowerLevel(uint8_t percentage) override { }
    void updateAirPurification(bool on) override { }
    void updateOnlineStatus(bool online) override { }
    void updateWifiLed(bool on) override { }
    void updateHumidity(uint8_t humidity) override { }
    void updateHumidityMode(uint8_t step) override { }
    void updateTotalEnergyConsumption(uint32_t totalEnergyWh) override { }
};
void test_handshake() {
    Transport t;
    t.serial.send_frame("F1");
    assert((t.hal().tx == std::vector<uint8_t>{2,'F','1',0x77,3}));
    t.receive({6,2,'G','1','1','4','F','A',0x64,3});
    assert((t.results == std::vector<Result>{Result::Ack,Result::Frame}));
    assert((t.payload == std::vector<uint8_t>{'G','1','1','4','F','A'}));
    assert(t.hal().tx.back()==6);
}
void test_checksums() {
    for (uint8_t sum : {2,3,6,0x15,0x42,4,5,8,0x17}) {
        Transport t;
        uint8_t value=uint8_t(sum-'F'-'1');
        t.serial.send_frame("F1", &value, 1);
        uint8_t wire=(sum==2||sum==3||sum==6||sum==0x15)?sum+2:sum;
        assert(t.hal().tx[t.hal().tx.size()-2]==wire);
        t.receive({2,'G','1',uint8_t(sum-'G'-'1'),wire,3});
        assert(t.results.back()==Result::Frame);
        assert(t.payload[2]==uint8_t(sum-'G'-'1'));
    }
    Transport t;
    t.serial.send_frame("F1");
    t.receive({2,'G','1',0x79,3});
    assert(t.results.back()==Result::Error);
    assert(t.hal().tx.back()==3); // no final ACK for bad checksum
}
void test_recovery() {
    Transport t;
    t.serial.send_frame("F1"); test_now+=1100; t.serial.loop();
    assert(t.results.back()==Result::Timeout && !t.serial.busy());
    t.serial.send_frame("D1"); t.receive({6});
    assert(!t.serial.busy());
    t.serial.send_frame("F1"); t.receive({0x15});
    assert(t.results.back()==Result::Nak && !t.serial.busy());
    t.serial.send_frame("F1"); t.receive({6,0x55,2,'G'}); test_now+=1000; t.serial.loop();
    assert(t.results.back()==Result::Timeout && !t.serial.busy());
    t.serial.send_frame("F1"); t.receive({2,'G',2,'G','1','1','4','F','A',0x64,3});
    assert(t.results.back()==Result::Frame);
    t.serial.send_frame("FU05");
    t.receive({2,'G','U','0','5'});
    for(int i=0;i<80;i++) t.receive({'A'});
    assert(t.results.back()==Result::Error && !t.serial.busy());
}
void test_no_unframed_fallback() {
    Transport t;
    for(int i=0;i<12;i++) {
        t.hal().tx.clear(); t.serial.send_frame("F8");
        assert(t.hal().tx.front()==2 && t.hal().tx.back()==3);
        test_now+=1100; t.serial.loop();
    }
}
void test_correlation() {
    Feedback feedback; DaikinDriver d(feedback); d.setup();
    d.queries_.emplace_back("FU04"); d.query_index_=0;
    d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    uint8_t wrong[]={'G','U','0','2','0'};
    d.handle_serial_result(Result::Frame,wrong,sizeof(wrong));
    assert(d.query_index_==0 && d.queries_[0].response_data.empty());
    uint8_t right[]={'G','U','0','4','0'};
    d.handle_serial_result(Result::Frame,right,sizeof(right));
    assert(d.query_index_==1 && d.queries_[0].response_data.size()==1);
    d.query_index_=0; d.query_state_=DaikinDriver::QueryState::Idle;
    d.handle_serial_result(Result::Frame,right,sizeof(right)); assert(d.query_index_==0);
}
void test_d6() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t readback[]={0x70,0xB0,0x31,0x34};
    d.handle_f6_response(readback,4);
    assert(d.d6_.write==DaikinDriver::CommandSupport::Unknown);
    d.pending_.climate.powerful=true; d.pending_.activate_powerful=true; d.sendPowerfulCommand();
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[1]=='D' && tx[2]=='6');
    assert(tx[3]==0x72 && tx[4]==0xB0 && tx[5]==0x31 && tx[6]==0x34);
    d.handle_serial_result(Result::Nak,nullptr,0);
    assert(d.d6_.write==DaikinDriver::CommandSupport::Unsupported);
    assert(!d.supportExternalRoomTemperatureSensor());
}
void test_econo() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g7[]={0x44,'0','A','G'}; d.handle_f7_response(g7,4);
    d.pending_.climate.econo=true; d.pending_.activate_econo=true; d.sendEconoCommand();
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[1]=='D' && tx[2]=='7' && tx[3]==0x44 && tx[4]=='2' && tx[5]=='A' && tx[6]=='G');
    assert(!d.d7_.valid); // cannot reuse stale readback on the next change
}
void test_demand() {
    Feedback f; DaikinDriver d(f); d.setup(); d.support_.demand=true;
    uint8_t g7[]={0x30,'2','A','G'}; d.handle_f7_response(g7,4);
    assert(d.state_.demand_percentage==100);
    d.setMaxPowerLevel(75); assert(!d.demand_pending_);
    assert(!d.configureDemandLimits(40,0)); assert(d.configureDemandLimits(40,5));
    d.setMaxPowerLevel(75);
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[3]==0x49 && tx[4]=='2' && tx[5]=='A' && tx[6]=='G');
    g7[0]=0x49; d.handle_f7_response(g7,4); assert(d.state_.demand_percentage==75);
    d.setMaxPowerLevel(101); assert(!d.demand_pending_);
}
void test_led() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g6[]={0x30,0x30,0x30,0x34};
    for (auto pair : {std::pair<uint8_t,daikin::LedState>{0x34,daikin::LedState::On},
                      {0x38,daikin::LedState::Dark},{0x3C,daikin::LedState::Off}}) {
        g6[3]=pair.first; d.handle_f6_response(g6,4); assert(d.state_.ledState==pair.second);
    }
    d.support_.sensor_hardware=true; g6[3]=0x3C; d.handle_f6_response(g6,4);
    assert(d.state_.sensor && d.state_.sensorLed);
}
void test_explicit_d1() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g1[]={'1','3',0x4C,'A'}; d.handle_f1_response(g1,4); // Cool, 24 C
    // Coalesce all explicit settings while a previous write is settling.
    d.query_state_=DaikinDriver::QueryState::PostWriteSettle;
    d.setMode(AirConditionModeAuto); d.setTargetTemperature(22); d.setPower(false); d.setFanSpeed(1);
    d.query_state_=DaikinDriver::QueryState::Idle; d.sendClimateCommand();
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[3]=='0' && tx[4]=='1' && tx[5]==0x48 && tx[6]=='B');
    assert(d.climate_dirty_==15); // transmission alone is not confirmation
}
void test_confirmed_d1() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g1[]={'1','3',0x4C,'A'}; d.handle_f1_response(g1,4);
    d.state_.online=true; d.gate_publish_until_full_sample_=false;
    d.setTargetTemperature(22);
    assert(d.state_.targetC==24); // queued write cannot become confirmed state
    d.handle_serial_result(Result::Ack,nullptr,0); assert(d.state_.targetC==24);
    g1[2]=0x48; d.handle_f1_response(g1,4);
    assert(d.state_.targetC==22 && !(d.climate_dirty_ & DaikinDriver::DirtyTemperature));
}
void test_swing_preserves_humidity() {
    Feedback f; DaikinDriver d(f); d.setup(); d.support_.swing=true;
    uint8_t g5[]={'0','0',0x3C,0x80}; d.handle_f5_response(g5,4);
    d.setSwingVertical(true);
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[2]=='5' && tx[3]=='1' && tx[4]=='?' && tx[5]==0x3C && tx[6]==0x80);
}
void test_humidity() {
    Feedback f; DaikinDriver d(f); d.setup(); d.humidity_modes_=0xB7;
    uint8_t g5[]={'1','?',0x3A,0x80};
    d.state_.mode=daikin::Mode::Heat; d.handle_f5_response(g5,4);
    assert(d.state_.humidityMode==daikin::HumidityMode::High);
    d.state_.mode=daikin::Mode::Cool; d.handle_f5_response(g5,4);
    assert(d.state_.humidityMode==daikin::HumidityMode::Low);
    assert(d.setHumidityMode(daikin::HumidityMode::High));
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[3]=='1' && tx[4]=='?' && tx[5]==0x3C && tx[6]==0x80);
    d.state_.mode=daikin::Mode::Dry; assert(!d.setHumidityMode(daikin::HumidityMode::Low));
    d.humidity_modes_=0; assert(!d.setHumidityMode(daikin::HumidityMode::High));
}
void test_no_f3_special_fallback() {
    Feedback f; DaikinDriver d(f); d.setup(); d.state_.powerful=true;
    uint8_t timers[]={'0',0xFE,0xFE,0}; d.handle_f3_response(timers,4);
    assert(d.state_.powerful);
    d.queries_.emplace_back("F6"); d.queries_[0].bad=true;
    d.checkF6ToF3Fallback(); assert(d.queries_[0].command=="F6");
}
void test_capability_bits() {
    Feedback f; DaikinDriver d(f); d.setup();
    for (size_t size=0;size<4;size++) {
        std::vector<uint8_t> truncated(size,0xFF); d.handle_f2_response(truncated.data(),size);
        assert(!d.support_.swing);
    }
    uint8_t g2[]={0x3C,0x32,0,0x90}; d.handle_f2_response(g2,4);
    assert(d.support_.swing && d.support_.horizontal_swing && d.humidity_modes_==0x92);
    assert(!d.support_.sensor && !d.support_.streamer && !d.support_.led && !d.support_.fan);
    uint8_t gk[]={0x30,0x38,0x34,0x31}; d.protocol_version_={3,20}; d.handle_fk_response(gk,4);
    assert(d.support_.sensor_hardware && d.support_.fan && d.support_.demand);
    std::array<uint8_t,32> gu{}; gu.fill('0'); gu[0]=gu[1]=gu[5]='3';
    d.handle_fu_fx_response("FU00",gu.data(),gu.size());
    assert(d.support_.powerful && d.support_.econo && d.support_.streamer);
}
void test_special_decoders() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g6[]={0xF2,0xB0,0x30,0x34}; d.handle_f6_response(g6,4);
    assert(d.state_.powerful && d.state_.comfort && d.state_.quiet && d.state_.streamer);
    for (size_t n=0;n<4;n++) d.handle_f6_response(g6,n);
    g6[0]=2; d.handle_f6_response(g6,4); assert(d.d6_.confirmed[0]==0xF2);
    uint8_t g7[]={0x94,'2','A','G'}; d.handle_f7_response(g7,4);
    assert(d.state_.demand_percentage==0 && d.state_.econo);
    g7[0]=0xFF; d.handle_f7_response(g7,4); assert(d.d7_.confirmed[0]==0x94);
}
void test_versions() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g8[]={'0','2',0,0}; d.handle_f8_response(g8,4); assert(d.protocol_version_.major==2);
    uint8_t g1[]={'0','1',0,0}; d.handle_f8_response(g1,4); assert(d.protocol_version_.major==1);
    for (uint8_t minor : {20,40}) {
        uint8_t gy[]={'0',uint8_t('0'+minor/10),'3','0'};
        d.handle_fy00_response(gy,4); assert(d.protocol_version_==(daikin::ProtocolVersion{3,minor}));
    }
    for (uint8_t major : {0,2,3}) {
        d.protocol_version_={major,40}; d.initializeQueries();
        auto has=[&](std::string_view cmd){ return std::any_of(d.queries_.begin(),d.queries_.end(),[&](const DaikinQueryState& q){return q.command==cmd;}); };
        assert(has("F3") && has("F5") && has("RH") && has("Rg"));
        assert(has("FK")==(major>=2));
    }
    d.protocol_version_=daikin::ProtocolUndetected; d.initializeProtocolDetection();
    d.query_state_=DaikinDriver::QueryState::WaitingForAck; d.handle_serial_result(Result::Nak,nullptr,0);
    d.query_state_=DaikinDriver::QueryState::WaitingForAck; d.handle_serial_result(Result::Nak,nullptr,0);
    assert(d.protocol_version_==daikin::ProtocolUnknown);
}
void test_rz_identity() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t b2[]={'C','0'}; d.handle_rzb2_response(b2,2); assert(d.state_.unitState.raw==0x0C);
    uint8_t garbage[]={0x80,0xFF}; d.handle_rzb2_response(garbage,2); assert(d.state_.unitState.raw==0x0C);
    d.protocol_version_={3,20}; d.initializeQueries();
    for(auto cmd : {"Rz52","Rz72"}) {
        auto it=std::find_if(d.queries_.begin(),d.queries_.end(),[&](const DaikinQueryState& q){return q.command==cmd;});
        assert(it!=d.queries_.end() && it->is_static && !it->handler);
    }
}
void test_query_rates() {
    Feedback f; DaikinDriver d(f); d.setup(); d.protocol_version_={3,40}; d.initializeQueries();
    for(const auto& q : d.queries_) {
        if(q.command=="FX60") assert(!q.is_static && q.interval_ms==10000);
        if(q.command=="FU05" || q.command=="FU35" || q.command=="FX00") assert(q.is_static);
    }
}
void test_read_write_capabilities() {
    Feedback f; DaikinDriver d(f); d.setup(); d.queries_.emplace_back("F6");
    using A=DaikinDriver::FeatureAccess;
    assert(d.commandAccess("F6")==A::Unknown);
    d.queries_[0].acked=true; assert(d.commandAccess("F6")==A::ReadOnly);
    d.d6_.write=DaikinDriver::CommandSupport::Unsupported; assert(d.commandAccess("F6")==A::ReadOnly);
    d.d6_.write=DaikinDriver::CommandSupport::Supported; assert(d.commandAccess("F6")==A::Writable);
    d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    d.handle_serial_result(Result::Timeout,nullptr,0);
    assert(!d.queries_[0].bad && d.queries_[0].naks==0);
}
void test_f4() {
    Feedback f; DaikinDriver d(f); uint8_t error[]={'0',0,0xA0,0};
    d.handle_f4_response(error,3); assert(!d.telemetry().serialErrorValid);
    d.handle_f4_response(error,4); assert(d.telemetry().serialError && d.telemetry().serialErrorSamples==1);
    error[2]=0x80; d.handle_f4_response(error,4); assert(!d.telemetry().serialError);
}
void test_timer_lamp() {
    Feedback f; DaikinDriver d(f); uint8_t gb[]={'1','0','0','0'};
    d.handle_fb_response(gb,4); assert(d.telemetry().timerLamp && d.telemetry().led);
    gb[0]='0'; d.handle_fb_response(gb,4); assert(!d.telemetry().timerLamp && d.telemetry().led);
}
void test_vertical_position() {
    Feedback f; DaikinDriver d(f); d.setup(); uint8_t gr[]={'0','0','0','0'};
    d.handle_fr_response(gr,4); assert(d.getMaximumVerticalFixPosition()==0);
    d.setSwingVerticalFixPosition(3);
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[1]=='D' && tx[2]=='R' && tx[3]=='3');
    d.handle_serial_result(Result::Ack,nullptr,0); gr[0]='3'; d.handle_fr_response(gr,4);
    assert(d.getMaximumVerticalFixPosition()==5 && d.getMaximumHorizontalFixPosition()==0);
    gr[0]='?'; d.handle_fr_response(gr,4); assert(!d.telemetry().verticalPositionValid);
}
void test_independent_status() {
    Feedback f; DaikinDriver d(f); uint8_t power[]={'1'}, mode[]={'7'}, target[]={'1','2','0','+'};
    d.handle_status_sensor("RA",power,1); d.handle_status_sensor("RB",mode,1); d.handle_status_sensor("RC",target,4);
    assert(d.telemetry().independentPower && d.telemetry().independentMode==7 && d.telemetry().coarseTargetC==21);
    assert(d.state_.targetC==22); // diagnostics do not overwrite precise feedback
}
void test_indoor_coil() {
    Feedback f; DaikinDriver d(f); uint8_t si[]={'5','7','2','+'};
    d.handle_ri_response(si,4); assert(d.telemetry().indoorHeatExchangerValid && d.telemetry().indoorHeatExchangerC==27.5f);
    si[3]='-'; d.handle_ri_response(si,4); assert(d.telemetry().indoorHeatExchangerC==-27.5f);
    for(size_t n=0;n<4;n++) { d.handle_ri_response(si,n); assert(!d.telemetry().indoorHeatExchangerValid); }
}
int main() { test_indoor_coil(); test_independent_status(); test_vertical_position(); test_timer_lamp(); test_f4(); test_read_write_capabilities(); test_query_rates(); test_rz_identity(); test_versions(); test_special_decoders(); test_capability_bits(); test_no_f3_special_fallback(); test_humidity(); test_swing_preserves_humidity(); test_confirmed_d1(); test_explicit_d1(); test_led(); test_demand(); test_econo(); test_d6(); test_handshake(); test_checksums(); test_recovery(); test_no_unframed_fallback(); test_correlation(); std::cout << "S21 native tests passed\n"; }
