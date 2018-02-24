
#include "mbed.h"
#include "EthernetInterface.h"

// Network interface
EthernetInterface net;

Serial pc(PA_9,PA_10);
DigitalOut led_red(PD_12);
DigitalOut led_blue(PD_13);
// Socket demo
int rc;
int main() {
    pc.baud(115200);
    pc.printf("Hello World\r\n");
    ///pc.puts("Hello");
    // Bring up the ethernet interface
    pc.printf("Ethernet socket example\n");
    net.connect();
    
    // Show the network address
    const char *ip = net.get_ip_address();
    const char *netmask = net.get_netmask();
    const char *gateway = net.get_gateway();
    pc.printf("IP address: %s\n", ip ? ip : "None");
    pc.printf("Netmask: %s\n", netmask ? netmask : "None");
    pc.printf("Gateway: %s\n", gateway ? gateway : "None");

    TCPSocket socket;
    rc = socket.open(&net);
    if(rc == 0) led_blue=1;
    else led_red = 1;
    socket.connect("api.ipify.org", 80);
    char *buffer = new char[256];
    // Send an HTTP request
    
    strcpy(buffer, "GET / HTTP/1.1\r\nHost: api.ipify.org\r\n\r\n");
    int scount = socket.send(buffer, strlen(buffer));
    pc.printf("sent %d [%.*s]\n", scount, strstr(buffer, "\r\n")-buffer, buffer);

    // Recieve an HTTP response and print out the response line
    int rcount = socket.recv(buffer, 256);
    pc.printf("recv %d [%.*s]\n", rcount, strstr(buffer, "\r\n")-buffer, buffer);

    // The api.ipify.org service also gives us the device's external IP address
    const char *payload = strstr(buffer, "\r\n\r\n")+4;
    pc.printf("External IP address: %.*s\n", rcount-(payload-buffer), payload);

    // Close the socket to return its memory and bring down the network interface
    socket.close();
    delete[] buffer;
    

    // Bring down the ethernet interface
    net.disconnect();
    pc.printf("Done\n");
}
