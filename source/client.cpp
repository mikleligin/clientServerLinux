#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
class Client{
    public:
        Client(const string& name, int port, int period):
        clientName(name), serverPort(port), timePeriod(period){};
        void run(){
            while(true){
                connectClient();
                std::this_thread::sleep_for(std::chrono::seconds(timePeriod));
            }
        }
    private:
        string clientName;
        int serverPort = 0;
        int timePeriod = 0;
        void connectClient()
        {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in servAddr;
            servAddr.sin_family = AF_INET;
            servAddr.sin_port = htons(serverPort);
            if(inet_pton(AF_INET, "127.0.0.1",&servAddr.sin_addr)<=0){
                cerr<<"Invalid address\n";
                close(sock);
                return;
            }
            if(connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0){
                cerr<<"No Connection :(\n";
                close(sock);
                return;
            }
            auto now = chrono::system_clock::now();
            auto ms = chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            time_t t = chrono::system_clock::to_time_t(now);
            tm tm = *std::localtime(&t);

            char timestamp[100];
            snprintf(timestamp, sizeof(timestamp), "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",
                    tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec, (int)ms.count());

            string message = string(timestamp) + " " + clientName;
            send(sock, message.c_str(), message.size(),0);
            close(sock);
        }
};

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <client_name> <server_port> <period>\n";
        return 1;
    }
    string name = argv[1];
    int port = stoi(argv[2]);
    int period = stoi(argv[3]);
    Client client(name, port, period);
    client.run();
    return 0;
}