#ifndef DNP3_ACTIONS_H
#define DNP3_ACTIONS_H

#include <opendnp3/ConsoleLogger.h>
#include <opendnp3/DNP3Manager.h>
#include <opendnp3/channel/PrintingChannelListener.h>
#include <opendnp3/gen/OperateType.h>
#include <opendnp3/gen/OperationType.h>
#include <opendnp3/gen/TripCloseCode.h>
#include <opendnp3/logging/LogLevels.h>
#include <opendnp3/master/DefaultMasterApplication.h>
#include <opendnp3/master/PrintingCommandResultCallback.h>
#include <opendnp3/master/PrintingSOEHandler.h>

#include <Windows.h>
#include <string.h>

#include <iomanip>
#include <thread>
using namespace std;
using namespace opendnp3;

Header GetHeader(int);
TripCloseCode GetTripCloseCode(uint8_t);
void Sbo(std::shared_ptr<IMaster>, OperationType, uint8_t, int, int, int);
void BruteForceOutstation(std::shared_ptr<IMaster>, uint8_t, int, int, int, uint8_t, uint8_t, uint32_t, uint32_t);
void ReadFromOutstation(std::shared_ptr<IMaster>, int, int, int, int);
void DisableUnsolicitedMessagesOnOutstation(std::shared_ptr<IMaster>, std::vector<int>);
void EnableUnsolicitedMessagesOnOutstation(std::shared_ptr<IMaster>, std::vector<int>);
void CollectAllDataFromOutstation(std::shared_ptr<IMaster>);
void RestartOutstation(std::shared_ptr<IMaster>, RestartType);
void ParseClasses(std::vector<int>, vector<Header>*);
void ToggleOnAllBreakers(std::shared_ptr<IMaster>, uint8_t, int, int, int);
void ToggleOffAllBreakers(std::shared_ptr<IMaster>, uint8_t, int, int, int);
void DenyTcpSessions(shared_ptr<IMaster>, DNP3Manager*, IPEndpoint*, MasterStackConfig, int, int);

/* Leverages some code from OpenDNP3 examples.
 *
 * Copyright 2013-2020 Automatak, LLC
 *
 * Licensed to Green Energy Corp (www.greenenergycorp.com) and Automatak
 * LLC (www.automatak.com) under one or more contributor license agreements.
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership. Green Energy Corp and Automatak LLC license
 * this file to you under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

class TestSOEHandler : public ISOEHandler
{
    virtual void BeginFragment(const ResponseInfo& info){};
    virtual void EndFragment(const ResponseInfo& info){};

    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values)
    {
        PrintHeading(info);
        PrintAll(info, values);
    };
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values)
    {
        PrintHeading(info);
        PrintAll(info, values);
    };
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values)
    {
        PrintHeading(info);
        PrintAll(info, values);
    };
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values)
    {
        PrintHeading(info);
        PrintAll(info, values);
    };
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values)
    {
        PrintHeading(info);
        PrintAll(info, values);
    };
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values)
    {
        PrintHeading(info);
        PrintAll(info, values);
    };
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values)
    {
        PrintHeading(info);
        PrintAll(info, values);
    };
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<OctetString>>& values){};
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<TimeAndInterval>>& values){};
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<BinaryCommandEvent>>& values){};
    virtual void Process(const HeaderInfo& info, const ICollection<Indexed<AnalogCommandEvent>>& values){};
    virtual void Process(const HeaderInfo& info, const ICollection<DNPTime>& values){};

    void PrintGroupVar(const HeaderInfo& info)
    {
        printf("%s: %s\n", GroupVariationSpec::to_string(info.gv), GroupVariationSpec::to_human_string(info.gv));
    };

    void PrintHeading(const HeaderInfo& info)
    {
        PrintGroupVar(info);
        std::cout << left << setw(20) << "index"
                  << "|" << left << setw(20) << "value"
                  << "|" << left << setw(20) << "flags"
                  << "|" << left << setw(20) << "time" << std::endl;
    };
    template<class T> static void PrintAll(const HeaderInfo& info, const ICollection<Indexed<T>>& values)
    {
        auto print = [&](const Indexed<T>& pair) { Print<T>(info, pair.value, pair.index); };
        values.ForeachItem(print);
    };

    template<class T> static void Print(const HeaderInfo& info, const T& value, uint16_t index)
    {
        std::cout << std::left << std::setw(20) << index << "|" << std::left << std::setw(20) << ValueToString(value)
                  << "|" << std::left << std::setw(20) << static_cast<int>(value.flags.value) << "|" << std::left
                  << std::setw(20) << value.time.value << std::endl;
    };

    template<class T> static std::string ValueToString(const T& meas)
    {
        std::ostringstream oss;
        oss << meas.value;
        return oss.str();
    };

    static std::string ValueToString(const DoubleBitBinary& meas)
    {
        return DoubleBitSpec::to_human_string(meas.value);
    }
};

#endif // DNP3_ACTIONS_H