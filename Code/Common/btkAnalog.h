/* 
 * The Biomechanical ToolKit
 * Copyright (c) 2009-2010, Arnaud Barré
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *     * Redistributions of source code must retain the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *     * Neither the name(s) of the copyright holders nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __btkAnalog_h
#define __btkAnalog_h

#include "btkMeasure.h"
#include "btkMacro.h"

namespace btk
{
  class Analog : public Measure<1>
  {
  public:
    typedef enum {Unknown = 0, PlusMinus10 = 10000, PlusMinus5 = 5000, PlusMinus2Dot5 = 2500, PlusMinus1Dot25 = 1250, PlusMinus1 = 1000} Gain;

    typedef SharedPtr<Analog> Pointer;
    typedef SharedPtr<const Analog> ConstPointer;
    
    static Pointer New(const std::string& label = "") {return Pointer(new Analog(label));};
    static Pointer New(int frameNumber) {return Pointer(new Analog("", frameNumber));};
    static Pointer New(const std::string& label, int frameNumber) {return Pointer(new Analog(label, frameNumber));};
    
    virtual ~Analog() {};
    
    const std::string& GetUnit() const {return this->m_Unit;};
    BTK_COMMON_EXPORT void SetUnit(const std::string& u);
    Gain GetGain() const {return this->m_Gain;};
    BTK_COMMON_EXPORT void SetGain(Gain g);
    int GetOffset() const {return this->m_Offset;};
    BTK_COMMON_EXPORT void SetOffset(int o);
    double GetScale() const {return this->m_Scale;};
    BTK_COMMON_EXPORT void SetScale(double s);
    Pointer Clone() const {return Pointer(new Analog(*this));}
    
  protected:
    BTK_COMMON_EXPORT Analog(const std::string& label);
    BTK_COMMON_EXPORT Analog(const std::string& label, int frameNumber, Gain g = Unknown);
    
  private:
    BTK_COMMON_EXPORT Analog(const Analog& toCopy);
    Analog& operator=(const Analog& ); // Not implemented.
    
    std::string m_Unit;
    Gain m_Gain;
    int m_Offset;
    double m_Scale;
  };
};

#endif // __btkAnalog_h
