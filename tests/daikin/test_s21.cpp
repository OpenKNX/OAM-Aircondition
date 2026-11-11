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
    gu.replace(0,8,"FFFFFFFF"); d.handle_fu_fx_response("FU04",reinterpret_cast<uint8_t*>(&gu[0]),32); assert(!d.telemetry().fu04_valid);
    gu.replace(0,8,"10000000"); d.handle_fu_fx_response("FU04",reinterpret_cast<uint8_t*>(&gu[0]),32); assert(d.telemetry().fu04_cooling_wh==100);
    uint8_t gm[]={'F','F','F','F'}; d.handle_fm_response(gm,4); assert(d.telemetry().totalEnergyWh==6553500);
    gm[0]='0'; gm[1]=gm[2]=gm[3]='0'; d.handle_fm_response(gm,4); assert(d.telemetry().totalEnergyWh==0);
}
void test_live_power() {
    Feedback f; DaikinDriver d(f); uint8_t gx[]={'2','3','0','0'};
    d.handle_fu_fx_response("FX60",gx,4); assert(d.telemetry().fx60_valid && d.telemetry().fx60_raw==0x32 && d.telemetry().fx60_power_w==500);
    gx[0]=gx[1]=gx[2]=gx[3]='F'; d.handle_fu_fx_response("FX60",gx,4); assert(!d.telemetry().fx60_valid);
    gx[0]='X'; d.handle_fu_fx_response("FX60",gx,4); assert(!d.telemetry().fx60_valid);
}
void test_unknown_extension_pages() {
    Feedback f; DaikinDriver d(f); d.setup(); d.queries_.emplace_back("FX40");
    d.query_state_=DaikinDriver::QueryState::WaitingForAck;
    uint8_t gx[]={'G','X','4','0',0xFF,0x80,'0','7'}; d.handle_serial_result(Result::Frame,gx,sizeof(gx));
    assert((d.diagnosticResponse("FX40")==std::vector<uint8_t>{0xFF,0x80,'0','7'}));
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
        assert(d.query_index_==0 && !d.queries_[0].acked);
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
    assert(hal.tx[2]=='6' && hal.tx[6]==0x3C);
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
int main() { test_compatibility_fixtures(); test_unknown_sensor_led_gate(); test_explicit_settings_readback(); test_combined_mode_changes(); test_decoder_lengths(); test_delayed_wire_frames(); test_coarse_sentinels(); test_real_timers(); test_unknown_extension_pages(); test_live_power(); test_energy_counters(); test_model_name(); test_temperature_limits(); test_identification(); test_raw_research(); test_supplemental_sensors(); test_vane_angles(); test_fan_rpm(); test_indoor_coil(); test_independent_status(); test_vertical_position(); test_timer_lamp(); test_f4(); test_read_write_capabilities(); test_query_rates(); test_rz_identity(); test_versions(); test_special_decoders(); test_capability_bits(); test_no_f3_special_fallback(); test_humidity(); test_swing_preserves_humidity(); test_confirmed_d1(); test_explicit_d1(); test_led(); test_demand(); test_econo(); test_d6(); test_handshake(); test_checksums(); test_recovery(); test_no_unframed_fallback(); test_correlation(); std::cout << "S21 native tests passed\n"; }
