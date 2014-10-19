#include "libskype_internals.h"
#include "tokenizer.h"

#include <stdlib.h>

#include <string>
#include <sstream>

using namespace std;

LibSkypeChat::LibSkypeChat(LibSkype_internals* connection, string id) {
	stringstream ss;
	_id = id;
	_activity_timestamp = 0;
	_connection = connection;
	_initialized = false;
}

void LibSkypeChat::message_handler(const string& message) {
	Tokenizer token(message);
	string cmd = token.next();

	if (cmd == "ACTIVITY_TIMESTAMP") {
		stringstream ss;
		_activity_timestamp = atoi(token.next().c_str());
		cout << "Chat " << _id << " timestamp is now " << _activity_timestamp << endl;
		if (_initialized) {
			ss << "GET CHAT " << _id << " RECENTCHATMESSAGES";
			_connection->api_message(ss.str());
		}
		_initialized = true;
	} else
	if (cmd == "RECENTCHATMESSAGES") {
		stringstream ss;
		vector<string> messages;
		token.remaining(messages);

		unsigned int max = 0;
		for (unsigned idx=0; idx<messages.size(); idx++) {
			unsigned int messageid = atoi(messages[idx].c_str());
			if (messageid <= _connection->get_max_message_id())
				continue;
			cout << "Max id is " << _connection->get_max_message_id() <<  ": Fetching message ID " << messageid << endl;
			if (messageid > max)
				max = messageid;
			ss.str("");
			ss << "GET CHATMESSAGE " << messages[idx] << " STATUS";
			_connection->api_message(ss.str());
		}
		_connection->update_max_message_id(max);

	} else
		cout << "Unhandled: " << message << endl;
}

