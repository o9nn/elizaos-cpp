#include "elizaos/mcp_gateway.hpp"
#include "elizaos/agentlogger.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <deque>
#include <limits>
#include <random>
#include <sstream>
#include <unordered_set>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif
#ifndef _SSIZE_T_DEFINED
using ssize_t = long long;
#define _SSIZE_T_DEFINED
#endif
namespace {
int closeSocket(int fd) { return ::closesocket(static_cast<SOCKET>(fd)); }
void shutdownSocket(int fd) { ::shutdown(static_cast<SOCKET>(fd), SD_BOTH); }
struct WinsockInit { WinsockInit(){ WSADATA d; WSAStartup(MAKEWORD(2,2), &d); } ~WinsockInit(){ WSACleanup(); } };
void ensureWinsock() { static WinsockInit init; }
}
#else
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
namespace {
int closeSocket(int fd) { return ::close(fd); }
void shutdownSocket(int fd) { ::shutdown(fd, SHUT_RDWR); }
void ensureWinsock() {}
}
#endif

namespace elizaos {
namespace {
using Clock = std::chrono::steady_clock;

MCPJsonValue rpcError(uint64_t id, int code, const std::string& message) {
    return {{"jsonrpc", "2.0"}, {"id", id},
            {"error", {{"code", code}, {"message", message}}}};
}

bool validParams(const MCPJsonValue& value) { return value.is_object() || value.is_array(); }
bool validError(const MCPJsonValue& value) {
    return value.is_object() && value.contains("code") && value["code"].is_number_integer() &&
           value.contains("message") && value["message"].is_string();
}
bool validEnvelope(const MCPJsonValue& value, std::string& error) {
    if (!value.is_object()) { error = "JSON-RPC message must be an object"; return false; }
    if (!value.contains("jsonrpc") || !value["jsonrpc"].is_string() ||
        value["jsonrpc"].get<std::string>() != "2.0") {
        error = "JSON-RPC version must be exactly 2.0"; return false;
    }
    const bool request = value.contains("method");
    const bool response = value.contains("result") || value.contains("error");
    if (request == response) { error = "JSON-RPC envelope has an invalid shape"; return false; }
    if (request) {
        if (!value["method"].is_string() || value["method"].get<std::string>().empty()) {
            error = "JSON-RPC method must be a non-empty string"; return false;
        }
        if (value.contains("params") && !validParams(value["params"])) {
            error = "JSON-RPC params must be an object or array"; return false;
        }
        if (value.contains("id") && !(value["id"].is_number_integer() ||
            value["id"].is_number_unsigned() || value["id"].is_string() || value["id"].is_null())) {
            error = "JSON-RPC id has an unsupported type"; return false;
        }
        return true;
    }
    if (!value.contains("id") || !(value["id"].is_number_integer() ||
        value["id"].is_number_unsigned() || value["id"].is_string() || value["id"].is_null())) {
        error = "JSON-RPC response requires a valid id"; return false;
    }
    if (value.contains("result") == value.contains("error")) {
        error = "JSON-RPC response requires exactly one of result or error"; return false;
    }
    if (value.contains("error") && !validError(value["error"])) {
        error = "JSON-RPC error requires integer code and string message"; return false;
    }
    return true;
}

template<class Callback, class... Args>
void invokeCallback(const Callback& callback, Args&&... args) noexcept {
    if (!callback) return;
    try { callback(std::forward<Args>(args)...); } catch (...) {}
}

std::string trim(std::string value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
}
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return value;
}
bool hasToken(const std::string& value, const std::string& token) {
    std::istringstream stream(value); std::string item;
    while (std::getline(stream, item, ',')) if (lower(trim(item)) == lower(token)) return true;
    return false;
}
uint32_t rol(uint32_t value, unsigned bits) { return (value << bits) | (value >> (32U - bits)); }
std::array<uint8_t,20> sha1(const std::string& input) {
    std::vector<uint8_t> bytes(input.begin(), input.end());
    const uint64_t bits = static_cast<uint64_t>(bytes.size()) * 8U;
    bytes.push_back(0x80U); while (bytes.size() % 64U != 56U) bytes.push_back(0);
    for (int shift=56; shift>=0; shift-=8) bytes.push_back(static_cast<uint8_t>(bits >> shift));
    uint32_t h0=0x67452301U,h1=0xefcdab89U,h2=0x98badcfeU,h3=0x10325476U,h4=0xc3d2e1f0U;
    for (size_t offset=0; offset<bytes.size(); offset+=64U) {
        uint32_t w[80]{};
        for (size_t i=0;i<16U;++i) { const size_t b=offset+i*4U;
            w[i]=(uint32_t(bytes[b])<<24U)|(uint32_t(bytes[b+1])<<16U)|(uint32_t(bytes[b+2])<<8U)|bytes[b+3]; }
        for (size_t i=16;i<80U;++i) w[i]=rol(w[i-3]^w[i-8]^w[i-14]^w[i-16],1);
        uint32_t a=h0,b=h1,c=h2,d=h3,e=h4;
        for (size_t i=0;i<80U;++i) { uint32_t f=0,k=0;
            if(i<20U){f=(b&c)|((~b)&d);k=0x5a827999U;} else if(i<40U){f=b^c^d;k=0x6ed9eba1U;}
            else if(i<60U){f=(b&c)|(b&d)|(c&d);k=0x8f1bbcdcU;} else {f=b^c^d;k=0xca62c1d6U;}
            const uint32_t t=rol(a,5)+f+e+k+w[i]; e=d;d=c;c=rol(b,30);b=a;a=t; }
        h0+=a;h1+=b;h2+=c;h3+=d;h4+=e;
    }
    std::array<uint8_t,20> out{}; const uint32_t h[]={h0,h1,h2,h3,h4};
    for(size_t i=0;i<5U;++i){out[i*4]=uint8_t(h[i]>>24U);out[i*4+1]=uint8_t(h[i]>>16U);out[i*4+2]=uint8_t(h[i]>>8U);out[i*4+3]=uint8_t(h[i]);}
    return out;
}
std::string b64(const uint8_t* data, size_t size) {
    static constexpr char chars[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out; out.reserve(((size+2U)/3U)*4U);
    for(size_t i=0;i<size;i+=3U){const uint32_t v=(uint32_t(data[i])<<16U)|(i+1<size?uint32_t(data[i+1])<<8U:0U)|(i+2<size?data[i+2]:0U);
        out+=chars[(v>>18U)&63U];out+=chars[(v>>12U)&63U];out+=i+1<size?chars[(v>>6U)&63U]:'=';out+=i+2<size?chars[v&63U]:'=';}
    return out;
}
std::string acceptFor(const std::string& key){const auto hash=sha1(key+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11");return b64(hash.data(),hash.size());}
std::string randomKey(){std::array<uint8_t,16> bytes{};std::random_device rd;for(auto& b:bytes)b=uint8_t(rd());return b64(bytes.data(),bytes.size());}

struct Endpoint { std::string host; std::string path="/"; int port=80; };
bool parseEndpoint(const std::string& url, Endpoint& endpoint, std::string& error) {
    std::string rest;
    if(url.rfind("ws://",0)==0) rest=url.substr(5);
    else if(url.rfind("wss://",0)==0){error="wss:// requires an injected TLS adapter";return false;}
    else{error="WebSocket URL must use ws:// or an injected adapter";return false;}
    const auto slash=rest.find('/'); const std::string authority=slash==std::string::npos?rest:rest.substr(0,slash);
    if(slash!=std::string::npos) endpoint.path=rest.substr(slash);
    const auto colon=authority.rfind(':'); endpoint.host=colon==std::string::npos?authority:authority.substr(0,colon);
    if(endpoint.host.empty()){error="WebSocket URL has no host";return false;}
    if(colon!=std::string::npos){try{const long p=std::stol(authority.substr(colon+1));if(p<=0||p>65535)throw std::out_of_range("port");endpoint.port=int(p);}catch(...){error="invalid WebSocket port";return false;}}
    return true;
}

class NativeWsAdapter final : public WebSocketTransport::Adapter {
public:
    explicit NativeWsAdapter(size_t maxFrame):maxFrame_(maxFrame){}
    ~NativeWsAdapter() override { close(); }
    bool connect(const std::string& url,const std::string& protocol,std::chrono::milliseconds timeout,
                 std::string& acknowledgement,std::string& error) override {
        close(); cancelled_.store(false); Endpoint ep; if(!parseEndpoint(url,ep,error))return false; ensureWinsock();
        addrinfo hints{};hints.ai_family=AF_UNSPEC;hints.ai_socktype=SOCK_STREAM;addrinfo* addresses=nullptr;const std::string port=std::to_string(ep.port);
        if(::getaddrinfo(ep.host.c_str(),port.c_str(),&hints,&addresses)!=0||!addresses){error="DNS resolution failed";return false;}
        for(addrinfo* a=addresses;a;a=a->ai_next){const int candidate=int(::socket(a->ai_family,a->ai_socktype,a->ai_protocol));if(candidate<0)continue;
            if(::connect(candidate,a->ai_addr,int(a->ai_addrlen))==0){fd_.store(candidate);break;}closeSocket(candidate);}
        ::freeaddrinfo(addresses);if(fd_.load()<0){error="TCP connect failed";return false;}
        const std::string key=randomKey();std::string request="GET "+ep.path+" HTTP/1.1\r\nHost: "+ep.host+":"+port+"\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: "+key+"\r\nSec-WebSocket-Version: 13\r\n";
        if (!protocol.empty()) { request += "Sec-WebSocket-Protocol: " + protocol + "\r\n"; }
        request += "\r\n";
        if(!sendAll(reinterpret_cast<const uint8_t*>(request.data()),request.size(),error)){close();return false;}
        std::string response;while(response.find("\r\n\r\n")==std::string::npos){if(response.size()>16384U){error="handshake too large";close();return false;}uint8_t byte=0;
            if(readExact(&byte,1,timeout,error)!=WebSocketTransport::ReceiveResult::Status::MESSAGE){if(error.empty())error="handshake timeout";close();return false;}response+=char(byte);}
        std::istringstream lines(response);std::string line;if(!std::getline(lines,line)||(line.find("HTTP/1.1 101")!=0&&line.find("HTTP/1.0 101")!=0)){error="upgrade rejected";close();return false;}
        std::unordered_map<std::string,std::string> headers;while(std::getline(lines,line)){if(line=="\r"||line.empty())break;const auto c=line.find(':');if(c!=std::string::npos)headers[lower(trim(line.substr(0,c)))]=trim(line.substr(c+1));}
        if(lower(headers["upgrade"])!="websocket"||!hasToken(headers["connection"],"upgrade")||headers["sec-websocket-accept"]!=acceptFor(key)){error="invalid upgrade acknowledgement";close();return false;}
        if(headers.count("sec-websocket-protocol")&&!protocol.empty()&&headers["sec-websocket-protocol"]!=protocol){error="unexpected subprotocol";close();return false;}
        acknowledgement="HTTP 101; accept="+headers["sec-websocket-accept"];return true;
    }
    bool sendText(const std::string& payload,bool,std::string& error) override{return sendFrame(0x1U,payload,error);}
    WebSocketTransport::ReceiveResult receive(std::chrono::milliseconds timeout) override {
        WebSocketTransport::ReceiveResult out;uint8_t h[2]{};out.status=readExact(h,2,timeout,out.error);if(out.status!=WebSocketTransport::ReceiveResult::Status::MESSAGE)return out;
        if((h[0]&0x70U)!=0U||(h[0]&0x80U)==0U){out.status=WebSocketTransport::ReceiveResult::Status::ERROR;out.error="fragmented/RSV frame unsupported";return out;}
        if((h[1]&0x80U)!=0U){out.status=WebSocketTransport::ReceiveResult::Status::ERROR;out.error="server frame is masked";return out;}const uint8_t op=h[0]&0x0fU;uint64_t n=h[1]&0x7fU;
        if(n==126U){uint8_t x[2]{};out.status=readExact(x,2,timeout,out.error);if(out.status!=WebSocketTransport::ReceiveResult::Status::MESSAGE)return out;n=(uint64_t(x[0])<<8U)|x[1];}
        else if(n==127U){uint8_t x[8]{};out.status=readExact(x,8,timeout,out.error);if(out.status!=WebSocketTransport::ReceiveResult::Status::MESSAGE)return out;if(x[0]&0x80U){out.status=WebSocketTransport::ReceiveResult::Status::ERROR;out.error="invalid frame length";return out;}n=0;for(uint8_t b:x)n=(n<<8U)|b;}
        if(n>maxFrame_||n>uint64_t(std::numeric_limits<size_t>::max())){out.status=WebSocketTransport::ReceiveResult::Status::ERROR;out.error="frame exceeds limit";return out;}
        std::string payload(size_t(n),'\0');if(n){out.status=readExact(reinterpret_cast<uint8_t*>(&payload[0]),payload.size(),timeout,out.error);if(out.status!=WebSocketTransport::ReceiveResult::Status::MESSAGE)return out;}
        if(op==0x8U){out.status=WebSocketTransport::ReceiveResult::Status::CLOSED;return out;}if(op==0x9U){std::string e;(void)sendFrame(0xAU,payload,e);out.status=WebSocketTransport::ReceiveResult::Status::TIMEOUT;return out;}
        if(op==0xAU){out.status=WebSocketTransport::ReceiveResult::Status::TIMEOUT;return out;}if(op!=0x1U){out.status=WebSocketTransport::ReceiveResult::Status::ERROR;out.error="unsupported opcode";return out;}
        out.payload=std::move(payload);out.status=WebSocketTransport::ReceiveResult::Status::MESSAGE;return out;
    }
    void cancel() noexcept override{cancelled_.store(true);const int fd=fd_.load();if(fd>=0)shutdownSocket(fd);}
    void close() noexcept override{std::lock_guard<std::mutex> lock(closeMutex_);const int fd=fd_.exchange(-1);if(fd>=0){shutdownSocket(fd);closeSocket(fd);}}
private:
    WebSocketTransport::ReceiveResult::Status readExact(uint8_t* dst,size_t size,std::chrono::milliseconds timeout,std::string& error){size_t got=0;const auto deadline=Clock::now()+timeout;
        while(got<size){if(cancelled_.load())return WebSocketTransport::ReceiveResult::Status::CLOSED;const int fd=fd_.load();if(fd<0)return WebSocketTransport::ReceiveResult::Status::CLOSED;const auto now=Clock::now();if(now>=deadline)return WebSocketTransport::ReceiveResult::Status::TIMEOUT;
            const auto left=std::chrono::duration_cast<std::chrono::microseconds>(deadline-now);fd_set set;FD_ZERO(&set);FD_SET(fd,&set);timeval tv{};tv.tv_sec=long(left.count()/1000000);tv.tv_usec=long(left.count()%1000000);
            const int ready=::select(fd+1,&set,nullptr,nullptr,&tv);if(ready==0)return WebSocketTransport::ReceiveResult::Status::TIMEOUT;if(ready<0){if(cancelled_.load())return WebSocketTransport::ReceiveResult::Status::CLOSED;error="receive select failed";return WebSocketTransport::ReceiveResult::Status::ERROR;}
            const ssize_t count=::recv(fd,reinterpret_cast<char*>(dst+got),int(size-got),0);if(count==0)return WebSocketTransport::ReceiveResult::Status::CLOSED;if(count<0){if(cancelled_.load())return WebSocketTransport::ReceiveResult::Status::CLOSED;error="receive failed";return WebSocketTransport::ReceiveResult::Status::ERROR;}got+=size_t(count);}
        return WebSocketTransport::ReceiveResult::Status::MESSAGE;}
    bool sendAll(const uint8_t* data,size_t size,std::string& error){std::lock_guard<std::mutex> lock(sendMutex_);size_t sent=0;while(sent<size){const int fd=fd_.load();if(fd<0||cancelled_.load()){error="socket closed";return false;}const ssize_t n=::send(fd,reinterpret_cast<const char*>(data+sent),int(size-sent),MSG_NOSIGNAL);if(n<=0){error="send failed";return false;}sent+=size_t(n);}return true;}
    bool sendFrame(uint8_t opcode,const std::string& payload,std::string& error){std::vector<uint8_t> frame;frame.push_back(uint8_t(0x80U|opcode));const uint64_t n=payload.size();if(n<126U)frame.push_back(uint8_t(0x80U|n));else if(n<=0xffffU){frame.push_back(0xfeU);frame.push_back(uint8_t(n>>8U));frame.push_back(uint8_t(n));}else{frame.push_back(0xffU);for(int s=56;s>=0;s-=8)frame.push_back(uint8_t(n>>s));}
        const uint32_t seq=masks_.fetch_add(0x9e3779b9U);const uint8_t mask[]={uint8_t(seq>>24U),uint8_t(seq>>16U),uint8_t(seq>>8U),uint8_t(seq)};frame.insert(frame.end(),std::begin(mask),std::end(mask));for(size_t i=0;i<payload.size();++i)frame.push_back(uint8_t(payload[i])^mask[i%4U]);return sendAll(frame.data(),frame.size(),error);}
    size_t maxFrame_;std::atomic<int> fd_{-1};std::atomic<bool> cancelled_{false};std::atomic<uint32_t> masks_{0x13579bdfU};std::mutex sendMutex_;std::mutex closeMutex_;
};

class LoopbackWsAdapter final : public WebSocketTransport::Adapter {
public:
    bool connect(const std::string&,const std::string& protocol,std::chrono::milliseconds,std::string& ack,std::string&) override{std::lock_guard<std::mutex> lock(mutex_);closed_=false;ack="loopback-adapter/1; subprotocol="+protocol;return true;}
    bool sendText(const std::string& payload,bool response,std::string& error) override{MCPJsonValue in;try{in=MCPJsonValue::parse(payload);}catch(...){error="invalid JSON";return false;}MCPJsonValue out=in;if(response)out={{"jsonrpc","2.0"},{"id",in["id"]},{"result",{{"method",in["method"]},{"params",in.value("params",MCPJsonValue::object())}}}};
        {std::lock_guard<std::mutex> lock(mutex_);if(closed_){error="loopback closed";return false;}queue_.push_back(out.dump());}cv_.notify_one();return true;}
    WebSocketTransport::ReceiveResult receive(std::chrono::milliseconds timeout) override{std::unique_lock<std::mutex> lock(mutex_);if(!cv_.wait_for(lock,timeout,[this]{return closed_||!queue_.empty();}))return {};if(closed_)return {WebSocketTransport::ReceiveResult::Status::CLOSED,{},{}};std::string p=std::move(queue_.front());queue_.pop_front();return {WebSocketTransport::ReceiveResult::Status::MESSAGE,std::move(p),{}};}
    void cancel() noexcept override{{std::lock_guard<std::mutex> lock(mutex_);closed_=true;}cv_.notify_all();}
    void close() noexcept override{cancel();}
private:std::mutex mutex_;std::condition_variable cv_;std::deque<std::string> queue_;bool closed_=true;
};

class LoopbackSseAdapter final : public SSETransport::Adapter {
public:
    bool connect(const std::string&,std::chrono::milliseconds,std::string& ack,std::string&) override{open_.store(true);ack="loopback-sse-adapter/1";return true;}
    bool sendCommand(const std::string&,std::string& ack,std::string& error) override{if(!open_.load()){error="closed";return false;}ack="loopback-command-accepted";return true;}
    void cancel() noexcept override{open_.store(false);}void close() noexcept override{open_.store(false);}
private:std::atomic<bool> open_{false};
};
} // namespace

struct WebSocketTransport::SharedState {
    explicit SharedState(Config c):config(std::move(c)){stats.lastActivity=Clock::now();if(config.adapter)adapter=config.adapter;else if(config.url.rfind("loopback://",0)==0)adapter=std::make_shared<LoopbackWsAdapter>();else adapter=std::make_shared<NativeWsAdapter>(config.maxFrameBytes);}
    struct Pending{std::mutex mutex;std::condition_variable cv;bool done=false;MCPJsonValue response;};
    Config config;std::shared_ptr<Adapter> adapter;std::atomic<State> status{State::DISCONNECTED};std::atomic<bool> running{false};std::atomic<uint64_t> nextId{1};
    std::mutex lifecycleMutex;std::mutex callbackMutex;MessageHandler messageHandler;ConnectHandler connectHandler;DisconnectHandler disconnectHandler;ErrorHandler errorHandler;
    mutable std::mutex statsMutex;Stats stats;mutable std::mutex pendingMutex;std::unordered_map<uint64_t,std::shared_ptr<Pending>> pending;std::unordered_set<uint64_t> retired;std::deque<uint64_t> retiredOrder;
};

WebSocketTransport::WebSocketTransport(const Config& config):state_(std::make_shared<SharedState>(config)){}
WebSocketTransport::~WebSocketTransport(){disconnect();}
namespace {
void rememberRetired(WebSocketTransport::SharedState& s,uint64_t id){if(s.retired.insert(id).second)s.retiredOrder.push_back(id);const size_t limit=std::max<size_t>(128U,s.config.maxPendingRequests*4U);while(s.retiredOrder.size()>limit){s.retired.erase(s.retiredOrder.front());s.retiredOrder.pop_front();}}
WebSocketTransport::ErrorHandler copyError(const std::shared_ptr<WebSocketTransport::SharedState>& s){std::lock_guard<std::mutex> lock(s->callbackMutex);return s->errorHandler;}
void failPending(const std::shared_ptr<WebSocketTransport::SharedState>& s,const std::string& reason){std::vector<std::pair<uint64_t,std::shared_ptr<WebSocketTransport::SharedState::Pending>>> items;{std::lock_guard<std::mutex> lock(s->pendingMutex);for(auto& item:s->pending){rememberRetired(*s,item.first);items.push_back(item);}s->pending.clear();}for(auto& item:items){{std::lock_guard<std::mutex> lock(item.second->mutex);item.second->response=rpcError(item.first,-32002,reason);item.second->done=true;}item.second->cv.notify_all();}}
}

bool WebSocketTransport::connect(){std::unique_lock<std::mutex> life(state_->lifecycleMutex);if(state_->status.load()==State::CONNECTED)return true;if(receiveThread_.joinable()){state_->running.store(false);state_->adapter->cancel();life.unlock();if(receiveThread_.get_id()!=std::this_thread::get_id())receiveThread_.join();life.lock();state_->adapter->close();}state_->status.store(State::CONNECTING);std::string ack,error;const bool ok=state_->adapter->connect(state_->config.url,state_->config.subprotocol,std::chrono::milliseconds(std::max(1,state_->config.connectionTimeout)),ack,error);if(!ok||ack.empty()){state_->adapter->close();state_->status.store(State::FAILED);{std::lock_guard<std::mutex> lock(state_->statsMutex);++state_->stats.rejectedRequests;}auto cb=copyError(state_);life.unlock();invokeCallback(cb,error.empty()?std::string("transport supplied no handshake acknowledgement"):error);return false;}state_->running.store(true);state_->status.store(State::CONNECTED);receiveThread_=std::thread(&WebSocketTransport::receiveLoop,state_);ConnectHandler cb;{std::lock_guard<std::mutex> lock(state_->callbackMutex);cb=state_->connectHandler;}life.unlock();invokeCallback(cb);return true;}
void WebSocketTransport::disconnect(){std::unique_lock<std::mutex> life(state_->lifecycleMutex);const State before=state_->status.exchange(State::DISCONNECTED);state_->running.store(false);state_->adapter->cancel();life.unlock();failPending(state_,"transport disconnected");if(receiveThread_.joinable()&&receiveThread_.get_id()!=std::this_thread::get_id())receiveThread_.join();state_->adapter->close();DisconnectHandler cb;{std::lock_guard<std::mutex> lock(state_->callbackMutex);cb=state_->disconnectHandler;}if(before==State::CONNECTED||before==State::CONNECTING||before==State::RECONNECTING)invokeCallback(cb,1000,std::string("normal closure"));}
WebSocketTransport::State WebSocketTransport::getState() const{return state_->status.load();}
void WebSocketTransport::send(const MCPJsonValue& message){std::string validation;if(!validEnvelope(message,validation)){{std::lock_guard<std::mutex> lock(state_->statsMutex);++state_->stats.rejectedRequests;}invokeCallback(copyError(state_),validation);return;}if(state_->status.load()!=State::CONNECTED){invokeCallback(copyError(state_),std::string("send() while not connected"));return;}const std::string payload=message.dump();if(payload.size()>state_->config.maxFrameBytes){invokeCallback(copyError(state_),std::string("outgoing frame exceeds limit"));return;}std::string error;if(!state_->adapter->sendText(payload,false,error)){invokeCallback(copyError(state_),error.empty()?std::string("transport rejected send"):error);return;}{std::lock_guard<std::mutex> lock(state_->statsMutex);++state_->stats.messagesSent;state_->stats.bytesTransferred+=payload.size();state_->stats.lastActivity=Clock::now();}}
MCPJsonValue WebSocketTransport::request(const std::string& method,const MCPJsonValue& params,std::chrono::milliseconds timeout){return requestWithId(state_->nextId.fetch_add(1),method,params,timeout);}
MCPJsonValue WebSocketTransport::requestWithId(uint64_t id,const std::string& method,const MCPJsonValue& params,std::chrono::milliseconds timeout){if(method.empty())return rpcError(id,-32600,"method must not be empty");if(!validParams(params))return rpcError(id,-32602,"params must be object or array");if(state_->status.load()!=State::CONNECTED)return rpcError(id,-32000,"transport is not connected");auto pending=std::make_shared<SharedState::Pending>();{std::lock_guard<std::mutex> lock(state_->pendingMutex);if(state_->pending.size()>=state_->config.maxPendingRequests){std::lock_guard<std::mutex> stats(state_->statsMutex);++state_->stats.rejectedRequests;return rpcError(id,-32005,"pending request limit reached");}if(state_->pending.count(id)||state_->retired.count(id)){std::lock_guard<std::mutex> stats(state_->statsMutex);++state_->stats.rejectedRequests;return rpcError(id,-32600,"duplicate or retired request id");}state_->pending.emplace(id,pending);}const MCPJsonValue envelope={{"jsonrpc","2.0"},{"id",id},{"method",method},{"params",params}};const std::string payload=envelope.dump();std::string error;if(payload.size()>state_->config.maxFrameBytes||!state_->adapter->sendText(payload,true,error)){std::lock_guard<std::mutex> lock(state_->pendingMutex);state_->pending.erase(id);rememberRetired(*state_,id);return rpcError(id,-32001,error.empty()?"transport rejected request":error);}{std::lock_guard<std::mutex> lock(state_->statsMutex);++state_->stats.messagesSent;state_->stats.bytesTransferred+=payload.size();state_->stats.lastActivity=Clock::now();}if(timeout<=std::chrono::milliseconds::zero())timeout=std::chrono::milliseconds(std::max(1,state_->config.connectionTimeout));std::unique_lock<std::mutex> wait(pending->mutex);if(!pending->cv.wait_for(wait,timeout,[&pending]{return pending->done;})){wait.unlock();{std::lock_guard<std::mutex> lock(state_->pendingMutex);auto found=state_->pending.find(id);if(found!=state_->pending.end()&&found->second==pending)state_->pending.erase(found);rememberRetired(*state_,id);}{std::lock_guard<std::mutex> stats(state_->statsMutex);++state_->stats.timedOutRequests;}return rpcError(id,-32003,"request timed out");}return pending->response;}
bool WebSocketTransport::cancelRequest(uint64_t id){std::shared_ptr<SharedState::Pending> pending;{std::lock_guard<std::mutex> lock(state_->pendingMutex);auto found=state_->pending.find(id);if(found==state_->pending.end())return false;pending=found->second;state_->pending.erase(found);rememberRetired(*state_,id);}{std::lock_guard<std::mutex> lock(pending->mutex);pending->response=rpcError(id,-32800,"request cancelled");pending->done=true;}pending->cv.notify_all();std::lock_guard<std::mutex> stats(state_->statsMutex);++state_->stats.cancelledRequests;return true;}
size_t WebSocketTransport::getPendingRequestCount() const{std::lock_guard<std::mutex> lock(state_->pendingMutex);return state_->pending.size();}
void WebSocketTransport::setMessageHandler(MessageHandler h){std::lock_guard<std::mutex> lock(state_->callbackMutex);state_->messageHandler=std::move(h);}void WebSocketTransport::onConnect(ConnectHandler h){std::lock_guard<std::mutex> lock(state_->callbackMutex);state_->connectHandler=std::move(h);}void WebSocketTransport::onDisconnect(DisconnectHandler h){std::lock_guard<std::mutex> lock(state_->callbackMutex);state_->disconnectHandler=std::move(h);}void WebSocketTransport::onError(ErrorHandler h){std::lock_guard<std::mutex> lock(state_->callbackMutex);state_->errorHandler=std::move(h);}WebSocketTransport::Stats WebSocketTransport::getStats() const{std::lock_guard<std::mutex> lock(state_->statsMutex);return state_->stats;}
void WebSocketTransport::receiveLoop(const std::shared_ptr<SharedState>& s){while(s->running.load()){auto received=s->adapter->receive(std::chrono::milliseconds(100));if(!s->running.load())break;if(received.status==WebSocketTransport::ReceiveResult::Status::TIMEOUT)continue;if(received.status==WebSocketTransport::ReceiveResult::Status::MESSAGE){if(received.payload.size()>s->config.maxFrameBytes){{std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.malformedMessages;}invokeCallback(copyError(s),std::string("incoming frame exceeds limit"));continue;}MCPJsonValue message;try{message=MCPJsonValue::parse(received.payload);}catch(...){{std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.malformedMessages;}invokeCallback(copyError(s),std::string("incoming frame is invalid JSON"));continue;}std::string validation;if(!validEnvelope(message,validation)){{std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.malformedMessages;}invokeCallback(copyError(s),validation);continue;}{std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.messagesReceived;s->stats.bytesTransferred+=received.payload.size();s->stats.lastActivity=Clock::now();}if(message.contains("method")){MessageHandler cb;{std::lock_guard<std::mutex> lock(s->callbackMutex);cb=s->messageHandler;}invokeCallback(cb,message);continue;}if(!message["id"].is_number_integer()&&!message["id"].is_number_unsigned()){{std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.malformedMessages;}invokeCallback(copyError(s),std::string("correlated response id must be integer"));continue;}uint64_t id=0;try{const int64_t signedId=message["id"].get<int64_t>();if(signedId<0)throw std::out_of_range("id");id=uint64_t(signedId);}catch(...){try{id=message["id"].get<uint64_t>();}catch(...){std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.malformedMessages;continue;}}std::shared_ptr<SharedState::Pending> pending;{std::lock_guard<std::mutex> lock(s->pendingMutex);auto found=s->pending.find(id);if(found!=s->pending.end()){pending=found->second;s->pending.erase(found);rememberRetired(*s,id);}}if(!pending){std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.duplicateOrLateResponses;continue;}{std::lock_guard<std::mutex> lock(pending->mutex);pending->response=std::move(message);pending->done=true;}pending->cv.notify_all();continue;}const std::string reason=received.error.empty()?"transport closed":received.error;failPending(s,reason);DisconnectHandler disconnected;{std::lock_guard<std::mutex> lock(s->callbackMutex);disconnected=s->disconnectHandler;}invokeCallback(disconnected,1006,reason);s->adapter->close();if(!s->config.autoReconnect){s->status.store(State::FAILED);invokeCallback(copyError(s),reason);break;}bool reconnected=false;int delay=std::max(1,s->config.reconnectDelay);for(int attempt=0;s->running.load()&&attempt<s->config.maxReconnectAttempts;++attempt){s->status.store(State::RECONNECTING);{std::lock_guard<std::mutex> stats(s->statsMutex);++s->stats.reconnectAttempts;}const auto until=Clock::now()+std::chrono::milliseconds(delay);while(s->running.load()&&Clock::now()<until)std::this_thread::sleep_for(std::chrono::milliseconds(5));if(!s->running.load())break;std::string ack,error;if(s->adapter->connect(s->config.url,s->config.subprotocol,std::chrono::milliseconds(std::max(1,s->config.connectionTimeout)),ack,error)&&!ack.empty()){s->status.store(State::CONNECTED);ConnectHandler connected;{std::lock_guard<std::mutex> lock(s->callbackMutex);connected=s->connectHandler;}invokeCallback(connected);reconnected=true;break;}s->adapter->close();delay=std::min(delay*2,60000);}if(!reconnected){if(s->running.load()){s->status.store(State::FAILED);invokeCallback(copyError(s),std::string("reconnect attempts exhausted"));}break;}}}

struct SSETransport::SharedState{explicit SharedState(Config c):config(std::move(c)){if(config.adapter)adapter=config.adapter;else if(config.url.rfind("loopback://",0)==0)adapter=std::make_shared<LoopbackSseAdapter>();}Config config;std::shared_ptr<Adapter> adapter;std::atomic<bool> connected{false};mutable std::mutex mutex;EventHandler handler;};
SSETransport::SSETransport(const Config& config):state_(std::make_shared<SharedState>(config)){}SSETransport::~SSETransport(){disconnect();}
bool SSETransport::connect(){if(state_->connected.load())return true;if(!state_->adapter)return false;std::string ack,error;if(!state_->adapter->connect(state_->config.url,std::chrono::milliseconds(std::max(1,state_->config.connectionTimeout)),ack,error)||ack.empty()){state_->adapter->close();return false;}state_->connected.store(true);EventHandler cb;{std::lock_guard<std::mutex> lock(state_->mutex);cb=state_->handler;}invokeCallback(cb,std::string("open"),MCPJsonValue{{"url",state_->config.url},{"acknowledgement",ack}});return true;}
void SSETransport::disconnect(){if(!state_->connected.exchange(false))return;if(state_->adapter){state_->adapter->cancel();state_->adapter->close();}EventHandler cb;{std::lock_guard<std::mutex> lock(state_->mutex);cb=state_->handler;}invokeCallback(cb,std::string("close"),MCPJsonValue{{"reason","closed"}});}
bool SSETransport::isConnected() const{return state_->connected.load();}void SSETransport::onEvent(EventHandler h){std::lock_guard<std::mutex> lock(state_->mutex);state_->handler=std::move(h);}bool SSETransport::sendCommand(const MCPJsonValue& command){if(!state_->connected.load()||!state_->adapter||!command.is_object())return false;std::string ack,error;if(!state_->adapter->sendCommand(command.dump(),ack,error)||ack.empty())return false;EventHandler cb;{std::lock_guard<std::mutex> lock(state_->mutex);cb=state_->handler;}invokeCallback(cb,std::string("command-ack"),MCPJsonValue{{"ack",true},{"acknowledgement",ack},{"command",command}});return true;}

struct TransportMultiplexer::SharedState{std::unordered_map<std::string,std::shared_ptr<WebSocketTransport>> ws;std::unordered_map<std::string,std::shared_ptr<SSETransport>> sse;std::unordered_set<std::string> stdio;std::unordered_map<std::string,uint64_t> counts;std::vector<std::string> failover;std::string primary;MessageHandler handler;mutable std::mutex mutex;std::atomic<bool> active{true};};
TransportMultiplexer::TransportMultiplexer():state_(std::make_shared<SharedState>()){}TransportMultiplexer::~TransportMultiplexer(){auto s=state_;s->active.store(false);std::vector<std::shared_ptr<WebSocketTransport>> ws;std::vector<std::shared_ptr<SSETransport>> sse;{std::lock_guard<std::mutex> lock(s->mutex);for(auto& item:s->ws)ws.push_back(item.second);for(auto& item:s->sse)sse.push_back(item.second);s->ws.clear();s->sse.clear();s->stdio.clear();s->handler=nullptr;}for(auto& item:ws)item->disconnect();for(auto& item:sse)item->disconnect();}
void TransportMultiplexer::addWebSocket(const std::string& id,const WebSocketTransport::Config& config){auto transport=std::make_shared<WebSocketTransport>(config);std::weak_ptr<SharedState> weak=state_;transport->setMessageHandler([weak,id](const MCPJsonValue& message){auto s=weak.lock();if(!s||!s->active.load())return;MessageHandler cb;{std::lock_guard<std::mutex> lock(s->mutex);cb=s->handler;++s->counts[id];}invokeCallback(cb,id,message);});const bool connected=transport->connect();std::lock_guard<std::mutex> lock(state_->mutex);state_->ws[id]=std::move(transport);if(connected&&state_->primary.empty())state_->primary=id;}
void TransportMultiplexer::addSSE(const std::string& id,const SSETransport::Config& config){auto transport=std::make_shared<SSETransport>(config);std::weak_ptr<SharedState> weak=state_;transport->onEvent([weak,id](const std::string& event,const MCPJsonValue& data){auto s=weak.lock();if(!s||!s->active.load())return;MessageHandler cb;{std::lock_guard<std::mutex> lock(s->mutex);cb=s->handler;++s->counts[id];}invokeCallback(cb,id,MCPJsonValue{{"event",event},{"data",data}});});const bool connected=transport->connect();std::lock_guard<std::mutex> lock(state_->mutex);state_->sse[id]=std::move(transport);if(connected&&state_->primary.empty())state_->primary=id;}
void TransportMultiplexer::addStdio(const std::string& id){std::lock_guard<std::mutex> lock(state_->mutex);state_->stdio.insert(id);if(state_->primary.empty())state_->primary=id;}
void TransportMultiplexer::removeTransport(const std::string& id){std::shared_ptr<WebSocketTransport> ws;std::shared_ptr<SSETransport> sse;{std::lock_guard<std::mutex> lock(state_->mutex);auto wi=state_->ws.find(id);if(wi!=state_->ws.end()){ws=std::move(wi->second);state_->ws.erase(wi);}auto si=state_->sse.find(id);if(si!=state_->sse.end()){sse=std::move(si->second);state_->sse.erase(si);}state_->stdio.erase(id);state_->counts.erase(id);state_->failover.erase(std::remove(state_->failover.begin(),state_->failover.end(),id),state_->failover.end());if(state_->primary==id)state_->primary=state_->failover.empty()?std::string():state_->failover.front();}if(ws)ws->disconnect();if(sse)sse->disconnect();}
std::unordered_map<std::string,std::string> TransportMultiplexer::getTransportStates() const{std::lock_guard<std::mutex> lock(state_->mutex);std::unordered_map<std::string,std::string> out;for(const auto& item:state_->ws){switch(item.second->getState()){case WebSocketTransport::State::CONNECTED:out[item.first]="connected";break;case WebSocketTransport::State::CONNECTING:out[item.first]="connecting";break;case WebSocketTransport::State::RECONNECTING:out[item.first]="reconnecting";break;case WebSocketTransport::State::FAILED:out[item.first]="failed";break;default:out[item.first]="disconnected";}}for(const auto& item:state_->sse)out[item.first]=item.second->isConnected()?"connected":"disconnected";for(const auto& id:state_->stdio)out[id]="connected";return out;}
void TransportMultiplexer::send(const std::string& id,const MCPJsonValue& message){std::shared_ptr<WebSocketTransport> ws;std::shared_ptr<SSETransport> sse;bool stdio=false;MessageHandler cb;{std::lock_guard<std::mutex> lock(state_->mutex);auto wi=state_->ws.find(id);if(wi!=state_->ws.end())ws=wi->second;auto si=state_->sse.find(id);if(si!=state_->sse.end())sse=si->second;stdio=state_->stdio.count(id)!=0U;cb=state_->handler;}bool accepted=false;if(ws){const auto before=ws->getStats().messagesSent;ws->send(message);accepted=ws->getStats().messagesSent>before;}else if(sse)accepted=sse->sendCommand(message);else if(stdio){accepted=true;invokeCallback(cb,id,message);}if(accepted){std::lock_guard<std::mutex> lock(state_->mutex);++state_->counts[id];}}
void TransportMultiplexer::broadcast(const MCPJsonValue& message){std::vector<std::string> ids;{std::lock_guard<std::mutex> lock(state_->mutex);for(const auto& item:state_->ws)ids.push_back(item.first);for(const auto& item:state_->sse)ids.push_back(item.first);for(const auto& id:state_->stdio)ids.push_back(id);}std::sort(ids.begin(),ids.end());for(const auto& id:ids)send(id,message);}
void TransportMultiplexer::setMessageHandler(MessageHandler h){std::lock_guard<std::mutex> lock(state_->mutex);state_->handler=std::move(h);}void TransportMultiplexer::setPrimaryTransport(const std::string& id){std::lock_guard<std::mutex> lock(state_->mutex);state_->primary=id;}void TransportMultiplexer::setFailoverOrder(const std::vector<std::string>& order){std::lock_guard<std::mutex> lock(state_->mutex);state_->failover=order;}std::string TransportMultiplexer::getPrimaryTransport() const{std::lock_guard<std::mutex> lock(state_->mutex);return state_->primary;}std::vector<std::string> TransportMultiplexer::getFailoverOrder() const{std::lock_guard<std::mutex> lock(state_->mutex);return state_->failover;}std::unordered_map<std::string,uint64_t> TransportMultiplexer::getMessageCounts() const{std::lock_guard<std::mutex> lock(state_->mutex);return state_->counts;}

EnhancedMCPGateway::EnhancedMCPGateway(const std::string& id):MCPGateway(id),multiplexer_(std::make_unique<TransportMultiplexer>()){}TransportMultiplexer& EnhancedMCPGateway::getTransportMultiplexer(){return *multiplexer_;}void EnhancedMCPGateway::enableWebSocketTransport(const WebSocketTransport::Config& config){multiplexer_->addWebSocket("websocket",config);}void EnhancedMCPGateway::enableSSETransport(const SSETransport::Config& config){multiplexer_->addSSE("sse",config);}void EnhancedMCPGateway::enableAutoFailover(bool enable){std::lock_guard<std::mutex> lock(enhancedMutex_);autoFailoverEnabled_=enable;}void EnhancedMCPGateway::setFailoverTimeout(std::chrono::milliseconds timeout){std::lock_guard<std::mutex> lock(enhancedMutex_);failoverTimeout_=timeout;}void EnhancedMCPGateway::setConnectionPoolSize(size_t size){std::lock_guard<std::mutex> lock(enhancedMutex_);connectionPoolSize_=size;}
size_t EnhancedMCPGateway::getActiveConnections() const{const auto states=multiplexer_->getTransportStates();size_t active=0;for(const auto& item:states)if(item.second=="connected")++active;{std::lock_guard<std::mutex> lock(enhancedMutex_);if(active&&active<=connectionPoolSize_)++connectionPoolHits_;}return active;}
EnhancedMCPGateway::PaymentVerification EnhancedMCPGateway::verifyBlockchainPayment(const std::string&) { PaymentVerification out{};out.verified=false;out.timestamp=std::chrono::system_clock::now();return out; }
EnhancedMCPGateway::EnhancedStats EnhancedMCPGateway::getEnhancedStatistics() const{EnhancedStats out{};out.base=getStatistics();out.transportMessageCounts=multiplexer_->getMessageCounts();std::lock_guard<std::mutex> lock(enhancedMutex_);out.averageLatency=0.0;out.failoverEvents=failoverEvents_;out.connectionPoolHits=connectionPoolHits_;return out;}
} // namespace elizaos
