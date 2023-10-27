#include "running_rms.h"
#include "math.h"
#include <string.h>
#include <iostream>

using namespace std;

RunningRMS::RunningRMS(float _window_time, uint32_t _samprate, uint32_t _nsamps, float _tAtt, float _tRel) : m_window_time(_window_time), m_samprate(_samprate), m_nsamps(_nsamps), m_tAtt(_tAtt), m_tRel(_tRel)
{
    m_window_size = m_window_time * (float)m_samprate;
    m_window_pos = 0;
    m_sum_of_squares = 0.0;
    m_window = new float[m_window_size]();
    m_alphaA = expf(-logf(9.0f)/(m_samprate * m_tAtt));
    m_alphaR = expf(-logf(9.0f)/(m_samprate * m_tRel));
    m_lc = 0.0;
    m_lsprev = 0.0;
    m_ls = 0.0;
}

RunningRMS::~RunningRMS()
{
    delete[] m_window;
}

void RunningRMS::process(float* in, float* out)
{
    for(uint32_t i = 0; i < m_nsamps; i++) {
        
        m_lc = powf(in[i], 2.0f);
        
        if(m_lc <= m_lsprev) {
            m_ls = m_alphaA * m_lsprev + (1.0f - m_alphaA) * m_lc;
        }
        else {
            m_ls = m_alphaR * m_lsprev + (1.0f - m_alphaR) * m_lc;
        }
    
        m_lsprev = m_ls;
                
        out[i] = sqrt(m_ls);
    }
}
