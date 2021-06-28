#include <opencv2/opencv.hpp>

#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

namespace ScreenShare::Network {
	class Client {
		public:
			Client();
			~Client();

			void Run();
			// void SendFrame(ScreenShare::Frame frame);

		private:
	};
} // namespace ScreenShare::Network

#endif
