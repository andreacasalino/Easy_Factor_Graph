#pragma once
#ifndef __SHELL_CLIENTS__H__
#define __SHELL_CLIENTS__H__

#include "Http_Server.h"
#include <thread>
#include <mutex>
#include <list>
#include "../../CRF/Header/Graphical_model.h"
#include "../__Header/Command.h"

class CRF_Shell {
public:
	CRF_Shell() : Graph(NULL), Graph_has_changed(false), JS_Interface(NULL) {  };
	~CRF_Shell();
	
	class Client {
	public:
		struct Client_Data {
			std::mutex*		Mutex;
			bool					bRequest;
			Command*		Request;
			bool					bResponse;
			std::string			Response;
		};

		Client(CRF_Shell* shell_to_link);
		virtual ~Client();
	protected:
		const std::string* __get_Response() { return &this->Data.Response; };
		const Command*__get_Request() { return this->Data.Request; };

		virtual void			__get_next_command(std::string* command) = 0;
		virtual void			__send_response(const std::string& response)= 0;

		void						__allow_loop_to_start();
	private:
		void						__loop();
		void						__write_command_and_wait_response();
// data
		bool						__loop_can_start;
		Client_Data			Data;
		std::thread*			th__loop;
	};

	void Activate_loop();
private:
	void __get_response(const Command* command, std::string* result);
	void __get_graph_JSON(std::string* graph_JSON);									//null when nothing has to be notified
// data
	std::list<Client::Client_Data*>								Requests;
	std::list<Segugio::Categoric_var*>					Open_set;							//variables created but not already inserted in the graph
	Segugio::Graph*														Graph;								//actual graph description
	bool																			Graph_has_changed;
	Client*																		JS_Interface;
};



class JS_Client : public CRF_Shell::Client {
public:
	JS_Client(CRF_Shell* shell_to_link);
private:
	virtual void			__get_next_command(std::string* command);
	virtual void			__send_response(const std::string& response);
//data
	Http_Server			Listener;
};

#endif 