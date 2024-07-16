#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
mutex log_mutex;
class Server{
    public:
        Server(int port):
        serverPort(port){};
        void run(){
            int server_fd, new_socket;
            struct sockaddr_in address;
            int opt = 1;
            int size = sizeof(address);
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                cerr << "Socket failed\n";
                exit(EXIT_FAILURE);
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(serverPort);
            if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
                std::cerr << "Bind failed\n";
                exit(EXIT_FAILURE);
            }
            if (listen(server_fd, 3) < 0) {
            std::cerr << "Listen\n";
            exit(EXIT_FAILURE);
            }

            while (true) {
                if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&size)) < 0) {
                    std::cerr << "Accept\n";
                    exit(EXIT_FAILURE);
                }
                std::thread(&Server::handleClient, this, new_socket).detach();
            }
        }
    private:
        int serverPort = 0;
        void handleClient(int socket){
            char buffer[1024] = {0};
            int valread = read(socket, buffer, 1024);
            if(valread>0){
                
                cout<<string(buffer, valread) << endl;
                lock_guard<mutex> guard(log_mutex);
                ofstream log_file("log.txt", ios_base::app);
                log_file << string(buffer, valread) << endl;
            }
            close(socket);
        }
};

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        cerr<<"Usage <port>\n";
        exit(1);
    }
    int sPort = stoi(argv[1]);
    Server server(sPort);
    server.run();
    return 0;
}