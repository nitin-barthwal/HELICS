/*
Copyright � 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/
#pragma once

#include "NetworkBroker.hpp"

namespace helics
{
template <class COMMS, NetworkBrokerData::interface_type baseline>
NetworkCore<COMMS, baseline>::NetworkCore () noexcept
{
}

template <class COMMS, NetworkBrokerData::interface_type baseline>
NetworkCore<COMMS, baseline>::NetworkCore (const std::string &core_name)
    : CommsBroker<COMMS, CommonCore> (core_name)
{
}

template <class COMMS, NetworkBrokerData::interface_type baseline>
void NetworkCore<COMMS, baseline>::initializeFromArgs (int argc, const char *const *argv)
{
    if (brokerState == created)
    {
        std::unique_lock<std::mutex> lock (dataMutex);
        if (brokerState == created)
        {
            netInfo.initializeFromArgs (argc, argv, "tcp://127.0.0.1");
            CommonCore::initializeFromArgs (argc, argv);
        }
    }
}

template <class COMMS, NetworkBrokerData::interface_type baseline>
bool NetworkCore<COMMS, baseline>::brokerConnect ()
{
    std::lock_guard<std::mutex> lock (dataMutex);
    if (netInfo.brokerAddress.empty())  // cores require a broker
    {
        netInfo.brokerAddress = "localhost";
    }
    comms->loadNetworkInfo (netInfo);
    comms->setName (getIdentifier ());
    comms->setTimeout (networkTimeout);
    // comms->setMessageSize(maxMessageSize, maxMessageCount);
    auto res = comms->connect ();
    if (res)
    {
        if (netInfo.portNumber < 0)
        {
            netInfo.portNumber = comms->getPort ();
        }
    }
    return res;
}

template <class COMMS, NetworkBrokerData::interface_type baseline>
std::string NetworkCore<COMMS, baseline>::generateLocalAddressString () const
{
    if (comms->isConnected ())
    {
        return comms->getAddress ();
    }
    std::lock_guard<std::mutex> lock (dataMutex);
    if (!netInfo.localInterface.empty () && (netInfo.localInterface.back () == '*'))
    {
        return makePortAddress (netInfo.localInterface.substr (0, netInfo.localInterface.size () - 1),
                                netInfo.portNumber);
    }
    else
    {
        return makePortAddress (netInfo.localInterface, netInfo.portNumber);
    }
}

}  // namespace helics