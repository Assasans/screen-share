#include <vector>
#include <thread>
#include <chrono>
#include <cstdint>
#include <cstring>

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include <IL/il.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>

#include <libpng16/png.h>

#include "compression/JPEG.hpp"
#include "compression/Raw.hpp"

#include "network/Server.hpp"

#include "Frame.hpp"

namespace po = boost::program_options;

void ImageFromDisplay(Display* display, ScreenShare::ScreenshotInfo* info) {
	Window root = DefaultRootWindow(display);

	XWindowAttributes attributes = {0};
	XGetWindowAttributes(display, root, &attributes);

	XImage* image = XGetImage(display, root, info->x, info->y, info->width, info->height, AllPlanes, ZPixmap);

	info->bpp = image->bits_per_pixel;

	info->pixels.resize(info->width * info->height * 4);

	memcpy(&info->pixels[0], image->data, info->pixels.size());

	XDestroyImage(image);
}

void GetCursor(Display* display, ScreenShare::CursorInfo* info) {
	XFixesCursorImage* cursor = XFixesGetCursorImage(display);

	info->x = cursor->x - cursor->xhot + 1;
	info->y = cursor->y - cursor->yhot + 1;

	info->width = cursor->width;
	info->height = cursor->height;

	info->pixels.resize(cursor->width * cursor->height * 8);

	memcpy(&info->pixels[0], cursor->pixels, info->pixels.size());
}

namespace ScreenShare::Host {
	class Application {
		private:
			std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point b = std::chrono::system_clock::now();

			std::chrono::system_clock::time_point capture_start = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point capture_end = std::chrono::system_clock::now();

			std::chrono::system_clock::time_point transform_start = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point transform_end = std::chrono::system_clock::now();

			std::chrono::system_clock::time_point compress_start = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point compress_end = std::chrono::system_clock::now();

			std::chrono::system_clock::time_point decompress_start = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point decompress_end = std::chrono::system_clock::now();

			std::chrono::system_clock::time_point render_start = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point render_end = std::chrono::system_clock::now();

		public:
			Application() {}
			~Application() {}

			int Run(po::variables_map options) {
				if(options.count("help")) {
					return 0;
				}

				if(!options.count("x")) {
					fprintf(stderr, "X offset was not set\n");
					return 2;
				}

				if(!options.count("y")) {
					fprintf(stderr, "Y offset was not set\n");
					return 2;
				}

				if(!options.count("width")) {
					fprintf(stderr, "Frame width was not set\n");
					return 2;
				}

				if(!options.count("height")) {
					fprintf(stderr, "Frame height was not set\n");
					return 2;
				}

				if(!options.count("quality")) {
					fprintf(stderr, "Frame quality was not set\n");
					return 2;
				}

				int offsetX = options["x"].as<int>();
				int offsetY = options["y"].as<int>();
				int width = options["width"].as<int>();
				int height = options["height"].as<int>();
				int quality = options["quality"].as<int>();

				Display* display = XOpenDisplay(nullptr);

				bool showCursor;

				Compression::JPEG jpeg = Compression::JPEG(width, height);
				// Compression::Raw jpeg = Compression::Raw(width, height);

				// std::vector<uint8_t> _data;
				char _data[width * height * 4];

				int screen = DefaultScreen(display);
				int dplanes = DisplayPlanes(display, screen);
				Visual* visual = XDefaultVisual(display, screen);
				Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, 1, BlackPixel(display, screen), WhitePixel(display, screen));
				XGCValues values;
				GC gc = DefaultGC(display, screen);
				XSelectInput(display, window, ExposureMask);
				XMapWindow(display, window);
				XImage* ximage;

				ScreenshotInfo screenshot;
				screenshot.x = offsetX;
				screenshot.y = offsetY;
				screenshot.width = width;
				screenshot.height = height;
				screenshot.pixels = std::vector<uint64_t>();

				CursorInfo cursor;
				cursor.pixels = std::vector<uint64_t>();

				cv::Mat _toCompress;
				cv::Mat toCompress;

				std::vector<uint8_t> compressed;

				cv::Mat _decompressed;
				cv::Mat decompressed;

				std::vector<uint8_t> buffer;

				uint32_t pos, col, row;

				cv::Size imgSize;
				cv::Size screenSize;
				cv::Rect screenRect;

				cv::Mat img;
				cv::Mat screenImg;
				cv::Mat cursorImg;

				cv::Mat alphaChannel = cv::Mat(cv::Size(width, height), CV_8UC1);

				while(true) {
					a = std::chrono::system_clock::now();
					std::chrono::duration<double, std::milli> work_time = a - b;

					if(work_time.count() < 13.3) {
						std::chrono::duration<double, std::milli> delta_ms(13.3 - work_time.count());
						auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
						std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
					}

					b = std::chrono::system_clock::now();
					std::chrono::duration<double, std::milli> sleep_time = b - a;

					// Your code here

					showCursor = true;

					capture_start = std::chrono::system_clock::now();

					ImageFromDisplay(display, &screenshot);
					GetCursor(display, &cursor);

					capture_end = std::chrono::system_clock::now();

					transform_start = std::chrono::system_clock::now();

					cursor.x -= offsetX;
					cursor.y -= offsetY;

					if(cursor.x > width || cursor.y > height) {
						showCursor = false;
					}

					imgSize = cv::Size(width + (cursor.width * 2), height + (cursor.height * 2));
					screenSize = screenshot.getSize();
					screenRect = cv::Rect(cursor.width, cursor.height, screenSize.width, screenSize.height);

					img = cv::Mat(imgSize, CV_8UC4, cv::Scalar(0));
					screenImg = cv::Mat(screenSize, screenshot.bpp > 24 ? CV_8UC4 : CV_8UC3, &screenshot.pixels[0]);

					screenImg.copyTo(img(screenRect));

					if(showCursor) {
						/*
						uint8_t a, r, g, b;

						cursorImg = cv::Mat(cv::Size(cursor.width, cursor.height), CV_8UC4);

						for(pos = row = 0; row < cursor.height; row++) {
							for(col = 0; col < cursor.width; col++, pos++) {
								a = (uint8_t)((cursor.pixels[pos] >> 24) & 0xff);
								r = (uint8_t)((cursor.pixels[pos] >> 16) & 0xff);
								g = (uint8_t)((cursor.pixels[pos] >>  8) & 0xff);
								b = (uint8_t)((cursor.pixels[pos] >>  0) & 0xff);

								cv::Vec4b color = cursorImg.at<cv::Vec4b>(row, col);

								color[0] = r;
								color[1] = g;
								color[2] = b;
								color[3] = a;
							}
						}

						cv::Rect rect = cv::Rect(cursor.x + cursor.width, cursor.y + cursor.height, cursor.width, cursor.height);

						cursorImg.copyTo(img(rect), cursorImg);
						*/
						// TODO(Assasans): Rewrite
					}

					_toCompress = img(screenRect);

					transform_end = std::chrono::system_clock::now();

					compress_start = std::chrono::system_clock::now();
					if(_toCompress.channels() == 4) {
						std::vector<cv::Mat> channels;

						cv::Mat image;

						cv::split(_toCompress, channels);

						channels.pop_back();

						cv::merge(channels, toCompress);

						// cv::cvtColor(_toCompress, image, cv::COLOR_RGBA2RGB);
					} else {
						toCompress = _toCompress;
					}

					jpeg.CompressFrame(toCompress, compressed, 100);
					// jpeg.CompressFrame(toCompress, compressed);
					compress_end = std::chrono::system_clock::now();

					decompress_start = std::chrono::system_clock::now();

					jpeg.DecompressFrame(compressed, _decompressed);

					if(_decompressed.channels() == 4) {
						decompressed = _decompressed;
					} else {
						std::vector<cv::Mat> channels;

						cv::split(_decompressed, channels);
						channels.push_back(alphaChannel);
						cv::merge(channels, decompressed);

						// cv::cvtColor(_decompressed, decompressed, cv::COLOR_RGB2RGBA);
					}

					decompress_end = std::chrono::system_clock::now();

					render_start = std::chrono::system_clock::now();

					// cv::imshow("Display window", decompressed);

					// _data.resize(width * height * 4);
					// for(pos = row = 0; row < decompressed.rows; row++) {
					// 	for(col = 0; col < decompressed.cols; col++, pos++) {
					// 		// a = (uint8_t)((decompressed.data[pos] >> 24) & 0xff);
					// 		// r = (uint8_t)((decompressed.data[pos] >> 16) & 0xff);
					// 		// g = (uint8_t)((decompressed.data[pos] >>  8) & 0xff);
					// 		// b = (uint8_t)((decompressed.data[pos] >>  0) & 0xff);

					// 		cv::Vec3b color = decompressed.at<cv::Vec3b>(row, col);

					// 		// color[0] = r;
					// 		// color[1] = g;
					// 		// color[2] = b;
					// 		// color[3] = a;

					// 		_data[(row * 4) * width + col * 4 + 0] = color[0];
					// 		_data[(row * 4) * width + col * 4 + 1] = color[1];
					// 		_data[(row * 4) * width + col * 4 + 2] = color[2];
					// 		// _data[(row * 4) * width + col * 4 + 3] = color[3];
					// 	}
					// }

					if(decompressed.isContinuous()) {
						buffer.assign(decompressed.data, decompressed.data + decompressed.total() * decompressed.channels());
					} else {
						for(int i = 0; i < decompressed.rows; ++i) {
							buffer.insert(buffer.end(), decompressed.ptr<uint8_t>(i), decompressed.ptr<uint8_t>(i) + decompressed.cols * decompressed.channels());
						}
					}

					// ilLoadL(IL_UNSIGNED_BYTE, buffer.data(), buffer.size());
					// ilCopyPixels(
					// 	0, 0, 0,
					// 	width, height, 1,
					// 	IL_RGB, IL_UNSIGNED_BYTE,
					// 	_data
					// );
					ximage = XCreateImage(display, visual, dplanes, ZPixmap, 0, (char*)buffer.data(), width, height, 32, 0);

      		XPutImage(display, window, gc, ximage, 0, 0, 0, 0, width, height);

					render_end = std::chrono::system_clock::now();

					// cv::waitKey(1);

					std::chrono::duration<double, std::milli> capture_time = capture_end - capture_start;
					std::chrono::duration<double, std::milli> transform_time = transform_end - transform_start;
					std::chrono::duration<double, std::milli> compress_time = compress_end - compress_start;
					std::chrono::duration<double, std::milli> decompress_time = decompress_end - decompress_start;
					std::chrono::duration<double, std::milli> render_time = render_end - render_start;

					printf(
						"Total: %05.2fms | Capture: %05.2fms | Transform: %05.2fms | Compress: %05.2fms | Decompress: %05.2fms | Render: %05.2fms\n",
						work_time.count(),
						capture_time.count(),
						transform_time.count(),
						compress_time.count(),
						decompress_time.count(),
						render_time.count()
					);

					// if(quality == 100) quality = 0;
					// else quality++;
				}

				delete &jpeg;

				XCloseDisplay(display);

				return 0;
			}
	};
} // namespace ScreenShare::Host

int main(int argc, char* argv[]) {
	// ScreenShare::Network::Server server = ScreenShare::Network::Server();
	// server.Run();

	// return 0;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show help")
		("x", po::value<int>(), "Set X offset")
		("y", po::value<int>(), "Set Y offset")
		("width", po::value<int>(), "Set frame width")
		("height", po::value<int>(), "Set frame height")
		("quality", po::value<int>(), "Set frame quality");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	ScreenShare::Host::Application application = ScreenShare::Host::Application();

	int result = application.Run(vm);

	return result;
}
