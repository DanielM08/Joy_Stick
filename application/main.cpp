#include <iostream>
#include <string>
#include <fstream>
#include <Gpio-int-test.h>
#include <thread>

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

bool potPriority = false;
bool ldrPriority = false;

void send_key(std::string key);
std::string get_gpio_direction(int gpio);
bool listen_gpio(int gpio);
int listen_ain(int ain);

void buttonRead(int, std::string);
void potRead(int, std::string, std::string);
void ldrRead(int, std::string);

int main(int argc, char* argv[]){
	const int gpio_btn1 = 0;
	const int gpio_btn2 = 0;
	const int ain_pot = 0;
	const int ain_ldr = 0;


	/*Inicializar componentes*/	
	/*Setar teclas de cada componente*/

	std::thread pot(potRead, ain_pot, "a", "d");
    std::thread button1(buttonRead, gpio_btn1, "w");
	std::thread button2(buttonRead, gpio_btn2, "x");
    std::thread ldr(ldrRead, ain_ldr, "s");

	pot.join();
    button1.join();
    button2.join();
    ldr.join();
	
	return 0;
}

void buttonRead(int pin, std::string key)
{
    while (1)
    {
        if (!potPriority && !ldrPriority)
        {
            if (listen_gpio(pin)) 
                send_key(key);
        }
        sleep(1);
    }
}

void potRead(int pin, std::string keyLeft, std::string keyRight)
{
    int value = 0;
    int prevValue = 0;

    while (1)
    {
        value = listen_ain(pin);

        if (prevValue == value)
        {
            potPriority = false;
        }
        else
        {
            potPriority = true;
            prevValue = value;
        }

        if (value == -1)
        {
           send_key(keyLeft);
        }
        else if (value == 1)
        {
           send_key(keyRight);
        }

        sleep(1);
    }
}

void ldrRead(int pin, std::string key)
{
    int value = 0;
 
    while (1)
    {
        if (!potPriority)
        {
            value = listen_ain(pin);

            if (value == 1)
            {
                ldrPriority = false;
            }
            else
            {
                ldrPriority = true;
                while (listen_ain(pin) < 0)
                {
                    send_key(key);
                }
            }
            sleep(1);
        }
    }
}

void send_key(std::string key){
	fwrite(key.c_str(), sizeof(char), key.size(),stdin);
}

std::string get_gpio_direction(int gpio){
	std::string direction;
    std::string gpio_name = "/sys/class/gpio";
    gpio_name += std::to_string(gpio);
    gpio_name += "/direction";
    std::ifstream input(gpio_name.c_str());
    if (!input.is_open()) {
            std::cerr << "gpio/get-direction";
            return nullptr;
    }

    if(!(input >> direction)) return nullptr;
    return direction;
}

bool listen_gpio(int gpio){
	int value;
    std::string gpio_name = "/sys/class/gpio";
    gpio_name += std::to_string(gpio);
    gpio_name += "/value";
    std::ifstream gpioput(gpio_name.c_str());
    if (!gpioput.is_open()) {
            std::cerr << "gpio/get-value";
            return false;
    }

    if(!(gpioput >> value)) return false;
    return value == 1;
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
