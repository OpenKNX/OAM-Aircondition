#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>

namespace daikin { namespace s21 {
inline bool forbidden(uint8_t b) { return b==2 || b==3 || b==6 || b==10 || b==0x15; }
inline int hex(uint8_t c) {
    if(c>='0' && c<='9') return c-'0';
    if(c>='A' && c<='F') return c-'A'+10;
    if(c>='a' && c<='f') return c-'a'+10;
    return -1;
}
inline bool decimal(const uint8_t* d,size_t n,bool sign=false) {
    if(!d || n!=(sign?4U:3U)) return false;
    for(size_t i=0;i<3;i++) if(d[i]<'0' || d[i]>'9') return false;
    return !sign || d[3]=='+' || d[3]=='-';
}
inline std::string responseHeader(std::string_view c) {
    if(c=="VS000M") return "VS";
    if(c=="A") return "C";
    std::string result(c);
    if(!result.empty() && result[0]=='F') result[0]='G';
    else if(!result.empty() && result[0]=='R') result[0]='S';
    return result;
}
// Exact lengths are specified even where the page's physical meaning is unknown.
inline size_t responseSize(std::string_view c) {
    if(c=="A") return 5;
    if(c=="VS000M") return 14;
    if(c=="FY10") return 8;
    if(c.substr(0,2)=="FU") return 32;
    if(c.substr(0,2)=="Rz") return 2;
    if(c=="RA" || c=="RB" || c=="RG" || c=="Rg") return 1;
    if(c=="RF" || c=="RW") return 2;
    if(c=="RK" || c=="RL" || c=="Rd" || c=="Re" || c=="Rb" || c=="Rm" || c=="Rt" || c=="RD" || c=="RE") return 3;
    if(c=="FX00" || c=="FX10" || c=="FX30" || c=="FX40" || c=="FX50" || c=="FXB0" || c=="FXC0" || c=="FX21" || c=="FX61" || c=="FX71" || c=="FX81") return 2;
    if(c=="FXD0" || c=="FXE0" || c=="FXF0" || c=="FX01" || c=="FX11" || c=="FX31" || c=="FX41") return 8;
    if(c=="FV") return 0; // no length is established by the reference
    if(c=="M" || c=="V" || c.substr(0,1)=="F" || c=="RH" || c=="RI" || c=="RX" || c=="Ra" || c=="RM" || c=="RN" || c=="RC") return 4;
    return 0;
}
inline bool validResponse(std::string_view c,const uint8_t* d,size_t n) {
    const auto expected=responseSize(c);
    if(!d || !n || n>32 || (expected && n!=expected)) return false;
    for(size_t i=0;i<n;i++) if(forbidden(d[i])) return false;
    if(c=="RH" || c=="RI" || c=="RX" || c=="Ra" || c=="RM" || c=="RN" || c=="RC") return decimal(d,n,true);
    if(c=="Rd" || c=="Re" || c=="RK" || c=="RL" || c=="RD" || c=="RE" || c=="Rm" || c=="Rt") return decimal(d,n);
    if(c=="RA" || c=="Rg") return d[0]=='0' || d[0]=='1';
    if(c=="RB") return std::string_view{"0123467"}.find(char(d[0]))!=std::string_view::npos;
    if(c=="RG") return std::string_view{"AB34567"}.find(char(d[0]))!=std::string_view::npos;
    if(c=="Rb" && !decimal(d,n)) return false;
    if(c=="Rb" && ((d[0]-'0')+(d[1]-'0')*10+(d[2]-'0')*100)>15) return false;
    if(c=="FC") for(size_t i=0;i<n;i++) if(d[i]<0x20 || d[i]>0x7E) return false;
    if(c=="F1") return (d[0]=='0' || d[0]=='1') && std::string_view{"0123467"}.find(char(d[1]))!=std::string_view::npos &&
        ((d[2]>=0x30 && d[2]<=0x7F) || (d[1]=='2' && d[2]==0x80) || (d[1]=='4' && d[2]==0xC0)) &&
        std::string_view{"AB34567"}.find(char(d[3]))!=std::string_view::npos;
    if(c=="F2") return (d[0]&0x30)==0x30 && (d[1]&0x30)==0x30 && (d[3]&0x80);
    if(c=="F3") {
        if(d[0]<'0' || d[0]>'3') return false;
        for(size_t i=1;i<3;i++) {
            if((d[0]-'0')&(1<<(i-1))) { if(d[i]<0x31 || d[i]>0xC0) return false; }
            else if(d[i]!=0x30 && d[i]!=0xFE) return false;
        }
    }
    if(c=="F5" || c=="RF") return std::string_view{"0127"}.find(char(d[0]))!=std::string_view::npos && (d[1]=='0' || d[1]==(c=="F5"?'?':'F'));
    if(c=="F6" || c=="FK") for(size_t i=0;i<4;i++) if((d[i]&0x30)!=0x30) return false;
    if(c=="F7") return d[0]>=0x30 && d[0]<=0x94 && (d[1]=='0' || d[1]=='2');
    if(c=="FR") return d[0]=='?' || (d[0]>='0' && d[0]<='5');
    if(c=="FB") return d[0]=='0' || d[0]=='1';
    if(c=="FS") return d[0]>='1' && d[0]<='4' && hex(d[1])>=0;
    if(c=="F8") {
        if(d[0]!='0') return false;
        if(d[1]==0) return d[2]==0 && d[3]==0;
        return (d[1]=='1' || d[1]=='2') && ((d[2]==0 && d[3]==0) || (d[2]=='0' && d[3]=='0'));
    }
    if(c=="FY00") {
        for(size_t i=0;i<4;i++) if(d[i]<'0' || d[i]>'9') return false;
        return (d[3]-'0')*10+d[2]-'0'>=3;
    }
    if(c=="M" || c=="V" || c=="FM" || c=="FX60" || c=="FY10" || c=="FY20" || c.substr(0,2)=="Rz")
        for(size_t i=0;i<n;i++) if(hex(d[i])<0) return false;
    if(c=="FT") for(size_t i=0;i<n;i++) if(d[i]<'0' || d[i]>'9') return false;
    if(c=="FU00") for(size_t i=0;i<n;i++) if(d[i]!='0' && d[i]!='3') return false;
    if(c=="FU04") for(size_t i=0;i<16;i++) if(hex(d[i])<0) return false;
    if(c=="VS000M") {
        for(size_t i=0;i<8;i++) if(d[i]<0x20 || d[i]>0x7E) return false;
        if(std::string_view(reinterpret_cast<const char*>(d+8),6)!="M00000") return false;
    }
    if(c=="FU05") for(size_t i=0;i<22 && d[i]!=0 && d[i]!=0xFF;i++) if(d[i]<0x20 || d[i]>0x7E) return false;
    return true;
}
} }
