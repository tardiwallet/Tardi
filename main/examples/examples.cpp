#include "examples.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#ifdef __cplusplus
}
#endif


// #include "DynamicQRCode.h"
// #include "json.hpp"
#include <string>
#include <stdexcept>
// #include <vector>
// #include <iostream>
// #include "Controller.h"
// #include "test.h"

#include "params.hpp"
#include "format.hpp"


void testBlockchainCore(void *args)
{
    const char *TAG = "testBlockchainCore";
    ESP_LOGI(TAG, "test start!");
    // main();

    ESP_LOGI(TAG, "test done!");
    vTaskDelete(NULL);
}

void testDynamicQRCode(void *args)
{
    const char *TAG = "testDynamicQRCode";
    ESP_LOGI(TAG, "test start!");
    // DynamicQRCode c;
    std::string data("salamsaber");
    auto digest = hashSha256_1(data);
    ESP_LOGI(TAG, "%s", digest.c_str());
    
    ESP_LOGI(TAG, "test done!");
    vTaskDelete(NULL);
}

/*
void testJson(void *args)
{
    const char *TAG = "testJson";
    ESP_LOGI(TAG, "test start!");
    using json = nlohmann::json;
    json obj;

    obj["int"] = 10;
    obj["bool"] = true;
    obj["float"] = 3.14;
    obj["str"] = "Hello World";

    ESP_LOGI(TAG, "%d", obj["int"].get<int>());
    ESP_LOGI(TAG, "%d", obj["bool"].get<bool>());
    ESP_LOGI(TAG, "%f", obj["float"].get<float>());
    ESP_LOGI(TAG, "%s", obj["str"].get<std::string>().c_str());

    obj["address"]["street"] = "John's Street";
    obj["address"]["code"] = "2700-12";

    obj["languages"] = {"pt-pt", "en-us"};

    auto street = obj["address"]["street"].get<std::string>();
    auto code = obj["address"]["code"].get<std::string>();

    auto languages = obj["languages"].get<std::vector<std::string>>();

    std::string serializedObject = obj.dump();
    ESP_LOGI(TAG, "%s", serializedObject.c_str());

    std::string serializedObjectPretty = obj.dump(3);
    ESP_LOGI(TAG, "%s", serializedObjectPretty.c_str());

    char str[] = R"(
   {
     "name": "John",
     "age": 10,
     "address": {
       "street": "St. Street",
       "code": "1234-12"
     }
   }
)";

    obj = json::parse(str);

    std::string name = obj["name"].get<std::string>();
    int age = obj["age"].get<int>();
    street = obj["address"]["street"].get<std::string>();
    code = obj["address"]["code"].get<std::string>();

    ESP_LOGI(TAG, "%s", name.c_str());
    ESP_LOGI(TAG, "%d", age);
    ESP_LOGI(TAG, "%s", street.c_str());
    ESP_LOGI(TAG, "%s", code.c_str());

    ESP_LOGI(TAG, "test done!");
    vTaskDelete(NULL);
}

void testCbor(void *args)
{
    const char *TAG = "testCbor";
    ESP_LOGI(TAG, "test start!");
    using json = nlohmann::json;

    char str[] = R"(
    {
      "name": "Jake",
      "age": 23,
      "address": {
        "street": "St. Street",
        "code": "1234-127"
      }
    }
  )";

    json obj = json::parse(str);
    std::vector<std::uint8_t> cborArray = json::to_cbor(obj);

    std::vector<std::uint8_t> cborArray2 = {
        163, 103, 97, 100, 100, 114, 101, 115, 115, 162, 100, 99,
        111, 100, 101, 104, 49, 50, 51, 52, 45, 49, 50, 55, 102,
        115, 116, 114, 101, 101, 116, 106, 83, 116, 46, 32, 83, 116,
        114, 101, 101, 116, 99, 97, 103, 101, 23, 100, 110, 97, 109,
        101, 100, 74, 97, 107, 101};

    obj = json::from_cbor(cborArray2);

    std::string serializedObject = obj.dump(3);

    ESP_LOGI(TAG, "test done!");
    vTaskDelete(NULL);
}
*/

void test_seedtool_cli(void *args)
{
    const char *TAG = "test_seedtool_cli";
    ESP_LOGI(TAG, "test start!");

    /*
        struct argp_option options[] = {
            {"in", 'i', "random|hex|btw|btwu|btwm|bits|cards|dice|base6|base10|ints|bip39|sskr|ur", 0, "The input format (default: random)"},
            {"out", 'o', "hex|btw|btwu|btwm|bits|cards|dice|base6|base10|ints|bip39|sskr", 0, "The output format (default: hex)"},
            {"count", 'c', "1-1024", 0, "The number of output units (default: 16)"},
            {"ur", 'u', "MAX_FRAGMENT_LENGTH", OPTION_ARG_OPTIONAL, "Encode output as a Uniform Resource (UR). If necessary the UR will be segmented into parts with fragments no larger than MAX_FRAGMENT_LENGTH."},
            {"parts", 'p', "FOUNTAIN_PARTS", 0, "For multi-part URs, the number of additional UR parts above the minimum to generate using fountain encoding."},

            {0, 0, 0, 0, "ints Input and Output Options:", 1},
            {"low", 'l', "0-254", 0, "The lowest int returned (default: 1)"},
            {"high", 'h', "1-255", 0, "The highest int returned (default: 9)"},
            {"low < high", 0, 0, OPTION_NO_USAGE, 0},

            {0, 0, 0, 0, "SSKR Output Options:", 2},
            {"group-threshold", 't', "1-16", 0, "The number of groups that must meet their threshold (default: 1)"},
            {"group", 'g', "M-of-N", 0, "The group specification (default: 1-of-1)"},
            {"The --group option may appear more than once.", 0, 0, OPTION_NO_USAGE, 0},
            {"M < N", 0, 0, OPTION_NO_USAGE, 0},
            {"The group threshold must be <= the number of group specifications.", 0, 0, OPTION_NO_USAGE, 0},

            {0, 0, 0, 0, "Deterministic Random Numbers:", 3},
            {"deterministic", 'd', "SEED", 0, "Use a deterministic random number generator with the given seed."},

            {0}};
    */
    // Params px;
    // default values
    // px.raw.input_format = "random"; // default to random
    // px.raw.output_format = "hex";   // default to hex
    // px.raw.count = "16";            // default to 16
    // p.raw.ints_low = "1"; //can only be used with output ints
    // p.raw.ints_high = "9";
    // p.raw.sskr_groups_threshold = "1";
    // p.raw.sskr_groups = 0; // default to 1-of-1

    // p.raw.random_deterministic = 0;
    // p.raw.is_ur = true;
    // p.raw.max_fragment_length = arg != NULL ? arg : "";
    // p.raw.fountain_parts = 0;

    Params p1;
    p1.raw.input_format = "random";  // default to random
    p1.raw.output_format = "hex"; // default to hex
    p1.raw.count = "16";          // default to 16

    Params p2;
    p2.raw.input_format = "btw"; 
    p2.raw.output_format = "hex";   
    p2.raw.args.push_back("taxi work belt cash echo acid soap lung pool very nail list dark gear dark zero each twin lava lamb");

    Params p3;
    p3.raw.input_format = "hex";   
    p3.raw.output_format = "btwu"; 
    p3.raw.args.push_back("d0f40d173301c88daee19987244b24fb");

    Params p4;
    p4.raw.input_format = "hex";   
    p4.raw.output_format = "btwm"; 
    p4.raw.args.push_back("d0f40d173301c88daee19987244b24fb");

    Params testParams[] = {p1, p2, p3, p4};

    for (int i=0; i< sizeof(testParams)/sizeof(testParams[0]); i++)
    {
        ESP_LOGI(TAG, "Testing p%d.", i+1);
        Params& p = testParams[i];
        p.validate();
        try
        {
            p.input_format->process_input(&p);
            p.output_format->process_output(&p);

            if (p.output.empty())
            {
                ESP_LOGE(TAG, "An internal error occurred.");
            }
            else
            {
                ESP_LOGI(TAG, "output %s", p.output.c_str());
            }
        }
        catch (std::exception &e)
        {
            ESP_LOGE(TAG, "error: %s", e.what());
        }
    }

    // case ARGP_KEY_INIT: break;
    // case 'c': raw.count = arg; break;
    // case 'd': raw.random_deterministic = arg; break;
    // case 'g': raw.sskr_groups.push_back(arg); break;
    // case 'h': raw.ints_high = arg; break;
    // case 'i': raw.input_format = arg; break;
    // case 'l': raw.ints_low = arg; break;
    // case 'o': raw.output_format = arg; break;
    // case 't': raw.sskr_groups_threshold = arg; break;
    // case 'u': raw.is_ur = true; raw.max_fragment_length = arg != NULL ? arg : ""; break;
    // case 'p': raw.fountain_parts = arg; break;
    // case ARGP_KEY_ARG: raw.args.push_back(arg); break;
    // case ARGP_KEY_END: {
    //     p->validate();

    ESP_LOGI(TAG, "test done!");
    vTaskDelete(NULL);
}