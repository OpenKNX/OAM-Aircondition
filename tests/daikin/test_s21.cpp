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
int main() { test_handshake(); test_checksums(); test_recovery(); test_no_unframed_fallback(); test_correlation(); std::cout << "S21 native tests passed\n"; }
