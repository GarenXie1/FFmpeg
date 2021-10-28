// CMakeProject1.cpp : Defines the entry point for the application.
//
#define __STDC_CONSTANT_MACROS

#include "CMakeProject1.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
}

using namespace std;

#define INBUF_SIZE 4096
const char infilename[] = "D:/Visual_Studio_2019/project/FFmpeg/CMakeProject1/Full_HD_Samsung_LED_Color_Demo_HD.mp4";
const char outfile_Mp4_name[] = "D:/Visual_Studio_2019/project/FFmpeg/CMakeProject1/test.mp4";
const char outH264File[] = "D:/Visual_Studio_2019/project/FFmpeg/CMakeProject1/test.h264";


static AVFormatContext* fmt_ctx = NULL;
static int video_stream_idx = -1, audio_stream_idx = -1;
static AVCodecContext* video_dec_ctx = NULL, * audio_dec_ctx;
static AVStream* video_stream = NULL, * audio_stream = NULL;
static AVFrame* frame = NULL;
static AVPacket* pkt = NULL;

void write_Mp4_File(const char* file , uint8_t* data, int len) {
	int count = 0;
	FILE* f;
	f = fopen(file, "ab+");
	count = fwrite(data, 1, len, f);
	if (count != len) {
		fprintf(stderr, "write MP4 file Error, real write -> %d , want to write -> %d \n", count, len);
	}
	fclose(f);
}

static void write_File(const char* file, uint8_t* data, int len) {
	int count = 0;
	FILE* f;
	f = fopen(file, "ab+");
	count = fwrite(data, 1, len, f);
	if (count != len) {
		fprintf(stderr, "write file Error, real write -> %d , want to write -> %d \n", count, len);
	}
	fclose(f);
}

static int open_codec_context(int* stream_idx,
	AVCodecContext** dec_ctx, AVFormatContext* fmt_ctx, enum AVMediaType type) {

	int ret, stream_index;
	AVStream* st;
	const AVCodec* dec = NULL;

	ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
	if (ret < 0) {
		fprintf(stderr, "Could not find %s stream in input file '%s'\n",
			av_get_media_type_string(type), infilename);
		return ret;
	}
	else {
		stream_index = ret;
		st = fmt_ctx->streams[stream_index];

		printf("%s ret -> %d \n", av_get_media_type_string(type), ret);
		printf("%s codec_name -> %s \n", av_get_media_type_string(type),avcodec_get_name(st->codecpar->codec_id));

		/* find decoder for the stream */
		dec = avcodec_find_decoder(st->codecpar->codec_id);
		if (!dec) {
			fprintf(stderr, "Failed to find %s codec\n",
				av_get_media_type_string(type));
			return AVERROR(EINVAL);
		}
		
		printf("Find %s decoder : %s\n", av_get_media_type_string(type) , dec->long_name);
		printf("%s stream codec format : %s \n", av_get_media_type_string(type),av_get_pix_fmt_name(st->codec->pix_fmt));

		/* Allocate a codec context for the decoder */
		*dec_ctx = avcodec_alloc_context3(dec);
		if (!*dec_ctx) {
			fprintf(stderr, "Failed to allocate the %s codec context\n",
				av_get_media_type_string(type));
			return AVERROR(ENOMEM);
		}

		/* Copy codec parameters from input stream to output codec context */
		if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
			fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
				av_get_media_type_string(type));
			return ret;
		}

		/* Init the decoders */
		if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0) {
			fprintf(stderr, "Failed to open %s codec\n",
				av_get_media_type_string(type));
			return ret;
		}

		*stream_idx = stream_index;
	}

	return 0;
}

static int decode_packet(AVCodecContext* dec, const AVPacket* pkt) {

	// 获取 解码前 的.h264 文件.
	printf("一帧 AVPacket size -> %d \n", pkt->buf->size);
	//write_File(outH264File,pkt->data,pkt->size);
	write_File(outH264File, pkt->buf->data, pkt->buf->size);
	return 0;
}

int main(int argc, char** argv) {

	int ret = 0;

	/* open input file, and allocate format context */
	if (avformat_open_input(&fmt_ctx, infilename, NULL, NULL) < 0) {
		fprintf(stderr, "Could not open source file %s\n", infilename);
		exit(1);
	}

	/* retrieve stream information */
	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		exit(1);
	}

	printf("MP4 时长 -> %d s \n", fmt_ctx->duration/1000000);
	printf("MP4 流个数 -> %d \n", fmt_ctx->nb_streams);
	printf("MP4 封装格式 -> %s \n", fmt_ctx->iformat->name);

	if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
		video_stream = fmt_ctx->streams[video_stream_idx];

		printf("video_dec_ctx->width -> %d \n", video_dec_ctx->width);
		printf("video_dec_ctx->height -> %d \n", video_dec_ctx->height);
		printf("video_dec_ctx->pix_fmt -> %s \n", av_get_pix_fmt_name(video_dec_ctx->pix_fmt));

	}

	/* dump input information to stderr */
	printf("\n");
	printf("\n");
	printf("############## Dump Format ####################### \n");
	av_dump_format(fmt_ctx, 0, infilename, 0);
	printf("\n");

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate frame\n");
		ret = AVERROR(ENOMEM);
	}

	pkt = av_packet_alloc();
	if (!pkt) {
		fprintf(stderr, "Could not allocate packet\n");
		ret = AVERROR(ENOMEM);
	}

	/* read frames from the file */
	FILE* f;
	f = fopen(outH264File, "wb+");

	while (av_read_frame(fmt_ctx, pkt) >= 0)
	{
		if (pkt->stream_index == video_stream_idx)
		{
			//decode_packet(video_dec_ctx, pkt);
			fwrite(pkt->buf->data,1, pkt->buf->size, f);

			printf("\n");
			for (int i = 0; i < pkt->buf->size;i++) {
				printf("%2x ", *pkt->buf->data);
				pkt->buf->data += 1;
			}
			printf("\n");
			break;
		}
		av_packet_unref(pkt);
	}

	fclose(f);

	avcodec_free_context(&video_dec_ctx);
	av_packet_free(&pkt);
	av_frame_free(&frame);
	avformat_close_input(&fmt_ctx);
	return ret < 0;
}


int main_back(int argc, char** argv)
{

	AVPacket* pkt;
	uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
	uint8_t* data;
	const AVCodec* codec;
	AVCodecParserContext* parser;
	AVCodecContext* context;
	FILE * inputfd;
	AVFrame* frame;
	int   data_size;
	int len;

	//printf("%s \n", avcodec_configuration());
	//printf("input file -> %s ; output file -> %s \n", argv[1], argv[2]);
	printf("input file -> %s ; output file -> %s \n", infilename, outH264File);

	// attribute_deprecated --> 表示 FFmpeg 已经废弃的 API 或者 结构体、变量. 将不再维护.

	/* 分配 存储 H.264 的 packet 内存空间 */
	pkt = av_packet_alloc();

	/* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
	memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

	/* find H.264 decoder */
	codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		return -1;
	}

	/* Find parser */
	printf("codec->id -> %d \n", codec->id);
	parser = av_parser_init(codec->id);
	if (!parser) {
		fprintf(stderr, "parser not found\n");
		return -1;
	}

	context = avcodec_alloc_context3(codec);
	if (!context) {
		fprintf(stderr, "Could not allocate video codec context\n");
		return -1;
	}

	/* Open Codec */
	if (avcodec_open2(context, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return -1;
	}

	/* Open VIdeo File */
	/* b 字符用来告诉函数库以二进制模式打开文件; r --> 读模式打开 */
	inputfd = fopen(infilename, "rb");
	if (!inputfd) {
		fprintf(stderr, "Could not open %s\n", infilename);
		return -1;
	}

	frame = av_frame_alloc();
	if (!frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		return -1;
	}

	/*
	feof()函数用来检测当前文件流上的文件结束标识，判断是否读到了文件结尾，其原型为：
    int feof(FILE * stream);
	【参数】stream为文件流指针。
	【返回值】检测到文件结束标识返回1，否则返回0。
	*/
	while (!feof(inputfd)) {
		/* 每次 INBUF_SIZE 项 buff , 每项 buff 大小为 1 bytes  */
		data_size = fread(inbuf, 1, INBUF_SIZE, inputfd);
		if (!data_size) {
			fprintf(stderr, "No data can be read on %s\n",infilename);
			break;
		}

		data = inbuf;
		printf("第一层: data_size -> %d \n", data_size);

		write_Mp4_File(outfile_Mp4_name, data, data_size);

		while (data_size > 0) {
			len = av_parser_parse2(parser, context, &pkt->data, &pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
			if (len < 0) {
				fprintf(stderr, "Error while parsing\n");
				return -1;
			}
			printf("parsing len -> %d \n", len);
			data += len;
			data_size -= len;
			printf("data_size -> %d \n", data_size);
		}
	}


	av_frame_free(&frame);
	fclose(inputfd);
	avcodec_free_context(&context);
	av_packet_free(&pkt);

	return 0;
}
