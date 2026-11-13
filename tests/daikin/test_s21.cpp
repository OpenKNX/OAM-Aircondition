#include <array>
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <string_view>
#include <cassert>
#include <iostream>
#include <deque>
#include <fstream>
#include <sstream>
#define private public
#define protected public
#include "DaikinDriver.h"
#undef private
#undef protected
#include "hal/SerialHAL_Factory.h"
#include "s21_protocol.h"

struct FakeHAL : daikin::hal::SerialHAL {
    std::deque<uint8_t> rx;
    std::vector<uint8_t> tx;
    bool fail_writes{false};
    bool begin(int,int,bool,bool,uint32_t) override { return true; }
    size_t available() override { return rx.size(); }
    int read(uint8_t* b,size_t n) override { size_t k=0; while(k<n && !rx.empty()) { b[k++]=rx.front(); rx.pop_front(); } return k; }
    size_t write(const uint8_t* b,size_t n) override { if(fail_writes) return 0; tx.insert(tx.end(),b,b+n); return n; }
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
    bool humidityControlValid{false}; uint8_t humidityLevel{0}, humidityLevels{0}, humidityModes{0};
    void updateHumidityControl(bool valid,AirConditionHumidityLevel level,uint8_t available,uint8_t modes) override {humidityControlValid=valid;humidityLevel=uint8_t(level);humidityLevels=available;humidityModes=modes;}
    AirConditionOptionalState options;
    void updateOptionalControls(const AirConditionOptionalState& s) override {options=s;}
    AirConditionTimerStatus timers;
    void updateTimerStatus(const AirConditionTimerStatus& s) override {timers=s;}
    AirConditionEnergyTelemetry energy;
    void updateEnergyTelemetry(const AirConditionEnergyTelemetry& s) override {energy=s;}
    AirConditionTelemetry extra;
    void updateAdditionalTelemetry(const AirConditionTelemetry& s) override {extra=s;}
    unsigned humidity_mode_updates{0};
    bool positionValid{false}; uint8_t positionCode{0}, positionMax{0}, positionMask{0};
    void updateVerticalPositionStatus(bool valid,uint8_t code,uint8_t maximum,uint8_t mask) override {positionValid=valid;positionCode=code;positionMax=maximum;positionMask=mask;}
    AirConditionDriverState driverState{AirConditionDriverStateNotStarted};
    unsigned driverStateUpdates{0};
    void driverStateChanged(AirConditionDriverState state, std::string error) override { driverState=state; ++driverStateUpdates; }
    AirConditionDriverState getDriverState() const override { return driverState; }
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
    void updateHumidityMode(uint8_t step) override { ++humidity_mode_updates; }
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
    uint8_t right[36]={'G','U','0','4'}; std::fill(right+4,right+36,'0');
    d.handle_serial_result(Result::Frame,right,sizeof(right));
    assert(d.query_index_==1 && d.queries_[0].response_data.size()==32);
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
    assert(d.d6_.write==DaikinDriver::CommandSupport::Unknown && d.writeOutcome("F6")==DaikinDriver::WriteOutcome::Rejected);
    assert(!d.supportExternalRoomTemperatureSensor());
}
void test_econo() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g7[]={0x44,'0','A','G'}; d.handle_f7_response(g7,4);
    d.pending_.climate.econo=true; d.pending_.activate_econo=true; d.sendEconoCommand();
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[1]=='D' && tx[2]=='7' && tx[3]==0x44 && tx[4]=='2' && tx[5]=='0' && tx[6]=='0');
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
    assert(tx[3]==0x49 && tx[4]=='2' && tx[5]=='0' && tx[6]=='0');
    g7[0]=0x49; d.handle_f7_response(g7,4); assert(d.state_.demand_percentage==75);
    d.setMaxPowerLevel(101); assert(!d.demand_pending_);
}
void test_led() {
    Feedback f; DaikinDriver d(f); d.setup(); d.support_.sensor_hardware_known=true;
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
    assert(tx[2]=='5' && tx[3]=='1' && tx[4]=='?' && tx[5]==0x3C && tx[6]=='0');
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
    assert(tx[3]=='1' && tx[4]=='?' && tx[5]==0x3C && tx[6]=='0');
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
        assert(it!=d.queries_.end() && !it->is_static && it->interval_ms==60000 && !it->handler);
    }
}
void test_query_rates() {
    Feedback f; DaikinDriver d(f); d.setup(); d.protocol_version_={3,40}; d.initializeQueries();
    for(const auto& q : d.queries_) {
        if(q.command=="FX60") assert(!q.is_static && q.interval_ms==10000);
        if(q.command=="FU05" || q.command=="FU35") assert(q.is_static);
    }
}
void test_read_write_capabilities() {
    Feedback f; DaikinDriver d(f); d.setup(); d.queries_.emplace_back("F6");
    using A=DaikinDriver::FeatureAccess;
    assert(d.commandAccess("F6")==A::Unknown);
    d.queries_[0].acked=true; assert(d.commandAccess("F6")==A::ReadSupported);
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
    d.handle_fr_response(gr,4); assert(d.getMaximumVerticalFixPosition()==5 && !d.telemetry().verticalPositionVerified);
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
void test_fan_rpm() {
    Feedback f; DaikinDriver d(f); uint8_t sk[]={'6','7','0'}, sl[]={'5','7','0'};
    d.handle_fan_rpm(true,sk,3); d.handle_fan_rpm(false,sl,3);
    assert(d.telemetry().fanRpmSetpoint==760 && d.telemetry().fanRpm==750);
    d.handle_fan_rpm(false,sl,2); assert(!d.telemetry().fanRpmValid);
    sl[0]='X'; d.handle_fan_rpm(false,sl,3); assert(!d.telemetry().fanRpmValid);
}
void test_vane_angles() {
    Feedback f; DaikinDriver d(f); uint8_t sn[]={'4','0','4','+'};
    d.handle_vane_angle(false,sn,4); assert(d.telemetry().swingVerticalAngle==404 && !d.telemetry().vaneAngleDegreesValid);
    assert(d.configureVaneAngleScale(10)); d.handle_vane_angle(false,sn,4);
    assert(std::abs(d.telemetry().vaneAngleDegrees-40.4f)<0.001f);
    sn[3]='-'; d.handle_vane_angle(true,sn,4); assert(d.telemetry().vaneAngleSetpointDegrees<0);
}
void test_supplemental_sensors() {
    Feedback f; DaikinDriver d(f); uint8_t sf[]={'1','F'}, sd[]={'4','4','1'}, se[]={'7','2','0'};
    d.handle_supplemental_sensor("RF",sf,2); d.handle_supplemental_sensor("RD",sd,3); d.handle_supplemental_sensor("RE",se,3);
    assert(d.telemetry().supplementalSwing==daikin::Swing::Vertical);
    assert(d.telemetry().onTimerPeriods==144 && d.telemetry().offTimerPeriods==27);
}
void test_raw_research() {
    Feedback f; DaikinDriver d(f); d.setup(); d.queries_.emplace_back("Rb");
    d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    uint8_t sb[]={'S','b','7','0','0'}; d.handle_serial_result(Result::Frame,sb,5);
    assert((d.diagnosticResponse("Rb")==std::vector<uint8_t>{'7','0','0'}));
    assert(d.diagnosticResponse("Rm").empty());
}
void test_identification() {
    Feedback f; DaikinDriver d(f); d.setup(); d.queries_.emplace_back("VS000M",[&](uint8_t* p,size_t n){d.handle_firmware_response(p,n);},true);
    d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    uint8_t vs[]={'V','S','1','0','1','7','1','0','2','2','M','0','0','0','0','0'};
    d.handle_serial_result(Result::Frame,vs,sizeof(vs)); assert(d.firmwareVersion()=="22017101");
    assert(d.queries_[0].acked);
    d.protocol_version_={3,20}; d.initializeQueries();
    for(auto cmd : {"FY10","FY20","VS000M"}) assert(std::any_of(d.queries_.begin(),d.queries_.end(),[&](const DaikinQueryState& q){return q.command==cmd && q.is_static;}));
}
void test_temperature_limits() {
    Feedback f; DaikinDriver d(f); d.setup(); std::array<uint8_t,32> gu; gu.fill(0xFF); gu[2]=0x30; gu[1]=0xA4;
    d.support_.auto_limit_offset=true; d.handle_fu_fx_response("FU02",gu.data(),32);
    d.state_.mode=daikin::Mode::Heat; assert(d.getMinimumTargetTemperature()==10 && d.telemetry().autoLimitOffsetC==2);
    d.query_state_=DaikinDriver::QueryState::PostWriteSettle; d.setTargetTemperature(5); assert(d.pending_.climate.targetC==10);
    d.state_.mode=daikin::Mode::Cool; assert(d.getMinimumTargetTemperature()==18);
    gu[2]=0xFF; d.handle_fu_fx_response("FU02",gu.data(),32); assert(!d.telemetry().heatMinimumValid);
}
void test_model_name() {
    Feedback f; DaikinDriver d(f); std::array<uint8_t,32> gu; gu.fill(0xFF);
    std::fill_n(gu.begin(),22,' '); const std::string model="FTXA35C2V1BS";
    std::copy(model.begin(),model.end(),gu.begin()); d.handle_fu_fx_response("FU05",gu.data(),32);
    assert(d.modelName()==model);
    std::fill_n(gu.begin(),22,' '); d.handle_fu_fx_response("FU05",gu.data(),32); assert(d.modelName().empty());
}
void test_energy_counters() {
    Feedback f; DaikinDriver d(f); std::string gu="44200000B3B20000FFFFFFFFFFFFFFFF";
    d.handle_fu_fx_response("FU04",reinterpret_cast<uint8_t*>(&gu[0]),32);
    assert(d.telemetry().fu04_valid && d.telemetry().fu04_cooling_wh==58000 && d.telemetry().fu04_heating_wh==1106700);
    gu.replace(0,8,"EFFFFFFF"); d.handle_fu_fx_response("FU04",reinterpret_cast<uint8_t*>(&gu[0]),32);
    assert(d.telemetry().fu04_cooling_wh==429496729400ULL);
    gu.replace(0,8,"FFFFFFFF"); d.handle_fu_fx_response("FU04",reinterpret_cast<uint8_t*>(&gu[0]),32); assert(d.telemetry().fu04_valid && d.telemetry().fu04_cooling_wh==429496729500ULL);
    gu.replace(0,8,"10000000"); d.handle_fu_fx_response("FU04",reinterpret_cast<uint8_t*>(&gu[0]),32); assert(d.telemetry().fu04_cooling_wh==100);
    uint8_t gm[]={'F','F','F','F'}; d.handle_fm_response(gm,4); assert(d.telemetry().totalEnergyWh==6553500);
    gm[0]='0'; gm[1]=gm[2]=gm[3]='0'; d.handle_fm_response(gm,4); assert(d.telemetry().totalEnergyWh==0);
}
void test_live_power() {
    Feedback f; DaikinDriver d(f); uint8_t gx[]={'2','3','0','0'};
    d.handle_fu_fx_response("FX60",gx,4); assert(d.telemetry().fx60_valid && d.telemetry().fx60_raw==0x32 && d.telemetry().fx60_power_w==500);
    gx[0]=gx[1]=gx[2]=gx[3]='F'; d.handle_fu_fx_response("FX60",gx,4); assert(d.telemetry().fx60_valid && d.telemetry().fx60_power_w==655350);
    gx[0]='X'; d.handle_fu_fx_response("FX60",gx,4); assert(!d.telemetry().fx60_valid);
}
void test_unknown_extension_pages() {
    Feedback f; DaikinDriver d(f); d.setup(); d.queries_.emplace_back("FX40");
    d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    uint8_t gx[]={'G','X','4','0',0xFF,0x80}; d.handle_serial_result(Result::Frame,gx,sizeof(gx));
    assert((d.diagnosticResponse("FX40")==std::vector<uint8_t>{0xFF,0x80}));
    assert(!d.telemetry().fx60_valid && !d.telemetry().fu04_valid);
}
void test_real_timers() {
    Feedback f; DaikinDriver d(f); d.setup(); uint8_t g3[]={'0',0xFE,0xFE,0};
    d.handle_f3_response(g3,4); assert(d.telemetry().timersValid && !d.telemetry().onTimerEnabled);
    assert(!d.setTimers(60,0)); assert(d.configureTimerGranularity(60,720)); assert(!d.setTimers(10,0));
    assert(d.setTimers(60,0)); auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx.size()==8 && tx[2]=='3' && tx[3]=='1' && tx[4]==0x36 && tx[5]==0xFE);
    g3[0]='1';g3[1]=0x36; d.handle_f3_response(g3,4); assert(d.telemetry().onTimerConfiguredMinutes==60);
}
void test_coarse_sentinels() {
    Feedback f; DaikinDriver d(f); uint8_t g9[]={0xAC,0xAA,0x4E,'0'};
    d.handle_f9_response(g9,4); assert(d.telemetry().homeC==22 && d.telemetry().outsideC==21 && d.telemetry().humidity==30);
    g9[0]=g9[1]=g9[2]=0xFF; d.handle_f9_response(g9,4);
    assert(!d.telemetry().insideTemperatureValid && !d.telemetry().outsideTemperatureValid && !d.telemetry().humidityValid);
    uint8_t rh[]={'5','6','2','+'}; d.handle_rh_response(rh,4);
    g9[0]=0xAC; d.handle_f9_response(g9,4); assert(d.telemetry().homeC==26.5f);
}
void test_delayed_wire_frames() {
    Transport t; t.serial.send_frame("F1");
    t.receive({6,2,'G','2','0',0xA9,3}); // valid but unrelated G2
    assert(t.results.size()==1 && t.results[0]==Result::Ack && t.serial.busy());
    t.receive({2,'G','1','1','4','F','A',0x64,3}); assert(t.results.back()==Result::Frame && !t.serial.busy());
    t.serial.force_framed_mode(); // reconnect/reset
    t.serial.send_frame("F1"); t.receive({6}); test_now+=1000; t.serial.loop();
    assert(t.results.back()==Result::Timeout && !t.serial.busy());
    t.serial.send_frame("F1"); t.receive({2,'G','1','1','4','F','A',0x64,3});
    assert(t.results.back()==Result::Frame); // STX without initial ACK is tolerated
}
void test_decoder_lengths() {
    Feedback f; DaikinDriver d(f); d.setup();
    for(auto cmd : {"F1","F2","F3","F5","F6","F7","F8","FY00","FU00","FU02","FU04","FX60","F4","FR","RH","RI","RK","RL","RM","RN"}) {
        d.queries_.clear(); d.query_index_=0; d.queries_.emplace_back(cmd,nullptr,true);
        std::string header=cmd; header[0]=header[0]=='F'?'G':'S';
        std::vector<uint8_t> wire(header.begin(),header.end()); wire.push_back('0');
        d.query_state_=DaikinDriver::QueryState::WaitingForAck;
        d.handle_serial_result(Result::Frame,wire.data(),wire.size());
        assert(d.query_index_==1 && !d.queries_[0].acked);
    }
    uint8_t sd[]={'4','5','0'}, se[]={'9','7','0'};
    d.handle_rd_response(sd,3); d.handle_re_response(se,3);
    assert(d.telemetry().compressorHz==54 && d.telemetry().humidity==79);
}
void test_combined_mode_changes() {
    Feedback f; DaikinDriver d(f); d.setup();
    uint8_t g6[]={0xB0,0xB0,0x31,0x34}; d.handle_f6_response(g6,4); // Quiet + Streamer
    uint8_t g7[]={0x44,'0','A','G'}; d.handle_f7_response(g7,4);
    d.setDeviceMode(AirConditionDeviceModeHiPower);
    auto& hal=*static_cast<FakeHAL*>(d.serial_->serial_hal_.get());
    assert(hal.tx[2]=='6' && hal.tx[3]==0x32 && hal.tx[4]==0xB0 && hal.tx[5]==0x31);
    assert(!d.d6_.valid && d.pending_.activate_econo);
    // A queued second change cannot rebuild D6 from the pre-write snapshot.
    size_t sent=hal.tx.size(); d.setAirPurification(false); assert(hal.tx.size()==sent);
    d.serial_->force_framed_mode(); d.query_state_=DaikinDriver::QueryState::Idle;
    g6[0]=0x32; d.handle_f6_response(g6,4); d.sendStreamerCommand();
    assert(hal.tx[sent+3]==0x32 && hal.tx[sent+4]==0x30 && hal.tx[sent+5]==0x31);
}
void test_explicit_settings_readback() {
    Feedback f; DaikinDriver d(f); d.setup(); uint8_t g1[]={'1','3',0x4C,'A'}; d.handle_f1_response(g1,4);
    d.query_state_=DaikinDriver::QueryState::PostWriteSettle;
    d.setTargetTemperature(22); d.setMode(AirConditionModeAuto); d.setPower(false); d.setFanSpeed(1);
    d.query_state_=DaikinDriver::QueryState::Idle; d.sendClimateCommand();
    d.handle_serial_result(Result::Nak,nullptr,0);
    assert(d.state_.targetC==24 && d.state_.mode==daikin::Mode::Cool && d.state_.power);
    g1[0]='0'; g1[1]='1'; g1[2]=0x48; g1[3]='A'; d.handle_f1_response(g1,4);
    // G1 can alias Quiet to Auto; RG supplies the reliable quiet fan confirmation.
    d.support_.fan_mode_query=true; uint8_t rg[]={'B'}; d.handle_rg_response(rg,1);
    assert(d.climate_dirty_==0 && !d.state_.power && d.state_.targetC==22 && d.state_.mode==daikin::Mode::Auto);
    d.query_state_=DaikinDriver::QueryState::PostWriteSettle;
    d.setTargetTemperature(100); assert(d.pending_.climate.targetC==32);
    d.setTargetTemperature(NAN); assert(d.pending_.climate.targetC==32);
    d.setTargetTemperature(-100); assert(d.pending_.climate.targetC==18);
}
void test_unknown_sensor_led_gate() {
    Feedback f; DaikinDriver d(f); d.setup(); uint8_t g6[]={0x30,0x30,0x30,0x3C};
    d.handle_f6_response(g6,4); assert(d.telemetry().ledState==daikin::LedState::Unknown);
    d.setWifiLed(false); auto& hal=*static_cast<FakeHAL*>(d.serial_->serial_hal_.get()); assert(hal.tx.empty());
    uint8_t gk[]={0x30,0x30,0x34,0x30}; d.handle_fk_response(gk,4); d.sendLedCommand();
    assert(hal.tx.empty() && !d.pending_.activate_led); // already Off after hardware discovery
}
void test_compatibility_fixtures() {
    std::ifstream fixtures("tests/daikin/compatibility.tsv"); assert(fixtures);
    std::string line; size_t rows=0;
    while (std::getline(fixtures,line)) {
        if (line.empty() || line[0]=='#') continue;
        std::istringstream row(line); std::string model,f8,fy; unsigned major,minor;
        row>>model>>major>>minor>>f8>>fy; assert(row);
        Feedback f; DaikinDriver d(f); d.setup(); d.initializeProtocolDetection();
        auto exchange=[&](std::string_view cmd,const std::string& hex){
            d.query_state_=DaikinDriver::QueryState::WaitingForAck;
            d.serial_->send_frame(cmd);
            auto& hal=*static_cast<FakeHAL*>(d.serial_->serial_hal_.get());
            if(hex=="NAK") hal.rx.push_back(0x15);
            else {
                std::vector<uint8_t> body; for(char c:cmd) body.push_back(c); body[0]='G';
                for(size_t i=0;i<hex.size();i+=2) body.push_back(std::stoul(hex.substr(i,2),nullptr,16));
                uint8_t checksum=d.serial_->calculate_checksum(body.data(),body.size());
                hal.rx.push_back(6); hal.rx.push_back(2);
                hal.rx.insert(hal.rx.end(),body.begin(),body.end()); hal.rx.push_back(checksum); hal.rx.push_back(3);
            }
            while(!hal.rx.empty()) d.serial_->loop();
        };
        exchange("F8",f8); exchange("FY00",fy);
        assert(d.protocol_version_==(daikin::ProtocolVersion{uint8_t(major),uint8_t(minor)}));
        d.initializeQueries();
        auto has=[&](std::string_view cmd){return std::any_of(d.queries_.begin(),d.queries_.end(),[&](const DaikinQueryState& q){return q.command==cmd;});};
        assert(has("F3") && has("F4") && has("F5") && has("RI") && has("RK") && has("RL"));
        assert(has("FK")== (major>=2));
        assert(has("FX60")== (major>=3 && minor>=20));
        rows++;
    }
    assert(rows==6);
}
void test_large_pages_and_tx_failures() {
    Transport t; t.serial.send_frame("FU05");
    std::vector<uint8_t> body={'G','U','0','5'}; body.insert(body.end(),32,'A');
    t.hal().rx.push_back(6); t.hal().rx.push_back(2); t.hal().rx.insert(t.hal().rx.end(),body.begin(),body.end());
    t.hal().rx.push_back(t.serial.calculate_checksum(body.data(),body.size())); t.hal().rx.push_back(3);
    size_t before=t.hal().rx.size(); t.serial.loop(); assert(t.hal().rx.size()==before-16);
    while(!t.hal().rx.empty()) t.serial.loop(); assert(t.payload.size()==36 && t.hal().tx.back()==6);
    Transport bad; bad.hal().fail_writes=true; bad.serial.send_frame("F1"); assert(bad.results.back()==Result::Error && !bad.serial.busy());
    bad.hal().fail_writes=false; bad.serial.send_frame("F1"); bad.hal().fail_writes=true;
    bad.receive({2,'G','1','1','4','F','A',0x64,3}); assert(bad.results.back()==Result::Error);
}
void test_optional_query_configuration() {
    Feedback f;DaikinDriver d(f);d.setup();d.protocol_version_={3,40};d.initializeQueries();
    auto has=[&](std::string_view c){return std::any_of(d.queries_.begin(),d.queries_.end(),[&](const DaikinQueryState& q){return q.command==c;});};
    for(auto c:{"RA","RB","RC","RF","RD","RE","Rb","FU15","FU25","FU35","FU45"}) assert(has(c));
    d.enableExtendedPolling(false);assert(!d.isQuerySupported("RA"));d.enableExtendedPolling(true);assert(d.isQuerySupported("RA"));
    d.enableProductionDiagnostics(false);assert(!d.isQuerySupported("FU35"));d.enableProductionDiagnostics(true);assert(d.isQuerySupported("FU35"));
    assert(!d.isQuerySupported("RW"));d.enableResearchPolling(true);assert(d.isQuerySupported("RW"));
}
void test_unknown_optional_state() {
    Feedback f; DaikinDriver d(f); d.gate_publish_until_full_sample_=false;
    assert(d.telemetry().demand_percentage==100);
    d.publishState(); assert(f.humidity_mode_updates==0);
}
void test_audit_regressions() {
    struct Capture: Feedback {
        unsigned powers=0, temperatures=0, demands=0; int humidityStep=-1;
        void updatePower(bool) override { ++powers; }
        void updateCurrentTemperature(float) override { ++temperatures; }
        void updateMaxPowerLevel(uint8_t p) override { ++demands; assert(p<=100); }
        void updateHumidityMode(uint8_t step) override { humidityStep=step; assert(step<=4); }
    } a,b;
    DaikinDriver d(a),other(b);d.setup();
    uint8_t g1[]={'1','3',0x4C,'A'}; d.handle_f1_response(g1,4);
    d.setTargetTemperature(22);d.handle_serial_result(Result::Nak,nullptr,0);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;
    g1[2]=0x50;d.handle_f1_response(g1,4);d.setFanSpeed(3);
    auto& hal=*static_cast<FakeHAL*>(d.serial_->serial_hal_.get());assert(hal.tx[hal.tx.size()-4]==0x50);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;
    d.setTargetTemperature(22.4f);assert(d.pending_.climate.targetC==22.5f);
    g1[2]=0x49;d.handle_f1_response(g1,4);assert(!(d.climate_dirty_ & DaikinDriver::DirtyTemperature));
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;
    uint8_t g2[]={0x34,0x32,0,0x90};d.handle_f2_response(g2,4);
    uint8_t g5[]={'0','0','0',0x80};d.handle_f5_response(g5,4);
    d.query_state_=DaikinDriver::QueryState::PostWriteSettle;d.setSwingVertical(true);d.setSwingVertical(false);
    d.query_state_=DaikinDriver::QueryState::Idle;size_t n=hal.tx.size();d.sendSwingCommand();assert(hal.tx.size()==n || hal.tx[n+3]=='0');
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.handle_f5_response(g5,4);
    n=hal.tx.size();d.setSwingHorizontal(true);assert(hal.tx.size()==n);
    uint8_t fk[]={0x30,0x30,0x30,0x30};d.handle_fk_response(fk,4);d.setFanSpeed(6);assert(hal.tx.size()==n);
    uint8_t sx[]={'0','0','2','+'};d.state_.sensor=false;d.handle_rx_response(sx,4);assert(!d.state_.sensor);
    d.state_.humidityMode=daikin::HumidityMode::Low;d.state_.humidityModeValid=true;d.humidity_modes_=0x92;d.publishState();assert(a.humidityStep==1);
    d.state_.demandValid=d.state_.demandPercentageValid=true;d.state_.demand_percentage=75;d.publishState();assert(a.demands);
    other.state_.basicValid=true;other.state_.power=false;other.publishState();unsigned before=b.powers;
    d.state_.power=true;d.publishState();other.state_.power=true;other.publishState();assert(b.powers==before+1);
    unsigned readings=a.temperatures;d.state_.insideTemperatureValid=false;d.state_.homeC=99;d.publishState();assert(a.temperatures==readings);
    d.online_=true;d.state_.online=true;d.last_rx_ok_ms_=100;d.stats_.acks=1;test_now=20000;d.checkOnlineTimeout();d.updateOnlineStatus();
    assert(!d.online_ && !d.state_.online && !d.state_.basicValid);
    uint8_t zero[]={0,0,0,0};d.protocol_version_=daikin::ProtocolUndetected;d.handle_f8_response(zero,4);assert(d.protocol_version_==daikin::ProtocolUndetected);
    DaikinDriver silent(a);silent.setup();test_now=100;silent.startCommunication(false);
    for(int i=0;i<6000;i++){test_now+=10;silent.loop();}
    assert(silent.protocol_detection_failed_reported_);
}
void test_legacy_and_diagnostic_transactions() {
    Transport t;assert(t.serial.send_frame("M"));assert((t.hal().tx==std::vector<uint8_t>{2,'M','M',3}));
    t.receive({6,2,'M','3','5','E','3',0x2D,3});assert(t.results.back()==Result::Frame && t.hal().tx.back()==6);
    t.hal().tx.clear();uint8_t address[]={'1','0','0','0'};assert(t.serial.send_frame("A",address,4));
    assert((t.hal().tx==std::vector<uint8_t>{2,'A','1','0','0','0',4,3}));
    t.receive({6,2,'C','1','0','0','D','4',0x4C,3});assert(t.results.back()==Result::Frame);
    t.hal().tx.clear();uint8_t forbidden[]={10};assert(!t.serial.send_frame("D2",forbidden,1) && t.hal().tx.empty());
    assert(t.serial.send_frame("RG"));t.receive({6,2,'S','G',6,0xA0,3});assert(t.results.back()==Result::Error && t.hal().tx.back()==3);
    Feedback f;DaikinDriver d(f);d.setup();assert(d.processCommand("s21 raw M",false));assert(!d.processCommand("s21 raw M",true));
    d.serviceDiagnostic();auto& hal=*static_cast<FakeHAL*>(d.serial_->serial_hal_.get());
    for(auto b:{6,2,int('M'),int('3'),int('5'),int('E'),int('3'),0x2D,3})hal.rx.push_back(b);
    while(!hal.rx.empty())d.serial_->loop();assert(d.diagnosticResult().status==DaikinDriver::DiagnosticStatus::Frame);
    assert((d.diagnosticResult().payload==std::vector<uint8_t>{'3','5','E','3'}));
    for(auto c:{"D2","DH","DJ","DL","DY10","DY20"}) {
        DaikinDriver raw(f);raw.setup();std::vector<uint8_t> payload(std::string(c)=="D2"?1:std::string(c)=="DY10"?8:4,'0');
        assert(raw.queueDiagnosticCommand(c,payload));raw.serviceDiagnostic();
        auto& h=*static_cast<FakeHAL*>(raw.serial_->serial_hal_.get());h.rx.push_back(6);raw.serial_->loop();
        assert(raw.diagnosticResult().status==DaikinDriver::DiagnosticStatus::Ack && raw.isInPostWriteSettle());
    }
    DaikinDriver bounded(f);for(int i=0;i<4;i++)assert(bounded.queueDiagnosticCommand("F1"));assert(!bounded.queueDiagnosticCommand("F1"));
    assert(!d.queueDiagnosticCommand("A",{}) && !d.queueDiagnosticCommand("D2",{6}));
}
void test_compound_and_queued_humidity() {
    Feedback f;DaikinDriver d(f);d.setup();uint8_t g2[]={0x3C,0x32,0,0x92};d.handle_f2_response(g2,4);
    uint8_t g1[]={'1','3',0x4C,'A'},g5[]={'0','0','0',0x80};d.handle_f1_response(g1,4);d.handle_f5_response(g5,4);
    assert(d.setClimateVariant(daikin::ClimateVariant::SararaDry));auto& h=*static_cast<FakeHAL*>(d.serial_->serial_hal_.get());assert(h.tx[4]=='2' && h.tx[5]==0x80);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;g1[1]='2';g1[2]=0x80;d.handle_f1_response(g1,4);d.handle_f5_response(g5,4);
    size_t n=h.tx.size();d.setFanSpeed(2);assert(h.tx[n+5]==0x80);
    DaikinDriver u(f);u.setup();u.handle_f2_response(g2,4);g1[1]='4';g1[2]=0x4C;u.handle_f1_response(g1,4);u.handle_f5_response(g5,4);
    assert(u.setClimateVariant(daikin::ClimateVariant::UruruHumidify));auto& uh=*static_cast<FakeHAL*>(u.serial_->serial_hal_.get());assert(uh.tx[4]=='4' && uh.tx[5]==0xC0);
    u.serial_->force_framed_mode();u.query_state_=DaikinDriver::QueryState::Idle;g1[2]=0xC0;u.handle_f1_response(g1,4);n=uh.tx.size();u.setPower(false);assert(uh.tx[n+5]==0xC0);
    DaikinDriver q(f);q.setup();q.handle_f2_response(g2,4);g1[1]='4';g1[2]=0x4C;q.handle_f1_response(g1,4);q.handle_f5_response(g5,4);
    q.query_state_=DaikinDriver::QueryState::PostWriteSettle;assert(q.setHumidityMode(daikin::HumidityMode::Low));
    g1[1]='3';q.handle_f1_response(g1,4);q.query_state_=DaikinDriver::QueryState::Idle;q.sendSwingCommand();
    auto& qh=*static_cast<FakeHAL*>(q.serial_->serial_hal_.get());assert(qh.tx[5]==0x3A);
    DaikinDriver a(f);a.setup();a.handle_f2_response(g2,4);g1[1]='7';a.handle_f1_response(g1,4);a.handle_f5_response(g5,4);
    assert(a.setHumidityMode(daikin::HumidityMode::Low));assert(static_cast<FakeHAL*>(a.serial_->serial_hal_.get())->tx[5]==0x38);
    assert(a.configureTemperatureLimits(daikin::Mode::Heat,10,40));a.state_.mode=daikin::Mode::Heat;assert(a.getMaximumTargetTemperature()==40);
    assert(!a.configureTemperatureLimits(daikin::Mode::Heat,10.2f,40));
}
void test_confirmed_combined_capabilities() {
    using W=DaikinDriver::WriteOutcome;using A=DaikinDriver::FeatureAccess;
    Feedback f;DaikinDriver d(f);d.setup();uint8_t g6[]={0x30,0x30,0x30,0x3C};d.handle_f6_response(g6,4);
    d.pending_.activate_powerful=true;d.pending_.climate.powerful=true;d.sendPowerfulCommand();
    assert(d.writeOutcome("F6")==W::AwaitingAck);d.handle_serial_result(Result::Ack,nullptr,0);
    assert(d.writeOutcome("F6")==W::AwaitingReadback && d.commandAccess("F6")==A::ReadSupported);
    d.handle_f6_response(g6,4);assert(d.writeOutcome("F6")==W::AwaitingReadback);
    test_now+=30001;d.expireWriteConfirmations();assert(d.writeOutcome("F6")==W::Mismatch);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;
    d.pending_.activate_powerful=true;d.pending_.climate.powerful=true;d.sendPowerfulCommand();d.handle_serial_result(Result::Ack,nullptr,0);g6[0]=0x32;d.handle_f6_response(g6,4);
    assert(d.commandAccess("F6")==A::Writable && d.fieldWriteVerified("F6",0,2) && !d.fieldWriteVerified("F6",1,0x80));
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.setAirPurification(true);d.handle_serial_result(Result::Nak,nullptr,0);
    assert(d.writeOutcome("F6")==W::Rejected && d.commandAccess("F6")==A::Writable);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.handle_f6_response(g6,4);size_t n=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx.size();
    d.pending_.activate_quiet=true;d.pending_.climate.quiet=true;d.sendQuietCommand();assert(static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx.size()>n);
    DaikinDriver v(f);v.setup();uint8_t gr[]={'0','0','0','0'};v.handle_fr_response(gr,4);assert(v.getMaximumVerticalFixPosition()==5);
    v.setSwingVerticalFixPosition(5);v.handle_serial_result(Result::Ack,nullptr,0);gr[0]='5';v.handle_fr_response(gr,4);assert(v.verifiedVerticalPositions()==0x20);
    v.serial_->force_framed_mode();v.query_state_=DaikinDriver::QueryState::Idle;assert(v.setVerticalPositionCode('?'));
    assert(static_cast<FakeHAL*>(v.serial_->serial_hal_.get())->tx[static_cast<FakeHAL*>(v.serial_->serial_hal_.get())->tx.size()-6]=='?');
}
void test_metadata_schema_and_model_discovery() {
    Feedback f;DaikinDriver d(f);d.setup();uint8_t fc[]={'1','2','A','B'},fn[]={'1','2','3','4'},fs[]={'2','A','0','0'},ft[]={'3','2','1','0'};
    d.handle_fc_response(fc,4);d.handle_metadata("FN",fn,4);d.handle_metadata("FS",fs,4);d.handle_metadata("FT",ft,4);
    assert(d.identification().modelCode=="BA21" && d.identification().itelc=="4321");
    assert(d.identification().wifiMode=='2' && d.identification().wifiCounter==10 && d.identification().ftRawDecimal==123);
    uint8_t ir[]={'A','B','C','D'};d.handle_fg_response(ir,4);ir[3]='E';d.handle_fg_response(ir,4);assert(d.state_.irCounterRaw[3]=='E' && d.state_.irObservedChanges==1);
    uint8_t pull[]={'5','1','0'};d.handle_aux_sensor("Rb",pull,3);assert(d.state_.pull==15);
    uint8_t fk[]={0x39,0x3D,0x3D,0x31};d.protocol_version_={3,40};d.handle_fk_response(fk,4);
    assert(d.identification().laundryAvailable && d.identification().japaneseTemperatureRange && d.identification().destinationRaw);
    std::array<uint8_t,32> prod;prod.fill(0xFF);const std::string data="AB1201234567SERIAL123456";std::copy(data.begin(),data.end(),prod.begin());
    d.handle_fu_fx_response("FU35",prod.data(),32);assert(d.identification().production.indoor.siteCode=="AB" && d.identification().production.indoor.dateCode=="01234567");
    for(auto version:{daikin::ProtocolVersion{0,0},daikin::ProtocolVersion{1,0},daikin::ProtocolVersion{2,0},daikin::ProtocolVersion{3,1}}) {
        d.protocol_version_=version;d.initializeQueries();auto has=[&](std::string_view c){return std::any_of(d.queries_.begin(),d.queries_.end(),[&](const DaikinQueryState& q){return q.command==c;});};
        assert(has("M") && has("V") && has("RzB2") && has("RzC3"));if(version.major==1)assert(has("FC"));if(version.major==3)assert(has("FU02"));
    }
    uint8_t fx[]={0x30,0x30,0x30,0x30};assert(!daikin::s21::validResponse("FX40",fx,4));assert(daikin::s21::validResponse("FX40",fx,2));
    uint8_t fw[]={'1','0','1','7','1','0','2','2','M','0','0','0','0','X'};assert(!daikin::s21::validResponse("VS000M",fw,14));
    d.queries_.clear();d.query_index_=0;d.queries_.emplace_back("FU00",nullptr,true);d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    std::vector<uint8_t> bad={'G','U','0','0'};bad.insert(bad.end(),32,'4');d.handle_serial_result(Result::Frame,bad.data(),bad.size());assert(!d.queries_[0].acked);
}
void test_timing_reprobe_and_missing_samples() {
    Feedback f;DaikinDriver d(f);d.setup();d.state_.insideTemperatureValid=true;d.support_.inside_temperature_query=true;
    d.queries_.emplace_back("RH",[&](uint8_t* p,size_t n){d.handle_rh_response(p,n);});d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    uint8_t malformed[]={'S','H','0'};d.handle_serial_result(Result::Frame,malformed,3);assert(!d.state_.insideTemperatureValid && !d.queries_[0].acked);
    DaikinDriver::TimingProfile profile;profile.ack_ms=0;assert(!d.configureTiming(profile));profile.ack_ms=500;profile.rx_ms=500;profile.reprobe_ms=1000;assert(d.configureTiming(profile));
    d.queries_.clear();d.queries_.emplace_back("F1");d.query_index_=0;d.queries_[0].bad=true;d.queries_[0].last_nak_ms=test_now;
    d.query_state_=DaikinDriver::QueryState::WaitingToSend;test_now+=1100;d.updateQueryStateMachine();assert(!d.queries_[0].bad && d.serial_->busy());
    d.state_.basicValid=true;d.state_.power=true;d.state_.online=true;d.state_.mode=daikin::Mode::Cool;d.state_.compressorOnValid=true;d.state_.compressorOn=false;
    assert(d.calculateCurrentAction()==daikin::Action::Idle);d.state_.compressorOn=true;assert(d.calculateCurrentAction()==daikin::Action::Cooling);
}
void test_temperature_numeric_edges() {
    using daikin::DaikinC10;
    assert(static_cast<int16_t>(DaikinC10(-12.36f))==-124);
    assert(static_cast<int16_t>(DaikinC10(12.36f))==124);
    assert(DaikinC10(50.0f).valid() && DaikinC10(50.0f).f_degc()==50.0f);
    assert(!DaikinC10(NAN).valid() && !DaikinC10(INFINITY).valid());
    assert(!DaikinC10(40000).valid() && !DaikinC10(65536u).valid());
    assert(!(DaikinC10(100)/DaikinC10(0)).valid());
    assert(!(DaikinC10(32767)+DaikinC10(1)).valid());
    assert(!(daikin::TEMPERATURE_INVALID+DaikinC10(100)).valid());
}
void test_shared_fixed_position_contract() {
    Feedback f; DaikinDriver d(f);d.setup(); AirConditionDriver& base=d;
    assert(!base.setFixedVerticalPosition(3));
    uint8_t gr[]={'?','0','0','0'};d.handle_fr_response(gr,4);d.publishState();
    assert(f.positionValid && f.positionCode==6 && f.positionMax==5 && f.positionMask==0);
    assert(!base.setFixedVerticalPosition(7));assert(base.setFixedVerticalPosition(0));
    d.handle_serial_result(Result::Ack,nullptr,0);gr[0]='0';d.handle_fr_response(gr,4);d.publishState();
    assert(f.positionValid && f.positionCode==0 && f.positionMask==1);
    d.invalidateTelemetry();assert(!f.positionValid);
}
void test_shared_telemetry_contract() {
    Feedback f;DaikinDriver d(f);d.setup();
    uint8_t coil[]={'5','2','2','+'}, rpm[]={'5','7','0'}, angle[]={'4','0','4','+'}, error[]={'0',0,0xA0,0};
    d.handle_ri_response(coil,4);d.handle_fan_rpm(false,rpm,3);d.handle_vane_angle(false,angle,4);d.handle_f4_response(error,4);
    d.state_.pullValid=true;d.state_.pull=15;d.state_.protocol={3,40};d.firmware_version_="22017101";
    d.model_name_="FTXM-LONG-MODEL-22";d.publishState();
    using T=AirConditionTelemetry;
    assert((f.extra.valid & (T::Coil|T::FanRpm|T::AngleRaw|T::ErrorByte|T::Protocol|T::Model))==(T::Coil|T::FanRpm|T::AngleRaw|T::ErrorByte|T::Protocol|T::Model));
    assert(f.extra.indoorCoilC==22.5f && f.extra.fanRpm==750 && f.extra.angleRaw==404 && f.extra.errorByte==0xA0);
    assert(!(f.extra.valid & T::AngleDegrees));assert(f.extra.protocol==0x0328 && f.extra.model==d.model_name_);
    assert(d.configureVaneAngleScale(10));d.handle_vane_angle(false,angle,4);d.publishState();assert(f.extra.valid & T::AngleDegrees);assert(std::abs(f.extra.angleDegrees-40.4f)<0.01f);
    d.invalidateTelemetry();assert(!(f.extra.valid & (T::Coil|T::FanRpm|T::AngleRaw|T::Protocol)));
}
void test_shared_optional_controls() {
    using C=AirConditionOptionalControl;
    Feedback f;DaikinDriver d(f);d.setup();AirConditionDriver& base=d;
    assert(!base.setOptionalControl(C::Comfort,true));assert(!base.setUnitLed(AirConditionUnitLed::On));
    d.support_.sensor_hardware_known=true;d.support_.sensor_hardware=true;
    uint8_t g6[]={0x32,0xB0,0x30,0x3C};d.handle_f6_response(g6,4);d.publishState();
    assert(f.options.values & 1);assert(f.options.values & 32);assert(f.options.values & 16);assert(f.options.ledLevels==5);
    assert(!base.setUnitLed(AirConditionUnitLed::Dark));assert(base.setOptionalControl(C::Powerful,false));
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;
    assert(tx[3]==0x30 && tx[4]==0xB0 && tx[6]==0x3C); // clearing Powerful preserves Streamer, Eye and LED
    d.publishState();assert(f.options.values & 1); // no optimistic status
    d.handle_serial_result(Result::Ack,nullptr,0);g6[0]=0x30;d.handle_f6_response(g6,4);d.publishState();assert(!(f.options.values & 1));
    assert(!base.setOptionalControl(static_cast<C>(7),true));d.invalidateTelemetry();assert(f.options.valid==0 && !f.options.ledValid);
}
void test_shared_humidity_control() {
    using H=AirConditionHumidityLevel;
    Feedback f;DaikinDriver d(f);d.setup();AirConditionDriver& base=d;
    assert(!base.setHumidityLevel(H::Standard));
    d.support_.f2_known=true;d.humidity_modes_=0xB7;
    uint8_t g1[]={'1','3',0x4C,'A'},g5[]={'0','0','0','0'};
    d.handle_f1_response(g1,4);d.handle_f5_response(g5,4);d.publishState();
    assert(f.humidityControlValid && f.humidityLevel==0 && f.humidityLevels==31 && f.humidityModes==0xB7);
    assert(!base.setHumidityLevel(static_cast<H>(5)));assert(base.setHumidityLevel(H::High));
    auto& tx=static_cast<FakeHAL*>(d.serial_->serial_hal_.get())->tx;assert(tx[5]==0x3C);
    d.publishState();assert(f.humidityLevel==0);d.handle_serial_result(Result::Ack,nullptr,0);
    g5[2]=0x3C;d.handle_f5_response(g5,4);d.publishState();assert(f.humidityLevel==3);
    d.state_.mode=daikin::Mode::Dry;d.state_.autoBias=daikin::AutoBias::Unknown;assert(!(d.availableHumidityLevels() & 2));
    d.state_.basicValid=false;d.publishState();assert(!f.humidityControlValid);
    d.support_.simple_humidify=true;assert(!base.setHumidityLevel(H::High));
    d.invalidateTelemetry();assert(!f.humidityControlValid && f.humidityLevels==0);
}
void test_shared_energy_telemetry() {
    Feedback f;DaikinDriver d(f);d.setup();d.publishState();assert(f.energy.valid==0);
    std::string fu="FFFFFFFFFFFFFFFF0000000000000000";
    d.handle_fu_fx_response("FU04",reinterpret_cast<uint8_t*>(&fu[0]),32);
    uint8_t power[]={'2','3','0','0'},meter[]={'1','0','0','0'};
    d.handle_fu_fx_response("FX60",power,4);d.handle_fm_response(meter,4);d.publishState();
    assert(f.energy.valid==15 && f.energy.knxValidity()==15);
    assert(f.energy.indoorPowerW==500 && f.energy.indoorCoolingWh==429496729500ULL && f.energy.indoorHeatingWh==429496729500ULL);
    assert(f.energy.systemWh==100); // independent system meter, never indoor sum
    power[0]='X';d.handle_fu_fx_response("FX60",power,4);d.publishState();assert(f.energy.valid==14);
    d.invalidateTelemetry();assert(f.energy.valid==0);
    AirConditionEnergyTelemetry sample;sample.valid=15;sample.indoorCoolingWh=UINT64_MAX;
    sample.indoorHeatingWh=uint64_t(INT64_MAX);sample.systemWh=uint64_t(INT64_MAX)+1;
    assert(sample.knxValidity()==(AirConditionEnergyTelemetry::Power|AirConditionEnergyTelemetry::Heating));
}
void test_shared_timers_and_lamp() {
    using T=AirConditionTimer;using W=AirConditionWriteResult;
    Feedback f;DaikinDriver d(f);d.setup();AirConditionDriver& base=d;
    assert(!base.setTimerDuration(T::On,60));assert(f.timers.timerResult==W::Unavailable);assert(!base.setTimerLamp(true));
    assert(!base.configureTimerGranularity(7,720));assert(!base.configureTimerGranularity(60,725));
    assert(base.configureTimerGranularity(60,720));
    uint8_t g3[]={'0',0x30,0xFE,0};d.handle_f3_response(g3,4);
    assert(f.timers.timersValid && f.timers.available==29);
    assert(!base.setTimerEnabled(T::On,true)); // no invented remembered delay
    assert(!base.setTimerDuration(T::On,10));assert(!base.setTimerDuration(T::On,780));assert(f.timers.timerResult==W::InvalidRequest);
    assert(!base.setTimerDuration(static_cast<T>(2),60));
    assert(base.setTimerDuration(T::On,60));
    assert(f.timers.timerResult==W::AwaitingAck && !f.timers.onEnabled);
    auto& hal=*static_cast<FakeHAL*>(d.serial_->serial_hal_.get());
    assert(hal.tx[3]=='1' && hal.tx[4]==0x36 && hal.tx[5]==0xFE);
    assert(base.setTimerDuration(T::Off,120));assert(f.timers.timerResult==W::Queued);
    d.handle_serial_result(Result::Ack,nullptr,0);g3[0]='1';g3[1]=0x36;d.handle_f3_response(g3,4);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.last_write_time_=0;hal.tx.clear();
    d.sendTimerCommand();assert(hal.tx[3]=='3' && hal.tx[4]==0x36 && hal.tx[5]==0x3C); // keeps newly confirmed On timer
    d.handle_serial_result(Result::Ack,nullptr,0);g3[0]='3';g3[2]=0x3C;d.handle_f3_response(g3,4);
    assert(f.timers.timerResult==W::Confirmed && f.timers.onMinutes==60 && f.timers.offMinutes==120);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.last_write_time_=0;hal.tx.clear();
    assert(base.setTimerEnabled(T::On,false));assert(hal.tx[3]=='2' && hal.tx[4]==0x30 && hal.tx[5]==0x3C); // per-axis disabled encodings
    d.handle_serial_result(Result::Ack,nullptr,0);g3[0]='2';g3[1]=0x30;d.handle_f3_response(g3,4);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.last_write_time_=0;hal.tx.clear();
    assert(base.setTimerEnabled(T::On,true));assert(hal.tx[4]==0x36 && hal.tx[5]==0x3C);
    d.handle_serial_result(Result::Ack,nullptr,0);d.handle_f3_response(g3,4);assert(f.timers.timerResult==W::AwaitingReadback);
    test_now+=30001;d.expireWriteConfirmations();assert(f.timers.timerResult==W::Mismatch);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.last_write_time_=0;hal.tx.clear();
    uint8_t gb[]={'0','0','0','0'};d.handle_fb_response(gb,4);assert(f.timers.lampValid && !f.timers.lamp);
    assert(base.setTimerLamp(true));assert(hal.tx[1]=='F' && hal.tx[2]=='B' && hal.tx[3]=='1');
    assert(f.timers.lampResult==W::AwaitingAck && !f.timers.lamp);
    d.handle_serial_result(Result::Ack,nullptr,0);assert(f.timers.lampResult==W::AwaitingReadback && !f.timers.lamp);
    gb[0]='1';d.handle_fb_response(gb,4);assert(f.timers.lampResult==W::Confirmed && f.timers.lamp && d.state_.led);
    d.serial_->force_framed_mode();d.query_state_=DaikinDriver::QueryState::Idle;d.last_write_time_=0;hal.tx.clear();
    assert(base.setTimerLamp(false));d.handle_serial_result(Result::Nak,nullptr,0);assert(f.timers.lampResult==W::Rejected && f.timers.lamp);
    d.invalidateTelemetry();assert(!f.timers.timersValid && !f.timers.lampValid);
    Feedback f2;DaikinDriver unknown(f2);unknown.setup();unknown.configureTimerGranularity(10,1440);
    uint8_t active[]={'3',0x36,0x3C,0};unknown.handle_f3_response(active,4);
    assert(!unknown.setTimerEnabled(T::On,false)); // no guessed disabled sentinel
}
void test_timer_lamp_optional_reply() {
    Transport t;uint8_t payload[]={'1','0','0','0'};
    assert(t.serial.send_frame("FB",payload,4,true));t.receive({6});assert(t.serial.busy());
    t.serial.rx_timeout_handler();assert(!t.serial.busy() && t.results.size()==1 && t.results.back()==Result::Ack);
    assert(t.serial.send_frame("FB",payload,4,true));t.receive({6,2,'G','B','1','0','0','0',0x4A,3});
    assert(!t.serial.busy() && t.results.back()==Result::Frame && t.hal().tx.back()==6);
    assert(t.serial.send_frame("FB"));t.receive({6});t.serial.rx_timeout_handler();assert(t.results.back()==Result::Timeout); // normal FB reads still require GB
}
void test_timer_lamp_preserves_unknown_fields()
{
    Feedback feedback;
    DaikinDriver driver(feedback);
    driver.setup();
    uint8_t reply[] = {'0', '3', '3', '1'};
    driver.handle_fb_response(reply, sizeof(reply));
    auto& hal = *static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());
    assert(driver.setTimerLamp(true));
    assert(hal.tx.size() == 9);
    assert(hal.tx[1] == 'F' && hal.tx[2] == 'B');
    assert(hal.tx[3] == '1' && hal.tx[4] == '3' && hal.tx[5] == '3' && hal.tx[6] == '1');
    driver.handle_serial_result(Result::Ack, nullptr, 0);
    reply[0] = '1';
    driver.handle_fb_response(reply, sizeof(reply));
    assert(feedback.timers.lamp && feedback.timers.lampResult == AirConditionWriteResult::Confirmed);
}

void test_diagnostic_retains_unrecognised_payload()
{
    Feedback feedback;
    DaikinDriver driver(feedback);
    driver.setup();
    assert(driver.queueDiagnosticCommand("F3"));
    driver.serviceDiagnostic();
    uint8_t reply[] = {'G', '3', '0', 0xC1, '3', '1'};
    driver.handle_serial_result(Result::Frame, reply, sizeof(reply));
    assert(driver.diagnosticResult().status == DaikinDriver::DiagnosticStatus::Error);
    assert((driver.diagnosticResult().payload == std::vector<uint8_t>{'0', 0xC1, '3', '1'}));
    assert(!driver.telemetry().timersValid);
}

void test_disabled_timer_retains_delay()
{
    Feedback feedback;
    DaikinDriver driver(feedback);
    driver.setup();
    // Captured from this indoor unit: disabled timers retain an old delay.
    uint8_t reply[] = {'0', '2', '0', '0'};
    assert(daikin::s21::validResponse("F3", reply, sizeof(reply)));
    driver.handle_f3_response(reply, sizeof(reply));
    assert(feedback.timers.timersValid);
    assert(!feedback.timers.onEnabled && !feedback.timers.offEnabled);
    assert(feedback.timers.onMinutes == 0 && feedback.timers.offMinutes == 0);
    assert(driver.timer_disabled_bytes_[0] == '2');
    driver.configureTimerGranularity(10, 1440);
    auto& hal = *static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());
    assert(driver.setTimerDuration(AirConditionTimer::Off, 60));
    assert(hal.tx[3] == '2' && hal.tx[4] == '2' && hal.tx[5] == '6');
    // The disabled On axis keeps its observed byte, not an invented sentinel.
    reply[1] = 0xC1;
    assert(!daikin::s21::validResponse("F3", reply, sizeof(reply)));
    reply[0] = '1'; reply[1] = '0';
    assert(!daikin::s21::validResponse("F3", reply, sizeof(reply)));
}

void test_console_unknown_optional_settings()
{
    Feedback feedback;
    DaikinDriver driver(feedback);
    driver.setup();
    driver.state_.led = true; // default bool must not masquerade as known LED state
    driver.state_.ledState = daikin::LedState::Unknown;
    driver.state_.humidityMode = daikin::HumidityMode::High;
    driver.state_.humidityModeValid = true;
    driver.support_.f2_known = true;
    driver.humidity_modes_ = 0;
    test_messages.clear();
    driver.showInformations();
    assert(std::find(test_messages.begin(), test_messages.end(), "LED: Unknown") != test_messages.end());
    assert(std::find(test_messages.begin(), test_messages.end(), "Humidity Mode: Unavailable") != test_messages.end());
    driver.state_.ledState = daikin::LedState::Dark;
    driver.state_.humidityModeValid = false;
    test_messages.clear();
    driver.showInformations();
    assert(std::find(test_messages.begin(), test_messages.end(), "LED: Dark") != test_messages.end());
    assert(std::find(test_messages.begin(), test_messages.end(), "Humidity Mode: Unknown") != test_messages.end());
}

void test_driver_online_lifecycle()
{
    test_now = 100;
    Feedback feedback;
    DaikinDriver driver(feedback);
    driver.setup();
    driver.startCommunication(false);
    assert(feedback.getDriverState() == AirConditionDriverStateStarting);
    driver.updateOnlineStatus();
    assert(feedback.getDriverState() == AirConditionDriverStateStarting);
    driver.markOnline(test_now);
    driver.updateOnlineStatus();
    assert(feedback.getDriverState() == AirConditionDriverStateOk);
    const auto notifications = feedback.driverStateUpdates;
    // Keep receiving for more than the module's 60-second retry interval.
    for (unsigned i = 0; i < 70; ++i)
    {
        test_now += 1000;
        driver.markOnline(test_now);
        driver.updateOnlineStatus();
        assert(feedback.getDriverState() == AirConditionDriverStateOk);
    }
    assert(feedback.driverStateUpdates == notifications);
    test_now += driver.timing_.offline_ms + 1;
    driver.checkOnlineTimeout();
    driver.updateOnlineStatus();
    assert(feedback.getDriverState() == AirConditionDriverStateError);
    driver.markOnline(test_now);
    driver.updateOnlineStatus();
    assert(feedback.getDriverState() == AirConditionDriverStateOk);
    driver.startCommunication(true);
    assert(feedback.getDriverState() == AirConditionDriverStateStarting);
    driver.markOnline(++test_now);
    driver.updateOnlineStatus();
    assert(feedback.getDriverState() == AirConditionDriverStateOk);
}

void test_delayed_climate_confirmation()
{
    using W=DaikinDriver::WriteOutcome;
    Feedback feedback; DaikinDriver driver(feedback); driver.setup();
    uint8_t reply[]={'1','3',0x4C,'A'};
    driver.handle_f1_response(reply,4);
    driver.setMode(AirConditionModeHeat);
    driver.handle_serial_result(Result::Ack,nullptr,0);
    driver.serial_->force_framed_mode();
    driver.handle_f1_response(reply,4); // first sample is still Cool
    assert(driver.writeOutcome("F1")==W::AwaitingReadback);
    assert(driver.sent_climate_dirty_ & DaikinDriver::DirtyMode);
    test_now+=18000; reply[1]='4'; driver.handle_f1_response(reply,4);
    assert(driver.writeOutcome("F1")==W::Confirmed && !driver.sent_climate_dirty_);

    driver.query_state_=DaikinDriver::QueryState::Idle;
    driver.setMode(AirConditionModeCool);
    driver.handle_serial_result(Result::Ack,nullptr,0);
    driver.serial_->force_framed_mode(); driver.handle_f1_response(reply,4);
    test_now+=30001; driver.loop();
    assert(driver.writeOutcome("F1")==W::Mismatch && !driver.climate_dirty_);
}

void test_confirmation_poll_preserves_cycle()
{
    Feedback feedback; DaikinDriver driver(feedback); driver.setup();
    driver.protocol_version_={3,30}; driver.initializeQueries();
    driver.sent_climate_dirty_=DaikinDriver::DirtyMode;
    driver.query_index_=12; driver.last_confirmation_poll_=test_now-1001;
    driver.selectConfirmationQuery();
    assert(driver.queries_[driver.query_index_].command=="F1");
    driver.advanceQuery(); assert(driver.query_index_==12 && driver.confirmation_resume_==SIZE_MAX);
    driver.sent_climate_dirty_=DaikinDriver::DirtyFan;
    driver.last_confirmation_poll_=test_now-1001;
    driver.selectConfirmationQuery();
    assert(driver.queries_[driver.query_index_].command=="RG");
    driver.advanceQuery(); assert(driver.query_index_==12);
}

void test_lamp_write_immediate_and_delayed_reply()
{
    using W=DaikinDriver::WriteOutcome;
    Feedback feedback; DaikinDriver driver(feedback); driver.setup();
    uint8_t gb[]={'0','4','4','1'}; driver.handle_fb_response(gb,4);
    assert(driver.setTimerLamp(true));
    driver.handle_serial_result(Result::Ack,nullptr,0);
    driver.handle_fb_response(gb,4);
    assert(driver.writeOutcome("FB")==W::AwaitingReadback && !driver.telemetry().timerLamp);
    uint8_t immediate[]={'G','B','1','4','4','1'};
    driver.handle_serial_result(Result::Frame,immediate,sizeof(immediate));
    assert(driver.writeOutcome("FB")==W::Confirmed && driver.telemetry().timerLamp);
    assert(driver.fieldWriteVerified("FB",0,0xFF));

    driver.serial_->force_framed_mode(); driver.query_state_=DaikinDriver::QueryState::Idle;
    assert(driver.setTimerLamp(false));
    driver.handle_serial_result(Result::Ack,nullptr,0);
    gb[0]='1'; driver.handle_fb_response(gb,4); // never applies
    assert(driver.writeOutcome("FB")==W::AwaitingReadback);
    driver.serial_->force_framed_mode(); test_now+=30001; driver.markOnline(test_now); driver.loop();
    assert(driver.writeOutcome("FB")==W::Mismatch && driver.telemetry().timerLamp);
}

void test_unsupported_econo_and_quiet_alias()
{
    Feedback feedback; DaikinDriver driver(feedback); driver.setup();
    uint8_t g6[]={0x32,0x30,0x30,0x34}; driver.handle_f6_response(g6,4);
    uint8_t g7[]={'0','0','0','0'}; driver.handle_f7_response(g7,4);
    driver.support_.special_features_known=true; driver.support_.econo=false; driver.support_.powerful=true;
    auto& hal=*static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());
    driver.setDeviceMode(AirConditionDeviceModeEco);
    assert(hal.tx.empty() && driver.telemetry().powerful);
    assert(!driver.pending_.activate_powerful && !driver.pending_.activate_quiet);
    assert(!driver.setOptionalControl(AirConditionOptionalControl::Econo,true));
    test_messages.clear(); driver.showInformations();
    assert(std::any_of(test_messages.begin(),test_messages.end(),[](const std::string& m){return m.find("FU00 advertised=No")!=std::string::npos;}));
    driver.setDeviceMode(AirConditionDeviceModeSilent2);
    assert(hal.tx[3]==0xB0); // the one Quiet bit, Powerful cleared
    assert(driver.pending_.climate.quiet);
}

void test_faikout_ambiguous_demand_sentinel()
{
    Feedback feedback; DaikinDriver driver(feedback); driver.setup();
    uint8_t g7[]={'1','2','0','0'}; driver.handle_f7_response(g7,4);
    assert(driver.telemetry().demandValid && driver.telemetry().econo);
    assert(!driver.telemetry().demandPercentageValid);
    assert(!driver.demand_configured_); // readable register is not a verified profile
    assert(driver.configureDemandLimits(0,1));
    g7[0]='0'; driver.handle_f7_response(g7,4);
    driver.setMaxPowerLevel(99);
    driver.handle_serial_result(Result::Ack,nullptr,0);
    g7[0]='1'; driver.handle_f7_response(g7,4);
    assert(driver.writeOutcome("F7")!=DaikinDriver::WriteOutcome::Confirmed);
    assert(!driver.fieldWriteVerified("F7",0,0xFF));
    g7[0]=':'; driver.handle_f7_response(g7,4);
    assert(driver.telemetry().demandPercentageValid && driver.telemetry().demand_percentage==90);
}

void test_model_profile_validation_and_reporting()
{
    Feedback feedback; DaikinDriver driver(feedback); driver.setup();
    AirConditionDriver& base=driver;
    assert(!driver.processCommand("s21 demand 90",false));
    assert(!base.configureDemandLimits(30,0));
    assert(!base.configureDemandLimits(31,5));
    assert(base.configureDemandLimits(30,5));
    assert(driver.processCommand("s21 demand 90",false));
    assert(!driver.processCommand("s21 demand 91",false));
    assert(driver.requested_demand_==90);
    assert(!driver.configureTimerGranularity(15,720));
    assert(driver.configureTimerGranularity(60,720));
    auto& hal=*static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());
    assert(hal.tx.empty()); // configuration/reporting never programs the AC
    test_messages.clear(); assert(driver.processCommand("s21 profiles",false));
    assert(std::any_of(test_messages.begin(),test_messages.end(),[](const std::string& m){return m.find("minimum=30%")!=std::string::npos;}));
    assert(std::any_of(test_messages.begin(),test_messages.end(),[](const std::string& m){return m.find("installer assertions")!=std::string::npos;}));
    assert(hal.tx.empty());
}

void test_rejected_response_evidence()
{
    Feedback f;DaikinDriver driver(f);driver.setup();
    driver.queries_.emplace_back("F7",nullptr);driver.query_index_=0;
    driver.query_state_=DaikinDriver::QueryState::WaitingForGrace;
    uint8_t bad[]={'G','7','0','1','A','G'};
    driver.handle_serial_result(Result::Frame,bad,sizeof(bad));
    assert(driver.stats_.frames_bad_format==1 && driver.rejected_response_count_==1);
    const auto& entry=driver.rejected_responses_[0];
    assert(entry.command=="F7" && !entry.headerMismatch && entry.bytes[1]=='1');
    std::vector<uint8_t> oversized(100,0x30);
    for(int i=0;i<20;i++) driver.recordRejectedResponse("TOOLONGCOMMAND",oversized.data(),oversized.size(),true);
    assert(driver.rejected_response_count_==8);
    for(const auto& e:driver.rejected_responses_) assert(e.bytes.size()<=38 && e.command.size()<=6);
    assert(driver.processCommand("s21 errors",false));assert(driver.processCommand("s21 writes",false));
    assert(!driver.processCommand("s21 errors",true));
}

void test_delayed_shared_register_confirmation()
{
    using W=DaikinDriver::WriteOutcome;
    Feedback f;DaikinDriver driver(f);driver.setup();
    uint8_t g7[]={'0','0','N','o'};driver.handle_f7_response(g7,4);
    assert(driver.configureDemandLimits(90,10));driver.setMaxPowerLevel(90);
    auto& hal=*static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());const auto count=hal.tx.size();
    driver.handle_serial_result(Result::Ack,nullptr,0);
    driver.serial_->force_framed_mode();driver.query_state_=DaikinDriver::QueryState::Idle;
    driver.handle_f7_response(g7,4);
    assert(driver.writeOutcome("F7")==W::AwaitingReadback && !driver.fieldWriteVerified("F7",0,0xFF));
    test_now+=15000;g7[0]=':';driver.handle_f7_response(g7,4);
    assert(driver.writeOutcome("F7")==W::Confirmed && driver.fieldWriteVerified("F7",0,0xFF));
    assert(hal.tx.size()==count); // observing stale status never retransmits
    driver.setMaxPowerLevel(100);driver.handle_serial_result(Result::Ack,nullptr,0);
    driver.handle_f7_response(g7,4);test_now+=30001;driver.expireWriteConfirmations();
    assert(driver.writeOutcome("F7")==W::Mismatch);
}

void test_econo_readback_overrides_negative_advertisement()
{
    Feedback f;DaikinDriver driver(f);driver.setup();
    uint8_t f1[]={'1','3',0x4C,'A'};driver.handle_f1_response(f1,4);
    std::array<uint8_t,32> fu{};fu.fill('0');fu[0]=fu[5]='3';
    driver.handle_fu_fx_response("FU00",fu.data(),fu.size());
    uint8_t f7[]={'0','0','N','o'};driver.handle_f7_response(f7,4);
    assert(driver.optionalControlState().available&2); // readable, not verified writable
    assert(!driver.fieldWriteVerified("F7",1,0xFF));
    driver.state_.basicValid=false; // an F1 poll in flight must not drop explicit intent
    assert(driver.probeEcono(true));
    auto& hal=*static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());
    assert(hal.tx[3]=='0' && hal.tx[4]=='2');
    driver.handle_serial_result(Result::Ack,nullptr,0);
    assert(!driver.fieldWriteVerified("F7",1,0xFF));
    driver.handle_f7_response(f7,4);assert(!driver.fieldWriteVerified("F7",1,0xFF));
    f7[1]='2';driver.handle_f7_response(f7,4);
    assert(driver.telemetry().econo && (driver.optionalControlState().available&2));
    assert(driver.fieldWriteVerified("F7",1,0xFF));
    driver.serial_->force_framed_mode();driver.query_state_=DaikinDriver::QueryState::Idle;
    assert(driver.setOptionalControl(AirConditionOptionalControl::Econo,false));
    driver.handle_serial_result(Result::Ack,nullptr,0);f7[1]='0';driver.handle_f7_response(f7,4);
    assert(!driver.telemetry().econo && driver.econoControlAvailable());
    driver.startCommunication(true);assert(!driver.econo_observed_on_);
}

void test_econo_and_outdoor_quiet_can_coexist()
{
    Feedback f;DaikinDriver driver(f);driver.setup();
    uint8_t g6[]={0x30,0xB0,0x30,0x34};driver.handle_f6_response(g6,4);
    uint8_t g7[]={'0','2','N','o'};driver.handle_f7_response(g7,4);
    auto& hal=*static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());
    assert(driver.setOptionalControl(AirConditionOptionalControl::Quiet,true));
    assert(hal.tx[1]=='D' && hal.tx[2]=='6' && hal.tx[3]==0xB0);
    assert(!driver.pending_.activate_econo && driver.telemetry().econo);
    driver.handle_serial_result(Result::Ack,nullptr,0);g6[0]=0xB0;driver.handle_f6_response(g6,4);
    driver.serial_->force_framed_mode();driver.query_state_=DaikinDriver::QueryState::Idle;hal.tx.clear();
    g7[1]='0';driver.handle_f7_response(g7,4);
    assert(driver.setOptionalControl(AirConditionOptionalControl::Econo,true));
    driver.sendEconoCommand();
    assert(hal.tx[1]=='D' && hal.tx[2]=='7' && hal.tx[4]=='2');
    assert(!driver.pending_.activate_quiet && driver.telemetry().quiet);
}

void test_unadvertised_econo_normal_request()
{
    Feedback f;DaikinDriver driver(f);driver.setup();
    uint8_t g1[]={'1','3',0x4C,'A'};driver.handle_f1_response(g1,4);
    uint8_t g7[]={'0','0','N','o'};driver.handle_f7_response(g7,4);
    driver.support_.special_features_known=true;driver.support_.econo=false;
    assert(driver.setOptionalControl(AirConditionOptionalControl::Econo,true));
    auto& hal=*static_cast<FakeHAL*>(driver.serial_->serial_hal_.get());
    assert(hal.tx[1]=='D' && hal.tx[2]=='7' && hal.tx[4]=='2');
    assert(!driver.telemetry().econo && !driver.fieldWriteVerified("F7",1,0xFF));
}

int main() { test_unadvertised_econo_normal_request(); test_econo_and_outdoor_quiet_can_coexist(); test_econo_readback_overrides_negative_advertisement(); test_delayed_shared_register_confirmation(); test_rejected_response_evidence(); test_model_profile_validation_and_reporting(); test_faikout_ambiguous_demand_sentinel(); test_unsupported_econo_and_quiet_alias(); test_lamp_write_immediate_and_delayed_reply(); test_delayed_climate_confirmation(); test_confirmation_poll_preserves_cycle(); test_console_unknown_optional_settings(); test_disabled_timer_retains_delay(); test_diagnostic_retains_unrecognised_payload(); test_timer_lamp_preserves_unknown_fields(); test_driver_online_lifecycle(); test_timer_lamp_optional_reply(); test_shared_timers_and_lamp(); test_shared_energy_telemetry(); test_shared_humidity_control(); test_shared_optional_controls(); test_shared_telemetry_contract(); test_shared_fixed_position_contract(); test_temperature_numeric_edges(); test_timing_reprobe_and_missing_samples(); test_metadata_schema_and_model_discovery(); test_confirmed_combined_capabilities(); test_compound_and_queued_humidity(); test_legacy_and_diagnostic_transactions(); test_audit_regressions(); test_unknown_optional_state(); test_optional_query_configuration(); test_large_pages_and_tx_failures(); test_compatibility_fixtures(); test_unknown_sensor_led_gate(); test_explicit_settings_readback(); test_combined_mode_changes(); test_decoder_lengths(); test_delayed_wire_frames(); test_coarse_sentinels(); test_real_timers(); test_unknown_extension_pages(); test_live_power(); test_energy_counters(); test_model_name(); test_temperature_limits(); test_identification(); test_raw_research(); test_supplemental_sensors(); test_vane_angles(); test_fan_rpm(); test_indoor_coil(); test_independent_status(); test_vertical_position(); test_timer_lamp(); test_f4(); test_read_write_capabilities(); test_query_rates(); test_rz_identity(); test_versions(); test_special_decoders(); test_capability_bits(); test_no_f3_special_fallback(); test_humidity(); test_swing_preserves_humidity(); test_confirmed_d1(); test_explicit_d1(); test_led(); test_demand(); test_econo(); test_d6(); test_handshake(); test_checksums(); test_recovery(); test_no_unframed_fallback(); test_correlation(); std::cout << "S21 native tests passed\n"; }
