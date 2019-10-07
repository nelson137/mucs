#include "sock.hpp"


void host_registration() {
    SocketServer sock_nb(SOCKET_NAME);
    sock_nb.bind();
    sock_nb.listen();

    no_echo_tty();

    cout << endl;
    cout << "Lab Registration:" << endl;
    cout << "Use ^D to stop." << endl;
    cout << endl;
    cout << "[Listening]" << endl;

    int data_fd, ret;
    string pawprint, input;
    char buffer[BUFFER_SIZE];

    // Handle connections
    struct timespec sleeptime = { 0, 100000000 };  // 0.1 seconds
    while (true) {
        if (input_available()) {
            cin >> input;
            if (cin.eof())
                break;
        }

        data_fd = sock_nb.accept();
        if (data_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                nanosleep(&sleeptime, nullptr);
            continue;
        }

        // Hanlde connection data
        pawprint = "";
        while (true) {
            ret = read(data_fd, buffer, BUFFER_SIZE - 1);
            if (ret  < 0) die("Error reading data");
            if (ret == 0) break;
            buffer[ret] = 0;
            pawprint += string(buffer);
        }

        cout << "Registering: " << pawprint << endl;

        close(data_fd);
    }

    cout << "[End Listening]" << endl << endl;
    restore_tty();
}


void send_username(string user) {
    SocketClient data_socket(SOCKET_NAME);
    data_socket.connect();
    data_socket.write(user);
}
