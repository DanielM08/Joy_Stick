#include <iostream>
#include <string>
#include <fstream>
#include <Gpio-int-test.h>

// Função que executa um comando no terminal e retorna o que seria printado
// como string
// Creditos:
// https://stackoverflow.com/questions/32039852/returning-output-from-bash-script-to-calling-c-function
std::string execmd(char *cmd)
{
    FILE *pipe = popen(cmd, "r");
    if (!pipe)
        return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

void send_key(std::string key);
bool listen_gpio(int gpio);
int listen_ain(int ain);

int main(int argc, char* argv[]){
	const int gpio_btn1 = 0;
	const int gpio_btn2 = 0;
	const int ain_pot_l = 0;
	const int ain_pot_r = 0;
	const int ain_ldr = 0;

	return 0;
}

void send_key(std::string key){
	fwrite(key.c_str(), sizeof(char), key.size(),stdin);
}

bool listen_gpio(int gpio){
	unsigned int value;
	gpio_get_value(gpio, &value);
	if(value == 1) return true;
	else return false;
}

int listen_ain(int ain){
    int value;
    std::string ain_name = "/sys/bus/iio/devices/iio:device0/in_voltage";
    ain_name += std::to_string(ain);
    std::ifstream ainput(ain_name.c_str());
    if (!ainput.is_open()) {
            std::cerr << "ain/get-value";
            return 0;
    }

    if(!(ainput >> value)) return 0;

    if (value > ((4096/2) + 1000)) {
            return 1;
    } else if (value < ((4096/2) - 1000)){
            return -1;
    }

    ainput.close();
    return 0;
}