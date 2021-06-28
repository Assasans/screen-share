#include <vector>
#include <cstdint>
#include <cstring>

#include "network/Client.hpp"


int main(int argc, char* argv[]) {
	ScreenShare::Network::Client client = ScreenShare::Network::Client();
	client.Run();

	return 0;
}
