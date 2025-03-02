//
//  AudioBuf.hpp
//  MacSmoov
//
//  Created by Paul Zaremba on 8/12/23.
//

#ifndef AudioBuf_hpp
#define AudioBuf_hpp

#include <stdio.h>
#include <cstdint>
#include <string>
#include <ios>

namespace fmsmoov {

using namespace std;

enum class AudioBufType {
    REFERENCE,
    REAL
};

class AudioBuf {
public:
    AudioBuf(AudioBufType _type, const string& _name, uint32_t _n_samps, AudioBuf* _refsrc = NULL);
    AudioBuf(const AudioBuf& rhs);
    void operator = (const AudioBuf& rhs);
    virtual ~AudioBuf();
    void get_name(string& name);
    void set_name(const string& _name);
    void write(float* in, uint32_t _n_samps);
    float* read(float* out, uint32_t* _n_samps);
    float* getbuf();
    void setbuf(float* ref);
    AudioBufType get_type();
    void set_type(AudioBufType _type);
private:
    AudioBufType type;
    string name;
    uint32_t n_samps;
    float* buf;
};

}

#endif /* AudioBuf_hpp */
