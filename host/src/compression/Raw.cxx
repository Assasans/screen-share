#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator

#include "Raw.hpp"

namespace ScreenShare::Host::Compression {
	Raw::Raw(int width, int height) {
		this->size = cv::Size(width, height);
	}

	Raw::~Raw() {
	}

	// template <typename T> void WriteValue(std::vector<uint8_t>& vector, T value, int offset) {
  //   uint8_t* bytePointer = reinterpret_cast<uint8_t*>(&value);
	// 	std::vector<uint8_t> buffer = std::vector<uint8_t>(bytePointer, bytePointer + sizeof(T));

	// 	for(int i = 0; i < buffer.size(); i++) {
  //   	vector[offset + i] = buffer[i];
	// 	}
	// }

	void WriteValue(std::vector<uint8_t>& vector, uint32_t value, int offset) {
		vector[offset + 0] = value & 0x000000ff;
		vector[offset + 1] = (value & 0x0000ff00) >> 8;
		vector[offset + 2] = (value & 0x00ff0000) >> 16;
		vector[offset + 3] = (value & 0xff000000) >> 24;
	}

	void WriteValue(std::vector<uint8_t>& vector, std::vector<uint8_t> value, int offset) {
		memcpy(&vector[offset], value.data(), value.size());

		// for(int i = 0; i < value.size(); i++) {
    // 	vector[offset + i] = value[i];
		// }
	}

	std::string hexStr(uint8_t* data, int len) {
		std::stringstream ss;
		ss << std::hex;

		for(int i(0); i < len; i++)
			ss << std::setw(2) << std::setfill('0') << (int)data[i] << " ";

		return ss.str();
	}

	// template <typename T> void ReadValue(std::vector<uint8_t>& vector, T* value, int offset) {
	// 	std::vector<uint8_t>::const_iterator begin = vector.begin() + offset;
	// 	std::vector<uint8_t>::const_iterator end = begin + sizeof(T);

	// 	std::vector<uint8_t> buffer;
	// 	buffer.resize(sizeof(T));
	// 	buffer.assign(begin, end);

  //   T* bytePointer = reinterpret_cast<T*>(buffer.data());
  //   value = bytePointer;
	// }

	void ReadValue(std::vector<uint8_t>& vector, uint32_t& value, int offset) {
		std::vector<uint8_t>::iterator begin = vector.begin() + offset;
		std::vector<uint8_t>::iterator end = begin + sizeof(uint32_t);

		std::vector<uint8_t> buffer = std::vector<uint8_t>(begin, end);

		uint32_t _value =
			buffer[0] << 0  |
			buffer[1] << 8  |
			buffer[2] << 16 |
			buffer[3] << 24;
		value = _value;
	}

	void ReadValue(std::vector<uint8_t>& vector, std::vector<uint8_t>& value, int offset, int size) {
		std::vector<uint8_t>::iterator begin = vector.begin() + offset;
		std::vector<uint8_t>::iterator end = begin + size;

		value = std::vector<uint8_t>(begin, end);
	}

	void ReadValue(std::vector<uint8_t>& vector, std::vector<uint64_t>& value, int offset, int size) {
		ReadValue(vector, (std::vector<uint8_t>&)value, offset, size);
	}

	void Raw::CompressFrame(cv::Mat image, std::vector<uint8_t>& compressed) {
		std::vector<uint8_t> buffer;

		if(image.isContinuous()) {
			buffer.assign(image.data, image.data + image.total() * image.channels());
		} else {
			for(int i = 0; i < image.rows; ++i) {
				buffer.insert(buffer.end(), image.ptr<uint8_t>(i), image.ptr<uint8_t>(i) + image.cols * image.channels());
			}
		}

		compressed.resize(4 + 4 + 4 + buffer.size());

		compressed.resize(sizeof(uint32_t) + buffer.size());

		WriteValue(compressed, buffer.size(), 0); // Size   (uint32)
		WriteValue(compressed, buffer, 4);        // Image  (uint8[])

		// std::cout << hexStr(compressed.data(), compressed.size()) << std::endl;
	}

	void Raw::DecompressFrame(std::vector<uint8_t> compressed, cv::Mat& image) {
		std::vector<uint8_t> buffer;

		uint32_t imageSize;

		ReadValue(compressed, imageSize, 0);
		ReadValue(compressed, buffer, 4, imageSize);

    std::vector<cv::Mat> channels;

		image = cv::Mat(size, CV_8UC3, buffer.data());
	}
} // namespace ScreenShare::Host::Compression
