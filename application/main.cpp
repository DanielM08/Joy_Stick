#include <iostream>
#include <string>
#include <fstream>
#include <Gpio-int-test.h>
#include <thread>
#include <sstream>

// Função que executa um comando no terminal e retorna o que seria printado
// como string
// Creditos:
// https://stackoverflow.com/questions/32039852/returning-output-from-bash-script-to-calling-c-function
std::string execmd(const char *cmd)
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

void send_key(char key);
std::string get_gpio_direction(int gpio);
bool listen_gpio(int gpio);
int listen_ain(int ain);

void buttonReady(int, char);
void potReady(int, char, char);
void ldrReady(int, char);
bool gpioReady(int, std::string);

int main(int argc, char* argv[]){
	const int gpio_btn1 = 5;
	const int gpio_btn2 = 30;
	const int ain_pot = 1;
	const int ain_ldr = 0;

	/*Inicializar componentes*/	
	//gpioReady(gpio_btn2, "in");
	gpio_export(gpio_btn2);
	gpio_set_dir(gpio_btn2, 0);
	/*Setar teclas de cada componente*/

	std::thread pot(potReady, ain_pot, (char)37, (char)39);
	//std::thread button1(buttonReady, gpio_btn1, (char)38);
   	std::thread button2(buttonReady, gpio_btn2, (char)32);
	std::thread ldr(ldrReady, ain_ldr, (char)40);

	pot.join();
        //button1.join();
        button2.join();
        ldr.join();
	
	return 0;
}

void buttonReady(int pin, char key)
{
    unsigned int value;

    while (1)
    {
        if (!potPriority && !ldrPriority)
        {	    

    	    /*std::string _pin = std::to_string(pin);
            std::string comando = "cat /sys/class/gpio/gpio" + _pin + "/value";
	    std::string resultado = execmd(comando.c_str());

	    std::istringstream reader(resultado);

	    reader >> value;
	    std::cout << "btn:" << value << std::endl;*/
		gpio_get_value(pin, &value);

		
            if (value) 
                send_key(key);
        }
        sleep(1);
    }
}

void potReady(int pin, char keyLeft, char keyRight)
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

	std::cout << "pot" << value << std::endl;

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

void ldrReady(int pin, char key)
{
    int value = 0;
 
    while (1)
    {
        if (!potPriority)
        {
            value = listen_ain(pin);
	    std::cout << "ldr:" << value << std::endl;

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

void send_key(char key){
	fwrite(&key, sizeof(char), 1 ,stdin);
}
/*
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
*/

bool gpioReady(int pin, std::string direction)
{

    if (pin < 1)
    {
       std::cerr << "Pin Invalido!" <<std::endl;
       return false;
    }

    if (!(direction == "out" || direction == "in"))
    {
        std::cerr << "Direcao invalida" << std::endl;
        return false;
    }

    std::string _pin = std::to_string(pin);

    std::string comando = "echo " + _pin + " > /sys/class/gpio/export";
    system(comando.c_str());

    comando = "echo " + direction + " > /sys/class/gpio/gpio" + _pin +"/direction";

    system(comando.c_str());
    return true;
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
    ain_name += "_raw";
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
