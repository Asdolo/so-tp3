#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "consola.hpp"
#include "HashMap.hpp"
#include "mpi.h"
#include <queue> 
using namespace std;

#define CMD_LOAD    "load"
#define CMD_ADD     "addAndInc"
#define CMD_MEMBER  "member"
#define CMD_MAXIMUM "maximum"
#define CMD_QUIT    "quit"
#define CMD_SQUIT   "q"

static unsigned int np;
queue<unsigned int>* nodosLibres;
// Crea un ConcurrentHashMap distribuido
static void load(list<string> params)
{   

    MPI_Request request;

    for (list<string>::iterator it = params.begin(); it != params.end(); ++it)
    {
        // TODO: Implementar
        unsigned int proximoNodoLibre = ProximoNodoLibre();

        string mensaje = "1" + (*it);
        cout << "[CONSOLA] mandé el mensaje \"" << mensaje << "\" cuyo tamaño es " << mensaje.size() << " a " << proximoNodoLibre << endl;
        MPI_Isend((void*) mensaje.c_str(), mensaje.size() + 1, MPI_CHAR, proximoNodoLibre, 0, MPI_COMM_WORLD, &request);
    }
    
    cout << "[CONSOLA] mandé todo" << endl;
    while(true)
    {
        // Espero un mensaje diciendo que se libero
        unsigned int respuesta;

        // Espero que me llegue un mensaje al TAG 1
        cout << "[CONSOLA] Voy a esperar a que termine alguien" << endl;
        MPI_Recv((void*) &respuesta, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "[CONSOLA] Me acaba de llegar un mensaje de " <<  respuesta << endl;


        // Encolo el que se liberó
        nodosLibres->push(respuesta);

        if (nodosLibres->size() == np - 1)
        {
            break;
        }
    }



    cout << "La listá esta procesada" << endl;
}
unsigned int ProximoNodoLibre()
{
    unsigned int res;
    if (nodosLibres->empty())
    {
        unsigned int respuesta;
        // Espero que me llegue un mensaje al TAG 1
        MPI_Recv((void*) &respuesta, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Encolo el que se liberó
        nodosLibres->push(respuesta);
        
    }

    // Devuelvo el siguiente libre
    res = nodosLibres->front();
    nodosLibres->pop();
    return res;

}

// Esta función debe avisar a todos los nodos que deben terminar
static void quit()
{
    // TODO: Implementar
    cout << "[CONSOLA] Kiteando..." << endl;
    for (int i = 0; i < nodosLibres->size(); ++i)
    {
        cout << "[CONSOLA] Voy a poppear (" << i << "/" << nodosLibres->size() << ")" << endl;
        nodosLibres->pop();
    }
    cout << "[CONSOLA] Terminé de poppear todos los nodos libres" << endl;
    delete nodosLibres;
    cout << "[CONSOLA] Terminé de poppear todos los nodos libres" << endl;
}

// Esta función calcula el máximo con todos los nodos
static void maximum()
{
    pair<string, unsigned int> result;

    // TODO: Implementar
    string str("a");
    result = make_pair(str,10);

    cout << "El máximo es <" << result.first <<"," << result.second << ">" << endl;
}

// Esta función busca la existencia de *key* en algún nodo
static void member(string key)
{
    bool esta = false;

    // TODO: Implementar

    cout << "El string <" << key << (esta ? ">" : "> no") << " está" << endl;
}


// Esta función suma uno a *key* en algún nodo
static void addAndInc(string key)
{

    // TODO: Implementar
    cout << "Agregado: " << key << endl;
}


/* static int procesar_comandos()
La función toma comandos por consola e invoca a las funciones correspondientes
Si devuelve true, significa que el proceso consola debe terminar
Si devuelve false, significa que debe seguir recibiendo un nuevo comando
*/

static bool procesar_comandos() {

    char buffer[BUFFER_SIZE];
    size_t buffer_length;
    char *res, *first_param, *second_param;

    // Mi mamá no me deja usar gets :(
    res = fgets(buffer, sizeof(buffer), stdin);

    // Permitimos salir con EOF
    if (res==NULL)
        return true;

    buffer_length = strlen(buffer);
    // Si es un ENTER, continuamos
    if (buffer_length<=1)
        return false;

    // Sacamos último carácter
    buffer[buffer_length - 1] = '\0';

    // Obtenemos el primer parámetro
    first_param = strtok(buffer, " ");

    if (strncmp(first_param, CMD_QUIT, sizeof(CMD_QUIT))==0 ||
        strncmp(first_param, CMD_SQUIT, sizeof(CMD_SQUIT))==0) {

        quit();
        return true;
    }

    if (strncmp(first_param, CMD_MAXIMUM, sizeof(CMD_MAXIMUM))==0) {
        maximum();
        return false;
    }

    // Obtenemos el segundo parámetro
    second_param = strtok(NULL, " ");
    if (strncmp(first_param, CMD_MEMBER, sizeof(CMD_MEMBER))==0) {
        if (second_param != NULL) {
            string s(second_param);
            member(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_ADD, sizeof(CMD_ADD))==0) {
        if (second_param != NULL) {
            string s(second_param);
            addAndInc(s);
        }
        else {
            printf("Falta un parámetro\n");
        }
        return false;
    }

    if (strncmp(first_param, CMD_LOAD, sizeof(CMD_LOAD))==0) {
        list<string> params;
        while (second_param != NULL)
        {
            string s(second_param);
            params.push_back(s);
            second_param = strtok(NULL, " ");
        }

        load(params);
        return false;
    }

    printf("Comando no reconocido");
    return false;
}

void consola(unsigned int np_param) {
    np = np_param;

    InicializarEstructuras();

    printf("Comandos disponibles:\n");
    printf("  "CMD_LOAD" <arch_1> <arch_2> ... <arch_n>\n");
    printf("  "CMD_ADD" <string>\n");
    printf("  "CMD_MEMBER" <string>\n");
    printf("  "CMD_MAXIMUM"\n");
    printf("  "CMD_SQUIT"|"CMD_QUIT"\n");

    bool fin = false;
    while (!fin) {
        printf("> ");
        fflush(stdout);
        fin = procesar_comandos();
    }
}
void InicializarEstructuras()
{
    //Inicializo la cola de nodos libres.
    nodosLibres = new queue<unsigned int>();

    // Excluimos el 0 ya que es la consola
    for (unsigned int i = 1; i < np; i++)
    {
        nodosLibres->push(i);
    }
}
