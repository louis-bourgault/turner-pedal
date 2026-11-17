#include <Adafruit_TinyUSB.h>
#include <bluefruit.h>
#include <MIDI.h>

const int PIN_PREV = 5;
const int PIN_NEXT = 6;
bool midiMode = true;

BLEDis bledis;
BLEHidAdafruit blehid;
BLEMidi blemidi;

MIDI_CREATE_INSTANCE(BLEMidi, blemidi, MIDI);

void setup()
{
    Serial.begin(115200);
    pinMode(PIN_PREV, INPUT_PULLUP);
    pinMode(PIN_NEXT, INPUT_PULLUP);

    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
    Bluefruit.begin();
    Bluefruit.setTxPower(4);
    Bluefruit.setName("midi foot pedal thing");

    // request connection interval between 7.5 and 15ms for low latency
    Bluefruit.Periph.setConnInterval(6, 12);
    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

    // device information service
    bledis.setManufacturer("audio");
    bledis.setModel("nrf52840");
    bledis.begin();

    blehid.begin();

    MIDI.begin(MIDI_CHANNEL_OMNI);
    startAdvertising();
    Serial.println("strarted");
}

void startAdvertising()
{
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);

    Bluefruit.Advertising.addService(blehid);
    Bluefruit.Advertising.addService(blemidi);

    Bluefruit.ScanResponse.addName();
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // pretty fast advertising interval
    Bluefruit.Advertising.start(0);
}

// this runs when something connects
void connect_callback(uint16_t conn_handle)
{
    BLEConnection *conn = Bluefruit.Connection(conn_handle);

    // request the fast interval
    conn->requestConnectionParameter(6, 12);
    // apparently this can make it faster on modern macos
    conn->requestPHY();

    Serial.println("we conected to something");
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;
    Serial.println("goodbye");
}

void loop()
{
    delay(5);
    // check buttons and things
}
