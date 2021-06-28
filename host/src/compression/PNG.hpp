#include <opencv2/opencv.hpp>

#include <boost/asio.hpp>

#include <libpng16/png.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define PNG_DEBUG 3

#ifndef COMPRESSION_PNG_H
#define COMPRESSION_PNG_H

namespace ScreenShare::Host::Compression {
	class PNG {
		public:
			PNG();
			~PNG();

			void abort_(const char * s, ...) {
				va_list args;
				va_start(args, s);
				vfprintf(stderr, s, args);
				fprintf(stderr, "\n");
				va_end(args);
				abort();
			}

			void CompressFrame(cv::Mat image, std::vector<uint8_t>& compressed, int quality) {
				boost::asio::streambuf buffer;

				int x, y;

				int width, height;
				png_byte color_type;
				png_byte bit_depth;

				png_structp png_ptr;
				png_infop info_ptr;
				int number_of_passes;
				png_bytep* row_pointers;

        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if(!png_ptr) abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if(!info_ptr) abort_("[write_png_file] png_create_info_struct failed");

        if(setjmp(png_jmpbuf(png_ptr))) abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, buffer);


        /* write header */
        if(setjmp(png_jmpbuf(png_ptr))) abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        for (y=0; y<height; y++)
                free(row_pointers[y]);
        free(row_pointers);

        fclose(fp);
			}
			void DecompressFrame(std::vector<uint8_t> compressed, cv::Mat& image);

		private:
			tjhandle _pngCompressor;
			tjhandle _pngDecompressor;

			unsigned long _pngBufferSize = 0;

			const int COLOR_COMPONENTS = 4;
	};
} // namespace ScreenShare::Host::Compression

#endif
