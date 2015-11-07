#include <asio.hpp>

#include "PetriNet.h"
#include "Cache.h"
#include <memory>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>

class MetabolismConnection
: public std::enable_shared_from_this<MetabolismConnection> {
public:

    static MetabolismConnection* create(asio::io_service& io_service,
            PetriNet* pn, Cache* cache, vector<string> text) {
        return new MetabolismConnection(io_service, pn, cache, text);
    }

    asio::ip::tcp::socket& socket() {
        return socket_;
    }

    void start() {
        socket_.async_receive(asio::buffer(buffer_, length),
                std::bind(&MetabolismConnection::handle_read,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

private:

    MetabolismConnection(asio::io_service& io_service,
            PetriNet* pn, Cache* cache, vector<string> text) :
    text_(text), pn_(pn), cache_(cache), socket_(io_service), strand_(io_service) {
    }
    //, metabolism_(metabolism) { }

    void handle_read(const asio::error_code& error, std::size_t bytes_transfered) {
        if (!error && bytes_transfered > 0) {
            std::string input_received(buffer_);


            //std::cout << "read: " << input_received;        
            // *** Tutaj wstawic procedure liczaca ***

            double result;

            if (cache_->isPresent(input_received)) {
                result = cache_->getValue(input_received);
		//std::cout << "hit\n";
            } else {
                result = pn_->runServer(text_, input_received);
                cache_->setValue(input_received, result);
            }

            asio::async_write(socket_,
                    asio::buffer(reinterpret_cast<const char*> (&result), sizeof (double)),
                    std::bind(&MetabolismConnection::handle_write,
                    shared_from_this(), std::placeholders::_1)
                    );
        } else {
            //metabolism_->dump();
        }
    }

    void handle_write(const asio::error_code& error) {
        if (!error) {
            socket_.async_receive(asio::buffer(buffer_, length),
                    std::bind(&MetabolismConnection::handle_read,
                    shared_from_this(),
                    std::placeholders::_1,
                    std::placeholders::_2));
        } else {
            //metabolism_->dump();
        }
    }

    asio::ip::tcp::socket socket_;
    static const std::size_t length = 10000;
    asio::io_service::strand strand_;
    char buffer_[length];
    PetriNet* pn_;
    Cache* cache_;
    vector<string> text_;
};




typedef std::shared_ptr<MetabolismConnection> connection_ptr;

class MetabolismServer {
public:

    MetabolismServer(const std::string& address, const std::string& port,
            asio::io_service& io_service, std::size_t thread_num, PetriNet* pn, Cache* cache, vector<string> text) :
    text_(text),
    pn_(pn),
    cache_(cache),
    thread_pool_size_(thread_num),
    io_service_(io_service),
    acceptor_(io_service_),
    new_connection_(MetabolismConnection::create(io_service_, pn_, cache_, text_)) {
        asio::ip::tcp::resolver resolver(io_service_);
        asio::ip::tcp::resolver::query query(address, port);
        asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        acceptor_.async_accept(
                new_connection_->socket(), std::bind(
                &MetabolismServer::handleAccept, this,
                std::placeholders::_1));
    }

    void run() {
        std::vector<std::shared_ptr<asio::thread> > threads;
        for (std::size_t i = 0; i < thread_pool_size_; ++i) {
            std::shared_ptr<asio::thread> thread(new asio::thread(
                    [ = ] (){io_service_.run();}));
            threads.push_back(thread);
        }

        for (std::size_t i = 0; i < threads.size(); ++i) {
            threads[i]->join();
        }
    }

    void stop() {
        io_service_.stop();
    }

private:

    void handleAccept(const asio::error_code& error) {
        if (!error) {
            new_connection_->start();
            new_connection_.reset(MetabolismConnection::create(io_service_, pn_, cache_, text_));
            acceptor_.async_accept(
                    new_connection_->socket(),
                    std::bind(&MetabolismServer::handleAccept, this,
                    std::placeholders::_1));
        }
    }
    vector<string> text_;
    PetriNet* pn_;
    Cache* cache_;
    std::size_t thread_pool_size_;
    asio::io_service& io_service_;
    asio::ip::tcp::acceptor acceptor_;
    connection_ptr new_connection_;
};

bool isJason(string path) {
    int l = (int) path.size();
    if ((char) path[l - 1] == 'n')
        if ((char) path[l - 2] == 'o')
            if ((char) path[l - 3] == 's')
                if ((char) path[l - 4] == 'j')
                    if ((char) path[l - 5] == '.')
                        return true;
    return false;
}

int main(int argc, char** argv) {
    if (argc != 3 && argc != 5 && argc != 6) {
        cout << VERSION << endl;
        cout << "USAGE FOR STANDALONE: qsspn qsspn_file ctrl_file" << endl;
        cout << "USAGE FOR SERVER: qsspn qsspn_file ctrl_file address port cache_size" << endl;
        cout << "USAGE FOR CLIENT: qsspn qsspn_file ctrl_file address port" << endl;
        cout << "The qsspn_file file with extension .json is read as Json file" << endl;
        cout << "The qsspn file without .json extension is assumed to be in old qsspn format" << endl;
        cout << "Example cache size is 100000000" << endl;
        exit(1);
    }

    if (argc == 3) {//standalone
        PetriNet *pn = new PetriNet();
        string path(argv[1]);
        string path1(argv[2]);
        if (isJason(path)) {
            pn->readModel(path);
        } else {
            pn->readPetriNet(path);
        }
        pn->runWorkflowStandalone(path1);
        exit(0);
    } else if (argc == 5) {//client
        PetriNet *pn = new PetriNet();
        string path(argv[1]);
        string path1(argv[2]);
        string serverAddress(argv[3]);
        string serverPort(argv[4]);
        if (isJason(path)) {
            pn->readModel(path);
        } else {
            pn->readPetriNet(path);
        }
       pn->runWorkflowClient(path1, serverAddress, serverPort);
        exit(0);
    } else try {//server
        string path(argv[1]);
        string path1(argv[2]);
        string address(argv[3]);
        string port(argv[4]);
        int cache_size = atoi(argv[5]);

        Cache *cache = new Cache(cache_size);
        PetriNet *pn = new PetriNet();
        if (isJason(path)) {
            pn->readModel(path);
        } else {
            pn->readPetriNet(path);
        }

        fstream infile;
        string word;
        vector<string> text;

        infile.open(path1.c_str());
        if (!infile) {
            cerr << "Could not open file:" << path1 << endl;
            exit(0);
        }

        while (infile >> word) text.push_back(word);
        infile.close();

        asio::io_service io_service;

        MetabolismServer server(address, port, io_service, 1, pn, cache, text);

        server.run();

        delete pn;
        delete cache;
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
