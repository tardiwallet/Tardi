/**
 * @file CodeScanner.h
 *
 */

#pragma once
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C"
{
#endif
#include "esp_code_scanner.h"
#include "esp_camera.h"
#ifdef __cplusplus
}
#endif

struct CodeScannerResult
{
    std::string type_name;
    std::string data;
    CodeScannerResult(const char *type_name, const char *data) : type_name(std::string(type_name)),
                                                                 data(std::string(data))
    {
    }
    CodeScannerResult(std::string type_name, std::string data) : type_name(type_name),
                                                                 data(data)
    {
    }
};

class CodeScanner
{
private:
    /* data */
    int64_t decodeTimeMs;
    std::vector<CodeScannerResult> decodedResults;
    void cleanUpResults();

public:
    CodeScanner();
    ~CodeScanner();
    bool decode(camera_fb_t *fb);
    std::vector<CodeScannerResult> getResults() { return decodedResults; }
    int64_t getDecodeTimeMs() { return decodeTimeMs; }
    int getDecodeCount() {return decodedResults.size();}
};
