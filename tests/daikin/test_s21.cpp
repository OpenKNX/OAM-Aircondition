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
int main() { test_handshake(); test_checksums(); test_recovery(); std::cout << "S21 native tests passed\n"; }
