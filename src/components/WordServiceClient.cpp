#include "../headers/WordServiceClient.h"
#include <json/json.h> // Si no tienes jsoncpp, usaremos parsing manual
#include <iostream>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")

WordServiceClient::WordServiceClient(string host, int port)
    : host(host), port(port), clientSocket(INVALID_SOCKET), connected(false)
{
}

WordServiceClient::~WordServiceClient()
{
    Disconnect();
}

bool WordServiceClient::Connect()
{
    if (connected)
    {
        return true;
    }

    // Crear socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Failed to create socket for word service. Error: " << WSAGetLastError() << endl;
        return false;
    }

    // Configurar dirección del servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr) <= 0)
    {
        cout << "Invalid address for word service: " << host << endl;
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    // Conectar
    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Failed to connect to word service at " << host << ":" << port << ". Error: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    connected = true;
    cout << "Connected to word service at " << host << ":" << port << endl;
    return true;
}

void WordServiceClient::Disconnect()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
    connected = false;
}

vector<string> WordServiceClient::GetWordsForTheme(const string &theme, int count)
{
    vector<string> words;

    if (!Connect())
    {
        cout << "Cannot connect to word service, using fallback words" << endl;
        // Retornar palabras por defecto si no se puede conectar
        return {"Casa", "Perro", "Sol", "Coche", "Flor", "Mesa", "Gato", "Luna", "Árbol", "Pelota"};
    }

    try
    {
        // Crear solicitud JSON manualmente (implementación simple)
        stringstream jsonRequest;
        jsonRequest << "{\"command\":\"GET_WORDS\",\"theme\":\"" << theme << "\",\"count\":" << count << "}";

        string response = SendRequest(jsonRequest.str());

        if (response.empty())
        {
            cout << "Empty response from word service" << endl;
            return {"Casa", "Perro", "Sol", "Coche", "Flor"};
        }

        // Parsear respuesta JSON
        map<string, string> responseData = ParseJsonResponse(response);

        if (responseData["status"] == "success")
        {
            // Buscar el array de palabras en la respuesta
            size_t wordsStart = response.find("\"words\":[");
            if (wordsStart != string::npos)
            {
                size_t arrayStart = response.find("[", wordsStart);
                size_t arrayEnd = response.find("]", arrayStart);

                if (arrayStart != string::npos && arrayEnd != string::npos)
                {
                    string wordsArray = response.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                    words = ParseJsonArray(wordsArray);
                }
            }
        }
        else
        {
            cout << "Error from word service: " << responseData["message"] << endl;
        }
    }
    catch (const std::exception &e)
    {
        cout << "Exception getting words from service: " << e.what() << endl;
    }

    // Si no obtuvimos palabras, usar valores por defecto
    if (words.empty())
    {
        cout << "Using fallback words for theme: " << theme << endl;

        // Palabras por defecto según temática
        if (theme.find("animal") != string::npos || theme.find("Animal") != string::npos)
        {
            words = {"Perro", "Gato", "León", "Tigre", "Elefante", "Jirafa", "Caballo", "Vaca", "Pollo", "Pez"};
        }
        else if (theme.find("deporte") != string::npos || theme.find("Deporte") != string::npos)
        {
            words = {"Fútbol", "Baloncesto", "Tenis", "Natación", "Correr", "Saltar", "Boxeo", "Golf", "Béisbol", "Voleibol"};
        }
        else if (theme.find("comida") != string::npos || theme.find("Comida") != string::npos)
        {
            words = {"Pizza", "Hamburguesa", "Helado", "Pastel", "Manzana", "Plátano", "Pan", "Queso", "Huevo", "Pollo"};
        }
        else if (theme.find("naturaleza") != string::npos || theme.find("Naturaleza") != string::npos)
        {
            words = {"Árbol", "Flor", "Sol", "Luna", "Estrella", "Río", "Montaña", "Océano", "Playa", "Bosque"};
        }
        else
        {
            words = {"Casa", "Coche", "Mesa", "Silla", "Libro", "Lápiz", "Reloj", "Teléfono", "Computadora", "Televisión"};
        }
    }

    return words;
}

bool WordServiceClient::Ping()
{
    if (!Connect())
    {
        return false;
    }

    try
    {
        string response = SendRequest("{\"command\":\"PING\"}");
        map<string, string> responseData = ParseJsonResponse(response);
        return responseData["status"] == "success";
    }
    catch (...)
    {
        return false;
    }
}

string WordServiceClient::SendRequest(const string &jsonRequest)
{
    if (clientSocket == INVALID_SOCKET)
    {
        return "";
    }

    // Enviar solicitud
    string requestWithNewline = jsonRequest + "\n";
    int bytesSent = send(clientSocket, requestWithNewline.c_str(), requestWithNewline.length(), 0);

    if (bytesSent == SOCKET_ERROR)
    {
        cout << "Failed to send request to word service. Error: " << WSAGetLastError() << endl;
        return "";
    }

    // Recibir respuesta
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0)
    {
        cout << "Failed to receive response from word service. Error: " << WSAGetLastError() << endl;
        return "";
    }

    buffer[bytesReceived] = '\0';
    return string(buffer);
}

map<string, string> WordServiceClient::ParseJsonResponse(const string &jsonResponse)
{
    map<string, string> result;

    // Implementación simple de parsing JSON para campos básicos
    // Buscar "status":"value"
    size_t statusPos = jsonResponse.find("\"status\":");
    if (statusPos != string::npos)
    {
        size_t valueStart = jsonResponse.find("\"", statusPos + 9);
        size_t valueEnd = jsonResponse.find("\"", valueStart + 1);
        if (valueStart != string::npos && valueEnd != string::npos)
        {
            result["status"] = jsonResponse.substr(valueStart + 1, valueEnd - valueStart - 1);
        }
    }

    // Buscar "message":"value"
    size_t messagePos = jsonResponse.find("\"message\":");
    if (messagePos != string::npos)
    {
        size_t valueStart = jsonResponse.find("\"", messagePos + 11);
        size_t valueEnd = jsonResponse.find("\"", valueStart + 1);
        if (valueStart != string::npos && valueEnd != string::npos)
        {
            result["message"] = jsonResponse.substr(valueStart + 1, valueEnd - valueStart - 1);
        }
    }

    return result;
}

vector<string> WordServiceClient::ParseJsonArray(const string &jsonArray)
{
    vector<string> words;

    // Parsear array simple: "word1","word2","word3"
    stringstream ss(jsonArray);
    string item;

    while (std::getline(ss, item, ','))
    {
        // Remover espacios y comillas
        item.erase(std::remove_if(item.begin(), item.end(), ::isspace), item.end());
        if (item.front() == '\"')
            item.erase(0, 1);
        if (item.back() == '\"')
            item.pop_back();

        if (!item.empty())
        {
            words.push_back(item);
        }
    }

    return words;
}