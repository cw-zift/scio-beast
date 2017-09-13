# Public Key Pinning
An example of Public Key Pinning:
```
static bool verifyWithRfc2818AndPubKeyPin(
	const std::string& publicKeySha256, const std::string& domain, bool preverified, ssl::verify_context& ctx)
{
	if(!ssl::rfc2818_verification(domain)(preverified, ctx)) {
		return false;
	}

	X509* cert = ::X509_STORE_CTX_get_current_cert(ctx.native_handle());
	if(!cert) {
		return false;
	}

	//
	//	Public Key Pinning
	//
	//	https://groups.google.com/forum/#!topic/mailing.openssl.users/l77PfxfjNzA
	//
	if(0 == ::X509_STORE_CTX_get_error_depth(ctx.native_handle())) {
		const int pubKeyLen = ::i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), nullptr);
		
		std::vector<uint8_t> pubKeyBuf(pubKeyLen);
		unsigned char* pubKeyBufPtr = &pubKeyBuf[0];
		::i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &pubKeyBufPtr);
	
		const EVP_MD* md = ::EVP_get_digestbyname("sha256");
		
		uint8_t digestBuffer[EVP_MAX_MD_SIZE]; 
		uint32_t digestLen;
		EVP_Digest(&pubKeyBuf[0], pubKeyLen, digestBuffer, &digestLen, md, 0);
	
		std::stringstream actualPublicKeySha256;
		for(uint32_t i = 0; i < digestLen; ++i) {
			actualPublicKeySha256 << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digestBuffer[i]);
		}
	
		if(actualPublicKeySha256.str() != publicKeySha256) {
			return false;
		}
	}

	return true;
}

// ...

scio_beast::SocketClusterClientOptions clientOpts;

scio_beast::SecureConnectOptions& secureOpts = clientOpts.connectOptions.secureOptions;
secureOpts.context.reset(new ssl::context(ssl::context::tlsv12_client));

secureOpts.context->set_verify_mode(ssl::verify_peer | ssl::verify_fail_if_no_peer_cert);	

secureOpts.context->set_verify_callback(std::bind(
	&verifyWithRfc2818AndPubKeyPin, 
	"0345169322e8d06033f677a4fccddbd18c9c66963d0fcd694b1313be7be1ff8b",
	"contentwatch.com", 
	std::placeholders::_1,
	std::placeholders::_2
));

```