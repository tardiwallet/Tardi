#include "NavButtons.h"
#include "Button.h"
#include "Camera.h"
#include "CodeScanner.h"
#include "GUI.h"
#include "MessageWindow.h"
#include "SimpleButtonsWindow.h"
#include "ListRotatorWindow.h"
#include "ScanQRWindow.h"
#include "DisplayQRWindow.h"
#include "ButtonsWindow.h"
#include "params.hpp"
#include "format.hpp"
#include <sstream>
#include <stdexcept>
#include "CaptureAP.h"


// #ifdef LV_LVGL_H_INCLUDE_SIMPLE
// #include "lvgl.h"
// #else
// #include "lvgl/lvgl.h"
// #endif
#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_log.h"
#include "string.h"
#include "esp_http_server.h"
#include "esp_heap_caps.h"


#ifdef __cplusplus
}
#endif


static const char *TAG = "Controller";

// static void config_my_buttons()
// {
//     ESP_LOGI(TAG, "Configuring buttons!");
//     // gpio_reset_pin(MY_KEY1);
//     // gpio_reset_pin(MY_KEY2);
//     // gpio_reset_pin(MY_KEY3);
//     // gpio_set_direction(MY_KEY1, GPIO_MODE_INPUT);
//     // gpio_set_direction(MY_KEY2, GPIO_MODE_INPUT);
//     // gpio_set_direction(MY_KEY3, GPIO_MODE_INPUT);

//     gpio_config_t io_conf = {};
//     //interrupt of rising edge
//     io_conf.intr_type = GPIO_INTR_POSEDGE;
//     //bit mask of the pins, use GPIO4/5 here
//     io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
//     //set as input mode
//     io_conf.mode = GPIO_MODE_INPUT;
//     //enable pull-up mode
//     io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
//     //disable pull-down mode
//     io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
//     gpio_config(&io_conf);
// }

// /*Test if `id` button is pressed or not*/
// static bool button_is_pressed(uint8_t id)
// {
//     gpio_num_t gpios[3] = {MY_KEY1, MY_KEY2, MY_KEY3};

//     return gpio_get_level(gpios[id]) == 1;
// }

void welcomeScreen(GUI &gui)
{
    ESP_LOGI(TAG, "welcomeScreen");
    MessageWindow mv;
    mv.setGui(&gui);
    mv.setMessage("Tardi Wallet v0.1");
    gui.showView(&mv);
    vTaskDelay(pdMS_TO_TICKS(3000));
    gui.closeView();
}

int selectProtocolScreen(GUI &gui, NavButtons& nb)
{
    ESP_LOGI(TAG, "selectProtocolScreen");
    ListRotatorWindow lrv(&nb);
    lrv.setGui(&gui);
    lrv.setMessage("Select Protocol");
    const char *items[3] = {"Bitcoin (Legacy)", "Adjust Camera", "Info"};
    lrv.addItem(items[0]);
    lrv.addItem(items[1]);
    lrv.addItem(items[2]);
    lrv.setVisibleItemsCount(3);
    gui.showView(&lrv);

    gui.waitToClose(&lrv);
    int outcome = lrv.getCurrentIndex();
    ESP_LOGI(TAG, "Outcome is %d", outcome);
    gui.closeView();
    return outcome;
}

int selectOperationScreen(GUI &gui, NavButtons& nb)
{
    ESP_LOGI(TAG, "selectOperationScreen");
    ListRotatorWindow lrv(&nb);
    lrv.setGui(&gui);
    lrv.setMessage("Select Operation");
    const char *items[4] = {"Create Simple Address", "Sign Message (QRDisp)", "Generate Seed", "Back to Protocol"};
    lrv.addItem(items[0]);
    lrv.addItem(items[1]);
    lrv.addItem(items[2]);
    lrv.addItem(items[3]);
    lrv.setVisibleItemsCount(3);
    gui.showView(&lrv);

    gui.waitToClose(&lrv);
    int outcome = lrv.getCurrentIndex();
    ESP_LOGI(TAG, "Outcome is %d", outcome);
    gui.closeView();
    return outcome;
}

int simpleAddressScreen(GUI &gui, NavButtons& nb)
{
    ESP_LOGI(TAG, "simpleAddressScreen");
    ListRotatorWindow lrv(&nb);
    lrv.setGui(&gui);
    lrv.setMessage("Show Your \nProtocol Seed QR ");
    const char *items[2] = {"Ok", "Cancel"};
    lrv.addItem(items[0]);
    lrv.addItem(items[1]);
    lrv.setVisibleItemsCount(2);
    gui.showView(&lrv);

    gui.waitToClose(&lrv);
    int outcome = lrv.getCurrentIndex();
    ESP_LOGI(TAG, "Outcome is %d", outcome);
    gui.closeView();
    return outcome;
}

int scanScreen(GUI &gui, NavButtons& nb)
{
    ESP_LOGI(TAG, "scanScreen");
    ListRotatorWindow lrv(&nb);
    lrv.setGui(&gui);
    lrv.setMessage("Select Scan when your \nQR is in the square.");
    const char *items[2] = {"Scan", "Back to Protocol"};
    lrv.addItem(items[0]);
    lrv.addItem(items[1]);
    lrv.setVisibleItemsCount(2);
    gui.showView(&lrv);

    gui.waitToClose(&lrv);
    int outcome = lrv.getCurrentIndex();
    ESP_LOGI(TAG, "Outcome is %d", outcome);
    gui.closeView();
    return outcome;
}

std::string seedTool(Params p)
{
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
            return std::string(p.output.c_str());
        }
    }
    catch (std::exception &e)
    {
        ESP_LOGE(TAG, "error: %s", e.what());
    }
    return std::string("");
}

std::string generateSeed(int seedType) 
{
    Params p1;
    std::string hex16;
    switch (seedType) 
    {
    case 1:
        /* 16 byte BIP 39 */
        // std::string hex = generateSeed(0);
        p1.raw.input_format = "random";
        p1.raw.output_format = "bip39";
        p1.raw.count = "16";
        // p1.raw.args.push_back(hex);
        return seedTool(p1);
        break;
    case 2:
        /* UR */
        {
            p1.raw.is_ur = true; 
            p1.raw.max_fragment_length = "";
            return seedTool(p1);

            // p1.raw.max_fragment_length = arg != NULL ? arg : ""; break;
            // char *argv[2] = {strdup("seedtool"), strdup("--u")};
            // auto p = Params::parse(2, (char**)argv);
            // return seedTool(*p);
            // hex16  = generateSeed(0);
            // p1.raw.input_format = "hex";
            // p1.raw.output_format = "btwu";
            // p1.raw.count = "16";
            // p1.raw.args.push_back(hex16);
            // p1.raw.args.push_back("--ur");
        }        
        break;
    case 3:
        /* 16 byte SSKR */
        p1.raw.input_format = "random";  
        p1.raw.output_format = "sskr"; 
        p1.raw.count = "16";          
        return seedTool(p1);
        break;    
    case 4:
        /* 64 byte hex */
        p1.raw.input_format = "random";  
        p1.raw.output_format = "hex"; 
        p1.raw.count = "64";          
        return seedTool(p1);
        break;
    case 0:
    default:
        /* 16 byte hex */
        p1.raw.input_format = "random";
        p1.raw.output_format = "hex";
        p1.raw.count = "16";
        return seedTool(p1);
        break;
    }
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



    // Params p2;
    // p2.raw.input_format = "btw"; 
    // p2.raw.output_format = "hex";   
    // p2.raw.args.push_back("taxi work belt cash echo acid soap lung pool very nail list dark gear dark zero each twin lava lamb");

    // Params p3;
    // p3.raw.input_format = "hex";   
    // p3.raw.output_format = "btwu"; 
    // p3.raw.args.push_back("d0f40d173301c88daee19987244b24fb");

    // Params p4;
    // p4.raw.input_format = "hex";   
    // p4.raw.output_format = "btwm"; 
    // p4.raw.args.push_back("d0f40d173301c88daee19987244b24fb");

    // Params testParams[] = {p1, p2, p3, p4};

    // for (int i=0; i< sizeof(testParams)/sizeof(testParams[0]); i++)
    // {
    //     ESP_LOGI(TAG, "Testing p%d.", i+1);
    //     Params& p = testParams[i];
    //     p.validate();
    //     try
    //     {
    //         p.input_format->process_input(&p);
    //         p.output_format->process_output(&p);

    //         if (p.output.empty())
    //         {
    //             ESP_LOGE(TAG, "An internal error occurred.");
    //         }
    //         else
    //         {
    //             ESP_LOGI(TAG, "output %s", p.output.c_str());
    //         }
    //     }
    //     catch (exception &e)
    //     {
    //         ESP_LOGE(TAG, "error: %s", e.what());
    //     }
    // }

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
    return "";

}


int seedScreen(GUI &gui, NavButtons& nb)
{
    ESP_LOGI(TAG, "seedScreen");
    ListRotatorWindow lrv(&nb);
    lrv.setGui(&gui);
    lrv.setMessage("Select a seed type. Your seed will be shown in the next screen.");
    const char *items[6] = {"16-byte Hex", "16-byte BIP-39", "16-byte UR", "16-byte SSKR", "64-byte Hex", "Back to Protocol"};
    lrv.addItem(items[0]);
    lrv.addItem(items[1]);
    lrv.addItem(items[2]);
    lrv.addItem(items[3]);
    lrv.addItem(items[4]);
    lrv.addItem(items[5]);
    gui.showView(&lrv);

    gui.waitToClose(&lrv);
    int outcome = lrv.getCurrentIndex();
    ESP_LOGI(TAG, "Outcome is %d", outcome);
    gui.closeView();

    if (outcome == 5)
        return -1;
    else
        return outcome;
}

std::string scanQRScreen(GUI &gui, NavButtons& nb)
{
    ESP_LOGI(TAG, "scanActionScreen");
    ScanQRWindow srv(&nb);
    srv.setGui(&gui);
    gui.showView(&srv);

    gui.waitToClose(&srv);
    std::ostringstream out("");
    std::vector<std::string> outcome = srv.getQR();
    // for(auto itr : outcome)
    for (int i = 0; i < outcome.size(); i++)
    {
        ESP_LOGI(TAG, "Outcome %d is %s", i, outcome[i].c_str());
        out << outcome[i] << "\n";
    }
    gui.closeView();
    return out.str();
}

void displayQRScreen(GUI &gui, NavButtons& nb, std::string qrCode)
{
    ESP_LOGI(TAG, "displayQRScreen");
    DisplayQRWindow dqv(&nb, qrCode);
    dqv.setGui(&gui);
    dqv.setCaption("I'm Tardi :)");
    // dqv.setData(qrCode);
    gui.showView(&dqv);

    gui.waitToClose(&dqv);
    gui.closeView();
}


void cameraScreen(GUI &gui, NavButtons& nb)
{
    /**/
    ESP_LOGI(TAG, "cameraScreen");
    Camera camera;
    camera.setFormat(PIXFORMAT_JPEG);
    camera.setFramesize(FRAMESIZE_VGA);

    if (camera.init())
    {
        // startAP();
        // httpd_handle_t httpHandle = start_webserver();
        const int LEN = 16;
        char buffer [LEN];
        // getIP(buffer, 16); 
        std::string ipaddr(buffer);
        std::ostringstream out("");
        out << "The lens can be adjusted based on the camera output. Please connect to the specified AP and open the following urls to see the camera output:\n";
        out << "  http://" << ipaddr << "/jpg\n";
        out << "  http://" << ipaddr << "/stream\n";
        out << "  Wifi SSID: " << CONFIG_ESP_WIFI_SSID << "\n";
        out << "  Wifi Pass: " << CONFIG_ESP_WIFI_PASSWORD;
        std::string outstr = out.str();

        ButtonsWindow bv(&nb);
        bv.setGui(&gui);
        bv.setCaption(outstr.c_str());
        bv.alignCaption(LV_LABEL_ALIGN_LEFT);
        bv.addButton("close");
        gui.showView(&bv);
        gui.waitToClose(&bv);
        // stop_webserver(httpHandle);
        // stopAP();
        gui.closeView();
    } 
    else 
    {
        ButtonsWindow bv(&nb);
        bv.setGui(&gui);
        bv.setCaption("ERROR: \n Could not initialize the camera.");
        bv.alignCaption(LV_LABEL_ALIGN_CENTER);
        bv.addButton("close");
        gui.showView(&bv);
        gui.waitToClose(&bv);
        gui.closeView();
    }
    //*/
}

#include "esp_chip_info.h"
#include "esp_spi_flash.h"

void infoScreen(GUI &gui, NavButtons& nb)
{
    ESP_LOGI(TAG, "infoScreen");
      // std::string infoStr = "";

    // std::vector<std::string> outcome = srv.getQR();
    // // for(auto itr : outcome)
    // for (int i = 0; i < outcome.size(); i++)
    // {
    //     ESP_LOGI(TAG, "Outcome %d is %s", i, outcome[i].c_str());
    //     out << outcome[i] << "\n";
    // }
    // gui.closeView();
    // return out.str();

    // std::format();

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    std::ostringstream out("");
    out << "Chip: " << CONFIG_IDF_TARGET << " \n";
    out << "  Cores: " << int(chip_info.cores) << "\n";
    out << "  Connectivity: WiFi" << ((chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "") << ((chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "") << "\n";
    out << "  Silicon revision: " << int(chip_info.revision) << "\n";
    out << "Flash: " << spi_flash_get_chip_size() / (1024 * 1024) << "MB "<< ((chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external")<<"\n";
    out << "Heap: "<< esp_get_minimum_free_heap_size() <<" bytes free\n";
    out << "  CAP_8BIT: " << heap_caps_get_free_size(MALLOC_CAP_8BIT) <<"\n";
    out << "  CAP_SPIRAM: " << heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    std::string outstr = out.str();
    ButtonsWindow bv(&nb);
    bv.setGui(&gui);
    bv.setCaption(outstr.c_str());
    // bv.alignCaption(LV_LABEL_ALIGN_LEFT);
    bv.addButton("ok");
    gui.showView(&bv);
    ESP_LOGI(TAG, "info: \n%s", out.str().c_str());
    gui.waitToClose(&bv);
    gui.closeView();
}

void controlTask(void *args)
{
    ESP_LOGI(TAG, "controller task starts.");
    GUI gui(0);
    gui.init();
    NavButtons nb;
    
    enum TardiState
    {
        WelcomeState = 1,
        ProtocolState,
        OperationState,
        SimpleAddressState,
        SignState,
        SeedState,
        ScanState,
        ScanQRState,
        DisplayQRState,
        ProAddressState,
        CameraState,
        InfoState,
    };
    int currentState = WelcomeState;
    int nextState = WelcomeState;
    int selectedProtocol = -1;
    int selectedOperation = -1;
    int seedType = -1;
    std::string qrCode;

    bool finished = false;
    while (!finished)
    {
        // vTaskDelay(10);
        currentState = nextState;
        switch (currentState)
        {
        case WelcomeState:
            welcomeScreen(gui);
            nextState = ProtocolState;
            break;

        case ProtocolState:
            selectedProtocol = selectProtocolScreen(gui, nb);
            if (selectedProtocol == 0)
                nextState = OperationState;
            else if (selectedProtocol == 1)
                nextState = CameraState;
            else if (selectedProtocol == 2)
                nextState = InfoState; 
            else
                nextState = ProtocolState;
            break;

        case OperationState:
            selectedOperation = selectOperationScreen(gui, nb);
            if (selectedOperation == 0)
                nextState = SimpleAddressState;
            else if (selectedOperation == 1)
                nextState = SignState;
            else if (selectedOperation == 2)
                nextState = SeedState;
            else
                nextState = ProtocolState;
            break;
        
        case SimpleAddressState:
            selectedOperation = simpleAddressScreen(gui, nb);
            if (selectedOperation == 0)
                nextState = ScanState;
            else
                nextState = OperationState;
            break;

        case SeedState:
            selectedOperation = seedScreen(gui, nb);
            if (selectedOperation == -1)
                nextState = OperationState;
            else
            {
                qrCode = generateSeed(selectedOperation);
                nextState = DisplayQRState;
            }
            break;

        case ScanState:
            selectedOperation = scanScreen(gui, nb);
            if (selectedOperation == 0)
                nextState = ScanQRState;
            else
                nextState = ProtocolState;
            break;

        case ScanQRState:
            qrCode = scanQRScreen(gui, nb);
            if (qrCode == "")
                nextState = ProtocolState;
            else
                nextState = DisplayQRState; // display the scanned code
            break;
        
        case DisplayQRState:
            displayQRScreen(gui, nb, qrCode);
            nextState = ProtocolState;
            break;

        case CameraState:
            cameraScreen(gui, nb);
            nextState = ProtocolState;
            break;

        case InfoState:
            infoScreen(gui, nb);
            nextState = ProtocolState;
            break;
        // case SignState: 
        // case ProAddressState:
        default:
            finished = true;
            break;
        }
    }

    vTaskDelete(NULL);
    ESP_LOGI(TAG, "controller ends");
}
