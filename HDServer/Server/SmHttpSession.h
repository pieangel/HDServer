#pragma once
#include "ServerDefine.h"
#include "SmWebsocketSession.h"
#include "SmCommon.h"
#include "SmSessionManager.h"


// This function produces an HTTP response for the given
	// request. The type of the response object depends on the
	// contents of the request, so the interface requires the
	// caller to pass a generic lambda for receiving the response.
template<
	class Body, class Allocator,
	class Send>
	void
	handle_request(
		beast::string_view doc_root,
		http::request<Body, http::basic_fields<Allocator>>&& req,
		Send&& send)
{
	// Returns a bad request response
	auto const bad_request =
		[&req](beast::string_view why)
	{
		http::response<http::string_body> res{ http::status::bad_request, req.version() };
		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		res.set(http::field::content_type, "text/html");
		res.keep_alive(req.keep_alive());
		res.body() = std::string(why);
		res.prepare_payload();
		return res;
	};

	// Returns a not found response
	auto const not_found =
		[&req](beast::string_view target)
	{
		http::response<http::string_body> res{ http::status::not_found, req.version() };
		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		res.set(http::field::content_type, "text/html");
		res.keep_alive(req.keep_alive());
		res.body() = "The resource '" + std::string(target) + "' was not found.";
		res.prepare_payload();
		return res;
	};

	// Returns a server error response
	auto const server_error =
		[&req](beast::string_view what)
	{
		http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		res.set(http::field::content_type, "text/html");
		res.keep_alive(req.keep_alive());
		res.body() = "An error occurred: '" + std::string(what) + "'";
		res.prepare_payload();
		return res;
	};

	// Make sure we can handle the method
	if (req.method() != http::verb::get &&
		req.method() != http::verb::head)
		return send(bad_request("Unknown HTTP-method"));

	// Request path must be absolute and not contain "..".
	if (req.target().empty() ||
		req.target()[0] != '/' ||
		req.target().find("..") != beast::string_view::npos)
		return send(bad_request("Illegal request-target"));

	// Build the path to the requested file
	std::string path = SmCommon::path_cat(doc_root, req.target());
	if (req.target().back() == '/')
		path.append("index.html");

	// Attempt to open the file
	beast::error_code ec;
	http::file_body::value_type body;
	body.open(path.c_str(), beast::file_mode::scan, ec);

	// Handle the case where the file doesn't exist
	if (ec == beast::errc::no_such_file_or_directory)
		return send(not_found(req.target()));

	// Handle an unknown error
	if (ec)
		return send(server_error(ec.message()));

	// Cache the size since we need it after the move
	auto const size = body.size();

	// Respond to HEAD request
	if (req.method() == http::verb::head)
	{
		http::response<http::empty_body> res{ http::status::ok, req.version() };
		res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
		res.set(http::field::content_type, SmCommon::mime_type(path));
		res.content_length(size);
		res.keep_alive(req.keep_alive());
		return send(std::move(res));
	}

	// Respond to GET request
	http::response<http::file_body> res{
		std::piecewise_construct,
		std::make_tuple(std::move(body)),
		std::make_tuple(http::status::ok, req.version()) };
	res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
	res.set(http::field::content_type, SmCommon::mime_type(path));
	res.content_length(size);
	res.keep_alive(req.keep_alive());
	return send(std::move(res));
}

class SmHttpSession : public std::enable_shared_from_this<SmHttpSession>
{
	// This queue is used for HTTP pipelining.
	class queue
	{
		enum
		{
			// Maximum number of responses we will queue
			limit = 8
		};

		// The type-erased, saved work item
		struct work
		{
			virtual ~work() = default;
			virtual void operator()() = 0;
		};

		SmHttpSession& self_;
		std::vector<std::unique_ptr<work>> items_;

	public:
		explicit
			queue(SmHttpSession& self)
			: self_(self)
		{
			static_assert(limit > 0, "queue limit must be positive");
			items_.reserve(limit);
		}

		// Returns `true` if we have reached the queue limit
		bool
			is_full() const
		{
			return items_.size() >= limit;
		}

		// Called when a message finishes sending
		// Returns `true` if the caller should initiate a read
		bool
			on_write()
		{
			BOOST_ASSERT(!items_.empty());
			auto const was_full = is_full();
			items_.erase(items_.begin());
			if (!items_.empty())
				(*items_.front())();
			return was_full;
		}

		// Called by the HTTP handler to send a response.
		template<bool isRequest, class Body, class Fields>
		void
			operator()(http::message<isRequest, Body, Fields>&& msg)
		{
			// This holds a work item
			struct work_impl : work
			{
				SmHttpSession& self_;
				http::message<isRequest, Body, Fields> msg_;

				work_impl(
					SmHttpSession& self,
					http::message<isRequest, Body, Fields>&& msg)
					: self_(self)
					, msg_(std::move(msg))
				{
				}

				void
					operator()()
				{
					http::async_write(
						self_.stream_,
						msg_,
						beast::bind_front_handler(
							&SmHttpSession::on_write,
							self_.shared_from_this(),
							msg_.need_eof()));
				}
			};

			// Allocate and store the work
			items_.push_back(
				boost::make_unique<work_impl>(self_, std::move(msg)));

			// If there was no previous work, start this one
			if (items_.size() == 1)
				(*items_.front())();
		}
	};

	beast::tcp_stream stream_;
	beast::flat_buffer buffer_;
	//std::shared_ptr<std::string const> doc_root_;
	std::shared_ptr<SmSessionManager> session_mgr_;
	queue queue_;

	// The parser is stored in an optional container so we can
	// construct it from scratch it at the beginning of each new message.
	boost::optional<http::request_parser<http::string_body>> parser_;

public:
	// Take ownership of the socket
	SmHttpSession(
		tcp::socket&& socket,
		std::shared_ptr<SmSessionManager> const& session_mgr)
		: stream_(std::move(socket))
		, session_mgr_(session_mgr)
		, queue_(*this)
	{
	}

	// Start the session
	void
		run()
	{
		do_read();
	}

private:
	void
		do_read()
	{
		// Construct a new parser for each message
		parser_.emplace();

		// Apply a reasonable limit to the allowed size
		// of the body in bytes to prevent abuse.
		parser_->body_limit(10000);

		// Set the timeout.
		stream_.expires_after(std::chrono::seconds(30));

		// Read a request using the parser-oriented interface
		http::async_read(
			stream_,
			buffer_,
			*parser_,
			beast::bind_front_handler(
				&SmHttpSession::on_read,
				shared_from_this()));
	}

	void
		on_read(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		// This means they closed the connection
		if (ec == http::error::end_of_stream)
			return do_close();

		if (ec)
			return SmCommon::fail(ec, "read");

		// See if it is a WebSocket Upgrade
		if (websocket::is_upgrade(parser_->get()))
		{
			// Create a websocket session, transferring ownership
			// of both the socket and the HTTP request.
			std::make_shared<SmWebsocketSession>(
				stream_.release_socket(), 
				session_mgr_)->do_accept(parser_->release());
			return;
		}

		// Send the response
		handle_request(session_mgr_->doc_root(), std::move(parser_->release()), queue_);

		// If we aren't at the queue limit, try to pipeline another request
		if (!queue_.is_full())
			do_read();
	}

	void
		on_write(bool close, beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if (ec)
			return SmCommon::fail(ec, "write");

		if (close)
		{
			// This means we should close the connection, usually because
			// the response indicated the "Connection: close" semantic.
			return do_close();
		}

		// Inform the queue that a write completed
		if (queue_.on_write())
		{
			// Read another request
			do_read();
		}
	}

	void
		do_close()
	{
		// Send a TCP shutdown
		beast::error_code ec;
		stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

		// At this point the connection is closed gracefully
	}
};
