#pragma once
#include "ServerDefine.h"
#include "SmCommon.h"
#include <memory>
class SmSessionManager;
class SmWebsocketSession : public std::enable_shared_from_this<SmWebsocketSession>
{
	websocket::stream<beast::tcp_stream> ws_;
	beast::flat_buffer buffer_;
	std::vector<boost::shared_ptr<std::string const>> queue_;
	std::shared_ptr<SmSessionManager> session_mgr_;
	int _SessionID = 0;
	// 0 : 老馆 家南, 1 : 矫技 家南
	int _Type = 0;
public:
	// Take ownership of the socket
	explicit
		SmWebsocketSession(tcp::socket && socket)
		: ws_(std::move(socket))
	{
	}

	SmWebsocketSession(
		tcp::socket&& socket,
		std::shared_ptr<SmSessionManager> const& session_mgr)
		: ws_(std::move(socket))
		, session_mgr_(session_mgr)
	{

	}

	~SmWebsocketSession();

	void send(boost::shared_ptr<std::string const> const& ss);

	// Start the asynchronous accept operation
	template<class Body, class Allocator>
	void do_accept(http::request<Body, http::basic_fields<Allocator>> req)
	{
		ws_.write_buffer_bytes(8192);
		// Set suggested timeout settings for the websocket
		ws_.set_option(
			websocket::stream_base::timeout::suggested(
				beast::role_type::server));

		// Set a decorator to change the Server of the handshake
		ws_.set_option(websocket::stream_base::decorator(
			[](websocket::response_type & res)
			{
				res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" advanced-server");
			}));

		// Accept the websocket handshake
		ws_.async_accept(
			req,
			beast::bind_front_handler(
				&SmWebsocketSession::on_accept,
				shared_from_this()));
	}

	void close_socket();
	int GetSendBufferQueueSize() {
		return queue_.size();
	}
	int SessionID() const { return _SessionID; }
	void SessionID(int val) { _SessionID = val; }
	int Type() const { return _Type; }
	void Type(int val) { _Type = val; }
private:
	void on_send(boost::shared_ptr<std::string const> const& ss);
	void on_accept(beast::error_code ec);

	void do_read();


	void on_read(
			beast::error_code ec,
			std::size_t bytes_transferred);

	void on_write(
		beast::error_code ec,
		std::size_t bytes_transferred);
};
