# scio-beast
[SocketCluster.io](socketcluster.io) C++ client utilizing [Boost.Beast](https://github.com/boostorg/beast) and [modern JSON](https://github.com/nlohmann/json).

# Status
Early work in progress!

## Needing Attention
* Simplify wss:// vs ws:// code
* MessagePack support
* SCChannel.publish()
* General code cleanup

# Dependencies
* C++11 or higher
* [Boost.Beast](https://github.com/boostorg/beast) (header only)
* [JSON](https://github.com/nlohmann/json) (header only)
* OpenSSL to link against

# Usage
```
// create options
scio_beast::SocketClusterClientOptions clientOpts;

clientOpts.connectOptions
  .setHost("localhost")
  .setPort("8000")
  ;

// create a client
auto client = scio_beast::SocketClusterClient::create(clientOpts);

// create a client socket
auto socket = client->socket();

socket->on<scio::beast::SCSocket::ConnectEvent>(
  [](const json& resp) {
    std::cout << "Connected!" << std::endl;
  }
);

// connect
socket->connect();
```

# License
See [LICENSE](LICENSE)