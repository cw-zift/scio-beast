# scio-beast
[SocketCluster.io](socketcluster.io) C++ client utilizing [Boost.Beast](https://github.com/boostorg/beast) and [modern JSON](https://github.com/nlohmann/json).

# Status
Early work in progress!

## Needing Attention
* Simplify wss:// vs ws:// code
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

# Codecs
Codecs can be created by implementing the `scio_beast::ICodecEngine` interface. A `scio_beast::CodecEngineMinBin` that works with [sc-codec-min-bin](https://github.com/SocketCluster/sc-codec-min-bin) is included. For example:
```
std::shared_ptr<scio_beast::ICodecEngine> codecEngine(new scio_beast::CodecEngineMinBin());
// ...
connectOptions.setCodecEngine(codecEngine);
```

# License
See [LICENSE](LICENSE)