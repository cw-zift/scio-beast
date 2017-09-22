//  Boost
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core/buffer_cat.hpp>
#include <boost/unordered_map.hpp>
#include <boost/asio/ssl/rfc2818_verification.hpp>

//  STL
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

//  scio_beast
#include "../src/scio_beast.hpp"

//  catch
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using json = nlohmann::json;

#define UNUSED(expr) do { (void)(expr); } while (0)

TEST_CASE("client can connect to socketcluster server", "[comm]") {

    using namespace boost;

    scio_beast::SocketClusterClientOptions clientOpts;

    clientOpts.connectOptions
        .setHost("localhost")
        .setPort("8000")
        ;

    auto client = scio_beast::SocketClusterClient::create(clientOpts);

    SECTION("basic connection functionality") {

        auto socket = client->socket();

        bool connectedEventReceived     = false;
        system::error_code disconnectEc = asio::error::make_error_code(asio::error::in_progress);

        socket->on<scio_beast::SCSocket::ConnectingEvent>([ &connectedEventReceived ] {
            connectedEventReceived = true;
        });

        socket->on<scio_beast::SCSocket::DisconnectEvent>([ &disconnectEc ](const system::error_code& ec) {
            disconnectEc = ec;
        });
    
        CHECK(scio_beast::SCSocket::State::CLOSED == socket->getState());
        socket->connect();
        CHECK(scio_beast::SCSocket::State::CONNECTING == socket->getState());

        //  wait... we should be open.
        this_thread::sleep_for(chrono::seconds(3));
        REQUIRE(scio_beast::SCSocket::State::OPEN == socket->getState());

        system::error_code ec = socket->disconnect();
        INFO("disconnect() returned " << ec.message()); //  :TODO: see notes in scio_beast about this. Needs cleaned up

        //  wait... we should have disconnected by now
        this_thread::sleep_for(chrono::seconds(3));     
        CHECK(scio_beast::SCSocket::State::CLOSED == socket->getState());

        client->shutdown();

        CHECK(connectedEventReceived);
        CHECK(disconnectEc == asio::error::eof);
    }

    SECTION("client to server emit") {
        auto socket = client->socket();

        struct AsyncStateChecks {
            AsyncStateChecks() : connResp(json::object()), resp(json::object()) {}

            json                connResp;
            json                resp;
            system::error_code  respEc;
        } asyncInfo;

        socket->on<scio_beast::SCSocket::ConnectEvent>([ socket, &asyncInfo ](const json& resp) {
            asyncInfo.connResp = resp;

            const json emitData = {
                { "name", "value" }
            };

            //  :TODO: how to test this further client side?
            socket->emit("event_no_resp", emitData);

            socket->emit(
                "event_with_resp",
                emitData,
                [ &asyncInfo ](boost::system::error_code ec, const json& resp) {
                    asyncInfo.resp      = resp;
                    asyncInfo.respEc    = ec;
                }
            );
        });

        socket->connect();

        //  wait... let events run
        this_thread::sleep_for(chrono::seconds(10));        
        socket->disconnect();

        //  wait... we should have disconnected by now
        this_thread::sleep_for(chrono::seconds(3));     
        CHECK(scio_beast::SCSocket::State::CLOSED == socket->getState());

        client->shutdown();

        CHECK(!asyncInfo.connResp.value("data", json::object()).empty());
        CHECK(asyncInfo.resp.value("got_it", false));
    }

    SECTION("client to server emit timeout") {
        scio_beast::SocketClusterClientOptions quickTimeoutClientOpts;

        quickTimeoutClientOpts.connectOptions
            .setHost("localhost")
            .setPort("8000")
            .setAckTimeout(2)   //  seconds
            ;

        auto toClient = scio_beast::SocketClusterClient::create(quickTimeoutClientOpts);

        auto socket = toClient->socket();

        struct AsyncStateChecks {
            AsyncStateChecks() : resp(json::object()) {}

            json                resp;
            system::error_code  respEc;
        } asyncInfo;

        socket->on<scio_beast::SCSocket::ConnectEvent>([ socket, &asyncInfo ](const json& resp) {
            UNUSED(resp);
            
            const json emitData = {
                { "ackTimeout", 2 }
            };

            //  we expect this to timeout
            socket->emit(
                "event_with_timed_out_resp",
                emitData,
                [ &asyncInfo ](boost::system::error_code ec, const json& resp) {
                    asyncInfo.respEc    = ec;
                    asyncInfo.resp      = resp;
                }
            );
        });

        socket->connect();

        //  wait... let events run
        this_thread::sleep_for(chrono::seconds(15));    //  we have to wait a bit longer this one

        socket->disconnect();

        //  wait... we should have disconnected by now
        this_thread::sleep_for(chrono::seconds(3));     
        CHECK(scio_beast::SCSocket::State::CLOSED == socket->getState());

        toClient->shutdown();

        CHECK(scio_beast::ack_timeout == asyncInfo.respEc);
        CHECK(!asyncInfo.resp.value("error", json::object()).value("message", "").empty());
    }
}
