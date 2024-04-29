#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <curl/curl.h>
#include <regex>

//Version 1.1
//Zusammengeschustert von April Seiffert :3

struct DownloadData {

    std::string html_raw;
    CURL* curl_handle;

};

size_t WriteCallback(void* ptr, size_t part_size, size_t npart, void* userdata){

    DownloadData* data = (DownloadData*)userdata;
    size_t totalSize = part_size * npart;

    if(data->html_raw.empty()){
        const char* tag = "<title>";
        const char* end_tag = "</title>";
        std::string content((char*)ptr, totalSize);

        size_t start_pos = content.find(tag);
        if (start_pos != std::string::npos){
            start_pos += std::strlen(tag);
            size_t end_pos = content.find(end_tag, start_pos);
            if(end_pos != std::string::npos){
                data -> html_raw = content.substr(start_pos,  - start_pos);
                std::cout << "Grabbing the raw HTML from https://discord.com/api/download?platform=linux&format=tar.gz" << std::endl;

            }
        }
    }

    return totalSize;

}

DownloadData curl_download(){

    CURL* curl_handle;
    CURLcode res;
    DownloadData redirect_data;
    std::string html_raw;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if(curl_handle){

        const char* url = "https://discord.com/api/download?platform=linux&format=tar.gz";

        redirect_data.curl_handle = curl_handle;

        curl_easy_setopt(curl_handle, CURLOPT_URL, url);

        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);

        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &html_raw);

        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK)
          throw std::runtime_error("curl_easy_perform() failed. Exiting.");

        if (res == CURLE_OK) {

            redirect_data.html_raw = html_raw;

        }

        if (!redirect_data.html_raw.empty()) {
            std::cout << "Downlaod finished." << std::endl;
        }

        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();

        return redirect_data;

    }

    return redirect_data;

}

std::string version_cutout(DownloadData download_data){

    std::string version_number = download_data.html_raw;

    std::regex versionRegex("\\d{1,}\\.\\d{1,}\\.\\d{2,}");

    std::smatch match;

    if (std::regex_search(version_number, match, versionRegex)){

        std::cout << "Found version number: " << match[0] << std::endl;

        return match[0];

    }

    return "";

}

void version_replace(std::string version_number){

    std::regex versionRegex("\\d{1,}\\.\\d{1,}\\.\\d{2,}");

    if (!std::regex_match(version_number, versionRegex)){
    throw std::runtime_error("Invalid version number. exiting");        return;
    }

    std::string filename = "/opt/discord/resources/build_info.json";

    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Error while writing to the file: " + filename + "\nAre you sudo? Exiting.");
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    content = std::regex_replace(content, versionRegex, version_number);

    file.close();

    std::ofstream outfile(filename);

    if (!outfile.is_open()) {
        throw std::runtime_error("Error while writing to the file: " + filename + "\nAre you sudo? Exiting.");
        return;
    }

    outfile << content;

    outfile.close();

    std::cout << "The verison number " << "in " << filename << " has been successfully changed to " << version_number << std::endl;

}

int main()
{

    try {

        DownloadData redirect_data = curl_download();

        std::string version_number = version_cutout(redirect_data);

        version_replace(version_number);

        std::cout << "(Re)Starting Discord." << std::endl;

        system("killall Discord&");

        std::cout << "Start Discord again to finish the uwupdate." << std::endl;

        return 0;

    }

    catch (std::exception& e ){

        std::cerr << "\n" << e.what() << std::endl;

        return -2;

    }

}
