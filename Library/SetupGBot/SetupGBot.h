#include <Arduino.h>
class SetupGBot{
	private:
		char buf[30];
		char buf2[30];
		char buf3[15];
		char buf4[15];
		char buf5[30];
		char buf6[40];
		int EEPROM_MIN_ADDR = 0;
		int EEPROM_MAX_ADDR = 511;
		const char* ssid;
		const char* password;
		String data1; 
		String data2;
		String data3;
		String data4;
		int i=0;
	public:
		String location;
		String forecastApiKey;
		double latitude;
		double longitude;
		SetupGBot();
		boolean eeprom_is_addr_ok(int addr);
		boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes);
		boolean eeprom_write_string(int addr, const char* string);
		boolean eeprom_read_string(int addr, char* buffer, int bufSize);
		void Setup();
		void Recieve();
		void Reconnect();
		void ChangeAPIKey();
		void ChangeLocations();

	
};