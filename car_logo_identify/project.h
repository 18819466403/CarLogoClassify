#pragma once
#include"rapidjson\document.h"
#include"rapidjson\writer.h"
#include"rapidjson\stringbuffer.h"
#include"rapidjson\filereadstream.h"
#include"rapidjson\prettywriter.h"
#include<fstream>
#include<sys\stat.h>
#include <stdlib.h>
#include<iostream>
#include<vector>
#include<Windows.h>
#include<MsXml6.h>
#include<io.h>  
#include<direct.h>
#include"algoritm.h"
#include"tinyxml.h"
#include"tinystr.h"
#include"image.h"
#pragma comment(lib,"tinyxml.lib")
#define WIN32
#define _MAKE_DIR_DELETE_EXIT_DIR false
#define _MAKE_DIR_RETAIN_EXIT_DIR true
#define _MAKE_FILE_RETAIN_EXIT_FILE true
#define _MAKE_FILE_DELETE_EXIT_FILE false
#define _BUFFER_SIZE 262144
#define _NANE_LENGTH 6
#define _TRAINVAL_PERCENT 0.8
#define _TRAIN_PERCENT 0.5
#define _TYPE_COUNT 30
