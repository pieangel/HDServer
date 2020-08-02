
#include "SmWebsocketSession.h"
#include "ServerDefine.h"
#include <iostream>
#include "SmSessionManager.h"
#include "../Service/SmProtocolManager.h"
#include "../Log/loguru.hpp"


void
SmWebsocketSession::
on_send(boost::shared_ptr<std::string const> const& ss)
{
	// Always add to queue
	queue_.push_back(ss);

	// Are we already writing?
	if (queue_.size() > 1) {
		//LOG_F(INFO, "on_send qsize = %d", queue_.size());
		return;
	}

	// We are not currently writing, so send this immediately
	ws_.async_write(
		net::buffer(*queue_.front()),
		beast::bind_front_handler(
			&SmWebsocketSession::on_write,
			shared_from_this()));
}


SmWebsocketSession::~SmWebsocketSession()
{
	session_mgr_->leave(this);
}

void SmWebsocketSession::send(boost::shared_ptr<std::string const> const& ss)
{
	// Post our work to the strand, this ensures
	// that the members of `this` will not be
	// accessed concurrently.
	try {
		boost::asio::post(
			ws_.get_executor(),
			beast::bind_front_handler(
				&SmWebsocketSession::on_send,
				shared_from_this(),
				ss));

	}
	catch (std::exception e) {
		LOG_F(INFO, "send error msg = %s", e.what());
	}
}

void SmWebsocketSession::close_socket()
{
	ws_.next_layer().close();
}

void SmWebsocketSession::on_accept(beast::error_code ec)
{
	// Handle the error, if any
	if (ec)
		return SmCommon::fail(ec, "accept");

	// Add this session to the list of active sessions
	session_mgr_->join(this);

	do_read();
}

void SmWebsocketSession::do_read()
{
	// Read a message into our buffer
	ws_.async_read(
		buffer_,
		beast::bind_front_handler(
			&SmWebsocketSession::on_read,
			shared_from_this()));
}

void SmWebsocketSession::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	// This indicates that the websocket_session was closed
	if (ec == websocket::error::closed)
		return;

	if (ec) {
		SmCommon::fail(ec, "read");
		session_mgr_->leave(this);
		return;
	}

	// Send to all connections
	//session_mgr_->send(beast::buffers_to_string(buffer_.data()));
	SmProtocolManager* msgMgr = SmProtocolManager::GetInstance();
	msgMgr->OnMessage(beast::buffers_to_string(buffer_.data()), this);
	// Clear the buffer
	buffer_.consume(buffer_.size());

	do_read();
}

void SmWebsocketSession::on_write(beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec)
		return SmCommon::fail(ec, "write");

	// Remove the string from the queue
	queue_.erase(queue_.begin());

	// Send the next message if any
	if (!queue_.empty())
		ws_.async_write(
			net::buffer(*queue_.front()),
			beast::bind_front_handler(
				&SmWebsocketSession::on_write,
				shared_from_this()));
}
