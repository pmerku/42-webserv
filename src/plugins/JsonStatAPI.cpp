//
// Created by mramadan on 14/04/2021.
//

#include "plugins/JsonStatAPI.hpp"
#include "server/http/BodyBuilder.hpp"

#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <dirent.h>

using namespace plugin;

JsonStatAPI::JsonStatAPI() : Plugin("json_stat_api") { }

JsonStatAPI::~JsonStatAPI() { }

bool JsonStatAPI::onBeforeFileServing(NotApache::HTTPClient &client) {
	struct stat buf;
	std::string body;

	std::string fileName = client.file.path.substr(client.file.path.rfind("/")+1);
	client.data.response.builder.setBody(NotApache::BodyBuilder()
		.fileName(fileName)
		.fileType(".html")
		.fileSize("1234")
		.build()
	);


	std::cout << "PATH: " << client.file.path << std::endl;
	if (::stat(client.file.path.c_str(), &buf) == -1)
		return false; // TODO error


	// std::cout << "filename: " << fileName << std::endl;


	// body = "{\n";
	// body += "\tst_dev {\n";
	// client.data.response.builder.setBody(body);
	// //  + "\t\ttype: \"dev_t\",\n");
	// std::cout << "body: " << body << std::endl;

// load json_stat_api
					// +"\t\tvalue: " + std::string(buf.st_dev)
					// +"\t},\n"							

					// +"\tst_ino {\n"						
					// +"\t\ttype: \"ino_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_ino)
					// +"\t},\n"							

					// +"\tst_mode {\n"					
					// +"\t\ttype: \"mode_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_mode)
					// +"\t},\n"							

					// +"\tst_nlink {\n"					
					// +"\t\ttype: \"nlink_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_nlink)
					// +"\t},\n"							

					// +"\tst_uid {\n"						
					// +"\t\ttype: \"uid_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_uid)
					// +"\t},\n"							

					// +"\tst_gid {\n"						
					// +"\t\ttype: \"gid_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_gid)
					// +"\t},\n"							

					// +"\tst_rdev {\n"					
					// +"\t\ttype: \"dev_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_rdev)
					// +"\t},\n"							

					// +"\tst_size {\n"					
					// +"\t\ttype: \"off_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_size)
					// +"\t},\n"							

					// +"\tst_blksize {\n"					
					// +"\t\ttype: \"blksize_t\",\n"		
					// +"\t\tvalue: " + std::string(buf.st_blksize)
					// +"\t},\n"							

					// +"\tst_blocks {\n"					
					// +"\t\ttype: \"blkcnt_t\",\n"		
					// +"\t\tvalue: " + std::string(buf.st_blocks)
					// +"\t},\n"							

					// +"\tst_atime {\n"					
					// +"\t\ttype: \"time_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_atime)
					// +"\t},\n"							

					// +"\tst_mtime {\n"					
					// +"\t\ttype: \"time_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_mtime)
					// +"\t},\n"							

					// +"\tst_ctime {\n"					
					// +"\t\ttype: \"time_t\",\n"			
					// +"\t\tvalue: " + std::string(buf.st_ctime)
					// +"\t}\n"							

					// + "}\n";
	return true;
}

// std::cout	<< "{" 											<< std::endl
// 					<< "\tst_dev {"								<< std::endl
// 					<< "\t\ttype: \"dev_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_dev			 	<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_ino {"								<< std::endl
// 					<< "\t\ttype: \"ino_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_ino			 	<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_mode {"							<< std::endl
// 					<< "\t\ttype: \"mode_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_mode			 	<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_nlink {"							<< std::endl
// 					<< "\t\ttype: \"nlink_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_nlink			<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_uid {"								<< std::endl
// 					<< "\t\ttype: \"uid_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_uid			 	<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_gid {"								<< std::endl
// 					<< "\t\ttype: \"gid_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_gid			 	<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_rdev {"							<< std::endl
// 					<< "\t\ttype: \"dev_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_rdev			 	<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_size {"							<< std::endl
// 					<< "\t\ttype: \"off_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_size			 	<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_blksize {"							<< std::endl
// 					<< "\t\ttype: \"blksize_t\","				<< std::endl
// 					<< "\t\tvalue: " << buf.st_blksize			<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_blocks {"							<< std::endl
// 					<< "\t\ttype: \"blkcnt_t\","				<< std::endl
// 					<< "\t\tvalue: " << buf.st_blocks			<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_atime {"							<< std::endl
// 					<< "\t\ttype: \"time_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_atime			<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_mtime {"							<< std::endl
// 					<< "\t\ttype: \"time_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_mtime			<< std::endl
// 					<< "\t},"									<< std::endl

// 					<< "\tst_ctime {"							<< std::endl
// 					<< "\t\ttype: \"time_t\","					<< std::endl
// 					<< "\t\tvalue: " << buf.st_ctime			<< std::endl
// 					<< "\t}"									<< std::endl

// 				<< "}"											<< std::endl;
