/**
 * @file   nb_p.cpp
 * @version 1.0.0
 *
 * \brief Provides the class implementation of NB_P.
 *
 * Provides the class implementation of NB_P.
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "nb_p.h"
#include "nb_runtime.h"
#define MAX_TX_LEN 10000
#define TELEM_FILE_PATH "/home/crinard/Desktop/sample_telem.txt" 
#define VIDEO_FILE_PATH "/home/crinard/Desktop/sample_video.mp4" //TODO: Fix this
extern char nb__my_host_id[];

/**
 * Adds the module for Nb_pModuleClass in ns2.
 */
static class Nb_pModuleClass : public TclClass
{
public:
	Nb_pModuleClass()
		: TclClass("Module/UW/Nb_p")
	{
	}

	TclObject *
	create(int, const char *const *)
	{
		return (new Nb_pModule);
	}
} class_nb_p_module;

static int recvd_packets = 0;
static size_t recvd_bytes = 0;
static int sent_packets = 0;
static int sent_bytes = 0;

static void callback(int event, nb__connection_t * c) {
	if (event == QUEUE_EVENT_READ_READY) {
		char buff[1000];
		int len = nb__read(c, buff, 1000);
		buff[len] = 0;	
		printf("Server recieved %s from client\n", buff);	
		recvd_packets++;
		recvd_bytes += len;
	}
}

Nb_pModule::Nb_pModule():chkTimerPeriod(this, false), chkNetBlocksTimer(this, true) {
	nb__desert_init((void*)this);
	nb__net_init();
	char server_id[] = {0, 0, 0, 0, 0, 1};
	char client_id[] = {0, 0, 0, 0, 0, 2};
	memcpy(nb__my_host_id, server_id, 6);
	conn = nb__establish(client_id, 8080, 8081, callback);
	chkNetBlocksTimer.resched(10.0);
	chkTimerPeriod.resched(60.0);
	recvBuf = (Packet**) calloc(READ_BUF_LEN, sizeof(Packet*));
	recvBufLen = 0;
	sim_type = NOT_SET;
	is_compressed = false;
}

Nb_pModule::~Nb_pModule(){
	nb__destablish(conn);
	chkNetBlocksTimer.force_cancel();
	chkTimerPeriod.force_cancel();
}

int Nb_pModule::recvSyncClMsg(ClMessage *m)
{
	return Module::recvSyncClMsg(m);
}

int Nb_pModule::command(int argc, const char *const *argv)
{
	Tcl &tcl = Tcl::instance();
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {
			std::cout << "***********start***********\n";
			start_gen();
			return TCL_OK;
		} else if (strcasecmp(argv[1], "stop") == 0) {
			std::cout << "***********stop***********\n";
			stop_gen();
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getsentpkts") == 0) {
			tcl.resultf("%f", getSentPkts());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getrecvpkts") == 0) {
			tcl.resultf("%f", getRecvPkts());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getdroppkts") == 0) {
			tcl.resultf("%f", getDropPkts());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getdelay") == 0) {
			tcl.resultf("%f", getDelay());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getheadersize") == 0) {
			tcl.resultf("%f", getHeaderSize());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "settelem") == 0) {
			if (set_mode_telem()) return TCL_OK;
			return TCL_ERROR;
		} else if (strcasecmp(argv[1], "setvideostream") == 0) {
			if (set_mode_video()) return TCL_OK;
			return TCL_ERROR;
		} else if (strcasecmp(argv[1], "setcompressed") == 0) {
			set_compressed(1);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "setnocompression") == 0) {
			set_compressed(0);
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getrecvbytes") == 0) {
			tcl.resultf("%f", getRecvBytes());
			return TCL_OK;
		} else if (strcasecmp(argv[1], "getsentbytes") == 0) {
			tcl.resultf("%f", getSentBytes());
			return TCL_OK;
		}
	}
	return Module::command(argc, argv);
}

void Nb_pModule::recv(Packet *p)
{
	hdr_cmn *ch = HDR_CMN(p);
	if(ch->direction() != hdr_cmn::UP) {
		std::cerr << "Something weird here, packet direction is not UP" << std::endl;
	} else {
		assert(recvBufLen < READ_BUF_LEN);
		recvBuf[recvBufLen] = p;
		recvBufLen++;	
	}
	return;
}

void Nb_pModule::start_gen(void) {
	chkTimerPeriod.resched(60.0);
}

void Nb_pModule::stop_gen(void) {
	std::cout << "stop_gen\n";
	chkTimerPeriod.force_cancel();
	chkNetBlocksTimer.force_cancel();
	nb__desert_deinit();
}

void Nb_pModule::uwSendTimerAppl::expire(Event *e)
{
	if (isNb_) {
		nb__main_loop_step();
		m_->chkNetBlocksTimer.resched(10.0);
	} else {
		m_->sendPkt();
		m_->chkTimerPeriod.resched(120.0); // schedule next transmission
	}
}

void Nb_pModule::sendPkt(void) {
	if (sim_type == NOT_SET) {
		std::cerr << "sim_type not set\n";
		nb__send(conn, "Hello from ServerHello from ServerHello from ServerHello from ServerHello from ServerHello from Server", sizeof("Hello from ServerHello from ServerHello from ServerHello from ServerHello from ServerHello from Server"));
		sent_packets++;
		sent_bytes += sizeof("Hello from ServerHello from ServerHello from ServerHello from ServerHello from ServerHello from Server");
		return;
	} else if (sim_type == VIDEO_STREAM) {
		int len = MAX_TX_LEN;
		char* buff = genVideoPkt(&len);
		assert(len < MAX_TX_LEN);
		nb__send(conn, buff, len);
		sent_packets++;
		sent_bytes += len;
	} else if (sim_type == CONTROL_STREAM) {
		int len = MAX_TX_LEN;
		char* buff = genTelemPkt(&len);
		if (len > MAX_TX_LEN) {assert(false);}
		std::cerr << "send telem pkt, len = " << len << "\n";
		nb__send(conn, buff, len);
		free(buff);
		sent_packets++;
		sent_bytes += len;
	}
}

char* Nb_pModule::genVideoPkt(int * size) {
	std::string lines[1000];
	std::ifstream myfile(VIDEO_FILE_PATH);
	int a = 0;
	if(!myfile) {
		std::cerr << "VIDEO FILE NOT FOUND\n";
		return NULL;
	}
	if (!myfile.eof()) {
		getline(myfile, lines[a],'\n');
		a++;
	}
	char* buf = (char*)malloc(lines[a].size());
	memcpy(buf, lines[a].c_str(), lines[a].size());		
	*size = lines[a].size();		
	return buf;
}
static int a = 0;
char* Nb_pModule::genTelemPkt(int * size) {
	if (a > 1000) {
		*size = 0;
		return NULL;
	}
	std::ifstream fin(TELEM_FILE_PATH);
	a++;
	std::string tmp;
	if(!fin) {
		std::cerr << "TELEM FILE NOT FOUND\n";
		return NULL;
	}
	for (int i = 0; ((i < a) && !fin.eof()); i++) {
		getline(fin, tmp, '\n');
	}
	char* buf = (char*)malloc(tmp.size());
	memcpy(buf, tmp.c_str(), tmp.size());		
	*size = tmp.size();
	// std::cout << "telem packet size: " << *size << std::endl;
	fin.close();
	return buf;
}

double Nb_pModule::getSentPkts(void) {
	return sent_packets;
}
double Nb_pModule::getRecvPkts(void) {
	return recvd_packets;
}

double Nb_pModule::getDropPkts(void) {
	return 0.0;
}
double Nb_pModule::getDelay(void) {
	return 0.0;
}
double Nb_pModule::getRecvBytes(void) {
	return recvd_bytes;
}
double Nb_pModule::getHeaderSize(void) {
	return 0.0;
}
bool Nb_pModule::set_mode_telem(void) {
	if (sim_type != NOT_SET)
		return false;
	sim_type = CONTROL_STREAM;
	return true;
}
bool Nb_pModule::set_mode_video(void) {
	if (sim_type != NOT_SET)
		return false;
	sim_type = VIDEO_STREAM;
	return true;
}
void Nb_pModule::set_compressed(bool c) {
	is_compressed = c;
}
double Nb_pModule::getSentBytes(void) {
	return sent_bytes;
}