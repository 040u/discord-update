#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <regex>
#include <stdexcept>

#include <curl/curl.h>

// Version 1.2
// Zusammengeschustert von April Seiffert :3

struct DownloadData
{
    std::string html_raw;
};

size_t WriteCallback(
    void* ptr,
    size_t size,
    size_t nmemb,
    void* userdata)
{
    auto* data = static_cast<DownloadData*>(userdata);

    data->html_raw.append(
        static_cast<char*>(ptr),
        size * nmemb
    );

    return size * nmemb;
}

DownloadData curl_download()
{
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl_handle = curl_easy_init();

    if (!curl_handle)
    {
        curl_global_cleanup();
        throw std::runtime_error(
            "Failed to initialize CURL.");
    }

    DownloadData download_data;

    curl_easy_setopt(
        curl_handle,
        CURLOPT_URL,
        "https://discord.com/api/download?platform=linux&format=tar.gz");

    curl_easy_setopt(
        curl_handle,
        CURLOPT_WRITEFUNCTION,
        WriteCallback);

    curl_easy_setopt(
        curl_handle,
        CURLOPT_WRITEDATA,
        &download_data);

    CURLcode result =
        curl_easy_perform(curl_handle);

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    if (result != CURLE_OK)
    {
        throw std::runtime_error(
            std::string("curl_easy_perform() failed: ") +
            curl_easy_strerror(result));
    }

    if (!download_data.html_raw.empty())
    {
        std::cout
            << "Download finished."
            << std::endl;
    }

    return download_data;
}

std::string version_cutout(
    const DownloadData& download_data)
{
    static const std::regex version_regex(
        R"(\d+\.\d+\.\d+)");

    std::smatch match;

    if (std::regex_search(
            download_data.html_raw,
            match,
            version_regex))
    {
        std::cout
            << "Found version number: "
            << match[0]
            << std::endl;

        return match[0];
    }

    throw std::runtime_error(
        "Could not find version number.");
}

void version_replace(
    const std::string& version_number)
{
    static const std::regex version_regex(
        R"(\d+\.\d+\.\d+)");

    if (!std::regex_match(
            version_number,
            version_regex))
    {
        throw std::runtime_error(
            "Invalid version number.");
    }

    const std::string filename =
        "/opt/discord/resources/build_info.json";

    std::ifstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open " +
            filename +
            "\nAre you running with sudo?");
    }

    std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    file.close();

    content = std::regex_replace(
        content,
        version_regex,
        version_number);

    std::ofstream outfile(filename);

    if (!outfile.is_open())
    {
        throw std::runtime_error(
            "Failed to write to " +
            filename +
            "\nAre you running with sudo?");
    }

    outfile << content;

    std::cout
        << "Version number in "
        << filename
        << " successfully changed to "
        << version_number
        << std::endl;
}

int main()
{
    try
    {
        DownloadData download_data =
            curl_download();

        std::string version_number =
            version_cutout(download_data);

        version_replace(version_number);

        std::cout
            << "(Re)Starting Discord."
            << std::endl;

        std::system("killall Discord");

        std::cout
            << "Start Discord again to finish the update."
            << std::endl;

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "\n"
            << e.what()
            << std::endl;

        return -2;
    }
}
