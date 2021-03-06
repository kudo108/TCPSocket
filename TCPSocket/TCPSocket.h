/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-better
 @ 2015 QuanNguyen
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __TCPSocket_h__
#define __TCPSocket_h__

#include "cocos2d.h"
#include "ByteBuffer.h"
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Packet.h"


#define kCCSocketMaxPacketSize (16 * 1024)
#define kCCSocketDefaultTimeout 30
#define kCCSocketInputBufferDefaultSize (64 * 1024)
#define kCCSocketOutputBufferDefaultSize (8 * 1024)
#define kCCSocketError -1
#define kCCSocketInvalid -1

namespace funny {
    namespace network {
        
        class TCPSocketHub;
        
        /**
         * TCP socket
         */
        class CC_DLL TCPSocket : public cocos2d::Ref {
            friend class TCPSocketHub;
            
        private:
            /// read buffer, it is a loop buffer
            char m_inBuf[kCCSocketInputBufferDefaultSize];
            
            /// available data in read buffer
            int m_inBufLen;
            
            /// block time for waiting socket connection
            int m_blockSec;
            
            /// pthread mutex
            pthread_mutex_t m_mutex;
            
        private:
            // we wait here until socket is connected or failed
            static void* tcpThreadEntry(void* arg);
            
            /// receive data from socket until no more data or buffer full, or error
            bool recvFromSock();
            
            /// has error
            bool hasError();
            
            /// close socket
            void closeSocket();
            
            /// compact in buf
            void compactInBuf(int consumed);
            
        protected:
            /**
             * init socket
             *
             * @param hostname host name or ip address, ipv4 only
             * @param port port
             * @param tag tag of socket
             * @param blockSec block time when create this socket, 0 means not block
             * @param keepAlive true means keep socket alive
             * @return true means initialization successful
             */
            bool init(const std::string& hostname, int port, int tag = -1, int blockSec = kCCSocketDefaultTimeout, bool keepAlive = false);
            
        public:
            TCPSocket();
            virtual ~TCPSocket();
            
            /**
             * create socket instance
             *
             * @param hostname host name or ip address, ipv4 only
             * @param port port
             * @param tag tag of socket
             * @param blockSec block time when create this socket, 0 means not block
             * @param keepAlive true means keep socket alive
             * @return instance or NULL if failed
             */
            static TCPSocket* create(const std::string& hostname, int port, int tag = -1, int blockSec = kCCSocketDefaultTimeout, bool keepAlive = false);
            
            /**
             * add packet to send queue
             *
             * @param p packet
             */
            void sendPacket(Packet* p);
            
            /**
             * check is there any data can be read
             *
             * @return true means socket is valid and readable, false means socket is closed.
             */
            bool hasAvailable();
            
            /// socket handle
            CC_SYNTHESIZE_READONLY(int, m_socket, Socket);
            
            /// tag
            CC_SYNTHESIZE_READONLY(int, m_tag, Tag);
            
            /// hub reference
            CC_SYNTHESIZE(TCPSocketHub*, m_hub, Hub);
            
            /// connected
            CC_SYNTHESIZE_READONLY(bool, m_connected, Connected);
            
            /// stop
            CC_SYNTHESIZE(bool, m_stop, Stop);
            
            /// server name
            CC_SYNTHESIZE_READONLY_PASS_BY_REF(std::string, m_hostname, Hostname);
            
            /// port
            CC_SYNTHESIZE_READONLY(int, m_port, Port);
            
            /// send queue
            CC_SYNTHESIZE_READONLY_PASS_BY_REF(cocos2d::Vector<Packet *>, m_sendQueue, SendQueue);
        };
        
    }
}

#endif //__TCPSocket_h__