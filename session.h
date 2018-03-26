#pragma once

#include "includes.h"
#include "context.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket aSocket, std::size_t aBulkSize);
    
    void Start();
    void Deliver(const std::string& aMessage);

private:
	void DoRead();

    Context mContext;
    tcp::socket mSocket;
    std::string mReadMsg;
};
