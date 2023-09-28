//
// Copyright (c) 2018 Regents of the SIGNET lab, University of Padova.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University of Padova (SIGNET lab) nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file    uwsocket.h
 * @author  Roberto Francescon
 * @version 0.0.1
 * @brief   Class that implements a connector and, specifically, the socket
 *          connector. BSD sockets are used, to connect to either TCP or UDP.
 */

#ifndef MSOCKET_H
#define MSOCKET_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <uwconnector.h>

#include <iostream>
#include <string>

constexpr char udp_init_string[] = "a";

/**
 * Class that implements a TCP or UDP socket.
 * For each UwSocket object, a thread is spawned and dedicated to reading
 * the socket buffer ans retrieving data: while sending is done in
 * the calling thread.
 */
class UwSocket : public UwConnector {
 public:
  /**
   * Enum structure thet represents the transport protocol being used.
   * The two possible choices are, of course, TCP or UDP.
   */
  enum class Transport { TCP = 0, UDP = 1 };
  /**
   * Constructor of the UwSocket class
   */
  UwSocket();

  /**
   * Destructor of the UwSocket class
   */
  virtual ~UwSocket();

  /**
   * Method that opens a TCP or UDP connection, accordinto to UwSocket::proto
   * variable: the behavior depends both on the transport protocol selected
   * and the path specified in the constructor parameter.
   * If TCP is set:
   * - IP:PORT will try to connect to the provided IP and PORT
   * - PORT will try to connect to the provided port on localhost
   * if, otherwise, UDP is set:
   * - IP:PORT will save the IP and port for data forwarding
   * - PORT will bind the provided port to receive data through it
   * @return boolean true if connection is correctly opened, false otherwise
   */
  virtual bool openConnection(const std::string &path);

  /**
   * Method that closes an active connection to a device
   * @return boolean true if connection is correctly closed, false otherwise
   */
  virtual bool closeConnection();

  /**
   * Returns true if socket fd differs from -1, that means the
   * connection is up
   * @return if socket file descriptor is valid
   */
  virtual const bool isConnected();

  /**
   * Method that writes a command to the modem interface
   * @param msg std::string command to be sent to the device
   */
  virtual int writeToDevice(const std::string &msg);

  /**
   * Function that dumps data from the device's memory to a backup buffer.
   * The unloaded data is saved to a temporary buffer, to be parsed later.
   * @param pos position to start writing data to: a pointer to some buffer
   * @return ineger number of read bytes
   */
  virtual int readFromDevice(void *wpos, int maxlen);

  /**
   * Method that sets TCP as transport protocol
   */
  virtual void setTCP() { proto = Transport::TCP; };
  /**
   * Method that sets UDP as transport protocol
   */
  virtual void setUDP() {
    proto = Transport::UDP;
    std::cout << "UDP set" << std::endl;
  };
  /**
   * Method that sets SERVER role
   */
  virtual void setServer() { isClient = false; };

 private:
  /**
   * Integer value that stores the socket descriptor as generated by the
   * function UwSocket::openConnection().
   */
  int socketfd;
  /**
   * Transport protocol to be used: either Transport::TCP, Transport::UDP
   */
  Transport proto;
  /**
   * Bool value that defines the role of the socket
   */
  bool isClient;

  /**
   *
   */
  struct sockaddr_in cl_addr;
};

#endif
